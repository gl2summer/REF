SRCPND   	EQU  0x4a000000    ; Source pending
INTPND   	EQU  0x4a000010    ; Interrupt request status

rEINTPEND   EQU  0x560000a8
INTOFFSET   EQU  0x4a000014

USERMODE    EQU 	0x10
FIQMODE     EQU 	0x11
IRQMODE     EQU 	0x12
SVCMODE     EQU 	0x13
ABORTMODE   EQU 	0x17
UNDEFMODE   EQU 	0x1b
MODEMASK    EQU 	0x1f
NOINT       EQU 	0xc0
;********************************************************************************************************
;                                          IMPORT AND EXPORT
;*******************************************************************************************************
	    IMPORT  high_ready_obj
	    IMPORT  raw_time_tick
	    IMPORT  raw_stack_check
        EXPORT  port_task_switch
		EXPORT  raw_int_switch
		IMPORT  raw_task_active
        EXPORT  raw_start_first_task
		EXPORT  OS_CPU_SR_Save
		EXPORT  OS_CPU_SR_Restore
		EXPORT  read_cpsr
		EXPORT	write_cpsr
        IMPORT  irq_process
        IMPORT  raw_enter_interrupt
        IMPORT  raw_finish_int  
        EXPORT  raw_os_interrupt               ;//	Êä³öµÄ±êºÅ	
        EXPORT	CPU_IntDis
        EXPORT	CPU_IntEn
    	IMPORT 	raw_int_nesting
		EXPORT  gorun
		EXPORT  jump_to_dest
    	
    PRESERVE8
	AREA UCOS_ARM, CODE, READONLY


CPU_IntDis
	MRS	R0,CPSR
	ORR	R0,R0,#0xC0
	MSR	CPSR_c,R0
	BX	LR


CPU_IntEn
	MRS	R0,CPSR
	AND	R1,R0,#0x3F
	MSR	CPSR_c,R1
	BX	LR
;;********************************************************************************************************
;;;                                          START MULTITASKING
;;                                       void OSStartHighRdy(void)
;;
;; Note(s) : 1) This function triggers a PendSV exception (essentially, causes a context switch) to cause
;;              the first task to start.
;;
;;           2) OSStartHighRdy() MUST:
;;              a) Setup PendSV exception priority to lowest;
;;              b) Set initial PSP to 0, to tell context switcher this is first run;
;;              c) Set OSRunning to TRUE;
;;              d) Trigger PendSV exception;
;;              e) Enable interrupts (tasks will run with interrupts enabled).
;;********************************************************************************************************;
raw_start_first_task
	
		LDR R0, =raw_task_active
		LDR R0, [R0]         
		LDR SP, [R0]
		
		LDMFD 	SP!, {R0}  
		MSR 	SPSR_cxsf, R0
		LDMFD 	SP!, {R0-R12, LR, PC}^

;;********************************************************************************************************
;;                               PERFORM A CONTEXT SWITCH (From task level)
;;                                           void OSCtxSw(void)
;;
;;; Note(s) : 1) OSCtxSw() is called when OS wants to perform a task context switch.  This function
;;;              triggers the PendSV exception which is where the real work is done.
;;********************************************************************************************************
port_task_switch
        STMFD SP!,{LR}              ; push pc (lr should be pushed in place of PC)
        STMFD SP!,{R0-R12,LR}       ; push lr & register file
        MRS   R0,CPSR
        STMFD SP!,{R0}              ; push current cpsr
   
        ; raw_task_active->task_stack = SP
		LDR	R0, =raw_task_active
		LDR	R0, [R0]
		STR	SP, [R0]

		PUSH    {R14}
		bl      raw_stack_check
		POP     {R14}


raw_int_switch

	LDR		R0, =high_ready_obj
	LDR		R1, =raw_task_active
	LDR		R0, [R0]
	STR		R0, [R1]
		
	LDR		R0, =raw_task_active
	LDR		R0, [R0]
	LDR		SP, [R0]
	
	;----------------------------------------------------------------------------------	
	; Restore New Task context
	;----------------------------------------------------------------------------------	
	LDMFD 	SP!, {R0}              ;POP CPSR
	MSR 	SPSR_cxsf, R0
	LDMFD 	SP!, {R0-R12, LR, PC}^	


OS_CPU_SR_Save

	MRS     R0, CPSR				; Set IRQ and FIQ bits in CPSR to disable all interrupts
	ORR     R1, R0, #0xC0
	MSR     CPSR_c, R1
	MRS     R1, CPSR				; Confirm that CPSR contains the proper interrupt disable flags
	AND     R1, R1, #0xC0
	CMP     R1, #0xC0
	BNE     OS_CPU_SR_Save				; Not properly disabled (try again)
	BX LR					; Disabled, return the original CPSR contents in R0

OS_CPU_SR_Restore

	MSR     CPSR_c, R0
	BX 		LR


read_cpsr
	MRS R0, CPSR
	BX  LR


write_cpsr
	MSR   CPSR_cxsf,R0
	BX  LR

gorun
    mov lr,r0
	bx lr

jump_to_dest
	ldr   r0, =0x33400000;
	bx r0

raw_os_interrupt

	STMFD   SP!, {R1-R3}			; We will use R1-R3 as temporary registers


	MOV     R1, SP
	ADD     SP, SP, #12             ;Adjust IRQ stack pointer
	SUB     R2, LR, #4              ;Adjust PC for return address to task

	MRS     R3, SPSR				; Copy SPSR (Task CPSR)
	
   

	MSR     CPSR_cxsf, #SVCMODE:OR:NOINT   ;Change to SVC mode

									; SAVE TASK''S CONTEXT ONTO OLD TASK''S STACK
									
	STMFD   SP!, {R2}				; Push task''s PC 
	STMFD   SP!, {R4-R12, LR}		; Push task''s LR,R12-R4
	
	LDMFD   R1!, {R4-R6}			; Load Task''s R1-R3 from IRQ stack 
	STMFD   SP!, {R4-R6}			; Push Task''s R1-R3 to SVC stack
	STMFD   SP!, {R0}			    ; Push Task''s R0 to SVC stack
	
	STMFD   SP!, {R3}				; Push task''s CPSR
	
	LDR     R4,=raw_task_active            
	LDR     R5,[R4]
	STR     SP,[R5] 

	PUSH    {R14}
	bl      raw_stack_check
	POP     {R14}
	
	BL raw_enter_interrupt	
	
	MSR    CPSR_c,#IRQMODE:OR:NOINT    ;Change to IRQ mode to use IRQ stack to handle interrupt
		
	BL      irq_process
    MSR		CPSR_c,#SVCMODE:OR:NOINT   ;Change to SVC mode
    BL raw_finish_int
    LDMFD   SP!,{R4}               ;POP the task''s CPSR 
    MSR		SPSR_cxsf,R4
    LDMFD   SP!,{R0-R12,LR,PC}^	   ;POP new Task''s context

	END


