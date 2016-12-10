#include <lib_string.h>
#include <cmd.h>
#include <rsh.h>
#include "watchdog.h"
#include "hs1_mmc.h"
#include <pwm.h>


xCommandLineInput xTestDemoCommand = {
	"test",
	"test: it's a demo command for user to add self define command into command list!\n",
	rsh_test_command,
	3
};


/*define a command list node for insert*/


RAW_S32 rsh_test_command(RAW_S8 *pcWriteBuffer, size_t xWriteBufferLen, const RAW_S8 *pcCommandString )
{
	RAW_S32 xReturn = 1;
	//get current command structure pointer
	RAW_S8 paramlen = 0;
	RAW_S8 *paramstart = 0;
	static int cnt = 0;
	RAW_S32 paramnum = xTestDemoCommand.cExpectedNumberOfParameters;
	while (cnt < paramnum) {
		paramstart = (RAW_S8 *)rsh_get_parameter((const RAW_S8 *)pcCommandString, (RAW_S32)(cnt+1), (RAW_S32 *)&paramlen);
		++cnt;
		if (paramstart != '\0' && paramlen != 0) {
			raw_memcpy((void *)pcWriteBuffer, (const void *)paramstart, (size_t)paramlen);
			
			// return 0 means it's not over yet, we have to do once more
			xReturn = 0;
			return xReturn;
		}
	}
	cnt = 0;
	return xReturn;
}

/*watchdog command node */

const xCommandLineInput xWatchdogCommand = {
	"wtd",
	"wtd(watchdog): <mode> <timeout> , first argument <mode> is to select reset(1), feed(2), close(3) or be a timer(4),second argument <timeout> used in the mode reset and timer function\r\n",
	rsh_watchdog_command,
	2
};

xCommandLineInputListItem xWatchdogRegisteredCommands = 
{
	&xWatchdogCommand,
	0
};


xCommandLineInput pwmopen = 
{
	"pwmopen",
	"pwm test j:   pwm_open --<nom>--\n",
	pwmopen_shell,
	1
};


xCommandLineInputListItem pwmopen_item = 
{
	&pwmopen,
	0
};


xCommandLineInput pwmclose = 
{
	"pwmclose",
	"pwm close test : pwmclose_shell --<nom>--\n",
	pwmclose_shell,
	1
};


xCommandLineInputListItem pwmclose_item = 
{
	&pwmclose,
	0
};


xCommandLineInputListItem * current_command_list_point = &xWatchdogRegisteredCommands;
const xCommandLineInputListItem *current_watchdog_command_point;



extern const xCommandLineInput xWatchdog_initCommand;

RAW_U16 str_to_int (void * input, RAW_S32 len)
{
	RAW_U16 out = 0;
	RAW_U8 * temp = input;
	RAW_S32 i = 0, num = len;
	while (*temp != '\0' && i < num)  {
		out = out * 10 + *(temp + i) - '0';
		i++;
	}
	
	return out;
}


RAW_S32 rsh_watchdog_command(RAW_S8 * pcWriteBuffer, size_t xWriteBufferLen, const RAW_S8 * pcCommandString)
{
	RAW_S32 xReturn = 1;

	RAW_S8 paramlen  = 0;
	RAW_S8 *paramstart = 0;
	
	RAW_U8 mode=0;
	RAW_U16 timeout = 0;
	
	static RAW_U8 cnt = 0;
	RAW_S32 paramnum = xWatchdogCommand.cExpectedNumberOfParameters;

	while (cnt < paramnum) {
		paramstart = (RAW_S8 *) rsh_get_parameter((const RAW_S8 *)pcCommandString, (RAW_S32)(cnt+1), (RAW_S32 *)&paramlen);
		++cnt;
		
		if ((paramstart != '\0') && paramlen > 0)
		{
			mode = str_to_int(paramstart,paramlen); //get the mode
			
/****************************************************************************
 *		mode : 1 -> call watchdog_init() function
 *		    	 2 -> call watchdog_feed() function
 *					 3 -> call watchdog_close() function
			timeout : only used in the watchdog_init() function 
 ****************************************************************************/
			
			switch (mode) {
				case 1:{
					paramstart = (RAW_S8 *) rsh_get_parameter((const RAW_S8 *)pcCommandString, (RAW_S32)(cnt+1), (RAW_S32 *)&paramlen);
					++cnt;
		
					if ((paramstart != '\0') && paramlen > 0)
						{
							timeout = str_to_int(paramstart,paramlen);
							timeout = (timeout != 0)? timeout : 16;
							watchdog_reset(timeout);
							Uart_Printf("the system will reset after %d seconds, if you never call watchdog_feed()\n", timeout);
							xReturn = 0;
						}
					break;
				}
				
				case 2:{
					paramstart = (RAW_S8 *) rsh_get_parameter((const RAW_S8 *)pcCommandString, (RAW_S32)(cnt+1), (RAW_S32 *)&paramlen);
					++cnt;
		
					if ((paramstart != '\0') && paramlen > 0)
						{
							timeout = str_to_int(paramstart,paramlen);
							watchdog_feed();
							xReturn = 0;
						}
					break;
				}
				
				case 3: {
					paramstart = (RAW_S8 *) rsh_get_parameter((const RAW_S8 *)pcCommandString, (RAW_S32)(cnt+1), (RAW_S32 *)&paramlen);
					++cnt;
		
					if ((paramstart != '\0') && paramlen > 0)
						{
							timeout = str_to_int(paramstart,paramlen);
							watchdog_close();
							Uart_Printf("close watchdog\n\n");
							xReturn = 0;
						}
					break;	
				}
				
				case 4: {
					paramstart = (RAW_S8 *) rsh_get_parameter((const RAW_S8 *)pcCommandString, (RAW_S32)(cnt+1), (RAW_S32 *)&paramlen);
					++cnt;
		
					if ((paramstart != '\0') && paramlen > 0)
						{
							timeout = str_to_int(paramstart,paramlen);
							timeout = (timeout != 0)? timeout : 10;
							Uart_Printf("set watchdog to be a timer(%d seconds)\n\n",timeout);
							watchdog_timer(timeout);							
							xReturn = 0;
						}
					break;	
				}

				default: {
					watchdog_reset(1);
					xReturn  = 0;
					break;
				}
			}
			return xReturn;		
		}
	}
	cnt = 0;
	return xReturn;
}

