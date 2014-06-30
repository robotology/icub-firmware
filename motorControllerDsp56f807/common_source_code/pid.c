#include "dsp56f807.h"
#include "options.h"
#include "pid.h"
#include "pwm_interface.h"
#include "currents_interface.h"
#include "trajectory.h"
#include "asc.h"
#include "can1.h"
#include "identification.h"
#include "check_range.h"
#include "control_enable.h"

/* stable global data */
#ifndef VERSION
#	error "No valid version specified"
#endif
extern byte	_board_ID;	


#define INPOSITION_THRESHOLD 			60
#define INPOSITION_CALIB_THRESHOLD 		 1
bool _in_position[JN] = INIT_ARRAY (true);


// GENERAL VARIABLES
byte    _interaction_mode[JN] = INIT_ARRAY (icubCanProto_interactionmode_stiff);
byte    _control_mode[JN] = INIT_ARRAY (MODE_IDLE); // control mode (e.g. position, velocity, etc.) 
Int16   _fault[JN] = INIT_ARRAY (0);				// amp fault memory 
Int16   _counter = 0;								// used to count cycles, it resets now and then to generate periodic events 
Int16   _counter_calib = 0;							// used in calibration to count the number of cycles
Int16   _pwm_calibration[JN] = INIT_ARRAY (0);		// pid value during calibration with hard stops 
Int16  _velocity_calibration[JN]=INIT_ARRAY (0);	// vel value during calibration with hard stops 
bool    _calibrated[JN] = INIT_ARRAY (false);
bool    _ended[];									// trajectory completed flag 
bool    _verbose = false;
bool    _pending_request = false;					// whether a request to another card is pending 
Int16   _timeout = 0;								// used to timeout requests 
Rec_Pid _received_pid[JN];
Int32   _bfc_PWMoutput [JN] = INIT_ARRAY (0);

// DEBUG VARIABLES
Int16 _debug_in0[JN] = INIT_ARRAY (0); 		 		// general purpouse debug
Int16 _debug_in1[JN] = INIT_ARRAY (0);				// general purpouse debug
Int16 _debug_in2[JN] = INIT_ARRAY (0); 		 		// general purpouse debug
Int16 _debug_in3[JN] = INIT_ARRAY (0);				// general purpouse debug
Int16 _debug_in4[JN] = INIT_ARRAY (0); 		 		// general purpouse debug
Int16 _debug_in5[JN] = INIT_ARRAY (0);				// general purpouse debug
Int16 _debug_in6[JN] = INIT_ARRAY (0); 		 		// general purpouse debug
Int16 _debug_in7[JN] = INIT_ARRAY (0);				// general purpouse debug
Int16 _debug_out1[JN] = INIT_ARRAY (0); 		 	// general purpouse debug
Int16 _debug_out2[JN] = INIT_ARRAY (0);				// general purpouse debug
Int16 _debug_out3[JN] = INIT_ARRAY (0); 		 	// general purpouse debug
Int16 _debug_out4[JN] = INIT_ARRAY (0);				// general purpouse debug
Int16 _sacc0[JN] = INIT_ARRAY (0);
Int16 _sacc1[JN] = INIT_ARRAY (0);
Int16 _sacc2[JN] = INIT_ARRAY (0);
byte  _t1c =0;

// BACK-EMF COMPENSATION
Int16 _backemf_gain[JN]  = INIT_ARRAY (0);
byte  _backemf_shift[JN] = INIT_ARRAY (0);

// DECOUPLING PARAMETERS
float _param_a10_coeff = -1.6455F;
float _param_a11_coeff =  1.6455F;
float _param_a20_coeff = -1.6455F;
float _param_a21_coeff =  1.6455F;
float _param_a22_coeff =  1.6455F;

// POSITION VARIABLES
Int32 _abs_pos_calibration[JN] = INIT_ARRAY (0); // absolute position to be reached during calibration
Int32 _filt_abs_pos[JN] = INIT_ARRAY (0);		 // filtered absolute position sensor position
Int32 _position[JN] = INIT_ARRAY (0);			 // encoder position 
Int32 _motor_position[JN] = INIT_ARRAY (0);		 // hall effect encoder position 
Int32 _position_enc[JN] = INIT_ARRAY (0);		 // incremental encoder position 

Int32 _position_old[JN] = INIT_ARRAY (0);		 // do I need to add bits for taking into account multiple rotations 
Int32 _position_enc_old[JN] = INIT_ARRAY (0);	 // incremental encoder position 

Int32 _real_position[JN]= INIT_ARRAY (0);
Int32 _real_position_old[JN]= INIT_ARRAY (0);
Int32 _desired[JN] = INIT_ARRAY (0);		 
Int16 _desired_absolute[JN] = INIT_ARRAY (0);    // PD ref value for the calibration 
Int32 _set_point[JN] = INIT_ARRAY (0);  	     // set point for position [user specified] 

Int32 _min_position[JN] = INIT_ARRAY (-DEFAULT_MAX_POSITION);
Int32 _max_position[JN] = INIT_ARRAY (DEFAULT_MAX_POSITION);
Int32 _max_real_position[JN]=INIT_ARRAY (0);
Int16 _optical_ratio[JN]=INIT_ARRAY (0);         //  optical encoder ratio	

