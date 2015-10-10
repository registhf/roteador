#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "header.h"

unsigned int INTERFACE_DEST;

static int isNumber(char *destStr);
static void fixMessageInput(char *message);
static void storm(int dest, t_arg *arg);

void userInterface(t_arg *arg) {
	char message[(MAX_LEN_MESSAGE*5)+1];
	char destStr[MAX_LEN_INPUT_DEST];
	INTERFACE_DEST = UI_NO_DEST;

	printf(BOLDWHITE "\nParâmetros carregados:\n" RESET);
	printf(" Timeout: \t\t" GREEN "%d \tms\n" RESET, TRANSM_TIMEOUT);
	printf(" Tempo de espera: \t" GREEN "%d \tms\n" RESET, TRANSM_USLEEP_TIME),
	printf(" Tentativas de envio: \t" GREEN "%d\n\n" RESET, TRANSM_MAX_ATTEMPTS);

	usleep(500000);
	printf(BOLDWHITE "\nInstruções: \n" RESET);
	printf("--> Informe o " BOLDWHITE "ID" RESET " do destino, depois a mensagem. (Com " INVERSE " enter " RESET ")\n");
	printf("--> Informe " BOLDWHITE "%d" RESET " para sair\n\n", UI_CLOSE);

	while(1) {
		INTERFACE_DEST = UI_NO_DEST;

		printf("#> ");

		if (!fgets(destStr, MAX_LEN_INPUT_DEST, stdin))
			continue;

		if (!isNumber(destStr)) {
			if (strlen(destStr) > 1)
				printf(BOLDWHITE "Destino inválido," RESET " verifique.\n");
			continue;
		}

		INTERFACE_DEST = atoi(destStr);

		if (INTERFACE_DEST == UI_CLOSE) {
			printf("\n\n" YELLOW "Desligando... " RESET);
			return;
		}

		printf(BOLDGREEN "%d> " RESET, INTERFACE_DEST);

		// Caracteres acentuados ocupam duas posições, deixei o fgets aceitando
		// até cinco vezes o máximo, mas trato o limite em fixMessageInput daí.
		if (!fgets(message, (MAX_LEN_MESSAGE*5)+1, stdin))
			continue;

		fixMessageInput(message);

		if (strcmp(message, "storm") == 0)
			storm(INTERFACE_DEST, arg);
		else
			sendMessage(INTERFACE_DEST, message, arg);
	}
}

static int isNumber(char *destStr) {
	int i;

	for (i = 0; i < MAX_LEN_INPUT_DEST; i++) {
		if (destStr[i] == '-')
			continue;

		if (iscntrl(destStr[i])) {
			if (i > 0)  return 1;
			else 		return 0;
		}

		if (isalnum(destStr[i]) && !isdigit(destStr[i]))
			return 0;
	}

	return 1;
}

static void fixMessageInput(char *message) {
	int i, max = MAX_LEN_MESSAGE, c = 1;

	for (i = 0; i < max; i++) {
		if ((int)message[i] < 0) {
			if (c) {
				max++;
				c = 0;
			} else
				c = 1;
		}

		if (message[i] == '\n')
			message[i] = '\0';

		if (iscntrl(message[i]))
			break;
	}

	message[i] = '\0';
}

static void storm(int dest, t_arg *arg) {
	int i;

	for (i = 0; i < 15; i++)
		sendMessage(dest, "DIE!", arg);
}