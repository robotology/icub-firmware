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

static EOnvSet* s_eo_hosttransceiver_nvset_get(const eOhosttransceiver_cfg_t *cfg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOhostTransceiver";
 

const eOhosttransceiver_cfg_t eo_hosttransceiver_cfg_default = 
{
    EO_INIT(.nvsetdevcfg)               NULL,
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
    EO_INIT(.nvsetprotection)           eo_nvset_protection_none
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
    
    if(NULL == cfg->nvsetdevcfg)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "need a nv set cfg");
    }  
    
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOhostTransceiver), 1);

    // 1. init the proper transceiver cfg

    retptr->nvset = s_eo_hosttransceiver_nvset_get(cfg);
    

    txrxcfg.capacityoftxpacket              = cfg->sizes.capacityoftxpacket;
    txrxcfg.capacityofrop                   = cfg->sizes.capacityofrop;
    txrxcfg.capacityofropframeregulars      = cfg->sizes.capacityofropframeregulars;
    txrxcfg.capacityofropframeoccasionals   = cfg->sizes.capacityofropframeoccasionals;
    txrxcfg.capacityofropframereplies       = cfg->sizes.capacityofropframereplies;
    txrxcfg.maxnumberofregularrops          = cfg->sizes.maxnumberofregularrops;
    txrxcfg.remipv4addr                     = cfg->remoteboardipv4addr;
    txrxcfg.remipv4port                     = cfg->remoteboardipv4port;
    txrxcfg.nvset                           = retptr->nvset;
    txrxcfg.mutex_fn_new                    = cfg->mutex_fn_new;
    txrxcfg.protection                      = cfg->transprotection;
    
    
    retptr->transceiver = eo_transceiver_New(&txrxcfg);
    
    
    return(retptr);        
}    


extern EOtransceiver* eo_hosttransceiver_GetTransceiver(EOhostTransceiver *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(p->transceiver);
}


extern EOnvSet * eo_hosttransceiver_GetNVset(EOhostTransceiver *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(p->nvset);
}


extern eOnvBRD_t eo_hosttransceiver_GetBoardNumber(EOhostTransceiver* p)
{
    if(NULL == p)
    {
        return(eo_nv_BRDdummy);
    }
    
    return(p->boardnumber);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static EOnvSet* s_eo_hosttransceiver_nvset_get(const eOhosttransceiver_cfg_t *cfg)
{
    const uint16_t numofdevices     = 1;    // one device only
    const uint16_t ondevindexzero   = 0;    // one device only


    EOnvSet* nvset = eo_nvset_New(numofdevices, cfg->nvsetprotection, cfg->mutex_fn_new);
    
    //i add the first and only device. it has remote ownership and ipaddress cfg->remoteboardipv4addr.
    eo_nvset_DEVpushback(nvset, ondevindexzero, (eOnvset_DEVcfg_t*)cfg->nvsetdevcfg, eo_nvset_ownership_remote, cfg->remoteboardipv4addr);
    
    eo_nvset_NVSinitialise(nvset);

    return(nvset);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




