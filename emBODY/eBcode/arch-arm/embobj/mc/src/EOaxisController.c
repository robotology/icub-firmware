
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

//extern int16_t posref_can;
//extern int16_t encoder_can;


extern const int32_t EMS_FREQUENCY_INT32;

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
// empty-section

#define TICKS_PER_REVOLUTION 0x10000


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

#define SET_POINT_REACHED(thr) (-(thr)<=o->err && o->err<=(thr) && eo_trajectory_IsDone(o->trajectory))

#define SET_BIT(bits) o->ready_mask |=  (bits)
#define RST_BIT(bits) o->ready_mask &= ~(bits)
#define CHK_BIT(bits) ((o->ready_mask & (bits)) == (bits))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOaxisController";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOaxisController* eo_axisController_New(void) 
{
    EOaxisController *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOaxisController), 1);

    if (o)
    {
        o->pidP = eo_pid_New();    
        o->pidT = NULL;
        
        o->speedmeter = eo_speedmeter_New(TICKS_PER_REVOLUTION); // ticks per revolution
        o->trajectory = eo_trajectory_New(TICKS_PER_REVOLUTION);
        
        ///////////////////////////////////////
        #warning (ALE) hard coded config values
        o->acc_stop_cmode   = TICKS_PER_REVOLUTION;
        o->acc_stop_alarm   = TICKS_PER_REVOLUTION*2;
        o->calib_timeout_ms = 5000;
        o->calib_max_error  = TICKS_PER_REVOLUTION/1024;
        o->reach_max_error  = TICKS_PER_REVOLUTION/1024;
        ///////////////////////////////////////
        
        o->vel_timer   = 0;
        o->vel_timeout = EMS_FREQUENCY_INT32/50;

        ///////////////////////////
        
        o->torque_ref = 0;
        o->torque_out = 0;
        o->torque_off = 0;
        o->torque_meas = 0;
        o->torque_last_pos_err = 0;
        o->torque_damp_lp_filt = 0;

        o->stiffness = 0;
        o->damping   = 0;
        ///////////////////////////
        
        o->openloop_out = 0;

        o->control_mode = CM_IDLE;

        o->err = 0.0f;

        o->ready_mask = 0;
    }

    return o;
}

extern void eo_axisController_StartCalibration(EOaxisController *o, int32_t pos, int32_t vel, int32_t offset)
{
    if (!o) return;
    
    if (!CHK_BIT(MASK_MIN_POS|MASK_MAX_POS|MASK_POS_PID)) return;
    
    eo_speedometer_EncoderCalibrate(o->speedmeter, offset);
	
    eo_trajectory_Init(o->trajectory, eo_speedometer_GetDistance(o->speedmeter), eo_speedometer_GetVelocity(o->speedmeter), 0);

    SET_BIT(MASK_CALIB_OK);
    
    return;
    
    RST_BIT(MASK_CALIB_OK);

    //o->calib_timeout_ms = timeout_ms;    
    //o->calib_max_error = max_error;

    o->control_mode = CM_CALIB_ABS_POS_SENS;
    
    eo_trajectory_SetPosReference(o->trajectory, pos, vel);
}

extern void eo_axisController_SetLimits(EOaxisController *o, int32_t pos_min, int32_t pos_max, int32_t vel_max)
{
    if (!o) return;
    
    SET_BIT(MASK_MAX_VEL|MASK_MIN_POS|MASK_MAX_POS);    

    eo_trajectory_SetPosMin(o->trajectory, pos_min);
    eo_trajectory_SetPosMax(o->trajectory, pos_max);
    eo_trajectory_SetVelMax(o->trajectory, vel_max);
}

extern void eo_axisController_SetPosMin(EOaxisController *o, int32_t pos_min)
{
    if (!o) return;
    
    SET_BIT(MASK_MIN_POS);

    eo_trajectory_SetPosMin(o->trajectory, pos_min);
}
extern void eo_axisController_SetPosMax(EOaxisController *o, int32_t pos_max)
{
    SET_BIT(MASK_MAX_POS);

    eo_trajectory_SetPosMax(o->trajectory, pos_max);
}
extern void eo_axisController_SetVelMax(EOaxisController *o, int32_t vel_max)
{
    if (!o) return;
    
    SET_BIT(MASK_MAX_VEL);

    eo_trajectory_SetVelMax(o->trajectory, vel_max);
}

extern void eo_axisController_SetVelTimeout(EOaxisController *o, int32_t vel_timeout)
{
    if (!o) return;
    
    SET_BIT(MASK_VEL_TOUT);

    o->vel_timeout = vel_timeout;
}

extern void eo_axisController_SetStiffness(EOaxisController *o, int32_t stiffness)
{
    if (!o) return;
    
    o->stiffness = stiffness;
}                 

extern void eo_axisController_ReadEncPos(EOaxisController *o, int32_t pos)
{
    if (!o) return;
    
    eo_speedometer_EncoderValid(o->speedmeter, pos);
} 

extern void eo_axisController_SkipEncPos(EOaxisController *o)
{
    if (!o) return;
    
    eo_speedometer_EncoderError(o->speedmeter);
}

