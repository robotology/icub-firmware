//   
//  PID controllers and related rouines
//  used for Current Velocity and Position control
//

#include "Controller.h"
#include "UserParms.h"



/////////////////////////////////////////////////////////////////////////////OLD PI /////////////////////////////////////////////////////////////
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


void ControllerSetCurrentDPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d)
{
	PIParmD.qKp = p;
	PIParmD.qKi = i;
//	PIParmD.qKc = d;

  	ControllerCurrentDPIDClear();
}

void ControllerSetCurrentQPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d)
{
	PIParmQ.qKp = p;
	PIParmQ.qKi = i;
//	PIParmQ.qKc = d;
	
    
    ControllerCurrentQPIDClear();
}

void ControllerGetCurrentDPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d)
{
	*p = PIParmD.qKp;
	*i = PIParmD.qKi;
    *d = 0;
//	*d = PIParmD.qKc;
}

void ControllerGetCurrentQPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d)
{
	*p = PIParmQ.qKp;
	*i = PIParmQ.qKi;
    *d = 0;
//	*d = PIParmQ.qKc;
}

void ControllerSetWPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d)
{
	PIParmW.qKp = p;
	PIParmW.qKi = i;
//	PIParmW.qKc = 0;
	PIParmW.qdSum = 0;
    //init PIDs
    InitPI(&PIParmW);
}

void ControllerGetWPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d)
{
	*p = PIParmW.qKp;
	*i = PIParmW.qKi;
	*d = 0;

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
// TODO: Va spostata all'interno del irq foc e sottocampionata perchè viene interrotta 
//   dal irq foc
// 
{
//
// Execute the velocity control loop
//
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


////////////////////////////////////////////////////////////////////////////////////////NEW PID//////////////////////////////////////////////////////// 

#elif defined(NEW_PID)

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


void ControllerCurrentDPIDClear()
{
	PID2Init(&CurrentDPID);
}

void ControllerCurrentQPIDClear()
{
	PID2Init(&CurrentQPID);
}

void ControllerSetCurrentDPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d)
{
	CurrentDIDPMCoefficient[0] = i;
	CurrentDIDPMCoefficient[1] = d;
	CurrentDIDPMCoefficient[2] = p;
	CurrentDIDPMCoefficient[3] = 16000;

  
    // Clear the controler history and the controller output
    PID2Init(&CurrentDPID);
}

void ControllerSetCurrentQPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d)
{
	CurrentQIDPMCoefficient[0] = i;
	CurrentQIDPMCoefficient[1] = d;
	CurrentQIDPMCoefficient[2] = p;
	CurrentQIDPMCoefficient[3] = 16000;

 
    // Clear the controler history and the controller output
    PID2Init(&CurrentQPID);
}

void ControllerGetCurrentDPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d)
{
	*i = CurrentDIDPMCoefficient[0];
	*d = CurrentDIDPMCoefficient[1];
	*p = CurrentDIDPMCoefficient[2];
}

void ControllerGetCurrentQPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d)
{
	*i = CurrentQIDPMCoefficient[0];
	*d = CurrentQIDPMCoefficient[1];
	*p = CurrentQIDPMCoefficient[2];
}

void ControllerSetWPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d)
{
	WIDPMCoefficient[0] = i;
	WIDPMCoefficient[1] = d;
	WIDPMCoefficient[2] = p;
	WIDPMCoefficient[3] = 16000;

    // Clear the controler history and the controller output
    PID2Init(&WPID);
}

void ControllerGetWPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d)
{
	*i = WIDPMCoefficient[0];
	*d = WIDPMCoefficient[1];
	*p = WIDPMCoefficient[2];
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

  PID2(&CurrentDPID);
  
  DPIDError = CurrentDPID.controlState[1];

  // Vd is not limited
  if(CurrentDPID.controlOutput > Q15(DOUTMAX)){

    ParkParm.qVd = Q15(DOUTMAX);

  }else if(CurrentDPID.controlOutput < -Q15(DOUTMAX)){
	ParkParm.qVd = -Q15(DOUTMAX);
  }else{

  	ParkParm.qVd = CurrentDPID.controlOutput;
  }


 
 
  CurrentQPID.controlReference = CtrlReferences.qIqRef;
  CurrentQPID.measuredOutput = ParkParm.qIq;

  PID2(&CurrentQPID);

  QPIDError =  CurrentQPID.controlState[1];

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
  
  ControllerSetCurrentDPIDParm( KPId,  KIId,  KDId);
  
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

  ControllerSetCurrentQPIDParm( KPIq,  KIIq,  KDIq);

/*

  PIParmW.qOutMax = WOUTMAX;
  PIParmW.qOutMin = -PIParmW.qOutMax;
*/


  WPID.IDPMCoefficients = WIDPMCoefficient;   
    // Set up pointer to controller history samples
  WPID.controlState = WState; 

  ControllerSetWPIDParm( KPWq,  KIWq,  KDWq);

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




///////////////////////////////////////////////////////////////////////////////////////////////////OLD PID ////////////////////////////////////////////////


#else
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

void ControllerSetCurrentDPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d)
{
	CurrentDPIDCoeffs[0] = p;
	CurrentDPIDCoeffs[1] = i;
	CurrentDPIDCoeffs[2] = d;

    //init PIDs
    PIDCoeffCalc(CurrentDPIDCoeffs, &CurrentDPID);    
    // Clear the controler history and the controller output
    PIDInit(&CurrentDPID);
}

void ControllerSetCurrentQPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d)
{
	CurrentQPIDCoeffs[0] = p;
	CurrentQPIDCoeffs[1] = i;
	CurrentQPIDCoeffs[2] = d;

    //init PIDs
    PIDCoeffCalc(CurrentQPIDCoeffs, &CurrentQPID);    
    // Clear the controler history and the controller output
    PIDInit(&CurrentQPID);
}

void ControllerGetCurrentDPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d)
{
	*p = CurrentDPIDCoeffs[0];
	*i = CurrentDPIDCoeffs[1];
	*d = CurrentDPIDCoeffs[2];
}

void ControllerGetCurrentQPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d)
{
	*p = CurrentQPIDCoeffs[0];
	*i = CurrentQPIDCoeffs[1];
	*d = CurrentQPIDCoeffs[2];
}

void ControllerSetWPIDParm(SFRAC16 p,SFRAC16 i,SFRAC16 d)
{
	WPIDCoeffs[0] = p;
	WPIDCoeffs[1] = i;
	WPIDCoeffs[2] = d;

    //init PIDs
    PIDCoeffCalc(WPIDCoeffs, &WPID);    
    // Clear the controler history and the controller output
    PIDInit(&WPID);
}

void ControllerGetWPIDParm(SFRAC16 *p,SFRAC16 *i,SFRAC16 *d)
{
	*p = WPIDCoeffs[0];
	*i = WPIDCoeffs[1];
	*d = WPIDCoeffs[2];
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
  
  ControllerSetCurrentDPIDParm( KPId,  KIId,  KDId);
  
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

  ControllerSetCurrentQPIDParm( KPIq,  KIIq,  KDIq);

/*

  PIParmW.qOutMax = WOUTMAX;
  PIParmW.qOutMin = -PIParmW.qOutMax;
*/


  WPID.abcCoefficients = WAbcCoefficient;   
    // Set up pointer to controller history samples
  WPID.controlHistory = WControlHistory; 

  ControllerSetWPIDParm( KPWq,  KIWq,  KDWq);

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
