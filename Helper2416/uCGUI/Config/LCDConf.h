#ifndef LCDCONF_H
#define LCDCONF_H

#include <project_def.h>

#if (LCD_SIZE_4_3 == 1)

#define LCD_XSIZE          (480)       //水平像素  (480)
#define LCD_YSIZE          (272)       //垂直像素  (272)

#else
#define LCD_XSIZE          (800)       //水平像素  (480)
#define LCD_YSIZE          (480)       //垂直像素  (272)

#endif


#define LCD_CONTROLLER     (9320)      //控制器型号
#define LCD_BITSPERPIXEL   (16)        //总线宽度
#define LCD_FIXEDPALETTE   (565)       //色彩格式 , r=5,g=6,b=5
#define LCD_SWAP_RB        (1)         //是否红蓝交换

#endif /* LCDCONF_H */
