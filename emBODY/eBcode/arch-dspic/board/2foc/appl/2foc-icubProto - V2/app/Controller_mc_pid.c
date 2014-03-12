
//   
//  PID controllers and related rouines
//  used for Current Velocity and Position control
//

// Microchip PID (with D) implementation

#include "Controller.h"
#include "UserParms.h"


#ifdef OLD_PID

// this implementation have a neg asm instruction that is known to cause problem
// if its operand is 0x8000 because -(0x8000) is 0x8000...
// it is not known if this can ever happen in this algo or if the algo consider this 
// thing and if it does eventually handle correctly that case

#warning THIS_IMPLEMENTATION_IS_KNOWN_TO_HAVE_A_POTENTIAL_BUG_PLEASE_SEE_COMMENT_ABOVE

SFRAC16 CurrentQAbcCoefficient[3] __attribute__ ((section (".xbss, bss, xmemory")));   
SFRAC16 CurrentQControlHistory[3] __attribute__ ((section (".ybss, bss, ymemory")));
SFRAC16 CurrentQPIDCoeffs[3];

SFRAC16 CurrentDAbcCoefficient[3] __attribute__ ((section (".xbss, bss, xmemory")));   
SFRAC16 CurrentDControlHistory[3] __attribute__ ((section (".ybss, bss, ymemory")));
SFRAC16 CurrentDPIDCoeffs[3];

SFRAC16 WAbcCoefficient[3] __attribute__ ((section (".xbss, bss, xmemory")));   
SFRAC16 WControlHistory[3] __attribute__ ((section (".ybss, bss, ymemory")));
SFRAC16 WPIDCoeffs[3];

fractional TorqueAbcCoefficient[3] __attribute__ ((section (".xbss, bss, xmemory")));
// A.M. ymemory on this device is addressable only with the large data model!
fractional TorqueControlHistory[3] __attribute__ ((section (".ybss, bss, ymemory")));

tPID TorqueSensorPID;
tPID CurrentQPID;
tPID CurrentDPID;
tPID WPID;


void ControllerCurrentDPIDClear()
{
	PIDInit(&CurrentDPID);
}

void ControllerCurrentQPIDClear()
{
	PIDInit(&CurrentQPID);
}

void ControllerSetCurrentDPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d, SFRAC16 max)
{
	CurrentDPIDCoeffs[0] = p;
	CurrentDPIDCoeffs[1] = i;
	CurrentDPIDCoeffs[2] = d;

    //init PIDs
    PIDCoeffCalc(CurrentDPIDCoeffs, &CurrentDPID);    
    // Clear the controler history and the controller output
    PIDInit(&CurrentDPID);
}

void ControllerSetCurrentQPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d, SFRAC16 max)
{
	CurrentQPIDCoeffs[0] = p;
	CurrentQPIDCoeffs[1] = i;
	CurrentQPIDCoeffs[2] = d;

    //init PIDs
    PIDCoeffCalc(CurrentQPIDCoeffs, &CurrentQPID);    
    // Clear the controler history and the controller output
    PIDInit(&CurrentQPID);
}

void ControllerGetCurrentDPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max)
{
	*p = CurrentDPIDCoeffs[0];
	*i = CurrentDPIDCoeffs[1];
	*d = CurrentDPIDCoeffs[2];
    *max = 0x7fff; //no lim
}

void ControllerGetCurrentQPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max)
{
	*p = CurrentQPIDCoeffs[0];
	*i = CurrentQPIDCoeffs[1];
	*d = CurrentQPIDCoeffs[2];
    *max = 0x7fff; //no lim
}

void ControllerSetWPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d, SFRAC16 max)
{
	WPIDCoeffs[0] = p;
	WPIDCoeffs[1] = i;
	WPIDCoeffs[2] = d;

    //init PIDs
    PIDCoeffCalc(WPIDCoeffs, &WPID);    
    // Clear the controler history and the controller output
    PIDInit(&WPID);
}

void ControllerGetWPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max)
{
	*p = WPIDCoeffs[0];
	*i = WPIDCoeffs[1];
	*d = WPIDCoeffs[2];
    *max = 0x7fff; //no lim
}

