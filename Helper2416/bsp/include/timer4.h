/**
 *******************************************************************************
 * @file       timer.h
 * @author     camel.shoko
 * @version    v1.1
 * @date       2014/05/26
 * @brief      S3C2416 TIMER4Çý¶¯
 * @copyright  camel.shoko@gmail.com
 *******************************************************************************
 */
 
 
#ifndef	_TIMER1_H_
#define	_TIMER1_H_

/* Includes ------------------------------------------------------------------*/
#include "clk_get.h"

/* Defines -------------------------------------------------------------------*/
#define TCNTB4_CFG					get_PCLK()/(4*(255+1)*1)


/* Global Function Prototype -------------------------------------------------*/
void TIMER4_Init(void);

void TIMER4_Start(void);
int TIMER4_Stop(void);

#endif

/****************** (C) COPYRIGHT 2014 Camle.shoko ***********END OF FILE******/
