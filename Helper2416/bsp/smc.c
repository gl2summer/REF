#include <2416_reg.h>
#include <raw_api.h>
#include <smc.h>
#include <irq.h>
#include <pbuf.h>

#include <mm/raw_page.h>
#include <mm/raw_malloc.h>
#include <raw_work_queue.h>

#include <fifo.h>
#include <lib_string.h>

RAW_U32 eth_ISR_cnt = 0;		
MEM_POOL eth_mem_pool;
RAW_U8 *eth_mem_ptr;
extern WORK_QUEUE_STRUCT eth_wq;
extern RAW_SEMAPHORE eth_sem_lock;

char MAC[6] = {0x12, 0x34, 0x56,0x78, 0x9A, 0xBC};		/* Ethernet physical address: 12-34-56-78-9A-BC */


static RAW_U32 smc9220_get_mac_csr(RAW_U8 reg);
extern void eth_handler(RAW_U32 arg, void *msg);
extern void eth_work_queue_task_init(void);

/* 16-bit bus read and write */
RAW_INLINE RAW_U32 reg_read(RAW_U32 addr)
{
	volatile RAW_U16 *addr_16 = (RAW_U16 *)addr;

	return ((*addr_16&0x0000FFFF)|(*(addr_16+1)<<16));
}

RAW_INLINE void reg_write(RAW_U32 addr, RAW_U32 val)
{
	volatile RAW_U16 *addr_16 = (RAW_U16 *)addr;

	*(addr_16 +1) = (RAW_U16)(val>>16);
	*addr_16 = (RAW_U16)val;
}

static void delay(RAW_U32 t)
{
	#if 0
	
	volatile RAW_U32 i, j;

	for (i=0; i<t; i++) {
		for (j=0; j<20000; j++) {
			;
		}
	}
	#endif
	
	raw_sleep(1);
	
}

void smc_bank1_init(void)
{
	EBICON_REG &= ~(1<<8);				/* memory type is SROM */
	
	SSMCCR_REG &= ~(1<<1);				/* SMCLK = HCLK */
	
	SMBCR1_REG = 0x303000;				/* default Bank1 control register */
	SMBCR1_REG |= 1;					/* 16 or 32 bit device */
	SMBCR1_REG |= (1<<4);				/* memory width with 16-bit */

	SMBWSTOENR1_REG = 5;				/* output enable assertion delay */
	SMBWSTWENR1_REG = 5;				/* write enable assertion delay */

	SMBIDCYR1_REG = 0xF;				/* idle or turnaround cycles */
	SMBWSTWRR1_REG = 20;	//0x1F;				/* write wait state */
	SMBWSTRDR1_REG = 20;	//0x1F;				/* read wait state */
}

static void wait_csr_busy(void)
{
	int i=100;
	while (reg_read(MAC_CSR_CMD)&MAC_CSR_CMD_CSR_BUSY) 
	{
		i--;
		if (i == 0) {
			Uart_Printf("wait_csr_busy timeout!\n");
			break;
		}
	}
}

static void wait_mii_busy(void)	
{
	int i=200;
	while (smc9220_get_mac_csr(MII_ACC)&MII_ACC_MII_BUSY) {
		i--;
		if (i == 0) {
			Uart_Printf("wait_mii_busy timeout!\n");
			break;
		}
	}
}

static RAW_U32 smc9220_get_mac_csr(RAW_U8 reg)
{
	wait_csr_busy();
	reg_write(MAC_CSR_CMD, MAC_CSR_CMD_CSR_BUSY | MAC_CSR_CMD_R_NOT_W | reg);
	wait_csr_busy();

	return reg_read(MAC_CSR_DATA);
}

static void smc9220_set_mac_csr(RAW_U8 reg, RAW_U32 data)
{
	wait_csr_busy();
	reg_write(MAC_CSR_DATA, data);
	reg_write(MAC_CSR_CMD,  MAC_CSR_CMD_CSR_BUSY | reg);
	wait_csr_busy();
}

static int smc9220_set_mac_address(void)
{
	RAW_U32 addrh = 0;
	RAW_U32 addrl = 0;

	addrl = MAC[0]|(MAC[1]<<8)|(MAC[2]<<16)|(MAC[3]<<24);
	addrh = MAC[4]|(MAC[5]<<8);
	smc9220_set_mac_csr(ADDRH, addrh);
	smc9220_set_mac_csr(ADDRL, addrl);

	return 0;
}

