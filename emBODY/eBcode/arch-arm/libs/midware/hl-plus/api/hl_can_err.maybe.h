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

#ifndef _HL_CAN_ERR_H_
#define _HL_CAN_ERR_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hl_can_err.h
	@brief      This file contains functions for error handling for hl can.
	@author     marco.accame@iit.it
    @date       01/14/2013
**/

/** @defgroup doxy_group_hl_can_err HL CAN ERR

    The HL CAN ERR contains functions for error handling of CAN. 
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hl_common.h"
#include "hl_can.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 



/** @typedef    typedef struct hl_can_comm_cfg_t;
    @brief      contains configuration for communication over can
 **/
typedef struct
{
    hl_irqpriority_t            priorityerr;
    hl_callback_t               callback_on_err;                /**< callback called by the err ISR */
    void*                       arg_cb_err;                     /**< argument of the err callback */    
} hl_can_err_cfg_t;

// VALE added type
/** @typedef    typedef struct hl_can_err_status_t;
    @brief      contains status of can peripheral.
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
} hl_can_err_status_t;
 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hl_result_t hl_can_err_init(hl_can_t id, const hl_can_err_cfg_t *cfg)
    @brief      This function configures error handling over CAN.
    @param      id              identifies CAN id
    @param      cfg             the configuration of the can error handling
    @return     hl_res_NOK_generic in case of error, else hl_res_OK
  */
extern hl_result_t hl_can_err_init(hl_can_t id, const hl_can_err_cfg_t *cfg);



/** @fn         extern hl_result_t hl_can_err_getstatus(hl_can_t id, hl_can_err_status_t *status)
    @brief      This function retrieves the can error status.
    @param      id              identifies CAN id 
    @param      status          the status
    @return     hl_res_NOK_generic else hl_res_OK
  */
extern hl_result_t hl_can_err_getstatus(hl_can_t id, hl_can_err_status_t *status)




/** @}            
    end of group doxy_group_hl_can_err  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



