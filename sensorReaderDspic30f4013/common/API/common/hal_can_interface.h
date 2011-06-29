/*  
 * Copyright (C) 2007 RobotCub Consortium, European Commission FP6 Project IST-004370
 * Author: Claudio Lorini, Marco Maggiali, Marco Randazzo, Valentina Gaggero
 * email:   claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
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
#ifndef _CAN_INTERFACE_H_
#define _CAN_INTERFACE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------
/**	@file 	hal_can_interface.h
	@brief 	provides can functionalities and defines can protocol messages.
	@author claudio.lorini@iit.it, marco.maggiali@iit.it, marco.randazzo@iit.it, valentina.gaggero@iit.it
	@date	-
*/
/*
Note: there are some inline functions that are wrapper of microchip's lib.
They are done for separete application code from micro's vendor code, in order to build ad Hardware Abstraction Layer.
*/

// - external dependencies --------------------------------------------------------------------------------------------
#include "dspic30_stdint.h"
#include <can.h>

#include "hal_errors.h"

// - public #define  --------------------------------------------------------------------------------------------------
// Frame formats
#define CAN_STANDARD_FORMAT 0
#define CAN_EXTENDED_FORMAT 1


#define CAN_MSG_CLASS_PERIODIC 0x0300
// For messages of class 011 the meaning of data/ID is defined as follows:
//  -------------------------- ----------------
// |           11b            |        8B      |
//  -------  -------  -------  -------  ------- 
// | 3b     | 4b     | 4b     |      B[0-7]    |
// |class	| Source | Type   |     Payload    |
//  -------  -------  -------  -------  ------- 

#define CAN_MSG_CLASS_POLLING 0x0200
// For messages of class 010 the meaning of data/ID is defined as follows:
//  -------------------------- -------------------------
// |           11b            |           8B            |
//  -------  -------  -------  -------  -------  ------- 
// | 3b     | 4b     | 4b     | B[0]   |     B[1-7]     |
// |class   | Source | Dest   | C type |    Payload     |
//  -------  -------  -------  -------  -------  ------- 

#define CAN_MSG_CLASS_LOADER   0x0700

#define CAN_TX_SOFTWARE_BUFFER_SIZE 10
#define CAN_MAX_MESSAGES_IN_BUFF 	10

//////////////////////////////////////////////////////////////
// CANLOADER MESSAGES, CLASS 0x02
// Incoming messages
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
#define CAN_CMD_MUX_EN         	  0xE  // ENABLE/DISABLES MUX
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
#define CAN_CMD_GET_FW_VERSION	  0X1C //get fw version and check can protocol compatibility
//////////////////////////////////////////////////////////////
// BROADCAST MESSAGES, CLASS 0x03
// Transmitted Torque values 3*16 bit 
#define CAN_CMD_FORCE_VECTOR      0xA // 010 SOURCE DEST 0xA t1 t1 t2 t2 t3 t3
#define CAN_CMD_TORQUE_VECTOR     0xB // 010 SOURCE DEST 0xB f1 f1 f2 f2 f3 f3
// hall effect sensors from 0 to 3*16bits
#define CAN_CMD_HES0TO3           0xC // 010 SOURCE DEST 0xC h0 h0 h1 h1 h2 h2 h3 h3
// hall effect sensors from 4 to 7*16bits
#define CAN_CMD_HES4TO7           0xD // 010 SOURCE DEST 0xC h4 h4 h5 h5 h6 h6 h7 h7
// hall effect sensors from 8 to 11*16bits
#define CAN_CMD_HES8TO11          0xE // 010 SOURCE DEST 0xC h8 h8 h9 h9 h10 h10 h11 h11
// hall effect sensors from 12 to 14*16bits
#define CAN_CMD_HES12TO14         0xF // 010 SOURCE DEST 0xC h12 h12 h13 h13 h14 h14 h15 h15

// hall effect sensors from 0 to 7*8bits
#define CAN_CMD_HES0TO6           0xC // 010 SOURCE DEST 0xC h0 h1 h2 h3 h4 h5 h6 
// hall effect sensors from 8 to 14*8bits
#define CAN_CMD_HES7TO14          0xD // 010 SOURCE DEST 0xD h7 h8 h9 h10 h11 h12 h13 h14 

