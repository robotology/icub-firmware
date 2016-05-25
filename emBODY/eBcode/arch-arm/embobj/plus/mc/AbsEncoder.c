#include "EoCommon.h"

#include "EoError.h"
#include "EOtheErrorManager.h"

#include "EOMtheEMSrunner.h"

#include "EOemsControllerCfg.h"

#include "AbsEncoder.h"

#include "EOtheMais.h"

/////////////////////////////////////////////////////////
// AbsEncoder

AbsEncoder* AbsEncoder_new(uint8_t n)
{
    AbsEncoder *o = NEW(AbsEncoder, n);
    
    for (uint8_t i=0; i<n; ++i)
    {
        AbsEncoder_init(&(o[i]));
    }
    
    return o;
}

void AbsEncoder_init(AbsEncoder* o)
{
    o->ID = 0;
    
    o->type = eomc_encoder_NONE;
    
    o->spike_cnt_limit = 32767;
    o->spike_mag_limit = 32767;
    
    //o->sign = 0;
    o->mul = 0;
    o->div = 1;
    
    o->fake = FALSE;
    
    o->offset = 0;
    
    o->distance = 0;
    o->position_last = 0;
    o->position_sure = 0;
    
    o->velocity = 0;

    o->delta = 0;

    o->partial_timer = 0;
    o->partial_space = 0;
    
    o->hardware_fault = FALSE;
    o->invalid_cnt = 0;
    o->timeout_cnt = 0;
    
    o->spike_cnt = 0;
    
    o->fault_state_prec.bitmask = 0;
    o->fault_state.bitmask = 0;
    o->diagnostics_refresh = 0;
    
    o->valid_first_data_cnt = 0;
    
    o->state.bits.not_configured  = TRUE;
    o->state.bits.not_calibrated  = TRUE;
    o->state.bits.not_initialized = TRUE;
    o->state.bits.hard_stop_calib = FALSE;
}

void AbsEncoder_destroy(AbsEncoder* o)
{
    DELETE(o);
}

void AbsEncoder_config(AbsEncoder* o, uint8_t ID/*, eOmc_EncoderType_t type*/, int32_t resolution, int16_t spike_mag_limit, uint16_t spike_cnt_limit)
{
    o->ID = ID;
    
    //o->fake = FALSE;

//    if(o->type != type)
//    {
//        eOerrmanDescriptor_t descriptor = {0};
//        char str[50];
//        snprintf(str, 50, "missmach encoder type: xmlFile=%d. fw=%d", type, o->type);
//        descriptor.par16 = o->ID;
//        descriptor.par64 = 0;
//        descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
//        descriptor.sourceaddress = 0;
//        descriptor.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
//        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, str, NULL, &descriptor);
//        return;
//    }
    
    AbsEncoder_config_resolution(o, resolution);
    
    if (!o->fake)
    {
        o->spike_mag_limit = spike_mag_limit;
        o->spike_cnt_limit = spike_cnt_limit;
    }
    else
    {
        o->state.bits.not_initialized = FALSE; 
    }
    
    o->state.bits.not_configured = FALSE;
}

void AbsEncoder_config_resolution(AbsEncoder* o, int32_t resolution)
{
    if (!o->fake)
    {
        //o->sign = resolution >= 0 ? 1 : -1;
        o->mul = resolution >= 0 ? 1 : -1;
    }
    else
    {
        //o->sign = 1;
        o->mul = o->div = 1;
    }
}

extern void AbsEncoder_config_divisor(AbsEncoder* o, int32_t divisor)
{
    o->div = divisor;
}

void AbsEncoder_start_hard_stop_calibrate(AbsEncoder* o, int32_t hard_stop_zero)
{
    o->offset = 0;
    o->zero = 0;
    
    o->hard_stop_zero = hard_stop_zero;
    
    o->distance = o->position_sure;
    
    o->state.bits.not_calibrated  = TRUE;
    o->state.bits.hard_stop_calib = TRUE;
}

void AbsEncoder_calibrate_in_hard_stop(AbsEncoder* o)
{
    o->offset = 0;

    o->zero = (o->mul*o->distance)/o->div - o->hard_stop_zero;  
    
    o->state.bits.not_calibrated  = FALSE;
    o->state.bits.hard_stop_calib = FALSE;
}

