/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
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

#warning -> acemor: da mettere in una sezione comune a tutte le applicazioni ed al bootloader
// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _CAN_PROTOCOL_H_
#define _CAN_PROTOCOL_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
// empty-section

// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - public #define  --------------------------------------------------------------------------------------------------

#define CAN_MSG_CLASS_PERIODIC 0x0300
/*
    For messages of class PERIODIC the meaning of data/ID is defined as follows:
    -------------------------- -----------------
    |           11b           |        8B      |
    -------------------------- -----------------
    | 3b     | 4b     | 4b    |      B[0-7]    |
    |class    | Source | ID msg |     Payload    |
    -------------------------- -----------------
*/
#define CAN_MSG_CLASS_POLLING 0x0200
/*
    For messages of class POLLING the meaning of data/ID is defined as follows:
    -------------------------- --------------------------------------
    |           11b            |               8B                   |
    -------------------------- --------------------------------------
    | 3b     | 4b     | 4b     | B[0]    |         B[1-7]           |
    |class   | Source | Dest   | commnad |        Payload           |
    -------------------------- --------------------------------------
*/
#define CAN_MSG_CLASS_LOADER   0x0700
/*
    For messages of class LOADER the meaning of data/ID is defined as follows:
    -------------------------- --------------------------------------
    |           11b            |               8B                   |
    -------------------------- --------------------------------------
    | 3b     | 4b     | 4b     | B[0]    |         B[1-7]           |
    |class   | Source | Dest   | commnad |        Payload           |
    -------------------------- --------------------------------------
*/


//********************************************
//**** Messages belong to periodic class *****
//********************************************
#define CAN_CMD_NONE              0x0
#define CAN_CMD_SET_BOARD_ADX     0x32 // Set board CAN address
#define CAN_CMD_SET_IIR           0x1  // select IIR filters parameters 
#define CAN_CMD_SET_MATRIX_RC     0x3  // Set SG to TF trasformation matrix 
#define CAN_CMD_SET_CH_DAC        0x4  // set DAC for channel x
#define CAN_CMD_SELECT_ACTIVE_CH  0x5  // set active channels (activation mask) only active channels are transmitted
#define CAN_CMD_CALIBRATE_OFFSET  0x6  // set the calibration offset
#define CAN_CMD_SET_TXMODE        0x7  // set continuous/on demand transmission mode
#define CAN_CMD_SET_CANDATARATE   0x8  // set board CAN speed in milliseconds minimum, datarate 210ms  
#define CAN_CMD_SAVE2EE           0x9  // save Config to EE
#define CAN_CMD_GET_MATRIX_RC     0xA  // Get TF trasformation matrix 
#define CAN_CMD_GET_CH_DAC        0xB  // Get DAC for channel x
#define CAN_CMD_GET_CH_ADC        0xC  // Get ADC for channel x
#define CAN_CMD_FILTER_EN         0xD  // ENABLE/DISABLES FILTER
#define CAN_CMD_MUX_EN               0xE  // ENABLE/DISABLES MUX
#define CAN_CMD_MUX_NUM           0xF
#define CAN_CMD_SET_RESOLUTION    0x10 //set data resolution
#define CAN_CMD_SET_MATRIX_G      0x11 //set matrix gain
#define CAN_CMD_GET_MATRIX_G      0x12 //get matrix gain
#define CAN_CMD_SET_CALIB_TARE    0x13 //set/reset calibration tare
#define CAN_CMD_GET_CALIB_TARE    0x14 //get calibration tare
#define CAN_CMD_SET_CURR_TARE     0x15 //set/reset current tare
#define CAN_CMD_GET_CURR_TARE     0x16 //get current tare
#define CAN_CMD_SET_FULL_SCALES   0x17 //set/reset current tare
#define CAN_CMD_GET_FULL_SCALES   0x18 //get current tare
#define CAN_CMD_SET_SERIAL_NO     0x19 //set/reset current tare
#define CAN_CMD_GET_SERIAL_NO     0x1A //get current tare
#define CAN_CMD_GET_EEPROM_STATUS 0x1B //get eeprom status (saved/not saved)
#define CAN_CMD_GET_FW_VERSION      0X1C //get fw version and check can protocol compatibility
#define CAN_CMD_GET_CH_GAIN       0X1D //get the gain of a channel
#define CAN_CMD_SET_CH_GAIN       0X1E //set the gain of a channel

//********************************************
//**** Messages belong to polling class  *****
//********************************************
#define CAN_CMD_FORCE_VECTOR      0xA // 010 SOURCE DEST 0xA t1 t1 t2 t2 t3 t3
#define CAN_CMD_TORQUE_VECTOR     0xB // 010 SOURCE DEST 0xB f1 f1 f2 f2 f3 f3


//********************************************
//**** Messages belong to loader class   *****
//********************************************
#define CMD_BOARD                 0x00 //Jump to CAN loader code. Replay with 0x00, 0x01
#define CMD_ADDRESS               0x01 //Address packet Data[1] is the length; Data[2] and Data[3] are the address; Data[4], is the block type (0x00 = program, 0x01 = data)
#define CMD_START                 0x02 //Start the program ????? Replay with 0x02, 0x01
#define CMD_DATA                  0x03 //Data packet: 6 bytes of payload are flashed to memory. Replay with 0x03, 0x01
#define CMD_END                   0x04 //The program is terminated. Replay with 0x04, 0x01 ????
#define CMD_ERROR                 0x05 //Command not used
#define CMD_GET_ADDITIONAL_INFO   0x0C // Get Additional Info
#define CMD_SET_ADDITIONAL_INFO      0x0D // Set Additional Info
#define CMD_BROADCAST             0xFF // Request for board type and firmware version
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section
    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


