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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "stdio.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOnv_hid.h"
#include "EOrop_hid.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOhostTransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOhostTransceiver_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static EOnvsCfg* s_eo_hosttransceiver_nvscfg_get(const eOhosttransceiver_cfg_t *cfg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOhostTransceiver";
 

const eOhosttransceiver_cfg_t eo_hosttransceiver_cfg_default = 
{
    EO_INIT(.vectorof_endpoint_cfg)     NULL,
    EO_INIT(.hashfunction_ep2index)     NULL,
    EO_INIT(.remoteboardipv4addr)       EO_COMMON_IPV4ADDR(10, 0, 1, 1), 
    EO_INIT(.remoteboardipv4port)       12345,
    EO_INIT(.sizes)
    {
        EO_INIT(.capacityoftxpacket)                EOK_HOSTTRANSCEIVER_capacityoftxpacket,
        EO_INIT(.capacityofrop)                     EOK_HOSTTRANSCEIVER_capacityofrop,
        EO_INIT(.capacityofropframeregulars)        EOK_HOSTTRANSCEIVER_capacityofropframeregulars,
        EO_INIT(.capacityofropframeoccasionals)     EOK_HOSTTRANSCEIVER_capacityofropframeoccasionals,
        EO_INIT(.capacityofropframereplies)         EOK_HOSTTRANSCEIVER_capacityofropframereplies,
        EO_INIT(.maxnumberofregularrops)            EOK_HOSTTRANSCEIVER_maxnumberofregularrops        
    },    
    EO_INIT(.mutex_fn_new)              NULL,
    EO_INIT(.transprotection)           eo_trans_protection_none,
    EO_INIT(.nvscfgprotection)          eo_nvscfg_protection_none

};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOhostTransceiver * eo_hosttransceiver_New(const eOhosttransceiver_cfg_t *cfg) 
{
    EOhostTransceiver* retptr = NULL;
    eo_transceiver_cfg_t txrxcfg = eo_transceiver_cfg_default;


    if(NULL == cfg)
    {
        cfg = &eo_hosttransceiver_cfg_default;
    }

    if(NULL == cfg->vectorof_endpoint_cfg)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "need a vector of endpoints");
    }
    
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOhostTransceiver), 1);

    // 1. init the proper transceiver cfg

    retptr->nvscfg = s_eo_hosttransceiver_nvscfg_get(cfg);
    

    txrxcfg.capacityoftxpacket              = cfg->sizes.capacityoftxpacket;
    txrxcfg.capacityofrop                   = cfg->sizes.capacityofrop;
    txrxcfg.capacityofropframeregulars      = cfg->sizes.capacityofropframeregulars;
    txrxcfg.capacityofropframeoccasionals   = cfg->sizes.capacityofropframeoccasionals;
    txrxcfg.capacityofropframereplies       = cfg->sizes.capacityofropframereplies;
    txrxcfg.maxnumberofregularrops          = cfg->sizes.maxnumberofregularrops;
    txrxcfg.remipv4addr                     = cfg->remoteboardipv4addr;
    txrxcfg.remipv4port                     = cfg->remoteboardipv4port;
    txrxcfg.nvscfg                          = retptr->nvscfg;
    txrxcfg.mutex_fn_new                    = cfg->mutex_fn_new;
    txrxcfg.protection                      = cfg->transprotection;
    
    
    retptr->transceiver = eo_transceiver_New(&txrxcfg);
    
    
    return(retptr);        
}    


extern EOtransceiver* eo_hosttransceiver_Transceiver(EOhostTransceiver *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(p->transceiver);
}

extern EOnvsCfg* eo_hosttransceiver_NVsCfg(EOhostTransceiver *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(p->nvscfg);
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static EOnvsCfg* s_eo_hosttransceiver_nvscfg_get(const eOhosttransceiver_cfg_t *cfg)
{
    const uint16_t ndevices         = 1;
    EOVstorageDerived * const stg   = NULL;
    const uint16_t ondevindex = 0;      // one device only, hence the index is 0

    EOnvsCfg* nvscfg;
    eOnvscfg_EP_t *epcfg;
    const EOconstvector* theepcfgs = NULL;
    uint16_t nendpoints = 0;
    uint16_t i;

    theepcfgs = cfg->vectorof_endpoint_cfg;

    // the HOSTtransceiver manages one device only, thus the first argument is 1
    // the HOSTtransceiver does not use any storage, thus the second argument is NULL.

    nvscfg = eo_nvscfg_New(ndevices, stg, cfg->nvscfgprotection, cfg->mutex_fn_new);

    nendpoints = eo_constvector_Size(theepcfgs);
    
    
    // i add the first and only device. it has remote ownership and ipaddress cfg->remoteboardipv4addr.
    eo_nvscfg_PushBackDevice(nvscfg, eo_nvscfg_ownership_remote, cfg->remoteboardipv4addr, cfg->hashfunction_ep2index, nendpoints);
    
    // add all the endpoints for the device
    for(i=0; i<nendpoints; i++)
    {
        epcfg = (eOnvscfg_EP_t*) eo_constvector_At(theepcfgs, i);        
        eo_nvscfg_ondevice_PushBackEP(nvscfg, ondevindex, epcfg);
    }
    
    eo_nvscfg_data_Initialise(nvscfg);

    return(nvscfg);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




