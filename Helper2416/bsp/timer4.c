/**
 *******************************************************************************
 * @file       TIMER4.c
 * @author     camel.shoko
 * @version    v1.1
 * @date       2014/05/26
 * @brief      S3C2416 TIMER4驱动
 * @copyright  camel.shoko@gmail.com
 *******************************************************************************
 */


/* Includes ------------------------------------------------------------------*/
#include "2416_reg.h"
#include "irq.h"
#include "clk_get.h"
#include "timer4.h"
#include "raw_api.h"

/* Local Variables -----------------------------------------------------------*/
volatile int OneSec = 0;


/* Local Functions -----------------------------------------------------------*/

/**
 * @brief  TIMER4_ISR_Service
 * @note   TIMER4设置波特率
 * @param  none
 * @retval none
 */
static void TIMER4_ISR_Service(void)
{
	/* 置位清除 */
	SRCPND_REG |= (0x01<<14);	
	/* 置位清除 */
	INTPND_REG |= (0x01<<14);
	
	OneSec++;
}


/**
 * @brief  TIMER4_ISR_Init
 * @note   TIMER4中断初始化
 * @param  none
 * @retval none
 */
static void TIMER4_ISR_Init(void)
{
	INTMOD_REG 			= 0x00000000;
	PRIORITY_MODE_REG 	= 0x00000000;
	
	/* 使能TIMER4中断 */
	INTMSK_REG &= ~(0x01<<14);		
}


/* Global Functions ----------------------------------------------------------*/

/**
 * @brief  TIMER4_Init
 * @note   TIMER4初始化
 * @param  none
 * @retval none
 */
void TIMER4_Init(void)
{
	/*
	 * TIMER INPUT_CLK = PCLK/{Precaler+1}/{Divider_Value}
	 * 
	 * TIMER INPUT_CLK = PCLK/(15+1)/4 = PCLK/64
	 * 
	 * TCNTB0_REG = INPUT_CLK/OS_TICKS_PER_SEC = get_PCLK()/(64*OS_TICKS_PER_SEC)
	 */
	
	
	/* 使用TIMER4前,清除相关配置 */
	TCON_REG  &= ~(0x07<<20);			
	TCFG0_REG &= ~(0xFF<<8);		
	TCFG1_REG &= ~(0x0F<<16);

	TIMER4_ISR_Init();

	register_irq(14, TIMER4_ISR_Service);
	
	/* TIMER4_Prescaler=255*/
	TCFG0_REG |= (0xFF<<8);	
	
	/* TIMER4_Divider_Value=1/4 */	
	TCFG1_REG |= (0x01<<16);	

	TCNTB4_REG = TCNTB4_CFG;

	/* 更新TCNTB4 */
	TCON_REG |= (0x02<<20);	
	
	/* 手动更新位必须在下次写前清除 */	
	TCON_REG &= ~(0x02<<20);	
	
	/* 自动重装,启动定时器 */	
	TCON_REG |= (0x05<<20);	
	
}

/**
 * @brief  TIMER4_Start
 * @note   TIMER4启动
 * @param  none
 * @retval none
 */
void TIMER4_Start(void)
{
	TIMER4_Init();
}


/**
 * @brief  TIMER4_Stop
 * @note   TIMER4停止
 * @param  none
 * @retval none
 */
int TIMER4_Stop(void)
{
    TCON_REG &= ~(0x1 << 20); //定时器停止
	return TCNTO4_REG;
}

/****************** (C) COPYRIGHT 2014 Camle.shoko ***********END OF FILE******/
