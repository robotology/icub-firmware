/*
 *  I2T implementation
 *  DSP ASM has been used to exploit more than 16bits
 *
 *  Written by AM for 2FOC board.
 *
 */


/****************************************************
 * implementation 1
 * This requires CORCON to be set with
 * - Fractional signed mode
 * - saturation enabled
 ****************************************************
 *  Formula E= id*id + iq*iq - IMAXNOMINAL*IMAXNOMINAL 
 *
 *  Algo pseudocode:
 *
 *  I2Q = Iq*Iq
 *  I2D = Id*Id
 *  N2 = IMAXNOMINAL*IMAXNOMINAL
 *
 *  Excess =  I2Q + I2D - N2; 
 *
 *  Temp += Excess
 *
 *  if(Temp < 0){
 *	  Temp = 0;
 *  }
 *
 *  if(Temp > ITHRESHOLD){
 *     PROTECTION!
 *  }
 *
 *  Where IMAXNOMINAL is the current that is
 *  considered safe to flow continuosly for
 *  an unlimited amount of time
 *
 *  Where Temp is persistent from one iteration to
 *  next iteration
 *
 * ***********************************************
 * TODO: probably it is possible to optimize
 * ***********************************************
 *
 * Performances: One iteration about TODO
 *
 */
      

; Local inclusions.
        .nolist
        .include        "I2T.inc"         ; fractsetup
        .list

        .equ    OffsetNominal, 0
        .equ    OffsetK,0
        .equ    OffsetThreshold, 2
        .equ    OffsetACCAL, 4
        .equ    OffsetACCAH, 6
        .equ    OffsetACCAU, 8
        .equ    OffsetQMeasured, 10
        .equ    OffsetDMeasured, 12

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .section  .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; _I2TIntegral:
; Prototype:
;              int I2TIntegral ( tI2T *fooI2TStruct )



        .global _I2TIntegral                    ; provide global scope to routine
_I2TIntegral:

        ; Save working registers.
		push    w8;
