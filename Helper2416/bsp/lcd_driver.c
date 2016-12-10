/*
* Copyright (c) 2013
* All rights reserved.
* 
* 文件名称：lcd_driver.C
* 文件标识：
* 开发环境：C编译器
* 描    述：SMDK2416 4.3'inch LCD驱动代码
*			LCD pixel:480*272
*			LCD控制器设置成：16BPP输出
*			driver IC实际输入为16bpp(565)
* 当前版本：V1.2
* 作    者：
* 更改说明：
* 完成日期：2013年04月01日
*
* 修改记录 
*       作者        时间        版本            修改描述
*     章光泉      20130401      V1.0        	初建
*	  章光泉      20130408      V1.1        	修改初始化时序，修复第一版行像素不刷新问题
*	  章光泉      20130409      V1.2        	简化程序，增添API
*/
#include <string.h>
#include <stdio.h>
#include <lib_string.h>
#include <raw_api.h>
#include <irq.h>
#include "2416_reg.h"
#include "lcd_driver.h"
#include "LCD_Private.h" 
#include "LCD_Protected.h" 

#if 0  //移动了头文件中
/*********************************配置宏开始***********************************/
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
#define LCD_VFPD		(3)
#define LCD_VSPW		(1)
#define LCD_VBPD		(13)
#define LCD_HFPD		(5)
#define LCD_HSPW		(20)
#define LCD_HBPD		(23)
#define LCD_HOZVAL		(480)
#define LCD_LINEVAL		(272)
#define LCD_FRAME_RATE	(60)
//16bpp 基色
#define RED_16BPP 	(0xf800)
#define BLUE_16BPP 	(0x7e0)
#define GREEN_16BPP (0x1f)
/*********************************配置宏结束***********************************/
#endif


/*******************************本地函数申明开始*************************************/
extern RAW_U32 get_HCLK(void);

static void lcd_control_reg_init(void);
static void lcd_port_init(void);
static void Delay(int time);
static void Display_Setting( RAW_U32 win_num,RAW_U32 buf_num);
//static void Display_Start(RAW_U8 win_num);
//static void Display_End(RAW_U8 win_num);
static void LcdWindowOnOff(RAW_U8 num, RAW_U8 onoff);
static void LcdEnvidOnOff(RAW_U8 onoff);
static void LCD_display_setting(void);
/*************************本地函数申明结束*******************************************/


/****************************全局变量申明开始********************************************/
//win0/1 寄存器
volatile RAW_U32 *WINCONx_Reg_Addr[]=
{
	&WINCON0_REG,	&WINCON1_REG 
};
volatile RAW_U32 *VIDOSDxA_Reg_Addr[]=
{
	&VIDOSD0A_REG, &VIDOSD1A_REG
};
volatile RAW_U32 *VIDOSDxB_Reg_Addr[]=
{
	&VIDOSD0B_REG, &VIDOSD1B_REG
};
volatile RAW_U32 *VIDOSDxC_Reg_Addr[]=
{
	NULL, &VIDOSD1C_REG
};

volatile RAW_U32 *VIDWxADD0_Reg_Addr[][2]=
{
	{&VIDW00ADD0B0_REG, &VIDW00ADD0B1_REG},
	{&VIDW01ADD0_REG, NULL}
};

volatile RAW_U32 *VIDWxADD1_Reg_Addr[][2]=
{
	{&VIDW00ADD1B0_REG, &VIDW00ADD1B1_REG},
	{&VIDW01ADD1_REG, NULL}
};

volatile RAW_U32 *VIDWxADD2_Reg_Addr[][2]=
{
	{&VIDW00ADD2B0_REG, &VIDW00ADD2B1_REG},
	{&VIDW01ADD2_REG, NULL}
};

volatile RAW_U32 *WINxMAP_Reg_Addr[]=
{
	&WIN0MAP_REG, &WIN1MAP_REG
};

volatile RAW_U32 *WxKEYCON0_Reg_Addr[]=
{
	0, &W1KEYCON0_REG
};

volatile RAW_U32 *WxKEYCON1_Reg_Addr[]=
{
	0, &W1KEYCON1_REG
};

RAW_U16 *lcd_framebuffer_vir = 0;
RAW_U32 lcd_framebuffer_phy = 0;
RAW_U16 tmp_framebuf[LCD_LINEVAL][LCD_HOZVAL] = {0};
//lcd_frame_buffer[0][0][0] win0 buf0 ; lcd_frame_buffer[1][0][0] win0 buf1 ; lcd_frame_buffer[2][0][0] win1 buf0 ;
RAW_U16 lcd_frame_buffer[3][LCD_LINEVAL][LCD_HOZVAL] __attribute__((at(0x33800000))) = {0}; 

