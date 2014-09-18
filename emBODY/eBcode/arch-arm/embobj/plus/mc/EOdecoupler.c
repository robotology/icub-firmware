
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
#include "EOemsControllerCfg.h"

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

#define MOTORS(m) for (uint8_t m=0; m<NAXLES; ++m)



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


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOmotors* eo_motors_New(uint8_t n_motors) 
{
    if (!n_motors) return NULL;
    
    EOmotors *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOmotors), 1);

    if (o)
    {
        o->motor_fault_mask = 0x00;

        //for (uint8_t m=0; m<MAX_MOTORS; ++m)
        //{
        //    o->zero_rotation_torque[m] = ZERO_ROTATION_TORQUE;
        //}
        
        if (n_motors > MAX_MOTORS) n_motors = MAX_MOTORS;
        
        o->n_motors = n_motors;
    }

    return o;
}

extern void eo_motor_set_motor_status(EOmotors *o, uint8_t m, uint8_t motor_error, uint8_t can_error)
{
    if (can_error || motor_error)
    {
        if (motor_error & 0x04) // external fault
        {
            o->motor_fault_mask |=  (2<<(m<<1));
        }
        else
        {
            o->motor_fault_mask |=  (1<<(m<<1));
        }
    }
    else
    {
        o->motor_fault_mask &= ~(3<<(m<<1));
    }
}

extern eObool_t eo_motor_are_motors_in_fault(EOmotors *o, uint8_t mask)
{
    return o->motor_fault_mask & mask;
}

// speed_motor  = J^-1 * speed_axis
// torque_motor = Jt   * torque_axis

extern void eo_motors_PWM(int32_t *pwm_joint, int16_t *pwm_motor, eObool_t* stiff)
{
    //if (!o) return;
    
    #if defined(SHOULDER_BOARD)
    
        //             | 1     0       0   |
        // J = dq/dm = | 1   40/65     0   |
        //             | 0  -40/65   40/65 | 

        //        |    1       0       0   |
        // J^-1 = | -65/40   65/40     0   |
        //        | -65/40   65/40   65/40 |

        //      | 1     1       0   |
        // Jt = | 0   40/65  -40/65 |
        //      | 0     0     40/65 | 
    
        {
            int16_t buff;
            
            pwm_motor[0] = (int16_t)(pwm_joint[0]);
            
            if (stiff[0])
            {    
                pwm_motor[1] = (int16_t)((-65*pwm_joint[0])/40);
                pwm_motor[2] = 0;
                //pwm_motor[2] = pwm_motor[1];
            }
            else
            {
                pwm_motor[1] = pwm_motor[2] = 0;
            }
            
            if (stiff[1])
            {
                buff = (int16_t)((65*pwm_joint[1])/40);
                pwm_motor[1] += buff;
                //pwm_motor[2] += buff;
            }
            else
            {
                pwm_motor[0] += (int16_t)(pwm_joint[1]);
                pwm_motor[1] += (int16_t)((40*pwm_joint[1])/65);
            }
            
            if (stiff[2])
            {
                pwm_motor[2] += (int16_t)((65*pwm_joint[2])/40);
            }
            else
            {
                buff = (int16_t)((40*pwm_joint[2])/65);
                pwm_motor[1] -= buff;
                pwm_motor[2] += buff;
            }
            
            // stiff
            //pwm_motor[0] = (int16_t)(pwm_joint[0]);
            //pwm_motor[1] = (int16_t)((65*(-pwm_joint[0]+pwm_joint[1]))/40);
            //pwm_motor[2] = (int16_t)((65*(-pwm_joint[0]+pwm_joint[1]+pwm_joint[2]))/40);
            
            // compliant
            //pwm_motor[0] = (int16_t)(pwm_joint[0]+pwm_joint[1]);
            //pwm_motor[1] = (int16_t)((40*(pwm_joint[1]-pwm_joint[2]))/65);
            //pwm_motor[2] = (int16_t)((40*pwm_joint[2])/65);
        
            pwm_motor[3] = (int16_t)pwm_joint[3];
        }
        
    #elif defined(WAIST_BOARD)
        
        //             |   1     1     0   |
        // J = dq/dm = |  -1     1     0   |
        //             | 44/80 44/80 44/80 | 

        //        | 1/2  -1/2    0   |
        // J^-1 = | 1/2   1/2    0   |
        //        | -1     0   80/44 |

        //      | 1  -1  44/80 |
        // Jt = | 1   1  44/80 |
        //      | 0   0  44/80 | 
        
        {
            /*
            if (stiff[0])
            {
                pwm_motor[0] = pwm_motor[1] = (int16_t)( pwm_joint[0]/2);
                pwm_motor[2] = (int16_t)(-pwm_joint[0]);
            }
            else
            {
                pwm_motor[0] = pwm_motor[1] = (int16_t)(pwm_joint[0]);
                pwm_motor[2] = 0;
            }
            
            if (stiff[1])
            {
                pwm_motor[0] += (int16_t)(-pwm_joint[1]/2);
                pwm_motor[1] += (int16_t)( pwm_joint[1]/2);
            }
            else
            {
                pwm_motor[0] += (int16_t)(-pwm_joint[1]);
                pwm_motor[1] += (int16_t)( pwm_joint[1]);
            }
            
            if (stiff[2])
            {
                pwm_motor[2] += (int16_t)((80*pwm_joint[2])/44);
            }
            else
            {
                int16_t buff = (int16_t)((44*pwm_joint[2])/80);
                //pwm_motor[0] += buff;
                //pwm_motor[1] += buff;
                pwm_motor[2] += buff;
            }
            */
            
            // stiff
            //pwm_motor[0] = (int16_t)((pwm_joint[0]-pwm_joint[1])/2);
            //pwm_motor[1] = (int16_t)((pwm_joint[0]+pwm_joint[1])/2);
            //pwm_motor[2] = (int16_t)(-pwm_joint[0]+(80*pwm_joint[2])/44);

            // compliant
            //pwm_motor[0] = (int16_t)(pwm_joint[0]-pwm_joint[1]+(44*pwm_joint[2])/80);
            //pwm_motor[1] = (int16_t)(pwm_joint[0]+pwm_joint[1]+(44*pwm_joint[2])/80);
            //pwm_motor[2] = (int16_t)                           (44*pwm_joint[2])/80);
            
            // original
            pwm_motor[0] = (int16_t)((pwm_joint[0]-pwm_joint[1])/2);
            pwm_motor[1] = (int16_t)((pwm_joint[0]+pwm_joint[1])/2);
            pwm_motor[2] = (int16_t)  pwm_joint[2];
        }
        
    #elif defined(UPPERLEG_BOARD)
        
        {
            if (stiff[0])
            {
                pwm_motor[0] = (75*pwm_joint[0])/50;
            }
            else
            {
                pwm_motor[0] = (50*pwm_joint[0])/75;
            }
            pwm_motor[1] = pwm_joint[1];
            pwm_motor[2] = pwm_joint[2];
            pwm_motor[3] = pwm_joint[3];
        }
        
    #elif defined(ANKLE_BOARD)
        
        {
            pwm_motor[0] = pwm_joint[0];
            pwm_motor[1] = pwm_joint[1];
        }
        
    #else
        MOTORS(m) pwm_motor[m] = 0;
    #endif
            
    MOTORS(m) LIMIT(pwm_motor[m], NOMINAL_CURRENT);
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




