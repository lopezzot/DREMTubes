#include <fcntl.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <iostream>

#include "myFIFO.h"

using namespace std;

myFIFO::myFIFO(key_t base_key, myFIFO::userType aType){

  struct sembuf ops[3];

  m_size = 0;

  backupData=NULL;
  valid=true;
  sam_ind=0;

  type=aType;
  
  //first of all get the semaphore
  int32_t my_semid = semget(SEMKEY+base_key,2,IPC_CREAT|S_IRUSR|S_IWUSR);
  
  if(my_semid==-1){
    cout << "Cannot get semaphores" << endl;
    valid=false;
    return;
  }      
  
  //wait for eventual initializiation of the other process
  ops[0].sem_num=0;
  ops[0].sem_op=0;
  ops[0].sem_flg=0;
  
  ops[1].sem_num=0;
  ops[1].sem_op=1;
  ops[1].sem_flg=0;
  
  ops[2].sem_num=0;
  ops[2].sem_op=-1;
  ops[2].sem_flg=0;

  semop(my_semid,ops,2);

  //Get the data shm
  int32_t dataShmId = shmget(DATAKEY+base_key,
			 sizeof(struct myFIFOData),
			 IPC_CREAT|S_IRUSR|S_IWUSR);
  
  if(dataShmId==-1){
    cout << "Cannot get data shm" << endl;
    valid=false;
    semop(my_semid,&ops[2],1);
    return;
  }      
  
  //attach the data shm
  internalData=(struct myFIFOData *)shmat(dataShmId,NULL,0);

  if(internalData==(struct myFIFOData *)-1){
    cout << "Cannot attach data shm" << endl;
    valid=false;
    semop(my_semid,&ops[2],1);
    return;
  }      
  
  //check if the data shm is already initialized
  if((internalData->initialized)!=myFIFO::INIT){
    //if not create FIFO shm and set init values
  
    internalData->datashmid=dataShmId;
    internalData->semid=my_semid;
    if (base_key == PED_BASE_KEY) m_size = PED_SHM_SIZE;
    if (base_key == PHYS_BASE_KEY) m_size = PHYS_SHM_SIZE;
cout << "internalData->datashmid " << hex << internalData->datashmid <<
" internalData->semid " << internalData->semid;
cout << "\n -- base_key PHYS_BASE_KEY PED_BASE_KEY " << base_key << " " << PHYS_BASE_KEY << " " << PED_BASE_KEY;
cout << "\n -- m_size " << m_size << dec << endl;
    internalData->size=m_size;
    internalData->space=EMPTY;
    
    internalData->offsetRD=0;
    internalData->offsetWR=0;
    internalData->oldoffsetWR=0;
    
    internalData->nsampler=0;
    internalData->spillnr=0;
    for(register uint32_t i=0;i<MAXSM;i++){
      internalData->offsetSM[i]=0;
      internalData->monspace[i]=EMPTY;
    }
    
    internalData->sam_locker=MAXSM+1;

    internalData->status=myFIFO::UNLOCKED;

    internalData->FIFOshmid=shmget(FIFOKEY+base_key,
				   m_size*sizeof(uint32_t),
				   IPC_CREAT|S_IRUSR|S_IWUSR);
    
    if(internalData->FIFOshmid==-1){
      cout << "Cannot get FIFO shm" << endl;
      valid=false;
      semop(my_semid,&ops[2],1);
      return;
    }      

    internalData->initialized=myFIFO::INIT;

  }
  
    if(type==myFIFO::READER){
    if(internalData->reader==myFIFO::PRESENT){
      cout << "A reader is already present" << endl;
      valid=false;
      semop(my_semid,&ops[2],1);
      return;
    }
  }else if(type==myFIFO::WRITER){
    if(internalData->writer==myFIFO::PRESENT){
      cout << "A writer is already present" << endl;
      valid=false;
      semop(my_semid,&ops[2],1);
      return;
    }
  }else if(type==myFIFO::SAMPLER){
    if(internalData->nsampler==MAXSM){
      cout << MAXSM <<" samplers are already present" << endl;
      valid=false;
      semop(my_semid,&ops[2],1);
      return;
    }
  }

  if(type==myFIFO::READER){
    internalData->startRD=(uint32_t*)shmat(internalData->FIFOshmid,
						NULL,0);

    cout << "READER Buffer start " << internalData->startRD << " end " << (internalData->startRD + m_size) << endl;

    if(internalData->startRD==(uint32_t*)-1){
      cout << "Cannot attach FIFO shm" << endl;
      valid=false;
      semop(my_semid,&ops[2],1);
      return;
    }      
  }else if(type==myFIFO::WRITER){
    internalData->startWR=(uint32_t*)shmat(internalData->FIFOshmid,
					       NULL,0);
    
    cout << "WRITER Buffer start " << internalData->startWR << " end " << (internalData->startWR + m_size) << endl;

    if(internalData->startWR==(uint32_t*)-1){
      cout << "Cannot attach FIFO shm" << endl;
      valid=false;
      semop(my_semid,&ops[2],1);
      return;
    }      
  }else if(type==myFIFO::SAMPLER){
    internalData->startSM[internalData->nsampler]=
      (uint32_t*)shmat(internalData->FIFOshmid,NULL,0);
    
    cout << "SAMPLER Buffer start " << internalData->startSM[internalData->nsampler]
         << " end " << (internalData->startSM[internalData->nsampler] + m_size) << endl;

    if(internalData->startSM[internalData->nsampler]==(uint32_t*)-1){
      cout << "Cannot attach FIFO shm" << endl;
      valid=false;
      semop(my_semid,&ops[2],1);
      return;
    }
  }
    

  if(type==myFIFO::READER){
    internalData->reader=myFIFO::PRESENT;
  }else if(type==myFIFO::WRITER){
    internalData->writer=myFIFO::PRESENT;
  }else if(type==myFIFO::SAMPLER){
    internalData->sampler[internalData->nsampler]=myFIFO::PRESENT;
    sam_ind=internalData->nsampler;
    internalData->nsampler++;
  }

  //release initialization sem lock
  semop(my_semid,&ops[2],1);
}


