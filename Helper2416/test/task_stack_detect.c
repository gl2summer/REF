#include <raw_api.h>
#include <lib_string.h>
#include <rsh.h>


static RAW_S32 rsh_task_task_command(RAW_S8 *pcWriteBuffer, size_t xWriteBufferLen, const RAW_S8 *pcCommandString)
{
	LIST *iter;
	LIST *iter_temp;
	RAW_U32 stack_free;
	RAW_TASK_OBJ *task_iter;
	
	iter = raw_task_debug.task_head.next;
	
	/*do it until list pointer is back to the original position*/ 
	while (iter != &(raw_task_debug.task_head)) {

		iter_temp  = iter->next;
		task_iter = raw_list_entry(iter, RAW_TASK_OBJ, task_debug_list);
		raw_task_stack_check(task_iter, &stack_free);
		Uart_Printf("task name is %s *** task free stack size is %d\n", task_iter->task_name, stack_free);
	
		/*move to list next*/
		iter = iter_temp;
	}
	
	return 1;
}


static xCommandLineInputListItem task_stack_item ;

static const xCommandLineInput task_stack_cmd = 
{
	"stack",
	"stack -- show all free task stack size\n",
	rsh_task_task_command,
	0
};


void register_task_stack_command(void)
{
	rsh_register_command(&task_stack_cmd, &task_stack_item);
}


