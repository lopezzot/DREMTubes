#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ReadCard.h"

int ReadCard(char *card_flag, void *store, char type, FILE *fptr, int id) {
  char id_flag[100];
  sprintf(id_flag, "%d", id);
  return ReadCard(card_flag, store, type, fptr, id_flag);
}


int ReadCard(char *card_flag, void *store, char type, FILE *fptr, char *id_flag) {
  char *card_name, *card_val, *card_id;
  char  line[160];

  rewind(fptr);

  while (fgets(line, 160, fptr) != NULL) {    // read line by line
    card_name = strtok(line," \t\n");
    if (id_flag!=NULL) {
      card_id = strtok(NULL," \t\n");
    }
    card_val  = strtok(NULL," \t\n");
    
    if (   card_name    != NULL  && card_val     != NULL   // comment or empty line?
        && card_name[0] != '*'   && card_name[0] != '#') {
      
      if (strcmp(card_name, card_flag)!=0) {   // is this the card name we are looking for?
	continue;  // no
      }
      if ((id_flag!=NULL) &&                // is id_flag required
	  (strcmp(card_id, id_flag)!=0)) {  // is it the correct id_flag
	continue;  // no
      }

      switch (type) {
      case 'I':
	*((int *) store)   = (int) strtol(card_val, (char**)NULL, 10);
	break;
      case 'i':
	*((short *) store) = (short) strtol(card_val, (char**)NULL, 10);
	break;
      case 'U':
	*((unsigned int *) store)   
	  = (unsigned int) strtoul(card_val, (char**)NULL, 10);
	break;
      case 'u':
	*((unsigned short *) store) 
	  = (unsigned short) strtoul(card_val, (char**)NULL, 10);
	break;
      case 'f' :
	*((float *) store) = atof(card_val);
	break;
      case 'd' :
	*((double *) store) = atof(card_val);
	break;
      case 's' :
	sprintf((char *) store,"%s",card_val);
	break;
      case 'c' :
	*((char *) store) = card_val[0];
	break;
      default :
	fprintf(stderr,"WARNING: ReadCard: unknown type: %c\n", type);
	return -2;
      }

      return 0; // found card name
    }
  }

  fprintf(stderr,"WARNING: ReadCard: card: %s not found\n", card_flag);
  return -1;
}
