
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef MC_ABS_ENCODER_HID_H___
#define MC_ABS_ENCODER_HID_H___


#include "EoCommon.h"
#include "EoMotionControl.h"
#include "EOemsControllerCfg.h"


typedef union
{
    struct
    {
        uint8_t tx_error      :1;
        uint8_t data_error    :1;
        uint8_t data_notready :1;
        uint8_t chip_error    :1;
            
        uint8_t spikes        :1;
        uint8_t unused        :3;
    } bits;
        
    uint8_t bitmask;
} EncoderFaultState;

struct AbsEncoder_hid
{
    //int32_t position_last;
    //int32_t position_sure;
    //int32_t offset;
    //int32_t delta;
    
    eOmc_encoder_t type;

    uint32_t partial_timer;
    int32_t  partial_space;
    
    uint16_t position_last;
    uint16_t position_sure;
    int16_t offset;
    int16_t delta;
    
    int32_t hard_stop_zero;
    
    int32_t zero;
    //int32_t sign;
    int32_t mul;
    float32_t div;
    float32_t toleranceCfg;
    
    int32_t distance;
    int32_t velocity;
    
    uint16_t invalid_cnt;
    uint16_t timeout_cnt;
    
    int16_t  spike_mag_limit;
    uint16_t spike_cnt_limit;
    uint16_t spike_cnt;
    
    BOOL hardware_fault;
    
    int8_t ID;
    int8_t valid_first_data_cnt;
    
    BOOL fake;
    
    union
    {
        struct
        {
            unsigned not_configured  :1;
            unsigned not_calibrated  :1;
            unsigned not_initialized :1;
            unsigned hard_stop_calib :1;
            unsigned unused          :4;
        } bits;
        
        uint8_t not_ready;
    } state;
    
    EncoderFaultState fault_state;
    EncoderFaultState fault_state_prec;
    uint16_t diagnostics_refresh;
    uint16_t count_diagn_mais;
    
};
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

