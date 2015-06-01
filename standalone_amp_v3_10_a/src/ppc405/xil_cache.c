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
* @file xil_cache.c
*
* This contains implementation of cache related driver functions.
*
* <pre>
* MODIFICATION HISTORY:
*
* Ver   Who  Date     Changes
* ----- ---- -------- -------------------------------------------------------
* 1.00  hbm  07/28/09 Initial release
*
* </pre>
*
* @note
*
* None.
*
******************************************************************************/


#include "xil_cache.h"
#include "xpseudo_asm.h"
#include "xparameters.h"

#define DCWR_VAL        0x0
#define SGR_VAL         0x0
#define CCR0_VAL        0x700a00

#define DCACHE_SIZE     (16*1024)

/****************************************************************************/
/**
*
* Write to the Core-Configuration Register (CCR0)
*
* @param    Value to be written to CCR0.  The bit fields are defined
*           in xreg405.h
*
* @return   None.
*
* @note
*
* None.
*
****************************************************************************/
void Xil_CacheWriteCCR0(u32 Val)
{
	sync;
	isync;
	mtspr(XREG_SPR_CCR0, Val);
	sync;
	isync;
}

/****************************************************************************/
/**
*
* Enable the data cache.
*
* @return   None.
*
* @note
*
* This function uses a 0x80000001 for the mask. Each pair of adjacent bits
* in the mask marks 256MB as cacheable.
* The CPU driver generates a mask (XPAR_CACHEABLE_REGION_MASK) for cacheable
* regions. This mask includes memories with size less than 256MB. Using this
* mask for enabling caches could cause a problem if there are any memory mapped
* peripherals around these smaller memory regions.
*
****************************************************************************/
void Xil_DCacheEnable()
{
	Xil_DCacheEnableRegion(0x80000001);
}

/****************************************************************************/
/**
*
* Enable the data cache region.
*
* @param    Regions to be marked as cachable.  Each bit in the regions
*           variable stands for 128MB of memory.
*           regions    --> cached address range
*           ------------|--------------------------------------------------
*           0x80000000  | [0, 0x7FFFFFF]
*           0x00000001  | [0xF8000000, 0xFFFFFFFF]
*           0x80000001  | [0, 0x7FFFFFF],[0xF8000000, 0xFFFFFFFF]
*
* @return   None.
*
* @note
*
* This function is specific to PPC405. Processor must be in real mode.
*
****************************************************************************/
void Xil_DCacheEnableRegion(u32 Regions)
{
	Xil_DCacheDisable();
	/* after this point cache is disabled and invalidated */

	mtspr(XREG_SPR_SGR,  SGR_VAL);
	mtspr(XREG_SPR_DCWR, DCWR_VAL);
	mtspr(XREG_SPR_DCCR, Regions);
	isync;
}


/****************************************************************************/
/**
*
* Disable the data cache.
*
* @param    None
*
* @return   None.
*
* @note
*
****************************************************************************/
void Xil_DCacheDisable(void)
{
	register unsigned int Address0;
	register unsigned int CCR0;
	register unsigned int Tag;
	register unsigned int I;
	unsigned int DCCR;

	DCCR = mfspr(XREG_SPR_DCCR);
	if (DCCR != 0) {
		/* dcache is enabled; read tags and flush valid and dirty
		 * lines */
		CCR0 = mfspr(XREG_SPR_CCR0);	/* read CCR0 */
		CCR0 = (CCR0 & 0xfffffffe) | 0x10;  /* read tags from A way */
		I = 0;
		while (I < 2) {
			mtspr(XREG_SPR_CCR0, CCR0);
			Address0 = 0x0;
			while (Address0 < (DCACHE_SIZE / 2)) {
				Tag = dcread(Address0);
				if ((Tag & 0x30) == 0x30)
					/* valid and dirty? */
					dcbf((Tag & 0xfffff000) | Address0);
				Address0 += 32;
			}
			CCR0 |= 1;
                        /* read tags from B way */
			I++;
		}
		sync;
		mtspr(XREG_SPR_DCCR, 0);
		isync;
	}
	else {
		/* dcache is not enabled; invalidate all lines */
		Address0 = 0;
		while (Address0 < (DCACHE_SIZE/2)) {
			dccci(Address0);
			Address0 += 32;
		}
	}
}

