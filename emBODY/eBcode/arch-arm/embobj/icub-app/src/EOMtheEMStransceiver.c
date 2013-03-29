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
#include "stdio.h"
#include "string.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"

#include "EOtheBOARDtransceiver_hid.h"

#include "EOtransceiver.h"

#include "EOtransceiver_hid.h" // used only for debug
#include "EOtransmitter_hid.h" // used only for debug
#include "EOreceiver_hid.h"    // used only for debug

#include "EOMmutex.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMStransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMStransceiver_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

//  DEBUG

extern EOMtheEMStransceiverDEBUG_t eom_emstransceiver_hid_DEBUG =
{
    .rxinvalidropframes                     = 0,
    .errorsinsequencenumber                 = 0,
    .lostreplies                            = 0,
    .failuresinloadofreplyropframe          = 0,
    .txropframeistoobigforthepacket         = 0,
    .cannotloadropinregulars                = 0,
    .cannotloadropinoccasionals             = 0    
};

const eOemstransceiver_cfg_t eom_emstransceiver_DefaultCfg = 
{
    EO_INIT(.vectorof_endpoint_cfg)     NULL,
    EO_INIT(.hashfunction_ep2index)     NULL,
    EO_INIT(.hostipv4addr)              EO_COMMON_IPV4ADDR(10, 0, 1, 200), 
    EO_INIT(.hostipv4port)              12345,
    EO_INIT(.sizes)
    {
        EO_INIT(.capacityofpacket)                  1024,
        EO_INIT(.capacityofrop)                     256,
        EO_INIT(.capacityofropframeregulars)        768,
        EO_INIT(.capacityofropframeoccasionals)     128,
        EO_INIT(.capacityofropframereplies)         128,
        EO_INIT(.maxnumberofregularrops)            32        
    },
    EO_INIT(.transprotection)           eo_trans_protection_none,
    EO_INIT(.nvscfgprotection)          eo_nvscfg_protection_none
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_emstransceiver_synch_DEBUG(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOMtheEMStransceiver";

 
static EOMtheEMStransceiver s_emstransceiver_singleton = 
{
    EO_INIT(.transceiver)                   NULL
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMStransceiver * eom_emstransceiver_Initialise(const eOemstransceiver_cfg_t *cfg)
{
    if(NULL != s_emstransceiver_singleton.transceiver)
    {
        return(&s_emstransceiver_singleton);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emstransceiver_DefaultCfg;
    }
    
    eOboardtransceiver_cfg_t brdtransceiver_cfg;
    
    brdtransceiver_cfg.vectorof_endpoint_cfg    = cfg->vectorof_endpoint_cfg;
    brdtransceiver_cfg.hashfunction_ep2index    = cfg->hashfunction_ep2index;   
    brdtransceiver_cfg.remotehostipv4addr       = cfg->hostipv4addr;
    brdtransceiver_cfg.remotehostipv4port       = cfg->hostipv4port; // it is the remote port where to send packets to
    memcpy(&brdtransceiver_cfg.sizes, &cfg->sizes, sizeof(eo_transceiver_sizes_t));
    brdtransceiver_cfg.mutex_fn_new             = (eov_mutex_fn_mutexderived_new)eom_mutex_New;
    brdtransceiver_cfg.transprotection          = cfg->transprotection;
    brdtransceiver_cfg.nvscfgprotection         = cfg->nvscfgprotection;
   
 
    //s_emstransceiver_singleton.transceiver = eo_boardtransceiver_Initialise(eom_emstransceiver_hid_userdef_get_cfg(cfg));
    s_emstransceiver_singleton.transceiver = eo_boardtransceiver_Initialise(&brdtransceiver_cfg);
    
    s_eom_emstransceiver_synch_DEBUG();
    
    char str[96];
    uint8_t *ipaddr = (uint8_t*) &cfg->hostipv4addr;
    snprintf(str, sizeof(str)-1, "initted with host: IP = %d.%d.%d.%d, port = %d\n\r", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3], cfg->hostipv4port);
    eo_errman_Info(eo_errman_GetHandle(), s_eobj_ownname, str);    
    
    return(&s_emstransceiver_singleton);
}


extern EOMtheEMStransceiver* eom_emstransceiver_GetHandle(void) 
{
    if(NULL != s_emstransceiver_singleton.transceiver)
    {
        return(&s_emstransceiver_singleton);
    }
    else
    {
        return(NULL);
    }
}

extern EOtransceiver* eom_emstransceiver_GetTransceiver(EOMtheEMStransceiver* p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(s_emstransceiver_singleton.transceiver);
}

extern EOnvsCfg* eom_emstransceiver_GetNVScfg(EOMtheEMStransceiver* p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(eo_boardtransceiver_hid_GetNvsCfg());
}



extern eOresult_t eom_emstransceiver_Parse(EOMtheEMStransceiver* p, EOpacket* rxpkt, uint16_t *numberofrops, eOabstime_t* txtime)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == rxpkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    res = eo_transceiver_Receive(s_emstransceiver_singleton.transceiver, rxpkt, numberofrops, txtime);
    
    s_eom_emstransceiver_synch_DEBUG();
    
    return(res);
}


extern eOresult_t eom_emstransceiver_Form(EOMtheEMStransceiver* p, EOpacket** txpkt, uint16_t *numberofrops)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == txpkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    res = eo_transceiver_Transmit(s_emstransceiver_singleton.transceiver, txpkt, numberofrops);
    
    s_eom_emstransceiver_synch_DEBUG();
    
    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_emstransceiver_synch_DEBUG(void)
{
    // retrieve the DEBUG variables of transceiver, transmitter, receiver and put them inside here.
    EOtransceiverDEBUG_t* transceiverDBG = &s_emstransceiver_singleton.transceiver->DEBUG;
    EOtransmitterDEBUG_t* transmitterDBG = &s_emstransceiver_singleton.transceiver->transmitter->DEBUG;
    EOreceiverDEBUG_t*    receiverDBG    = &s_emstransceiver_singleton.transceiver->receiver->DEBUG;
    
    // update the extern variable used for debug ...
    eom_emstransceiver_hid_DEBUG.rxinvalidropframes             = receiverDBG->rxinvalidropframes;
    eom_emstransceiver_hid_DEBUG.errorsinsequencenumber         = receiverDBG->errorsinsequencenumber;
    eom_emstransceiver_hid_DEBUG.lostreplies                    = receiverDBG->lostreplies;
    eom_emstransceiver_hid_DEBUG.failuresinloadofreplyropframe  = transceiverDBG->failuresinloadofreplyropframe;
    eom_emstransceiver_hid_DEBUG.txropframeistoobigforthepacket = transmitterDBG->txropframeistoobigforthepacket;   
    eom_emstransceiver_hid_DEBUG.cannotloadropinregulars        = transceiverDBG->cannotloadropinregulars;
    eom_emstransceiver_hid_DEBUG.cannotloadropinoccasionals     = transceiverDBG->cannotloadropinoccasionals;
    
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





