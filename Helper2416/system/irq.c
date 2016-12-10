#include <raw_api.h>
#include <2416_reg.h>
#include <irq.h>


ISR_FUNCTION ISR_REGISTER[32];


void register_irq(int irq_number, ISR_FUNCTION fun)
{

	ISR_REGISTER[irq_number] = fun;


}


static void default_interrupt()
{

	Uart_Printf("interrupt has not been installed, please check it\n");
	while (1);

}


void inerrupt_vector_init()
{
	RAW_S8 i;

	INTMSK_REG = 0xffffffff;
	SRCPND_REG = 0xffffffff;
	INTPND_REG = 0xffffffff;

	for (i = 0; i < 32; i++) {
		
		ISR_REGISTER[i] = default_interrupt;

	}

}



void  irq_process (void)
{
	int isr_offset = INTOFFSET_REG;
	ISR_REGISTER[isr_offset]();

}




void reset_exception_process()
{
	Uart_Printf("reset_exception_process\r\n");
	while (1);

}
void Undef_exception_process(unsigned int pc_point)
{
	
	Uart_Printf("Undef_exception_process\r\n");
	Uart_Printf("exception pc is %x\r\n", pc_point);
	while (1);

}
void SWI_exception_process()
{
	Uart_Printf("SWI_exception_process\r\n");
	while (1);

}
void Pabort_exception_process()
{
	Uart_Printf("Pabort_exception_process\r\n");
	while (1);

}
void Dabort_exception_process()
{
	Uart_Printf("Dabort_exception_process\r\n");
	while (1);

}
void FIQ_exception_process()
{
	Uart_Printf("FIQ_exception_process\r\n");
	while (1);

}