/*************************************** hsmmc ***************************************/

xCommandLineInput xHsmmcCommand = {
	"hsmmc",
	"hsmmc: <mode> <startblock> <blocknums> <data>  mode:1 write ,2 read  \n",
	rsh_hsmmc_command,
	4
};
xCommandLineInputListItem xHsmmcRegisteredCommands = 
{
	&xHsmmcCommand,
	0
};


static unsigned  int HEXstringToNum(const RAW_S8* str,  int length,unsigned int *num_return)
{  
 unsigned char  revstr[32]={0}; 
 unsigned int   num[32]={0};  
 unsigned int   count=1;  
 unsigned int   result=0; 
 int 	i;
 raw_memcpy(revstr,str,32);  
 for(i=length-1;i>=0;i--)  
 {  	
		if(revstr[i]<'0'|revstr[i]>'9')
		{
			return 0;
		}	
		num[i]=revstr[i]-48;		 	 
		result=result+num[i]*count;  
		count=count*10;
	}	    	
 *num_return = result;
 return 1;  
}


RAW_S32 rsh_hsmmc_command(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{
	RAW_S32 pxParameterStringLength;
	const RAW_S8 *pcom;
	RAW_U32 arg[4] = {1,0,1,0};
	RAW_U32 i = 0;
	for(i = 0; i<4; i++)
	{
		pcom = rsh_get_parameter(pcCommandString, i + 1, &pxParameterStringLength);
		if(HEXstringToNum(pcom, pxParameterStringLength,&arg[i] ) == 0)
		{
			return 1;
		}
	}	

	if(arg[0] == 1)//write
	{
		HS_MMC_SETGPIO_CH1();	
		HS_MMC_Reset_CH1();		
		if(!HS_MMC_init_CH1())
		{
			Uart_Printf("\nCard initial fail\n");
		}
		raw_sleep(20);
		HS_MMC_WriteBlocks_CH1(arg[1], arg[2], (RAW_U8*)&arg[3], 1);
	}
	else if(arg[0] == 2)//read
	{
		HS_MMC_SETGPIO_CH1();	
		HS_MMC_Reset_CH1();		
		if(!HS_MMC_init_CH1())
		{
			Uart_Printf("\nCard initial fail\n");
		}
		raw_sleep(20);
		HS_MMC_ReadBlocks_CH1(arg[1], arg[2],  (RAW_U8*)&arg[3], 1);
	}
	else 
		return 1;
	Uart_Printf("----------------sd test finish!--------------\n");
	return 1;
}


/*************************************** fatfs ***************************************/
extern void start_test_fatfs (void);

xCommandLineInput xFatfsCommand = {
	"fatfs",
	"fatfs: no argument. it will creat tow floder in your car\n",
	rsh_fatfs_command,
	0
};
xCommandLineInputListItem xFatfsRegisteredCommands = 
{
	&xFatfsCommand,
	0
};

RAW_S32 rsh_fatfs_command(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{
	start_test_fatfs();
	Uart_Printf("----------------fatfs test finish!--------------\n");
	return 1;
}

/*************************************** *********************************************/
/************************************ucgui********************************************/
extern void start_ucgui_test(void);
RAW_S32 rsh_ucgui_command(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString);

xCommandLineInput xUcguiCommand = {
	"ucgui",
	"ucgui: no argument. it will creat a uCGUI task in you system\n",
	rsh_ucgui_command,
	0
};
xCommandLineInputListItem xUcguiRegisteredCommands = 
{
	&xUcguiCommand,
	0
};

RAW_S32 rsh_ucgui_command(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{
	start_ucgui_test();	
	return 1;
}

/***************************************************************************************/

