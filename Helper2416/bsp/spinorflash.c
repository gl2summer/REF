
/**********************************************************************
* filename: m25p20.c
* author : string
* versions: V1.0
* fuction: m25p20`s derive
* complier：MDK4.4.3
* chip：S3C2416
* email:stringling@qq.com
* sysytem:RAW_OS
* please edit at 
* other: m25p20 is a serial nor flash 
**********************************************************************/
/********************************************************************** 	
2013-4  Created by string
xxxxxx   please add here
**********************************************************************/


#include <m25p20.h>
#include <2416_reg.h>
#include <time.h>
#include <rsh.h>

extern RAW_U32 get_PCLK(void);

static void read_page(RAW_U32 add,RAW_U8 length,RAW_U8 *st);
//static RAW_MUTEX inside_mutex;

RAW_U8 pBuffer[1000]={0};
RAW_U8 destdat[1000]={0};
RAW_U8  dat[] = "hello world";
RAW_U32 m25p20add = 0x0000;
int length = 255;

RAW_U16 spi_device_open(SPI_DEVICE *device)
{	
	RAW_U8 return_number;
#if 1
		Uart_Printf("spi open test\n");
#endif		
	if((*device).status == SPI_OPEN)
	{
		return SPI_MUTI_OPEN;
	}
	return_number=device_init((*device).freq);
	if(return_number)
		{
#if 1
		Uart_Printf("device_init error\n");
#endif		
		return 	return_number;
	}
	return_number = raw_mutex_create( (*device).mutex ,"SPI MUTEX",RAW_MUTEX_INHERIT_POLICY ,CONFIG_RAW_PRIO_MAX );
	if(return_number != RAW_SUCCESS)
		{
		
#if 0	
		Uart_Printf("mutex_creat error \n");	
#endif
		return return_number;
	}	
	(*device).chip_id =  read_ID ();	
	(*device).status  =  SPI_OPEN;
	
//#if DEBUG_SPI
#if 1
	Uart_Printf("spi open success\nthe spi chip`s id is %x\n",(*device).chip_id);
#endif	
	return RAW_SUCCESS;
}


RAW_U16 spi_device_close(SPI_DEVICE *device)
{
	int error_status;
	if((*device).status == SPI_CLOSE)
	{
		return SPI_MUTI_CLOSE;
	}	
	busy_state();
	error_status = raw_mutex_get((*device).mutex, 210);
	if(error_status != RAW_SUCCESS)
	{
		return error_status;
	}
	(*device).status = SPI_CLOSE;
#if 1
	Uart_Printf("spi close success\n");
#endif
	return raw_mutex_delete((*device).mutex);
}

RAW_U16 spi_device_read(SPI_DEVICE *device,RAW_U32 add,RAW_U8 length, RAW_U8  *data)
{	
	int error_status;
	
#if 1
	Uart_Printf("spi read start\n");
#endif
	
	if((*device).status!=SPI_OPEN)
	{
		return (*device).status;
	}
	
	error_status = raw_mutex_get((*device).mutex, 210); //wait option is according to the erase bulk cost ticks 
	if(error_status != RAW_SUCCESS)
		{
		return error_status;
	}	
//	(*device).chip_status = CHIP_WRITE;
	read_page(add,length,data);
//	(*device).chip_status = CHIP_FREE;
	return (raw_mutex_put((*device).mutex));	
}

RAW_U16 spi_device_write(SPI_DEVICE *device,RAW_U32 add,RAW_U8 length, RAW_U8  *data)
{
	int error_status;
	error_status = raw_mutex_get((*device).mutex, 210); //wait option is according to the erase bulk cost ticks 
	if(error_status != RAW_SUCCESS)
		{
		return error_status;
	}	
	write_page(add,length,data);
#if 1
	Uart_Printf("spi write end\n");
#endif
	return (raw_mutex_put((*device).mutex));	
}

RAW_U16 spi_device_erase_sector(SPI_DEVICE *device,RAW_U32 add )
{
	
	int error_status;
#if 1
	Uart_Printf("erase erase test\n");    
#endif
	error_status = raw_mutex_get((*device).mutex, 210); //wait option is according to the erase bulk cost ticks 
	if(error_status != RAW_SUCCESS)
		{
		return error_status;
	}	
	erase_sector( add );
#if 1
	Uart_Printf("erase erase success\n");    
#endif
	return (raw_mutex_put((*device).mutex));	
}

