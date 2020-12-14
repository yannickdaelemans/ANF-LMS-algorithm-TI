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

MU .set  200

; Functions callable from C code

	.sect	".text"
	.global	_anf

;*******************************************************************************
;* FUNCTION DEFINITION: _anf_asm		                           *
;*******************************************************************************
;int anf(int v, int *X , int *A, int *rho, unsigned int* index);
; v: ?
; X: ?
; A: ?
; rho: ? 
; index: ?
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
    		

		;add your own code here

		POP 	mmap(ST2_55)
		POP	mmap(ST1_55)	; Restore FRCT, SXMD, SATD, M_40, C
		POP	mmap(ST0_55)
                               
		RET			; Return with result in AC0
    

;*******************************************************************************
;* End of anf.asm                                              				   *
;*******************************************************************************
