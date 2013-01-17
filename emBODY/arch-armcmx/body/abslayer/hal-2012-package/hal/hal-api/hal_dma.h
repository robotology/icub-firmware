/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HAL_DMA_H_
#define _HAL_DMA_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       hal_dma.h
    @brief      This header file implements public interface to the hal dma module.
    @author     marco.accame@iit.it
    @date       12/21/2012
**/

/** @defgroup doxy_group_hal_dma HAL DMA

    The HAL DMA ...
 
    @todo acemor-facenda: review documentation.
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "hal_base.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef enum hal_dma_transfer_t 
    @brief      hal_dma_transfer_t is used to specify the nature of the dma transfer
 **/                
typedef enum
{ 
//    hal_dma_transfer_mem2mem    = 0,    /**< from memory to memory */
    hal_dma_transfer_mem2per    = 1,    /**< from memory to peripheral */
    hal_dma_transfer_per2mem    = 2     /**< from peripheral to memory */
} hal_dma_transfer_t; 

/** @typedef    typedef enum hal_dma_mode_t 
    @brief      hal_dma_mode_t is used to specify the mode of the transfer
 **/                
typedef enum
{ 
    hal_dma_mode_oneshot        = 0,    /**< once the transfer is completed it calls the callback and stops the port */
    hal_dma_mode_forever        = 1     /**< once the transfer is completed ...................... not supported yet */
} hal_dma_mode_t; 

/** @typedef    typedef enum hal_dma_port_t 
    @brief      hal_dma_port_t contains the possible dma ports.
 **/
typedef enum  
{ 
    hal_dma_port1  = 0,         /**< DMA port1  */
    hal_dma_port2  = 1,         /**< DMA port2  */
    hal_dma_port3  = 2,         /**< DMA port3  */
    hal_dma_port4  = 3,         /**< DMA port4  */
    hal_dma_port5  = 4,         /**< DMA port5  */
    hal_dma_port6  = 5,         /**< DMA port6  */
    hal_dma_port7  = 6,         /**< DMA port7  */
    hal_dma_port8  = 7,         /**< DMA port8  */
    hal_dma_port9  = 8,         /**< DMA port9  */
    hal_dma_port10 = 9,         /**< DMA port10 */
    hal_dma_port11 = 10,        /**< DMA port11 */
    hal_dma_port12 = 11,        /**< DMA port12 */
    hal_dma_port13 = 12,        /**< DMA port13 */
    hal_dma_port14 = 13,        /**< DMA port14 */
    hal_dma_port15 = 14,        /**< DMA port15 */
    hal_dma_port16 = 15         /**< DMA port16 */
} hal_dma_port_t; 

enum { hal_dma_ports_num = 16 };



/** @typedef    typedef struct hal_dma_cfg_t;
    @brief      contains configuration data of dma port
 **/
typedef struct
{
    hal_dma_transfer_t          transfer;
    hal_dma_mode_t              mode;
    hal_interrupt_priority_t    intpriority;
    uint16_t                    datasize;
    void*                       source;
    void*                       destin;
    hal_callback_t              cbk_on_transfer_done;
    void*                       arg_on_transfer_done;
} hal_dma_cfg_t;



 
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern hal_result_t hal_dma_init(hal_dma_port_t port, const hal_dma_cfg_t *cfg)
    @brief      This function configures a dma port.
    @param      port            identifies the DMA port
    @param      cfg             the configuration of DMA port
    @return     hal_res_NOK_generic in case of error, else hal_res_OK
  */
extern hal_result_t hal_dma_init(hal_dma_port_t port, const hal_dma_cfg_t *cfg);


/** @fn         extern hal_result_t hal_dma_enable(hal_dma_port_t port)
    @brief      This function starts DMA. It must be invoked after hal_dma_init.
    @param      port            identifies port
    @return     hal_res_NOK_generic in case of error, else hal_res_OK
  */
extern hal_result_t hal_dma_enable(hal_dma_port_t port);


/** @fn         extern hal_result_t hal_dma_disable(hal_dma_port_t port)
    @brief      This function disable DMA.
    @param      port            identifies DMA port
    @return     hal_res_NOK_generic in case of error, else hal_res_OK
  */
extern hal_result_t hal_dma_disable(hal_dma_port_t port);


/** @fn         extern hal_result_t hal_dma_retrigger(hal_dma_port_t port)
    @brief      This function retriggers the dma. it must be called only inside the callback on transfer done when the mode is one shot.
                If destin is NULL it has the same effect as hal_dma_retrigger().
    @param      port            identifies DMA port
    @param      source          the new source pointer
    @return     hal_res_NOK_generic in case queue is full or wrong port, else hal_res_OK
  */
extern hal_result_t hal_dma_retrigger(hal_dma_port_t port);

extern hal_result_t hal_dma_dontdisable(hal_dma_port_t port);

/** @fn         extern hal_result_t hal_dma_source_set(hal_dma_port_t port, void* source)
    @brief      This function changes the dma source. it must be called only inside the callback on transfer done.
                If source is NULL it has the same effect as hal_dma_retrigger().
    @param      port            identifies DMA port
    @param      source          the new source pointer
    @return     hal_res_NOK_generic in case queue is full or wrong port, else hal_res_OK
  */
extern hal_result_t hal_dma_source_set(hal_dma_port_t port, void* source);


/** @fn         extern hal_result_t hal_dma_destin_set(hal_dma_port_t port, void* destin)
    @brief      This function changes the dma destination. it must be called only inside the callback on transfer done.
                If destin is NULL it has the same effect as hal_dma_retrigger().
    @param      port            identifies DMA port
    @param      source          the new source pointer
    @return     hal_res_NOK_generic in case queue is full or wrong port, else hal_res_OK
  */
extern hal_result_t hal_dma_destin_set(hal_dma_port_t port, void* destin);


extern void hal_dma_isr_enable(hal_dma_port_t port);

extern void hal_dma_isr_disable(hal_dma_port_t port);

/** @}            
    end of group doxy_group_hal_dma  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



