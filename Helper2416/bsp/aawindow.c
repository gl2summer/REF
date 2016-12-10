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
#include "multiedit.h"
#include "messagebox.h"

extern const unsigned char gImage_hua[261120];
extern const unsigned char gImage_1[180000];
extern const unsigned char gImage_2[180000];
extern const unsigned char gImage_3[180000];
extern const unsigned char gImage_4[180000];


static const GUI_BITMAP __bitmap[] = {
	{ 400, 225,	400, 16, (RAW_U8*)gImage_1,	NULL, GUI_DRAW_BMP565},
	{ 400, 225, 400, 16, (RAW_U8*)gImage_2, NULL, GUI_DRAW_BMP565},
	{ 400, 225, 400, 16, (RAW_U8*)gImage_3, NULL, GUI_DRAW_BMP565},
	{ 400, 225, 400, 16, (RAW_U8*)gImage_4, NULL, GUI_DRAW_BMP565},
	{ 480, 272, 480, 16, (RAW_U8*)gImage_hua, NULL, GUI_DRAW_BMP565}
};

int position = 0;	
char buffer[80] = {0};
int ret0 = 0;
char tmp0[1024] = {0};



#define NUM(Array) (sizeof(Array) / sizeof(Array[0]))
static const GUI_ConstString ListBoxtable[] = {
	"ADC_TS_TEST", 
	"DrawWindow",
	NULL
};

//----- 桌面对话框 -----//
static const GUI_WIDGET_CREATE_INFO aDialogCreate_DeskTop[] = 
{
    //无父窗体，坐标0，0，窗体大小为全屏幕，不能移动
    {FRAMEWIN_CreateIndirect, "DeskTop", 0, 0, 0, 480, 272, FRAMEWIN_CF_ACTIVE, 0},
//    {BUTTON_CreateIndirect,"Back", GUI_ID_BUTTON1,400, 230, 60, 30, WM_CF_SHOW,0}
};
//Welcome 对话框资源表
static const GUI_WIDGET_CREATE_INFO aDialogCreate[] = 
{
	{ FRAMEWIN_CreateIndirect,	"Photo Book", 	0, 0, 0, 480, 272, WM_CF_SHOW | WM_CF_ANCHOR_LEFT, 0},
	{ LISTBOX_CreateIndirect,	NULL, 	GUI_ID_LISTBOX0, 10, 20, 100, 200, WM_CF_SHOW, 0},
	{ BUTTON_CreateIndirect,	"View", GUI_ID_BUTTON0, 120, 140, 80, 40, WM_CF_SHOW, 0},
	{ BUTTON_CreateIndirect,	"Edit", GUI_ID_BUTTON1, 230, 210, 60, 30, WM_CF_SHOW, 0},
	{ BUTTON_CreateIndirect,	"Save", GUI_ID_BUTTON2, 310, 210, 60, 30, WM_CF_SHOW, 0},
	{ BUTTON_CreateIndirect,	"Clr", 	GUI_ID_BUTTON3, 390, 210, 60, 30, WM_CF_SHOW, 0},
	{ BUTTON_CreateIndirect,	"Add",	GUI_ID_BUTTON4, 10, 230, 40, 20, WM_CF_SHOW, 0},
	{ BUTTON_CreateIndirect,	"Del",	GUI_ID_BUTTON5, 60, 230, 40, 20, WM_CF_SHOW, 0},
	{ BUTTON_CreateIndirect,	"Rename",	GUI_ID_BUTTON6, 110, 230, 60, 20, WM_CF_SHOW, 0},
	{ RADIO_CreateIndirect,		NULL, 	GUI_ID_RADIO0, 120, 40, 100, 50, WM_CF_SHOW, 2},
	{ TEXT_CreateIndirect,		"Welcome to Jack's world!", GUI_ID_TEXT0, 0, 0, 200, 20, WM_CF_SHOW, 0},
	{ MULTIEDIT_CreateIndirect, NULL,	GUI_ID_MULTIEDIT0, 220, 20, 240, 180, WM_CF_SHOW, 0}
};

/* 桌面相关句柄 */
WM_HWIN aDialog_DeskTop;    //桌面对话框句柄
WM_HWIN BackButton;

