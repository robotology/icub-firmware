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
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



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

const eo_transceiver_cfg_t eo_transceiver_cfg_default = 
{
    EO_INIT(.capacityofpacket)              512, 
    EO_INIT(.capacityofrop)                 128, 
    EO_INIT(.capacityofropframeregulars)    256,
    EO_INIT(.capacityofropframeoccasionals) 128,
    EO_INIT(.capacityofropframereplies)     128, 
    EO_INIT(.maxnumberofregularrops)        16,
    EO_INIT(.remipv4addr)                   EO_COMMON_IPV4ADDR_LOCALHOST,
    EO_INIT(.remipv4port)                   10001,
    EO_INIT(.nvscfg)                        NULL,
    EO_INIT(.mutex_fn_new)                  NULL,
    EO_INIT(.protection)                    eo_trans_protection_none
};




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOtransceiver* eo_transceiver_New(const eo_transceiver_cfg_t *cfg)
{
    EOtransceiver *retptr = NULL;  
    eo_receiver_cfg_t rec_cfg;
    eo_transmitter_cfg_t tra_cfg;


    if(NULL == cfg)
    {    
        cfg = &eo_transceiver_cfg_default;
    }
    
    memcpy(&rec_cfg, &eo_receiver_cfg_default, sizeof(eo_receiver_cfg_t));
    rec_cfg.capacityofropframereply         = cfg->capacityofropframereplies;
    rec_cfg.capacityofropinput              = cfg->capacityofrop;
    rec_cfg.capacityofropreply              = cfg->capacityofrop;
    rec_cfg.nvscfg                          = cfg->nvscfg;

    
    memcpy(&tra_cfg, &eo_transmitter_cfg_default, sizeof(eo_transmitter_cfg_t));
    tra_cfg.capacityoftxpacket              = cfg->capacityofpacket;
    tra_cfg.capacityofropframeregulars      = cfg->capacityofropframeregulars;
    tra_cfg.capacityofropframeoccasionals   = cfg->capacityofropframeoccasionals;
    tra_cfg.capacityofropframereplies       = cfg->capacityofropframereplies;
    tra_cfg.capacityofrop                   = cfg->capacityofrop;
    tra_cfg.maxnumberofregularrops          = cfg->maxnumberofregularrops;
    tra_cfg.ipv4addr                        = cfg->remipv4addr;     // it is the address of the remote host: we filter incoming packet with this address and sends packets only to it
    tra_cfg.ipv4port                        = cfg->remipv4port;     // it is the remote port where to send packets
    tra_cfg.nvscfg                          = cfg->nvscfg;
    tra_cfg.mutex_fn_new                    = cfg->mutex_fn_new;
    tra_cfg.protection                      = (eo_trans_protection_none == cfg->protection) ? (eo_transmitter_protection_none) : (eo_transmitter_protection_total);
    
    
    
    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOtransceiver), 1);
    
    
    memcpy(&retptr->cfg, cfg, sizeof(eo_transceiver_cfg_t)); 
    
    retptr->receiver = eo_receiver_New(&rec_cfg);
    
    retptr->transmitter = eo_transmitter_New(&tra_cfg);
    
#if defined(USE_DEBUG_EOTRANSCEIVER)    
    memset(&retptr->DEBUG, 0, sizeof(EOtransceiverDEBUG_t)); 
#endif

//     if((eo_trans_protection_enabled == cfg->protection) && (NULL != cfg->mutex_fn_new))
//     {
//         retptr->mtx_tx_replies      = cfg->mutex_fn_new();
//         retptr->mtx_tx_regulars     = cfg->mutex_fn_new();
//         retptr->mtx_tx_occasionals  = cfg->mutex_fn_new();
//     }
    
    return(retptr);
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
    
    // remember: we process a packet only if the source ipaddress is the same as in p->cfg.remipv4addr. the source port can be any.
    eo_packet_Addressing_Get(pkt, &remaddr, &remport);
    if(remaddr != p->cfg.remipv4addr)
    {
        return(eores_NOK_generic);
    }
    
    if(eores_OK != (res = eo_receiver_Process(p->receiver, pkt, p->cfg.nvscfg, numberofrops, &thereisareply, txtime)))
    {
        return(res);
    }  

    if(eobool_true == thereisareply)
    {
        // must put the reply inside the transmitter
        EOropframe* ropframereply = NULL;
//        eOipv4addr_t toipv4addr;
//        eOipv4port_t toipv4port;
        
        // if in here, i am sure that there is a reply and that return value will be eores_OK
        res = eo_receiver_GetReply(p->receiver, &ropframereply);
        
        // i will transmit back a reply to the remote host at address p->cfg.remipv4addr and port p->cfg.remipv4port  ...
        // which are the ones also assigned to the p->transmitter at its creation.
        
        //eov_mutex_Take(p->mtx_tx_replies, eok_reltimeINFINITE);
        res = eo_transmitter_reply_ropframe_Load(p->transmitter, ropframereply);      
        //eov_mutex_Release(p->mtx_tx_replies);  
        
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



extern eOresult_t eo_transceiver_Transmit(EOtransceiver *p, EOpacket **pkt, uint16_t *numberofrops)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == pkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    // refresh regulars ...    
    //eov_mutex_Take(p->mtx_tx_regulars, eok_reltimeINFINITE); 
    eo_transmitter_regular_rops_Refresh(p->transmitter);
    //eov_mutex_Release(p->mtx_tx_regulars); 
    
    
    // finally retrieve the packet from the transmitter. it will be formed by replies, regulars, occasionals.
    //eov_mutex_Take(p->mtx_tx_replies, eok_reltimeINFINITE);
    //eov_mutex_Take(p->mtx_tx_regulars, eok_reltimeINFINITE);
    //eov_mutex_Take(p->mtx_tx_occasionals, eok_reltimeINFINITE);
    res = eo_transmitter_outpacket_Get(p->transmitter, pkt, numberofrops);
    //eov_mutex_Release(p->mtx_tx_occasionals);
    //eov_mutex_Release(p->mtx_tx_regulars);
    //eov_mutex_Release(p->mtx_tx_replies);
    
    
    return(res);    
}