RAW_U16 *lcd_frame_buffer_offset[][2]=
{
	{&lcd_frame_buffer[0][0][0], &lcd_frame_buffer[1][0][0]},
	{&lcd_frame_buffer[2][0][0], NULL}
};

RAW_U16 lcd_horizon_value=0,lcd_line_value=0;
RAW_U8 lcd_vbpd=0,lcd_vfpd=0,lcd_vspw=0,lcd_hbpd=0,lcd_hfpd=0,lcd_hspw=0;
RAW_U8 lcd_frame_rate=0;

RAW_U8 lcd_bit_order[]=
{
	1,2,4,8,8,16,16,16,32,32,32,32,32,32
};
RAW_U8 lcd_burst_in_byte[]=
{
	64,32,16
};

RAW_U8 lcd_burst_mode=WINCONx_16WORD_BURST;		

/****************************全局变量申明结束********************************************/

/*************************************************
    函数名称:       lcd_control_reg_init
    功能描述:       lcd端口初始化
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
static void lcd_control_reg_init(void)
{
	RAW_U8 clkval=0;
	RAW_U16 h_cnt,v_cnt;
	RAW_U32 vclk;
	
	//显示窗口设置
	lcd_horizon_value = LCD_HOZVAL;
	lcd_line_value = LCD_LINEVAL;
	lcd_vbpd = LCD_VBPD;
	lcd_vfpd = LCD_VFPD;
	lcd_vspw = LCD_VSPW;
	lcd_hbpd = LCD_HBPD;
	lcd_hfpd = LCD_HFPD;
	lcd_hspw = LCD_HSPW;
	lcd_frame_rate = LCD_FRAME_RATE;
	
	
	//VIDCON0 寄存器设置
	VIDCON0_REG = VIDCON0_S_RGB_IF|VIDCON0_S_RGB_PAR/*VIDCON0_S_RGB_SER*/|VIDCON0_S_VCLK_GATING_OFF/*VIDCON0_S_VCLK_GATING_ON|*/\
		/*VIDCON0_S_CLKVAL_F_AlWAYS_UPDATE*/|VIDCON0_S_CLKDIR_DIVIDED|VIDCON0_S_CLKSEL_HCLK;
	//计算VCLK	
	v_cnt = (lcd_vbpd+lcd_vfpd+lcd_vspw+lcd_line_value);
	h_cnt = (lcd_hbpd+lcd_hfpd+lcd_hspw+lcd_horizon_value);
	
	clkval = (RAW_U8)( ((float)get_HCLK()/(float)(v_cnt*h_cnt*lcd_frame_rate) )+0.5)-1;
	
	vclk = (get_HCLK()/(clkval+1))/1000;
	Uart_Printf("VCLK: %dKHz(%dMHz)\n", vclk, vclk / 1000);
	
	if(clkval > 0x3f)
		Uart_Printf("!!! Check frame rate: over flow !!!\n");
	

	VIDCON0_REG |= (clkval <<VIDCON0_CLKVAL_F_SHIFT);	

	
	//VIDCON1 寄存器设置
	#if 1		//ZhangGQ changed 20130408
	VIDCON1_REG = VIDCON1_S_HSYNC_INVERTED|VIDCON1_S_VSYNC_INVERTED;
	#else 
	VIDCON1_REG = VIDCON1_S_HSYNC_INVERTED;	//测试OK！！！ ZhangGQ changed 20130408
	#endif


	//VIDTCON0/1/2 寄存器设置
	VIDTCON0_REG=((lcd_vbpd-1)<<VIDTCON0_BPD_S)|((lcd_vfpd-1)<<VIDTCON0_FPD_S)|(lcd_vspw-1);
	VIDTCON1_REG=((lcd_hbpd-1)<<VIDTCON0_BPD_S)|((lcd_hfpd-1)<<VIDTCON0_FPD_S)|(lcd_hspw-1);
	VIDTCON2_REG = ((lcd_line_value-1)<<VIDTCON2_LINEVAL_S)|(lcd_horizon_value-1);

}

