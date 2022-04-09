// os.c
// Runs on LM4F120/TM4C123/MSP432
// Lab 2 starter file.
// Daniel Valvano
// February 20, 2016

#include <stdint.h>
#include "os.h"
#include "CortexM.h"
#include "BSP.h"
#include "tm4c123gh6pm.h"

// function definitions in osasm.s
void StartOS(void);

// TCBs
tcbType tcbs[NUMTHREADS];
int currentThreadCount = 0; // not used
// ptcbType ptcbs[NUMPERIODICTHREADS];
// int currentPeriodicThreadCount = 0;   // number of ptcbs in action
tcbType *RunPt;
tcbType *PrioPt[NUMPRIORITY];
uint32_t maxprio = 0;
int32_t Stacks[NUMTHREADS][STACKSIZE];

// FIFO
fifoItemType fifo_data[FIFO_SIZE];
fifoItemType* fifo_front;
fifoItemType* fifo_back;
int32_t fifo_curr_size; // semaphore
int fifo_lost_data;

// Mailbox
int32_t sema_mailbox;
uint32_t mailbox_data[MAILBOX_SIZE];
int mailbox_pos = 0;

void SetInitialStack(int i, int32_t initPt) {

  tcbs[i].sp = &Stacks[i][STACKSIZE - 16]; // thread stack pointer
  Stacks[i][STACKSIZE - 1] = 0x01000000;   // Thumb bit
  Stacks[i][STACKSIZE - 2] = initPt;       // Initial PC
  Stacks[i][STACKSIZE - 3] = 0x14141414;   // LR
  Stacks[i][STACKSIZE - 4] = (int32_t)i;   // R12
  Stacks[i][STACKSIZE - 5] = 0x03030303;   // R3
  Stacks[i][STACKSIZE - 6] = 0x02020202;   // R2
  Stacks[i][STACKSIZE - 7] = 0x01010101;   // R1
  Stacks[i][STACKSIZE - 8] = 0x00000000;   // R0
  Stacks[i][STACKSIZE - 9] = 0x11111111;   // R11
  Stacks[i][STACKSIZE - 10] = 0x10101010;  // R10
  Stacks[i][STACKSIZE - 11] = 0x09090909;  // R9
  Stacks[i][STACKSIZE - 12] = 0x08080808;  // R8
  Stacks[i][STACKSIZE - 13] = 0x07070707;  // R7
  Stacks[i][STACKSIZE - 14] = 0x06060606;  // R6
  Stacks[i][STACKSIZE - 15] = 0x05050505;  // R5
  Stacks[i][STACKSIZE - 16] = 0x04040404;  // R4
}

// ******** OS_Init ************
// Initialize operating system, disable interrupts
// Initialize OS controlled I/O: systick, bus clock as fast as possible
// Initialize OS global variables
// Inputs:  none
// Outputs: none
void OS_Init(void)
{
  DisableInterrupts();
  BSP_Clock_InitFastest(); // set processor clock to fastest speed
  // initialize any global variables as needed
}

//******** OS_AddThread ***************
// Add one main thread to the scheduler
// Inputs: function pointer to void/void main thread
// Outputs: 1 if successful, 0 if this thread can not be added
int OS_AddThread(void (*thr)(void), uint32_t prio) {
  if (currentThreadCount == 0) {
    
    tcbs[0].next = &tcbs[0];
    PrioPt[prio] = &tcbs[0];
    SetInitialStack(0, (int32_t)thr);
    tcbs[0].blocked = 0;
    tcbs[0].sleep = 0;
    tcbs[0].priority = prio;
    tcbs[0].hasRun = 0;
    currentThreadCount += 1;

    return 1;
  } else if (currentThreadCount < NUMTHREADS) {
    if (prio > NUMPRIORITY) {
      return 0;
    }

    tcbType *firstInPrio = 0;
    tcbType *lastInPrio = 0;

    for (int i = 0; i < currentThreadCount; i++) {
      if (tcbs[i].priority == prio) {
        firstInPrio = &tcbs[i];
        lastInPrio = tcbs[i].next;
        while (lastInPrio->next != firstInPrio) {
          lastInPrio = lastInPrio->next;
        }
        break;
      }
    }

    if (firstInPrio == 0) { // no thread with specified priority has been added
      tcbs[currentThreadCount].next = &tcbs[currentThreadCount];
      PrioPt[prio] = &tcbs[currentThreadCount];
    } else { // there's already a thread with the specified priority
      lastInPrio->next = &tcbs[currentThreadCount];
      tcbs[currentThreadCount].next = firstInPrio;
    }

    if (prio > maxprio) maxprio = prio;
    
    tcbs[currentThreadCount].blocked = 0;
    tcbs[currentThreadCount].sleep = 0;
    tcbs[currentThreadCount].priority = prio;
    tcbs[currentThreadCount].hasRun = 0;
    SetInitialStack(currentThreadCount, (int32_t)thr);
    currentThreadCount += 1;
    
    return 1;
  } else {
    // no more space for new threads
    return 0;
  }
}