RAW_U16 spi_device_erase_bulk(SPI_DEVICE *device  )
{
	int error_status;
#if 1
	Uart_Printf("erase bulk test\n");    
#endif
	error_status = raw_mutex_get((*device).mutex, 210); //wait option is according to the erase bulk cost ticks 
	if(error_status != RAW_SUCCESS)
		{
		return error_status;
	}	
	erase_bulk();
#if 1
	Uart_Printf("erase erase success\n");    
#endif
	return (raw_mutex_put((*device).mutex));	
}

RAW_U32 spi_device_read_chipid(SPI_DEVICE *device)
{
	(*device).chip_id = read_ID();
	return ((*device).chip_id);	
}

RAW_U16 spi_device_read_status(SPI_DEVICE *device,RAW_U32 add,RAW_U8 length, RAW_U8  *data)
{
	return ((*device).status);	
}

static RAW_U16 device_init(RAW_U32 freq)
{    //spi init :freq the frequency of the spi bus
#if DEBUG_SPI
	#pragma void freq_check();//if error return FREQ_ERROR    --not define!!!!
			
#endif	

    RAW_U32 prescaler = ((get_PCLK()/freq)/2)-1;    
    GPECON_REG |= (2<<22) | (2<<24) | (2<<26);	//[GPE11 12 13]设置成SPIMISO、SPIMOSI、SPICLK功能引脚
	GPACON_REG &= ~(1<<3);			            //GPA3 设置为Output,即CS			
	CH_CFG_REG |= (0<<2) | (0<<3);		    	//High active and format A CPOL/CPHA=00
	CH_CFG_REG |= (0<<4);	    				//SPI Channel is Master			
	CLK_CFG_REG |= (0<<9) | (0<<10);			//PCLK 选择
//    CLK_CFG_REG = prescaler;					//2MHz频率
	CLK_CFG_REG |= 0;
	CLK_CFG_REG |= (1<<8);						//enable CLK			
	MODE_CFG_REG |= (0<<17) | (0 << 18);		//BUS onebyte
	SLAVE_SLECTION_REG &= (0); 					//开启写	
	CH_CFG_REG  |=(1) |(1<<1);					//开启TX/RX channel
	
#if DEBUG_SPI		
	Uart_Printf("pclk = %x\n",get_PCLK());
	Uart_Printf("prescaler = %x\n",prescaler);	
	Uart_Printf("初始化完成\n");
	Uart_Printf("SPI INIT SUCCESS ,the frequency is \n");
	Uart_Printf("SPI INIT SUCCESS\n");
#endif	
    return RAW_SUCCESS;
}

static RAW_U8 write_byte( RAW_U8 data)
{
     int mnt;  
     while(!(HS_SPI_STATUS>>(21))&1);     
     HS_SPI_TX_DATA = data;  
     while(~(HS_SPI_STATUS)&(1<<13));
     mnt = HS_SPI_RX_DATA;
     return mnt;
}

