#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "header.h"

static void forwardDatagram(t_arg *arg);
static void processDatagram(t_arg *arg);

void recieveDatagram(t_arg *arg) {
	int s, recv_len;
	struct sockaddr_in si_me, si_other;
	unsigned int slen = sizeof(si_other);
	char buffer[INCOMING_BUFLEN];
	Datagram recievedData;

	printf("Criando socket do roteador...\n");
	Router host = getRouter(arg->R, ROUTER_ID);
	if (!host) {
		printf("ERRO: Configuração do roteador não encontrada. Impossível inicializar.\n");
		exit(1);
	}

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("ERRO: Falha no socket().\n");
		exit(1);
	}

	memset((char *) &si_me, 0, sizeof(si_me));

	si_me.sin_family = AF_INET;
	si_me.sin_port = htons(host->port);
	si_me.sin_addr.s_addr = htonl(INADDR_ANY);

	if (bind(s , (struct sockaddr*)&si_me, sizeof(si_me)) == -1) {
		printf("ERRO: Falha no bind(). Verifique se a porta não está sendo utilizada.\n");
		exit(1);
	}

	printf("Escutando!\n");

	while(1) {
		fflush(stdout);
		memset(buffer,'\0', INCOMING_BUFLEN);

		if ((recv_len = recvfrom(s, buffer, INCOMING_BUFLEN, 0, (struct sockaddr *)&si_other, &slen)) == -1) {
			printf("recvfrom()");
			exit(1);
		}

		recievedData = unpackDatagram(buffer);

		arg->data = recievedData;
		arg->recv_len = recv_len;
		if (recievedData->destID == ROUTER_ID) {
			processDatagram(arg);
		} else {
			forwardDatagram(arg);
		}
	}
}

static void forwardDatagram(t_arg *arg) {
	Packet p;

	printf("Roteador %d: Encaminhando mensagem #%d de %d para %d. (%d bytes)\n",
			ROUTER_ID, arg->data->ID, arg->data->srcID, arg->data->destID, arg->recv_len);
	arg->data->TTL--;
	p = findOutputRoute(arg->G, arg->R, arg->data);
	p->type = TP_FORWARD;
	sendDatagram(p, arg);
}



static void processDatagram(t_arg *arg) {
	if (arg->data->type == TM_MESSAGE) {
		printf("\nMensagem de %d: \"", arg->data->srcID);
		printf("%s\"\n", arg->data->message);

		sendConfirmation(arg->data->srcID, arg->data->ID, arg);
	} else if (arg->data->type == TM_CONFIRM)
		confirmDelivery(arg->data->srcID, arg->data->ID);

	destroyDatagram(arg->data);
}