void runperiodicevents(void){
// **RUN PERIODIC THREADS, DECREMENT SLEEP COUNTERS
  for (int i = 0; i < currentThreadCount; i++) {
    if (tcbs[i].sleep > 0) {
      tcbs[i].sleep--;
    }
  }
}

//******** OS_Launch ***************
// Start the scheduler, enable interrupts
// Inputs: number of clock cycles for each time slice
// Outputs: none (does not return)
// Errors: theTimeSlice must be less than 16,777,216
void OS_Launch(uint32_t theTimeSlice)
{
  // BSP_PeriodicTask_Init(&runperiodicevents, 1000, 0);
  STCTRL = 0;                                    // disable SysTick during setup
  STCURRENT = 0;                                 // any write to current clears it
  SYSPRI3 = (SYSPRI3 & 0x00FFFFFF) | 0xE0000000; // priority 7
  STRELOAD = theTimeSlice - 1;                   // reload value
  STCTRL = 0x00000007;                           // enable, core clock and interrupt arm
  EnableInterrupts();                            // interrupts enabled
  StartOS();                                     // start on the first task
}

// runs every ms
void Scheduler(void){
  // uint32_t max = NUMPRIORITY;
  runperiodicevents();

  for (int i = 0; i <= maxprio; i++) {
    tcbType *curr = PrioPt[i]; 
    tcbType *best = 0;
    do {
      if ((curr->blocked == 0) && (curr->sleep == 0)) {
        best = curr; 
      }
      curr = curr->next;
    } while (curr != PrioPt[i]);

    if (best != 0) { // priority thread is not sleeping
      RunPt = best; 
      PrioPt[i] = best;
      break;
    }
  }
  RunPt->hasRun += 1;
}

// ******** OS_InitSemaphore ************
// Initialize counting semaphore
// Inputs:  pointer to a semaphore
//          initial value of semaphore
// Outputs: none
void OS_InitSemaphore(int32_t *semaPt, int32_t value)
{

  DisableInterrupts();
  (*semaPt) = value;
  EnableInterrupts();
}

// ******** OS_Wait ************
// Decrement semaphore
// Lab2 spinlock (does not suspend while spinning)
// Lab3 block if less than zero
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Wait(int32_t *semaPt)
{

  DisableInterrupts();
  if ((*semaPt) <= 0)
  {
    RunPt->blocked = semaPt; // sets the blocked field to the blocking semaphore
    EnableInterrupts();
    OS_Suspend(); // spin here
    //DisableInterrupts();
  }
  (*semaPt) = (*semaPt) - 1;
  EnableInterrupts();
}

void OS_Suspend(void){ 

  STCURRENT = 0; // Reset time
  INTCTRL = 0x04000000; // trigger SysTick, but not reset time
  
} 

// ******** OS_Sleep ************
// place this thread into a dormant state
// input:  number of msec to sleep
// output: none
// OS_Sleep(0) implements cooperative multitasking
void OS_Sleep(uint32_t sleepTime){
// set sleep parameter in TCB
// suspend, stops running
    DisableInterrupts();
    RunPt->sleep = sleepTime;
    EnableInterrupts();
    OS_Suspend();
}

// ******** OS_Signal ************
// Increment semaphore
// Lab2 spinlock
// Lab3 wakeup blocked thread if appropriate
// Inputs:  pointer to a counting semaphore
// Outputs: none
void OS_Signal(int32_t *semaPt)
{

  DisableInterrupts();
  // tcbType *pt;
  if ((*semaPt) <= 0)
  {
    for (int i = 0; i < currentThreadCount; i++) {
      if (tcbs[i].blocked == semaPt) {
        tcbs[i].blocked = 0;
        break;
      }
    }
    // while (pt->blocked != semaPt){
    //   pt = pt->next;
    // }
    // // at this point we should have a thread that is not blocked
    // pt->blocked = 0;
  }
  *semaPt = *semaPt + 1;
  EnableInterrupts();
}

