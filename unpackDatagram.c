#include <stdlib.h>
#include <string.h>
#include "header.h"

static char *deserializeString(void **buffer);
static int deserializeInt(void **buffer);
static unsigned char deserializeUnsignedChar(void **buffer);
static short unsigned int deserializeShortUnsignedInt(void **buffer);

Datagram unpackDatagram(void *buffer) {
	Datagram data = (Datagram)malloc(sizeof(datagram));
	memset(data, 0, sizeof(datagram));

	data->ID 		= deserializeShortUnsignedInt(&buffer);
	data->type 		= deserializeUnsignedChar(&buffer);
	data->code 		= deserializeUnsignedChar(&buffer);
	data->TTL 		= deserializeShortUnsignedInt(&buffer);
	data->destIP 	= deserializeString(&buffer);
	data->srcIP 	= deserializeString(&buffer);
	data->destPort 	= deserializeInt(&buffer);
	data->srcPort 	= deserializeInt(&buffer);
	data->destID 	= deserializeInt(&buffer);
	data->srcID 	= deserializeInt(&buffer);
	data->message 	= deserializeString(&buffer);

	return data;
}

static char *deserializeString(void **buffer) {
	char *str = NULL;
	int i = 0;

	do {
		str = realloc(str, (i+1) * sizeof(char));
		str[i++] = *(char *)*buffer;
		*buffer += sizeof(char);
	} while (*(char *)*buffer != '\0');
	*buffer += sizeof(char);

	str = realloc(str, (i+1) * sizeof(char));
	str[i] = '\0';

	return str;
}

static int deserializeInt(void **buffer) {
	int r = *(int *)*buffer;
	*buffer += sizeof(int);
	return r;
}

static short unsigned int deserializeShortUnsignedInt(void **buffer) {
	short unsigned int r = *(short unsigned int *)*buffer;
	*buffer += sizeof(short unsigned int);
	return r;
}

static unsigned char deserializeUnsignedChar(void **buffer) {
	unsigned char r = *(unsigned char *)*buffer;
	*buffer += sizeof(unsigned char);
	return r;
}
