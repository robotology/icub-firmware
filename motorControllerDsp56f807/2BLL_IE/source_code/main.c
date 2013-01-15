 /*
 * firmware/controller application.
 *
 */

#include "dsp56f807.h"
#include "options.h"
#include "asc.h"
#include "ti1.h"
#include "pid.h"
#include "calibration.h"   
#include "trajectory.h"
#include "can_interface.h"
#include "serial_interface.h"
#include "ad.h"
#include "can1.h" 
#include "leds_interface.h"
#include "currents_interface.h"
#include "flash_interface.h"
#include "pwm_interface.h"
#include "faults_interface.h"
#include "check_range.h"

#include "encoders_interface.h"
#include "filters.h" 
#include "decoupling.h"
#include "abs_ssi_interface.h"
#include "2bllie_brushless_comm.h" 
#include "phase_hall_sens.h"
#include "pwm_a.h"
#include "pwm_b.h"


byte	_board_ID = 16;	
char    _additional_info [32];
UInt8    mainLoopOVF=0;
word    _build_number = 67;
int     _countBoardStatus[2] ={0,0};
Int16   _flash_version=0; 
UInt8   BUS_OFF=false;
const byte winShift=5;
byte    _my_can_protocol_major = 1;
byte    _my_can_protocol_minor = 1;
bool    _can_protocol_ack = false; 

#ifdef TEMPERATURE_SENSOR
	//temperature sensor variables
	Int16    TempSens[]=	 {0,0};
	UInt8    overtemp[]={0,0};
	UInt8    errortemp[]={0,0};

#endif 

//**********************
// externs
//**********************
extern sDutyControlBL DutyCycle[2];
extern sDutyControlBL DutyCycleReq[2];

//********************
// Local prototypes 
//********************

//void check_range        (byte, Int16, Int32 *);
//void check_range_torque (byte, Int16, Int32 *);
//********************
// Local prototypes 
//********************

#if VERSION == 0x0250
Int16 _version = 0x0250;
#elif VERSION == 0x0251
Int16 _version = 0x0251;
#elif VERSION == 0x0252
Int16 _version = 0x0252;
#elif VERSION == 0x0254
Int16 _version = 0x0254;
#elif VERSION == 0x0255
Int16 _version = 0x0255;
#elif VERSION == 0x0257
Int16 _version = 0x0257;
#elif VERSION == 0x0258
Int16 _version = 0x0258;
#endif

#ifndef VERSION 
#	error "No valid version specified"
#endif


#ifdef ENCODER_SHIFT_11BITS
	#define Filter_Bit(x) ((x>>1)<<1)
#else
	#define Filter_Bit(x) (x)
#endif

/***********************************************************************
	This is the main controller loop.
	sequences of operations:
		- reads from CAN bus or serial port 1.
		- reads encoders (or ADC).
		- computes the control value (a PID in this version).
		- checks limits and other errors.
		- sets PWM
		- does extra functions (e.g. communicate with neighboring cards).
 ***********************************************************************/
