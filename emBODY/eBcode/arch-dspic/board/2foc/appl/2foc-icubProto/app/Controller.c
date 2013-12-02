//   
//  PID controllers and related rouines
//  used for Current Velocity and Position control
//

// It is equal to the Controller_edl_pid.c, the EDL newest implementation, totally rewritten

#include "Controller.h"
#include "UserParms.h"

#ifdef NEW_PID

SFRAC16 CurrentQIDPMCoefficient[4] __attribute__ ((section (".xbss, bss, xmemory")));   
SFRAC16 CurrentQState[2] __attribute__ ((section (".ybss, bss, ymemory")));


SFRAC16 CurrentDIDPMCoefficient[4] __attribute__ ((section (".xbss, bss, xmemory")));   
SFRAC16 CurrentDState[2] __attribute__ ((section (".ybss, bss, ymemory")));

SFRAC16 WIDPMCoefficient[4] __attribute__ ((section (".xbss, bss, xmemory")));   
SFRAC16 WState[2] __attribute__ ((section (".ybss, bss, ymemory")));

fractional TorqueIDPMCoefficient[4] __attribute__ ((section (".xbss, bss, xmemory")));
// A.M. ymemory on this device is addressable only with the large data model!
fractional TorqueState[2] __attribute__ ((section (".ybss, bss, ymemory")));

tPID2 TorqueSensorPID;
tPID2 CurrentQPID;
tPID2 CurrentDPID;
tPID2 WPID;
volatile unsigned char current_open_loop=0;

void ControllerCurrentDPIDClear()
{
	PID2Init(&CurrentDPID);
}

void ControllerCurrentQPIDClear()
{
	PID2Init(&CurrentQPID);
}

void ControllerSetCurrentDPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d, SFRAC16 max)
{
	CurrentDIDPMCoefficient[0] = i;
	CurrentDIDPMCoefficient[1] = d;
	CurrentDIDPMCoefficient[2] = p;
	CurrentDIDPMCoefficient[3] = max;

    CurrentDPID.PShift = KSHIFT_D_P;
    CurrentDPID.IShift = KSHIFT_D_I; 
    CurrentDPID.DShift = KSHIFT_D_D;

  
    // Clear the controler history and the controller output
    PID2Init(&CurrentDPID);
}

void ControllerSetCurrentQPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d,SFRAC16 max)
{
	CurrentQIDPMCoefficient[0] = i;
	CurrentQIDPMCoefficient[1] = d;
	CurrentQIDPMCoefficient[2] = p;
	CurrentQIDPMCoefficient[3] = max;

 
    CurrentQPID.PShift = KSHIFT_Q_P;
    CurrentQPID.IShift = KSHIFT_Q_I; 
    CurrentQPID.DShift = KSHIFT_Q_D;

    // Clear the controler history and the controller output
    PID2Init(&CurrentQPID);
}

void ControllerGetCurrentDPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max)
{
	*i = CurrentDIDPMCoefficient[0];
	*d = CurrentDIDPMCoefficient[1];
	*p = CurrentDIDPMCoefficient[2];
	*max = CurrentDIDPMCoefficient[3];
}

void ControllerGetCurrentQPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max)
{
	*i = CurrentQIDPMCoefficient[0];
	*d = CurrentQIDPMCoefficient[1];
	*p = CurrentQIDPMCoefficient[2];
	*max = CurrentQIDPMCoefficient[3];
}

void ControllerSetWPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d, SFRAC16 max)
{
	WIDPMCoefficient[0] = i;
	WIDPMCoefficient[1] = d;
	WIDPMCoefficient[2] = p;
	WIDPMCoefficient[3] = max;


    WPID.PShift = KSHIFT_W_P;
    WPID.IShift = KSHIFT_W_I; 
    WPID.DShift = KSHIFT_W_D;

    // Clear the controler history and the controller output
    PID2Init(&WPID);
}

void ControllerGetWPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max)
{
	*i = WIDPMCoefficient[0];
	*d = WIDPMCoefficient[1];
	*p = WIDPMCoefficient[2];
	*max = WIDPMCoefficient[3];
}