// hall effect sensors from 0 to 4*12bits
#define CAN_CMD_HES0TO4           0xC // 010 SOURCE DEST 0xC h0 h1 h2 h3 h4 
// hall effect sensors from 5 to 9*12bits
#define CAN_CMD_HES5TO9          0xD // 010 SOURCE DEST 0xD h5 h6 h7 h8 h9 
// hall effect sensors from 10 to 14*12bits
#define CAN_CMD_HES10TO14          0xE // 010 SOURCE DEST 0xD h10 h11 h12 h13 h14 

//////////////////////////////////////////////////////////////
// CANLOADER MESSAGES, CLASS 0x07
#define CMD_BOARD                 0x00 //Jump to CAN loader code. Replay with 0x00, 0x01
#define CMD_ADDRESS               0x01 //Address packet Data[1] is the length; Data[2] and Data[3] are the address; Data[4], is the block type (0x00 = program, 0x01 = data)
#define CMD_START                 0x02 //Start the program ????? Replay with 0x02, 0x01
#define CMD_DATA                  0x03 //Data packet: 6 bytes of payload are flashed to memory. Replay with 0x03, 0x01
#define CMD_END                   0x04 //The program is terminated. Replay with 0x04, 0x01 ????
#define CMD_ERROR                 0x05 //Command not used
#define CMD_GET_ADDITIONAL_INFO   0x0C // Get Additional Info
#define CMD_SET_ADDITIONAL_INFO	  0x0D // Set Additional Info
#define CMD_BROADCAST             0xFF // Request for board type and firmware version

 
// - declaration of public user-defined types ------------------------------------------------------------------------- 
// Even if DSPIC33f4013 has not 2 CAN ports, we define hal_can_port_t for compatibility with other HAL library.
// In all can function, the param of type hal_can_port_t, is not considerated.
typedef enum
{
	hal_can_portCAN1 = 1,
	hal_can_portCAN2 = 2
}hal_can_port_t; 

typedef enum
{
	hal_can_frameType_data = 0,
	hal_can_frameType_remote = 1
}hal_can_frameType_t;


// can bus message structure
typedef struct
{
  union
  {
    struct // for polling messages
    {
      uint16_t CAN_Poll_RxIDE            :1;
      uint16_t CAN_Poll_SRR              :1;
      uint16_t CAN_Poll_Msg_Dest         :4;  // Destination
      uint16_t CAN_Poll_Msg_Source       :4;  // Source
      uint16_t CAN_Poll_Msg_Class        :3;  // Class of message
      uint16_t Poll_NotUsed              :3;
      hal_can_frameType_t CAN_Poll_Msg_CType;       // C+Type
      uint8_t CAN_Msg_Length;           // Message Length
      uint8_t CAN_Poll_Msg_PayLoad[8];  // Payload 
    };
    struct // for periodic messages
    {
  	  uint16_t CAN_Per_RxIDE             :1;
	  uint16_t CAN_Per_SRR               :1;
	  uint16_t CAN_Per_Msg_Type          :4;  // Destination
	  uint16_t CAN_Per_Msg_Source        :4;  // Source
	  uint16_t CAN_Per_Msg_Class         :3;  // Class of message
	  uint16_t Per_NotUsed               :3; 
	  hal_can_frameType_t CAN_Poll_Msg_CType;       // C+Type
	  uint8_t CAN_Msg_Length;           // Message Length
	  uint8_t CAN_Per_Msg_PayLoad[8];   // Payload 
    };
    struct
    {
	  uint16_t CAN_Poll_RxIDE          :1;
	  uint16_t CAN_Poll_SRR            :1;
	  uint16_t CAN_Msg_ID              :11; 
	  uint16_t NotUsed                 :3;
	  hal_can_frameType_t CAN_Poll_Msg_CType;       // C+Type
	  uint8_t CAN_Msg_Length;           // Message Length
	  uint8_t CAN_Msg_PayLoad[8];   // Payload  
    };
    uint16_t CAN_Msg_AsBytes[12];   // Message data formatted as bytes 
  };
} hal_canmsg_t;


// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
/**
  * @fn      extern void hal_can_init(hal_can_port_t can_port);
  * @brief   initialises can peripheral
  * @param      can_port : indicate which interface initialise (CAN1 or CAN2). Actually it is ignored.
  * @retval  none
  */
extern void hal_can_init(hal_can_port_t can_port);


/**
  * @fn     extern void hal_can_boardFilter_set(hal_can_port_t can_port, unsigned char can_board_addr);
  * @brief  sets filters to can interface
  * @param      can_port: indicate on witch interface applay filters. Actually it is ignored.
  * @param      can_board_addr: is the filter. Usually is the can board address
  * @retval none
  */
extern void hal_can_boardFilter_set(hal_can_port_t can_port, uint8_t can_board_addr);


/**
  * @fn     extern hal_result_t char hal_can_put(hal_can_port_t can_port, unsigned char can_board_addr, unsigned int MessageID,
                             unsigned char FrameType, unsigned char Length, unsigned char *Data);
  * @brief  puts a new massage in the queue and enable tx interrupt
  * @param      can_port:  can interface. Actually it is ignored.
  * @param      can_board_addr
  * @param      MessageID
  * @param      FrameType
  * @param      MessageID
  * @param      FrameType
  * @param      Length
  * @param      Data
  * @retval      - HAL_RES_NOK in case of overrun
                 - HAL_RES_OK otherwise;
  */
extern hal_result_t hal_can_put(hal_can_port_t can_port, uint8_t can_board_addr, uint16_t MessageID,
                             hal_can_frameType_t FrameType, uint8_t Length, uint8_t *Data); 

/**
  * @fn    extern uint8_t hal_can_buff_rx_isEmpty(hal_can_port_t can_port);
  * @brief indicates if there is any massage in buffer rx
  * @param    can_port: can interface. Actually it is ignored.
  * @retval   1 if the buffer rx is empty
              0 if there is 1 or more message in the buffer rx
  */
extern uint8_t hal_can_buff_rx_isEmpty(hal_can_port_t can_port);

/**
  * @fn    extern hal_result_t hal_can_get(hal_can_port_t can_port, canmsg_t *msg_ptr);
  * @brief this function get the last inserted message. Note that durinf this function interupts on can are disabled.
  * @param    can_port: can interface. Actually it is ignored.
  * @param    msg_ptr: the function will copy the message from buffer to the memory location pointed by msg_ptr
  * @retval   HAL_RES_NOK in case msg_ptr is NULL
              HAL_RES_OK otherwise
  */
extern hal_result_t hal_can_get(hal_can_port_t can_port, hal_canmsg_t *msg_ptr);


/**
  * @fn		inline extern void hal_can_put_immediately(hal_can_port_t can_port, uint16_t SID, uint8_t *data_ptr, uint8_t data_length, uint8_t hw_buff )
  * @brief  is a simple wrapper of CAN1SendMessage. 	
  * @param		hw_buff: indicates hardaware buffer; it can values: hw e può valere 0, 1 o 2.
						If it is bigger than 2 it will be considerated 0. 
  * @param		can_port:  can_interface. Actually it is ignored.
  * @retval 		
  */

inline extern void hal_can_put_immediately(hal_can_port_t can_port, uint16_t SID, uint8_t *data_ptr, uint8_t data_length, uint8_t hw_buff )
{
	CAN1SendMessage((CAN_TX_SID(SID)) & CAN_TX_EID_DIS & CAN_SUB_NOR_TX_REQ, (CAN_TX_EID(0x0)) & CAN_NOR_TX_REQ,
					 data_ptr,data_length,hw_buff);
}

/**
  * @fn		inline extern uint8_t hal_can_txHwBuff_isEmpty(hal_can_port_t can_port, uint8_t hwBuff_num)
  * @brief is only a wrapper oh CAN1IsTXReady(hwBuff_num). 
  * @param		can_port:  can_interface. Actually it is ignored.
  * @retval	1 if tx hardware buffer is empty, else 0.
  */
inline extern uint8_t hal_can_txHwBuff_isEmpty(hal_can_port_t can_port, uint8_t hwBuff_num)
{
	return(CAN1IsTXReady(hwBuff_num));
}




// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


