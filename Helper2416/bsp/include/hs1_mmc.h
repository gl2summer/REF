#ifndef __HS1_MMC_H__
#define __HS1_MMC_H__

#ifdef __cplusplus
extern "C" {
#endif
	
#include "raw_type.h"
	
void HS_MMC_Reset_CH1(void);
void HS_MMC_WriteBlocks_CH1(RAW_U32 StartAddr, RAW_U32 BlockNums, RAW_U8 * Buff, RAW_U8 testmode);
void HS_MMC_ReadBlocks_CH1(RAW_U32 StartAddr, RAW_U32 BlockNums, RAW_U8 * Buff, RAW_U8 testmode);
void HS_MMC_Reset_CH1(void);
void HS_MMC_SETGPIO_CH1(void);
int HS_MMC_init_CH1(void);
void ClockOnOff_CH1(int OnOff);
void ClockConfig_CH1(RAW_U32 Clksrc, RAW_U32 Divisior);
int WaitForCommandComplete_CH1(void);
void WaitForTransferComplete_CH1(void);
int WaitForBufferWriteReady_CH1(void);
void ClearBufferWriteReadyStatus_CH1(void);
void ClearBufferReadReadyStatus_CH1(void);
void ClearCommandCompleteStatus_CH1(void);
void ClearTransferCompleteStatus_CH1(void);
void ClearErrInterruptStatus_CH1(void);
void SetTransferModeReg_CH1(RAW_U32 MultiBlk,RAW_U32 DataDirection, RAW_U32 AutoCmd12En,RAW_U32 BlockCntEn,RAW_U32 DmaEn);
void SetArgumentReg_CH1(RAW_U32 uArg);
void SetBlockCountReg_CH1(RAW_U16 uBlkCnt);
void SetBlockSizeReg_CH1(RAW_U16 uDmaBufBoundary, RAW_U16 uBlkSize);
void SetSDSpeedMode_CH1(RAW_U32 eSDSpeedMode);
void SetCommandReg_CH1(RAW_U16 uCmd,RAW_U32 uIsAcmd);
void SetClock_CH1(RAW_U32 ClkSrc, RAW_U16 Divisor);
void SetSdhcCardIntEnable_CH1(RAW_U8 ucTemp);
int SetDataTransferWidth_CH1(RAW_U32 ucBusWidth);
int SetSDOCR_CH1(void);
int IsCardInProgrammingState_CH1(void);
int IssueCommand_CH1( RAW_U16 uCmd, RAW_U32 uArg, RAW_U32 uIsAcmd);
void InterruptEnable_CH1(RAW_U16 NormalIntEn, RAW_U16 ErrorIntEn);
int WaitForBufferReadReady_CH1(void);
void WaitForDataLineReady_CH1(void);
int SetMMCOCR_CH1(void);
void HostCtrlSpeedMode_CH1(RAW_U8 SpeedMode);
void DisplayCardInformation_CH1(void);
int ReadExtCSD_CH1(void);

#define	SD_HCLK_CH1	1
#define	SD_EPLL_CH1		2
#define	SD_EXTCLK_CH1	3

#define	NORMAL_CH1	0
#define	HIGH_CH1	1

// chapter2 SYSEM CONTROLLER - jcs
#define rLOCKCON0    (*(volatile unsigned *)0x4C000000)  		//MPLL lock time conut
#define rLOCKCON1    (*(volatile unsigned *)0x4C000004)  		//EPLL lock time count
#define rOSCSET      (*(volatile unsigned *)0x4C000008)  		//OSC stabilization control
#define rMPLLCON     (*(volatile unsigned *)0x4C000010)  		//MPLL configuration
#define rEPLLCON     (*(volatile unsigned *)0x4C000018)  		//EPLL configuration
#define rEPLLCON_K     (*(volatile unsigned *)0x4C00001C)  		//EPLL configuration
#define rCLKSRC      (*(volatile unsigned *)0x4C000020)  		//Clock source control
#define rCLKDIV0     (*(volatile unsigned *)0x4C000024)  		//Clock divider ratio control
#define rCLKDIV1     (*(volatile unsigned *)0x4C000028)  		//Clock divider ratio control
#define rCLKDIV2     (*(volatile unsigned *)0x4C00002C)  		//Clock divider ratio control
#define rHCLKCON     (*(volatile unsigned *)0x4C000030)  		//HCLK enable
#define rPCLKCON     (*(volatile unsigned *)0x4C000034)  		//PCLK enable
#define rSCLKCON     (*(volatile unsigned *)0x4C000038)  		//Special clock enable
#define rPWRMODE     (*(volatile unsigned *)0x4C000040)  		//Power mode control
#define rSWRST      (*(volatile unsigned *)0x4C000044)  		//Software reset control
#define rBUSPRI0     (*(volatile unsigned *)0x4C000050)  		//Bus priority control
#define rPWRCFG      (*(volatile unsigned *)0x4C000060)  		//Power management configuration control
#define rRSTCON      (*(volatile unsigned *)0x4C000064)  		//Reset control 
#define rRSTSTAT     (*(volatile unsigned *)0x4C000068)  		//Reset status
#define rWKUPSTAT    (*(volatile unsigned *)0x4C00006c)  		//Wakeup status
#define rINFORM0     (*(volatile unsigned *)0x4C000070)  		//Sleep mode information 0
#define rINFORM1     (*(volatile unsigned *)0x4C000074)  		//Sleep mode information 1
#define rINFORM2     (*(volatile unsigned *)0x4C000078)  		//Sleep mode information 2
#define rINFORM3     (*(volatile unsigned *)0x4C00007C)  		//Sleep mode information 3
#define rUSB_PHYCTRL (*(volatile unsigned *)0x4C000080)  		//USB phy control
#define rUSB_PHYPWR  (*(volatile unsigned *)0x4C000084)  		//USB phy power control
#define rUSB_RSTCON  (*(volatile unsigned *)0x4C000088)  		//USB phy reset control
#define rUSB_CLKCON  (*(volatile unsigned *)0x4C00008C)  		//USB phy clock control

// chapter11 I/O PORT - oh
#define  rGPACON			(*(volatile unsigned *)(0x56000000))	
#define  rGPADAT			(*(volatile unsigned *)(0x56000004))	
#define  rGPBCON			(*(volatile unsigned *)(0x56000010))	
#define  rGPBDAT			(*(volatile unsigned *)(0x56000014))	
#define  rGPBUDP			(*(volatile unsigned *)(0x56000018))	
#define  rGPBSEL			(*(volatile unsigned *)(0x5600001c))	
#define  rGPCCON			(*(volatile unsigned *)(0x56000020))	
#define  rGPCDAT			(*(volatile unsigned *)(0x56000024))	
#define  rGPCUDP			(*(volatile unsigned *)(0x56000028))	
#define  rGPDCON			(*(volatile unsigned *)(0x56000030))	
#define  rGPDDAT			(*(volatile unsigned *)(0x56000034))	
#define  rGPDUDP			(*(volatile unsigned *)(0x56000038))	
#define  rGPECON			(*(volatile unsigned *)(0x56000040))	
#define  rGPEDAT			(*(volatile unsigned *)(0x56000044))	
#define  rGPEUDP			(*(volatile unsigned *)(0x56000048))	
#define  rGPESEL			(*(volatile unsigned *)(0x5600004c))	
#define  rGPFCON			(*(volatile unsigned *)(0x56000050))	
#define  rGPFDAT			(*(volatile unsigned *)(0x56000054))	
#define  rGPFUDP			(*(volatile unsigned *)(0x56000058))	
#define  rGPGCON			(*(volatile unsigned *)(0x56000060))	
#define  rGPGDAT			(*(volatile unsigned *)(0x56000064))	
#define  rGPGUDP			(*(volatile unsigned *)(0x56000068))	
#define  rGPHCON			(*(volatile unsigned *)(0x56000070))	
#define  rGPHDAT			(*(volatile unsigned *)(0x56000074))	
#define  rGPHUDP			(*(volatile unsigned *)(0x56000078))	
#define  rGPJCON			(*(volatile unsigned *)(0x560000d0))	
#define  rGPJDAT				(*(volatile unsigned *)(0x560000d4))	
#define  rGPJUDP			(*(volatile unsigned *)(0x560000d8))	
#define  rGPJSEL				(*(volatile unsigned *)(0x560000dc))	
#define  rGPKCON			(*(volatile unsigned *)(0x560000e0))	
#define  rGPKDAT			(*(volatile unsigned *)(0x560000e4))	
#define  rGPKUDP			(*(volatile unsigned *)(0x560000e8))	
#define  rGPLCON			(*(volatile unsigned *)(0x560000f0))	
#define  rGPLDAT			(*(volatile unsigned *)(0x560000f4))	
#define  rGPLUDP			(*(volatile unsigned *)(0x560000f8))	
#define  rGPLSEL			(*(volatile unsigned *)(0x560000fc))	
#define  rGPMCON			(*(volatile unsigned *)(0x56000100))	
#define  rGPMDAT			(*(volatile unsigned *)(0x56000104))	
#define  rGPMUDP			(*(volatile unsigned *)(0x56000108))	
#define  rMISCCR			(*(volatile unsigned *)(0x56000080))	
#define  rDCLKCON			(*(volatile unsigned *)(0x56000084))	
#define  rEXTINT0			(*(volatile unsigned *)(0x56000088))	
#define  rEXTINT1			(*(volatile unsigned *)(0x5600008c))	
#define  rEXTINT2			(*(volatile unsigned *)(0x56000090))	
#define  rEINTFLT2			(*(volatile unsigned *)(0x5600009c))	
#define  rEINTFLT3			(*(volatile unsigned *)(0x4c6000a0))	
#define  rEINTMASK			(*(volatile unsigned *)(0x560000a4))	
#define  rEINTPEND			(*(volatile unsigned *)(0x560000a8))	
#define  rGSTATUS0			(*(volatile unsigned *)(0x560000ac))	
#define  rGSTATUS1			(*(volatile unsigned *)(0x560000b0))	
#define  rDSC0				(*(volatile unsigned *)(0x560000c0))	
#define  rDSC1				(*(volatile unsigned *)(0x560000c4))	
#define	 rDSC2				(*(volatile unsigned *)(0x560000c8))
#define  rDSC3		    		(*(volatile unsigned *)(0x56000110))	
#define  rPDDMCON			(*(volatile unsigned *)(0x56000114))	
#define  rPDSMCON			(*(volatile unsigned *)(0x56000118))

// chapter28 HS_MMC Interface - gom
#define rHM1_SYSAD      	(*(volatile unsigned *)0x4AC00000)		//SDI control register
#define rHM1_BLKSIZE    	(*(volatile unsigned short*)0x4AC00004)	//Host buffer boundary and transfer block size register
#define rHM1_BLKCNT	    (*(volatile unsigned short*)0x4AC00006)		//Block count for current transfer
#define rHM1_ARGUMENT   	(*(volatile unsigned int*)0x4AC00008)		//Command Argument
#define rHM1_TRNMOD	    (*(volatile unsigned short*)0x4AC0000C)		//Transfer Mode setting register
#define rHM1_CMDREG	    (*(volatile unsigned short*)0x4AC0000E)		//Command register
#define rHM1_RSPREG0    	(*(volatile unsigned int*)0x4AC00010)		//Response 0
#define rHM1_RSPREG1    	(*(volatile unsigned int*)0x4AC00014)		//Response 1
#define rHM1_RSPREG2    	(*(volatile unsigned int*)0x4AC00018)	//Response 2
#define rHM1_RSPREG3    	(*(volatile unsigned int*)0x4AC0001C)	//Response 3
#define rHM1_BDATA      	(*(volatile unsigned int*)0x4AC00020)	//Buffer Data register
#define rHM1_PRNSTS     	(*(volatile unsigned int*)0x4AC00024)	//Present state
#define rHM1_HOSTCTL    	(*(volatile unsigned char*)0x4AC00028)	//Host control
#define rHM1_PWRCON     	(*(volatile unsigned char*)0x4AC00029)	//Power control
#define rHM1_BLKGAP     	(*(volatile unsigned char*)0x4AC0002A)	//Block Gap control
#define rHM1_WAKCON     	(*(volatile unsigned char*)0x4AC0002B)	//Wakeup control
#define rHM1_CLKCON     	(*(volatile unsigned short*)0x4AC0002C)	//Clock control
#define rHM1_TIMEOUTCON  (*(volatile unsigned char*)0x4AC0002E)	//Time out control
#define rHM1_SWRST      	(*(volatile unsigned char*)0x4AC0002F)	//Software reset
#define rHM1_NORINTSTS  	(*(volatile unsigned short*)0x4AC00030)	//Normal interrupt status
#define rHM1_ERRINTSTS  	(*(volatile unsigned short*)0x4AC00032)	//Error interrupt status
#define rHM1_NORINTSTSEN (*(volatile unsigned short*)0x4AC00034)	//Normal interrupt status enable
#define rHM1_ERRINTSTSEN (*(volatile unsigned short*)0x4AC00036)	//Error interrupt status enable
#define rHM1_NORINTSIGEN (*(volatile unsigned short*)0x4AC00038)	//Normal interrupt signal enable
#define rHM1_ERRINTSIGEN (*(volatile unsigned short*)0x4AC0003A)	//Error interrupt signal enable
#define rHM1_ACMD12ERRSTS (*(volatile unsigned short*)0x4AC0003C)	//Auto CMD12 Error Status
#define rHM1_CAPAREG  	(*(volatile unsigned int*)0x4AC00040)	//Capability
#define rHM1_MAXCURR  	(*(volatile unsigned int*)0x4AC00048)	//Maximum current Capability
#define rHM1_CONTROL2  	(*(volatile unsigned int*)0x4AC00080)	//Control 2
#define rHM1_CONTROL3  	(*(volatile unsigned int*)0x4AC00084)	//Control 3
#define rHM1_HCVER  		(*(volatile unsigned short*)0x4AC000FE)	//Host controller version
#define rHM1_DEBUG  		(*(volatile unsigned int*)0x4AC00088)
#define rHM1_CONTROL4  	(*(volatile unsigned int*)0x4AC0008C)
#define rHM1_FEAER  		(*(volatile unsigned short*)0x4AC00050)
#define rHM1_FEERR  		(*(volatile unsigned short*)0x4AC00052)
#define rHM1_ADMAERR  	(*(volatile unsigned int*)0x4AC00054)
#define rHM1_ADMSYSADDR  	(*(volatile unsigned int*)0x4AC00058)


#ifdef __cplusplus
}
#endif
#endif /*__HS1_MMC_H__*/
