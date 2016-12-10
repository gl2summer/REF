#include <stdio.h>
#include <string.h>
#include "hs1_mmc.h"
#include "raw_type.h"
#include "uart.h"
#include <raw_api.h>

#define MAX_BLOCKS	65535

volatile int OCRcheck_ch1=0;
volatile int ThisIsMmc_ch1=0;
volatile int m_uRca_ch1=0;
volatile int m_ucMMCSpecVer_ch1 = 0;
volatile int SDSpecVer_ch1=0;
volatile int SectorMode_ch1 =0;
volatile unsigned int TotalCardBlock_number_ch1 =0;

extern RAW_U32 get_HCLK(void);

int HS_MMC_init_CH1(void)
{
	int uArg;

	ClockOnOff_CH1(0);
	rSCLKCON |=(1<<13); //FIFO SD Address pointer interrupt 2 Signal Enable

	SetClock_CH1(SD_HCLK_CH1, 0x40);
	
	rHM1_TIMEOUTCON = 0xe;

	HostCtrlSpeedMode_CH1(NORMAL_CH1);

	InterruptEnable_CH1(0xff, 0xff);

	IssueCommand_CH1(0,0x00,0);//Idle State
	IssueCommand_CH1(0,0x00,0);//Idle State
	IssueCommand_CH1(0,0x00,0);//Idle State

	uArg = (0x1<<8)|(0xaa<<0); //This Line for HC SD card.
	IssueCommand_CH1(8,uArg,0);

	OCRcheck_ch1 =1;

	if(!(SDSpecVer_ch1 == 2))
	{
		if(SetMMCOCR_CH1())
		{
			ThisIsMmc_ch1=1;
			Uart_Printf("MMC card is detected\n");
		}
		else if(SetSDOCR_CH1())
		{
			ThisIsMmc_ch1=0;
			Uart_Printf("SD card is detected\n");
		}
		else
			return 0;
	}
	else
	{
		if(SetSDOCR_CH1())
		{
			ThisIsMmc_ch1=0;
			Uart_Printf("SD card ver2.0 is detected\n");
		}		
	}

	OCRcheck_ch1 =0;
	
	// Check the attached card and place the card in the IDENT state rHM_RSPREG0
	IssueCommand_CH1(2,0,0);	

	// Send RCA(Relative Card Address). It places the card in the STBY state
	m_uRca_ch1 = (ThisIsMmc_ch1) ? 0x0001 : 0x0000;
 	IssueCommand_CH1(3,m_uRca_ch1,0);	
	
	if(!ThisIsMmc_ch1)
	{
		m_uRca_ch1 = (rHM1_RSPREG0>>16)&0xFFFF;			
		Uart_Printf("=>  RCA=0x%x\n", m_uRca_ch1);			
	}	
	
	//Uart_Printf("\nEnter to the Stand-by State\n");
	
	IssueCommand_CH1(9, m_uRca_ch1, 0);//Send CSD
	
	DisplayCardInformation_CH1();

	IssueCommand_CH1(7, m_uRca_ch1, 0);	
	//printf("\nEnter to the Transfer State\n");		
		
//////////////////////// Operating Clock setting ////////////////////////////
#if 0
	ClockConfig_CH1(SD_HCLK_CH1, 4);// Divisor 1 = Base clk /2	,Divisor 2 = Base clk /4, Divisor 4 = Base clk /8 ...		
#else
	rLOCKCON1=0x800; 
	rCLKSRC|=(1<<6);  // EPLL Output
	rEPLLCON=	((33<<16) | (1<<8) | 2);	
	rEPLLCON &= ~(1<<24);  // EPLL ON 
	rCLKDIV1 = (rCLKDIV1 & ~(0x3<<6)) | (0x0<<6);
	rMISCCR = rMISCCR & ~(0x7<<8) | (1<<8);
	rGPHCON = rGPHCON & ~(0x3<<28) | (1<<29);
	ClockConfig_CH1(SD_EPLL_CH1, 4);// Divisor 1 = Base clk /2 ,Divisor 2 = Base clk /4, Divisor 4 = Base clk /8 ...	
#endif
///////////////////////////////////////////////////////////////////////
	while (!IsCardInProgrammingState_CH1());

	if(ThisIsMmc_ch1)
	{
		if(!ReadExtCSD_CH1())
		Uart_Printf("\nFail to read Ext CSD");
	}

	while (!SetDataTransferWidth_CH1(4));	


	while (!IsCardInProgrammingState_CH1());

	while(!IssueCommand_CH1(16, 512, 0));//Set the One Block size	

	rHM1_NORINTSTS = 0xffff;

	return 1;
}

