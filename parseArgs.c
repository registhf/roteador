#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include "header.h"

static error_t parse_opt(int key, char *arg, struct argp_state *state);

const char *argp_program_version = "Router 1.0";
const char *argp_program_bug_address = "<registhiagofeyh@gmail.com>";

/* Program documentation. */
static char doc[] = "Roteador - T1 Redes";

static char args_doc[] = "--ID=[ID do roteador] | -i[ID do roteador]";

/* Opções aceitas como argumento. */
static struct argp_option options[] = {
  {"ID",		'i', "ID do roteador",			OPTION_NO_USAGE, "ID do roteador" },
  {"timeout",	't', "Tempo em ms",				OPTION_NO_USAGE, "Tempo máximo de espera por resposta em uma tentativa" },
  {"retries",	'r', "Número de tentativas",	OPTION_NO_USAGE, "Número máximo de tentativas de envio de uma mensagem" },
  {"sleep",		's', "Tempo em ms",				OPTION_NO_USAGE, "Tempo de espera para processar a fila de transmissão" },
  {"ifdelay",	'd', "Tempo em us",				OPTION_NO_USAGE, "Tempo entre o envio de uma frame e outra na transmissão" },
  {0}
};

/* Used by main to communicate with parse_opt. */
struct arguments {
	char *arg1;
	char **strings;
	unsigned int ID;
	unsigned int timeout;
	unsigned int retries;
	unsigned int sleep;
	unsigned int ifdelay;
};

static struct argp argp = { options, parse_opt, args_doc, doc };


void parseArgs(int argc, char const *argv[]) {
	struct arguments arguments;

	/* Default values. */
	arguments.ID 		= 0;
	arguments.timeout 	= DF_TRANSM_TIMEOUT;
	arguments.retries 	= DF_TRANSM_MAX_ATTEMPTS;
	arguments.sleep 	= DF_TRANSM_USLEEP_TIME;
	arguments.ifdelay	= DF_INTERFRAME_DELAY;

	argp_parse(&argp, argc, (void *)argv, 0, 0, &arguments);

	ROUTER_ID 			= arguments.ID;
	TRANSM_TIMEOUT 		= arguments.timeout;
	TRANSM_MAX_ATTEMPTS = arguments.retries;
	TRANSM_USLEEP_TIME 	= arguments.sleep;
	INTERFRAME_DELAY 	= arguments.ifdelay;

	//printf("ID: %d, timeout: %d, retries: %d, sleep: %d\n", ROUTER_ID, TRANSM_TIMEOUT, TRANSM_MAX_ATTEMPTS, TRANSM_USLEEP_TIME);
}


static int hasID = 0;
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
	struct arguments *arguments = state->input;

	switch (key) {
		case 'i':
			hasID = 1;
			arguments->ID = arg ? atoi(arg) : 0;
			break;
		case 't':
			arguments->timeout = arg ? atoi(arg) : DF_TRANSM_TIMEOUT;
			break;
		case 'r':
			arguments->retries = arg ? atoi(arg) : DF_TRANSM_MAX_ATTEMPTS;
			break;
		case 's':
			arguments->sleep = arg ? atoi(arg)*1000 : DF_TRANSM_USLEEP_TIME;
			break;
		case 'd':
			arguments->ifdelay = arg ? atoi(arg) : DF_INTERFRAME_DELAY;
			break;

		case ARGP_KEY_END:
			if (!hasID) {
				printf(ERROR "O " BOLD "ID" RESETBOLD " do roteador é " BOLD UNDERLINE "obrigatório" RESETBOLD RESETULINE " (--ID=Num)" RESET "\n");
				argp_usage(state);
			}
			break;

		case ARGP_KEY_ARG:
			arguments->arg1 = arg;
			arguments->strings = &state->argv[state->next];
			state->next = state->argc;

			break;
		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}
