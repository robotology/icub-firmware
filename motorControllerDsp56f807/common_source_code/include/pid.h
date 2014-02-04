
#ifndef __pidh__
#define __pidh__

#include "dsp56f807.h"
#include "controller.h"
#include "options.h"

#ifndef VERSION
#	error "No valid version specified"
#endif

#include "strain_board.h"


/******************************************************/
// pid prototypes 
/******************************************************/

Int32 compute_pwm(byte j);
Int32 compute_pid2(byte j);
Int32 compute_pid_speed(byte j);
Int32 compute_pid_impedance(byte j);
Int32 compute_pid_torque(byte j, Int16 strain_val);
Int32 compute_current_pid(byte j);
Int32 compute_pid_abs(byte j);
Int32 compute_filtpid(byte jnt, Int32 PID);
Int32 step_velocity (byte jj);
void  compute_desired(byte i);
bool  check_in_position(byte jnt);
void  init_smooth_pid(byte jnt,Int16 finalp,Int16 finald,byte finals, Int16 Time);
void  smooth_pid(byte jnt);
bool  read_force_data (byte,byte,byte);
inline Int32 compensate_bemf(byte j, Int16 motor_speed);

/******************************************************/
// stable global data 
/******************************************************/


#if VERSION == 0x0164 || VERSION == 0x0158 || VERSION == 0x0114
// analog feedback 
#define INPOSITION_THRESHOLD 150
#else
// digital encoder feedback 
#define INPOSITION_THRESHOLD 			60
#endif
#define INPOSITION_CALIB_THRESHOLD 		 1
extern bool _in_position[JN] ;

#define IS_DONE(jj) (_ended[jj])
extern bool _ended[];

typedef union {
    byte rec_pid;
    struct {
     		 bool kp : 1;                	// kp received 
      	 	 bool kd : 1;				 	// kd received 	
      		 bool ki : 1;                	// ki received 
      		 bool kr:  1;                	// kr received 
     		 bool ko:  1;                	// ko received 
     		 bool ilim:  1;              	// ilim received 
     		 bool tlim:  1;            		// tlim received 
     		 bool notused:  1;         	 	// notused received 
           }rec_pid_bits;
  } Rec_Pid;                                   


// GENERAL VARIABLES
extern byte    _control_mode[JN] ;		// control mode (e.g. position, velocity, etc.) 
extern Int16   _fault[JN] ;				// amp fault memory 
extern bool    _pad_enabled[JN] ;
extern Int16   _counter ;					// used to count cycles, it resets now and then to generate periodic events 
extern Int16   _counter_calib ;			// used in calibration to count the number of cycles											
extern Int16   _pwm_calibration[JN] ;		// pid value during calibration with hard stops 
extern bool    _calibrated[JN] ;
extern bool    _verbose ;
extern Int16   _version ;
extern bool    _pending_request ;			// whether a request to another card is pending 
extern Int16   _timeout ;					// used to timeout requests 
extern Rec_Pid _received_pid[JN];
extern Int32   _bfc_PWMoutput [JN];

// DEBUG VARIABLES
extern byte  _t1c;                      // general purpouse counter
extern Int16 _debug_in0[JN] ;			// general purpouse debug
extern Int16 _debug_in1[JN] ;			// general purpouse debug
extern Int16 _debug_in2[JN] ;			// general purpouse debug
extern Int16 _debug_in3[JN] ;			// general purpouse debug
extern Int16 _debug_in4[JN] ;			// general purpouse debug
extern Int16 _debug_in5[JN] ;			// general purpouse debug
extern Int16 _debug_in6[JN] ;			// general purpouse debug
extern Int16 _debug_in7[JN] ;			// general purpouse debug
extern Int16 _debug_out1[JN] ;			// general purpouse debug
extern Int16 _debug_out2[JN] ;			// general purpouse debug
extern Int16 _debug_out3[JN] ;			// general purpouse debug
extern Int16 _debug_out4[JN] ;			// general purpouse debug

extern Int16 _sacc0[JN] ;
extern Int16 _sacc1[JN] ;
extern Int16 _sacc2[JN] ;

// BACK-EMF COMPENSATION
extern Int16 _backemf_gain[JN];
extern byte  _backemf_shift[JN];

// POSITION VARIABLES											
extern Int32 _abs_pos_calibration[JN] ;	// absolute position to be reached during calibration
extern Int32 _filt_abs_pos[JN] ;		// filtered absolute position sensor position
extern Int32 _position[JN] ;			// encoder position 
extern Int32 _motor_position[JN] ;		// hall effect encoder position 
extern Int32 _position_old[JN] ;		// do I need to add bits for taking into account multiple rotations 