static int smc9220_miiphy_read(RAW_U8 phy, RAW_U8 reg, RAW_U16 *val)
{
	wait_mii_busy();

	smc9220_set_mac_csr(MII_ACC, phy<<11|reg<<6|MII_ACC_MII_BUSY);
	
	wait_mii_busy();

	*val = smc9220_get_mac_csr(MII_DATA);

	return 0;
}

static int smc9220_miiphy_write(RAW_U8 phy, RAW_U8 reg, RAW_U16 val)
{
	wait_mii_busy();

	smc9220_set_mac_csr(MII_DATA, val);
	smc9220_set_mac_csr(MII_ACC, phy<<11|reg<<6|MII_ACC_MII_WRITE|MII_ACC_MII_BUSY);

	wait_mii_busy();

	return 0;
}

static int smc9220_phy_reset(void)
{
	RAW_U32 reg;

	reg = reg_read(PMT_CTRL);
	reg |= PMT_CTRL_PHY_RST;
	reg_write(PMT_CTRL, reg);

	delay(100);
	return 0;
}

static void smc9220_phy_configure(void)
{
	int timeout;
	RAW_U16 status;


	smc9220_phy_reset();

	smc9220_miiphy_write(1, PHY_BMCR, PHY_BMCR_RESET);	/* reset */
	delay(10);
	smc9220_miiphy_write(1, PHY_ANAR, 0x01e1);			//set Auto-negotiation register,Tx with 100M full duplex and 10M full deplex ,IEEE802.3
	smc9220_miiphy_write(1, PHY_BMCR, PHY_BMCR_AUTON | PHY_BMCR_RST_NEG);	//restart auto-negation
	
	timeout = 5000;				/* time is longer enough */
	do {							/* after auto-negation "link is up" & "Auto-Negotiate complete "*/
		delay(100);
		if ((timeout--) == 0) {
			Uart_Printf(DRIVERNAME":autonegotiation timed out\n");
			break;
		}
		smc9220_miiphy_read(1, PHY_BMSR, &status);
	}while(!(status&PHY_BMSR_LS));     //link status

	Uart_Printf(DRIVERNAME ": phy initialized\n");

	return ;
}

static void smc9220_reset(void)
{
	int timeout;
	RAW_U32 reg;

	/* Take out of PM setting first */
	if (reg_read(PMT_CTRL)&PMT_CTRL_READY) {	/* LAN9220 ready to accessed */
		
		reg_write(BYTE_TEST, 0x0);			/* Write to the bytetest will take out of powerdown */
	}
	else {
		timeout = 10;
		while (timeout-- && !(reg_read(PMT_CTRL)&PMT_CTRL_READY)) {
			delay(10);
		}
		if (!timeout) {
			Uart_Printf(DRIVERNAME":timeout waiting for PM restore!\n");
			return ;
		}
	}

	/* Disable interrupts */
	reg_write(INT_EN, 0);
	
	reg_write(HW_CFG, HW_CFG_SRST);	/* Soft Reset, pay atation soft reset will clear some register */

	reg = *(RAW_U32 *)FIFO_INT;	
	reg |= 0x80;
	*(RAW_U32 *)FIFO_INT = reg;  //set FIFO_INT Rx status level
	reg_write(INT_CFG, (1<<8)|(5<<24));		/* enable &  Set interrupt deassertion to 50uS */
	
	timeout = 1000;					/* check EPC status */
	while (timeout-- && reg_read(E2P_CMD)&E2P_CMD_EPC_BUSY) {
		delay(10);
	}
	if (!timeout) {
		Uart_Printf(DRIVERNAME": EPC is busy!\n");
		return ;
	}

	reg = reg_read(INT_EN);
	reg |= 0x00000008;				// set Software Interrupt (SW_INT_EN)
	reg_write(INT_EN, reg);
	
	smc9220_set_mac_csr(FLOW, FLOW_FCPT|FLOW_FCEN);	/* Reset the FIFO level and flow control settings */

	reg_write(AFC_CFG, 0x0050287F);
	
	reg_write(GPIO_CFG, 0x70070000);						/* Set to LED outputs */
}


static void smc9220_enable(void)
{	/* Enable TX */

	reg_write(HW_CFG, 8<<16|HW_CFG_SF);		/* Tx&Rx Data FIFO both is 7680 bytes(8K - 512Byte), Tx&Rx Status FIFO both is 512 bytes*/
	
	reg_write(GPT_CFG, GPT_CFG_TIMER_EN|10000);	//general purpose timer enable and pre-load(10000)

	reg_write(TX_CFG, TX_CFG_TX_ON);			/* enable transmitter */

	/* no padding to start of packets */
	reg_write(RX_CFG, 0);

	smc9220_set_mac_csr(MAC_CR, MAC_CR_TXEN | MAC_CR_RXEN | MAC_CR_HBDIS);  //MAC_CR_HBDIS this bit is reserved
}