void main(void)
{

	Int32 PWMoutput [JN];
	Int32 PWMoutput_old [JN];
	byte i=0;
	byte wi=0;
	byte k=0;
	UInt16 *value=0;
	Int32 t1val=0;
	Int32 PID_R= 2;
	Int32 kpp=1;
	Int16 current_turn=0;
	Int16 print_number=0;
	Int16 real_pos=0;
	byte first_step=0;

#if (VERSION == 0x0351)

	#define winSizeMax 32
	#define initialWindowSize 4
#elif (VERSION == 0x0258)
	#define winSizeMax 4
	#define initialWindowSize 4
#else
	#define winSizeMax 32
	#define initialWindowSize 30
#endif

	byte divJntPos[JN]=INIT_ARRAY(initialWindowSize-1);
	byte divJntVel[JN]=INIT_ARRAY(initialWindowSize-1);
	byte divMotPos[JN]=INIT_ARRAY(initialWindowSize-1);
	byte divMotVel[JN]=INIT_ARRAY(initialWindowSize-1);
	byte headJntPos[JN]=INIT_ARRAY(0);  //current joint pos
	byte tailJntPos[JN]=INIT_ARRAY(0); 
	byte headJntVel[JN]=INIT_ARRAY(0);  //current joint vel
	byte tailJntVel[JN]=INIT_ARRAY(0); 
	byte headMotPos[JN]=INIT_ARRAY(0);  //current motor pos
	byte tailMotPos[JN]=INIT_ARRAY(0); 
	byte headMotVel[JN]=INIT_ARRAY(0);  //current motor vel
	byte tailMotVel[JN]=INIT_ARRAY(0); 
	Int32 jntPosWindow[winSizeMax][JN]; //max window size = winSizeMax
	Int32 jntVelWindow[winSizeMax][JN]; //max window size = winSizeMax
	Int32 motPosWindow[winSizeMax][JN]; //max window size = winSizeMax
	Int32 motVelWindow[winSizeMax][JN]; //max window size = winSizeMax
	Int16 _safeband[JN];	//it is a value for reducing the JOINT limit of 2*_safeband [tick encoder]
#ifdef TEMPERATURE_SENSOR
	byte   TempSensCount1 = 0;
	UInt32 TempSensCount2 = 0;
	byte   temp_sens_status=0;
	overtemp[0]=0;
	overtemp[1]=0;
	errortemp[0]=0;
	errortemp[1]=0;

#endif
	
	/* gets the address of flash memory from the linker */
	_flash_addr = get_flash_addr();
		
	/* enable interrupts */
	setReg(SYS_CNTL, 0);
	
	// IPL channels from 0 to 6 enabled
	// external interrupts IRQA and IRQB disabled
	setRegBits(IPR, 0xFE00); 

	// enable FAULT
	__ENIGROUP (61, 3); 
	#if (VERSION == 0x0254 || VERSION == 0x0258)
	#else
	__ENIGROUP (60, 3);
	#endif	
	// enable SCI
	__ENIGROUP (52, 4);
	__ENIGROUP (53, 4);
	__ENIGROUP (50, 4);
	__ENIGROUP (51, 4);
	
    // enable data flash
	__ENIGROUP (13, 4);
	
	// enable CAN	
	__ENIGROUP (14, 6);
	__ENIGROUP (15, 6);
	__ENIGROUP (16, 6);
	__ENIGROUP (17, 6);
	
	// enable ADCA/ADCB
	__ENIGROUP (55, 6);
	__ENIGROUP (54, 6);
	
	//enable PWM reload 
	__ENIGROUP (59, 7); // PMWA
	
	#if (VERSION == 0x0254 || VERSION == 0x0258)
	#else	
	__ENIGROUP (58, 7); // PWMB
	#endif
	// enable timers
	// TIMER_A
	__ENIGROUP (45, 7); //Timer for the encoder commutation if used
    __ENIGROUP (44, 7); //
	__ENIGROUP (43, 7); //
	__ENIGROUP (42, 4); //TI1 1ms delay main loop
	// TIMER_B

	__ENIGROUP (41, 7); //
	__ENIGROUP (40, 7); //
	__ENIGROUP (39, 7); //
	__ENIGROUP (38, 7);

	// TIMER_C
	__ENIGROUP (37, 1); 
	__ENIGROUP (36, 1);
	__ENIGROUP (35, 1);
	__ENIGROUP (34, 1);
	// TIMER_D
	__ENIGROUP (33, 7); //1ms delay duty cycle
	__ENIGROUP (32, 1);
	__ENIGROUP (31, 1);
	__ENIGROUP (30, 1);

	__EI();
	
	flash_interface_init  (JN);			
	readFromFlash (_flash_addr);  
	if (_version==_flash_version)
	{
		
	}
	else
	{
		writeToFlash(_flash_addr);
	}
	__DI();
	
	#warning "debug"//   ;
	
	__EI();

	init_leds  			  ();
	
	#if (VERSION == 0x0254 || VERSION == 0x0258)
	
	Init_Brushless_Comm	  (1,HALL); 
  	
  	#else 

	Init_Brushless_Comm	  (JN,HALL); 
  	
  	#endif
	can_interface_init    (JN);
	 
#ifdef TORQUE_CNTRL
    init_strain ();
#endif 	

    init_position_abs_ssi ();
#if VERSION ==0x0257 
    init_relative_position_abs_ssi();
#endif 
 
    init_faults           (true,true,true);	 
    
    init_position_encoder ();


	TI1_init 			  ();

//variable init	
	mainLoopOVF=0;
	_count=0;
	
	for(i=0;i<JN;i++)
	{
	_received_pid[i].rec_pid=0;
	}
	
	BUS_OFF=false;
 	#warning "debug"//   ;
	
	//__EI();

	  
//	print_version ();
	
	/* initialization */
	for (i=0; i<JN; i++) _calibrated[i] = false;
	
	/* reset trajectory generation */
	for (i=0; i<JN; i++) abort_trajectory (i, 0);
	
	
#if VERSION !=0x0258
	///////////////////////////////////////
	// reset of the ABS_SSI
	// this is needed because the AS5045 gives the first value wrong !!!
    for (i=0; i<JN; i++)	_position[i]=(Int32) Filter_Bit(get_position_abs_ssi(i));
    for (i=0; i<JN; i++)    _max_real_position[i]=Filter_Bit(4095);
#else 
	_position[0]=0;	
   	_position[1]=(Int32) Filter_Bit(get_position_abs_ssi(1));
    _max_real_position[1]=Filter_Bit(4095);

#endif
 	
	//////////////////////////////////////
	
	/* initialize speed and acceleration to zero (useful later on) */
	for (i=0; i<JN; i++) _position_old[i] = 0;
	for (i=0; i<JN; i++) _speed[i] = 0;
	for (i=0; i<JN; i++) _accel[i] = 0;
	for (i=0; i<JN; i++) _safeband[i] =5; //5 ticks => 1 grado di AEA.
	for (i=0; i<JN; i++) PWMoutput [i] = PWMoutput_old[i] = 0;
	
	/* reset the recursive windows for storage of position and velocity data */
	/* (for velocity and position estimates) */
	for(i=0;i<JN;i++)
	{
		for(wi=0;wi<winSizeMax;wi++)
		{
			jntPosWindow[wi][i]=_position[i];	
			jntVelWindow[wi][i]=0;
			motPosWindow[wi][i]=0;	
			motVelWindow[wi][i]=0;
		}
	}
	
	//set_relative_position_abs_ssi(1,get_absolute_real_position_abs_ssi(1));
	/* main control loop */
	for(_counter = 0;; _counter ++) 
	{
		if (_counter >= CAN_SYNCHRO_STEPS) _counter = 0;
		led3_on
		while (_wait);
		_count=0;
		led3_off
		
// BUS_OFF check
		if (getCanBusOffstatus() )
		{
			#ifdef DEBUG_CAN_MSG
				can_printf("DISABLE BUS OFF");
			#endif	
			for (i=0; i<JN; i++) _control_mode[i]=MODE_IDLE;
			led1_off
		}
		else
			led1_on

// READING CAN MESSAGES
		can_interface();

		
	
	
	    //Position calculation
	    // This is used to have a shift of the zero-cross out of the 
	    // joint workspace
	    //
	    // max_real_position is the limit of the joint starting from 
	    // 4095 and going to decrease this number without zero-cross
	    // untill the joint limit is reached
#if    VERSION == 0x0257 
		_position_old[0]=_position[0]; 
		if(get_error_abs_ssi(0)==ERR_OK)
			_position[0]=Filter_Bit (get_position_abs_ssi(0));
    	_position_old[1]=_position[1];
		if(get_error_abs_ssi(1)==ERR_OK) 
			_position[1]=Filter_Bit (get_position_abs_ssi(1));

		 
#elif VERSION ==0x0255
		_position_old[0]=_position[0];
		_position[0]=Filter_Bit (get_position_abs_ssi(0));
		_position_old[1]=_position[1]; 
		_position[1]=Filter_Bit (get_position_abs_ssi(1));
#elif VERSION ==0x0258
		_position_old[0]=_position[0];
		_position[0]=get_position_encoder(0);//get_position_encoder(0);//Get_Sens1_Status();
		_position[1]=Filter_Bit (get_position_abs_ssi(1));
		_position_old[1]=_position[1]; 

#else
	 	for (i=0; i<JN; i++) 
		{
		_position_old[i]=_position[i];
		if(get_error_abs_ssi(i)==ERR_OK)
		_position[i]=Filter_Bit (get_position_abs_ssi(i));
		
		}
#endif 

		// get_commutations() is used to read the incremental encoder of the motors.
		// the variable _motor_position is then used to estimate the rotor speed and
		// compensate the back-EMF of the motor.
		for (i=0; i<JN; i++) _motor_position[i]=get_position_encoder(i);//get_commutations(i);


///////////////////////////////////////////DEBUG////////////
// ADDED VERSION !=0x0171
#if (VERSION !=0x0254) && (VERSION !=0x0258)
	    for (i=0; i<JN; i++) 
		{		
		   if (get_error_abs_ssi(i)==ERR_ABS_SSI)
		   {
					_control_mode[i] = MODE_IDLE;	
					_pad_enabled[i] = false;
					PWM_outputPadDisable(i);
			#ifdef DEBUG_CAN_MSG
		    	can_printf("ABS error %d",i);	
			#endif
		   }				
		}  
#endif
	
#warning "here we should put a control for 0x0258 and 0x255"	
#if (VERSION ==0x0254) || (VERSION ==0x0255)
		   if (get_error_abs_ssi(0)==ERR_ABS_SSI)
		   {
					_control_mode[0] = MODE_IDLE;	
					_pad_enabled[0] = false;
					PWM_outputPadDisable(0);
			#ifdef DEBUG_CAN_MSG
		    	can_printf("ABS error %d",0);	
			#endif
		   }	
					 
#endif	


 //DO NOTHING


		// decoupling the position	 	
		decouple_positions();
		

				/* velocity and acceleration estimators */
		{	
			for (i=0; i<JN; i++)
			{	
				//joint velocity estimator
				tailJntPos[i]=headJntPos[i]+(winSizeMax-divJntPos[i]); if(tailJntPos[i]>=winSizeMax) tailJntPos[i]=tailJntPos[i]%winSizeMax;			
				_speed_old[i] = _speed[i];
				jntPosWindow[headJntPos[i]][i]=_position[i];
				_speed[i] = (Int32) (((jntPosWindow[headJntPos[i]][i] - jntPosWindow[tailJntPos[i]][i] ))<<_jntVel_est_shift[i]);
			//	_speed[i] <<= _jntVel_est_shift[i];
				_speed[i] = (Int32)(_speed[i]) / divJntPos[i];
				headJntPos[i]=headJntPos[i]+1; if(headJntPos[i]>=winSizeMax) headJntPos[i]=0;
/*
				//joint acceleration estimator
				tailJntVel[i]=headJntVel[i]+(winSizeMax-divJntVel[i]); if(tailJntVel[i]>=winSizeMax) tailJntVel[i]=tailJntVel[i]%winSizeMax;			
				_accel_old[i] = _accel[i];
				jntVelWindow[headJntVel[i]][i]=_speed[i];
				_accel[i] = ((jntVelWindow[headJntVel[i]][i] - jntVelWindow[tailJntVel[i]][i] ));
				_accel[i] << _jntAcc_est_shift[i];
				_accel[i] = (Int32)(_accel[i]) / divJntVel[i];
				headJntVel[i]=headJntVel[i]+1; if(headJntVel[i]>=winSizeMax) headJntVel[i]=0;
*/				
				//motor velocity estimator
				tailMotPos[i]=headMotPos[i]+(winSizeMax-divMotPos[i]); if(tailMotPos[i]>=winSizeMax) tailMotPos[i]=tailMotPos[i]%winSizeMax;			
				_motor_speed_old[i] = _motor_speed[i];
				motPosWindow[headMotPos[i]][i]=_motor_position[i];
				_motor_speed[i] = ((motPosWindow[headMotPos[i]][i] - motPosWindow[tailMotPos[i]][i] ));
				_motor_speed[i] <<= _motVel_est_shift[i];
				_motor_speed[i] = (_motor_speed[i]) / divMotPos[i];
				headMotPos[i]=headMotPos[i]+1; if(headMotPos[i]>=winSizeMax) headMotPos[i]=0;				
			}
		}
		


					
		/* in position? */
#if (VERSION != 0x0254)  && (VERSION != 0x0258)
		for (i=0; i<JN; i++) _in_position[i] = check_in_position(i); 
#else
		_in_position[0] = check_in_position(0);
#endif
				
		/* in reference configuration for calibration? */
		//for (i=0; i<JN; i++) check_in_position_calib(i); 

	
//******************************************* POSITION LIMIT CHECK ***************************/
#if  (VERSION != 0x0258)

		for (i=0; i<JN; i++)  check_range(i, _safeband[i], PWMoutput);
#endif
//******************************************* COMPUTES CONTROLS *****************************/

		//FT sensor watchdog update 
		for (i=0; i<STRAIN_MAX; i++) 
			if (_strain_wtd[i]>0) _strain_wtd[i]--;
			
		for (i=0; i<JN; i++) 
		{
			//computing the PWM value (PID)
			PWMoutput[i] = compute_pwm(i);

			// PWM filtering in torque control 
			#if (VERSION != 0x0351)
			if (_control_mode[i] == MODE_TORQUE ||
			 	_control_mode[i] == MODE_IMPEDANCE_POS ||
			 	_control_mode[i] == MODE_IMPEDANCE_VEL)
				{
					PWMoutput[i] = lpf_ord1_3hz (PWMoutput[i], i);
				}	
			// saving the PWM value before the decoupling					
			_bfc_PWMoutput[i] = PWMoutput_old[i] = PWMoutput[i];

			// applying the saturation to the PWM
			if      (_bfc_PWMoutput[i] < -MAX_DUTY) _bfc_PWMoutput[i]=-MAX_DUTY;
			else if (_bfc_PWMoutput[i] > MAX_DUTY)  _bfc_PWMoutput[i]= MAX_DUTY;
			#endif //(VERSION != 0x0351)			
		}

//		decouple PWM	
#ifdef USE_NEW_DECOUPLING
		#ifdef USE_PARAMETRIC_DECOUPLING
		decouple_dutycycle_new_joint_parametric(PWMoutput);  //parametric version
		#else
		decouple_dutycycle_new_joint(PWMoutput); //new version (july 2010) with torque decupling
		#endif
#else
		decouple_dutycycle(PWMoutput);				
#endif
	
		
//******************************************* SATURATES CONTROLS ***************************/                
		/* back emf compensation + controls saturation (if necessary) */
		for (i=0; i<JN; i++)
		{
			if (_control_mode[i] == MODE_TORQUE ||
				_control_mode[i] == MODE_IMPEDANCE_POS ||
				_control_mode[i] == MODE_IMPEDANCE_VEL)
			{
				#if (VERSION != 0x0351)
				// Back emf compensation			
			
				PWMoutput[i]+=compensate_bemf(i, _comm_speed[i]);
				// Protection for joints out of the admissible range during force control
				check_range_torque(i, _safeband[i], PWMoutput);
				// PWM saturation
				ENFORCE_LIMITS	(i,PWMoutput[i], _pid_limit_torque[i] );
				#else  //(VERSION != 0x0351)
				ENFORCE_LIMITS	(i,PWMoutput[i], _pid_limit[i] );
				#endif //(VERSION != 0x0351)
			}
			else
			{
				ENFORCE_LIMITS	(i,PWMoutput[i], _pid_limit[i] );
			}			
			if      (_pid[i] < -MAX_DUTY) _pid[i]=-MAX_DUTY;
			else if (_pid[i] > MAX_DUTY)  _pid[i]= MAX_DUTY;
		}
				
		/* generate PWM */		
		for (i=0; i<JN; i++)
		{
			if (_pad_enabled[i] == false) 
			{
				_control_mode[i] = MODE_IDLE;
			}
			else	
			{
				PWM_generate(i,_pid[i]);        				
			}
			
		}
	
		/* Check Current done in T1 */

		/* do extra functions, communicate, etc. */
		//send broadcast data	
		can_send_broadcast();

		//send additional debug information
		//can_send_broadcast_debug(1,1);

#ifdef IDENTIF
#warning "***** IDENTIFICATION MODE ON *****"
		can_send_broadcast_identification(IDENTIF); //IDENTIF is the axis number 
#endif
	 

//	Check for the MAIN LOOP duration
 
			
//		t1val= (UInt16) TI1_getCounter(); 	
		if (	_count>0)
		{	
			mainLoopOVF=1;
			_count=0;
		}


		
		/* tells that the control cycle is completed */
		
		_wait = true;		
		
	} /* end for(;;) */
}