/*************************************************
    函数名称:       lcd_port_init
    功能描述:       lcd端口初始化
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
static void lcd_port_init(void)
{
	//lcd 端口设置
	GPCPU_REG   = /*0xffffffff*/0;  	// 禁止内部上拉  ZhangGQ changed 20120408, it's different with S3C2440!
    GPCCON_REG  = 0xaaaa02aa;   		// GPIO管脚用于VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND 
	
    GPDPU_REG   = /*0xffffffff*/0;   	// 禁止内部上拉
    GPDCON_REG  = 0xaaaaaaaa;   		// GPIO管脚用于VD[23:8]		
	
	//GPB3 HIGH LCD_BACK_LGIHT
	GPBCON_REG &= ~(0x3<<6);
	GPBCON_REG |= (0x1<<6);
	GPBDAT_REG |= (0x1<<3);
}

/*************************************************
    函数名称:       lcd_init
    功能描述:       lcd初始化函数
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void lcd_init(void)
{
	
	
	//lcd端口初始化
	lcd_port_init();
	//LCD控制器寄存器设置
	lcd_control_reg_init();

	
	//初始化关闭窗口
	Display_End(LCD_WIN_0);
	Display_End(LCD_WIN_1);	

	
	
	//显示窗口，buf设置
	LCD_display_setting();
}


/*************************************************
    函数名称:       lcd_test
    功能描述:       lcd测试函数
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void lcd_test(void)
{
	Uart_Printf("\r\n##### Test TFT LCD 480*272 #####\r\n");
		
	while(1)
	{
		#if 0
		//送显示
		LCD_fill_rectangle( 0, 0, LCD_HOZVAL, LCD_LINEVAL, 0);	
		//清显示
		LCD_clr();
		//点显示
		LCD_one_pixel_write(10,10,RED_16BPP);
		LCD_one_pixel_write(20,20,RED_16BPP);
		LCD_one_pixel_write(30,30,RED_16BPP);
		LCD_one_pixel_write(40,40,RED_16BPP);
		LCD_one_pixel_write(50,50,RED_16BPP);
		LCD_one_pixel_write(60,60,RED_16BPP);
		LCD_drawing_Vertical_line( 0, 0, LCD_LINEVAL,RED_16BPP);
		LCD_drawing_Vertical_line( 1, 0, LCD_LINEVAL,BLUE_16BPP);
		#endif
		LCD_drawing_Horizon_line( 10, 0, LCD_HOZVAL,RED_16BPP);
		LCD_drawing_Horizon_line( 20, 0, LCD_HOZVAL,BLUE_16BPP);
	}
}

/*************************************************
    函数名称:       Display_Start
    功能描述:       
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void Display_Start(RAW_U8 win_num)	// 0: LCD_WIN_0, 1: LCD_WIN_1, 2: LCD_WIN_ALL
{	
	LcdWindowOnOff(win_num,LCD_ON);
	LcdEnvidOnOff(LCD_ON);
	Delay(5);	//此延时可调整
//	Delay(50000);
//	Delay(50000);
		
	return;
}

static void Delay(int time)
{
	static int delayLoopCount = 4;//000;
	int i;       
    
    for(;time>0;time--)
        for(i=0;i<delayLoopCount;i++);    
}

/*************************************************
    函数名称:       Display_End
    功能描述:       
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void Display_End(RAW_U8 win_num)
{	
	LcdWindowOnOff(win_num,LCD_OFF);
	LcdEnvidOnOff(LCD_OFF);	

	return;
}

/*************************************************
    函数名称:       LcdWindowOnOff
    功能描述:       
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
static void LcdWindowOnOff(RAW_U8 num, RAW_U8 onoff)
{
	switch(num)
	{
		case LCD_WIN_0:
			if(onoff==LCD_ON)
				WINCON0_REG |= 0x01;
			else
				WINCON0_REG &= ~0x01;
			break;
					
		case LCD_WIN_1:
			if(onoff==LCD_ON)
				WINCON1_REG |= 0x01;
			else
				WINCON1_REG &= ~0x01;
			break;
			
		case LCD_WIN_ALL:
			if(onoff==LCD_ON)
			{
				WINCON0_REG |= 0x01;
				WINCON1_REG |= 0x01;
			}
			else
			{
				WINCON0_REG &= ~0x01;
				WINCON1_REG &= ~0x01;
			}
		
		default:
			break;
	}
	return;
}

/*************************************************
    函数名称:       LcdEnvidOnOff
    功能描述:       
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
static void LcdEnvidOnOff(RAW_U8 onoff)
{
	if(onoff==LCD_ON)
		VIDCON0_REG |= (3); 		// ENVID On using Per Frame method
	else 
	{
		VIDCON0_REG &= (~1); 		// ENVID Off using Per Frame method
		
		while( VIDCON0_REG & 1);	// Check end of frame
	}
} 

/*************************************************
    函数名称:       Display_Setting
    功能描述:       
	// 2006.06.02
	// Frame buffer address setting : Windows0 have the two buffer like as buffer0, buffer1. 
	// Each buffer have the register for setting the start and end address. But Window1 have one buffer
	子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
static void Display_Setting( RAW_U32 win_num,RAW_U32 buf_num)
{
	RAW_U32 screenwidth_in_byte=0;

	screenwidth_in_byte = LCD_HOZVAL*lcd_bit_order[WINCONx_16BPP_565]/8;

	// WINCON0/1
	#if 0
	*WINCONx_Reg_Addr[win_num] = \
		(buf_num<<WINCON_BUFSEL)|(0<<WINCON_BUFAUTOEN)|(0<<WINCON_SWAP_S)|(lcd_burst_mode<<WINCON_BURSTLEN_S)|(WINCONx_16BPP_565<<WINCON_BPP_S); 
	#else  //考虑我们是按半字存储DMA数据，此处应该 WINCON_SWAP_S 设置为1.高低半字倒转。ZhangGQ add 20130408 待测试！！
	*WINCONx_Reg_Addr[win_num] = \
		(buf_num<<WINCON_BUFSEL)|(0<<WINCON_BUFAUTOEN)|(1<<WINCON_SWAP_S)|(lcd_burst_mode<<WINCON_BURSTLEN_S)|(WINCONx_16BPP_565<<WINCON_BPP_S); 
	#endif
	
	// WIN0/1 OSD(top)
	*VIDOSDxA_Reg_Addr[win_num] = (0<<VIDOSDxAB_HORIZON_X_S)|(0);

	// WIN0/1 OSD(bottom)
	*VIDOSDxB_Reg_Addr[win_num] = ((LCD_HOZVAL-1)<<VIDOSDxAB_HORIZON_X_S)|(LCD_LINEVAL-1);	

	lcd_framebuffer_vir = lcd_frame_buffer_offset[win_num][buf_num];

	lcd_framebuffer_phy = (RAW_U32)lcd_framebuffer_vir; //- (0xc0000000 - 0x30000000);

	//lcd_framebuffer_phy = 0x33800000; 


	// WIN0/1 buffer start address
	*VIDWxADD0_Reg_Addr[win_num][buf_num] = lcd_framebuffer_phy;  //转换为物理地址 		
	// WIN0/1 buffer end address
	*VIDWxADD1_Reg_Addr[win_num][buf_num] = lcd_framebuffer_phy + (screenwidth_in_byte)*(LCD_LINEVAL);
	*VIDWxADD2_Reg_Addr[win_num][buf_num] = (0<<VIDWxADD2_OFFSET_SIZE_S)|screenwidth_in_byte;
	*WINxMAP_Reg_Addr[win_num] = 0;
	
	if (win_num>0)
	{
		*VIDOSDxC_Reg_Addr[win_num] = 0;
		*WxKEYCON0_Reg_Addr[win_num] = 0;
		*WxKEYCON1_Reg_Addr[win_num] = 0;	
	}
}

/*************************************************
    函数名称:       LCD_fill_rectangle
    功能描述:  		填充一个矩形区域   
	子 函 数:       无
    调用限制:       LCD初始化之后
    输入参数:       pos_x1: 左上角x坐标
										pos_y1：左上角y坐标
										pos_x2:	右下角x坐标	
										pos_y2:	右下角y坐标
										image_flg:	0: 非图片，1：图片，读取图片bmp数据
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void LCD_fill_rectangle( RAW_U32 pos_x1, RAW_U32 pos_y1, RAW_U32 pos_x2, RAW_U32 pos_y2, RAW_U8 image_flg)
{
	RAW_U32 len_x = 0;
	RAW_U32	len_y = 0;
	RAW_U32 i = 0;
	RAW_U32 j = 0;
	
	if( (pos_x2 <= pos_x1) || (pos_y2 <= pos_y1))
	{
		return;	//不合法
	}
	
	len_x = pos_x2 - pos_x1;
	len_y = pos_y2 - pos_y1;
	
	if( (len_x > LCD_HOZVAL) || (len_y > LCD_LINEVAL))
	{
		return;	//越界
	}
	
	if(image_flg == 0)
	{
		//备份原数据
		if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
		{
			memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[0][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
		}
		else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
		{
			memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[1][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
		}
		else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
		{
			memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[2][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
		}
		else
		{
				
		}
				
		//修改显示区数据，显示red\blue\green 测试用		
		for(j = 0 ; j < (len_y/3) ; j++)
		{
			for(i = 0 ; i < len_x ; i++) 
			{			
				tmp_framebuf[j][i] = GREEN_16BPP;
			}				
		}
		for(j = (len_y/3) ; j < (len_y*2/3) ; j++)
		{
			for(i = 0 ; i < len_x ; i++) 
			{			
				tmp_framebuf[j][i] = RED_16BPP;
			}				
		}
		for(j = (len_y*2/3) ; j < (len_y) ; j++)
		{
			for(i = 0 ; i < len_x ; i++) 
			{			
				tmp_framebuf[j][i] = BLUE_16BPP;
			}				
		}
		//刷新矩形区的像素	
		if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
		{
			memcpy(&lcd_frame_buffer[0][pos_y1][pos_x1],&tmp_framebuf[0][0],len_x*len_y*2);
//			memcpy(&lcd_frame_buffer[0][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//			Display_Start(LCD_WIN_0);
//			Display_End(LCD_WIN_0);
		}
		else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
		{
			memcpy(&lcd_frame_buffer[1][pos_y1][pos_x1],&tmp_framebuf[0][0],len_x*len_y*2);
//			memcpy(&lcd_frame_buffer[1][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//			Display_Start(LCD_WIN_0);
//			Display_End(LCD_WIN_0);
		}
		else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
		{
			memcpy(&lcd_frame_buffer[2][pos_y1][pos_x1],&tmp_framebuf[0][0],len_x*len_y*2);
//			memcpy(&lcd_frame_buffer[2][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//			Display_Start(LCD_WIN_1);
//			Display_End(LCD_WIN_1);
		}
		else
		{
				
		}
	}
	else
	{
		//暂时无图片数据，预留接口
	}
}

/*************************************************
    函数名称:       LCD_one_pixel_write()
    功能描述:  		点亮一个点的像素  
	子 函 数:       无
    调用限制:       LCD初始化之后
    输入参数:       pos_x: x坐标
					pos_y：y坐标					
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void LCD_one_pixel_write( RAW_U32 pos_x, RAW_U32 pos_y, RAW_U32 color)
{

	if( (pos_x > LCD_HOZVAL) || (pos_y > LCD_LINEVAL))
	{
		RAW_ASSERT(0);	//越界
	}
#if USE_FRAMEBUFF
	//备份原数据
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[0][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[1][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[2][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else
	{
			
	}
#endif	

	//修改显示区数据
#if USE_FRAMEBUFF
	tmp_framebuf[pos_y][pos_x] = (color&0xffff);
//	Uart_Printf("use framebuff\n");
#endif	
//	Uart_Printf("Never use framebuff\n");
	//刷新矩形区的像素	
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
#if USE_FRAMEBUFF
		lcd_frame_buffer[0][pos_y][pos_x] = tmp_framebuf[pos_y][pos_x];
#else
		lcd_frame_buffer[0][pos_y][pos_x] = (color&0xffff);
#endif
//		memcpy(&lcd_frame_buffer[0][pos_y][pos_x],&tmp_framebuf[pos_y][pos_x],2);
//		memcpy(&lcd_frame_buffer[0][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		memcpy(&lcd_frame_buffer[0][0][0],fd,LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
#if USE_FRAMEBUFF
		lcd_frame_buffer[1][pos_y][pos_x] = tmp_framebuf[pos_y][pos_x];
#else
		lcd_frame_buffer[1][pos_y][pos_x] = (color&0xffff);
#endif
//		memcpy(&lcd_frame_buffer[1][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		memcpy(&lcd_frame_buffer[0][0][0],fd,LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
#if USE_FRAMEBUFF
		lcd_frame_buffer[2][pos_y][pos_x] = tmp_framebuf[pos_y][pos_x];
#else
		lcd_frame_buffer[2][pos_y][pos_x] = (color&0xffff);
#endif
//		memcpy(&lcd_frame_buffer[2][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		memcpy(&lcd_frame_buffer[0][0][0],fd,LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_1);
//		Display_End(LCD_WIN_1);
	}
	else
	{
			
	}
	
}

/*************************************************
    函数名称:       LCD_one_pixel_read()
    功能描述:  		读取一个点的像素   
	子 函 数:       无
    调用限制:       LCD初始化之后
    输入参数:       pos_x: x坐标
					pos_y：y坐标					
    输出参数:       无
    返 回 值:       点像素
    说    明:       
*************************************************/
RAW_U32 LCD_one_pixel_read( RAW_U32 pos_x, RAW_U32 pos_y)
{
	volatile RAW_U32 test;
	
	if( (pos_x > LCD_HOZVAL) || (pos_y > LCD_LINEVAL))
	{
		RAW_ASSERT(0);	//越界
	}

	test = lcd_frame_buffer[_TEST_WHICH_WIN_AND_BUF][pos_y][pos_x];
		
	return test;

	#if 0
	//备份原数据
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		return (lcd_frame_buffer[0][pos_y][pos_x]);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		return (lcd_frame_buffer[1][pos_y][pos_x]);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		return (lcd_frame_buffer[2][pos_y][pos_x]);
	}
	else
	{
			
	}
	
	return 1;	
	#endif
}

