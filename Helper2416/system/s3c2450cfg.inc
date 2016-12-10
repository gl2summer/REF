;/*
;--------------------------------------------
;
;名称:		S3C2450 初始化配置文件
;修改时间:	2009-05-02
;作者:		Liuwei
;Emall:		liuweiele@163.com
;QQ:		304466616
;说明:		本代码只供学习使用.
;
;--------------------------------------------
;*/

	
	GBLA	BSP_TYPE
	GBLA	BSP_SMDK2443
	GBLA	BSP_SMDK2450
	
BSP_SMDK2443	SETA	1
BSP_SMDK2450	SETA	2

BSP_TYPE		SETA	BSP_SMDK2450
	
;------------------------------------------------------------------------------
; General CPU constants

Mode_USR         EQU        (0x10)
Mode_FIQ         EQU        (0x11)
Mode_IRQ         EQU        (0x12)
Mode_SVC         EQU        (0x13)
Mode_ABT         EQU        (0x17)
Mode_UND         EQU        (0x1B)
Mode_SYS         EQU        (0x1F)
I_Bit            EQU        (0x80)
F_Bit            EQU        (0x40)

;------------------------------------------------------------------------------
;  Miscellaneous defines

WORD_SIZE       EQU         (4)
DW8             EQU         (0x0)
DW16            EQU         (0x1)
DW32            EQU         (0x2)
WAIT            EQU         (0x1<<2)
UBLB            EQU         (0x1<<3)

;------------------------------------------------------------------------------
; MMU constants

MMU_CTL_MASK     EQU        (0x3FFF0000)
MMU_TTB_MASK     EQU        (0x00003FFF)
MMU_ID_MASK      EQU        (0xFFFFFFF0)

;------------------------------------------------------------------------------

;------------------------------------------------------------------------------

;---------------------------------------
; nGCS0 = AMD Flash = Bank0 Controller Parameter setting
;---------------------------------------
	GBLL	AMDBOOT			; bacnk0, 16-bit and flash
;	GBLL	NANDBOOT		; bacnk0, 8-bit nand
					
	IF :DEF: AMDBOOT

IDCY0				EQU		0x0			;	Idle or turnaround cycles IDCY*HCLK
WSTRD0				EQU		0x4			;	Read wait state = tacc
WSTWR0				EQU		0x4			;	wrie wait state
WSTOEN0				EQU		0			; 	output enable assertion delay from CS
WSTWEN0				EQU		0			; 	write enable assertion delay

BlWriteEn			EQU		1			;bit21-SMBAA signal control:0-1at all times, 1 active for sync
AddrValidWriteEn	EQU		1			;bit20-SMADDRVALD during write:0-always high,1-active for write 	
BurstLenWrite		EQU		0			;bit1819-burst transfer length:0-4,1-8,3-continu(sync only) 	
SyncWriteDev		EQU		0			;bit17-0:async, 1:sync 	
BMWrite				EQU		0			;bit16-burt mode write : 0-non-burst, 1-burst 	
WrapRead			EQU		0			;bit14-0-disable, 1 enable
BlReadEn			EQU		1			;bit13-SMBAA signal :0-1 at all time, 1-active for sync read
AddrValidReadEn		EQU		1			;bit12-SMADDRVALID signal: 0-always HIGH, 1-active for async & sync read
BurstLenRead		EQU		0			;bit1011-burst transfer length:0-4,1-8,2-16,3-cont(sync only)
SyncReadDev			EQU		0			;bit9-sync access :0-async, 1-sync
BMRead				EQU		0			;bit8-burst mode red and async page mode
SMBLSPOL			EQU		0			;bit6-polarit of signal nSMBLS
MW					EQU		1			;bit45-memory width : 00-8bit,01-16bit,10-32bit
WP					EQU		0			;bit3-write protect
WaitEn				EQU		0			;bit2-external wait signal enable
WaitPol				EQU		0			;bit1-polarity of the external wait input for actiation
RBLE				EQU		0			;bit0-read byte lane enable
SMBCR0_0			EQU		((BMRead<<8)+(SMBLSPOL<<6)+(MW<<4)+(WP<<3)+(WaitEn<<2)+(WaitPol<<1)+RBLE)
SMBCR0_1			EQU		((WrapRead<<14)+(BlReadEn<<13)+(AddrValidReadEn<<12)+(BurstLenRead<<10)+(SyncReadDev<<9))
SMBCR0_2			EQU		((BlWriteEn<<21)+(AddrValidWriteEn<<20)+(BurstLenWrite<<18)+(SyncWriteDev<<17)+(BMWrite<<16))

