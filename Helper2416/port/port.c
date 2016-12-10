#include <raw_api.h>

RAW_VOID  *port_stack_init(PORT_STACK  *p_stk_base, RAW_U32 stk_size,  RAW_VOID   *p_arg, RAW_TASK_ENTRY p_task)
{
	PORT_STACK *stk;
	RAW_U32 temp = (RAW_U32)(p_stk_base + stk_size);

	/*stack need 8 bytes align*/
	temp &= 0xfffffff8;
	
	stk = (PORT_STACK  *)temp;
	                                       
	*(--stk)    = (RAW_U32)p_task;       /* Entry Point                                               */
	*(--stk)  = (RAW_U32)0;             /* LR                                        */
	*(--stk)  = (RAW_U32)0;             /* R12						*/
	*(--stk)  = (RAW_U32)0;             /* R11                                                */
	*(--stk)  = (RAW_U32)0;             /* R10                                                 */
	*(--stk)  = (RAW_U32)0;             /* R9                                                 */
	*(--stk)  = (RAW_U32)0;             /* R8                                                 */
	*(--stk)  = (RAW_U32)0;             /* R7 :                                       */
	*(--stk)  = (RAW_U32)0;             /* R6                                               */
	*(--stk)  = (RAW_U32)0;             /* R5                                               */
	*(--stk)  = (RAW_U32)0;             /* R4                                                 */
	*(--stk)  = (RAW_U32)0;             /* R3                                                */
	*(--stk)  = (RAW_U32)0;             /* R2                                                 */
	*(--stk)  = (RAW_U32)0;             /* R1                                                 */
	*(--stk)  = (RAW_U32)p_arg;         /* R0 argument                                                */
	*(--stk)  = (RAW_U32)0x13;     /* CPSR SVC mode                                                */
	
	 return stk;

	 
}





