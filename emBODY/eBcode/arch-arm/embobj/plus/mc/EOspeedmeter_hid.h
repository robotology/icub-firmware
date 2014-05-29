
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOSPEEDMETER_HID_H_
#define _EOSPEEDMETER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*  @file       EOspeedmeter_hid.h
    @brief      This header file implements hidden interface to speed meter from slow encoder.
    @author     alessandro.scalzo@iit.it
    @date       19/05/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOspeedmeter.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOspeedmeter_hid
    @brief      Hidden definition. Implements private data used only internally by the
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/

#define SM_OK                  0x0000

#define SM_NOT_CALIBRATED      0x0001
#define SM_NOT_INITIALIZED     0x0002
#define SM_ENC_SIGN_NOT_SET    0x0004
#define SM_NOT_READY           (SM_NOT_CALIBRATED|SM_NOT_INITIALIZED|SM_ENC_SIGN_NOT_SET)

#define SM_HARDWARE_FAULT      0x8000

#ifdef OLD_STYLE_ENCODER

struct EOspeedmeter_hid
{
    //int32_t time;

    int32_t distance;
    int32_t position_last;
    int32_t position_sure;

    int32_t enc_sign;
    int32_t offset;

    int32_t distance_x_reduction;
    int32_t fast_enc_pos;
    
    int32_t speed;
    int32_t delta;

    uint16_t state_mask;
    uint8_t  first_valid_data;
    uint16_t invalid_data_cnt;
};

#else

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
    uint16_t invalid_data_cnt;
};

struct EOaxleVirtualEncoder_hid
{
    eObool_t inverted;
    
    int32_t velocity;
    int32_t position;
    
    int32_t axle_inc_pos;
    int32_t axle_abs_pos;
    int32_t axle_virt_pos;
};

#endif

// - declaration of extern hidden functions ---------------------------------------------------------------------------


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
