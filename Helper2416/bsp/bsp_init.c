/*
* Copyright (c) 2013
* All rights reserved.
* 
* �ļ����ƣ�bsp_init.C
* �ļ���ʶ��
* ����������C������
* ��    ��:	bsp��ʼ���ļ�
* ��ǰ�汾��V1.0
* ��    �ߣ�
* ����˵����
* ������ڣ�2013��04��01��
*
* �޸ļ�¼ 
*       ����        ʱ��        �汾            �޸�����
*     �¹�Ȫ      20130401      V1.0        
*/


#include <uart.h>
#include "bsp_init.h"
#include "lcd_driver.h"
#include "ADCTs_driver.h"
#include "GUI.h"
#include "app_api.h"

void bsp_init(void)
{
	//LCD��ʼ��
#ifdef __lcd_bsp_test_	
	lcd_init();
	Uart_Printf("lcd_init()\n");
#endif	
	//Touch panel ��ʼ��
	//adc_ts_init();
	
	//����ʼ��
	//...
	Uart_Printf("Initialize over!!!..........\n");
	
	return;
}
