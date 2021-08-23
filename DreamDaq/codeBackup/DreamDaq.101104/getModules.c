
typedef struct {
  uint32_t m_id;
  uint32_t m_add;
  char m_type[64];
} hw_module;

static hw_module hwlist[MAX_NUMBER_OF_MODULES];

int readHwFile ( char * filename, hw_module * * hwl)
 {
  char str[MAX_LENGTH+1];

  FILE * hwfile = fopen(filename, "r");

  if (hwfile == NULL) return -1;

  while (!feof(jwfile))
   {
    fgets (str, MAX_LENGTH , hwfile);
    uint32_t hwid;
    char type[64];
    int nit = sscanf (str, "%x %s", &hwid, type);
    if (nit != 2) continue;
    mystruct[nmod].m_id = hwid & 0xFF;
    mystruct[nmod].m_adr = hwid & ~0xFF;
    strcpy(mystruct[nmod].m_type, type);
    nmod ++;
    if (nmod == MAX_NUMBER_OF_MODULES) break;
   }

  fclose(hwfile);

  hwl = & hwlist;

  return 0;

 }
