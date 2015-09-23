#include <stdlib.h>
#include "header.h"
#include "priorityQueue.h"

static int getOrSetGraphNode(Graph G, int ID);

Graph initDigraph(int V) {
	int v;
	Graph G = (Graph)malloc(sizeof(graph));
	G->V = V;
	G->A = 0;
	G->adj = malloc(V * sizeof(Link));
	G->ID = malloc(V * sizeof(int));
	for (v = 0; v < V; v++){
		G->adj[v] = NULL;
		G->ID[v] = -1;
	}

	G->parent = malloc(V * sizeof (int));
	G->dist = malloc(V * sizeof (int));

	return G;
}

Link createNode(int w, int cost, Link next) {
	Link a = malloc(sizeof(struct node));
	a->w = w;
	a->cost = cost;
	a->next = next;

	return a;
}

void insertLink(Graph G, int ID1, int ID2, int cost) {
	Link a;
	int v, w;

	v = getOrSetGraphNode(G, ID1);
	w = getOrSetGraphNode(G, ID2);

	for (a = G->adj[v]; a != NULL; a = a->next){
		if (a->w == w) return;
	}

	G->adj[v] = createNode(w, cost, G->adj[v]);
	G->adj[w] = createNode(v, cost, G->adj[w]);
	G->A += 2;
}

static int getOrSetGraphNode(Graph G, int ID) {
	int v;

	for (v = 0; v < G->V; v++) {
		if (G->ID[v] == -1 || G->ID[v] == ID) {
			G->ID[v] = ID;
			return v;
		}
	}

	return -1;
}

int getGraphNode(Graph G, int ID) {
	int v;

	for (v = 0; v < G->V; v++) {
		if (G->ID[v] == ID)
			return v;
	}

	return -1;
}

void GraphSPT (Graph G, int s, int parent[], int dist[]) {
	if (!G || s < 0 || !parent || !dist)
		return;

	int v0, w, frj[MAX_V];
	Link a;
	int c;
	for (w = 0; w < G->V; w++)
		parent[w] = frj[w] = -1;
	parent[s] = s;
	dist[s] = 0;

	PQinit( G->V);
	for (a = G->adj[s]; a != NULL; a = a->next) {
		w = a->w;
		c = a->cost;
		dist[w] = c;
		PQinsert( w, dist);
		frj[w] = s;
	}

	while (!PQempty( )) {
		v0 = PQdelmin( dist);
		parent[v0] = frj[v0];
		for (a = G->adj[v0]; a != NULL; a = a->next) {
			w = a->w, c = a->cost;
			if (parent[w] != -1) continue;
			if (frj[w] == -1) {
				dist[w] = dist[v0] + c;
				PQinsert( w, dist);
				frj[w] = v0;
			} else if (dist[w] > dist[v0] + c) {
				dist[w] = dist[v0] + c;
				PQdec( w, dist);
				frj[w] = v0;
			}
		}
	}
	PQfree( );
}