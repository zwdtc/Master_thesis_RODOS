
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
* @file sync testing, modified based on the James Mistry's code
* @data 2014/10/31
* @author Wei Zhang, james Mistry
* @
* @Copyright 2014 DLR
**********************************************************/

#include "rodos.h"
#include "xil_printf.h"
extern cpuComBlock * cpuComBlockPtr;

	static volatile int32_t interTaskMessage = 0;
	static volatile int32_t isSecondTaskAlive = 0;
	static volatile int32_t firstSpinCount = -50;
	static volatile int32_t secondSpinCount = -50;

	/* Constants for the sync test tasks */
	static const int32_t SYNC_RUNS = 25000;
	static const int32_t SYNCHRONISER_UNKNOWN = 0;
	static const int32_t SYNCHRONISER_ALIVE = 1;
	static const int32_t SYNCHRONISER_FINISHED = 2;
	static const int32_t CHECK_TASK_DELAY_PERIOD = 100;
	static const int32_t NO_CHECKIN_THRESHOLD = 5000;

	class monitorThread : public Thread {
	public:

	monitorThread() : Thread("monitorThread", 10) { }
	void init() {
		xprintf("monitor thread for sync");
	  }
	void run(){
		int32_t noCheckinSince = 0;
		int32_t currentRunCount[2] = {0, 0};
		int32_t lastRunCount[2] = {0, 0};
		while(1)
		{
			currentRunCount[0] = cpuComBlockPtr->RunCount[0];
			currentRunCount[1] = cpuComBlockPtr->RunCount[1];

			for (int32_t i=0;i<2;i++) {

				if (currentRunCount[i] > lastRunCount[i]) {

					if ((currentRunCount[i] - lastRunCount[i]) >= 100) {

						if (i == 0) xprintf("Thread 1: Still alive (%d runs complete).\n", cpuComBlockPtr->RunCount[i]);
						else if (i == 1) xprintf("Thread 2: Still alive (%d runs complete).\n", cpuComBlockPtr->RunCount[i]);

						lastRunCount[i] = currentRunCount[i];

						}
					}
				}

				if ((cpuComBlockPtr->syncState[0] == SYNCHRONISER_UNKNOWN) || (cpuComBlockPtr->syncState[1] == SYNCHRONISER_UNKNOWN)) {

					noCheckinSince++;

				}
				else if (((cpuComBlockPtr->syncState[0] == SYNCHRONISER_ALIVE) && (cpuComBlockPtr->syncState[1] == SYNCHRONISER_ALIVE))
									|| ((cpuComBlockPtr->syncState[0] == SYNCHRONISER_FINISHED) && (cpuComBlockPtr->syncState[1] == SYNCHRONISER_ALIVE))
									|| ((cpuComBlockPtr->syncState[1] == SYNCHRONISER_FINISHED) && (cpuComBlockPtr->syncState[0] == SYNCHRONISER_ALIVE))) {

					noCheckinSince = 0;
					cpuComBlockPtr->syncState[0] = SYNCHRONISER_UNKNOWN;
					cpuComBlockPtr->syncState[1] = SYNCHRONISER_UNKNOWN;

				}
				else if (((cpuComBlockPtr->syncState[0]) == SYNCHRONISER_FINISHED) && ((cpuComBlockPtr->syncState[1]) == SYNCHRONISER_FINISHED)) {

					noCheckinSince = 0;
					if (cpuComBlockPtr->failedSync == 0) xprintf("Tasks finished without deadlock or synchronisation failure.\n");
						else if (cpuComBlockPtr->failedSync == 1) xprintf("!!! Tasks finished without deadlock, but there was a synchronisation failure !!!");
						while(1);

					}

				if (noCheckinSince >= NO_CHECKIN_THRESHOLD) {
						xprintf("!!! Possible deadlock detected !!!\n");
					}

				if (cpuComBlockPtr->failedSync != 0) {
						xprintf("!!! Synchronisation failure detected !!!\n");
					}
					//vTaskDelay(((systemTaskParameters *)(pvParameters))->taskHandle, CHECK_TASK_DELAY_PERIOD);
				suspendCallerUntil(NOW() + 500*MILLISECONDS);
				}
			}
	} monitorthread;