int ReadExtCSD_CH1(void)
{	
  	RAW_U32   uSfr;	
	RAW_U32  S_CMD_SET = 0;
//	RAW_U32 	uHsTiming = 0;
	int i;
	
	IssueCommand_CH1(16, 512, 0);	

	rHM1_BLKSIZE = (7<<12)|(512);	// Maximum DMA Buffer Size, Block Size	
	rHM1_BLKCNT = 1;	
	rHM1_TRNMOD = (0<<5)|(1<<4)|(0<<2)|(0<<1)|(0<<0);
	rHM1_CMDREG = (8<<8)|(1<<5)|(1<<4)|(1<<3)|(2<<0);
	
       WaitForBufferReadReady_CH1();	
	ClearBufferReadReadyStatus_CH1();	

	for(i=0; i<128; i++)			
	{		
	      uSfr = rHM1_BDATA;

		if (i==126)				
		 	S_CMD_SET = (uSfr & 0xff);
		if(i==53)
		{
			if(uSfr != 0)
			{
				TotalCardBlock_number_ch1 = uSfr;
				Uart_Printf("TotalCardBlock_number_ch1 = %d",TotalCardBlock_number_ch1);
			}
		}				
		//if (i==46)				
		// 	uHsTiming = ((uSfr>>8) & 0xff);				
	}

	Uart_Printf("\nS_CMD_SET=%x",S_CMD_SET);
	WaitForTransferComplete_CH1();	
	ClearTransferCompleteStatus_CH1();

	if(S_CMD_SET & (1<<4))
		Uart_Printf("\n========CE-ATA ========\n");
	else if(S_CMD_SET & (1<<3))
		Uart_Printf("\n========Secure MMC 2.0========\n");
	else if(S_CMD_SET & (1<<2))
		Uart_Printf("\n========Content Protection SecureMMC Card detect========\n");
	else if(S_CMD_SET & (1<<1))
		Uart_Printf("\n========Secure MMC Card detect========\n");
	else if(S_CMD_SET & (1<<0))
		Uart_Printf("\n========Standard MMC Card detect========\n");

	return 1;		
}

void HS_MMC_WriteBlocks_CH1(RAW_U32 StartAddr, RAW_U32 BlockNums, RAW_U8 * Buff, RAW_U8 testmode)
{
	RAW_U32 i, j;
	RAW_U32 blocknums_temp = 0;
	RAW_U32 temp = 0;
	if(SectorMode_ch1 == 1)
	{
		StartAddr = StartAddr;
		//Uart_Printf("\nSector Mode Addressing\n");
	}
	else
	{
		StartAddr = StartAddr * 512;
	}

	while(1)
	{
		if(BlockNums == 0)
			break;
		else if(BlockNums >= MAX_BLOCKS)
		{
			blocknums_temp = MAX_BLOCKS;
			BlockNums -= MAX_BLOCKS;
		}
		else if(BlockNums < MAX_BLOCKS)
		{
			blocknums_temp = BlockNums;
			BlockNums = 0;
		}
		if(testmode)
			Uart_Printf("write %ld blocks start %ld\n",blocknums_temp, StartAddr);
		SetBlockSizeReg_CH1(7, 512); // Maximum DMA Buffer Size, Block Size
		SetBlockCountReg_CH1(blocknums_temp ); // Block Numbers to Write
		SetArgumentReg_CH1(StartAddr); // Card Address to Write
		if(SectorMode_ch1 == 1)
		{
			StartAddr += MAX_BLOCKS;
		}
		else
		{
			StartAddr += 512*MAX_BLOCKS;
		}

		if(blocknums_temp == 1)//single block
		{
			SetTransferModeReg_CH1(0, 0, 1, 1, 0);
			SetCommandReg_CH1(24, 0); 	
		}
		else//multi block
		{
			SetTransferModeReg_CH1(1, 0, 1, 1, 0);
			SetCommandReg_CH1(25, 0); 
		}
		if (!WaitForCommandComplete_CH1())
		{
			Uart_Printf("\nCommand is NOT completed\n");
		}
		ClearCommandCompleteStatus_CH1();
		for(j=0; j<blocknums_temp; j++)
		{
			WaitForDataLineReady_CH1();
			WaitForBufferWriteReady_CH1();
			ClearBufferWriteReadyStatus_CH1();
			
			for(i=0; i<512/4; i++)//512 byte should be writed.
			{
				memcpy(&temp, Buff, sizeof(RAW_U32));
				rHM1_BDATA = temp;
				if(testmode)
					*((RAW_U32*)Buff) += 1;
				else
					Buff += 4;
			}
		}

		WaitForTransferComplete_CH1();

		ClearTransferCompleteStatus_CH1();
		while (!IsCardInProgrammingState_CH1());
	}

}