RAW_U32 read_ID (void)
{   //read_ID:return the ID of the m25p20
    RAW_U32 spiID; 
    {   
        spi_enable();
        write_byte(0x9f);
        spiID = write_byte(0);
        spiID = (spiID<<8)|write_byte(0);
        spiID = (spiID<<8)|write_byte(0);
        spiID = (spiID<<8)|write_byte(0);            
    }
    spi_disable();
//	Uart_Printf("the ID is 0x%x \n",spiID);
    return spiID;
}
RAW_U8 read_status (void)
{   //read_status:read the statu from the m25p20,return status
      char st;
      spi_enable();
      write_byte(RDSR);
      st=write_byte(0);    
      spi_disable(); 
      return st;
}
void write_enable()
{   //write_enable: set the m25p20 to write enable
    read_status();
    spi_enable();             //使能器件   
    write_byte(WREN);         //发送读取命令   
    spi_disable();   
}
/*
*	the busy_state() function is not well
*/
RAW_U16 busy_state()
{  
/*
 
	int tmp=0;
    RAW_U8 cnt=0;
	*/
	int i = 0;
//	int tmp=0;
	 while ((read_status()&0x01)==0x01)   // 等待BUSY位清空
	 {
		if(i<3)
		{
			raw_sleep(2);
			continue;
		}
//		Uart_Printf(">>");
		else
		{
			raw_sleep(0x6a);
			//continue;
		}
//		Uart_Printf(">>");
		i++;		
	 }
//	 Uart_Printf(">>");
//	 Uart_Printf("busy`s time is %x\n",i);
	 return 0;
/*
     do{	
			//raw_sleep(50);
			tmp = read_status();
			Uart_Printf("status is %x\n",tmp);
			//raw_sleep(50);			
			if(~(tmp&0x01))
			{	
				return 0;
			}
			if( cnt > 0xf )
			{
				return 0xff;
			}         
			Uart_Printf("<<>>>>");
			cnt++;
			Uart_Printf("count is %x\n",cnt);
			raw_sleep(50);
		}while(tmp&0x01);

*/	
/*
		do{
        tmp = read_status();
       
		Uart_Printf("status is %x\n",tmp);
		if(~(tmp&0x01)){
		Uart_Printf("status is %x\n",tmp);
		return 0x00
		;} 
		
        Uart_Printf("<<>>>>>>>>");
		cnt++;
        if(cnt > 250){return 0xff;}
		Uart_Printf("&&&&&&&&&&&&&&&&&&&\n");
		raw_sleep(2);
		
        //return 0xff;
        }while(tmp&0x01);		
			
*/		
//    Uart_Printf("<<\n");
//	  Uart_Printf("status is %x\n",tmp);
//      return 0;
}
RAW_U16 write_page(RAW_U32 add,RAW_U8 length, RAW_U8  *data)
{   //write_page:add-the address of you will write,length-the data length of you will write,data-pointer to the data of you want write
	//this function not consider the page over
	//    int cnt =0;
//	NORFLASHADD address;
//	address = (NORFLASHADD)(*(&add));
	
    int tmp = length;
	int busy;
//    int ready_status = 0;
    RAW_U8 *data_p = data;
	busy =	busy_state();
	
    if(busy)
    {
        Uart_Printf("the nor flash is busy,please try again\n");
		return SPI_TIME_OOUT ;
    }
    //Uart_Printf("\n");
	
    write_enable();
    spi_enable();         
    write_byte(PP);
	
    
//	write_byte((address.high));    
	
    write_byte((add>>16));    
    write_byte((add>>8));   
    write_byte((add>>0));    
    while((tmp--)>=0)
    {
        write_byte(*data_p);
//		Uart_Printf("write is %x\n",*data_p);		
        data_p++;
		//Uart_Printf("write is %x",data_p);
    }    
    spi_disable();   
	return RAW_SUCCESS;
}

void read_page(RAW_U32 add,RAW_U8 length,RAW_U8 *st)
{   //read_page:add-the address you want to read,length-the data length of you will read,point to a data you want to read to it
    int tmp = length;
    RAW_U8 *dat_p=st;
	if(busy_state())
    {
        Uart_Printf("the nor flash is busy,please try again\n");
		return;
    }
    spi_enable();
    write_byte(READ);
    
    write_byte((add>>16));    
    write_byte((add>>8));   
    write_byte((add>>0)); 
    
    while((tmp--)>=0)
    {
        *dat_p=write_byte(0);
         dat_p++;      
    }  
    spi_disable(); 
}

void write_disable()
{   //write_disable: set the m25p20 to write disable   
    spi_enable();             //使能器件   
    write_byte(WREN);         //发送读取命令   
    spi_disable();   
}

void write_status(char statu)
{   //write_status:write the statu to the m25p20 status`s register
    spi_enable();                           //使能器件   
    write_byte(WRSR);         //发送读取命令   
    write_byte(statu);
    spi_disable();     
}


