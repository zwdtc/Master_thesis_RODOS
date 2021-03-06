
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
* @file concurency performance testing, modified based on the James Mistry's code
* @data 2014/10/31
* @author Wei Zhang, james Mistry
* @
* @Copyright 2014 DLR
**********************************************************/

#include "rodos.h"
#include "xil_printf.h"

extern cpuComBlock * cpuComBlockPtr;

class MonitorThread : public Thread {
	public:

	MonitorThread() : Thread("monitorthread", 10) { }
	void init() {
		xprintf("monitor thread");
	  }
	void run(){

	while(1) {

		if ((cpuComBlockPtr->calc1Value == 10000000) && (cpuComBlockPtr->calc2Value == 10000000)) {

			//portTickType ticks = xTaskGetTickCount();
			//xprintf("Calculations complete in %lu ticks!\n", ticks);
			xprintf("Time lapses  : %d.%ds   \n", (int)(NOW()/330000000), (int)((NOW()/330)%1000000));
			while(1);

		} else {

			suspendCallerUntil(NOW() + 100*MILLISECONDS);
		}

	   }
	}
}monitortethread;

class CalculateThread_one : public Thread {
	public:

	CalculateThread_one() : Thread("calculate thread one", 10) { }
	void init() {
		xprintf("calculate thread one");
	  }
	void run(){

	//while(1) {

		cpuComBlockPtr->calc1Value = 0;
		for (int32_t i=0;i<10000000;i++) {

			cpuComBlockPtr->calc2Value++;

		}

		while(1);

	//}
  }
}calcuthread_one;



