#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

// Limites
#define MAX_LEN_MESSAGE 	100

//					  su_int u_char int   2ip  		 message
#define INCOMING_BUFLEN 2*2 + 1*2 + 4*4 + 16*2 + MAX_LEN_MESSAGE+1

#define MAX_LEN_INPUT_DEST 	10+2 // 10 max strlen int + 1 (\n) + 1 (\0) (fgets)

#define MAX_DGRAM_ID 		32000
#define DEFAULT_TTL 		30

#define NOT_USED 		0

// Tipos de mensagem
#define T_MESSAGE 		1
#define T_CONFIRMATION 	2

#define UI_CLOSE 		-1
#define UI_NO_DEST 		-2


#define TRANSMISSION_USLEEP_TIME 500000 // = 500ms
#define TRANSMISSION_TIMEOUT 3 			// seconds
#define TRANSMISSION_MAX_ATTEMPTS 3


/*
 * Estrutura utilizada para representar a lista
 * com a configuração dos roteadores
 */
typedef struct router {
    int ID;
    int port;
    char *IP;
    struct router *next;
} router;

typedef struct router *Router;


/*
 * Etrutura utilizada para representar o
 * nodo de um enlace
 */
typedef struct node {
	int ID;
	int w;
	int cost;
	struct node *next;
} node;

typedef struct node *Link;

/*
 * Grafo utilizado para representar a topologia
 * de rede dos roteadores.
 */
typedef struct graph {
	int V;
	int A;
	Link *adj;
	int *ID;
	int aaaa;
	int *parent;
	int *dist;
} graph;

typedef struct graph *Graph;



typedef struct datagram {
	short unsigned int ID;
	unsigned char type;
	unsigned char code;
	short unsigned int TTL;
	char *destIP;
	char *srcIP;
	int destPort;
	int srcPort;
	int destID;
	int srcID;
	char *message;
} datagram;

typedef struct datagram *Datagram;


typedef struct packet {
	char *IP;
	int port;
	Datagram data;
	time_t timestamp;
	int attempts;
	unsigned char delivered;
	unsigned char forward;
	struct packet *next;
} packet;


typedef struct packet *Packet;


typedef struct t_arg {
	Router R;
	Graph  G;
	Datagram data;
	int recv_len;
} t_arg;


typedef struct control_queue {
	unsigned int N;
	Packet first;
} control_queue;

typedef struct control_queue *Queue;


extern unsigned int		  ROUTER_ID;
extern short unsigned int LAST_SENT_DGRAM_ID;
extern unsigned int 	  INTERFACE_DEST;
extern size_t 			  OUT_BUFF_LEN;
extern Queue 			  TRANSMIT_QUEUE;

/*
 * Lê as configurações dos roteadores no arquivo roteador.config.
 *
 * Retorna uma lista encadeada com todos os roteadores
 */
Router readRouterConfig();

/*
 * Imprime a lista de roteadores configurados
 */
void printRouters(Router r);

/*
 * Lê as configurações da topologia de rede dos roteadores do
 * arquivo enlaces.config.
 *
 * Retorna um grafo com lista de adjacência.
 */
Graph readLinkConfig();

int getGraphNode(Graph G, int ID);

/*
 * Imprime os enlaces ativos da rede
 */
void printNetworkConfig(Graph G);

/*
 * Busca um roteador nas lista de configuração dos roteadores
 * através do ID
 */
Router getRouter(Router r, int ID);

void destroyRouterList(Router r);

void userInterface(t_arg *arg);

void sendMessage(int dest, char *message, t_arg *arg);


/*
 * Grafos
 */

#define MAX_V 50

void 	insertLink(Graph G, int ID1, int ID2, int cost);

Graph 	initDigraph(int V);

Link 	createNode(int w, int cost, Link next);

void 	GraphSPT(Graph G, int s, int parent[], int dist[]);

void 	destroyGraph(Graph G);

/*
 * Rede
 */
void sendDatagram(Packet p, t_arg *arg);

void recieveDatagram(t_arg *arg);

void printMessageData(Datagram data);

Packet findOutputRoute(Graph G, Router R, Datagram data);

void *packDatagram(Datagram data);

Datagram unpackDatagram(void *buffer);

void sendConfirmation(int dest, int ID, t_arg *arg);

void destroyDatagram(Datagram data);
void destroyPacket(Packet R);

void waitConfirmation(t_arg *arg);

void sendConfirmationMessage(t_arg *arg);

void queuePacket(Packet packet);

int transmissionControl();

void confirmDelivery(int destID, short unsigned int ID);

void reinitInterface();