RAW_U16 erase_sector(RAW_U32 add)
{   //erase_sector:add-the sector you want erase`s address ,return 255 if erase fault else return RAW_SUCCESS   
//    int tmp;
//    RAW_U8 cnt=0;
//	RAW_U8 ch;
    if(busy_state())
    {
        Uart_Printf("the nor flash is busy,please try again\n");
		return 0XFF;
    }
    write_enable();
    spi_enable();    
    write_byte(SE);    
    write_byte((add>>16));    
    write_byte((add>>8));   
    write_byte((add>>0));
	spi_disable();
  /*
    write_byte((0));    
    write_byte((0));   
    write_byte((0));
  */ 
	busy_state();  
/*   
    do{
        tmp = read_status();
        cnt++;
        raw_sleep(2);
        Uart_Printf("<<");
        if(cnt > 255)
        return 0xff;
    }while(tmp&0x01);
*/
	
/*
	ch = busy_state();
	Uart_Printf("the busy state is %2x\n",ch);
	if(busy_state())
    {
        Uart_Printf("the nor flash is busy,please try again\n");
		return 0xff;
    }
*/
   Uart_Printf("erase success\n");    
    return RAW_SUCCESS;
}

RAW_U16 erase_bulk()
{   //erase_bulk:return 255 if erase fault else return RAW_SUCCESS
//    int tmp;
//    RAW_U8 cnt=0;
//	  not consider the memory protect


	write_enable();
	
    spi_enable();
    write_byte(BE);        
    spi_disable(); 

/*	
    do{
        tmp = read_status();
		
        		
        if(cnt > 250)
        return 0xff;
		cnt++;
    }while(tmp&0x01);
    return RAW_SUCCESS;
*/
/*
	do{
        tmp = read_status();
//		Uart_Printf("tmp is %2x",tmp);
        cnt++;
        raw_sleep(2);
        Uart_Printf("<<");
        if(cnt > 255)
        return 0xff;
    }while(tmp&0x01);
	Uart_Printf("\nerase bulk success\n");	
*/
	busy_state();
	Uart_Printf("erase bulk success\n");
	return 0;
}

/*****************************      test  function       *************************
*								after the line for test onely
*
**********************************************************************************/


/***************** test data init and output *************/
void string_init(RAW_U8 *p)
{
	int cnt = 0;
	for(cnt=0;cnt<=255;cnt++)
	{
		*p=cnt;
		p++;
	}
//	Uart_Printf("%s",p);
}
void string_out(RAW_U8 *p)
{
	int cnt = 0;
	for(cnt=0;cnt<=255;cnt++)
	{
		if(cnt%0x10==0){Uart_Printf("\n");}
/*		
		if(cnt<=0x0f)
		{
			Uart_Printf(" 0x0%x",p[cnt]);
			continue;
		}
*/
		Uart_Printf(" 0x%2x",p[cnt]);	
	}
	Uart_Printf("\n");
}
void spi_test_times()
{//	Uart_Printf("test ---------------   page %d   ---------------\n",time_cont);
	
	char complet_percent=0;
	RAW_U32 time_cont = 0;	
	RAW_U32 time_tick_end;
	RAW_U32 time_tick_start;
	
	string_init( pBuffer );	
	time_tick_start = raw_tick_count;
	for(time_cont=0;time_cont<1024;time_cont++)
	{	
			
			read_page(time_cont<<8,255,(RAW_U8 *)destdat);		
			
			
//			string_out(destdat);			
//			write_page(time_cont<<8,255,pBuffer);
//			read_page(time_cont<<8,255,(RAW_U8 *)destdat);
//        	  Uart_Printf("%s\n",destdat);
//			string_out(destdat);
			//time_cont++;
	}
	
	time_tick_end = raw_tick_count;
	Uart_Printf("this write time is %d system tick\n",time_tick_end - time_tick_start);
	
	
	time_tick_start = raw_tick_count;
	Uart_Printf("please wait for amoment \n");
	Uart_Printf("completed     ");
	for(time_cont=0;time_cont<1024;time_cont++)
	{	
			
//			read_page(time_cont<<8,255,(RAW_U8 *)destdat);		
			
			
//			string_out(destdat);			
			write_page(time_cont<<8,255,pBuffer);
//			read_page(time_cont<<8,255,(RAW_U8 *)destdat);
//        	  Uart_Printf("%s\n",destdat);
//			string_out(destdat);
			//time_cont++;	
			if(time_cont %(10) == 0 )
			{
				complet_percent = (time_cont*100/1024);
				//Uart_Printf("\0x7f");
				Uart_Printf("\b \b\b \b\b \b\b \b%2d %%",complet_percent);
			} 
			else if(time_cont==1023)
			{
				Uart_Printf("\b \b\b \b\b \b\b \b%2d %%",100);
			}			
	}	
	Uart_Printf("\n");
	time_tick_end = raw_tick_count;
	Uart_Printf("this read time is %d system tick\n",time_tick_end - time_tick_start);
	
}

