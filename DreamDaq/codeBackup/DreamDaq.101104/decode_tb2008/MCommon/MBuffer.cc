//////////////////////////////////////////////////////////
//  class MBuffer
//
//  
//  
//
//  created: Florian Goebel   Aug 2002
//////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <limits.h>

#include "MBuffer.h"

//#define DEBUG


/////////////////////////////////////////////////////////////////////////////
// MBuffer::MBuffer(unsigned int BufferSizeBytes, 
//	            unsigned int BlockSizeBytes, 
//	            int MaxWriteThreads, unsigned int WriteSegmentSizeBlocks,
//	            int MaxReadThreads,  unsigned int ReadSegmentSizeBlocks,
//	            int MaxAnaThreads,   unsigned int AnaSegmentSizeBlocks)
//
//  allocate buffer of <bSize> bytes length
//
/////////////////////////////////////////////////////////////////////////////
MBuffer::MBuffer(unsigned int BufferSizeBytes, 
		 unsigned int BlockSizeBytes, 
		 int MaxWriteThreads, unsigned int WriteSegmentSizeBlocks,
		 int MaxReadThreads,  unsigned int ReadSegmentSizeBlocks,
		 int MaxAnaThreads,   unsigned int AnaSegmentSizeBlocks) 
{

  mutex     = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
  lock_free = (pthread_cond_t  *) malloc(sizeof(pthread_cond_t));
  
  alloc_mutex = 1;  // mutex has been allocated by MBuffer

  pthread_mutex_init (mutex, NULL);
  pthread_cond_init  (lock_free, NULL);

  init_buffer(BufferSizeBytes, BlockSizeBytes,
	      MaxWriteThreads, WriteSegmentSizeBlocks,
	      MaxReadThreads,  ReadSegmentSizeBlocks,
	      MaxAnaThreads,   AnaSegmentSizeBlocks);

}

/////////////////////////////////////////////////////////////////////////////
// MBuffer::MBuffer(pthread_mutex_t *Mutex,
//                  pthread_cond_t  *LockFree,
//                  unsigned int BufferSizeBytes, 
//	            unsigned int BlockSizeBytes, 
//	            int MaxWriteThreads, unsigned int WriteSegmentSizeBlocks,
//	            int MaxReadThreads,  unsigned int ReadSegmentSizeBlocks,
//	            int MaxAnaThreads,   unsigned int AnaSegmentSizeBlocks)
//
//  allocate buffer of <bSize> bytes length
//
/////////////////////////////////////////////////////////////////////////////
MBuffer::MBuffer(pthread_mutex_t *Mutex,
		 pthread_cond_t  *LockFree,
		 unsigned int BufferSizeBytes, 
		 unsigned int BlockSizeBytes, 
		 int MaxWriteThreads, unsigned int WriteSegmentSizeBlocks,
		 int MaxReadThreads,  unsigned int ReadSegmentSizeBlocks,
		 int MaxAnaThreads,   unsigned int AnaSegmentSizeBlocks) {

  if (Mutex    != NULL)  mutex     = Mutex;
  if (LockFree != NULL)  lock_free = LockFree;

  alloc_mutex = 0;  // mutex has been allocated by external function

  init_buffer(BufferSizeBytes, BlockSizeBytes,
	      MaxWriteThreads, WriteSegmentSizeBlocks,
	      MaxReadThreads,  ReadSegmentSizeBlocks,
	      MaxAnaThreads,   AnaSegmentSizeBlocks);
} 

/////////////////////////////////////////////////////////////////////////////
// void MBuffer::init_buffer(unsigned int BufferSizeBytes, 
//	                unsigned int BlockSizeBytes, 
//	                int MaxWriteThreads, unsigned int WriteSegmentSizeBlocks,
//	                int MaxReadThreads,  unsigned int ReadSegmentSizeBlocks,
//	                int MaxAnaThreads,   unsigned int AnaSegmentSizeBlocks)
//
//  allocate buffer of <bSize> bytes length
//
/////////////////////////////////////////////////////////////////////////////
void MBuffer::init_buffer(unsigned int BufferSizeBytes, 
			  unsigned int BlockSizeBytes, 
			  int MaxWriteThreads, unsigned int WriteSegmentSizeBlocks,
			  int MaxReadThreads,  unsigned int ReadSegmentSizeBlocks,
			  int MaxAnaThreads,   unsigned int AnaSegmentSizeBlocks) {

  if (MaxWriteThreads < 1) {
    fprintf(stderr,"!!ERROR!! MBuffer\n");
    fprintf(stderr,"          Number of MaxWriteThreads:%d must be > 0\n", MaxWriteThreads);
  }

  block_size_bytes   = BlockSizeBytes;
  buffer_size_blocks = (int) (BufferSizeBytes / BlockSizeBytes);
  buffer_size_bytes  = buffer_size_blocks * block_size_bytes; 


  buffer         = malloc(buffer_size_bytes);

  num_stream_limits = 0;
  int next_stream_limit = 0;

  stream_limit[num_stream_limits++] = new StreamLimit(1);

  if (MaxAnaThreads > 0) {
    stream_limit[num_stream_limits++] = new StreamLimit(1);
  }
  if (MaxReadThreads > 0) {
    stream_limit[num_stream_limits++] = new StreamLimit(1);
  }

  write_stream = new Stream(this, 
			    MaxWriteThreads,  
			    WriteSegmentSizeBlocks,
			    // stream_limit[1], stream_limit[0],
			    stream_limit[num_stream_limits-1], stream_limit[0],
			    0,
			    1, 1);

  if (MaxAnaThreads > 0) {
    ana_stream = new Stream(this, 
			    MaxAnaThreads, 
			    AnaSegmentSizeBlocks,
			    stream_limit[next_stream_limit], stream_limit[next_stream_limit+1],
			    //			    1,
			    0,
			    0, 1);
    ana_stream_active = 1;
    next_stream_limit++;
  } else {
    ana_stream = NULL;
    ana_stream_active = 0;
    AnaSegmentSizeBlocks = 1;
  }

  if (MaxReadThreads > 0) {
    read_stream = new Stream(this, 
			     MaxReadThreads, 
			     ReadSegmentSizeBlocks,
			     stream_limit[next_stream_limit], stream_limit[next_stream_limit+1],
			     0,
			     0, 1);
    read_stream_active = 1;
  } else {
    read_stream = NULL;
    read_stream_active = 0;
    ReadSegmentSizeBlocks = 1;
  }


  if (MaxAnaThreads == 0)  deactivate_ana();
  if (MaxReadThreads == 0) deactivate_read();

  max_blocks = INT_MAX;

  if (   ((buffer_size_blocks % WriteSegmentSizeBlocks) != 0)
      || ((buffer_size_blocks % ReadSegmentSizeBlocks) != 0)
      || ((buffer_size_blocks % AnaSegmentSizeBlocks) != 0)) {
      fprintf(stderr,"!!ERROR!! MBuffer\n");
      fprintf(stderr,"          Buffer Size not multiple of Segment Size\n");
      exit(1);
  }
}