WaitTourErr0			EQU		0			; external wait timeout error flag

WSTBRD0				EQU		0x1f			; burst read wait state

MemClkRatio			EQU		1			; SMMEMCLK :0-HCLK,1-HCLK/2,2-HCLK/3
SMClockEn			EQU		1			; SMCLK enable 0-only active during mem access,1-always running	

	ELSE	; NAND Boot....
IDCY0				EQU		0x0			;	Idle or turnaround cycles IDCY*HCLK
WSTRD0				EQU		0xe			;	Read wait state = tacc
WSTWR0				EQU		0xe			;	wrie wait state
WSTOEN0				EQU		0			; 	output enable assertion delay from CS
WSTWEN0				EQU		0			; 	write enable assertion delay

BlWriteEn			EQU		1			;bit21-SMBAA signal control:0-1at all times, 1 active for sync
AddrValidWriteEn	EQU		1			;bit20-SMADDRVALD during write:0-always high,1-active for write 	
BurstLenWrite		EQU		0			;bit1819-burst transfer length:0-4,1-8,3-continu(sync only) 	
SyncWriteDev		EQU		0			;bit17-0:async, 1:sync 	
BMWrite				EQU		0			;bit16-burt mode write : 0-non-burst, 1-burst 	
WrapRead			EQU		0			;bit14-0-disable, 1 enable
BlReadEn			EQU		1			;bit13-SMBAA signal :0-1 at all time, 1-active for sync read
AddrValidReadEn		EQU		1			;bit12-SMADDRVALID signal: 0-always HIGH, 1-active for async & sync read
BurstLenRead		EQU		0			;bit1011-burst transfer length:0-4,1-8,2-16,3-cont(sync only)
SyncReadDev			EQU		0			;bit9-sync access :0-async, 1-sync
BMRead				EQU		0			;bit8-burst mode red and async page mode
SMBLSPOL			EQU		0			;bit6-polarit of signal nSMBLS
MW					EQU		0			;bit45-memory width : 00-8bit,01-16bit,10-32bit
WP					EQU		0			;bit3-write protect
WaitEn				EQU		0			;bit2-external wait signal enable
WaitPol				EQU		0			;bit1-polarity of the external wait input for actiation
RBLE				EQU		0			;bit0-read byte lane enable
SMBCR0_0			EQU		((BMRead<<8)+(SMBLSPOL<<6)+(MW<<4)+(WP<<3)+(WaitEn<<2)+(WaitPol<<1)+RBLE)
SMBCR0_1			EQU		((WrapRead<<14)+(BlReadEn<<13)+(AddrValidReadEn<<12)+(BurstLenRead<<10)+(SyncReadDev<<9))
SMBCR0_2			EQU		((BlWriteEn<<21)+(AddrValidWriteEn<<20)+(BurstLenWrite<<18)+(SyncWriteDev<<17)+(BMWrite<<16))

WaitTourErr0			EQU		0			; external wait timeout error flag

WSTBRD0				EQU		0x1f			; burst read wait state

MemClkRatio			EQU		1			; SMMEMCLK :0-HCLK,1-HCLK/2,2-HCLK/3
SMClockEn			EQU		1			; SMCLK enable 0-only active during mem access,1-always running	
    ENDIF ;	IF :DEF: AMDBOOT
;#########################################################################################




;#########################################################################################


;=====================================================================================
;=====================================================================================
	GBLA	CLKVAL
	GBLA	DVSON		
	GBLA	HCLKVAL
	
	
