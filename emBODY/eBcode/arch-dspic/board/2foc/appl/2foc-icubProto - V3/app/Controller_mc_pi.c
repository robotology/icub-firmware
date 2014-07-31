//   
//  PID controllers and related rouines
//  used for Current Velocity and Position control
//

#include "Controller.h"
#include "UserParms.h"

//
//OLD PI - Microchip original implementation
//
#ifdef OLD_PI

//
// PI regulators 
//
// Structure definition for Flux component of current, or Id
tPIParm PIParmD;
// Structure definition for Torque component of current, or Iq
tPIParm PIParmQ;
// Structure definition for Speed (Omega) regulator 
tPIParm PIParmW;

fractional TorqueAbcCoefficient[3] __attribute__ ((section (".xbss, bss, xmemory")));
// A.M. ymemory on this device is addressable only with the large data model!
fractional TorqueControlHistory[3] __attribute__ ((section (".ybss, bss, ymemory")));

tPID TorqueSensorPID;

void ControllerCurrentDPIDClear()
{
	PIParmD.qdSum = 0;
    //init PIDs
    InitPI(&PIParmD);
}

void ControllerCurrentQPIDClear()
{
	PIParmQ.qdSum = 0;
    //init PIDs
    InitPI(&PIParmQ);
}


void ControllerSetCurrentDPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d, SFRAC16 max)
{
	PIParmD.qKp = p;
	PIParmD.qKi = i;
//	PIParmD.qKc = d;

  	ControllerCurrentDPIDClear();
}

void ControllerSetCurrentQPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d, SFRAC16 max)
{
	PIParmQ.qKp = p;
	PIParmQ.qKi = i;
//	PIParmQ.qKc = d;
	
    
    ControllerCurrentQPIDClear();
}

void ControllerGetCurrentDPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max)
{
	*p = PIParmD.qKp;
	*i = PIParmD.qKi;
    *d = 0;
//	*d = PIParmD.qKc;
    *max = 0x7fff; // no lim
}

void ControllerGetCurrentQPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max)
{
	*p = PIParmQ.qKp;
	*i = PIParmQ.qKi;
    *d = 0;
//	*d = PIParmQ.qKc;
    *max = 0x7fff; // no lim
}

void ControllerSetWPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d, SFRAC16 max)
{
	PIParmW.qKp = p;
	PIParmW.qKi = i;
//	PIParmW.qKc = 0;
	PIParmW.qdSum = 0;
    //init PIDs
    InitPI(&PIParmW);
}

void ControllerGetWPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d, SFRAC16 *max)
{
	*p = PIParmW.qKp;
	*i = PIParmW.qKi;
	*d = 0;
    *max = 0x7fff; // no lim

}

void ZeroRegulators(void)
// Zero PID regulator status
{
  // zero integral sums
  PIParmD.qdSum = 0;
  InitPI(&PIParmD);

  PIParmQ.qdSum = 0; 
  InitPI(&PIParmQ);

  // speed
  PIParmW.qdSum = 0;
  InitPI(&PIParmW);
  // 
  
// TODO: reset other regulators
}


void IdIqControl(void)
// Executes one PI iteration for current loops Id,Iq,
{
  SFRAC16 qVdSquared = 0; // Vd and Vq vector limitation variables
    // This variable is used to know what is left from the VqVd vector
    // in order to have maximum output PWM without saturation.
  // PI control for D
  PIParmD.qInMeas = ParkParm.qId;
  PIParmD.qInRef  = CtrlReferences.qIdRef;

  CalcPI(&PIParmD);

  ParkParm.qVd = PIParmD.qOut;

  // Vector limitation
  // Vd is not limited
  // Vq is limited so the vector Vs is less than a maximum of 95%. 
  // Vs = SQRT(Vd^2 + Vq^2) < 0.95
  // Vq = SQRT(0.95^2 - Vd^2)
  qVdSquared = FracMpy(ParkParm.qVd, ParkParm.qVd);
  PIParmQ.qOutMax = _Q15sqrt(Q15(QOUTMAX*QOUTMAX) - qVdSquared);
  PIParmQ.qOutMin = -PIParmQ.qOutMax;

  // PI control for Q
  PIParmQ.qInMeas = ParkParm.qIq;
  PIParmQ.qInRef  = CtrlReferences.qIqRef;
  CalcPI(&PIParmQ);

  ParkParm.qVq = PIParmQ.qOut;
};


void OmegaControl(void)
// Executes one PI iteration for speed loops Omega
//
{
#ifdef SPEEDLOOPINASSEMBLER  
// call the assembler PID 
  PIParmW.qInMeas = VelocityParm.Velocity;
  PIParmW.qInRef  = CtrlReferences.qWRef;
  CalcPI(&PIParmW);
  CtrlReferences.qIqRef = PIParmW.qOut;
#else
// Versione C del PID 'vagamente' funzionante
  SFRAC16 error, Out, Excess;
  static SFRAC16 U=0;
  static SFRAC16 Sum=0;

  // Choose witch speed measuremet to use for closing the omega loop 
#if !(defined(ENCODER_TLE) || defined(AIE_TLE5012_PARAMS)) 
  // only FW calculation of speed available
  error = CtrlReferences.qWRef - VelocityParm.Velocity;
#else
  // choose between TLE internal or FW calculation of speed
  #ifdef TLE_INTERNAL_SPEED
    // tle provided
    error = CtrlReferences.qWRef - tle_speed;
  #else 
    // FW calculation
    error = CtrlReferences.qWRef - VelocityParm.last_velocity;
  #endif
#endif


  WPIDerror = error;

  U = Sum + (PIParmW.qKp * error);
  if (U > PIParmW.qOutMax)
    Out = PIParmW.qOutMax;
  else 
    if (U < PIParmW.qOutMin)
      Out = PIParmW.qOutMin;
    else
      Out = U;
  Excess = U - Out;
  Sum = Sum + (PIParmW.qKi * error)-(PIParmW.qKc * Excess);

  CtrlReferences.qIqRef  = Out;
#endif
};




void SetupControlParameters(void)
//
// PID control loop parameter inizialization
//
{
  //
  // DQ space current regulators parameters
  //

  // PI Id
  // Proportional
  PIParmD.qKp = KPId;
  // Integral
  PIParmD.qKi = KIId;   
// TODO: cosa rappresenta KC? la componente FF?           
  PIParmD.qKc = KCId;      
  // saturation values  
  PIParmD.qOutMax = Q15(DOUTMAX);
  PIParmD.qOutMin = -PIParmD.qOutMax;
  // zero integral sum 
  PIParmD.qdSum = 0;

  InitPI(&PIParmD);

  // PI Iq
  // Proportional
  PIParmQ.qKp = KPIq;
  // Integral
  PIParmQ.qKi = KIIq;
  PIParmQ.qKc = KCIq;
  PIParmQ.qOutMax = Q15(QOUTMAX);
  PIParmQ.qOutMin = -PIParmQ.qOutMax;
  // zero integral sum 
  PIParmQ.qdSum = 0; 

  InitPI(&PIParmQ);

  //
  // w regulator parameters
  //
  PIParmW.qKp = KPWq;
  PIParmW.qKi = KIWq;
  PIParmW.qKc = KCWq;
  PIParmW.qOutMax = WOUTMAX;
  PIParmW.qOutMin = -PIParmW.qOutMax;
  // zero integral sum
  PIParmW.qdSum = 0;

  InitPI(&PIParmW);

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
