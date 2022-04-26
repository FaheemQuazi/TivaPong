#ifndef _PONG_OS_GAMEOS_H_
#define _PONG_OS_GAMEOS_H_

#include <stdint.h>
#include <stdbool.h>
#include "BSP.h"
#include "CortexM.h"

// Thread Control Blocks
#define NUMTHREADS 6    // maximum number of threads
#define NUMPRIORITY 255 // max value for priority
#define STACKSIZE 100   // number of 32-bit words in stack per thread

typedef void(*gOS_Func)(void);

struct tcb {
  int32_t *sp;        // pointer to stack (valid for threads not running
  struct tcb *next;       // linked-list pointer
  uint32_t period;
  uint32_t count;
  int32_t * blocked; // pointer to blocking semaphore
  int locked;
  void (*thRef)(void);
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
void gOS_Suspend(void);
void gOS_Sleep(uint32_t sleepTime);
void gOS_InitSemaphore(int32_t *semaPt, int32_t value);
void gOS_Wait(int32_t *semaPt);
void gOS_Signal(int32_t *semaPt);
// int gOS_Lock(int thIdx);
// int gOS_Unlock(int thIdx);
int gOS_Lock(void (*th)(void));
int gOS_Unlock(void (*th)(void));
void gOS_AddThreads(void (*ths[])(void));

#endif
