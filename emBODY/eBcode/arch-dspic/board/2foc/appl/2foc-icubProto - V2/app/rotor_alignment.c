#include <p33FJ128MC802.h>
#include <dsp.h>

#include "UserParms.h"
#include "rotor_alignment.h"
#include "encoder.h"
#include "Faults.h"
#include "velocity.h"
#include "Park.h"
#include "controller.h"

// Used when performing rotor alignment with dhes as AUX encoder.
// It stores initial approx value for ESPP to make a check
// against the final (hopefully more precise) ESPP value
SFRAC16 rotor_alignment_dhes_approx_espp;

// current rotor position for rotor alignment strategis that
// need to move the rotor to find the position
static long int align_position = 1;

// flag to indicate the rotor alignment with dhes in aux mode
// has found the required transition
int rotor_alignment_encoder_dhes_aux_found;

// auxiliary function used by strategis that meed to move
// the rotor (to start it).  
void RampUPCurrent()
// increase gradually the current generated
// overriding the Vq variable from 0 to IraqVq_max
{
  static int rampUstatus;
  static int ramp_steepness;

  // set ramp initial value
  if( False == SysStatus.CurrentRampUPInProgress )
  {
    IraqVq = 0;
    rampUstatus = IraqVq_max;
    ramp_steepness = 0;
  }

  // set the ramping flag
  SysStatus.CurrentRampUPInProgress = True;

  if (0 == rampUstatus)
  {
    // Ramp finished 
    SysStatus.CurrentRampUPInProgress = 0;
    SysStatus.CurrentRampUPComplete = 1;
  }
  else
  {
    if(ramp_steepness == Ira_ramp_steepness) {

      // update ramp
      rampUstatus--;
      IraqVq++; 
      ramp_steepness = 0;
    }else{
      ramp_steepness++;
    }
  }
}

// auxiliary function used by strategis that meed to move
// the rotor (to stop it).  
void RampDOWNCurrent()
// decrease gradually the current generated
// overriding the Vq variabls from IraqVq_max, IRA_VQ to 0
{
  static int rampDstatus;
  static int ramp_steepness;
  // set ramp initial value
  if( False == SysStatus.CurrentRampDOWNInProgress )
  {
   
    IraqVq = IraqVq_max;
    rampDstatus = IraqVq_max;
    ramp_steepness = 0;
  }

  // set the ramping flag
  SysStatus.CurrentRampDOWNInProgress = True;

  if (0 == rampDstatus)
  {
    // Ramp down finished 
    SysStatus.CurrentRampDOWNInProgress = 0;
    SysStatus.CurrentRampDOWNComplete = 1;
  }
  else
  {
    if(ramp_steepness == Ira_ramp_steepness) {
      // update ramp
      rampDstatus--;
      IraqVq = rampDstatus; 
      ramp_steepness = 0;
    }else{
      ramp_steepness++;
    }
  }
}


// auxiliary function called internally when the alignemnt is
// complete. This is 
// This is the final step (variables setup) common to all
// rotor alignment strategies
void RotorAlignComputeESPP(SFRAC16 reference_encoder_position,SFRAC16 rotor_alignment_encoder_position)
{
  //if the rotor alignment has been performed with the AUX encoder then the ESPP value
  //must be recalculated for the reference encoder (that is used for control loop).
  //note that the calculation below don't modify the ESPP value if the rotor alignment 
  //and reference encoder have the same value (that is, the rotor alignment encoder is
  //the reference encoder, and the AUX encoder is NOT used for rotor alignment), so no 
  //ifdef is required, and this calculation can be leaved active even when using 
  //reference encoder for rotor alignemnt.
  // Note the computation is performed turning back positions to electrical angle
  Encoder_SyncPulsePosition += rotor_alignment_encoder_position*ELECTRICCYCLESPERENCODERCYCLE - 
								reference_encoder_position*ELECTRICCYCLESPERENCODERCYCLE;
}

