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
; *    Filename:       InvPark.s                                       *
; *    Date:           10/01/08                                         *
; *                                                                     *
; *    Tools used:     MPLAB IDE -> 8.14                                *
; *                    C30 -> 3.10                                      *
; *    Linker File:    p33FJ256MC710.gld                                 *
; *                                                                     *
; **********************************************************************/
; InvPark
;  
;Description:        
;  Calculate the inverse Park transform. Assumes the Cos and Sin values 
;  are in the ParkParm structure.
;
;         Valpha =  Vd*cos(Angle) - Vq*sin(Angle)
;         Vbeta  =  Vd*sin(Angle) + Vq*cos(Angle)
;
;  This routine works the same for both integer scaling and 1.15 scaling.
;
;Functional prototype:
; 
; void InvPark( void )
;
;On Entry:   The ParkParm structure must contain qCos, qSin, qVd and qVq.
;
;On Exit:    ParkParm will contain qValpha, qVbeta.
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
; Registers modified: w3 -> w7, A
;
; Timing:
;  About 14 instruction cycles
;
;*******************************************************************
;
          .include "p33FJ128MC802.inc"

; External references
          .include "park.inc"

; Register usage
          .equ ParmW,  w3    ; Ptr to ParkParm structure

          .equ SinW,   w4
          .equ CosW,   w5
          .equ VdW,    w6    ; copy of qVd
          .equ VqW,    w7    ; copy of qVq



;  C O D E

          .section  .text
          .global   _InvPark
          .global   InvPark

_InvPark:
InvPark:
; saves w0 on the stack because it is used as temp below
          push.w w0
; saves DSP accumulator A to the stack
          mov ACCAL,w0				
          push.w w0					
          mov ACCAH,w0				
          push.w w0					
          mov ACCAU,w0				
          push.w w0

     ;; Get qVd, qVq from ParkParm structure
          mov.w     _ParkParm+Park_qVd,VdW
          mov.w     _ParkParm+Park_qVq,VqW

     ;; Get qSin, qCos from ParkParm structure
          mov.w     _ParkParm+Park_qSin,SinW
          mov.w     _ParkParm+Park_qCos,CosW

     ;; Valpha =  Vd*cos(Angle) - Vq*sin(Angle)

          mpy       CosW*VdW,A        ; Vd*qCos -> A
          msc       SinW*VqW,A        ; sub Vq*qSin from A

          mov.w     #_ParkParm+Park_qValpha,ParmW
          sac       A,[ParmW++]        ; store to qValpha, inc ptr to qVbeta

     ;; Vbeta  =  Vd*sin(Angle) + Vq*cos(Angle)
          mpy       SinW*VdW,A        ; Vd*qSin -> A
          mac       CosW*VqW,A        ; add Vq*qCos to A
          sac       A,[ParmW]          ; store to Vbeta


          pop.w w0
;restore DSP accumulator A from the stack
          mov w0,ACCAU 
          pop.w w0
          mov w0,ACCAH 
          pop.w w0
          mov w0,ACCAL
; restore saved w0 from the stack
          pop.w w0

           return
          .end
