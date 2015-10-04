#include <stdio.h>
#include "header.h"

static int findIDInList(int *list, int len, int ID);

/*
 * LEITURA DAS CONFIGURAÇÕES DOS ENLACES
 * Cria um grafo com lista de adjecência e peso nas arestas
 */
Graph readLinkConfig() {
	FILE *config;
	Graph G = NULL;
	int v, w, cost, ID[MAX_V], count = 0;

	for (v = 0; v < MAX_V; v++) {
		ID[v] = -1;
	}

	config = fopen("enlaces.config", "r");
	if (!config)
		return NULL;

	// Conta quantos roteadores diferentes foram declarados
	// no arquivo enlaces.config
	while (fscanf(config, "%d %d %d", &v, &w, &cost) == 3) {
		count += findIDInList(ID, count, v);
		count += findIDInList(ID, count, w);
	}

	if (feof(config)) {
		G = initDigraph(count); // Inicializa um grafo

		if (G) {
			fseek(config, 0, SEEK_SET);
			while (fscanf(config, "%d %d %d", &v, &w, &cost) == 3) {
				insertLink(G, v, w, cost); // Insere um enlace no grafo
			}
		}
	}

	if (ferror(config)) {
		printf("Erro na leitura do arquivo de configuração de enlaces.\n");
		fclose(config);
		return NULL;
	}

	fclose(config);
	return G;
}

static int findIDInList(int *list, int len, int ID) {
	int i;
	for (i = 0; i < MAX_V; i++) {
		if (list[i] == ID)
			return 0;
		else if (list[i] == -1) {
			list[i] = ID;
			return 1;
		}
	}

	return 0;
}
