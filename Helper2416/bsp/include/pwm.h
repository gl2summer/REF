#ifndef _PWM_H
#define _PWM_H

int pwm_open(RAW_U32 channel);
void pwm_close(int channel);
RAW_S32 pwmopen_shell(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString);
RAW_S32 pwmclose_shell(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString);
#endif  //_UART_H