extern Int32 _position_enc[JN] ;		// incremental encoder position 
extern Int32 _position_enc_old[JN] ;	// do I need to add bits for taking into account multiple rotations 

extern Int32 _real_position[JN];
extern Int32 _real_position_old[JN];
extern Int32 _desired[JN] ;				// PID ref value, computed by the trajectory generator 
extern Int16 _desired_absolute[JN] ;	// PD ref value for the calibration 
extern Int32 _set_point[JN] ;			// set point for position [user specified] 

extern Int32 _min_position[JN] ;		// software position limits 
extern Int32 _max_position[JN] ;		// software position limits 
extern Int16 _max_position_enc[JN];     // max allowed position for encoder while controlling with absolute position sensors
extern Int16 _min_position_enc[JN];     // min allowed position for encoder while controlling with absolute position sensors
extern Int32 _max_real_position[JN];		
extern Int16 _optical_ratio[JN];        //  optical encoder ratio								


// SPEED VARIABLES
extern Int16 _speed[JN] ;				// encoder speed 
extern Int16 _speed_old[JN]; 			// previous speed
extern Int16 _comm_speed[JN] ;			// brushless commutation speed 
extern Int16 _comm_speed_old[JN] ;		// previous brushless commutation speed 
extern Int32 _motor_speed[JN] ;			// brushless motor speed 
extern Int32 _motor_speed_old[JN] ;		// previous brushless motor speed 
extern Int16 _desired_vel[JN] ;			// speed reference value, computed by the trajectory gen. 
extern Int16 _accu_desired_vel[JN];		// accumultor for the fractional part of the desired vel 
extern Int16 _set_vel[JN] ;				// set point for velocity [user specified] 
extern byte  _jntVel_est_shift[JN];     // shift for the speed estimator (joint)
extern byte  _motVel_est_shift[JN];     // shift for the speed estimator (motor)
extern Int16 _max_vel[JN] ;
extern Int32 _vel_shift[JN]	;
extern Int16 _vel_counter[JN]	;
extern Int16 _vel_timeout[JN]   ;


// ACCELERATION VARIABLES
extern Int16 _accel[JN]; 				// encoder estimated acceleration
extern Int16 _accel_old[JN]; 			// previous acceleration		
extern Int16 _set_acc[JN] ;				// set point for acceleration [too low!] 
extern byte  _jntAcc_est_shift[JN];     // shift for the acceleration estimator (joint)
extern byte  _motAcc_est_shift[JN];     // shift for the acceleration estimator (motor)


// TORQUE VARIABLES
extern Int32 _desired_torque[JN] ;		// PID ref value for torque control


// POSITION PID VARIABLES
extern Int16  _error_position[JN] ;		// actual feedback error 
extern Int16  _error_position_old[JN] ;	// error at t-1 
extern Int16  _absolute_error[JN] ;		// actual feedback error from absolute sensors
extern Int16  _absolute_error_old[JN] ;	// error at t-1 
extern Int16  _pid[JN] ;				// pid result 
extern Int16  _pid_limit[JN] ;			// pid limit 
extern Int16  _velocity_calibration[JN];// vel value during calibration with hard stops 
extern Int32  _pd[JN] ;              	// pd portion of the pid
extern Int32  _pi[JN] ;
extern Int32  _integral[JN] ;			// store the sum of the integral component 
extern Int16  _integral_limit[JN] ;

extern Int16  _kp[JN] ;					// PID gains: proportional... 
extern Int16  _kd[JN] ;					// ... derivative  ...
extern Int16  _ki[JN] ;					// ... integral
extern Int16  _ko[JN] ;					// offset 
extern Int16  _kr[JN] ;					// scale factor (negative power of two) 
extern Int16  _kstp[JN] ;               // stiction compensation: positive val
extern Int16  _kstn[JN] ;               // stiction compensation: negative val
extern Int16  _kstc[JN] ;               // stiction compensation: current  val 

// TORQUE PID
extern Int16  _strain_val[JN];
extern Int16  _error_torque[JN] ;			// actual feedback error 
extern Int16  _error_old_torque[JN] ;		// error at t-1 
extern Int16  _pid_limit_torque[JN] ;		// pid limit 
extern Int16  _integral_limit_torque[JN] ;

