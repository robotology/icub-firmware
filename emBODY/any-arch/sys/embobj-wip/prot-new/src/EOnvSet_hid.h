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
#ifndef _EONVSET_HID_H_
#define _EONVSET_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOnvSet_hid.h
    @brief      This header file implements hidden interface to a to a collector of EOnv objects.
    @author     marco.accame@iit.it
    @date       06/07/2013
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv_hid.h"
#include "EOvector.h"
#include "EOconstvector.h"
#include "EOVmutex.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOnvSet.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    eObool_t                            initted;
    eOnvEP_t                            endpoint;
    uint16_t                            epnvs_numberof; 
    uint16_t                            epram_sizeof;
    void*                               epram_ram;
    void*                               epram_rem;
    eOvoid_fp_uint16_voidp_voidp_t      fptr_epram_initialise;  // ep, vol, rem
    eOvoid_fp_uint16_voidp_voidp_t      fptr_epram_retrieve; // ep, vol, rem
    eOuint16_fp_uint16_uint16_t         fptr_epid2nvprogressivenumber;
    eOvoidp_fp_voidp_uint16_uint16_t    fptr_dataepid2nvram;    
    eOvoidp_fp_uint16_uint16_t          fptr_epid2nvrom; 
    EOVmutexDerived*                    mtx_endpoint;    
    EOvector*                           themtxofthenvs;    
} eOnvset_ep_t;


typedef struct
{
    eOipv4addr_t                    ipaddress;
    EOvector*                       theendpoints;
    eOnvsetOwnership_t              ownership;
    uint16_t                        theendpoints_numberof; 
    eOuint16_fp_uint16_t            fptr_ep2index;   
    EOVmutexDerived*                mtx_device;      
} eOnvset_dev_t;



typedef enum
{
    eo_nvset_devicesownership_none                  = 0,
    eo_nvset_devicesownership_onelocal              = 1,
    eo_nvset_devicesownership_onelocalsomeremote    = 2,
    eo_nvset_devicesownership_someremote            = 3
} eOnvsetDevicesOwnership_t;


/** @struct     EOnvSet_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/
struct EOnvSet_hid 
{
    EOvector*                       thedevices;
    EOvector*                       theipaddresses;
    uint16_t                        devindexoflocaldevice;
    eOnvsetDevicesOwnership_t       devicesowneship;
    eOnvset_protection_t            protection;
    eov_mutex_fn_mutexderived_new   mtxderived_new;
    EOVmutexDerived*                mtx_object;
};   
 




// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern uint16_t eo_nvset_hid_NVprogressivenumber(EOnvSet* p, eOipv4addr_t ip, eOnvEP_t ep, eOnvID_t id);



 

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




