#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "header.h"

static Message createConfirmData(int dest, Router R);

void sendConfirmation(int dest, int ID, t_arg *arg) {
	Message data;
	Packet p;

	data = createConfirmData(dest, arg->R);
	if (!data)
		return;

	p = findOutputRoute(arg->G, arg->R, data);
	if (!p)
		return;

	sendDatagram(p, arg);

	destroyPacket(p);
}

static Message createConfirmData(int dest, Router R) {
	Router src  	= getRouter(R, ROUTER_ID);
	Router destR 	= getRouter(R, dest);

	if (!destR || !src) {
		printf("ERRO: Configuração do roteador não encontrada.\n");
		return NULL;
	}

	Message data = (Message)malloc(sizeof(message_data));

	data->srcID 	= ROUTER_ID;
	data->destID	= dest;
	data->message 	= (char *)malloc(sizeof(char));
	data->message[0]= '\0';
	data->destIP 	= (char *)malloc(1 + (sizeof(char) * strlen(destR->IP)));
	data->srcIP 	= (char *)malloc(1 + (sizeof(char) * strlen(src->IP)));
	data->destPort 	= destR->port;
	data->srcPort 	= src->port;
	data->type 		= T_CONFIRMATION;
	data->code 		= NOT_USED;
	data->TTL 		= DEFAULT_TTL;
	strcpy(data->destIP, destR->IP);
	strcpy(data->srcIP, src->IP);

	return data;
}