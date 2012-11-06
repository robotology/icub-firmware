
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

extern const int32_t EMS_FREQUENCY_INT32;


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


#define LIMIT(min,x,max) if (x>(max)) x=(max); else if (x<(min)) x=(min)

#define DELTA_THR 64

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

extern EOspeedmeter* eo_speedmeter_New(int32_t impulse_per_revolution)
{
    EOspeedmeter *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOspeedmeter), 1);

    if (o)
    {
        o->FREQUENCYxTHR = EMS_FREQUENCY_INT32 * (DELTA_THR + DELTA_THR/2);
        o->impulse_per_revolution = impulse_per_revolution;
        o->impulse_per_revolution_by_2 = impulse_per_revolution/2;
        o->first_reading = eobool_true;
        o->time_from_last_reading = 0;
        o->last_valid_reading = 0;
        o->last_reading = 0;
        o->calibration = 0;
        o->distance  = 0;
        
        o->odo_x_freq = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;
    }

    return o;
}

extern void eo_speedometer_EncoderCalibrate(EOspeedmeter* o, int32_t new_calibration)
{
    o->distance += new_calibration - o->calibration;
    o->calibration = new_calibration; 
}

extern void eo_speedometer_ReadSpeed(EOspeedmeter* o, int32_t speed)
{
    o->speed_filt = (o->speed_filt + speed + 1)/2;
    o->odo_x_freq += o->speed_filt;
}

extern void eo_speedometer_EncoderError(EOspeedmeter* o)
{
    ++o->time_from_last_reading;
}

extern int32_t eo_speedometer_GetVelocity(EOspeedmeter* o)
{
    return o->speed_filt;
}

extern int32_t eo_speedometer_GetDistance(EOspeedmeter* o)
{
    return o->distance + o->odo_x_freq/EMS_FREQUENCY_INT32;
}

extern void eo_speedometer_EncoderValid(EOspeedmeter* o, int32_t encoder)
{
    if (o->first_reading)
    {
        o->first_reading = eobool_false;
        o->time_from_last_reading = 0;
        o->last_reading = encoder;
        o->distance = encoder;

        o->odo_x_freq = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;
                
        return;
    }

    ////////////////////////////////////////
    
    int32_t delta = encoder - o->last_reading;

    if (delta > o->impulse_per_revolution_by_2)
    {
        delta -= o->impulse_per_revolution;
    }
    else if (delta < -o->impulse_per_revolution_by_2)
    {
        delta += o->impulse_per_revolution;
    }
    
    #ifdef USE_2FOC_FAST_ENCODER
    
    if (delta)
    {
        o->distance   += delta;
        o->odo_x_freq -= delta*EMS_FREQUENCY_INT32;
        
        o->last_reading = encoder;
    }
    
    #else
    
    ++o->time_from_last_reading;
    
    if (delta<=-32 || delta>=32 || (delta<0 && o->dir==-1) || (delta>0 && o->dir==1))
    {
        int32_t delta_x_freq = delta*EMS_FREQUENCY_INT32; 
            
        o->distance   += delta;
        o->odo_x_freq -= delta_x_freq;

        o->last_reading = encoder;
        
        o->speed = delta_x_freq / o->time_from_last_reading;
        
        if (delta<0 ^ o->speed_filt<0) o->speed_filt = o->speed;
        
        o->time_from_last_reading = 0;
    }
    else
    {
        int32_t speed_max = 16000 / o->time_from_last_reading;
        
        LIMIT(-speed_max, o->speed, speed_max);
    }
    
    if (o->time_from_last_reading > 500)
    {
        o->odo_x_freq = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;
    }
    else
    {
        o->speed_filt = (3*o->speed_filt + o->speed + 2) / 4;
        
        o->odo_x_freq += o->speed_filt;
        
        LIMIT(-16000, o->odo_x_freq, 16000);
    }
    
    #endif
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