void eth_ISR_service(void)
{
	RAW_U32 reg;
	RAW_U32 intsts;
	RAW_U32 inten;
	void *block_addr;
	RAW_U16 block_ret;
	RAW_U8 *tmp2;

	RAW_S32 packetlen;
	
	SRCPND_REG |= (0x01<<5);				/* write 1 to clear */
	INTPND_REG |= (0x01<<5);				/* write 1 to clear */

	if (EINTPEND_REG & (1<<15)) {			/* EINT15 occur */
		//Uart_Printf("\n****************eth_ISR_service*****************\n");
		intsts = reg_read(INT_STS);			//Software Interrupt (SW_INT). This interrupt is generated when the SW_INT_EN bit is set high. Writing a one clears this interrupt.
		inten = reg_read(INT_EN);

		if (intsts & inten & 0x80000000) {
			//Uart_Printf("-----------------------------------1\n");
			reg = reg_read(INT_EN);
			reg &= ~0x80000000;				//clear the interrupt bit
			reg_write(INT_EN, reg);			//enble the interrupt
			reg_write(INT_STS, 0x80000000);
		}

		//RX Status FIFO Level Interrupt (RSFL)
		if (intsts & inten & 0x00000008) {  
			/* Disable Rx interrupts */
			reg = reg_read(INT_EN);
			reg &= ~0x00000008;
			reg_write(INT_EN, reg);
			
			if ((reg_read(RX_FIFO_INF)&RX_FIFO_INF_RXSUSED) >> 16) {		//RX Status FIFO Used Space (RXSUSED).
				block_ret = raw_block_allocate(&eth_mem_pool, &block_addr);
				if (block_ret == RAW_SUCCESS) {
					tmp2 = block_addr;
					packetlen = eth_receive(tmp2);
					if (packetlen) {
						sche_work_queue(&eth_wq, packetlen, block_addr, eth_handler);
					}
					else {
						raw_block_release(&eth_mem_pool, block_addr);
					}
				}
			}

			/* Enable Rx interrupts */
			reg_write(INT_STS, 0x00000008);
			reg = reg_read(INT_EN);
			reg |= 0x00000008;
			reg_write(INT_EN, reg);	
		}
		
		EINTPEND_REG |= (1<<15);

		reg = reg_read(INT_STS);		/* clear interrupt status */
	}
}

void eth_interrupt_init(void)
{
	GPGCON_REG &= ~(0x03<<14);
	GPGCON_REG |= (2<<14);			/* EINT[15] */

	GPGPU_REG &= ~(2<<14);			/* EINT15 up down disable */

	INTMOD_REG &= ~(1<<5);			/* EINT8_15 is IRQ mode */
	INTMSK_REG &= ~(1<<5);			/* EINT8_15 availabe */

	EINTCON1_REG |= 0x80000000;		/* EINT15 filter enable */
	EINTCON1_REG &= ~(0x7<<28);		/* falling edge  */ 	
	
	register_irq(5, eth_ISR_service);

	/* Ensure interrupts are globally disabled before connecting ISR */
	reg_write(INT_EN, 0);
	reg_write(INT_STS, 0xFFFFFFFF);

	EINTMASK_REG &= ~(1<<15);		/* enable external interrupt 15*/
}

void eth_halt(void)
{
	smc9220_reset();
}

RAW_S32 eth_init(void)
{
	RAW_U32 val;	
	
	Uart_Printf(DRIVERNAME ": initializing\n");
	eth_work_queue_task_init();

	eth_halt();						/* device LAN9220 reset by call func smc9220_reset()*/
	
	eth_interrupt_init();			/* configure LAN9220 interrupt service */

	smc_bank1_init();				/* output pin init */		
	
	val = reg_read(BYTE_TEST);		/* little endian or big endian */
	//Uart_Printf("val = 0x08lx\n", val);
	//raw_sleep(1000);
	if (val !=0x87654321) {
		Uart_Printf(DRIVERNAME ": Invalid chip endian 0x%08lx\n", val);
		return -1;
	}

	smc9220_reset();
	
	smc9220_phy_configure();		/* Configure the PHY, initialize the link state */
	
	smc9220_set_mac_address();		/* set MAC*/

	smc9220_enable();				/* Turn on Tx+Rx */

	raw_sleep(20);

	return 0;
}

