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


#define             INIT_TASK_STK_SIZE      2048

RAW_TASK_OBJ 		init_task_obj;

PORT_STACK          init_task_stack[INIT_TASK_STK_SIZE];

/*command buffer qingluo*/
char cmdbuf[1024];
int cmdlen;
/*test command register node*/
xCommandLineInputListItem test_register_item;
extern xCommandLineInput xTestDemoCommand;

/*watchdog command register node , defined in file cmd.c */
extern const xCommandLineInput xWatchdogCommand;
extern xCommandLineInputListItem xWatchdogRegisteredCommands;

extern xCommandLineInput netio;
extern xCommandLineInputListItem netio_item;

/* tftp command register node */
extern xCommandLineInput tftp;
extern xCommandLineInputListItem tftp_item;


/* tftp command register node */
extern xCommandLineInput tftp;
extern xCommandLineInputListItem tftp_item;


//added by laneyu
#define				SHELL_TASK_STK_SIZE		2048
RAW_TASK_OBJ 		shell_task_obj;

PORT_STACK          shell_task_stack[SHELL_TASK_STK_SIZE];

/*定义commandlineinputlistitem都在此定义*/
xCommandLineInputListItem iic_shell_listitem;
xCommandLineInputListItem spi_shell_listitem;

extern xCommandLineInputListItem xHsmmcRegisteredCommands;
extern xCommandLineInput xHsmmcCommand;

extern xCommandLineInputListItem xFatfsRegisteredCommands;
extern xCommandLineInput xFatfsCommand;

extern xCommandLineInputListItem xUcguiRegisteredCommands;
extern xCommandLineInput xUcguiCommand;

extern xCommandLineInputListItem pwmopen_item;
extern xCommandLineInput pwmopen;


extern xCommandLineInput pwmclose;
extern xCommandLineInputListItem pwmclose_item ;

extern xCommandLineInput lua_shell_cmd;
extern xCommandLineInputListItem lua_shell_item ;

/*定义commandlineinput都在此定义*/
static const xCommandLineInput iic_shell_cmd = 
{
	"iic_test",
	"iic_test  -- <iic addres> <dat>  if the arg1 is -a iic a loop\n",
	iic_shell, 
	2
};



static const xCommandLineInput spi_shell_cmd = 
{
	"spi",
	"spi -- < arg > i:init a:test all b:eraser bulk w:test write r:test read t:test time\n",
	spi_shell,
	1
};

extern RAW_S32 cmd_yaffs_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString);
xCommandLineInputListItem yaffs_shell_item ;

static const xCommandLineInput yaffs_shell_cmd = 
{
	"yaffs",
	"yaffs -- < no arg >\n",
	cmd_yaffs_test,
	0
};

extern RAW_S32 cmd_usbd_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString);

xCommandLineInputListItem usbd_shell_item ;

static const xCommandLineInput usbd_shell_cmd = 
{
	"usbd",
	"usbd -- < arg > i:init a:test all d:deinit\n",
	cmd_usbd_test,
	1
};

extern RAW_S32 cmd_wave_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString);

xCommandLineInputListItem wave_shell_item ;

static const xCommandLineInput wave_shell_cmd = 
{
	"wave",
	"wave -- < arg > i:init a:test all\n",
	cmd_wave_test,
	1
};

extern RAW_S32 cmd_ugfx_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString);

xCommandLineInputListItem ugfx_shell_item ;

static const xCommandLineInput ugfx_shell_cmd = 
{
	"ugfx",
	"ugfx -- < no arg >\n",
	cmd_ugfx_test,
	0
};


extern RAW_S32 cmd_reboot_test(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString);

xCommandLineInputListItem reboot_shell_item ;

static const xCommandLineInput reboot_shell_cmd = 
{
	"reboot",
	"reboot -- < no arg >\n",
	cmd_reboot_test,
	0
};



