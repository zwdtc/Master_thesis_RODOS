/*********************************************************** Copyright
 **
 ** Copyright (c) 2008, German Aerospace Center (DLR)
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
 ** LIMITED TO, PROCUREMENT OF SUBSTITuTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ***************************************************************************/


/**
 * @file multicore.h 
 * @data 2014/10/30 7:45
 * @anther Wei Zhang
 * @Copyright 2014 DLR
 * @brief multicore interface

 ************************************************/
#ifndef MULTICORE_H_
#define MULTICORE_H_

/**
 * the definition of cpu shared common block memory struct
 */
typedef struct {
	
	volatile bool isDataFromCore0Ready;
	volatile bool isDataFromCore1Ready; 
	volatile bool isSecondCoreStart;
	volatile void *dataFromCore0;
	volatile void *dataFromCore1;
	//for synchroinzation testing below
	volatile int syncState[2];
	volatile int RunCount[2];
	volatile int sharedVar;
	volatile int failedSync;
	//for concurency testing below
	volatile int calc1Value;
	volatile int calc2Value;
} cpuComBlock;


#endif /* MULTICORE_H_ */
