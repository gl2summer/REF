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
/*set Prescaler_value = 0xff(255),Division_factor = 128, ʱ��Ƶ��Ϊ2037.048Hz
 *���Ź���ʱ��ʱ��Ƶ�� = 1/[PCLK/(Prescaler value + 1)/Division_factor] 
 *                     = 1/(66750000/(255+1)/128) = 1/2037
 */
 

	if (!timeout) {                          	// �ж�����Ŀ��Ź���ʱ����ʱ���Ƿ�Ϊ0
		WTDAT_REG = 0x8000;                 	// �������timeoutΪ0,��ʹ��WTDAT�Ĵ���Ĭ��ֵ0x8000, 
	} else {											// ���Ź���ʱ����ʱʱ��Ϊtimeout = 0x8000 /2037 = 16.086s
		WTDAT_REG  = 0xffff & (2037 * timeout);	// ��Ϊ��0ֵ,���ÿ��Ź���ʱ����ʱʱ��Ϊtimeout�루2037 �� 2037 * timeout��
	}									        // ���Ź����ʱʱ��Ϊ32.17s

	WTCNT_REG  = WTDAT_REG;						// 	��ʼ�����Ź������Ĵ���

	
	WTCON_REG |= (( 0x01 << 5 ) | 0x01);  //���ÿ��Ź�����reset
}


/* �����Ĺ��� : ���Ĵ��� WTCNT���¸�ֵΪ WTDAT, ��"ι��" */
void watchdog_feed(void)
{
	WTCNT_REG = WTDAT_REG;
}

//�رտ��Ź�����
void watchdog_close(void)
{
	WTCON_REG &= ~(( 1 << 5 ) | ( 1 << 2 ) | ( 0x01 << 0));	//�رտ��Ź��ж�,�رո�λ
		
	INTSUBMSK_REG |= (0x1 << 27);							//�رտ��Ź��жϷ���
	INTMSK_REG    |= (0x1 << 9);
}


/*�����Ź����ó���ͨ��ʱ�� */
void watchdog_timer(RAW_U16 timeout)
{
	WTCON_REG = (0xff << 8) | (0x03 << 3);  
//	WTCON_REG &= ~(0x01 << 0);	//�ر�reset

	if (!timeout) {                          	// �ж�����Ŀ��Ź���ʱ����ʱ���Ƿ�Ϊ0
		WTDAT_REG  = 0x4f92;                 	// �������timeoutΪ0,��ʹ��WTDAT�Ĵ���Ĭ��ֵ0x4f92(��20370), 
	} else {											// ���Ź���ʱ����ʱʱ��Ϊtimeout = 0x8000 /2037 = 16.086s
		WTDAT_REG  = 0xffff & (2037 * timeout);	// ��Ϊ��0ֵ,���ÿ��Ź���ʱ����ʱʱ��Ϊtimeout�루2037 �� 2037 * timeout��
	}									        // ���Ź����ʱʱ��Ϊ32.17s

	WTCNT_REG  = WTDAT_REG;						// 	��ʼ�����Ź������Ĵ���

	INTSUBMSK_REG &= ~(0x01 << 27);
	INTMSK_REG    &= ~(0x01 << 9);

	register_irq( 9 , watchdog_irq);
//    Uart_Printf("---------------\n");
	
	WTCON_REG |= ( 0x01 << 5 ) | (0x01 << 2 ); //���ÿ��Ź���ʱ������, �жϿ���
}

