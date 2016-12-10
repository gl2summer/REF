#include "GUI.h"
#include "wm.h"  

#include <raw_api.h>
#include "lcd_driver.h"
#include "2416_reg.h"
#include "22.c"

#define  GUI_TASK_STK_SIZE 10240
PORT_STACK gui_task_stack1[GUI_TASK_STK_SIZE];
PORT_STACK gui_task_stack2[GUI_TASK_STK_SIZE];
RAW_TASK_OBJ 		test_gui_obj[10];


void ucgui_task(void * pParam) ;
void RePaint(void *pParam);
extern void GUIDEMO_Dialog(void);
extern void DemoRedraw(void);
extern void GUIDEMO_Automotive(void);
extern void GUIDEMO_main(void);

/*XY坐标校准*/
static void Touch_Calibrate(void){
    
    GUI_TOUCH_Calibrate(0,0,480,800,240);             //X轴校验
    GUI_TOUCH_Calibrate(1,0,272,640,360);             //Y轴校验
}

extern int touch_state_set(void);


static void RePaint(void *pParam)
{
	GUI_PID_STATE t_state;
	
	Uart_Printf("*****^^^^^^^^^^^^^^^^^^^^\n");
	
	TouchPanel_HardInit_u();
	GUI_Init();

	Display_Start(LCD_WIN_0);
	
	while(1) 
	{
		/*触摸屏*/
		GUI_TOUCH_Exec();

		/*bug here, do not add the following line*/
		//GUI_Exec();              //Redraw form

		raw_sleep(1);
		touch_state_set();
	
	}
}

void start_ucgui_test(void)
{
	Uart_Printf("ucgui_test\n");

	raw_task_create(&test_gui_obj[2], (RAW_U8 *)"Exec()"	, 0, IDLE_PRIORITY - 2, 0, gui_task_stack2, GUI_TASK_STK_SIZE, RePaint, 1);
	raw_task_create(&test_gui_obj[1], (RAW_U8 *)"ucgui_test", 0, IDLE_PRIORITY - 1, 0, gui_task_stack1, GUI_TASK_STK_SIZE, ucgui_task, 1);
	
}


void zgqDialogTest(void);


extern const GUI_FONT GUI_FontHZ_SimSun_20;
extern const GUI_FONT GUI_FontHZ_SimSun_17;
extern const GUI_FONT GUI_FontHZ_SimSun_13;

extern GUI_CONST_STORAGE GUI_BITMAP bm22;
extern GUI_CONST_STORAGE GUI_BITMAP GUI_BitmapArrowS;

extern unsigned char ac22[];

static void ucgui_task (void * pParam)
{
	Uart_Printf("*****************start\n");

	GUI_CURSOR_Show();

	
	while(1)
	{

		#if 0
		
		GUI_SetBkColor(GUI_BLUE);
		GUI_Clear();

		
		GUI_SetFont(&GUI_FontHZ_SimSun_20);
		
		GUI_SetColor(GUI_WHITE);
		GUI_DispStringAt("净化风速  ",100,100);
		GUI_SetColor(GUI_YELLOW);
		GUI_DispString("111  ");
		GUI_SetColor(GUI_WHITE);
		GUI_DispString("加湿风速  ");
		GUI_SetColor(GUI_YELLOW);
		GUI_DispString("222  ");
		GUI_SetColor(GUI_WHITE);
		GUI_DispString("PM2.5颗粒物  ");
		GUI_SetColor(GUI_YELLOW);
		GUI_DispString("33aa ");
		GUI_SetColor(GUI_WHITE);
		GUI_DispString("空气洁净  ");
		GUI_SetColor(GUI_YELLOW);
		GUI_DispString("44bb ");
		
		GUI_GIF_Draw(safety_icon, sizeof(safety_icon), 120, 300);

		GUI_SetFont(&GUI_FontHZ_SimSun_17);
		
		GUI_SetColor(GUI_RED);
		GUI_DispStringAt("注意安全  ",100,330);


				
		GUI_GIF_Draw(help_icon, sizeof(help_icon), 210, 300);

		GUI_SetFont(&GUI_FontHZ_SimSun_17);
		
		GUI_SetColor(GUI_RED);
		GUI_DispString("使用帮助");


		while (1);

		//GUIDEMO_Touch();
	
		//MainTask2();

		#endif
		
		GUIDEMO_main();
		raw_sleep(100);
	}


	

}





