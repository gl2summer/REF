#ifndef _LCD_DRIVER_H_
#define _LCD_DRIVER_H_

#include <raw_api.h>
#include <project_def.h>
/*********************************配置宏***************************************/

enum
{
	WIN0_BUF0,
	WIN0_BUF1,
	WIN1_BUF0
};

#define _TEST_WHICH_WIN_AND_BUF WIN0_BUF0

//窗口
#define LCD_WIN_0					0
#define LCD_WIN_1					1
#define LCD_WIN_ALL					2
//显示缓存
#define LCD_BUF_0					0
#define LCD_BUF_1					1
//窗口设置开关
#define LCD_OFF						0
#define LCD_ON						1
//RGB 信号参数

#if (LCD_SIZE_4_3 == 1)

#define LCD_VFPD		(3)
#define LCD_VSPW		(1)
#define LCD_VBPD		(13)
#define LCD_HFPD		(5)
#define LCD_HSPW		(20)
#define LCD_HBPD		(23)
#define LCD_HOZVAL		(480)
#define LCD_LINEVAL		(272)

#else 

#define LCD_VFPD		(22)
#define LCD_VSPW		(10)
#define LCD_VBPD		(23)
#define LCD_HFPD		(210)
#define LCD_HSPW		(20)
#define LCD_HBPD		(46)
#define LCD_HOZVAL		(800)
#define LCD_LINEVAL		(480)

#endif



#define LCD_FRAME_RATE	(60)
//16bpp 基色
#define RED_16BPP 	(0xf800)
#define BLUE_16BPP 	(0x7e0)
#define GREEN_16BPP (0x1f)
/*配置数据上屏时，是否选用缓冲区(1为使用，0为不用)*/
#define USE_FRAMEBUFF (0)

/*********************************配置宏结束***********************************/

/*************************对外函数申明开始*************************************/
void lcd_init(void);
void lcd_test(void);
void LCD_fill_rectangle( RAW_U32 pos_x1, RAW_U32 pos_y1, RAW_U32 pos_x2, RAW_U32 pos_y2, RAW_U8 image_flg);
void LCD_one_pixel_write( RAW_U32 pos_x, RAW_U32 pos_y, RAW_U32 color);
RAW_U32 LCD_one_pixel_read( RAW_U32 pos_x, RAW_U32 pos_y);
void LCD_clr(void);
void LCD_drawing_Vertical_line( RAW_U32 pos_x, RAW_U32 pos_y1, RAW_U32 pos_y2,RAW_U32 color);
void LCD_drawing_Horizon_line( RAW_U32 pos_y, RAW_U32 pos_x1, RAW_U32 pos_x2,RAW_U32 color);
void LCD_fill_rectangle_color( RAW_U32 pos_x1, RAW_U32 pos_y1, RAW_U32 pos_x2, RAW_U32 pos_y2, RAW_U32 color);
void LCD_ShowBitmap(const void * bitmap);
void LCD_SetBitmap(int x, int y, RAW_U8 const *p, int Diff, int xsize, const RAW_U16 *pTrans);
void Display_Start(RAW_U8 win_num);
void Display_End(RAW_U8 win_num);


/*************************对外函数申明开结束***********************************/

#endif
