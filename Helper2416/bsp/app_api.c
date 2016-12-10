/*
* Copyright (c) 2013
* All rights reserved.
* 
* �ļ����ƣ�app_api.C
* �ļ���ʶ��
* ����������C������
* ��    �����ṩ�ϲ���Ľӿ��ļ�
* ��ǰ�汾��V1.0
* ��    �ߣ�
* ����˵����
* ������ڣ�2013��04��01��
*
* �޸ļ�¼ 
*       ����        ʱ��        �汾            �޸�����
*       �¹�Ȫ      20130401      V1.0        
*/

#include "app_api.h"
#include "lcd_driver.h"
#include "adcts_driver.h"
#include "gui_t.h"
#include <raw_api.h>
#include "GUI.h"

/*************************************************
    ��������:       lcd_test_api
    ��������:       lcd
    �� �� ��:       
    ��������:       
    �������:       ��
    �������:       ��
    �� �� ֵ:       ��
    ˵    ��:       
*************************************************/
void lcd_test_api(void *p_arg)
{
	lcd_test();
	//return;
}

/*************************************************
    ��������:       gui_test_api
    ��������:       gui
    �� �� ��:       
    ��������:       
    �������:       ��
    �������:       ��
    �� �� ֵ:       ��
    ˵    ��:       
*************************************************/

extern const GUI_FONT GUI_Font8x16;
extern const GUI_FONT GUI_Font6x8;
extern const GUI_FONT GUI_Font10_1;

void gui_test_api(void *p_arg)
{
	//_DemoAntialiasing();
	GUI_SetBkColor(GUI_BLUE);//����δ������
	GUI_SetColor(GUI_RED);
	
	while (1) 
	{
		#if 0  //���Ի���API��ȷ����ɫ��ȷ 
		//�޸�API������Ҫ���Ը�API����ɫ����������
		GUI_SetColor(GUI_RED);
		GUI_DrawHLine(20,0,480);	//������
		GUI_SetColor(GUI_GREEN);
		GUI_DrawHLine(60,0,360);	
		GUI_SetColor(GUI_BLUE);
		GUI_DrawHLine(80,0,240);	
		//GUI_DrawVLine(20,0,272);	//������
		#endif
		
		#if 0
		//GUI_FillRect(10,10,40,40);//�����ο�		
		GUI_SetBkColor(GUI_RED);
		GUI_ClearRect(0,0,480,100);	//���ο�
		GUI_SetBkColor(GUI_GREEN);
		GUI_ClearRect(0,100,360,200);	
		GUI_SetBkColor(GUI_BLUE);
		GUI_ClearRect(0,200,120,272);	
		
		//GUI_DrawPixel(80,80);
		//GUI_DrawPoint(10,10);
		//GUI_InvertRect(90,90,150,150);  //��ɫ
		#endif
		
		#if 1  //���弰��ɫ 
		
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