// auxiliary function called internally when the alignment is
// complete (including current ramp down).
// This is the final step (variables setup) common to all
// rotor alignment strategies
void RotorAlignFinish(SFRAC16 reference_encoder_position,SFRAC16 rotor_alignment_encoder_position)
{


  // Null current PID status 
  //PIParmD.qdSum = 0;
  //PIParmQ.qdSum = 0;
  ControllerCurrentDPIDClear();
  ControllerCurrentQPIDClear();

  

  // zeroes the turn count;
  Turns = 0;
  
  // update global position-related variables
  UpdatePositionVariables(reference_encoder_position);
  
  // update aux position variable for velocity calc
  Previous_position_for_velocity_calc = Current_position;

  // reinitialize velocity calculation SW module
  VelocityInit();
  VelocityParm.Velocity = 0;

  // TODO: azzerare lo stato  del regolatore di velocita' 

  // Rotor alignment completed 
  SysStatus.InitialRotorAlignmentComplete = 1;
  SysStatus.InitialRotorAlignmentInProgress = 0;

}

// initiate rotor alignment with the blind one electrical
// sweep strategy.
// This is encoder independent and should work with
// every encoder 
void RotorAlignStartElectricalSweep(SFRAC16 reference_encoder_position,SFRAC16 rotor_alignment_encoder_position)
{

  // in Openloop the angle is not read from the encoder but forcedly increased
  // also during initial rotor alignment the movment are performed in open loop 
   
  // init starting value
  ParkParm.qAngle = 0x01; // Q15(0.999999);
  // init torque reference
  ParkParm.qVd = 0;
  ParkParm.qVq = 0;

  SysStatus.CurrentRampUPComplete = 0;
 
}

// initiate rotor alignement with QE encoder specific
// skills.. This means using the INDEX signal from QE
// encoder..
void RotorAlignStartQE(SFRAC16 reference_encoder_position,SFRAC16 rotor_alignment_encoder_position)
{
  // use index signal to perform rotor alignment then
  // we need to temporarly turn the QEP hw configuration to use index
  // signal to reset counter
  QEPSetUseZero(1);

  // in Openloop the angle is not read from the encoder but forcedly increased
  // also during initial rotor alignment the movment are performed in open loop 
  
  // init starting value
  ParkParm.qAngle = 0x01; // Q15(0.999999);
  // init torque reference
  ParkParm.qVd = 0;
  ParkParm.qVq = 0;

  SysStatus.CurrentRampUPComplete = 0;
 
}

// initiate rotor alignement with DHES (as AUX encoder!) specific
// skills.. This means we will move rotor waiting for a DHES
// transition..
void RotorAlignStartDHESAux(SFRAC16 reference_encoder_position,SFRAC16 rotor_alignment_encoder_position)
{
  
  // in Openloop the angle is not read from the encoder but forcedly increased
  // also during initial rotor alignment the movment are performed in open loop 
  
  // init starting value. Start with the best available align value to avoid
  // rotor jumping.
  // We have the dhes-emulated encoder value (mechanical angle). Turn back to electrical turn 
  ParkParm.qAngle = rotor_alignment_encoder_position*ELECTRICCYCLESPERENCODERCYCLE + ENCODER_DHES_SYNCPULSEPOSITION;//0x01; 
  // init torque reference
  ParkParm.qVd = 0;
  ParkParm.qVq = 0;


  SysStatus.CurrentRampUPComplete = 0;
  rotor_alignment_encoder_dhes_aux_found = 0;
  // set initial ESPP. This is an approx value for DHES encoder
  Encoder_SyncPulsePosition = ENCODER_DHES_SYNCPULSEPOSITION;
  // calculate the approx value for QE.
  // This is used when the procedure is finished (and a new, hopefully
  // better value has been found), to perform an additional check:
  // The values must not differ more than 60 degrees 
  RotorAlignComputeESPP(reference_encoder_position, rotor_alignment_encoder_position);
  rotor_alignment_dhes_approx_espp = Encoder_SyncPulsePosition;

  // set again initial ESPP for DHES, in order to make it possible
  // to calc (again) the ESPP for QE by calling "RototAlignComputeESPP"
  // when the procedure will be finished
  Encoder_SyncPulsePosition = ENCODER_DHES_SYNCPULSEPOSITION;
  
}


