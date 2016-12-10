/*
* Copyright (c) 2013
* All rights reserved.
* 
* 文件名称：bsp_init.C
* 文件标识：
* 开发环境：C编译器
* 描    述:	bsp初始化文件
* 当前版本：V1.0
* 作    者：
* 更改说明：
* 完成日期：2013年04月01日
*
* 修改记录 
*       作者        时间        版本            修改描述
*     章光泉      20130401      V1.0        
*/


#include <uart.h>
#include "bsp_init.h"
#include "lcd_driver.h"
#include "ADCTs_driver.h"
#include "GUI.h"
#include "app_api.h"

void bsp_init(void)
{
	//LCD初始化
#ifdef __lcd_bsp_test_	
	lcd_init();
	Uart_Printf("lcd_init()\n");
#endif	
	//Touch panel 初始化
	//adc_ts_init();
	
	//各初始化
	//...
	Uart_Printf("Initialize over!!!..........\n");
	
	return;
}
