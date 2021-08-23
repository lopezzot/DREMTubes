//////////////////////////////////////////////////////////
//  class Stream
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
#include "Stream.h"
#include "StreamLimit.h"


#define noDEBUG2
#define noDEBUG

//////////////////////////////////////////////////////////
//  Stream::Stream(MBuffer      *Buffer, 
//		 unsigned int  MaxThreads, 
//		 unsigned int  SegmentSizeBlocks,
//		 StreamLimit   *Buffer_Limit,
//               StreamLimit   *Stream_Limit,
//		 unsigned int  StreamLimitId,
//		 unsigned int  Type,
//		 unsigned int  NumberIncrement)
//
//////////////////////////////////////////////////////////
Stream::Stream(MBuffer      *Buffer, 
	       unsigned int  MaxThreads, 
	       unsigned int  SegmentSizeBlocks,
	       StreamLimit  *Buffer_Limit,
	       StreamLimit  *Stream_Limit,
	       unsigned int  StreamLimitId,
	       unsigned int  Type,
	       unsigned int  NumberIncrement) {

  buffer              = Buffer;
  max_threads         = MaxThreads;
  segment_size_blocks = SegmentSizeBlocks;
  buffer_limit        = Buffer_Limit;
  stream_limit        = Stream_Limit;
  stream_limit_id     = StreamLimitId;
  type                = Type;
  number_increment    = NumberIncrement;

  create_segments();
  reset_segments();
  
  buffer_size_bytes    = buffer->buffer_size_bytes;
  buffer_size_blocks   = buffer->buffer_size_blocks;
  block_size_bytes     = buffer->block_size_bytes;
  segment_size_bytes   = segment_size_blocks * block_size_bytes;
  max_blocks           = &buffer->max_blocks;
  buffer_start_address = buffer->buffer;
  mutex                = buffer->mutex;
  lock_free            = buffer->lock_free;
  check_segment_block  = INT_MAX;
}


//////////////////////////////////////////////////////////
// Stream::~Stream()
//  free segment
//
//////////////////////////////////////////////////////////
Stream::~Stream() {

#ifdef DEBUG
  printf("~Stream: before delete_segments()\n");
#endif

  delete_segments();

#ifdef DEBUG
  printf("~Stream: after delete_segments()\n");
#endif
}


//////////////////////////////////////////////////////////
// Stream::reset()
//
//////////////////////////////////////////////////////////
void Stream::reset() {

  reset_segments();
  check_segment_block = INT_MAX;
}


//////////////////////////////////////////////////////////
// bSegment* Stream::lock_segment()
//  
//////////////////////////////////////////////////////////
bSegment* Stream::lock_segment() {
  int cond = 1;
  return lock_segment(&cond);
}


//////////////////////////////////////////////////////////
// bSegment* Stream::lock_segment(int *condition)
//  
//      the pointer condition should also be protected 
//      by the same mutex variable.
//////////////////////////////////////////////////////////
bSegment* Stream::lock_segment(int *condition) {
  bSegment* segment;
  unsigned int next_block;

  pthread_mutex_lock(mutex);

  next_block = (last_segment->number + 1) * segment_size_blocks;
  while (! (   (   (next_block <= buffer_limit->update() + type * buffer_size_blocks)
		&& (next_block <= check_segment_block + type * buffer_size_blocks ))
	    || (next_block  > *max_blocks)
            || (! (*condition)) )) {           // external condition //

#ifdef DEBUG2
    fprintf(stderr,"Stream::lock_segment  this:%x  blocked\n", this);
    fprintf(stderr,"         next_block: %d  max_blocks: %d\n", next_block, *max_blocks);
    fprintf(stderr,"         buffer_limit: %d  type * buffer_size_blocks: %d check_segment_block: %d\n", 
	    buffer_limit->limit(), type * buffer_size_blocks, check_segment_block);
#endif

    pthread_cond_wait(lock_free, mutex);
    next_block = (last_segment->number + 1) * segment_size_blocks;
  }

#ifdef DEBUG2
    fprintf(stderr,"Stream::lock_segment  this:%x  unblocked\n", this);
    fprintf(stderr,"         next_block: %d  max_blocks: %d\n", next_block, *max_blocks);
    fprintf(stderr,"         buffer_limit: %d  type * buffer_size_blocks: %d check_segment_block: %d\n", 
	    buffer_limit->limit(), type * buffer_size_blocks, check_segment_block);
    fprintf(stderr,"         ext condition %d\n", *condition);
#endif

  if (   (next_block  > *max_blocks)
      || (! (*condition))) {
    pthread_mutex_unlock(mutex);
    return NULL;                    // no more writing //
  }

  segment = last_segment;

#ifdef ALLOC
  last_segment = (bSegment *) malloc(sizeof(bSegment));
#else
  last_segment = find_free_segment();
#endif

  last_segment->number           = segment->number + number_increment;
  last_segment->previous_segment = segment;
  last_segment->next_segment     = NULL;
  last_segment->status           = 1;

  segment->next_segment = last_segment;
    
  pthread_mutex_unlock(mutex);

#ifdef DEBUG
  printf("lock_segment: got segment: ");
  print_segment(segment);
#endif

  return segment;
}


