#include "GUI.h"
#include "GUI_X.h"
#include "FRAMEWIN.h"
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "raw_api.h"
#include "lcd_driver.h"
#include "button.h"
#include "somefunc.h"

/*************************************LCD_ADC_test()*********************************
 *call lcdtest() to test the LCD is "OK" or not
 ************************************************************************************/
 
/* DISTANCE used for calculate the distance between two point P1(x0, y0) and P2(xx, yy) */  
#define DISTANCE(x0,y0,xx,yy) (((xx)-(x0))*((xx)-(x0))+((yy)-(y0))*((yy)-(y0)))
extern const unsigned char gImage_hua[261120];
BUTTON_Handle hbutton;
int ret;
/*
static void cbcallback(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId) {
		case WM_TOUCH:
			LCD_ShowBitmap(gImage_hua);
			break;
		case WM_PAINT:
			GUI_SetBkColor(GUI_GREEN);
			break;
		default:
			WM_DefaultProc(pMsg);
	}
}

*/
#define TEST 1
static unsigned char lcdtest(void)
{
#if TEST
	GUI_PID_STATE pid[5];
//	GUI_PID_STATE EscState;
	GUI_POINT point[5] = {
		{20, 20}, {460, 20}, {20, 252}, {460, 252}, {240, 136}
	};
	char *testpoint[] = {
		"LEFTUP",
		"RIGHTUP",
		"LEFTDOWN",
		"RIGHTDOWN",
		"MIDDLE"
	};
	RAW_U8 i, j, result=0;
	RAW_U16 r = 10;
	
//	Uart_Printf("ADC_Ts and LCD test is start:\n\n");
	GUI_DispStringAt("ADC_Ts and LCD test is start:", 50, 10);
	for(i = 0; i < 5; i++) {
		GUI_DrawCircle(point[i].x, point[i].y, r);
		GUI_DrawLine(point[i].x - (r/2+1), point[i].y, point[i].x + (r/2+1), point[i].y);
		GUI_DrawLine(point[i].x, point[i].y - (r/2+1), point[i].x, point[i].y + (r/2+1));
	}
//		WM_Exec();
	for(i = 0; i < 5; i++) {
//			Uart_Printf("%10s test, please click the %10s circle\n", testpoint[i], testpoint[i]);
			GUI_SetColor(GUI_WHITE);
			GUI_DispStringAt(testpoint[i], 50, 30);
			GUI_DispString(" test, please click the ");
			GUI_DispString(testpoint[i]);
			GUI_DispString(" circle");
			GUI_TOUCH_Exec();

	get_point:
		do{
			GUI_TOUCH_Exec();
			GUI_TOUCH_GetState(&pid[i]);
//			Uart_Printf("Point (%04d,%04d) state is %s\n",pid[i].x, pid[i].y, (pid[i].Pressed == 1? "Prewssed":"Up"));
		} while(!(pid[i].Pressed)); 
//		Uart_Printf("Point (%04d,%04d) state is %s\n",pid[i].x, pid[i].y, (pid[i].Pressed == 1? "Prewssed":"Up"));
		if((DISTANCE(point[i].x, point[i].y, pid[i].x, pid[i].y) - r*r) <= 0){
//			Uart_Printf("%s tested is Correct\n", testpoint[i]);
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(50, 200, 400, 260);
			GUI_SetColor(GUI_RED);
			GUI_DispStringAt(testpoint[i], 50, 200);
			GUI_DispString(" tested is Correct");
			GUI_DrawCircle(point[i].x, point[i].y, r);
			GUI_DrawLine(point[i].x - (r/2+1), point[i].y, point[i].x + (r/2+1), point[i].y);
			GUI_DrawLine(point[i].x, point[i].y - (r/2+1), point[i].x, point[i].y + (r/2+1));
			result |= 0x01 << i;
//			raw_sleep(200);
		} else {
//			Uart_Printf("%s tested is not Correct\n", testpoint[i]);
//			Uart_Printf("please click the %s circle again\n", testpoint[i]);
			GUI_FillRect(50, 200, 400, 260);
			GUI_DispStringAt(testpoint[i], 50, 200);
			GUI_DispString(" tested is not Correct");
			GUI_DispStringAt("please click the %s circle again\n", 50, 230);
			goto get_point;
		}
		if (!(result^0x1f)) {
//			Uart_Printf("Test finished, ADC_Ts and LCD is OK\n\n");
			GUI_Clear();
			GUI_SetColor(GUI_RED);
			GUI_DispStringAt("Test finished, ADC_Ts and LCD is OK!", 50, 10);
			GUI_Clear();
			LCD_ShowBitmap(gImage_hua);
			GUI_SetPenSize(5);
			for (j = 0; j < 1000; j++) {
/*				GUI_SetColor(GUI_RED);
				GUI_DispStringAt("Test is OK!\n", 20, 30);
//				raw_sleep(10);
				GUI_SetColor(GUI_GREEN);
				GUI_DispStringAt("Test is OK!\n", 20, 30);
*///				raw_sleep(10);
#endif					
//do{					
/*		
				hbutton = BUTTON_Create(100, 100, 200, 80, GUI_ID_BUTTON0, WM_CF_SHOW | WM_CF_FGND | WM_CF_ACTIVATE);
				BUTTON_SetTextColor(hbutton, 0, GUI_BLUE);
				BUTTON_SetBkColor(hbutton, 0, GUI_WHITE);
				BUTTON_SetTextColor(hbutton, 1, GUI_RED);
				BUTTON_SetBkColor(hbutton, 1, GUI_GREEN);
				BUTTON_SetFont(hbutton, &GUI_Font16B_ASCII);
				BUTTON_SetText(hbutton, "OK");
				GUI_TOUCH_Exec();
				ret = BUTTON_IsPressed(hbutton);
				Uart_Printf("ret = %d\n",ret);
				if (ret) {
					WM_SelectWindow(0);
					GUI_DispString("finished");
					}
*/				WM_Exec();//WM_Paint(hbutton);
//}while(1);
#if TEST				
			}
			return 1;
		}
	}
	return 0;			
#endif
}

