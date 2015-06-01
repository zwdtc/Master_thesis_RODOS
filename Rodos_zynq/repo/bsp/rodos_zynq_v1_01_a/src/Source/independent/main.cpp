
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
 * @file main.cc(v2.0) modified based on the v1.0.
 * @data 2014/10/30 7:45
 * @anther Wei Zhang
 * @Copyright 2014 DLR
 *
 ************************************************
 * @file main.cc(v1.0)
 * @date 2008/04/22 17:08
 * @author Lutz Dittrich, Sergio Montenegro
 *
 * Copyright 2008 DLR
 *
 * @brief system initialization and main function
 *
 */
#include "hw_specific.h"
#include "rodos.h"
#include "scheduler.h"
#include "xil_printf.h"
#include "xil_printf.h"
#include <stdio.h>
#include "xil_io.h"
#include "xil_mmu.h"
#include "xil_exception.h"
#include "xpseudo_asm.h"
#include "xscugic.h"

#define INTC            XScuGic
#define INTC_DEVICE_ID  XPAR_PS7_SCUGIC_0_DEVICE_ID
#define INTC_HANDLER    XScuGic_InterruptHandler

#define sev() __asm__("sev")
#define CPU1STARTADR 0xfffffff0

/**
 * declare a shared common block printer for multicore synchronisation
 */
cpuComBlock * cpuComBlockPtr = (cpuComBlock *)(0xFFFF0000);

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

static int  SetupIntrSystem(INTC *IntcInstancePtr);
INTC   IntcInstancePtr;

/** This shall be in topicInterface, but to do not link if we do not need...*/
List TopicInterface::topicList = 0;

bool isHostBigEndian = false; // will  be updated in main

union ByteSexDetector {
    char asBytes[4];
    long asLong;
};

void initSystem() {
    long num;

    ByteSexDetector byteSexDetector;
    byteSexDetector.asLong = 1;
    isHostBigEndian = byteSexDetector.asBytes[3] == 1; // if bigendian, lsb is in byte 3

    hwInitTime();

    /**** Initiators **********/
    xprintf("Calling Initiators and application initiators\n");
    ITERATE_LIST(Initiator, Initiator::initiatorList) {
        iter->init();
    }
    ITERATE_LIST(Application, Application::applicationList) {
        iter->init();
    }

    /**************** Middleware topics & Subscribers ******/

    xprintf("Distribute Subscribers to Topics\n");
    Subscriber* next;
    // we can not use INTERALTE_LIST because iter->getNext is set to 0 in the loop
    for (Subscriber* iter = (Subscriber*)Subscriber::subscriberList; iter!=0; iter = next) {

        next = (Subscriber*)iter->getNext();
        if (iter->isAGateway) {
            iter->append(defaultGatewayTopic.mySubscribers);
        } else {
            iter->append(iter->topicInterface.mySubscribers);
        }
    }

    if (TopicInterface::topicList != 0) {
        xprintf("List of Middleware Topics:\n");
        ITERATE_LIST(TopicInterface, TopicInterface::topicList) {
            xprintf(" %s  Id = %ld len = %ld.   -- Subscribers:\n", iter->getName(), iter->topicId, iter->msgLen);
            for (Subscriber* subs = (Subscriber*)iter->mySubscribers; subs !=0; subs = (Subscriber*)subs->getNext()) {
                xprintf("     %s\n", subs->getName());
            }
        }
    }
    /************* TimeEvents *************/

    xprintf("\nEvent servers:\n");
    num = TimeEvent::initAllElements();
    if (num != 0) xprintf(" %ld TimeEvent managers\n", num);

    Thread::initializeThreads();
    //(cpuComBlockPtr->startFirstTaskPtr) = (volatile void*)(Scheduler::idle);
}

static int SetupIntrSystem(INTC *IntcInstancePtr)
{
    int Status;
    XScuGic_Config *IntcConfig;
    /*
     * Initialize the interrupt controller driver so that it is ready to
     * use.
     */
    IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
    if (NULL == IntcConfig) {
        return XST_FAILURE;
    }

    Status = XScuGic_CfgInitialize(IntcInstancePtr, IntcConfig,
                    IntcConfig->CpuBaseAddress);
    if (Status != XST_SUCCESS) {
        return XST_FAILURE;
    }

    /*
     * Initialize the  exception table
     */
    Xil_ExceptionInit();

    /*
     * Register the interrupt controller handler with the exception table
     */
    Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
             (Xil_ExceptionHandler)INTC_HANDLER,
             IntcInstancePtr);

    /*
     * Enable non-critical exceptions
     */
    Xil_ExceptionEnable();

    return XST_SUCCESS;
}

void Secondary_CPU_Init(){
    int Status;

    // Initialize the SCU Interrupt Distributer (ICD)
    Status = SetupIntrSystem(&IntcInstancePtr);
    Xil_Out32(CPU1STARTADR, 0x02000000);
    //waits until write has finished
    dmb(); 

    //xprintf("CPU0: sending the SEV to wake up CPU1\n\r");
    sev();
    //waiting here until the notification from core1.     
    while(!cpuComBlockPtr->isSecondCoreStart);
}

#ifndef NO_RODOS_NAMESPACE
}
#endif

/***********************************/
//the core0 main code start here

int main (int argc, char** argv) {
 
    xprintf("RODOS %s OS Version \nLoaded Applications:\n", RODOS_VERSION_TEXT);
    Application::printApplications();
    //Disable cache on OCM
    // S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
    Xil_SetTlbAttributes(0xFFFF0000,0x14de2);  
    initSystem();
    /*
     * some host information functions, like, getIsHostBigEndian(), getHostCpuArch(), getHostBasisOS(), will 
     * be invoked by core1.
     */

    Timer::setInterval(PARAM_TIMER_INTERVAL);
    Timer::init(); // here begin the timer interrups
    // init and wake up core1
    Secondary_CPU_Init();
    //the scheduler will start working.
    Scheduler::idle();
    return 0;
}






