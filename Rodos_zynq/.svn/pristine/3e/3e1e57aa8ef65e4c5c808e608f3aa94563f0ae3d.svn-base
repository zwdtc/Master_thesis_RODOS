/*
* @file hw_specific.cpp for cortex A9 Zynq MicroZed.
* @data 2014/07/07
* @author Wei Zhang
* @Modified based on the previous version below
* @Copyright 2014 DLR
**********************************************************/
/*
 * This file contains source code copyrighted by David Welch distributed under the terms of the MIT license.
 */
/*orignial version below
 *
 * @file hw_specific.cpp
 * @date 2013/06/01
 * @author Johannes Freitag, Sergio Montenegro, David Welch
 *
 * Copyright 2013 DLR and University Wuerzburg
 */

#include <rodos.h>
#include "params.h"
#include "hw_specific.h"

typedef struct s_context
	{
		/* @{ The context of the thread. */
		long CPSR; 	/* Current Status Register */

		/* @{ Argument, result or scratch register, named a1-a4 */
		long r0;
		long r1;
		long r2;
		long r3;
		/* @}} */

		/* @{ Variable-register, named v1-v8 */
		long r4;
		long r5;
		long r6;
		long r7;
		long r8;
		long r9;	/* SB - Static base register */
		long r10;
		long r11;
		/* @} */
		long r12;	/* IP - Intra-Procedure-call scratch register */
		long SP;	/* Stack Pointer */
		long LR;	/* Link Register - store the return address */
		long PC;	/* Program Counter */
	} TContext;

volatile long *contextT;
//for testing, copy from the old cortexa9.
TContext context_to_be_saved;
unsigned int context_pointer= 0xDEADBEEF;
//////////////////////////////////////////


#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif
	
	
/*********************************************************************************************/
/* CONTEXT SWITCH AND INTERRUPT HANDLING */
/*********************************************************************************************/

extern int64_t timeToTryAgainToSchedule;
extern bool isSchedulingEnabled;
volatile long taskRunning = 0;

#define CONTEXT_SAVE  	__asm volatile ( \
	/* push r0 to use the register*/ \
	"push {r0}            \n\t"  \
	/* set r0 to point to the task stack pointer */ \
	"stmdb sp, {sp}^      \n\t" /* ^ means get the system mode SP value */  \
	"NOP                  \n\t" \
	"sub sp, sp, #4       \n\t" \
	"pop {r0}             \n\t" \
	/* push the return address onto the task stack */ \
	"stmdb r0!, {lr}      \n\t" \
	/* now we have saved lr we can use it instead of r0 */ \
	"mov lr, r0           \n\t" \
	/* pop r0 so we can save it onto the task stack */ \
	"ldmia sp!, {r0}      \n\t" \
	/* push all the system mode registers onto the task stack */ \
	"stmdb lr,{r0-r14}^   \n\t" \
	"NOP                  \n\t" \
	"sub lr, lr, #60      \n\t" \
	/* push all the fpu mode registers onto the task stack */ \
	/*"vstmdb lr!,{s0-s31}   \n\t"*/ \
	/* Push the SPSR onto the task stack. */ \
	"MRS R0, SPSR         \n\t" \
	"STMDB LR!, {R0}      \n\t" );

#define CONTEXT_LOAD  	__asm volatile ( \
	/* get the top of stack for the task */  \
	"ldr r0, =contextT     \n\t" \
	/* move the task stack pointer into the Link Register */  \
	"ldr lr, [r0]          \n\t" \
	"ldr r0, [r0]          \n\t" \
	/* Get the SPSR from the stack. */  \
	"LDMFD LR!, {R0}       \n\t" \
	"MSR SPSR_cxsf, R0     \n\t" \
	/* restore all fpu registers */  \
	/*"vldmia lr!, {s0-s31}   \n\t" */ \
	/* restore all system mode registers for the task */  \
	"ldmfd lr, {r0-r14}^   \n\t" \
	"NOP                   \n\t" \
	/* restore the return address */  \
	"ldr lr, [lr, #+60]    \n\t" \
	/* return to next instruction in new task and leave irq */  \
	"SUBS	PC, LR, #4		\n\t"	\
	"NOP					\n\t"	\
	"NOP					\n\t"	);

extern "C" {

/*
 * Handles the hardware interrupt after saving the context and determines who has raised the interrupt
 */
void handleInterrupt(long int* context) {
	contextT = context;

}
/**
 * pushes the context to the old task stack - handles hardware interrupts - pops the context of the new task from the new task stack
 */
__attribute__ (( naked ))
void c_irq_handler(void) {

	//lr has to be adjusted in hardware irq
	__asm volatile ( "sub lr,lr,#4         \n\t" );

	// save old context
	CONTEXT_SAVE

	//call handler and sheduler
	__asm volatile (
			"mov r0, lr           \n\t"
			"bl handleInterrupt             \n\t" );

	// restore context of new task
	CONTEXT_LOAD

}

/**
 * pushes the context to the old task stack - schedules - pops the context of the new task from the new task stack
 *
 * contextSwitch is called from the ISR of the software interrupt
 * a software interrupt is raised in yield and in the irq_handler for preemtion
 * in swi the interrupts are disabled
 */
__attribute__ (( naked ))
void contextSwitch(void) {

	__asm volatile ( "ADD	LR, LR, #4" );
	// save old context
	CONTEXT_SAVE

    // call scheduler with top of task stack in r0
	__asm volatile (
			"mov r0, lr            \n\t"
			"bl schedulerWrapper   \n\t");

	// restore context of new task
	CONTEXT_LOAD

}
}			//end extern "C"
long* hwInitContext(long* stack, void* object) {

	long* tos = stack;		/* Top Of Stack */
	//d*tos = 0xdeaddead;		/* debug */
	TContext *ctx = (TContext*) (tos - sizeof(TContext)/sizeof(long));

	ctx->r0 = (long) object;
	ctx->r1 = 0x01010101;
	ctx->r2 = 0x02020202;
	ctx->r3 = 0x03030303;
	ctx->r4 = 0x04040404;
	ctx->r5 = 0x05050505;
	ctx->r6 = 0x06060606;
	ctx->r7 = 0x07070707;
	ctx->r8 = 0x08080808;
	ctx->r9 = 0x09090909;
	ctx->r10 = 0x10101010;
	ctx->r11 = 0x11111111;
	ctx->r12 = 0x12121212;
	ctx->SP = ((long) ctx)-8;
	ctx->LR = 0;
	ctx->PC = (long) threadStartupWrapper;	/* this method is defined in /bare-meta-generic/thread_on_hw.cpp */

	/* ctx->CPSR = CPSR */
	asm volatile (
		"mrs  %0, CPSR     \n\t"
		: "=r"(ctx->CPSR)       // Output registers
	);


	return (long*) ctx;
}

/** switches to the context of the initial idle thread
 * called in main() -> Scheduler::idle();
 * basically the same as the "restore context of new task" part from c_swi_handler
 */
__attribute__ (( naked ))
void startIdleThread() {
	//CONTEXT_LOAD
}


#ifndef NO_RODOS_NAMESPACE
}
#endif

