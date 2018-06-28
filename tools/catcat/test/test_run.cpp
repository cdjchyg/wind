// linux tun/tap测试

//http://blog.chinaunix.net/uid-317451-id-92474.html/
//https://www.cnblogs.com/litifeng/p/5631353.html

/*
ifconfig tun0 0.0.0.0 up
route add 10.10.10.1 dev tun0
ping 10.10.10.1
*/

#include <linux/if_tun.h>
#include <net/if.h>
#include <cstring>
#include <iostream>
#include <fcntl.h>  
#include <unistd.h>
#include <sys/ioctl.h>

using namespace std;

int tun_create(char *dev, int flags)
{
    int fd = open("/dev/net/tun", O_RDWR);
    if (fd < 0)
    {
        return fd;
    }
    ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags |= flags;
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

int main(int argc, char* argv[])
{
    char tun_name[IFNAMSIZ] = {0};
    int tun = tun_create(tun_name, IFF_TUN|IFF_NO_PI);
    if(tun < 0)
    {
        cout << "create tun error:" << tun << endl;
        return -1;
    }

    cout << "tun name:" << tun_name << endl;

    unsigned char buff[4096] = {0};
    unsigned char ip[4];
    while(1)
    {
        unsigned char ip[4];
        int ret = read(tun, buff, sizeof(buff));
        if (ret < 0)
        {
            break;
        }

        memcpy(ip, &buff[12], 4);
        memcpy(&buff[12], &buff[16], 4);
        memcpy(&buff[16], ip, 4);
        buff[20] = 0 ;
        buff[22] += 8;
        cout << "read " << ret << "bytes\n";
        ret = write(tun, buff, ret);
        cout << "write " << ret << "bytes\n";
    }

    return 0;
}