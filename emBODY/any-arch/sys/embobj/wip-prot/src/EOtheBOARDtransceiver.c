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

#include "EOtheBOARDtransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheBOARDtransceiver_hid.h" 


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

static EOnvSet* s_eo_boardtransceiver_nvset_get(const eOboardtransceiver_cfg_t *cfg);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOtheBOARDtransceiver";
 
static EOtheBOARDtransceiver s_eo_theboardtrans = 
{
    EO_INIT(.transceiver)               NULL,
    EO_INIT(.nvset)                     NULL,
    EO_INIT(.boardnumber)               0xff
};


const eOboardtransceiver_cfg_t eo_boardtransceiver_cfg_default = 
{
    EO_INIT(.nvsetdevcfg)               NULL,
    EO_INIT(.remotehostipv4addr)        0,
    EO_INIT(.remotehostipv4port)        0,
    EO_INIT(.sizes)                     {0},
    EO_INIT(.mutex_fn_new)              NULL,
    EO_INIT(.transprotection)           eo_trans_protection_none,
    EO_INIT(.nvsetprotection)           eo_nvset_protection_none
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOtheBOARDtransceiver * eo_boardtransceiver_Initialise(const eOboardtransceiver_cfg_t *cfg) 
{
    eo_transceiver_cfg_t txrxcfg = eo_transceiver_cfg_default;
    
    if(NULL != s_eo_theboardtrans.transceiver)
    {
        return(&s_eo_theboardtrans);
    }


    if(NULL == cfg)
    {
        cfg = &eo_boardtransceiver_cfg_default;
    }
    
    if(NULL == cfg->nvsetdevcfg)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "need a nv set cfg");
    }

    if((0 == cfg->sizes.capacityoftxpacket) || (0 == cfg->sizes.capacityofrop) || (0 == cfg->sizes.capacityofropframeregulars) ||
       (0 == cfg->sizes.capacityofropframeoccasionals) || (0 == cfg->sizes.capacityofropframereplies) || (0 == cfg->sizes.maxnumberofregularrops))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "a cfg->sizes field is 0");
    }    
    
    // 1. init the proper transceiver cfg

    s_eo_theboardtrans.nvset = s_eo_boardtransceiver_nvset_get(cfg);
    
    s_eo_theboardtrans.boardnumber = cfg->nvsetdevcfg->boardnum;
    

    txrxcfg.capacityoftxpacket              = cfg->sizes.capacityoftxpacket;
    txrxcfg.capacityofrop                   = cfg->sizes.capacityofrop;
    txrxcfg.capacityofropframeregulars      = cfg->sizes.capacityofropframeregulars;
    txrxcfg.capacityofropframeoccasionals   = cfg->sizes.capacityofropframeoccasionals;
    txrxcfg.capacityofropframereplies       = cfg->sizes.capacityofropframereplies;
    txrxcfg.maxnumberofregularrops          = cfg->sizes.maxnumberofregularrops;
    txrxcfg.remipv4addr                     = cfg->remotehostipv4addr;
    txrxcfg.remipv4port                     = cfg->remotehostipv4port;
    txrxcfg.nvset                           = s_eo_theboardtrans.nvset;
    txrxcfg.mutex_fn_new                    = cfg->mutex_fn_new;
    txrxcfg.protection                      = cfg->transprotection;
    
    s_eo_theboardtrans.transceiver = eo_transceiver_New(&txrxcfg);
    
    
    return(&s_eo_theboardtrans);        
}    


extern EOtheBOARDtransceiver * eo_boardtransceiver_GetHandle(void) 
{
    if(NULL != s_eo_theboardtrans.transceiver)
    {
        return(&s_eo_theboardtrans);
    }
    else
    {    
        return(NULL);
    }
}

extern EOtransceiver * eo_boardtransceiver_GetTransceiver(EOtheBOARDtransceiver* p)
{
    if(NULL != p)
    {
        return(s_eo_theboardtrans.transceiver);
    }
    else
    {    
        return(NULL);
    }     
}

extern EOnvSet * eo_boardtransceiver_GetNVset(EOtheBOARDtransceiver* p)
{
    if(NULL != p)
    {
        return(s_eo_theboardtrans.nvset);
    }
    else
    {    
        return(NULL);
    }        
}

extern eOnvBRD_t eo_boardtransceiver_GetBoardNumber(EOtheBOARDtransceiver* p)
{
    if(NULL != p)
    {
        return(s_eo_theboardtrans.boardnumber);
    }
    else
    {    
        return(eo_nv_BRDdummy);
    }          
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// extern EOnvSet * eo_boardtransceiver_hid_GetNVset(void)
// {
//     return(s_eo_theboardtrans.nvset);
// }

// extern eOnvBRD_t eo_boardtransceiver_hid_GetBoardNumber(void)
// {
//     return(s_eo_theboardtrans.boardnumber);
// }

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static EOnvSet* s_eo_boardtransceiver_nvset_get(const eOboardtransceiver_cfg_t *cfg)
{
#if 1
    const uint16_t numofdevices     = 1;    // one device only
    const uint16_t ondevindexzero   = 0;    // one device only

    if(NULL != s_eo_theboardtrans.nvset)
    {   // if i call it more than once ... then i return the configuration but allocate and init only once 
        return(s_eo_theboardtrans.nvset);
    }

    EOnvSet* nvset = eo_nvset_New(numofdevices, cfg->nvsetprotection, cfg->mutex_fn_new);
    
    eo_nvset_DEVpushback(nvset, ondevindexzero, (eOnvset_DEVcfg_t*)cfg->nvsetdevcfg, eo_nvset_ownership_local, EO_COMMON_IPV4ADDR_LOCALHOST);
    
    eo_nvset_NVSinitialise(nvset);

    return(nvset);

#else    
    EOnvSet* nvset = s_eo_theboardtrans.nvset;
    eOnvset_EPcfg_t *epcfg;
    const EOconstvector* theepcfgs = NULL;
    uint16_t nendpoints = 0;
    uint16_t i;
    
    const uint16_t numofdevices = 1;    // one device only
    const uint16_t ondevindex = 0;      // hence only index 0
    
    if(NULL != nvset)
    {
        // if i call it more than once ... then i return the configuration but allocate and init only once 
        return(nvset);
    }

 
    theepcfgs = cfg->vectorof_endpoint_cfg;
    

//    #warning --> so far the BOARDtransceiver does not use any storage. if needed ... change the NULL into a ...
    nvset = eo_nvset_New(numofdevices, cfg->nvsetprotection, cfg->mutex_fn_new);
    
    
    //foreach<device>
    // begin

    nendpoints = eo_constvector_Size(theepcfgs);
    
    // use local-host address as we dont need to put the actual address of board in nvset.
    eo_nvset_PushBackDEV(nvset, eo_nvset_ownership_local, EO_COMMON_IPV4ADDR_LOCALHOST, cfg->hashfunction_ep2index, nendpoints);
    
    for(i=0; i<nendpoints; i++)
    {
        epcfg = (eOnvset_EPcfg_t*) eo_constvector_At(theepcfgs, i);
        eo_nvset_onDEV_PushBackEP(nvset, ondevindex, epcfg);         
    }
    
    //end
    
    eo_nvset_InitialiseNVs(nvset);

    return(nvset);
#endif    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