// SPEED VARIABLES
Int16 _speed[JN] = INIT_ARRAY (0);			 	 		// encoder speed 
Int16 _speed_old[JN] = INIT_ARRAY (0);					// encoder old speed 
Int16 _comm_speed[JN] = INIT_ARRAY (0);		     		// brushless commutation speed 
Int16 _comm_speed_old[JN] = INIT_ARRAY (0);				// previous brushless commutation speed 
Int32 _motor_speed[JN] = INIT_ARRAY (0);		     		// brushless motor speed 
Int32 _motor_speed_old[JN] = INIT_ARRAY (0);				// previous brushless motor speed 
Int16 _accu_desired_vel[JN] = INIT_ARRAY (0);			// accumultor for the fractional part of the desired vel 
Int16 _desired_vel[JN] = INIT_ARRAY (0);				// speed reference value, computed by the trajectory gen. 
Int16 _set_vel[JN] = INIT_ARRAY (DEFAULT_VELOCITY);		// set point for velocity [user specified] 
Int16 _max_vel[JN] = INIT_ARRAY (DEFAULT_MAX_VELOCITY);	// assume this limit is symmetric 
Int32 _vel_shift[JN] = INIT_ARRAY (4);
Int16 _vel_counter[JN] = INIT_ARRAY (0);
Int16 _vel_timeout[JN] = INIT_ARRAY (2000);             // timeout on velocity messages
byte  _jntVel_est_shift[JN] = INIT_ARRAY (5);			// shift of the speed estimator (joint)
byte  _motVel_est_shift[JN] = INIT_ARRAY (5);			// shift of the speed estimator (motor)

// ACCELERATION VARIABLES
Int16  _accel[JN] = INIT_ARRAY (0);			 			 // encoder acceleration 
Int16  _accel_old[JN] = INIT_ARRAY (0); 		       	 // previous acceleration
Int16  _set_acc[JN] = INIT_ARRAY (DEFAULT_ACCELERATION); // set point for acceleration [too low!] 
byte   _jntAcc_est_shift[JN] = INIT_ARRAY (5);			 // shift of the acceleration estimator (joint)
byte   _motAcc_est_shift[JN] = INIT_ARRAY (5);			 // shift of the acceleration estimator (motor)

// TORQUE VARIABLES
Int32 _desired_torque[JN] = INIT_ARRAY (0); 	// PID ref value, computed by the trajectory generator 


// POSITION PID VARIABLES
Int16  _error_position[JN] = INIT_ARRAY (0);	// actual feedback error 
Int16  _error_position_old[JN] = INIT_ARRAY (0);// error at t-1 
Int16  _absolute_error[JN] = INIT_ARRAY (0);	// actual feedback error from absolute sensors
Int16  _absolute_error_old[JN] = INIT_ARRAY (0);// error at t-1 
Int16  _pid[JN] = INIT_ARRAY (0);				// pid result 
Int16  _pid_limit[JN] = INIT_ARRAY (0);			// pid limit 
Int32  _pd[JN] = INIT_ARRAY (0);              	// pd portion of the pid
Int32  _pi[JN] = INIT_ARRAY (0);
Int32  _integral[JN] = INIT_ARRAY (0);	 		// store the sum of the integral component 
Int16  _integral_limit[JN] = INIT_ARRAY (0x7fff);

Int16  _kp[JN] = INIT_ARRAY (10);				// PID gains: proportional... 
Int16  _kd[JN] = INIT_ARRAY (40);				// ... derivative  ...
Int16  _ki[JN] = INIT_ARRAY (0);				// ... integral
Int16  _ko[JN] = INIT_ARRAY (0);				// offset 
Int16  _kr[JN] = INIT_ARRAY (3);				// scale factor (negative power of two) 
Int16  _kstp[JN] = INIT_ARRAY (0);              // stiction compensation: positive val
Int16  _kstn[JN] = INIT_ARRAY (0);              // stiction compensation: negative val
Int16  _kstc[JN] = INIT_ARRAY (0);              // stiction compensation: current  val 

// TORQUE PID
Int16  _strain_val[JN] = INIT_ARRAY (0);
Int16  _error_torque[JN] = INIT_ARRAY (0);		// actual feedback error 
Int16  _error_old_torque[JN] = INIT_ARRAY (0);	// error at t-1  
Int16  _pid_limit_torque[JN] = INIT_ARRAY (0);	// pid limit 
Int16  _integral_limit_torque[JN] = INIT_ARRAY (0x7fff);
Int16  _kp_torque[JN] = INIT_ARRAY (0);			// PID gains: proportional... 
Int16  _kd_torque[JN] = INIT_ARRAY (0);			// ... derivative  ...
Int16  _ki_torque[JN] = INIT_ARRAY (0);			// ... integral
Int16  _ko_torque[JN] = INIT_ARRAY (0);			// offset 
Int16  _kr_torque[JN] = INIT_ARRAY (10);		// scale factor (negative power of two) 
Int16  _kstp_torque[JN] = INIT_ARRAY (0);       // stiction compensation: positive val
Int16  _kstn_torque[JN] = INIT_ARRAY (0);       // stiction compensation: negative val
Int16  _kff_torque[JN] = INIT_ARRAY (0);        // feedforward gain

// OPENLOOP
Int16  _ko_openloop[JN] = INIT_ARRAY (0);		// offset 

// SPEED PID (iKart)
Int16  _error_speed[JN] = INIT_ARRAY (0);	    // actual feedback error 
Int16  _error_speed_old[JN] = INIT_ARRAY (0);   // error at t-1