void HS_MMC_ReadBlocks_CH1(RAW_U32 StartAddr, RAW_U32 BlockNums, RAW_U8 * Buff, RAW_U8 testmode)
{
	RAW_U32 i, j;
	RAW_U32 blocknums_temp = 0;
	RAW_U32 temp = 0;

	if(SectorMode_ch1 == 1)
		StartAddr = StartAddr;
	else
		StartAddr = StartAddr * 512;

	while(1)
	{	
		if(BlockNums == 0)
			break;
		else if(BlockNums >= MAX_BLOCKS)
		{
			blocknums_temp = MAX_BLOCKS;
			BlockNums -= MAX_BLOCKS;
		}
		else if(BlockNums < MAX_BLOCKS)
		{
			blocknums_temp = BlockNums;
			BlockNums = 0;
		}
		while (!IsCardInProgrammingState_CH1());
		if(testmode)
			Uart_Printf("read %ld blocks start %ld\n",blocknums_temp, StartAddr);
		SetBlockSizeReg_CH1(7, 512); // Maximum DMA Buffer Size, Block Size

		SetBlockCountReg_CH1(blocknums_temp); // Block Numbers to Write

		SetArgumentReg_CH1(StartAddr); // Card Address to Write
		if(SectorMode_ch1 == 1)
		{
			StartAddr += MAX_BLOCKS;
		}
		else
		{
			StartAddr += 512*MAX_BLOCKS;
		}

		if(blocknums_temp == 1)//single block
		{
			SetTransferModeReg_CH1(0, 1, 0, 1, 0);
			SetCommandReg_CH1(17, 0); // CMD17: Single-Read						
		}
		else//multi block
		{
			SetTransferModeReg_CH1(1, 1, 1, 1, 0);
			SetCommandReg_CH1(18, 0); // CMD18: Multi-Read
		}
		while (!WaitForCommandComplete_CH1());
		ClearCommandCompleteStatus_CH1();
		for(j=0; j<blocknums_temp; j++)
		{
			WaitForDataLineReady_CH1();
			if (!WaitForBufferReadReady_CH1())
				Uart_Printf("ReadBuffer NOT Ready\n");
			else
				ClearBufferReadReadyStatus_CH1();
			for(i=0; i<512/4; i++)
			{
				temp = rHM1_BDATA;
				memcpy(Buff, &temp, sizeof(RAW_U32));
				if(testmode)
					;//Uart_Printf("%ld ",temp);
				else
					Buff += 4;
			}
		}
		if(testmode)
			Uart_Printf("last data is %ld\n",temp );
		WaitForTransferComplete_CH1();
		ClearTransferCompleteStatus_CH1();
	}
}



int SetDataTransferWidth_CH1(RAW_U32 ucBusWidth)
{
	RAW_U8 uBitMode=0;
	RAW_U32 uArg=0;
	RAW_U8 m_ucHostCtrlReg = 0;

	SetSdhcCardIntEnable_CH1(0); // Disable sd card interrupt

	if(!ThisIsMmc_ch1)// <------------------------- SD Card Case
	{
		if (!IssueCommand_CH1(55, m_uRca_ch1, 0))
			return 0;
		else
		{
			if (ucBusWidth==1)
			{
				uBitMode = 0;
				if (!IssueCommand_CH1(6, 0, 1)) // 1-bits
					return 0;
			}
			else
			{
				uBitMode = 1;
				if (!IssueCommand_CH1(6, 2, 1)) // 4-bits
					return 0;
			}
		}
	}
	else // <-------------------------------- MMC Card Case
	{
		if (m_ucMMCSpecVer_ch1==4) // It is for a newest MMC Card
		{
			if (ucBusWidth==1)
				uBitMode = 0;
			else if (ucBusWidth==4)
				uBitMode = 1;//4            		// 4-bit bus
			else
				uBitMode = 2;//8-bit bus
			
			uArg=((3<<24)|(183<<16)|(uBitMode<<8));
			while(!IssueCommand_CH1(6, uArg, 0));
		}
		else
			uBitMode = 0;
	}
	
	if (uBitMode==2)
	{
		m_ucHostCtrlReg &= 0xdf;
		m_ucHostCtrlReg |= 1<<5;
	}
	else
	{
		m_ucHostCtrlReg &= 0xfd;
		m_ucHostCtrlReg |= uBitMode<<1;
	}
	
	rHM1_HOSTCTL = m_ucHostCtrlReg;
	SetSdhcCardIntEnable_CH1(1);

	return 1;
}

void SetSdhcCardIntEnable_CH1(RAW_U8 ucTemp)
{
    	rHM1_NORINTSTSEN &= 0xFEFF;
	rHM1_NORINTSTSEN |= (ucTemp<<8);
}

