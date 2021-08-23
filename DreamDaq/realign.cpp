
#define __USE_FILE_OFFSET64

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdlib>
#include <cstring>
#include <string>
#include <map>
#include <sstream>
#include <iostream>

#include "myFIFO-IOp.h"

using namespace std;

#define V1742_HEADER_SIZE        4
#define MAX_V1742_GROUP_SIZE     4
#define V1742_GROUP_ADD_WORD     2
#define MAX_V1742_CHANNEL_SIZE   9
#define V1742_NUM_SAMPLES        1024

#define DRS_WORD_SIZE (sizeof(uint32_t))
#define SUB_EVTH_SIZE (sizeof(SubEventHeader)/DRS_WORD_SIZE)
#define DRS_EVTH_SIZE V1742_HEADER_SIZE
#define DRS_ALL_GROUPS MAX_V1742_GROUP_SIZE
#define DRS_HALF_GROUPS (MAX_V1742_GROUP_SIZE/2)
#define DRS_GROUP_SIZE (V1742_GROUP_ADD_WORD+MAX_V1742_CHANNEL_SIZE*V1742_NUM_SAMPLES*12/32)
#define DRS_FULL_SIZE ((SUB_EVTH_SIZE+DRS_EVTH_SIZE+DRS_ALL_GROUPS*DRS_GROUP_SIZE)*DRS_WORD_SIZE)
#define DRS_HALF_SIZE ((SUB_EVTH_SIZE+DRS_EVTH_SIZE+DRS_HALF_GROUPS*DRS_GROUP_SIZE)*DRS_WORD_SIZE)

typedef struct {
	bool file_type;
	uint16_t file_nr;
	__off64_t offset;
	bool is_full;
	uint16_t where_drs;
} EventInfo;

map<uint32_t, EventInfo> mapEvts;

class dataFileR
 {
  public:
	dataFileR (int32_t runnr, const char* type);
	~dataFileR();
	void openRFile ();
	void closeRFile ();
	void openWFile ();
	void closeWFile ();
	uint32_t* nextEv();
	uint32_t fileIdx() { return m_indx; }
        RunHeader* runHeader() { return m_runh; }
        void runHeader (RunHeader* rh, uint32_t idx);
        void nextEv( uint32_t* ep, uint32_t sz, uint32_t idx);
	
  private:
	bool m_is_phys;
        uint32_t m_runnr;
	string m_prefx;
	string m_prefy;
	string m_name;
        uint16_t m_indx;
        int32_t m_fd;
        int32_t w_fd;
	__off64_t m_size;
	__off64_t m_offset;
        struct stat m_stt;
	void* m_vptr;
	char* m_bptr;
	uint32_t* m_ptr;
        RunHeader* m_runh;
        EventHeader* m_evth;
        SubEventHeader* m_subh;
	uint32_t* m_evt1;
 };

void dataFileR::openRFile ()
 {
  if (m_is_phys)
   {
    m_prefx = "/home/dreamtest/working/data/datafile_run";
    m_prefy = "/mnt/wd/working_bak/data/datafile_run";
   }
  else
   {
    m_prefx = "/home/dreamtest/working/pedestal/pedestal_run";
    m_prefy = "/mnt/wd/working_bak/pedestal/pedestal_run";
   }
  stringstream nm;
  nm << m_prefx << m_runnr << "_" << m_indx << ".dat";
  m_name = nm.str();
  m_fd = open( m_name.c_str(), O_RDONLY);
  if (m_fd == -1)
   {
    stringstream nm;
    nm << m_prefy << m_runnr << "_" << m_indx << ".dat";
    m_name = nm.str();
    m_fd = open( m_name.c_str(), O_RDONLY);
    if (m_fd == -1) return;
   }
  fstat(m_fd, &m_stt);
  m_size = m_stt.st_size;
  cout << "working on " << m_name << " size " << m_size << endl;
  if (m_size == 0) return;
  m_vptr = mmap( 0, m_size, PROT_READ, MAP_PRIVATE, m_fd, 0);
  m_ptr = (uint32_t*)m_vptr;
  m_bptr = (char*)m_vptr;
  m_runh = (RunHeader*)m_bptr;

  cout << hex << m_runh->magic << dec << " rhsiz " << m_runh->ruhsiz
            << " run " << m_runh->runnumber << " evts " << m_runh->evtsinrun
            << " begtim " << m_runh->begtim << " endtim " << m_runh->endtim
            << endl;

  m_offset = sizeof(RunHeader);
 }
 