// JOINT IMPEDANCE
Int16  _ks_imp[JN] = INIT_ARRAY (0);			// stiffness coefficient
Int16  _kd_imp[JN] = INIT_ARRAY (0);			// damping coefficient
Int16  _ko_imp[JN] = INIT_ARRAY (0);			// offset
Int16  _kr_imp[JN] = INIT_ARRAY (0);		    // scale factor (negative power of two) 

#if VERSION==0x0157 || VERSION==0x0150 || VERSION==0x0147 || VERSION==0x0140 || VERSION==0x0351 || VERSION==0x0250 || VERSION==0x0257
Int32  _cpl_pos_received[JN] = INIT_ARRAY (0);	// the position of the synchronized card 
Int32  _cpl_pos_prediction[JN] = INIT_ARRAY (0);// the actual adjustment (compensation) 
Int32  _cpl_pos_delta[JN] = INIT_ARRAY (0);		// velocity over the adjustment 
Int16  _cpl_pos_counter = 0;					// counter to check when last _cpl_pos_ was received  
Int16  _cpl_err[JN] = INIT_ARRAY (0);    		// the error of the syncronized card
Int16  _cpl_pid_received[JN] = INIT_ARRAY (0);  // the duty of the syncronized card
Int16  _cpl_pid_prediction[JN] = INIT_ARRAY (0);// the predicted adjustment
Int16  _cpl_pid_delta[JN] = INIT_ARRAY (0);		// the adjustment step
Int16   _cpl_pid_counter = 0;					// counter to check when last _cpl_pid_ was received  
#endif

#if ((VERSION == 0x0128) || (VERSION == 0x0130) || (VERSION == 0x0228) || (VERSION == 0x0230))
Int32  _adjustment[JN]=INIT_ARRAY (0);          // the sum of the three value coming from the MAIS board
#endif

#if ((VERSION == 0x0128) || (VERSION == 0x0130) || (VERSION == 0x0228) || (VERSION == 0x0230))
// max allowed position for encoder while controlling with absolute position sensors
Int16 _max_position_enc[JN] = INIT_ARRAY (0);
Int16 _min_position_enc[JN] = INIT_ARRAY (0);
#endif


/*
 * compute PWM in different modalities
 */
