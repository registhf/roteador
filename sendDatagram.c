#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include "header.h"

static void generateDatagramID(Message data);

size_t OUT_BUFF_LEN;

void sendDatagram(Packet p, t_arg *arg) {
	struct sockaddr_in si_other;
	int s, slen = sizeof(si_other);

	generateDatagramID(p->data);

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("ERRO: Não foi possível criar o socket\n");
		return;
	}

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;
	si_other.sin_port = htons(p->port);

	if (inet_aton(p->IP , &si_other.sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	void *serial_data = packDatagram(p->data);

	if (sendto(s, serial_data, OUT_BUFF_LEN, 0, (struct sockaddr *)&si_other, slen) == -1) {
		printf("ERRO: Não foi possível enviar...\n");
		exit(1);
	}

	free(serial_data);
	close(s);
}

static void generateDatagramID(Message data) {
	LAST_SENT_DGRAM_ID++;
	if (LAST_SENT_DGRAM_ID > MAX_DGRAM_ID) {
		LAST_SENT_DGRAM_ID = 0;
	}

	data->ID 	= LAST_SENT_DGRAM_ID;
}

