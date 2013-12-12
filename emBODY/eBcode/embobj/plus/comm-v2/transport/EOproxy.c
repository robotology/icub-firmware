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
#include "EOVtheSystem.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOproxy.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOproxy_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if defined(EO_TAILOR_CODE_FOR_ARM)
    #define EONV_DONT_USE_EOV_MUTEX_FUNCTIONS
#endif


#if defined(EONV_DONT_USE_EOV_MUTEX_FUNCTIONS)
    #define eov_mutex_Take(a, b)   
    #define eov_mutex_Release(a)
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct              // 24+24=48
{
    eOropdescriptor_t       ropdes; // ropdes.time contains the expiry time ...
    EOnv                    nv;
} eo_proxy_ropdes_plus_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_proxy_forward_set(EOproxy *p, EOrop *rop, EOrop *ropout);

static eOresult_t s_eo_proxy_forward_rst(EOproxy *p, EOrop *rop, EOrop *ropout);

static eOresult_t s_eo_proxy_forward_ask(EOproxy *p, EOrop *rop, EOrop *ropout);

static eOresult_t s_matching_rule_id32(void *item, void *param);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOproxy";
 
const eOproxy_cfg_t eo_proxy_cfg_default = 
{
    EO_INIT(.mode)                          eoproxy_mode_disabled,
    EO_INIT(.capacityoflistofropdes)        8, 
    EO_INIT(.replyroptimeout)               10*1000, 
    EO_INIT(.mutex_fn_new)                  NULL,
    EO_INIT(.transceiver)                   NULL
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOproxy* eo_proxy_New(const eOproxy_cfg_t *cfg) 
{    
    EOproxy *retptr = NULL;   

    if(NULL == cfg)
    {    
        cfg = &eo_proxy_cfg_default;
        //eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "cfg is NULL");
    }
    
    if(eoproxy_mode_disabled == cfg->mode)
    {
        return(NULL);
    }
    
    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOproxy), 1);
    
    memcpy(&retptr->config, cfg, sizeof(eOproxy_cfg_t));
    
    // i get the list ...
    
    retptr->transceiver = cfg->transceiver;
    
    retptr->listofropdes    = (0 == cfg->capacityoflistofropdes) ? (NULL) : (eo_list_New(sizeof(eo_proxy_ropdes_plus_t), cfg->capacityoflistofropdes, NULL, 0, NULL, NULL));
    
    if(NULL != cfg->mutex_fn_new)
    {
        retptr->mtx = cfg->mutex_fn_new();
    }
           
    return(retptr);       
}    



extern eOresult_t eo_proxy_ROP_Forward(EOproxy *p, EOrop* rop, EOrop* ropout)
{
    eOresult_t res = eores_NOK_generic;
    
    if((NULL == p) || (NULL == rop))
    {
        return(eores_NOK_nullpointer);
    }

    EOnv *nv = &rop->netvar;
    
    if(eobool_false == eo_nv_IsProxied(nv))
    {
        return(eores_NOK_generic);
    }

    switch(rop->ropdes.ropcode)
    {
        case eo_ropcode_ask:
        {
            res = s_eo_proxy_forward_ask(p, rop, ropout);
        } break;
            
        case eo_ropcode_set:
        {
            res = s_eo_proxy_forward_set(p, rop, ropout);
        } break; 

        case eo_ropcode_rst:
        {
            res = s_eo_proxy_forward_rst(p, rop, ropout);
        } break; 

        default:
        {
            res = eores_NOK_generic;
        } break;
        
    }
    
    return(res);
}

extern eOresult_t eo_proxy_ReplyROP_Load(EOproxy *p, eOnvID32_t id32, void *data)
{
    eOresult_t res = eores_NOK_generic;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
        
    eov_mutex_Take(p->mtx, eok_reltimeINFINITE);
    
    EOlistIter* li = eo_list_Find(p->listofropdes, s_matching_rule_id32, &id32);

    if(NULL == li)
    {   // there is no entry with id32 in the list ... i dont load any reply rop
        eov_mutex_Release(p->mtx);
        return(eores_NOK_generic);
    }
    
    eo_proxy_ropdes_plus_t *item = eo_list_At(p->listofropdes, li);
    
    if(NULL != data)
    {
        eo_nv_Set(&item->nv, data, eobool_true, eo_nv_upd_dontdo);   
    }

    // item->ropdes is already ok (opc is say, id32 is ..., etc.) ... i just change the time
    item->ropdes.time = 0;    
    res = eo_transceiver_ReplyROP_Load(p->config.transceiver, &item->ropdes);
    
    eo_list_Erase(p->listofropdes, li);
    
    eov_mutex_Release(p->mtx);

    return(res);    
}
    
    
extern eOresult_t eo_proxy_Tick(EOproxy *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eOreltime_t timenow = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    
    eov_mutex_Take(p->mtx, eok_reltimeINFINITE);
    
    // i assume that the items are in expiry order, thus i get the front and i keep on removing until timenow is higher than item->ropdes.time          
    eo_proxy_ropdes_plus_t* item = eo_list_Front(p->listofropdes);   
    while((NULL != item) && (timenow > item->ropdes.time))
    {
        eo_list_PopFront(p->listofropdes);
        item = eo_list_Front(p->listofropdes);
    }
    
    eov_mutex_Release(p->mtx);

    return(eores_OK);    
}    


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static eOresult_t s_eo_proxy_forward_set(EOproxy *p, EOrop *rop, EOrop *ropout)
{
    EOnv *nv = &rop->netvar;
    eOropdescriptor_t* ropdes = &rop->ropdes;
    eOresult_t res = eo_nv_hid_SetROP(nv, ropdes->data, eo_nv_upd_ifneeded, ropdes);   
    
    // if operation was successfull, ... fill a possible reply 
    if(eores_OK == res)  
    {                
        // mark the ropcode of the reply (if any) to be ack
        if((1 == rop->stream.head.ctrl.rqstconf) && (NULL != ropout))
        {
            ropout->stream.head.ctrl.confinfo = eo_ropconf_ack;
        } 
    }    
    
    return(res);
}