int SetSDOCR_CH1(void)
{
	RAW_U32 i;
	
	for(i=0; i<250; i++)
	{
	#if 1
		IssueCommand_CH1(55, 0x0000, 0); // CMD55 (For ACMD)
		IssueCommand_CH1(41, 0x40ff8000, 1); // (Ocr:2.7V~3.6V)
	#else
		IssueCommand_CH1(55, 0x0, 0); // CMD55 (For ACMD)
		IssueCommand_CH1(41, 0x0, 1); // (Ocr:2.7V~3.6V)
		OCR = rHM_RSPREG0 | (1<<30);
		//printf("\nrHM_RSPREG0=%x",rHM_RSPREG0);

		//Delay(1000);

		IssueCommand_CH1(55, 0x0, 0); // CMD55 (For ACMD)
		IssueCommand_CH1(41, OCR, 1); // (Ocr:2.7V~3.6V)
	#endif
		//if (rHM1_RSPREG0&(0x1<<31))
		if (rHM1_RSPREG0&0x80000000)
		{
			if(rHM1_RSPREG0 & (1<<7))
				Uart_Printf("\nVoltage range : 1.65V ~ 1.95V");
			if(rHM1_RSPREG0 & (1<<21))
				Uart_Printf("\nVoltage range: 2.7V ~ 3.4V\n");	
			else if(rHM1_RSPREG0 & (1<<20))
				Uart_Printf("\nVoltage range: 2.7V ~ 3.3V\n");	
			else if(rHM1_RSPREG0 & (1<<21))
				Uart_Printf("\nVoltage range: 2.7V ~ 3.4V\n");
			else if(rHM1_RSPREG0 & (1<<23))
				Uart_Printf("\nVoltage range: 2.7V ~ 3.6V\n");

			if(rHM1_RSPREG0&(0x1<<30))
			{
				Uart_Printf("High Capacity Card\n");
				SectorMode_ch1 = 1;
			}
			
			ThisIsMmc_ch1 = 0;
			return 1;
		}
		//Delay(10);
		raw_sleep(50);
	}
	// The current card is MMC card, then there's time out error, need to be cleared.
	ClearErrInterruptStatus_CH1();
	return 0;
}

int SetMMCOCR_CH1(void)
{
	RAW_U32 i, OCR;
	for (i=0; i<250; i++)
	{
		IssueCommand_CH1(1, 0x0, 0);
		OCR = rHM1_RSPREG0 | (1<<30);	

		IssueCommand_CH1(1, OCR, 0); // (Ocr:2.7V~3.6V)

		if (rHM1_RSPREG0&0x80000000)
		{			
			if(rHM1_RSPREG0 & (1<<7))
				Uart_Printf("\nVoltage range : 1.65V ~ 1.95V");
			if(rHM1_RSPREG0 & (1<<21))
				Uart_Printf("\nVoltage range: 2.7V ~ 3.4V\n");	
			else if(rHM1_RSPREG0 & (1<<20))
				Uart_Printf("\nVoltage range: 2.7V ~ 3.3V\n");	
			else if(rHM1_RSPREG0 & (1<<19))
				Uart_Printf("\nVoltage range: 2.7V ~ 3.2V\n");	
			else if(rHM1_RSPREG0 & (1<<18))
				Uart_Printf("\nVoltage range: 2.7V ~ 3.1V\n");	

			if(rHM1_RSPREG0 & (1<<30))
			{
				Uart_Printf("\nHigh Capacity Sector Mode Operation\n");
				SectorMode_ch1 = 1;
			}
			ThisIsMmc_ch1=1;
			Uart_Printf("\nrHM_RSPREG0 = %x\n",rHM1_RSPREG0);
			return 1;
		}
	}
	ClearErrInterruptStatus_CH1();
	return 0;
}

void SetCommandReg_CH1(RAW_U16 uCmd,RAW_U32 uIsAcmd)
{
	RAW_U16 usSfr = 0;
	
	if (!uIsAcmd)//No ACMD
	{
		/* R2: 136-bits Resp.*/
		if ((uCmd==2||uCmd==9||uCmd==10))
			usSfr=(uCmd<<8)|(0<<4)|(1<<3)|(1<<0);

		/* R1,R6,R5: 48-bits Resp. */
		else if (uCmd==3||uCmd==8||uCmd==13||uCmd==16||uCmd==27||uCmd==30||uCmd==32||uCmd==33||uCmd==35||uCmd==36||uCmd==42||uCmd==55||uCmd==56)
			usSfr=(uCmd<<8)|(1<<4)|(1<<3)|(2<<0);

		else if (uCmd==11||uCmd==14||uCmd==17||uCmd==18||uCmd==19||uCmd==20||uCmd==24||uCmd==25)
			usSfr=(uCmd<<8)|(1<<5)|(1<<4)|(1<<3)|(2<<0);

		/* R1b,R5b: 48-bits Resp. */
		else if (uCmd==6||uCmd==7||uCmd==12||uCmd==28||uCmd==29||uCmd==38)
		{
			if (uCmd==12)
				usSfr=(uCmd<<8)|(3<<6)|(1<<4)|(1<<3)|(3<<0);
			else if (uCmd==6)
			{
				if(!ThisIsMmc_ch1)	// SD card
					usSfr=(uCmd<<8)|(1<<5)|(1<<4)|(1<<3)|(2<<0);
				else			// MMC card
					usSfr=(uCmd<<8)|(1<<4)|(1<<3)|(3<<0);
			}
			else
				usSfr=(uCmd<<8)|(1<<4)|(1<<3)|(3<<0);
		}

		/* R3,R4: 48-bits Resp.  */
		else if (uCmd==1)
			usSfr=(uCmd<<8)|(0<<4)|(0<<3)|(2<<0);

		/* No-Resp. */
		else
			usSfr=(uCmd<<8)|(0<<4)|(0<<3)|(0<<0);
	}
	else//ACMD
	{
		if (uCmd==6||uCmd==22||uCmd==23)		// R1
			usSfr=(uCmd<<8)|(1<<4)|(1<<3)|(2<<0);
		else if (uCmd==13||uCmd==51)
			usSfr=(uCmd<<8)|(1<<5)|(1<<4)|(1<<3)|(2<<0);
		else
			usSfr=(uCmd<<8)|(0<<4)|(0<<3)|(2<<0);
	}
	rHM1_CMDREG = usSfr;
}


