#!/bin/bash 

BASE_DIR=/root/yxy/CarGeo6/CarGeo6-v0.9.8.1/cargeo6-src-v0.9.8.1
Egress_IF=wlan0

# === Software ===
Tunctl=/root/yxy/CarGeo6/tunctl-1.5/tunctl
ITSNET=$BASE_DIR/src/itsnet
GPS_SENDER=$BASE_DIR/static-position-sender/gps_position_sender
PositionSensor=$BASE_DIR/position-sensor/position_sensor
GPSD=/usr/sbin/gpsd
#MNPPD=mnppd
RADVD=/root/yxy/CarGeo6/CarGeo6-v0.9.8.1/cargeo6-src-v0.9.8.1/mnpp/radvd-v1.5-mnpp-v0.3/radvd

# === config ===
RADVD_CONF=./radvd.conf
#MNPP_CONF=./mnppd.conf
ITSNET_CONF=./itsnet-sample.conf
GPS_SENDER_CONF=./gps_data1

#===== Interfaces =====
Ingress_IF=wlan1
TUNNEL=tun0

#===== Addresses =====
IngressAddress=2001:660:3013:CA08::1/64
TunnelLinkLocal=fe80::ca08/64
InVehicleNetwork=2001:660:3013:CA08::/64
#TunnelAddress=2001:660:3013:F005::CA08/64
TunnelAddress=fd00:aaaa:bbbb:cccc::CA08/64

#==== GPS =========
PositionSensorDevice=/dev/ttyACM0         # Serial port used to connect GPS serial cable

StaticRSUDestPrefix=default
#StaticRSUIPNextHop=2001:660:3013:F005::CA08     # static routing

######### IPC2C Configuration ###########
echo "kill some process"
sh kill-sample.sh
echo ""

# tap0
echo "tap0 config"
rmmod -f tun; sleep 1; modprobe tun;
#modprobe -r tun; sleep 1; modprobe tun;
$Tunctl -t $TUNNEL -n # point-to point
#ip link set $TUNNEL arp off
ip link set $TUNNEL arp on
echo "Tunneling interface mounted"
sysctl -w net.ipv6.conf.all.forwarding=1

ifconfig $TUNNEL inet6 add $TunnelLinkLocal up
ifconfig $Egress_IF up
ifconfig $Ingress_IF inet6 add $IngressAddress up
ifconfig $TUNNEL inet6 add $TunnelAddress up
ifconfig $TUNNEL mtu 1350 up
echo "Tunnel address configured"

#ip -6 route add $StaticRSUDestPrefix via $StaticRSUIPNextHop dev $TUNNEL
#ip -6 route change $StaticRSUDestPrefix via $StaticRSUIPNextHop dev $TUNNEL
ip -6 route add default via fe80::eda:41ff:fe12:ef72 dev wlan1
#ip -6 route add 2001:660:3013:ca00::/56 dev tun0
ip -6 route del ff00::/8 dev eth0
ip -6 route del ff00::/8 dev wlan0
ip -6 route del ff00::/8 dev wlan1
ip -6 route del 2001:da8:215:509::/64 dev wlan1 

sysctl -w net.ipv6.neigh.$TUNNEL.mcast_solicit=1
echo "route set for V2V communication "

# real gps
#$GPSD -N $PositionSensorDevice &
#sleep 1

# in the case of test without GPS
#$GPS_SENDER -c $GPS_SENDER_CONF &

#rm /root/itsnet.log
rm /var/log/syslog
rm /var/log/kern.log

$ITSNET -c $ITSNET_CONF & 

sleep 1

$PositionSensor &

#$RADVD -C $RADVD_CONF
#echo "radvd started!"

killall unicast
cd /root
./unicast &

killall mchat
cd /root/mcast
./mchat 1&

#killall ping6
#ping6 2001:da8:270:2018:f816:3eff:fe11:9211 &
