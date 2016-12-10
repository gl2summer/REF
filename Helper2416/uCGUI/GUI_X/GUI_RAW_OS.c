/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUI_X.C
Purpose     : COnfig / System dependent externals for GUI
---------------------------END-OF-HEADER------------------------------
*/


#include "GUI_X.h"
#include <raw_api.h>

/*********************************************************************
*
*       GUI_X_Init()
*/
void GUI_X_Init(void) {};

/*********************************************************************
*
*       GUI_X_ExecIdle()
*/
void GUI_X_ExecIdle(void) {
  raw_sleep(1);
};


/*********************************************************************
*
*      Timing:
*                 GUI_GetTime()
*                 GUI_Delay(int)

  Some timing dependent routines of uC/GUI require a GetTime
  and delay funtion. Default time unit (tick), normally is
  1 ms.
*/

int GUI_X_GetTime(void) {

  return (raw_system_time_get() * 10);
	
}

void GUI_X_Delay(int Period) {
  int delay =  Period / 10;
	
  raw_sleep(delay);
}

/*********************************************************************
*
*      Multitask interface for Win32
*
*  The folling section consisting of 4 routines is used to make
*  the GUI software thread safe with WIN32
*/


RAW_SEMAPHORE  x_sem;

void GUI_X_InitOS(void) {
//	unsigned char ret;
/*	ret = */raw_semaphore_create(&x_sem, "sem1", 1);
//	Uart_Printf("ret = %d\n",ret);

	
}


U32 GUI_X_GetTaskId(void) {

 return ((U32)raw_task_identify());
 	
}

void GUI_X_Lock(void) {

	raw_semaphore_get(&x_sem,  RAW_WAIT_FOREVER);
  
}

void GUI_X_Unlock(void) {

	 raw_semaphore_put(&x_sem);
  
}


/*********************************************************************
*
*      Text output for Logging, warnings and errors

  Logging - required only for higher debug levels
*/
//void GUI_X_Log     (const char *s)     { SIM_Log(s); }
//void GUI_X_Warn    (const char *s)     { SIM_Warn(s); }
//void GUI_X_ErrorOut(const char *s)     { SIM_ErrorOut(s); }

//下面三个函数需要添加
void GUI_X_Log     (const char *s) { GUI_USE_PARA(s); }
void GUI_X_Warn    (const char *s) { GUI_USE_PARA(s); }
void GUI_X_ErrorOut(const char *s) { GUI_USE_PARA(s); }







