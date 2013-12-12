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
#ifndef _EOPROXY_H_
#define _EOPROXY_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOproxy.h
    @brief      This header file implements public interface to the parser singleton used for communication protocol
    @author     marco.accame@iit.it
    @date       12/10/2013
**/

/** @defgroup eo_proxy Object EOproxy
    The EOproxy is an object used to forward requests to a different device, typically on CAN.
      
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOrop.h"
#include "EOVmutex.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct EOProxy_hid EOproxy
    @brief      EOproxy is an opaque struct. It is used to implement data abstraction for the proxy  
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOproxy_hid EOproxy;

typedef enum 
{
    eoproxy_mode_disabled   = 0,
    eoproxy_mode_enabled    = 1   
} eOproxymode_t;

typedef struct
{
    eOproxymode_t                   mode;
    uint16_t                        capacityoflistofropdes;
    eOreltime_t                     replyroptimeout;            // no timeout if EOK_uint64dummy 
    eov_mutex_fn_mutexderived_new   mutex_fn_new;
    void*                           transceiver; // points to a EOtransceiver
} eOproxy_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
 
/** @fn         extern EOproxy* eo_proxy_New(const eOproxy_cfg_t *cfg)
    @brief      creates a EOproxy. 
    @param      cfg         the configuration.
    @return     A valid and not-NULL pointer to the EOproxy object. NULL only if cfg->mode is eoproxy_mode_disabled
 **/
extern EOproxy* eo_proxy_New(const eOproxy_cfg_t *cfg);

extern eOresult_t eo_proxy_ROP_Forward(EOproxy *p, EOrop* rop, EOrop* ropout);

extern eOresult_t eo_proxy_ReplyROP_Load(EOproxy *p, eOnvID32_t id32, void *data);

extern eOresult_t eo_proxy_Tick(EOproxy *p);





/** @}            
    end of group eo_proxy  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