extern Int16  _kp_torque[JN] ;				// PID gains: proportional... 
extern Int16  _kd_torque[JN] ;				// ... derivative  ...
extern Int16  _ki_torque[JN] ;				// ... integral
extern Int16  _ko_torque[JN] ;				// offset 
extern Int16  _kr_torque[JN] ;				// scale factor (negative power of two) 
extern Int16  _kstp_torque[JN] ;            // stiction compensation: positive val
extern Int16  _kstn_torque[JN] ;            // stiction compensation: negative val
extern Int16  _kff_torque[JN] ;             // feedforward gain

// OPENLOOP
extern Int16  _ko_openloop[JN] ;	        // offset 

// SPEED PID (iKart)
extern Int16  _error_speed[JN];	            // actual feedback error 
extern Int16  _error_speed_old[JN];         // error at t-1

// JOINT IMPEDANCE
extern Int16  _ks_imp[JN] ;				// stiffness coeffficient
extern Int16  _kd_imp[JN] ;				// damping coeffficient
extern Int16  _ko_imp[JN] ;				// offset
extern Int16  _kr_imp[JN] ;				// scale factor (negative power of two) 

// DECOUPLING PARAMETERS
extern float  _param_a10_coeff;
extern float  _param_a11_coeff;
extern float  _param_a20_coeff;
extern float  _param_a21_coeff;
extern float  _param_a22_coeff;
								
#if VERSION == 0x0156 || VERSION == 0x0166 || VERSION == 0x0116
// CURRENT PID
extern Int32  _desired_current[JN] ;	// PID ref value, computed by the trajectory generator 
extern Int16  _error_current[JN] ;		// current error
extern Int16  _error_current_old[JN] ;	// current error at t-1 
extern Int16  _kp_current[JN] ;			// PID gains: proportional ... 
extern Int16  _kd_current[JN] ;			// ... derivative  ...
extern Int16  _ki_current[JN] ;			// integral
extern Int16  _kr_current[JN] ;			// scale factor (negative power of two) 
extern Int32  _integral_current[JN] ;	// store the sum of the integral component 
extern Int16  _current_limit[JN] ;		// pid current limit 
extern Int32  _pd_current[JN] ;         // pd portion of the current pid
#endif


#if VERSION == 0x0153 || VERSION==0x0147 || VERSION==0x0140 || VERSION==0x0157 || VERSION==0x0150 || VERSION==0x0351 || VERSION==0x0250 || VERSION==0x0257
extern Int32  _cpl_pos_received[JN] ;		// the position of the synchronized card 
extern Int32  _cpl_pos_prediction[JN] ;		// the actual adjustment (compensation) 
extern Int32  _cpl_pos_delta[JN] ;			// velocity over the adjustment 
extern Int16  _cpl_pos_counter ;			// the adjustment step
extern Int16  _cpl_err[JN] ;        		// the error of the syncronized card
extern Int16  _cpl_pid_received[JN] ;   	// the duty of the syncronized card
extern Int16  _cpl_pid_prediction[JN] ;		// the predicted adjustment
extern Int16  _cpl_pid_delta[JN] ;			// the adjustment step 
extern Int16  _cpl_pid_counter ;			// the adjustment step
#endif

#if VERSION == 0x0113
extern Int32  _other_position[JN] ;	// the position of the synchronized card 
extern Int32  _adjustment[JN] ;		// the actual adjustment (compensation) 
extern Int32  _delta_adj[JN] ;		// velocity over the adjustment 
#endif

#if ((VERSION == 0x0121) || (VERSION == 0x0128) || (VERSION == 0x0130) || (VERSION == 0x0228) || (VERSION == 0x0230))
extern Int32  _adjustment[JN];         // the sum of the three value coming from the MAIS board
#endif

#ifdef SMOOTH_PID_CTRL
extern float _pid_old[JN] ;				// pid control at previous time step 
extern float _filt_pid[JN] ;			// filtered pid control
#endif





/******************************************************/
// macro
/******************************************************/

//void enforce_PIDlimits(byte j, Int32 PIDoutput)
#define ENFORCE_CURRENT_LIMITS(j, I) \
{ \
	/* Anti reset wind up scheme */ \
	if (I > _current_limit[j]) \
    { \
    	if ( _ki[j] != 0) \
		{ \
    		_integral_current[j] =  ((Int32) _current_limit[j]) - _pd_current[j]; \
		} \
	} \
	else \
	{\
		if (I < -_current_limit[j]) \
		{ \
			if ( _ki[j] != 0) \
			{ \
	    		_integral_current[j] =  ((Int32) (-_current_limit[j])) - _pd_current[j]; \
			} \
		} \
	}\
	/* Control saturation */ \
	if (I > (Int32) _current_limit[j]) \
    	_desired_current[j] = _current_limit[j]; \
	else \
	{\
	if ( I < - ((Int32) _current_limit[j])) \
		_desired_current[j] =  -_current_limit[j]; \
	else \
		_desired_current[j] = (Int16)(I); \
	}\
}


