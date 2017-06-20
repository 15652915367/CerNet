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
 * @file store_forward.c
 * store forward code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#include "store_forward.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <unistd.h>
#include <errno.h>

#include <asm/types.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>


#include "geo_broadcast.h"
#include "geo_unicast.h"
#include "geo_anycast.h"
#include "geo_topo.h"

#include "location_table.h"
#include "routing_manager.h"
#include "itsnet.h"

/*
itsnet_store_forward_packet* list_find_stored_packet(
    struct list_head * store_forward_packet_list,
    itsnet_time_stamp ts,
    itsnet_node_id id)
{
	struct list_head* packet_list_entry;
	list_for_each(packet_list_entry, store_forward_packet_list) {
		itsnet_store_forward_packet* i;
		i = list_entry(packet_list_entry, struct itsnet_store_forward_packet, list);
		if ((memcmp( (const void*)(id), (const void*)(&(i->source_node_id)),NODE_ID_LEN) == 0)&&(ts==i->time_stamp))
			return i;
	}
	return NULL;

}
*/

int itsnet_store_forward_packet_list_add(
    struct list_head* store_forward_packet_list,
    itsnet_store_forward_packet * packet)
{
	/*	itsnet_store_forward_packet * new_packet;
		struct timespec exp_in;
		tssetmsec(exp_in,EXPIRE_TIME);


		if ( (new_packet = malloc(sizeof(*packet))) == NULL)
			{
				return -1;
			}
		memcpy(new_packet, packet, sizeof(*packet));
		new_packet->expires=exp_in;
	*/

	INIT_LIST_HEAD(&(packet->tqe.list));

	add_task_rel(&(packet->expires), &(packet->tqe), itsnet_forward);

	list_add_tail(&packet->list, store_forward_packet_list);

	fprintf(LOG, "store packet succeed \n");
	if (dbg_level > 0)  fflush(LOG);
	return 0;

}


/*
void itsnet_store_forward_packet_list_print_node(
    const itsnet_store_forward_packet * packet)
{
	fprintf(LOG, "source node id       : %d \n",packet->source_node_id);
	fprintf(LOG,"time_stamp 	     : %d \n",packet->time_stamp);
	if (dbg_level > 0)  fflush(LOG);
}

void itsnet_store_forward_packet_list_print(
    const struct list_head * store_forward_packet_list)
{
	struct list_head* packet_list_entry;
	list_for_each(packet_list_entry, store_forward_packet_list) {
		itsnet_store_forward_packet * i;
		i = list_entry(packet_list_entry, struct itsnet_store_forward_packet, list);
		itsnet_store_forward_packet_list_print_node(i);

	}
}



*/

/*
void store_forward_packet_remove(
    struct tq_elem *tqe)
{

	itsnet_store_forward_packet * packet;
	packet=  tq_data(tqe,  itsnet_store_forward_packet, tqe);
	list_del(&packet->list);
	free(packet);

}
*/


void itsnet_forward(struct tq_elem *tqe)
{

	itsnet_store_forward_packet * packet;

	struct timespec exp_in;

	tssetsec(exp_in, 5);

	struct timespec expire;

	clock_gettime(CLOCK_REALTIME, &expire);

	tsadd(expire, exp_in, expire);


	packet=  tq_data(tqe,  itsnet_store_forward_packet, tqe);

	if(itsnet_neighbor_list_is_empty(&neighbor_list) == 0)
	{

		if((packet->retry_times)<5)
		{
			(packet->retry_times)++;
			add_task_rel(&exp_in, &(packet->tqe), itsnet_forward);
		}
		else
		{
			list_del(&packet->list);
			free(packet->p);
			free(packet);
		}


	}

	else

	{

		switch (packet->p->common_header.protocol_info.itsnet_header_type)
		{
		case itsnet_unicast_id:
			itsnet_geounicast_send(packet->p);
			break;
		case itsnet_geoanycast_id:
			itsnet_geoanycast_send(packet->p);
			break;
		case itsnet_geobroadcast_id:
			itsnet_geobroadcast_send(packet->p);
			break;
		case itsnet_tsb_id:
			break;

		default:
			list_del(&packet->list);
			//free(packet->p);
			free(packet);
			fprintf(LOG, "Unknown packet type\n");
			break;
		}


		list_del(&packet->list);
		//free(packet->p);
		if (dbg_level > 0)  fflush(LOG);
		free(packet);
	}


}

int itsnet_store(struct itsnet_packet *p,int retry_time)
{


	itsnet_store_forward_packet * new_packet;

	struct timespec exp_in;

	tssetsec(exp_in, 5);

	struct timespec expire;

	clock_gettime(CLOCK_REALTIME, &expire);

	tsadd(expire, exp_in, expire);

	if(list_size(&store_forward_packet_list) > MAX_BUFFERED_ENTRIES )
	{
		return 1;
	}
	else
	{
		new_packet=(itsnet_store_forward_packet*)malloc(sizeof(itsnet_store_forward_packet));
		new_packet->p=(itsnet_packet*)malloc(sizeof(itsnet_packet));

		memcpy(new_packet->p,p,sizeof(struct itsnet_packet));

		new_packet->retry_times=retry_time;
		new_packet->expires=exp_in;

		itsnet_store_forward_packet_list_add(&store_forward_packet_list,new_packet);


		fprintf(LOG, "  \n\nstore and forward packet\n\n");
		if (dbg_level > 0)  fflush(LOG);
		return 0;
	}

}
