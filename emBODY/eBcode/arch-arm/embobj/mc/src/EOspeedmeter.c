
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

#include "hal_led.h"

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

#define IMPULSE_x_REVOLUTION      65536
#define IMPULSE_x_REVOLUTION_by_2 32768

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
static void encoder_init(EOspeedmeter* o, int32_t position);

static int32_t normalize_angle(int32_t a);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOspeedmeter";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOspeedmeter* eo_speedmeter_New(void)
{
    EOspeedmeter *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOspeedmeter), 1);

    if (o)
    {        
        o->time = 0;
        
        o->distance = 0;
        o->position_last = 0;
        o->position_sure = 0;
        
        o->calibrated = eobool_false;
        o->offset     = 0;
        o->enc_sign   = 0;
        
        o->odo_x_freq = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;
        
        o->invalid_data_cnt = 0;
        o->first_valid_data = 0;
        
        o->hard_fault = eobool_false;
        o->is_started = eobool_false;
        
        o->sindex = 0;
        
        o->sample[0] = 0;
        o->sample[1] = 0;
        o->sample[2] = 0;
        o->sample[3] = 0;
        o->sample[4] = 0;
    }

    return o;
}

extern void eo_speedometer_SetEncSign(EOspeedmeter* o, int32_t enc_sign)
{
    o->enc_sign = (enc_sign > 0) ? 1 : -1;
}

extern void eo_speedometer_Calibrate(EOspeedmeter* o, int32_t offset)
{
    o->offset = offset;
    
    o->is_started = eobool_false;
    o->calibrated = eobool_true;
}

extern void eo_speedometer_Reset(EOspeedmeter* o)
{
    o->hard_fault = eobool_false;
    o->is_started = eobool_false;
    
    o->invalid_data_cnt = 0;
    o->first_valid_data = 0;
}

extern eObool_t eo_speedometer_IsHardFault(EOspeedmeter* o)
{
    return o->hard_fault;
}

extern eObool_t eo_speedometer_IsStarted(EOspeedmeter* o)
{
    return o->is_started;
}

extern eObool_t eo_speedometer_IsCalibrated(EOspeedmeter* o)
{
    return o->calibrated;
}

extern int32_t eo_speedometer_GetVelocity(EOspeedmeter* o)
{    
    return o->speed_filt;
}

extern int32_t eo_speedometer_GetDistance(EOspeedmeter* o)
{
    return o->distance + o->odo_x_freq/EMS_FREQUENCY_INT32;
}

#ifdef USE_2FOC_FAST_ENCODER
extern void eo_speedometer_FastEncoderRead(EOspeedmeter* o, int32_t velocity)
{
    if (!o->enc_sign) return;
    
    o->speed_filt = (o->speed_filt + velocity + 1)/2;
    o->odo_x_freq += o->speed_filt;
}
#endif

