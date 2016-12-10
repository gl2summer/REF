#ifndef _m25p20_H
#define _m25p20_H
#include <raw_api.h>
#include <2416_reg.h>

/*
//SPI register define
//#define __REG(x)	(*(volatile RAW_U32 *)(x))
//#define __REGb(x)   (*(volatile RAW_U8 *)(x))
*/
#define CH_CFG_REG              (*(volatile RAW_U32 *)(0x52000000))     //HS_SPI_CH_CFG(Ch0)  0x52000000  R/W  HS_SPI  configuration register  
#define CLK_CFG_REG             (*(volatile RAW_U32 *)(0x52000004))     //Clk_CFG(Ch0)  0x52000004  R/W  Clock configuration register    
#define MODE_CFG_REG            (*(volatile RAW_U32 *)(0x52000008))     //MODE_CFG(Ch0)  0x52000008  R/W  HS_SPI  FIFO control register     
#define SLAVE_SLECTION_REG  (*(volatile RAW_U32 *)(0x5200000C))     //Slave_slection_reg(Ch0)  0x5200000C  R/W  Slave selection signal 
#define HS_SPI_INT_EN	    (*(volatile RAW_U32 *)(0x52000010))		//HS_SPI_INT_EN(Ch0)  0x52000010    R/W  HS_SPI Interrupt Enable register 
#define HS_SPI_STATUS       (*(volatile RAW_U32 *)(0x52000014))     //HS_SPI_STATUS(Ch0)  0x52000014    R    HS_SPI status register  0x0 
#define HS_SPI_TX_DATA      (*(volatile RAW_U32 *)(0x52000018))     //HS_SPI_TX_DATA(Ch0)  0x52000018  W  HS_ SPI TX DATA register   0x0 
#define HS_SPI_RX_DATA      (*(volatile RAW_U32 *)(0x5200001C))     //HS_SPI_RX_DATA(Ch0)  0x5200001C  R  HS_ SPI RX DATA register   0x0 
#define HS_SPI_PACKET_COUNT        (*(volatile RAW_U32 *)(0x52000020))     //Packet_Count_reg(Ch0)  0x52000020  R/W Count  how many data master gets  0x0 
#define PENDING_CLR         (*(volatile RAW_U32 *)(0x52000024))     //Pending_clr_reg(Ch0)  0x52000024  R/W P ending clear register  0x0 
#define SWAP_CFG            (*(volatile RAW_U32 *)(0x52000028))     //SWAP_CFG(Ch0)  0x52000028  R/W SWAP config register  0x0                     
#define FB_CLK_SEL          (*(volatile RAW_U32 *)(0x5200002C))     //FB_Clk_sel (Ch0)  0x5200002C  R/W Feedback  clock selecting register.  0x3 
/*
//gpio
*/
#define   SPICLK0   13//GPE13
#define   SPIMOSI0  12//GPE12 
#define   SPIMISO0  11//GPE11 

/*
//gpio
*/
#define   EEPROM_CS 3//GPA3

#define WREN 		0X06    // write enable
#define WRDI  		0X04    // write disable
#define RDID  		0X9F    //read identification
#define RDSR  		0X05    //read status register
#define WRSR  		0X01    //write status register
#define READ  		0X03    //read data bytes
#define FAST_READ   0X0B    //read data bytes at highter
#define PP   		0x02    //page program
#define SE   		0xD8    //sector erase
#define BE   		0xC7    //bulk erase
#define DP   		0xB9    //deep power-down
#define RES  		0xAB    //release from deep power-down ,and read electronic signature




#define   spi_enable() GPADAT_REG &= (~(1<<3)) //GPA3
#define   spi_disable() GPADAT_REG |= (1<<3) //GPA3

/****************   type of the file  ************/
typedef struct SPI_DEVICE
{
	RAW_MUTEX *mutex;
	RAW_U32 chip_id;
	RAW_U32 chip_status;
	RAW_U8  *spiname;
	RAW_U32 freq;//frequensy
	RAW_U8	status;
	
#if DEBUG_SPI
	RAW_U8 slave;//slave mode
	RAW_U32 size;//the nor flash`s size
#endif

}SPI_DEVICE;

typedef struct  {  	
					RAW_U8 low:8;
					RAW_U8 mid:8;
					RAW_U8 high:8; 
}NORFLASHADD;

enum 
{
	SPI_ERROR_INIT_ERROR = 1,
	FREQ_ERROR ,
	SPI_TIME_OOUT,
	SPI_MUTI_OPEN,
	SPI_MUTI_CLOSE,
};

enum 
{	
	SPI_OPEN =10,
	SPI_CLOSE,	
};
enum 
{	
	CHIP_READ =20 ,
	CHIP_WRITE ,
	CHIP_FREE  ,
	CHIP_ERASE_SECTOR,
	CHIP_ERASE_BULK
};


/***************  public fun   ******************/

RAW_U16 spi_device_open(SPI_DEVICE *device);
RAW_U16 spi_device_close(SPI_DEVICE *device);
RAW_U16 spi_device_read(SPI_DEVICE *device,RAW_U32 add,RAW_U8 length, RAW_U8  *data);
RAW_U16 spi_device_write(SPI_DEVICE *device,RAW_U32 add,RAW_U8 length, RAW_U8  *data);
RAW_U16 spi_device_erase_sector(SPI_DEVICE *device,RAW_U32 add );
RAW_U16 spi_device_erase_bulk( SPI_DEVICE *device );
RAW_U32 spi_device_read_chipid(SPI_DEVICE *device);
RAW_U16 spi_device_read_status(SPI_DEVICE *device,RAW_U32 add,RAW_U8 length, RAW_U8  *data);

/********   for test   ********/
void spi_shell_register(void );
void spi_task_creat( void );
RAW_S32 spi_shell(RAW_S8 *pcWriteBuffer, RAW_U32 xWriteBufferLen, const RAW_S8 * const pcCommandString);

/****************  private fun  *******************/
static RAW_U16 device_init(RAW_U32 freq);
static RAW_U32 read_ID ( void );
static RAW_U8 read_status (void);
//static void read_page(RAW_U32 add,RAW_U8 length,RAW_U8 *st);
static RAW_U16 write_page(RAW_U32 add,RAW_U8 length, RAW_U8  *data);
static RAW_U16 busy_state( void );
static RAW_U16 erase_sector(RAW_U32 add);
static RAW_U16 erase_bulk( void );



#endif
