//
// HW/FW Fault conditions handlers
//

#include "PWM.h"
#include "Faults.h"
#include "2FOC.h"
#include "can_icubProto.h"
#include "can_icubProto_trasmitter.h"
#include <timer.h>
#include <string.h> // memset

volatile tSysError SysError;

int fault = 0;

inline int Fault(void) __attribute__((always_inline));
int Fault(void)
{
    // exclude external fault
    if (SysError.L & 0xFFFFFFFE) return 1;

    return fault;
}

void FaultRecheck()
// retrigger the fault handler if
// a fault happened.
// This is used if a fault happened but someone might
// have enabled PWM or CAN or changed LEDs to restore
// the fault state
{
    if(fault)
    {
        fault = 0;
        FaultConditionsHandler();
    }
}

//volatile extern unsigned char HALL;

void FaultReset(void)
// called when receiving shutdown cmd and
// going to readytoswitchon
{
    fault = 0;

    LED_status.RedBlinkRate = BLINKRATE_OFF;
    
    //HALL = 0xFF;

    SysError.L = 0;
}

inline void FaultConditionsHandler(void) __attribute__((always_inline));
void FaultConditionsHandler(void)
// General pourpouse fault handler (for Overload, Exernal, Encoder, DCLink, Spped, CAN, )
{
    if(fault) return;

    fault = 1;

    DisableDrive();

    //I2T will run in behalf of timer 3 when PWM is disabled
    Timer3Enable();

    // Send Periodic Status Message with fault inf
    //CanIcubProtoTrasmitter_SendStatusMsg();

    // turn on Red Led
    LED_status.RedBlinkRate=BLINKRATE_FAST;
    LED_status.GreenBlinkRate=BLINKRATE_OFF;

    gControlMode = icubCanProto_controlmode_hwFault;
}

