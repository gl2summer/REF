/*
 * This file is subject to the terms of the GFX License. If a copy of
 * the license was not distributed with this file, you can obtain one at:
 *
 *              http://ugfx.org/license.html
 */

#include "gfx.h"

#if (GFX_USE_GINPUT && GINPUT_NEED_MOUSE) /*|| defined(__DOXYGEN__)*/


#include "src/ginput/driver_mouse.h"

#include "ginput_lld_mouse_board.h"

#include "ADCTs_driver.h"
#include <math.h>

static coord_t	lastx, lasty;
extern int	ts_get_x_data_raw_u(void);
extern int	ts_get_y_data_raw_u(void);
extern int touch_state_pressed(void);


static uint16_t sampleBuf[7];
static void filter(void) {
	uint16_t temp;
	int i,j;

	for(i = 0; i < 4; i++) {
		for(j = i; j < 7; j++) {
			if(sampleBuf[i] > sampleBuf[j]) {
				/* Swap the values */
				temp = sampleBuf[i];
				sampleBuf[i] = sampleBuf[j];
				sampleBuf[j] = temp;
			}
		}
	}
}

void ginput_lld_mouse_init(void) {
	init_board();
}

void ginput_lld_mouse_get_reading(MouseReading *pt) {
	uint16_t i;
	static uint8_t flag = 0;
	static coord_t	ax, ay;
	static coord_t	bx, by;
	// If touch-off return the previous results
	if (!touch_state_pressed()) 
	{
		pt->x = lastx;
		pt->y = lasty;
		pt->z = 0;
		pt->buttons = 0;
		flag = 0;
		return;
	}
	lastx = (coord_t)ts_get_x_data_raw_u();
	lasty = (coord_t)ts_get_y_data_raw_u();
	if((lastx==-1)&&(lasty==-1))
	{
		pt->x = lastx;
		pt->y = lasty;
		pt->z = 0;
		pt->buttons = 0;
		flag = 0;
		return;
	}
	
	if(flag == 0)
	{
			ax = (coord_t)ts_get_x_data_raw_u();
			ay = (coord_t)ts_get_y_data_raw_u();
			pt->x = lastx;
			pt->y = lasty;
			pt->z = 0;
			pt->buttons = 0;
			flag = 1;
			return;
	}
	else if(flag == 1)
	{
			bx = (coord_t)ts_get_x_data_raw_u();
			by = (coord_t)ts_get_y_data_raw_u();
			if ((abs(ax - bx) < 20) && (abs(ay - by) < 20)) 
			{
					lastx = (ax+bx)>>1;
					lasty = (ay+by)>>1;
					pt->x = lastx;
					pt->y = lasty;
					pt->z = 100;
					pt->buttons = GINPUT_TOUCH_PRESSED;
					flag = 2;
					return;
			}
			else
			{
					pt->x = lastx;
					pt->y = lasty;
					pt->z = 0;
					pt->buttons = 0;
					flag = 0;
					return;
			}
	}
	else if(flag == 2)
	{
			pt->x = lastx;
			pt->y = lasty;
			pt->z = 100;
			pt->buttons = GINPUT_TOUCH_PRESSED;
			flag = 2;
			return;
	}
	return;
}

#endif /* GFX_USE_GINPUT && GINPUT_NEED_MOUSE */
