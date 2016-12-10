#include <iic.h>
#include <2416_reg.h>
#include <raw_type.h>
#include <raw_api.h>
#include <time.h>
#include <rsh.h>
//#include <stringtoint.h>

#define IICCON_REG __REG(0x54000000)
#define IICADD_REG __REGb(0x54000008) 
#define IICSTAT_REG __REGb(0x54000004)
#define IICLC_REG __REGb(0x54000010)
#define IICDS_REG __REGb(0x5400000C)
/*
	*define the iic state
*/
#define	SlaveRX			(0)
#define	SlaveTX			(1)
#define	MasterRX		(2)
#define	MasterTX		(3)

/*
	*define the eeprom address
*/
//#define	EEPROMSlaveAddr		(0xa0+0)
#define	EEPROMSlaveAddr		(0xa0+1)
//#define	EEPROMSlaveAddr		(0xa0+2)
//#define	EEPROMSlaveAddr		(0xa0+3)
//#define	EEPROMSlaveAddr		(0xa0+4)
//#define	EEPROMSlaveAddr		(0xa0+5)
//#define	EEPROMSlaveAddr		(0xa0+6)
//#define	EEPROMSlaveAddr		(0xa0+7)

static volatile char *IIC_BUFFER;
static volatile unsigned int IIC_PT;
static unsigned int IIC_DATALEN;
static volatile unsigned char IIC_STAT;
static RAW_MUTEX IIC_MUTEX;
//static RAW_TASK_OBJ *IIC_TASK;

extern RAW_U32 get_PCLK(void);
//unsigned  int HEXstringToNum(const RAW_S8  *str,  int length,unsigned int *num_return);