/*******************    test for shell   ********************/
RAW_S32 spi_shell(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{   
    RAW_S32 comleng;
    RAW_S8 *pcom=(RAW_S8 *)rsh_get_parameter( pcCommandString,1,&comleng ); 
	int time_cont = 0;
	
	string_init(pBuffer);
//	string_out(pBuffer);
	
    if(*pcom == 'i')
    {	
		Uart_Printf("spi init test\n");
        device_init( 2*1000*1000*10); 
		Uart_Printf("spi init end\n");
		
    }    
    if(*pcom =='e')
    {   
		Uart_Printf("spi erase sector test\n"); 
        erase_sector(m25p20add);
//		Uart_Printf("erase end.................\n");    
    }
    if(*pcom =='w')
    {	
		Uart_Printf("spi write test\n");
        write_page(m25p20add,255,pBuffer);
		//spi_test_times();
    }
	if(*pcom =='t')
    {
		Uart_Printf("spi time check test\n");
		spi_test_times();        
    } 
    if(*pcom =='r')
    {
		Uart_Printf("spi read test\n");
        read_page(m25p20add,255,(RAW_U8 *)destdat);
        string_out(destdat);       
    } 
	if(*pcom =='b')
    {	
		Uart_Printf("spi erase bulk test\n");
        erase_bulk();
//        Uart_Printf("%s\n",destdat);		
    }    	
    if(*pcom =='a')
    {   
//        read_page(m25p20add,length,(RAW_U8 *)destdat);
//        Uart_Printf("%s\n",destdat);
		Uart_Printf("spi test all\n");
        erase_sector(m25p20add); 
		erase_bulk();
		for(time_cont=0;time_cont<1024;time_cont++)
		{	
			Uart_Printf("test ---------------   page %d   ---------------\n",time_cont);
			
			read_page(time_cont<<8,255,(RAW_U8 *)destdat);
			string_out(destdat);
			
			write_page(time_cont<<8,255,pBuffer);
			read_page(time_cont<<8,255,(RAW_U8 *)destdat);
//        	  Uart_Printf("%s\n",destdat);
			string_out(destdat);
			//time_cont++;
		}
		spi_test_times();
		
    }  
//	string_out(destdat);
    return 1;
}


void spi_shell_register()
{
	
}

/*******************    test for task   ********************/
static RAW_TASK_OBJ 	spi_task_obj;
static PORT_STACK       spi_task_stack[1024];
static RAW_MUTEX 		spi_mutex;

void spi_task(void *p_arg)
{
	SPI_DEVICE norflash;
	norflash.mutex = &spi_mutex;
	
//	Uart_Printf("shell task entry**********************\n");
	while (1) {
			spi_device_open( & norflash );
//			Uart_Printf("SPI TASK IS RUN\n");
//			Uart_Printf("spi chip id is %x \n",norflash.chip_id);
			string_init( pBuffer );			
			/*erase bulk test*/
			spi_device_erase_bulk( &norflash );
			spi_device_read(&norflash,0,255,(RAW_U8 *)destdat);
			string_out(destdat);
			
			/*write and read test*/
			spi_device_write(&norflash,0,255,pBuffer);
			spi_device_read(&norflash,0,255,(RAW_U8 *)destdat);
			string_out(destdat);//printf the string destdat
			
			/*erase sector test*/
			spi_device_erase_sector( &norflash ,0);
			spi_device_read(&norflash,0,255,(RAW_U8 *)destdat);
			string_out(destdat);
			
			spi_device_write(&norflash,0,255,pBuffer);
			spi_device_read(&norflash,0,255,(RAW_U8 *)destdat);
			string_out(destdat);//printf the string destdat			
			
			spi_device_close( & norflash );
			
			raw_sleep(100);
	}
}

void spi_task_creat()
{	/*if you want add the spi to the task list ,please call this function*/
	
	raw_task_create(&spi_task_obj, (RAW_U8  *)"SPI TASK", 0,
	                         1, 0,  spi_task_stack, 
	                         1024 , spi_task, 1); 
}



