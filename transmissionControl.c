#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include "header.h"

Queue TRANSMIT_QUEUE;
static void initQueue();
static void dequeuePacket(Packet packet);
static void sendTo(struct sockaddr_in *si_other, Packet p, int s);

void printFila();

int transmissionControl() {
	Packet p;
	struct sockaddr_in si_other;
	int s;

	printf("Fila de transmissão iniciada!\n");
	initQueue();

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printf("ERRO: Não foi possível criar o socket. Controle de transmissão não iniciado.\n");
		return -1;
	}

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;

	while (1) {
		usleep(TRANSMISSION_USLEEP_TIME);
		if (TRANSMIT_QUEUE->N == 0) continue;
		//printFila();

		for (p = TRANSMIT_QUEUE->first; p != NULL; p = p->next) {
			if (p->delivered == 1) {
				printf("Mensagem #%d enviada para %d com sucesso!\n", p->data->ID, p->data->destID);
				dequeuePacket(p);
			} else if (p->type == TP_FORWARD || p->type == TP_CONFIRM) {
				sendTo(&si_other, p, s);
				dequeuePacket(p);
			} else if (p->attempts == 0) {
				sendTo(&si_other, p, s);

				p->timestamp = time(0);
				p->attempts++;
				printf("Enviando mensagem #%d para roteador %d.\n", p->data->ID, p->data->destID);
			} else if (p->attempts > 0) {
				double elapsed = difftime(time(0), p->timestamp);

				if (p->attempts >= TRANSMISSION_MAX_ATTEMPTS && elapsed > TRANSMISSION_TIMEOUT) {
					printf("Não foi possível enviar a mensagem #%d ao roteador %d.\n", p->data->ID, p->data->destID);
					dequeuePacket(p);
				} else {
					if (elapsed > TRANSMISSION_TIMEOUT) {
						printf("Tentando enviar o pacote #%d para o roteador %d pela %dª vez.\n", p->data->ID, p->data->destID, p->attempts+1);
						sendTo(&si_other, p, s);

						p->timestamp = time(0);
						p->attempts++;
					}
				}
			}
		}
	}

	return 0;
}

static void sendTo(struct sockaddr_in *si_other, Packet p, int s) {
	void *serial_data;
	size_t slen = sizeof(*si_other);
	si_other->sin_port = htons(p->port);

	if (inet_aton(p->IP , &si_other->sin_addr) == 0) {
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	serial_data = packDatagram(p->data);

	if (sendto(s, serial_data, OUT_BUFF_LEN, 0, (struct sockaddr *)si_other, slen) == -1) {
		printf("ERRO: Não foi possível enviar...\n");
	}

	free(serial_data);
}


void queuePacket(Packet packet) {
	Packet p;

	if (TRANSMIT_QUEUE->N == 0) {
		packet->next = NULL;
		TRANSMIT_QUEUE->first = packet;
	} else {
		for (p = TRANSMIT_QUEUE->first; p->next != NULL; p = p->next);
		p->next = packet;
		packet->next = NULL;
	}

	TRANSMIT_QUEUE->N++;
}


void confirmDelivery(int destID, short unsigned int ID) {
	Packet p;
	for (p = TRANSMIT_QUEUE->first; p != NULL; p = p->next) {
		if (p->data->destID == destID && p->data->ID == ID) {
			p->delivered = 1;
			//printf("#%d -> %d confirmado!\n", ID, destID);
		}
	}
}


static void dequeuePacket(Packet packet) {
	Packet p, ant = NULL;
	if (TRANSMIT_QUEUE->N == 0) return;

	for (p = TRANSMIT_QUEUE->first; p != NULL && p != packet; p = p->next) {
		ant = p;
	}

	if (p == packet) {
		if (ant != NULL)
			ant->next = p->next;
		destroyPacket(packet);

		TRANSMIT_QUEUE->N--;
	}
}


static void initQueue() {
	TRANSMIT_QUEUE = (Queue)malloc(sizeof(control_queue));
	TRANSMIT_QUEUE->N = 0;
	TRANSMIT_QUEUE->first = NULL;
}


void printFila() {
	Packet p;
	printf("------------------------\n");
	printf("Tamanho da fila: %d\n", TRANSMIT_QUEUE->N);
	printf("ID \t| Source \t| Dest \t| Type \t| Type \t| Attempts \t| Delivered\n");
	for (p = TRANSMIT_QUEUE->first; p != NULL; p = p->next) {
		printf(" %d \t| %d \t\t|%d \t|%d \t|%d \t\t|%d \t\t| %u\n", p->data->ID, p->data->srcID, p->data->destID, p->data->type, p->type, p->attempts, p->delivered);
	}
	printf("------------------------\n");
}