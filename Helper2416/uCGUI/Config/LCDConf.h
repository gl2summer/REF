#ifndef LCDCONF_H
#define LCDCONF_H

#include <project_def.h>

#if (LCD_SIZE_4_3 == 1)

#define LCD_XSIZE          (480)       //ˮƽ����  (480)
#define LCD_YSIZE          (272)       //��ֱ����  (272)

#else
#define LCD_XSIZE          (800)       //ˮƽ����  (480)
#define LCD_YSIZE          (480)       //��ֱ����  (272)

#endif


#define LCD_CONTROLLER     (9320)      //�������ͺ�
#define LCD_BITSPERPIXEL   (16)        //���߿��
#define LCD_FIXEDPALETTE   (565)       //ɫ�ʸ�ʽ , r=5,g=6,b=5
#define LCD_SWAP_RB        (1)         //�Ƿ��������

#endif /* LCDCONF_H */
