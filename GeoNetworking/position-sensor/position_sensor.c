/**
 * @file position_sensor.c
 * position sensor code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#include <errno.h>
#include <asm/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <math.h>
#include "itsnet_types.h"

#include <fcntl.h>
#include <termios.h>
#include <sys/stat.h>
#include <time.h>
#include <string.h>

#define SOCK_ITSNET_PATH "/tmp/itsnet_socket"
#define PATH_MAX 20

#define PI 3.14159265358979323846
#define RADIUS 6371  //Radius of the earth

#define BUF_SIZE 1024
#define GPS_INTERVAL 250*1000	//Interval of reading(ms)
#define GPS_DEV_PATH "/dev/ttyACM0"

static struct itsnet_position_information ego_position_information;

int convert_format_lat(const double lat);

double inv_convert_format_lat(const int lat);

int convert_format_long(const double lon);

double inv_convert_format_long(const int lon);

int convert_time_stamp(const double ts);

int convert_format_spd(const double spd);

double inv_convert_format_spd(const int spd);

double _deg2rad(double deg);

int convert_format_alt(const double alt);

/*static void gps_data_update()
{


	ego_position_information.accuracy=0;
	ego_position_information.time_stamp=convert_time_stamp(GPS.fix.time);
	ego_position_information.latitude=convert_format_lat(GPS.fix.latitude);
	ego_position_information.longitude=convert_format_long(GPS.fix.longitude);
	ego_position_information.speed=convert_format_spd(GPS.fix.speed);
	ego_position_information.heading=convert_format_head(GPS.fix.track);
	ego_position_information.altitude=convert_format_alt(GPS.fix.altitude);

	printf("mode=%d,time=%f,lat=%f,lon=%f,speed=%f,alt=%f,heading=%f\n",
		GPS.fix.mode,
		GPS.fix.time,
		GPS.fix.latitude,
		GPS.fix.longitude,
		GPS.fix.speed,
		GPS.fix.altitude,
		GPS.fix.track);

	printf("time=%ld,lat=%d,lon=%d,speed=%d,alt=%d\n",
		ego_position_information.time_stamp,
		ego_position_information.latitude,
		ego_position_information.longitude,
		ego_position_information.speed,
		ego_position_information.altitude);
	
}*/



