
/*  @file       EOdecoupler.c
    @brief      This file implements internal implementation of motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOdecoupler.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOdecoupler_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define ZERO_ROTATION_TORQUE 0

#define CABLE_WARNING_x_100  2000

#define MOTORS(m) for (uint8_t m=0; m<o->n_motors; ++m)

#define SAFE_MAX_CURRENT 2500
#define NOMINAL_CURRENT  6500
#define LIMIT(x,L) if (x>(L)) x=(L); else if (x<-(L)) x=-(L)

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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOdecoupler";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOmotors* eo_motors_New(uint8_t n_motors) 
{
    if (!n_motors) return NULL;
    
    EOmotors *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOmotors), 1);

    if (o)
    {
        o->motor_error_mask = 0x00;
        
        for (uint8_t m=0; m<MAX_MOTORS; ++m)
        {
            o->zero_rotation_torque[m] = ZERO_ROTATION_TORQUE;
        }
        
        if (n_motors > MAX_MOTORS) n_motors = MAX_MOTORS;
        
        o->n_motors = n_motors;
    }

    return o;
}

/*
extern eObool_t eo_motors_is_motorON(EOmotors *o, uint8_t m)
{
    if (o) return 0 != (o->active_motors_mask & (1<<m));
    
    return eobool_false;
}
*/

extern void eo_motor_set_motor_error_status(EOmotors *o, uint8_t m, eObool_t bError)
{
    if (!o) return;
    
    if (bError)
    {
        o->motor_error_mask |=  (1<<m);
    }
    else
    {
        o->motor_error_mask &= ~(1<<m);
    }
}

extern uint8_t eo_motors_PWM(EOmotors *o, emsBoardType_t board_type, int32_t *pwm_joint, int16_t *pwm_motor, uint8_t alarm_mask, eObool_t control_mode_trq)
{
    if (!o) return 0xFF;
    
    uint8_t stop_mask = 0;
    
    switch (board_type)
    {
    case EMS_GENERIC:
        break;
    
    case EMS_SHOULDER:
        if (o->motor_error_mask & 0x07)
        {
            stop_mask |= 0x07;
            pwm_motor[0] = pwm_motor[1] = pwm_motor[2] = 0;
        }
        else
        {
            /*
            if (control_mode_trq)
            {
                pwm_motor[0] = (int16_t)  pwm_joint[0];
                pwm_motor[1] = (int16_t)(-pwm_joint[0]+pwm_joint[1]);
                pwm_motor[2] = (int16_t)(-pwm_joint[0]+pwm_joint[1]+pwm_joint[2]);
            }
            else
            */
            {
                pwm_motor[0] = (int16_t)(      pwm_joint[0]);
                pwm_motor[1] = (int16_t)((65*(-pwm_joint[0]+pwm_joint[1]))/40);
                pwm_motor[2] = (int16_t)((65*(-pwm_joint[0]+pwm_joint[1]+pwm_joint[2]))/40);
            }
        }
        
        if (o->motor_error_mask & 0x08)
        {            
            stop_mask |= 0x08;
            pwm_motor[3] = 0;
        }
        else
        {   
            pwm_motor[3] = (int16_t)  pwm_joint[3];
        }
        
        break;

    case EMS_WAIST:
        if (o->motor_error_mask & 0x07)
        {
            stop_mask |= 0x07;
            pwm_motor[0] = pwm_motor[1] = pwm_motor[2] = 0;
        }
        else
        {
            pwm_motor[0] = (int16_t)((pwm_joint[0]-pwm_joint[1])/2);
            pwm_motor[1] = (int16_t)((pwm_joint[0]+pwm_joint[1])/2);
            pwm_motor[2] = (int16_t)  pwm_joint[2];
        }
        
        break;

    case EMS_UPPERLEG:
        if (o->motor_error_mask & 0x04)
        {
            stop_mask |= 0x04;
            pwm_motor[2] = 0;
        }
        else
        {
            pwm_motor[2] = pwm_joint[2];
        }
        
        if (o->motor_error_mask & 0x08)
        {
            stop_mask |= 0x08;
            pwm_motor[3] = 0;
        }
        else
        {
            pwm_motor[3] = pwm_joint[3];
        }
    
    case EMS_ANKLE:
        if (o->motor_error_mask & 0x01)
        {
            stop_mask |= 0x01;
            pwm_motor[0] = 0;
        }
        else
        {
            pwm_motor[0] = pwm_joint[0];
        }
        
        if (o->motor_error_mask & 0x02)
        {
            stop_mask |= 0x02;
            pwm_motor[1] = 0;
        }
        else
        {
            pwm_motor[1] = pwm_joint[1];
        }
    }
    
    if (alarm_mask)
    {
        switch (board_type)
        {
        case EMS_SHOULDER:
            if (alarm_mask & 0x08) LIMIT(pwm_motor[3], SAFE_MAX_CURRENT);
            if (alarm_mask & 0x07)
            {
                LIMIT(pwm_motor[0], SAFE_MAX_CURRENT);
                LIMIT(pwm_motor[1], SAFE_MAX_CURRENT);
                LIMIT(pwm_motor[2], SAFE_MAX_CURRENT);
            }
            break;
      
        case EMS_WAIST:
            if (alarm_mask & 0x07)
            {
                LIMIT(pwm_motor[0], SAFE_MAX_CURRENT);
                LIMIT(pwm_motor[1], SAFE_MAX_CURRENT);
                LIMIT(pwm_motor[2], SAFE_MAX_CURRENT);
            }
            break;

        case EMS_UPPERLEG:
            if (alarm_mask & 0x04) LIMIT(pwm_motor[2], SAFE_MAX_CURRENT);
            if (alarm_mask & 0x08) LIMIT(pwm_motor[3], SAFE_MAX_CURRENT);        
            
        case EMS_ANKLE:
            if (alarm_mask & 0x01) LIMIT(pwm_motor[0], SAFE_MAX_CURRENT);
            if (alarm_mask & 0x02) LIMIT(pwm_motor[1], SAFE_MAX_CURRENT);
        }
    }
    
    MOTORS(m) LIMIT(pwm_motor[m], NOMINAL_CURRENT);
    
    return stop_mask;
}

extern eObool_t eo_motors_CableLimitAlarm(int32_t j0, int32_t j1, int32_t j2)
{
    int32_t cond = 171*(j0-j1);
    
    if (cond < -34700 + CABLE_WARNING_x_100) return eobool_true;
 
    cond -= 171*j2;
    
    if (cond < -36657 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >  11242 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j1+j2;
    
    if (cond <  -6660 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >  21330 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j0;
    
    if (cond <  -9600 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >    500 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j1;
    
    if (cond <          CABLE_WARNING_x_100) return eobool_true;
    if (cond >  19500 - CABLE_WARNING_x_100) return eobool_true;
    
    cond = 100*j2;
    
    if (cond < -9000 + CABLE_WARNING_x_100) return eobool_true;
    if (cond >  9000 - CABLE_WARNING_x_100) return eobool_true;
    
    return eobool_false;
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




