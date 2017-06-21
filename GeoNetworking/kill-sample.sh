#####" stop itsnet daemon"######


killall gps_position_sender
killall position_sensor
killall itsnet
killall mip6d
killall gpsd
killall radvd
killall radvd-org


#####"stop wifi and tap0 interfaces"######

#ifconfig tap0 down
#tunctl -d tap0
#modprobe -r tun
#ifconfig eth1 down

#####"stop gps daemon"#########

#kill -SIGINT `pgrep gpsd`

#####"runing network-manager again"######

#/etc/init.d/network-manager start

