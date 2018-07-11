killall mptun
./mptun -i tun0 -p 2200 -v 192.168.0.2 -t 192.168.0.1 -k hyg2015 &
sleep 3 
iptables -A FORWARD -i tun+ -j ACCEPT
iptables -A INPUT -i tun+ -j ACCEPT
iptables -t nat -A POSTROUTING -o eth0 -s 192.168.0.2  -j MASQUERADE
echo 1 > /proc/sys/net/ipv4/ip_forward
service iptables restart
