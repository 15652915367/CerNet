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
 * @file location_table.c
 * location table code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#include <stdio.h>
#include <stdlib.h>
#include "itsnet_pseudonym.h"
#include "location_table.h"
#include "position_calculation.h"
#include "itsnet.h"


/**
   * list_find_node
   * @param struct list_head * neighbor_list
   * @param itsnet_node_id id
   * @return itsnet_node*
   */
itsnet_node * list_find_node(struct list_head * neighbor_list, struct itsnet_node_id * node_id)
{
	struct list_head* node_list_entry;
	list_for_each(node_list_entry, neighbor_list)
	{
		itsnet_node* i;
		i = list_entry(node_list_entry, struct itsnet_node, list);
		if (memcmp(node_id,&(i->node_id),sizeof(itsnet_node_id)) == 0 )
			return i;
	}
	return NULL;
}


/**
   * itsnet_neighbor_number
   * @param struct list_head * neighbor_list
   * @return int
   */
int itsnet_neighbor_number(struct list_head * neighbor_list)
{
	struct list_head* node_list_entry;
	int NodeNbre=0;
	list_for_each(node_list_entry, neighbor_list)
	{
		itsnet_node* i;
		i = list_entry(node_list_entry, struct itsnet_node, list);

		NodeNbre+=1;
	}
	return NodeNbre;
}



int itsnet_neighbor_list_is_empty( struct list_head* neighbor_list)
{
	if(list_empty(neighbor_list))
		return 0;
	else
		return 1;
}

/**
 * add the node to the list
 * @param neighbor_list nodes list
 * @param node node to add
 * @return an integer
 */

int itsnet_neighbor_list_add(struct list_head* neighbor_list,itsnet_node * node)
{
	itsnet_node * new_node;
	struct timespec exp_in;

	tssetmsec(exp_in,5000);  /** TODO */

	if(list_size(neighbor_list) > MAX_NEIGHBOR)
	{
		return 1;

	}
	else
	{
		if ( (new_node = malloc(sizeof(*node))) == NULL)
		{
			return -1;
		}
		memcpy(new_node, node, sizeof(*node));

		new_node->expires=exp_in;
		INIT_LIST_HEAD(&(new_node->tqe.list));

		if (node_id_cmp(new_node->pos_vector.node_id,get_ego_node_id()) != 0 )
		{

			add_task_rel(&exp_in, &(new_node->tqe), itsnet_neighbor_list_remove_node);
		}

		list_add_tail(&new_node->list, neighbor_list);

		return 0;
	}
}


/**
 * print the given node2001:660:3013:f001:3:3:ca03:3
 * @param itsnet_node node to print
 */

void itsnet_neighbor_list_print_node(const itsnet_node * node)
{
	fprintf(LOG, "| mac_id=");	
	print_mac_id(node->mac_id);
	fprintf(LOG, "| node_id=");
	print_node_id(node->node_id);
	fprintf(LOG, " | time=%d",node->pos_vector.time_stamp);
//	fprintf(LOG, " | expires=%d",node->expires);
	fprintf(LOG, " | lat=%d",node->pos_vector.latitude);
	fprintf(LOG, " | long=%d",node->pos_vector.longitude);
	fprintf(LOG, " | alt=%d",node->pos_vector.altitude);
	fprintf(LOG, " | sp=%d",node->pos_vector.speed);
	fprintf(LOG, " | hd=%d |\n",node->pos_vector.heading);
	if (dbg_level > 0)  fflush(LOG);
}

/**
 * print the given list of nodes
 * @param neighbor_list nodes list to print
 */

void itsnet_neighbor_list_print(
    const struct list_head* neighbor_list)
{
	struct list_head* node_list_entry;

	fprintf(LOG, "======================================= neighbor list ========================================\n");
	list_for_each(node_list_entry, neighbor_list)
	{
		itsnet_node * i;
		i = list_entry(node_list_entry, struct itsnet_node, list);
		itsnet_neighbor_list_print_node(i);
	}
	fprintf(LOG, "==============================================================================================\n");
	if (dbg_level > 0)  fflush(LOG);
}

/**
 * free the given list of nodes
 * @param neighbor_list nodes list to free
 */

void itsnet_neighbor_list_free(
    struct list_head* neighbor_list)
{
	struct list_head *node_list_entry, *i;
	list_for_each_safe(node_list_entry, i, neighbor_list)
	{
		itsnet_node* node;
		node = list_entry(node_list_entry, struct itsnet_node, list);
		//itsnet_neighbor_list_remove_node(node);
	}
}

/**
 * remove the given node from list
 * @param itsnet_node node to remove
 */
void itsnet_neighbor_list_remove_node(
    struct tq_elem *tqe)
{

	itsnet_node * node;
	node=  tq_data(tqe,  itsnet_node, tqe);
	list_del(&node->list);
	free(node);

}



/**
 * update the given node
 * @param itsnet_node node to remove
 */

void itsnet_neighbor_list_update_node(
    struct itsnet_node * node)
{

	struct timespec exp_in;

	tssetmsec(exp_in,5000);
	fprintf(LOG, "\titsnet_neighbor_list_update_node (");
	print_node_id(node->node_id);
	fprintf(LOG, ")\n");
	del_task(&node->tqe);
	add_task_rel(&exp_in, &(node->tqe), itsnet_neighbor_list_remove_node);
	if (dbg_level > 0)  fflush(LOG);
}




itsnet_node_id  itsnet_select_forwarder(
    struct list_head * neighbor_list,
    struct itsnet_position_vector pos,
    uint32_t lat_dest,
    uint32_t long_dest)
{
	double dist1;
	double distmp=5000;
	struct list_head* node_list_entry;
	struct itsnet_node_id  node_id;

	list_for_each(node_list_entry, neighbor_list)
	{
		itsnet_node* i;
		i = list_entry(node_list_entry, struct itsnet_node, list);

		dist1= 1000 * haversine_distance(i->pos_vector.latitude,i->pos_vector.longitude,lat_dest,long_dest);
		fprintf(LOG, "dist1 = %f\n", dist1);
		
		if(dist1<distmp)
		{
			node_id=i->node_id;
			distmp=dist1;
		}
	}	

	fprintf(LOG, "\tforward to ");
	print_node_id(node_id);
	fprintf(LOG, "\n");
	return node_id;
}

void print_node_id( struct itsnet_node_id  node_id)
{
	fprintf(LOG, "%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x",
	       (node_id.id[0]&0xFF),
	       (node_id.id[1]&0xFF),
	       (node_id.id[2]&0xFF),
	       (node_id.id[3]&0xFF),
	       (node_id.id[4]&0xFF),
	       (node_id.id[5]&0xFF),
	       (node_id.id[6]&0xFF),
	       (node_id.id[7]&0xFF));
}

void print_mac_id( struct mac_addr  mac_id)
{
	fprintf(LOG, "%2.2x:%2.2x:%2.2x:%2.2x:%2.2x:%2.2x",
	       (mac_id.address[0]&0xFF),
	       (mac_id.address[1]&0xFF),
	       (mac_id.address[2]&0xFF),
	       (mac_id.address[3]&0xFF),
	       (mac_id.address[4]&0xFF),
	       (mac_id.address[5]&0xFF));
}
