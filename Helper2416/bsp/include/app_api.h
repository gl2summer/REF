#ifndef _APP_API_H_
#define _APP_API_H_

//#define __lcd_bsp_test_ 		//ע�ͣ�GUI����		��ע�ͣ�LCD�������

#ifdef  __lcd_bsp_test_
	#undef	__lcd_GUI_test_
#else
	#define	__lcd_GUI_test_
#endif


/*************************���⺯��������ʼ*************************************/
void lcd_test_api(void *p_arg);
void gui_test_api(void *p_arg);
/*************************���⺯������������***********************************/
#endif
