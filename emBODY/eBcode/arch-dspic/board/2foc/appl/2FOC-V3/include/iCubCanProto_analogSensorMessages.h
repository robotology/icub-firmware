/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

#ifndef _ICUBCANPROTO_ANALOGSENSORMESSAGES_H_
#define _ICUBCANPROTO_ANALOGSENSORMESSAGES_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       iCubCanProto_analogSensorMessages.h
    @brief      This header file gives the definitions of the iCub CAN protocols commands relating to analog sensor
    @author     valentina.gaggero@iit.it
    @date       12/03/2012
    @ingroup    iCubCanProtocol
**/

/** @ingroup    iCubCanProtocol
    @{
 **/




// - external dependencies --------------------------------------------------------------------------------------------
// empty-section


// - public #define  --------------------------------------------------------------------------------------------------

/***************************************************************************/
// command messages of class ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR 
/***************************************************************************/

#define ICUBCANPROTO_POL_AS_CMD__NONE              0x0
#define ICUBCANPROTO_POL_AS_CMD__SET_IIR           0x1  // select IIR filters parameters 
#define ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_RC     0x3  // Set SG to TF trasformation matrix 
#define ICUBCANPROTO_POL_AS_CMD__SET_CH_DAC        0x4  // set DAC for channel x
#define ICUBCANPROTO_POL_AS_CMD__SELECT_ACTIVE_CH  0x5  // set active channels (activation mask) only active channels are transmitted
#define ICUBCANPROTO_POL_AS_CMD__CALIBRATE_OFFSET  0x6  // set the calibration offset
#define ICUBCANPROTO_POL_AS_CMD__SET_TXMODE        0x7  // set continuous/on demand transmission mode
#define ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE   0x8  // set board CAN speed in milliseconds minimum, datarate 210ms  
#define ICUBCANPROTO_POL_AS_CMD__SAVE2EE           0x9  // save Config to EE
#define ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_RC     0xA  // Get TF trasformation matrix 
#define ICUBCANPROTO_POL_AS_CMD__GET_CH_DAC        0xB  // Get DAC for channel x
#define ICUBCANPROTO_POL_AS_CMD__GET_CH_ADC        0xC  // Get ADC for channel x
#define ICUBCANPROTO_POL_AS_CMD__FILTER_EN         0xD  // ENABLE/DISABLES FILTER
#define ICUBCANPROTO_POL_AS_CMD__MUX_EN            0xE  // ENABLE/DISABLES MUX
#define ICUBCANPROTO_POL_AS_CMD__MUX_NUM           0xF
#define ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION    0x10 //set data resolution
#define ICUBCANPROTO_POL_AS_CMD__SET_MATRIX_G      0x11 //set matrix gain
#define ICUBCANPROTO_POL_AS_CMD__GET_MATRIX_G      0x12 //get matrix gain
#define ICUBCANPROTO_POL_AS_CMD__SET_CALIB_TARE    0x13 //set/reset calibration tare
#define ICUBCANPROTO_POL_AS_CMD__GET_CALIB_TARE    0x14 //get calibration tare
#define ICUBCANPROTO_POL_AS_CMD__SET_CURR_TARE     0x15 //set/reset current tare
#define ICUBCANPROTO_POL_AS_CMD__GET_CURR_TARE     0x16 //get current tare
#define ICUBCANPROTO_POL_AS_CMD__SET_FULL_SCALES   0x17 //set/reset current tare
#define ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES   0x18 //get current tare
#define ICUBCANPROTO_POL_AS_CMD__SET_SERIAL_NO     0x19 //set/reset current tare
#define ICUBCANPROTO_POL_AS_CMD__GET_SERIAL_NO     0x1A //get current tare
#define ICUBCANPROTO_POL_AS_CMD__GET_EEPROM_STATUS 0x1B //get eeprom status (saved/not saved)
#define ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION    0X1C //get fw version and check can protocol compatibility
#define ICUBCANPROTO_POL_AS_CMD__SET_BOARD_ADX     0x32 // Set board CAN address