void setmmcspeedmode_CH1(RAW_U32 eSDSpeedMode)
{
	RAW_U8  ucSpeedMode;
	RAW_U32 uArg=0;
	ucSpeedMode = (eSDSpeedMode == HIGH_CH1) ? 1 : 0;
	uArg=(3<<24)|(185<<16)|(ucSpeedMode<<8); // Change to the high-speed mode
	while(!IssueCommand_CH1(6, uArg, 0));	
}


void SetSDSpeedMode_CH1(RAW_U32 eSDSpeedMode)
{
	RAW_U32 volatile uSfr;
	RAW_U32 uArg = 0;
	RAW_U8  ucSpeedMode;
	int i;
	
	ucSpeedMode = (eSDSpeedMode == HIGH_CH1) ? 1 : 0;

	if (!IssueCommand_CH1(16, 64, 0)) // CMD16
		Uart_Printf("CMD16 fail\n");
	else
	{
		SetBlockSizeReg_CH1(7, 64);
		SetBlockCountReg_CH1(1);
		SetArgumentReg_CH1(0*64);

		SetTransferModeReg_CH1(0, 1, 0, 0, 0);

		uArg = 0x80000000|(0xffff<<8)|(ucSpeedMode<<0);

		if (!IssueCommand_CH1(6, uArg, 0))
			Uart_Printf("CMD6 fail\n");
		else
		{
			WaitForBufferReadReady_CH1();
			ClearBufferReadReadyStatus_CH1();
			
			for(i=0; i<16; i++)
			{
				uSfr = rHM1_BDATA	;			
			}			

			WaitForTransferComplete_CH1();
			ClearTransferCompleteStatus_CH1();
		}
	}	
}


void SetClock_CH1(RAW_U32 ClkSrc, RAW_U16 Divisor)
{

	rHM1_CONTROL2 = (rHM1_CONTROL2 & ~(0xffffffff)) | (0x1<<15)|(0x1<<14)|(0x1<<8)|(ClkSrc<<4);
	//rHM_CONTROL2 = (rHM_CONTROL2 & ~(0xffffffff)) |(0x1<<14)|(0x1<<8)|(ClkSrc<<4);
	//rHM1_CONTROL3 = (0<<31) | (1<<23) | (0<<15) | (1<<7);//SD OK
	rHM1_CONTROL3 = 0x80000000 | (1<<23) | (1<<15) | (1<<7);

	// SDCLK Value Setting + Internal Clock Enable	
	rHM1_CLKCON = (rHM1_CLKCON & ~((0xff<<8)|(0x1))) | (Divisor<<8)|(1<<0);

	// CheckInternalClockStable
	while (!(rHM1_CLKCON&0x2));
	ClockOnOff_CH1(1);
}

