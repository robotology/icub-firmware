; ***********************************************************************
; *
; *    Filename:       MeasCurr.s
; *
; *    Tools used:     MPLAB IDE -> 8.30
; *                    C30 -> 3.10
; *
; **********************************************************************/
;
; MeasAndCompIaIb
; Description:        
;  Read Channels 1 & 2 of ADC, scale them as signed fractional values 
;  using qKa, qKb and put the results qIa and qIb of ParkParm. 
;  When this function is used the phases of the motor should be connected 
;  accordingly on the board X->A, Y->C, Z->B  
;
; MeasAndCompIaIcCalculateIb
; Description:        
;  Read Channels 1 & 2 of ADC, scale them as signed fractional values 
;  using qKa, qKc and put the results qIa and qIc of ParkParm, 
;  calculate qIb for debug pourposes from qIa and qIc
;
;  Specifically the offset is used to correct the raw ADC by
;         CorrADC = ADCBUFn - Offset
;
;  Do not call this routine until conversion is completed.
;
;  Scaling constant, qKa and qKc, must be set elsewhere such that
;         qIa = qKa * CorrADC1
;         qIc = qKc * CorrADC2
;
;Functional prototypes:  
;         void MeasAndCompIaIcCalculateIb();
;       
;On Start:   Must call ADCDoOffsetCalibration()
;
;On Entry:   MeasCurrParm structure must contain qKa & qKc. ADC channels 1 & 2
;            must contain signed fractional value.
;
;On Exit:    ParkParm will contain qIa, qIb and qIc.
;
;Parameters: 
; Input arguments: None
;
; Return:
;   Void
;
; SFR Settings required:
;   CORCON.SATA  = 0
; If there is any chance that Accumulator will overflow must set
;   CORCON.SATDW = 1  
;
; Support routines required: None
;
; Local Stack usage: None
;
; Registers modified: w0,w1,w4,w5,w6,w7
;
; Timing: ... cycles
;
;*******************************************************************

          .include "p33FJ128MC802.inc"

; External references
          .include "MeasCurr.inc"
          .include "Park.inc"

          .global   _MeasAndCompIaIcCalculateIb
          .global   MeasAndCompIaIcCalculateIb

_MeasAndCompIaIcCalculateIb:
MeasAndCompIaIcCalculateIb:

; Saves DSP accumulator A on the STACK
          mov ACCAL,w0				
          push.w w0					
          mov ACCAH,w0				
          push.w w0					
          mov ACCAU,w0				
          push.w w0

          mov.w     _ADCBuffer+0,w0			; w0 = -qIa (with offset)
	  sub.w     _MeasCurrParm+ADC_Offseta,WREG	; w0 = qIa

          mov.w     w0,w5                           ; w5 = qIa
          mov.w     _MeasCurrParm+ADC_qKa,w4        ; w4 = qKa
          mpy       w4*w5,A                         ; Accum = qKa*qIa
          sac.r     A,#0,w4                         ; w4 = qKa*qIa
          mov.w     w4,_ParkParm+Park_qIa           ; ParkParm.qIa = qIa * qKa
        
          mov.w     _ADCBuffer+2,w0			; w0 = -qIc (with offset)
    	  sub.w     _MeasCurrParm+ADC_Offsetbc,WREG	; w0 = qIc   
		  
; This code is for diagnostic pourpose only, ic is not used in the algorithm
          mov.w     w0,w6                           ; w6 = qIc
          mov.w     _MeasCurrParm+ADC_qKc, w7       ; w7 = qKc
          mpy       w6*w7,A                         ; Accum = qIc * qKc
          sac.r     A,#0, w4                        ; w4 = qIc * qKc
          mov.w     w4,_ParkParm+Park_qIc           ; ParkParm.qIc = qIc * qKc
; end of Ic related calculations

;          add.w     w5,w0,w0;                       ; w0 = qIa + qIc
;	   mov.w     w0,w5		             ; w5 = qIa + qIc	

;          mov.w     _MeasCurrParm+ADC_qKb,w4        ; w4 = qKb
;          mpy       w4*w5,A                         ; Accum = qIb * qKb
;          sac.r     A,#0,w4                         ; w4 = qIb * qKb
;          mov.w     w4,_ParkParm+Park_qIb           ; ParkParm.qIb = qIb * qKb

;restore DSP accumulator A from the stack
          pop.w w0
          mov w0,ACCAU 
          pop.w w0
          mov w0,ACCAH 
          pop.w w0
          mov w0,ACCAL

          return


          .global   _MeasAndCompIaIb
          .global   MeasAndCompIaIb

_MeasAndCompIaIb:
MeasAndCompIaIb:

; saves DSP accumulator A to the stack
          mov ACCAL,w0				
          push.w w0					
          mov ACCAH,w0				
          push.w w0					
          mov ACCAU,w0				
          push.w w0

          mov.w     _ADCBuffer+0,w0                 ; w0 = -qIa (with offset)
          sub.w     _MeasCurrParm+ADC_Offseta,WREG  ; w0 = qIa 
          mov.w     w0,w5                           ; w5 = qIa // magari si può evitare?
          mov.w     _MeasCurrParm+ADC_qKa,w4        ; w4 = qKa
          mpy       w4*w5,A                         ; Accum = qKa*qIa 
          sac.r     A,#0,w4                         ; w4 = qKa*qIa
          mov.w     w4,_ParkParm+Park_qIa           ; ParkParm.qIa = qKa*qIa

          mov.w	    _ADCBuffer+2,w0                 ; w0 = -qIb (with offset)
    	  sub.w     _MeasCurrParm+ADC_Offsetbc,WREG ; w0 = qIb 
          mov.w     w0,w5                           ; w5 = qIb
          mov.w     _MeasCurrParm+ADC_qKb,w4        ; w4 = qKb
          mpy       w4*w5,A                         ; Accum = qKb*qIb
          sac.r     A,#0,w4                         ; w4 = qKb*qIb
          mov.w     w4,_ParkParm+Park_qIb           ; ParkParm.qIb = qKb*qIb

;restore DSP accumulator A from the STACK
          ; qIc is not calculated for performance optimization
          pop.w w0
          mov w0,ACCAU 
          pop.w w0
          mov w0,ACCAH 
          pop.w w0
          mov w0,ACCAL

          return

          .end
