;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;文件名称：2440_init.s
;;功能说明：2440启动代码
;;编写：张锡冰 ADaiPlaying工作室 河北大学
;;编写时间：2010/6/12
;;修改记录：2010/6/29,添加2440.inc和Option.inc文件，添加PLL控制相关代码
;;			2010/7/24,添加中断映射表和堆栈初始化
;;			2010/8/15,将启动文件移到KEIL MDK上，修改如下：
;;					  1.PRESERVE8
;;			2010/8/18,将IRQ中断注册给uCOS-II管理
;;版本说明：V1.2
;;其他说明：2010/7/20日始，在保定飞凌TE-II板上学习
;;			版权所有，盗版请注明出处
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	GET 2440.inc
	GET option.inc
	GET memcfg.inc
	
	
	
	
	;预定义工作模式常量
USERMODE    EQU 	0x10	;用户模式
FIQMODE     EQU 	0x11	;快速中断模式
IRQMODE     EQU 	0x12	;中断模式
SVCMODE     EQU 	0x13	;管理模式
ABORTMODE   EQU 	0x17	;中止模式
UNDEFMODE   EQU 	0x1b	;未定义模式
MODEMASK    EQU 	0x1f	
NOINT       EQU 	0xc0	



	;分配堆栈区域
UserStack	EQU	(_STACK_BASEADDRESS-0x3800)	;0x33ff4800 ~
SVCStack	EQU	(_STACK_BASEADDRESS-0x2800)	;0x33ff5800 ~
UndefStack	EQU	(_STACK_BASEADDRESS-0x2400)	;0x33ff5c00 ~
AbortStack	EQU	(_STACK_BASEADDRESS-0x2000)	;0x33ff6000 ~
IRQStack	EQU	(_STACK_BASEADDRESS-0x1000)	;0x33ff7000 ~
FIQStack	EQU	(_STACK_BASEADDRESS-0x0)	;0x33ff8000 ~

     
	MACRO
$HandlerLabel HANDLER $HandleLabel