Int32 compute_pwm(byte j)
{
	Int32 PWMoutput = 0;
	Int32 Ioutput = 0;
	Int32 speed_damping =0;

	/*watchdog check for strain messages in torque control mode + input selection*/
	//the function turns off pwm of joint <jnt> if <strain_num> watchdog is triggered
	//the first number is joint, the second number is the watchdog identifier
	{
		byte i=0;
		for (i=0;i<JN; i++)
		{
		    //can_printf("%d %d %d %d",i,_selected_strain_id[i], _selected_strain_chan[i], _strain_wtd[_selected_strain_id[i]]);
			read_force_data (i, _selected_strain_id[i], _selected_strain_chan[i]);		
		}
	}
#if	VERSION == 0x0351
		//iKart has a fake torque mode that just compensates for back-emf
		_strain_val[j]=0;
#endif	  
	
	switch (_control_mode[j]) 
	{ 
#if VERSION == 0x0351 
	//iKart control
 	case MODE_POSITION: 
 		PWMoutput = 0;
 		_pd[j] = 0;
 	break;
 	
	case MODE_VELOCITY: 
		PWMoutput = compute_pid_speed(j);
		PWMoutput = PWMoutput + _ko[j];
		_pd[j] = _pd[j] + _ko[j];
	break;
	
	case MODE_TORQUE: 
		PWMoutput = compensate_bemf(j, _speed[j]);
		_pd[j] = 0;
	break;
	
    default:
	    can_printf ("UNKNOWN CONTROLMODE PID");
		put_motor_in_fault(j);	
		PWMoutput=0;
	break;
    	
#elif VERSION == 0x0215 
   	case MODE_POSITION:
	case MODE_VELOCITY:
	case MODE_MIXED:
	{
        if (_interaction_mode[j]==icubCanProto_interactionmode_stiff)
        {
	        if (_sacc0[j] == 0)
	        {
	     		compute_desired(j);
	     		PWMoutput = compute_pid2(j);
	     		PWMoutput = PWMoutput + _ko[j];
	     		_pd[j] = _pd[j] + _ko[j];
	        }
	        else
	        {
	       	    static bool up[4] =INIT_ARRAY (1);
	       	    static byte time_div[4] = INIT_ARRAY (0);
	       	    static Int16 accum[4] = INIT_ARRAY (0);
	       		Int32 _des_saved = 0;
	       
	       	    time_div[j]++;
	       	    if (time_div[j] > 4) time_div[j] =0;
	       		
	       		if (j==2)
	       		{
	       			if (up[j]) accum[j] += 2;
	       			else       accum[j] -= 2;
	       			if      (accum[j] >  200) up[j] = false; //280 = 1 deg
	       			else if (accum[j] < -200) up[j] = true;				
	       		}
	       		if (j==1)
	       		{
	       			if (time_div[j]==0)
	       			{
	       				if (up[j]) accum[j] += 1;
	       				else       accum[j] -= 1;	
	       			}
	       			if      (accum[j] >  5) up[j] = false; //11 = 1 deg
	       			else if (accum[j] < -5) up[j] = true;				
	       		}
	       		
	       		compute_desired(j);
	       		//microsaccades
	       		_des_saved=_desired[j];	
	       		//randomNext();
	       		
	       		if (j==1 || j == 2) {_desired[j] = _desired[j]+accum[j];}
	            //can_printf("%d %f %f %f %f", j, _desired[0], _desired[1], _desired[2], _desired[3]); //jnt == 2 corrisponde al giunto 4
	       		
	       		PWMoutput = compute_pid2(j);
	       		PWMoutput = PWMoutput + _ko[j];
	       		_pd[j] = _pd[j] + _ko[j];
	       		_desired[j]=_des_saved;       	
            }
        }
        else if (_interaction_mode[j]==icubCanProto_interactionmode_compliant)
        {
 		 	can_printf ("UNSUPPORTED INTERACTION PID");
		 	put_motor_in_fault(j);	
		 	PWMoutput=0;       	
        }
        else
        {
 		 	can_printf ("UNKNOWN INTERACTION PID");
		 	put_motor_in_fault(j);	
		 	PWMoutput=0;       	
        }
	}
	break;  

#else //all other firmware versions	
	case MODE_POSITION:
	case MODE_VELOCITY:
	case MODE_MIXED:
		 if (_interaction_mode[j]==icubCanProto_interactionmode_stiff)
		 {
			compute_desired(j);
			PWMoutput = compute_pid2(j);
			PWMoutput = PWMoutput + _ko[j];
			_pd[j] = _pd[j] + _ko[j];		 	
			
			#if (CURRENT_BOARD_TYPE == BOARD_TYPE_4DC) 
				PWMoutput = PWMoutput + _kstc[j];
				_pd[j] = _pd[j] + _kstc[j];//stiction compensation for 4dc motros
				if (_ended[j])
				{
					if (_kstc[j]> 0 ) _kstc[j]--;
					if (_kstc[j]< 0 ) _kstc[j]++;
				}
			#endif
		 }
		 else if (_interaction_mode[j]==icubCanProto_interactionmode_compliant)
		 {
			compute_desired(j);
			compute_pid_impedance(j);
			PWMoutput = compute_pid_torque(j, _strain_val[j]);
			PWMoutput = PWMoutput + _ko_torque[j];
			_pd[j] = _pd[j] + _ko_torque[j];		 	
		 }
		 else
		 {
		 	can_printf ("UNKOWN INTERACTION PID");
		 	put_motor_in_fault(j);	
		 	PWMoutput=0;
		 }
	break;

	case MODE_TORQUE: 
		PWMoutput = compute_pid_torque(j, _strain_val[j]);		
		// additional speed damping. It uses the same shift factor for bemf compensation
		speed_damping = ((Int32) (-_speed[j]) * ((Int32) _debug_in7[j]));
		speed_damping >>= (_backemf_shift[j]+_jntVel_est_shift[j]);		
		PWMoutput = PWMoutput + _ko_torque[j] + speed_damping;
		_pd[j] = _pd[j] + _ko_torque[j] + speed_damping;
	break;
#endif

	case MODE_DIRECT:
		PWMoutput = compute_pid2(j);
		PWMoutput = PWMoutput + _ko[j];
		_pd[j] = _pd[j] + _ko[j];
	break;
	
	case MODE_IMPEDANCE_POS:
	case MODE_IMPEDANCE_VEL: 
		compute_desired(j);
		compute_pid_impedance(j);
		PWMoutput = compute_pid_torque(j, _strain_val[j]);
		PWMoutput = PWMoutput + _ko_torque[j];
		_pd[j] = _pd[j] + _ko_torque[j];
	break;
	
	case MODE_OPENLOOP:
		PWMoutput = _ko_openloop[j];
	break;
	
	case MODE_CALIB_ABS_POS_SENS:
	case MODE_CALIB_ABS_AND_INCREMENTAL:
		compute_desired(j);
		PWMoutput = compute_pid2(j);
		PWMoutput = PWMoutput + _ko[j];
		_pd[j] = _pd[j] + _ko[j];
	break;
	
	case MODE_CALIB_HARD_STOPS:
        #if (CURRENT_BOARD_TYPE == BOARD_TYPE_4DC) 
		_desired[j]+=_velocity_calibration[j];
		PWMoutput = compute_pid2(j);
		if 	(PWMoutput > _pwm_calibration[j])
			PWMoutput = _pwm_calibration[j];
		if 	(PWMoutput < -_pwm_calibration[j])	
			PWMoutput = -_pwm_calibration[j];
		#else
		PWMoutput = _pwm_calibration[j];
		_counter_calib +=1;
		#endif
	break;

	case MODE_HANDLE_HARD_STOPS:
		can_printf("MODE HANDLE HARD STOP");
	    PWM_outputPadDisable(j);
		_control_mode[j] = MODE_IDLE;
		PWMoutput=0;
		break;
		
	case MODE_IDLE:
	case MODE_HW_FAULT:
		PWMoutput=0;
	break;
	 
	}
	
	return PWMoutput;
}

/*
 * compute PID impedence (integral is implemented).
 */
