#ifndef	IRQ_H
#define	IRQ_H

typedef  void    (*ISR_FUNCTION)();

void register_irq(int irq_number, ISR_FUNCTION fun);

#endif
