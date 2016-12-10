#include <raw_api.h>
#include <lib_string.h>
#include <2416_reg.h>
#include "ucgui_test.h"

extern void inerrupt_vector_init(void);
extern void exception_vector(void);
extern void MMU_EnableDCache(void);
extern void MMU_EnableICache(void);
extern void MMU_DisableMMU(void);
extern void read_cpsr(void);
extern void init_task_start(void);
extern void make_mmu_table (void);
extern void ucgui_test_init(void);


void Main()
{

	inerrupt_vector_init();
	raw_memcpy((void *)0xffff0000, (void *)(exception_vector), 128);
	raw_os_init();
	init_task_start();
	raw_os_start();

}




