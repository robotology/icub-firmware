#ifndef MC_ABS_ENCODER_H___
#define MC_ABS_ENCODER_H___

#include "EoCommon.h"

#include "EOEmsControllerCfg.h"

#include "hal_spiencoder.h"

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
    int32_t distance;
    int16_t position_last;
    int16_t position_sure;
    
    int32_t velocity;

    int32_t sign;
    int16_t offset;

    int16_t delta;

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
            unsigned unused          :5;
        } bits;
        
        uint8_t not_ready;
    } state;
    
    EncoderFaultState fault_state;
    EncoderFaultState fault_state_prec;
    uint16_t diagnostics_refresh;
    
} AbsEncoder;

extern AbsEncoder* AbsEncoder_new(uint8_t n);
extern void AbsEncoder_init(AbsEncoder* o);
extern void AbsEncoder_destroy(AbsEncoder* o);
extern void AbsEncoder_config(AbsEncoder *o, uint8_t ID, int32_t resolution, int16_t spike_mag_limit, uint16_t spike_cnt_limit);
extern void AbsEncoder_calibrate(AbsEncoder* o, int32_t offset);

extern int32_t AbsEncoder_update(AbsEncoder* o, int16_t position);
extern void AbsEncoder_invalid(AbsEncoder* o, hal_spiencoder_errors_flags error_flags);
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
extern void AbsEncoder_config_fake(AbsEncoder* o, uint8_t ID);

#endif

// AbsEncoder
/////////////////////////////////////////////////////////
