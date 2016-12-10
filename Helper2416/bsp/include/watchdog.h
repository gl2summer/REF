#ifndef WATCHDOG_H
#define WATCHDOG_H
#include "raw_api.h"

void watchdog_reset(RAW_U16 timeout);
void watchdog_feed(void);
void watchdog_close(void);
void watchdog_timer(RAW_U16 timeout);

#endif
