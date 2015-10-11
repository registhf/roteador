/*
 * DEFINIÇÃO DOS CÓDIGOS PARA AUXILIAR NAS CORES
 * E FORMATAÇÃO DAS MENSAGENS
 */
// Cores e formatação das mensagens
#define RESET		"\033[0m"			// Texto normal
#define BLACK		"\033[30m"
#define RED			"\033[31m"
#define GREEN		"\033[32m"
#define YELLOW		"\033[33m"
#define BLUE		"\033[34m"
#define MAGENTA		"\033[35m"
#define CYAN		"\033[36m"
#define WHITE		"\033[37m"
#define BOLDBLACK	"\033[1m\033[30m"
#define BOLDRED		"\033[1m\033[31m"
#define BOLDGREEN	"\033[1m\033[32m"
#define BOLDYELLOW	"\033[1m\033[33m"
#define BOLDBLUE	"\033[1m\033[34m"
#define BOLDMAGENTA	"\033[1m\033[35m"
#define BOLDCYAN	"\033[1m\033[36m"
#define BOLDWHITE	"\033[1m\033[37m"
#define UNDERLINE	"\033[4m"
#define RESETULINE	"\033[24m"
#define BOLD		"\033[1m"
#define RESETBOLD	"\033[22m"
#define ITALIC		"\033[3m"
#define BLINK		"\033[5m"
#define INVERSE		"\033[7m"
#define SUCCESS		BOLDGREEN 	"SUCCESS: " RESETBOLD
#define INFO		BOLDBLUE 	"INFO: " RESETBOLD
#define ERROR		BOLDRED 	"ERROR: " RESETBOLD
#define WARNING		BOLDYELLOW 	"WARNING: " RESETBOLD

/* ********************************************************************
 * Para desativar as cores, basta desabilitar as definições acima
 * e habilitar estas:
 *********************************************/
/*
#define RESET		""				// Texto normal
#define BLACK		""
#define RED			""
#define GREEN		""
#define YELLOW		""
#define BLUE		""
#define MAGENTA		""
#define CYAN		""
#define WHITE		""
#define BOLDBLACK	""
#define BOLDRED		""
#define BOLDGREEN	""
#define BOLDYELLOW	""
#define BOLDBLUE	""
#define BOLDMAGENTA	""
#define BOLDCYAN	""
#define BOLDWHITE	""
#define UNDERLINE	""
#define RESETULINE	""
#define BOLD		""
#define RESETBOLD	""
#define ITALIC		""
#define BLINK		""
#define INVERSE		""
#define SUCCESS		"SUCCESS: "
#define INFO		"INFO: "
#define ERROR		"ERROR: "
#define WARNING		"WARNING: "
*/