/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOSENSOR_BNO055_H_
#define _EOSENSOR_BNO055_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOsensorBNO055.h
    @brief      This file provides interface to specific sensor BNO055
    @author     valentina.gaggero@iit.it
    @date       06/08/2016
**/

/** @defgroup eo_EOsensorBNO055 Object EOsensorBNO055
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

//#include "EoCommon.h" to be added later
#include "stdint.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
enum{eo_sens_bno055_command_maxNumOfBytes = 20};
typedef enum
{
    eo_sens_bno055_cmdOp_write = 0x0,
    eo_sens_bno055_cmdOp_read  = 0x1
}eo_sens_bno055_commandOperation_t;

typedef enum
{
    eo_sens_bno055_register_CHIP_ID        = 0x00,
    eo_sens_bno055_register_SW_VERSION     = 0x04,    // 2 software revision bytes
    eo_sens_bno055_register_BOOTLOADER     = 0x06,    // 1 byte  bootloader version
    eo_sens_bno055_register_PAGE_ID        = 0x07,    // page ID number

    eo_sens_bno055_register_ACC_DATA       = 0x08,    // 3*2 bytes: LSB first (LSB 0x08, MSB 0x09) for X
    eo_sens_bno055_register_MAGN_DATA      = 0x0E,    // 3*2 bytes: LSB first
    eo_sens_bno055_register_GYRO_DATA      = 0x14,    // 3*2 bytes: LSB first
    eo_sens_bno055_register_RPY_DATA       = 0x1A,    // 3*2 bytes: LSB first  (raw order is Yaw, Roll, Pitch)
    eo_sens_bno055_register_QUATERN_DATA   = 0x20,    // 4*2 bytes: LSB first  (raw order is w, x, y, z)
    eo_sens_bno055_register_GRAVITY        = 0x2E,    // Gravity Vector data
    eo_sens_bno055_register_CALIB_STATUS   = 0x35,    // Check if sensors are calibrated, 2 bits each. SYS - GYRO - ACC - MAG. 3 means calibrated, 0 not calbrated
    eo_sens_bno055_register_SYS_CLK_STATUS = 0x38,    // only 1 last LSB
    eo_sens_bno055_register_SYS_STATUS     = 0x39,
    eo_sens_bno055_register_SYS_ERR        = 0x3A,
    eo_sens_bno055_register_UNIT_SEL       = 0x3B,
    eo_sens_bno055_register_OP_MODE        = 0x3D,
    eo_sens_bno055_register_POWER_MODE     = 0x3E,
    eo_sens_bno055_register_SYS_TRIGGER    = 0x3F

}eo_sens_bno055_registerAddress_t;
typedef struct
{
    eo_sens_bno055_commandOperation_t operation;
    eo_sens_bno055_registerAddress_t regAddr;
    uint8_t numofbytes;
} eo_sens_bno055_command_t;

typedef enum
{
    eo_sens_bno055_err_OK          = 0x0,
    eo_sens_bno055_err_writeFail   = 0x3 //etc
} eo_sens_bno055_error_t;
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern void eo_sens_bno055_sendCommand(eo_sens_bno055_command_t *cmd, uint8_t *data);
extern void eo_sens_bno055_readResponse_raw(uint8_t *data, uint8_t *len);
extern void eo_sens_bno055_initialize(void);
extern uint8_t eo_sens_bno055_check_fbk_is_ok(uint8_t *data, uint8_t len);
extern void eo_sens_bno055_sendWriteCommand(eo_sens_bno055_registerAddress_t regAddr, uint8_t *data, uint8_t len);
extern void eo_sens_bno055_sendReadCommand(eo_sens_bno055_registerAddress_t regAddr, uint8_t numOfByteToRead);

extern void s_eo_sens_bno055_init_usart(void);//to make static

/** @}            
    end of group eo_EOsensorBNO055
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


