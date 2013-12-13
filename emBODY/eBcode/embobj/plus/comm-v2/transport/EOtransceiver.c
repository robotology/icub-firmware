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
#include "EOtheMemoryPool.h"
#include "EOtheParser.h"
#include "EOtheFormer.h"
#include "EOropframe_hid.h"
#include "EOnv_hid.h"
#include "EOrop_hid.h"

#include "EOVmutex.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtransceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtransceiver_hid.h" 


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
// empty-section    


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOtransceiver";

const eOtransceiver_cfg_t eo_transceiver_cfg_default = 
{
    EO_INIT(.sizes)
    {
        EO_INIT(.capacityoftxpacket)            512, 
        EO_INIT(.capacityofrop)                 128, 
        EO_INIT(.capacityofropframeregulars)    256,
        EO_INIT(.capacityofropframeoccasionals) 128,
        EO_INIT(.capacityofropframereplies)     128, 
        EO_INIT(.maxnumberofregularrops)        16
    },    
    EO_INIT(.remipv4addr)                   EO_COMMON_IPV4ADDR_LOCALHOST,
    EO_INIT(.remipv4port)                   10001,
    EO_INIT(.nvset)                         NULL,
    EO_INIT(.confmancfg)                    NULL,
    EO_INIT(.proxycfg)                      NULL,
    EO_INIT(.mutex_fn_new)                  NULL,
    EO_INIT(.protection)                    eo_trans_protection_none
};





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOtransceiver* eo_transceiver_New(const eOtransceiver_cfg_t *cfg)
{
    EOtransceiver *retptr = NULL;  
    eOreceiver_cfg_t rec_cfg;
    eOtransmitter_cfg_t tra_cfg;


    if(NULL == cfg)
    {    
        cfg = &eo_transceiver_cfg_default;
    }
    
    
    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOtransceiver), 1);
    
    // save the config
    
    memcpy(&retptr->cfg, cfg, sizeof(eOtransceiver_cfg_t)); 
    
    
    // create the conf manager  
    
    if((NULL != cfg->confmancfg) && (eoconfman_mode_disabled != cfg->confmancfg->mode))
    {
        eOconfman_cfg_t confmancfg;
        memcpy(&confmancfg, cfg->confmancfg, sizeof(eOconfman_cfg_t));
        confmancfg.mutex_fn_new = (eo_trans_protection_enabled == cfg->protection) ? (cfg->mutex_fn_new) : (NULL);
        retptr->confmanager = eo_confman_New(&confmancfg);
    }
    else
    {
        retptr->confmanager = NULL;
    }
    
    
    // create the proxy
    
    if((NULL != cfg->proxycfg) && (eoproxy_mode_disabled != cfg->proxycfg->mode))
    {
        eOproxy_cfg_t proxycfg;
        memcpy(&proxycfg, cfg->proxycfg, sizeof(eOproxy_cfg_t));
        proxycfg.mutex_fn_new   = (eo_trans_protection_enabled == cfg->protection) ? (cfg->mutex_fn_new) : (NULL);
        proxycfg.transceiver    = retptr;        
        retptr->proxy           = eo_proxy_New(&proxycfg);        
    }        
    else
    {
        retptr->proxy = NULL;
    }


    // create the agent
    
    eOagent_cfg_t agentcfg;
    agentcfg.nvset      = cfg->nvset;
    agentcfg.proxy      = retptr->proxy;
    agentcfg.confman    = retptr->confmanager;
    
    retptr->agent = eo_agent_New(&agentcfg);               
 
    
    // create the receiver
    
    memcpy(&rec_cfg, &eo_receiver_cfg_default, sizeof(eOreceiver_cfg_t));
    rec_cfg.sizes.capacityofropframereply   = cfg->sizes.capacityofropframereplies;
    rec_cfg.sizes.capacityofropinput        = cfg->sizes.capacityofrop;
    rec_cfg.sizes.capacityofropreply        = cfg->sizes.capacityofrop;
    rec_cfg.agent                           = retptr->agent;

    retptr->receiver = eo_receiver_New(&rec_cfg);

    
    // create the transmitter
    
    memcpy(&tra_cfg, &eo_transmitter_cfg_default, sizeof(eOtransmitter_cfg_t));
    tra_cfg.sizes.capacityoftxpacket            = cfg->sizes.capacityoftxpacket;
    tra_cfg.sizes.capacityofropframeregulars    = cfg->sizes.capacityofropframeregulars;
    tra_cfg.sizes.capacityofropframeoccasionals = cfg->sizes.capacityofropframeoccasionals;
    tra_cfg.sizes.capacityofropframereplies     = cfg->sizes.capacityofropframereplies;
    tra_cfg.sizes.capacityofrop                 = cfg->sizes.capacityofrop;
    tra_cfg.sizes.maxnumberofregularrops        = cfg->sizes.maxnumberofregularrops;
    tra_cfg.ipv4addr                            = cfg->remipv4addr;     // it is the address of the remote host: we filter incoming packet with this address and sends packets only to it
    tra_cfg.ipv4port                            = cfg->remipv4port;     // it is the remote port where to send packets
    tra_cfg.agent                               = retptr->agent;
    tra_cfg.mutex_fn_new                        = cfg->mutex_fn_new;
    tra_cfg.protection                          = (eo_trans_protection_none == cfg->protection) ? (eo_transmitter_protection_none) : (eo_transmitter_protection_total);
    
    retptr->transmitter = eo_transmitter_New(&tra_cfg);
    
    
    // manage the debug info
    
#if defined(USE_DEBUG_EOTRANSCEIVER)    
    memset(&retptr->DEBUG, 0, sizeof(EOtransceiverDEBUG_t)); 
#endif
    
    return(retptr);
}