static int send_socket(message  *m)
{
	int s, len;
	int length;
	struct sockaddr_un rem;
	if ((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
		//perror("socket");
		return 1;
	}
	//printf("Trying to connect...\n");

	rem.sun_family = AF_UNIX;
	strcpy(rem.sun_path, SOCK_ITSNET_PATH);
	len = strlen(rem.sun_path) + sizeof(rem.sun_family);
	if (connect(s, (struct sockaddr *)&rem, len) == -1) {
		perror("connect");
		return 2;
	}

	//printf("Connected.\n"); 

	length=sizeof(*m);
	write (s, &length, sizeof (length));
	write (s,m, length);


	close(s);
	return 0;
}

int fd_gps;//File descriptor
const char *dev_path = GPS_DEV_PATH;//GPS device file path

int set_opt(int fd, int nSpeed, int nBits, char nEvent, int nStop)
{
	struct termios newtio, oldtio;
	if(tcgetattr(fd,&oldtio)!=0){
		perror("SetupSerial 1 for GPS device");
        	return -1;
    	}
   	memset(&newtio,0,sizeof(newtio));
    	newtio.c_cflag |= CLOCAL | CREAD;
    	newtio.c_cflag &= ~CSIZE;

    	switch(nBits){
        	case 7:
            		newtio.c_cflag |= CS7;
            		break;
        	case 8:
            		newtio.c_cflag |= CS8;
            		break;
    	}

    	switch(nEvent){
        	case 'N':
            		newtio.c_cflag &= ~PARENB;
            		break;
   	}

    	switch(nSpeed){
        	case 4800:
            		cfsetispeed(&newtio,B4800);
            		cfsetospeed(&newtio,B4800);
            		break;
        	case 9600:
            		cfsetispeed(&newtio,B9600);
            		cfsetospeed(&newtio,B9600);
            		break;
        	default:
            		cfsetispeed(&newtio,B9600);
            		cfsetospeed(&newtio,B9600);
            		break;
    	}

    	if(1==nStop)
    		newtio.c_cflag &= ~CSTOPB;
    	else if (2==nStop)
    		newtio.c_cflag |= CSTOPB;

    	newtio.c_cc[VTIME] = 0;
    	newtio.c_cc[VMIN] = 100;

    	tcflush(fd,TCIFLUSH);
    	if((tcsetattr(fd,TCSANOW,&newtio))!=0){
        	perror("Com set error for GPS device");
        	return -1;
    	}
  	
    	return 0;
}

int set_para()
{
    	int set_serial;
    	fd_gps  = open(dev_path,O_RDWR|O_NONBLOCK);
    	if(-1 == fd_gps ){
        	perror("Error opening GPS device");
        	return -1;
    	}
    	set_serial = set_opt(fd_gps,9600,8,'N',1); //set serial parameters
    	if(-1 == set_serial){
        	perror("Error setting serial port for GPS device");
        	return -1;
    	}
   	return 0;
}

double vss_pre = 0, vss_now = 0;

double cal_acl(double speed)
{
	double accelt = 0;
	double timeuse;

	//printf("speed = %d\n", speed);
		
	vss_pre = vss_now;
	vss_now = speed;

	timeuse = 0.25;
	accelt = (vss_now - vss_pre) / timeuse; 
	//printf ("vss_pre=%f, vss_now=%f, accelt=%f\n", vss_pre, vss_now, accelt);	
	
	if(accelt < 0)
		return (int)(-accelt * 100);
	else
		return 0;
}

void data_convert(char* vtime, char* vlat, char* vlng, char* vspeed, char* vheading)
{
  double time;
  double lat;
  double lng;
  double speed;
  double heading;

  time = atof(vtime) * 100;
  lat = atof(vlat) / 100.0;
  lng = atof(vlng) / 100.0;
  speed = atof(vspeed) * 1.852 / 3.6;
  heading = atof(vheading);

  if((lat - floor(lat)) * 100 < 60.0)
	lat = floor(lat) + (lat - floor(lat)) * 100 / 60.0;
  else
	lat = floor(lat) + (lat - floor(lat)) * 10 / 60.0;

  if((lng - floor(lng)) * 100 < 60.0)
	lng = floor(lng) + (lng - floor(lng)) * 100 / 60.0;
  else
	lng = floor(lng) + (lng - floor(lng)) * 10 / 60.0;

  ego_position_information.time_stamp=convert_time_stamp(time);
  ego_position_information.latitude=convert_format_lat(lat);
  ego_position_information.longitude=convert_format_long(lng);
  ego_position_information.speed=convert_format_spd(speed);
  ego_position_information.heading=convert_format_head(heading);
  ego_position_information.altitude=cal_acl(speed);

  /*printf("lat=%d,lon=%d,speed=%d,heading=%d\n",
		ego_position_information.latitude,
		ego_position_information.longitude,
		ego_position_information.speed,
		ego_position_information.heading);
  */

}



int reading_gps()
{
   	char* commaPos[13] = {0};
   	char *gprmc = NULL;
   	char buff_gps[BUF_SIZE];
		char date[7];
		char time[10];
		char lat[11];
		char lng[12];
		char speed[6];
		char heading[4];
		char lat_N_S;
		char lng_E_W;
		char status;

   	memset(buff_gps,0,sizeof(buff_gps));
   	read(fd_gps,buff_gps,BUF_SIZE);

   	if((gprmc = strstr(buff_gps,"$GPRMC")) != NULL){
       		int nthComma = 0;
       		char *p;
       		for(p=gprmc;(*p!='*');p++){
          		if ((*p!=','))
                		continue;
           		nthComma++;
           		commaPos[nthComma] = p;
       		}
	
		strncpy(time, (commaPos[1]+1),9);
		status = *(commaPos[2]+1);
		strncpy(lat, (commaPos[3]+1), 10);
    lat_N_S = *(commaPos[4]+1);
    strncpy(lng, (commaPos[5]+1), 11);
    lng_E_W = *(commaPos[6]+1);
    strncpy(speed, (commaPos[7]+1), 5);
    sprintf(heading,"%d",atoi(commaPos[8]+1));
		strncpy(date, (commaPos[9]+1),6);
	
    data_convert(time, lat, lng, speed, heading);
	}
	return 0;
}

int main() {
	struct message m;	

	m.msg_hdr.aid=itsnet_position_sensor;
	m.msg_hdr.sid=itsnet_management_sap;
	m.msg_hdr.opcode=itsnet_request;
	m.msg_hdr.aid=itsnet_position_sensor;
	m.payload.itsnet_position_sensor_req.position_information=ego_position_information;

	set_para();
	printf("Start Golable Position System module ...\n");
	
	while(1){
		reading_gps();	
		m.payload.itsnet_position_sensor_req.position_information=ego_position_information;

		send_socket(&m);
	}
    	
	usleep(GPS_INTERVAL);


	return EXIT_SUCCESS;
}



int convert_format_lat(const double lat)
{
	return (((int)(lat * 1000000))<<3);
}

int convert_format_alt(const double alt)
{
	return (int)(alt * 10);
}

double inv_convert_format_lat(const int lat)
{

	return ((double)(lat>>3))/1000000;
}


int convert_format_long(const double lon)
{
	return (((int)(lon * 1000000))<<3);
}


double inv_convert_format_long(const int lon)
{
	return ((double)(lon>>3))/1000000;
}

int convert_time_stamp(const double ts)
{
	//return(((int)ts) % 65536);
	return (unsigned long)(ts);
}


int convert_format_spd(const double spd)
{
	return (int)(spd*100);
}

double inv_convert_format_spd(const int spd)
{
	return (double)(spd/100);
}

int convert_format_head(const double heading)
{
	return (int)(heading*100);
}

double inv_convert_format_head(const int heading)
{
	return (double)(heading/100);
}

double _deg2rad(double deg)
{
	return ( deg * ( PI / 180.0 ) );
}
