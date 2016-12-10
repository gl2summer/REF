/**
 *******************************************************************************
 * @file       platform.h 
 * @author     camel.shoko
 * @version    v1.1
 * @date       2014/08/20
 * @brief      平台相关定义
 * @copyright  camel.shoko@gmail.com
 *******************************************************************************
 */


#ifndef PLATFORM_H_
#define PLATFORM_H_

/* Includes ------------------------------------------------------------------*/
#include <raw_api.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <2416_reg.h>
#include "uart.h"

/* Defines -------------------------------------------------------------------*/

/* 小端模式 */
#define LITTELE_ENDIAN 									1      					        

/* 软件版本定义1.10 */
#define VER_MAJOR            					        1
#define VER_MINOR            					        1
#define VER_PATCH          					            0

#define S3C2416_ARM926_PORT								1
#define USE_STDARG_PRINTF								0
#define USE_IAR_LIB_PRINTF								0


/* Macros --------------------------------------------------------------------*/

/* 调度器和协程 */
#ifdef CRTHREAD_SCHED
	#define MAX_CRTHREAD                    			10
#endif


/* 硬件平台PORTING */
#if S3C2416_ARM926_PORT
	#define UCOSII_RTOS_PORTING							0 
	#define RAW_RTOS_PORTING							1  
#endif


/* 指定调试方法 */
#if USE_IAR_LIB_PRINTF == 1
	#define ENALBE_IAR_LIB_PRINTF						1
	#define ENABLE_STDARG_PRINTF						0
#elif USE_STDARG_PRINTF == 1
	#define ENALBE_IAR_LIB_PRINTF						0
	#define ENABLE_STDARG_PRINTF						1
#else
	#define uprintf										Uart_Printf
#endif


#if ENABLE_STDARG_PRINTF
    #include "printf_stdarg.h"
#endif

#if ENALBE_IAR_LIB_PRINTF
	#include <stdio.h>
#endif



#define _STR(x)              					        #x
#define STR(x)               					        _STR(x)
#define VERSION              					        STR(VER_MAJOR)"."STR(VER_MINOR)"."STR(VER_PATCH)



#ifdef __cplusplus
    #define DECLARE_EXTERN_C_BEGIN 			            extern "C" {
    #define DECLARE_EXTERN_C_END  			            }
#else
    #define DECLARE_EXTERN_C_BEGIN
    #define DECLARE_EXTERN_C_END
#endif


#ifndef NULL
	#ifdef __cplusplus
		#define NULL 									( 0 )
	#else
		#define NULL 									( (void *)0 )
	#endif
#endif
	
	
#if defined(DEBUG) || defined(_DEBUG)
    #define DEBUG(x)           						   	Uart_Printf(x"\r\n");
    #define DEBUGN(x)                                   Uart_Printf(x);
    #define DEBUG0(x)                                   Uart_Printf(x"\r\n");
    #define DEBUG1(x, y)       						    Uart_Printf(x"\r\n", y);
    #define DEBUG2(x, y, z)    						    Uart_Printf(x"\r\n", y, z);
    #define DEBUG3(x, y, z, a) 						    Uart_Printf(x"\r\n", y, z, a);
    #define DEBUG4(x, y, z, a, b) 						Uart_Printf(x"\r\n", y, z, a, b);
	#define DEBUG5(x, y, z, a, b, c) 					Uart_Printf(x"\r\n", y, z, a, b, c); 
    #define DEBUG_HEX(pData, nLen)		                							\
    do                                 	                							\
    {                                  	               		 						\
        int i = 0, len = nLen;         	                							\
        uint8_t *pByte = (uint8_t *)pData;   	        							\
        for (; i<len; i++)             	                							\
        {                              	                							\
           Uart_Printf("%02X ", pByte[i]); 											\
        }                              	                							\
        Uart_Printf("\r\n");										     			\
        }  								               		 						\
    while (0);
#else
    #define DEBUG(x) 
    #define DEBUG0(x)
    #define DEBUGN(x)
    #define DEBUG1(x, y)
    #define DEBUG2(x, y, z)
    #define DEBUG3(x, y, z, a) 
    #define DEBUG4(x, y, z, a, b) 
	#define DEBUG5(x, y, z, a, b, c) 
    #define DEBUG_HEX(pData, nLen)
#endif


		
#if LITTELE_ENDIAN
    #define SWAP16(A)             				        ((((WORD)(A) & 0xFF00) >> 8) | (((WORD)(A) & 0x00FF) << 8) )
    
    #define SWAP32(A)             				        \
       ((((DWORD)(A) & 0xff000000) >> 24) | 	        \
        (((DWORD)(A) & 0x00ff0000) >> 8)  |   	        \
        (((DWORD)(A) & 0x0000ff00) << 8)  |   	        \
        (((DWORD)(A) & 0x000000ff) << 24))
#else
    #define SWAP16(A)             				        ( A ) 
    #define SWAP32(A)             				        ( A ) 
#endif
   
   
#if UCOSII_RTOS_PORTING
	#define msleep(ms)									OSTimeDly((1000*ms)/OS_TICKS_PER_SEC)
#endif


#if RAW_RTOS_PORTING
	#define msleep(ms)									raw_sleep(ms)
#endif


/* Global Function Prototype -------------------------------------------------*/
extern void MMU_EnableDCache(void);
extern void MMU_EnableICache(void);
extern void exception_vector(void);



#endif /*PLATFORM_H_*/

/****************** (C) COPYRIGHT 2013 Camle.shoko ***********END OF FILE******/