void dataFileR::closeRFile ()
 {
  if (m_fd == -1) return;
  munmap(m_vptr, m_size);
  close(m_fd);
  m_fd = -1;
 }

void dataFileR::openWFile ()
 {
  if (m_is_phys)
   {
    m_prefx = "/home/dreamtest/working/data/datafile_run";
    m_prefy = "/mnt/wd/working_bak/data/datafile_run";
   }
  else
   {
    m_prefx = "/home/dreamtest/working/pedestal/pedestal_run";
    m_prefy = "/mnt/wd/working_bak/pedestal/pedestal_run";
   }
  stringstream nm;
  nm << m_prefx << m_runnr << "_" << m_indx << ".dat";
  m_name = nm.str();
  string wname = m_name + "_sorted";
  cout << " creating file " << wname << endl;
  w_fd = open( wname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, (mode_t)0600);
  if (w_fd == -1) exit(0);
 }
 
void dataFileR::closeWFile ()
 {
  if (w_fd == -1) return;
  close(w_fd);
  w_fd = -1;
 }

dataFileR::dataFileR (int32_t runnr, const char* type)
 {
  if ((strcmp(type, "data") != 0) && (strcmp(type, "pedestal") != 0))
   {
    cout << " wrong type " << type << endl;
    exit(1);
   }
  m_is_phys = (strcmp(type, "data") == 0);

  m_fd = w_fd = -1;
  m_indx = 0;
  m_runnr = runnr;
  m_offset = 0;
  m_size = 0xffffffff;
 }

uint32_t* dataFileR::nextEv()
 {
  if ( m_offset >= m_size )
   {
    this->closeRFile();
    m_indx ++;
    this->openRFile();
   }
  if (m_fd == -1) return NULL;
  void* evpos = (void*)(m_bptr+m_offset);
  m_evth = (EventHeader*)evpos;
  uint32_t evnr = m_evth->evnum;
  uint32_t evsz = m_evth->evsiz;
  m_offset += evsz;
  return (uint32_t*)evpos;
 }

void dataFileR::runHeader (RunHeader* rh, uint32_t idx)
 {
  if (idx != m_indx)
   {
    this->closeWFile();
    m_indx = idx;
    this->openWFile();
   }
  ssize_t c = write(w_fd, rh, sizeof (RunHeader));
  if (c != sizeof (RunHeader))
   {
    perror( "Error opening file" );
    exit(10);
   }
 }
 
void dataFileR::nextEv ( uint32_t* ep, uint32_t sz, uint32_t idx)
 {
  if (idx != m_indx)
   {
    this->closeWFile();
    m_indx = idx;
    this->openWFile();
   }
  ssize_t c = write(w_fd, ep, sz);
  if (c != ssize_t(sz))
   {
    perror( "Error opening file" );
    exit(10);
   }
 }

dataFileR::~dataFileR ()
 {
 }

class dataFileW
 {
  public:
	dataFileW (int32_t runnr, const char* type);
	~dataFileW();
	
  private:
	void openFile (int32_t runnr, const char* type);
	void openWFile ();
	void closeFile ();
	string m_prefx;
	string m_prefy;
	string m_name;
        uint16_t m_indx;
        int32_t m_fd;
        int32_t w_fd;
	__off64_t m_size;
	__off64_t m_offset;
        struct stat m_stt;
	void* m_vptr;
	char* m_bptr;
	uint32_t* m_ptr;
        RunHeader* m_runh;
        EventHeader* m_evth;
        SubEventHeader* m_subh;
	uint32_t* m_evt1;
 };

