/******************************************************
*		how to add a command api for user
*1.create a command process routine in cmd.h define it in cmd.c
*2.build a command structure in shell_task.c
*3.build a command list node for insert in shell_task.c
*4.register this structure to command list	in init_task
*						qingluo
******************************************************/

#ifndef _CMD_H
#define _CMD_H
#include <raw_api.h>
#include <rsh.h>

RAW_S32 rsh_test_command(RAW_S8 *pcWriteBuffer, size_t xWriteBufferLen, const RAW_S8 *pcCommandString );

RAW_S32 rsh_watchdog_command(RAW_S8 *pcWriteBuffer, size_t xWriteBufferLen, const RAW_S8 *pcCommandString );

RAW_S32 rsh_hsmmc_command(RAW_S8 *pcWriteBuffer, size_t xWriteBufferLen, const RAW_S8 *pcCommandString );

RAW_S32 rsh_fatfs_command(RAW_S8 *pcWriteBuffer, size_t xWriteBufferLen, const RAW_S8 *pcCommandString );
#endif //_CMD_H
