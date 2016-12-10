#include "timer.h"
#include "uart.h"
#include "raw_api.h"
#include <2416_reg.h>
#include <irq.h>

#include "rtc.h"


RAW_U8 *week[7] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};




void rtc_0_irq(void)
{
	

	SRCPND_REG |= (0x01 << 30);
//	INTMOD_REG &= ~(0x01 << 30);
	INTPND_REG |= (0x01 << 30);
//	INTMSK_REG |= (0x01 << 30);
	
}

void rtc_0_init (void)
{
//	RAW_U32 counter;

	INTMOD_REG &= ~(0x01 << 30);
	INTMSK_REG &= ~(0x01 << 30);

//	RTCCON_REG &= ~(0x1f);
	RTCCON_REG |= 0x19;    //设置频率为2^15
	TICNT_REG  &= ~(0x7f);
	TICNT_REG  |= (0x01 << 7);
	TICNT1_REG &= ~(0xff);
	TICNT2_REG &= ~(0xffff);
	TICNT2_REG |= (0x04);
//  tick time count value 储存在counter里
//	counter = (((TICNT_REG & 0x7f) << 8) + (TICNT1_REG & 0xff) + ((TICNT2_REG & 0xffff) << 15));

	RTCALM_REG = 0x7f;
//初始化日期  时间  星期	
	set_rtc_date(2000,1,1,0,0,0,6);
	
	register_irq (30, rtc_0_irq);
	
}

void set_rtc_date (RAW_U16 year, RAW_U8 month, RAW_U8 day, RAW_U8 hour, RAW_U8 minute, RAW_U8 second, RAW_U8 weekday)
{
	year = year % 100;
 	RTCCON_REG |= 0x01;
 	BCDSEC_REG  = 0xff & ((second / 10) << 4) | (second % 10);
	BCDMIN_REG  = 0x7f & ((minute / 10) << 4) | (minute % 10);
	BCDHOUR_REG = 0x3f & ((hour	  / 10) << 4) | (hour % 10);
	BCDDATE_REG = 0x3f & ((day    / 10) << 4) | (day % 10);
	BCDMON_REG  = 0x1f & ((month  / 10) << 4) | (month % 10);
	BCDYEAR_REG = 0xff & ((year   / 10) << 4) | (year % 10);
	BCDDAY_REG  = weekday & 0x7;

	RTCCON_REG &= ~(0x01);
}


struct Date get_rtc_time (void)
{	
	Date nowtime;
	nowtime.year	= ((BCDYEAR_REG & 0xf0) >> 4) *10  + (BCDYEAR_REG & 0xf) + 2000;
	nowtime.month	= ((BCDMON_REG  & 0x10) >> 4) *10  + (BCDMON_REG  & 0xf);
	nowtime.day		= ((BCDDATE_REG & 0x30) >> 4) *10  + (BCDDATE_REG & 0xf);
	nowtime.hour	= ((BCDHOUR_REG & 0x30) >> 4) *10  + (BCDHOUR_REG & 0xf);
	nowtime.minute	= ((BCDMIN_REG  & 0x70) >> 4) *10  + (BCDMIN_REG  & 0xf);
	nowtime.second	= ((BCDSEC_REG  & 0xf0) >> 4) *10  + (BCDSEC_REG  & 0xf);
	nowtime.weekday	= BCDDAY_REG;
	return 	nowtime;
}