/* Welcome 窗口相关句柄 */
WM_HWIN WelcomeWinHandle;
WM_HWIN ListboxChoose;
WM_HWIN ButtonView;
WM_HWIN ButtonEdit;
WM_HWIN ButtonSave;
WM_HWIN ButtonClr;
WM_HWIN ButtonAdd;
WM_HWIN ButtonDel;
WM_HWIN ButtonRename;
WM_HWIN ButtonUp;
WM_HWIN ButtonDown;
WM_HWIN RadioSel;
WM_HWIN TextWelcome;
WM_HWIN MultieditHandle;
WM_HWIN ScrollbarHandle;
WM_HWIN MessageBoxHandle1;
WM_HWIN MessageBoxHandle2;
WM_HWIN MessageBoxHandle3;

/*新建 win相关句柄, 用于显示照片*/
WM_HWIN hWin;


char const *radio_text[16] = {"Picture", "PowerPoint"};


/****************************************************************************************************************** 
*                                              void DeskTop_uCGUI(void)
*     描述：  桌面环境
*     参数：  无
*   返回值：  无
******************************************************************************************************************/
static void DeskTop_CallBack(WM_MESSAGE *pMsg);
void cbfunc(WM_MESSAGE *pMsg);


void DeskTop(void)
{
    //----- 建立桌面 -----//
    aDialog_DeskTop = GUI_CreateDialogBox(aDialogCreate_DeskTop, GUI_COUNTOF(aDialogCreate_DeskTop), DeskTop_CallBack, 0, 0, 0);
    FRAMEWIN_SetFont(aDialog_DeskTop, &GUI_FontComic18B_1);	  //对话框字体设置 
//	BackButton = WM_GetDialogItem(aDialog_DeskTop, GUI_ID_BUTTON1);
//	FRAMEWIN_AddMaxButton(aDialog_DeskTop,   FRAMEWIN_BUTTON_RIGHT, 0);		//添加窗体最大化按钮
//	FRAMEWIN_AddMinButton(aDialog_DeskTop,   FRAMEWIN_BUTTON_RIGHT, 0);		//添加窗体最小化按钮
}


