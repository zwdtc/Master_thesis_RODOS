;* $Id: translation_table.s,v 1.1.4.1 2011/10/24 09:35:18 sadanan Exp $
;******************************************************************************
;
; (c) Copyright 2009-2010 Xilinx, Inc. All rights reserved.
;
; This file contains confidential and proprietary information of Xilinx, Inc.
; and is protected under U.S. and international copyright and other
; intellectual property laws.
;
; DISCLAIMER
; This disclaimer is not a license and does not grant any rights to the
; materials distributed herewith. Except as otherwise provided in a valid
; license issued to you by Xilinx, and to the maximum extent permitted by
; applicable law: (1) THESE MATERIALS ARE MADE AVAILABLE "AS IS" AND WITH ALL
; FAULTS, AND XILINX HEREBY DISCLAIMS ALL WARRANTIES AND CONDITIONS, EXPRESS,
; IMPLIED, OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF
; MERCHANTABILITY, NON-INFRINGEMENT, OR FITNESS FOR ANY PARTICULAR PURPOSE;
; and (2) Xilinx shall not be liable (whether in contract or tort, including
; negligence, or under any other theory of liability) for any loss or damage
; of any kind or nature related to, arising under or in connection with these
; materials, including for any direct, or any indirect, special, incidental,
; or consequential loss or damage (including loss of data, profits, goodwill,
; or any type of loss or damage suffered as a result of any action brought by
; a third party) even if such damage or loss was reasonably foreseeable or
; Xilinx had been advised of the possibility of the same.
;
; CRITICAL APPLICATIONS
; Xilinx products are not designed or intended to be fail-safe, or for use in
; any application requiring fail-safe performance, such as life-support or
; safety devices or systems, Class III medical devices, nuclear facilities,
; applications related to the deployment of airbags, or any other applications
; that could lead to death, personal injury, or severe property or
; environmental damage (individually and collectively, "Critical
; Applications"). Customer assumes the sole risk and liability of any use of
; Xilinx products in Critical Applications, subject only to applicable laws
; and regulations governing limitations on product liability.
;
; THIS COPYRIGHT NOTICE AND DISCLAIMER MUST BE RETAINED AS PART OF THIS FILE
; AT ALL TIMES.
;
;****************************************************************************
;****************************************************************************
;**
; @file translation_table.s
;
; This file contains the initialization for the MMU table in RAM
; needed by the Cortex A9 processor
;
; <pre>
; MODIFICATION HISTORY:
;
; Ver   Who  Date     Changes
; ----- ---- -------- ---------------------------------------------------
; 1.00a ecm  10/20/09 Initial version
; 3.07a sgd  07/05/2012 Configuring device address spaces as shareable device
;		       instead of strongly-ordered.
; </pre>
;
; @note
;
; None.
;
;****************************************************************************
	EXPORT  MMUTable

	AREA |.mmu_tbl|,CODE,ALIGN=14

MMUTable
	; Each table entry occupies one 32-bit word and there are
	; 4096 entries, so the entire table takes up 16KB.
	; Each entry covers a 1MB section.


   GBLA count
   GBLA sect

; 0x00000000 - 0x3ffffff (DDR Cacheable) 
count   SETA  0
sect    SETA  0
   WHILE count<0x400
   DCD	sect + 0x15de6		; S=1, TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1 
sect    SETA  sect+0x100000
count   SETA  count+1
   WEND

; 0x40000000 - 0x7fffffff (GpAxi0)
count   SETA  0
   WHILE count<0x400
   DCD	sect + 0xc06		; S=0, TEX=b010 AP=b11, Domain=b0, C=b0, B=b0 
sect    SETA  sect+0x100000
count   SETA  count+1
   WEND

; 0x80000000 - 0xbfffffff (GpAxi1)
count   SETA  0
   WHILE count<0x400
   DCD	sect + 0xc06		; S=0, TEX=b010 AP=b11, Domain=b0, C=b0, B=b0 
sect    SETA  sect+0x100000
count   SETA  count+1
   WEND

; 0xc0000000 - 0xdfffffff (undef)
count   SETA  0
   WHILE count<0x200
   DCD	sect 			; S=0, TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 
sect    SETA  sect+0x100000
count   SETA  count+1
   WEND

; 0xe0000000 - 0xefffffff (IOP dev)
count   SETA  0
   WHILE count<0x100
   DCD	sect + 0xc06		; S=0, TEX=b010 AP=b11, Domain=b0, C=b0, B=b0 
sect    SETA  sect+0x100000
count   SETA  count+1
   WEND

; 0xf0000000 - 0xf7ffffff (reserved)
count   SETA  0
   WHILE count<0x80
   DCD	sect 			; S=0, TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 
sect    SETA  sect+0x100000
count   SETA  count+1
   WEND

; 0xf8000000 - 0xf9ffffff (APB device regs)
count   SETA  0
   WHILE count<0x20
   DCD	sect + 0xc06		; S=0, TEX=b010 AP=b11, Domain=b0, C=b0, B=b0 
sect    SETA  sect+0x100000
count   SETA  count+1
   WEND

; 0xfa000000 - 0xfbffffff (reserved)
count   SETA  0
   WHILE count<0x20
   DCD	sect 			; S=0, TEX=b000 AP=b00, Domain=b0, C=b0, B=b0 
sect    SETA  sect+0x100000
count   SETA  count+1
   WEND

; 0xfc000000 - 0xfffffff (OCM/QSPI) 
count   SETA  0
   WHILE count<0x40
   DCD	sect + 0x15de6		; S=1, TEX=b101 AP=b11, Domain=b1111, C=b0, B=b1 
sect    SETA  sect+0x100000
count   SETA  count+1
   WEND

   END
