
/*********************************************************** Copyright
 **
 ** Copyright (c) 2014, German Aerospace Center (DLR)
 ** All rights reserved.
 **
 ** Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **
 ** 1 Redistributions of source code must retain the above copyright
 **   notice, this list of conditions and the following disclaimer.
 **
 ** 2 Redistributions in binary form must reproduce the above copyright
 **   notice, this list of conditions and the following disclaimer in the
 **   documentation and/or other materials provided with the
 **   distribution.
 **
 ** 3 Neither the name of the German Aerospace Center nor the names of
 **   its contributors may be used to endorse or promote products derived
 **   from this software without specific prior written permission.
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/

/*
* @file the top level user defined application
* @data 2014/10/31
* @author Wei Zhang
* @
* @Copyright 2014 DLR
**********************************************************/

#include "rodos.h"
#include "xil_printf.h"

#define memory_barrier()  __asm__ __volatile__("DMB")

extern cpuComBlock * cpuComBlockPtr;
/**
* @ wait the previous data be received and then send a new data to core1
* @ param *data, pass the address of data to the common block space.
* @
* @ return void
*/
void waitAndSendDataToCore1(void * data){
	while(cpuComBlockPtr->isDataFromCore0Ready);
    cpuComBlockPtr->dataFromCore0 = data;
    memory_barrier();
    cpuComBlockPtr->isDataFromCore0Ready = 1;
}
/**
* @ dont wait and send a new data to core1 directly
* @ param *data, pass the address of data to the common block space.
* @
* @ return void
*/
void dontWaitAndSendDataToCore1(void * data){
    cpuComBlockPtr->dataFromCore0 = data;
    memory_barrier();
    cpuComBlockPtr->isDataFromCore0Ready = 1;
}
/**
* @ receive a data from core1, wait a certain amount of time if the data is not ready
* @ param waiting_time, the max time for waiting expected data. the unit is nanoseconds
* @
* @ return cpuComBlockPtr->dataFromCore1 if the data is ready, otherwise, return NULL.
*/
volatile void* revDataFromCore1(int64_t waiting_time){
	int64_t timepoint_old = NOW();
    while(!cpuComBlockPtr->isDataFromCore1Ready && NOW() < (timepoint_old + waiting_time/3));
    if(cpuComBlockPtr->isDataFromCore1Ready)
    {
        	cpuComBlockPtr->isDataFromCore1Ready = 0;
        	return cpuComBlockPtr->dataFromCore1;
       }
    else
    	return NULL;
}

/*class HighPriorityThread : public Thread {

public:

  HighPriorityThread() : Thread("HighPriorityThread", 25) { }
  void init() {
	  xprintf("High priority Thread");
	  }
  void run(){
	  xprintf("high priority thread: the first RODOS program in Xilinx Zedboard!\n");
  }
} highproritythread;
*/

class MiddlePriorityThread : public Thread {

public:

  MiddlePriorityThread() : Thread("MiddlePriorityThread", 20) { }
  void init() {
	  xprintf("Middle priority Thread");
	  }
  void run(){
	  while(1){
		  int* t = (int*)revDataFromCore1(15000);
		  if(*t < -1)
			  xprintf("Core0: Waiting time is up, the data is not ready!\n");
		  else
			  xprintf("Core0: the received data is %d\n",*t);

		  suspendCallerUntil(NOW() + 1*SECONDS);
	  }
  }
} Middleproritythread;




