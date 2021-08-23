
#include <sched.h>
#include <unistd.h>

void setMess(const char * mess);

int my_sleep()
 {
  sched_yield();
  return 0;
 }

int my_loop()
 {
  do { usleep(1000); my_sleep(); } while(1);
  return 0;
 }
int main()
 {
  my_loop();
  return 0;
 }

