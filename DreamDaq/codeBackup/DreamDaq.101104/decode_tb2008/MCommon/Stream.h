#ifndef STREAM_H_SEEN
#define STREAM_H_SEEN
/******************************************************************************
 *  class Stream
 *
 *  
 *  
 *
 *  created: Florian Goebel   Aug 2002
 ******************************************************************************/

#include <pthread.h>

#include "StreamLimit.h"


typedef class MBuffer *mbuffer_ptr;

typedef struct bSegment *bsegment_ptr;

typedef struct bSegment {
  unsigned int number;
  char status;
  bsegment_ptr previous_segment;
  bsegment_ptr next_segment;
} bSegment;


class Stream {
  
 public:
    Stream(mbuffer_ptr   Buffer, 
	   unsigned int  MaxThreads, 
	   unsigned int  SegmentSizeBlocks,
	   StreamLimit   *Buffer_Limit,
	   StreamLimit   *Stream_Limit,
	   unsigned int  StreamLimitId,
	   unsigned int  Type = 1,
	   unsigned int  NumberIncrement = 1);
   ~Stream();
   
 public:
   void         reset();

   bSegment*    lock_segment ();
   bSegment*    lock_segment (int *condition);
   void         unlock_segment (bSegment *segment, int free_segment = 1);

   int          lock_check_segment(unsigned int *check_segment_number);
   void         unlock_check_segment();

   void         set_number_increment(unsigned int NumberIncrement);

   unsigned int segment_number (bSegment *segment);
   unsigned int segment_block  (bSegment *segment);
   void*        segment_address(bSegment *segment);
   void*        segment_number_address(unsigned int segment_number);
   void         activate();
   void         deactivate();

   bSegment    *create_segments();
   void         delete_segments();
   void         reset_segments();
   void         print_segment(bSegment *segment);

   int          is_last_segment() { 
       return ((last_segment->number+1) * segment_size_blocks > *max_blocks); 
   };

#ifndef ALLOC
 private:
   bSegment    *find_free_segment ();
#endif

 private:
   MBuffer     *buffer;

/* The following variables are redundant */
/* but lead to faster program */
   unsigned int    buffer_size_bytes;
   unsigned int    buffer_size_blocks;
   unsigned int    block_size_bytes;
   unsigned int    *max_blocks;
   void            *buffer_start_address;   
   pthread_mutex_t *mutex;
   pthread_cond_t  *lock_free;
   
   unsigned int  max_threads;
   unsigned int  segment_size_blocks;
   unsigned int  segment_size_bytes;

 public:
   StreamLimit  *buffer_limit;
   StreamLimit  *stream_limit;

 private:
   unsigned int  stream_limit_id;
   unsigned int  type;            /* 1: writing stream   0: reading stream */
   unsigned int  number_increment;

#ifndef ALLOC
   bSegment     *segments;
#endif
   bSegment     *first_segment;
   bSegment     *last_segment;

   unsigned int check_segment_block;
   
};

#endif
