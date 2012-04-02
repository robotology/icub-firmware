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
#ifndef _EOSOCKET_H_
#define _EOSOCKET_H_

/** @file       EOsocket.h
    @brief      This header file implements public interface to a base socket object.
    @author     marco.accame@iit.it
    @date       08/25/2011
**/

/** @defgroup eo_socket Object EOsocket
    The EOsocket is the base object for socket-based communication. It is used for derivation. See for
    example #EOsocketDatagram.
     
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

 

/** @typedef    typedef struct EOsocket_hid EOsocket
    @brief      EOsocket is an opaque struct. It is used to implement data abstraction for the timer 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOsocket_hid EOsocket;


/** @typedef    typedef void EOsocketDerived
    @brief      EOsocketDerived is used to implement polymorphism in the objects derived from EOsocket
 **/
typedef void EOsocketDerived;



typedef enum  
{
    eo_skttyp_datagram  = 0,                    /**< datagram socket        */    
    eo_skttyp_stream    = 1,                    /**< stream socket: not supported so far    */
    eo_skttyp_none      = EOK_uint08dummy       /**< no type */
} eOsocketType_t;



/** @typedef        typedef enum eOsocketDirection_t
    @brief          eOsocketDirection_t contains the directions which a socket can be (tx, rx or both)
 **/
typedef enum  
{
    eo_sktdir_TXonly    = 1,         /**< The socket only transmits             */
    eo_sktdir_RXonly    = 2,         /**< The socket only receives              */
    eo_sktdir_TXRX      = 3          /**< The socket transmits and receives     */
} eOsocketDirection_t;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section



// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOsocket* eo_socket_New(void);



/** @}            
    end of group eo_socket  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