void LCD_ADC_test (void )
{
	static unsigned char flag = 0;
	if (!flag)
		flag = lcdtest();
}



/********************************KeyboardEntry**********************************
 *Function used for get the keyboard input, and send to LCD screem
 *Call nextline() function turn to next line first, like "Enter" key
 *Call backclean() function to clean a character, like "Backspace" key
 *Set edit window size:
 *	Redefine LINEDIST to set the distance between two line
 *	Redefine LEFTOFF to set the offset of left side
 *	Redefine RIGHTOFF to set the offset of right side
 *	Redefine UPOFF to set the offset of up side
 *	Redefine DOWNOFF to set the offset of down side
 *******************************************************************************/
 

#include "LCDConf.h"
#include "uart.h"

/******************define for set edit window*******************************/
#ifndef LINEDIST
	#define LINEDIST	0
#endif

#ifndef LEFTOFF
	#define LEFTOFF		0
#endif

#ifndef RIGHTOFF
	#define RIGHTOFF	0
#endif

#ifndef UPOFF
	#define UPOFF		5
#endif
#ifndef DOWNOFF
	#define DOWNOFF		12
#endif

#define HLEN	(LCD_XSIZE - RIGHTOFF - LEFTOFF)	//水平长度
#define VLEN	(LCD_YSIZE - DOWNOFF - UPOFF)		//竖起高度

/****************************************************************************/

static void nextline(GUI_POINT *posxy)
{
	GUI_FONT *pFont;
	pFont = (GUI_FONT *)GUI_GetFont();
	posxy->y += pFont->YSize + LINEDIST;
	posxy->x = LEFTOFF;
	GUI_GotoXY(posxy->x, posxy->y);
}

