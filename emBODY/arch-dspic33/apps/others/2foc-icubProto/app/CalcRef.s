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
; *    Filename:       CalcRef.s                                        *
; *    Date:           10/01/08                                         *
; *                                                                     *
; *    Tools used:     MPLAB IDE -> 8.14                                *
; *                    C30 -> 3.10                                      *
; *    Linker File:    p33FJ256MC710.gld                                 *
; *                                                                     *
; **********************************************************************/
; CalcRefVec
;  
;Description:        
;  Calculate the scaled reference vector, (Vr1,Vr2,Vr3), from qValpha,qVbeta.
;  The method is an modified inverse Clarke transform where Valpha & Vbeta 
;  are swapped compared to the normal Inverse Clarke.
;
;       Vr1 = Vbeta
;       Vr2 = (-Vbeta/2 + sqrt(3)/2 * Valpha)
;       Vr3 = (-Vbeta/2 - sqrt(3/2) * Valpha)
;
;Functional prototype:
; 
; void CalcRefVec( void )
;
;On Entry:   The ParkParm structure must contain qCos ?, qSin ?, qValpha and qVbeta.
;
;On Exit:    SVGenParm will contain qVr1, qVr2, qVr3
;
;Parameters: 
; Input arguments: None
;
; Return:
;   Void
;
; SFR Settings required:
;         CORCON.SATA  = 0
; Support routines required: None
;
; Local Stack usage: none
;
; Registers modified: w0, w4,w5,w6 
;
; Timing:
;  About 20 instruction cycles
;
;*******************************************************************
;
; External references
          .include "park.inc"
          .include "SVGen.inc"

; Register usage
          .equ WorkW,   w0    ; working

          .equ ValphaW, w4    ; qValpha (scaled)
          .equ VbetaW,  w5    ; qVbeta (scaled)
          .equ ScaleW,  w6    ; scaling

; Constants

          .equ Sq3OV2,0x6ED9 ; sqrt(3)/2 in 1.15 format

;  C O D E

          .section  .text
          .global   _CalcRefVec
          .global   CalcRefVec

_CalcRefVec:
CalcRefVec:

   ; save DSP accumulator A on the STACK
          mov ACCAL,WorkW				
          push.w WorkW					
          mov ACCAH,WorkW				
          push.w WorkW			
          mov ACCAU,WorkW				
          push.w WorkW

     ;; Get qValpha, qVbeta from ParkParm structure
          mov.w     _ParkParm+Park_qValpha,ValphaW
          mov.w     _ParkParm+Park_qVbeta,VbetaW

     ;; Put Vr1 = Vbeta
          mov.w     VbetaW,_SVGenParm+SVGen_qVr1

     ;; Load Sq(3)/2
          mov.w     #Sq3OV2,ScaleW

     ;; AccA = -Vbeta/2
          neg.w     VbetaW,VbetaW
          lac       VbetaW,#1,A

     ;; Vr2 = -Vbeta/2 + sqrt(3)2 * Valpha)
          mac       ValphaW*ScaleW,A ; add Valpha*sqrt(3)/2 to A
          sac       A,WorkW
          mov.w     WorkW,_SVGenParm+SVGen_qVr2

     ;; AccA = -Vbeta/2
          lac       VbetaW,#1,A

     ;; Vr3 = (-Vbeta/2 - sqrt(3)2 * Valpha)
          msc       ValphaW*ScaleW,A ; sub Valpha*sqrt(3)2 to A
          sac       A,WorkW
          mov.w     WorkW,_SVGenParm+SVGen_qVr3

; restore DSP accumulator A from the stack
          pop.w WorkW
          mov WorkW,ACCAU 
          pop.w WorkW
          mov WorkW,ACCAH 
          pop.w WorkW
          mov WorkW,ACCAL

          return
          .end