// initiate rotor alignement with ABSOLUTE encoder specific
// skills.. This means trusting the absolute position
// read by the encoder and asjusting internal variables
// to get aligned with current well-known position..
void RotorAlignStartABS(SFRAC16 reference_encoder_position,SFRAC16 rotor_alignment_encoder_position)
// Initiate rotor alignment with ABSolute encoder (AS504x or TLE)
{
  // TODO: pensaci un poco bene ma mi pare che in ogni caso Encoder_SyncPulsePosition va messo =0
  Encoder_SyncPulsePosition = 0;
  
  RotorAlignComputeESPP(reference_encoder_position, rotor_alignment_encoder_position);
  RotorAlignFinish(reference_encoder_position, rotor_alignment_encoder_position);
  
}

#ifdef ENCODER_DHES
// initiate rotor alignement with DHES (as main encoder!) 
// specific skills.. 
// This means trusting the absolute position read by the
// DHES and asjusting internal variables to get aligned 
// with current well-known position..
void RotorAlignStartDHES(SFRAC16 reference_encoder_position,SFRAC16 rotor_alignment_encoder_position)
{

  Encoder_SyncPulsePosition = ENCODER_DHES_SYNCPULSEPOSITION;

  // Rotor alignment completed 
  RotorAlignComputeESPP(reference_encoder_position, rotor_alignment_encoder_position);
  RotorAlignFinish(reference_encoder_position, rotor_alignment_encoder_position);

}
#endif

extern unsigned int temp1,temp2;

#ifdef ENCODER_AUX_DHES
// This function is for the blind electrical sweep strategy.
// Once this strategy has been initiated, calling this function
// makes it advance by one step in its execution
void RotorAlignStepDHESAux(SFRAC16 reference_encoder_position,SFRAC16 rotor_alignment_encoder_position)
// Rotor alignment is done spinning the field for a complete electrical turn and dragging
// the rotor in position.
// To avoid spiking during turn on and off the current is ramped on and off
//  
// TODO: verificare con l'encoder l'aggancio del rotore 
//    catturare lo zero encoder per aumentare la precisione della fasatura
{
  static SFRAC16 prev_hall_sensor;
  unsigned int check_tmp_val;
  SFRAC16 hall_sensor;
  
  // switch back to electrical cycle	 
  hall_sensor = rotor_alignment_encoder_position % (0xFFFFU/ELECTRICCYCLESPERENCODERCYCLE);


  // SysStatus.CurrentRampUPComplete is 0 at startup;
  // Ramp-up current
  if(0 == SysStatus.CurrentRampUPComplete)
  {

    RampUPCurrent();

  }else{
    // current ramp up completed. Start dragging the rotor
      
    // If we got DHES transition. Ramp down, then finish.
    // To check if we did a complete electrical turn, we are
    // very tolerant because initially the rotor might nor turn
    // (until we move the field enough)
  	if((rotor_alignment_encoder_dhes_aux_found) || (0x17000 == align_position )){

      if(0 == SysStatus.CurrentRampDOWNComplete){
        // ramp down the current
        RampDOWNCurrent();

      }else{ 
        // if we got here because the full turn has been done,
        // without found any transition
  		  if(!rotor_alignment_encoder_dhes_aux_found){
          SysError.RotorAlignmentFault = 1;
          FaultConditionsHandler();
        }else{
          // current ramped down successfully. Procedure finished.
          RotorAlignFinish(reference_encoder_position, rotor_alignment_encoder_position);
		    }
      }

    }else{ // transition not found yet

      // continue rotation by increasing angle
      ParkParm.qAngle += Ira_direction;
      align_position++;


	  
      // Transition not found since last round.
      // Check if we got one right now. We pretend the considered DHES
      // to have changed state (from zero) to 1

      if ((prev_hall_sensor == 0) && (hall_sensor != 0)){  
        // We got the transition right now, do computation of ESPP with
        // current encoder values.

        RotorAlignComputeESPP(reference_encoder_position, rotor_alignment_encoder_position);
        
        // Check the just computed value by comparing with the initial approx value.
        // They should not differ by more than 60 degrees (plus some tolerance..), 
        // otherwise we should not trust the result.
        check_tmp_val = abs(rotor_alignment_dhes_approx_espp - Encoder_SyncPulsePosition);
        // 60 deg plus some tolerance -> about 70 deg.
        // This check avoid the rotor to turn backward with strong torques.
        // It is still possible that (assuming a problem that caused wrong value happened) 
        // the angle falls more than 90 deg over the "true correct" value, but
        // hopefully the torque will be not so much.
        // Indeed, assuming the ESPP has been found wrong, this check
        // should be able to detect at least the conditions in which the
        // rotor will turn backward with strong torques..
        if(check_tmp_val > (0x2aaa + 0x700)){
          SysError.RotorAlignmentFault = 1;
          FaultConditionsHandler();
        }else{
          // we found the value for ESPP
          rotor_alignment_encoder_dhes_aux_found = 1;
        }

       
 	    }       
    }
  } 
  prev_hall_sensor = hall_sensor;
}

