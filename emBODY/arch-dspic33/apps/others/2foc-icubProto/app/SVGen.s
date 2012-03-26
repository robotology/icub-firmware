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
; *    Filename:       SVGEN.s                                          *
; *    Date:           10/01/08                                         *
; *                                                                     *
; *    Tools used:     MPLAB IDE -> 8.14                                *
; *                    C30 -> 3.10                                      *
; *    Linker File:    p33FJ256MC710.gld                                 *
; *                                                                     *
; **********************************************************************/
; SVGen
;  
;Description:  Calculate and load SVGen PWM values.
;
;
;
;Functional prototype:
; 
; void CalcSVGen( void )
;
;On Entry:   SVGenParm structure must contain qVr1, qVr2, qVr3
;
;On Exit:    PWM registers loaded
;
;Parameters: 
; Input arguments: None
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
; Registers modified: w0,w2,w3,w4,w5,w6,AccA
;
; Timing: 34 instruction cycles
;
;*******************************************************************
; C-Version of code
;
;void CalcRefVec( void )
;{ 
;    if( Vr1 >= 0 )
;        {       
;        // (xx1)
;        if( Vr2 >= 0 )
;            {
;            // (x11)
;            // Must be Sector 3 since Sector 7 not allowed
;            // Sector 3: (0,1,1)  0-60 degrees
;            T1 = Vr2
;            T2 = Vr1
;            CalcTimes();
;            dPWM1 = Ta
;            dPWM2 = Tb
;            dPWM3 = Tc
;            }
;        else
;            {            
;            // (x01)
;            if( Vr3 >= 0 )
;                {
;                // Sector 5: (1,0,1)  120-180 degrees
;                T1 = Vr1
;                T2 = Vr3
;                CalcTimes();
;                dPWM1 = Tc
;                dPWM2 = Ta
;                dPWM3 = Tb
;
;                }
;            else
;                {
;                // Sector 1: (0,0,1)  60-120 degrees
;                T1 = -Vr2;
;                T2 = -Vr3;
;                CalcTimes();
;                dPWM1 = Tb
;                dPWM2 = Ta
;                dPWM3 = Tc
;                }
;            }
;        }
;    else
;        {
;        // (xx0)
;        if( Vr2 >= 0 )
;            {
;            // (x10)
;            if( Vr3 >= 0 )
;                {
;                // Sector 6: (1,1,0)  240-300 degrees
;                T1 = Vr3
;                T2 = Vr2
;                CalcTimes();
;                dPWM1 = Tb
;                dPWM2 = Tc
;                dPWM3 = Ta
;                }
;            else
;                {
;                // Sector 2: (0,1,0)  300-0 degrees
;                T1 = -Vr3
;                T2 = -Vr1
;                CalcTimes();
;                dPWM1 = Ta
;                dPWM2 = Tc
;                dPWM3 = Tb
;                }
;            }
;        else
;            {            
;            // (x00)
;            // Must be Sector 4 since Sector 0 not allowed
;            // Sector 4: (1,0,0)  180-240 degrees
;            T1 = -Vr1
;            T2 = -Vr2
;            CalcTimes();
;            dPWM1 = Tc
;            dPWM2 = Tb
;            dPWM3 = Ta
;
;            }
;        }
;}
;
;
;void CalcTimes(void)
;{
;    T1 = PWM*T1
;    T2 = PWM*T2
;    Tc = (PWM-T1-T2)/2
;    Tb = Ta + T1
;    Ta = Tb + T2
;}        
;
;
;*******************************************************************
;
          .include "p33FJ128MC802.inc"

; External references
          .include "Park.inc"
          .include "SVGen.inc"

; Register usage
                        
          .equ WorkW,     w1  ; Working register
                        
          .equ T1W,       w2
          .equ T2W,       w3
                        
          .equ WorkDLoW,  w4  ; double word (multiply results)
          .equ Vr1W,      w4
          .equ TaW,       w4
          .equ WorkDHiW,  w5  ; double word (multiply results)
          .equ Vr2W,      w5
          .equ TbW,       w5
          .equ Vr3W,      w6
          .equ TcW,       w6

        .equ dPWM1, PDC1
        .equ dPWM2, PDC2
        .equ dPWM3, PDC3

;=================== CODE =====================

          .section  .text
          .global   _CalcSVGen
          .global   CalcSVGen

_CalcSVGen:
CalcSVGen:

     ;; Get qVr1,qVr2,qVr3
          mov.w     _SVGenParm+SVGen_qVr1,Vr1W
          mov.w     _SVGenParm+SVGen_qVr2,Vr2W
          mov.w     _SVGenParm+SVGen_qVr3,Vr3W

     ;; Test Vr1
          cp0       Vr1W
          bra       LT,jCalcRef20       ; Vr1W < 0

     ;; Test Vr2
          cp0       Vr2W
          bra       LT,jCalcRef10       ; Vr2W < 0

     ;; Must be Sector 3 since Sector 7 not allowed
     ;; Sector 3: (0,1,1)  0-60 degrees

     ;; T1 = Vr2
     ;; T2 = Vr1
          mov.w     Vr2W,T2W
          mov.w     Vr1W,T1W
          rcall     CalcTimes

     ;; dPWM1 = Ta
     ;; dPWM2 = Tb
     ;; dPWM3 = Tc
          mov.w     TaW,dPWM1
          mov.w     TbW,dPWM2
          mov.w     TcW,dPWM3
          return

