
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

static int32_t normalize_angle(int32_t a);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOspeedmeter";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#ifdef OLD_STYLE_ENCODER

#error using old style encoder

#else 
/*
struct EOabsCalibratedEncoder_hid
{
    int32_t distance;
    int32_t position_last;
    int32_t position_sure;

    int32_t sign;
    int32_t offset;

    int32_t delta;

    uint16_t state_mask;
    uint8_t  first_valid_data;
};
*/

extern EOabsCalibratedEncoder* eo_absCalibratedEncoder_New(void)
{
    EOabsCalibratedEncoder *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOabsCalibratedEncoder), 1);

    if (o)
    {        
        o->distance = 0;
        o->position_last = 0;
        o->position_sure = 0;
        
        o->offset = 0;
        o->sign = 0;
        
        o->delta = 0;
        
        o->invalid_data_cnt = 0;
        o->first_valid_data = 0;
        
        o->state_mask = SM_NOT_READY;
    }

    return o;
}

extern void eo_absCalibratedEncoder_SetSign(EOabsCalibratedEncoder* o, int32_t sign)
{
    if (sign > 0) 
        o->sign =  1; 
    else if (sign < 0)
        o->sign = -1; 
    else 
        o->sign =  0;
		
    o->sign = (sign > 0) ? 1 : -1;
    RST_BITS(o->state_mask, SM_ENC_SIGN_NOT_SET);
}

extern eObool_t eo_absCalibratedEncoder_IsOk(EOabsCalibratedEncoder* o)
{
    return o->state_mask == SM_OK;
}

extern eObool_t eo_absCalibratedEncoder_IsHardFault(EOabsCalibratedEncoder* o)
{
    return o->state_mask & SM_HARDWARE_FAULT;
}

extern void eo_absCalibratedEncoder_ClearFaults(EOabsCalibratedEncoder* o)
{
    o->invalid_data_cnt = 0;
    RST_BITS(o->state_mask,SM_HARDWARE_FAULT);
}

extern void eo_absCalibratedEncoder_Calibrate(EOabsCalibratedEncoder* o, int32_t offset)
{
    o->offset = offset;
    RST_BITS(o->state_mask, SM_NOT_CALIBRATED);
    SET_BITS(o->state_mask, SM_NOT_INITIALIZED);
}

static void encoder_init(EOabsCalibratedEncoder* o, int32_t position);

extern int32_t eo_absCalibratedEncoder_Acquire(EOabsCalibratedEncoder* o, int32_t position)
{
    static const int16_t MAX_ENC_CHANGE = 3*ENCODER_QUANTIZATION;
    
    if (!o->sign) return 0;
    
    /*
    if (o->state_mask & SM_HARDWARE_FAULT)
    {
        return o->sign*o->distance;
    }
    */
	
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
    /*
    else
    {
        ++o->invalid_data_cnt;
        
        if (o->invalid_data_cnt > 500)
        {
            SET_BITS(o->state_mask,SM_HARDWARE_FAULT);
        }
    }
    */
    
    if (o->state_mask & SM_NOT_INITIALIZED)
    {
        encoder_init(o, position);
        
        return o->sign*o->distance;
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
                }
            }
        }
    }
    
    return o->sign*o->distance;
}

/*
struct EOaxleVirtualEncoder_hid
{
    int32_t sign;
    int32_t divisor;

    int32_t 
};
*/

extern EOaxleVirtualEncoder* eo_axleVirtualEncoder_New(void)
{
    EOaxleVirtualEncoder *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOaxleVirtualEncoder), 1);

    if (o)
    {        
        o->inverted = eobool_false;
        o->axle_virt_pos = 0;
        o->axle_abs_pos = 0;
        o->axle_inc_pos = 0;
    }

    return o;
}

extern void eo_axleVirtualEncoder_SetSign(EOaxleVirtualEncoder* o, eObool_t inverted)
{
    o->inverted = inverted;
}

extern void eo_axleVirtualEncoder_Acquire(EOaxleVirtualEncoder* o, int32_t axle_abs_pos, int32_t axle_virt_pos, int32_t axle_virt_vel)
{
    
    static const int32_t N_BITS_PRECISION_BOUND = GEARBOX_REDUCTION * ENCODER_QUANTIZATION;
                        
    int32_t inc = axle_abs_pos - o->axle_abs_pos;
    
    o->axle_abs_pos = axle_abs_pos;
    
    if (o->inverted)
    {
        o->velocity = -axle_virt_vel;    
        o->axle_inc_pos -= axle_virt_pos - o->axle_virt_pos;
    }
    else
    {
        o->velocity =  axle_virt_vel;
        o->axle_inc_pos += axle_virt_pos - o->axle_virt_pos;
    }

    o->axle_inc_pos -= inc*GEARBOX_REDUCTION;
                        
    LIMIT(o->axle_inc_pos, N_BITS_PRECISION_BOUND);
    
    #ifdef USE_4BIT_INC_ENC_PRECISION
    o->position = axle_abs_pos + o->axle_inc_pos/GEARBOX_REDUCTION;
    #else
    o->position = axle_abs_pos;
    #endif
    
    o->axle_virt_pos = axle_virt_pos;
}

extern int32_t eo_axleVirtualEncoder_GetPos(EOaxleVirtualEncoder* o)
{
    return o->position;
}

extern int32_t eo_axleVirtualEncoder_GetVel(EOaxleVirtualEncoder* o)
{
    return o->velocity;
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void encoder_init(EOabsCalibratedEncoder* o, int32_t position)
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
        
        o->delta = 0;

        o->first_valid_data = 0;
        //o->invalid_data_cnt = 0;
        RST_BITS(o->state_mask, SM_NOT_INITIALIZED);
    }
}

#endif

static int32_t normalize_angle(int32_t a)
{
    while (a < -TICKS_PER_HALF_REVOLUTION) a += TICKS_PER_REVOLUTION;
    while (a >  TICKS_PER_HALF_REVOLUTION) a -= TICKS_PER_REVOLUTION;

    return a;
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




