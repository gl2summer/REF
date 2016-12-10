/*============================================================================*/
/*============================================================================*/
/*============================================================================*/
#include <raw_api.h>


extern RAW_U32 get_PCLK(void);


#define	ADC_REG_PA_BASE	0x58000000
#define	ADC_REG_VA_BASE	(ADC_REG*)ADC_REG_PA_BASE

typedef struct {
    unsigned int rADCCON;                      // control register
    unsigned int rADCTSC;                      // touch screen control register
    unsigned int rADCDLY;                      // start or interval delay register
    unsigned int rADCDAT0;                     // conversion data register 0
    unsigned int rADCDAT1;                     // conversion data register 1
    unsigned int rADCUPDN;						// touch screen up-down int check
    unsigned int rADCMUX;
}volatile ADC_REG;  



/*============================================================================*/

#define	TS_DBG	1

/*============================================================================*/
#define	ADC_CLK	4000000	/*5MHZ*/


// ADCTSC
#define UD_SEN_DOWN        (0<<8)
#define UD_SEN_UP            (1<<8)
#define YM_SEN_DIS            (0<<7)
#define YM_SEN_EN            (1<<7)
#define YP_SEN_EN            (0<<6)
#define YP_SEN_DIS            (1<<6)
#define XM_SEN_DIS            (0<<5)
#define XM_SEN_EN            (1<<5)
#define XP_SEN_EN            (0<<4)
#define XP_SEN_DIS            (1<<4)
#define PULL_UP_EN            (0<<3)
#define PULL_UP_DIS            (1<<3)
#define AUTO_PST_DIS        (0<<2)
#define AUTO_PST_EN        (1<<2)
#define XY_PST_NOP            (0<<0)
#define XY_PST_XPOS            (1<<0)
#define XY_PST_YPOS            (2<<0)
#define XY_PST_WAITINT        (3<<0)
#define XY_PST_MASK            (3<<0)

// ADCDLY
#define FILCLKSRC_EXTCLK    (0<<16)
#define FILCLKSRC_RTCCLK    (1<<16)
#define ADC_DELAY(n)        ((n+1)&0xffff)
#define ADC_DELAY_MASK        (0xffff)

// ADCDAT0
#define D_UPDOWN_DOWN    (0<<15)
#define D_UPDOWN_UP        (1<<15)
#define D_AUTO_PST_DIS        (0<<14)
#define D_AUTO_PST_EN        (1<<14)
#define D_XY_PST_NOP        (0<<12)
#define D_XY_PST_XPOS        (1<<12)
#define D_XY_PST_YPOS        (2<<12)
#define D_XY_PST_WAITINT    (3<<12)
#define D_XY_PST_MASK        (3<<12)

#define D_XPDATA_MASK(n)    ((n)&0xfff)
#define D_YPDATA_MASK(n)    ((n)&0xfff)

// ADCUPDN
#define TSC_DN_INT            (1<<0)
#define TSC_UP_INT            (1<<1)

// ADCCLRINT
#define CLEAR_ADC_INT        (0xff)    // can be any value...

// ADCCLRWK
#define CLEAR_ADCWK_INT    (0xff)    // can be any value...

#define ADCTSC_WAIT_PENDOWN    (UD_SEN_DOWN    |YM_SEN_EN|YP_SEN_DIS|XM_SEN_DIS|XP_SEN_DIS|PULL_UP_EN|AUTO_PST_DIS|XY_PST_WAITINT)
#define ADCTSC_WAIT_PENUP        (UD_SEN_UP        |YM_SEN_EN|YP_SEN_DIS|XM_SEN_DIS|XP_SEN_DIS|PULL_UP_EN|AUTO_PST_DIS|XY_PST_WAITINT)
#define ADCTSC_AUTO_ADC        (UD_SEN_DOWN    |YM_SEN_EN|YP_SEN_DIS|XM_SEN_DIS|XP_SEN_DIS|PULL_UP_DIS|AUTO_PST_EN|XY_PST_NOP)

/*============================================================================*/

/*============================================================================*/



static RAW_SEMAPHORE touch_sema;

/*============================================================================*/

