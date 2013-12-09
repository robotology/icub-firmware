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
#ifndef _EONVSET_H_
#define _EONVSET_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOnvSet.h
    @brief      This header file implements public interface to a collector of EOnv objects.
    @author     marco.accame@iit.it
    @date       06/07/2013
**/

/** @defgroup eo_nvset EOnvSet
    cefcewqfcew
      
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"
#include "EOconstvector.h"
#include "EOVmutex.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types -------------------------------------------------------------------------    


/** @typedef    typedef const struct EOnvSet_hid EOnvSet
    @brief      EOaction is an opaque struct. It is used to implement data abstraction for the datagram 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOnvSet_hid EOnvSet;


typedef enum
{
    eo_nvset_ownership_local       = 0,
    eo_nvset_ownership_remote      = 1
} eOnvsetOwnership_t; 


/** @typedef    typedef struct eOnvset_DEVcfg_t
    @brief      It contains the configuration of the managed NVs in a device, organised by endpoints 
 **/  
typedef struct
{
    eOnvBRD_t               boardnum;
    uint8_t                 dummy[3];
    eOres_fp_uint8_t        fptr_device_initialise; /*< used to initialise whatever is needed before using the eOnvset_EPcfg_t. for instance the number of entities in the board */
    const EOconstvector*    vectorof_epcfg;         /*< a const vector of eOnvset_EPcfg_t items, of size equal to the number of managed endpoints */
    eOuint16_fp_uint8_t     fptr_ep2indexofepcfg;   /*< a function which maps a given ep to and index inside @e vectorof_epcfg. It returns EOK_uint16dummy if the ep is not managed */
} eOnvset_DEVcfg_t;


typedef struct
{
    eOres_fp_uint8_uint8_voidp_uint16_t loadram;            /*< a function which loads the ram of the endpoint given: (brd, ep, ram, sizeof) */
    eOuint16_fp_uint8_uint8_t           getvarsnumberof;    /*< a function which returns the total number of variables given: (brd, ep) */
    eObool_fp_uint8_uint32_t            isidsupported;      /*< a function which tells if the id is supported given: (brd, id) */
    eOuint32_fp_uint8_uint8_uint32_t    getid;              /*< a function which returns the full ID given: (brd, ep, prognumber)  */
    eOuint32_fp_uint8_uint32_t          getprognumber;      /*< a function which returns a progressive number given: (brd, id) */
    eOvoidp_fp_uint8_uint32_t           getrom;             /*< a function which returns the .rom part of the NV given: (brd, id) */
    eOvoidp_fp_uint8_uint32_t           getram; 
    eObool_fp_uint8_uint32_t            isvarcached;        /*< a function which tells if the var is cached given: (brd, id) */    
} eOnvset_protocol_Interface_t;


/** @typedef    typedef struct eOnvset_EPcfg_t
    @brief      It contains the configuration of the managed NVs inside an endpoint. 
 **/ 
typedef struct                     
{
    eOnvEP8_t                           endpoint;                   /*< the endpoint value */
    uint8_t                             dummy;
    uint16_t                            epram_sizeof;               /*< the size of the ram used for the NVs in the endpoint */
    eOvoid_fp_uint32_voidp_t            fptr_ram_initialise;        /*< a function which initialises the ram (ram) */
    eOnvset_protocol_Interface_t*       protif;                     /* functions used to interface with the protocol library */
} eOnvset_EPcfg_t;


/** @typedef    typedef enum eOnvset_protection_t
    @brief      It contains the protection to be used in the NV set. 
 **/ 
typedef enum
{
    eo_nvset_protection_none               = 0,    /**< we dont protect vs concurrent access at all */
    eo_nvset_protection_one_per_object     = 1,    /**< all the NVs in the object share the same mutex: reduced use of memory but minimum concurrency */
    eo_nvset_protection_one_per_device     = 2,    /**< all the NVs in a device inside the object share the same mutex */
    eo_nvset_protection_one_per_endpoint   = 3,    /**< all the NVs in an endpoint inside each device inside the object share the same mutex */
    eo_nvset_protection_one_per_netvar     = 4     /**< every NV has its own mutex: heavy use of memory but maximum concurrency */
} eOnvset_protection_t;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOnvSet* eo_nvset_New(uint16_t ndevices, eOnvset_protection_t prot, eov_mutex_fn_mutexderived_new mtxnew);

extern eOresult_t eo_nvset_DEVpushback(EOnvSet* p, uint16_t ondevindex, eOnvset_DEVcfg_t* cfgofdev, eOnvsetOwnership_t ownership, eOipv4addr_t ipaddress);

extern eOresult_t eo_nvset_NVSinitialise(EOnvSet* p);

extern eOresult_t eo_nvset_BRD_Get(EOnvSet* p, eOipv4addr_t ip, eOnvBRD_t* brd);

extern eOresult_t eo_nvset_NV_Get(EOnvSet* p, eOipv4addr_t ip, eOnvID32_t id32, EOnv* thenv);

extern void* eo_nvset_RAMofEndpoint_Get(EOnvSet* p, eOipv4addr_t ip, eOnvEP8_t ep8);

extern void* eo_nvset_RAMofEntity_Get(EOnvSet* p, eOipv4addr_t ip, eOnvEP8_t ep8, eOnvENT_t ent, uint8_t index);

extern void* eo_nvset_RAMofVariable_Get(EOnvSet* p, eOipv4addr_t ip, eOnvID32_t id32);


/** @}            
    end of group eo_nvset 
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