//////////////////////////////////////////////////////////
// MBuffer::~MBuffer()
//  free buffer, mutex, streams
//
//////////////////////////////////////////////////////////
MBuffer::~MBuffer() {

  free(buffer);

#ifdef DEBUG
  printf("~MBuffer(): after free(buffer)\n");
#endif

  if (alloc_mutex) {

#ifdef DEBUG
    printf("~MBuffer(): before pthread_mutex_destroy (mutex)\n");
#endif

    pthread_mutex_destroy (mutex);

#ifdef DEBUG
    printf("~MBuffer(): after pthread_mutex_destroy\n");
#endif
    
    pthread_cond_destroy  (lock_free);
    
#ifdef DEBUG
    printf("~MBuffer(): after pthread_cond_destroy\n");
#endif

    free (mutex);

#ifdef DEBUG
    printf("~MBuffer(): after  free (mutex)\n");
#endif
  
    free (lock_free);

#ifdef DEBUG
    printf("~MBuffer(): after  free (lock_free)\n");
#endif
  
  }

#ifdef DEBUG
  printf("~MBuffer(): before delete *_stream\n");
#endif

  delete write_stream;
  delete read_stream;
  delete ana_stream;
  
#ifdef DEBUG
  printf("~MBuffer(): after delete *_stream\n");
#endif

  for (int i=0; i<num_stream_limits; i++) {
    delete stream_limit[i];
  }

#ifdef DEBUG
  printf("~MBuffer(): after delete stream_limit\n");
#endif

}


//////////////////////////////////////////////////////////
// int MBuffer::reset()
//  
//////////////////////////////////////////////////////////
int MBuffer::reset() {

  pthread_mutex_lock(mutex);

  max_blocks = INT_MAX;
  
  if (write_stream) {
    write_stream->reset();
  }
  if (read_stream) {
    read_stream->reset();
  }
  if (ana_stream) {
    ana_stream->reset();
  }

  for (int i=0; i<num_stream_limits; i++) {
    stream_limit[i]->reset();
  }

  pthread_cond_broadcast(lock_free);
  pthread_mutex_unlock(mutex);

  return 1;
}


//////////////////////////////////////////////////////////
// int MBuffer::stop_writing()
//  
//////////////////////////////////////////////////////////
int MBuffer::stop_writing() {
  unsigned int wlimit;

  pthread_mutex_lock(mutex);
    
  wlimit = stream_limit[0]->update();
    
  if (max_blocks > wlimit) {
    max_blocks = wlimit;
  }
    
  pthread_cond_broadcast(lock_free);
  pthread_mutex_unlock(mutex);
  
  return max_blocks;
}


//////////////////////////////////////////////////////////
// int MBuffer::set_max_events(int max_events)
//  
//////////////////////////////////////////////////////////
int MBuffer::set_max_events(int max_events) {
  
  max_blocks = max_events;
  
  return max_blocks;
}




void MBuffer::deactivate_read() {
  if (read_stream) {
    read_stream_active = 0;
    if (ana_stream_active) {
      write_stream->buffer_limit = ana_stream->stream_limit;
    } else {
      write_stream->buffer_limit = write_stream->stream_limit;
    }
  }
}

void MBuffer::activate_read() {
  if (read_stream) {
    read_stream_active = 1;
    write_stream->buffer_limit = read_stream->stream_limit;
  }
}


void MBuffer::deactivate_ana() {
  if (ana_stream) {
    ana_stream_active = 0;
    if (read_stream_active) {
      read_stream->buffer_limit  = write_stream->stream_limit;
    } else {
      write_stream->buffer_limit = write_stream->stream_limit;
    }
  }
}

void MBuffer::activate_ana() {
  if (ana_stream) {
    ana_stream_active = 1;
    if (read_stream_active) {
      read_stream->buffer_limit = ana_stream->stream_limit;
    } else {
      write_stream->buffer_limit = ana_stream->stream_limit;
    }      
  }
}

