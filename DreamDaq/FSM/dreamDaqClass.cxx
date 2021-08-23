
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "dreamDaqClass.h"

inline void nnsleep( uint32_t ndelay )
 {
  struct timespec tsleep = { 0, ndelay };
  nanosleep(&tsleep, NULL);
 }

dreamDaq::dreamDaq( int32_t argc, char** argv ) :
  m_isrunning(false), m_exit(false),
  m_runnr(1), m_maxphy(0), m_maxped(0), m_phy2ped(0),
  m_t1t2(true), m_drsrdo(false), m_oscrdo(false),
  m_state(TRIG_LOCKED), m_trigmask(0)
 {
  std::ifstream ddrc( "/home/dreamtest/.dreamdaqrc", std::ifstream::in );
  std::string a;
  std::string s_drsro, s_oscro, s_t1t2, s_t1t2t3;
  while ( ddrc.good() )
   {
    a = "";
    ddrc >> a;
    if (a == "RunNumber=") ddrc >> m_runnr;
    else if (a == "NrOfPhysicsEvents=") ddrc >> m_maxphy;
    else if (a == "PhysToPedRatio=") ddrc >> m_phy2ped;
    else if (a == "DRSReadout=") ddrc >> s_drsro;
    else if (a == "ScopeReadout=") ddrc >> s_oscro;
    else if (a == "T1*T2=") ddrc >> s_t1t2;
    else if (a == "T1*T2*T3=") ddrc >> s_t1t2t3;
   }
  bool ist1t2 = (s_t1t2 == "ON");
  bool ist1t2t3 = (s_t1t2t3 == "ON");
  m_t1t2 = ist1t2;
  m_drsrdo = (s_drsro == "ON");
  m_oscrdo = (s_oscro == "ON");
  std::cout << " RunNumber = " << m_runnr << "\n";
  std::cout << " PhysicsEvents = " << m_maxphy << "\n";
  std::cout << " Phy2Ped = " << m_phy2ped << "\n";
  std::cout << " DRS = " << (m_drsrdo ? "ON" : "OFF") << "\n";
  std::cout << " Scope = " << (m_oscrdo ? "ON" : "OFF") << "\n";
  std::cout << " T1*T2 = " << (ist1t2 ? "ON" : "OFF") << "\n";
  std::cout << " T1*T2*T3 = " << (ist1t2t3 ? "ON" : "OFF") << "\n";

  std::fstream filestr ("DreamDaqConf.txt", std::fstream::in);
  char* line((char*)malloc(256));
  while (!filestr.eof())
   {
    filestr.getline (line,256);
    char vtype[256], t[256];
    uint32_t v;
    int32_t rc = sscanf(line,"%s %x %s", vtype, &v, t);
    if (rc == 3) std::cout << "[" << vtype << "\t\t" << std::hex << v << std::dec << "\t\t" << t << "]\n";
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
    else if (m_drsrdo && (strcmp(vtype, "v1742") == 0))
     {
      v1742* m_vme = new v1742(v, t);
      m_v1742.push_back(m_vme);
     }
    else if (strcmp(vtype, "v2718") == 0)
     {
      v2718* m_vme = new v2718(v, t);
      m_v2718.push_back(m_vme);
     }
/*
    else if (m_oscrdo && (strcmp(vtype, "tekosc") == 0))
     {
      tekOsc* m_vme = new tekOsc(v, 0, 1, 0);
      m_tekOsc.push_back(m_vme);
     }
*/
   }
  filestr.close();
  m_io = m_v262[0];
 }

dreamDaq::~dreamDaq()
 {
std::cout << " qui siamo \n";
  for (size_t k=0; k<m_v258.size(); k++) delete m_v258[k];
  m_v258.clear();
  for (size_t k=0; k<m_v814.size(); k++) delete m_v814[k];
  m_v814.clear();
  for (size_t k=0; k<m_v262.size(); k++) delete m_v262[k];
  m_v262.clear();
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
/*
  for (size_t k=0; k<m_tekOsc.size(); k++) delete m_tekOsc[k];
  m_tekOsc.clear();
*/
 }

int32_t dreamDaq::initDaq()
 {
  for (size_t k=0; k<m_v258.size(); k++) m_v258[k]->init();
  for (size_t k=0; k<m_v814.size(); k++) m_v814[k]->init();
  for (size_t k=0; k<m_v262.size(); k++) m_v262[k]->init();
  for (size_t k=0; k<m_v775.size(); k++) m_v775[k]->init();
  for (size_t k=0; k<m_v792ac.size(); k++) m_v792ac[k]->init();
  for (size_t k=0; k<m_v862ac.size(); k++) m_v862ac[k]->init();
  for (size_t k=0; k<m_v1742.size(); k++) m_v1742[k]->init();
  for (size_t k=0; k<m_v2718.size(); k++) m_v2718[k]->init();
/*
  for (size_t k=0; k<m_tekOsc.size(); k++) m_tekOsc[k]->init();
*/
  return 0;
 }

int32_t dreamDaq::stopDaq()
 {
  return 0;
 }
int32_t dreamDaq::readEvent()
 {
  return 0;
 }
int32_t dreamDaq::readOscEvent()
 {
  if ( oscFull() ) oscfflush();
  return 0;
 }
int32_t dreamDaq::oscfflush()
 {
  return 0;
 }

int32_t dreamDaq::handleEvent()
 {
  return 0;
 }

void dreamDaq::oscEvLoop()
 {
  do
   {
    while ( !inSpill() && !exitNow() && !hasTrigger() ) { nnsleep(1000000); }
    acquireBuffers();
    do
      if ( hasTrigger() )
       {
        readOscEvent();
        unlockTrigger();
       }
    while ( inSpill() && !exitNow() );
    if ( hasTrigger() )
     {
      readOscEvent();
      unlockTrigger();
     }
    oscfflush();
    releaseBuffers();
   }
  while( !exitNow() );
 }

void dreamDaq::vmeEvLoop()
 {
  do
   {
    while ( !inSpill() && !exitNow() && !hasTrigger() ) { nnsleep(1000000); }
    acquireBuffers();
    do
      if ( hasTrigger() )
       {
        readEvent();
        unlockTrigger();
       }
    while ( inSpill() && !exitNow() );
    if ( hasTrigger() )
     {
      readEvent();
      unlockTrigger();
     }
    releaseBuffers();
   }
  while( !exitNow() );
 }

void dreamDaq::Run()
 {
  initDaq();

  printf("Loop start\n");

  enableDaq();

  m_oscrdo ? oscEvLoop() : vmeEvLoop();

  printf("Loop done\n");

  stopDaq();
 }

