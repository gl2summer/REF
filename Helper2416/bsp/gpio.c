#include <stdio.h>
#include <2416_reg.h>
#include "raw_api.h"
#include "timer.h"
#include "uart.h"
#include "irq.h"


#define GPIO_STK_SIZE 2048

PORT_STACK gpio_task_stack[GPIO_STK_SIZE];

RAW_TASK_OBJ gpio_task_obj[10];

//typedef void (*ISR_FUNCTION)(void *);
//extern void register_irq (int irq_num, ISR_FUNCTION fun);

static void delay(int time);
static void delay (int time) 
{
	int i,j,k;
	while(time--){
		for( i = 1000; i > 0; i--) {
			for (j = 1000; j > 0; j--) {
				for (k = 1000; k > 0; k--);
			}
		}
	}
}

void run(void)
{
	GPKDAT_REG |= 0xf << 5;
	while (1) {
		GPKDAT_REG &= ~( 0x01 << 5);
		delay(200);
		GPKDAT_REG |= (0x01 << 5);
		GPKDAT_REG &= ~( 0x01 << 6);
		delay(200);
		GPKDAT_REG |= (0x01 << 6);
		GPKDAT_REG &= ~( 0x01 << 7);
		delay(200);
		GPKDAT_REG |= (0x01 << 7);
		GPKDAT_REG &= ~( 0x01 << 8);
		delay(200);
		GPKDAT_REG |= (0x01 << 8);
	}
}
void gpio_init(void * pPagm)
{
	int i=1;
	GPKCON_REG &= ~(0xff << 10);
	GPKCON_REG |= (0x55 << 10);
	GPKDAT_REG &= ~(0xf << 5);
	
	raw_sleep(200);
	
	Uart_Printf(" light is run\n");
	GPKDAT_REG |= 0xf << 5;
	while (i++) {
		Uart_Printf("%d\n", i-1);
		
		GPKDAT_REG &= ~( 0x01 << 5);
		raw_sleep(50);
		GPKDAT_REG |= (0x01 << 5);
		GPKDAT_REG &= ~( 0x01 << 6);
		raw_sleep(50);
		GPKDAT_REG |= (0x01 << 6);
		GPKDAT_REG &= ~( 0x01 << 7);
		raw_sleep(50);
		GPKDAT_REG |= (0x01 << 7);
		GPKDAT_REG &= ~( 0x01 << 8);
		raw_sleep(50);
		GPKDAT_REG |= (0x01 << 8);
		/*
		GPKDAT_REG &= ~(0xf << 5);
		raw_sleep(200);
		GPKDAT_REG |= 0xf << 5;
		raw_sleep(200);
		*/
	}
	//run();
}

void gpio_test(void)
{
	
	Uart_Printf(" light is run\n");
	raw_task_create(&gpio_task_obj[1], (RAW_U8 *)"gpio_test", 0, 10, 0, gpio_task_stack, GPIO_STK_SIZE, gpio_init, 1);
}
