/*
* Copyright (c) 2013
* All rights reserved.
* 
* 文件名称：lcd_driver.C
* 文件标识：
* 开发环境：C编译器
* 描    述：touch panel driver
* 当前版本：V1.0
* 作    者：
* 更改说明：
* 完成日期：2013年04月01日
*
* 修改记录 
*       作者        时间        版本            修改描述
*     章光泉      20130402      V1.0        
*/
#include <stdio.h>
#include <lib_string.h>
#include <raw_api.h>
#include <irq.h>
#include "2416_reg.h"
#include "ADCTs_driver.h"
#include "GUI.h"

// chapter24 ADC - gom
#define rADCCON		(*(volatile unsigned *)0x58000000)	//ADC control
#define rADCTSC		(*(volatile unsigned *)0x58000004)	//ADC touch screen control
#define rADCDLY		(*(volatile unsigned *)0x58000008)	//ADC start or Interval Delay
#define rADCDAT0	(*(volatile unsigned *)0x5800000c)	//ADC conversion data 0
#define rADCDAT1	(*(volatile unsigned *)0x58000010)	//ADC conversion data 1
#define rADCUPDN	(*(volatile unsigned *)0x58000014)	//Stylus Up/Down interrupt status
#define rADCMUX		(*(volatile unsigned *)0x58000018)	//Stylus Up/Down interrupt status



// chapter10 INTERRUPT - gom
#define rSRCPND		(*(volatile unsigned *)0x4a000000)	//Interrupt request status
#define rINTMOD		(*(volatile unsigned *)0x4a000004)	//Interrupt mode control
#define rINTMSK		(*(volatile unsigned *)0x4a000008)	//Interrupt mask control
#define rINTPND		(*(volatile unsigned *)0x4a000010)	//Interrupt request status
#define rINTOFFSET	(*(volatile unsigned *)0x4a000014)	//Interruot request source offset
#define rSUBSRCPND	(*(volatile unsigned *)0x4a000018)	//Sub source pending
#define rINTSUBMSK	(*(volatile unsigned *)0x4a00001c)	//Interrupt sub mask
#define rPRIORITY_MODE1			(*(volatile unsigned *)0x4a000030)	//Interrupt request status
#define rPRIORITY_UPDATE1		(*(volatile unsigned *)0x4a000034)	//Interrupt mode control
#define rSRCPND2	(*(volatile unsigned *)0x4a000040)	//Interrupt request status
#define rINTMOD2	(*(volatile unsigned *)0x4a000044)	//Interrupt mode control
#define rINTMSK2		(*(volatile unsigned *)0x4a000048)	//Interrupt mask control
#define rINTPND2		(*(volatile unsigned *)0x4a000050)	//Interrupt request status
#define rINTOFFSET2	(*(volatile unsigned *)0x4a000054)	//Interruot request source offset
#define rPRIORITY_MODE2			(*(volatile unsigned *)0x4a000070)	//Interrupt request status
#define rPRIORITY_UPDATE2		(*(volatile unsigned *)0x4a000074)	//Interrupt mode control


#define ClearPending(bit) {\
                rSRCPND = bit;\
                rINTPND = bit;\
                rINTPND;\
                }  

extern RAW_U32 get_PCLK(void);

#define BIT_ADC         0x80000000
#define BIT_SUB_TC      (0x1<<9)
#define BIT_SUB_ADC     (0x1<<10)


/****************************全局变量定义开始************************************/

RAW_U32 adcpsr=0;           // ADCPSR   
RAW_U32 adcclock=500000;   // ADC clock
RAW_U32 xtal=12000000;      // X-tal clock
  
static volatile RAW_S32 xdata, ydata;
RAW_U32 count=0;            // check interrupt
int method=0;

static int touch_down;
static int ad_count;
static int adc_finish;

static int x_data[16];
static int y_data[16];

#define	TS_SAMPLE_COUNT	10

static	void InsertSort(int *A, int p, int r)
{
    int i,j;
    int key;
    for(i=p+1; i<=r; i++)
    {
        key = A[i];
        j = i-1;
        while (j >= 0 && A[j] > key)
        {
            A[j+1] = A[j];
            j--;
        }
        A[j+1] = key;
    }
}