myFIFO::~myFIFO(){
  if (backupData) free(backupData);
  int32_t mysemid=internalData->semid;
  int32_t dataid=internalData->datashmid;
  struct shmid_ds shmdata;

  if(type==myFIFO::READER){
    internalData->reader=myFIFO::MISSING;
    shmdt(internalData->startRD);
  }else if(type==myFIFO::WRITER){
    internalData->writer=myFIFO::MISSING;
    shmdt(internalData->startWR);
  }else if(type==myFIFO::SAMPLER){
    internalData->sampler[sam_ind]=myFIFO::MISSING;
    shmdt(internalData->startSM[sam_ind]);
    internalData->nsampler--;
  }
  
  shmctl(internalData->FIFOshmid,IPC_STAT,&shmdata);
  
  if(shmdata.shm_nattch==0)
    shmctl(internalData->FIFOshmid,IPC_RMID,NULL);

  shmdt(internalData);
  
  shmctl(dataid,IPC_STAT,&shmdata);
  
  if(shmdata.shm_nattch==0){
    shmctl(dataid,IPC_RMID,NULL);
    semctl(mysemid,0,IPC_RMID,NULL);
  }
  
}

myFIFO::result myFIFO::waitlock(myFIFO::userType aType){
  
  myFIFO::result res=myFIFO::FAILED;
  struct sembuf ops[3];
  
  ops[0].sem_num=1;
  ops[0].sem_op=0;
  ops[0].sem_flg=0;
  
  ops[1].sem_num=1;
  ops[1].sem_op=1;
  ops[1].sem_flg=0;
  
  ops[2].sem_num=1;
  ops[2].sem_op=-1;
  ops[2].sem_flg=0;

    
  //lock the variable before read
  semop(internalData->semid,ops,2);
  

  if(aType==myFIFO::SAMPLER){
    if(internalData->status==myFIFO::LOCKEDFORSM
       && internalData->sam_locker==sam_ind){
      res = myFIFO::SUCCESS;
    }else if(internalData->status==myFIFO::UNLOCKED){
      internalData->status=myFIFO::LOCKEDFORSM;
      internalData->sam_locker=sam_ind;
      res = myFIFO::SUCCESS;
    }else{
      while(1){
	semop(internalData->semid,&ops[2],1);
	usleep(10);
	semop(internalData->semid,ops,2);
	if(internalData->status==myFIFO::UNLOCKED){
	  internalData->status=myFIFO::LOCKEDFORSM;
	  internalData->sam_locker=sam_ind;
	  res = myFIFO::SUCCESS;
	  break;
	}
      }
    }
  }else if(aType==myFIFO::READER){
    if(internalData->status==myFIFO::LOCKEDFORRD)
      res = myFIFO::SUCCESS;
    else if(internalData->status==myFIFO::UNLOCKED){
      internalData->status=myFIFO::LOCKEDFORRD;
      res = myFIFO::SUCCESS;
    }else{
      while(1){
	semop(internalData->semid,&ops[2],1);
	usleep(10);
	semop(internalData->semid,ops,2);
	if(internalData->status==myFIFO::UNLOCKED){
	  internalData->status=myFIFO::LOCKEDFORRD;
	  res = myFIFO::SUCCESS;
	  break;
	}
      }
    }
  }else if(aType==myFIFO::WRITER){
    if(internalData->status==myFIFO::LOCKEDFORWR)
      res = myFIFO::SUCCESS;
    else if(internalData->status==myFIFO::UNLOCKED){
      internalData->status=myFIFO::LOCKEDFORWR;
      res = myFIFO::SUCCESS;
    }else{
      while(1){
	semop(internalData->semid,&ops[2],1);
	usleep(10);
	semop(internalData->semid,ops,2);
	if(internalData->status==myFIFO::UNLOCKED){
	  internalData->status=myFIFO::LOCKEDFORWR;
	  res = myFIFO::SUCCESS;
	  break;
	}
      }
    }
  }

  semop(internalData->semid,&ops[2],1);
  return res;
  
}

