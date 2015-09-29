#include <stdlib.h>
#include <string.h>
#include "header.h"

static size_t calcBufferLen(Datagram data);
static void serializeString(void **buffer, char *str);
static void serializeInt(void **buffer, int value);
static void serializeShortUnsignedInt(void **buffer, short unsigned int value);
static void serializeUnsignedChar(void **buffer, unsigned char value);

void *packDatagram(Datagram data) {
	void *buffer = NULL, *initial = NULL;

	OUT_BUFF_LEN 		= calcBufferLen(data);
	initial = buffer 	= malloc(OUT_BUFF_LEN);
	memset(buffer, 0, OUT_BUFF_LEN);

	serializeUnsignedChar(		&buffer, data->type);
	serializeUnsignedChar(		&buffer, data->code);
	serializeShortUnsignedInt(	&buffer, data->ID);
	serializeShortUnsignedInt(	&buffer, data->TTL);
	serializeInt(				&buffer, data->destID);
	serializeInt(				&buffer, data->srcID);
	serializeString(			&buffer, data->message);

	return initial;
}

static size_t calcBufferLen(Datagram data) {
	return 	sizeof(int)*2 + sizeof(short unsigned int)*2 + (sizeof(unsigned char)+1)*2
	 		+ (sizeof(char)*strlen(data->message))+1;
}

static void serializeString(void **buffer, char *str) {
	if (*str == '\0') {
		*(char *)*buffer = *str;
		*buffer += sizeof(char);
	} else {
		do {
			*(char *)*buffer = *str;
			*buffer += sizeof(char);
			str++;
		} while (*str != '\0');
		*buffer += sizeof(char);
	}
}

static void serializeInt(void **buffer, int value) {
	*(int *)*buffer = value;
	*buffer += sizeof(int);
}

static void serializeShortUnsignedInt(void **buffer, short unsigned int value) {
	*(short unsigned int *)*buffer = value;
	*buffer += sizeof(short unsigned int);
}

static void serializeUnsignedChar(void **buffer, unsigned char value) {
	*(unsigned char *)*buffer = value;
	*buffer += sizeof(unsigned char);
}