extern void eo_speedometer_SlowEncoderRead(EOspeedmeter* o, int32_t position)
{
    if (!o || o->hard_fault) return;
    
    if (!o->enc_sign) return;
    
    if (position != ENC_INVALID)
    {
        position -= o->offset;
        
        if (position < 0)
        {
            position += IMPULSE_x_REVOLUTION;
        }
        else if (position >= IMPULSE_x_REVOLUTION)
        {
            position -= IMPULSE_x_REVOLUTION;
        }
        
        position *= o->enc_sign;
    }
    
    if (!o->is_started)
    {
        encoder_init(o, position);
        return;
    }
    
    //////////////////////////////

    if (o->time < 32000) ++o->time;

    if (o->invalid_data_cnt >= 500)
    {
        o->hard_fault = eobool_true;
        o->invalid_data_cnt = 0;
        return;
    }
    
    if (position == ENC_INVALID)
    {
        o->invalid_data_cnt += 10;
    }
    else
    {        
        if (o->invalid_data_cnt) --o->invalid_data_cnt;

        int32_t check = normalize_angle(position - o->position_last);
        o->position_last = position;

        if (-48 <= check && check <= 48)
        {
            int32_t delta = normalize_angle(position - o->position_sure);

            if (delta<=-32 || delta>=32 || (o->dir==-1 && delta<=-16) || (o->dir==1 && delta>=16))
            {
                int32_t delta_x_freq = delta*EMS_FREQUENCY_INT32;

                o->odo_x_freq -= delta_x_freq;
                o->position_sure = position;
                o->distance += delta;
                
                int32_t speed = delta_x_freq / o->time;
                
                LIMIT(-48000, speed, 48000);
                o->time = 0;
                
                if (!o->dir || ((o->speed_filt<0) ^ (speed<0)))
                {
                    o->sample[0] = speed;
                    o->sample[1] = speed;
                    o->sample[2] = speed;
                    o->sample[3] = speed;
                    o->sample[4] = speed;
                    
                    o->speed_filt = speed;
                }
                else
                {
                    o->sample[o->sindex++] = speed;
                    o->sindex %= 5;
                
                    //o->speed_filt = (o->sample[0]+o->sample[1]+o->sample[2]+o->sample[3]+o->sample[4]) / 5;
                    
                    o->speed_filt = 0;
                    
                    // triangular filter
                    for (int32_t i=0; i<5; ++i)
                    {
                        o->speed_filt += (i+1)*o->sample[(o->sindex+i)%5];
                    }
                    
                    o->speed_filt /= 15;
                }                
                
                
                /*
                int32_t acc = o->sample[0];
                int32_t min = acc, max = acc;
                for (uint8_t i=1; i<5; ++i)
                {
                    acc += o->sample[i];
                  
                    if (min > o->sample[i]) min = o->sample[i];
                    if (max < o->sample[i]) max = o->sample[i];
                }
                o->speed_filt = (acc-min-max)/3;
                */
                
                o->dir = (delta>0) ? 1 : -1;
            }
        }
        else
        {
            o->invalid_data_cnt += 11;
        }
    }
    
    if (o->time < 31999)
    {
        if (o->time > 1)
        {
            int32_t speed_max = 32000 / (o->time-1);
            LIMIT(-speed_max, o->speed_filt, speed_max);
        }
        
        o->odo_x_freq += o->speed_filt;
        
        LIMIT(-16000, o->odo_x_freq, 16000);
    }
    else if (o->time == 31999) // is still
    {
        o->odo_x_freq = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;
        
        //o->sindex = 0;
        
        o->sample[0] = 0;
        o->sample[1] = 0;
        o->sample[2] = 0;
        o->sample[3] = 0;
        o->sample[4] = 0;
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void encoder_init(EOspeedmeter* o, int32_t position)
{
    if (!o) return;
    
    if (position == ENC_INVALID)
    {
        o->first_valid_data = 0;
        return;
    }
    
    if (!o->first_valid_data)
    {
        o->position_last = position;
    }
    
    if (o->position_last != position)
    {
        o->first_valid_data = 0;
        return;
    }
    
    if (++o->first_valid_data >= 3)
    {
        o->time = 0;
        
        o->position_last = position;
        o->position_sure = position;

        o->distance = position;
        
        o->odo_x_freq = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;

        o->first_valid_data = 0;
        o->invalid_data_cnt = 0;
        
        o->is_started = eobool_true;
        
        o->sindex = 0;
        
        o->sample[0] = 0;
        o->sample[1] = 0;
        o->sample[2] = 0;
        o->sample[3] = 0;
        o->sample[4] = 0;
    }
}

static int32_t normalize_angle(int32_t a)
{
    while (a < -IMPULSE_x_REVOLUTION_by_2) a += IMPULSE_x_REVOLUTION;
    while (a >  IMPULSE_x_REVOLUTION_by_2) a -= IMPULSE_x_REVOLUTION;

    return a;
}
























#if 0
extern void eo_speedometer_SlowEncoderRead(EOspeedmeter* o, int32_t position)
{
    if (!o || o->hard_fault) return;
    
    if (!o->enc_sign) return;
    
    if (position != ENC_INVALID)
    {
        position -= o->offset;
        
        if (position < 0)
        {
            position += IMPULSE_x_REVOLUTION;
        }
        else if (position >= IMPULSE_x_REVOLUTION)
        {
            position -= IMPULSE_x_REVOLUTION;
        }
        
        position *= o->enc_sign;
    }
    
    if (!o->is_started)
    {
        encoder_init(o, position);
        return;
    }
    
    //////////////////////////////

    if (o->time < 1000) ++o->time;

    if (o->invalid_data_cnt >= 500)
    {
        o->hard_fault = eobool_true;
        o->invalid_data_cnt = 0;
        return;
    }
    
    if (position == ENC_INVALID)
    {
        o->invalid_data_cnt += 10;
    }
    else
    {        
        if (o->invalid_data_cnt) --o->invalid_data_cnt;

        int32_t check = normalize_angle(position - o->position_last);
        o->position_last = position;

        if (-48 <= check && check <= 48)
        {
            int32_t delta = normalize_angle(position - o->position_sure);

            if (delta<=-32 || delta>=32 || (o->dir==-1 && delta<=-16) || (o->dir==1 && delta>=16))
            {
                int32_t delta_x_freq = delta*EMS_FREQUENCY_INT32;

                o->odo_x_freq -= delta_x_freq;
                o->position_sure = position;
                o->distance += delta;
                
                int32_t slowiness = ((o->time)<<10) / delta;
                
                if (!o->dir || ((o->slowiness<0) ^ (slowiness<0)))
                {
                    o->slowiness = slowiness;
                }
                else
                {
                    o->slowiness = (4*o->slowiness + slowiness + 2) / 5;
                }
                
                o->speed_filt = 1024000 / (o->slowiness = slowiness);

                LIMIT(-48000, o->speed_filt, 48000);
                
                o->time = 0;
                
                o->dir = (delta>0) ? 1 : -1;
            }
        }
        else
        {
            o->invalid_data_cnt += 11;
        }
    }
    
    if (o->time < 999)
    {
        if (o->time > 1)
        {
            int32_t speed_max = 32000 / (o->time-1);
            LIMIT(-speed_max, o->speed_filt, speed_max);
        }
        
        o->odo_x_freq += o->speed_filt;
        
        LIMIT(-16000, o->odo_x_freq, 16000);
    }
    else if (o->time == 999) // is still
    {
        o->odo_x_freq = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;
        
        //o->sindex = 0;
        
        o->sample[0] = 0;
        o->sample[1] = 0;
        o->sample[2] = 0;
        o->sample[3] = 0;
        o->sample[4] = 0;
    }
}

extern void eo_speedometer_SlowEncoderRead(EOspeedmeter* o, int32_t position)
{
    if (!o || o->hard_fault) return;
    
    if (!o->is_started)
    {
        encoder_init(o, position);
        return;
    }
    
    //////////////////////////////

    ++o->time;

    if (position == ENC_INVALID)
    {
        if (o->invalid_data_cnt < 200)
        {
            o->invalid_data_cnt += 20;
        }
        else
        {
            o->hard_fault = eobool_true;
            o->invalid_data_cnt = 0;
            return;
        }
    }
    else
    {
        if (o->invalid_data_cnt) --o->invalid_data_cnt;

        int32_t check = normalize_angle(position - o->position_last);
        o->position_last = position;

        if (check >= -16 && check <= 16)
        {
            int32_t delta = normalize_angle(position - o->position_sure);

            #ifdef USE_2FOC_FAST_ENCODER
            if (delta)
            {
                o->odo_x_freq -= delta*EMS_FREQUENCY_INT32;   
                //LIMIT(-16000, o->odo_x_freq, 16000);
        
                o->distance += delta;
                o->position_sure = position;
            }
            #else

            if (delta<=-32 || delta>=32 || (o->dir==-1 && delta<=-16) || (o->dir==1 && delta>=16))
            {
                int32_t delta_x_freq = delta*EMS_FREQUENCY_INT32;

                o->speed = delta_x_freq / o->time;

                o->odo_x_freq -= delta_x_freq;
                o->distance += delta;

                o->position_sure = position;
                o->time = 0;

                if (delta > 0)
                {
                    o->dir =  1;
                    if (o->speed_filt > o->speed) o->speed_filt = o->speed;
                }
                else // if (delta < 0)
                {
                    o->dir = -1;
                    if (o->speed_filt < o->speed) o->speed_filt = o->speed;
                }
            }
            #endif
        }
    }
    
    #ifndef USE_2FOC_FAST_ENCODER
    
    if (o->time > 500) // is still
    {
        o->odo_x_freq = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;
    }
    else
    {
        if (o->time)
        {
            int32_t speed_max = 16000 / o->time;
        
            LIMIT(-speed_max, o->speed,      speed_max);
            LIMIT(-speed_max, o->speed_filt, speed_max);
        }
        
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
#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




