;******************************************************************************
;* FILENAME                                                                   *
;*   anf.asm      				                                              *
;*                                                                            *
;*                                                                            *
;*----------------------------------------------------------------------------*
;*                                                                            *
;*  The rate of convergence of the filter is determined by MU                 *
;*                                                                            *
;******************************************************************************
;/*

	 .mmregs

MU .set  500
LAMBDA .set	32400	;0.763*2^15
LAMBDA_MIN_ONE .set 368

; Functions callable from C code

	.sect	".text"
	.global	_anf

;*******************************************************************************
;* FUNCTION DEFINITION: _anf_asm		                           *
;*******************************************************************************
;int anf(int v, int *X , int *A, int *rho, unsigned int* index);
; v: 		T0
; X: 		AR0 		--> extended AR0 register
; A: 		AR1			--> extended AR1 register
; rho: 		AR2		--> extended AR2 register
; index: 	AR3		--> extended AR3 register
;
; T0: output argument (e in this case) must be stored in T0 at the end of code

_anf:

		PSH  mmap(ST0_55)	; Keep original values of flags
		PSH  mmap(ST1_55)
		PSH  mmap(ST2_55)

		mov   #0,mmap(ST0_55)      ; Clear all fields (OVx, C, TCx)
    		or    #4100h,mmap(ST1_55)  ; Set CPL (bit 14), SXMD (bit 8), Note that FRCT is not set since here we are not only multiplying Q15 formats								;
    		and   #07940h,mmap(ST1_55)      ; Clear BRAF, M40, SATD, C16, 54CM, ASM
    		bclr  ARMS                      ; Disable ARMS bit 15 in ST2_55


		;MAKE CIRCULAR BUFFER X-VALUES
		mov 	mmap(AR0), 	BSA01		 	; set base address buffer for X
		mov  	#3, 				BK03			; set the size of the buffer to 3
		bset	AR0LC									; make AR0 a circular buffer
		mov 	*AR3, 			AR0				; make the circular buffer start at index

		;CALCULATE rho
		amov 	LAMBDA, T1;
		amov	LAMBDA_MIN_ONE, T2
		mpym	*AR2,	T1,	AC2					; lambda*rho(t-1) -> AC2 Q30
		macm	*AR2(1), T2, AC2, AC3; (1-lambda)*rho(inf) + lambda*rho(t-1) -> AC2 Q30  make (1)?
		sfts 	AC3, #1, AC3				; shift to Q31
		mov		AC3<<#-16, *AR2						; push rho back to address

		;CALCULATE rho^2, PUT IN AC2
		sqr		AC3, AC2							; make AC2 the square of rho
		sfts 	AC2, #1, AC2					; shift to Q31

		;CALCULATE X
		mpym 	*AR2, 	*AR0+, 	AC0			; rho*x(t-1) Q15*Q11 = Q26 --> post 1 is added to AR0L
		sfts 	AC0, 	#2, 	AC0			; extend to Q28
		mpym	*AR1, 	AC0, 	AC1			; a*rho*x(t-1)	Q26
		masm	*AR0+,	AC2,	AC1			; a*rho*x(t-1) - x(t-2)*rho^2 Q26
		sfts	AC1, 	#-15,	AC1			; make Q11
		mov		T0, AC2			 			; The content of T0 is copied to AC0
		add		AC2<<#-4, AC1
		mov		AC1, 	*AR0 :: mov AR0, *AR3	; move X to buffer, and index back to index again

		;CALCULATE OUTPUT E
		mov		*AR0+,	AC0						; put x(t) in AC0
		mpym 	*AR0+,	*AR1, 	AC1				; x(t-1)*a	Q25
		sub		AC1<<#-14, 	AC0					; x(t) - x(t-1)*a Q11
		add 	*AR0+,	AC0						; x(t-2) + x(t) - a*x(t-1) Q11
		mov 	AC0, T0							; store output back in T0

		;CALCULATE A
		sfts 	AC0, #16, AC0					; make output to LSB
		mpym	*+AR0, AC0, AC1					; x(t-1)*e Q22
		sfts	AC1, #8, AC1					; Q30
		amov	MU, T1
		mpy		T1, AC1, AC0					; 2*mu*x(t-1)*e Q29
		sfts	AC0, #-15, AC0					; Q14
		add 	*AR1, AC0, T1					; a(t-1) + 2*mu*x(t-1)*e
		mov 	T1, *AR1 						; store a in AR1


		POP 	mmap(ST2_55)
		POP	mmap(ST1_55)	; Restore FRCT, SXMD, SATD, M_40, C
		POP	mmap(ST0_55)

		RET			; Return with result in AC0


;*******************************************************************************
;* End of anf.asm                                              				   *
;*******************************************************************************
