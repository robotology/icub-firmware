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
#ifndef _EOETHBASEMODULE_H_
#define _EOETHBASEMODULE_H_

/** @file       EOethBaseModule.h
    @brief      This header file implements public interface to EOethBaseModule,
                       that provides base functionalities of ethernet communication.
    @author     valentina.gaggero@iit.it
    @date       01/02/2012
**/

/** @defgroup eo_ethBaseModule Object EOethBaseModule
                           The EOethBaseModule object provides base functionalities of ethernet communication: it let the user to configure/open/close 
                       a socket in Tx-Rx mode, to transmit and receive data, to be advised on rx packet, etc.
                      This module can be usefull if you have an obj that peforms communication on ethernet in simple way: 
                      you just create this EOethBaseModule obj and use its function without write any more code.
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "EOaction.h"
#include "EOpacket.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOethBaseModule_hid EOethBaseModule
    @brief      ........... 
 **/  
typedef struct EOethBaseModule_hid  EOethBaseModule; 


/** @typedef    typedef struct EOethBaseModule_cfg_t
    @brief            contains configuration data of this object
 **/  
typedef struct
{
    eOsizecntnr_t               dtagramQueue_itemNum;
    eOsizeitem_t                dtagramQueue_itemSize;
    eOipv4addr_t                remaddr;    /**< Remote address: if 0 it will initilized on connect */
    eOipv4port_t                remport;    /**< Remote port: if 0 it will initilized on connect */
    eOipv4port_t                localport;
    EOaction                    *action_onRec;   /**< if NULL, no action is performed on receive msg, but it is possible set it after. */
    eOreltime_t                 periodTx;       /**< period for trasmission: if 0 it isn't periodic socket */
} EOethBaseModule_cfg_t;       

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


/** @fn         extern EOethBaseModule* eo_ethBaseModule_New(EOethBaseModule_cfg_t *cfg)
    @brief      create a new EOethBaseModule obj
    @param      cfg              obj's configuration
    @return     The value eores_NOK_nullpointer if @e cfg is NULL, eores_OK otherwise.
 **/
extern EOethBaseModule* eo_ethBaseModule_New(EOethBaseModule_cfg_t *cfg);


/** @fn         extern eOresult_t eo_ethBaseModule_SetActionOnRec(EOethBaseModule *p, EOaction *action)
    @brief      set the action @e action that will be performed on packet reception
    @param      p                  reference to EOethBaseModule obj
    @param      action          the action to perform on reception of packet   
    @return     The value eores_NOK_nullpointer if @e p or  @e action are NULL, eores_OK otherwise.
 **/
extern eOresult_t eo_ethBaseModule_SetActionOnRec(EOethBaseModule *p, EOaction *action);


/** @fn        extern eOresult_t eo_ethBaseModule_Activate(EOethBaseModule *p)
    @brief      makes the obj active,  i.e. ready to transmit and receive.
    @param      p                   reference to EOethBaseModule obj
    @return     The value eores_NOK_nullpointer if @e p or  @e action are NULL, eores_OK otherwise.
 **/
extern eOresult_t eo_ethBaseModule_Activate(EOethBaseModule *p);


/** @fn        extern eOresult_t eo_ethBaseModule_Deactivate(EOethBaseModule *p)
    @brief      makes the obj deactive,  i.e. it is not ready to transmit and receive.
    @param      p                   reference to EOethBaseModule obj
    @return     The value eores_NOK_nullpointer if @e p or  @e action are NULL, eores_OK otherwise.
 **/
extern eOresult_t eo_ethBaseModule_Deactivate(EOethBaseModule *p);


/** @fn        extern eOresult_t eo_ethBaseModule_Receive(EOethBaseModule *p, uint8_t **payload_ptr, uint16_t *payloadsize)
    @brief      gets the received packet and returns its payloads. if the obj  had been configured with remote address equal to 0, 
                then it perform a connect on first received packet
    @param      p                    reference to EOethBaseModule obj
    @param      payload_ptr          in output contains the pointer to recieved packet's payload.
   @param      payloadsize           in output contains the size of to recieved packet
    @return     The value eores_NOK_nullpointer if @e p or  @e payload_ptr or  @e payloadsize are NULL, eores_OK otherwise.
 **/
extern eOresult_t eo_ethBaseModule_Receive(EOethBaseModule *p, uint8_t **payload_ptr, uint16_t *payloadsize);

/** @fn        extern eOresult_t eo_ethBaseModule_Transmit(EOethBaseModule *p, uint8_t *payload_ptr, uint16_t payloadsize)
    @brief      transmits  a packet with data pointed by  @e payload_ptr with size @e payloadsize. the destination of packet is that
                       then it perform a connect on first received packet
    @param      p                            reference to EOethBaseModule obj
    @param      payload_ptr         pointer packet's payload to transmit
   @param      payloadsize            size od data to transmit
    @return     The value eores_NOK_nullpointer if @e p is NULL,  eores_NOK_generic if destination is not valid, eores_OK otherwise.
 **/
extern eOresult_t eo_ethBaseModule_Transmit(EOethBaseModule *p, uint8_t *payload_ptr, uint16_t payloadsize);



/** @fn        extern eOresult_t eo_ethBaseModule_GetPacket(EOethBaseModule *p, EOpacket **pkt)
    @brief      gets the received packet. if the obj  had been configured with remote address equal to 0, 
                       then it perform a connect on first received packet
    @param      p                            reference to EOethBaseModule obj
    @param      pkt          in output contains the pointer to recieved packet.
    @return     The value eores_NOK_nullpointer if @e p or  @e payload_ptr or  @e payloadsize are NULL, eores_OK otherwise.
 **/
extern eOresult_t eo_ethBaseModule_GetPacket(EOethBaseModule *p, EOpacket **pkt);



/** @}            
    end of group eo_ethBaseModule 
 **/
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