$HandlerLabel
    SUB	    SP,SP,#4			;decrement sp(to store jump address)
    STMFD   SP!,{R0}			;PUSH the work register to stack(lr does't push because it return to original address)
    LDR	    R0,=$HandleLabel	;load the address of HandleXXX to r0
    LDR	    R0,[R0]				;load the contents(service routine start address) of HandleXXX
    STR	    R0,[SP,#4]			;store the contents(ISR) of HandleXXX to stack
    LDMFD   SP!,{R0,PC}			;POP the work register and pc(jump to ISR)
    MEND



	IMPORT  |Image$$RO$$Base|	;只读代码基(起始)地址
	IMPORT  |Image$$RO$$Limit|  ;只读代码结束地址
	IMPORT  |Image$$RW$$Base|   ;RW段起始地址,存放初始化的变量
	IMPORT  |Image$$ZI$$Base|   ;Zero段起始地址
	IMPORT  |Image$$ZI$$Limit|  ;Zero段结束地址

	IMPORT	OS_CPU_IRQ_ISR		;引入uCOS-II IRQ中断入口

  	EXPORT	HandleEINT0			;为Os_cpu_a.s输出标号


	PRESERVE8
  AREA    RESET,CODE,READONLY 
    ENTRY
    EXPORT	__ENTRY		;映像入口标号，提供给ARM Linker的地址
__ENTRY
ResetEntry
	;异常中断向量表
    B ResetHandler				;调试用，复位异常		工作于管理模式	 	
    B HandlerUndef				;未定义的指令UND异常	工作于中止模式
    B HandlerSWI				;软件中断SWI异常		工作于管理模式
    B HandlerPabort				;指令预取终止PABT异常	工作于中止模式
    B HandlerDabort				;数据访问中止DABT异常	工作于中止模式
    B .							;handlerReserved
    B HandlerIRQ				;中断请求IRQ异常		工作于中断模式
    B HandlerFIQ				;外部中断请求FIQ异常	工作于快速中断模式



 	LTORG	;声明数据缓冲池伪指令	

HandlerFIQ		HANDLER	HandleFIQ
HandlerIRQ		HANDLER HandleIRQ
HandlerUndef	HANDLER HandleUndef
HandlerSWI		HANDLER HandleSWI
HandlerDabort	HANDLER HandleDabort
HandlerPabort	HANDLER HandlePabort



	
	
	LTORG

;复位入口地址

ResetHandler
	;****************************************************
	;关闭看门狗        								
	;****************************************************
	LDR	R0,=WTCON       ;关闭看门狗
	LDR	R1,=0x0
	STR	R1,[R0]
	
	;****************************************************
	;屏蔽所有中断        								
	;****************************************************
	LDR R0, =INTMSK		
	LDR R1,=0xffffffff		;屏蔽所有中断
	STR R1,[R0]
	
	LDR R0,=SUBINTMSK
	LDR R1,=0xffff			;清除子中断控制器
	STR R1,[R0]
	

	
	;****************************************************
	;初始化时钟        								
	;****************************************************
   	LDR	R0,=LOCKTIME
    LDR	R1,=0xffffff	    ;设置更改锁相环时的锁定时间		
    STR	R1,[R0]
    
    
	;****************************************************
	;设置PLL相关        								
	;****************************************************
    [ PLL_ON_START
	;设置 Fclk:Hclk:Pclk 的分频比值
	LDR	R0,=CLKDIVN
	LDR	R1,=CLKDIV_VAL		; 0=1:1:1, 1=1:1:2, 2=1:2:2, 3=1:2:4, 4=1:4:4, 5=1:4:8, 6=1:3:3, 7=1:3:6.
	STR	R1,[R0]
	
	[ CLKDIV_VAL>1 			;表示Fclk:Hclk 的比值不是 1:1.
	MRC p15,0,R0,c1,c0,0
	ORR R0,R0,#0xc0000000	;R1_nF:OR:R1_iA
	MCR p15,0,R0,c1,c0,0
	|
	MRC p15,0,R0,c1,c0,0
	BIC R0,R0,#0xc0000000	;R1_iA:OR:R1_nF
	MCR p15,0,R0,c1,c0,0
	]

	;配置UPLL
	LDR	R0,=UPLLCON
	LDR	R1,=((U_MDIV<<12)+(U_PDIV<<4)+U_SDIV)  ;Fin = 12MHz,UCLK = 96MHz
	STR	R1,[r0]
	NOP	; Caution: After UPLL setting, at least 7-clocks delay must be inserted for setting hardware be completed.
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	;配置 MPLL
	LDR	R0,=MPLLCON			;PLL频率控制寄存器
	LDR	R1,=((M_MDIV<<12)+(M_PDIV<<4)+M_SDIV)  	;Fin=12MHz,MCLK = 400MHz
	STR	R1,[R0]									;前面设置Fclk:Hclk:Pclk = 1:4:8,故Hclk = 100MHz
    ]											;								  Pclk = 50MHz
    
    BL	InitStacks			;初始化堆栈指针
    						;在本函数中，初始化堆栈过程将使能IRQ和FIQ中断标志


	;****************************************************
	;配置存储器控制寄存器      
	;****************************************************
 	ADRL	R0,SMRDATA	
	LDR	R1,=BWSCON	;装载BWSCON地址到R1
	ADD	R2,R0,#52	;SMRDATA结束地址(Memory控制寄存器总共13个，13*4=52字节)

0
	LDR	R3,[R0],#4	;装载R0指向的地址一字的数据到R3，并且R0自动加4(R0指向SMRDATA)
	STR	R3,[R1],#4	;将R3中的数据存储到R1指向的地址(R1指向Memory Control寄存器)
	CMP	R2,R0		;比较是否已经配置完毕
	BNE	%B0
	
	
	MOV	R0,#&1000	;延时函数
1
	SUBS	R0,R0,#1
	BNE	%B1

    
    
	;****************************************************
	;将OS_CPU_IRQ_ISR地址装入HandleIRQ中,IRQ中断交给uCOS-II
	;管理      
	;****************************************************
    LDR	    R0,=HandleIRQ				;This routine is needed
    LDR	    R1,=OS_CPU_IRQ_ISR			;if there isn't 'subs pc,lr,#4' at 0x18, 0x1c
    STR	    R1,[r0]
    
    
    
	;****************************************************
	;判断是从Nor启动还是从Nand启动
	;判断依据：1.外部引脚OM[1:0]状态
	;				OM[1:0]=00->从Nand启动
	;				OM[1:0]=01->从Nor启动，16位模式
	;				OM[1:0]=10->从Nor启动，32位模式
	;				OM[1:0]=11->保留
	;****************************************************
	LDR		R0, =BWSCON
	LDR		R0, [R0]
	ANDS	R0,R0,#6		;OM[1:0] != 0,从NOR FLash启动
	BNE		NORRoCopy		;如果为非零值则跳转执行从Nor复制代码子程序
	ADR		R0,ResetEntry	;OM[1:0] == 0, 从NAND FLash启动，ADR伪指令汇编结果:SUB R0,PC,#0xXXX;
	CMP		R0,#0			;和上句配合，判断是否使用调试工具直接下载代码到RAM，是则不需复制代码
							;调试模式下，如果复制代码可能出错，因为所编写的程序并未烧写到Flash中，Flash中的数据是不确定的
	BNE		InitRamZero		;如果R0为非零值则执行跳转，不再重新读取Nand数据
    
     
	;****************************************************
	;将程序从nandflash拷贝到sdram
	;NandFlash:K9F2G08,2048块，每块64页，每页2048字节
	;		   共256M字节
	;****************************************************
Nand_boot_beg
	MOV	R5,#NFCONF
	;set timing value
	LDR	R0,=(7<<12) :or: (7<<8) :or: (7<<4)
	STR	R0,[R5]
	;enable control
	LDR	R0,=(0<<13) :or: (0<<12) :or: (0<<10) :or: (0<<9) :or: (0<<8) :or: (1<<6) :or: (1<<5) :or: (1<<4) :or: (1<<1) :or: (1<<0)
	STR	R0,[R5,#4]
	
	BL	ReadNandID
	MOV	R6,#0
	LDR	R0,=0xECF1	;K9F1G08U0A ID号，(EC厂商编号，F1-Device编号)
	CMP	R5,R0		;R5中为ReadNandID返回的FlashID
	BEQ	%F1
	LDR	R0,=0xEC76	;K9F1208U0 ID号，(EC厂商编号，76-Device编号)
	CMP	R5,R0
	BEQ	%F1
	MOV	R6,#1		;标记Nandaddr(寻址周期 0:4  1:5)
				;本处默认不是以上两种Flash则以K9F2G08时序操作
1	
	BL	ReadNandStatus	;返回的状态存放于R1中
	
	MOV	R8,#0			;R8为页地址计数器
	LDR	R9,=ResetEntry
	MOV R10,#32			;+081010 feiling--总共拷贝32页,64KByte
2	
	ANDS	R0,R8,#0x3F	;如果是第一页，则检测坏块(如果R8低6位全为零则R0为0,即第一页)
	BNE		%F3			;如果R0全为0,则Z=1(EQ),不跳转,顺序执行
	MOV		R0,R8		;将当前NandFlash地址指针传给R0
	BL		CheckBadBlk
	CMP		R0,#0		;如果CheckBadBlk返回的R0=0,则说明不是坏块
	ADDNE	R8,R8,#64	;每块的页数
	ADDNE	R10,R10,#64 ;+081010 feiling
	BNE		%F4
3	
	MOV	R0,R8
	MOV	R1,R9			;R9中存放ResetEntry地址，传递给ReadNandPage
	BL	ReadNandPage
	ADD	R9,R9,#2048		;每页的字节数
	ADD	R8,R8,#1		;页数＋1
4	
	CMP	R8,R10  		;要拷贝的页数 081010 pht:#32->R10 
	BCC	%B2
	
	MOV	R5,#NFCONF		;DsNandFlash
	LDR	R0,[R5,#4]
	BIC R0,R0,#1
	STR	R0,[R5,#4]
	LDR	PC,=InitRamZero	;此处跳转到内存空间 LDR 装载数据，寻址灵活。 但不改变PSR
						;要装载一个被存储的‘状态’并正确的恢复它 可以这样写：ldr r0, [base] 换行  moves pc, r0


;=============================================================================================
;若是从Nand启动,则拷贝工作在nand_boot_beg已完成,所以直接跳转到main
;若是从Nor情动,则将RO和RW部分拷贝到RAM,然后跳转到RAM执行
;注：未考虑直接在Nor中运行方式
;=============================================================================================
NORRoCopy	
	;bl	ClearSdram

	ADR	R0,ResetEntry		;判断当前程序是否在RAM中运行,若在RAM中运行，R0=PC-0xXXX=BaseOfROM=ARM Linker中指定的地址
							;若在ROM中运行，R0=PC-0xXXX=0,(此句为伪指令)
	LDR	R2,BaseOfROM		;如果是则跳转到RwCopy,否则将程序拷贝到RAM中 
	CMP	R0,R2
	BEQ	NORRwCopy			;相等说明程序代码已经在RAM中，只需初始化数据区域和零区域即可	
	LDR R3,TopOfROM			;不相等的话，说明程序由0x0000_0000地址考试执行，拷贝代码到RAM中
0							
	LDMIA	R0!,{R4-R7}
	STMIA	R2!,{R4-R7}
	CMP		R2, R3
	BCC		%B0
	
		
NORRwCopy	
	LDR	R0,TopOfROM
	LDR R1,BaseOfROM
	SUB R0,R0,R1			;TopOfROM-BaseOfROM得到从0开始RW的偏移地址(RW段在ROM中的起始地址)
	LDR	R2,BaseOfBSS		;将RW部分的数据从ROM拷贝到RAM
	LDR	R3,BaseOfZero	
0
	CMP		R2,R3
	LDRCC	R1,[R0],#4
	STRCC	R1,[R2],#4
	BCC		%B0	
	
	B	InitRamZero	


    IMPORT  App_Main


CEntry
	B		App_Main


	;****************************************************
	;初始化各种工作模式下的堆栈指针     
	;Don t use DRAM,such as stmfd,ldmfd......
	;SVCstack is initialized before
	;Under toolkit ver 2.5, 'msr cpsr,r1' can be used instead of 'msr cpsr_cxsf,r1'
	;****************************************************
InitStacks
	MRS	R0,CPSR
	BIC	R0,R0,#MODEMASK
	ORR	R1,R0,#UNDEFMODE :or: NOINT
	MSR	cpsr_cxsf,R1		;未定义模式，UndefMode
	LDR	SP,=UndefStack		;UndefStack=0x33FF_5C00

	ORR	R1,R0,#ABORTMODE :or: NOINT
	MSR	cpsr_cxsf,R1		;中止模式，AbortMode
	LDR	SP,=AbortStack		;AbortStack=0x33FF_6000

	ORR	R1,R0,#IRQMODE :or: NOINT
	MSR	cpsr_cxsf,R1		;中断模式，IRQMode
	LDR	SP,=IRQStack		;IRQStack=0x33FF_7000

	ORR	R1,R0,#FIQMODE :or: NOINT
	MSR	cpsr_cxsf,R1		;快速中断模式，FIQMode
	LDR	SP,=FIQStack		;FIQStack=0x33FF_8000

	BIC	R0,R0,#MODEMASK :or: NOINT
	ORR	R1,R0,#SVCMODE
	MSR	cpsr_cxsf,R1		;管理模式，SVCMode
	LDR	SP,=SVCStack		;SVCStack=0x33FF_5800

	;用户模式堆栈指针还没有被初始化

	BX	LR
	;The LR register won t be valid if the current mode is not SVC mode.
	
	;****************************************************
	;读取nand flash ID
	;****************************************************
ReadNandID
	MOV      R7,#NFCONF
	LDR      R0,[R7,#4]		;NFChipEn();
	BIC      R0,R0,#2
	STR      R0,[R7,#4]
	MOV      R0,#0x90		;WrNFCmd(RdIDCMD);
	STRB     R0,[R7,#8]
	MOV      R4,#0			;WrNFAddr(0);
	STRB     R4,[R7,#0xC]
1							;while(NFIsBusy());
	LDR      R0,[R7,#0x20]
	TST      R0,#1
	BEQ      %B1
	LDRB     R0,[R7,#0x10]	;id  = RdNFDat()<<8;
	MOV      R0,R0,LSL #8
	LDRB     R1,[R7,#0x10]	;id |= RdNFDat();
	ORR      R5,R1,R0		;16位FlashID存放在R5中
	LDR      R0,[R7,#4]		;NFChipDs();
	ORR      R0,R0,#2
	STR      R0,[R7,#4]
	BX		 LR	
	

	;****************************************************
	;读取nandflash状态
	;****************************************************
ReadNandStatus
	MOV		 R7,#NFCONF
	LDR      R0,[R7,#4]		;NFChipEn();
	BIC      R0,R0,#2
	STR      R0,[R7,#4]
	MOV      R0,#0x70		;WrNFCmd(QUERYCMD);
	STRB     R0,[R7,#8]	
	LDRB     R1,[R7,#0x10]	;R1 = RdNFDat(); R1中存放NF状态
	LDR      R0,[R7,#4]		;NFChipDs();
	ORR      R0,R0,#2
	STR      R0,[R7,#4]
	BX		 LR

	;****************************************************
	;查看nandflash是否是在忙状态
	;70H,NandFalsh读状态命令
	;****************************************************
WaitNandBusy
	MOV      R0,#0x70		;WrNFCmd(QUERYCMD);
	MOV      R1,#NFCONF
	STRB     R0,[R1,#8]		;NFCMMD = 70H;发送读命令到2440命令寄存器
1							;while(!(RdNFDat()&0x40));	
	LDRB     R0,[R1,#0x10]
	TST      R0,#0x40
	BEQ		 %B1
	MOV      R0,#0			;WrNFCmd(READCMD0);
	STRB     R0,[R1,#8]
	BX       LR

	;****************************************************
	;检查nandflash坏块
	;出口参数：R0，R0为零则不是坏块，否则是坏块
	;****************************************************
CheckBadBlk
	MOV		R7,LR
	MOV		R5,#NFCONF
	
	BIC     R0,R0,#0x3F		;addr &= ~0x3f;
	LDR     R1,[R5,#4]		;NFChipEn()
	BIC     R1,R1,#2
	STR     R1,[R5,#4]		;NFCONT &= ~(1<<2);即Reg_nCE=0(使能NandFlash); 

	MOV     R1,#0x00		;WrNFCmd(READCMD)
	STRB    R1,[R5,#8]
	MOV     R1,#0			;2048&0xff
	STRB    R1,[R5,#0xC]	;WrNFAddr(2048&0xff);
	MOV     R1, #8			;(2048>>8)&0xf
	STRB    R1,[R5,#0xC]
	
	STRB    R0,[R5,#0xC]	;WrNFAddr(addr)
	MOV     R1,R0,LSR #8	;WrNFAddr(addr>>8)
	STRB    R1,[R5,#0xC]

	;=============================================
	;为2G08-NandFlash添加，2010/8/5
	CMP      R6,#0			;if(NandAddr)
	MOVNE    R1,R0,LSR #16	;WrNFAddr(addr>>16)
	STRNEB     R1,[R5,#0xC]
	;=============================================
	MOV     R1,#0x30		;WrNFCmd(0x30)
	STRB    R1,[R5,#8]
		
;	CMP     R6,#0			;if(NandAddr)		
;	MOVNE   R0,R0,LSR #16	;WrNFAddr(addr>>16)
;	STRNEB  R0,[R5,#0xC]
	
;	BL		WaitNandBusy	;WaitNFBusy()
	;don t use WaitNandBusy, after WaitNandBusy will read part A!
	
	MOV	R0, #100			;延时一段时间
1
	SUBS	R0,R0,#1
	BNE	%B1
2
	LDR	R0,[R5,#0x20]		;NandFlash状态寄存器
	TST	R0, #1				;NFSTAT的最低位RnB=1则NandFlash准备好,否则忙状态
	BEQ	%B2	

	LDRB	R0,[R5,#0x10]	;RdNFDat()
	SUB		R0,R0,#0xFF		;将读取的数据减0xFF,并将结果返回
		
	LDR     R1,[R5,#4]		;NFChipDs()
	ORR     R1,R1,#2
	STR     R1,[R5,#4]		;NFCONT |= (1<<2);即Reg_nCE=1(禁止NandFlash);
	
	BX		R7


	;****************************************************
	;读取nandflash一页数据
	;入口参数:R0:需要读取数据的页地址指针
	;		  R1:读取数据存放到SRAM的目标地址指针
	;****************************************************	
ReadNandPage
	MOV		 R7,LR
	MOV      R4,R1
	MOV      R5,#NFCONF

	LDR      R1,[R5,#4]		;NFChipEn()
	BIC      R1,R1,#2
	STR      R1,[R5,#4]	

	MOV      R1,#0			;WrNFCmd(READCMD0)
	STRB     R1,[R5,#8]	
	STRB     R1,[R5,#0xC]	;WrNFAddr(0),发送页内指针第8位
	STRB     R1,[R5,#0xC]	;WrNFAddr(0),发送页内指针高8位	
	STRB     R0,[R5,#0xC]	;WrNFAddr(addr),发送页指针第8位
	MOV      R1,R0,LSR #8	;WrNFAddr(addr>>8),发送页指针高8位
	STRB     R1,[R5,#0xC]	
	
	;=============================================
	;为2G08添加-2010/8/5
	CMP      R6,#0			;if(NandAddr)
	MOVNE    R1,R0,LSR #16	;WrNFAddr(addr>>16)
	STRNEB     R1,[R5,#0xC]	
	;=============================================	

	MOV      R1,#0x30		;WrNFCmd(0x30)
	STRB     R1,[R5,#8]		;发送读命令到NFCMMD
		
;	CMP      R6,#0			;if(NandAddr)		
;	MOVNE    R0,R0,LSR #16	;WrNFAddr(addr>>16)
;	STRNEB   R0,[R5,#0xC]
	
	LDR      R0,[R5,#4]		;InitEcc()
	ORR      R0,R0,#0x10
	STR      R0,[R5,#4]		;NFCONT |= 1<<4;即InitECC=1;初始化ECC
	
	BL       WaitNandBusy	;WaitNFBusy()
	
	MOV     R0,#0			;for(i=0; i<2048; i++)
1
	LDRB     R1,[R5,#0x10]	;buf[i] = RdNFDat()
	STRB     R1,[R4,R0]
	ADD      R0,R0,#1
	BIC      R0,R0,#0x10000	;?--------是个问题-------------------------------------------------
	CMP      R0,#0x800
	BCC      %B1			;(CC无符号小于,C=0)
	 
	LDR      R0,[R5,#4]		;NFChipDs()
	ORR      R0,R0,#2
	STR      R0,[R5,#4]
		
	BX		 R7



	;****************************************************
	;初始化SDRAM零数据段
	;****************************************************	
InitRamZero
	MOV	R0,#0
	LDR R2,BaseOfZero
	LDR	R3,EndOfBSS
1	
	CMP		R2,R3			;初始化Zero部分 不管从哪里启动，这部分都需要执行
	STRCC	R0,[R2],#4
	BCC		%B1
	
	LDR	PC,=CEntry			;goto compiler address
	
	
SMRDATA DATA
	;内存配置应为最佳的性能优化
	;下面的参数未优化。
	;存储器访问周期参数战略
	; 1）内存设置安全参数，即使在HCLK = 100MHz的。
	; 2）SDRAM的刷新周期为HCLK <= 100MHz的.

	DCD (0+(B1_BWSCON<<4)+(B2_BWSCON<<8)+(B3_BWSCON<<12)+(B4_BWSCON<<16)+(B5_BWSCON<<20)+(B6_BWSCON<<24)+(B7_BWSCON<<28))
	DCD ((B0_Tacs<<13)+(B0_Tcos<<11)+(B0_Tacc<<8)+(B0_Tcoh<<6)+(B0_Tah<<4)+(B0_Tacp<<2)+(B0_PMC))   ;GCS0
	DCD ((B1_Tacs<<13)+(B1_Tcos<<11)+(B1_Tacc<<8)+(B1_Tcoh<<6)+(B1_Tah<<4)+(B1_Tacp<<2)+(B1_PMC))   ;GCS1
	DCD ((B2_Tacs<<13)+(B2_Tcos<<11)+(B2_Tacc<<8)+(B2_Tcoh<<6)+(B2_Tah<<4)+(B2_Tacp<<2)+(B2_PMC))   ;GCS2
	DCD ((B3_Tacs<<13)+(B3_Tcos<<11)+(B3_Tacc<<8)+(B3_Tcoh<<6)+(B3_Tah<<4)+(B3_Tacp<<2)+(B3_PMC))   ;GCS3
	DCD ((B4_Tacs<<13)+(B4_Tcos<<11)+(B4_Tacc<<8)+(B4_Tcoh<<6)+(B4_Tah<<4)+(B4_Tacp<<2)+(B4_PMC))   ;GCS4
	DCD ((B5_Tacs<<13)+(B5_Tcos<<11)+(B5_Tacc<<8)+(B5_Tcoh<<6)+(B5_Tah<<4)+(B5_Tacp<<2)+(B5_PMC))   ;GCS5
	DCD ((B6_MT<<15)+(B6_Trcd<<2)+(B6_SCAN))    ;GCS6
	DCD ((B7_MT<<15)+(B7_Trcd<<2)+(B7_SCAN))    ;GCS7
	DCD ((REFEN<<23)+(TREFMD<<22)+(Trp<<20)+(Tsrc<<18)+(Tchr<<16)+REFCNT)

	DCD 0x32	    ;SCLK power saving mode, BANKSIZE 128M/128M,FL2440实际SRAM大小可以是64M，因为两片SRAM挂接在同一BANK上，这里可以直接设置为128M,只要使用时注意不要超过实际地址即可
	;DCD 0x02	    ;SCLK power saving disable, BANKSIZE 128M/128M

	DCD 0x20	    ;MRSR6 CL=2clk
	DCD 0x20	    ;MRSR7 CL=2clk
	
	
BaseOfROM	DCD	|Image$$RO$$Base|
TopOfROM	DCD	|Image$$RO$$Limit|
BaseOfBSS	DCD	|Image$$RW$$Base|
BaseOfZero	DCD	|Image$$ZI$$Base|
EndOfBSS	DCD	|Image$$ZI$$Limit|

		
	
	;****************************************************
	;建立RAM中的中断向量表
	;****************************************************
	AREA RamData, DATA, READWRITE
	
	
	^ _ISR_STARTADDRESS		;_ISR_STARTADDRESS=0x33FF_FF00
HandleReset 	#   4
HandleUndef 	#   4
HandleSWI		#   4
HandlePabort    #   4
HandleDabort    #   4
HandleReserved  #   4
HandleIRQ		#   4
HandleFIQ		#   4

;Don t use the label 'IntVectorTable',
;The value of IntVectorTable is different with the address you think it may be.
;IntVectorTable
;@0x33FF_FF20
HandleEINT0		#   4
HandleEINT1		#   4
HandleEINT2		#   4
HandleEINT3		#   4
HandleEINT4_7	#   4
HandleEINT8_23	#   4
HandleCAM		#   4		; Added for 2440.
HandleBATFLT	#   4
HandleTICK		#   4
HandleWDT		#   4
HandleTIMER0 	#   4
HandleTIMER1 	#   4
HandleTIMER2 	#   4
HandleTIMER3 	#   4
HandleTIMER4 	#   4
HandleUART2  	#   4
;@0x33FF_FF60
HandleLCD 		#   4
HandleDMA0		#   4
HandleDMA1		#   4
HandleDMA2		#   4
HandleDMA3		#   4
HandleMMC		#   4
HandleSPI0		#   4
HandleUART1		#   4
HandleNFCON		#   4		; Added for 2440.
HandleUSBD		#   4
HandleUSBH		#   4
HandleIIC		#   4
HandleUART0 	#   4
HandleSPI1 		#   4
HandleRTC 		#   4
HandleADC 		#   4
;@0x33FF_FFA0

    END