extern EOnvSet * eo_transceiver_GetNVset(EOtransceiver *p)
{    
    if(NULL == p)
    {
        return(NULL);
    }
         
    return(p->cfg.nvset);
}

extern EOproxy * eo_transceiver_GetProxy(EOtransceiver *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
         
    return(p->proxy);    
}


extern eOresult_t eo_transceiver_Receive(EOtransceiver *p, EOpacket *pkt, uint16_t *numberofrops, eOabstime_t* txtime)
{
    eObool_t thereisareply = eobool_false;  
    eOresult_t res;
    eOipv4addr_t remaddr;
    eOipv4port_t remport;
    
    if((NULL == p) || (NULL == pkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    // we tick the proxy to remove timed-out replies enqueued by EOreceiver and not yet
    // inserted in EOtransmitter with eo_transceiver_ReplyROP_Load() called by eo_proxy_ReplyROP_Load()
    // if p->proxy is NULL the following call does not harm
    eo_proxy_Tick(p->proxy);
    
    // remember: we process a packet only if the source ipaddress is the same as in p->cfg.remipv4addr. the source port can be any.
    eo_packet_Addressing_Get(pkt, &remaddr, &remport);
    if(remaddr != p->cfg.remipv4addr)
    {
        return(eores_NOK_generic);
    }
    
    if(eores_OK != (res = eo_receiver_Process(p->receiver, pkt, numberofrops, &thereisareply, txtime)))
    {
        return(res);
    }  

    if(eobool_true == thereisareply)
    {
        // must put the reply inside the transmitter
        EOropframe* ropframereply = NULL;
        
        // if in here, i am sure that there is a reply and that return value will be eores_OK
        res = eo_receiver_GetReply(p->receiver, &ropframereply);
        
        // i will transmit back a reply to the remote host at address p->cfg.remipv4addr and port p->cfg.remipv4port  ...
        // which are the ones assigned to the p->transmitter at its creation.
        
        res = eo_transmitter_reply_ropframe_Load(p->transmitter, ropframereply);      
        
#if defined(USE_DEBUG_EOTRANSCEIVER) 
        {   // DEBUG
            if(eores_OK != res)
            {
                p->DEBUG.failuresinloadofreplyropframe ++;            
            }
        }
#endif        

    }    
    
    return(res);
}



extern eOresult_t eo_transceiver_outpacket_Prepare(EOtransceiver *p, uint16_t *numberofrops)
{  
    eOresult_t res = eores_NOK_generic;
    
    if((NULL == p) || (NULL == numberofrops))
    {
        return(eores_NOK_nullpointer);
    }
    
    // refresh regulars ...    
    eo_transmitter_regular_rops_Refresh(p->transmitter);
    
    // finally retrieve the packet from the transmitter. it will be formed by replies, regulars, occasionals.
    res = eo_transmitter_outpacket_Prepare(p->transmitter, numberofrops);
    
    // we also need to tick the proxy to remove timed-out replies enqueued by EOreceiver and not yet
    // inserted in EOtransmitter with eo_transceiver_ReplyROP_Load() called by eo_proxy_ReplyROP_Load()
    // if p->proxy is NULL the following call does not harm
    eo_proxy_Tick(p->proxy);
       
    return(res);
}



extern eOresult_t eo_transceiver_outpacket_Get(EOtransceiver *p, EOpacket **pkt)
{    
    if(NULL == p) 
    {
        return(eores_NOK_nullpointer);
    }
    
    return(eo_transmitter_outpacket_Get(p->transmitter, pkt)); 
}


extern eOresult_t eo_transceiver_RegularROPs_Clear(EOtransceiver *p)
{
    eOresult_t res;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    res = eo_transmitter_regular_rops_Clear(p->transmitter);
    
    return(res);
}

extern eOresult_t eo_transceiver_RegularROP_Load(EOtransceiver *p, eOropdescriptor_t *ropdesc)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == ropdesc))
    {
        return(eores_NOK_nullpointer);
    }
    
    res = eo_transmitter_regular_rops_Load(p->transmitter, ropdesc);


#if defined(USE_DEBUG_EOTRANSCEIVER)     
    {   // DEBUG    
        if(eores_OK != res)
        {
            p->DEBUG.cannotloadropinregulars ++;
        }
    } 
#endif    
    
    return(res);
}

extern eOresult_t eo_transceiver_RegularROP_Unload(EOtransceiver *p, eOropdescriptor_t *ropdesc)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == ropdesc))
    {
        return(eores_NOK_nullpointer);
    }
    
    res = eo_transmitter_regular_rops_Unload(p->transmitter, ropdesc);
    
    return(res);
}


extern eOresult_t eo_transceiver_OccasionalROP_Load(EOtransceiver *p, eOropdescriptor_t *ropdesc)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == ropdesc))
    {
        return(eores_NOK_nullpointer);
    }
    

    res = eo_transmitter_occasional_rops_Load(p->transmitter, ropdesc);
 
#if defined(USE_DEBUG_EOTRANSCEIVER)  
    {   // DEBUG    
        if(eores_OK != res)
        {
            p->DEBUG.cannotloadropinoccasionals ++;
        }
    }   
#endif
    
    return(res);

}    


extern eOresult_t eo_transceiver_ReplyROP_Load(EOtransceiver *p, eOropdescriptor_t *ropdesc)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == ropdesc))
    {
        return(eores_NOK_nullpointer);
    }
    

    res = eo_transmitter_reply_rops_Load(p->transmitter, ropdesc);
 
#if defined(USE_DEBUG_EOTRANSCEIVER)  
    {   // DEBUG    
        if(eores_OK != res)
        {
            p->DEBUG.cannotloadropinreplies ++;
        }
    }   
#endif
    
    return(res);

}    

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