void read_cmd()
{
	int handle = 0;
	char tmp[100] = {0};
	char printbuf[100] = {0};
	int cnt = 0;
	int ret = 0;

	ret = Uart_0_ReceiveString((void *)tmp, 100, 2);
	
	while (cnt < ret) {
		//Uart_Printf("************read ret %d***\n", ret);
		//tell if "\r\n" and only check when '\r' found so won't be segment fault
		if ('\r' == *(tmp + cnt) || '\n' == *(tmp + cnt)) {
			if ('\r' == *(tmp + cnt) && '\n' == *(tmp + cnt +1)) {
				++cnt;
			}
			*(cmdbuf + cmdlen) = '\0';
			Uart_Printf("\n");
			if (*cmdbuf) {
				do {
					handle = (int)rsh_process_command((const RAW_S8 *)cmdbuf, (RAW_S8 *)printbuf, (size_t)100);
					Uart_Printf(printbuf);
					raw_memset(printbuf, 0, 100);
				} while (handle == 0);
			}
			Uart_Printf("\r\n");
			Uart_Printf("rawos # ");
			raw_memset((RAW_VOID *)cmdbuf, 0, 1024);
			cmdlen = 0;
		} else if ('\b' == *(tmp + cnt)) {
			if (cmdlen) {
				--cmdlen;
				*(cmdbuf + cmdlen) = '\0';
				Uart_Printf("\b \b");
			}
		} else {
			*(cmdbuf + cmdlen) = *(tmp + cnt);
			++cmdlen;
			Uart_Printf("%c", *(tmp + cnt));
		}
		++cnt;
	}
	
}

void shell_task(void *p_arg)
{	
	Uart_Printf("\r\n");
	Uart_Printf("rawos # ");
	while (1) {
		read_cmd();
	}

}


extern void print_cpuinfo(void);

extern RAW_SEMAPHORE eth_sem_lock;


#define HEAP_ADDRESS_START  	0x30200000
#define HEAP_ADDRESS_END  		0x30600000



void lwip_ping_test(void);
void tftp_client_task(void *arg);
void adc_ts_init(void);
extern void register_task_stack_command(void);


void init_task(void *arg)
{

	//GPB0 HIGH to avoid buzzer noise
	GPBCON_REG &= ~(1 << 1);
	GPBCON_REG |= (1 << 0);
	GPBDAT_REG |= (1 << 0);
	
	raw_page_init((RAW_VOID *)HEAP_ADDRESS_START, (RAW_VOID *)HEAP_ADDRESS_END);
	raw_malloc_init();

	raw_semaphore_create(&eth_sem_lock, "ethernet sem lock", 1);
	
	timer_0_init();	
	Uart_0_Init();
	
	print_cpuinfo();

	/**register shell command*/
	register_task_stack_command();
	rsh_register_command(&iic_shell_cmd, &iic_shell_listitem);
	rsh_register_command(&xTestDemoCommand, &test_register_item);
	rsh_register_command(&xWatchdogCommand, &xWatchdogRegisteredCommands);
	rsh_register_command(&tftp, &tftp_item);
	rsh_register_command(&netio, &netio_item);
	rsh_register_command(&spi_shell_cmd, &spi_shell_listitem);
	rsh_register_command(&xHsmmcCommand, &xHsmmcRegisteredCommands);
	rsh_register_command(&xFatfsCommand, &xFatfsRegisteredCommands);
	rsh_register_command(&pwmopen, &pwmopen_item);
	rsh_register_command(&pwmclose,&pwmclose_item);
	rsh_register_command(&yaffs_shell_cmd,&yaffs_shell_item);
	rsh_register_command(&usbd_shell_cmd,&usbd_shell_item);	
	rsh_register_command(&wave_shell_cmd,&wave_shell_item);	
	rsh_register_command(&ugfx_shell_cmd,&ugfx_shell_item);	
	rsh_register_command(&reboot_shell_cmd,&reboot_shell_item);	
	rsh_register_command(&lua_shell_cmd,&lua_shell_item);
	rsh_register_command(&xUcguiCommand, &xUcguiRegisteredCommands);
	
	raw_task_create(&shell_task_obj, (RAW_U8  *)"testtask", 0,
	                 IDLE_PRIORITY - 5, 0, shell_task_stack, 
	                 SHELL_TASK_STK_SIZE , shell_task, 1); 

	
	raw_task_delete(raw_task_identify());

}


void init_task_start()
{

	raw_task_create(&init_task_obj, (RAW_U8  *)"task1", 0,
	                         1, 0,  init_task_stack, 
	                         INIT_TASK_STK_SIZE , init_task, 1); 
}

