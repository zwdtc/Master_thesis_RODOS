
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
* @ wait the previous data be received and then send a new data to core0
* @ param *data, pass the address of data to the common block space.
* @
* @ return void
*/
void waitAndSendDataToCore0(void * data){
	while(cpuComBlockPtr->isDataFromCore1Ready);
    cpuComBlockPtr->dataFromCore1 = data;
    memory_barrier();
    cpuComBlockPtr->isDataFromCore1Ready = 1;
}
/**
* @ dont wait and send a new data to core0 directly
* @ param *data, pass the address of data to the common block space.
* @
* @ return void
*/
void dontWaitAndSendDataToCore0(void * data){
    cpuComBlockPtr->dataFromCore1 = data;
    memory_barrier();
    cpuComBlockPtr->isDataFromCore1Ready = 1;
}
/**
* @ receive a data from core0, wait a certain amount of time if the data is not ready
* @ param waiting_time, the max time for waiting expected data.the unit is nanoseconds
* @
* @ return cpuComBlockPtr->dataFromCore0 if the data is ready, otherwise, return NULL.
*/
volatile void* revDataFromCore0(int64_t waiting_time){
	int64_t timepoint_old = NOW();
    while(!cpuComBlockPtr->isDataFromCore0Ready && NOW() < (timepoint_old + waiting_time/3));
    if(cpuComBlockPtr->isDataFromCore1Ready)
    {
        	cpuComBlockPtr->isDataFromCore0Ready = 0;
        	return cpuComBlockPtr->dataFromCore0;
       }
    else
    	return NULL;
}

class LowPriorityThread : public Thread {
public:

  LowPriorityThread() : Thread("LowPriorityThread", 10) { }
  void init() {
	  xprintf("Low priority Thread");
	  }
  void run(){
	  int TEMP = 10;
	  int *t = &TEMP;
	  while(1){
		  waitAndSendDataToCore0(t);
		   //t = (int*)revDataFromCore0();
		  //xprintf("Core1: the received data is %d\n",*t);
		  *t += 1;
		  //sendDataToCore1(t);
		  suspendCallerUntil(NOW() + 3*SECONDS);
	  }
  }
} lowproritythread;

