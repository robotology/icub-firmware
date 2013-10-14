
/*  @file       EOaxisController.c
    @brief      This file implements internal implementation of a motor minimum jerk trajectory generator.
    @author     alessandro.scalzo@iit.it
    @date       27/03/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOVtheSystem.h"

extern const int32_t EMS_FREQUENCY_INT32;
extern const int32_t TICKS_PER_REVOLUTION;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOaxisController.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOaxisController_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define GET_AXIS_POSITION() (o->position)
#define GET_AXIS_VELOCITY() (o->velocity)

#define OUT_OF_RANGE(x,M) (((x)<-(M)) || ((M)<(x)))
#define IN_RANGE(x,M)     ((-(M)<(x)) && ((x)<(M)))

#define VELOCITY_TIMEOUT      100
#define TORQUE_SENSOR_TIMEOUT 200
#define CHANGE_MODE_STOP_ACC  TICKS_PER_REVOLUTION

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

#ifdef MC_CAN_DEBUG
int32_t encoder_can_pos = 0;
int32_t encoder_can_vel = 0;
//int16_t torque_debug_can[4];
#endif

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

//#define SET_POINT_REACHED(thr) (-(thr)<=o->err && o->err<=(thr) && eo_trajectory_IsDone(o->trajectory))

#define SET_BIT(bits) o->limits_mask |=  (bits)
#define RST_BIT(bits) o->limits_mask &= ~(bits)
#define CHK_BIT(bits) ((o->limits_mask & (bits)) == (bits))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOaxisController";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOaxisController* eo_axisController_New(filter_cut_freq_t cf, uint8_t id) 
{
    EOaxisController *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOaxisController), 1);

    if (o)
    {
        o->filter_cut_freq = cf;
        
        o->axisID = id;
        
        o->pidP = eo_pid_New();    
        o->pidT = eo_pid_New();
        
        o->trajectory = eo_trajectory_New();
        
        
        o->velocity_timer = 0;

        ///////////////////////////
        o->pos_min = o->pos_max = TICKS_PER_REVOLUTION/2;
        
        o->torque_ref = 0;
        o->torque_out = 0;
        o->torque_off = 0;
        o->torque_meas = 0;
        o->torque_wdog = 0;

        o->stiffness = 0;
        o->damping   = 0;
        ///////////////////////////
        
        o->openloop_out = 0;

        o->control_mode = eomc_controlmode_idle;

        o->err = 0;

        o->limits_mask = 0;
        
        o->calibrated = eobool_false;
        
        o->position = 0;
        o->velocity = 0;
    }

    return o;
}

extern eObool_t eo_axisController_HasLimits(EOaxisController *o)
{
    if (!o) return eobool_false;
    
    return CHK_BIT(MASK_HAS_LIMITS);
}

extern void eo_axisController_StartCalibration(EOaxisController *o)
{
    if (!o) return;
    
    o->calibrated = eobool_false;
    
    o->control_mode = eomc_controlmode_calib;
}

extern void eo_axisController_SetCalibrated(EOaxisController *o)
{
    if (!o) return;
    
    o->calibrated = eobool_true;
}

extern eObool_t eo_axisController_IsCalibrated(EOaxisController *o)
{
    if (!o) return eobool_false;
    
    return o->calibrated;
}

extern int32_t eo_axisController_EncRangeAdj(EOaxisController *o, int32_t* ep)
{
    if (!o) return eobool_false;
    
    int32_t jp = ep[0]-ep[1]+(40*ep[2])/65;
    
    if (o->pos_min <= jp && jp <= o->pos_max) return jp;
    
    int32_t err = o->pos_min - jp;
    int32_t min_err = err;
    int32_t jret = jp;
    
    err = jp - o->pos_max;
    if (min_err > err) min_err = err;
    
    jp += 40330; // 40330 = 40*65536/65;
        
    if (o->pos_min <= jp && jp <= o->pos_max) return jp;
        
    err = o->pos_min - jp;
    if (min_err > err)
    {
        min_err = err;
        jret = jp;
    }
    
    err = jp - o->pos_max;
    if (min_err > err)
    {
        min_err = err;
        jret = jp;
    }
    
    jp -= 80660; // 40330 = 40*65536/65;
        
    if (o->pos_min <= jp && jp <= o->pos_max) return jp;
        
    err = o->pos_min - jp;
    if (min_err > err)
    {
        min_err = err;
        jret = jp;
    }
    
    err = jp - o->pos_max;
    if (min_err > err)
    {
        min_err = err;
        jret = jp;
    }
    
    return jret;
}

extern void eo_axisController_SetLimits(EOaxisController *o, int32_t pos_min, int32_t pos_max, int32_t vel_max)
{
    if (!o) return;
    
    SET_BIT(MASK_MAX_VEL|MASK_MIN_POS|MASK_MAX_POS);    

    o->pos_min = pos_min;
    o->pos_max = pos_max;
    
    eo_trajectory_SetPosMin(o->trajectory, pos_min);
    eo_trajectory_SetPosMax(o->trajectory, pos_max);
  //eo_trajectory_SetVelMax(o->trajectory, vel_max);
}

extern void eo_axisController_SetPosMin(EOaxisController *o, int32_t pos_min)
{
    if (!o) return;
    
    SET_BIT(MASK_MIN_POS);
    
    o->pos_min = pos_min;

    eo_trajectory_SetPosMin(o->trajectory, pos_min);
}
extern void eo_axisController_SetPosMax(EOaxisController *o, int32_t pos_max)
{
    if (!o) return;
    
    SET_BIT(MASK_MAX_POS);
    
    o->pos_max = pos_max;

    eo_trajectory_SetPosMax(o->trajectory, pos_max);
}
extern void eo_axisController_SetVelMax(EOaxisController *o, int32_t vel_max)
{
    if (!o) return;
    
    SET_BIT(MASK_MAX_VEL);

  //eo_trajectory_SetVelMax(o->trajectory, vel_max);
}

extern void eo_axisController_SetVelTimeout(EOaxisController *o, int32_t vel_timeout)
{
    if (!o) return;
    
    SET_BIT(MASK_VEL_TOUT);

  //o->velocity_timeout = vel_timeout;
}

extern void eo_axisController_SetImpedance(EOaxisController *o, int32_t stiffness, int32_t damping, int32_t offset)
{
    if (o)
    {
        o->stiffness  = stiffness;
        o->damping    = (damping*EMS_FREQUENCY_INT32)/1000;
        o->torque_off = offset;
    }
}                 

extern void eo_axisController_GetImpedance(EOaxisController *o, int32_t *stiffness, int32_t *damping, int32_t *offset)
{
    if (o)
    {
        *stiffness = o->stiffness;
        *damping   = (o->damping*1000)/EMS_FREQUENCY_INT32;
        *offset    = o->torque_off;
    }
}

extern void eo_axisController_SetEncPos(EOaxisController *o, int32_t pos)
{
    if (o) o->position = pos;
} 

extern void eo_axisController_SetEncVel(EOaxisController *o, int32_t vel)
{
    if (o) o->velocity = vel;
} 

extern void eo_axisController_SetTorque(EOaxisController *o, int16_t trq)
{
    if (o)
    {
        if (o->torque_meas != trq)
        {
            o->torque_meas = trq;
            o->torque_wdog = TORQUE_SENSOR_TIMEOUT;
        }
    }
}

extern void eo_axisController_SetOutput(EOaxisController *o, int16_t out)
{
    if (o) o->openloop_out = out;
}

/*
extern void eo_axisController_GetPosRef(EOaxisController *o, int32_t *pos, int32_t *avg_vel)
{
     eo_trajectory_GetPosReference(o->trajectory, pos, avg_vel);
}
*/

