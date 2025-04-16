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


#include "EoCommon.h"
#include "EOemsControllerCfg.h"
#include "embot_app_eth_Encoder.h"

using ae_errortype_t = embot::app::eth::encoder::v1::Error;

// opaque struct

typedef struct AbsEncoder_hid AbsEncoder;

// API

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

extern BOOL AbsEncoder_is_initialized(AbsEncoder* o);

 
#endif  // include-guard



// AbsEncoder
/////////////////////////////////////////////////////////