/****************************************************************************************************************** 
*                                     static void DeskTop_uCGUI(WM_MESSAGE *pMsg)
*     描述：  桌面回调函数
*     参数：  无
*   返回值：  无
******************************************************************************************************************/
static void DeskTop_CallBack(WM_MESSAGE *pMsg)
{
    RAW_U16 NCode;//, Id;
    WM_HWIN hDlg;
//	Uart_Printf("%d\t",pMsg->MsgId);
    hDlg = pMsg->hWin;
//	Uart_Printf("%d\n",WM_GetId(pMsg->hWinSrc));

    switch (pMsg->MsgId)                  //获取时间ID号
    {
        case WM_INIT_DIALOG   : break;    //对话框初始化事件

        case WM_KEY           : break;

        case WM_PAINT         : GUI_Clear(); break;               //窗体重绘

        case WM_NOTIFY_PARENT : 
//			Id    = WM_GetId(pMsg->hWinSrc);  //获取控件Id
//	        NCode = pMsg->Data.v;             //通知代码
//	        Uart_Printf("%d\n",Id);
	                 
	        switch(NCode)
	        {
	            case WM_NOTIFICATION_RELEASED   : 
					break;
				case WM_NOTIFICATION_CLICKED:
/*					if (Id == GUI_ID_BUTTON7){
						
						WM_HideWindow(aDialog_DeskTop);
						Uart_Printf("enter");
						WM_ShowWindow(WelcomeWinHandle);
						WM_SelectWindow(WelcomeWinHandle);
						WM_Paint(WelcomeWinHandle);
//						WM_Activate();
//						WM_Exec();
//						WM_DeleteWindow(WelcomeWinHandle);
//						WelcomeWinHandle = GUI_CreateDialogBox(aDialogCreate, NUM(aDialogCreate), &cbfunc, 0, 0, 0);
	                }
*/					break;
	            case WM_NOTIFICATION_SEL_CHANGED :
					break;
	        }
        	break; 
		case WM_TOUCH:
/*			Uart_Printf("BackButton Pressed is %d\n",BUTTON_IsPressed(BackButton));
			WM_HideWindow(aDialog_DeskTop);
			WM_ShowWindow(WelcomeWinHandle);
			WM_SelectWindow(WelcomeWinHandle);
			FRAMEWIN_SetActive(WelcomeWinHandle, WM_CF_ACTIVATE | WM_CF_ANCHOR_TOP);
			WM_SetFocus(WelcomeWinHandle);
			WM_BringToBottom(aDialog_DeskTop);
			WM_BringToTop(WelcomeWinHandle);
			FRAMEWIN_SetActive(WelcomeWinHandle, WM_CF_FGND | WM_CF_ACTIVATE | WM_CF_ANCHOR_TOP);
			NCode = pMsg->Data.v;
			Uart_Printf("enter\n");
			Uart_Printf("pMsg->hWinSrc = %d\t",pMsg->hWinSrc);
			Uart_Printf("BackButton = %d\t",BackButton);
			Uart_Printf("NCode = %d\t",NCode);
			Uart_Printf("pMsg->MsgId = %d\n",pMsg->MsgId);
			if(pMsg->hWinSrc == BackButton){
				WM_HideWindow(aDialog_DeskTop);
				
				GUI_DispCharAt('a', 100, 100);
				WM_ShowWindow(WelcomeWinHandle);
				WM_SelectWindow(WelcomeWinHandle);
				Uart_Printf("Welcome Window Show\n");
			}
*/			break;
/*		case WM_PID_STATE_CHANGED:
			
			Id	  = WM_GetId(pMsg->hWinSrc);  //获取控件Id
			NCode = pMsg->Data.v;			  //通知代码
			Uart_Printf("%d\n",Id);
					 
			switch(NCode)
			{
				case WM_NOTIFICATION_RELEASED	: 
					break;
				case WM_NOTIFICATION_CLICKED:
					if (Id == GUI_ID_BUTTON1){
						
						WM_HideWindow(aDialog_DeskTop);
						Uart_Printf("enter");
						GUI_DispCharAt('a', 100, 100);
						WM_ShowWindow(WelcomeWinHandle);
						WM_SelectWindow(WelcomeWinHandle);
						WM_Activate();
//						WM_Exec();
//						WM_DeleteWindow(WelcomeWinHandle);
//						WelcomeWinHandle = GUI_CreateDialogBox(aDialogCreate, NUM(aDialogCreate), &cbfunc, 0, 0, 0);
					}
				break;
			}
			break;

   */     default: WM_DefaultProc(pMsg);  
    }
}
void wm_callback(WM_MESSAGE *pMsg)
{
	RAW_U16 NCode, Id;
//	RAW_U8 str[1024] = {0};
	WM_HWIN hDlg;

	switch(pMsg->MsgId){
		case WM_NOTIFY_PARENT:{
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;  
//			listboxsel = LISTBOX_GetSel(ListboxChoose);
//			Uart_Printf("%d\n",Id);
			switch(NCode) {
				case WM_NOTIFICATION_RELEASED :{
					switch(Id) {
						case GUI_ID_BUTTON7: {
							WM_DeleteWindow(hWin);
//						GUI_DrawBitmap(&__bitmap1, 60, 5);
						}  //GUI_ID_BUTTON7
						break;
						case GUI_ID_BUTTON8: {
							if (position > 0) {
								--position;
								GUI_Clear();
								GUI_DrawBitmap(&__bitmap[position], 40, 20);
							}
						}//GUI_ID_BUTTON8
						break;
						case GUI_ID_BUTTON9: {
							if (position < NUM(__bitmap) - 1) {
								++position;
								GUI_Clear();
								GUI_DrawBitmap(&__bitmap[position], 40, 20);
							}
						}//GUI_ID_BUTTON7
						break;
					}
					break;
				}//WM_NOTIFICATION_RELEASED
				case WM_NOTIFICATION_CLICKED :{
					}//WM_NOTIFICATION_CLICKED
					break;
					
				}
			}//WM_NOTIFY_PARENT
			break;
		default: WM_DefaultProc(pMsg);
	}
}

