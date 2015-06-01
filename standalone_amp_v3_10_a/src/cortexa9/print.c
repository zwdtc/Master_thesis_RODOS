/* print.c -- print a string on the output device.
 *
 * Copyright (c) 1995 Cygnus Support
 *
 * The authors hereby grant permission to use, copy, modify, distribute,
 * and license this software and its documentation for any purpose, provided
 * that existing copyright notices are retained in all copies and that this
 * notice is included verbatim in any distributions. No written agreement,
 * license, or royalty fee is required for any of the authorized uses.
 * Modifications to this software may be copyrighted by their authors
 * and need not follow the licensing terms described here, provided that
 * the new terms are clearly indicated on the first page of each file where
 * they apply.
 *
 */

/*
 * print -- do a raw print of a string
 */
#include "xil_printf.h"
#include "xuartps_hw.h"
#include "xparameters.h"
 
void print(const char *ptr)
{
#if defined STDOUT_BASEADDRESS || USE_AMP==1
  while (*ptr) {
    //outbyte (*ptr++);
     XUartPs_SendByte(STDOUT_BASEADDRESS, *ptr++);
  }
#else
(void)ptr;
#endif
}
