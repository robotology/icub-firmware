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
#ifndef _EONVSCFG_H_
#define _EONVSCFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOnvsCfg.h
    @brief      This header file implements public interface to the configuration of netvars.
    @author     marco.accame@iit.it
    @date       09/06/2011
**/

/** @defgroup eo_n Objevct EOnvsCfg
    Offers methods for reading and writing some data on RAM, ROM and storage and for doing some
    actions upon such operations. It is to be manipulated only via its public methods.
    It is not created but loaded as a constant object from a configuration mapped in ROM
      
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"
#include "EOconstvector.h"
#include "EOtreenode.h"
#include "EOVmutex.h"
#include "EOVstorage.h"


// - public #define  --------------------------------------------------------------------------------------------------





  

// - declaration of public user-defined types -------------------------------------------------------------------------    


/** @typedef    typedef const struct EOnvsCfg_hid EOnvsCfg
    @brief      EOaction is an opaque struct. It is used to implement data abstraction for the datagram 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOnvsCfg_hid EOnvsCfg;


typedef enum
{
    eo_nvscfg_ownership_local       = 0,
    eo_nvscfg_ownership_remote      = 1
} eOnvscfgOwnership_t; 

typedef enum
{
    eo_nvscfg_devicesownership_none                  = 0,
    eo_nvscfg_devicesownership_onelocal              = 1,
    eo_nvscfg_devicesownership_onelocalsomeremote    = 2,
    eo_nvscfg_devicesownership_someremote            = 3
} eOnvscfgDevicesOwnership_t;

typedef struct
{
    const EOconstvector*    vectorof_endpoint_cfg;
    eOuint16_fp_uint16_t    hashfunction_ep2index;
} eOnvscfg_setof_EPs_t;

typedef struct                      // size is 24 bytes
{
    eOnvEP_t                        endpoint;
    uint16_t                        sizeof_endpoint_data;
    eOuint16_fp_uint16_t            hashfunction_id2index;
    const EOconstvector* const      constvector_of_treenodes_EOnv_con;
    const EOconstvector* const      constvector_of_EOnv_usr;
    eOvoid_fp_uint16_voidp_voidp_t  endpoint_data_init;
    eOvoid_fp_uint16_voidp_voidp_t  endpoint_data_retrieve;
} eOnvscfg_EP_t;

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------





// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOnvsCfg* eo_nvscfg_New(uint16_t ndevices, EOVstorageDerived* stg);

extern eOresult_t eo_nvscfg_PushBackDevice(EOnvsCfg* p, eOnvscfgOwnership_t ownership, eOipv4addr_t ipaddress, eOuint16_fp_uint16_t hashfn_ep2index, uint16_t nendpoints);


extern uint16_t eo_nvscfg_GetIndexOfLocalDevice(EOnvsCfg* p);

//extern eOresult_t eo_nvscfg_ondevice_PushBackEndpoint(EOnvsCfg* p, uint16_t ondevindex, eOnvEP_t endpoint, eOuint16_fp_uint16_t hashfn_id2index, const EOconstvector* treeofnvs_con, const EOconstvector* datanvs_usr, uint32_t datanvs_size, eOvoid_fp_uint16_voidp_voidp_t datanvs_init, EOVmutexDerived* mtx);

extern eOresult_t eo_nvscfg_ondevice_PushBackEP(EOnvsCfg* p, uint16_t ondevindex, eOnvscfg_EP_t *cfgofep, EOVmutexDerived* mtx);


extern eOresult_t eo_nvscfg_data_Initialise(EOnvsCfg* p);

// removed. it was useful to retrieve ram of endpoin, but much better using the variable instead
//extern void * eo_nvscfg_localdev_endpoint_GetRAM(EOnvsCfg* p, eOnvEP_t endpoint);


extern eOresult_t eo_nvscfg_GetIndices(EOnvsCfg* p, 
                                        eOipv4addr_t ip, eOnvEP_t ep, eOnvID_t id, 
                                        uint16_t *ipindex, uint16_t *epindex, uint16_t *idindex); 


extern EOtreenode* eo_nvscfg_GetTreeNode(EOnvsCfg* p, uint16_t ondevindex, uint16_t onendpointindex, uint16_t onidindex);


extern EOnv* eo_nvscfg_GetNV(EOnvsCfg* p, uint16_t ondevindex, uint16_t onendpointindex, uint16_t onidindex, EOtreenode* treenode, EOnv* nvtarget);



/** @}            
    end of group eo_nv 
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