static eOresult_t s_eo_proxy_forward_rst(EOproxy *p, EOrop *rop, EOrop *ropout)
{
    EOnv *nv = &rop->netvar;
    eOropdescriptor_t* ropdes = &rop->ropdes;
    eOresult_t res = eo_nv_hid_ResetROP(nv, eo_nv_upd_ifneeded, ropdes);
    
    // if operation was successfull, ... fill a possible reply 
    if(eores_OK == res)  
    {                
        // mark the ropcode of the reply (if any) to be ack
        if((1 == rop->stream.head.ctrl.rqstconf) && (NULL != ropout))
        {
            ropout->stream.head.ctrl.confinfo = eo_ropconf_ack;
        } 
    }    
     
    return(res);
}

static eOresult_t s_eo_proxy_forward_ask(EOproxy *p, EOrop *rop, EOrop *ropout)
{
    EOnv *nv = &rop->netvar;
    eOropdescriptor_t* ropdes = &rop->ropdes;
    eOresult_t res = eores_NOK_generic;
    eo_proxy_ropdes_plus_t ropdesplus;
     
    eov_mutex_Take(p->mtx, eok_reltimeINFINITE);
    
    if(eobool_true != eo_list_Full(p->listofropdes))
    {   // we can process the ask        
        res = eo_nv_hid_UpdateROP(nv, eo_nv_upd_always, ropdes);       
    }
    else
    {
        res = eores_NOK_generic;    
    }
    
    
    if(eores_OK != res)
    {
        // in such a case, if the ask was with ack-required ... we dont send a nak
        #define DONT_SEND_NAK
        #if defined(DONT_SEND_NAK)
        eo_rop_Reset(ropout); 
        #else
        // mark the ropcode of the reply (if any) to be nak
        if((1 == rop->stream.head.ctrl.rqstconf) && (NULL != ropout))
        {
            ropout->stream.head.ctrl.confinfo = eo_ropconf_nak;
        } 
        #endif
        
        eov_mutex_Release(p->mtx);
        return(res);
    }
    
    // if we can process the ask we dont send a roput, not even a ack. thus we reset the rop so that the caller of roxy object cannot send it out
    eo_rop_Reset(ropout); 
       
    // we prepare the ropdes for transmission.
    ropdesplus.ropdes.control.confinfo  = eo_ropconf_none;
    ropdesplus.ropdes.control.plustime  = ropdes->control.rqsttime;
    ropdesplus.ropdes.control.plussign  = ropdes->control.plussign;
    ropdesplus.ropdes.control.rqsttime  = 0;
    ropdesplus.ropdes.control.rqstconf  = 0;
    ropdesplus.ropdes.control.version   = ropdes->control.version;
    ropdesplus.ropdes.ropcode           = eo_ropcode_say;
    ropdesplus.ropdes.size              = 0;
    ropdesplus.ropdes.id32              = ropdes->id32;
    ropdesplus.ropdes.data              = NULL;
    ropdesplus.ropdes.signature         = ropdes->signature;
    // in ropdes.time we put the timeout
    ropdesplus.ropdes.time              =  p->config.replyroptimeout;
    if(EOK_uint64dummy != p->config.replyroptimeout)
    {
        ropdesplus.ropdes.time += eov_sys_LifeTimeGet(eov_sys_GetHandle());
    }   

    // we copy the nv
    memcpy(&ropdesplus.nv, nv, sizeof(EOnv));
       
    // now we insert the item in the list according to expiry time. 
    // since the timeouts  are all equal, i just append the item at the end of the list
    eo_list_PushBack(p->listofropdes, &ropdesplus);
     
    eov_mutex_Release(p->mtx);    
    return(res);
}


static eOresult_t s_matching_rule_id32(void *item, void *param)
{
    eo_proxy_ropdes_plus_t *inside = (eo_proxy_ropdes_plus_t*)item;
    eOnvID32_t *id32ptr = (eOnvID32_t*)param;

    if(inside->ropdes.id32 == *id32ptr)
    {
        return(eores_OK);
    }
    else
    {
        return(eores_NOK_generic);
    }
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