BOOL AbsEncoder_is_hard_stop_calibrating(AbsEncoder* o)
{
    return o->state.bits.hard_stop_calib;
}

void AbsEncoder_calibrate_absolute(AbsEncoder* o, int32_t offset, int32_t zero)
{
    o->offset = offset;
    o->zero = zero;
    
    uint16_t position = o->position_sure;
    position -= o->offset;
    o->distance = position;
    
    o->state.bits.not_calibrated  = FALSE;
    o->state.bits.hard_stop_calib = FALSE;
}

void AbsEncoder_calibrate_fake(AbsEncoder* o)
{
    AbsEncoder_calibrate_absolute(o, 0, 0);
}

int32_t AbsEncoder_position(AbsEncoder* o)
{
    return (o->mul*o->distance)/o->div - o->zero;
}

int32_t AbsEncoder_velocity(AbsEncoder* o)
{
    return (o->mul*o->velocity)/o->div;
}

void AbsEncoder_posvel(AbsEncoder* o, int32_t* position, int32_t* velocity)
{
    *position = (o->mul*o->distance)/o->div - o->zero;
    *velocity = (o->mul*o->velocity)/o->div;
}

//static void AbsEncoder_position_init(AbsEncoder* o, int32_t position)
static void AbsEncoder_position_init_aea(AbsEncoder* o, uint16_t position)
{
    
    if (!o->valid_first_data_cnt)
    {
        o->position_last = position;
    }
    
    if (o->position_last != position)
    {
        o->valid_first_data_cnt = 0;
        
        return;
    }
    
    if (++o->valid_first_data_cnt >= 3)
    {
        o->position_last = position;
        o->position_sure = position;

        position -= o->offset;
        
        o->distance = position;
        
        o->velocity = 0;
        
        o->delta = 0;

        o->valid_first_data_cnt = 0;
        
        o->state.bits.not_initialized = FALSE;
    }
}


static void AbsEncoder_position_init_others(AbsEncoder* o, uint16_t position)
{
    //mais doesn't need an init procedure like aea encoder
    o->position_last = position;
    o->position_sure = position;

    position -= o->offset;
    
    o->distance = position;
    
    o->velocity = 0;
    
    o->delta = 0;

    o->valid_first_data_cnt = 0;
    
    o->state.bits.not_initialized = FALSE;

}

static void AbsEncoder_position_init(AbsEncoder* o, uint16_t position)
{
    if (!o) return;
    
    if (o->fake) return;
    
    switch(o->type)
    {
        case eomc_encoder_AEA:
            AbsEncoder_position_init_aea(o, position);
            break;
        
        case eomc_encoder_MAIS:
        case eomc_encoder_HALL_ADC:
            AbsEncoder_position_init_others(o, position);
            break;
            
        default:
            return;
    };
}

void AbsEncoder_timeout(AbsEncoder* o)
{
    if (!o) return;
    
    if (o->fake) return;
    
    if (o->state.bits.not_configured) return;
    
    if (o->state.bits.not_calibrated) return;
    
    if (o->timeout_cnt > ENCODER_TIMEOUT_LIMIT)
    {
        o->hardware_fault = TRUE;
    }
    else
    {
        ++o->timeout_cnt;
    }
    
    o->fault_state.bits.data_notready = TRUE;
    
    o->valid_first_data_cnt = 0;
}

void AbsEncoder_invalid(AbsEncoder* o, eOencoderreader_errortype_t error_type)
{
    if (!o) return;
    
    if (o->fake) return;
    
    if (o->state.bits.not_configured) return;
    
    if (o->state.bits.not_calibrated) return;

    if (o->invalid_cnt > ENCODER_INVALID_LIMIT)
    {
        o->hardware_fault = TRUE;
    }
    else
    {
        ++o->invalid_cnt;
    }
    
    switch (error_type)
    {
        case encreader_err_READING:
            o->fault_state.bits.data_error = TRUE;
            break;
        case encreader_err_PARITY:
            o->fault_state.bits.tx_error = TRUE;
            break;
        case encreader_err_CHIP:
            o->fault_state.bits.chip_error = TRUE;
            break;
        default:
            break;
    }
        
    o->valid_first_data_cnt = 0;
}