Int32 compute_pid_impedance(byte j)
{
	Int32 ImpInputError=0;
	Int32 DampingPortion=0;
	byte k=0;
	static Int32 DmpAccu[JN]=INIT_ARRAY(0);  
	static byte headDmpPor[JN]=INIT_ARRAY(0);  
	static byte tailDmpPor[JN]=INIT_ARRAY(0); 
	static Int32 DmpPort[2][10]={{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}};	
	
	_error_position_old[j] = _error_position[j];
	ImpInputError = L_sub(_position[j], _desired[j]);				
	if (ImpInputError > MAX_16)
	{
		_error_position[j] = MAX_16;
	}
	else if (ImpInputError < MIN_16) 
	{
		_error_position[j] = MIN_16;	
	}
	else
	{
		_error_position[j] = extract_l(ImpInputError);
	}
	
	/* Proportional part (stiffness) */
	_desired_torque[j] = -(Int32) _ks_imp[j] * (Int32)(_error_position[j]);

	/* Additional offset (for gravity compensation) */
	_desired_torque[j] += (Int32)_ko_imp[j];
	
	/* Derivative part (damping) */	
	DampingPortion = -((Int32) (_error_position[j]-_error_position_old[j])) * ((Int32) _kd_imp[j]);
	//DampingPortion = -((Int32) (_speed[j])) * ((Int32) _kd_imp[j]) >> _jntVel_est_shift[j];
  
    _debug_out1[j]=_kd_imp[j];
    _debug_out2[j]=DampingPortion;
    
	#if (CURRENT_BOARD_TYPE  == BOARD_TYPE_BLL)
		//derivative filtering in BLL boards
		//this filter performs a mean on last 10 samples
		DmpAccu[j] -= DmpPort[j][tailDmpPor[j]];
		tailDmpPor[j]=headDmpPor[j]+1; if(tailDmpPor[j]>=10) tailDmpPor[j]=0;			
		DmpPort[j][headDmpPor[j]] = DampingPortion;
		DmpAccu[j] += DmpPort[j][headDmpPor[j]];

		headDmpPor[j]=headDmpPor[j]+1; if(headDmpPor[j]>=10) headDmpPor[j]=0;
		DampingPortion=DmpAccu[j]/10;
	#endif
	
	_desired_torque[j] += DampingPortion;	
	
	_debug_out3[j]=DampingPortion;
	_debug_out4[j]=_desired_torque[j];	
	
	return _desired_torque[j];
}

/*
 * compute PID torque
 */
Int32 compute_pid_torque(byte j, Int16 strain_val)
{
	Int32 ProportionalPortion, DerivativePortion, IntegralPortion;
	Int32 FeedforwardPortion;
	Int32 IntegralError;
	Int32 PIDoutput;
	Int32 InputError;
	float temp_err[JN] = INIT_ARRAY (0.0);

	byte i=0;
	byte k=0;
	static Int32 DerAccu[JN]=INIT_ARRAY(0);  
	static byte headDerPor[JN]=INIT_ARRAY(0);  
	static byte tailDerPor[JN]=INIT_ARRAY(0); 
	static Int32 DerPort[2][10]={{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}};	
	
	
	/* the error @ previous cycle */
	
	_error_old_torque[j] = _error_torque[j];

	InputError = L_sub(	_desired_torque[j], (Int32)strain_val);
			
	if (InputError > MAX_16)
		_error_torque[j] = MAX_16;
	else
	if (InputError < MIN_16) 
		_error_torque[j] = MIN_16;
	else
	{
		_error_torque[j] = extract_l(InputError);
	}

    /* Feed-forward */
    FeedforwardPortion = (Int32)_desired_torque[j] * ((Int32)_kff_torque[j]);
	if (FeedforwardPortion>=0) 
	{
		FeedforwardPortion = FeedforwardPortion >> _kr_torque[j]; 
	}
	else
	{
		FeedforwardPortion = -(-FeedforwardPortion >> _kr_torque[j]);
	}
	    
	/* Proportional */
	ProportionalPortion = ((Int32) _error_torque[j]) * ((Int32)_kp_torque[j]);

	if (ProportionalPortion>=0) 
	{
		ProportionalPortion = ProportionalPortion >> _kr_torque[j]; 
	}
	else
	{
		ProportionalPortion = -(-ProportionalPortion >> _kr_torque[j]);
	}
	
	/* Force Derivative */	
	DerivativePortion = ((Int32) (_error_torque[j]-_error_old_torque[j])) * ((Int32) _kd_torque[j]);

	if (DerivativePortion>=0)
	{
		DerivativePortion = DerivativePortion >> _kr_torque[j]; 
	}
	else
	{
		DerivativePortion = -(-DerivativePortion >> _kr_torque[j]);
	}
	 
    /* Derivative part filtering */
	//this filter performs a mean on last 10 samples
	DerAccu[j] -= DerPort[j][tailDerPor[j]];
	tailDerPor[j]=headDerPor[j]+1; if(tailDerPor[j]>=10) tailDerPor[j]=0;			
	DerPort[j][headDerPor[j]] = DerivativePortion;
	DerAccu[j] += DerPort[j][headDerPor[j]];
	headDerPor[j]=headDerPor[j]+1; if(headDerPor[j]>=10) headDerPor[j]=0;
	DerivativePortion=DerAccu[j]/10;
		
	/* Integral */
	IntegralError =  ( (Int32) _error_torque[j]) * ((Int32) _ki_torque[j]);
	
	_integral[j] = _integral[j] + IntegralError;
	IntegralPortion = _integral[j];
	
	if (IntegralPortion>=0)
	{
		IntegralPortion = (IntegralPortion >> _kr_torque[j]); // integral reduction 
	}
	else
	{
		IntegralPortion = -((-IntegralPortion) >> _kr_torque[j]); // integral reduction 
	} 

	/* Accumulator saturation */
	if (IntegralPortion >= _integral_limit_torque[j])
	{
		IntegralPortion = _integral_limit_torque[j];
		_integral[j] =  _integral_limit_torque[j];
	}		
	else if (IntegralPortion < - (_integral_limit_torque[j]))
	{
		IntegralPortion = - (_integral_limit_torque[j]);
		_integral[j] = (-_integral_limit_torque[j]);
	}
	
	_pd[j] = L_add(ProportionalPortion, FeedforwardPortion);
	_pd[j] = L_add(_pd[j], DerivativePortion);
	_pi[j] = IntegralPortion;
	PIDoutput = L_add(_pd[j], IntegralPortion);
	
	return PIDoutput;
}

