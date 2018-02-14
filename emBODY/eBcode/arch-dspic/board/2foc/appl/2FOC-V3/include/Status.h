//
// Status of the System
//

#ifndef __STATUS_H
#define __STATUS_H


//
// Flags for various Stuatus of the System
//
typedef union
{
  //
  // control status
  //

struct{
  
  /*
  // Indicates if motor is running in open or closed loop
  unsigned OpenLoop:1;
  // enables Torque mode when running closed loop
  unsigned TorqueControl:1;
  // enables Speed mode when running closed loop 
  unsigned SpeedControl:1;
  // enables position mode when running closed loop
  unsigned PositionControl:1;        //4
  // enables torque loop mode with FTS feedback when running closed loop
  unsigned TorqueSensorLoop:1;
  
  // Start the loop and indicates if the selected control is running
  unsigned LoopRunning:1;
  // PWM Signals are disabled
  unsigned PWM_is_disabled:1;

  //
  // various warnings and errors 
  //
  // enable I2T check
  unsigned EnableI2T:1;              //8
  */

  unsigned CurrentLoop:1;
  unsigned SpeedLoop:1;
  unsigned dummy:6;

  //unsigned char controlMode;


  // enable overvoltage check
  unsigned EnableOverVoltage:1;
  // enable over speed check
  unsigned EnableOverSpeed:1;
  // Bit that enables Voltage Ripple Compensation
  unsigned EnVoltRipComp:1;
  // Bit that enables Dead Time Compensation
  unsigned EnDeadTComp:1;           //12

  // Parameter init stasus
  // if all those parameters are not set it's not possible to change status to 
  // Idle to Running 
  unsigned PIDisInitialized:1;
  unsigned MaxIisInitialized:1;
  unsigned MaxOmegaisInitialized:1;
  unsigned MaxTorqueisInitialized:1; //16

  // Indicate that initial rotor alignment is in progress
  unsigned InitialRotorAlignmentInProgress:1;
  // Indicate that initial rotor alignment has been successfully completed
  unsigned InitialRotorAlignmentComplete:1;
  // Indicate that current ramping during rotor alignment is in progress 
  unsigned CurrentRampUPInProgress:1;
  unsigned CurrentRampDOWNInProgress:1; //20
  // Indicate currrent ramp successfully completed
  unsigned CurrentRampUPComplete:1; 
  unsigned CurrentRampDOWNComplete:1;  //22

  //unsigned char iCubProtControlMode;
};

  unsigned char b[3];

} __attribute__((__packed__)) tSysStatus;

//extern char iCubProtControlMode;

#endif