int IssueCommand_CH1( RAW_U16 uCmd, RAW_U32 uArg, RAW_U32 uIsAcmd)
{
	RAW_U32 uSfr;

	while ((rHM1_PRNSTS&0x1)); // Check CommandInhibit_CMD
	//while ((rHM1_PRNSTS&0x2)); // Check dataInhibit_CMD

	if (!uIsAcmd)//R1b type commands have to check CommandInhibit_DAT bit
	{
		if((uCmd==6&&ThisIsMmc_ch1)||uCmd==7||uCmd==12||uCmd==28||uCmd==29||uCmd==38||((uCmd==42||uCmd==56)&&(!ThisIsMmc_ch1)))
		{
			do	
			{
				uSfr = rHM1_PRNSTS;
			} while((uSfr&0x2)); // Check CommandInhibit_DAT
		}
	}
	// Argument Setting
	if (!uIsAcmd)// <------------------- Normal Command (CMD)
	{
		if(uCmd==3||uCmd==4||uCmd==7||uCmd==9||uCmd==10||uCmd==13||uCmd==15||uCmd==55)
			rHM1_ARGUMENT = uArg<<16;
		else
			rHM1_ARGUMENT = uArg;
	}
	else// <--------------------------- APP.Commnad (ACMD)
		rHM1_ARGUMENT = uArg;

	SetCommandReg_CH1(uCmd,uIsAcmd);

	if (!WaitForCommandComplete_CH1())
	{
		Uart_Printf("Command NOT Complete %ld\n",uCmd);
	}
	else
		ClearCommandCompleteStatus_CH1();	

	if (!(rHM1_NORINTSTS&0x8000))
	{
		if(((rHM1_CMDREG>>8) == 0x8) &&(ThisIsMmc_ch1 !=1))
		{
			Uart_Printf("\nCMD8 Support");
			SDSpecVer_ch1 = 2;
		}
		return 1;
	}
	else
	{
		if(OCRcheck_ch1 == 1)
			return 0;
		else
		{
			Uart_Printf("Command = %d, Error Stat = %x\n",(rHM1_CMDREG>>8),rHM1_ERRINTSTS);
			rHM1_ERRINTSTS = rHM1_ERRINTSTS;
			rHM1_NORINTSTS =rHM1_NORINTSTS;
			if((rHM1_CMDREG>>8) == 0x8)
			{
				Uart_Printf("\nThis Card version is NOT SD 2.0\n");
			}
			return 0;								
		}
	}

}
/*
void GetResponseData_CH1(RAW_U32 uCmd)
{
	RAW_U32 uSfr0,uSfr1,uSfr2,uSfr3;

	uSfr0 = rHM1_RSPREG0;
	uSfr1 = rHM1_RSPREG1;
	uSfr2 = rHM1_RSPREG2;
	uSfr3 = rHM1_RSPREG3;

	if(uCmd==3)
	{
 		if(!ThisIsMmc_ch1)
 		{
			m_uRca_ch1 = (uSfr0>>16)&0xFFFF;			
			Uart_Printf("=>  RCA=%d\n", m_uRca_ch1);
 		}
	}
	else if (uCmd==9)
	{
		if(ThisIsMmc_ch1)
		{
			m_ucMMCSpecVer_ch1=(rHM1_RSPREG3>>18)& 0xF;
			Uart_Printf("=>  m_ucMMCSpecVer_ch1=%d\n", m_ucMMCSpecVer_ch1);
		}
	}
}
*/
int GetSdSCR_CH1()
{
	RAW_U32 volatile  uSCR1, uSCR2;
	if (!IssueCommand_CH1(16, 8, 0))
		return 0;
	else
	{
		SetBlockSizeReg_CH1(7, 8);
		SetBlockCountReg_CH1(1);
		SetArgumentReg_CH1(0*8);

		SetTransferModeReg_CH1(0, 1, 0, 0, 0);
		if (!IssueCommand_CH1(55, m_uRca_ch1, 0))		// CMD55 (For ACMD)
			return 0;
		else
		{
			if (!IssueCommand_CH1(51, 0, 1))			// Acmd51
				return 0;
			else
			{
				WaitForBufferReadReady_CH1();
				ClearBufferReadReadyStatus_CH1();

				uSCR1 = rHM1_BDATA;
				uSCR2 = rHM1_BDATA;

				WaitForTransferComplete_CH1();
				ClearTransferCompleteStatus_CH1();

				if ((uSCR1&0xf) == 0x0)
					SDSpecVer_ch1 = 0; // Version 1.0 ~ 1.01
				else if ((uSCR1&0xf) == 0x1)
					SDSpecVer_ch1 = 1; // Version 1.10, support cmd6
				else if((uSCR1&0xf) == 0x2)
					SDSpecVer_ch1 = 2; // Version 2.0 support cmd6 and cmd8

				Uart_Printf("SDSpecVer_ch1=%d\n", SDSpecVer_ch1);
				return 1;
			}
		}
	}
}

int IsCardInProgrammingState_CH1(void)
{
	if (!IssueCommand_CH1(13, m_uRca_ch1, 0))
		{
		Uart_Printf("Card status = %x",((rHM1_RSPREG0>>9)&0xf));
		return 0;
		}
	else
	{
		if(((rHM1_RSPREG0>>9)&0xf) == 4)
			{
			//Uart_Printf("Card is transfer status\n");
			return 1;
			}
		return 0;
	}
}

void HostCtrlSpeedMode_CH1(RAW_U8 SpeedMode)
{
	RAW_U8  ucSpeedMode;
	ucSpeedMode = (SpeedMode == HIGH_CH1) ? 1 : 0;
	rHM1_HOSTCTL &= ~(0x1<<2);
	rHM1_HOSTCTL |= ucSpeedMode<<2;	
}

