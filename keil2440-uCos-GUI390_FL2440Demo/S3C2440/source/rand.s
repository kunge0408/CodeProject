	AREA	|subr|, CODE, READONLY

	EXPORT  randxxx
randxxx
; on exit:
;	a1 = low 32-bits of pseudo-random number
;   a2 = high bit (if you want to know it)
    LDR     ip, |seedpointer|
    LDMIA   ip, {a1, a2}
    TST     a2, a2, LSR#1           ; to bit into carry
    MOVS    a3, a1, RRX             ; 33-bit rotate right
    ADC     a2, a2, a2              ; carry into LSB of a2
	ADC     a2, a2, a2              ; carry into LSB of a2
    EOR     a3, a3, a1, LSL#12      ; (involved!)
    EOR     a1, a3, a3, LSR#20      ; (similarly involved!)
    STMIA   ip, {a1, a2}

    MOV     pc, lr

|seedpointer|
	DCD     seed

    AREA    |Random$$data|, DATA

    EXPORT  seed
seed
	DCD     &55555555
    DCD     &55555555     
       
    END