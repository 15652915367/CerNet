#include <stdio.h>
#include <stdlib.h>

#include <sys/socket.h>
#include <arpa/inet.h>

#include "auto_wifi.h"
#include "itsnet_parser.h"
#include "location_table.h"
#include "itsnet_header.h"
#include "position_sensor.h"

#define M2U 1000
#define AUTO_WIFI_DATA_SIZE 1500

static int sockfd;
struct   sockaddr_in send_sock_addr;
auto_wifi_msg msg;

int create_sock()
{
   if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
      fprintf(stderr, "auto wifi socket error\n");
      return -1;
   }
   return 0;
}

void sendto_auto_wifi(char* buff, int length)
{
   if (sendto(sockfd, buff, length, 0, (struct sockaddr *)&send_sock_addr, sizeof(send_sock_addr)) < 0) {
      fprintf(stderr, "sendto() error\n");
      return;
   }
}

int traverse_neighbor_list()
{
   char buff[AUTO_WIFI_DATA_SIZE];
   uint16_t car_number;
   int p;
   struct list_head* node_list_entry;
   struct itsnet_position_vector pos;
	 auto_wifi_msg msg;

   car_number = itsnet_neighbor_number(&neighbor_list) + 1;
   memcpy(buff, &car_number, sizeof(uint16_t));
   p = sizeof(uint16_t);
   
   pos=get_position_vector();
   msg.pos_vector.node_id = pos.node_id;
   msg.pos_vector.latitude = pos.latitude;
   msg.pos_vector.longitude = pos.longitude;
   msg.pos_vector.speed = pos.speed;
   msg.pos_vector.heading = pos.heading;
	 msg.pos_vector.time_stamp = pos.time_stamp;
   msg.pos_vector.altitude = pos.altitude;

   memcpy(buff+p, &msg, sizeof(auto_wifi_msg));
   p += sizeof(auto_wifi_msg);

   list_for_each(node_list_entry, &neighbor_list){
      itsnet_node * i;
      i = list_entry(node_list_entry, struct itsnet_node, list);

      msg.pos_vector.node_id = i->node_id;
      msg.pos_vector.latitude = i->pos_vector.latitude;
      msg.pos_vector.longitude = i->pos_vector.longitude;
      msg.pos_vector.speed = i->pos_vector.speed;
      msg.pos_vector.heading = i->pos_vector.heading;
		  msg.pos_vector.time_stamp = i->pos_vector.time_stamp;
		  msg.pos_vector.altitude = i->pos_vector.altitude;

      memcpy(buff+p, &msg, sizeof(auto_wifi_msg));
      p += sizeof(auto_wifi_msg);
   }

   sendto_auto_wifi(buff, p);

   return 0;
}

int start_transmitter()
{
   int intval;

   intval = AutoWifiInterval_parse();

   while (1) {
      traverse_neighbor_list();
      usleep (intval * M2U);
   }   
}

void *pth_auto_wifi_recv(void* m)
{
   return;
}

void *pth_auto_wifi_send(void* m)
{
   int      port;
   int      intval;
   char*    ip_addr;

   /*get param from itsnet-sample.conf*/
   port = AutoWifiPort_parse();
   intval = AutoWifiInterval_parse();
   ip_addr = AutoWifiIpAddr_parse();

   /*init socket address*/ 
   send_sock_addr.sin_family  =  AF_INET;
   send_sock_addr.sin_port    =  htons(port);
   send_sock_addr.sin_addr.s_addr = inet_addr(ip_addr);    

   if (create_sock() < 0){
      fprintf(stderr, "create_sock error\n");
      return;
   }

   start_transmitter();
}

int auto_wifi_init()
{
   pthread_t auto_wifi_send;
   pthread_t auto_wifi_recv;

   /*create pthread*/
   if (pthread_create(&auto_wifi_recv, NULL, pth_auto_wifi_recv, NULL) < 0){
      fprintf(stderr, "auto_wifi recv pthread create error\n");
      return -1;
   }

   if (pthread_create(&auto_wifi_send, NULL, pth_auto_wifi_send, NULL) < 0){
      fprintf(stderr, "auto_wifi send pthread create error\n");
      return -1;
   }
   
}