jCalcRef10:
     ;; Test Vr3
          cp0       Vr3W
          bra       LT,jCalcRef15       ; Vr3W < 0

     ;; Sector 5: (1,0,1)  120-180 degrees
     ;; T1 = Vr1
     ;; T2 = Vr3
          mov.w     Vr1W,T2W
          mov.w     Vr3W,T1W
          rcall     CalcTimes

     ;; dPWM1 = Tc
     ;; dPWM2 = Ta
     ;; dPWM3 = Tb
          mov.w     TcW,dPWM1
          mov.w     TaW,dPWM2
          mov.w     TbW,dPWM3
          return

jCalcRef15:

     ;; Sector 1: (0,0,1)  60-120 degrees
     ;; T1 = -Vr2
     ;; T2 = -Vr3
          neg.w     Vr2W,T2W
          neg.w     Vr3W,T1W
          rcall     CalcTimes

     ;; dPWM1 = Tb
     ;; dPWM2 = Ta
     ;; dPWM3 = Tc
          mov.w     TbW,dPWM1
          mov.w     TaW,dPWM2
          mov.w     TcW,dPWM3
          return

jCalcRef20:

     ;; Test Vr2
          cp0       Vr2W
          bra       LT,jCalcRef30       ; Vr2W < 0

     ;; Test Vr3
          cp0       Vr3W
          bra       LT,jCalcRef25       ; Vr3W < 0

     ;; Sector 6: (1,1,0)  240-300 degrees
     ;; T1 = Vr3
     ;; T2 = Vr2
          mov.w     Vr3W,T2W
          mov.w     Vr2W,T1W
          rcall     CalcTimes

     ;; dPWM1 = Tb
     ;; dPWM2 = Tc
     ;; dPWM3 = Ta
          mov.w     TbW,dPWM1
          mov.w     TcW,dPWM2
          mov.w     TaW,dPWM3
          return

jCalcRef25:
     ;; Sector 2: (0,1,0)  300-360 degrees
     ;; T1 = -Vr3
     ;; T2 = -Vr1
          neg.w     Vr3W,T2W
          neg.w     Vr1W,T1W
          rcall     CalcTimes

     ;; dPWM1 = Ta
     ;; dPWM2 = Tc
     ;; dPWM3 = Tb
          mov.w     TaW,dPWM1
          mov.w     TcW,dPWM2
          mov.w     TbW,dPWM3
          return

jCalcRef30:
     ;; Must be Sector 4 since Sector 0 not allowed
     ;; Sector 4: (1,0,0)  180-240 degrees

     ;; T1 = -Vr1
     ;; T2 = -Vr2
          neg.w     Vr1W,T2W
          neg.w     Vr2W,T1W
          rcall     CalcTimes

     ;; dPWM1 = Tc
     ;; dPWM2 = Tb
     ;; dPWM3 = Ta
          mov.w     TcW,dPWM1
          mov.w     TbW,dPWM2
          mov.w     TaW,dPWM3
          return

;*******************************************************************
; CalcTimes
;
;
;void CalcTimes(void)
;{
;    T1 = PWM*T1
;    T2 = PWM*T2
;    Tc = (PWM-T1-T2)/2
;    Tb = Ta + T1
;    Ta = Tb + T2
;}        
;
; Timing: 17instruction cycles
;
;*******************************************************************
CalcTimes:

     ;; T1 = PWM*T1

     ;; Since T1 is in 1.15 and PWM in integer we do multiply by
     ;; 2*PWM*T1 as integers and use upper word of results

     ;; Load PWMPeriod 
          sl.w      _SVGenParm+SVGen_iPWMPeriod,WREG	;Mul PWM * 2 to allow for full range of voltage
          mul.us    w0,T1W,WorkDLoW
          mov.w     WorkDHiW,T1W

     ;; T2 = PWM*T2
          mul.us    w0,T2W,WorkDLoW
          mov.w     WorkDHiW,T2W

     ;; Tc = (PWM-T1-T2)/2
          ;mov.w     _SVGenParm+SVGen_iPWMPeriod,WorkW
          mov.w	    _SVGenParm+SVGen_iPWMPeriod,WREG				
          sub.w     w0,T1W,WorkW        ;PWM-T1
          sub.w     WorkW,T2W,WorkW     ; -T2
          asr.w     WorkW,WorkW         ; /2
          mov.w     WorkW,TcW           ; store Tc

     ;; Tb = Tc + T1
          add.w     WorkW,T1W,WorkW
          mov.w     WorkW,TbW


     ;; Ta = Tb + T2
          add.w     WorkW,T2W,WorkW
          mov.w     WorkW,TaW
          return