/****************************************************************************/
/**
*
* Flush a data cache line. If the byte specified by the address (Addr)
* is cached by the data cache, the cacheline containing that byte is
* invalidated.  If the cacheline is modified (dirty), the entire
* contents of the cacheline are written to system memory before the
* line is invalidated.
*
* @param    Addr is the address to be flushed
*
* @return   None.
*
* @note
*
****************************************************************************/

static void DCacheFlushLine(u32 Addr)
{
	dcbf(Addr);
	sync;
	isync;
}

/****************************************************************************/
/**
* Flush the entire data cache. If any cacheline is dirty, the cacheline will be
* written to system memory. The entire data cache will be invalidated.
*
* @return   None.
*
* @note
*
****************************************************************************/
void Xil_DCacheFlush(void)
{
	register u32 Address0;
	register u32 CCR0;
	register u32 Tag;
	register unsigned int I;
	u32 DCCR;

	DCCR = mfspr(XREG_SPR_DCCR);
	if (DCCR != 0) {
		/* dcache is enabled; read tags and flush valid and dirty
		 * lines
		 */
		CCR0 = mfspr(XREG_SPR_CCR0);                /* read CCR0 */
		CCR0 = (CCR0 & 0xfffffffe) | 0x10;  /* read tags from A way */
		I = 0;
		while (I < 2) {
			mtspr(XREG_SPR_CCR0, CCR0);
			Address0 = 0x0;
			while (Address0 < (DCACHE_SIZE / 2)) {
				Tag = dcread(Address0);
				if ((Tag & 0x30) == 0x30)
					/* valid and dirty? */
					dcbf((Tag & 0xfffff000) | Address0);
				Address0 += 32;
			}
			CCR0 |= 1;
                        /* read tags from B way */
			I++;
		}
		sync;
		isync;
	}
}

/****************************************************************************/
/**
*
* Invalidate a data cache line. If the byte specified by the address (Addr)
* is cached by the data cache, the cacheline containing that byte is
* invalidated.  If the cacheline is modified (dirty), the modified contents
* are lost and are NOT written to system memory before the line is
* invalidated.
*
* @param    Addr is the address to be invalidated.
*
* @return   None.
*
* @note
*
****************************************************************************/
static void DCacheInvalidateLine(u32 Addr)
{
	dcbi(Addr);
	sync;
	isync;
}

/****************************************************************************/
/**
*
* Invalidate entire data cache.
*
* @param    None.
*
* @return   None.
*
* @note
*
****************************************************************************/
void Xil_DCacheInvalidate(void)
{
	u32 Address0 = 0;
	while (Address0 < (DCACHE_SIZE/2)) {
		dccci(Address0);
		Address0 += 32;
	}
}



/****************************************************************************/
/**
* Flush the data cache for the given address range.
* If the bytes specified by the address (Addr) are cached by the data cache,
* the cacheline containing that byte is invalidated.  If the cacheline
* is modified (dirty), the written to system memory first before the
* before the line is invalidated.
*
* @param    Addr is the starting address of range to be flushed
* @param    Len is the length of the range to be flushed in bytes
*
* @return   None.
*
* @note
*
****************************************************************************/

void Xil_DCacheFlushRange(u32 Addr, unsigned Len)
{
	const unsigned CacheLine = 32;
	unsigned int End;

	if (Len != 0) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until Addr+Len
		 */
		End = Addr + Len;
		Addr = Addr & ~(CacheLine - 1);

		while (Addr < End) {
			DCacheFlushLine(Addr);
			Addr += CacheLine;
		}
	}
}