static	void	xx_ts_isr(void)
{
	ADC_REG *adc=ADC_REG_VA_BASE;
	
#if 0
	if(rSUBSRCPND & BIT_SUB_ADC)		// ADC interrupt
	{
		ClearSubPending(BIT_SUB_ADC);

		if((method==1)|(method==3))	// read start
			rADCCON &= ~(1<<1);	// Disable read_start

		xdata=(rADCDAT0&0xfff);		// Read XY-position Conversion data
		ydata=(rADCDAT1&0xfff);
		DebugPrintf("count=%d XP=%04d, YP=%04d\n", count++, xdata, ydata);

		rADCTSC = 0x1d3;			// Up detect, Waiting for interrupt
	}
	else if(rSUBSRCPND & BIT_SUB_TC)	// TC interrupt
	{
		ClearSubPending(BIT_SUB_TC)
		
		if(rADCUPDN == 0x1)			// Stylus Down
		{
			rADCUPDN &= ~(0x1);		// Clear Down state
			DebugPrintf("Stylus Down\n");
			rADCDLY=(CPU_GetPclkHZ()/1000000)*500;// 500us delay(input stable)
			rADCTSC = 1<<2;			// Auto conversion mode

			if((method==1)|(method==3))	// read start
			{
				rADCCON |= (1<<1);		// Enable read_start
				rADCDAT0;				// Read_start
			}
			else
			{
				rADCCON |= 0x1;			// Start ADC
			}
		}
		else if(rADCUPDN == 0x2)	// Stylus Up
		{
			rADCUPDN &= ~(0x1<<1);	// Clear Up state
			DebugPrintf("Stylus Up\n\n");
			
			rADCTSC = 0xd3;    		// Down detect, Waiting for interrupt
		}
	}

	
	ClearPending(BIT_ADC);
	
#endif

}

static	void	ts_isr(void)
{
	ADC_REG *adc=ADC_REG_VA_BASE;
	
#if 0
	if(rSUBSRCPND & BIT_SUB_ADC)		// ADC interrupt
	{
		ClearSubPending(BIT_SUB_ADC);

		if((method==1)|(method==3))	// read start
			rADCCON &= ~(1<<1);	// Disable read_start

		xdata=(rADCDAT0&0xfff);		// Read XY-position Conversion data
		ydata=(rADCDAT1&0xfff);
		DebugPrintf("count=%d XP=%04d, YP=%04d\n", count++, xdata, ydata);

		rADCTSC = 0x1d3;			// Up detect, Waiting for interrupt
	}
	else if(rSUBSRCPND & BIT_SUB_TC)	// TC interrupt
	{
		ClearSubPending(BIT_SUB_TC)
		
		if(rADCUPDN == 0x1)			// Stylus Down
		{
			rADCUPDN &= ~(0x1);		// Clear Down state
			DebugPrintf("Stylus Down\n");
			rADCDLY=(CPU_GetPclkHZ()/1000000)*500;// 500us delay(input stable)
			rADCTSC = 1<<2;			// Auto conversion mode

			if((method==1)|(method==3))	// read start
			{
				rADCCON |= (1<<1);		// Enable read_start
				rADCDAT0;				// Read_start
			}
			else
			{
				rADCCON |= 0x1;			// Start ADC
			}
		}
		else if(rADCUPDN == 0x2)	// Stylus Up
		{
			rADCUPDN &= ~(0x1<<1);	// Clear Up state
			DebugPrintf("Stylus Up\n\n");
			
			rADCTSC = 0xd3;    		// Down detect, Waiting for interrupt
		}
	}

	
	ClearPending(BIT_ADC);
	
#endif

}

/*============================================================================*/

/*============================================================================*/

int	TouchPanel_HardInit_u(void)
{

	int prs;
	
	ADC_REG *adc=ADC_REG_VA_BASE;

	//ADS7843_Init();
/*
	INTR_Disable(INT_NUM_ADC);
	SUBINTR_Disable(INT_NUM_SUB_TC);
	
	INTR_ClearPend(INT_NUM_ADC);
	SUBINTR_ClearPend(INT_NUM_SUB_TC);
	
	INTR_SetHandler(INT_NUM_ADC,ts_isr);
*/	
	prs	= (get_PCLK()/ADC_CLK)+1; 
	
	adc->rADCDLY	= 4000;                  //Normal conversion mode delay about (1/3.6864M)*50000=13.56ms
    adc->rADCCON	= (1<<14) | (prs<<6) | (1<<3)| (0<<2)  ;	   //read start
    adc->rADCTSC	= 0xd3;  //Wfait,XP_PU,XP_Dis,XM_Dis,YP_Dis,YM_En

  	raw_semaphore_create(&touch_sema, (RAW_U8 *)"touch_sema", 1);
  /*  
    SUBINTR_Enable(INT_NUM_SUB_TC);
    INTR_Enable(INT_NUM_ADC);
  */
	return	1;
	


}

/*============================================================================*/

