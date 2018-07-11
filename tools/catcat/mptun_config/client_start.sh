#./mptun -i tun0 -p 2200 -v 10.0.0.1 -t 10.0.0.2 -r 104.225.145.30 -k hyg2015 
killall mptun
./mptun -i tun0 -p 2200 -v 192.168.0.1 -t 192.168.0.2 -r 104.225.145.30 -k hyg2015
sleep 3 
route add -host 104.225.145.30 gw 10.0.2.2
route add -net 0.0.0.0/0 gw 192.168.0.1
echo 1 > /proc/sys/net/ipv4/ip_forward
iptables -A FORWARD -i tun+ -j ACCEPT
iptables -A INPUT -i tun+ -j ACCEPT
iptables -A INPUT -s 104.225.145.30 -j ACCEPT
service iptables restart

# 在/etc/resolv.conf
# nameserver 8.8.8.8
# nameserver 8.8.4.4
#