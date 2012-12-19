//
// HW/FW Fault conditions handlers
//

#include "Faults.h"
#include <timer.h>
#include <string.h> // memset

void FaultReset(void)
// called when receiving shutdown cmd and
// going to readytoswitchon
{
	LED_status.RedBlinkRate = BLINKRATE_OFF;
// clear faults, but do not clear flags that keep track of 
// past error conditions like can state, SPI communication errors
//	memset(SysError.b,0,sizeof(SysError));

  	//unsigned HESInvalidValue:1;
    //unsigned HESInvalidSequence:1;

    // ADC calibration failure
    SysError.ADCCalFailure = 0;
    //unsigned Reserved:1;                    //4

    // Overvoltage
    SysError.OverVoltageFailure = 0;
    // Undervoltage
    SysError.UnderVoltageFailure = 0;
    // OverCurrent
    SysError.OverCurrentFailure = 0;
    // I2T protection
    SysError.I2TFailure = 0;                     //8
    // External Fault
    SysError.ExternalFaultAsserted = 0;

    //unsigned EMUROMFault:1;
    //unsigned EMUROMCRCFault:1;

//   unsigned EncoderFault:1;                   //12 

//   unsigned AS5045CSumError:1;               
//   unsigned AS5045CalcError:1;				 
//   unsigned FirmwarePWMFatalError:1;         //16
//   unsigned CAN_RTRFlagActive:1;
//   unsigned CAN_BufferOverRun:1;
//   unsigned CAN_TXWasPasv:1;
//   unsigned CAN_TXIsPasv:1;
//   unsigned CAN_RXWasPasv:1;
//   unsigned CAN_RXIsPasv:1;
//   unsigned CAN_IsWarn:1;
//   unsigned CAN_WasWarn:1;
//   unsigned CAN_DLCError:1;	
  
	
}

void FaultConditionsHandler(void)
// General pourpouse fault handler (for Overload, Exernal, Encoder, DCLink, Spped, CAN, )
// TODO: all faluts handlers
{
  // TODO: To be fully completed

  
  // Disable PWM
  PWMDisable();
  
  //I2T will run in behalf of timer 3 when PWM is disabled
  EnableIntT3;

  // Stop periodic CAN communication
//VALE: even if application is in fault it continues to send status msg
  //DisableIntT4;

  // turn on Red Led
  LED_status.RedBlinkRate=BLINKRATE_FAST;
  LED_status.GreenBlinkRate=BLINKRATE_OFF;

}

void __attribute__((__interrupt__,no_auto_psv)) _FLTA1Interrupt(void) 
// Interrupt handler for OVERCURRENT fault.
// HW will automatically stop PWM generation.
// We need to set SW flag here in order to update SW state information 
{

  // signal overcurrent
  SysError.OverCurrentFailure = 1;

  // call fault handler
  FaultConditionsHandler();

  // clear irq flag
  IFS3bits.FLTA1IF = 0;
}


void FaultExternalTriggered()
// External fault interrupt handler
{
  SysError.ExternalFaultAsserted = 1;
  // call generic handler
  FaultConditionsHandler();
}


