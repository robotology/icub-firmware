/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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

#ifndef _HL_CAN_COMM_H_
#define _HL_CAN_COMM_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_can_comm.h
	@brief      This file contains functions for communication over hl can.
	@author     marco.accame@iit.it
    @date       01/14/2013
**/

/** @defgroup doxy_group_hl_can_comm HL CAN COMM

    The HL CAN COMM contains functions for communication over CAN. The CAN port must be initialised and enabled
    by the basic hl_can module. Then the hl_can_comm module offers transmission of can frames either directly
    or by buffering first in a fifo and then by issuing a transmit command which empties the queue. Also, the
    hl_can_comm module offers background reception of can frames which are buffered in a fifo and retrieved using
    polling or a callback on rx mechanism. The hl_can_comm module uses ISRs.
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_can.h"
#include "hl_sys.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hl_can_comm_frameID_format_t 
    @brief      hl_can_comm_frameID_format_t is used to identify the format of the CAN frame.
 **/                
typedef enum
{ 
    hl_can_comm_frameID_std = 0,         /**< CAN frame ID of standard format       */
    hl_can_comm_frameID_ext = 1          /**< CAN frame ID of extended format      */
} hl_can_comm_frameID_format_t; 


/** @typedef    typedef enum hl_can_comm_frame_type_t 
    @brief      hl_can_comm_frame_type_t is used to identify the type of CAN frames.
 **/ 
typedef enum
{ 
    hl_can_comm_frame_data   = 0,       /**< CAN frame of type DATA                 */
    hl_can_comm_frame_remote = 1        /**< CAN frame of type REMOTE               */
} hl_can_comm_frame_type_t; 


enum { hl_can_comm_frame_data_maxsize = 8 };

/** @typedef    typedef struct hl_can_comm_frame_t 
    @brief      hl_can_comm_frame_t contains a can frame.
 **/ 
typedef struct
{   
    uint32_t                            id;             /**< can frame id    */
    hl_can_comm_frameID_format_t        id_type;        /**< can frame id format */
    hl_can_comm_frame_type_t            frame_type;     /**< frame type */
    uint8_t                             size;           /**< data size */
    uint8_t                             unused;         /**< filler */
    uint8_t                             data[hl_can_comm_frame_data_maxsize]; /**< the data */
} hl_can_comm_frame_t;


/** @typedef    typedef struct hl_can_comm_cfg_t;
    @brief      contains configuration for communication over can
 **/
typedef struct
{
    uint8_t                     capacityofrxfifoofframes;
    hl_irqpriority_t            priorityrx;
    hl_callback_t               callback_on_rx;                 /**< callback called by the rx ISR */
    void*                       arg_cb_rx;                      /**< argument of the rx callback */     
    uint8_t                     capacityoftxfifoofframes;
    hl_irqpriority_t            prioritytx;
    hl_callback_t               callback_on_tx;                 /**< callback called by the tx ISR */
    void*                       arg_cb_tx;                      /**< argument of the tx callback */
    hl_irqpriority_t            priorityerr;
    hl_callback_t               callback_on_err;                /**< callback called by the err ISR */
    void*                       arg_cb_err;                     /**< argument of the err callback */    
} hl_can_comm_cfg_t;


/** @typedef    typedef enum hl_can_comm_send_mode_t;
    @brief      expresses the send mode: immediate or deferred 
 **/
typedef enum
{
    hl_can_comm_send_now        = 0,          
    hl_can_comm_send_later      = 1   
} hl_can_comm_send_mode_t;


// VALE added type
/** @typedef    typedef struct hl_can_comm_status_t;
    @brief      contains status of can
 **/
typedef struct 
{
    union
    {
        uint32_t status;
        struct
        {
            struct
            {
                uint8_t  REC;           /**< Receive Error Counter */ 
                uint8_t  TEC;           /**< Trasmit Error Counter */ 
                uint8_t  warning:1;     /**< This bit is set by hardware when the warning limit has been reached
                                            (Receive Error Counter or Transmit Error Counter >= 96).See standard for more information*/
                uint8_t  passive:1;     /**<  This bit is set by hardware when the Error Passive limit has been reached (Receive Error
                                             Counter or Transmit Error Counter>127). */
                uint8_t  busoff:1;      /**< This bit is set by hardware when it enters the bus-off state. The bus-off state is entered on
                                             TEC overflow, greater than 255*/
                uint8_t  txqueueisfull:1; /**< if its value is 1 then the tx hw buffer is full. */
                uint8_t  rxqueueisfull:1; /**< if its value is 1 then the rx hw buffer is full. */
                uint8_t  dummy3b:3;
                
            } hw_status; //3 bytes NOTE: this information are set by hardware and propagate to application layer by an interrupt
            
            struct
            {
                uint8_t txqueueisfull:1; 
                uint8_t rxqueueisfull:1;
            } sw_status; //1 bytes NOTE: this information are set by software when try to put a new frame in sw output queue and it is full (txqueueisfull =1)
                        // or when the can receive a new frame and sw input queue is full (rxqueueisfull = 1).  en sw interrupt is generated.??!! 
        } s;
    } u;
} hl_can_comm_status_t;
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hl_result_t hl_can_comm_init(hl_can_t id, const hl_can_comm_cfg_t *cfg)
    @brief      This function configures communication over CAN. The CAN id   and CAN transceiver must have been already configured.
    @param      id              identifies CAN id   (CAN1 or CAN2)
    @param      cfg             the configuration of the can communication
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_can_comm_init(hl_can_t id, const hl_can_comm_cfg_t *cfg);


