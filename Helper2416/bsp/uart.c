#include <raw_api.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <2416_reg.h>
#include <uart.h>
#include <fifo.h>
#include <irq.h>

static struct raw_fifo uart0_receive_fifo;



static char fifo0_receive_buf[1024];


static RAW_SEMAPHORE uart0_receive_sem;

RAW_SEMAPHORE uart2_receive_sem;


void Uart_Printf(char *fmt,...)
{
	va_list ap;
	char string[1024];

	va_start(ap,fmt);
	vsprintf(string,fmt,ap);
	va_end(ap);

	Uart_0_SendString(string); //default send from uart0

}

void Uart_0_Isr(void)
{
	int i = 0;
	char tmp = 0;
	int cnt = 0;
	//Uart_Printf("uart 0 irq entry\n");
	SRCPND_REG |= (0x1 << 28);
	INTPND_REG |= (0x1 << 28);
	if (SUBSRCPND_REG & (0x1)) {
		//receive irq
		SUBSRCPND_REG |= (0x1);
		cnt = UFSTAT0_REG & 0x3F;
		//Uart_Printf("receive fifo count is %d\n", cnt);
		for (i = 0; i < cnt; ++i) {
			tmp = Uart_0_ReceiveByte();
			fifo_in(&uart0_receive_fifo, (const void *)&tmp, (RAW_U32)1);
		}
		raw_semaphore_put(&uart0_receive_sem);
	}
	if (SUBSRCPND_REG & (0x2)) {
		//transmit irq
		SUBSRCPND_REG |= (0x2);
		return;
	}
	if (SUBSRCPND_REG & (0x4)) {
		//err irq
		SUBSRCPND_REG |= (0x4);
		return;
	}
}


static	void uart_set_baud(unsigned int baud)
{
	int i;
	unsigned int	PclkHZ;	   

	PclkHZ	= get_PCLK();		

	UBRDIV0_REG = (int)(PclkHZ/(baud*16))-1;
	UDIVSLOT0_REG = 0x0888;

}



void Uart_0_Init(void)
{
	//Uart_Printf("Uart 0 init entry!\n");

	/*init GPIO*/
	GPHCON &= ~(0xf);
	GPHCON |= 0xa;
	
	raw_semaphore_create(&uart0_receive_sem, "uart0receive", 0); //dont care about muilt task
	fifo_init(&uart0_receive_fifo, (void *)fifo0_receive_buf, (unsigned long)1024);
	register_irq(28, Uart_0_Isr);
	//interrupt offset of uart0 is 28
	/************************
	*notice: interrupt has enabled all interrupts includes uart errors, uart transmit and receive
	*we just need receive[or include error deal function], so just ignore transmit interrupt in isr
	************************/
	/*uart interrupt registers*/
	INTMOD_REG &= ~(1 << 28);
	INTSUBMSK_REG |= (0x7); // mask all uart0's interrupt request
	INTSUBMSK_REG &= ~(0x1); //enable  receive interrupt request of uart0
	
	/*uart registers*/
	ULCON0_REG = 0x3;
	UFCON0_REG = (0xa << 4) | 0x7;
	UMCON0_REG = 0;
	//UBRDIV0_REG = 35;
	//UDIVSLOT0_REG = 0x0888;
	uart_set_baud(115200);
	UCON0_REG = 0x5|(0x1 << 7)|(0x2 << 10);
	INTMSK_REG &= ~(1 << 28);
	Uart_Printf("Uart 00 init over\n");
}



void Uart_0_SendByte(char b)
{
	while (!(UTRSTAT0_REG & 0x2));
	UTXH0_REG = b;
}


void Uart_0_SendString(char *s)
{
	char *tmp = s;
	while (*tmp) {
		if ('\n' == (char)*tmp) {
			Uart_0_SendByte('\r');
		}
		Uart_0_SendByte(*tmp);
		++tmp;
	}
}


int Uart_0_ReceiveString(void *buf, int cnt, int timeout)
{
	int ret= 0;
	raw_memset((RAW_VOID *)buf, (RAW_U8)0, cnt);
	//Uart_Printf("ready to suspend\n");
	ret = raw_semaphore_get(&uart0_receive_sem, timeout);
	//Uart_Printf("get semaphore+++++++++++++\n");
	if (RAW_SUCCESS == ret) {
		ret = fifo_out(&uart0_receive_fifo, buf, cnt);
	} else if (RAW_BLOCK_TIMEOUT == ret) {
		ret = 0;
	}

	return ret;
}

char Uart_0_ReceiveByte(void)
{
	return URXH0_REG;
}

