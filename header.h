#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>

/********************************
 * Limites fixos
 *********************
 */
// Tamanho máximo da mensagem enviada pelo usuário
// Letras acentuadas, representadas por 2 chars, são
// tratados como 1 char. Ou seja, o usuário só vê
// a quantidade definida aqui, mas pode ser que sejam
// transmitidos mais caracteres.
#define MAX_LEN_MESSAGE 	100

// Tamanho do buffer de entrada
// Quando um pacote é recebido, são aceitos
// no máximo essa quantidade de bytes.
//					  		su_int 	u_char 	int     message 			fator_cagaço
#define INCOMING_BUFLEN 	((2*2 + 	1*2 + 	4*2 + 	MAX_LEN_MESSAGE+1)	*2)

// Tamanho máximo da entrada do usuário para
// representar o ID do roteador de destino.
// Essa entrada é lida com fgets e depois convertida
// para um inteiro.
#define MAX_LEN_INPUT_DEST 	(10+2) // 10 max strlen int + 1 (\n) + 1 (\0) (fgets)

// ID máximo de um pacote enviado para outro roteador
#define MAX_DGRAM_ID 		32000

// TTL dos pacotes. Definido, decrementado, mas não utilizado (por enquanto)
#define DEFAULT_TTL 		30

// Constante auxiliar para utilizar em uma propriedade que não
// é utilizada, dependendo da circunstância.
#define NOT_USED 			0


/************************************
 * DEFINIÇÕES AUXILIARES
 **************************
 */

// Tipos de mensagem
#define TM_MESSAGE 		1	// Mensagem de usuário
#define TM_CONFIRM 		2	// Mensagem de confirmação de recebimento
#define TP_NONE 		0	// Campo não utilizado
#define TP_CONFIRM 		1 	// Pacote de confirmação
#define TP_FORWARD 		2	// Pacote para encaminhamento

// Códigos da interface do usuário
#define UI_CLOSE 		-1	// Usuário solicitou o desligamento do roteador
#define UI_NO_DEST 		-2	// Usuário não definiu um destino para a mensagem

/************************************
 * VALOR PADRÃO DOS PARÂMETROS
 * Utilizados caso o usuário não informe como argumento do programa
 **************************
 */
// Tempo em microssegundos que a thread "transmissionControl" fica
// bloqueada entre um processamento da fila de transmissão e outro.
#define DF_TRANSM_USLEEP_TIME 	200000

// Tempo máximo de espera, em milissegundos, por uma confirmação de entrega de mensagem
#define DF_TRANSM_TIMEOUT 		3000

// Número máximo de tentativas de entregar uma mensagem
// Após isso um erro é gerado e a mensagem é descartada
#define DF_TRANSM_MAX_ATTEMPTS 	3


/***********************************************************
 * Estrutura utilizada para representar a lista
 * com a configuração dos roteadores
 **********************************
 */
typedef struct router {
    int ID;
    int port;
    char *IP;
    struct router *next;
} router;

typedef struct router *Router;


/******************************************************
 * Etrutura utilizada para representar o
 * nodo de um enlace
 ******************************
 */
typedef struct node {
	int ID;
	int w;
	int cost;
	struct node *next;
} node;

typedef struct node *Link;

/**********************************************************
 * Grafo utilizado para representar a topologia
 * de rede dos roteadores.
 ***********************************
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


/*************************************************************
 * Datagrama enviado para o socket.
 * Essa estrutura é convertida para uma cadeia de bytes e enviada.
 ***********************
 */
typedef struct datagram {
	unsigned char type;
	unsigned char code;
	short unsigned int ID;
	short unsigned int TTL;
	int destID;
	int srcID;
	char *message;
} datagram;

typedef struct datagram *Datagram;


/***********************************************
 * Pacote contendo informações para controle interno dos datagramas
 ********************************
 */
typedef struct packet {
	char *IP;					// IP para onde o datagrama será enviado
	int port;					// Porta do destino
	Datagram data;				// Dados que serão convertidos e enviados
	long timestamp;				// Timestamp da última tentativa de envio
	int attempts;				// Número de tentativas de envio do pacote
	unsigned char delivered;	// Flag que indica que o pacote já foi entregue
	unsigned char type; 		// Tipo de pacote (encaminhamento, confirmação, mensagem,...)
	struct packet *next;		// Próximo pacote na fila para ser processado
} packet;

typedef struct packet *Packet;


