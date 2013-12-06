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
#include "iCubCanProtocol.h"

// - public #define  --------------------------------------------------------------------------------------------------
// Frame formats
#define CAN_STANDARD_FORMAT 0
#define CAN_EXTENDED_FORMAT 1


#define CAN_MSG_CLASS_PERIODIC (ICUBCANPROTO_CLASS_PERIODIC_ANALOGSENSOR << 8) //0x0300
// For messages of class 011 the meaning of data/ID is defined as follows:
//  -------------------------- ----------------
// |           11b            |        8B      |
//  -------  -------  -------  -------  ------- 
// | 3b     | 4b     | 4b     |      B[0-7]    |
// |class	| Source | Type   |     Payload    |
//  -------  -------  -------  -------  ------- 

#define CAN_MSG_CLASS_POLLING (ICUBCANPROTO_CLASS_POLLING_ANALOGSENSOR<< 8)//x0200
// For messages of class 010 the meaning of data/ID is defined as follows:
//  -------------------------- -------------------------
// |           11b            |           8B            |
//  -------  -------  -------  -------  -------  ------- 
// | 3b     | 4b     | 4b     | B[0]   |     B[1-7]     |
// |class   | Source | Dest   | C type |    Payload     |
//  -------  -------  -------  -------  -------  ------- 

#define CAN_MSG_CLASS_LOADER   (ICUBCANPROTO_CLASS_BOOTLOADER<<8)//0x0700

#define CAN_TX_SOFTWARE_BUFFER_SIZE 10
#define CAN_MAX_MESSAGES_IN_BUFF 	10

 
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


