#ifndef CBF_H
#define CBF_H


#include "itsnet_header.h"
#include "config.h"
#include "tqueue.h"

int itsnet_contention_send(struct itsnet_packet *p);
int itsnet_contention_handle(struct itsnet_packet *p);


#endif