static	int	ts_is_down(void)
{
	//return	ADS7843_IsDown();	

	 
	ADC_REG *adc=ADC_REG_VA_BASE;
	unsigned char	i;
	///
	
	for(i=0;i<4;i++)	//¼ì²â8´Î
	{
		if((adc->rADCDAT0&(1<<15)))	return	0;
		if((adc->rADCDAT1&(1<<15)))	return	0;
		//for(j=0;j<20;j++);	//Delay
	}
	////
	//DebugPrintf("ts_down\r\n");
	return	1; 
}

/*============================================================================*/

/*============================================================================*/
//²åÈë·¨ÅÅÐò

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

#define	TS_SAMPLE_COUNT	16
static	int ad_buf[TS_SAMPLE_COUNT];

int	__ts_get_x_data_raw_u(void)
{
	int i,j,prs;
	int dat;
	ADC_REG *adc=ADC_REG_VA_BASE;
	/////

	raw_semaphore_get(&touch_sema, RAW_WAIT_FOREVER);
	
	prs	 = (get_PCLK()/ADC_CLK)+1;
	
	adc->rADCDLY	= 3000;
    adc->rADCCON	= (1<<14) | (prs<<6)|(1<<3)  ;	   //read start
	adc->rADCTSC 	= 0xd3;
	
	for(i=0;i<TS_SAMPLE_COUNT;i++)
	{
		for(j=200;j>0;j--);
		adc->rADCTSC = (adc->rADCTSC&~(0xF))|(0<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(1);// read x
		for(j=200;j>0;j--);	
	
		adc->rADCCON	|=0x1;                   //start ADC
				
		while(adc->rADCCON & 0x1);			//check if Enable_start is low
		while(!(adc->rADCCON & (1<<15)));     //check if EC(End of Conversion) flag is high, This line is necessary~!!
							
		
		ad_buf[i]	=adc->rADCDAT0&0xFFF;
			
			
	}
	
	adc->rADCTSC = 0xd3;
	
	InsertSort(ad_buf,0,TS_SAMPLE_COUNT-1);
	i = TS_SAMPLE_COUNT>>1;

	dat	= 0;
	dat += ad_buf[i++];
	dat += ad_buf[i++];		
	dat	= dat>>1;

	Uart_Printf("x is %d\n", dat);
	
//	DebugPrintf("ts_adx =%04XH\r\n",dat);
	raw_semaphore_put(&touch_sema);

  	return dat;
  	////	
}

int	__ts_get_y_data_raw_u(void)
{
	int i,j,prs;
	int dat;
	ADC_REG *adc=ADC_REG_VA_BASE;
	/////
	raw_semaphore_get(&touch_sema, RAW_WAIT_FOREVER);
	
	prs	 = (get_PCLK()/ADC_CLK)+1;
	
	adc->rADCDLY	= 3000;
    adc->rADCCON	= (1<<14) | (prs<<6)|(1<<3)  ;	   //read start
	adc->rADCTSC 	= 0xd3;
	
	for(i=0;i<TS_SAMPLE_COUNT;i++)
	{
		for(j=200;j>0;j--);
		adc->rADCTSC = (adc->rADCTSC&~(0xF))|(0<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(2);// read y
		for(j=200;j>0;j--);	
	
		adc->rADCCON	|=0x1;                   //start ADC
				
		while(adc->rADCCON & 0x1);			//check if Enable_start is low
		while(!(adc->rADCCON & (1<<15)));     //check if EC(End of Conversion) flag is high, This line is necessary~!!
							
		ad_buf[i]	=adc->rADCDAT1&0xFFF;
					
			
	}
	
	adc->rADCTSC = 0xd3;
	
	InsertSort(ad_buf,0,TS_SAMPLE_COUNT-1);
	i = TS_SAMPLE_COUNT>>1;

	
	dat	= 0;
	dat += ad_buf[i++];
	dat += ad_buf[i++];		
	dat	= dat>>1;

	Uart_Printf("y is %d\n", dat);
	
//	DebugPrintf("ts_ady =%04XH\r\n",dat);
	raw_semaphore_put(&touch_sema);

  	return dat;
  	////	
	
}

/*============================================================================*/
volatile int touch_down;

int touch_state_set(void)
{	
	
	raw_semaphore_get(&touch_sema, RAW_WAIT_FOREVER);
	touch_down = ts_is_down();
	raw_semaphore_put(&touch_sema);

	return touch_down;
	
}


int touch_state_pressed(void)
{

	return touch_down;

}


int ts_get_x_data_raw_u(void)
{

	if (touch_down) {

		return __ts_get_x_data_raw_u();
	}


	return -1;


}


int ts_get_y_data_raw_u(void)
{
	if (touch_down) {

		return __ts_get_y_data_raw_u();
	}

	return -1;
	
}

/*============================================================================*/

