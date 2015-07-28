#include <pt2.h>   // include custom protothread library
static struct pt pt1; // each protothread needs one of these

static struct
{
  int xx1 = 0;
  int xx2 = 0;
} THREAD_VARS_1;

static int protothread1(struct pt *pt, int interval) {
  static unsigned long timestamp = 0;
  PT_BEGIN(pt);
  while(1) { // never stop 
    /* each time the function is called the second boolean
    *  argument "millis() - timestamp > interval" is re-evaluated
    *  and if false the function exits after that. */
    PT_WAIT_UNTIL(pt, millis() - timestamp > interval );
    timestamp = millis(); // take a new timestamp
    
    //run thread at given ms in loop
    Thread1();
  }
  PT_END(pt);
}


//the real threads of the system, please don't use delay too much.
void Thread1() {
  //stuff in the thread
  THREAD_VARS_1.xx1++;
  if (THREAD_VARS_1.xx1 > 99) {
    THREAD_VARS_1.xx2++;
    THREAD_VARS_1.xx1=0;
  }
}




void setup() {
  PT_INIT(&pt1);
  pt1.data = &THREAD_VARS_1;
}

void loop() {
  protothread1(&pt1, 1000); // update every 1000 frames (aka ONE FPS)
}
