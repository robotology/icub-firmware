/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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

#ifndef _HAL_CAN_H_
#define _HAL_CAN_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_can.h
    @brief      This header file implements public interface to the hal can module.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       09/09/2011
**/

/** @defgroup doxy_group_hal_can HAL CAN

    The HAL CAN ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------

#define hal_can_frame_payload_maxlen    8   /**< max length of can frame's payload (expressed in number of bytes) */
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hal_can_frameID_format_t 
    @brief      hal_can_frameID_format_t is used to identify the format of the CAN frame.
 **/                
typedef enum
{ 
    hal_can_frameID_std = 0,         /**< CAN frame ID of standard format       */
    hal_can_frameID_ext = 1          /**< CAN frame ID of externded format      */
} hal_can_frameID_format_t; 


/** @typedef    typedef enum hal_can_frame_type_t 
    @brief      hal_can_frame_type_t is used to identify the type of CAN frames.
 **/ 
typedef enum
{ 
    hal_can_frame_data   = 0,       /**< CAN frame of type DATA                 */
    hal_can_frame_remote = 1        /**< CAN frame of type REMOTE               */
} hal_can_frame_type_t; 


/** @typedef    typedef struct hal_can_frame_t 
    @brief      hal_can_frame_t contains a can frame.
 **/ 
typedef struct
{   
    uint32_t                        id;             /**< can frame id    */
    hal_can_frameID_format_t        id_type;        /**< can frame id format */
    hal_can_frame_type_t            frame_type;     /**< frame type */
    uint8_t                         size;           /**< data size */
    uint8_t                         unused;         /**< filler */
    uint8_t                         data[hal_can_frame_payload_maxlen]; /**< the data (payload) */
} hal_can_frame_t;


/** @typedef    typedef enum hal_can_t 
    @brief      hal_can_t contains the usable can ports.
 **/
typedef enum  
{ 
    hal_can1 = 0,           /**< CAN1        */
    hal_can2 = 1            /**< CAN2        */
} hal_can_t; 

enum { hal_cans_number = 2 };


/** @typedef    typedef enum hal_can_send_mode_t;
    @brief      expresses the send mode: immediate on normal queue or priority or deferred on normal queue
 **/
typedef enum
{
    hal_can_send_normprio_now           = 0,          
    hal_can_send_normprio_later         = 1,      
    hal_can_send_highprio_now           = 2        
} hal_can_send_mode_t;


/** @typedef    typedef enum hal_can_runmode_t;
    @brief      It specifies how the CAN module works underneath the API layer. It is possible to make it work without
                any ISR for basic use or with ISRs (at least RX and TX CAN, but depending on the implementation also DMA
                can be used).
 **/
typedef enum
{
    hal_can_runmode_noisr_0q        = 0,    /**< No use of ISRs and no queues. It is for basic use, e.g., in a CAN-based bootloader */           
    hal_can_runmode_isr_1txq1rxq    = 1,    /**< ISRs but also queues in RX and TX */   
    hal_can_runmode_isr_2txq1rxq    = 2    /**< ISRs but also queues in RX and TX and a high priority queue in TX */   
//    hal_can_runmode_isr_1txq1rxqram = 3     /**< ISRs but also queues in RX and TX which use dynamic ram */ 
} hal_can_runmode_t;

enum { hal_can_runmode_num = 3};


/** @typedef    typedef enum hal_can_baudrate_t;
    @brief      expresses can baudrate. 
 **/
typedef enum
{
    hal_can_baudrate_1mbps      = 0,
    hal_can_baudrate_500kbps    = 1    
} hal_can_baudrate_t;


/** @typedef    typedef struct hal_can_cfg_t;
    @brief      contains configuration data of can peripheral.
 **/
typedef struct
{
    hal_can_runmode_t           runmode;
    hal_can_baudrate_t          baudrate; 
    hal_interrupt_priority_t    priorx;
    hal_interrupt_priority_t    priotx;
    uint8_t                     capacityofrxfifoofframes;
    uint8_t                     capacityoftxfifoofframes;
    uint8_t                     capacityoftxfifohighprio;
    void (*callback_on_rx)(void *arg);
    void *arg_cb_rx;
    void (*callback_on_tx)(void *arg);
    void *arg_cb_tx;
    void (*callback_on_err)(void *arg); //VALE added field
    void *arg_cb_err; //VALE added field
} hal_can_cfg_t;

//VALE added type
/** @typedef    typedef struct hal_can_status_t;
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
                
            }hw_status; //3 bytes NOTE: this information are set by hardware and propagate to application layer by an interrupt
            
            struct
            {
                uint8_t txqueueisfull:1; 
                uint8_t rxqueueisfull:1;
            }sw_status; //1 bytes NOTE: this information are set by software when try to put a new frame in sw output queue and it is full (txqueueisfull =1)
                        // or when the can receive a new frame and sw input queue is full (rxqueueisfull = 1).  en sw interrupt is generated.??!! 
        }s;
    }u;
} hal_can_status_t;

 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const hal_can_cfg_t hal_can_cfg_default;


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hal_result_t hal_can_init(hal_can_t id, const hal_can_cfg_t *cfg)
    @brief      This function configures CAN.
    @param      port            identifies CAN port (CAN1 or CAN2)
    @param      cfg             teh configuration of teh can peripheral
    @return     hal_res_NOK_generic in case of error, else hal_res_OK
  */