void ClockConfig_CH1(RAW_U32 Clksrc, RAW_U32 Divisior)
{
	RAW_U32 SrcFreq = 0, WorkingFreq;
	
	if (Clksrc == SD_HCLK_CH1)
		SrcFreq = get_HCLK();
	else if (Clksrc == SD_EPLL_CH1)//Epll Out 48MHz
		SrcFreq = 100000000;
	else
		Clksrc = get_HCLK();

	if (Divisior !=0)
	{
		WorkingFreq = SrcFreq/(Divisior*2);
		//Uart_Printf("HCLK = %d, SD WorkingFreq = %dMHz\n",get_HCLK(),WorkingFreq/(1000000));
	}
	else
	{
		WorkingFreq = SrcFreq;	
		//Uart_Printf("WorkingFreq = %dMHz\n",WorkingFreq/(1000000));
	}

	if (ThisIsMmc_ch1)
	{
		if (m_ucMMCSpecVer_ch1==4)
		{
			if (WorkingFreq>20000000)// It is necessary to enable the high speed mode in the card before changing the clock freq to a freq higher than 20MHz.
			{
				setmmcspeedmode_CH1(HIGH_CH1);					
				Uart_Printf("\nSet MMC High speed mode OK!!\n");
			}
			else
			{
				setmmcspeedmode_CH1(NORMAL_CH1);	
				Uart_Printf("\nSet MMC Normal speed mode OK!!\n");
			}
		}
		else // old version
			Uart_Printf("Old version MMC card can not support working frequency higher than 25MHz");
	}
	else
	{
		GetSdSCR_CH1();
		if (SDSpecVer_ch1==1||SDSpecVer_ch1==2 )
		{
			if (WorkingFreq>25000000)
			{
				SetSDSpeedMode_CH1(HIGH_CH1);//Higher than 25MHz, should use high speed mode. Max 50MHz and 25MB/sec
				Uart_Printf("\nSet SD High speed mode OK!!\n");
			}
			else
			{
				SetSDSpeedMode_CH1(NORMAL_CH1);
				Uart_Printf("\nSet SD Normal speed mode OK!!\n");
			}
		}
		else 
			Uart_Printf("Old version SD card can not support working frequency higher than 25MHz");
	}

	if (WorkingFreq>25000000)// Higher than 25MHz, it is necessary to enable high speed mode of the host controller.
	{
		HostCtrlSpeedMode_CH1(HIGH_CH1);
	}
	else
	{
		HostCtrlSpeedMode_CH1(NORMAL_CH1);
	}

	ClockOnOff_CH1(0); // when change the sd clock frequency, need to stop sd clock.
	SetClock_CH1(Clksrc, Divisior); 
}

void HS_MMC_Reset_CH1(void)
{
	rHM1_SWRST = 0x3;
}

void HS_MMC_SETGPIO_CH1(void)//
{
	//SD1
	rGPLCON = rGPLCON & ~(0xfffff) | (0xaaaaa);
	rGPJCON = rGPJCON & ~0xFC000000 | 0xA8000000;
	rGPJCON &= ~0xc0000000 | (1<<30);
	rGPJDAT |=(1<<15);

	//SD0
	rGPECON = (rGPECON& ~(0x3ffc00))|(0x1<<21)|(0x1<<19)|(0x1<<17)|(0x1<<15)|(0x1<<13)|(0x1<<11);
}

void SetTransferModeReg_CH1(RAW_U32 MultiBlk,RAW_U32 DataDirection, RAW_U32 AutoCmd12En,RAW_U32 BlockCntEn,RAW_U32 DmaEn)
{
	rHM1_TRNMOD = (rHM1_TRNMOD & ~(0xffff)) | (MultiBlk<<5)|(DataDirection<<4)|(AutoCmd12En<<2)|(BlockCntEn<<1)|(DmaEn<<0);
}

void SetArgumentReg_CH1(RAW_U32 uArg)
{
	rHM1_ARGUMENT = uArg;
}

void SetBlockCountReg_CH1(RAW_U16 uBlkCnt)
{
	rHM1_BLKCNT = uBlkCnt;
}

void SetBlockSizeReg_CH1(RAW_U16 uDmaBufBoundary, RAW_U16 uBlkSize)
{
	rHM1_BLKSIZE = (uDmaBufBoundary<<12)|(uBlkSize);
}

void ClockOnOff_CH1(int OnOff)
{
	if (OnOff == 0)
	{
		rHM1_CLKCON &=~(0x1<<2);
	}
		
	else
	{
		rHM1_CLKCON|=(0x1<<2);		
		while (1)
		{
			if (rHM1_CLKCON&(0x1<<3)) // SD_CLKSRC is Stable Ready
				break;
		}
	}
}

void ClearCommandCompleteStatus_CH1(void)
{
	rHM1_NORINTSTS=(1<<0);
	while (rHM1_NORINTSTS&0x1)
	{
		rHM1_NORINTSTS=(1<<0);
	}	
}

void ClearTransferCompleteStatus_CH1(void)
{

	rHM1_NORINTSTS = (1<<1);
	while (rHM1_NORINTSTS&0x2)
	{
		rHM1_NORINTSTS = (1<<1);
	}
}

void ClearBufferWriteReadyStatus_CH1(void)
{
	rHM1_NORINTSTS = (1<<4);
	while (rHM1_NORINTSTS & 0x10)
		rHM1_NORINTSTS = (1<<4);
}

