#include "gameOS.h"

// functions in osasm.s
void StartOS(void);
// variables accessed in osasm.s
Thread *RunPt;

// Scheduled Thread Control
Thread tcbs[NUMTHREADS];
int threadCount = 0;

// Stack stuff
int32_t Stacks[NUMTHREADS][STACKSIZE];

// Screen Draw Handling
int numDrawFunctions = 0;
gOS_Func DrawFunctions[NUMTHREADS];

// "Render" Function
void gOS_RunDrawFunctions() {
  for (int i = 0; i < numDrawFunctions; i++) {
    DrawFunctions[i]();
  }
}

void gOS_Init(void) {
  DisableInterrupts();
  BSP_Clock_InitFastest();
  BSP_PeriodicTask_Init(&gOS_RunDrawFunctions, 32, 0);
}

bool gOS_RegisterDrawFunction(gOS_Func thr) {
  if (numDrawFunctions < NUMTHREADS) {
    DrawFunctions[numDrawFunctions++] = thr;
    return true;
  }
  return false;
}

void sleepManager() {
  for (int i = 0; i < NUMTHREADS; i++) {
    if (tcbs[i].count > 0) {
      tcbs[i].count = tcbs[i].count - 1;
    }
  }
}

void gOS_Launch(uint32_t theTimeSlice)
{
  STCTRL = 0;                                    // disable SysTick during setup
  STCURRENT = 0;                                 // any write to current clears it
  SYSPRI3 = (SYSPRI3 & 0x00FFFFFF) | 0xE0000000; // priority 7
  STRELOAD = theTimeSlice - 1;                   // reload value
  STCTRL = 0x00000007;                           // enable, core clock and interrupt arm
  EnableInterrupts();                            // interrupts enabled
  StartOS();                                     // start on the first task
}

void SetInitialStack(int i, int32_t initPt) {
  tcbs[i].sp = &Stacks[i][STACKSIZE-16]; // thread stack pointer 
  Stacks[i][STACKSIZE-1] = 0x01000000;  // Thumb bit 
  Stacks[i][STACKSIZE-2] = initPt;      // Initial PC
  Stacks[i][STACKSIZE-3] = 0x14141414;  // LR
  Stacks[i][STACKSIZE-4] = (int32_t)i;  // R12 
  Stacks[i][STACKSIZE-5] = 0x03030303;  // R3 
  Stacks[i][STACKSIZE-6] = 0x02020202;  // R2 
  Stacks[i][STACKSIZE-7] = 0x01010101;  // R1 
  Stacks[i][STACKSIZE-8] = 0x00000000;  // R0 
  Stacks[i][STACKSIZE-9] = 0x11111111;  // R11 
  Stacks[i][STACKSIZE-10] = 0x10101010; // R10 
  Stacks[i][STACKSIZE-11] = 0x09090909; // R9 
  Stacks[i][STACKSIZE-12] = 0x08080808; // R8 
  Stacks[i][STACKSIZE-13] = 0x07070707; // R7 
  Stacks[i][STACKSIZE-14] = 0x06060606; // R6 
  Stacks[i][STACKSIZE-15] = 0x05050505; // R5 
  Stacks[i][STACKSIZE-16] = 0x04040404; // R4 
}

void gOS_Suspend(void){ 
  STCURRENT = 0; // Reset time
  INTCTRL = 0x04000000; // trigger SysTick, but not reset time
}

int gOS_AddThreads(void (*th0)(void),void (*th1)(void), void (*th2)(void), void (*th3)(void)) {
  // Initialize linked list 
  tcbs[0].next = &tcbs[1];
  tcbs[1].next = &tcbs[2];
  tcbs[2].next = &tcbs[3];
  tcbs[3].next = &tcbs[0];

  tcbs[0].count = 0;
  tcbs[1].count = 0;
  tcbs[2].count = 0;
  tcbs[3].count = 0;

  threadCount = 4;

  // Initialize Stack for the 3 threads
  SetInitialStack(0, (int32_t)th0);
  SetInitialStack(1, (int32_t)th1);
  SetInitialStack(2, (int32_t)th2);
  SetInitialStack(3, (int32_t)th3);

  RunPt = &tcbs[0];

  return 1;
}

// Round Robin scheduler
void Scheduler(void){
    sleepManager();
    Thread* np = RunPt->next;

    while (np->count > 0) {
      np = np->next;
    }

    RunPt = np;
}

// void OS_InitSemaphore(int32_t *semaPt, int32_t value)
// {

//   DisableInterrupts();
//   (*semaPt) = value;
//   EnableInterrupts();
// }

// void OS_Wait(int32_t *semaPt)
// {
//   DisableInterrupts();
//   if ((*semaPt) <= 0)
//   {
//     // sets the blocked field to the blocking semaphore
//     RunPt->blocked = semaPt;
//   }
//   (*semaPt) = (*semaPt) - 1;
//   EnableInterrupts();
// }

void gOS_Sleep(uint32_t sleepTime){
  // set sleep parameter in TCB
  DisableInterrupts();
  RunPt->count = sleepTime;
  gOS_Suspend();
  EnableInterrupts();
}
