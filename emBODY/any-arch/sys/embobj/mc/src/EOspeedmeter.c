
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

extern EOspeedmeter* eo_speedmeter_New(int32_t impulse_per_revolution, int32_t frequency)
{
    EOspeedmeter *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOspeedmeter), 1);

    if (o)
    {
        o->FREQUENCY = frequency;
        o->FREQUENCYxTHR = frequency * (DELTA_THR + 1);
        o->impulse_per_revolution = impulse_per_revolution;
        o->impulse_per_revolution_by_2 = impulse_per_revolution / 2;
        o->time_from_last_reading = 0;
        o->last_reading = 0;
        o->speed = 0;
        o->first_reading = eobool_true;
    }

    return o;
}

extern void eo_speedometer_EncoderValid(EOspeedmeter* o, int32_t encoder)
{
    int32_t delta;

    if (o->first_reading)
    {
        o->first_reading = eobool_false;
        o->time_from_last_reading = 0;
        o->last_reading = encoder;
        
        return;
    }

    delta = encoder - o->last_reading;

    if (delta > o->impulse_per_revolution_by_2)
    {
        delta -= o->impulse_per_revolution;
    }
    else if (delta < -o->impulse_per_revolution_by_2)
    {
        delta += o->impulse_per_revolution;
    }

    ++o->time_from_last_reading;

    if (delta <= -DELTA_THR || delta >= DELTA_THR)
    {
        o->speed = (15 * o->speed + (delta * o->FREQUENCY) / o->time_from_last_reading);
        
        if (o->speed >= 8)        
        {
            o->speed =  ((8+o->speed)>>4);
        }
        else if (o->speed <= -8)
        {
            o->speed = -((8-o->speed)>>4);
        }
        else
        {
            o->speed = 0;
        }
         
        o->time_from_last_reading = 0.0f;
        o->last_reading = encoder;
    }
    else
    {
        int32_t max_speed = o->FREQUENCYxTHR / o->time_from_last_reading;

        if (o->speed > max_speed)
        {    
            o->speed = max_speed;
        }
        else if (o->speed < -max_speed)
        {
            o->speed = -max_speed;
        }
    }
}

extern void eo_speedometer_EncoderError(EOspeedmeter* o)
{
    ++o->time_from_last_reading;
}

extern int32_t eo_speedometer_GetSpeed(EOspeedmeter* o)
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




