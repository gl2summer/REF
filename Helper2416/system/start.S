	INCLUDE s2450addr.inc
	INCLUDE	s3c2450cfg.inc
	
	IMPORT Main					;//c语言入口函数
	IMPORT raw_os_interrupt;
	IMPORT Undef_exception_process	;//未定义指令异常
	IMPORT SWI_exception_process		;//软中断异常
	IMPORT Pabort_exception_process	;//取指中止异常
	IMPORT Dabort_exception_process ;
	IMPORT FIQ_exception_process    ;//
	IMPORT Undef_exception_process
	IMPORT reset_exception_process
	IMPORT make_mmu_table
	
	EXPORT  exception_vector			;//
	
	 PRESERVE8						;//字节对齐
	 CODE32							;//ARM代码
	 AREA RESET,CODE,READONLY		;//reset名，代码段，只读
	 ENTRY						    ;//程序入口


	 b HANDLE_ResetInit 		
	

R1_M	 EQU (1<<0)
R1_A	 EQU (1<<1)
R1_C	 EQU (1<<2)
R1_W	 EQU (1<<3)
R1_Z	 EQU (1<<11)   
R1_I	 EQU (1<<12)
R1_V	 EQU (1<<13)
R1_RR	 EQU (1<<14)
R1_VE	 EQU (1<<24)
R1_nF	 EQU (1<<30)
R1_iA	 EQU (1<<31) 


HANDLE_ResetInit

	mrc 	p15,0,r0,c1,c0,0
	ldr		r1,=~(R1_M :OR R1_I :OR R1_C :OR R1_W)	   ;//Disable MMU 
	and		r0,r0,r1
	mcr 	p15,0,r0,c1,c0,0

	
	mov r0, #0 
	;invalidate I,D caches on v4 
	mcr p15, 0, r0, c7, c7 
	;drain write buffer on v4 
	mcr p15, 0, r0, c7, c10, 4
	;invalidate I,D TLBs on v4
	mcr p15, 0, r0, c8, c7 

	
	
	mov r1, #0xd3			;//svc mode
	msr		cpsr_cxsf,r1
	ldr		sp,=0x33fe4000
	
	;set to high vector address
	;read c1 to r5
	MRC p15,0,r5,c1,c0,0
	
	;set bit 13 of c1
	orr	r5, r5, #0x2000
	
	;write r5 to c1
	mcr	p15, 0, r5, c1, c0, 0



	;------------------------
	; disable the watchdog timer.

	ldr		r0, =WTCON
	mov		r1, #0
	str		r1, [r0]

	;------------------------
	; Interrupt configuration
	
	ldr	r0, =INTMSK      ; mask all first-level interrupts.
	ldr	r1, =0xffffffff
	str	r1, [r0]

	
	ldr	r0, =INTSUBMSK   ; mask all second-level interrupts.
	ldr	r1, =0xffffffff
	str	r1, [r0]
	
	bl make_mmu_table

	;mmu page table
	ldr r1,=0x30100000
	mcr	p15, 0, r1, c2, c0, 0

	;//MMU_SetDomain
  	;ldr		r0,=0x55555550|(0<<2)|(1<<0)
   	ldr		r0,=0x55555555
   	mcr 	p15,0,r0,c3,c0,0
		
 	;//MMU_SetProcessId
   	ldr 	r0,=0
   	mcr 	p15,0,r0,c13,c0,0
   	   
	;//MMU_Enable
  	ldr 	r1, =(R1_M :OR R1_I :OR R1_C :OR R1_W :OR R1_V:OR R1_iA:OR R1_nF)
   	mrc 	p15,0,r0,c1,c0,0
   	orr 	r0,r0,r1
   	mcr 	p15,0,r0,c1,c0,0
	nop 
	nop   	
	ldr		pc,=virtual_start
	nop 
	nop
virtual_start
	nop
	nop
	nop
	nop


	mov r1, #0x11			;//FIQ mode
	msr		cpsr_cxsf,r1
	ldr		sp,=0x30200000
	
	mov r1, #0x12			;//IRQ mode
	msr		cpsr_cxsf,r1
	ldr		sp,=0x301fc000

	mov r1, #0x1f			;//sys mode
	msr		cpsr_cxsf,r1
	ldr		sp,=0x301f8000	
		
	mov r1, #0x17			;//abt mode
	msr		cpsr_cxsf,r1
	ldr		sp,=0x301f4000

	mov r1, #0x1b			;//undef mode
	msr		cpsr_cxsf,r1
	ldr		sp,=0x301f0000
	
	mov r1, #0xd3			;//svc mode
	msr		cpsr_cxsf,r1
	ldr		sp,=0x301ec000

	BL InitRORWZI
	
	LDR PC,=Main


;//***********************************************
;//clear bss
;//***********************************************
	;IMPORT |Image$$ER_ROM1$$RO$$Limit| ; End of ROM code (=start of ROM data)
	;IMPORT |Image$$RW_RAM1$$RW$$Base| ; Base of RAM to initialise
	IMPORT |Image$$RW_RAM1$$ZI$$Base| ; Base and limit of area
	IMPORT |Image$$RW_RAM1$$ZI$$Limit| ; to zero initialise
	

InitRORWZI

   	STMFD SP!,{R0-R7,LR}

	LDR R1,=|Image$$RW_RAM1$$ZI$$Base|
	LDR R2,=|Image$$RW_RAM1$$ZI$$Limit|

	MOV R0,#0

2	
	CMP R1,R2
	BGE %F3
	STR R0,[R1]
	add r1, r1, #4
	B %B2
3
	LDMFD SP!,{R0-R7,PC}



Undef_exception_process_asm
	mov r0, lr
	b Undef_exception_process


exception_vector	;
	LDR PC,=reset_exception_process 			;//复位异常
	LDR PC,=Undef_exception_process_asm		;//未定义指令异常
	LDR PC,=SWI_exception_process		;//软中断异常
	LDR PC,=Pabort_exception_process	;//取指中止异常
	LDR PC,=Dabort_exception_process	;//数据中止异常
	LDR PC,.							;//保留
	LDR PC,=raw_os_interrupt
	LDR PC,=FIQ_exception_process		;//FIQ中断异常

	
   END

