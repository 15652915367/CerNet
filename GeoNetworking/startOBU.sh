#!/bin/bash

OBU_ID=lara-boxe03
BASE_DIR=/root/imara-espritec
SCRIPT_DIR=$BASE_DIR/Scripts/camp0.1/$OBU_ID
Egress_IF=wlan1
VERSION=0.9.8.1

# === Software ===
Tunctl=$BASE_DIR/Sources/others/tunctl-1.5/tunctl 
ITSNET=$BASE_DIR/Sources/itsnet-v$VERSION/src/itsnet
GPS_SENDER=$BASE_DIR/Sources/itsnet-v$VERSION/fake-position-sensor/gps_position_sender
PositionSensor=$BASE_DIR/Sources/itsnet-v$VERSION/position-sensor/position_sensor
MIP6D=$BASE_DIR/Sources/mip6d-v0.4-itsnet-v0.9.5/src/mip6d 
GPSD=/usr/sbin/gpsd
MNPPD=/root/imara-espritec/Sources/mnpp/radvd-v1.5-mnpp-v0.3/radvd 
RADVD=/usr/local/sbin/radvd-org

# === config ===
RADVD_CONF=$SCRIPT_DIR/radvd.conf
MNPP_CONF=$SCRIPT_DIR/mnppd.conf
ITSNET_CONF=$SCRIPT_DIR/itsnet.conf
MIP6D_CONF=$SCRIPT_DIR/mip6d.conf
GPS_SENDER_CONF=$SCRIPT_DIR/gps_data3 

#===== Interfaces =====
Ingress_IF=eth1
TUNNEL=tun0

#===== Addresses =====
IngressAddress=2001:660:3013:CA03::1/64
TunnelLinkLocal=fe80::ca03/64 			# for NEMO
InVehicleNetwork=2001:660:3013:CA03::/64 
TunnelAddress=2001:660:3013:F007::CA03/64 

#==== GPS =========
WithGPS=yes
PositionSensorDevice=/dev/ttyS0 	# Serial port used to connect GPS serial cable

#=== MNPP ========
MNPP_ENABLE=no
# (MNPP_ENABLE=no) otherwise use following route
StaticRSUDestPrefix=2001:660:3013:CA06::/64
StaticRSUIPNextHop=2001:660:3013:F007::CA06	# static routing

#===== Static conf for AnaVANET ===
AnaVANET=yes
SERIAL_SPEED=38400			# 57600 / 9600 ?
PCAP_FILE=tcpdump 			# Base name of the output tcpdump files
GPS_FILE=gps				# Base name of the GPS output
MAC_ADDR="00:06:80:00:a7:1a"   		# OBU4

# set the cell ID (what is this?)
# iwconfig $Egress_IF ap 00:11:22:33:44:55

######### IPC2C Configuration ###########
sh kill.sh

# tap0
echo "tap0 config"
modprobe -r tun; sleep 1; modprobe tun;
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

#===== Static route confinguration for demo : route to the server =====
if [ $MNPP_ENABLE = 'yes' ]; then
	# Execute MNPP (radvd patched MNPP)
	$MNPPD -C $MNPP_CONF
	echo "ip6tables config"
	ip6tables -F
	ip -6 rule add from $InVehicleNetwork fwmark 0xc9 lookup 201 prio 301
	ip -6 route add default from $InVehicleNetwork dev $TUNNEL table 201 metric 10 proto 16
	ip -6 route add default dev $TUNNEL table 201 metric 10

	ip6tables -t mangle -F PREROUTING
	ip6tables -A PREROUTING -t mangle -j MARK -p icmpv6 --set-mark 201
	ip6tables -A PREROUTING -t mangle -j MARK -p tcp --set-mark 201
	ip6tables -A PREROUTING -t mangle -j MARK -p udp --set-mark 201

else
	ip6tables -F
	ip -6 route add $StaticRSUDestPrefix via $StaticRSUIPNextHop dev $TUNNEL 
	ip -6 route change $StaticRSUDestPrefix via $StaticRSUIPNextHop dev $TUNNEL 
	$RADVD -C $RADVD_CONF
fi

sysctl -w net.ipv6.neigh.$TUNNEL.mcast_solicit=1
echo "route set for V2V communication "

#======GPS SPEED======
if [ $WithGPS = 'no' ]; then # With GPS
	echo "Fake GPS mode"
	$GPS_SENDER -c $GPS_SENDER_CONF &
fi


#===============AnaVAMET START===============
if [ $AnaVANET = 'yes' ]; then

#====== Time setup =====
TEST_ID=`date "+%Y%m%d-%H-%M-%N"` # for file name

# Start logging the position from GPS
echo "Starting GPS logging..."
/bin/stty $SERIAL_SPEED cs8 -parenb crtscts -echo -F $PositionSensorDevice

if [ $WithGPS = 'yes' ]; then # With GPS
	echo "GPS mode"
	$GPSD -N $PositionSensorDevice &
	$PositionSensor &
fi

sleep 1
filename=./logs/$GPS_FILE"_"MR"_"$TEST_ID".log"
echo $TEST_ID >> ./logs/test-memo.txt
#cat $PositionSensorDevice >> $filename &
#$JAVA -jar $GPSGET localhost >> $filename &
echo "connecting gpsd........."
exec 3<>/dev/tcp/localhost/2947
echo -e "?WATCH={\"raw\":1}">&3
cat <&3 >> $filename &
echo "connecting gpsd.........done"

#cats=`ps aux | grep "cat $PositionSensorDevice" | awk '{split($0,s," "); print s[2]}'`    # Get process PID for cat
cats=`ps aux | grep "java -jar" | awk '{split($0,s," "); print s[2]}'`    # Get process PID for cat
trap "kill `echo $cats | awk '{split($0,s," "); print s[1]}'`; exit" 0 1 2 3 15        # Kill gps logging at the end
for (( i=0; i<150; i++ ))  # Stablish a synchronization point between GPS time and local time
do
	sleep 0.01
	date '+TIME:%H:%M:%S:%N' >> $filename
	echo -n "*"
done
echo 
echo "OK"

# Start tcpdump listen
echo "Starting tcpdump listening..."
filename=./logs/$PCAP_FILE"_"MR"_"$TEST_ID".log"
/usr/sbin/tcpdump -s1500 -ni $Egress_IF -w $filename wlan host $MAC_ADDR &
echo "OK"

fi 


#===============AnaVAMET EMD===============



#====== ITSNET ======

#$MIP6D -m -c $MIP6D_CONF & #-l /var/log/mip6d.log &


while true;
do

	# C2C NET
	echo "==== C2C NET ===="
	sleep 0.1
	$ITSNET -m -c $ITSNET_CONF 
done