;        push    CORCON                  ; Prepare CORCON for fractional computation.
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

 ;       fractsetup      w8;

        mov.w [w0 + #OffsetACCAL], w1; load old val 
        mov.w [w0 + #OffsetACCAH], w2; 
        mov.w [w0 + #OffsetACCAU], w3;

        mov.w [w0 + #OffsetQMeasured], w4;  load Q

       
        mov.w [w0 + #OffsetNominal], w5 ; load max I nominal
;		sub  w4,w8,w4; w4 = w4 - IMAX

;		mov.w	w4,w5;    copy q for square

;		btsc    w4,#15;   preserve sign: if it is neg then negate w4
;		neg     w4,w4;

        mpy     w4*w4,b ;b = q*q

		mov.w [w0 + #OffsetDMeasured], w4 ; load measured D

 ;      	sub  w4,w8,w4; w4 = w4 - IMAX

;		mov.w	w4,w5;  copy D for square
;		btsc    w4,#15; preserve sign: if it is neg then negate w4
;		neg     w4,w4;
        
        mac     w4*w4,b;  b += d*d

		mpy     w5*w5,a;
		sub     b;

		sftac   b,#16 ; take most significant 16 bits.


	
		mov.w   w1,ACCAL  ;restore old val
		mov.w   w2,ACCAH;
		mov.w   w3,ACCAU;
 	
        add     a ; old I += new I

		sac.r   a,w1;    store ACCA with saturation in w1
		btsc    w1,#15;  check sign
		clr		a;       clear A if it was <0

		mov.w   ACCAL,w1  ; extract new val
		mov.w   ACCAH,w2
		mov.w   ACCAU,w3
 		
		mov.w     w1, [w0 + #OffsetACCAL]; store new val 
        mov.w     w2, [w0 + #OffsetACCAH]; 
        mov.w     w3, [w0 + #OffsetACCAU];
		
		mov.w    [w0 + #OffsetThreshold], w1 ; load threshold

		lac     w1,b;
		sub     b;                      ; accb = accb - acca

		sac.r   b,w1                    ; extract b with saturation
	    mov.w	#0,w0;                  ; default to NOT protection
        btsc    w1,#15;                 ; check sign	
		mov.w	#1,w0;					; protection!   


;		mpy 	w4*w6, b, [w8]+=4, w4, [w10]+=2, w7 ; b = DERIVATIVE, w4 = ki, w10 pt to tPID.State (I), w8 point to kp
;		mac 	w4*w5, a, [w8]+=2, w4   ; A += e*ki, w4 = kp, w8 point to IMAX		   

  
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
       
;        pop     CORCON                  ; restore CORCON. 
        pop     w8

       return




/****************************************************
 * implementation 2
 ****************************************************
 *  Formula: Out += (QI*QI+Qd*QD+Out)*K
 *
 *  Algo pseudocode:
 *
 *  I2Q = Iq*Iq
 *  I2D = Id*Id
 * 
 *  B = I2Q+I2D
 *  B -= A
 *  Normalize(B) -> temp
 *  B = temp * K2048
 *  UnNormalize(B);
 *  B /= 2048;
 *
 *  A += B
 *
 *  if( A - ITHRESHOLD > 0){
 *     PROTECTION!
 *  }
 *
 *
 *  Where A is persistent from one iteration to
 *  next iteration
 *
 *  Where K2048 is K * 1024
 *  K = 1-(e^(-tc/tau))
 *
 *  3*tau is time to reach about 90%
 *  2*tau is time to reach about 87%
 *  tau is time to reach about 63%
 *
 * ***********************************************
 * TODO: probably it is possible to optimize
 * ***********************************************
 *
 * Performances: One iteration about TODO
 *
 */



        .global _I2TFilter                    ; provide global scope to routine
_I2TFilter:

        ; Save working registers.
		push    w8;
      ;  push    CORCON                  ; Prepare CORCON for fractional computation.
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

;        fractsetup      w8;

        mov.w [w0 + #OffsetACCAL], w1; load old val 
        mov.w [w0 + #OffsetACCAH], w2; 
        mov.w [w0 + #OffsetACCAU], w3;
    	mov.w   w1,ACCAL  ;restore old val
		mov.w   w2,ACCAH;
		mov.w   w3,ACCAU;   a is last output

        mov.w [w0 + #OffsetQMeasured], w4;  load Q
      
        mpy     w4*w4,b ;    b = q*q

		mov.w [w0 + #OffsetDMeasured], w4 ; load measured D

        
        mac     w4*w4,b;  b += d*d
		sub     b; b = b-a

		MOV #ACCBH, W5 ; W5 points to ACCAH
		BRA	OA, FBCL_GUARD ; if overflow we right shift
FBCL_HI:
		FBCL [W5], W1 ; extract exponent for left shift
		BRA SHIFT_ACC ; branch to the shift
FBCL_GUARD:
		FBCL [++W5], W1 ; extract exponent for right shift
	;	ADD.B W1, #15, W1 ; adjust the sign for right shift
        neg w1,w1;
SHIFT_ACC:
		SFTAC B, W1 ; shift ACCA to normalize

		sac.r   b,w5
 
        mov.w [w0 + #OffsetK], w4 ; load K (was k*2048)
        
		mpy     w5*w4,b ; b = b*k 
        sftac   b,#11 ; divide by 2048 for the K 

        ; restore magnitude after normalization
        neg     w1,w1;
        sftac   b, w1;
	 	
        add     a ; old I += new I

		mov.w   ACCAL,w1  ; extract new val
		mov.w   ACCAH,w2
		mov.w   ACCAU,w3
 		
		mov.w     w1, [w0 + #OffsetACCAL]; store new val 
        mov.w     w2, [w0 + #OffsetACCAH]; 
        mov.w     w3, [w0 + #OffsetACCAU];
		
		mov.w     [w0 + #OffsetThreshold], w4 ; load threshold

		mpy     w4*w4,b;
		sub     b;                      ; accb = accb - acca

		sac.r   b,w1                    ; extract b with saturation
	    mov.w	#0,w0;                  ; default to NOT protection
        btsc    w1,#15;                 ; check sign	
		mov.w	#1,w0;					; protection!   


;		mpy 	w4*w6, b, [w8]+=4, w4, [w10]+=2, w7 ; b = DERIVATIVE, w4 = ki, w10 pt to tPID.State (I), w8 point to kp
;		mac 	w4*w5, a, [w8]+=2, w4   ; A += e*ki, w4 = kp, w8 point to IMAX		   

  
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
       
 ;       pop     CORCON                  ; restore CORCON. 
        pop     w8

       return





/*
 *  I2T implementation 3
 ***********************************************
 *  DSP ASM has been used to exploit more than 16bits
 *
 *  Written by AM for 2FOC board.
 *
 * **********************************************
 *
 *  Algo pseudocode:
 *
 *  Excess = Iread - IMAXNOMINAL;
 *
 *  Temp += sign(Excess)*Excess*Excess; 
 *
 *  if(Temp < 0){
 *	  Temp = 0;
 *  }
 *
 *  if(Temp > ITHRESHOLD){
 *     PROTECTION!
 *  }
 *
 *  where IMAXNOMINAL is the current that is
 *  considered safe to flow continuosly for
 *  an unlimited amount of time
 *
 * ***********************************************
 * TODO: probably it is possible to optimize
 * ***********************************************
 *
 * Performances: One iteration about 2 us
 *
 */
      


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .section  .text

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; _I2T:
; Prototype:
;              int I2T ( tI2T *fooI2TStruct )



        .global _I2TIntegral2                    ; provide global scope to routine
_I2TIntegral2:

        ; Save working registers.
		push    w8;
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

        fractsetup      w8;

        mov.w [w0 + #OffsetACCAL], w1; load old val 
        mov.w [w0 + #OffsetACCAH], w2; 
        mov.w [w0 + #OffsetACCAU], w3;

        mov.w [w0 + #OffsetQMeasured], w4;  load Q

		mov.w   w1,ACCAL  ;restore old val
		mov.w   w2,ACCAH;
		mov.w   w3,ACCAU;
        
        mov [w0 + #OffsetNominal], w8 ; load max I nominal
		sub  w4,w8,w4; w4 = w4 - IMAX

		mov.w	w4,w5;    copy q for square

;		btsc    w4,#15;   preserve sign: if it is neg then negate w4
;		neg     w4,w4;  DO NOT DO HERE BECAUSE -0x8000 is 0x8000

        mpy     w4*w5,b ;b = (q*sign(q))*q

		btsc    w4,#15;   preserve sign: if it was neg then negate result
		neg     b;

		mov.w [w0 + #OffsetDMeasured], w4 ; load measured D

       	sub  w4,w8,w4; w4 = w4 - IMAX

		mov.w	w4,w5;  copy D for square
		btss    w4,#15; preserve sign: if it is neg then negate w4
		bra     signok
		mov.w		#0x8000,w1
		cp		w1,w4;
		bra		nz,okneg;
		mov.w		#0x8001,w4
okneg:
		neg     w4,w4;
        
signok:
        mac     w4*w5,b;  b += (d*sign(d))*d

		sftac   b,#16 ; take most significant 16 bits.
		
        add     a ; old I += new I

		sac.r   a,w1;    store ACCA with saturation in w1
		btsc    w1,#15;  check sign
		clr		a;       clear A if it was <0

		mov.w   ACCAL,w1  ; extract new val
		mov.w   ACCAH,w2
		mov.w   ACCAU,w3
 		
		mov     w1, [w0 + #OffsetACCAL]; store new val 
        mov     w2, [w0 + #OffsetACCAH]; 
        mov     w3, [w0 + #OffsetACCAU];
		
		mov     [w0 + #OffsetThreshold], w1 ; load threshold

		lac     w1,b;
		sub     b;                      ; accb = accb - acca

		sac.r   b,w1                    ; extract b with saturation
	    mov.w	#0,w0;                  ; default to NOT protection
        btsc    w1,#15;                 ; check sign	
		mov.w	#1,w0;					; protection!   


;		mpy 	w4*w6, b, [w8]+=4, w4, [w10]+=2, w7 ; b = DERIVATIVE, w4 = ki, w10 pt to tPID.State (I), w8 point to kp
;		mac 	w4*w5, a, [w8]+=2, w4   ; A += e*ki, w4 = kp, w8 point to IMAX		   

  
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
        pop     w8

       return





;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; _PIDInit:
;
; Prototype:
; void I2TInit ( tI2T *fooI2TStruct )
;
; This initializes the tI2T structure clearing current state 
;.........................................................
        .global _I2TInit                ; provide global scope to routine
_I2TInit:
        push    w0
		push    w1
		push    w2
		push    w3
  
        mov ACCAL,w1				
        push.w w1					
        mov ACCAH,w1				
        push.w w1					
        mov ACCAU,w1				
        push.w w1

        clr a;

		mov.w   ACCAL,w1  ; extract new val
		mov.w   ACCAH,w2
		mov.w   ACCAU,w3
 		
		mov     w1, [w0 + #OffsetACCAL]; store new val 
        mov     w2, [w0 + #OffsetACCAH]; 
        mov     w3, [w0 + #OffsetACCAU];

        pop.w w1
        mov w1,ACCAU 
        pop.w w1
        mov w1,ACCAH 
        pop.w w1
        mov w1,ACCAL

        pop     w3
        pop     w2
        pop     w1
        pop     w0
        return




;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

        .end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; OEF