/*
 * compute PID control (integral is implemented).
 */
Int32 compute_pid2(byte j)
{
	Int32 ProportionalPortion, DerivativePortion, IntegralPortion;
	Int32 IntegralError;
	
	Int32 PIDoutput;
	Int32 InputError;
	byte i=0;
	byte k=0;

	static Int32 DerAccu[JN]=INIT_ARRAY(0);  
	static byte headDerPor[JN]=INIT_ARRAY(0);  
	static byte tailDerPor[JN]=INIT_ARRAY(0); 
	static Int32 DerPort[2][10]={{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}};	
	
	/* the error @ previous cycle */
	
	_error_position_old[j] = _error_position[j];

	InputError = L_sub(_desired[j], _position[j]);
		
	if (InputError > MAX_16)
		_error_position[j] = MAX_16;
	else
	if (InputError < MIN_16) 
		_error_position[j] = MIN_16;
	else
	{
		_error_position[j] = extract_l(InputError);
	}
			
	/* Proportional */
	ProportionalPortion = ((Int32) _error_position[j]) * ((Int32)_kp[j]);
	
	if (ProportionalPortion>=0)
	{
		ProportionalPortion = ProportionalPortion >> _kr[j]; 
	}
	else
	{
		ProportionalPortion = -(-ProportionalPortion >> _kr[j]);
	}
	
#if VERSION==0x162
	
	/* Derivative */	
	if (j==0)
	{
		DerivativePortion = ((Int32) -(_speed[0]-_speed[1])) * ((Int32) _kd[j]);	
	}
	else 
	{
		DerivativePortion = ((Int32) -(_speed[0]+_speed[1])) * ((Int32) _kd[j]);	
	}

	if (DerivativePortion>=0)
	{
		DerivativePortion = DerivativePortion >> (_kr[j]);//+_jntVel_est_shift[j]); 
	}
	else
	{
		DerivativePortion = -(-DerivativePortion >> (_kr[j]));//+_jntVel_est_shift[j]));
	}

#else

	/* Derivative */	
	DerivativePortion = ((Int32) (_error_position[j]-_error_position_old[j])) * ((Int32) _kd[j]);

	if (DerivativePortion>=0)
	{
		DerivativePortion = DerivativePortion >> _kr[j]; 
	}
	else
	{
		DerivativePortion = -(-DerivativePortion >> _kr[j]);
	}
  
#endif		

#if (CURRENT_BOARD_TYPE  == BOARD_TYPE_BLL)
	//derivative filtering in BLL boards
	//this filter performs a mean on last 10 samples
	DerAccu[j] -= DerPort[j][tailDerPor[j]];
	tailDerPor[j]=headDerPor[j]+1; if(tailDerPor[j]>=10) tailDerPor[j]=0;			
	DerPort[j][headDerPor[j]] = DerivativePortion;
	DerAccu[j] += DerPort[j][headDerPor[j]];
	headDerPor[j]=headDerPor[j]+1; if(headDerPor[j]>=10) headDerPor[j]=0;
	DerivativePortion=DerAccu[j]/10;
#endif

	/* Integral */
	IntegralError =  ( (Int32) _error_position[j]) * ((Int32) _ki[j]);

	_integral[j] = _integral[j] + IntegralError;
	IntegralPortion = _integral[j];
	
	if (IntegralPortion>=0)
	{
		IntegralPortion = (IntegralPortion >> _kr[j]); // integral reduction 
	}
	else
	{
		IntegralPortion = -((-IntegralPortion) >> _kr[j]); // integral reduction 
	} 
	
	/* Accumulator saturation */
	if (IntegralPortion >= _integral_limit[j])
	{
		IntegralPortion = _integral_limit[j];
		_integral[j] =  _integral_limit[j];
	}		
	else if (IntegralPortion < - (_integral_limit[j]))
	{
		IntegralPortion = - (_integral_limit[j]);
		_integral[j] = (-_integral_limit[j]);
	}
		
	_pd[j] = L_add(ProportionalPortion, DerivativePortion);
	_pi[j] = IntegralPortion;
	PIDoutput = L_add(_pd[j], IntegralPortion);

					
	return PIDoutput;
}

/*
 * compute explicit speed PID control (iKart).
 */
