#ifndef AUTO_WIFI_H
#define AUTO_WIFI_H

#include "beaconing.h"
#include "list.h"

struct auto_wifi_msg{
	itsnet_position_vector pos_vector;
};
typedef struct auto_wifi_msg auto_wifi_msg;

int auto_wifi_init();
int create_sock();
void sendto_auto_wifi(char* buff, int length);
int traverse_neighbor_list();
int start_transmitter();

void* pth_auto_wifi_send(void* m);
void* pth_auto_wifi_recv(void* m);
#endif 