BOOL AbsEncoder_is_still(AbsEncoder* o, int32_t space_window, int32_t time_window)
{
    o->partial_space += (int32_t)o->delta;
    
    BOOL still = FALSE;
    
    if (++o->partial_timer > time_window)
    {        
        still = abs(o->partial_space) < space_window;
        
        o->partial_timer = 0;
        o->partial_space = 0;
    }
    
    return still;
}

void AbsEncoder_still_check_reset(AbsEncoder* o)
{
    o->partial_timer = 0;
    o->partial_space = 0;
}

void AbsEncoder_update(AbsEncoder* o, uint16_t position)
{
    if (!o) return;
        
    if (o->fake) return;
    
    if (o->state.bits.not_configured) return;
    
    //if (o->state.bits.not_calibrated) return;
    
    o->invalid_cnt = 0;
    o->timeout_cnt = 0;
    
    if (o->state.bits.not_initialized)
    {
        AbsEncoder_position_init(o, position);
        
        o->velocity = 0;
        
        return;
    }
    
    o->delta = 0;
    
    int16_t check = position - o->position_last;
    
    o->position_last = position;

    if( ((o->spike_mag_limit != 0 ) && (-o->spike_mag_limit <= check && check <= o->spike_mag_limit)) || //if spike_mag_limit has been configured and there is not a spike
        (o->spike_mag_limit == 0 )) // or spike_mag_limit has not been configured
    {
        int16_t delta = position - o->position_sure;

        if (delta)
        {
            o->position_sure = position;
                
            o->delta = delta;
                
            position -= o->offset;
            
            o->distance += (int32_t)delta;
            
            //o->distance = position;
                
            o->velocity = (7*o->velocity + ((int32_t)CTRL_LOOP_FREQUENCY)*o->delta) >> 3;
        }
        else
        {
            o->velocity = (7*o->velocity) >> 3;
        }
    }
    else
    {
        o->spike_cnt++;
       
        o->velocity = (7*o->velocity) >> 3;
    }
        
    //every second
    
    eOemsrunner_diagnosticsinfo_t* runner_info = eom_emsrunner_GetDiagnosticsInfoHandle(eom_emsrunner_GetHandle());
    
    if ((runner_info->numberofperiods % 1000) == 0)
    {
        if (o->spike_cnt > 0)
        {                
            //message "spike encoder error"
            eOerrmanDescriptor_t descriptor = {0};
            descriptor.par16 = o->ID;           
            descriptor.par64 = o->spike_cnt;
            descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
            descriptor.sourceaddress = 0;
            descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_aea_abs_enc_spikes);
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &descriptor);
                
            if (o->spike_cnt > o->spike_cnt_limit)
            {
                o->fault_state.bits.spikes = TRUE;
                o->hardware_fault = TRUE;
            }
            
            o->spike_cnt = 0;
        }
    }
}

/*
void AbsEncoder_update(AbsEncoder* o, int32_t position)
{
    if (!o) return;
        
    if (o->fake) return;
    
    if (o->state.bits.not_configured) return;
    
    if (o->state.bits.not_calibrated) return;
    
    position -= o->offset;
    
    if (position<0)
    {
        position += 65536L;
    }
    else if (position>=65536L)
    {
        position -= 65536L;
    }
    
    o->invalid_cnt = 0;
    o->timeout_cnt = 0;
    
    if (o->state.bits.not_initialized)
    {
        AbsEncoder_position_init(o, position);
        
        o->velocity = 0;
        
        return;
    }
    
    int32_t check = position - o->position_last;
    
    while (check<-32768) check+=65536;    
    while (check> 32768) check-=65536;
    
    o->position_last = position;

    if (-o->spike_mag_limit <= check && check <= o->spike_mag_limit)
    {
        int32_t delta = position - o->position_sure;

        while (delta<-32768) delta+=65536;        
        while (delta> 32768) delta-=65536;
        
        if (delta)
        {
            o->position_sure = position;
                
            o->delta = delta;
                
            o->distance += o->delta;
                
            o->velocity = (7*o->velocity + ((int32_t)CTRL_LOOP_FREQUENCY)*o->delta) >> 3;
        }
        else
        {
            o->velocity = (7*o->velocity) >> 3;
        }
    }
    else
    {
        o->spike_cnt++;
       
        o->velocity = (7*o->velocity) >> 3;
    }
        
    //every second
    
    eOemsrunner_diagnosticsinfo_t* runner_info = eom_emsrunner_GetDiagnosticsInfoHandle(eom_emsrunner_GetHandle());
    
    if ((runner_info->numberofperiods % 1000) == 0)
    {
        if (o->spike_cnt > 0)
        {                
            //message "spike encoder error"
            eOerrmanDescriptor_t descriptor = {0};
            descriptor.par16 = o->ID;           
            descriptor.par64 = o->spike_cnt;
            descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
            descriptor.sourceaddress = 0;
            descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_aea_abs_enc_spikes);
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &descriptor);
                
            if (o->spike_cnt > o->spike_cnt_limit)
            {
                o->fault_state.bits.spikes = TRUE;
                o->hardware_fault = TRUE;
            }
            
            o->spike_cnt = 0;
        }
    }
}
*/

