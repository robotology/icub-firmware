#include "EoCommon.h"

#include "EoError.h"
#include "EOtheErrorManager.h"

#include "EOMtheEMSrunner.h"

#include "EOemsControllerCfg.h"

#include "AbsEncoder.h"

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
    ////////////////////// set by config
    
    o->ID = 0;
    
    o->spike_cnt_limit = 32767;
    o->spike_mag_limit = 32767;
    
    o->sign = 0;
    
    o->fake = FALSE;
    
    ////////////////////// set by calibrate
    
    o->offset = 0;
    
    //////////////////////
    
    o->distance = 0;
    o->position_last = 0;
    o->position_sure = 0;
    
    o->velocity = 0;

    o->delta = 0;

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
}

void AbsEncoder_destroy(AbsEncoder* o)
{
    DELETE(o);
}

void AbsEncoder_config(AbsEncoder* o, uint8_t ID, int32_t resolution, int16_t spike_mag_limit, uint16_t spike_cnt_limit)
{
    o->ID = ID;
    
    o->fake = FALSE;
    
    o->spike_mag_limit = spike_mag_limit;//7*(65536L/resolution);
    o->spike_cnt_limit = spike_cnt_limit;//7*(65536L/resolution);
    
    o->sign = resolution >= 0 ? 1 : -1;
    
    o->state.bits.not_configured = FALSE;
}

void AbsEncoder_config_fake(AbsEncoder* o, uint8_t ID)
{
    o->ID = ID;
    
    o->fake = TRUE;
    
    o->sign = 1;
    
    o->state.bits.not_configured = FALSE;
    o->state.bits.not_initialized = FALSE;    
}

void AbsEncoder_calibrate(AbsEncoder* o, int32_t offset)
{
    o->offset = offset;
    
    o->state.bits.not_calibrated = FALSE;
}

int32_t AbsEncoder_position(AbsEncoder* o)
{
    return o->sign*o->distance;
}

int32_t AbsEncoder_velocity(AbsEncoder* o)
{
    return o->sign*o->velocity;
}

void AbsEncoder_posvel(AbsEncoder* o, int32_t* position, int32_t* velocity)
{
    *position = o->sign*o->distance;
    *velocity = o->sign*o->velocity;
}

static void AbsEncoder_position_init(AbsEncoder* o, int16_t position)
{
    if (!o) return;
    
    if (o->fake) return;
    
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

        o->distance = position;
        
        o->velocity = 0;
        
        o->delta = 0;

        o->valid_first_data_cnt = 0;
        
        o->state.bits.not_initialized = FALSE;
    }
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

void AbsEncoder_invalid(AbsEncoder* o, hal_spiencoder_errors_flags error_flags)
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
    
    if (error_flags.data_error)    o->fault_state.bits.data_error = TRUE;
    if (error_flags.tx_error)      o->fault_state.bits.tx_error   = TRUE;
    if (error_flags.chip_error)    o->fault_state.bits.chip_error = TRUE;
    if (error_flags.data_notready) o->fault_state.bits.chip_error = TRUE;
        
    o->valid_first_data_cnt = 0;
}

int32_t AbsEncoder_update(AbsEncoder* o, int16_t position)
{
    if (!o) return 0;
        
    if (o->fake) return 0;
    
    if (o->state.bits.not_configured) return 0;
    
    if (o->state.bits.not_calibrated) return 0;
    
    position -= o->offset;
        
    o->invalid_cnt = 0;
    o->timeout_cnt = 0;
    
    if (o->state.bits.not_initialized)
    {
        AbsEncoder_position_init(o, position);
        
        o->velocity = 0;
        
        return o->sign*o->distance;
    }
    
    int16_t check = position - o->position_last;
        
    o->position_last = position;

    if (-o->spike_mag_limit <= check && check <= o->spike_mag_limit)
    {
        int16_t delta = position - o->position_sure;
            
        if (delta)
        {
            o->position_sure = position;
                
            o->delta = delta;
                
            o->distance += delta;
                
            o->velocity = (7*o->velocity + ((int32_t)CTRL_LOOP_FREQUENCY)*delta) >> 3;
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
    
    return o->sign*o->distance;
}

void AbsEncoder_overwrite(AbsEncoder* o, int32_t position, int32_t velocity)
{
    if (!o->fake) return;
    
    o->distance = position + o->offset;
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
    if (!o) return TRUE;
    
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
