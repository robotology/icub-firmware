/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _EOSOCKETDATAGRAM_HID_H_
#define _EOSOCKETDATAGRAM_HID_H_


/* @file       EOsocketDatagram_hid.h
    @brief      This header file implements hidden interface to a datagram socket object.
    @author     marco.accame@iit.it
    @date       08/25/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOfifo.h"
#include "EOsocket.h"
#include "EOtimer.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOsocketDatagram.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/* @struct     EOsocketDatagram_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOsocketDatagram_hid 
{
    EOsocket                *socket;            /**< the base socket */
    EOfifo                  *dgramfifoinput;    /**< fifo of input datagrams */
    EOfifo                  *dgramfifooutput;   /**< fifo of output datagrams */
    eOreltime_t             toutfifos;          /**< timeout for fifo manipulation */
    EOtimer                 *txtimer;
    eOsktdtgTXmode_t        txmode;  
    EOaction                *actiontx;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