static void backclean(GUI_POINT *posxy)
{	
	GUI_FONT *pFont;
	int x = posxy->x;
	GUI_COLOR StoreForgroundColor = GUI_GetColor();
	
	pFont = (GUI_FONT *)GUI_GetFont();	//get Font
	
	posxy->x -= pFont->p.pMono->XSize;
//	GUI_GotoX(posxy->x);
	posxy->x = posxy->x > LEFTOFF ? posxy->x : LEFTOFF;
	GUI_SetColor(GUI_GetBkColor());
	GUI_DispChar(' ');
	GUI_FillRect(posxy->x, posxy->y, x, posxy->y + pFont->YSize + LINEDIST); // LINEDIST表示行间距
	GUI_SetColor(StoreForgroundColor);
	GUI_GotoXY(posxy->x, posxy->y);
}



void KeyboardInputInit(void)
{
	char get[1024] = {0};
	int cnt = 0;
	int ret = 0;
	GUI_POINT CursorPosition;					//光标位置: CursorPosition实时光标
	GUI_FONT *pFont;
	GUI_POINT OldPosition;						//光标位置: OldPosition:第一次定位的光标
	static unsigned char flag = 1;
	pFont = (GUI_FONT *)GUI_GetFont();			//获取字体

	if(flag == 1) {
		OldPosition.x = GUI_GetDispPosX();
		OldPosition.y = GUI_GetDispPosY();
		OldPosition.x = OldPosition.x > LEFTOFF ? OldPosition.x : LEFTOFF;
		OldPosition.y = OldPosition.y > UPOFF ? OldPosition.y : UPOFF;
		GUI_GotoXY(OldPosition.x, OldPosition.y);
		flag = 0;
		Uart_Printf("OldPosition is (%d,%d)\t",OldPosition.x,OldPosition.y);
		Uart_Printf("LEFTUP point is (%d,%d)\n",LEFTOFF, UPOFF);
//		Uart_Printf("(%d,%d)(%d,%d)BytesPerLine=%d\n",pFont->p.pMono->XSize, pFont->YSize,pFont->p.pMono->XDist,pFont->YDist,pFont->p.pMono->BytesPerLine);
	}
	ret = Uart_0_ReceiveString((void *)get, 1024, 2);
	while(cnt < ret) {
		if ('\r' == *(get + cnt) || '\n' == *(get+ cnt)) {
			if ('\r' == *(get + cnt) && '\n' == *(get + cnt +1)) {
				++cnt;
			}
			CursorPosition.x = GUI_GetDispPosX();
			CursorPosition.y = GUI_GetDispPosY();
			nextline(&CursorPosition);
//			GUI_DispChar('\n');
			Uart_Printf("\n");
		} else if ('\b' == *(get + cnt)) {
			CursorPosition.x = GUI_GetDispPosX();
			CursorPosition.y = GUI_GetDispPosY();
			Uart_Printf("\b \b");
			if (LEFTOFF >= CursorPosition.x) {												//Cursor 在第一列的位置
				if(CursorPosition.y - UPOFF < (pFont->YSize + LINEDIST)){					//is fisrt line
					CursorPosition.x = pFont->p.pMono->XSize;
				} else {																	//not first line
					if (CursorPosition.y == OldPosition.y + (pFont->YSize + LINEDIST)) {	//is first input (Old+1)line
						CursorPosition.x = HLEN - ((HLEN - OldPosition.x) % pFont->p.pMono->XSize);	//求取第一次写入所在行的最后剩下的空白区
					} else {																//not first input (Old+1)line
						CursorPosition.x = HLEN - (HLEN % pFont->p.pMono->XSize);			//求取每行最后的空白区
					}
					CursorPosition.y = CursorPosition.y - (pFont->YSize + LINEDIST);
				} 
			}
			
			backclean(&CursorPosition);
//			GUI_DispChar('\b');
		} else {
			CursorPosition.y = GUI_GetDispPosY();
			CursorPosition.x = GUI_GetDispPosX();
			if ( CursorPosition.x + pFont->p.pMono->XSize > HLEN) {
				if(CursorPosition.y > VLEN - (pFont->YSize + LINEDIST)) {
					GUI_GotoX(CursorPosition.x - pFont->p.pMono->XSize);
				} else {
					nextline(&CursorPosition);
				}
			}
			GUI_DispChar(*(get + cnt));
			Uart_Printf("%c",*(get + cnt));
		}
		++cnt;
	}
}

