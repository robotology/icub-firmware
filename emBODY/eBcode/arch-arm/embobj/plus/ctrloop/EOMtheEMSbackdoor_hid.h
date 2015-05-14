/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOMTHEEMSBACKDOOR_HID_H_
#define _EOMTHEEMSBACKDOOR_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOMtheEMSbackdoor_hid.h
    @brief      This header file implements hidden interface to the error manager
    @author     marco.accame@iit.it
    @date       03/21/2013
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"
#include "EOpacket.h"
#include "EOsocketDatagram.h"
#include "osal_semaphore.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheEMSbackdoor.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EOMtheEMSbackdoor_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
struct EOMtheEMSbackdoor_hid 
{
    EOMtask*                            task;
    EOsocketDatagram*                   socket;
    EOpacket                            *rxpkt;
    EOpacket                            *txpkt;
    eOipv4addr_t                        hostaddress;
    eObool_t                            connected2host;   
    eOemsbackdoor_cfg_t                 cfg;
    osal_semaphore_t*                   txsemaphore;
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------

//extern void eom_emsbackdoor_hid_userdef_DoJustAfterPacketParsing(EOMtheEMSbackdoor* p);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