Int32 compute_pid_speed(byte j)
{
	Int32 ProportionalPortion, DerivativePortion, IntegralPortion;
	Int32 IntegralError;
	
	Int32 PIDoutput;
	Int32 InputError;
	byte i=0;
	byte k=0;

	static Int32 DerAccu[JN]=INIT_ARRAY(0);  
	static byte headDerPor[JN]=INIT_ARRAY(0);  
	static byte tailDerPor[JN]=INIT_ARRAY(0); 
	static Int32 DerPort[2][10]={{0,0,0,0,0,0,0,0,0,0},{0,0,0,0,0,0,0,0,0,0}};	
	
	/* the error @ previous cycle */
	
	_error_speed_old[j] = _error_speed[j];

	InputError = L_sub(_set_vel[j], _speed[j]);
		
	if (InputError > MAX_16)
		_error_speed[j] = MAX_16;
	else
	if (InputError < MIN_16) 
		_error_speed[j] = MIN_16;
	else
	{
		_error_speed[j] = extract_l(InputError);
	}
			
	/* Proportional */
	ProportionalPortion = ((Int32) _error_speed[j]) * ((Int32)_kp[j]);
	
	if (ProportionalPortion>=0)
	{
		ProportionalPortion = ProportionalPortion >> (_kr[j]+_jntVel_est_shift[j]); 
	}
	else
	{
		ProportionalPortion = -(-ProportionalPortion >> (_kr[j]+_jntVel_est_shift[j]));
	}
	
	/* Derivative */	
	DerivativePortion = ((Int32) (_error_speed[j]-_error_speed_old[j])) * ((Int32) _kd[j]);

	if (DerivativePortion>=0)
	{
		DerivativePortion = DerivativePortion >> (_kr[j]+_jntVel_est_shift[j]); 
	}
	else
	{
		DerivativePortion = -(-DerivativePortion >> (_kr[j]+_jntVel_est_shift[j]));
	}
  
	//this filter performs a mean on last 10 samples
	DerAccu[j] -= DerPort[j][tailDerPor[j]];
	tailDerPor[j]=headDerPor[j]+1; if(tailDerPor[j]>=10) tailDerPor[j]=0;			
	DerPort[j][headDerPor[j]] = DerivativePortion;
	DerAccu[j] += DerPort[j][headDerPor[j]];
	headDerPor[j]=headDerPor[j]+1; if(headDerPor[j]>=10) headDerPor[j]=0;
	DerivativePortion=DerAccu[j]/10;
		
	/* Integral */
	IntegralError =  ( (Int32) _error_speed[j]) * ((Int32) _ki[j]);

	_integral[j] = _integral[j] + IntegralError;
	IntegralPortion = _integral[j];
	
	if (IntegralPortion>=0)
	{
		IntegralPortion = (IntegralPortion >> (_kr[j]+_jntVel_est_shift[j])); // integral reduction 
	}
	else
	{
		IntegralPortion = -((-IntegralPortion) >> (_kr[j]+_jntVel_est_shift[j])); // integral reduction 
	} 
	
	/* Accumulator saturation */
	if (IntegralPortion >= _integral_limit[j])
	{
		IntegralPortion = _integral_limit[j];
		_integral[j] =  _integral_limit[j];
	}		
	else if (IntegralPortion < - (_integral_limit[j]))
	{
		IntegralPortion = - (_integral_limit[j]);
		_integral[j] = (-_integral_limit[j]);
	}
		
	_pd[j] = L_add(ProportionalPortion, DerivativePortion);
	_pi[j] = IntegralPortion;
	PIDoutput = L_add(_pd[j], IntegralPortion);

					
	return PIDoutput;
}


/* 
 * Compute PD for calibrating with the absolute postion sensors
 */
Int32 compute_pid_abs(byte j)
{
	Int32 ProportionalPortion, DerivativePortion;
	Int32 PIDoutput;
	Int16 Kp = 1;
	Int16 Kd = 10;
		
	/* the error @ previous cycle */
	_absolute_error_old[j] = _absolute_error[j];
	/* the errore @ current cycle */
	_absolute_error[j] = _desired_absolute[j] - extract_h(_filt_abs_pos[j]);
	//_absolute_error[j] = 0x5a0 - extract_h(_filt_abs_pos[j]);	

	/* Proportional */
	ProportionalPortion = _absolute_error[j] * Kp;
	/* Derivative */	
	DerivativePortion = (_absolute_error[j]-_absolute_error_old[j]) * Kd;
	
	PIDoutput = (ProportionalPortion + DerivativePortion);
	//AS1_printDWordAsCharsDec (PIDoutput/70);		
	

	return (PIDoutput >> 1);
}

/*
 * a step in the trajectory generation for velocity control. 
 */
