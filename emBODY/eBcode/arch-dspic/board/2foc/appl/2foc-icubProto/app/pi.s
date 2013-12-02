; /**********************************************************************
; *                                                                     *
; *                        Software License Agreement                   *
; *                                                                     *
; *    The software supplied herewith by Microchip Technology           *
; *    Incorporated (the "Company") for its dsPIC controller            *
; *    is intended and supplied to you, the Company's customer,         *
; *    for use solely and exclusively on Microchip dsPIC                *
; *    products. The software is owned by the Company and/or its        *
; *    supplier, and is protected under applicable copyright laws. All  *
; *    rights are reserved. Any use in violation of the foregoing       *
; *    restrictions may subject the user to criminal sanctions under    *
; *    applicable laws, as well as to civil liability for the breach of *
; *    the terms and conditions of this license.                        *
; *                                                                     *
; *    THIS SOFTWARE IS PROVIDED IN AN "AS IS" CONDITION.  NO           *
; *    WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING,    *
; *    BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND    *
; *    FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE     *
; *    COMPANY SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL,  *
; *    INCIDENTAL OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.  *
; *                                                                     *
; ***********************************************************************
; *                                                                     *
; *    Filename:       pi.s                                             *
; *    Date:           10/01/08                                         *
; *                                                                     *
; *    Tools used:     MPLAB IDE -> 8.14                                *
; *                    C30 -> 3.10                                      *
; *    Linker File:    p33FJ256MC710.gld                                 *
; *                                                                     *
; **********************************************************************/
; PI
;  
;Description:  Calculate PI correction.
;
;void CalcPI( tPIParm *pParm)
;{
;    Err  = InRef - InMeas
;    U  = Sum + Kp * Err
;    if( U > Outmax )
;        Out = Outmax
;    else if( U < Outmin )
;        Out = Outmin
;    else        
;        Out = U 
;    Exc = U - Out
;    Sum = Sum + Ki * Err - Kc * Exc
;}
;
;
;void InitPI( tPIParm *pParm)
;{
;    Sum = 0
;    Out = 0
;}
;
;----------------------------
; Representation of PI constants:
; The constant Kp is scaled so it can be represented in 1.15 format by 
; adjusting the constant by a power of 2 which is removed when the
; calculation is completed.
;
; Kp is scaled Kp = qKp * 2^NKo 
;
; Ki & Kc are scaled Ki = qKi, Kc = qKc
;
;
;Functional prototype:
; 
; void InitPI( tPIParm *pParm)
; void CalcPI( tPIParm *pParm)
;
;On Entry:   PIParm structure must contain qKp,qKi,qKc,qOutMax,qOutMin,
;                                           InRef,InMeas
;On Exit:    PIParm will contain qOut
;
;Parameters: 
; Input arguments: tPIParm *pParm
;
; Return:
;   Void
;
; SFR Settings required:
;         CORCON.SATA  = 0
;         CORCON.IF    = 0
;
; Support routines required: None
;
; Local Stack usage: 0
;
; Registers modified: w0-w6,AccA
;
; Timing:
;  31 instruction cycles max, 28 cycles min
;
;*******************************************************************
;
          .include "p33FJ128MC802.inc"

; External references
          .include "PI.inc"

; Register usage

          .equ BaseW0,   w0  ; Base of parm structure

          .equ OutW1,    w1  ; Output
          .equ SumLW2,   w2  ; Integral sum
          .equ SumHW3,   w3  ; Integral sum

          .equ ErrW4,    w4  ; Error term: InRef-InMeas
          .equ WorkW5,   w5  ; Working register
          .equ UnlimitW6,w6  ; U: unlimited output 
          .equ WorkW7,   w7  ; Working register


;  C O D E

          .section  .text

          .global   _InitPI
          .global   InitPI
_InitPI:
InitPI:
          mov.w     w1,[BaseW0+PI_qOut]
          return


          .global   _CalcPI
          .global   CalcPI

