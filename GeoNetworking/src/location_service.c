/***************************************************************************
 *                              CarGeo6 Project                            *
 *         C2CNet layer implementation conforming with GeoNet D2.2         *
 *                                                                         *
 *                      Copyright(C)2010 ESPRIT-INRIA                      *
 *                                                                         *
 *                                                                         *
 *   Authors:                                                              *
 *   Hichem BARGAOUI <barghich@gmail.com>                                  *
 *   Anouar CHEMEK <anouar.chemek@gmail.com>                               *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
 * @file location_service.c
 * location service code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */


#include <unistd.h>
#include <errno.h>


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


#include "location_service.h"
#include "position_sensor.h"
#include "location_table.h"
#include "itsnet_pseudonym.h"
#include "itsnet.h"

int list_process_location_packet(
    struct list_head * location_packet_list,
    itsnet_node_id id,
    uint32_t latitude,
    uint32_t longitude)
{
	struct list_head* packet_list_entry;

	list_for_each(packet_list_entry, location_packet_list)
	{
		itsnet_location_packet* i;
		i = list_entry(packet_list_entry, struct itsnet_location_packet, list);
		if ((node_id_cmp( id,i->p->payload.itsnet_ls.ls.ls_reply.dest_node_id) == 0))
		{
			fprintf(LOG, "The Reply location service is destinated to this NODE");
			print_node_id(id);
			fprintf(LOG, "Send a Unicast Packet for the Node who sent the request location service \n");

			i->p->payload.itsnet_unicast.dest_node_id=id;
			i->p->payload.itsnet_unicast.dest_latitude=latitude;
			i->p->payload.itsnet_unicast.dest_longitude=longitude;

			list_del(&i->list);
			itsnet_geounicast_send(i->p);

			return 0;

		}

	}
	return 0;
}



int itsnet_location_packet_list_add(
    struct list_head* location_packet_list,
    itsnet_location_packet * packet)
{
	itsnet_location_packet * new_packet;
	struct timespec exp_in;
	tssetmsec(exp_in,1000);

	if(list_size(location_packet_list) > LOCATION_PACKET_LIST_SIZE)
	{
		return 1;
	}
	else
	{
		if ( (new_packet = malloc(sizeof(struct itsnet_location_packet))) == NULL)
		{
			return -1;
		}
		memcpy(new_packet, packet, sizeof(struct itsnet_location_packet));
		new_packet->expires=exp_in;
		INIT_LIST_HEAD(&(new_packet->tqe.list));

		add_task_rel(&exp_in, &(new_packet->tqe), location_packet_remove);

		list_add_tail(&new_packet->list, location_packet_list);
		return 0;

	}

}

void location_packet_remove(
    struct tq_elem *tqe)
{

	itsnet_location_packet * packet;
	packet=  tq_data(tqe,  itsnet_location_packet, tqe);
	list_del(&packet->list);
	free(packet);

}



/**
   * location request
   * @param itsnet_node_id node_id
   * @return int
   */
int itsnet_location_request (struct itsnet_packet *unicast_packet)
{

	int result;
	struct itsnet_packet *p;
	struct itsnet_position_vector pos;

	itsnet_location_packet * packet;

	pos=get_position_vector();
	p=(itsnet_packet*)malloc(sizeof(itsnet_packet));
	packet=(itsnet_location_packet*)malloc(sizeof(itsnet_location_packet));
	if (p==NULL)
	{
		fprintf(stderr, "erreur allocation \n");
		result = -1;
	}
	else
	{

		memset(p,0,sizeof(itsnet_packet));
		p->common_header.protocol_info.itsnet_header_type=itsnet_ls_id;
		p->common_header.traffic_class=CLASS02;
		p->common_header.protocol_info.itsnet_header_subtype=ITSNET_LS_REQUEST;
		p->common_header.flags=OBU_SECURITY_ENABLED;
		p->common_header.payload_lenght=ITSNET_DATA_SIZE;
		p->common_header.hop_limit=255;
		p->common_header.forwarder_position_vector=pos;
		p->common_header.protocol_info.itsnet_next_header=other;

		p->payload.itsnet_ls.ls.ls_request.request_id=unicast_packet->payload.itsnet_unicast.dest_node_id;
		p->payload.itsnet_ls.ls.ls_request.source_position_vector=pos;
	}

	if ( itsnet_packet_send(p, get_broadcast_mac_id()) == -1 )
	{
		fprintf(stderr, "erreur envoie packet \n");
		result = -1;
	}

	fprintf(LOG, "\tAdding The Packet to the Packet_list\n");

	packet->p=(itsnet_packet*)malloc(sizeof(itsnet_packet));

	memcpy(packet->p,unicast_packet,sizeof(struct itsnet_packet));

	itsnet_location_packet_list_add(&location_packet_list,packet);

	free(p);
	if (dbg_level > 0)  fflush(LOG);
	return 0;


}

