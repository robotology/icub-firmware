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


//#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"

#include "EOtheBOARDtransceiver_hid.h"

#include "EOtransceiver.h"

#include "EOtransceiver_hid.h" // used only for debug
#include "EOtransmitter_hid.h" // used only for debug
#include "EOreceiver_hid.h"    // used only for debug

#include "EOMmutex.h"

#include "OPCprotocolManager_Cfg.h"

#include "EOtheInfoDispatcher.h"

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



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


const eOemstransceiver_cfg_t eom_emstransceiver_DefaultCfg = 
{
    EO_INIT(.nvsetbrdcfg)               NULL,  
    EO_INIT(.hostipv4addr)              EO_COMMON_IPV4ADDR(10, 0, 1, 200), 
    EO_INIT(.hostipv4port)              12345,
    EO_INIT(.sizes)
    {
        EO_INIT(.capacityoftxpacket)                1024,
        EO_INIT(.capacityofrop)                     256,
        EO_INIT(.capacityofropframeregulars)        768,
        EO_INIT(.capacityofropframeoccasionals)     128,
        EO_INIT(.capacityofropframereplies)         128,
        EO_INIT(.maxnumberofregularrops)            32     
    },
    EO_INIT(.transprotection)           eo_trans_protection_none,
    EO_INIT(.nvsetprotection)           eo_nvset_protection_none,
    EO_INIT(.proxycfg)
    {
		EO_INIT(.mode)            					eoproxy_mode_enabled,
        EO_INIT(.capacityoflistofropdes)            8,
        EO_INIT(.replyroptimeout)                   10*1000,
        EO_INIT(.mutex_fn_new)                      NULL,
        EO_INIT(.transceiver)                       NULL
    }   
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_emstransceiver_update_diagnosticsinfo(void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOMtheEMStransceiver";

 
static EOMtheEMStransceiver s_emstransceiver_singleton = 
{
    EO_INIT(.transceiver)                   NULL
};


static eOemstransceiver_diagnosticsinfo_t s_eom_emstransceiver_diagnosticsinfo =
{
    .rxinvalidropframes                     = 0,
    .rxseqnumwrong                          = 0,
    .lostreplies                            = 0,
    .failuresinloadofreplyropframe          = 0,
    .txropframeistoobigforthepacket         = 0,
    .cannotloadropinregulars                = 0,
    .cannotloadropinoccasionals             = 0    
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
    

    brdtransceiver_cfg.nvsetbrdcfg              = cfg->nvsetbrdcfg;

    brdtransceiver_cfg.remotehostipv4addr       = cfg->hostipv4addr;
    brdtransceiver_cfg.remotehostipv4port       = cfg->hostipv4port; // it is the remote port where to send packets to
    memcpy(&brdtransceiver_cfg.sizes, &cfg->sizes, sizeof(eOtransceiver_sizes_t));
    brdtransceiver_cfg.mutex_fn_new             = (eov_mutex_fn_mutexderived_new)eom_mutex_New;
    brdtransceiver_cfg.transprotection          = cfg->transprotection;    
    brdtransceiver_cfg.nvsetprotection          = cfg->nvsetprotection;
    brdtransceiver_cfg.proxycfg                 = (eOproxy_cfg_t*)&cfg->proxycfg;
    memset(&brdtransceiver_cfg.extfn, 0, sizeof(eOtransceiver_extfn_t));
    brdtransceiver_cfg.extfn.onerrorseqnumber   = eom_emstransceiver_callback_incaseoferror_in_sequencenumberReceived;
    brdtransceiver_cfg.extfn.onerrorinvalidframe = eom_emstransceiver_callback_incaseoferror_invalidframe;
    eo_boardtransceiver_Initialise(&brdtransceiver_cfg);
    s_emstransceiver_singleton.transceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle());
    
    s_eom_emstransceiver_update_diagnosticsinfo();
    
   
    //#warning --> marco.accame: so far the dispatcher uses capacity = 16. maybe we can increase the number.
    eOinfodispatcher_cfg_t config = {0};
    config.capacity     = 24; // 16 or or eo_sizecntnr_dynamic ....
    config.transmitter  = eo_transceiver_GetTransmitter(eom_emstransceiver_GetTransceiver(eom_emstransceiver_GetHandle()));
    eo_infodispatcher_Initialise(&config);    
    
    
    char str[eomn_info_status_extra_sizeof];
    uint8_t *ipaddr = (uint8_t*) &cfg->hostipv4addr;
    snprintf(str, sizeof(str), "EOMtheEMStransceiver <-> %d.%d.%d.%d, %d", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3], cfg->hostipv4port);
    eo_errman_Info(eo_errman_GetHandle(), str, s_eobj_ownname, &eo_errman_DescrRunningHappily);    
    
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


extern void eom_emstransceiver_DeInitialise(EOMtheEMStransceiver* p)
{
    if(NULL == p)
    {
        return;
    }
    
    if(NULL == p->transceiver)
    {
        return;
    }
    
    eo_infodispatcher_DeInitialise(eo_infodispatcher_GetHandle());
    
    eo_boardtransceiver_DeInitialise(eo_boardtransceiver_GetHandle());
    

    p->transceiver = NULL;
    
    return;
}


extern eOipv4addr_t eom_emstransceiver_GetIPhost(EOMtheEMStransceiver* p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(s_emstransceiver_singleton.transceiver->cfg.remipv4addr);    
}

extern EOtransceiver* eom_emstransceiver_GetTransceiver(EOMtheEMStransceiver* p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(s_emstransceiver_singleton.transceiver);
}

extern EOnvSet* eom_emstransceiver_GetNVset(EOMtheEMStransceiver* p)
{
    if(NULL == p)
    {
        return(NULL);
    }    
    return(eo_boardtransceiver_GetNVset(eo_boardtransceiver_GetHandle()));
}


extern eOresult_t eom_emstransceiver_Parse(EOMtheEMStransceiver* p, EOpacket* rxpkt, uint16_t *numberofrops, eOabstime_t* txtime)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == rxpkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    res = eo_transceiver_Receive(s_emstransceiver_singleton.transceiver, rxpkt, numberofrops, txtime);
    
    s_eom_emstransceiver_update_diagnosticsinfo();
    
    return(res);
}



