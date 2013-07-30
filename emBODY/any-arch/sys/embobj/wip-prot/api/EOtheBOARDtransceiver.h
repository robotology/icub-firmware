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
#ifndef _EOTHEBOARDTRANSCEIVER_H_
#define _EOTHEBOARDTRANSCEIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtheBOARDtransceiver.h
    @brief      This header file implements public interface to board transceiver
    @author     marco.accame@iit.it
    @date       09/06/2011
**/

/** @defgroup eo_ecvrevrebvtr2342r4 Object EOtheBOARDtransceiver
    The EOtheBOARDtransceiver is a singleton .....
      
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtransceiver.h"


// - public #define  --------------------------------------------------------------------------------------------------

//#define EOK_BOARDTRANSCEIVER_capacityofpacket                   640
//#define EOK_BOARDTRANSCEIVER_capacityofrop                      192
//#define EOK_BOARDTRANSCEIVER_capacityofropframeregulars         384
//#define EOK_BOARDTRANSCEIVER_capacityofropframeoccasionals      128
//#define EOK_BOARDTRANSCEIVER_capacityofropframereplies          128 
//#define EOK_BOARDTRANSCEIVER_maxnumberofregularrops             16 

// - declaration of public user-defined types ------------------------------------------------------------------------- 


typedef struct
{
    const eOnvset_DEVcfg_t*         nvsetdevcfg;          
    eOipv4addr_t                    remotehostipv4addr;
    eOipv4port_t                    remotehostipv4port;
    eo_transceiver_sizes_t          sizes; 
    eov_mutex_fn_mutexderived_new   mutex_fn_new;    
    eOtransceiver_protection_t      transprotection;
    eOnvset_protection_t            nvsetprotection;
    eOconfman_cfg_t*                confmancfg;
} eOboardtransceiver_cfg_t;


/** @typedef    typedef struct EOtheBOARDtransceiver_hid EOtheBOARDtransceiver
    @brief      EOtheBOARDtransceiver is an opaque struct. It is used to implement data abstraction for the Parser  
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOtheBOARDtransceiver_hid EOtheBOARDtransceiver;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOboardtransceiver_cfg_t eo_boardtransceiver_cfg_default; 


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
 
/** @fn         extern EOtheBOARDtransceiver * eo_boardtransceiver_Initialise(void)
    @brief      Initialise the singleton EOtheBOARDtransceiver. 
    @param      cfg         Contains actions to be done on reception or transmission which are specific to the application.
                            If NULL, then  is is issued a info by the EOtheErrorManager.
    @return     A valid and not-NULL pointer to the EOtheBOARDtransceiver singleton.
 **/
extern EOtheBOARDtransceiver * eo_boardtransceiver_Initialise(const eOboardtransceiver_cfg_t *cfg);


/** @fn         extern EOtheBOARDtransceiver * eo_boardtransceiver_GetHandle(void)
    @brief      Gets the handle of the EOtheBOARDtransceiver singleton 
    @return     Constant pointer to the singleton.
 **/
extern EOtheBOARDtransceiver * eo_boardtransceiver_GetHandle(void);

extern EOtransceiver * eo_boardtransceiver_GetTransceiver(EOtheBOARDtransceiver* p);

extern EOnvSet * eo_boardtransceiver_GetNVset(EOtheBOARDtransceiver* p);

extern eOnvBRD_t eo_boardtransceiver_GetBoardNumber(EOtheBOARDtransceiver* p);


/** @}            
    end of group eo_ecvrevrebvtr2342r4  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