/**
   * location reply
   * @param
   */
void itsnet_location_reply (struct itsnet_packet *p)
{


	int result=0;
	struct itsnet_position_vector pos;
	pos=get_position_vector();
	struct itsnet_node_id node_id_req;
	struct itsnet_node *node;

	struct itsnet_packet *reply_packet;

	node_id_req=p->payload.itsnet_ls.ls.ls_request.request_id;

	node=list_find_node(&neighbor_list,&node_id_req);

	if(node != NULL)
	{
		reply_packet=(itsnet_packet*)malloc(sizeof(itsnet_packet));

		if (reply_packet==NULL)
		{
			fprintf(stderr, "erreur allocation \n");
			result = -1;
			return result;
		}


		fprintf(LOG, "NodeId destination is a neighbor \n");

		memset(reply_packet,0,sizeof(itsnet_packet));
		reply_packet->common_header.protocol_info.itsnet_header_type=itsnet_ls_id;
		reply_packet->common_header.traffic_class=CLASS02;
		reply_packet->common_header.protocol_info.itsnet_header_subtype=ITSNET_LS_REPLY;
		reply_packet->common_header.flags=OBU_SECURITY_ENABLED;
		reply_packet->common_header.payload_lenght=ITSNET_DATA_SIZE;
		reply_packet->common_header.hop_limit=255;
		reply_packet->common_header.forwarder_position_vector=p->common_header.forwarder_position_vector;
		reply_packet->common_header.protocol_info.itsnet_next_header=other;

		reply_packet->payload.itsnet_ls.ls.ls_reply.source_position_vector=pos;
		reply_packet->payload.itsnet_ls.ls.ls_reply.dest_node_id=node_id_req;
		reply_packet->payload.itsnet_ls.ls.ls_reply.dest_latitude=node->pos_vector.latitude;
		reply_packet->payload.itsnet_ls.ls.ls_reply.dest_longitude=node->pos_vector.longitude;

		if (  itsnet_packet_send(reply_packet, get_broadcast_mac_id()) == -1 )
		{
			fprintf(stderr,"erreur envoie packet \n");
			result = -1;
		}

		free(reply_packet);
		if (dbg_level > 0)  fflush(LOG);
		return result;
	}
}


void itsnet_location_handler(struct itsnet_packet *p)
{

	switch (p->common_header.protocol_info.itsnet_header_subtype)
	{
	case ITSNET_LS_REQUEST:
		fprintf(LOG, "value of packet : location sercive request \n");
		print_node_id(p->common_header.forwarder_position_vector.node_id);
		itsnet_location_reply(p);
		break;
	case ITSNET_LS_REPLY:
		fprintf(LOG, "value of packet : location sercive reply \n");
	if(node_id_cmp(p->common_header.forwarder_position_vector.node_id,get_ego_node_id())==0)
	{

		list_process_location_packet(&location_packet_list,p->payload.itsnet_ls.ls.ls_reply.dest_node_id,p->payload.itsnet_ls.ls.ls_reply.dest_latitude,p->payload.itsnet_ls.ls.ls_reply.dest_longitude);
}
		break;
	default:
//		free(p);
		fprintf(LOG, "Unknown packet type\n");
		break;
	}
	if (dbg_level > 0)  fflush(LOG);
	free(p);

}
