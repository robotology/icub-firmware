
/* @file       EOspeedmeter.c
   @brief      This file implements internal implementation of a speed meter from slow axis encoder.
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



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOspeedmeter.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOspeedmeter_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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

static const char s_eobj_ownname[] = "EOspeedmeter";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOspeedmeter* eo_speedmeter_New(int32_t impulse_per_revolution, float period)
{
    EOspeedmeter *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOspeedmeter), 1);

    if (o)
    {
        o->period = period;
        o->frequency = 1.0f/period;
        o->impulse_per_revolution = impulse_per_revolution;
        o->impulse_per_revolution_by_2 = impulse_per_revolution / 2;
        o->time_from_last_reading = 0.0f;
        o->last_reading = 0;
        o->speed = 0.0f;
        o->speed_new = 0.0f;
        o->first_reading = eobool_true;
    }
}

extern void eo_speedometer_EncoderValid(EOspeedmeter* o, int32_t encoder)
{
    int32_t delta;

    if (o->first_reading)
    {
        o->first_reading = ebool_false;
        o->last_reading = encoder;
        
        return;
    }

    o->time_from_last_reading += o->period;

    delta = o->last_reading - encoder;

    if (delta > o->impulse_per_revolution_by_2)
    {
        delta -= o->impulse_per_revolution;
    }
    else if (delta < -o->impulse_per_revolution_by_2)
    {
        delta += o->impulse_per_revolution;
    }
    
    if (delta)
    {
        if (delta == 1 || delta == -1)
        {
            if (o->time_from_last_reading == o->period)
            {
                float speed_old = o->speed_new;
                o->speed_new = ((float)delta) * o->frequency;
                o->speed = 0.5f * (speed_old + o->speed_new);
            }
            else
            {
                o->speed_new = ((float)delta) / o->time_from_last_reading;
                o->speed = o->speed_new;
            }
        }
        else
        {
            float speed_old = o->speed_new;
            o->speed_new = ((float)delta) * o->frequency;
            o->speed = 0.5f * (speed_old + o->speed_new);
        }

        o->last_reading = encoder;
        o->time_from_last_reading = 0.0f;
    }
    else // delta == 0
    {
        float max_speed = 1.0f / o->time_from_last_reading;

        if (o->speed > max_speed)
        {    
            o->speed_new =  max_speed;
        }
        else if (o->speed < -max_speed)
        {
            o->speed_new = -max_speed;
        }

        o->speed = o->speed_new;
    }
}

extern void eo_speedometer_EncoderError(EOspeedmeter* o)
{
    o->time_from_last_reading += o->period;
}

extern float eo_speedometer_GetSpeed(EOspeedmeter* o)
{
    return o->speed;
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