extern eOresult_t eom_emstransceiver_NumberofOutROPs(EOMtheEMStransceiver *p, uint16_t *numberofreplies, uint16_t *numberofoccasionals, uint16_t *numberofregulars)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }  
    
    return(eo_transceiver_NumberofOutROPs(s_emstransceiver_singleton.transceiver, numberofreplies, numberofoccasionals, numberofregulars));        
}



extern eOresult_t eom_emstransceiver_Form(EOMtheEMStransceiver* p, EOpacket** txpkt, uint16_t *numberofrops, eOtransmitter_ropsnumber_t *ropsnum)
{
    eOresult_t res;
    uint16_t numofrops = 0;
    //static int nn = 0;
    
    if((NULL == p) || (NULL == txpkt))
    {
        return(eores_NOK_nullpointer);
    }

    
    // call the info-dispatcher so that it may insert sig<info> rops in occasional ropframe.
    eo_infodispatcher_Send(eo_infodispatcher_GetHandle(), eoinfodispatcher_sendnumber_all, NULL);
       
    res = eo_transceiver_outpacket_Prepare(s_emstransceiver_singleton.transceiver, &numofrops, ropsnum);
    if(eores_OK != res)
    {
        return(res);
    }
    
    if(NULL != numberofrops)
    {
        *numberofrops = numofrops;
    }
    
    // we now retrieve the out packet only if we have at least one rop to tx. it is the transmitter inside
    // the transceiver that decides which rops to tx. even if we call every ms we can tx only when needed
    // and we reduce tx rate.
    if(numofrops > 0)
    {
        res = eo_transceiver_outpacket_Get(s_emstransceiver_singleton.transceiver, txpkt);
    }
    
    s_eom_emstransceiver_update_diagnosticsinfo();
    
    return(res);
}


extern eOresult_t eom_emstransceiver_GetDiagnosticsInfo(EOMtheEMStransceiver* p, eOemstransceiver_diagnosticsinfo_t *dgn_ptr)
{
    if((NULL == p) || (NULL == dgn_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    memcpy(dgn_ptr, &s_eom_emstransceiver_diagnosticsinfo, sizeof(eOemstransceiver_diagnosticsinfo_t));
    
    return(eores_OK);
}

extern eOemstransceiver_diagnosticsinfo_t* eom_emstransceiver_GetDiagnosticsInfoHandle(EOMtheEMStransceiver* p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    s_eom_emstransceiver_update_diagnosticsinfo();
    return(&s_eom_emstransceiver_diagnosticsinfo);
    
}


EO_weak extern void eom_emstransceiver_callback_incaseoferror_in_sequencenumberReceived(EOreceiver *receiver)
{ 
    // nothing: if needed an action please redefine the function
}

EO_weak extern void eom_emstransceiver_callback_incaseoferror_invalidframe(EOreceiver *receiver)
{
    
}

extern void eo_receiver_callback_incaseoferror_in_sequencenumberReceived(eOipv4addr_t remipv4addr, uint64_t rec_seqnum, uint64_t exp_seqnum)
{
    // if any compilation error about a redefinition of eo_receiver_callback_incaseoferror_in_sequencenumberReceived() .... 
    // remove it from your code and redefine eom_emstransceiver_callback_incaseoferror_in_sequencenumberReceived() instead
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eom_emstransceiver_update_diagnosticsinfo(void)
{
    // retrieve the DEBUG variables of transceiver, transmitter, receiver and put them inside here.
    EOtransceiverDEBUG_t* transceiverDBG = &s_emstransceiver_singleton.transceiver->debug;
    EOtransmitterDEBUG_t* transmitterDBG = &s_emstransceiver_singleton.transceiver->transmitter->debug;
    EOreceiverDEBUG_t*    receiverDBG    = &s_emstransceiver_singleton.transceiver->receiver->debug;
    
    // update the extern variable used for debug ...
    s_eom_emstransceiver_diagnosticsinfo.rxinvalidropframes             = receiverDBG->rxinvalidropframes;
    s_eom_emstransceiver_diagnosticsinfo.rxseqnumwrong                  = receiverDBG->errorsinsequencenumber;
    s_eom_emstransceiver_diagnosticsinfo.lostreplies                    = receiverDBG->lostreplies;
    s_eom_emstransceiver_diagnosticsinfo.failuresinloadofreplyropframe  = transceiverDBG->failuresinloadofreplyropframe;
    s_eom_emstransceiver_diagnosticsinfo.txropframeistoobigforthepacket = transmitterDBG->txropframeistoobigforthepacket;   
    s_eom_emstransceiver_diagnosticsinfo.cannotloadropinregulars        = transceiverDBG->cannotloadropinregulars;
    s_eom_emstransceiver_diagnosticsinfo.cannotloadropinoccasionals     = transceiverDBG->cannotloadropinoccasionals;
  
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





