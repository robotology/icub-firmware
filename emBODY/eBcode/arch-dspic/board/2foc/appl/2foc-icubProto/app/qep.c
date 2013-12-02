//
//
// TODO : completare con una bella descrizione!
//
//
#include <p33FJ128MC802.h>
#include <qei.h>

#include "UserParms.h"

volatile int qep_zero;

void QEPSetUseZero(int ena)
{

#ifdef ENCODER_ZERO_DETECTION_USE_INTERRUPT
	if(ena){
        qep_zero = 0;
		CNEN2bits.CN23IE = 1;
  	//	IFS1bits.CNIF = 0;
	
  	}else{
		CNEN2bits.CN23IE = 0;
	}
#else
	if(ena){
  		QEICONbits.QEIM = 6; 
  		IFS3bits.QEI1IF = 0;
  	}else{
		QEICONbits.QEIM = 7; 
	}
#endif

}

// called by the system.c module when the IDX pin changes state
void QEPZeroInt()
{
	POSCNT = 0;
	qep_zero = 1;
	
}

int QEPZeroCrossed()
{

#ifdef ENCODER_ZERO_DETECTION_USE_INTERRUPT
	if(qep_zero){
		qep_zero = 0;		
		return 1;
	 }	
#else

	if(/*QEICONbits.INDEX ||*/ IFS3bits.QEI1IF){
		IFS3bits.QEI1IF = 0;		
		return 1;
	 }
#endif

	 return 0;
}

void QEPInit()
// init the quadrature encoder peripheral
{

//
// Rivedere il tutto utilizzando <qei.h>
//

#if defined(ENCODER_QE) || defined(ENCODER_AIE)
  // Configure QEI pins as digital inputs
  // ADPCFG |= 0x0038;
  // ADPCFG |= (1<<5 | 1<<6 | 1<<7); //0x0038;

  // Disable QEI Module
  QEICONbits.QEIM = 0; 

  // Clear any count errors
  QEICONbits.CNTERR = 0; 
   // Continue operation during sleep
  QEICONbits.QEISIDL = 0;
  // QEA and QEB eventually swapped 
  QEICONbits.SWPAB = QE_SWAPAB; 
  // do not output direction on IO pin
  QEICONbits.PCDOUT = 0; 
  // Index pulse resets position counter
  QEICONbits.POSRES = 1; 
  
  // Count error interrupts disabled
  DFLTCONbits.CEID = 1; 
  // Digital filters output enabled for QEn and IDX pins
  DFLTCONbits.QEOUT = 1; 
  // 1:4 clock divide for digital filter for QEn
  // main CK = 40Mhz. Filter CK = 10 Mhz
  // Filter accept edge if it persist over 3 CK
  // cycles. So max QEP pin freq is about 3.3Mhz
  DFLTCONbits.QECK = 2; 

//  IFS3bits.QEI1IF = 0;
  // I was joking
//  IEC3bits.QEI1IE = 1;
	
//#ifdef ENCODER_USE_ZERO_FOR_ROTOR_ALIGNMENT
//  QEICONbits.QEIM = 6; 
//#else
  // X4 mode with position counter reset by match with MAXCNT
  QEICONbits.QEIM = 7; 

//#endif
  // Initialize the QEP module counter to run in modulus.
  // 
  // Example (drawn in X1 mode): encoder 3ppr, MAXCNT = 2;
  //      __      __      __      __      __      __      __      __      __      __ 
  //     |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |    |  |
  //  ___|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |____|  |
  // 2   0       1       2       0       1       2       0       1       2       0  
  //                       |                       |                       |     
  //                     round                   round                   round
  
  MAXCNT = QE_RESOLUTION - 1;
  // Reset position counter.
  POSCNT = 0; //QE_RESOLUTION -1;
#endif
}

unsigned int QEPGet()
{
  return POSCNT;
}