Int32 step_velocity (byte jj)
{
	Int32 u0;
	Int16 dv, da;
	Int16 _tmp_desired_vel;
	Int16 _tmp_diff_vel;
	
	/* dv is a signed 16 bit value, need to be checked for overflow */
	if (_set_vel[jj] < -_max_vel[jj])
		_set_vel[jj] = -_max_vel[jj];
	else
	if (_set_vel[jj] > _max_vel[jj])
		_set_vel[jj] = _max_vel[jj];
	
	dv = _set_vel[jj] - _desired_vel[jj];
	da = _set_acc[jj] * CONTROLLER_PERIOD;
	
	if (__abs(dv) < da)
	{
		_desired_vel[jj] = _set_vel[jj];
	}
	else
	if (dv > 0)
	{
		_desired_vel[jj] += da;
	}
	else
	{
		_desired_vel[jj] -= da;
	}
	
	//since the desired velocity is expressed in
	//[16*encoders_tics/ms] we divide the desired
	//velocity by 16 with a shift of 4 bits
	//(more in general: _desired_vel[jj] which has a default value of 4)
	_tmp_desired_vel = (__abs(_desired_vel[jj]) >> _vel_shift[jj]);
	if (_desired_vel[jj] > 0)
		u0 =   _tmp_desired_vel * CONTROLLER_PERIOD;
	else
		u0 = - _tmp_desired_vel * CONTROLLER_PERIOD;
	
	//the additional 4 bits (which have not been used
	//in computing the desired velocity) are accumulated
	_tmp_diff_vel = __abs(_desired_vel[jj]) - (_tmp_desired_vel << _vel_shift[jj]);
	if (_desired_vel[jj] > 0)
		_accu_desired_vel[jj] = _accu_desired_vel[jj] + _tmp_diff_vel;
	else
		_accu_desired_vel[jj] = _accu_desired_vel[jj] - _tmp_diff_vel;
	
	//if accumulated additional bits overflow (i.e.
	//the fifth...sixteenth bits are different from zero)
	//the overflown part is added to the output
	//finally the accumulator is updated, taking into
	//account that the overflown bit have been considered
	_tmp_desired_vel = (__abs(_accu_desired_vel[jj]) >> _vel_shift[jj]);
	if (_desired_vel[jj] > 0)
	{
		u0 = u0 + _tmp_desired_vel * CONTROLLER_PERIOD;
		_accu_desired_vel[jj] = _accu_desired_vel[jj] - (_tmp_desired_vel<<_vel_shift[jj]);
	}
		
	else
	{
		u0 = u0 - _tmp_desired_vel * CONTROLLER_PERIOD;
		_accu_desired_vel[jj] = _accu_desired_vel[jj] + (_tmp_desired_vel<<_vel_shift[jj]);
	}
	
	return u0;
}


/*
 * helper function to generate desired position.
 */
void compute_desired(byte i)
{		
 	Int32 previous_desired;
	
	if (!mode_is_idle(i))
	{
		previous_desired = _desired[i];
		
		switch (_control_mode[i])
		{
		case MODE_POSITION:
		case MODE_IMPEDANCE_POS:
		case MODE_CALIB_ABS_AND_INCREMENTAL:
			_desired[i] = step_trajectory (i);
			break;
			
		case MODE_CALIB_ABS_POS_SENS:	
			_desired_absolute[i] = (Int16) step_trajectory (i);
			#if (CURRENT_BOARD_TYPE  == BOARD_TYPE_4DC)		
			    _desired[i] = _desired[i] + compute_pid_abs (i);
			#endif	
			break;
							
							
		case MODE_MIXED:							
		case MODE_VELOCITY:
		case MODE_IMPEDANCE_VEL:
			// checks if the velocity messages streaming has been interrupted (i.e. last message
			// received  more than _vel_timeout ms ago)
			_vel_counter[i]++; 
		    if(_vel_counter[i] > _vel_timeout[i])
		    {	
		    	//disabling velocity control						
				_vel_counter[i] = 0;
				_desired_vel[i] = 0;	
				_set_vel[i] = 0;		  	
				#ifdef DEBUG_CAN_MSG
					can_printf("No vel msgs in %d[ms]", _vel_counter[i]);
				#endif		
		    }	
			_desired[i] += step_trajectory_delta (i);
			_desired[i] += step_velocity (i);
			break;
		}	
		check_desired_within_limits(i, previous_desired);
	}
}

/***************************************************************** 
 * this function checks if the trajectory is terminated
 * and if trajectory is terminated sets the variable _in_position
 *****************************************************************/
bool check_in_position(byte jnt)
{
	if (_control_mode[jnt] == MODE_POSITION ||
	    _control_mode[jnt] == MODE_VELOCITY ||
	    _control_mode[jnt] == MODE_MIXED    ||
	    _control_mode[jnt] == MODE_IMPEDANCE_POS ||
	    _control_mode[jnt] == MODE_IMPEDANCE_VEL)
		{
			//if (__abs(_position[jnt] - _set_point[jnt]) < INPOSITION_THRESHOLD && _ended[jnt])
			if (_ended[jnt])
				return true;
			else
				return false;
		}
	else if (_control_mode[jnt] == MODE_DIRECT)
	{
		return true;
	}
	else
	{
		return false;			
	}			
}

/***************************************************************************/
/**
 * this function turns off pwm of joint <jnt> if <strain_num> watchdog is
 * triggered (returns false). Returns true otherwise (all ok).
 * the force value contained in the <strain_channel> is assigned to strain_val
 ***************************************************************************/
bool read_force_data (byte jnt, byte strain_num, byte strain_chan)
{
	if (_control_mode[jnt] == MODE_TORQUE ||
		_control_mode[jnt] == MODE_IMPEDANCE_POS ||
		_control_mode[jnt] == MODE_IMPEDANCE_VEL ||
		_interaction_mode[jnt] == icubCanProto_interactionmode_compliant )
		{
			if (strain_num==-1)
			{
				put_motor_in_fault(jnt);	

				#ifdef DEBUG_CAN_MSG					
					can_printf("WARN:force control not allowed jnt:%d",jnt);
				#endif
								
				_strain_val[jnt]=0;
				return false;				
			}
			if (_strain_wtd[strain_num]==0)
			{
				put_motor_in_fault(jnt);	
					
				#ifdef DEBUG_CAN_MSG
					can_printf("WARN:strain watchdog disabling pwm jnt:%d",jnt);				
				#endif	
				
				_strain_val[jnt]=0;
				return false;
			}
			else
			{
				_strain_val[jnt]=_strain[strain_num][strain_chan];
				return true;	
			}	
		}
	else
	{
		//here we are in stiff control mode (i.e. position)
		return true;			
	}	
}