/******************************************************************
 * Estrutura auxiliar para passagem de argumentos para as threads
 **********************************
 */
typedef struct t_arg {
	Router R;
	Graph  G;
	Datagram data;
	int recv_len;
} t_arg;


/*********************************************************
 * Estrutura utilizada para a fila de controle dos
 * pacotes que precisam ser transmitidos
 ***********************************
 */
typedef struct control_queue {
	unsigned int N;
	pthread_mutex_t mutex;
	Packet first;
} control_queue;

typedef struct control_queue *Queue;


/*****************************************
 * VARIÁVEIS GLOBAIS
 ***********************
 */
extern unsigned int		  ROUTER_ID;			// ID do roteador definido por argumento --ID=1
extern short unsigned int LAST_SENT_DGRAM_ID;	// ID do último datagrama enviado
extern unsigned int 	  INTERFACE_DEST;		// Destino definido pelo usuário para envio da mensagem
extern size_t 			  OUT_BUFF_LEN;			// Tamanho do buffer de saída do socket
extern Queue 			  TRANSMIT_QUEUE;		// Fila de transmissão de pacotes

/****************************************
 * PARÂMETROS DO ROTEADOR
 * Recebe os valores default, definidos acima,
 * ou os argumentos passados pelo usuário (se existirem)
 ************************
 */
// Tempo em microssegundos que a thread "transmissionControl" fica
// bloqueada entre um processamento da fila de transmissão e outro.
extern unsigned int TRANSM_USLEEP_TIME;

// Tempo máximo de espera, em milissegundos, por uma confirmação de entrega de mensagem
extern unsigned int TRANSM_TIMEOUT;

// Número máximo de tentativas de entregar uma mensagem
// Após isso um erro é gerado e a mensagem é descartada
extern unsigned int TRANSM_MAX_ATTEMPTS;





/********************************************
 * FUNÇÕES GLOBAIS
 * **********************
 */
// Processa argc e argv para buscar o ID do roteador
// e demais parâmetros.
void parseArgs(int argc, char const *argv[]);

// Lê as configurações dos roteadores no arquivo roteador.config.
// Retorna uma lista encadeada com todos os roteadores
Router readRouterConfig();

// Imprime a lista de roteadores configurados
void printRouters(Router r);

// Lê as configurações da topologia de rede dos roteadores do
// arquivo enlaces.config.
// Retorna um grafo com lista de adjacência.
Graph readLinkConfig();

// Busca o nodo do Grafo G que possui o ID=ID
int getGraphNode(Graph G, int ID);

// Imprime os enlaces ativos da rede
void printNetworkConfig(Graph G);

// Busca o roteador na lista Router r que possuo o ID=ID
Router getRouter(Router r, int ID);

// Destrói a lista de roteadores r
void destroyRouterList(Router r);

// Inicializa a interface do usuário para receber
// os comandos para envio de mensagem
void userInterface(t_arg *arg);

// Envia uma mensagem do usuário para o destino especificado
void sendMessage(int dest, char *message, t_arg *arg);


/****************************
 * Funções para processamento do grafo da rede
 ****************
 */
// Número máximo de vértices
#define MAX_V 50

// Inicializa um grafo
Graph 	initDigraph(int V);

// Cria um nodo no grafo, neste caso, representa um roteador
// da rede
Link 	createNode(int w, int cost, Link next);

// Insere uma aresta, neste caso, representa um enlace entre
// dois roteadores
void 	insertLink(Graph G, int ID1, int ID2, int cost);

// Algoritmo de Dijkstra para o cálculo do caminho mais curto.
// Preenche o vetor de pais parent[] que é utilizado para determinar
// por qual roteador um datagrama deve ser enviado para chegar ao destino
void 	GraphSPT(Graph G, int s, int parent[], int dist[]);

// Destrói o grafo
void 	destroyGraph(Graph G);



/*************************************************
 * Funções globais para o roteador
 *************************
 */
// Recebe um pacote com a rota de saída já calculada
// e adiciona o mesmo na fila de transmissão de pacotes
void sendDatagram(Packet p, t_arg *arg);

// Thread que inicializa o socket na porta definida na
// configuração e aguarda o recebimento de pacotes.
// Processa os pacotes recebidos mostrando as mensagens
// recebidas, ou encaminhando o que for necessário
void recieveDatagram(t_arg *arg);

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

void destroyGlobalQueue();

void printFila();