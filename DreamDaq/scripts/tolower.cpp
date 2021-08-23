#include <ctype.h>
#include <cstdlib>
#include <cstring>
#include <iostream>

char* tolower(char* p)
 {
  char* t=p;
  size_t l=strlen(p);
  for (; l; l--, p++) *p=tolower(*p);
  return t;
 }

int32_t main(int32_t argc, char** argv)
 {
  argv ++;
  argc --;
  for (; argc; argc--)
   {
    const char* p = *argv++;
    size_t l = strlen(p);
    char* c = (char*)malloc(l);
    strcpy(c,p);
    std::cout << tolower(c);
    free(c);
    if (argc > 1) std::cout << " ";
   }
  std::cout << std::endl;
  return 0;
 }