void IdIqControl(void)
// Executes one PI iteration for current loops Id,Iq,
{
  SFRAC16 qVdSquared = 0; // Vd and Vq vector limitation variables
    // This variable is used to know what is left from the VqVd vector
    // in order to have maximum output PWM without saturation.
  SFRAC16 max;

  CurrentDPID.controlReference = CtrlReferences.qIdRef;

  if (current_open_loop)
  {
  	CurrentDPID.measuredOutput = 0; 
  	CurrentQPID.measuredOutput = 0; 
  }
  else
  {
    CurrentDPID.measuredOutput = ParkParm.qId;
  	CurrentQPID.measuredOutput = ParkParm.qIq;
  }

  PID2(&CurrentDPID);  
  DPIDError = CurrentDPID.controlState[1];

  // Vd is not limited ????????????????????
  if(CurrentDPID.controlOutput > Q15(DOUTMAX))
  {
    ParkParm.qVd = Q15(DOUTMAX);
  }
  else
  { 
    if(CurrentDPID.controlOutput < -Q15(DOUTMAX))
    {
	  ParkParm.qVd = -Q15(DOUTMAX);
    }
    else
    {
  	  ParkParm.qVd = CurrentDPID.controlOutput;
    }
  }

  CurrentQPID.controlReference = CtrlReferences.qIqRef;


  PID2(&CurrentQPID);

  QPIDError =  CurrentQPID.controlState[1];

  if(CurrentQPID.controlOutput > Q15(QOUTMAX))
  {
    ParkParm.qVq = Q15(QOUTMAX);
  }
  else
  { 
    if(CurrentQPID.controlOutput < -Q15(QOUTMAX))
    {
	  ParkParm.qVq = -Q15(QOUTMAX);
    }
    else
    {
  	  ParkParm.qVq = CurrentQPID.controlOutput;
    }
  }



  // Vector limitation
 
  // Vq is limited so the vector Vs is less than a maximum of 95%.
  // The 5% left is needed to be able to measure current through
  // shunt resistors.
  // Vs = SQRT(Vd^2 + Vq^2) < 0.95
  // Vq = SQRT(0.95^2 - Vd^2)


  qVdSquared = FracMpy(ParkParm.qVd, ParkParm.qVd);
  max = _Q15sqrt(Q15(QOUTMAX*QOUTMAX) - qVdSquared);


  if(CurrentQPID.controlOutput > max){

    ParkParm.qVq = max;

  }else if(CurrentQPID.controlOutput < -max){
	ParkParm.qVq = -max;
  }else{

  	ParkParm.qVq = CurrentQPID.controlOutput;
  }

};


// Executes one PID iteration for speed loops Omega

void OmegaControl(void)
{
  WPID.controlReference = CtrlReferences.qWRef;
  WPID.measuredOutput = VelocityParm.Velocity;

  PID2(&WPID);
  WPIDError =  WPID.controlState[1];
  CtrlReferences.qIqRef = WPID.controlOutput;
}

void ZeroRegulators(void)
// Zero PID regulator status 
{
  // zero integral sums
  PID2Init(&CurrentQPID);

  PID2Init(&CurrentDPID);

  PID2Init(&WPID);
  // 
  
// TODO: reset other regulators
}
 
void SetupControlParameters(void)
//
// PID control loop parameter inizialization
//
{


  CurrentDPID.IDPMCoefficients = CurrentDIDPMCoefficient;   
    // Set up pointer to controller history samples
  CurrentDPID.controlState = CurrentDState; 
  
  ControllerSetCurrentDPIDParm( KPId,  KIId,  KDId, KMId );
  
 // saturation values  
 //  PIParmD.qOutMax = DOUTMAX;
 // PIParmD.qOutMin = -PIParmD.qOutMax;
  
 /* 
  PIParmQ.qOutMax = QOUTMAX;
  PIParmQ.qOutMin = -PIParmQ.qOutMax;
*/



  CurrentQPID.IDPMCoefficients = CurrentQIDPMCoefficient;   
    // Set up pointer to controller history samples
  CurrentQPID.controlState = CurrentQState; 

  ControllerSetCurrentQPIDParm( KPIq,  KIIq,  KDIq, KMIq);

/*

  PIParmW.qOutMax = WOUTMAX;
  PIParmW.qOutMin = -PIParmW.qOutMax;
*/


  WPID.IDPMCoefficients = WIDPMCoefficient;   
    // Set up pointer to controller history samples
  WPID.controlState = WState; 

  ControllerSetWPIDParm( KPWq,  KIWq,  KDWq,  KMWq);

  //
  // init Torque Sensored Loop parameters:
  //  data buffer pointers and calc internal (scrambled) abc coeffs from PID coeffs
  //
  {
    fractional kCoeffs[3];

    // setup PID memory buffers
    // Set up pointer to derived coefficients
    TorqueSensorPID.IDPMCoefficients = TorqueIDPMCoefficient;    
    // Set up pointer to controller history samples
    TorqueSensorPID.controlState = TorqueState;
    
    // Clear the controler history and the controller output
    PID2Init(&TorqueSensorPID);
    
    // Set P gain
    kCoeffs[0] = KPTSensq;
    // Set I gain
    kCoeffs[1] = KITSensq;
    // Set D gain
    kCoeffs[2] = KDTSensq;

  
    // Set initial Reference to zero 
    TorqueSensorPID.controlReference = Q15(0);
    // Set initial Measured value to zero.     
    TorqueSensorPID.measuredOutput = Q15(0);

    // Further multiplicative coefficient *after* PID elaboration (scaling) 
    TorqueGain=1;
  }
}

#endif
