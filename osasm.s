;/*****************************************************************************/
; OSasm.s: low-level OS commands, written in assembly                       */
; Runs on LM4F120/TM4C123/MSP432
; Lab 2 starter file
; February 10, 2016
;


        AREA |.text|, CODE, READONLY, ALIGN=2
        THUMB
        REQUIRE8
        PRESERVE8

        EXTERN  RunPt            ; currently running thread
        EXPORT  StartOS
        EXPORT  SysTick_Handler
        IMPORT  Scheduler


SysTick_Handler                ; 1) Saves R0-R3,R12,LR,PC,PSR
    CPSID   I                  ; 2) Prevent interrupt during switch
    PUSH {R4-R11}              ; 3) Save Registers R4-11
    LDR R0, =RunPt             ; 4) R0 = &RunPt for old thread
    LDR R1, [R0]               ; 5) R1 = RunPt
    STR SP, [R1]               ; 6) Save SP into TCB // tcb.sp = SP
    PUSH {R0, LR}
    BL Scheduler               ; 7) Run our scheduler
    POP {R0, LR}
    LDR R0, =RunPt
    LDR R1, [R0]               ; 8) R1 = RunPt for new thread
    LDR SP, [R1]               ; 9) SP = new thread SP; SP = RunPt->sp;
    POP {R4-R11}
    CPSIE   I                  ; 10) tasks run with interrupts enabled
    BX      LR                 ; 11) restore R0-R3,R12,LR,PC,PSR

StartOS
    LDR R0, =RunPt
    LDR R1, [R0]               ; 8) R1 = RunPt for new thread
    LDR SP, [R1]               ; 9) SP = new thread SP; SP = RunPt->sp;
    POP {R4-R11}
    POP {R0-R3}
    POP {R12}
    ADD SP, SP, #4             ; Discard 
    POP {LR}
    ADD SP,SP,#4               ; Discard
    CPSIE   I                  ; Enable interrupts at processor level
    BX      LR                 ; start first thread

    ALIGN
    END
