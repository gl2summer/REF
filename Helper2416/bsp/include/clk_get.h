/**
 *******************************************************************************
 * @file       clk_get.h
 * @author     camel.shoko
 * @version    v1.1
 * @date       2014/05/26
 * @brief      S3C2416 CLK驱动
 * @copyright  camel.shoko@gmail.com
 *******************************************************************************
 */
 
#ifndef _CLK_GET_H_
#define _CLK_GET_H_

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>


/* Defines -------------------------------------------------------------------*/
#define MPLL 										0
#define UPLL 										1

/* 
 * Input clock of PLL 
 * The SMDK2416 has 12MHz input clock 
 */
#define CONFIG_SYS_CLK_FREQ							12000000	


/* Global Function Prototype -------------------------------------------------*/
void CPUInfoPrint(void);
void DelayMs(volatile int ms);

uint32_t get_FCLK(void);
uint32_t get_HCLK(void);
uint32_t get_PCLK(void);
uint32_t get_UCLK(void);
uint32_t get_ARMCLK(void);


#endif

/****************************** END OF FILE ***********************************/