extern void eo_axisController_ReadSpeed(EOaxisController *o, int32_t speed)
{
    if (!o) return;
    
    eo_speedometer_ReadSpeed(o->speedmeter, speed);
} 

/*
extern int32_t eo_axisController_GetVelocity(EOaxisController *o)
{
    return eo_speedometer_GetVelocity(o->speedmeter);
}
*/

extern void eo_axisController_SetOutput(EOaxisController *o, int16_t out)
{
    if (!o) return;
    
    o->openloop_out = out;
}

extern void eo_axisController_SetPosRef(EOaxisController *o, int32_t pos, int32_t avg_vel)
{
    if (!o) return;
    
    if (!CHK_BIT(MASK_POS_INIT_OK)) return;

    switch (o->control_mode)
    {
    case CM_IDLE: 
    case CM_TORQUE: 
    case CM_OPENLOOP: 
        return;

    case CM_POS_VEL:
    case CM_VELOCITY:
        o->control_mode = CM_POSITION;
    case CM_POSITION:
    case CM_CALIB_ABS_POS_SENS:
        eo_trajectory_SetPosReference(o->trajectory, pos, avg_vel);
        break;
        
    case CM_IMPEDANCE_VEL:
        o->control_mode = CM_IMPEDANCE_POS;
    case CM_IMPEDANCE_POS:
        #warning (ALE) to be implemented
        break;            
    }    
}

extern void eo_axisController_SetVelRef(EOaxisController *o, int32_t vel, int32_t avg_acc)
{
    if (!o) return;
    
    if (!CHK_BIT(MASK_POS_INIT_OK)) return;
    
    switch (o->control_mode)
    {
    case CM_IDLE: 
    case CM_TORQUE: 
    case CM_OPENLOOP:
    case CM_CALIB_ABS_POS_SENS:
        return;
         
    case CM_POSITION:
        o->control_mode = CM_POS_VEL;
    case CM_POS_VEL:
        o->vel_timer = o->vel_timeout;
        eo_trajectory_BoostStart(o->trajectory, vel, avg_acc);
        break;

    //case CM_POSITION:
        //o->control_mode = CM_VELOCITY;
        //eo_trajectory_StopBoost(o->trajectory);
    case CM_VELOCITY:
        eo_trajectory_SetVelReference(o->trajectory, vel, avg_acc);
        break;
        
    case CM_IMPEDANCE_POS:
        o->control_mode = CM_IMPEDANCE_VEL;
    case CM_IMPEDANCE_VEL:
        #warning (ALE) to be implemented
        break;          
    }
}

extern void eo_axisController_SetTrqRef(EOaxisController *o, int32_t trq)
{
    if (!o) return;
    
    if (!CHK_BIT(MASK_TRQ_INIT_OK)) return;
    
    #warning (ALE) to be implemented
    o->torque_ref = trq;
}

extern eObool_t eo_axisController_SetControlMode(EOaxisController *o, control_mode_t control_mode)
{
    if (!o) return eobool_false;
    
    if (o->control_mode == control_mode) return eobool_true;

    switch (control_mode)
    {
        case CM_POSITION:
            if (!CHK_BIT(MASK_POS_INIT_OK)) return eobool_false;
        
        case CM_FORCE_POSITION:
            eo_pid_Reset(o->pidP);
            eo_trajectory_Stop(o->trajectory, o->acc_stop_cmode);            
            o->control_mode = CM_POSITION;
            
            break;

        
        case CM_VELOCITY:
            if (!CHK_BIT(MASK_POS_INIT_OK)) return eobool_false;
            
            eo_pid_Reset(o->pidP);
            eo_trajectory_SetVelReference(o->trajectory, 0, o->acc_stop_cmode); 
            
            break;

        
        case CM_IDLE:
            eo_pid_Reset(o->pidP);
            eo_pid_Reset(o->pidT);

            eo_trajectory_Stop(o->trajectory, o->acc_stop_cmode);

            break;
    }

    o->control_mode = control_mode;

    return eobool_true;
}

