
#include <cstring>
#include <iostream>
#include <linux/if_tun.h>
#include <net/if.h>
#include <fcntl.h>  
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <signal.h>
#include <inttypes.h>
#include <errno.h>
#include <sys/stat.h>

using namespace std;

/* buffer for reading , must be >= 1500 */
#define BUFF_SIZE 2000
#define EDECRY_MASK 128

#define tun_read(...) read(__VA_ARGS__)
#define tun_write(...) write(__VA_ARGS__)

typedef sockaddr_in SOCKADDR;
typedef in_addr INADDR;


struct tundev
{
    int tunfd;
    int localfd;

    SOCKADDR remote;
};

int open_tun(char *dev)
{
    int fd = open("/dev/net/tun", O_RDWR);
    if (fd < 0)
    {
        return fd;
    }
    ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TUN | IFF_NO_PI;
    if(dev && *dev != '\0')
    {
        strcpy(ifr.ifr_name, dev);
    }
    int err = ioctl(fd, TUNSETIFF, &ifr);
    if(err < 0)
    {
        close(fd);
        return err;
    }
    if(dev)
    {
        strcpy(dev, ifr.ifr_name);
    }

    return fd;

}

int ifconfig(const char* ifname, const char* va, const char* pa)
{
    char cmd[1024];
    sprintf(cmd, "ifconfig %s %s netmask 255.255.255.255 pointopoint %s",
        ifname, va, pa);
    int ret = system(cmd);
    if(ret < 0)
    {
        cout << "ifconfig error:" << ret << endl;
        return ret;
    }
    return 0;
}

int inet_bind(INADDR *addr, int port)
{
    int resue = 1;
    SOCKADDR address;
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0)
    {
        cout << "inet create socket failed:" << fd << endl;
        return fd;
    }
    address.sin_family = AF_INET;
    address.sin_addr = *addr;
    address.sin_port = htons(port);

    int ret = bind(fd, (sockaddr*)&address, sizeof(address));
    if(ret != 0)
    {
        cout << "inet bind failed:" << ret << endl;
        close(fd);
        return ret;
    }

    return fd;
}

void inet_to_tun(tundev *tdev)
{
    SOCKADDR sa;
    int inetfd = tdev->localfd;
    int tunfd = tdev->tunfd;
    char buff[BUFF_SIZE];
    socklen_t addrLen = sizeof(sa);
    ssize_t n = 0;
    for(;;)
    {
        n = recvfrom(inetfd, buff, BUFF_SIZE, 0, (sockaddr*)&sa, &addrLen);
        if(n < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                cout << "recv from error:" << n << endl;
                exit(-1);
            }
        }
        else
        {
            break;
        }
    }

    for (ssize_t i = 0; i < n; ++i)
    {
        buff[i] ^= EDECRY_MASK;
    }

    for(;;)
    {
        int ret = tun_write(tunfd, buff, n);
        if(ret < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                cout << "tun write error:" << ret << endl;
                exit(-1);
            }
        }
        else
        {
            break;
        }
    }

    tdev->remote = sa;
}

void tun_to_inet(tundev *tdev)
{
    int tunfd = tdev->tunfd;
    int inetfd = tdev->localfd;
    char buff[BUFF_SIZE];
    SOCKADDR* addr = &tdev->remote;
    ssize_t n = 0;
    for(;;)
    {
        n = tun_read(tunfd, buff, BUFF_SIZE);
        if (n < 0)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                cout << "tun read error:" << n << endl;
                exit(-1);
            }
        }
        else
        {
            break;
        }
    }

     for (size_t i = 0; i < n; ++i)
    {
        buff[i] ^= EDECRY_MASK;
    }

    for(;;)
    {
        int ret = sendto(inetfd, buff, n, 0, (sockaddr*)addr, sizeof(SOCKADDR));
        if(ret < 0 && errno == EINTR)
        {
            continue;
        }
        else
        {
            break;
        }
    }
}

void forwarding(tundev *tdev, int maxrd, fd_set *rdset, 
                int maxwt, fd_set *wtset)
{
    fd_set rd, wt;
    rd = *rdset;
    int ret = select(maxrd, &rd, NULL, NULL, NULL);
    if(ret < 0)
    {
        cout << "select read error:" << ret << endl;
        exit(-1);
    }

    if(FD_ISSET(tdev->localfd, &rd))
    {
        inet_to_tun(tdev);
    }

    if(FD_ISSET(tdev->tunfd, &rd))
    {
        wt = *wtset;
        int ret = select(maxwt, NULL, &wt, NULL, NULL);
        if(ret < 0)
        {
            cout << "select write error:" << ret << endl;
            exit(-1);
        }

        tun_to_inet(tdev);
    }
}

void start(tundev *tdev)
{
    fd_set rdset;
    fd_set wtset;
    FD_ZERO(&rdset);
    FD_ZERO(&wtset);
    FD_SET(tdev->tunfd, &rdset);

    FD_SET(tdev->localfd, &rdset);
    FD_SET(tdev->localfd, &wtset);

    int maxrd_fd = tdev->tunfd > tdev->localfd ? 
            tdev->tunfd : tdev->localfd;
    int maxwd_fd = tdev->localfd;

    cout << "start ...\n";
    for(;;)
    {
        forwarding(tdev, maxrd_fd+1, &rdset, maxwd_fd + 1, & wtset);
    }
}

int main(int argc, char* argv[])
{
    tundev tdev;
    char ifname[256] = {0};
    //tun_alloc
    tdev.tunfd = open_tun(ifname);
    if (tdev.tunfd < 0)
    {
        cout << "create tun error:" << endl;
        return -1;
    }

    char vpnaddress[] = "192.168.0.1";
    char ptpaddress[] = "104.225.145.30";

    int ret = ifconfig(ifname, vpnaddress, ptpaddress);
    if (ret < 0)
    {
        return -1;
    }

    INADDR addr;
    addr.s_addr = htonl(INADDR_ANY);
    int port = 2205;

    tdev.localfd = inet_bind(&addr, port);
    if(tdev.localfd < 0)
    {
        return -1;
    }

    tdev.remote.sin_family = AF_INET;
    tdev.remote.sin_port = port;

    start(&tdev);

    return 0;
}