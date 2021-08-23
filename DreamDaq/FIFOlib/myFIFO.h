#include <sys/types.h>
#include <stdint.h>
#include <string>

#define FIFOKEY (0x7)
#define DATAKEY (0x8)
#define SEMKEY  (0x9)

// #define PHYS_SHM_SIZE (0x30000000>>1)
// #define PED_SHM_SIZE (0x06000000>>1)
#define PHYS_SHM_SIZE (0x20000000>>1)
#define PED_SHM_SIZE (0x04000000>>1)
#define PED_BASE_KEY (0x1900)
#define PHYS_BASE_KEY (0x1970)

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
    int32_t FIFOshmid;
    int32_t datashmid;
    int32_t semid;
    uint32_t size;
    uint32_t* startSM[MAXSM];
    uint32_t* startRD;
    uint32_t* startWR;
    uint32_t offsetSM[MAXSM];
    uint32_t offsetRD;
    uint32_t offsetWR;
    uint32_t oldoffsetWR;
    uint32_t spillnr;
    uint32_t numevt;
    uint32_t nsampler;
    uint32_t sam_locker;
    spaceStatus space;
    spaceStatus monspace[MAXSM];
    lockStatus status;
    userStatus reader;
    userStatus writer;
    userStatus sampler[MAXSM];
    initStatus initialized;
  };
  
  struct myFIFOData* internalData;
  struct myFIFOData* backupData;
  
  userType type;
  bool valid;
  
protected:
  struct myFIFOData* getData(){return internalData;};
  uint32_t sam_ind;
  uint32_t m_size;

public:
  enum result {SUCCESS=1,FIFOFULL,FIFOEMPTY,RDLOCKED,WRLOCKED,FAILED,PARTIALREAD};

  myFIFO(key_t base_key, userType aType);
  ~myFIFO();

  inline bool isvalid(){return valid;}
  
  result status();

  bool isPresent(userType aType);

  void waitFor(userType aType);
  
  uint32_t getSpillNr(){return internalData->spillnr;}
  
  void updateSpillNr(){internalData->spillnr++;}

  uint32_t getNevt(){return internalData->numevt;}
  
  void updateNevt(uint32_t nevt){internalData->numevt = nevt;}

  result unlock(userType aType);
  
  result waitlock(userType aType);
  //result trylock(userType aType);

  void backup();
  void restore();

};
