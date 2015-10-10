#include <stdlib.h>
#include <curses.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include "header.h"


long getMillisecondsOfDay() {
	struct timeval tim;

	gettimeofday(&tim, NULL);

	return (long)((tim.tv_sec+(tim.tv_usec/1000000.0)) * 1000);
}

char *getTimeStr() {
	struct timeval tv;
	struct tm* ptm;
	long milliseconds;
	char *time_string = (char *)malloc(sizeof(char) * 15);
	char *ms = (char *)malloc(sizeof(char) * 5);

	gettimeofday (&tv, NULL);
	ptm = localtime (&tv.tv_sec);

	strftime(time_string, 15, "%H:%M:%S", ptm);

	milliseconds = tv.tv_usec / 1000;
 	snprintf(ms, 5, ".%03ld\n", milliseconds);

 	strcat(time_string, ms);
 	free(ms);
 	return time_string;
}

void printTime() {
	char *aux = getTimeStr();

	printf("\r" BOLDWHITE "%s:\033[0m " RESET, aux);

	free(aux);
}