// ******** OS_MailBox_Init ************
// Initialize communication channel
// Producer is an event thread, consumer is a main thread
// Inputs:  none
// Outputs: none
void OS_MailBox_Init(void)
{
  // include data field and semaphore

  OS_InitSemaphore(&sema_mailbox, 0);
  mailbox_pos = 0;
}

// ******** OS_MailBox_Send ************
// Enter data into the MailBox, do not spin/block if full
// Use semaphore to synchronize with OS_MailBox_Recv
// Inputs:  data to be sent
// Outputs: none
// Errors: data lost if MailBox already has data
void OS_MailBox_Send(uint32_t data)
{

  DisableInterrupts();
  if (mailbox_pos < MAILBOX_SIZE)
  {
    mailbox_data[mailbox_pos++] = data;
    OS_Signal(&sema_mailbox);
  }
  EnableInterrupts();
  // else data will be lost
}

// ******** OS_MailBox_Recv ************
// retreive mail from the MailBox
// Use semaphore to synchronize with OS_MailBox_Send
// Lab 2 spin on semaphore if mailbox empty
// Lab 3 block on semaphore if mailbox empty
// Inputs:  none
// Outputs: data retreived
// Errors:  none
uint32_t OS_MailBox_Recv(void)
{

  OS_Wait(&sema_mailbox);
  return mailbox_data[--mailbox_pos];
}

// ******** OS_FIFO_Init ************
// Initialize FIFO.
// One event thread producer, one main thread consumer
// Inputs:  none
// Outputs: none
void OS_FIFO_Init(void)
{

  OS_InitSemaphore(&fifo_curr_size, 0);
  DisableInterrupts();
  // set up circular array
  for (int i = 0; i < FIFO_SIZE - 1; i++)
  {
    fifo_data[i].data = 0; // set values to zero
    fifo_data[i].next = &fifo_data[i+1];
  }
  fifo_data[FIFO_SIZE-1].next = &fifo_data[0];

  // At the beginning / when no items: front = back
  fifo_front = &fifo_data[0];
  fifo_back = &fifo_data[0];
  
  fifo_lost_data = 0;
  EnableInterrupts();
}

// ******** OS_FIFO_Put ************
// Put an entry in the FIFO.
// Exactly one event thread puts,
// do not block or spin if full
// Inputs:  data to be stored
// Outputs: 0 if successful, -1 if the FIFO is full
int OS_FIFO_Put(uint32_t data)
{

  DisableInterrupts();
  if (fifo_back->next == fifo_front) {
    fifo_lost_data++;
    
    EnableInterrupts();
    return -1;
  }

  fifo_back->data = data;
  fifo_back = fifo_back->next;
  
  OS_Signal(&fifo_curr_size);
  
  EnableInterrupts();
  return 0; // success
}

// ******** OS_FIFO_Get ************
// Get an entry from the FIFO.
// Exactly one main thread get,
// do block if empty
// Inputs:  none
// Outputs: data retrieved
uint32_t OS_FIFO_Get(void)
{
  OS_Wait(&fifo_curr_size);
  DisableInterrupts();
  uint32_t data;


  data = fifo_front->data;
  fifo_front = fifo_front->next;

  EnableInterrupts();
  return data;
}



// *****periodic events****************
int32_t *PeriodicSemaphore0;
uint32_t Period0; // time between signals
int32_t *PeriodicSemaphore1;
uint32_t Period1; // time between signals
void RealTimeEvents(void){int flag=0;
  static int32_t realCount = -10; // let all the threads execute once
  // Note to students: we had to let the system run for a time so all user threads ran at least one
  // before signalling the periodic tasks
  realCount++;
  if(realCount >= 0){
		if((realCount%Period0)==0){
      OS_Signal(PeriodicSemaphore0);
      flag = 1;
		}
    if((realCount%Period1)==0){
      OS_Signal(PeriodicSemaphore1);
      flag=1;
		}
    if(flag){
      OS_Suspend();
    }
  }
}
// ******** OS_PeriodTrigger0_Init ************
// Initialize periodic timer interrupt to signal 
// Inputs:  semaphore to signal
//          period in ms
// priority level at 0 (highest
// Outputs: none
void OS_PeriodTrigger0_Init(int32_t *semaPt, uint32_t period){
	PeriodicSemaphore0 = semaPt;
	Period0 = period;
	BSP_PeriodicTask_InitC(&RealTimeEvents,1000,0);
}
// ******** OS_PeriodTrigger1_Init ************
// Initialize periodic timer interrupt to signal 
// Inputs:  semaphore to signal
//          period in ms
// priority level at 0 (highest
// Outputs: none
void OS_PeriodTrigger1_Init(int32_t *semaPt, uint32_t period){
	PeriodicSemaphore1 = semaPt;
	Period1 = period;
	BSP_PeriodicTask_InitC(&RealTimeEvents,1000,0);
}

