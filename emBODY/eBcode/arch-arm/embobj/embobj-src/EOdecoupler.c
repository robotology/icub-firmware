
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

#define ZERO_ROTATION_TORQUE 1000

#define MOTORS(m) for (uint8_t m=0; m<o->n_motors; ++m)


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
    EOmotors *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOmotors), 1);

    if (o)
    {
        if (n_motors > MAX_MOTORS) n_motors = MAX_MOTORS;
        
        o->n_motors = n_motors;
        
        MOTORS(m)
        {
            o->zero_rotation_torque[m] = ZERO_ROTATION_TORQUE;
        }
    }

    return o;
}

extern void eo_motors_PWMs_Shoulder(EOmotors *o, int32_t *pwm_joint, int32_t *vel_joint, int16_t *pwm_motor)
{
    if (!o) return;
     
    int32_t vel_motor[4];
    
    vel_motor[0] =  vel_joint[0];
    vel_motor[1] = -vel_joint[0]+vel_joint[1];
    vel_motor[2] = -vel_joint[0]+vel_joint[1]+vel_joint[2];
    vel_motor[3] =  vel_joint[3];
    
    pwm_motor[0] = (int16_t)(      pwm_joint[0]);
    pwm_motor[1] = (int16_t)((65*(-pwm_joint[0]+pwm_joint[1]))/40);
    pwm_motor[2] = (int16_t)((65*(-pwm_joint[0]+pwm_joint[1]+pwm_joint[2]))/40);
    pwm_motor[3] = (int16_t)(      pwm_joint[3]);
    
    MOTORS(m)
    {
        if (pwm_motor[m])
        {
            if (vel_motor[m] > 0)
            {
                pwm_motor[m] += o->zero_rotation_torque[m];
            }
            else if (vel_motor[m] < 0)
            {
                pwm_motor[m] -= o->zero_rotation_torque[m];
            }
        }
    }
    
    pwm_motor[0] = -pwm_motor[0];
    pwm_motor[1] = -pwm_motor[1];
    pwm_motor[2] = -pwm_motor[2];
    pwm_motor[3] = -pwm_motor[3];
}

extern void eo_motors_PWMs_Waist(EOmotors *o, int32_t *pwm_joint, int32_t *vel_joint, int16_t *pwm_motor)
{
    if (!o) return;
     
    int32_t vel_motor[3];
    
    vel_motor[0] =  (vel_joint[0]-vel_joint[1])/2;
    vel_motor[1] =  (vel_joint[0]+vel_joint[1])/2;
    vel_motor[2] =   vel_joint[2];
    
    pwm_motor[0] = (int16_t)((pwm_joint[0]-pwm_joint[1])/2);
    pwm_motor[1] = (int16_t)((pwm_joint[0]+pwm_joint[1])/2);
    pwm_motor[2] = (int16_t  )pwm_joint[2];
    
    MOTORS(m)
    {
        if (pwm_motor[m])
        {
            if (vel_motor[m] > 0)
            {
                pwm_motor[m] += o->zero_rotation_torque[m];
            }
            else if (vel_motor[m] < 0)
            {
                pwm_motor[m] -= o->zero_rotation_torque[m];
            }
        }
    }
    
//? pwm_motor[0] = -pwm_motor[0];
//? pwm_motor[1] = -pwm_motor[1];
//? pwm_motor[2] = -pwm_motor[2];
}

extern void eo_motors_PWMs_UpperLeg(EOmotors *o, int32_t *pwm_joint, int32_t *vel_motor, int16_t *pwm_motor)
{
    MOTORS(m)
    {
        pwm_motor[m] = (int16_t)pwm_joint[m];
        
        if (pwm_motor[m])
        {
            if (vel_motor[m] > 0)
            {
                pwm_motor[m] += o->zero_rotation_torque[m];
            }
            else if (vel_motor[m] < 0)
            {
                pwm_motor[m] -= o->zero_rotation_torque[m];
            }
        }
    }
    
//  pwm_motor[0] = -pwm_motor[0];
//  pwm_motor[1] = -pwm_motor[1];
    pwm_motor[2] = -pwm_motor[2];
//  pwm_motor[3] = -pwm_motor[3];
}

extern void eo_motors_PWMs_Ankle(EOmotors *o, int32_t *pwm_joint, int32_t *vel_motor, int16_t *pwm_motor)
{
    MOTORS(m)
    {
        pwm_motor[m] = (int16_t)pwm_joint[m];
        
        if (pwm_motor[m])
        {
            if (vel_motor[m] > 0)
            {
                pwm_motor[m] += o->zero_rotation_torque[m];
            }
            else if (vel_motor[m] < 0)
            {
                pwm_motor[m] -= o->zero_rotation_torque[m];
            }
        }
    }
    
//? pwm_motor[0] = -pwm_motor[0];
//? pwm_motor[1] = -pwm_motor[1];
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




