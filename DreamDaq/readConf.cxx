
#define LINUX

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "myV258.h"
#include "myV814.h"
#include "myV262.h"
#include "myV488.h"
#include "myV775.h"
#include "myV792AC.h"
#include "myV862AC.h"
#include "myV1742.h"
#include "myV2718.h"

using namespace std;

vector <v258*> m_v258;              // discriminator
vector <v814*> m_v814;              // discriminator
vector <v262*> m_v262;              // I/O
vector <v488*> m_v488;              // TDC
vector <v775*> m_v775;              // TDC
vector <v792ac*> m_v792ac;          // QDC
vector <v862ac*> m_v862ac;          // QDC
vector <v1742*> m_v1742;            // DRS4
vector <v2718*> m_v2718;    // optical link bridge

int32_t main( int32_t argc, char** argv )
 {

  fstream filestr ("DreamDaqConf.txt", fstream::in);
  char* line((char*)malloc(256));
  while (!filestr.eof())
   {
    filestr.getline (line,256);
    char vtype[256], t[256];
    uint32_t v;
    int32_t rc = sscanf(line,"%s %x %s", vtype, &v, t);
    if (rc == 3) cout << "[" << vtype << "\t\t" << hex << v << dec << "\t\t" << t << "]" << endl;
    if (strcmp(vtype, "v258") == 0)
     {
      v258* m_vme = new v258(v, t);
      m_v258.push_back(m_vme);
     }
    else if (strcmp(vtype, "v814") == 0)
     {
      v814* m_vme = new v814(v, t);
      m_v814.push_back(m_vme);
     }
    else if (strcmp(vtype, "v262") == 0)
     {
      v262* m_vme = new v262(v, t);
      m_v262.push_back(m_vme);
     }
    else if (strcmp(vtype, "v488") == 0)
     {
      v488* m_vme = new v488(v, t);
      m_v488.push_back(m_vme);
     }
    else if (strcmp(vtype, "v775") == 0)
     {
      v775* m_vme = new v775(v, t);
      m_v775.push_back(m_vme);
     }
    else if (strcmp(vtype, "v792ac") == 0)
     {
      v792ac* m_vme = new v792ac(v, t);
      m_v792ac.push_back(m_vme);
     }
    else if (strcmp(vtype, "v862ac") == 0)
     {
      v862ac* m_vme = new v862ac(v, t);
      m_v862ac.push_back(m_vme);
     }
    else if (strcmp(vtype, "v1742") == 0)
     {
      v1742* m_vme = new v1742(v, t);
      m_v1742.push_back(m_vme);
     }
    else if (strcmp(vtype, "v2718") == 0)
     {
      v2718* m_vme = new v2718i(v, t);
      m_v2718.push_back(m_vme);
     }
   }
  filestr.close();

  for (size_t k=0; k<m_v258.size(); k++) delete m_v258[k];
  m_v258.clear();
  for (size_t k=0; k<m_v814.size(); k++) delete m_v814[k];
  m_v814.clear();
  for (size_t k=0; k<m_v262.size(); k++) delete m_v262[k];
  m_v262.clear();
  for (size_t k=0; k<m_v488.size(); k++) delete m_v488[k];
  m_v488.clear();
  for (size_t k=0; k<m_v775.size(); k++) delete m_v775[k];
  m_v775.clear();
  for (size_t k=0; k<m_v792ac.size(); k++) delete m_v792ac[k];
  m_v792ac.clear();
  for (size_t k=0; k<m_v862ac.size(); k++) delete m_v862ac[k];
  m_v862ac.clear();
  for (size_t k=0; k<m_v1742.size(); k++) delete m_v1742[k];
  m_v1742.clear();
  for (size_t k=0; k<m_v2718.size(); k++) delete m_v2718[k];
  m_v2718.clear();

  return 0;
 }
