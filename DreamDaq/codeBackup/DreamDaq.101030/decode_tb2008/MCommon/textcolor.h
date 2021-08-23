#ifndef TEXTCOLORH_SEEN
#define TEXTCOLORH_SEEN

#include <stdio.h>

#define COL_RESET	0
#define COL_BRIGHT 	1
#define COL_DIM		2
#define COL_UNDERLINE 	3
#define COL_BLINK	4
#define COL_REVERSE	7
#define COL_HIDDEN	8

#define BLACK 		0
#define RED		1
#define GREEN		2
#define YELLOW		3
#define BLUE		4
#define MAGENTA		5
#define CYAN		6
#define	WHITE		7
#define	NONE		9

#ifdef __cplusplus
extern "C" {
#endif

void textcol(int fg);
void ftextcol(FILE *ptr, int fg);

void textcolreset();
void ftextcolreset(FILE *ptr);

#ifdef __cplusplus
}
#endif


#endif