#define ICUBCANPROTO_POL_AS_CMD_MAXNUM                 (ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION + 1)


/***************************************************************************/
// command messages of class ICUBCANPROTO_CLASS_PERIODIC_ANALOGSENSOR 
/***************************************************************************/

#define ICUBCANPROTO_PER_AS_MSG__DIGITAL_GYROSCOPE                  0x00    // placeholder for possible inertial gyros message status in class 3
#define ICUBCANPROTO_PER_AS_MSG__DIGITAL_ACCELEROMETER              0x01    // placeholder for possible inertial accel message status in class 3

#define ICUBCANPROTO_PER_AS_MSG__FORCE_VECTOR                       0xA // Transmit Torque values t1 t1 t2 t2 t3 t3
#define ICUBCANPROTO_PER_AS_MSG__TORQUE_VECTOR                      0xB // Transmit Force  values f1 f1 f2 f2 f3 f3
#define ICUBCANPROTO_PER_AS_MSG__HES0TO6                            0xC //hall effect sensors from 0  to 6  *8bits
#define ICUBCANPROTO_PER_AS_MSG__HES7TO14                           0xD //hall effect sensors from 7  to 14  *8bits
#define ICUBCANPROTO_PER_AS_MSG__UNCALIBFORCE_VECTOR_DEBUGMODE      0x8 // used by strain to transmit Force uncalib data when is enabled DebugMode (ICUBCANPROTO_POL_AS_CMD__SET_TXMODE with payload[1]=4)
#define ICUBCANPROTO_PER_AS_MSG__UNCALIBTORQUE_VECTOR_DEBUGMODE     0x9 // used by strain to transmit Force uncalib data when is enabled DebugMode (ICUBCANPROTO_POL_AS_CMD__SET_TXMODE with payload[1]=4)

#define ICUBCANPROTO_PER_AS_MSG_MAXNUM                              6



//the following msg are deprecated!!!

// hall effect sensors from 0 to 3*16bits
#define ICUBCANPROTO_PER_AS_MSG__HES0TO3           0xC // 010 SOURCE DEST 0xC h0 h0 h1 h1 h2 h2 h3 h3
// hall effect sensors from 4 to 7*16bits
#define ICUBCANPROTO_PER_AS_MSG__HES4TO7           0xD // 010 SOURCE DEST 0xC h4 h4 h5 h5 h6 h6 h7 h7
// hall effect sensors from 8 to 11*16bits
#define ICUBCANPROTO_PER_AS_MSG__HES8TO11          0xE // 010 SOURCE DEST 0xC h8 h8 h9 h9 h10 h10 h11 h11
// hall effect sensors from 12 to 14*16bits
#define ICUBCANPROTO_PER_AS_MSG__HES12TO14         0xF // 010 SOURCE DEST 0xC h12 h12 h13 h13 h14 h14 h15 h15

// hall effect sensors from 0 to 7*8bits
#define ICUBCANPROTO_PER_AS_MSG__HES0TO6           0xC // 010 SOURCE DEST 0xC h0 h1 h2 h3 h4 h5 h6 
// hall effect sensors from 8 to 14*8bits
#define ICUBCANPROTO_PER_AS_MSG__HES7TO14          0xD // 010 SOURCE DEST 0xD h7 h8 h9 h10 h11 h12 h13 h14 

// hall effect sensors from 0 to 4*12bits
#define ICUBCANPROTO_PER_AS_MSG__HES0TO4           0xC // 010 SOURCE DEST 0xC h0 h1 h2 h3 h4 
// hall effect sensors from 5 to 9*12bits
#define ICUBCANPROTO_PER_AS_MSG__HES5TO9           0xD // 010 SOURCE DEST 0xD h5 h6 h7 h8 h9 
// hall effect sensors from 10 to 14*12bits
#define ICUBCANPROTO_PER_AS_MSG__HES10TO14          0xE // 010 SOURCE DEST 0xD h10 h11 h12 h13 h14 








// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


/** @} **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