static unsigned  int HEXstringToNum(const RAW_S8* str,  int length,unsigned int *num_return)
{  
 unsigned char  revstr[16]={0}; 
 unsigned int   num[16]={0};  
 unsigned int   count=1;  
 unsigned int   result=0; 
 int 	i;
 raw_memcpy(revstr,str,16);  
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

unsigned char IIC_init(unsigned int freq)
{
    RAW_U32  pclk;
	unsigned int	clk_prescaler;
	unsigned int clk_divider;
	
	GPECON_REG = GPECON_REG & 0x0fffffff | 0xa0000000; // gpio setting      
	GPEPU_REG =  GPEPU_REG 	& 0x0fffffff | 0xa0000000;		
	pclk = get_PCLK();  		
	if (((pclk>>4)/freq)>0xf) {
		clk_prescaler	=	1;
		clk_divider		=	(pclk>>9)/freq;		//	PCLK/512/freq
		} else {
		clk_prescaler	=	0;
		clk_divider		=	(pclk>>4)/freq;		//	PCLK/16/freq
	}
	
	//	Prescaler IICCLK=PCLK/16, Enable interrupt, Transmit clock value Tx clock=IICCLK/16
	IICCON_REG		=	(clk_prescaler<<6) | (1<<5) | (clk_divider&0xf);
	
	IICADD_REG		=	0x10;						  //	Slave address = [7:1]
	IICLC_REG		    =	0;		 					//	SDA Filter disable
	
	return raw_mutex_create(&IIC_MUTEX,"IIC MUTEX",RAW_MUTEX_INHERIT_POLICY,CONFIG_RAW_PRIO_MAX);
	
}

/***************************************************
	open the iic deveice			
	freq:the frequence of the iic
***************************************************/

unsigned char IIC_open( )		//	Hz order.
{	int tmp =0;  	
	tmp = raw_mutex_get(&IIC_MUTEX,1);
	{
		if(tmp!=RAW_SUCCESS)
		return tmp;
	}	
	IICSTAT_REG	=	0x10;               //	IIC bus data output enable(Rx/Tx)
	return RAW_SUCCESS;
}

unsigned char  IIC_close( void)
{
	IICSTAT_REG = 0x0;                    		//	IIC bus data output disable(Rx/Tx)
	return raw_mutex_put(&IIC_MUTEX);
}

static void IIC_Write_Pol( unsigned char SlaveAddr, char *Data, unsigned int n)
{
	
	while(IICSTAT_REG&(1<<5));					//	Wait until IIC bus is free.
	IIC_BUFFER	=	Data;
	IIC_PT		=	0;
	IIC_DATALEN	=	n;
	
	IICCON_REG 	|=  (1<<7); 					// acknowledge enable bit	
	IICDS_REG		=	SlaveAddr;
	IICSTAT_REG	=	0xF0;						//	Master Tx Start.
	
	while(IIC_DATALEN >= IIC_PT)
	{
		while(!(IICCON_REG&(1<<4))); 			// check the interrupt pending bit		
		IIC_STAT = IICSTAT_REG;
		
			
			if (IIC_PT<IIC_DATALEN)
            {
                IICDS_REG	=	IIC_BUFFER[IIC_PT];	
            }
			else	
            {
                IICSTAT_REG= 	0xd0;				//	ÊÍ·ÅStop Master Tx condition, ACK flag clear
            }
			IIC_PT++;
			IICCON_REG &= ~(1<<4);				//	Clear pending bit to resume
		
		IIC_STAT&=0xf;  // check status flag
	}	
}

static void IIC_Read_Pol( unsigned char SlaveAddr, char *Data, unsigned int n)
{
	while(IICSTAT_REG&(1<<5));					//	Wait until IIC bus is free. not in busy
	
    IIC_BUFFER	=	Data;
	IIC_PT		=	0;
	IIC_DATALEN	=	n;
	
	IICCON_REG 	|=  (1<<7);//ack enable
	IICDS_REG		=	SlaveAddr;
	IICSTAT_REG	=	0xB0;					//	Master Rx Start
	
	while(IIC_DATALEN >= IIC_PT)
	{
		
		while(!(IICCON_REG&(1<<4)));		
            IIC_STAT = IICSTAT_REG;		
            if (IIC_PT>0)
            {
                IIC_BUFFER[IIC_PT-1] = IICDS_REG;	
            }            
			IIC_PT++;			
			if (IIC_PT==IIC_DATALEN)
            {
                IICCON_REG 	&=  ~(1<<7);        //diable ack	
            }            
			else if (IIC_PT>IIC_DATALEN)
            {
                IICSTAT_REG	=	0x90;			//	Stop Master Rx condition	
            }            
			IICCON_REG &= ~(1<<4);			//	Clear pending bit to resume		
		IIC_STAT&=0xf;  // check status flag
	}
}



static void IIC_Wait( void)						//	Waiting for the command takes effect.
{											//	But not for IIS bus free.
	while(IIC_PT<=IIC_DATALEN);
}

static unsigned char IIC_Status( void)						//	Return IIC Status Register value at last interrupt occur.
{
	return	IIC_STAT;
}


//****************************************************************//
//*	Basic test code for Serial EEPROM with the basic functions.
//****************************************************************//
void eeprom_write( char Addr, char Data)
{
	char D[2];	
	D[0]=Addr;
	D[1]=Data;
	IIC_Write_Pol( EEPROMSlaveAddr, D, 2);	
	do	{												//	Polling for an ACK signal from SerialEEPROM.
		IIC_Write_Pol( EEPROMSlaveAddr, (char*)(0), 0);
		IIC_Wait();
	} while(IIC_Status()&0x1); // ACK was not received	
}

void eerom_write_string(char add,char *ch)
{
    char *ch_tmp = ch ;
    char add_tmp = add;     
    while((*ch_tmp) != '\0')
    {
        eeprom_write( add_tmp, *ch_tmp);
        ch_tmp++;   
        add_tmp++;
    }    
}


void eeprom_read( char Addr,  char *Data)
{
	IIC_Write_Pol( EEPROMSlaveAddr, &Addr, 1);
	//IIC_Write_Pol( EEPROMSlaveAddr, Data, 1);
	IIC_Read_Pol( EEPROMSlaveAddr, Data, 1);
	IIC_Wait();															//	Waiting for read complete.
}
void wm8731_write(unsigned int reg, unsigned int value)
{
	char data[2];	
	data[0] = reg;
	data[1] = value;
	//data[0] = (reg << 1) | ((value >> 8) & 0x0001);   
  //data[1] = value & 0x00ff; 
	IIC_Write_Pol( 0x34, data, 2);	
	do	
	{												
		IIC_Write_Pol( 0x34, (char*)(0), 0);
		IIC_Wait();
	} while(IIC_Status()&0x1); // ACK was not received	
}
void i2c_test( void *p)
{
	char i=0;
	char tmp = 0;
	IIC_init(200);
	while(1)
	{
		IIC_open(  );
		if(RAW_SUCCESS != IIC_open(  ))
		{
			Uart_Printf("iic error \n");
		}
		raw_sleep( 100 );
		i++;
		
		eeprom_write(i,i);
		eeprom_read(i,&tmp);
		
		if(tmp == i)
		{
			Uart_Printf("succuss %d\n",tmp);
		}
		else
		{
			Uart_Printf("error %d\n",tmp);
			raw_task_suspend( raw_task_active);			
		}
		IIC_close();
	}
}
void iic_test_for_shell_all(int i)
{   
    int mnt=0;
    char tmp;
    Uart_Printf("i2c test start......................... \n");
	IIC_init(2000);	
	IIC_open( );
    while(i--)
    {
	while(mnt <= 255)
        {
            eeprom_write(mnt,mnt+1);
            Uart_Printf("i2c write addres is %d,the number is %d \n",mnt,(char)(mnt+1));
            eeprom_read(mnt,&tmp);
            Uart_Printf("i2c read  addres is %d,the number is %d \n",mnt,tmp);
            mnt ++;
            raw_sleep(1);
        }
    }	
	Uart_Printf("loop test end...................\n");	
	IIC_close();

}
void iic_test_for_shell( int t,int i)
{
	char tmp = 0;
	if(t<0|t>(0xff))
	{	
		tmp = 1;
		Uart_Printf("the iic addres is an error value\n");
	}
	if(i<0|i>0xff)
	{
		tmp = 1;
		Uart_Printf("the dat is an error value\n");
	}
	if(tmp != 0 )
	{
		return;
	}
	
	Uart_Printf("i2c test start......................... \n");
	IIC_init(2000);	
	IIC_open( );

    eerom_write_string(10,"welcom to raw os test this string is read from eeprom\n");
    eeprom_write(t,i);
	Uart_Printf("i2c write addres is %d,the number is %d \n",t,i);
	eeprom_read(t,&tmp);
	Uart_Printf("i2c read  addres is %d,the number is %d \n",t,tmp);
	Uart_Printf("write test end....................... \n");
		
	//
	IIC_close();
}

RAW_S32 iic_shell(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString)
{
	RAW_S32 pxParameterStringLength;
	const RAW_S8 *pcom;
	RAW_U32 arg1 = 0;
	RAW_U32 arg2 = 0;
	//RAW_U32 ad_value = 0;
	
	pcom = rsh_get_parameter(pcCommandString, 1, &pxParameterStringLength);
	if(*pcom=='-'&&*(pcom+1)=='a')
	{
		iic_test_for_shell_all(1);
		return 1;
	}
	
	if(HEXstringToNum(pcom, pxParameterStringLength,&arg1 ) == 0)
	{
		return 1;
	}		
	pcom = rsh_get_parameter(pcCommandString, 2, &pxParameterStringLength);
    if(HEXstringToNum(pcom, pxParameterStringLength,&arg2)==0)    /* hex convert to decimal number*/		
	{
		return 1;
	}		
	iic_test_for_shell(arg1,arg2);
    Uart_Printf("end \n");    
	return 1;
}








