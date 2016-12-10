#include <raw_api.h>
#include <lib_string.h>
#include <2416_reg.h>
#include <pwm.h>
#include <irq.h>
#include <timer.h>
#include <pwm.h>
#include <rsh.h>
extern RAW_U32 get_PCLK(void);



static void timer_0_ISRInit(void )
{
		INTMOD_REG = 0x00000000;
		PRIORITY_MODE_REG = 0x00000000;
		INTMSK_REG &= ~(0x1 << 10);				//open the time_0_ISR
	  //INTMSK_REG
}


static void timer_1_ISRInit(void)
{
		INTMOD_REG = 0x00000000;
		PRIORITY_MODE_REG = 0x00000000;
		INTMSK_REG &= ~(0x1 << 11);
	Uart_Printf("tim1 inintn"); 
}

static void timer_2_ISRInit(void)
{
	INTMOD_REG = 0x00000000;
	PRIORITY_MODE_REG = 0x00000000;
	INTMSK_REG &= ~(0x1 << 12);
}

static void timer_3_ISRInit(void)
{
	INTMOD_REG = 0x00000000;
	PRIORITY_MODE_REG = 0x00000000;
	INTMSK_REG &= ~(0x1 << 13);
	
}

static void timer_0_ISRService( void )
{
		SRCPND_REG |= (0x01 << 10);
		INTPND_REG |= (0x01 << 10);
		// light the led
	
	
	
}

static void timer_1_ISRService(void )
{
	SRCPND_REG |= ( 0x01 << 11);
	INTPND_REG |= ( 0x01 << 11);
	Uart_Printf("AAAAAAAAAAAAAAA");
	
	//light the led 
}

 //           timer_2_ISRService
static void timer_2_ISRService(void)
{ 
	SRCPND_REG |= (0x01 << 12);
	INTPND_REG |= (0x01 << 12);
	Uart_Printf("BBBBBBBBBBBBBBB");
	//LIGHT THE LED
}

static void timer_3_ISRService(void)
{
	SRCPND_REG |= (0x01 << 13);
	INTPND_REG |= (0x01 << 13);
	Uart_Printf("CCCCCCCCCCCCCCC");
	//LIGHT THE LED
}

int pwm_open(RAW_U32 channel)
{

		
	switch(channel)
	{
	 case 0:
			// set gpb0 as PWM TIMER0 to signal output 
			GPBCON_REG |= (0x1 << 2);
			GPBCON_REG &= ~(0x1);
		
		GPBCON_REG =GPBCON_REG &(~(0X03)) | (0X2);
			
			TCON_REG &= ~(0xF);
			TCFG0_REG &= ~(0xFF);
			TCFG1_REG &= ~(0x0F);
		
			timer_0_ISRInit();
		  register_irq(10, timer_0_ISRService);
		  register_irq(10, timer_0_ISRService);
		
		TCFG0_REG |= 15;							//prescaler = 15
		TCFG1_REG |= 0x01;							// divide value = 1/4
		TCNTB0_REG = get_PCLK()/(15*4*RAW_TICKS_PER_SECOND) - 1;
		TCMPB0_REG = TCNTB0_REG/2; //the dudy ratio is 0.5  
	 	Uart_Printf("pwm1 begin....................... \n  the dudy ratio is 0.5.......................... \n");   
		
		TCON_REG |= 0x2;
		TCON_REG &= ~0x2;
		TCON_REG |= 0x0F;

			break; 

		case 1:
			/* set gpBo1 as PWM TIMER1 to signal output*/
		Uart_Printf("pwm2 begin......................................... \n  the dudy ratio is 0.5............. \n ");    
		GPBCON_REG |= (0x1 << 3);
		GPBCON_REG &= ~(0x1 <<2);
		
		TCON_REG &= ~(0x0F << 8);
		TCFG0_REG &= ~(0xFF);
		TCFG1_REG &= ~(0xF0);
		
		timer_1_ISRInit();
		register_irq(11,timer_1_ISRService);
		
		TCFG0_REG |= 15;				//Prescaler = 15
		TCFG1_REG |= 0x10;				//divide value = 1/4
		TCNTB1_REG = get_PCLK()/(15*4*RAW_TICKS_PER_SECOND) - 1;
		TCMPB1_REG = TCNTB0_REG/2; //the dudy ratio is a
	 
		
		TCON_REG |= (0x1 << 9);
		TCON_REG &= ~0x200;
		TCON_REG |= (0x0b << 8); 
			break;
			
		case 2:
	  /*set gpio2 as pwm time2 to singal output*/
			GPBCON_REG |= (0x1 << 5);
			GPBCON_REG &= ~(0x1 << 4);
		
		TCON_REG &= ~(0x0F << 12);
		TCFG0_REG &= ~(0XFF00);
		TCFG1_REG &= ~(0xF00);
		Uart_Printf("pwm3 begin....................... \n  the dudy ratio is 0.5............... \n ");   
		
		timer_2_ISRInit();
		register_irq(12,timer_2_ISRService);
		
		TCFG0_REG |= (15 << 8);
		TCFG1_REG |= (0x1 << 8);
		TCNTB2_REG = get_PCLK()/(15*4*RAW_TICKS_PER_SECOND) - 1;
		TCMPB2_REG = TCNTB2_REG/2;
		
		TCON_REG |= (0x1 << 13);
		TCON_REG &= (0x1 << 13);
		TCON_REG |= (0xF << 12);
		
			break;
			
		case 3:
		//SET GPIO3 AS PWM SINGNAL OUTPUT 
		GPBCON_REG |= (0x1 << 7);
		GPBCON_REG &= ~(0x1 << 6);
		Uart_Printf("pwm4 begin....................... \n  the dudy ratio is 0.5 ......................\n");   
		TCON_REG &= ~(0x0F << 16);
		TCFG0_REG &= ~(0xFF00);
		TCFG1_REG &= ~(0x0F << 12);
		
		timer_3_ISRInit();
		register_irq(13,timer_3_ISRService);
		
		TCFG0_REG |= (15 << 8);
		TCFG1_REG |= (0x1 << 12);
		TCNTB3_REG = get_PCLK()/(15*4*RAW_TICKS_PER_SECOND) - 1;
		TCMPB3_REG = TCNTB3_REG/2;
		
		TCON_REG |= (0x1 << 17);
		TCON_REG &= ~(0x1 << 17);
		TCON_REG |= (0x0F << 16);
		
		break;
	}	
	return 1;
}

