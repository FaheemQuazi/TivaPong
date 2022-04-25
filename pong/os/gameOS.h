#ifndef _PONG_OS_GAMEOS_H_
#define _PONG_OS_GAMEOS_H_

#include <stdint.h>
#include <stdbool.h>
#include "BSP.h"
#include "CortexM.h"

// Thread Control Blocks
#define NUMTHREADS 4    // maximum number of threads
#define NUMPRIORITY 255 // max value for priority
#define STACKSIZE 100   // number of 32-bit words in stack per thread

typedef void(*gOS_Func)(void);

struct tcb {
  int32_t *sp;        // pointer to stack (valid for threads not running
  struct tcb *next;       // linked-list pointer
  uint32_t period;
  uint32_t count;
  // Semaphore *blocked; // pointer to blocking semaphore
  // uint32_t priority;
};
typedef struct tcb Thread;

// typedef struct sema {
//   Thread *blocked[NUMTHREADS];
//   int blockedThreads;
// } Semaphore;

// typedef struct mtx {
//   void* res;          // the resource being gated
//   Semaphore lock;     // the locking semaphore
// } Mutex;

void gOS_Init(void);
void gOS_Launch(uint32_t theTimeSlice);
int gOS_AddThreads(void (*th0)(void),void (*th1)(void), void (*th2)(void), void (*th3)(void));
void gOS_Suspend(void);
void gOS_Sleep(uint32_t sleepTime);

#endif