/** @fn         extern hl_result_t hl_can_comm_deinit(hl_can_t id)
    @brief      This function de-inits communication over CAN and reverts what is done in hl_can_comm_init().
    @param      id              identifies CAN id   (CAN1 or CAN2)
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_can_comm_deinit(hl_can_t id);


/** @fn         extern hl_result_t hl_can_enable(hl_can_t id)
    @brief      This function starts CAN communication. It must be invoked after hl_can_comm_init() and hl_can_enable().
    @param      id              identifies CAN id (CAN1 or CAN2)
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_can_comm_enable(hl_can_t id);


/** @fn         extern hl_result_t hl_can_comm_disable(hl_can_t id)
    @brief      This function disable CAN communication.
    @param      id              identifies CAN id   (CAN1 or CAN2)
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_can_comm_disable(hl_can_t id);


/** @fn         extern hl_result_t hl_can_comm_put(hl_can_t id, hl_can_comm_frame_t *frame, hl_can_comm_send_mode_t sm)
    @brief      This function puts a frame in the transmission queue.
                If sm flag has value hl_can_comm_send_now, it is also called the function hl_can_comm_transmit().
    @param      id              identifies CAN id   (CAN1 or CAN2)
    @param      frame           frame to put in queue
    @param      sm              indicates when to transmit
    @return     hl_res_NOK_generic in case the queue is full, else hl_res_OK
  */
extern hl_result_t hl_can_comm_put(hl_can_t id, hl_can_comm_frame_t *frame, hl_can_comm_send_mode_t sm);


/** @fn         extern hl_result_t hl_can_comm_transmit(hl_can_t id)
    @brief      This function transmits all frames in queue by direct transmission of the first element and then 
                by activating the CAN ISR. 
    @param      id          identifies CAN id (CAN1 or CAN2)
    @return     hl_res_NOK_generic in case wrong id, else hl_res_OK
  */
extern hl_result_t hl_can_comm_transmit(hl_can_t id);


/** @fn         extern hl_result_t hl_can_comm_received(hl_can_t id, uint8_t *numberof)
    @brief      This function gets number of frames in rx queue.
    @param      id              identifies CAN id (CAN1 or CAN2)
    @param      numberof        contains numbers of frames.
    @return     hl_res_NOK_generic in case wrong id or NULL argument, else hl_res_OK.
  */
extern hl_result_t hl_can_comm_received(hl_can_t id, uint8_t *numberof);

/** @fn         extern hl_result_t hl_can_comm_outgoing(hl_can_t id, uint8_t *numberof)
    @brief      This function gets number of frames in tx queue.
    @param      id              identifies CAN id (CAN1 or CAN2)
    @param      numberof        contains numbers of frames.
    @return     hl_res_NOK_generic in case wrong id or NULL argument, else hl_res_OK.
  */
extern hl_result_t hl_can_comm_outgoing(hl_can_t id, uint8_t *numberof);

/** @fn         extern hl_result_t hl_can_comm_get(hl_can_t id, hl_can_comm_frame_t *frame, uint8_t *remaining)
    @brief      This function gets the first frame in the rx queue.
    @param      id              identifies CAN id  CAN1 or CAN2)
    @param      frame           pointer to memory where the function copies the received frame
    @param      remaining       if not NULL, it returns the number of frames remaining in rx queue. 
    @return     hl_res_NOK_generic in case wrong id, hl_res_NOK_nodata in case of empty queue, else hl_res_OK.
  */
extern hl_result_t hl_can_comm_get(hl_can_t id, hl_can_comm_frame_t *frame, uint8_t *remaining); 


extern hl_result_t hl_can_comm_getstatus(hl_can_t id, hl_can_comm_status_t *status);

/** @}            
    end of group doxy_group_hl_can_comm  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



