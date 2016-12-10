#include <raw_api.h>
#include <2416_reg.h>
#include <irq.h>
#include <timer.h>

extern RAW_U32 get_PCLK(void);

static void timer_0_ISRService(void)
{
	//Uart_Printf("timer_0_ISRSerivce\n");
	SRCPND_REG |= (0x01<<10);			/* write 1 to clear */
	INTPND_REG |= (0x01<<10);			/* write 1 to clear */
	raw_time_tick();
}

static void timer_0_ISRInit(void)
{
	INTMOD_REG = 0x00000000;
	PRIORITY_MODE_REG = 0x00000000;
	INTMSK_REG &= ~(0x01<<10);		/* set timer0 interrupt available */
}

/*
 *Tclk = PCLK/{precaler+1}/{divider value}
 *precaler = PCLK/{Tclk*{divider value}} - 1
 */
void timer_0_init(void)
{
	TCON_REG  &= ~0x0F;		/* default timer0*/
	TCFG0_REG &= ~0xFF;		
	TCFG1_REG &= ~0x0F;

	timer_0_ISRInit();

	register_irq(10, timer_0_ISRService);
	
	TCFG0_REG |= 15;			/* prescaler: 15 */
	TCFG1_REG |= 0x01;		/* divider value :1/4*/
	TCNTB0_REG = get_PCLK()/((15 + 1) * 4 * RAW_TICKS_PER_SECOND);

	TCON_REG |= 0x02;			/* update TCNTB0&TCMPB0 */
	TCON_REG &= ~0x02;			/* must clear 'manual update', note page 297*/
	TCON_REG |= 0x09;			/* 1001-- auto reload, start timer0 */
}