extern hal_result_t hal_can_init(hal_can_t id, const hal_can_cfg_t *cfg);


/** @fn         extern hal_result_t hal_can_enable(hal_can_t id)
    @brief      This function starts CAN. It must be invoked after hal_can_init.
    @param      port            identifies CAN port (CAN1 or CAN2)
    @return     hal_res_NOK_generic in case of error, else hal_res_OK
  */
extern hal_result_t hal_can_enable(hal_can_t id);


/** @fn         extern hal_result_t hal_can_disable(hal_can_t id)
    @brief      This function disable CAN.
    @param      port            identifies CAN port (CAN1 or CAN2)
    @return     hal_res_NOK_generic in case of error, else hal_res_OK
  */
extern hal_result_t hal_can_disable(hal_can_t id);


/** @fn         extern hal_result_t hal_can_put(hal_can_t id, hal_can_frame_t *frame, hal_can_send_mode_t sm)
    @brief      This function puts frame in the trasmission queue.
  *             If sm flag values "send_now", this functions invokes isr to trasmit frames in queue,
                  even if return value is hal_res_NOK_generic.The frame in input is dicsarded.
    @param      port            identifies CAN port (CAN1 or CAN2)
    @param      frame           frame to put in queue
    @param      sm              indicates if trasmit fames now or not.
    @return     hal_res_NOK_generic in case queue is full or wrong port, else hal_res_OK
  */
extern hal_result_t hal_can_put(hal_can_t id, hal_can_frame_t *frame, hal_can_send_mode_t sm);


/** @fn         extern hal_result_t hal_can_transmit(hal_can_t id)
    @brief      This function trasmits all frames in queue.
    @param      port        identifies CAN port (CAN1 or CAN2)
    @return     hal_res_NOK_generic in case wrong port, else hal_res_OK
  */
extern hal_result_t hal_can_transmit(hal_can_t id);


/** @fn         extern hal_result_t hal_can_received(hal_can_t id, uint8_t *numberof)
    @brief      This function gets number of frames in rx queue (FIFO).
    @param      port            identifies CAN port (CAN1 or CAN2)
    @param      numberof        contains numbers of frames.
    @return     hal_res_NOK_generic in case wrong port or NULL argument, else hal_res_OK.
  */
extern hal_result_t hal_can_received(hal_can_t id, uint8_t *numberof);


/** @fn         extern hal_result_t hal_can_get(hal_can_t id, hal_can_frame_t *frame, uint8_t *remaining)
    @brief      This function gets first frame in queue (FIFO).
    @param      port            identifies CAN port (CAN1 or CAN2)
    @param      frame           ptr to memeory where functions put getted frame
    @param      remaining       in output contains numbers of remaining frames. (it can be NULL)
    @return     hal_res_NOK_generic in case wrong port, hal_res_NOK_nodata in case of empty queue, else hal_res_OK.
  */
extern hal_result_t hal_can_get(hal_can_t id, hal_can_frame_t *frame, uint8_t *remaining); 


/** @fn         extern hal_result_t hal_can_receptionfilter_set(hal_can_t id, uint8_t mask_num, uint32_t mask_val, uint8_t identifier_num,
                                                uint32_t identifier_val, hal_can_frameID_format_t idformat)
    @brief      This function configures reception filter.
    @details    A reception filter behaves in this way: if rec_id is the identifier of the received id, the packet will be 
                passed to software reception queue if and only if this equation is satisfied: 
                (mask_val & rec_id) == (mask_val & identifier_val).
                So a filter is composed by the couple mask and identifier: the mask says which bits must be examinated, 
                while the identifier says the values of these bits.
                In order to configure a filter correctly, you must know that there is a table containing the mask values 
                and a table containing the identifier_val. Also, it is possible to associate to a mask more indentifier_val.
                Pay attention: if you use the same mask_num with different mask_val, the last value overwrites the first.
    @param      port            identifies CAN port
    @param      mask_num        number of row in mask table. (Must start at 0)
    @param      mask_val        value of mask at @e mask_num row
    @param      identifier_num  number of row in identifier table. (Must start at 0)
    @param      identifier_val  value of identifier at @e identifier_num row
    @param      idformat        the id format
    @return     hal_res_NOK_generic in case of error, else hal_res_OK
**/
extern hal_result_t hal_can_receptionfilter_set(hal_can_t id, uint8_t mask_num, uint32_t mask_val, uint8_t identifier_num,
                                                uint32_t identifier_val, hal_can_frameID_format_t idformat);


/** @fn         extern hal_result_t hal_can_out_get(hal_can_t id, uint8_t *numberof)
    @brief      This function gets number of frames in tx queue (FIFO).
    @param      port            identifies CAN port (CAN1 or CAN2)
    @param      numberof        contains numbers of frames.
    @return     hal_res_NOK_generic in case wrong port or NULL argument, else hal_res_OK.
  */
extern hal_result_t hal_can_out_get(hal_can_t id, uint8_t *numberof);


/** @}            
    end of group doxy_group_hal_can  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