void AdcTsSep(void)
{
	rINTMSK |= (U32)BIT_ADC;	// ADC Touch Screen Mask bit set
	rINTSUBMSK |= (BIT_SUB_ADC | BIT_SUB_TC);

	if( (rSUBSRCPND & (0x1<<10)) & ((count&1)<<10) ) // ADC interrupt(end of Y-posion conversion)
	{
		rSUBSRCPND |= BIT_SUB_ADC;	// Clear ADC pending bit
		
		if((method==1)|(method==3))	// read start
			rADCCON &= ~(1<<1);		// Disable read_start

		ydata=(rADCDAT1&0xfff);		// Read Y-position Conversion data
		y_data[ad_count] = ydata;
		count++;

		ad_count++;
		//Uart_Printf("ad_count=%d\n", ad_count, ydata);
		//Uart_Printf("YP=%04d\n", ydata);

		if (ad_count < TS_SAMPLE_COUNT) {

			//rADCUPDN &= ~(0x1);		// Clear down state
			//rADCDLY=(get_PCLK()/1000000)*500;// 500us delay(input stable)
 			rADCTSC = 0x1;			// X-position measurement state

			rADCCON |= 0x1;			// Start ADC
		}


		else {
			
			ad_count = 0;
			adc_finish = 1;
			
			InsertSort(x_data,0,TS_SAMPLE_COUNT-1);
			InsertSort(y_data,0,TS_SAMPLE_COUNT-1);
			
			rADCCON &= ~(1<<1);		// Disable read_start
			
			rADCDLY=(int)(xtal/1000)*2;	// 2ms delay(filter effect)
			rADCTSC = 0x1d3;			// Up detect, Waiting for interrupt
		}
		
	}
	else if( (rSUBSRCPND & (0x1<<10)) & (((count+1)&1)<<10) )// ADC interrupt(end of X-posion conversion)
	{
		rSUBSRCPND |= BIT_SUB_ADC;	// Clear ADC pending bit

		rADCTSC = 0x2;				// Y-position measurement state
		xdata=(rADCDAT0&0xfff);		// Read X-position Conversion data
		x_data[ad_count] = xdata;
		
		//Uart_Printf("XP=%04d\n", xdata);
		count++;
		//rADCDLY=(get_PCLK()/1000000)*500;// 500us delay(input stable)
		rADCCON |= 0x1;			// Start ADC
		
		if((method==0)|(method==2))	// manual start
			rADCCON |= 0x1;
	}
	else if(rSUBSRCPND & (0x1<<9))	// TC interrupt
	{
		rSUBSRCPND |= BIT_SUB_TC;	// Clear TC pending bit
		
		if(rADCUPDN == 0x1)			// Stylus Down
		{
			rADCUPDN &= ~(0x1);		// Clear down state
			rADCDLY=(get_PCLK()/1000000)*500;// 500us delay(input stable)
 			rADCTSC = 0x1;			// X-position measurement state
 			
			if((method==1)|(method==3))	// read start
			{
				rADCCON |= (1<<1);		// Enable read_start
				rADCDAT0;				// Read_start
			}
			else						// Manual start
				rADCCON |= 0x1;			// Start ADC
			//Uart_Printf("Stylus Down\n");
			touch_down = 1;
			

		}
		else if(rADCUPDN == 0x2)// Stylus Up
		{
			rADCUPDN &= ~(0x1<<1);	// Cleart Up state
			//Uart_Printf("Stylus Up\n\n");
			touch_down = 0;
			adc_finish = 0;
			rADCTSC = 0xd3;			// Down detect, Waiting for interrupt
		}
	}

	ClearPending((U32)BIT_ADC);
	
	rINTMSK &= ~BIT_ADC;       //ADC Touch Screen Mask bit clear
	rINTSUBMSK &= ~(BIT_SUB_ADC | BIT_SUB_TC);
}


/****************************全局变量定义结束************************************/

/****************************全局函数定义开始************************************/
void AdcTsAuto(void);
/****************************全局函数定义结束************************************/