////////////////////////////////////////////////////////////////////
// Stream::unlock_segment(bSegment *segment, free_segment)
//  
//  free_segment:  if free_segment == 0 then the segment will not
//                 be freed for access from other stream
//
////////////////////////////////////////////////////////////////////
void Stream::unlock_segment(bSegment *segment, int free_segment) {
  bSegment* tmp_segment;

  pthread_mutex_lock(mutex);

  if (segment->next_segment == NULL) {
    fprintf(stderr,"!!ERROR!! Stream::unlock_segment\n");
    fprintf(stderr,"segment->next_segment == NULL\n");
#ifdef DEBUG
    printf("unlock_segment: got segment: ");
    print_segment(segment);
#endif
    exit(1);
  }

// if next segment already unlock 
// => merge with next segment
  if (segment->next_segment->status == 2) { 
    tmp_segment           = segment->next_segment;
    segment->next_segment = tmp_segment->next_segment;
    if (segment->next_segment != NULL) {
	segment->next_segment->previous_segment = segment;
    }
#ifdef ALLOC
    free(tmp_segment);
#else
    tmp_segment->status = 0;
#endif
  }

// if previous segment already unlock 
// => merge with previous segment
  if (segment->previous_segment != NULL
      && segment->previous_segment->status == 2) {
    tmp_segment = segment;
    segment     = segment->previous_segment;
    segment->next_segment = tmp_segment->next_segment;
    if (segment->next_segment != NULL) {
	segment->next_segment->previous_segment = segment;
    }
#ifdef ALLOC
    free(tmp_segment);
#else
    tmp_segment->status = 0;
#endif
  }

// segment status unlocked
  segment->status = 2;

  if (segment == first_segment) {
    first_segment = segment->next_segment;
    first_segment->previous_segment = NULL;
#ifdef ALLOC
    free(segment);
#else
    segment->status = 0;
#endif

    if (free_segment > 0) {
      stream_limit->set_limit(first_segment->number * segment_size_blocks, 
			      stream_limit_id);
    }

#ifdef DEBUG
    fprintf(stderr,"Stream::unlock_segment: broadcast\n");
#endif
    pthread_cond_broadcast(lock_free);
  }

  pthread_mutex_unlock(mutex);
}


/////////////////////////////////////////////////////////////////////
// int Stream::lock_check_segment(unsigned int check_segment_number)
//  
/////////////////////////////////////////////////////////////////////
int Stream::lock_check_segment(unsigned int *check_segment_number) {
  int locked = 0;
  
  *check_segment_number = 0;
  
#ifdef DEBUG
  fprintf(stderr,">>>>Stream::lock_check_segment() this:%x\n", this);
#endif

  pthread_mutex_lock(mutex);
  
  if (check_segment_block == INT_MAX  &&  first_segment->number > 0) {
    *check_segment_number = first_segment->number - 1;
    check_segment_block  = *check_segment_number * segment_size_blocks;
    locked = 1;
  } 

  pthread_mutex_unlock(mutex);

#ifdef DEBUG
  fprintf(stderr,">>>>Stream::lock_check_segment() check_segment_block: %d\n", 
	  check_segment_block);
#endif

  return locked;
}


//////////////////////////////////////////////////////////
// void Stream::unlock_check_segment()
//  
//////////////////////////////////////////////////////////
void Stream::unlock_check_segment() {

#ifdef DEBUG
  fprintf(stderr,">>>>Stream::unlock_check_segment() this:%x\n", this);
#endif

  pthread_mutex_lock(mutex);

  check_segment_block = INT_MAX;

#ifdef DEBUG
    fprintf(stderr,"Stream::unlock_check_segment: broadcast\n");
#endif
  pthread_cond_broadcast(lock_free);
  pthread_mutex_unlock(mutex);

#ifdef DEBUG
  fprintf(stderr,">>>>Stream::unlock_check_segment() check_segment_block: %d\n", 
	  check_segment_block);
#endif
}


//////////////////////////////////////////////////////////
// void Stream::set_number_increment(unsigned int NumberIncrement)
//  
//////////////////////////////////////////////////////////
void Stream::set_number_increment(unsigned int NumberIncrement) {

  fprintf(stderr,"set_number_increment: Does not make sense in the current implementation\n");
  exit(1);

  number_increment = NumberIncrement;
}