_CalcPI:
CalcPI:
; saves DSP accumulator A on the STACK
          mov ACCAL,WorkW7				
          push.w WorkW7					
          mov ACCAH,WorkW7				
          push.w WorkW7					
          mov ACCAU,WorkW7				
          push.w WorkW7
; saves DSP accumulator B on the STACK
          mov ACCBL,WorkW7				
          push.w WorkW7					
          mov ACCBH,WorkW7				
          push.w WorkW7					
          mov ACCBU,WorkW7				
          push.w WorkW7

     ;; Err  = InRef - InMeas
          mov.w     [BaseW0+PI_qInRef],WorkW7
          mov.w     [BaseW0+PI_qInMeas],WorkW5
          sub.w     WorkW7,WorkW5,ErrW4        
      ;; U  = Sum + Kp * Err * 2^NKo
          lac       [++BaseW0],B               ; AccB = Sum          
          mov.w     [--BaseW0],WorkW5
          mov.w     WorkW5,ACCBLL

          mov.w     [BaseW0+PI_qKp],WorkW5
          mpy       ErrW4*WorkW5,A
          sftac     A,#-NKo                  ; AccA = Kp*Err*2^NKo     
          add       A                        ; Sum = Sum + Kp*Err*2^NKo
          sac       A,UnlimitW6              ; store U before tests

     ;; if( U > Outmax )
     ;;     Out = Outmax
     ;; else if( U < Outmin )
     ;;     Out = Outmin
     ;; else        
     ;;     Out = U 

          mov.w     [BaseW0+PI_qOutMax],OutW1
          cp        UnlimitW6,OutW1
          bra       GT,jPI5            ; U > Outmax; OutW1 = Outmax

          mov.w     [BaseW0+PI_qOutMin],OutW1
          cp        UnlimitW6,OutW1
          bra       LE,jPI5            ; U < Outmin; OutW1 = Outmin

          mov.w     UnlimitW6,OutW1         ; OutW1 = U
jPI5:
          mov.w     OutW1,[BaseW0+PI_qOut]

     ;; Ki * Err
          mov.w     [BaseW0+PI_qKi],WorkW5
          mpy       ErrW4*WorkW5,A

     ;; Exc = U - Out
          sub.w     UnlimitW6,OutW1,UnlimitW6

     ;; Ki * Err - Kc * Exc 
          mov.w     [BaseW0+PI_qKc],WorkW5
          msc       WorkW5*UnlimitW6,A

     ;; Sum = Sum + Ki * Err - Kc * Exc 
          add       A
          
          sac       A,[++BaseW0]             ; store Sum 
          mov.w     ACCALL,WorkW5
          mov.w     WorkW5,[--BaseW0]
; restore DSP accumulator B from the stack
          pop.w WorkW7
          mov WorkW7,ACCBU 
          pop.w WorkW7
          mov WorkW7,ACCBH 
          pop.w WorkW7
          mov WorkW7,ACCBL

; restore DSP accumulator A from the stack
          pop.w WorkW7
          mov WorkW7,ACCAU 
          pop.w WorkW7
          mov WorkW7,ACCAH 
          pop.w WorkW7
          mov WorkW7,ACCAL

          return


          .global   _FracMpy
          .global   FracMpy

_FracMpy:
FracMpy:

    push.w WorkW7					
; saves DSP accumulator A to the stack
    mov ACCAL,WorkW7				
    push.w WorkW7					
    mov ACCAH,WorkW7				
    push.w WorkW7					
    mov ACCAU,WorkW7				
    push.w WorkW7

	push W4
	push W5
	mov	W0, W4
	mov	W1, W5
	mpy W4*W5, A
	sac.r A, W0
	pop W5
	pop W4
;restore DSP accumulator A from the stack
    pop.w WorkW7
    mov WorkW7,ACCAU 
    pop.w WorkW7
    mov WorkW7,ACCAH 
    pop.w w0
    mov WorkW7,ACCAL
    pop.w WorkW7
	
    return

          .global   _FracDiv
          .global   FracDiv

_FracDiv:
FracDiv:

	push W2
	mov	W1, W2
	repeat #17
	divf W0, W2
	pop W2
	return

          .end
