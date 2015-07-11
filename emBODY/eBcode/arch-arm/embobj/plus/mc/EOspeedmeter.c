
/* @file       EOspeedmeter.c
   @brief      This file implements internal implementation of a speed meter from slow axis encoder.
   @author     alessandro.scalzo@iit.it
   @date       27/03/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include <string.h>

#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EoError.h"
#include "EOtheErrorManager.h"
#include "EOVtheSystem.h"
//#include "hal_led.h"
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

extern EOabsCalibratedEncoder* eo_absCalibratedEncoder_New(uint8_t ID)
{
    EOabsCalibratedEncoder *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOabsCalibratedEncoder), 1);

    if (o)
    {        
        o->ID = ID;
        o->distance = 0;
        o->position_last = 0;
        o->position_sure = 0;
        
        #ifndef USE_2FOC_FAST_ENCODER
        o->velocity = 0;
        #endif
        
        o->offset = 0;
        o->sign = 0;
        
        o->delta = 0;
        
        o->invalid_fault_cnt = 0;
        o->timeout_fault_cnt = 0;
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
		
    //o->sign = (sign > 0) ? 1 : -1;
    RST_BITS(o->state_mask, SM_ENC_SIGN_NOT_SET);
}

extern eObool_t eo_absCalibratedEncoder_IsOk(EOabsCalibratedEncoder* o)
{
    return o->state_mask == SM_OK;
}

extern uint16_t eo_absCalibratedEncoder_IsHardFault(EOabsCalibratedEncoder* o)
{
    return o->state_mask & SM_HARDWARE_FAULT;
}

extern uint16_t eo_absCalibratedEncoder_IsTimeoutFault(EOabsCalibratedEncoder* o)
{
    return o->state_mask & SM_TIMEOUT_FAULT;
}

extern uint16_t eo_absCalibratedEncoder_IsInvalidFault(EOabsCalibratedEncoder* o)
{
    return o->state_mask & SM_INVALID_FAULT;
}

extern void eo_absCalibratedEncoder_ClearFaults(EOabsCalibratedEncoder* o)
{
    o->invalid_fault_cnt = 0;
    o->timeout_fault_cnt = 0;
    RST_BITS(o->state_mask,SM_HARDWARE_FAULT);
}

extern void eo_absCalibratedEncoder_Calibrate(EOabsCalibratedEncoder* o, int32_t offset)
{
    o->offset = offset;
    RST_BITS(o->state_mask, SM_NOT_CALIBRATED);
    SET_BITS(o->state_mask, SM_NOT_INITIALIZED);
}

static void encoder_init(EOabsCalibratedEncoder* o, int32_t position, uint8_t error_mask);

extern int32_t eo_absCalibratedEncoder_Acquire(EOabsCalibratedEncoder* o, int32_t position, uint8_t error_mask)
{
    static const int16_t MAX_ENC_CHANGE = 7*ENCODER_QUANTIZATION;
    
    if (!o->sign) return 0;
	
    if (!error_mask)
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
        
        o->invalid_fault_cnt = 0;
        o->timeout_fault_cnt = 0;
    }
    else
    {
        if (error_mask & 0x01)
        {
            if (o->invalid_fault_cnt > 50)
            {
                SET_BITS(o->state_mask, SM_INVALID_FAULT);
            }
            else
            {
                ++o->invalid_fault_cnt;
            }
        }
        else
        {
            o->invalid_fault_cnt = 0;
        }    
        
        if (error_mask & 0x02)
        {
            if (o->timeout_fault_cnt > 50)
            {
                SET_BITS(o->state_mask, SM_TIMEOUT_FAULT);
            }
            else
            {
                ++o->timeout_fault_cnt;
            }
        }
        else
        {
            o->timeout_fault_cnt = 0;
        }  
    }
    
    if (o->state_mask & SM_NOT_INITIALIZED)
    {
        encoder_init(o, position, error_mask);
        
        #ifndef USE_2FOC_FAST_ENCODER
        o->velocity = 0;
        #endif
        
        return o->sign*o->distance;
    }
    
    if (!error_mask)
    {        
        int32_t check = normalize_angle(position - o->position_last);
        
        o->position_last = position;

        if (-MAX_ENC_CHANGE <= check && check <= MAX_ENC_CHANGE)
        {
            int32_t delta = normalize_angle(position - o->position_sure);
            
            //if (delta || o->delta)
            if (delta)
            {
                o->position_sure = position;
                
                //int32_t inc = (o->delta + delta) >> 1;
                
                o->delta = delta;
                
                o->distance += delta;
                //if (inc)
                //{
                //    o->distance += inc;
                //}
                
                #ifndef USE_2FOC_FAST_ENCODER
                //o->velocity = (7*o->velocity + o->sign*EMS_FREQUENCY_INT32*inc) >> 3;
                o->velocity = (7*o->velocity + o->sign*EMS_FREQUENCY_INT32*delta) >> 3;
                #endif
            }
            #ifndef USE_2FOC_FAST_ENCODER
            else
            {
                o->velocity = (7*o->velocity) >> 3;
            }
            #endif
        }
        else
        {
            static uint16_t count = 0;
            count++;
            //we don't want to send up too many messages...
            if (count == 100)
            {
                //message "spike encoder error"
                eOerrmanDescriptor_t descriptor = {0};
                descriptor.par16 = check;   // unless required
                descriptor.par64 = 0;       // unless required
                descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
                descriptor.sourceaddress = o->ID; // oppure l'id del can che ha dato errore
                descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_aea_abs_enc_spikes);
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &descriptor);
                
                count = 0;
            }  
            #ifndef USE_2FOC_FAST_ENCODER
            o->velocity = (7*o->velocity) >> 3;
            #endif
        }
    }
    
    return o->sign*o->distance;
}

#ifndef USE_2FOC_FAST_ENCODER
extern int32_t eo_absCalibratedEncoder_GetVel(EOabsCalibratedEncoder* o)
{
    return o->velocity;
}
#endif







#ifdef USE_2FOC_FAST_ENCODER

extern EOaxleVirtualEncoder* eo_axleVirtualEncoder_New(void)
{
    EOaxleVirtualEncoder *o = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOaxleVirtualEncoder), 1);

    if (o)
    {   
        o->axle_virt_pos = 0;
        o->axle_abs_pos = 0;
        o->axle_inc_pos = 0;
    }

    return o;
}

extern void eo_axleVirtualEncoder_Acquire(int32_t gearbox_reduction, EOaxleVirtualEncoder* o, int32_t axle_abs_pos, int32_t axle_virt_pos, int32_t axle_virt_vel)
{    
    int32_t N_BITS_PRECISION_BOUND = gearbox_reduction * ENCODER_QUANTIZATION;
    int32_t inc = axle_abs_pos - o->axle_abs_pos;
    
    o->axle_abs_pos = axle_abs_pos;
    
    o->velocity = + axle_virt_vel;    
    o->axle_inc_pos += axle_virt_pos - o->axle_virt_pos;
  
    o->axle_inc_pos -= inc*gearbox_reduction;
                        
    LIMIT(o->axle_inc_pos, N_BITS_PRECISION_BOUND);
    
    #ifdef USE_4BIT_INC_ENC_PRECISION
    #ifdef USE_ONLY_QE
    o->position = axle_virt_pos/gearbox_reduction;
    #else
    o->position = axle_abs_pos + o->axle_inc_pos/gearbox_reduction;
    #endif
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

#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void encoder_init(EOabsCalibratedEncoder* o, int32_t position, uint8_t error_mask)
{
    if (!o) return;
    
    if (error_mask)
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
    
	// check if it's working now...
    // for incremental encoders this function has only to set a flag in a bit mask
    // how can I detect that the encoder is incremental?
    
    //old method using function encoder type dependent
    /*
    if (joint2encodertype(o->ID) == 2)
    {
        RST_BITS(o->state_mask, SM_NOT_INITIALIZED);
    }
    */
    // nb: for inc encoders, this part is never executed 
    if (++o->first_valid_data >= 3)
    {
        //o->time = 0;
        
        o->position_last = position;
        o->position_sure = position;

        o->distance = position;
        
        #ifndef USE_2FOC_FAST_ENCODER
        o->velocity = 0;
        #endif
        
        o->delta = 0;

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

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