//////////////////////////////////////////////////////////
// unsigned int Stream::segment_number(bSegment *segment)
//  
//////////////////////////////////////////////////////////
unsigned int Stream::segment_number(bSegment *segment) {
  return segment->number;
}


//////////////////////////////////////////////////////////
// unsigned int Stream::segment_block(bSegment *segment)
//  
//////////////////////////////////////////////////////////
unsigned int Stream::segment_block(bSegment *segment) {
  return segment->number * segment_size_blocks;
}


//////////////////////////////////////////////////////////
// (void *) Stream::segment_address(bSegment *segment)
//
//////////////////////////////////////////////////////////
void* Stream::segment_address(bSegment *segment) {
 
  return (void *) ((int)buffer_start_address 
		   + ((segment->number * segment_size_blocks) 
		      % buffer_size_blocks) * block_size_bytes);
}


//////////////////////////////////////////////////////////
// (void *) Stream::segment_number_address(unsigned int segment_number)
//
//////////////////////////////////////////////////////////
void* Stream::segment_number_address(unsigned int segment_number) {
 
  return (void *) ((int)buffer_start_address 
		   + ((segment_number * segment_size_blocks) 
		      % buffer_size_blocks) * block_size_bytes);
}


//////////////////////////////////////////////////////////
// void  Stream::activate()
//
//////////////////////////////////////////////////////////
void Stream::activate() {

  stream_limit->set_mask(STREAM_ACTIV, stream_limit_id);
}


//////////////////////////////////////////////////////////
// void  Stream::deactivate()
//
//////////////////////////////////////////////////////////
void Stream::deactivate() {

  stream_limit->set_mask(STREAM_INACTIV, stream_limit_id);
}




#ifndef ALLOC
//////////////////////////////////////////////////////////
// bSegment *Stream::find_free_segment()
//  
//////////////////////////////////////////////////////////
bSegment *Stream::find_free_segment() {
  unsigned int i;

  for (i=0; i<2*max_threads; i++) {
    if (segments[i].status == 0) {

#ifdef DEBUG2
      printf("find_free_segment: found segment %d\n", i);
#endif
      return &segments[i];
    }
  }

  fprintf(stderr, "!!ERROR!! Stream::find_free_segment \n");
  fprintf(stderr, "          no free segment found\n");
  fprintf(stderr, "          too many threads for this stream?\n");
  exit(1);
}
#endif


//////////////////////////////////////////////////////////
// bSegment *Stream::create_segments()
//  
//////////////////////////////////////////////////////////
bSegment *Stream::create_segments() {
  unsigned int i;

#ifdef ALLOC
  first_segment = (bSegment *) malloc(sizeof(bSegment));
#else
  segments      = (bSegment *) calloc(2*max_threads, sizeof(bSegment));
  first_segment = &segments[0];
  for (i=0; i<2*max_threads; i++) {
    segments[i].status = 0;
  }

#ifdef DEBUG
  printf("Stream::create_segments: created segments: %x\n",(unsigned int)segments);
#endif

#endif

  return first_segment;
}


//////////////////////////////////////////////////////////
// bSegment *Stream::delete_segments()
//  
//////////////////////////////////////////////////////////
void Stream::delete_segments() {

#ifdef ALLOC
  bSegment *segment;
  bSegment *tmp_segment;
    
  segment = first_segment;
  while (segment != NULL) {
    tmp_segment = segment;
    segment     = tmp_segment->next_segment;
    free(tmp_segment);
  }
#else

#ifdef DEBUG
  printf("Stream::delete_segments: free segments: %x\n",(unsigned int)segments);
#endif

 free(segments);

#ifdef DEBUG
  printf("Stream::delete_segments: freed segments: %x\n",(unsigned int)segments);
#endif

#endif  
}

//////////////////////////////////////////////////////////
// void Stream::reset_segments()
//  
//////////////////////////////////////////////////////////
void Stream::reset_segments() {
  unsigned int i;

#ifdef ALLOC
  bSegment *segment;
  bSegment *tmp_segment;

  segment = first_segment->next_segment;
  while (segment != NULL) {
    tmp_segment = segment;
    segment     = tmp_segment->next_segment;
    free(tmp_segment);
  }
#else
  for (i=0; i<2*max_threads; i++) {
    segments[i].status = 0;
  }  
#endif

  first_segment->number           = 0;
  first_segment->previous_segment = NULL;
  first_segment->next_segment     = NULL;
  first_segment->status           = 1;

  last_segment  = first_segment;
}


void Stream::print_segment(bSegment *segment) {
  printf("Segment 0x%x num: %d status: %d prev: 0x%x next: 0x%x\n", 
	 (unsigned int)segment,
	 segment->number,
	 segment->status,
	 (unsigned int)segment->previous_segment,
	 (unsigned int)segment->next_segment);
}
