/******************************************************************************
*
* (c) Copyright 2009 Xilinx, Inc. All rights reserved.
*
* This file contains confidential and proprietary information of Xilinx, Inc.
* and is protected under U.S. and international copyright and other
* intellectual property laws.
*
* DISCLAIMER
* This disclaimer is not a license and does not grant any rights to the
* materials distributed herewith. Except as otherwise provided in a valid
* license issued to you by Xilinx, and to the maximum extent permitted by
* applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
* FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
* IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
* MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
* and (2) Xilinx shall not be liable (whether in contract or tort, including
* negligence, or under any other theory of liability) for any loss or damage
* of any kind or nature related to, arising under or in connection with these
* materials, including for any direct, or any indirect, special, incidental,
* or consequential loss or damage (including loss of data, profits, goodwill,
* or any type of loss or damage suffered as a result of any action brought by
* a third party) even if such damage or loss was reasonably foreseeable or
* Xilinx had been advised of the possibility of the same.
*
* CRITICAL APPLICATIONS
* Xilinx products are not designed or intended to be fail-safe, or for use in
* any application requiring fail-safe performance, such as life-support or
* safety devices or systems, Class III medical devices, nuclear facilities,
* applications related to the deployment of airbags, or any other applications
* that could lead to death, personal injury, or severe property or
* environmental damage (individually and collectively, "Critical
* Applications"). Customer assumes the sole risk and liability of any use of
* Xilinx products in Critical Applications, subject only to applicable laws
* and regulations governing limitations on product liability.
*
* THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
* AT ALL TIMES.
*
******************************************************************************/
/*****************************************************************************/
/**
*
* @file xil_io.h
*
* This file contains the interface for the XIo component, which encapsulates
* the Input/Output functions for processors that do not require any special
* I/O handling.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 3.00a hbm  07/28/09 Initial release
* 3.00a hbm  07/21/10 Added Xil_EndianSwap32/16, Xil_Htonl/s, Xil_Ntohl/s
*
* </pre>
*
* @note
*
* This file may contain architecture-dependent items.
*
******************************************************************************/
#ifndef XIL_IO_H
#define XIL_IO_H

#include "xil_types.h"

#ifdef __cplusplus
extern "C" {
#endif

extern u8 Xil_In8(u32 Addr);
extern u16 Xil_In16(u32 Addr);
extern u32 Xil_In32(u32 Addr);
extern void Xil_Out8(u32 Addr, u8 Value);
extern void Xil_Out16(u32 Addr, u16 Value);
extern void Xil_Out32(u32 Addr, u32 Value);

extern u16 Xil_EndianSwap16(u16 Data);
extern u32 Xil_EndianSwap32(u32 Data);

extern u16 Xil_In16LE(u32 Addr);
extern u32 Xil_In32LE(u32 Addr);
extern void Xil_Out16LE(u32 Addr, u16 Value);
extern void Xil_Out32LE(u32 Addr, u32 Value);

/***************** Macros (Inline Functions) Definitions *********************/

/* The following macros are specific to the PowerPC family. They perform
 * isync and eieio operations such that instruction execution and I/O
 * operations are synced correctly.
 * These macros are not necessarily portable across compilers since they
 * use inline assembly.
 */
#if defined __GNUC__
#  define INST_SYNC	__asm__ __volatile__("isync\n")
#  define DATA_SYNC	__asm__ __volatile__("eieio\n")
#elif defined __DCC__
#  define INST_SYNC	asm volatile("isync\n")
#  define DATA_SYNC	asm volatile("eieio\n")
#else
#  define INST_SYNC
#  define DATA_SYNC
#endif /* __GNUC__ */

/**
*
* Perform an big-endian input operation for a 16-bit memory location
* by reading from the specified address and returning the value read from
* that address.
*
* @param	Addr contains the address to perform the input operation at.
*
* @return	The value read from the specified input address with the
*		proper endianness. The return value has the same endianness
*		as that of the processor, i.e. if the processor is
*		little-engian, the return value is the byte-swapped value read
*		from the address.
*
* @note		None.
*
******************************************************************************/
#define Xil_In16BE(Addr) Xil_In16(Addr)

/**
*
* Perform a big-endian input operation for a 32-bit memory location
* by reading from the specified address and returning the value read from
* that address.
*
* @param	Addr contains the address to perform the input operation at.
*
* @return	The value read from the specified input address with the
*		proper endianness. The return value has the same endianness
*		as that of the processor, i.e. if the processor is
*		little-engian, the return value is the byte-swapped value read
*		from the address.
*
*
* @note		None.
*
******************************************************************************/
#define Xil_In32BE(Addr) Xil_In32(Addr)

/*****************************************************************************/
/**
*
* Perform a big-endian output operation for a 16-bit memory location
* by writing the specified value to the specified address.
*
* @param	Addr contains the address to perform the output operation at.
* @param	Value contains the value to be output at the specified address.
*		The value has the same endianness as that of the processor.
*		If the processor is little-endian, the byte-swapped value is
*		written to the address.
*
*
* @return	None
*
* @note		None.
*
******************************************************************************/
#define Xil_Out16BE(Addr, Value) Xil_Out16(Addr, Value)

/*****************************************************************************/
/**
*
* Perform a big-endian output operation for a 32-bit memory location
* by writing the specified value to the specified address.
*
* @param	Addr contains the address to perform the output operation at.
* @param	Value contains the Value to be output at the specified address.
*		The value has the same endianness as that of the processor.
*		If the processor is little-endian, the byte-swapped value is
*		written to the address.
*
* @return	None
*
* @note		None.
*
******************************************************************************/
#define Xil_Out32BE(Addr, Value) Xil_Out32(Addr, Value)

/*****************************************************************************/
/**
*
* Convert a 32-bit number from host byte order to network byte order.
*
* @param	Data the 32-bit number to be converted.
*
* @return	The converted 32-bit number in network byte order.
*
* @note		None.
*
******************************************************************************/
#define Xil_Htonl(Data) (Data)

/*****************************************************************************/
/**
*
* Convert a 16-bit number from host byte order to network byte order.
*
* @param	Data the 16-bit number to be converted.
*
* @return	The converted 16-bit number in network byte order.
*
* @note		None.
*
******************************************************************************/
#define Xil_Htons(Data) (Data)

/*****************************************************************************/
/**
*
* Convert a 32-bit number from network byte order to host byte order.
*
* @param	Value the 32-bit number to be converted.
*
* @return	The converted 32-bit number in host byte order.
*
* @note		None.
*
******************************************************************************/
#define Xil_Ntohl(Data) (Data)

/*****************************************************************************/
/**
*
* Convert a 16-bit number from network byte order to host byte order.
*
* @param	Value the 16-bit number to be converted.
*
* @return	The converted 16-bit number in host byte order.
*
* @note		None.
*
******************************************************************************/
#define Xil_Ntohs(Data) (Data)

#ifdef __cplusplus
}
#endif

#endif