void cbfunc(WM_MESSAGE *pMsg)
{
	
    RAW_U16 NCode, Id, radio_value = 0, loop = 0;//, listboxsel;
//	RAW_U8 str[1024] = {0};
    WM_HWIN hDlg;

    hDlg = pMsg->hWin;
		
	switch(pMsg->MsgId) {
		case WM_PAINT:
			GUI_Clear();
			break;
		case WM_NOTIFY_PARENT:
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;  
			radio_value = RADIO_GetValue(RadioSel);
			Uart_Printf("radio_value = %d\n",radio_value);
			if (0 == radio_value)
				MULTIEDIT_SetText(MultieditHandle, radio_text[radio_value]);
			if (1 == radio_value)
				MULTIEDIT_SetText(MultieditHandle, radio_text[radio_value]);
//			listboxsel = LISTBOX_GetSel(ListboxChoose);
//			Uart_Printf("%d\n",Id);
			switch(NCode) {
				case WM_NOTIFICATION_RELEASED :
					switch(Id){
						case GUI_ID_BUTTON0: {//ButtonView
								Uart_Printf("Show Photo\n");
								hWin = WM_CreateWindowAsChild(0, 0, 480, 272, WelcomeWinHandle, WM_CF_SHOW | WM_CF_STAYONTOP | WM_CF_ACTIVATE, &wm_callback, 1024*1024);
								GUI_Clear();
								BackButton = BUTTON_CreateAsChild(430, 250, 40, 20, hWin, GUI_ID_BUTTON7, WM_CF_SHOW);
								BUTTON_SetText(BackButton, "Back");
								if (0 == radio_value) {
									ButtonUp = BUTTON_CreateAsChild(10, 110, 20, 30, hWin, GUI_ID_BUTTON8, WM_CF_SHOW);
									ButtonDown = BUTTON_CreateAsChild(450, 110, 20, 30, hWin, GUI_ID_BUTTON9, WM_CF_SHOW);
									BUTTON_SetText(ButtonUp, "<");
									BUTTON_SetText(ButtonDown, ">");
									position = 0;
									GUI_DrawBitmap(&__bitmap[position], 40, 20);
									}
								else if( 1 == radio_value) {
									BUTTON_Delete(ButtonUp);
									BUTTON_Delete(ButtonDown);
									while(loop < NUM(__bitmap)) {
//										loop = loop % NUM(__bitmap);
										GUI_DrawBitmap(&__bitmap[loop++], 40, 20);
										raw_sleep(100);
									}
								}
/*								WM_SelectWindow(aDialog_DeskTop);
								
								FRAMEWIN_SetActive(aDialog_DeskTop, WM_CF_ACTIVATE | WM_CF_ANCHOR_TOP);
								WM_SetFocus(aDialog_DeskTop);
								Uart_Printf("Hide Welcome Window\n");
//								LCD_ShowBitmap(gImage_hua);
*/							}
							break;
						case GUI_ID_BUTTON1: {//ButtonEdit
								Uart_Printf("Enable Edit Photo Info text\n");
								WM_SetFocus(MultieditHandle);
								MULTIEDIT_SetReadOnly(MultieditHandle,0);
								MULTIEDIT_SetInsertMode(MultieditHandle, 1);
//								MULTIEDIT_SetText(MultieditHandle, buffer);
							}
							break;
						case GUI_ID_BUTTON2: {//ButtonSave
								Uart_Printf("Save Photo Info text\n");
								MULTIEDIT_SetInsertMode(MultieditHandle, 0);
								MULTIEDIT_SetReadOnly(MultieditHandle,1);
							}
							break;
						case GUI_ID_BUTTON3: {//ButtonClear
								Uart_Printf("Clear Photo Info text\n");
								MessageBoxHandle1 = MESSAGEBOX_Create("Are you sure to \nclear the photo info?\n","Warning", 0);
								FRAMEWIN_AddCloseButton(MessageBoxHandle1, FRAMEWIN_BUTTON_RIGHT, 0);        //添加窗体关闭按钮
								FRAMEWIN_AddMaxButton(MessageBoxHandle1,   FRAMEWIN_BUTTON_RIGHT, 0);		//添加窗体最大化按钮
								FRAMEWIN_AddMinButton(MessageBoxHandle1,   FRAMEWIN_BUTTON_RIGHT, 0);		//添加窗体最小化按钮
								FRAMEWIN_SetFont(MessageBoxHandle1, &GUI_Font8x12_ASCII);
								MULTIEDIT_SetReadOnly(MultieditHandle,0);
								MULTIEDIT_SetText(MultieditHandle, " ");
								
							}
							break;
						case GUI_ID_BUTTON4: {
								Uart_Printf("Add a picture to the Listbox\n");
								LISTBOX_AddString(ListboxChoose, "newname");
								MULTIEDIT_SetText(MultieditHandle, "Add a picturn \"newname\" to Listbox\n");
									
							}
							break;
						case GUI_ID_BUTTON5: {
								Uart_Printf("Delete a picture from the Listbox\n");
								MessageBoxHandle2 = MESSAGEBOX_Create("Are you sure to Delete\nthe picture?\n","Warning", 0);
								
								LISTBOX_DeleteItem(ListboxChoose, LISTBOX_GetSel(ListboxChoose));
								MULTIEDIT_SetText(MultieditHandle, "Delete the picture from the Listbox\n");
								
							}
							break;
						case GUI_ID_BUTTON6: {
								Uart_Printf("Change a picture name of the Listbox\n");
								MessageBoxHandle3 = MESSAGEBOX_Create("Are you sure to change\nthe name?\n","Warning", 0);
								
								LISTBOX_SetString(ListboxChoose, "change name", LISTBOX_GetSel(ListboxChoose));
								MULTIEDIT_SetText(MultieditHandle, "Change the picture name\n");
									
							}
							break;
/*						case GUI_ID_RADIO0: {//Set View mode
								radio_value = RADIO_GetValue(GUI_ID_RADIO0);
								Uart_Printf("radio_value = %d",radio_value);
								if (0 == radio_value)
									MULTIEDIT_SetText(MultieditHandle, radio_text[radio_value]);
								if (1 == radio_value)
									MULTIEDIT_SetText(MultieditHandle, radio_text[radio_value]);
							}	
							break;
*/					}
					break;
				case WM_NOTIFICATION_CLICKED :
/*					case GUI_ID_CLOSE:{
						MessageBoxHandle = MESSAGEBOX_Create("WarningBox",\
							"If you click the \n\"OK\" key, the \n\"Welcome\" window\nwill Closed\n", 1);
						if(WM_GetId(pMsg->hWinSrc) == GUI_ID_OK)
							WM_DeleteWindow(WelcomeWinHandle);
						
						Uart_Printf("Just close the welcome window\n");
						}
						break;
					}
*/					break;
				}
			break;




		case WM_TOUCH:
/*			if(pMsg->MsgId == GUI_ID_BUTTON0)
				GUI_DispStringAt("That go! Come on!", 100, 200);
*/			break;
		case WM_DELETE:
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}


void WindowInit(void)
{
	GUI_PID_STATE PointState;
//	int num = 0;
	DeskTop();
	WelcomeWinHandle = GUI_CreateDialogBox(aDialogCreate, NUM(aDialogCreate), &cbfunc, 0, 0, 0);
//	BackButton= BUTTON_CreateAsChild(400, 230, 60, 30, aDialog_DeskTop, GUI_ID_BUTTON7, WM_CF_SHOW);
//	BackButton= BUTTON_CreateAsChild(400, 230, 60, 30, aDialog_DeskTop, GUI_ID_BUTTON7, WM_CF_SHOW);
//	BUTTON_SetText(BackButton, "Back");
	FRAMEWIN_SetFont(WelcomeWinHandle, &GUI_Font16B_ASCII);
	FRAMEWIN_AddCloseButton(WelcomeWinHandle, FRAMEWIN_BUTTON_RIGHT, 0);        //添加窗体关闭按钮
    FRAMEWIN_AddMaxButton(WelcomeWinHandle,   FRAMEWIN_BUTTON_RIGHT, 0);        //添加窗体最大化按钮
    FRAMEWIN_AddMinButton(WelcomeWinHandle,   FRAMEWIN_BUTTON_RIGHT, 0);        //添加窗体最小化按钮
    ListboxChoose = WM_GetDialogItem(WelcomeWinHandle, GUI_ID_LISTBOX0);
	ButtonView = WM_GetDialogItem(WelcomeWinHandle, GUI_ID_BUTTON0);
	ButtonEdit = WM_GetDialogItem(WelcomeWinHandle, GUI_ID_BUTTON1);
	ButtonSave = WM_GetDialogItem(WelcomeWinHandle, GUI_ID_BUTTON2);
	ButtonClr  = WM_GetDialogItem(WelcomeWinHandle, GUI_ID_BUTTON3);
	ButtonAdd  = WM_GetDialogItem(WelcomeWinHandle, GUI_ID_BUTTON4);
	ButtonDel  = WM_GetDialogItem(WelcomeWinHandle, GUI_ID_BUTTON5);
	ButtonRename = WM_GetDialogItem(WelcomeWinHandle, GUI_ID_BUTTON6);
	RadioSel     = WM_GetDialogItem(WelcomeWinHandle, GUI_ID_RADIO0);
	TextWelcome  = WM_GetDialogItem(WelcomeWinHandle, GUI_ID_TEXT0);
	MultieditHandle = WM_GetDialogItem(WelcomeWinHandle, GUI_ID_MULTIEDIT0);
    ScrollbarHandle = SCROLLBAR_CreateAttached(ListboxChoose, WM_CF_SHOW | SCROLLBAR_CF_VERTICAL);
	RADIO_SetText(RadioSel, radio_text[0], 0);
	RADIO_SetText(RadioSel, radio_text[1], 1);
	BUTTON_SetBkColor(ButtonClr, 0, GUI_BLUE);
	LISTBOX_SetText(ListboxChoose, ListBoxtable);
	MULTIEDIT_SetMaxNumChars(MultieditHandle, 80);
//	MULTIEDIT_SetText(MultieditHandle, "Photo Info");
	MULTIEDIT_SetBufferSize(MultieditHandle, 80);
	MULTIEDIT_SetAutoScrollV(MultieditHandle, 0);
//	HEADER_SetBitmap(aDialog_DeskTop, 1, &__bitmap);
//	FRAMEWIN_SetActive(WelcomeWinHandle, WM_CF_ACTIVATE | WM_CF_ANCHOR_TOP);

	while(1) {
//		ret = Uart_0_ReceiveString(buffer, 80, 3);
/*		while(num + ret0 <= 80){ 
			raw_memcpy(buffer + num, tmp0, ret0);
			if ( num > 0)
				MULTIEDIT_SetText(MultieditHandle, buffer);
			num += ret0;
			}
		
		if ( num > 0)
			MULTIEDIT_SetText(MultieditHandle, buffer);
		Uart_Printf("%s\n",buffer);
		raw_memset(buffer, 0, 80);
		num = 0;
*/		if (0 == WM_GetActiveWindow())
			if (GUI_PID_GetState(&PointState))
				WM_ShowWindow(WelcomeWinHandle);
		GUI_TOUCH_Exec();
		WM_Exec();
/*		if(BUTTON_IsPressed(BackButton)) {
			WelcomeWinHandle = GUI_CreateDialogBox(aDialogCreate, NUM(aDialogCreate), &cbfunc, 0, 0, 0);
		
			WM_Activate();
			WM_SetFocus(WelcomeWinHandle);
			WM_Activate();
			}
*/		raw_sleep(1);
	}
}

#define  GUI_TASK_STK_SIZE 10240



PORT_STACK gui_task_stack1[GUI_TASK_STK_SIZE];
PORT_STACK gui_task_stack2[GUI_TASK_STK_SIZE];
//PORT_STACK gui_task_stack3[GUI_TASK_STK_SIZE];
//PORT_STACK gui_task_stack4[GUI_TASK_STK_SIZE];

RAW_TASK_OBJ 	test_gui_obj[10];

void ucgui_task(void * pParam) ;
void RePaint(void *pParam);
void ucgui_task (void * pParam)
{
	GUI_Init();
	Display_Start(LCD_WIN_0);
//	LCD_clr();
//	LCD_ShowBitmap(gImage_hua);
//	while(1)
		WindowInit();
	
}

void RePaint(void *pParam)
{

	while(1)
	{
		WM_Exec();
//		if (touch_bool)
			GUI_TOUCH_Exec();
//		ret0 = Uart_0_ReceiveString((void *)tmp0, 1024, 3);

		raw_sleep(5);
	}

}


void start_ucgui_test(void)
{
	
	Uart_Printf("ucgui_test\n");
	raw_task_create(&test_gui_obj[1], (RAW_U8 *)"ucgui_test", 0, 10, 0, gui_task_stack1, GUI_TASK_STK_SIZE, ucgui_task, 1);
	raw_task_create(&test_gui_obj[2], (RAW_U8 *)"Exec()"	, 0, 9, 10, gui_task_stack2, GUI_TASK_STK_SIZE, RePaint, 1);

}

