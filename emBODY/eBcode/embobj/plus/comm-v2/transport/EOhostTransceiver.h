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
#ifndef _EOONEHOSTTRANSCEIVER_H_
#define _EOONEHOSTTRANSCEIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOhostTransceiver.h
    @brief      This header file implements public interface to the host transceiver (pc104)
    @author     marco.accame@iit.it
    @date       09/06/2011
**/

/** @defgroup eo_ecvrevrebvtr2342r7 Object EOhostTransceiver
    The EOhostTransceiver is a singleton .....
      
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtransceiver.h"
#include "EOVmutex.h"
#include "EOropframe.h"



// - public #define  --------------------------------------------------------------------------------------------------

//#warning --> instead of 20 ... can i put 0? answer: seems yes  .. but be carefule w/ eo_ropframe_ROP_NumberOf_quickversion()
/*  Dimensions hereafter are related to the biggest packet the pc104 can send to the EMSs. It has to be equal to the biggest packet the EMS
    can receive and process ( which is different from the max packet the EMS can send to the pc104, because the protocol is asymmetric)
*/
#define EOK_HOSTTRANSCEIVER_emptyropframe_dimension            eo_ropframe_sizeforZEROrops     // header + footer + 8 byte for progressive number = 28
#define EOK_HOSTTRANSCEIVER_capacityoftxpacket                 768
#define EOK_HOSTTRANSCEIVER_capacityofrxpacket                 1408
#define EOK_HOSTTRANSCEIVER_capacityofrop                      256
#define EOK_HOSTTRANSCEIVER_capacityofropframeregulars         eo_ropframe_sizeforZEROrops
#define EOK_HOSTTRANSCEIVER_capacityofropframereplies          eo_ropframe_sizeforZEROrops
#define EOK_HOSTTRANSCEIVER_TMP                                ( EOK_HOSTTRANSCEIVER_capacityofropframeregulars + EOK_HOSTTRANSCEIVER_capacityofropframereplies + EOK_HOSTTRANSCEIVER_emptyropframe_dimension)
#define EOK_HOSTTRANSCEIVER_capacityofropframeoccasionals      (EOK_HOSTTRANSCEIVER_capacityoftxpacket - EOK_HOSTTRANSCEIVER_TMP)
#define EOK_HOSTTRANSCEIVER_maxnumberofregularrops             0
#define EOK_HOSTTRANSCEIVER_maxnumberofconfreqrops             16

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct
{
    const eOnvset_DEVcfg_t*         nvsetdevcfg;
    eOipv4addr_t                    remoteboardipv4addr;
    eOipv4port_t                    remoteboardipv4port;
    eOtransceiver_sizes_t           sizes;       
    eov_mutex_fn_mutexderived_new   mutex_fn_new;    
    eOtransceiver_protection_t      transprotection;
    eOnvset_protection_t            nvsetprotection; 
    eOconfman_cfg_t*                confmancfg;
} eOhosttransceiver_cfg_t;



/** @typedef    typedef struct EOhostTransceiver_hid EOhostTransceiver
    @brief      EOhostTransceiver is an opaque struct. It is used to implement data abstraction for the Parser  
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOhostTransceiver_hid EOhostTransceiver;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOhosttransceiver_cfg_t eo_hosttransceiver_cfg_default; // = { ... };


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
 
/** @fn         extern EOhostTransceiver * eo_hosttransceiver_Initialise(void)
    @brief      Initialise the singleton EOhostTransceiver. 
    @param      cfg         Contains actions to be done on reception or transmission which are specific to the application.
                            If NULL, then  is is issued a info by the EOtheErrorManager.
    @return     A valid and not-NULL pointer to the EOhostTransceiver singleton.
 **/
extern EOhostTransceiver * eo_hosttransceiver_New(const eOhosttransceiver_cfg_t *cfg);

extern EOtransceiver * eo_hosttransceiver_GetTransceiver(EOhostTransceiver *p);

extern EOnvSet * eo_hosttransceiver_GetNVset(EOhostTransceiver *p);

extern eOnvBRD_t eo_hosttransceiver_GetBoardNumber(EOhostTransceiver *p);

extern eOipv4addr_t eo_hosttransceiver_GetRemoteIP(EOhostTransceiver* p);



/** @}            
    end of group eo_ecvrevrebvtr2342r7  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



