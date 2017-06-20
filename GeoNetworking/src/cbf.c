#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "itsnet.h"
#include "list.h"
#include "cbf.h"
#include "dll_sap.h"
#include "itsnet_header.h"
#include "routing_manager.h"
#include "position_calculation.h"
#include "position_sensor.h"

#define DIST_MAX 500
#define TO_CBF_MIN	1
#define TO_CBF_MAX	100

int itsnet_contention_send(struct itsnet_packet *p)
{
  gettimeofday(&stv, NULL);

	struct timespec exp_in;
	itsnet_routing_packet * routing_packet;
	struct itsnet_position_vector pos;
	int intval;
	double dist;

  printf(LOG, "contention send...\n");

	/*pos = get_position_vector();

	tssetmsec(exp_in, 5000);

	if((routing_packet = list_find_routing_packet(&routing_packet_list, p->payload.itsnet_geobroadcast.source_position_vector.time_stamp, 
						p->payload.itsnet_geobroadcast.source_position_vector.node_id)) != NULL)
	{		
		fprintf(LOG, "pre-routing packet, drop it\n");
		//routing_packet_remove(routing_packet);
		del_task(&routing_packet->tqe);
		//add_task_rel(&exp_in, &(routing_packet->tqe), routing_packet_remove);
		return -1;		
	}

	dist = (haversine_distance(p->common_header.forwarder_position_vector.latitude,
																		p->common_header.forwarder_position_vector.longitude,
																		pos.latitude, pos.longitude) * 1000);
	print_node_id(p->common_header.forwarder_position_vector.node_id);

	if((int)dist <= DIST_MAX)
		intval = (int)(TO_CBF_MAX + (TO_CBF_MIN - TO_CBF_MAX) * ((dist)/DIST_MAX));
	else
		intval = TO_CBF_MIN;

	//intval = 2000;
	fprintf(LOG, "dist=%f, intval=%d\n", dist, intval);
	*/
  fprintf(LOG, "contention send...\n");
	intval = 0;

  gettimeofday(&etv, NULL); 

	itsnet_routing_packet_list_add(&routing_packet_list,
			p->payload.itsnet_geobroadcast.source_position_vector.time_stamp,
		  p->payload.itsnet_geobroadcast.source_position_vector.node_id,
			p,
			intval);		

	itsnet_routing_packet_list_print(&routing_packet_list);

	if (dbg_level > 0) fflush(LOG);	
	return 0;

}


int itsnet_contention_handler(struct itsnet_packet *p)
{
  gettimeofday(&stv, NULL);

	struct timespec exp_in;
	itsnet_routing_packet * routing_packet;
	struct itsnet_position_vector pos;
	int intval;
	double dist;

  fprintf(LOG, "contention handler...\n");

	pos = get_position_vector();

	tssetmsec(exp_in, 5000);

	if((routing_packet = list_find_routing_packet(&routing_packet_list, p->payload.itsnet_geobroadcast.source_position_vector.time_stamp, 
						p->payload.itsnet_geobroadcast.source_position_vector.node_id)) != NULL)
	{		
		fprintf(LOG, "pre-routing packet, drop it\n");
		//routing_packet_remove(routing_packet);
		del_task(&routing_packet->tqe);
		//add_task_rel(&exp_in, &(routing_packet->tqe), routing_packet_remove);
		return -1;		
	}

	dist = (haversine_distance(p->common_header.forwarder_position_vector.latitude,
																		p->common_header.forwarder_position_vector.longitude,
																		pos.latitude, pos.longitude) * 1000);
	print_node_id(p->common_header.forwarder_position_vector.node_id);

	if((int)dist <= DIST_MAX)
		intval = (int)(TO_CBF_MAX + (TO_CBF_MIN - TO_CBF_MAX) * ((dist)/DIST_MAX));
	else
		intval = TO_CBF_MIN;

	//intval = 2000;
	fprintf(LOG, "dist=%f, intval=%d\n", dist, intval);

	gettimeofday(&etv, NULL);

	itsnet_routing_packet_list_add(&routing_packet_list,
			p->payload.itsnet_geobroadcast.source_position_vector.time_stamp,
		  p->payload.itsnet_geobroadcast.source_position_vector.node_id,
			p,
			intval);		

	itsnet_routing_packet_list_print(&routing_packet_list);

	if (dbg_level > 0) fflush(LOG);	
	return 0;
}
