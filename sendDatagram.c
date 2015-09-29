#include <stdlib.h>
#include <time.h>
#include "header.h"

static void generateDatagramID(Datagram data);

size_t OUT_BUFF_LEN;

void sendDatagram(Packet p, t_arg *arg) {
	if (p->type == TP_NONE)
		generateDatagramID(p->data);

	p->attempts 	= 0;
	p->delivered 	= 0;

	queuePacket(p);
}

static void generateDatagramID(Datagram data) {
	if (data->type == TM_CONFIRM)
		return;


	LAST_SENT_DGRAM_ID++;
	if (LAST_SENT_DGRAM_ID > MAX_DGRAM_ID) {
		LAST_SENT_DGRAM_ID = 0;
	}

	data->ID 	= LAST_SENT_DGRAM_ID;
}

