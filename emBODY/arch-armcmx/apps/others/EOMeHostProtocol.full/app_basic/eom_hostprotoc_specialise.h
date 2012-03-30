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
#ifndef _EOM_HOSTPROTOC_SPECIALISE_H_
#define _EOM_HOSTPROTOC_SPECIALISE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eom_hostprotoc_specialise.h
    @brief      This header file implements ....
    @author     marco.accame@iit.it
    @date       01/11/2012
**/

/** @defgroup sdew12345fefc cedcew fcevw
    vcrvrev frv ef e rfc d
 
    @todo acemor-facenda: do documentation.
    

    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "ipal.h"
#include "EOtheErrormanager.h"
#include "EOMtheIPnet.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const ipal_cfg_t*            eom_hostprotoc_specialise_ipal_cfg;

extern const eOerrman_cfg_t         eom_hostprotoc_specialise_errcfg;

extern const eOmipnet_cfg_dtgskt_t  eom_hostprotoc_specialise_dtgskt_cfg; 



// - declaration of extern public functions ---------------------------------------------------------------------------


extern void eom_hostprotoc_specialise_updserver_start(void);

extern void eom_hostprotoc_specialise_onpktreceived_set( void (*cbk)(EOpacket*) );

extern void eom_hostprotoc_specialise_otherthings(void);

extern void eom_hostprotoc_specialise_transmit(EOpacket *txpkt);

extern eObool_t eom_hostprotoc_specialise_connect(eOipv4addr_t remotehostaddr);
 

/** @}            
    end of group sdew12345fefc 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