void pwm_close(int channel)
{
	switch(channel)
	{
	case 1:
		TCON_REG &=~0X1;
		INTMSK_REG |= (0X1 << 10);
	Uart_Printf("pwm1 closed....................... \n ");
	break;
	case 2:
		TCON_REG &= ~(0X1 << 8);
		INTMSK_REG |= (0X1 << 11);
		Uart_Printf("pwm2 closed....................... \n ");
	break;
	case 3:
		TCON_REG &= ~(0X1 << 12);
	  INTMSK_REG |= (0X1 << 12);
		Uart_Printf("pwm3 closed....................... \n" );
	break;
	case 4:
		TCON_REG &= ~(0X1 << 16);
		INTMSK_REG |= (0X1 << 13);
		Uart_Printf("pwm4 closed....................... \n" );
	break;
}
}

RAW_S32 pwmopen_shell(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{
	RAW_S32 pxParameterStringLength;
	const RAW_S8 *pcom;
	
	pcom = (RAW_S8 *)rsh_get_parameter(pcCommandString, 1, &pxParameterStringLength);

    if(((*pcom) >='0') || ((*pcom)<='4') )   		
{
	switch (*pcom){
	case '1':
  pwm_open(0);   
	break;
	
	case '2':
		pwm_open(1);
	break;
	
	case '3':
		pwm_open(2);
	break;
	
	case '4':
		pwm_open(3);
	break;
   }
}
	else 
		Uart_Printf(" input the wrong number,please input number1,2,3,4,\n");
return 1;
}
RAW_S32 pwmclose_shell(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{	RAW_S32 pxParameterStringLength;
	const RAW_S8 *pcom;
	
	pcom = (RAW_S8 *)rsh_get_parameter(pcCommandString, 1, &pxParameterStringLength);

    if(((*pcom) >='0') || (*pcom)<='4')    		
{
switch (*pcom){
	case '1':
  pwm_close(1);   
	break;
	
	case '2':
		pwm_close(2);
	break;
	
	case '3':
		pwm_close(3);
	break;
	case '4':
		pwm_close(4);
	break;
		}}
	else 
		Uart_Printf(" input the wrong number,please input number1,2,3,4,\n");
  
	return 1;
}		

	