extern eOresult_t eo_transceiver_rop_regular_Clear(EOtransceiver *p)
{
    eOresult_t res;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    //eov_mutex_Take(p->mtx_tx_regulars, eok_reltimeINFINITE); 
    res = eo_transmitter_regular_rops_Clear(p->transmitter);
    //eov_mutex_Release(p->mtx_tx_regulars); 
    
    return(res);
}

extern eOresult_t eo_transceiver_rop_regular_Load(EOtransceiver *p, eOropdescriptor_t *ropdesc)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == ropdesc))
    {
        return(eores_NOK_nullpointer);
    }
    
    //eov_mutex_Take(p->mtx_tx_regulars, eok_reltimeINFINITE); 
    res = eo_transmitter_regular_rops_Load(p->transmitter, ropdesc);
    //eov_mutex_Release(p->mtx_tx_regulars); 

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

extern eOresult_t eo_transceiver_rop_regular_Unload(EOtransceiver *p, eOropdescriptor_t *ropdesc)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == ropdesc))
    {
        return(eores_NOK_nullpointer);
    }
    
    //eov_mutex_Take(p->mtx_tx_regulars, eok_reltimeINFINITE); 
    res = eo_transmitter_regular_rops_Unload(p->transmitter, ropdesc);
    //eov_mutex_Release(p->mtx_tx_regulars); 
    
    return(res);
}

extern eOresult_t eo_transceiver_rop_occasional_Load_without_data(EOtransceiver *p, eOropdescriptor_t *ropdesc, uint8_t itisobsolete)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == ropdesc))
    {
        return(eores_NOK_nullpointer);
    }

    //eov_mutex_Take(p->mtx_tx_occasionals, eok_reltimeINFINITE);
    res = eo_transmitter_occasional_rops_Load_without_data(p->transmitter, ropdesc, itisobsolete);
    //eov_mutex_Release(p->mtx_tx_occasionals);

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


extern eOresult_t eo_transceiver_rop_occasional_Load(EOtransceiver *p, eOropdescriptor_t *ropdesc)
{
    eOresult_t res;
    
    if((NULL == p) || (NULL == ropdesc))
    {
        return(eores_NOK_nullpointer);
    }
    
//     uint16_t ondevindex;
//     uint16_t onendpointindex;
//     uint16_t onidindex;
//     
//     EOtreenode* treenode;
//     EOnv nv;
//     
//     eObool_t hasdata2send = eobool_false;
    
    
//      // caso in cui data e' non NULL
//     if((eo_ropcode_say == ropdesc->ropcode) || (eo_ropcode_sig == ropdesc->ropcode) || (eo_ropcode_set == ropdesc->ropcode))
//     {
//         // we need data
//         if((NULL == data) || (0 == size))
//         {
//             return(eo_transmitter_occasional_rops_Load(p->transmitter, ropdesc->ropcode, ropdesc->nvep, ropdesc->nvid, ropdesc->ropcfg));
//         }
//         
//     }   

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

//  #if 0  
//
//         
//     eOnvOwnership_t nvownership = eo_rop_hid_GetOwnership(ropdesc->ropcode, eo_ropconf_none, eo_rop_dir_outgoing);
//       
//     
//     // retrieve the indices inside the nvscfg given the triple (ip, ep, id)
//     res = eo_nvscfg_GetIndices( (p->cfg.nvscfg),  
//                                 (eo_nv_ownership_local == nvownership) ? (eok_ipv4addr_localhost) : (p->cfg.remipv4addr), 
//                                 ropdesc->nvep, ropdesc->nvid, 
//                                 &ondevindex,
//                                 &onendpointindex,
//                                 &onidindex); 

//     // if the nvscfg does not have the triple (ip, ep, id) then we return an error because we cannot form the rop
//     if(eores_OK != res)
//     {
//         return(eores_NOK_generic);
//     }

//     // we need a treenode of the nv
//     //treenode = eo_nvscfg_GetTreeNode(&(p->cfg.nvscfg), ondevindex, onendpointindex, onidindex);
//     
//     // we need the nv (but only if the rop needs data).
//     treenode = NULL; // eo_nvscfg_GetNV() internally calls eo_nvscfg_GetTreeNode()
//     eo_nvscfg_GetNV((p->cfg.nvscfg), ondevindex, onendpointindex, onidindex, treenode, &nv);
//     
//     // now we have the nv. we set its value in local ram
//     if(eobool_true == hasdata2send)
//     {       
//         eo_nv_Set(&nv, data, eobool_true, eo_nv_upd_dontdo);
//     
//     }
//     
//     //eo_agent_OutROPfromNV();
//     
//     
//     
//     
//     
//         
//     return(res);
//     
//     #endif
}    


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