;CLKVAL	SETA	133
;CLKVAL	SETA	266
;CLKVAL	SETA	400
CLKVAL	SETA	533
;CLKVAL	SETA	667


	[ CLKVAL = 667
	
DVSON	SETA	0	
HCLKVAL	SETA	133
Startup_Mdiv		EQU 	667
Startup_Pdiv		EQU 	3
Startup_Sdiv		EQU 	2

Startup_ARMCLKdiv	EQU	0	;	0	 :	ARMCLK	= MPLL/1
							;	1	 :	ARMCLK	= MPLL/2
							;	2	 :	ARMCLK	= MPLL/3
							;	3	 :	ARMCLK	= MPLL/4
							;	5	 :	ARMCLK	= MPLL/6
							;	7	 :	ARMCLK	= MPLL/8
							;	13	 :	ARMCLK	= MPLL/12
							;	15	 :	ARMCLK	= MPLL/16

Startup_PREdiv		EQU	0	;	0 :	PREDIV_CLK	= MPLL
							;	1 :	PREDIV_CLK	= MPLL/2
							;	2 :	PREDIV_CLK	= MPLL/3
							;	3 :	PREDIV_CLK	= MPLL/4
										
							;  	PREdiv   HCLKdiv

Startup_HCLKdiv		EQU	3	;	0 :	HCLK	= PREDIV_CLK
							;	1 :	HCLK	= PREDIV_CLK/2
							;	2 :	HCLK	= PREDIV_CLK/3
							;	3 :	HCLK	= PREDIV_CLK/4

Startup_PCLKdiv		EQU	1			;	0	 :	PCLK	= HCLK
									;	1	 :	PCLK	= HCLK/2
	]


	[ CLKVAL = 533
	
DVSON	SETA	0	
HCLKVAL	SETA	133
Startup_Mdiv		EQU 	533
Startup_Pdiv		EQU 	3
Startup_Sdiv		EQU 	2

Startup_ARMCLKdiv	EQU	0			;	0	 :	ARMCLK	= MPLL/1
							;	1	 :	ARMCLK	= MPLL/2
							;	2	 :	ARMCLK	= MPLL/3
							;	3	 :	ARMCLK	= MPLL/4
							;	5	 :	ARMCLK	= MPLL/6
							;	7	 :	ARMCLK	= MPLL/8
							;	13	 :	ARMCLK	= MPLL/12
							;	15	 :	ARMCLK	= MPLL/16

Startup_PREdiv		EQU	0	;	0  :	PREDIV_CLK	= MPLL
							;	1 :	PREDIV_CLK	= MPLL/2
							;	2 :	PREDIV_CLK	= MPLL/3
							;	3 :	PREDIV_CLK	= MPLL/4
										
							;  	PREdiv   HCLKdiv

Startup_HCLKdiv		EQU	3	;	0 :	HCLK	= PREDIV_CLK
							;	1 :	HCLK	= PREDIV_CLK/2
							;	2 :	HCLK	= PREDIV_CLK/3
							;	3 :	HCLK	= PREDIV_CLK/4

Startup_PCLKdiv		EQU	1	;	0	 :	PCLK	= HCLK
							;	1	 :	PCLK	= HCLK/2
	]

	
	[ CLKVAL = 400
	
DVSON	SETA	0	
HCLKVAL	SETA	133
Startup_Mdiv		EQU 	400
Startup_Pdiv		EQU 	3
Startup_Sdiv		EQU 	2

Startup_ARMCLKdiv	EQU	0	;	0	 :	ARMCLK	= MPLL/1
							;	1	 :	ARMCLK	= MPLL/2
							;	2	 :	ARMCLK	= MPLL/3
							;	3	 :	ARMCLK	= MPLL/4
							;	5	 :	ARMCLK	= MPLL/6
							;	7	 :	ARMCLK	= MPLL/8
							;	13	 :	ARMCLK	= MPLL/12
							;	15	 :	ARMCLK	= MPLL/16

Startup_PREdiv		EQU	0	;	0  :	PREDIV_CLK	= MPLL
							;	1 :	PREDIV_CLK	= MPLL/2
							;	2 :	PREDIV_CLK	= MPLL/3
							;	3 :	PREDIV_CLK	= MPLL/4
										
							;  	PREdiv   HCLKdiv
Startup_HCLKdiv		EQU	2	;	0 :	HCLK	= PREDIV_CLK
							;	1 :	HCLK	= PREDIV_CLK/2
							;	2 :	HCLK	= PREDIV_CLK/3
							;	3 :	HCLK	= PREDIV_CLK/4

Startup_PCLKdiv		EQU	1	;	0	 :	PCLK	= HCLK
							;	1	 :	PCLK	= HCLK/2
	]


	[ CLKVAL = 266
	
DVSON	SETA	0	
HCLKVAL	SETA	133
Startup_Mdiv		EQU 	266
Startup_Pdiv		EQU 	3
Startup_Sdiv		EQU 	2

Startup_ARMCLKdiv	EQU	0			;	0	 :	ARMCLK	= MPLL/1
							;	1	 :	ARMCLK	= MPLL/2
							;	2	 :	ARMCLK	= MPLL/3
							;	3	 :	ARMCLK	= MPLL/4
							;	5	 :	ARMCLK	= MPLL/6
							;	7	 :	ARMCLK	= MPLL/8
							;	13	 :	ARMCLK	= MPLL/12
							;	15	 :	ARMCLK	= MPLL/16

Startup_PREdiv		EQU	0	;	0 :	PREDIV_CLK	= MPLL
							;	1 :	PREDIV_CLK	= MPLL/2
							;	2 :	PREDIV_CLK	= MPLL/3
							;	3 :	PREDIV_CLK	= MPLL/4
										
							;  	PREdiv   HCLKdiv
Startup_HCLKdiv		EQU	1	;	0  :	HCLK	= PREDIV_CLK
							;	1 :	HCLK	= PREDIV_CLK/2
							;	2 :	HCLK	= PREDIV_CLK/3
							;	3 :	HCLK	= PREDIV_CLK/4

Startup_PCLKdiv		EQU	1	;	0	 :	PCLK	= HCLK
							;	1	 :	PCLK	= HCLK/2
	]

	
	[ CLKVAL = 133
	
DVSON	SETA	0	
HCLKVAL	SETA	133
Startup_Mdiv		EQU 	133
Startup_Pdiv		EQU 	3
Startup_Sdiv		EQU 	2

Startup_ARMCLKdiv	EQU	0			;	0	 :	ARMCLK	= MPLL/1
							;	1	 :	ARMCLK	= MPLL/2
							;	2	 :	ARMCLK	= MPLL/3
							;	3	 :	ARMCLK	= MPLL/4
							;	5	 :	ARMCLK	= MPLL/6
							;	7	 :	ARMCLK	= MPLL/8
							;	13	 :	ARMCLK	= MPLL/12
							;	15	 :	ARMCLK	= MPLL/16

Startup_PREdiv		EQU	0	;	0  :	PREDIV_CLK	= MPLL
							;	1 :	PREDIV_CLK	= MPLL/2
							;	2 :	PREDIV_CLK	= MPLL/3
							;	3 :	PREDIV_CLK	= MPLL/4
										
							;  	PREdiv   HCLKdiv
Startup_HCLKdiv		EQU	0	;	0 :	HCLK	= PREDIV_CLK
							;	1 :	HCLK	= PREDIV_CLK/2
							;	2 :	HCLK	= PREDIV_CLK/3
							;	3 :	HCLK	= PREDIV_CLK/4

Startup_PCLKdiv		EQU	1	;	0	 :	PCLK	= HCLK
							;	1	 :	PCLK	= HCLK/2
	]
	