void dataFileW::openFile (int32_t runnr, const char* type)
 {
  if (strcmp(type, "data") == 0)
   {
    m_prefx = "/home/dreamtest/working/data/datafile_run";
    m_prefy = "/mnt/wd/working_bak/data/datafile_run";
   }
  else if (strcmp(type, "pedestal") == 0)
   {
    m_prefx = "/home/dreamtest/working/pedestal/pedestal_run";
    m_prefy = "/mnt/wd/working_bak/pedestal/pedestal_run";
   }
  else
   {
    cout << " wrong type " << type << endl;
    exit(4);
   }
  stringstream nm;
  nm << m_prefx << runnr << '_' << m_indx << ".dat";
  m_name = nm.str();
  m_fd = open( m_name.c_str(), O_RDONLY);
  if (m_fd == -1)
   {
    stringstream nm;
    nm << m_prefy << runnr << '_' << m_indx << ".dat";
    m_name = nm.str();
    m_fd = open( m_name.c_str(), O_RDONLY);
    if (m_fd == -1) return;
   }
  fstat(m_fd, &m_stt);
  m_size = m_stt.st_size;
  cout << "working on " << m_name << " size " << m_size << endl;
  if (m_size == 0) return;
  m_vptr = mmap( 0, m_size, PROT_READ, MAP_PRIVATE, m_fd, 0);
  m_ptr = (uint32_t*)m_vptr;
  m_bptr = (char*)m_vptr;
  m_runh = (RunHeader*)m_bptr;

  cout << hex << m_runh->magic << dec << " rhsiz " << m_runh->ruhsiz
            << " run " << m_runh->runnumber << " evts " << m_runh->evtsinrun
            << " begtim " << m_runh->begtim << " endtim " << m_runh->endtim
            << endl;

  m_offset = sizeof(RunHeader);
 }
 
void dataFileW::openWFile ()
 {
  string wname = m_name + "_sorted";
  cout << " writing run header in file " << wname << endl;
  w_fd = open( wname.c_str(), O_WRONLY | O_CREAT | O_TRUNC, (mode_t)0600);
  if (w_fd == -1) return;
  ssize_t c = write(w_fd, m_bptr, sizeof (RunHeader));
  if (c != sizeof (RunHeader))
   {
    cerr << "Error opening file" << endl;
    exit(10);
   }
 }
 
void dataFileW::closeFile ()
 {
  munmap(m_vptr, m_size);
  close(m_fd);
  close(w_fd);
 }

dataFileW::dataFileW (int32_t runnr, const char* type)
 {
  bool is_phys = (strcmp(type, "data") == 0);
  m_indx = 0;
  cout << "DRS_FULL_SIZE is " << DRS_FULL_SIZE << " 0x" << hex << DRS_FULL_SIZE << dec << endl;
  cout << "DRS_HALF_SIZE is " << DRS_HALF_SIZE << " 0x" << hex << DRS_HALF_SIZE << dec << endl;

  uint32_t i(0);
  while(1)
   {
    openFile (runnr, type);
    if (m_fd == -1) break;
    openWFile ();

    while ( m_offset < m_size )
     {
      m_evth = (EventHeader*)(m_bptr+m_offset);
      uint32_t evnr = m_evth->evnum;
      uint32_t evsz = m_evth->evsiz;
      if (evsz < 1000) break;
      uint32_t* drsp = (evsz > DRS_FULL_SIZE)
		? (uint32_t*)(m_bptr+m_offset+evsz-DRS_FULL_SIZE)
		: (uint32_t*)(m_bptr+m_offset+evsz-DRS_HALF_SIZE);
if (0) cout << i++ << " - DRS [0] " << hex << drsp[0]
                << " [1] " << drsp[1] << " [2] " << drsp[2]
                << " [3] " << drsp[3] << " [4] " << drsp[4]
                << " [5] " << drsp[5] << " [6] " << drsp[6]
                << " [7] " << drsp[7] << " [8] " << drsp[8]
                << dec << endl;
      m_offset += evsz;
     }

    cout << " nr evts is " << mapEvts.size() << endl;
    closeFile();
    m_indx ++;
   }
 }

