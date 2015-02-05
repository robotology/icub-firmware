//
// DSP402 State Machine
//

#ifndef __DS402_H
#define __DS402_H

//
// DS402 state machine status word and control word 
// as defined in 'CiA DSP 402 V 2.0' pag. 45
//
typedef union uControlword
{
  struct 
  {
    // Control transition 3
    unsigned SwitchOn:1;
    // 
    unsigned EnableVoltage:1;
    // Control transition 7,10,11
    unsigned QuickStop:1;
    // Control transition 4,16
    unsigned EnableOperation:1;
    // Has different meanings depending on the current profile 
    unsigned OperationModeSpecific:3;
    // Control transition 15
    unsigned FaultReset:1;
    //  
    unsigned Halt:1;
    //
    unsigned Reserved:2;
    // 
    unsigned ManufacturerSpecific:5;
  } Flags;
  // Permits to access the whole structure data in byte/word/array fashon 
  unsigned int W[1];
} tControlword;

typedef union uStatusword
{
  struct 
  {
    // High voltage may be applied to the drive.
    // The drive parameters may be changed.
    // The drive function is disabled.
    unsigned ReadyToSwitchOn :1;
    // High voltage has been applied to the drive.
    // The drive parameters may be changed.
    // The drive function is disabled.
    unsigned SwitchedOn :1;
    // No faults have been detected.
    // The drive function is enabled and power is applied to the motor.
    // The drive parameters may be changed.
    // (This corresponds to normal operation of the drive.)
    unsigned OperationEnabled :1;
    // The drive parameters may be changed.
    // A fault has occurred in the drive.
    // The quick stop function is being executed.
    // The drive function is enabled and power is applied to the motor.
    unsigned Fault :1;
    // High voltage is applied to the drive
    unsigned VoltageEnabled :1;
    // The drive parameters may be changed.
    // The quick stop function is being executed.
    // The drive function is enabled and power is applied to the motor.
    // o If the quick stop option code is switched to 5 (stay in the state QUICK STOP
    // ACTIVE), you can’t leave the state QUICK STOP ACTIVE, but you can
    // transmit to the state OPERATION ENABLE with the command ‘Enable
    // Operation’. DEVICE CONTROL Drives and Motion Control CiA DSP 402 V 2.047
    unsigned QuickStop :1;
    // Drive initialization is complete.
    // The drive parameters have been set up.
    // Drive parameters may be changed.
    // The drive is disabled.
    unsigned SwitchOnDisabled :1;
    // A drive warning is present. The cause means no error but a state that has to be
    // mentioned, e.g. temperature limit, communication QOS, ecc... 
    unsigned Warning :1;
    //
    unsigned ManufacturerSpecific :1;
    // If set parameters may be modified via the CAN, and the drive executes the
    // content of a command message. 
    // If the bit remote is reset, then the drive is in local mode and will not
    // execute the command message. The drive may transmit messages containing valid actual values like
    // a position actual value, depending on the actual drive configuration.
    unsigned Remote :1;
    // set-point has been reached
    // The change of a target value alters this bit.
    // If halt occurred and the drive has halted then this bit is set too.
    unsigned TargetReached :1;
    // Indicates that an internal limitation is active (e.g. position range limit).
    unsigned InternalLimitActive :1;
    //
    unsigned OperationModeSpecific :2;
    //
    unsigned ManufacturerSpecific_ :2;
  } Flags;
  // Permits to access the whole structure data in byte/word/array fashon 
  unsigned int W[1];
} tStatusword;


//
// Masks for DS402
//

// status word:
// xxxx xxxx x0xx 0000 Not ready to switch on
// xxxx xxxx x1xx 0000 Switch on disabled
// xxxx xxxx x01x 0001 Ready to switch on
// xxxx xxxx x01x 0011 Switched on
// xxxx xxxx x01x 0111 Operation enabled
// xxxx xxxx x00x 0111 Quick stop active
// xxxx xxxx x0xx 1111 Fault reaction active
// xxxx xxxx x0xx 1000 Fault

#endif