void AbsEncoder_overwrite(AbsEncoder* o, int32_t position, int32_t velocity)
{
    if (!o->fake) return;
    
    o->distance = position - o->offset;
    o->velocity = velocity;
}

BOOL AbsEncoder_is_fake(AbsEncoder* o)
{
    return o->fake;
}

BOOL AbsEncoder_is_ok(AbsEncoder* o)
{
    return (!o->state.not_ready) && (!o->hardware_fault);
}

BOOL AbsEncoder_is_calibrated(AbsEncoder* o)
{
    return !o->state.bits.not_calibrated;
}

static void AbsEncoder_send_error(uint8_t id, eOerror_value_MC_t err_id, uint64_t mask)
{
    static eOerrmanDescriptor_t descriptor = {0};
    descriptor.par16 = id;
    descriptor.par64 = mask;
    descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
    descriptor.sourceaddress = 0;
    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, err_id);
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
}

BOOL AbsEncoder_is_in_fault(AbsEncoder* o)
{
    if(eomc_encoder_MAIS == o->type)
    {
        if(!eo_mais_isAlive(eo_mais_GetHandle()))
        {
            o->hardware_fault = TRUE;
            eOerrmanDescriptor_t descriptor = {0};
            descriptor.par16 = o->ID;
            descriptor.par64 = 0;
            descriptor.sourcedevice = eo_errman_sourcedevice_localboard;
            descriptor.sourceaddress = 0;
            descriptor.code = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag02);
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, "mais timeout", NULL, &descriptor);
        }
    }
    if (!o->hardware_fault) return FALSE;
    
    if (++o->diagnostics_refresh > 5*CTRL_LOOP_FREQUENCY_INT)
    {
        o->diagnostics_refresh = 0;
        o->fault_state_prec.bitmask = 0;
    }
    
    if (o->fault_state_prec.bitmask != o->fault_state.bitmask)
    {     
        if ((o->fault_state.bits.tx_error   && !o->fault_state_prec.bits.tx_error)  
         || (o->fault_state.bits.data_error && !o->fault_state_prec.bits.data_error) 
         || (o->fault_state.bits.chip_error && !o->fault_state_prec.bits.chip_error))
        {
            AbsEncoder_send_error(o->ID, eoerror_value_MC_aea_abs_enc_invalid, o->fault_state.bitmask);
        }
        
        if (o->fault_state.bits.data_notready)
        {
            AbsEncoder_send_error(o->ID, eoerror_value_MC_aea_abs_enc_timeout, 0);
        }
        
        if (o->fault_state.bits.spikes)
        {
            AbsEncoder_send_error(o->ID, eoerror_value_MC_aea_abs_enc_spikes, 0);
        }
        
        o->fault_state_prec.bitmask = o->fault_state.bitmask;
    }
    
    return TRUE;
}

void AbsEncoder_clear_faults(AbsEncoder* o)
{
    o->hardware_fault = FALSE;
    
    o->fault_state_prec.bitmask = 0;
    o->fault_state.bitmask = 0;
    
    o->invalid_cnt = 0;
    o->timeout_cnt = 0;
    
    o->spike_cnt = 0;
}

// AbsEncoder
/////////////////////////////////////////////////////////
