#include <raw_api.h>
#include <yaffsfs.h>

#include <timer.h>
#include <uart.h>
#include <lib_string.h>
#include <rsh.h>
#include <cmd.h>

#include <mm/raw_malloc.h>
#include <mm/raw_page.h>
#include <nand.h>

#include <lcd_driver.h>

extern void TouchPanel_HardInit_u(void);


extern void gfxInit(void);
extern void * ginputGetMouse(RAW_U16 instance);
extern int gui_task(void *param);

RAW_TASK_OBJ 	gui_task_obj;
PORT_STACK          gui_task_stack[1024];
int guiapp(void)
{
	gfxInit();
	ginputGetMouse(0);
	raw_task_create(&gui_task_obj, (RAW_U8  *)"gui", 0,
	                         4, 0,  gui_task_stack, 
	                         1024 , gui_task, 1); 
}


RAW_S32 cmd_ugfx_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{
		RAW_S32 comleng;
    RAW_S8 *pcom=(RAW_S8 *)rsh_get_parameter( pcCommandString,1,&comleng ); 
		
		TouchPanel_HardInit_u();
		lcd_init();
		Display_Start(2);
		guiapp();
		return 1;
}


