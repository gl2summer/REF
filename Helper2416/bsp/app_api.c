/*
* Copyright (c) 2013
* All rights reserved.
* 
* 文件名称：app_api.C
* 文件标识：
* 开发环境：C编译器
* 描    述：提供上层调的接口文件
* 当前版本：V1.0
* 作    者：
* 更改说明：
* 完成日期：2013年04月01日
*
* 修改记录 
*       作者        时间        版本            修改描述
*       章光泉      20130401      V1.0        
*/

#include "app_api.h"
#include "lcd_driver.h"
#include "adcts_driver.h"
#include "gui_t.h"
#include <raw_api.h>
#include "GUI.h"

/*************************************************
    函数名称:       lcd_test_api
    功能描述:       lcd
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void lcd_test_api(void *p_arg)
{
	lcd_test();
	//return;
}

/*************************************************
    函数名称:       gui_test_api
    功能描述:       gui
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/

extern const GUI_FONT GUI_Font8x16;
extern const GUI_FONT GUI_Font6x8;
extern const GUI_FONT GUI_Font10_1;

void gui_test_api(void *p_arg)
{
	//_DemoAntialiasing();
	GUI_SetBkColor(GUI_BLUE);//背景未测试完
	GUI_SetColor(GUI_RED);
	
	while (1) 
	{
		#if 0  //调试基本API，确保颜色正确 
		//修改API驱动后，要测试各API的颜色情况。需调试
		GUI_SetColor(GUI_RED);
		GUI_DrawHLine(20,0,480);	//画横线
		GUI_SetColor(GUI_GREEN);
		GUI_DrawHLine(60,0,360);	
		GUI_SetColor(GUI_BLUE);
		GUI_DrawHLine(80,0,240);	
		//GUI_DrawVLine(20,0,272);	//画竖线
		#endif
		
		#if 0
		//GUI_FillRect(10,10,40,40);//画矩形框		
		GUI_SetBkColor(GUI_RED);
		GUI_ClearRect(0,0,480,100);	//矩形框
		GUI_SetBkColor(GUI_GREEN);
		GUI_ClearRect(0,100,360,200);	
		GUI_SetBkColor(GUI_BLUE);
		GUI_ClearRect(0,200,120,272);	
		
		//GUI_DrawPixel(80,80);
		//GUI_DrawPoint(10,10);
		//GUI_InvertRect(90,90,150,150);  //反色
		#endif
		
		#if 1  //字体及颜色 
		
		const GUI_FONT * oldFont=GUI_SetFont(&GUI_Font8x16);
		GUI_DispStringAt("This text is 8 by 16 pixels", 0,0);
		GUI_SetFont(&GUI_Font6x8);
		GUI_DispStringAt("This text is 6 by 8 pixels", 0,20);
		GUI_SetFont(&GUI_Font10_1);
		GUI_DispStringAt("This text is proportinal ", 0,40);
		GUI_SetFont(oldFont);
		#endif
	}
}


