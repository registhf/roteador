#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <argp.h>
#include "header.h"


// Instancia as variáveis globais
unsigned int		ROUTER_ID;
unsigned int 		TRANSM_TIMEOUT;
unsigned int 		TRANSM_MAX_ATTEMPTS;
unsigned int 		TRANSM_USLEEP_TIME;
short unsigned int 	LAST_SENT_DGRAM_ID;


int main(int argc, char const *argv[]) {
	Router R;
	Graph G;
	pthread_t L[3];
	t_arg *arg = malloc(sizeof(t_arg));

	parseArgs(argc, argv);
	printf("Carregando configuração do roteador #%d...\n", ROUTER_ID);

	// Carrega para a lista r as configurações dos roteadores lendo
	// os dados do arquivo roteador.config
	R = readRouterConfig();
	if (getRouter(R, ROUTER_ID) == NULL) {
		printf("ERRO: ID definida para o roteador não está configurada!\n");
		printf("\nDigite: ./Router --help, caso precise de ajuda\n");
		exit(1);
	}

	// Carrega para o grafo G as configurações dos enlaces lendo
	// os dados do arquivo enlaces.config
	G = readLinkConfig();
	if (getGraphNode(G, ROUTER_ID) == -1) {
		printf("ERRO: Verifique a configuração de enlaces! Roteador offline.\n");
		exit(1);
	}

	// Atualiza parent e dist do grafo baseado nas configurações de enlace
	// carregadas para o grafo G.
	GraphSPT(G, getGraphNode(G, ROUTER_ID), G->parent, G->dist);

	LAST_SENT_DGRAM_ID = 0;

	arg->R = R;
	arg->G = G;

	pthread_create(&L[0], NULL, (void *)recieveDatagram, (void *)arg);
	pthread_create(&L[1], NULL, (void *)userInterface, (void *)arg);
	pthread_create(&L[2], NULL, (void *)transmissionControl, NULL);

	pthread_join(L[1], NULL);

	pthread_cancel(L[0]);
	pthread_cancel(L[2]);
	pthread_join(L[0], NULL);
	pthread_join(L[2], NULL);

	free(arg);
	destroyGraph(G);
	destroyRouterList(R);
	destroyGlobalQueue();
	printf("Ok!\n");
	return 0;
}