/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or  FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License foFITNESSr more details
*/

#ifndef MC_ABS_ENCODER_H___
#define MC_ABS_ENCODER_H___

#ifdef __cplusplus
extern "C" {
#endif

#include "EoCommon.h"

//#include "EOtheEncoderReader.h"
#include "EoMotionControl.h"

#include "EOemsControllerCfg.h"

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
    
} AbsEncoder;

extern AbsEncoder* AbsEncoder_new(uint8_t n);
extern void AbsEncoder_init(AbsEncoder* o);
extern void AbsEncoder_destroy(AbsEncoder* o);

extern void AbsEncoder_config(AbsEncoder* o, uint8_t ID, int32_t resolution, float32_t tolerance);
extern void AbsEncoder_calibrate_absolute(AbsEncoder* o, int32_t offset, int32_t zero);
extern void AbsEncoder_calibrate_fake(AbsEncoder* o);

//extern void AbsEncoder_update(AbsEncoder* o, int32_t position);

/** @brief Updates the velocity based on current joint's position.
 *
 *  If the encoder has not been initialized yet or it is a fake encoder, 
 *  or the state-bits are not configured, the function has no effect.
 *  Despite an iCubDegree value is stored in a uint32_t, an int16_t variable
 *  is used to avoid that a fictional spike will be triggered due to the wraparound.
 *  It is a safe workaround as long as we accept an offset of few degrees when 
 *  a real spike rises within the range [-spike_mag_limit, spike_mag_limit] (iCubDegree).
 *  If a spike will be detected it will rise a warning that AEA encoder has spikes.
 *  
 *
 *  @param o The absolute encoder.
 *  @param position the current position (iCubDegree) just read from the encoder.
 *  @return Void.
 */
extern void AbsEncoder_update(AbsEncoder* o, uint16_t position);

typedef enum
{
    ae_err_NONE                  = 0,
    ae_err_AEA_READING           = 1,
    ae_err_AEA_PARITY            = 2,
    ae_err_AEA_CHIP              = 3,
//    encreader_err_QENC_GENERIC          = 4,
//    encreader_err_ABSANALOG_GENERIC     = 5,
//    encreader_err_MAIS_GENERIC          = 6,
//    encreader_err_SPICHAINOF2_GENERIC   = 7,
//    encreader_err_SPICHAINOF3_GENERIC   = 8,
//    encreader_err_AMO_GENERIC           = 9,
//    encreader_err_PSC_GENERIC           = 10,  
//    encreader_err_POS_GENERIC           = 11,    
    ae_err_GENERIC               = 14,    
    ae_err_NOTCONNECTED          = 15 /* this error happens when the encoder type is none or encoder is not local, for example it is connected to 2foc board */
} ae_errortype_t;

extern void AbsEncoder_invalid(AbsEncoder* o, ae_errortype_t error_type);

extern void AbsEncoder_config_resolution(AbsEncoder* o, float resolution);
extern void AbsEncoder_config_divisor(AbsEncoder* o, float32_t divisor);

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

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard



// AbsEncoder
/////////////////////////////////////////////////////////
