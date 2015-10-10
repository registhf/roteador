#include <stdio.h>
#include <string.h>
#include "header.h"

static Router newRouter(int ID, int port, char *IP, Router next);

/*
 * LEITURA DAS CONFIGURAÇÕES DOS ROTEADORES
 * Cria uma lista encadeada com as informações e retorna.
 */
Router readRouterConfig() {
	Router r;
	char IP[20];
	int ID, port;
	FILE *config;

	config = fopen("roteador.config", "r");
	if (!config)
		return NULL;

	r = NULL;
	while (fscanf(config, "%d %d %s", &ID, &port, IP) == 3) {
		r = newRouter(ID, port, IP, r);
	}

	if (ferror(config)) {
		printf(ERROR "Erro na leitura do arquivo de configuração dos roteadores." RESET "\n");
		fclose(config);
		return NULL;
	}

	fclose(config);
	return r;
}

static Router newRouter(int ID, int port, char *IP, Router first) {
	Router p, new = (Router)malloc(sizeof(router));

	new->ID = ID;
	new->port = port;
	new->IP = (char *)malloc((sizeof(char)*strlen(IP))+1);
	strcpy(new->IP, IP);

	if (first != NULL) {
		for (p = first; p->next != NULL; p = p->next);
		p->next = new;
		new->next = NULL;
		return first;
	} else {
		new->next = NULL;
		return new;
	}
}
