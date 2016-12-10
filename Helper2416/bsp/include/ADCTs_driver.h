#ifndef _ADC_TS_DRIVER_
#define _ADC_TS_DRIVER_
#include "raw_type.h"


/*************************对外函数申明开始*************************************/

void adc_ts_init(void);
unsigned short TPReadX(void);
unsigned short TPReady(void);
void TPReadXY(void);



/*************************对外函数申明开结束***********************************/



#endif