extern void eo_axisController_SetPosRef(EOaxisController *o, int32_t pos, int32_t avg_vel)
{
    if (!o) return;
    
    if (!CHK_BIT(MASK_POS_READY)) return;

    switch (o->control_mode)
    {
    case eomc_controlmode_velocity:
        o->control_mode = eomc_controlmode_position;
    case eomc_controlmode_position:
        eo_trajectory_SetPosReference(o->trajectory, pos, avg_vel);
        break;
        
    case eomc_controlmode_torque:
    case eomc_controlmode_impedance_vel:
        o->control_mode = eomc_controlmode_impedance_pos;
    case eomc_controlmode_impedance_pos:
        eo_trajectory_SetPosReference(o->trajectory, pos, avg_vel);
        break;

    case eomc_controlmode_idle: 
    case eomc_controlmode_calib: 
    case eomc_controlmode_openloop: 
        // command ignored
        return;    
    }    
}

extern void eo_axisController_SetPosRaw(EOaxisController *o, int32_t pos)
{
    if (!o) return;
    
    if (!CHK_BIT(MASK_POS_READY)) return;

    switch (o->control_mode)
    {
    case eomc_controlmode_velocity:
        o->control_mode = eomc_controlmode_position;
    case eomc_controlmode_position:
        eo_trajectory_SetPosRaw(o->trajectory, pos);
        break;
        
    case eomc_controlmode_torque:
    case eomc_controlmode_impedance_vel:
        o->control_mode = eomc_controlmode_impedance_pos;
    case eomc_controlmode_impedance_pos:
        eo_trajectory_SetPosRaw(o->trajectory, pos);
        break;

    case eomc_controlmode_idle: 
    case eomc_controlmode_calib: 
    case eomc_controlmode_openloop: 
        // command ignored
        return;    
    }    
}