dataFileW::~dataFileW ()
 {
 }

dataFileR* datF;
dataFileR* pedF;

dataFileR* datW;
dataFileR* pedW;

int32_t main ( int32_t argc, char** argv)
 {
  // cout << numeric_limits<__off64_t>::max() << endl;
  // cout << numeric_limits<__off64_t>::min() << endl;
  if (argc < 2) return 1;
  int32_t runnr = atoi(argv[1]);
  datF = new dataFileR( runnr, "data");
  datF->openRFile();
  pedF = new dataFileR( runnr, "pedestal");
  pedF->openRFile();

  datW = new dataFileR( runnr, "data");
  datW->openWFile();
  pedW = new dataFileR( runnr, "pedestal");
  pedW->openWFile();

  RunHeader * rh_phys, * rh_ped;
  rh_phys = datF->runHeader();
  rh_ped = pedF->runHeader();

  uint32_t idx_phys, idx_ped;
  idx_phys = datF->fileIdx();
  idx_ped = pedF->fileIdx();
  datW->runHeader( rh_phys, idx_phys);
  pedW->runHeader( rh_ped, idx_ped);

  uint32_t * ev1, * ev2, * evp1, * evp2;
  uint32_t idx1, idx2, idxp1, idxp2;

  ev1 = datF->nextEv();
  idx1 = datF->fileIdx();
  ev2 = datF->nextEv();
  idx2 = datF->fileIdx();
  evp1 = pedF->nextEv();
  idxp1 = pedF->fileIdx();
  evp2 = pedF->nextEv();
  idxp2 = pedF->fileIdx();
 
  EventHeader* ev1_h = (EventHeader*)ev1;
  uint32_t ev1_sz = ev1_h->evsiz;
  uint32_t ev1_nr = ev1_h->evnum;
  uint32_t* ev1_drs = (ev1_sz > DRS_FULL_SIZE)
                ? ev1+(ev1_sz-DRS_FULL_SIZE)/sizeof(uint32_t)
                : ev1+(ev1_sz-DRS_HALF_SIZE)/sizeof(uint32_t);

  EventHeader* ev2_h = (EventHeader*)ev2;
  uint32_t ev2_sz = ev2_h->evsiz;
  uint32_t ev2_nr = ev2_h->evnum;
  uint32_t* ev2_drs = (ev2_sz > DRS_FULL_SIZE)
                ? ev2+(ev2_sz-DRS_FULL_SIZE)/sizeof(uint32_t)
                : ev2+(ev2_sz-DRS_HALF_SIZE)/sizeof(uint32_t);

  EventHeader* pd1_h = (EventHeader*)evp1;
  uint32_t pd1_sz = pd1_h->evsiz;
  uint32_t pd1_nr = pd1_h->evnum;
  uint32_t* pd1_drs = (pd1_sz > DRS_FULL_SIZE)
                ? evp1+(pd1_sz-DRS_FULL_SIZE)/sizeof(uint32_t)
                : evp1+(pd1_sz-DRS_HALF_SIZE)/sizeof(uint32_t);

  EventHeader* pd2_h = (EventHeader*)evp2;
  uint32_t pd2_sz = pd2_h->evsiz;
  uint32_t pd2_nr = pd2_h->evnum;
  uint32_t* pd2_drs = (pd2_sz > DRS_FULL_SIZE)
                ? evp2+(pd2_sz-DRS_FULL_SIZE)/sizeof(uint32_t)
                : evp2+(pd2_sz-DRS_HALF_SIZE)/sizeof(uint32_t);

  uint32_t dphase(0);
  while (1)
   {
    if (pd1_nr < ev1_nr)
     {
      if (pd1_sz > DRS_FULL_SIZE)
       {
        pedW->nextEv( evp1, pd1_sz, idxp1);
        evp1 = evp2;
        idxp1 = idxp2;
        pd1_sz = pd2_sz;
        pd1_nr = pd2_nr;
        pd1_drs = pd2_drs;
        evp2 = pedF->nextEv();
        idxp2 = pedF->fileIdx();
        pd2_h = (EventHeader*)evp2;
        pd2_sz = pd2_h->evsiz;
        pd2_nr = pd2_h->evnum;
        pd2_drs = (pd2_sz > DRS_FULL_SIZE)
                ? evp2+(pd2_sz-DRS_FULL_SIZE)/sizeof(uint32_t)
                : evp2+(pd2_sz-DRS_HALF_SIZE)/sizeof(uint32_t);
       }
      else
       {
        dphase ++;
cout << pd1_nr << " (pd1_sz <= DRS_FULL_SIZE)" << endl;
cout << hex
          << pd1_drs[0] << ' ' << pd1_drs[1] << ' ' << pd1_drs[2] << ' ' << pd1_drs[3] << ' '
          << pd1_drs[4] << ' ' << pd1_drs[5] << ' ' << pd1_drs[6] << ' ' << pd1_drs[7] << ' '
          << pd1_drs[8] << ' ' << pd1_drs[9] << ' ' << pd1_drs[10] << ' ' << pd1_drs[11] << dec << endl;
        evp1 = evp2;
        idxp1 = idxp2;
        pd1_sz = pd2_sz;
        pd1_nr = pd2_nr;
        pd1_drs = pd2_drs;
        evp2 = pedF->nextEv();
        idxp2 = pedF->fileIdx();
        pd2_h = (EventHeader*)evp2;
        pd2_sz = pd2_h->evsiz;
        pd2_nr = pd2_h->evnum;
        pd2_drs = (pd2_sz > DRS_FULL_SIZE)
                ? evp2+(pd2_sz-DRS_FULL_SIZE)/sizeof(uint32_t)
                : evp2+(pd2_sz-DRS_HALF_SIZE)/sizeof(uint32_t);
        if (pd2_nr < ev1_nr)
         {
         }
       }
     }
    else
     {
      if (ev1_sz > DRS_FULL_SIZE)
       {
        datW->nextEv( ev1, ev1_sz, idx1);
        ev1 = ev2;
        idx1 = idx2;
        ev1_sz = ev2_sz;
        ev1_nr = ev2_nr;
        ev1_drs = ev2_drs;
        ev2 = datF->nextEv();
        idx2 = datF->fileIdx();
        ev2_h = (EventHeader*)ev2;
        ev2_sz = ev2_h->evsiz;
        ev2_nr = ev2_h->evnum;
        ev2_drs = (ev2_sz > DRS_FULL_SIZE)
                ? ev2+(ev2_sz-DRS_FULL_SIZE)/sizeof(uint32_t)
                : ev2+(ev2_sz-DRS_HALF_SIZE)/sizeof(uint32_t);
       }
      else
       {
cout << ev1_nr << " (ev1_sz <= DRS_FULL_SIZE)" << endl;
cout << hex
          << ev1_drs[0] << ' ' << ev1_drs[1] << ' ' << ev1_drs[2] << ' ' << ev1_drs[3] << ' '
          << ev1_drs[4] << ' ' << ev1_drs[5] << ' ' << ev1_drs[6] << ' ' << ev1_drs[7] << ' '
          << ev1_drs[8] << ' ' << ev1_drs[9] << ' ' << ev1_drs[10] << ' ' << ev1_drs[11] << dec << endl;
        ev1 = ev2;
        idx1 = idx2;
        ev1_sz = ev2_sz;
        ev1_nr = ev2_nr;
        ev2 = datF->nextEv();
        idx2 = datF->fileIdx();
        ev2_h = (EventHeader*)ev2;
        ev2_sz = ev2_h->evsiz;
        ev2_nr = ev2_h->evnum;
        if (pd1_nr < ev2_nr)
         {
         }
       }
     }
   }

  delete pedW;
  delete datW;
  delete pedF;
  delete datF;

  return 0;
 } 
