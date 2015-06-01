
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
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 **
 ****************************************************************************/


/**
 * @file main.cc(v2.0) modified based on the v1.0.
 * @data 2014/10/30 7:43
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
#include "xparameters.h"
#include <stdio.h>
#include "xil_io.h"
#include "xil_mmu.h"
#include "xil_cache.h"
#include "xil_exception.h"
#include "xscugic.h"
#include "sleep.h"

/**
 * declare a shared common block printer for multicore synchronisation
 */
cpuComBlock * cpuComBlockPtr = (cpuComBlock *)(0xFFFF0000);

#ifndef NO_RODOS_NAMESPACE
namespace RODOS {
#endif

#define INTC            XScuGic
#define INTC_HANDLER    XScuGic_InterruptHandler
#define INTC_DEVICE_ID  XPAR_PS7_SCUGIC_0_DEVICE_ID
#define PL_IRQ_ID       XPS_IRQ_INT_ID
#define IRQ_PCORE_GEN_BASE  0x78600000



/** This shall be in topicInterface, but to do not link if we do not need...*/
List TopicInterface::topicList = 0;

/**
 * This typedef contains configuration information for the device driver.
 */
typedef struct {
    u16 DeviceId;       /**< Unique ID of device */
    u32 BaseAddress;    /**< Base address of the device */
} Pl_Config;

/**
 * The driver instance data. The user is required to allocate a
 * variable of this type.
 * A pointer to a variable of this type is then passed to the driver API
 * functions.
 */
typedef struct {
    Pl_Config Config;   /**< Hardware Configuration */
    u32 IsReady;        /**< Device is initialized and ready */
    u32 IsStarted;      /**< Device is running */
} XPlIrq;


void        Xil_L1DCacheFlush(void);
static int  SetupIntrSystem(INTC *IntcInstancePtr, XPlIrq *PeriphInstancePtr, u16 IntrId);
static void DisableIntrSystem(INTC *IntcInstancePtr, u16 IntrId);
static void PlIntrHandler(void *CallBackRef);

extern u32 MMUTable;
/**
 * Assign the driver structures for the interrupt controller and
 * PL Core interrupt source
 */
INTC   IntcInstancePtr;
XPlIrq PlIrqInstancePtr;

// Global for IRQ communication to main()
int irq_count;

bool isHostBigEndian = false; // will  be updated in main

union ByteSexDetector {
    char asBytes[4];
    long asLong;
};

/********************************/
void initSystem() {

    Thread::initializeThreads();
}
#ifndef NO_RODOS_NAMESPACE
}
#endif

/***********************************/
/**
 * The core1 main() function start here.
 */
int main () {

    //Disable cache on OCM
    //S=b1 TEX=b100 AP=b11, Domain=b1111, C=b0, B=b0
    Xil_SetTlbAttributes(0xFFFF0000,0x14de2);
    //enable the signal to notify core0. 
    cpuComBlockPtr->isSecondCoreStart = 1;
    initSystem();
    xprintf("BigEndianity = %d, cpu-Arc = %s, Basis-Os = %s, Cpu-Speed (K-Loops/sec) = %ld yeildtim (ns) %ld\n",
            getIsHostBigEndian(), getHostCpuArch(), getHostBasisOS(), getSpeedKiloLoopsPerSecond(), getYieldTimeOverhead());
    //xprintf("Node Number: HEX: %lx Dec: %ld\n",getCurrentCPU(),getCurrentCPU());
    xprintf("-----------------------------------------------------\n");
    Timer::setInterval(PARAM_TIMER_INTERVAL);
    Timer::init(); // here begin the timer interrups
    xprintf("--------------- application running ------------\n");
    //the core1 scheduler start here.
    Scheduler::idle();

    return 0;
}