extern void eo_axisController_SetVelRef(EOaxisController *o, int32_t vel, int32_t avg_acc)
{
    if (!o) return;
    
    if (!CHK_BIT(MASK_POS_READY)) return;
    
    switch (o->control_mode)
    {
    case eomc_controlmode_position:
        o->control_mode = eomc_controlmode_velocity;
    case eomc_controlmode_velocity:
        o->velocity_timer = VELOCITY_TIMEOUT;
        eo_trajectory_SetVelReference(o->trajectory, vel, avg_acc);
        break;
        
    case eomc_controlmode_torque:
    case eomc_controlmode_impedance_pos:
        o->control_mode = eomc_controlmode_impedance_vel;
    case eomc_controlmode_impedance_vel:
        o->velocity_timer = VELOCITY_TIMEOUT;
        eo_trajectory_SetVelReference(o->trajectory, vel, avg_acc);
        break;  

    case eomc_controlmode_idle: 
    case eomc_controlmode_calib: 
    case eomc_controlmode_openloop: 
        // command ignored
        return;    
    }
}

extern void eo_axisController_SetTrqRef(EOaxisController *o, int32_t trq)
{
    if (!o) return;
    
    if (!CHK_BIT(MASK_TRQ_READY))
    {
        o->torque_ref = 0;
        
        return;
    }
    
    if (o->control_mode == eomc_controlmode_torque)
    {
        o->torque_ref = trq;
    }
    else
    {
        o->torque_ref = 0;
    }
}

extern eOmc_controlmode_t eo_axisController_GetControlMode(EOaxisController *o)
{
    if (!o) return eomc_controlmode_idle;
    
    return o->control_mode;
}

extern eObool_t eo_axisController_SetControlMode(EOaxisController *o, eOmc_controlmode_command_t cmc)
{
    if (!o) return eobool_false;
    
    if (o->control_mode == eomc_controlmode_calib)
    {
        if (cmc == eomc_controlmode_cmd_switch_everything_off)
        {
            o->control_mode = eomc_controlmode_idle;
            
            return eobool_true;
        }
        
        return eobool_false;
    }
    
    if (!o->calibrated || !CHK_BIT(MASK_POS_READY))
    {
        o->control_mode = eomc_controlmode_idle;
        
        return eobool_false;
    }
    
    switch (cmc)
    { 
    case eomc_controlmode_cmd_position:
        eo_pid_Reset(o->pidP);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION(), CHANGE_MODE_STOP_ACC);
        o->control_mode = eomc_controlmode_position;
        break;
    
    case eomc_controlmode_cmd_velocity:   
        eo_pid_Reset(o->pidP);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION(), CHANGE_MODE_STOP_ACC);
        o->control_mode = eomc_controlmode_velocity;
        break;

    case eomc_controlmode_cmd_switch_everything_off:
        eo_pid_Reset(o->pidP);
        eo_pid_Reset(o->pidT);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION(), CHANGE_MODE_STOP_ACC);
        o->control_mode = eomc_controlmode_idle;
        break;
    
    case eomc_controlmode_cmd_torque:
        if (!CHK_BIT(MASK_TRQ_READY)) return eobool_false;
        eo_pid_Reset(o->pidT);
        o->torque_ref = 0;
        o->err = 0;
        o->control_mode = eomc_controlmode_torque;
        break;
    
    case eomc_controlmode_cmd_impedance_pos:
        if (!CHK_BIT(MASK_TRQ_READY)) return eobool_false;
        eo_pid_Reset(o->pidT);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION(), CHANGE_MODE_STOP_ACC);
        o->err = 0;
        o->control_mode = eomc_controlmode_impedance_pos;
        break;
   
    case eomc_controlmode_cmd_impedance_vel:
        if (!CHK_BIT(MASK_TRQ_READY)) return eobool_false;
        eo_pid_Reset(o->pidT);
        eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION(), CHANGE_MODE_STOP_ACC);
        o->err = 0;
        o->control_mode = eomc_controlmode_impedance_vel;
        break;

    case eomc_controlmode_cmd_current:
        return eobool_false;
    
    case eomc_controlmode_cmd_openloop:
        return eobool_false;
        //o->control_mode = eomc_controlmode_openloop;
        //break;
    }
    
    return eobool_true;
}

