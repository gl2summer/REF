#ifndef IIC_H_
#define IIC_H_
#include <iic.h>
#include <2416_reg.h>
#include <raw_type.h>

unsigned char IIC_open( void );		//	Hz order.
unsigned char IIC_close( void);
void eeprom_write( char Addr, char Data);
void eeprom_read( char Addr, char *Data);
void iic_shell_register( void );
RAW_S32 iic_shell(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString);

#endif
