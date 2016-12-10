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

extern void usbd_init(void);
extern void usbd_deinit(void);
extern void hid_keypress(RAW_U8 buf[8]);

RAW_S32 cmd_usbd_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{
		RAW_S32 comleng;
    RAW_S8 *pcom=(RAW_S8 *)rsh_get_parameter( pcCommandString,1,&comleng ); 
		int time_cont = 0;
		if(*pcom == 'i')
    {	
			Uart_Printf("hid keyboard init\n");
			usbd_init();
			Uart_Printf("hid keyboard init end\n");
    }
		else if(*pcom == 'd')
    {
			Uart_Printf("hid keyboard deinit\n");
			usbd_deinit();
			Uart_Printf("hid keyboard deinit end\n");
		}
		else if(*pcom == 'a')
		{
			RAW_U8 key[8] = {00,00,00,0,00,00,00,00};
			Uart_Printf("hid keyboard test\n");
			key[2] = 0x1e;hid_keypress(key);
			key[2] = 0;hid_keypress(key);raw_sleep(1);
			key[2] = 0x1f;hid_keypress(key);
			key[2] = 0;hid_keypress(key);raw_sleep(1);
			key[2] = 0x20;hid_keypress(key);
			key[2] = 0;hid_keypress(key);raw_sleep(1);
			key[2] = 0x21;hid_keypress(key);
			key[2] = 0;hid_keypress(key);raw_sleep(1);
			key[2] = 0x22;hid_keypress(key);
			key[2] = 0;hid_keypress(key);raw_sleep(1);
			key[2] = 0x23;hid_keypress(key);
			key[2] = 0;hid_keypress(key);
		}
		else
		{
				Uart_Printf("help for usbd\r\n");
				Uart_Printf("usbd i,usbd d or usbd a\r\n");
		}
		return 1;
}