/*myFIFO::result myFIFO::trylock(myFIFO::userType aType){

  myFIFO::result res=myFIFO::FAILED;
  struct sembuf ops[3];
  
  ops[0].sem_num=1;
  ops[0].sem_op=0;
  ops[0].sem_flg=IPC_NOWAIT;
  
  ops[1].sem_num=1;
  ops[1].sem_op=1;
  ops[1].sem_flg=0;
  
  ops[2].sem_num=1;
  ops[2].sem_op=-1;
  ops[2].sem_flg=0;

  if(aType==myFIFO::SAMPLER)
    return myFIFO::FAILED;

  //lock the variable before read
  if(semop(internalData->semid,ops,2)==-1)
    return myFIFO::FAILED;

  if(aType==myFIFO::READER){
    if(internalData->status==myFIFO::LOCKEDFORRD)
      res = myFIFO::SUCCESS;
    else if(internalData->status==myFIFO::UNLOCKED){
      internalData->status=myFIFO::LOCKEDFORRD;
      res = myFIFO::SUCCESS;
    }else if(internalData->status==myFIFO::LOCKEDFORWR)
      res = myFIFO::FAILED;
  }else{
    if(internalData->status==myFIFO::LOCKEDFORWR)
      res = myFIFO::SUCCESS;
    else if(internalData->status==myFIFO::UNLOCKED){
      internalData->status=myFIFO::LOCKEDFORWR;
      res = myFIFO::SUCCESS;
    }else if(internalData->status==myFIFO::LOCKEDFORRD)
      res = myFIFO::FAILED;
  }

  semop(internalData->semid,&ops[2],1);
  return res;
}*/

myFIFO::result myFIFO::unlock(myFIFO::userType aType){

  myFIFO::result res=myFIFO::FAILED;
  struct sembuf ops[3];
  
  ops[0].sem_num=1;
  ops[0].sem_op=0;
  ops[0].sem_flg=0;
  
  ops[1].sem_num=1;
  ops[1].sem_op=1;
  ops[1].sem_flg=0;
  
  ops[2].sem_num=1;
  ops[2].sem_op=-1;
  ops[2].sem_flg=0;

  //lock the variable before read
  semop(internalData->semid,ops,2);

  if(internalData->status==myFIFO::UNLOCKED)
      res=myFIFO::SUCCESS;
  else{
    if(aType==myFIFO::SAMPLER){
      if(internalData->status!=myFIFO::LOCKEDFORSM &&
	 internalData->sam_locker!=sam_ind)
	res=myFIFO::FAILED;
      else{
	internalData->status=myFIFO::UNLOCKED;
	res=myFIFO::SUCCESS;
      }
    }else if(aType==myFIFO::READER){
      if(internalData->status!=myFIFO::LOCKEDFORRD)
	res=myFIFO::FAILED;
      else{
	internalData->status=myFIFO::UNLOCKED;
	res=myFIFO::SUCCESS;
      }
    }else if(aType==myFIFO::WRITER){
      if(internalData->status!=myFIFO::LOCKEDFORWR)
	res=myFIFO::FAILED;
      else{
	internalData->status=myFIFO::UNLOCKED;
	res=myFIFO::SUCCESS;
      }
    }
  }
  semop(internalData->semid,&ops[2],1);
  return res;
}


bool myFIFO::isPresent(userType aType){

  if(aType==myFIFO::READER){
    if(internalData->reader==PRESENT)
      return true;
  }else if(aType==myFIFO::WRITER){
    if(internalData->writer==PRESENT)
      return true;
  }

  return false;
}

void myFIFO::waitFor(userType aType){

  if(aType==myFIFO::READER){
    while(internalData->reader!=PRESENT)
      usleep(10);
  }else if(aType==myFIFO::WRITER){
    while(internalData->writer!=PRESENT)
      usleep(10);
  }
}

myFIFO::result myFIFO::status(){

  if(internalData->status==LOCKEDFORWR)
    return WRLOCKED;
  if(internalData->status==LOCKEDFORRD
     || internalData->status==LOCKEDFORSM)
    return RDLOCKED;
  if(internalData->status==UNLOCKED)
    return SUCCESS;

  return FAILED;
}

void myFIFO::backup(){
  size_t sz = sizeof(struct myFIFOData);
  if (backupData == NULL) backupData = (struct myFIFOData*)malloc(sz);
  memcpy(backupData, internalData, sz);
}

void myFIFO::restore(){
  size_t sz = sizeof(struct myFIFOData);
  uint32_t sn = internalData->spillnr;
  if (backupData != NULL) memcpy(internalData, backupData, sz);
  internalData->spillnr = sn;
}