void IdIqControl(void)
// Executes one PI iteration for current loops Id,Iq,
{
  SFRAC16 qVdSquared = 0; // Vd and Vq vector limitation variables
    // This variable is used to know what is left from the VqVd vector
    // in order to have maximum output PWM without saturation.
  SFRAC16 max;

  CurrentDPID.controlReference = CtrlReferences.qIdRef;
  CurrentDPID.measuredOutput = ParkParm.qId;

  PID(&CurrentDPID);

  DPIDError = CurrentDPID.controlHistory[0];

  if(CurrentQPID.controlOutput > Q15(DOUTMAX)){

    ParkParm.qVd = Q15(DOUTMAX);

  }else if(CurrentQPID.controlOutput < -Q15(DOUTMAX)){
	ParkParm.qVd = -Q15(DOUTMAX);
  }else{

  	ParkParm.qVd = CurrentQPID.controlOutput;
  }

  CurrentQPID.controlReference = CtrlReferences.qIqRef;
  CurrentQPID.measuredOutput = ParkParm.qIq;

  PID(&CurrentQPID);

  QPIDError = CurrentQPID.controlHistory[0];

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

  PID(&WPID);
  WPIDError = WPID.controlHistory[0];
  CtrlReferences.qIqRef = WPID.controlOutput;
}

void ZeroRegulators(void)
// Zero PID regulator status
{
  // zero integral sums
  PIDInit(&CurrentQPID);

  PIDInit(&CurrentDPID);

  PIDInit(&WPID);
  // 
  
// TODO: reset other regulators
}
 
void SetupControlParameters(void)
//
// PID control loop parameter inizialization
//
{


  CurrentDPID.abcCoefficients = CurrentDAbcCoefficient;   
    // Set up pointer to controller history samples
  CurrentDPID.controlHistory = CurrentDControlHistory; 
  
  ControllerSetCurrentDPIDParm( KPId,  KIId,  KDId, KMId);
  
 // saturation values  
 //  PIParmD.qOutMax = DOUTMAX;
 // PIParmD.qOutMin = -PIParmD.qOutMax;
  
 /* 
  PIParmQ.qOutMax = QOUTMAX;
  PIParmQ.qOutMin = -PIParmQ.qOutMax;
*/



  CurrentQPID.abcCoefficients = CurrentQAbcCoefficient;   
    // Set up pointer to controller history samples
  CurrentQPID.controlHistory = CurrentQControlHistory; 

  ControllerSetCurrentQPIDParm( KPIq,  KIIq,  KDIq, KMIq);

/*

  PIParmW.qOutMax = WOUTMAX;
  PIParmW.qOutMin = -PIParmW.qOutMax;
*/


  WPID.abcCoefficients = WAbcCoefficient;   
    // Set up pointer to controller history samples
  WPID.controlHistory = WControlHistory; 

  ControllerSetWPIDParm( KPWq,  KIWq,  KDWq, KMWq);

  //
  // init Torque Sensored Loop parameters:
  //  data buffer pointers and calc internal (scrambled) abc coeffs from PID coeffs
  //
  {
    fractional kCoeffs[3];

    // setup PID memory buffers
    // Set up pointer to derived coefficients
    TorqueSensorPID.abcCoefficients = &TorqueAbcCoefficient[0];    
    // Set up pointer to controller history samples
    TorqueSensorPID.controlHistory = &TorqueControlHistory[0]; 
    
    // Clear the controler history and the controller output
    PIDInit(&TorqueSensorPID);
    
    // Set P gain
    kCoeffs[0] = KPTSensq;
    // Set I gain
    kCoeffs[1] = KITSensq;
    // Set D gain
    kCoeffs[2] = KDTSensq;

    // Derive the a,b, & c coefficients from the Kp, Ki & Kd
    PIDCoeffCalc(&kCoeffs[0], &TorqueSensorPID);

    // Set initial Reference to zero 
    TorqueSensorPID.controlReference = Q15(0);
    // Set initial Measured value to zero.     
    TorqueSensorPID.measuredOutput = Q15(0);

    // Further multiplicative coefficient *after* PID elaboration (scaling) 
    TorqueGain=1;
  }
}



#endif
