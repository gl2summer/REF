#include "GUI.h"
#include "GUI_X.h"
#include "FRAMEWIN.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "somefunc.h" //include myself function 

#include <raw_api.h>
#include <math.h>
#include "ucgui_test.h"
#include "uart.h"
#include "gui_t.h"
#include "app_api.h"
#include "bsp_init.h"
#include "lcd_driver.h"
#include "adcts_driver.h"
#include <lib_string.h>
#include "PROGBAR.h"
#include "wm.h"  //use wm mode
#include "widget.h"
#include "button.h"
#include "checkbox.h"
#include "edit.h"
#include "framewin.h"
#include "listbox.h"
#include "progbar.h"
#include "radio.h"
#include "scrollbar.h"
#include "slider.h"
#include "text.h"
#include "menu.h"

#define TEST_BUTTON1_ID 50
#define TEST_BUTTON2_ID 51

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
	{ FRAMEWIN_CreateIndirect,"Dialog",0,0,0,480,272,0,0 },
	{ BUTTON_CreateIndirect,"BUTTON1",TEST_BUTTON1_ID,10,10,60,60,0,0 },
	{ BUTTON_CreateIndirect,"BUTTON2",TEST_BUTTON2_ID,100,100,60,60,0,0 },	
};

static void _cbCallback(WM_MESSAGE * pMsg)
{
	int NCode, Id;
	WM_HWIN hEdit0;
    WM_HWIN hWin = pMsg->hWin;
    switch(pMsg->MsgId)
    {
        case WM_INIT_DIALOG:
			hEdit0 = WM_GetDialogItem(hWin, GUI_ID_EDIT0);
			EDIT_SetText(hEdit0,"No button down!");
			break;
		case WM_KEY:			
			switch(((WM_KEY_INFO*)(pMsg->Data.p))->Key)
			{
				case GUI_ID_ESCAPE:
					GUI_EndDialog(hWin, 1); 
					break;
				case GUI_ID_ENTER:
					GUI_EndDialog(hWin, 0); 
					break;
				case GUI_KEY_F1:
					Uart_Printf("Testing key1 down\n");
					break;
			}
			break;
		case WM_NOTIFY_PARENT:			
			Id = WM_GetId(pMsg->hWinSrc); /* 控件的 Id */
			NCode = pMsg->Data.v; /* 通知代码 */			
			switch(NCode)
			{
				case WM_NOTIFICATION_RELEASED/*WM_NOTIFICATION_CLICKED*/: /* 如果释放则起作用 */
					
					if(Id == TEST_BUTTON1_ID)
					{
						Uart_Printf("Button1 down\n");						
					}
					if(Id == TEST_BUTTON2_ID)
					{
						Uart_Printf("Button2 down\n");						
					}					
					break;
				case WM_NOTIFICATION_SEL_CHANGED: /* 改变选择 */
					FRAMEWIN_SetText(hWin, "Dialog - sel changed");
					break;
				default:
					FRAMEWIN_SetText( hWin,"Dialog-notification received");
			}
			break;
		default:
            WM_DefaultProc(pMsg);
    }
}

void zgqDialogTest(void)
{
	GUI_ExecDialogBox( _aDialogCreate,GUI_COUNTOF(_aDialogCreate),&_cbCallback,0, 0, 0);
}