#endif

// This function is for the blind electrical sweep strategy.
// Once this strategy has been initiated, calling this function
// makes it advance by one step in its execution
void RotorAlignStepElectricalSweep(SFRAC16 reference_encoder_position,SFRAC16 rotor_alignment_encoder_position)
// Rotor alignment is done spinning the field for a complete electrical turn and dragging
// the rotor in position.
// To avoid spiking during turn on and off the current is ramped on and off
//  
// TODO: verificare con l'encoder l'aggancio del rotore 
//    catturare lo zero encoder per aumentare la precisione della fasatura
{
  SFRAC16 mechanical_angle;
  
  // SysStatus.CurrentRampUPComplete is 0 at startup;
  // Ramp-up current
  if(0 == SysStatus.CurrentRampUPComplete)
  {
    RampUPCurrent();
  }
  else
  {
    // current ramp up completed start dragging the rotor

    // if the rotor did a complete electric turn..
    // To check if we did a complete electrical turn, we are
    // very tolerant because initially the rotor might nor turn
    // (until we move the field enough)
    if( 0x17000 == align_position ){
    
      
        // SysStatus.CurrentRampDOWNComplete is 0 at startup;
      if(0 == SysStatus.CurrentRampDOWNComplete)
      {
        // ramp down the current
        RampDOWNCurrent();
      }
      else
      { 
        // current ramped down successfully

        // GENERIC 90 deegree for rotor alignment without zero signal
        // adjust Encoder_SyncPulsePosition in order to obtain position 90deg from EncoderPosition()
        // AM was 0x3f00 before merge with linear motor code
        mechanical_angle = rotor_alignment_encoder_position * ELECTRICCYCLESPERENCODERCYCLE;
        Encoder_SyncPulsePosition = 0x3f00 - mechanical_angle; 
        // a complete turn has completed (hopefully)
        RotorAlignComputeESPP(reference_encoder_position, rotor_alignment_encoder_position);
        RotorAlignFinish(reference_encoder_position, rotor_alignment_encoder_position);
      
      }
    }
    else
    {
      // complete rotation by increasing angle
      ParkParm.qAngle += Ira_direction;
      align_position++;

    }
  }
}

#ifdef ENCODER_QE