//****edge-triggered event************
int32_t *edgeSemaphore;
// ******** OS_EdgeTrigger_Init ************
// Initialize button2, PD7, to signal on a falling edge interrupt
// Inputs:  semaphore to signal
//          priority
// Outputs: none
void OS_EdgeTrigger_Init(int32_t *semaPt, uint8_t priority){
	edgeSemaphore = semaPt;

  SYSCTL_RCGCGPIO_R |= 0x00000008; // 1) activate clock for Port D
  while((SYSCTL_PRGPIO_R&0x08) == 0){};// allow time for clock to stabilize
  GPIO_PORTD_LOCK_R = 0x4C4F434B;  // 2) unlock GPIO Port D
  GPIO_PORTD_CR_R = 0xFF;          // allow changes to PD7-0
  GPIO_PORTD_AMSEL_R &= ~0x80;     // 3) disable analog on PD7
                                   // 4) configure PD7 as GPIO
  GPIO_PORTD_PCTL_R = (GPIO_PORTD_PCTL_R&0x0FFFFFFF)+0x00000000;
  GPIO_PORTD_DIR_R &= ~0x80;       // 5) make PD7 input
  GPIO_PORTD_AFSEL_R &= ~0x80;     // 6) disable alt funct on PD7
  GPIO_PORTD_PUR_R &= ~0x80;       // disable pull-up on PD7
  GPIO_PORTD_DEN_R |= 0x80;        // 7) enable digital I/O on PD7

  GPIO_PORTD_IS_R &= ~0x80;   // (d) PD7 are edge-sensitive 
  GPIO_PORTD_IBE_R &= ~0x80;  // PD7 are not both edges 
  GPIO_PORTD_IEV_R &= ~0x80;  // PD7 falling edge event 
  GPIO_PORTD_ICR_R = 0x80;    // (e) clear flags
  GPIO_PORTD_IM_R |= 0x80;    // (f) arm interrupt on PD7
  NVIC_PRI0_R = (NVIC_PRI0_R&0x1FFFFFFF)|(priority << 29); // (g) priority 
  NVIC_EN0_R = 0x00000008; // (h) enable interrupt 3 in NVIC
  
// Not implemented with BSP
// (d) PD is edge-sensitive 
//     PD is not both edges 
//     PD is falling edge event 
// (e) clear PD flag
// (f) arm interrupt on PD
// (g) priority on Port D edge trigger is NVIC_PRI0_R	31 � 29
// (h) enable is bit 3 in NVIC_EN0_R
}

// ******** OS_EdgeTrigger_Restart ************
// restart button1 to signal on a falling edge interrupt
// rearm interrupt
// Inputs:  none
// Outputs: none
void OS_EdgeTrigger_Restart(void){
  DisableInterrupts();
  GPIO_PORTD_IM_R |= 0x80;// rearm interrupt 3 in NVIC
  GPIO_PORTD_ICR_R = 0x80;// clear flag7
  
  EnableInterrupts();
}

void GPIOPortD_Handler(void){
	// step 1 acknowledge by clearing flag
  // step 2 signal semaphore (no need to run scheduler)
  // step 3 disarm interrupt to prevent bouncing to create multiple signals
    DisableInterrupts();
    if(GPIO_PORTD_RIS_R&0x80){  // poll PD7
      GPIO_PORTD_ICR_R = 0x80;  // acknowledge flag7
      GPIO_PORTD_IM_R &= ~0x80;
      OS_Signal(edgeSemaphore);          // signal occurred
    }
    EnableInterrupts();
}