extern int16_t eo_axisController_PWM(EOaxisController *o)
{
    if (!o) return 0;
    
    float pos_ref;
    float vel_ref;
    
    int32_t pos = eo_speedometer_GetDistance(o->speedmeter);
    int32_t vel = eo_speedometer_GetVelocity(o->speedmeter);
    
    if (o->vel_timer)
    {
        if (!--o->vel_timer)
        {
            eo_trajectory_BoostTimeout(o->trajectory);

            if (o->control_mode == CM_POS_VEL)
            {
                o->control_mode = CM_POSITION;
            }    
        }
    }
    
    switch (o->control_mode)
    {
        case CM_IDLE:
        {
            eo_trajectory_Init(o->trajectory, pos, vel, 0);
            o->err = 0.0f;
            return 0;
        }

        case CM_OPENLOOP:
        {
            eo_trajectory_Init(o->trajectory, pos, vel, 0);
            o->err = 0.0f;
            return o->openloop_out;
        }
        
        case CM_CALIB_ABS_POS_SENS:
        {
            eo_trajectory_PosStep(o->trajectory, &pos_ref, &vel_ref);
            
            o->err = pos_ref - pos;

            if (eo_trajectory_IsDone(o->trajectory))
            {
                if (!o->calib_timeout_ms)
                {
                    RST_BIT(MASK_CALIB_OK);
                    eo_pid_SafeMode(o->pidP, eobool_true);
                    eo_pid_SafeMode(o->pidT, eobool_true);
                    o->control_mode = CM_IDLE;
                
                    return 0;
                }

                --o->calib_timeout_ms;
                
                if (SET_POINT_REACHED(o->calib_max_error))
                {
                    SET_BIT(MASK_CALIB_OK);
                    eo_pid_SafeMode(o->pidP, eobool_false);
                    eo_pid_SafeMode(o->pidT, eobool_false);
                    o->control_mode = CM_POSITION;
                }
            } 

            return eo_pid_PWM2(o->pidP, o->err, vel_ref, vel);
        }

        case CM_POS_VEL:
        case CM_POSITION:
        case CM_VELOCITY:
        {
            if (eo_trajectory_PosStep(o->trajectory, &pos_ref, &vel_ref))
            {
                eo_pid_Reset(o->pidP); // position limit reached
            }

            o->err = pos_ref - pos;

            return eo_pid_PWM2(o->pidP, o->err, vel_ref, vel);
        }
       
        case CM_IMPEDANCE_POS:
        case CM_TORQUE:
        case CM_IMPEDANCE_VEL:
            eo_trajectory_Init(o->trajectory, pos, vel, 0);
            #warning (ALE) to be implemented
            break;     
    }
    
    return 0;   
}

extern void eo_axisController_Stop(EOaxisController *o)
{
    if (!o) return;
    
    eo_trajectory_Stop(o->trajectory, o->acc_stop_cmode);
}

extern EOpid* eo_axisController_GetPosPidPtr(EOaxisController *o)
{
    if (!o) return NULL;
    
    return o->pidP;
}

extern EOpid* eo_axisController_GetTrqPidPtr(EOaxisController *o)
{
    if (!o) return NULL;
    
    return o->pidT;
}

extern void eo_axisController_SetPosPid(EOaxisController *o, float K, float Kd, float Ki, float Ymax, float Imax, float Yoff)
{
    if (!o) return;
    
    SET_BIT(MASK_POS_PID);

    eo_pid_SetPid(o->pidP, K, Kd, Ki, Ymax, Imax, Yoff);
}
extern void eo_axisController_SetTrqPid(EOaxisController *o, float K, float Kd, float Ki, float Ymax, float Imax, float Yoff)
{
    if (!o) return;
    
    SET_BIT(MASK_TRQ_PID);

    eo_pid_SetPid(o->pidT, K, Kd, Ki, Ymax, Imax, Yoff);
}

extern void eo_axisController_GetJointStatus(EOaxisController *o, eOmc_joint_status_basic_t* jointStatus)
{
    if (!o) return;
    
    jointStatus->controlmodestatus   = o->control_mode;
    jointStatus->position            = eo_speedometer_GetDistance(o->speedmeter);           
    jointStatus->velocity            = eo_speedometer_GetVelocity(o->speedmeter);          
    
    #warning (ALE) to be implemented
    jointStatus->acceleration        = 0; //eo_speedometer_GetAcceleration(o->speedmeter);       
    jointStatus->torque              = 0;
    
}

extern eObool_t eo_axisController_GetMotionDone(EOaxisController *o)
{
    if (!o) return eobool_false;
    
    switch (o->control_mode)
    {
        case CM_IDLE:
        case CM_OPENLOOP:
            return eobool_false;

        case CM_POS_VEL:
        case CM_POSITION:
        case CM_VELOCITY:
        case CM_CALIB_ABS_POS_SENS:
            return eo_trajectory_IsDone(o->trajectory);
       
        case CM_IMPEDANCE_POS:
        case CM_TORQUE:
        case CM_IMPEDANCE_VEL:
        #warning (ALE) to be implemented
            return eobool_false;
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
        case CM_IDLE:
            pidStatus->reference = 0;
            pidStatus->output    = 0;
            pidStatus->error     = 0;
            break;

        case CM_OPENLOOP:
            pidStatus->reference = o->openloop_out;
            pidStatus->output    = o->openloop_out;
            pidStatus->error     = 0;
            break;

        case CM_VELOCITY:
            pidStatus->reference = eo_trajectory_GetVel(o->trajectory);
            eo_pid_GetStatus(o->pidP, &(pidStatus->output), &(pidStatus->error));
            break;

        case CM_POS_VEL:
        case CM_POSITION:
        case CM_CALIB_ABS_POS_SENS:
            pidStatus->reference = eo_trajectory_GetPos(o->trajectory);
            eo_pid_GetStatus(o->pidP, &(pidStatus->output), &(pidStatus->error));
            break;

        case CM_TORQUE:
            #warning (ALE) to be implemented
            pidStatus->reference = 0;

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




