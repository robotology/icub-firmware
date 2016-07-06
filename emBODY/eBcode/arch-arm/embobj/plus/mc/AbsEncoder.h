#ifndef MC_ABS_ENCODER_H___
#define MC_ABS_ENCODER_H___

#include "EoCommon.h"

#include "EOtheEncoderReader.h"

#include "EOEmsControllerCfg.h"

/////////////////////////////////////////////////////////
// AbsEncoder

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

typedef struct //AbsEncoder
{
    //int32_t position_last;
    //int32_t position_sure;
    //int32_t offset;
    //int32_t delta;
    
    eOmc_EncoderType_t type;

    uint32_t partial_timer;
    int32_t  partial_space;
    
    uint16_t position_last;
    uint16_t position_sure;
    int16_t offset;
    int16_t delta;
    
    int32_t hard_stop_zero;
    
    int32_t zero;
    //int32_t sign;
    int32_t mul,div;
    
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
    
} AbsEncoder;

extern AbsEncoder* AbsEncoder_new(uint8_t n);
extern void AbsEncoder_init(AbsEncoder* o);
extern void AbsEncoder_destroy(AbsEncoder* o);

extern void AbsEncoder_config(AbsEncoder* o, uint8_t ID/*, eOmc_EncoderType_t type*/, int32_t resolution, int16_t spike_mag_limit, uint16_t spike_cnt_limit);
extern void AbsEncoder_calibrate_absolute(AbsEncoder* o, int32_t offset, int32_t zero);
extern void AbsEncoder_calibrate_fake(AbsEncoder* o);

//extern void AbsEncoder_update(AbsEncoder* o, int32_t position);
extern void AbsEncoder_update(AbsEncoder* o, uint16_t position);
extern void AbsEncoder_invalid(AbsEncoder* o, eOencoderreader_errortype_t error_type);

extern void AbsEncoder_config_resolution(AbsEncoder* o, float resolution);
extern void AbsEncoder_config_divisor(AbsEncoder* o, int32_t divisor);

extern void AbsEncoder_timeout(AbsEncoder* o);

extern int32_t AbsEncoder_position(AbsEncoder* o);
extern int32_t AbsEncoder_velocity(AbsEncoder* o);
extern void AbsEncoder_posvel(AbsEncoder* o, int32_t* position, int32_t* velocity);

extern BOOL AbsEncoder_is_ok(AbsEncoder* o);
extern BOOL AbsEncoder_is_calibrated(AbsEncoder* o);
extern BOOL AbsEncoder_is_in_fault(AbsEncoder* o);
extern void AbsEncoder_clear_faults(AbsEncoder* o);

extern void AbsEncoder_overwrite(AbsEncoder* o, int32_t position, int32_t velocity);
extern BOOL AbsEncoder_is_fake(AbsEncoder* o);

extern BOOL AbsEncoder_is_still(AbsEncoder* o, int32_t space_window, int32_t time_window);
extern void AbsEncoder_still_check_reset(AbsEncoder* o);

extern void AbsEncoder_start_hard_stop_calibrate(AbsEncoder* o, int32_t hard_stop_zero);
extern BOOL AbsEncoder_is_hard_stop_calibrating(AbsEncoder* o);
extern void AbsEncoder_calibrate_in_hard_stop(AbsEncoder* o);
#endif

// AbsEncoder
/////////////////////////////////////////////////////////