Startup_EMdiv		EQU 	32			; EPLL -> 96Mhz
Startup_EPdiv		EQU   	1
Startup_ESdiv		EQU 	2			


; EBICON(0x4E80_0008) setting
;[31:11] Reserved
;[10] BANK3 0:SROM, 1:CF
;[9] BANK2 0:SROM, 1:CF
;[8] BANK1 0:SROM, 1:NAND
	GBLA    EBICON_VAL
;EBICON_VAL	SETA	((1<<7)+(1<<6)+(1<<5)+(0<<4)+(0<<3)+(1<<2)+(0<<1)+(0<<0))	; BANK[7:2]=SDRAM/SDRAM/RAM/CF/NAND/SRAM
;gao0105
;EBICON_VAL	SETA	((0<<10)+(0<<9)+(1<<8)+(1<<2)+(0<<1)+(0<<0))	; BANK[7:2]=SDRAM/SDRAM/RAM/CF/NAND/SRAM
EBICON_VAL	SETA	((0<<10)+(0<<9)+(0<<8)+(1<<2)+(0<<1)+(0<<0))	; BANK[7:2]=SDRAM/SDRAM/RAM/CF/ETHERNET/SRAM
;======================================================================================
;	MEM config	[SDR, DDR]
;======================================================================================

	[ BSP_TYPE = BSP_SMDK2450 
        ;gao1216
	GBLL	DDR2		; 256Mbit x16 x2ea = 512Mbit mem size
	]




	[ BSP_TYPE = BSP_SMDK2450 
	IF :DEF: mSDR

;---------------------------------------
; BANKCFG register  : SDRAM configure
;---------------------------------------
RASBW0				EQU		2			;	RAS addr 00=11bit,01-12bit,10=13bit, 11=14bit
RASBW1				EQU		2			;	RAS addr 00=11bit,01-12bit,10=13bit, 11=14bit
CASBW0				EQU		1			;	CAS addr 00=8bit,01-9bit,10=10bit, 11=11bit
CASBW1				EQU		1			;	CAS addr 00=8bit,01-9bit,10=10bit, 11=11bit
ADDRCFG0			EQU		1			; 	addre configure
												;   00={BA,RAS,CAS}, 01={RAS,BA,CAS}
ADDRCFG1			EQU		1			; 	addre configure
												;   00={BA,RAS,CAS}, 01={RAS,BA,CAS}
MEMCFG				EQU		2			; 	Ext.Mem 000=SDR,010=MSDR,100=DDRz,110=MDDR,001=DDR2
BW				EQU		0			; 	Bus width 00=32bit,01=16bit

;---------------------------------------
; BANKCON1 register : SDRAM timing control
;---------------------------------------
BStop				EQU		0			;	read burst stop control
WBUF				EQU		1			;	write buffer control
AP					EQU		1			;	auto precharge control
PWRDN				EQU		0			;	power down mode
DQSDelay			EQU		4			;	DQS delay

;-----------------------[SDRAM]-------------------------------
;    HCLK		tRAS		tARFC		CAS		tRCD		RP		REFCYC
;---------------------------------------------------------------
;18MHz			0		1		3		0		0		140
;36MHz			1		2		3		0		0		280
; 100Mhz		5		7		3		1		1		780
; 110Mhz		5		7		3		2		2		858
; 120Mhz		6		7		3		2		2		936
; 133Mhz		6		10		3		2		2		1037
; 133.5Mhz		6		10		3		2		2		1041
; 135Mhz		6		10		3		2		2		1053
; 136Mhz		6		10		3		2		2		1060
; 136.5Mhz		6		10		3		2		2		1064
; 137.3Mhz		6		10		3		2		2		1070
; 138Mhz		6		10		3		2		2		1076
; 138.67Mhz		6		10		3		2		2		1081
; 140Mhz		6		11		3		3		3		1092 (out of spec)
; 141Mhz		6		11		3		3		3		1099
; 142Mhz		7		11		3		3		3		1107 (out of spec)
; 143.6Mhz		7		11		3		3		3		1120 (out of spec)		
;---------------------------------------------------------------
;---------------------------------------
; BANKCON2 register : SDRAM timing control 
;---------------------------------------

	[ HCLKVAL = 18
tRAS					EQU		4			;	Row active time
tARFC					EQU		6			;	Row cycle time
CL					EQU		3			;	CAS latency control
tRCD					EQU		1			;	RAS to CAS delay
tRP					EQU		1			; 	Row pre-charge time
	]
	[ HCLKVAL = 133
tRAS					EQU		6			;	Row active time
tARFC					EQU		10			;	Row cycle time
CL					EQU		3			;	CAS latency control
tRCD					EQU		2			;	RAS to CAS delay
tRP					EQU		2			; 	Row pre-charge time
	]
;---------------------------------------
; BANKCON3 register : SDRAM MRS/EMRS register
;---------------------------------------
BA_EMRS				EQU		2			;	BA : EMRS
DS				EQU		0			;	Driver strength
PASR				EQU		0			;	PASR
BA_MRS				EQU		0			;	BA : MRS
TM				EQU		0			; 	Test Mode - mode register set
CL_MRS				EQU		3			; 	CAS Latency

;---------------------------------------
; REFRESH register : SDRAM refresh register

	[ HCLKVAL = 18
REFCYC				EQU		140		; 	refresh cycle
	]
	[ HCLKVAL = 133
REFCYC				EQU		1037		; 	refresh cycle
	]
	ENDIF

	IF :DEF: mDDR

;---------------------------------------
; BANKCFG register  : mDDR SDRAM configure
;---------------------------------------
RASBW0				EQU		2			;	RAS addr 00=11bit,01-12bit,10=13bit, 11=14bit
RASBW1				EQU		2			;	RAS addr 00=11bit,01-12bit,10=13bit, 11=14bit
CASBW0				EQU		2			;	CAS addr 00=8bit,01-9bit,10=10bit, 11=11bit
CASBW1				EQU		2			;	CAS addr 00=8bit,01-9bit,10=10bit, 11=11bit
ADDRCFG0			EQU		1			; 	addre configure
									;   	00={BA,RAS,CAS}, 01={RAS,BA,CAS}
ADDRCFG1			EQU		1			; 	addre configure
												;   00={BA,RAS,CAS}, 01={RAS,BA,CAS}
MEMCFG				EQU		6			; 	Ext.Mem 000=SDR,010=MSDR,100=DDRz,110=MDDR,001=DDR2
BW				EQU		1			; 	Bus width 00=32bit,01=16bit

;---------------------------------------
; BANKCON1 register : mDDR SDRAM timing control
;---------------------------------------
BStop				EQU		0			;	read burst stop control
WBUF				EQU		1			;	write buffer control
AP				EQU		1			;	auto precharge control
PWRDN				EQU		1			;	power down mode
DQSDelay			EQU		4			;	DQS delay

;-----------------------[DDR2 SDRAM]--------------------------
;    HCLK		tRAS		tARFC		CAS		tRCD		tRP		REFCYC
;---------------------------------------------------------------
;18MHz			0		1		3		0		0		140
;36MHz			1		2		3		0		0		280
;100Mhz			4		7		3		2		2		780
;110Mhz			4		8		3		2		2		858
;120Mhz			5		9		3		2		2		936
;133Mhz			5		10		3		2		2		1037		
;---------------------------------------------------------------

;---------------------------------------
; BANKCON2 register : mDDR SDRAM timing control
;---------------------------------------
	[ HCLKVAL = 18
tRAS				EQU		0			;	Row active time
tARFC				EQU		1			;	Row cycle time
CL				EQU		3			;	CAS latency control
tRCD				EQU		0			;	RAS to CAS delay
tRP				EQU		0			; 	Row pre-charge time
	]
	[ HCLKVAL = 133
tRAS				EQU		5			;	Row active time
tARFC				EQU		10			;	Row cycle time
CL				EQU		3			;	CAS latency control
tRCD				EQU		2			;	RAS to CAS delay
tRP				EQU		2			; 	Row pre-charge time
	]
;---------------------------------------
; BANKCON3 register : mDDR SDRAM MRS/EMRS register
;---------------------------------------
BA_EMRS				EQU		2			;	BA : EMRS
DS				EQU		0			;	Driver strength
PASR				EQU		0			;	PASR
BA_MRS				EQU		0			;	BA : MRS
TM				EQU		0			; 	Test Mode - mode register set
CL_MRS				EQU		3			; 	CAS Latency

;---------------------------------------
; REFRESH register : mDDR SDRAM refresh register
;--------------------------------------
	[ HCLKVAL = 18
REFCYC				EQU		140			; 	refresh cycle
	]
	[ HCLKVAL = 133
REFCYC				EQU		1037			; 	refresh cycle
	]
;---------------------------------------------------------------------------

	ENDIF
	IF :DEF: DDR2

;---------------------------------------
; BANKCFG register  : DDR2 SDRAM configure
;---------------------------------------
;gao1216
;64M DDR-II
RASBW0				EQU		2			;	RAS addr 00=11bit,01-12bit,10=13bit, 11=14bit
;128M DDR-II
;RASBW0				EQU		3			;	RAS addr 00=11bit,01-12bit,10=13bit, 11=14bit
RASBW1				EQU		2			;	RAS addr 00=11bit,01-12bit,10=13bit, 11=14bit
CASBW0				EQU		2			;	CAS addr 00=8bit,01-9bit,10=10bit, 11=11bit
CASBW1				EQU		2			;	CAS addr 00=8bit,01-9bit,10=10bit, 11=11bit
ADDRCFG0			EQU		1			; 	addre configure
												;   00={BA,RAS,CAS}, 01={RAS,BA,CAS}
ADDRCFG1			EQU		1			; 	addre configure
												;   00={BA,RAS,CAS}, 01={RAS,BA,CAS}
MEMCFG				EQU		1			; 	Ext.Mem 000=SDR,010=MSDR,100=DDRz,110=MDDR,001=DDR2
BW				EQU		1			; 	Bus width 00=32bit,01=16bit


;---------------------------------------
; BANKCON1 register : DDR2 SDRAM timing control
;---------------------------------------
PADLOAD				EQU		1
BStop				EQU		0			;	read burst stop control
WBUF				EQU		1			;	write buffer control
AP					EQU		0			;	auto precharge control
PWRDN				EQU		0			;	power down mode
DQSDelay			EQU		1			;3	DQS delay

;-----------------------[DDR2 SDRAM]--------------------------
;    HCLK		tRAS		tARFC		CAS		tRCD		tRP		REFCYC
;---------------------------------------------------------------
;18MHz			0		1		3		0		0		140
;36MHz			1		2		3		0		0		280
;100Mhz			4		7		3		2		2		780
;110Mhz			4		8		3		2		2		858
;120Mhz			5		9		3		2		2		936
;133Mhz			5		10		3		2		2		1037		
;---------------------------------------------------------------

;---------------------------------------
; BANKCON2 register : DDR2 SDRAM timing control
;---------------------------------------
	[ HCLKVAL = 18
tRAS				EQU		0			;	Row active time
tARFC				EQU		1			;	Row cycle time
CL					EQU		3			;	CAS latency control
tRCD				EQU		0			;	RAS to CAS delay
tRP					EQU		0			; 	Row pre-charge time
	]
	[ HCLKVAL = 133
tRAS				EQU		5			;5	Row active time
tARFC				EQU		10			;10	Row cycle time
CL					EQU		3			;3	CAS latency control
tRCD				EQU		2			;2	RAS to CAS delay
tRP					EQU		2			;2 	Row pre-charge time
	]

;---------------------------------------
; BANKCON3 register : DDR2 SDRAM MRS/EMRS register
;---------------------------------------
BA_EMRS1			EQU		1			;	BA : EMRS
DLL_DIS				EQU		1
DLL_EN				EQU		0
nDQS_DIS			EQU		1
RDQS_DIS			EQU		0
OCD_MODE_EXIT		EQU		0
OCD_MODE_DEFAULT	EQU		7
BA_EMRS2			EQU		2			;	BA : EMRS
BA_EMRS3			EQU		3			;	BA : EMRS
DS					EQU		0			;	Driver strength
PASR				EQU		0			;	PASR
BA_MRS				EQU		0			;	BA : MRS
TM					EQU		0			; 	Test Mode - mode register set
CL_MRS				EQU		3			; 	CAS Latency
DLL_RESET_HIGH		EQU		1
DLL_RESET_LOW		EQU		0

;---------------------------------------
; REFRESH register : DDR2 SDRAM refresh register
;---------------------------------------
	[ HCLKVAL = 18
REFCYC				EQU		140			; 	refresh cycle
	]
	[ HCLKVAL = 133
REFCYC				EQU		1037			; 	refresh cycle
	]
;---------------------------------------------------------------------------
					
	ENDIF	
	]	

	[ BSP_TYPE = BSP_SMDK2450 
BANKCFGVAL   EQU		((RASBW0<<17)+(RASBW1<<14)+(CASBW0<<11)+(CASBW1<<8)+(ADDRCFG0<<6)+(ADDRCFG1<<4)+(MEMCFG<<1)+(BW<<0))
BANKCON1VAL  EQU		((DQSDelay<<28)+(1<<26)+(BStop<<7)+(WBUF<<6)+(AP<<5)+(PWRDN<<4))
BANKCON2VAL  EQU		((tRAS<<20)+(tARFC<<16)+(CL<<4)+(tRCD<<2)+(tRP<<0))
	IF :DEF: mSDR
BANKCON3VAL  EQU		((BA_EMRS<<30)+(DS<<21)+(PASR<<16)+(BA_MRS<<14)+(TM<<7)+(CL_MRS<<4))	
	ENDIF
	IF :DEF: mDDR
BANKCON3VAL  EQU		((BA_EMRS<<30)+(DS<<21)+(PASR<<16)+(BA_MRS<<14)+(TM<<7)+(CL_MRS<<4))	
	ENDIF
	]
	END

