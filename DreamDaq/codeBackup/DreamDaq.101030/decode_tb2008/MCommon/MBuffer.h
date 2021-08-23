#ifndef MBUFFER_H_SEEN
#define MBUFFER_H_SEEN
/******************************************************************************
 *  class MBuffer
 *
 *  
 *  
 *
 *  created: Florian Goebel   Aug 2002
 ******************************************************************************/

#include <pthread.h>

#include "Stream.h"
#include "StreamLimit.h"


class MBuffer {
  
 public:
   MBuffer(unsigned int BufferSizeBytes, 
	   unsigned int BlockSizeBytes, 
	   int MaxWriteThreads, unsigned int WriteSegmentSizeBlocks,
	   int MaxReadThreads,  unsigned int ReadSegmentSizeBlocks,
	   int MaxAnaThreads = 0,   
	   unsigned int AnaSegmentSizeBlocks = 1);
   MBuffer(pthread_mutex_t *Mutex,
	   pthread_cond_t  *LockFree,
	   unsigned int BufferSizeBytes, 
	   unsigned int BlockSizeBytes, 
	   int MaxWriteThreads, unsigned int WriteSegmentSizeBlocks,
	   int MaxReadThreads,  unsigned int ReadSegmentSizeBlocks,
	   int MaxAnaThreads = 0, 
	   unsigned int AnaSegmentSizeBlocks = 1); 
   ~MBuffer();
   
   int reset();
   int stop_writing();
   int set_max_events(int max_events);

   void deactivate_read();
   void activate_read();
   void deactivate_ana();
   void activate_ana();


   inline int num_blocks_used() {
     return stream_limit[0]->update() - stream_limit[num_stream_limits-1]->update();
   };

   inline int num_blocks_free() {
     return  buffer_size_blocks - num_blocks_used();
   };



 private:
   void init_buffer(unsigned int BufferSizeBytes, 
		    unsigned int BlockSizeBytes, 
		    int MaxWriteThreads, unsigned int WriteSegmentSizeBlocks,
		    int MaxReadThreads,  unsigned int ReadSegmentSizeBlocks,
		    int MaxAnaThreads = 0, 
		    unsigned int AnaSegmentSizeBlocks = 1);

 public:
   Stream* write_stream;
   Stream* read_stream;
   Stream* ana_stream;

   int read_stream_active;
   int ana_stream_active;

 private:
   void* buffer;
   unsigned int buffer_size_bytes;
   unsigned int buffer_size_blocks;
   unsigned int block_size_bytes;
   unsigned int max_blocks;

   pthread_mutex_t *mutex;
   pthread_cond_t  *lock_free;

   int alloc_mutex; 

   //   StreamLimit* writing_limit;
   //   StreamLimit* reading_limit;
   //   StreamLimit* ana_limit;

   StreamLimit* stream_limit[3];
   int num_stream_limits;
   
   friend class Stream;

};

#endif

