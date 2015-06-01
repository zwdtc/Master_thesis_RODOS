

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
 * @file rodos-apis.h
 * @date 2008/06/06 15:57
 * @author Sergio Montenero
 *
 * Copyright 2008 DLR
 *
 */

#pragma once

#define RODOS_VERSION 116
#define RODOS_VERSION_TEXT "RODOS-116"

#include <stdint.h>

#include "hw_predefines.h"

//#include "string_pico.h"
//change system call to original syscall
#include <string.h>
#include "debug.h"

#include "hostinfos.h"  /* architecture dependent */
#include "params.h"     /* architecture dependent */
#include "hw_stdlib.h"  /* architecture dependent: for printf, memcpy */
#include "hw_thread_defs.h"   /* architecture dependent defines, etg atomartiy */

#include "misc.h"
#include "checksumes.h"
#include "bytesex.h"

#include "listelement.h"

#include "initiator.h"

#include "timemodel.h"
#include "application.h"
#include "reserved_application_ids.h"
#include "thread.h"

#include "putter.h"
#include "fifo.h"
#include "commbuffer.h"

#include "semaphore.h"
#include "timeevent.h"
#include "timepoints.h"

#include "topic.h"
#include "reserved_topic_ids.h"
#include "subscriber.h"

#include "udp.h"

#include "yprintf.h"
#include "hw_postdefines.h"

#include "gateway.h"
#include "multicore.h"