extern int16_t eo_axisController_PWM(EOaxisController *o, eObool_t *big_error_flag)
{
    if (!o) return 0;
    
    float pos_ref;
    float vel_ref;
    float acc_ref;
    
    int32_t pos = GET_AXIS_POSITION();
    int32_t vel = GET_AXIS_VELOCITY();
    
    switch (o->control_mode)
    {
        case eomc_controlmode_calib:
        {
            if (o->calibrated)
            {
                eo_pid_Reset(o->pidP);
                eo_trajectory_Init(o->trajectory, pos, vel, 0);
                eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION(), CHANGE_MODE_STOP_ACC); 
                o->control_mode = eomc_controlmode_position;
            }
            
            return 0;
        }
        
        case eomc_controlmode_idle:
        {
            eo_trajectory_Init(o->trajectory, pos, vel, 0);
            o->err = 0;
            return 0;
        }

        case eomc_controlmode_openloop:
        {
            eo_trajectory_Init(o->trajectory, pos, vel, 0);
            o->err = 0;
            return o->openloop_out;
        }

        case eomc_controlmode_velocity:
            if (o->velocity_timer)
            {
                if (!--o->velocity_timer)
                {
                    eo_trajectory_VelocityStop(o->trajectory);
                    o->control_mode = eomc_controlmode_position;    
                }
            }
        case eomc_controlmode_position:
        {            
            eo_trajectory_Step(o->trajectory, &pos_ref, &vel_ref, &acc_ref);
            
            o->err = pos_ref - pos;

            int32_t pwm = eo_pid_PWM_piv(o->pidP, o->err, vel, vel_ref, acc_ref);
            
            if (big_error_flag)
            {
                if (OUT_OF_RANGE(o->err,1820/*10 deg*/))
                {
                    *big_error_flag = eobool_true;
                }                
            }
            
            return pwm;
        }
        
        
        case eomc_controlmode_impedance_vel:
            if (o->velocity_timer)
            {
                if (!--o->velocity_timer)
                {
                    eo_trajectory_VelocityStop(o->trajectory);
                    o->control_mode = eomc_controlmode_impedance_pos;

                    o->err = 0;
                }
            }
        case eomc_controlmode_impedance_pos:
        {       
            eo_trajectory_Step(o->trajectory, &pos_ref, &vel_ref, &acc_ref);
            
            int32_t err = pos_ref - pos;
            
            o->torque_ref = o->torque_off + (o->stiffness*err)/1000 + o->damping*(err - o->err);

            o->err = err;
        }    
        case eomc_controlmode_torque:
        {
            if (o->torque_wdog)
            {
                --o->torque_wdog;
            }
            else
            {
                o->torque_ref = o->torque_meas = 0;
                
                return 0;
            }
            
            int32_t pwm = 0; // = eo_pid_PWM_pi_3_0Hz_1stLPF(o->pidT, o->torque_ref, o->torque_meas);
            
            switch (o->filter_cut_freq)
            {
            case NO_FILTER:
                pwm = eo_pid_PWM_pi(o->pidT, o->torque_ref, o->torque_meas); 
                break;
            
            case CUT_FREQ_1_1_Hz: 
                pwm = eo_pid_PWM_pi_1_1Hz_1stLPF(o->pidT, o->torque_ref, o->torque_meas); 
                break;
                
            case CUT_FREQ_3_0_Hz: 
                pwm = eo_pid_PWM_pi_3_0Hz_1stLPF(o->pidT, o->torque_ref, o->torque_meas); 
                break;
            }
            
            if (big_error_flag)
            {
                if ((pos <= o->pos_min && o->torque_ref < 0) || (pos >= o->pos_max && o->torque_ref > 0))
                {
                    //if (pwm >  2500) pwm =  2500;
                    //if (pwm < -2500) pwm = -2500;
                
                    *big_error_flag = eobool_true;
                }
            }
            
            return pwm;
        }   
    }
    
    return 0;   
}

