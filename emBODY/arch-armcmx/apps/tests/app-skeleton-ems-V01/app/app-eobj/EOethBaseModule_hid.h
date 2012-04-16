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
#ifndef _EOETHBASEMODULE_HID_H_
#define _EOETHBASEMODULE_HID_H_


/** @file       EOethBaseModule_hid.h
    @brief      This header file implements hidden interface to ....
    @author     valentina.gaggero@iit.it
    @date       01/02/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EoCommon.h"
#include "EOSocketDatagram.h"
#include "EOaction.h"
#include "EOpacket.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOethBaseModule.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------
struct EOethBaseModule_hid
{
    EOsocketDatagram                    *socket;
    EOaction                            *action_onrx;
    eOipv4addr_t                        remaddr;
    eOipv4port_t                        remport;
    eOipv4port_t                        localport;
    eOreltime_t                         periodTx;
    EOpacket                            *rxpkt;
    EOpacket                            *txpkt;
    eOreltime_t                         connect_tout; //timeout on connect
    uint8_t                             conected_st;
} /*appEthBaseModule_hid_t*/;



// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



