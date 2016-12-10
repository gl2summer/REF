/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              �C/GUI is protected by international copyright laws. Knowledge of the
*              source code may not be used to write a similar product. This file may
*              only be used in accordance with a license and should not be redistributed
*              in any way. We appreciate your understanding and fairness.
*
----------------------------------------------------------------------
File        : GUITouch.Conf.h
Purpose     : Configures touch screen module
----------------------------------------------------------------------
*/


#ifndef __GUITOUCH_CONF_H
#define __GUITOUCH_CONF_H




#if (LCD_SIZE_4_3 == 1)

#define GUI_TOUCH_AD_LEFT       3180      	//���������AD����ֵ    
#define GUI_TOUCH_AD_RIGHT      985    	//�������Ҳ�AD����ֵ    
#define GUI_TOUCH_AD_TOP        2575     	//����������AD����ֵ   
#define GUI_TOUCH_AD_BOTTOM     1560    	//�������ײ�AD����ֵ  

#define GUI_TOUCH_SWAP_XY       0         //X��Y����ת
#define GUI_TOUCH_MIRROR_X      0         //X������
#define GUI_TOUCH_MIRROR_Y      0         //Y������

#else 

#define A 3225
#define B 890
#define C 1425
#define D 2775

#define GUI_TOUCH_AD_LEFT       A      	//���������AD����ֵ    
#define GUI_TOUCH_AD_RIGHT      B    	//�������Ҳ�AD����ֵ    
#define GUI_TOUCH_AD_TOP        C     	//����������AD����ֵ   
#define GUI_TOUCH_AD_BOTTOM     D    	//�������ײ�AD����ֵ

#define GUI_TOUCH_SWAP_XY       1         //X��Y����ת
#define GUI_TOUCH_MIRROR_X      0         //X������
#define GUI_TOUCH_MIRROR_Y      0         //Y������


#endif


#endif /* GUITOUCH_CONF_H */

