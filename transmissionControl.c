#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <unistd.h>
#include "header.h"

Queue TRANSMIT_QUEUE;

static void initQueue();
static void dequeuePacket(Packet packet);
static void sendTo(struct sockaddr_in *si_other, Packet p, int s);

int transmissionControl() {
	Packet p, tmp;
	struct sockaddr_in si_other;
	int s;

	printTime();
	printf("Fila de transmissão iniciada!\n");
	initQueue();

	if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1) {
		printTime();
		printf("ERRO: Não foi possível criar o socket. Controle de transmissão não iniciado.\n");
		return -1;
	}

	memset((char *) &si_other, 0, sizeof(si_other));
	si_other.sin_family = AF_INET;

	#define divi 10.0
	while (1) {
		usleep(TRANSM_USLEEP_TIME);
		if (TRANSMIT_QUEUE->N == 0) continue;

		pthread_mutex_lock(&TRANSMIT_QUEUE->mutex);

		for (p = TRANSMIT_QUEUE->first; p != NULL; p = tmp) {
			tmp = p->next;

			if (p->delivered == 1) {
				printTime();
				printf("Mensagem #%d enviada para %d com sucesso!\n", p->data->ID, p->data->destID);

				dequeuePacket(p);
			} else if (p->type == TP_FORWARD || p->type == TP_CONFIRM) {
				sendTo(&si_other, p, s);

				dequeuePacket(p);
			} else if (p->attempts == 0) {
				sendTo(&si_other, p, s);

				p->timestamp = getMillisecondsOfDay();

				p->attempts++;
				printTime();
				printf("Enviando mensagem #%d para roteador %d.\n", p->data->ID, p->data->destID);
			} else if (p->attempts > 0) {
				long elapsed = getMillisecondsOfDay() - p->timestamp;

				if (p->attempts >= TRANSM_MAX_ATTEMPTS && elapsed > TRANSM_TIMEOUT) {
					printTime();
					printf("Não foi possível enviar a mensagem #%d ao roteador %d.\n", p->data->ID, p->data->destID);

					dequeuePacket(p);
				} else {
					if (elapsed > TRANSM_TIMEOUT) {
						printTime();
						printf("Tentando enviar o pacote #%d para o roteador %d pela %dª vez.\n", p->data->ID, p->data->destID, p->attempts+1);
						sendTo(&si_other, p, s);

						p->timestamp = getMillisecondsOfDay();
						p->attempts++;
					}
				}
			}
		}

		pthread_mutex_unlock(&TRANSMIT_QUEUE->mutex);
	}

	return 0;
}

static void sendTo(struct sockaddr_in *si_other, Packet p, int s) {
	void *serial_data;
	size_t slen = sizeof(*si_other);
	si_other->sin_port = htons(p->port);

	if (inet_aton(p->IP , &si_other->sin_addr) == 0) {
		printTime();
		fprintf(stderr, "inet_aton() failed\n");
		exit(1);
	}

	serial_data = packDatagram(p->data);

	if (sendto(s, serial_data, OUT_BUFF_LEN, 0, (struct sockaddr *)si_other, slen) == -1) {
		printTime();
		printf("ERRO: Não foi possível enviar...\n");
	}

	free(serial_data);
}


void queuePacket(Packet packet) {
	Packet p;

	pthread_mutex_lock(&TRANSMIT_QUEUE->mutex);

	if (TRANSMIT_QUEUE->N == 0) {
		packet->next = NULL;
		TRANSMIT_QUEUE->first = packet;
	} else {
		for (p = TRANSMIT_QUEUE->first; p->next != NULL; p = p->next);
		p->next = packet;
		packet->next = NULL;
	}

	TRANSMIT_QUEUE->N++;

	pthread_mutex_unlock(&TRANSMIT_QUEUE->mutex);
}


void confirmDelivery(int destID, short unsigned int ID) {
	Packet p;

	for (p = TRANSMIT_QUEUE->first; p != NULL; p = p->next)
		if (p->data->destID == destID && p->data->ID == ID)
			p->delivered = 1;
}


static void dequeuePacket(Packet packet) {
	Packet p, ant = NULL;

	if (TRANSMIT_QUEUE->N == 0) return;

	for (ant = p = TRANSMIT_QUEUE->first; p != NULL && p != packet; ant = p, p = p->next);

	if (p == packet) {
		if (p == TRANSMIT_QUEUE->first)
			TRANSMIT_QUEUE->first = p->next;
		else
			ant->next = p->next;

		destroyPacket(p);

		TRANSMIT_QUEUE->N--;
	}
}


static void initQueue() {
	TRANSMIT_QUEUE = (Queue)malloc(sizeof(control_queue));
	TRANSMIT_QUEUE->N = 0;
	TRANSMIT_QUEUE->first = NULL;
	pthread_mutex_init(&TRANSMIT_QUEUE->mutex, NULL);
}

void printFila() {
	Packet p;
	printf("------------------------\n");
	printf("Tamanho da fila: %d\n", TRANSMIT_QUEUE->N);
	printf("ID \t| Source \t| Dest \t| Type \t| Type \t| Attempts \t| Delivered\n");
	for (p = TRANSMIT_QUEUE->first; p != NULL; p = p->next) {
		printf(" %d \t| %d \t\t|%d \t|%d \t|%d \t|%d \t\t| %u\n", p->data->ID, p->data->srcID, p->data->destID, p->data->type, p->type, p->attempts, p->delivered);
	}
	printf("------------------------\n");
}
