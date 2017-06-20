#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "itsnet_pseudonym.h"
#include "itsnet_security.h"
#include "itsnet_listener.h"
#include "position_sensor.h"

#include "location_table.h"
#include "itsnet_parser.h"
#include "location_service.h"
#include "store_forward.h"
#include "position_calculation.h"

#include "geo_broadcast.h"
#include "routing_manager.h"
#include "itsnet_ip.h"
#include "itsnet.h"
#include "itsnet_pseudonym.h"

#include "greedy.h"

int itsnet_greedy_send(struct itsnet_packet *p)
{
  gettimeofday(&stv, NULL);

	int result = 0;
	char* dest_mac;
	itsnet_node_id next_node;
 
  fprintf(LOG, "greedy send...\n"); 
			
	next_node =	itsnet_select_forwarder(&neighbor_list,p->common_header.forwarder_position_vector,
					p->payload.itsnet_geobroadcast.dest_latitude,p->payload.itsnet_geobroadcast.dest_longitude);

	if(node_id_cmp(next_node, p->common_header.forwarder_position_vector.node_id) == 0){
		fprintf(LOG, "next node is pre-forwarder, don't forward!\n");
		result = -1;
		free(p);
		if (dbg_level > 0) fflush(LOG);
		return result;
	}

	//p->common_header.forwarder_position_vector.node_id = get_ego_node_id();
  p->common_header.forwarder_position_vector.node_id = next_node;

		if (dbg_level > 4) {
			fprintf(LOG, "\tNodeId of the forwarder ");
			print_node_id(p->common_header.forwarder_position_vector.node_id);
			itsnet_neighbor_list_print(&neighbor_list);
			fprintf(LOG, "select forwarder  \n");
		}
		
		dest_mac=(list_find_node(&neighbor_list,&(next_node)))->mac_id.address;
		
		if (  itsnet_packet_send(p,dest_mac) == -1 )
		{
			fprintf(stderr, "erreur envoie packet \n");
			result = -1;
		}
	
  	gettimeofday(&etv, NULL);
	
		itsnet_routing_packet_list_add(&routing_packet_list,
    		p->payload.itsnet_geobroadcast.source_position_vector.time_stamp,
    		p->payload.itsnet_geobroadcast.source_position_vector.node_id,
    		p,
				-1);

		itsnet_routing_packet_list_print(&routing_packet_list);
		
	
		free(p);
		if (dbg_level > 0) fflush(LOG);
		return result;

}

int itsnet_greedy_handler(struct itsnet_packet *p)
{
  gettimeofday(&stv, NULL);

	int result = 0;
	char* dest_mac;
	itsnet_node_id next_node;

  fprintf(LOG, "greedy handler...\n");
			
	next_node =	itsnet_select_forwarder(&neighbor_list,p->common_header.forwarder_position_vector,
					p->payload.itsnet_geobroadcast.dest_latitude,p->payload.itsnet_geobroadcast.dest_longitude);

	if(node_id_cmp(next_node, p->common_header.forwarder_position_vector.node_id) == 0){
		fprintf(LOG, "next node is pre-forwarder, don't forward!\n");
		result = -1;
		free(p);
		if (dbg_level > 0) fflush(LOG);
		return result;
	}

	//p->common_header.forwarder_position_vector.node_id = get_ego_node_id();
  	p->common_header.forwarder_position_vector.node_id = next_node;
  
		if (dbg_level > 4) {
			fprintf(LOG, "\tNodeId of the forwarder ");
			print_node_id(p->common_header.forwarder_position_vector.node_id);
			itsnet_neighbor_list_print(&neighbor_list);
			fprintf(LOG, "select forwarder  \n");
		}
		
		dest_mac=(list_find_node(&neighbor_list,&(next_node)))->mac_id.address;
		
		if (  itsnet_packet_send(p,dest_mac) == -1 )
		{
			fprintf(stderr, "erreur envoie packet \n");
			result = -1;
		}
		
		gettimeofday(&etv, NULL);

		itsnet_routing_packet_list_add(&routing_packet_list,
    		p->payload.itsnet_geobroadcast.source_position_vector.time_stamp,
    		p->payload.itsnet_geobroadcast.source_position_vector.node_id,
    		p,
				-1);

		itsnet_routing_packet_list_print(&routing_packet_list);
		
    
		free(p);
		if (dbg_level > 0) fflush(LOG);
		return result;
}