//void enforce_PIDlimits(byte j, Int32 PIDoutput)
#define ENFORCE_LIMITS(j, PID, PID_LIMIT) \
{ \
	/* Anti reset wind up scheme */ \
	if (PID > PID_LIMIT) \
    { \
    	if ( _ki[j] != 0) \
		{ \
    		_integral[j] =  ((Int32)(PID_LIMIT)) - _pd[j]; \
		} \
	} \
	else \
	{\
		if (PID < -PID_LIMIT) \
		{ \
			if ( _ki[j] != 0) \
			{ \
	    		_integral[j] =  ((Int32)(-PID_LIMIT)) - _pd[j]; \
			} \
		} \
	}\
	/* Control saturation */ \
	if (PID > (Int32) PID_LIMIT) \
    	_pid[j] = PID_LIMIT; \
	else \
	{\
	if ( PID < - ((Int32) PID_LIMIT)) \
		_pid[j] =  -PID_LIMIT; \
	else \
		_pid[j] = (Int16)(PID); \
	}\
}

//	/* Control hard limit reached ? i.e. abnormal high current ? */ \
//	if (_control_mode[j] == MODE_POSITION) \
//	{\
//		if (_filt_current[j] > _max_allowed_current[j] ) \
//		{\
//			_control_mode[j] = MODE_HANDLE_HARD_STOPS; \
//			_pid[j] = 0; \
//			if      (j == 0) PWMoutput[0] = 0; \
//			else if (j == 1) PWMoutput[1] = 0; \
//			else if (j == 2) PWMoutput[2] = 0; \
//			else if (j == 3) PWMoutput[3] = 0; \
//		}\
//	}\

/* Version 0x120 0x121 0x128 0x130 only: encoder reached its limit? */
/* the encoder from 0 should not go more then a certain value specified in the calibration message*/
#define ENFORCE_ENC_LIMITS(PID, POS, MAX_POS) \
{\
	if (MAX_POS>0)\
	{\
		if ((POS<0) && (PID<0))\
		{\
			/*can_printf("+ %d, %d, %d", PID, POS, MAX_POS);*/ \
			PID = 0;\
		}\
		if ((POS>MAX_POS) && (PID>0))\
		{\
			/*can_printf("- %d, %d, %d", PID, POS, MAX_POS);*/ \
			PID = 0; \
		}\
	}\
	else /*MAX_POS <= 0*/\
	{\
		if (POS > 0 && PID > 0) \
		{ \
			PID = 0; \
		} \
		if ((POS < MAX_POS ) && PID < 0) \
		{ \
			PID = 0;\
		} \
	}\
}
#define ENFORCE_ENC_LIMITS_HANDV2(PID, POS, MIN_POS, MAX_POS) \
{\
	if (MAX_POS>0)\
	{\
		if ((POS<MIN_POS) && (PID<0))\
		{\
			/*can_printf("+ %d, %d, %d", PID, POS, MAX_POS);*/ \
			PID = 0;\
		}\
		if ((POS>MAX_POS) && (PID>0))\
		{\
			/*can_printf("- %d, %d, %d", PID, POS, MAX_POS);*/ \
			PID = 0; \
		}\
	}\
	else /*MAX_POS <= 0*/\
	{\
		if (POS > MIN_POS && PID > 0) \
		{ \
			PID = 0; \
		} \
		if ((POS < MAX_POS ) && PID < 0) \
		{ \
			PID = 0;\
		} \
	}\
}

/***************************************************************************/
/**
 * This function calculates additional PWM required for back emf compensation
 ***************************************************************************/
inline Int32 compensate_bemf(byte j, Int16 motor_speed)
{
	Int32 PWM_bemf;
	PWM_bemf = ((Int32) motor_speed) * ((Int32)_backemf_gain[j]);
	
	if (PWM_bemf>=0)
	{
		PWM_bemf = PWM_bemf >> (_backemf_shift[j]+_jntVel_est_shift[j]); 
	}
	else
	{
		PWM_bemf = -(-PWM_bemf >> (_backemf_shift[j]+_jntVel_est_shift[j]));
	}
	return PWM_bemf;
}

#endif //__pidh__


