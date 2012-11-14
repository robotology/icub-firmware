
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
        o->impulse_per_revolution = impulse_per_revolution;
        o->impulse_per_revolution_by_2 = impulse_per_revolution/2;
        
        o->time_from_last_change = 0;
        o->first = eobool_true;
        
        o->calibration = 0;
        o->distance = 0;
        o->pos = 0;
        
        o->odo_x_freq = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;
    }

    return o;
}

extern int32_t eo_speedometer_GetVelocity(EOspeedmeter* o)
{
    return o->speed_filt;
}

extern int32_t eo_speedometer_GetDistance(EOspeedmeter* o)
{
    return o->distance + o->odo_x_freq/EMS_FREQUENCY_INT32;
}

extern void eo_speedometer_EncoderCalibrate(EOspeedmeter* o, int32_t new_calibration)
{
    o->distance += new_calibration - o->calibration;
    o->calibration = new_calibration; 
}

extern void eo_speedometer_FastEncoderRead(EOspeedmeter* o, int32_t vel)
{
    #ifdef USE_2FOC_FAST_ENCODER
    o->speed_filt = (o->speed_filt + vel + 1)/2;
    o->odo_x_freq += o->speed_filt;
    #endif
}

extern void eo_speedometer_SlowEncoderRead(EOspeedmeter* o, int32_t pos)
{
    if (o->first)
    {
        o->first = eobool_false;
        o->time_from_last_change = 0;
        
        o->distance = pos;
        o->pos = pos;
        
        o->odo_x_freq = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;
                
        return;
    }

    ////////////////////////////////////////
    
    ++o->time_from_last_change;
    
    if (pos == ENC_INVALID) return;
    
    int32_t delta = pos - o->pos;

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
        
        LIMIT(-16000, o->odo_x_freq, 16000);
        
        o->pos = pos;
    }
    
    #else
    
    if (delta<=-32 || delta>=32 || (delta<0 && o->dir==-1) || (delta>0 && o->dir==1))
    {
        int32_t delta_x_freq = delta*EMS_FREQUENCY_INT32; 
            
        o->distance   += delta;
        o->odo_x_freq -= delta_x_freq;

        o->pos = pos;
        
        o->speed = delta_x_freq / o->time_from_last_change;
        
        //int32_t speed_max = (o->time_from_last_change>2) ? delta_x_freq/(o->time_from_last_change-1) : delta_x_freq;
        
        if (delta > 0)
        {
            //if (o->speed_filt > speed_max) o->speed_filt = speed_max;
            if (o->speed_filt > o->speed) o->speed_filt = o->speed;
            
            o->dir =  1;
        }
        else
        {
            //if (o->speed_filt < speed_max) o->speed_filt = speed_max;
            if (o->speed_filt < o->speed) o->speed_filt = o->speed;
            
            o->dir = -1;
        }
        
        o->time_from_last_change = 0;
    }
    else
    {
        int32_t speed_max = 16000 / o->time_from_last_change;
        
        LIMIT(-speed_max, o->speed, speed_max);
        LIMIT(-speed_max, o->speed_filt, speed_max);
    }
    
    if (o->time_from_last_change > 500)
    {
        o->odo_x_freq = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;
    }
    else
    {
        if (!o->speed_filt || (o->speed_filt<0 ^ o->speed<0))
        {
            o->speed_filt = o->speed;
        }
        else
        {
            o->speed_filt = (7*o->speed_filt + o->speed + 4)/8;
        }
        
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