/*************************************************
    函数名称:       adc_ts_init
    功能描述:       触摸屏初始化
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void TouchPanel_HardInit_u(void)
{
	
	Uart_Printf("[ADC touch screen test.]\n");

	Uart_Printf("Select measue/start method\n");
	Uart_Printf("0.Manual start/ Seperate measure\n");
	Uart_Printf("1.Read	 start/ Seperate measurement\n");
	Uart_Printf("2.Manual start/ Auto	 measure\n");
	Uart_Printf("3.Read	 start/ Auto	 measurement\n");
    //Uart_Printf("[ADC touch screen test.]\n");
    
    adcpsr=(RAW_U32)((get_PCLK()/adcclock)-1);
    //Uart_Printf("PSR=%d, ADC freq.=%dHz\n", adcpsr, (int)(get_PCLK()/(adcpsr+1)));
    
    ADCDLY_REG=(int)(xtal/1000)*2;  // 2ms delay(filter effect)
    //Uart_Printf("ADCDLY = %d\n", ADCCON_REG);

	method = 1;

	if((method==1)|(method==3))//read start
		rADCCON = rADCCON & ~(0xffff) | (1<<14) | (adcpsr<<6) |(1<<3)|(0<<2)|(1<<1);//read start
	else
		rADCCON = rADCCON & ~(0xffff) | (1<<14) | (adcpsr<<6) |(1<<3)|(0<<2);//manual start
		
    
    ADCTSC_REG = 0xd3/*0xd7*/;  //Wfint,XP_PU_dis,XP_Dis,XM_Dis,YP_Dis,YM_En

	if((method==0)|(method==1))
		register_irq(31,AdcTsSep);	// Seperate measurement
	else
		register_irq(31, AdcTsAuto);;	// Auto measurement
        
  
    SUBSRCPND_REG |= (RAW_U32)BIT_SUB_ADC;// Clear previous pending bit
    INTMSK_REG &= ~BIT_ADC;       // ADC Touch Screen Mask bit clear
    INTSUBMSK_REG &=~(BIT_SUB_ADC | BIT_SUB_TC);
    
    //Uart_Printf("\nTouch panel initial over!... \n\n");
    //Uart_Printf("\nWating Stylus Down... \n\n");

	

	#if 0
    INTSUBMSK_REG |= (BIT_SUB_ADC|BIT_SUB_TC);
    INTMSK_REG |= (RAW_U32)BIT_ADC;
    ADCCON_REG &= ~(1<<14);// disable prescaler
    ADCCON_REG |= (1<<2);   // Standby
	#endif
	

}

/*************************************************
    函数名称:       AdcTsAuto
    功能描述:       AD & TS int
    子 函 数:       
    调用限制:       
    输入参数:       无
    输出参数:       无
    返 回 值:       无
    说    明:       
*************************************************/
void AdcTsAuto(void)
{

    INTMSK_REG |= (RAW_U32)BIT_ADC;     //ADC Touch Screen Mask bit clear
    INTSUBMSK_REG |= (BIT_SUB_ADC | BIT_SUB_TC);
	
    //三星原厂代码
    if(SUBSRCPND_REG & (0x1<<10))       // ADC interrupt
    {
        xdata = (ADCDAT0_REG&0xfff);
        ydata = (ADCDAT1_REG&0xfff);
		
        Uart_Printf("count=%d XP=%04d, YP=%04d\n", count++, xdata, ydata);

        ADCTSC_REG = 0x1d3;         // Up detect, Waiting for interrupt

        // 清INT_ADC中断
        SUBSRCPND_REG |= BIT_SUB_ADC;
        SRCPND_REG    |= BIT_ADC;
        INTPND_REG    |= BIT_ADC;
    }
    else if(SUBSRCPND_REG & (0x1<<9))   // TC interrupt
    {
		if(rADCUPDN == 0x1)			// Stylus Down
		{
			rADCUPDN &= ~(0x1);		// Clear Down state
			rSUBSRCPND |= BIT_SUB_TC;
			//Uart_Printf("Stylus Down\n");
			touch_down = 1;
			
			rADCDLY=(get_PCLK()/1000000)*500;// 500us delay(input stable)
			rADCTSC = 1<<2;			// Auto conversion mode

			if((method==1)|(method==3))	// read start
			{
				rADCCON |= (1<<1);		// Enable read_start
				rADCDAT0;				// Read_start
			}
			else {
				rADCCON |= 0x1;			// Start ADC
			}
		}
		else if(rADCUPDN == 0x2)	// Stylus Up
		{
			rADCUPDN &= ~(0x1<<1);	// Clear Up state
			//Uart_Printf("Stylus Up\n\n");
			touch_down = 0;
			rADCTSC = 0xd3;    		// Down detect, Waiting for interrupt
		}
		
      
    }
	
	ClearPending((U32)BIT_ADC);

	rINTMSK &= ~BIT_ADC;	   //ADC Touch Screen Mask bit clear
	rINTSUBMSK &= ~(BIT_SUB_ADC | BIT_SUB_TC);


}

int ts_get_x_data_raw_u(void)
{
	if (touch_down && adc_finish) {
		
		//return ((x_data[TS_SAMPLE_COUNT >> 1] + x_data[TS_SAMPLE_COUNT >> 1 + 1]) / 2);
		return (x_data[TS_SAMPLE_COUNT >> 1]);
	}

	return -1;
}

int ts_get_y_data_raw_u(void)
{ 
	if (touch_down && adc_finish) {
		
		//return ((y_data[TS_SAMPLE_COUNT >> 1] + y_data[TS_SAMPLE_COUNT >> 1 + 1]) / 2);
		return (y_data[TS_SAMPLE_COUNT >> 1]);
		
	}
	
	return -1;
}


int touch_state_set(void)
{	
	
	

	return touch_down;
	
}


int touch_state_pressed(void)
{

	return touch_down;

}




