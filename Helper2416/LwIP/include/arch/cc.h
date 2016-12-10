/*
 * Copyright (c) 2001, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * This file is part of the lwIP TCP/IP stack.
 *
 * Author: Adam Dunkels <adam@sics.se>
 *
 * $Id: cc.h,v 1.1.1.1 2004/12/16 14:17:13 bear Exp $
 */
#ifndef __ARCH_CC_H__
#define __ARCH_CC_H__

#include <raw_api.h>

typedef RAW_U8	u8_t;
typedef RAW_S8	s8_t;
typedef RAW_U16	u16_t;
typedef RAW_S16	s16_t;
typedef RAW_U32	u32_t;
typedef RAW_S32	s32_t;
typedef RAW_U32	mem_ptr_t;

#define U16_F "hu"
#define S16_F "hd"
#define X16_F "hx"
#define U32_F "lu"
#define S32_F "ld"
#define X32_F "lx"
//#define sys_prot_t 

#define sys_prot_t RAW_U32

#define BYTE_ORDER  LITTLE_ENDIAN


#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))

#define  LWIP_PROVIDE_ERRNO  1


#if 0
#if defined(__CC_ARM)   /* ARMCC compiler */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#elif defined(__IAR_SYSTEMS_ICC__)   /* IAR Compiler */
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_STRUCT
#define PACK_STRUCT_END
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_USE_INCLUDES
#elif defined(__GNUC__)     /* GNU GCC Compiler */
#define PACK_STRUCT_FIELD(x) x
#define PACK_STRUCT_STRUCT __attribute__((packed))
#define PACK_STRUCT_BEGIN
#define PACK_STRUCT_END
#endif
#endif



#define LWIP_PLATFORM_DIAG(x)	do {Uart_Printf x;} while(0)
#define LWIP_PLATFORM_ASSERT(x) do {RAW_SR_ALLOC(); Uart_Printf(x); RAW_CRITICAL_ENTER(); RAW_ASSERT(0);RAW_CRITICAL_EXIT();}while(0)


#include "string.h"

#endif /* __ARCH_CC_H__ */

