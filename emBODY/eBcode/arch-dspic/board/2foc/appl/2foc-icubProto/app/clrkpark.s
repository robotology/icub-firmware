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
; *    Filename:       clrkpark.s                                       *
; *    Date:           10/01/08                                         *
; *                                                                     *
; *    Tools used:     MPLAB IDE -> 8.14                                *
; *                    C30 -> 3.10                                      *
; *    Linker File:    p33FJ256MC710.gld                                *
; *                                                                     *
; **********************************************************************/
; ClarkePark
;  
;Description:        
;  Calculate Clarke & Park transforms. 
;  Assumes the Cos and Sin values are in qSin & qCos.
;
;         Ialpha = Ia
;         Ibeta  = Ia*dOneBySq3 + 2*Ib*dOneBySq3;
;             where Ia+Ib+Ic = 0
;
;         Id =  Ialpha*cos(Angle) + Ibeta*sin(Angle)
;         Iq = -Ialpha*sin(Angle) + Ibeta*cos(Angle)
;
;  This routine works the same for both integer scaling and 1.15 scaling.
;
;Functional prototype:
; 
; void ClarkePark( void )
;
;On Entry:   ParkParm structure must contain qSin, qCos, qIa and qIb.
;
;On Exit:    ParkParm will contain qId, qIq  
;
;Parameters: 
; Input arguments: None
;
; Return:
;   Void
;
; SFR Settings required:
;         CORCON.SATA  = 0
;     If there is any chance that (Ia+2*Ib)/sqrt(3) will overflow must set
;         CORCON.SATDW = 1  
;
; Support routines required: None
;
; Local Stack usage: None
;
; Registers modified: w3 -> w7
;
; Timing: 20 cycles
;
;*******************************************************************
;
          .include "p33FJ128MC802.inc"

; External references
          .include "park.inc"

; Register usage
          .equ ParmW,  w3    ; Ptr to ParkParm structure

          .equ Sq3W,   w4    ; OneBySq3
          .equ SinW,   w4    ; replaces Work0W

          .equ CosW,   w5


          .equ IaW,    w6    ; copy of qIa
          .equ IalphaW,w6    ; replaces Ia

          .equ IbW,    w7    ; copy of qIb
          .equ IbetaW, w7    ; Ibeta  replaces Ib

; Constants
          .equ OneBySq3,0x49E7   ; 1/sqrt(3) in 1.15 format


;  C O D E

          .section  .text
          .global   _ClarkePark
          .global   ClarkePark

_ClarkePark:
ClarkePark:
     ;; Ibeta = Ia*OneBySq3 + 2*Ib*OneBySq3;

; save DSP accumulator A to the stack
          mov ACCAL,w4				
          push.w w4					
          mov ACCAH,w4				
          push.w w4					
          mov ACCAU,w4				
          push.w w4

          mov.w     #OneBySq3,Sq3W     ; 1/sqrt(3) in 1.15 format

          mov.w     _ParkParm+Park_qIa,IaW
          mpy       Sq3W*IaW,A

          mov.w     _ParkParm+Park_qIb,IbW
          mac       Sq3W*IbW,A
          mac       Sq3W*IbW,A

          mov.w     _ParkParm+Park_qIa,IalphaW
          mov.w     IalphaW,_ParkParm+Park_qIalpha
          sac       A,IbetaW
          mov.w     IbetaW,_ParkParm+Park_qIbeta

     ;; Ialpha and Ibeta have been calculated. Now do rotation.

     ;; Get qSin, qCos from ParkParm structure
          mov.w     _ParkParm+Park_qSin,SinW
          mov.w     _ParkParm+Park_qCos,CosW

     ;; Id =  Ialpha*cos(Angle) + Ibeta*sin(Angle)

          mpy       SinW*IbetaW,A     ; Ibeta*qSin -> A
          mac       CosW*IalphaW,A    ; add Ialpha*qCos to A
          mov.w     #_ParkParm+Park_qId,ParmW
          sac       A,[ParmW++]        ; store to qId, inc ptr to qIq

     ;; Iq = -Ialpha*sin(Angle) + Ibeta*cos(Angle)
          mpy       CosW*IbetaW,A     ; Ibeta*qCos -> A
          msc       SinW*IalphaW,A    ; sub Ialpha*qSin from A
          sac       A,[ParmW]          ; store to qIq

;restore DSP accumulator A from the stack.
          pop.w w4
          mov w4,ACCAU 
          pop.w w4
          mov w4,ACCAH 
          pop.w w4
          mov w4,ACCAL

          return
          .end
