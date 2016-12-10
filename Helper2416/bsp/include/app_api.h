#ifndef _APP_API_H_
#define _APP_API_H_

//#define __lcd_bsp_test_ 		//注释：GUI测试		不注释：LCD裸机测试

#ifdef  __lcd_bsp_test_
	#undef	__lcd_GUI_test_
#else
	#define	__lcd_GUI_test_
#endif


/*************************对外函数申明开始*************************************/
void lcd_test_api(void *p_arg);
void gui_test_api(void *p_arg);
/*************************对外函数申明开结束***********************************/
#endif
