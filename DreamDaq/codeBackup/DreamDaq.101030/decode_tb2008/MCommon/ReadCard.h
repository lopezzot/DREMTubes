#ifndef READCARD_H_SEEN
#define READCARD_H_SEEN

#include <stdio.h>

int ReadCard(char *card_flag, void *store, char type, FILE *fptr, char *id_flag=NULL);
int ReadCard(char *card_flag, void *store, char type, FILE *fptr, int id);

#endif
