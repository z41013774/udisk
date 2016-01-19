
;Pre-defined constants
USERMODE    EQU 	0x10
FIQMODE     EQU 	0x11
IRQMODE     EQU 	0x12
SVCMODE     EQU 	0x13
ABORTMODE   EQU 	0x17
UNDEFMODE   EQU 	0x1b
MODEMASK    EQU 	0x1f
NOINT       EQU 	0xc0
_STACK_BASEADDRESS	EQU   0x33ff8000

;The location of stacks
UserStack	EQU	(_STACK_BASEADDRESS-0x3800)	;0x33ff4800 ~
SVCStack	EQU	(_STACK_BASEADDRESS-0x2800)	;0x33ff5800 ~
UndefStack	EQU	(_STACK_BASEADDRESS-0x2400)	;0x33ff5c00 ~
AbortStack	EQU	(_STACK_BASEADDRESS-0x2000)	;0x33ff6000 ~
IRQStack	EQU	(_STACK_BASEADDRESS-0x1000)	;0x33ff7000 ~
FIQStack	EQU	(_STACK_BASEADDRESS-0x0)	;0x33ff8000 ~	IMPORT main

	MACRO
$Handerlable	HANDLER 	$Handlelable
$Handerlable
	
		;sub sp,sp,#4
		stmfd sp!,{r0,lr}
		ldr 	r0,=$Handlelable;
		ldr 	r0,[r0]  ;
		str 	r0,[sp,#4]		
		ldmfd	sp!,{r0,pc}  
		MEND

	IMPORT maskintr
	IMPORT memoryconfig
	IMPORT initsystemclk
	IMPORT disable_watch_dog
	IMPORT copycodetoddr
	IMPORT main
	IMPORT  |Image$$RO$$Base|	; Base of ROM code
	IMPORT  |Image$$RO$$Limit|  ; End of ROM code (=start of ROM data)
	IMPORT  |Image$$RW$$Base|   ; Base of RAM to initialise
	IMPORT  |Image$$ZI$$Base|   ; Base and limit of area
	IMPORT  |Image$$ZI$$Limit|  ; to zero initialise

		IMPORT	CopyProgramFromNand

	AREA init,CODE, READONLY
	ENTRY
		EXPORT __ENTRY
__ENTRY
ResetEntry
	b   ResetHandler
	b	HandlerUndef	;handler for Undefined mode
	b	HandlerSWI	 	;handler for SWI interrupt
	b	HandlerPabort	;handler for PAbort
	b	HandlerDabort	;handler for DAbort
	b	.				;reserved
	b	HandlerIRQ		;handler for IRQ interrupt
	b	HandlerFIQ		;handler for FIQ interrupt

	
HandlerFIQ		HANDLER HandleFIQ
HandlerIRQ		HANDLER HandleIRQ
HandlerUndef	HANDLER HandleUndef
HandlerSWI		HANDLER HandleSWI
HandlerDabort	HANDLER HandleDabort
HandlerPabort	HANDLER HandlePabort

ResetHandler
	bl disable_watch_dog
	bl maskintr
	bl initsystemclk
	bl memoryconfig
	bl initstacks
	;bl judgecodeposition
	bl InitRam
	;bl CopyProgramFromNand
	bl sethandler 
	b main
MAIN_LOOP
			b MAIN_LOOP

IsrIRQ
;	sub sp,sp,#4	   ;reserved for PC
	stmfd	sp!,{r8-r9,lr}
	
	ldr r9,=0x4a000014
	ldr r9,[r9]
	ldr r8,=HandleEINT0
	add r8,r8,r9,lsl #2
	ldr r8,[r8]
	str r8,[sp,#8]
	ldmfd	sp!,{r8-r9,pc}
sethandler
	ldr	r0,=HandleIRQ		;This routine is needed
	ldr r1,=IsrIRQ	  ;if there is not 'subs pc,lr,#4' at 0x18, 0x1c
	str r1,[r0]
	mov pc,lr

judgecodeposition
	
	bl InitRam
	;copy code to ddr in feature
	adr	r0, ResetEntry		;OM[1:0] == 0, NAND FLash boot
	cmp	r0, #0
	;bl copycodetoddr
	
	mov pc,lr
	
InitRam 
	ldr r2, BaseOfBSS
	ldr r3, BaseOfZero	
0
	cmp r2, r3
	ldrcc	r1, [r0], #4
	strcc	r1, [r2], #4
	bcc %B0 

	mov r0, #0
	ldr r3, EndOfBSS
1	
	cmp r2, r3
	strcc	r0, [r2], #4
	bcc %B1
	mov pc,lr

initstacks

		;Do not use DRAM,such as stmfd,ldmfd......
		;SVCstack is initialized before
		;Under toolkit ver 2.5, 'msr cpsr,r1' can be used instead of 'msr cpsr_cxsf,r1'
		mrs r0,cpsr
		bic r0,r0,#MODEMASK
		orr r1,r0,#UNDEFMODE|NOINT
		msr cpsr_cxsf,r1		;UndefMode
		ldr sp,=UndefStack		; UndefStack=0x33FF_5C00
	
		orr r1,r0,#ABORTMODE|NOINT
		msr cpsr_cxsf,r1		;AbortMode
		ldr sp,=AbortStack		; AbortStack=0x33FF_6000
	
		orr r1,r0,#IRQMODE|NOINT
		msr cpsr_cxsf,r1		;IRQMode
		ldr sp,=IRQStack		; IRQStack=0x33FF_7000
	
		orr r1,r0,#FIQMODE|NOINT
		msr cpsr_cxsf,r1		;FIQMode
		ldr sp,=FIQStack		; FIQStack=0x33FF_8000
	
		bic r0,r0,#MODEMASK|NOINT
		orr r1,r0,#SVCMODE
		msr cpsr_cxsf,r1		;SVCMode
		ldr sp,=SVCStack		; SVCStack=0x33FF_5800
	
		;USER mode has not be initialized.
	
		mov pc,lr

BaseOfROM	DCD |Image$$RO$$Base|
TopOfROM	DCD |Image$$RO$$Limit|
BaseOfBSS	DCD |Image$$RW$$Base|
BaseOfZero	DCD |Image$$ZI$$Base|
EndOfBSS	DCD |Image$$ZI$$Limit|

	ALIGN
		
	AREA RamData, DATA, READWRITE
		
	^	0x33ffff00		; _ISR_STARTADDRESS=0x33FF_FF00
HandleReset 	#	4
HandleUndef 	#	4
HandleSWI		#	4
HandlePabort	#	4
HandleDabort	#	4
HandleReserved	#	4
HandleIRQ		#	4
HandleFIQ		#	4
		
		;Do not use the label 'IntVectorTable',
		;The value of IntVectorTable is different with the address you think it may be.
		;IntVectorTable
		;@0x33FF_FF20
HandleEINT0 	#	4
HandleEINT1 	#	4
HandleEINT2 	#	4
HandleEINT3 	#	4
HandleEINT4_7	#	4
HandleEINT8_23	#	4
HandleCAM		#	4		; Added for 2440.
HandleBATFLT	#	4
HandleTICK		#	4
HandleWDT		#	4
HandleTIMER0	#	4
HandleTIMER1	#	4
HandleTIMER2	#	4
HandleTIMER3	#	4
HandleTIMER4	#	4
HandleUART2 	#	4
		;@0x33FF_FF60
HandleLCD		#	4
HandleDMA0		#	4
HandleDMA1		#	4
HandleDMA2		#	4
HandleDMA3		#	4
HandleMMC		#	4
HandleSPI0		#	4
HandleUART1 	#	4
HandleNFCON 	#	4		; Added for 2440.
HandleUSBD		#	4
HandleUSBH		#	4
HandleIIC		#	4
HandleUART0 	#	4
HandleSPI1		#	4
HandleRTC		#	4
HandleADC		#	4
	;@0x33FF_FFA0

	END
		

