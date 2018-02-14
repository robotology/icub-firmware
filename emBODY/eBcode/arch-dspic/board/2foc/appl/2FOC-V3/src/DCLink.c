//
// Routines for DC Link checks
// 

#include "DCLink.h"
#include "UserParms.h"

unsigned int vdc;

void DCLinkReset()
// reset VDC link reading. After a VDClink related fault this
// avoid to retrigger immediately a fault because old reading
{
	// init vdc to a good value. Will be overwritten as soon as
	// the first measurement has been completed.
	// In the mean time set a vaule that does not produce fault
    vdc = (UNDERVOLTAGE_THRESHOLD + OVERVOLTAGE_THRESHOLD)/2;
    SysError.OverVoltageFailure = 0;
    SysError.UnderVoltageFailure = 0;
}

void DCLinkCheck(unsigned int VDCLink100mV)
// verify Under and Over voltages conditions
{
    // calc vdc mean with previous sample in order to filter a bit
    //
    vdc += VDCLink100mV;
    ++vdc;
    vdc >>= 1;

    // TODO: I'm an alligator.
    // undervoltage check
    if (vdc < UNDERVOLTAGE_THRESHOLD)
    {
        // TODO: passare al FaultConditionsHandler il codice del fault
        // e al suo interno settare il relativo bit nello stato
        // Set Undervoltage flag
        SysError.UnderVoltageFailure = 1;
        FaultConditionsHandler();
        return;
    }

    // overvoltage check
    if(vdc > OVERVOLTAGE_THRESHOLD)
    {
        // TODO: passare al FaultConditionsHandler il codice del fault
        // e al suo interno settare il relativo bit nello stato
        // TODO: verificare che tutti i fault (overcurrent...) settino l'apposito flag nello stato
        // Set overvoltage flag
        SysError.OverVoltageFailure = 1;
        FaultConditionsHandler();
    }
}