// This function is for the QE-zero search strategy.
// The index-signal is used to reset the encoder to zero. Then
// a well-known values is assigned to EncoderSyncPulsePosition.
// Once this strategy has been initiated, calling this function
// makes it advance by one step in its execution
void RotorAlignStepQE(SFRAC16 reference_encoder_position,SFRAC16 rotor_alignment_encoder_position)
// Rotor alignment is done spinning the field for a complete electrical turn and dragging
// the rotor in position.
// To avoid spiking during turn on and off the current is ramped on and off
//  
// TODO: verificare con l'encoder l'aggancio del rotore 
//    catturare lo zero encoder per aumentare la precisione della fasatura
{
  

  static int zero_found = 0;

  // SysStatus.CurrentRampUPComplete is 0 at startup;
  // Ramp-up current
  if(0 == SysStatus.CurrentRampUPComplete)
  {
    RampUPCurrent();
  }
  else
  {
    // current ramp up completed start dragging the rotor
    // if the rotor completed a mechanic turn plus some extra pulses
    // to be really sure it crossed the zero
    // To check if we did a complete electrical turn, we are
    // very tolerant because initially the rotor might nor turn
    // (until we move the field enough)
    if( zero_found  || ((0x10000 * ELECTRICCYCLESPERENCODERCYCLE + 0x7000) == align_position)){


      // a complete turn has completed or the zero has been found (hopefully)

      // start ramping down the current
      // SysStatus.CurrentRampDOWNComplete is 0 at startup;
      if(0 == SysStatus.CurrentRampDOWNComplete)
      {
        // ramp down the current
        RampDOWNCurrent();
      }
      else
      {  
        // current ramped down successfully

        // adjust Encoder_SyncPulsePosition in order to make EncoderPosition return the 
        // right (motor dependant) value in the zero position.
        Encoder_SyncPulsePosition = (SFRAC16)QE_ZERO_SYNCPULSEPOSITION;
        // do not trust the index signal for foc loop: use
        // maxcount to recirculate QEP to zero
        QEPSetUseZero(0);


        if(0 == zero_found){
          SysError.RotorAlignmentFault = 1;
          FaultConditionsHandler();
        }
        RotorAlignComputeESPP(reference_encoder_position, rotor_alignment_encoder_position);
        RotorAlignFinish(reference_encoder_position, rotor_alignment_encoder_position);
      }
    }
    else
    {
      // complete rotation by increasing angle
      ParkParm.qAngle += Ira_direction;
      align_position++;

      if(QEPZeroCrossed()){
        zero_found = 1;
      }

    }
  }
}
#endif 

// Perform one step for the rotor align procedure.
// This is good for all encoders and all strategies
// that needs multi-step alignement procedure..
// Typically this will make the rotor advance by 1 step.
// With some strategies (for example absolute encoder with
// rotative motors), all is done in one initial step and
// there is no need to do any rotor movement or any other
// step-by-step procedeure. In this case this function 
// will not be ever called 
void RotorAlignStep(SFRAC16 reference_encoder_position,SFRAC16 rotor_alignment_encoder_position)
{
// if the blind electrical sweep procedure is
// requested, then do it, despite the encoder type
#ifdef ALIGN_ROTOR_WITH_ONE_ELECTRICAL_SWEEP
  RotorAlignStepElectricalSweep(reference_encoder_position, rotor_alignment_encoder_position);
#else 
// Otherwise use encoder-specific procedure (if any)

// procedure for QE encoders
#if ((defined (ENCODER_QE)     && !defined(ROTOR_ALIGNMENT_USE_AUX_ENCODER)) || \
	(defined (ENCODER_AUX_QE) &&  defined(ROTOR_ALIGNMENT_USE_AUX_ENCODER))) 
  RotorAlignStepQE(reference_encoder_position, rotor_alignment_encoder_position);
#elif (defined (ENCODER_AUX_DHES) && defined(ROTOR_ALIGNMENT_USE_AUX_ENCODER)) 
// DHES as main encoder does not need stepping. The ESPP is just
// assigned to the right value.
// When DHES is the AUX encoder, we search for a transition, then we compute
// the ESPP based on the DHES value and MAIN encoder value. In this way we use
// the best MAIN encoder value (that otherwise will change in the DHES dead-range)

  RotorAlignStepDHESAux(reference_encoder_position, rotor_alignment_encoder_position);