void ClearBufferReadReadyStatus_CH1(void)
{
	rHM1_NORINTSTS = (1<<5);
	while (rHM1_NORINTSTS & 0x20)
		rHM1_NORINTSTS = (1<<5);
}


void ClearErrInterruptStatus_CH1(void)
{
	while (rHM1_NORINTSTS&(0x1<<15))
	{
		rHM1_NORINTSTS =rHM1_NORINTSTS;
		rHM1_ERRINTSTS =rHM1_ERRINTSTS;
	}
}

int WaitForBufferWriteReady_CH1(void)
{
	while (!(rHM1_NORINTSTS&0x10));
	return 1;
}

int WaitForBufferReadReady_CH1(void)
{
	RAW_U32 uLoop=0;

	while (!(rHM1_NORINTSTS&0x20))
	{
		if (uLoop%500000==0&&uLoop>0)
		{			
			return 0;
		}
		uLoop++;
	}
	return 1;
}

void WaitForDataLineReady_CH1(void)
{
	while (!(rHM1_PRNSTS&0x04));
}

int WaitForCommandComplete_CH1(void)
{
	RAW_U32 Loop=0;

	while (!(rHM1_NORINTSTS&0x1))
	{
		if (Loop%500000==0&&Loop>0)
		{			
			return 0;
		}
		Loop++;
	}
	return 1;
}

void WaitForTransferComplete_CH1(void)
{
	while (!(rHM1_NORINTSTS&0x2));
}

void InterruptEnable_CH1(RAW_U16 NormalIntEn, RAW_U16 ErrorIntEn)
{
	ClearErrInterruptStatus_CH1();	
	rHM1_NORINTSTSEN = NormalIntEn;
	rHM1_ERRINTSTSEN = ErrorIntEn;
}

void DisplayCardInformation_CH1(void)
{
	RAW_U32 CSD_STRUCTURE,C_SIZE,READ_BL_LEN, READ_BL_PARTIAL, CardSize, OneBlockSize ,C_SIZE_MULT = 0;
//	RAW_U32 i,j,k;
	
	if(ThisIsMmc_ch1)
	{
		m_ucMMCSpecVer_ch1=(rHM1_RSPREG3>>18)& 0xF;
		Uart_Printf("=>  m_ucMMCSpecVer_ch1=%d\n", m_ucMMCSpecVer_ch1);
	}
	//Uart_Printf("\n %lX %lX %lX %lX",rHM1_RSPREG3,rHM1_RSPREG2,rHM1_RSPREG1,rHM1_RSPREG0);

	CSD_STRUCTURE = (rHM1_RSPREG3>>22)&0x03;
	if(CSD_STRUCTURE == 0x01)
	{
		Uart_Printf("CSD_STRUCTURE is ver 2.0\n");
		READ_BL_LEN = ((rHM1_RSPREG2>>8) & 0xf) ;
		READ_BL_PARTIAL = ((rHM1_RSPREG2>>7) & 0x1) ;
		C_SIZE = ((rHM1_RSPREG1>>8)&0x3ffff);
		CardSize = (C_SIZE+1)*512/1024;
		OneBlockSize = (1<<READ_BL_LEN);
	}
	else
	{
		Uart_Printf("CSD_STRUCTURE is ver 1.0\n");
		READ_BL_LEN = ((rHM1_RSPREG2>>8) & 0xf) ;
		READ_BL_PARTIAL = ((rHM1_RSPREG2>>7) & 0x1) ;
		C_SIZE = ((rHM1_RSPREG2 & 0x3) << 10) | ((rHM1_RSPREG1 >> 22) & 0x3ff);
		C_SIZE_MULT = ((rHM1_RSPREG1>>7)&0x7);
		
		CardSize = (1<<READ_BL_LEN)*(C_SIZE+1)*(1<<(C_SIZE_MULT+2))/1048576;
		OneBlockSize = (1<<READ_BL_LEN);
	}

	Uart_Printf("\n READ_BL_LEN: %d",READ_BL_LEN);	
	Uart_Printf("\n READ_BL_PARTIAL: %d",READ_BL_PARTIAL);	
	Uart_Printf("\n C_SIZE: %d",C_SIZE);	
	Uart_Printf("\n C_SIZE_MULT: %d\n",C_SIZE_MULT);	

	Uart_Printf("\n One Block Size: %dByte",OneBlockSize);	
	Uart_Printf("\n Total Card Size: %dMByte\n\n\n",CardSize+1);	

	//i = (unsigned int)((rHM1_RSPREG2>>8) & 0xf);										// Max Read data block length
	//j = (unsigned int)(((rHM1_RSPREG2 & 0x3) << 10) | ((rHM1_RSPREG1 >> 22) & 0x3ff));	// Device Size
	//k = (unsigned int)((rHM1_RSPREG1>>7)&0x7);										// Device Size Multiflier
	// 1<<i   ->  1<<(i-9)

}

