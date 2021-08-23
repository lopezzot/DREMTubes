//$Id: myFIFO.h,v 1.9 2009/07/21 09:14:45 dreamdaq Exp $
#include <sys/types.h>
#include <string>

#define FIFOKEY (0x7)
#define DATAKEY (0x8)
#define SEMKEY  (0x9)

//#define SIZE (8388608)
//#define SIZE 131072
// #define SIZE (0x800000)
#define PHYS_SHM_SIZE (0x30000000>>2)
#define PED_SHM_SIZE (0x06000000>>2)
#define PED_BASE_KEY (0x1900)
#define PHYS_BASE_KEY (0x1970)
//#define SIZE (0x400000)

#define MAXSM (16)

using namespace std;

class myFIFO{

public:
  enum userStatus {MISSING=2,PRESENT};

protected:
  enum lockStatus {LOCKEDFORWR=2,
		   LOCKEDFORRD,LOCKEDFORSM,UNLOCKED};

  enum userType {READER=2,WRITER,SAMPLER};

  enum spaceStatus {EMPTY=2,FREE,FULL};

private:
  enum initStatus {UNINIT=2,INIT};
  
  struct myFIFOData{
    int FIFOshmid;
    int datashmid;
    int semid;
    unsigned int size;
    unsigned int *startSM[MAXSM];
    unsigned int *startRD;
    unsigned int *startWR;
    unsigned int offsetSM[MAXSM];
    unsigned int offsetRD;
    unsigned int offsetWR;
    unsigned int oldoffsetWR;
    unsigned int spillnr;
    unsigned int nsampler;
    unsigned int sam_locker;
    spaceStatus space;
    spaceStatus monspace[MAXSM];
    lockStatus status;
    userStatus reader;
    userStatus writer;
    userStatus sampler[MAXSM];
    initStatus initialized;
  };
  
  struct myFIFOData *internalData;
  struct myFIFOData *backupData;
  
  userType type;
  bool valid;
  
protected:
  struct myFIFOData * getData(){return internalData;};
  unsigned int sam_ind;
  unsigned int m_size;

public:
  enum result {SUCCESS=1,FIFOFULL,FIFOEMPTY,RDLOCKED,WRLOCKED,FAILED,PARTIALREAD};

  myFIFO(key_t base_key, userType aType);
  ~myFIFO();

  inline bool isvalid(){return valid;}
  
  result status();

  bool isPresent(userType aType);

  void waitFor(userType aType);
  
  unsigned int getSpillNr(){return internalData->spillnr;}; 
  
  void updateSpillNr(){internalData->spillnr++;};

  result unlock(userType aType);
  
  result waitlock(userType aType);
  //result trylock(userType aType);

  void backup();
  void restore();

};
