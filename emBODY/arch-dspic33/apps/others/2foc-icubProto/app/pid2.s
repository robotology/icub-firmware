/*
 *  PID implementation
 *
 *  - With I anti-windup feature
 *  - With anti-overflow saturation feature
 *  - Without Excess (Kc)
 *  - With DSP context (CORCON, ACCx) save & restore
 *
 *  Based on Microchip PI and PID implementations.
 *  Written by AM for 2FOC board.
 *
 * **********************************************
 *
 *  Algo pseudocode:
 *
 *  D = (Err-Preverr) * Kd;
 *  I += Err * Ki;
 *  Clamp(I,I_max);
 *  Out = Kp*(2^KNo) * Err + I + D;
 *
 * ***********************************************
 *
 *  Coefficients in coefficient array are 
 *                  ---------------------------- 
 *    coeff array: |  Ki  |  Kd  |  Kp  | I_max |
 *                  ---------------------------- 
 *          index:	   0      1      2      3
 *
 * ***********************************************
 *
 * Performances: One iteration about 2us 
 *
 */
      

; Local inclusions.
        .nolist
        .include        "PID2.inc"         ; fractsetup
        .list

        .equ    offsetIDPMCoefficients, 0
        .equ    offsetState, 2
        .equ    offsetControlOutput, 4
        .equ    offsetMeasuredOutput, 6
        .equ    offsetControlReference, 8

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .section  .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; _PID:
; Prototype:
;              tPID2 PID ( tPID *fooPIDStruct )


        .global _PID2                    ; provide global scope to routine
_PID2:

        ; Save working registers.
		push	w6	
        push    w8
		push    w9
        push    w10
        push    CORCON                  ; Prepare CORCON for fractional computation.
; save DSP accumulator A to the stack
        mov ACCAL,w8				
        push.w w8					
        mov ACCAH,w8				
        push.w w8					
        mov ACCAU,w8				
        push.w w8
; save DSP accumulator B to the stack
        mov ACCBL,w8				
        push.w w8					
        mov ACCBH,w8				
        push.w w8					
        mov ACCBU,w8				
        push.w w8

       fractsetup      w8

        mov [w0 + #offsetIDPMCoefficients], w8    ; w8 = Base Address of Coefficients array [ I,D,P,MAX_I ]
        mov [w0 + #offsetState], w10    ; w10 = Address of state array (D prev err and I)
        mov [w0 + #offsetMeasuredOutput], w2
        mov [w0 + #offsetControlReference], w3

        lac     w3, a                   ; A = tPID.controlReference
        lac     w2, b                   ; B = tPID.MeasuredOutput
        sub     a                       ; A = tPID.controlReference - tPID.measuredOutput
        sac.r   a, w5                   ; w5 = Sat(Rnd(A))

		lac 	[w10], b				; B = tPID.State (preverr)
        lac		w5, a					; A = error (sat & RND)

		mov.w   w5,[w10]				; tPID.State (preverr) error (sat & RND)

		sub		b						; B = error - preverr

		mov.w	[w8+2],w4				; w4 = kd
		
		sac.r 	b, w6					; w2 = error - preverr

		mpy 	w4*w6, b, [w8]+=4, w4, [w10]+=2, w7 ; b = DERIVATIVE, w4 = ki, w10 pt to tPID.State (I), w8 point to kp

		lac 	[w10], a				; A = tPID.State (I)
		mac 	w4*w5, a, [w8]+=2, w4   ; A += e*ki, w4 = kp, w8 point to IMAX
		   
		sac.r	a,w1					; w1 = integral;
		mov.w 	[w8],w6					; load IMAX
		cp 		w6,w1					; integral-max 					
		bra		gt,	notok				; > 0 ?
		neg.w 	w6,w6					; max = -max
		cp		w1,w6					; integral - (-max)
		bra 	lt, notok				; < 0 ?

ok:
		mov.w	w1,[w10]				; store I
		lac 	w1,a					; A = I
		add		b						; B = A + B

		mpy		w4*w5,a					; A = kp*error
		sftac	a,#-NKo					; A = A << NKo
		add		a 						; A = A + B
		sac.r	a, w1

		mov.w   w1,[w0 + #offsetControlOutput]				; OUT = A

  
;Restore DSP accumulator B from the stack

        pop.w w8
        mov w8,ACCBU
        pop.w w8
        mov w8,ACCBH
        pop.w w8
        mov w8,ACCBL
;restore DSP accumulator A from the stack.
        pop.w w8
        mov w8,ACCAU 
        pop.w w8
        mov w8,ACCAH 
        pop.w w8
        mov w8,ACCAL
       
        pop     CORCON                  ; restore CORCON.
        pop     w10                     ; Restore working registers.
		pop     w9
        pop     w8

		pop		w6
        return

notok:
		mov.w	w6,w9
		bra ok;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; _PIDInit:
;
; Prototype:
; void PIDInit ( tPID *fooPIDStruct )
;
; This initializes the tPID2 structure clearing current state (D prev err and I)
; and current Output value
;.........................................................
        .global _PID2Init                ; provide global scope to routine
_PID2Init:
        push    w0
        add  #offsetControlOutput, w0   ; Set up pointer for controlOutput
	    clr  [w0]                       ; Clear controlOutput
	    pop  w0
	    push w0
                                        ;Set up pointer to the base of
                                        ;controlHistory variables within tPID
        mov     [w0 + #offsetState], w0
                                        ; Clear controlHistory variables
                                        ; within tPID
        clr     [w0++]                  ; State[0]=0
        clr     [w0]                    ; State[1] = 0
        pop     w0                      ;Restore pointer to base of tPID
        return




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OEF