extern void eo_axisController_Stop(EOaxisController *o)
{
    if (o) eo_trajectory_Stop(o->trajectory, GET_AXIS_POSITION(), CHANGE_MODE_STOP_ACC);
}

extern EOpid* eo_axisController_GetPosPidPtr(EOaxisController *o)
{
    return o ? o->pidP : NULL;
}

extern EOpid* eo_axisController_GetTrqPidPtr(EOaxisController *o)
{
    return o ? o->pidT : NULL;
}

extern void eo_axisController_SetPosPid(EOaxisController *o, float K, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff)
{
    if (!o) return;
    
    SET_BIT(MASK_POS_PID);

    eo_pid_SetPid(o->pidP, K, Kd, Ki, Imax, Ymax, Yoff);
}
extern void eo_axisController_SetTrqPid(EOaxisController *o, float K, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff)
{
    if (!o) return;
    
    SET_BIT(MASK_TRQ_PID);

    eo_pid_SetPid(o->pidT, K, Kd, Ki, Imax, Ymax, Yoff);
}

extern void eo_axisController_GetJointStatus(EOaxisController *o, eOmc_joint_status_basic_t* jointStatus)
{
    if (!o) return;
    
    jointStatus->controlmodestatus   = o->control_mode;
    jointStatus->position            = GET_AXIS_POSITION();           
    jointStatus->velocity            = GET_AXIS_VELOCITY();          
    
    #warning (ALE) to be implemented
    jointStatus->acceleration        = 0; //eo_speedometer_GetAcceleration(o->speedmeter);       
    
    jointStatus->torque              = o->torque_meas;
}

extern eObool_t eo_axisController_GetMotionDone(EOaxisController *o)
{
    if (!o) return eobool_false;
    
    switch (o->control_mode)
    {
        case eomc_controlmode_idle:
        case eomc_controlmode_torque:
        case eomc_controlmode_openloop:
            return eobool_false;
        
        case eomc_controlmode_position:
        case eomc_controlmode_velocity:
        case eomc_controlmode_impedance_pos:
        case eomc_controlmode_impedance_vel:
            return eo_trajectory_IsDone(o->trajectory);
    }
    
    return eobool_false;
}


extern void eo_axisController_GetActivePidStatus(EOaxisController *o, eOmc_joint_status_ofpid_t* pidStatus)
{
    if (!o)
    {
        pidStatus->reference = 0;
        pidStatus->output    = 0;
        pidStatus->error     = 0;
        
        return;
    }
    
    switch (o->control_mode)
    {
        case eomc_controlmode_idle:
            pidStatus->reference = 0;
            break;

        case eomc_controlmode_openloop:
            pidStatus->reference = o->openloop_out;
            break;

        case eomc_controlmode_velocity:
        case eomc_controlmode_impedance_vel:
            pidStatus->reference = eo_trajectory_GetVel(o->trajectory);
            break;

        case eomc_controlmode_position:
        case eomc_controlmode_impedance_pos:
            pidStatus->reference = eo_trajectory_GetPos(o->trajectory);
            break;

        case eomc_controlmode_torque:
            pidStatus->reference = o->torque_ref;
            break;
    }
    
    switch (o->control_mode)
    {
        case eomc_controlmode_idle:
            pidStatus->output    = 0;
            pidStatus->error     = 0;
            break;

        case eomc_controlmode_openloop:
            pidStatus->output    = o->openloop_out;
            pidStatus->error     = 0;
            break;

        case eomc_controlmode_velocity:
        case eomc_controlmode_position:
            eo_pid_GetStatus(o->pidP, &(pidStatus->output), &(pidStatus->error));
            break;

        case eomc_controlmode_torque:
        case eomc_controlmode_impedance_pos:
        case eomc_controlmode_impedance_vel:
            eo_pid_GetStatus(o->pidT, &(pidStatus->output), &(pidStatus->error));
            break;
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