/****************************************************************************/
/**
*
* Invalidate the data cache for the given address range.
* If the bytes specified by the address (Addr) are cached by the data cache,
* the cacheline containing that byte is invalidated.  If the cacheline
* is modified (dirty), the modified contents are lost and are NOT
* written to system memory before the line is invalidated.
*
* @param    Addr is the starting address of the range to be invalidated
* @param    Len is the length of range to be invalidated in bytes
*
* @return   None.
*
* @note
*
* Processor must be in real mode.
****************************************************************************/
void Xil_DCacheInvalidateRange(u32 Addr, unsigned Len)
{
	const unsigned CacheLine = 32;
	unsigned int End;

	if (Len != 0) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until Addr+Len
		 */
		End = Addr + Len;
		Addr = Addr & ~(CacheLine - 1);

		while (Addr < End) {
			DCacheInvalidateLine(Addr);
			Addr += CacheLine;
		}
	}
}

/****************************************************************************/
/**
*
* Enable the instruction cache region.
*
* @param    Regions to be marked as cachable.  Each bit in the regions
*           variable stands for 128MB of memory.
*           regions    --> cached address range
*           ------------|--------------------------------------------------
*           0x80000000  | [0, 0x7FFFFFF]
*           0x00000001  | [0xF8000000, 0xFFFFFFFF]
*           0x80000001  | [0, 0x7FFFFFF],[0xF8000000, 0xFFFFFFFF]
*
* @return   None.
*
* @note     This function is specific to PPC405.
*
*
****************************************************************************/
void Xil_ICacheEnableRegion(u32 Regions)
{
	Xil_ICacheDisable();

	mtspr(XREG_SPR_SGR, SGR_VAL);
	/* mtspr(XREG_SPR_SLER, SLER_VAL); */
	/* mtspr(XREG_SPR_SU0R, SU0R_VAL); */
	iccci;
	mtspr(XREG_SPR_ICCR, Regions);
	isync;
}

/****************************************************************************/
/**
*
* Enable the instruction cache.
*
* @return   None.
*
* @note
*
* This function uses a 0x80000001 for the mask. Each pair of adjacent bits
* in the mask marks 256MB as cacheable.
* The CPU driver generates a mask (XPAR_CACHEABLE_REGION_MASK) for cacheable
* regions. This mask includes memories with size less than 256MB. Using this
* mask for enabling caches could cause a problem if there are any memory mapped
* peripherals around these smaller memory regions.
*
****************************************************************************/
void Xil_ICacheEnable()
{
	Xil_ICacheEnableRegion(0x80000001);
}


/****************************************************************************/
/**
*
* Disable the instruction cache.
*
* @param    None.
*
* @return   None.
*
* @note
*
****************************************************************************/
void Xil_ICacheDisable(void)
{
	iccci;
	mtspr(XREG_SPR_ICCR, 0);
	isync;
}

/****************************************************************************/
/**
*
* Invalidate the entire instruction cache.
*
* @param    None.
*
* @return   None.
*
* @note
*
****************************************************************************/
void Xil_ICacheInvalidate(void)
{
	iccci;
	sync;
	isync;
}

/****************************************************************************/
/**
*
* Invalidate an instruction cache line.  If the instruction specified by the
* parameter Addr is cached by the instruction cache, the cacheline containing
* that instruction is invalidated.
*
* @param    Addr is the address to be invalidated.
*
* @return   None.
*
* @note
*
* Processor must be in real mode.
*
****************************************************************************/
void ICacheInvalidateLine(u32 Addr)
{
	icbi(Addr);
	sync;
	isync;
}

/****************************************************************************/
/**
*
* Invalidate the instruction cache for the given address range.
*
* @param    Addr is address of range to be invalidated.
* @param    Len is the length in bytes to be invalidated.
*
* @return   None.
*
****************************************************************************/
void Xil_ICacheInvalidateRange(u32 Addr, unsigned Len)
{
	const unsigned CacheLine = 32;
	unsigned int End;

	if (Len != 0) {
		/* Back the starting address up to the start of a cache line
		 * perform cache operations until Addr+Len
		 */
		End = Addr + Len;
		Addr = Addr & ~(CacheLine - 1);

		while (Addr < End) {
			ICacheInvalidateLine(Addr);
			Addr += CacheLine;
		}
	}
}