#endif

// Other encoders currently does not need anything.
// If one day, one new encoder shall need something, 
// then most likely code should be added here..

#endif
}

// Initiate rotor alignment.
// (Align encoder index with maximum flux angle)
// This is the start function for all encoders and
// all alignment strategies.
void RotorAlignStart()
{
  
  SFRAC16 reference_encoder_position,aux_encoder_position, rotor_alignment_encoder_position;

  EncoderTriggerSample();
  EncoderWaitUpdate();

  // read value from reference (for control loop) encoder
  reference_encoder_position = EncoderPosition(0);


#ifdef ENCODER_ENABLE_AUX
  // read Meccanical postion [-1..0.9999] from aux encoder
  aux_encoder_position = EncoderPosition(1);
#else
  // to make it possible to avoid some #ifdef in the code.
  // when there is no AUX encoder emulate it with the same value of
  // the reference encoder. In this way some portions of code
  // can use the encoder aux value without worrying if it
  // really exists or not..
  aux_encoder_position = reference_encoder_position;
#endif

  // assign value of rotor_alignment_encoder_position
  // by using value from reference or aux encoder 
  // depending by the configuration
#ifdef ROTOR_ALIGNMENT_USE_AUX_ENCODER
  rotor_alignment_encoder_position = aux_encoder_position;
#else
  rotor_alignment_encoder_position = reference_encoder_position;
#endif


  // Assert that the rotor init is in progress.
  // for some rotor alignment strategies (for example when using Incremental encoders) 
  // the phase is completed when a complete open loop electrical 
  // angle sweep is completed   
  SysStatus.InitialRotorAlignmentInProgress = 1;
  align_position = 1;

  // if the blind electrical turn strategy has been
  // selected, then go that way despite the
  // encoder type.
#ifdef ALIGN_ROTOR_WITH_ONE_ELECTRICAL_SWEEP
  RotorAlignStartElectricalSweep(reference_encoder_position, rotor_alignment_encoder_position);
#else

// if the blind electrical turn strategy has been NOT selected
// then depending by the encoder type (absolute, qe, hall) we 
// uses different strategies

#ifndef ROTOR_ALIGNMENT_USE_AUX_ENCODER
 
#if defined(ENCODER_ABS) || defined(ENCODER_TLE)
  RotorAlignStartABS(reference_encoder_position, rotor_alignment_encoder_position);
#elif defined(ENCODER_QE)
  RotorAlignStartQE(reference_encoder_position, rotor_alignment_encoder_position);
#elif defined (ENCODER_DHES)
  RotorAlignStartDHES(reference_encoder_position, rotor_alignment_encoder_position);
#endif

#else

#if defined(ENCODER_AUX_ABS) || defined(ENCODER_AUX_TLE)
  RotorAlignStartABS(reference_encoder_position, rotor_alignment_encoder_position);
#elif defined(ENCODER_AUX_QE)
  RotorAlignStartQE(reference_encoder_position, rotor_alignment_encoder_position);
#elif defined (ENCODER_AUX_DHES)
  // rotor alignment with DHES as AUX encoder is different than rotor alignment with
  // DHES as MAIN encoder:
  // When DHES is main encoder, there is no rotor alignment at all. ESPP is just
  // assigned with the right value.
  // When DHES is the AUX encoder, we search for a transition, then we compute
  // the ESPP based on the DHES value and MAIN encoder value. In this way we use
  // the best MAIN encoder value (that otherwise will change in the DHES dead-range)
  RotorAlignStartDHESAux(reference_encoder_position, rotor_alignment_encoder_position);
#endif

#endif // ROTOR_ALIGNMENT_USE_AUX_ENCODER

#endif //ALIGN_ROTOR_WITH_ONE_ELECTRICAL_SWEEP 
}
