#include "uart.h"
#include "2416_reg.h"

#include "timer.h"
#include "irq.h"
#include "watchdog.h"
#include "raw_api.h"


extern void register_irq(int irq_number,ISR_FUNCTION fun);


void watchdog_irq(void)
{
//	Uart_Printf("++++++++++++\n");
	SUBSRCPND_REG |= (0x1 << 27);

	SRCPND_REG |= (0x1 << 9);	//INT_WDT M[9] 0 = Not requested, 1 = Requested
	INTPND_REG |= (0x1 << 9);
}


void watchdog_reset(RAW_U16 timeout)
{
	
	
	WTCON_REG = (0xff << 8) | (0x03 << 3);  
/*set Prescaler_value = 0xff(255),Division_factor = 128, 时钟频率为2037.048Hz
 *看门狗定时器时钟频率 = 1/[PCLK/(Prescaler value + 1)/Division_factor] 
 *                     = 1/(66750000/(255+1)/128) = 1/2037
 */
 

	if (!timeout) {                          	// 判断输入的看门狗定时器超时间是否为0
		WTDAT_REG = 0x8000;                 	// 若传入的timeout为0,则使用WTDAT寄存器默认值0x8000, 
	} else {											// 看门狗定时器超时时间为timeout = 0x8000 /2037 = 16.086s
		WTDAT_REG  = 0xffff & (2037 * timeout);	// 若为非0值,设置看门狗定时器超时时间为timeout秒（2037 ÷ 2037 * timeout）
	}									        // 看门狗最大超时时间为32.17s

	WTCNT_REG  = WTDAT_REG;						// 	初始化看门狗计数寄存器

	
	WTCON_REG |= (( 0x01 << 5 ) | 0x01);  //设置看门狗开启reset
}


/* 函数的功能 : 将寄存器 WTCNT重新赋值为 WTDAT, 即"喂狗" */
void watchdog_feed(void)
{
	WTCNT_REG = WTDAT_REG;
}

//关闭看门狗功能
void watchdog_close(void)
{
	WTCON_REG &= ~(( 1 << 5 ) | ( 1 << 2 ) | ( 0x01 << 0));	//关闭看门狗中断,关闭复位
		
	INTSUBMSK_REG |= (0x1 << 27);							//关闭看门狗中断服务
	INTMSK_REG    |= (0x1 << 9);
}


/*将看门狗设置成普通计时器 */
void watchdog_timer(RAW_U16 timeout)
{
	WTCON_REG = (0xff << 8) | (0x03 << 3);  
//	WTCON_REG &= ~(0x01 << 0);	//关闭reset

	if (!timeout) {                          	// 判断输入的看门狗定时器超时间是否为0
		WTDAT_REG  = 0x4f92;                 	// 若传入的timeout为0,则使用WTDAT寄存器默认值0x4f92(即20370), 
	} else {											// 看门狗定时器超时时间为timeout = 0x8000 /2037 = 16.086s
		WTDAT_REG  = 0xffff & (2037 * timeout);	// 若为非0值,设置看门狗定时器超时时间为timeout秒（2037 ÷ 2037 * timeout）
	}									        // 看门狗最大超时时间为32.17s

	WTCNT_REG  = WTDAT_REG;						// 	初始化看门狗计数寄存器

	INTSUBMSK_REG &= ~(0x01 << 27);
	INTMSK_REG    &= ~(0x01 << 9);

	register_irq( 9 , watchdog_irq);
//    Uart_Printf("---------------\n");
	
	WTCON_REG |= ( 0x01 << 5 ) | (0x01 << 2 ); //设置看门狗计时器开启, 中断开启
}

