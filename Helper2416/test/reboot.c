#include <raw_api.h>
#include <yaffsfs.h>

#include <timer.h>
#include <uart.h>
#include <lib_string.h>
#include <rsh.h>
#include <cmd.h>

#include <mm/raw_malloc.h>
#include <mm/raw_page.h>
#include <iic.h>
#include <m25p20.h>
#include <pwm.h>
#include <nand.h>


extern void watchdog_reset(RAW_U16 timeout);


RAW_S32 cmd_reboot_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{


	watchdog_reset(1);

	return 1;
}


