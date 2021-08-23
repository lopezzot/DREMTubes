#include "textcolor.h"

void textcol(int fg) {
  printf("%c[%dm", 0x1B, fg + 30);
}

void ftextcol(FILE *ptr, int fg) {
  fprintf(ptr, "%c[%dm", 0x1B, fg + 30);
}

void textcolreset() {
  printf("%c[0;;m", 0x1B);
}

void ftextcolreset(FILE *ptr) {
  fprintf(ptr, "%c[0;;m", 0x1B);
}

