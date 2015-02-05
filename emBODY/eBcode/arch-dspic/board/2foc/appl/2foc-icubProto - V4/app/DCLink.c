//
// Routines for DC Link checks
// 

#include "DCLink.h"
#include "UserParms.h"

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




void DCLinkCheck()
// verify Under and Over voltages conditions
{
  // calc vdc mean with previous sample in order to filter a bit
  vdc += ADCVDCLinkTo100mV(VDCLink);
  vdc /= 2;

// TODO: questa roba qui dentro non ci azzecca una favonia.
  // undervoltage check 
  // TODO: portare le variabili in ram come per l'I2T
  if(vdc < UNDERVOLTAGE_THRESHOLD)
  { 
    // TODO: passare al FaultConditionsHandler il codice del fault 
    // e al suo interno settare il relativo bit nello stato 
    // Set Undervoltage flag
    SysError.UnderVoltageFailure = 1;
    FaultConditionsHandler();
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
