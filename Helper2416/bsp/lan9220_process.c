#include <raw_api.h>
#include <mm/raw_page.h>
#include <mm/raw_malloc.h>
#include <raw_work_queue.h>

#include <fifo.h>
#include <lib_string.h>
#include <smc.h>


#define	ETH_TASK_STK_SIZE		4096
#define ETH_WORK_MSG_NUMBER     4

static PORT_STACK work_queue_stack[ETH_TASK_STK_SIZE];
WORK_QUEUE_STRUCT eth_wq;
static RAW_VOID *eth_queue_work[ETH_WORK_MSG_NUMBER];
static OBJECT_WORK_QUEUE_MSG eth_work_msg[100];

RAW_SEMAPHORE eth_sem_lock;

extern MEM_POOL eth_mem_pool;

extern void ethernetif_input(RAW_U8 *param, RAW_U8 *pkt_data, RAW_U16 len);

void eth_handler(RAW_U32 arg, void *msg)
{
	RAW_U8 param;
	RAW_U32 *tmp;

	raw_semaphore_get(&eth_sem_lock, RAW_WAIT_FOREVER);
	tmp = msg;
	
	ethernetif_input(&param, (RAW_U8 *)(tmp), arg);

	raw_block_release(&eth_mem_pool, tmp);
	raw_semaphore_put(&eth_sem_lock);
}


extern MEM_POOL eth_mem_pool;
extern RAW_U8 *eth_mem_ptr;
RAW_U32 eth_rx_buf[10240];

void eth_work_queue_task_init(void)
{

	raw_block_pool_create(&eth_mem_pool, "eth_mem_pool", 2048, eth_rx_buf, 2048*20);

	global_work_queue_init(eth_work_msg, 100);
	
	work_queue_create(&eth_wq, 5, ETH_TASK_STK_SIZE, work_queue_stack, (RAW_VOID **)eth_queue_work, ETH_WORK_MSG_NUMBER);
}
