/*
*********************************************************************************************************
*                                                uC/GUI
*                        Universal graphic software for embedded applications
*
*                       (c) Copyright 2002, Micrium Inc., Weston, FL
*                       (c) Copyright 2002, SEGGER Microcontroller Systeme GmbH
*
*              礐/GUI is protected by international copyright laws. Knowledge of the
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

#define GUI_TOUCH_AD_LEFT       3180      	//触摸屏左侧AD测量值    
#define GUI_TOUCH_AD_RIGHT      985    	//触摸屏右侧AD测量值    
#define GUI_TOUCH_AD_TOP        2575     	//触摸屏顶部AD测量值   
#define GUI_TOUCH_AD_BOTTOM     1560    	//触摸屏底部AD测量值  

#define GUI_TOUCH_SWAP_XY       0         //X、Y方向翻转
#define GUI_TOUCH_MIRROR_X      0         //X方向镜像
#define GUI_TOUCH_MIRROR_Y      0         //Y方向镜像

#else 

#define A 3225
#define B 890
#define C 1425
#define D 2775

#define GUI_TOUCH_AD_LEFT       A      	//触摸屏左侧AD测量值    
#define GUI_TOUCH_AD_RIGHT      B    	//触摸屏右侧AD测量值    
#define GUI_TOUCH_AD_TOP        C     	//触摸屏顶部AD测量值   
#define GUI_TOUCH_AD_BOTTOM     D    	//触摸屏底部AD测量值

#define GUI_TOUCH_SWAP_XY       1         //X、Y方向翻转
#define GUI_TOUCH_MIRROR_X      0         //X方向镜像
#define GUI_TOUCH_MIRROR_Y      0         //Y方向镜像


#endif


#endif /* GUITOUCH_CONF_H */

