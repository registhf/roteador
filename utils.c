#include <stdlib.h>
#include <stdio.h>
#include "header.h"

void destroyDatagram(Datagram data) {
	free(data->message);
	free(data);
}

void destroyPacket(Packet R) {
	free(R->IP);
	destroyDatagram(R->data);

	free(R);
}

void destroyRouterList(Router r) {
	Router p, q;

	for (p = r; p != NULL; p = q) {
		q = p->next;
		free(p->IP);
		free(p);
	}
}

void destroyGraph(Graph G) {
	int v;
	Link p, q;

	for (v = 0; v < G->V; v++) {
		for (p = G->adj[v]; p != NULL; p = q) {
			q = p->next;
			free(p);
		}
	}

	free(G->adj);
	free(G->ID);
	free(G->parent);
	free(G->dist);
	free(G);
}

void destroyGlobalQueue() {
	Packet p, a = NULL;

	if (TRANSMIT_QUEUE->N > 0) {
		for (p = TRANSMIT_QUEUE->first; p != NULL; a = p, p = p->next)
			if (a != NULL)
				destroyPacket(a);

		if (a != NULL)
			destroyPacket(a);
	}

	free(TRANSMIT_QUEUE);
}

void printRouters(Router r) {
	Router p;
	printf("ID\tPORT\t\tIP\n------------------------------------------------\n");
	for (p = r; p; p = p->next) {
		printf("[%d]\t[%d]\t\t%s\n", p->ID, p->port, p->IP);
	}
}

void printNetworkConfig(Graph G) {
	int v;
	printf("\n\nEnlaces ativos:\n-------------------------------------------------");
	for (v = 0; v < G->V; v++){
		Link a;
		if (G->adj[v] == NULL)
			continue;
		printf("\n%d ", G->ID[v]);
		for (a = G->adj[v]; a != NULL; a = a->next) {
			if (a != G->adj[v])
				printf(" \\");
			printf("--%d--> %d \n", a->cost, G->ID[a->w]);
		}
	}
	printf("\n");

	printf("Partindo do %d: \n", ROUTER_ID);
	for (v = 0; v < G->V; v++) {
		printf("Pra ir pro %d, passa primeiro pelo %d\n", G->ID[v], G->ID[G->parent[v]]);
	}
}

void printMessageData(Datagram data) {
	printf("ID: %d | Type: %d | Code: %d \n", data->ID, data->type, data->code);
	printf("TTL: %d\n", data->TTL);
	//printf("Destination: %d: %s:%d\n", data->destID, data->destIP, data->destPort);
	//printf("Source: %d: %s:%d\n", data->srcID, data->srcIP, data->srcPort);
	printf("Datagram: [%s]\n",	data->message);
}