/*************************************************
    函数名称:       LCD_clr
    功能描述:  		清屏 
	子 函 数:       无
    调用限制:       
    输入参数:       
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void LCD_clr(void)
{
	RAW_U32 i = 0;
	RAW_U32 j = 0;
#if USE_FRAMEBUFF
	//备份原数据
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[0][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[1][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[2][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else
	{
			
	}
#endif			
	//修改显示区数据，显示red\blue\green 测试用		
	for(j = 0 ; j < LCD_LINEVAL ; j++)
	{
		for(i = 0 ; i < LCD_HOZVAL ; i++) 
		{	
#if USE_FRAMEBUFF
			tmp_framebuf[j][i] = 0xffff;
#else
			lcd_frame_buffer[_TEST_WHICH_WIN_AND_BUF][j][i] = 0xffff;
#endif
		}				
	}
	
#if USE_FRAMEBUFF
	
	//刷新矩形区的像素	
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		memcpy(&lcd_frame_buffer[0][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		memcpy(&lcd_frame_buffer[1][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		memcpy(&lcd_frame_buffer[2][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_1);
//		Display_End(LCD_WIN_1);
	}
	else
	{
			
	}
#endif	
}

/*************************************************
    函数名称:       LCD_display_setting
    功能描述:  		清屏 
	子 函 数:       无
    调用限制:       
    输入参数:       
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
static void LCD_display_setting(void)
{
	//测试选择
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		Uart_Printf("\r\n LCD:win0,buf0 test!\r\n");
		Display_Setting(LCD_WIN_0,LCD_BUF_0); //窗口0，buf0	
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		Uart_Printf("\r\n LCD:win0,buf1 test!\r\n");
		Display_Setting(LCD_WIN_0,LCD_BUF_1); //窗口0，buf1	
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		Uart_Printf("\r\n LCD:win1,buf0 test!\r\n");
		Display_Setting(LCD_WIN_1,LCD_BUF_0); //窗口1，buf0	
	}
	else
	{
			
	}
}


/*************************************************
    函数名称:       LCD_draw_Horizon_line()
    功能描述:  		画横线  
	子 函 数:       无
    调用限制:       LCD初始化之后
    输入参数:       pos_x1: x左坐标
					pos_x2: x右坐标
					pos_y：y坐标					
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void LCD_drawing_Horizon_line( RAW_U32 pos_y, RAW_U32 pos_x1, RAW_U32 pos_x2,RAW_U32 color)
{
	RAW_U32 i = 0;	
	
	if( (pos_x1 > LCD_HOZVAL) || (pos_x2 > LCD_HOZVAL) || (pos_y > LCD_LINEVAL))
	{
		return;	//越界
	}

	if ( pos_x1 >= pos_x2 ) {
		pos_x1 ^= pos_x2;
		pos_x2 ^= pos_x1;
		pos_x1 ^= pos_x2;
		}
	
#if USE_FRAMEBUFF
	//备份原数据
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[0][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[1][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[2][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else
	{
			
	}
#endif			
	//修改显示区数据
	for( i = pos_x1; i <= pos_x2 ; i++)
	{
#if USE_FRAMEBUFF
		tmp_framebuf[pos_y][i] = (color&0xffff);
#else
		lcd_frame_buffer[_TEST_WHICH_WIN_AND_BUF][pos_y][i] = (color&0xffff);
#endif
	}

#if USE_FRAMEBUFF
	//刷新矩形区的像素	
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		memcpy(&lcd_frame_buffer[0][pos_y][pos_x1],&tmp_framebuf[pos_y][pos_x1],(pos_x2 - pos_x1 + 1)*2);
//		memcpy(&lcd_frame_buffer[0][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		memcpy(&lcd_frame_buffer[1][pos_y][pos_x1],&tmp_framebuf[pos_y][pos_x1],(pos_x2 - pos_x1 + 1)*2);
//		memcpy(&lcd_frame_buffer[1][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		memcpy(&lcd_frame_buffer[0][pos_y][pos_x1],&tmp_framebuf[pos_y][pos_x1],(pos_x2 - pos_x1 + 1)*2);
//		memcpy(&lcd_frame_buffer[2][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_1);
//		Display_End(LCD_WIN_1);
	}
	else
	{
			
	}
#endif
}

/*************************************************
    函数名称:       LCD_draw_Vertical_line()
    功能描述:  		画纵线  
	子 函 数:       无
    调用限制:       LCD初始化之后
    输入参数:       pos_y1: y上坐标
					pos_y2: y下坐标
					pos_x：x坐标					
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void LCD_drawing_Vertical_line( RAW_U32 pos_x, RAW_U32 pos_y1, RAW_U32 pos_y2,RAW_U32 color)
{
	RAW_U32 j = 0;	
	
	if( (pos_x > LCD_HOZVAL) || (pos_y1 > LCD_LINEVAL) || (pos_y2 > LCD_LINEVAL))
	{
		return;	//越界
	}

	if ( pos_y1 >= pos_y2 ) {
		pos_y1 ^= pos_y2;
		pos_y2 ^= pos_y1;
		pos_y1 ^= pos_y2;
		}
	
#if USE_FRAMEBUFF	
	//备份原数据
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[0][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[1][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[2][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else
	{
			
	}
#endif			
	//修改显示区数据
	for( j = pos_y1; j <= pos_y2 ; j++)
	{
#if USE_FRAMEBUFF
		tmp_framebuf[j][pos_x] = (color&0xffff);
#else
		lcd_frame_buffer[_TEST_WHICH_WIN_AND_BUF][j][pos_x] = (color&0xffff);
#endif
	}
#if USE_FRAMEBUFF		
	//刷新矩形区的像素	
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
//		memcpy(&lcd_frame_buffer[0][pos_y1][pos_x],&tmp_framebuf[pos_y1][pos_x],(pos_y2 - pos_y1 + 1) * 2);
		memcpy(&lcd_frame_buffer[0][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
//		memcpy(&lcd_frame_buffer[1][pos_y1][pos_x],&tmp_framebuf[pos_y1][pos_x],(pos_y2 - pos_y1 + 1) * 2);
		memcpy(&lcd_frame_buffer[1][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
//		memcpy(&lcd_frame_buffer[2][pos_y1][pos_x],&tmp_framebuf[pos_y1][pos_x],(pos_y2 - pos_y1 + 1) * 2);
		memcpy(&lcd_frame_buffer[2][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_1);
//		Display_End(LCD_WIN_1);
	}
	else
	{
			
	}
#endif
}


/*************************************************
    函数名称:       LCD_fill_rectangle_color
    功能描述:  		填充一个矩形区域   
	子 函 数:       无
    调用限制:       LCD初始化之后
    输入参数:       pos_x1: 左上角x坐标
					pos_y1：左上角y坐标
					pos_x2:	右下角x坐标	
					pos_y2:	右下角y坐标
					color:	填充颜色 
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void LCD_fill_rectangle_color( RAW_U32 pos_x1, RAW_U32 pos_y1, RAW_U32 pos_x2, RAW_U32 pos_y2, RAW_U32 color)
{
	RAW_U32 len_x = 0;
	RAW_U32	len_y = 0;
	RAW_U32 i = 0;
	RAW_U32 j = 0;
	
	if( (pos_x2 <= pos_x1) || (pos_y2 <= pos_y1))
	{
		RAW_ASSERT(0);	//不合法
	}
	
	len_x = pos_x2 - pos_x1;
	len_y = pos_y2 - pos_y1;
	
	if( (pos_x2 > LCD_HOZVAL) || (pos_y2 > LCD_LINEVAL))
	{
		RAW_ASSERT(0);	//越界
	}
	
	
#if USE_FRAMEBUFF	
	//备份原数据
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[0][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[1][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[2][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else
	{
			
	}
#endif		
	//修改显示区数据，显示red\blue\green 测试用		
	for(j = 0 ; j < len_y ; j++)
	{
		for(i = 0 ; i < len_x ; i++) 
		{	
#if USE_FRAMEBUFF	
			tmp_framebuf[pos_y1 + j][pos_x1 + i] = (color&0xffff);
#else
			lcd_frame_buffer[_TEST_WHICH_WIN_AND_BUF][pos_y1 + j][pos_x1 + i] = (color&0xffff);
#endif
	
		}				
	}
#if USE_FRAMEBUFF	
	//刷新矩形区的像素	
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		memcpy(&lcd_frame_buffer[0][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		memcpy(&lcd_frame_buffer[1][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		memcpy(&lcd_frame_buffer[2][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_1);
//		Display_End(LCD_WIN_1);
	}
	else
	{
			
	}
#endif	
	
}

/*************************************************
    函数名称:       LCD_ShowBitmap
    功能描述:  		 将一个位图显示到LCD屏上
	  子 函 数:       无
    调用限制:       LCD初始化之后
    输入参数:       bitmap: 要显示的的位图的地址
    输出参数:       无
    返 回 值:       无
    说    明:        
*************************************************/
void LCD_ShowBitmap(const void * bitmap)
{

	memcpy(&lcd_frame_buffer[_TEST_WHICH_WIN_AND_BUF][0][0],(RAW_U16 *)bitmap,LCD_LINEVAL*LCD_HOZVAL*2);
#if 0
	//刷新整屏像素	
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		memcpy(&lcd_frame_buffer[0][0][0],(RAW_U16 *)bitmap,LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		memcpy(&lcd_frame_buffer[1][0][0],(RAW_U16 *)bitmap,LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		memcpy(&lcd_frame_buffer[2][0][0],(RAW_U16 *)bitmap,LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_1);
//		Display_End(LCD_WIN_1);
	}
	else
	{
			
	}
#endif		
}


/*************************************************
    函数名称:       LCD_SetBitmap
    功能描述:  		将一个位图显示到LCD屏上的(x,y)坐标位置
	子 函 数:       无
    调用限制:       LCD初始化之后
    输入参数:       bitmap: 要显示的的位图的地址
    输出参数:       无
    返 回 值:       无
    说    明:       整屏显示像素图 
*************************************************/

#if 1
extern  GUI_CONTEXT GUI_Context;
void LCD_SetBitmap(int x, int y, RAW_U8 const GUI_UNI_PTR *p, int Diff, int xsize, const RAW_U16 *pTrans)
{
	RAW_U16 Index0 = *(pTrans+0);
	RAW_U16 Index1 = *(pTrans+1);
	int Pixel;
	x += Diff;
	
	//判断数据合法性
	if( (x + xsize > LCD_HOZVAL) || (y > LCD_LINEVAL))
	{
		return;	//越界
	}

#if USE_FRAMEBUFF
	//备份原数据
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[0][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[1][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		memcpy(&tmp_framebuf[0][0],&lcd_frame_buffer[2][0][0],LCD_LINEVAL*LCD_HOZVAL*2);
	}
	else
	{
			
	}

#endif	

	//选择显示的模式
	switch (GUI_Context.DrawMode & (LCD_DRAWMODE_TRANS | LCD_DRAWMODE_XOR)) {
		case 0:
	
		  do {
			tmp_framebuf[y][x++] = (*p & (0x80 >> Diff)) ? Index1 : Index0;
				  if (++Diff == 8) {
					  Diff = 0;
					  p++;
				  }
			  } while (--xsize);
	
		  break;
		case LCD_DRAWMODE_TRANS:
	
		  do {
			  if (*p & (0x80 >> Diff))
			  	tmp_framebuf[y][x] = Index1;
				x++;
				  if (++Diff == 8) {
					  Diff = 0;
					  p++;
				  }
			  } while (--xsize);
	
		  break;
		case LCD_DRAWMODE_XOR:;
		  do {
			  if (*p & (0x80 >> Diff)) {
			  	Pixel = LCD_L0_GetPixelIndex(x, y);
			  	tmp_framebuf[y][x] = LCD_NUM_COLORS - 1 - Pixel;
				}
				x++;
				  if (++Diff == 8) {
					  Diff = 0;
					  p++;
				  }
			  } while (--xsize);
		  break;
		}

	memcpy(&lcd_frame_buffer[_TEST_WHICH_WIN_AND_BUF][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);

#if 0		
	//刷屏	
	if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF0)
	{
		memcpy(&lcd_frame_buffer[0][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		memcpy(&lcd_frame_buffer[0][0][0],fd,LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN0_BUF1)
	{
		memcpy(&lcd_frame_buffer[1][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		memcpy(&lcd_frame_buffer[0][0][0],fd,LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_0);
//		Display_End(LCD_WIN_0);
	}
	else if(_TEST_WHICH_WIN_AND_BUF == WIN1_BUF0)
	{
		memcpy(&lcd_frame_buffer[2][0][0],&tmp_framebuf[0][0],LCD_LINEVAL*LCD_HOZVAL*2);
//		memcpy(&lcd_frame_buffer[0][0][0],fd,LCD_LINEVAL*LCD_HOZVAL*2);
//		Display_Start(LCD_WIN_1);
//		Display_End(LCD_WIN_1);
	}
	else
	{
			
	}
#endif
}

#endif

