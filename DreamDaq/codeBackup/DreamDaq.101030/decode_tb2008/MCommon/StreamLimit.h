#ifndef STREAMLIMIT_H_SEEN
#define STREAMLIMIT_H_SEEN

#define STREAM_ACTIV   1
#define STREAM_INACTIV 0

class StreamLimit {

 public:
   StreamLimit(int N);
   ~StreamLimit();

   void         reset();

   unsigned int update(unsigned int Val, unsigned int Id);
   unsigned int update();
   unsigned int limit();
   void         set_limit(unsigned int Val, unsigned int Id);

   void         set_mask(unsigned int Mask, unsigned int Id);

 private:
   int n;
   unsigned int *val;
   unsigned int *mask;
   unsigned int min;
};

#endif
