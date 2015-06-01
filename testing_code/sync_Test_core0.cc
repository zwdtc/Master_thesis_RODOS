
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

	/* Constants for the sync test tasks */
	static const int32_t SYNC_RUNS = 25000;
	static const int32_t SYNCHRONISER_UNKNOWN = 0;
	static const int32_t SYNCHRONISER_ALIVE = 1;
	static const int32_t SYNCHRONISER_FINISHED = 2;
	static const int32_t CHECK_TASK_DELAY_PERIOD = 100;
	static const int32_t NO_CHECKIN_THRESHOLD = 5000;

	Semaphore s1;

	class synchTesting_one : public Thread {
	public:

	  synchTesting_one() : Thread("synchTesting1", 10) { }
	  void init() {
		  xprintf("sync testing thread one");
		  }
	  void run(){
		  cpuComBlockPtr->RunCount[0] = 0; // Initialise the run count
		  cpuComBlockPtr->syncState[0] = 0; // Initialise the check-in flag

		  //secondSpinCount = 0; // For debugging
		  while(1)
		  {
			  //secondSpinCount++; // Helps with debugging
			  if (cpuComBlockPtr->RunCount[0] == SYNC_RUNS)
		  	  {
				  //while(1)
				  //{
		  			  	//xprintf("Task1 now runs 25000!\n");
		  			  	cpuComBlockPtr->syncState[0] = SYNCHRONISER_FINISHED;
		  			  	//while(1);
		  			    suspendCallerUntil(NOW() + 500*MILLISECONDS);
		  		  //}
		  	  }
			  else
			  {
				  cpuComBlockPtr->syncState[0] = SYNCHRONISER_ALIVE;
			  }
			  s1.enter();
			  // If the shared variable is not a multiple of 1000, synchronisation has failed somewhere
			  if (cpuComBlockPtr->sharedVar % 1000 != 0)
				  cpuComBlockPtr->failedSync = 1;

			  	  cpuComBlockPtr->syncState[0] = SYNCHRONISER_ALIVE;
			  for (int32_t i=0;i<1000;i++) {
		  			 cpuComBlockPtr->sharedVar++;
		  		}

		  		//vTaskReleaseNamedMutex(((systemTaskParameters *)(pvParameters))->taskHandle, SYNC_TEST_MUTEX);
			  	s1.leave();
			  	cpuComBlockPtr->RunCount[0]++;
		  		}
	  }
	} synchtesting_one;

	class synchTesting_two : public Thread {
		public:

		  synchTesting_two() : Thread("synchTesting2", 10) { }
		  void init() {
			  xprintf("sync testing thread two");
			  }
		  void run(){
			  cpuComBlockPtr->RunCount[1] = 0; // Initialise the run count
			  //int32_t * checkIn = &(cpuComBlockPtr->usSynchroniserCheckin[0]);
			  cpuComBlockPtr->syncState[1] = 0; // Initialise the check-in flag

			  //secondSpinCount = 0; // For debugging
			  while(1)
			  {

				  //secondSpinCount++; // Helps with debugging
			  	  if (cpuComBlockPtr->RunCount[1] == SYNC_RUNS)
			  	  {
			  			//while(1) {
			  				//xprintf("Task2 now runs 25000!\n");
			  				cpuComBlockPtr->syncState[1] = SYNCHRONISER_FINISHED;
			  				//}
			  				//while(1);
			  				suspendCallerUntil(NOW() + 500*MILLISECONDS);
			  	  }
			  	  else
			  	  {
			  			cpuComBlockPtr->syncState[1] = SYNCHRONISER_ALIVE;
			  	  }

			  	  s1.enter();
			  	  // If the shared variable is not a multiple of 1000, synchronisation has failed somewhere
			  	  if (cpuComBlockPtr->sharedVar % 1000 != 0)
			  		  cpuComBlockPtr->failedSync = 1;

			  	  cpuComBlockPtr->syncState[1] = SYNCHRONISER_ALIVE;
			  	  for (int32_t i=0;i<1000;i++) {
			  			cpuComBlockPtr->sharedVar++;
			  		}

			  			//vTaskReleaseNamedMutex(((systemTaskParameters *)(pvParameters))->taskHandle, SYNC_TEST_MUTEX);
			  	  s1.leave();
			  	  cpuComBlockPtr->RunCount[1]++;
			  	}
		  }
		} synchtesting_two ;

