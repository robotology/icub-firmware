
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

#define FAST_ENC_UNINIT 0x80000000

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
        //o->time = 0;
        
        o->distance = 0;
        o->position_last = 0;
        o->position_sure = 0;
        
        o->offset   = 0;
        o->enc_sign = 0;
        
        o->distance_x_reduction = 0;
        o->fast_enc_pos = FAST_ENC_UNINIT;
        
        o->speed = 0;
         
        o->delta = 0;
        
        //o->invalid_data_cnt = 0;
        o->first_valid_data = 0;
        
        o->state_mask = SM_NOT_READY;
    }

    return o;
}

extern void eo_speedometer_SetEncSign(EOspeedmeter* o, int32_t enc_sign)
{
    o->enc_sign = (enc_sign > 0) ? 1 : -1;
    RST_BITS(o->state_mask, SM_ENC_SIGN_NOT_SET);
}

extern void eo_speedometer_Calibrate(EOspeedmeter* o, int32_t offset)
{
    o->offset = offset;
    o->distance_x_reduction = 0;
    RST_BITS(o->state_mask, SM_NOT_CALIBRATED);
    SET_BITS(o->state_mask, SM_NOT_INITIALIZED);
}

/*
extern void eo_speedometer_Reset(EOspeedmeter* o)
{
    o->hard_fault = eobool_false;
    o->is_started = eobool_false;
    
    o->invalid_data_cnt = 0;
    o->first_valid_data = 0;
}
*/

extern eObool_t eo_speedometer_IsOk(EOspeedmeter* o)
{
    return o->state_mask == SM_OK;
}

extern eObool_t eo_speedometer_IsHardFault(EOspeedmeter* o)
{
    return o->state_mask & SM_HARDWARE_FAULT;
}

extern int32_t eo_speedometer_GetVelocity(EOspeedmeter* o)
{    
    return o->enc_sign*o->speed;
}

extern int32_t eo_speedometer_GetDistance(EOspeedmeter* o)
{
#ifdef USE_2FOC_FAST_ENCODER
    return o->enc_sign*(o->distance + o->distance_x_reduction/GEARBOX_REDUCTION);
#else
    return o->enc_sign*o->distance;
#endif
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
        //o->time = 0;
        
        o->position_last = position;
        o->position_sure = position;

        o->distance = position;
        
        o->distance_x_reduction = 0;
        
        o->delta = 0;
        o->speed = 0;

        o->first_valid_data = 0;
        //o->invalid_data_cnt = 0;
        RST_BITS(o->state_mask, SM_NOT_INITIALIZED);
    }
}

static int32_t normalize_angle(int32_t a)
{
    while (a < -TICKS_PER_HALF_REVOLUTION) a += TICKS_PER_REVOLUTION;
    while (a >  TICKS_PER_HALF_REVOLUTION) a -= TICKS_PER_REVOLUTION;

    return a;
}

#ifdef USE_2FOC_FAST_ENCODER
extern void eo_speedometer_FastEncoderRead(EOspeedmeter* o, int32_t speed, int32_t fast_enc_pos)
{
    o->speed = speed;

    if (o->fast_enc_pos == FAST_ENC_UNINIT) o->fast_enc_pos = fast_enc_pos;
    
    o->distance_x_reduction += fast_enc_pos - o->fast_enc_pos;
    
    o->fast_enc_pos = fast_enc_pos;
}
#endif

extern void eo_speedometer_SlowEncoderRead(EOspeedmeter* o, int32_t position)
{
    static const int16_t MAX_ENC_CHANGE = 3*ENCODER_QUANTIZATION;
    
    if (!o) return;
    
    if (!o->enc_sign) return;
    
    if (position != ENC_INVALID)
    {
        position -= o->offset;
        
        if (position < 0)
        {
            position += TICKS_PER_REVOLUTION;
        }
        else if (position >= TICKS_PER_REVOLUTION)
        {
            position -= TICKS_PER_REVOLUTION;
        }
    }
    
    if (o->state_mask & SM_NOT_INITIALIZED)
    {
        encoder_init(o, position);
        
        return;
    }
    
    if (position != ENC_INVALID)
    {        
        int32_t check = normalize_angle(position - o->position_last);
        
        o->position_last = position;

        if (-MAX_ENC_CHANGE <= check && check <= MAX_ENC_CHANGE)
        {
            int32_t delta = normalize_angle(position - o->position_sure);
            
            if (delta || o->delta)
            {
                o->position_sure = position;
                
                int32_t inc = (o->delta + delta) >> 1;
                
                o->delta = delta;
                
                if (inc)
                {
                    o->distance += inc;
                    
#ifdef USE_2FOC_FAST_ENCODER
                    {
                        static const int32_t N_BITS_PRECISION_BOUND = GEARBOX_REDUCTION * ENCODER_QUANTIZATION;
                        
                        o->distance_x_reduction -= inc*GEARBOX_REDUCTION;
                        
                        LIMIT(o->distance_x_reduction, N_BITS_PRECISION_BOUND);
                    }
#endif
                }
            }
        }
    }
}

#if 0
extern void eo_speedometer_SlowEncoderRead(EOspeedmeter* o, int32_t position)
{
    if (!o) return;
    
    if (!o->enc_sign) return;
        
    //if (o->hard_fault) return;
    
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

    /*
    if (o->invalid_data_cnt >= 10000)
    {
        o->hard_fault = eobool_true;
        o->invalid_data_cnt = 0;
        return;
    }
    */
    
    if (position == ENC_INVALID)
    {
        //o->invalid_data_cnt += 10;
    }
    else
    {        
        //if (o->invalid_data_cnt) --o->invalid_data_cnt;

        int32_t check = normalize_angle(position - o->position_last);
        o->position_last = position;

        if (-48 <= check && check <= 48)
        {
            int32_t delta = normalize_angle(position - o->position_sure);

            if (delta<=-32 || delta>=32 || (o->dir!=1 && delta<=-16) || (o->dir!=-1 && delta>=16))
            //if (delta)
            {        
                o->position_sure = position;
                
                o->distance += delta;

                o->delta_filt = 0;
                o->delta = delta*EMS_FREQUENCY_INT32;
                
                o->window = o->time;
                
                o->time = 0;
                
                o->dir = (delta>0) ? 1 : -1;
            }
            else if (delta)
            {
                o->dir = 0;
            }
        }
        /*
        else
        {
            o->invalid_data_cnt += 11;
        }
        */
    }
    
    if (o->time < 999)
    {        
        int32_t w = o->window - o->time;
        if (w <= 0) w = 1;

        o->distance_pred = o->distance + (o->delta_filt + 500) / EMS_FREQUENCY_INT32;
    
        o->delta_filt = (w*o->delta_filt + o->delta + (w>>1)) / (w+1);
        
    
    
        int32_t distance_filt_old = o->distance_filt;

        o->distance_filt = (w*o->distance_filt + o->distance + (w>>1)) / (w+1);
        
        int32_t speed_old = o->speed;
        
        o->speed = EMS_FREQUENCY_INT32*(o->distance_filt - distance_filt_old);

        o->speed_filt = (198*o->speed_filt + speed_old + o->speed + 100) / 200;
    }
    else if (o->time == 999) // is still
    {
        o->distance_filt = o->distance_pred = o->distance;
        
        o->delta_filt = 0;
        o->delta = 0;
        o->speed_filt = 0;
        o->speed = 0;
        o->dir = 0;
    }
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