RAW_S32 eth_send(volatile void *packet, int length)
{
	RAW_U32 *data = (RAW_U32 *)packet;
	RAW_U32 tmplen;
	RAW_U32 status;
	RAW_U32 reg_val;

	raw_semaphore_get(&eth_sem_lock, RAW_WAIT_FOREVER);

	reg_val = reg_read(INT_CFG);			/* disable final interrupt output to the IRQ pin*/
	reg_val &= ~(1<<8);
	reg_write(INT_CFG, reg_val);

	reg_write(TX_DATA_FIFO, TX_CMD_A_INT_FIRST_SEG|TX_CMD_A_INT_LAST_SEG|length);	//command A
	reg_write(TX_DATA_FIFO, length);	//command B, Disable Ethernet Frame Padding

	tmplen = (length+3)/4;

	while (tmplen--) {
		reg_write(TX_DATA_FIFO, *data++);
	}

	while (!((reg_read(TX_FIFO_INF)&TX_FIFO_INF_TSUSED) >> 16)) {		/* wait for transmission */
		;
	}

	status = reg_read(TX_STATUS_FIFO)&(TX_STS_LOC|TX_STS_LATE_COLL| \
		TX_STS_MANY_COLL|TX_STS_MANY_DEFER|TX_STS_UNDERRUN);

	reg_val = reg_read(INT_CFG);			/* enable final interrupt output to the IRQ pin*/
	reg_val |= 1<<8;
	reg_write(INT_CFG, reg_val);

	raw_semaphore_put(&eth_sem_lock);

	if (!status) 
		return 0;

	Uart_Printf(DRIVERNAME ": failed to send packet: %s%s%s%s%s\n",
		status & TX_STS_LOC ? "TX_STS_LOC " : "",
		status & TX_STS_LATE_COLL ? "TX_STS_LATE_COLL " : "",
		status & TX_STS_MANY_COLL ? "TX_STS_MANY_COLL " : "",
		status & TX_STS_MANY_DEFER ? "TX_STS_MANY_DEFER " : "",
		status & TX_STS_UNDERRUN ? "TX_STS_UNDERRUN" : "");

	return -1;
}

RAW_S32 eth_receive(void *dest) 
{
	RAW_U32 *pbuf = (RAW_U32 *)dest;
	RAW_U32 pktlen=0;
	RAW_U32 tmplen;
	RAW_U32 status;
	RAW_U32 reg_val;

	reg_val = reg_read(INT_CFG);			/* disable final interrupt output to the IRQ pin*/
	reg_val &= ~(1<<8);
	reg_write(INT_CFG, reg_val);

	if ((reg_read(RX_FIFO_INF)&RX_FIFO_INF_RXSUSED) >> 16 ) {		/* RX status FIFO Used space */
		status = reg_read(RX_STATUS_FIFO);
		pktlen = ((status & RX_STS_PKT_LEN)>>16);			/* get packet length */

		reg_write(RX_CFG, 0);		/* 4-byte align */

		tmplen = (pktlen+2+3)/4;

		while (tmplen--) {
			*pbuf++ = reg_read(RX_DATA_FIFO);
		}
		
		if (status & RX_STS_ES)	 {	/* error status */
			Uart_Printf(DRIVERNAME": dropped bad packet. Status: 0x%08x\n",status);
		}
	}

	reg_val = reg_read(INT_CFG);			/* enable final interrupt output to the IRQ pin*/
	reg_val |= 1<<8;
	reg_write(INT_CFG, reg_val);

	return pktlen;
}

void arp_test(void)
{
/*定义整个arp报文包，总长度42字节*/
char eth_frame[48] = {
0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 	//destination MAC/*destination ethernet addrress */
0xAA, 0xBB, 0xCC,0xDD, 0xEE, 0xEE,  	//source MAC	 /* source ethernet addresss */
0x08, 0x06,								//frame format   /*ethernet pachet type*/
0x00, 0x01 , 							//hardware format
0x08, 0x00,								//protocol format
0x06,									//hardware address length
0x04,									//protocol address length
0x00, 0x01 ,							//arp/RARP operation
0xAA, 0xBB, 0xCC,0xDD, 0xEE, 0xEE,  	//source MAC	 /*sender hardware address*/
0xC0, 0xA8, 1, 14 , 					//source ip 	 /*sender protocol address*/
0, 0, 0 ,0 ,0 ,0 ,						//dest mac fill	 /*target hardware address */
0xC0, 0xA8, 1, 0x64						//dest mac		 /*target protocol address */
};
	
	eth_halt();

	eth_init();
	eth_send(eth_frame, sizeof(eth_frame));
	
}
