#ifndef _ARC_H
#define _ARC_H

#ifdef __cplusplus
extern "C" {
#endif

 typedef struct Date
	{ 
		RAW_U16 year;
		RAW_U8 month, day, hour, minute, second, weekday;
	}Date;

void rtc_0_init (void);
void set_rtc_date (RAW_U16 year, RAW_U8 month, RAW_U8 day, RAW_U8 hour, RAW_U8 minute, RAW_U8 second, RAW_U8 weekday);
struct Date get_rtc_time (void);




#ifdef __cplusplus
}
#endif


#endif	//_ARC_H
