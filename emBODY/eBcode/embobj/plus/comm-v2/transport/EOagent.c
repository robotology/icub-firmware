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

#include "EOVtheSystem.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOagent.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOagent_hid.h" 


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



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_agent_rop_process(EOrop *p, EOrop *replyrop, EOproxy* proxy);
static void s_eo_agent_rop_exec(EOrop *rop_in, EOrop *rop_o);

static EOrop * s_eo_agent_rop_prepare_reply(EOrop *ropin, EOrop *ropout);
static eObool_t s_eo_agent_rop_cannot_manage(EOrop *ropin);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOagent";
 
 
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOagent* eo_agent_New(const eOagent_cfg_t *cfg) 
{    
    EOagent *retptr = NULL;   

    if(NULL == cfg)
    {    
        //cfg = &eo_agent_cfg_default;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "cfg is NULL");
    }
    
    // of all the config, only nvset is mandatory
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != cfg->nvset), s_eobj_ownname, "cfg->nvset is NULL");
    
   
    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOagent), 1);
    
    memcpy(&retptr->config, cfg, sizeof(eOagent_cfg_t));
               
    return(retptr);       
}    

extern EOnvSet* eo_agent_GetNVset(EOagent *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(p->config.nvset);
}

extern EOconfirmationManager* eo_agent_GetConfirmationManager(EOagent *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(p->config.confman);
}

extern EOproxy* eo_agent_GetProxy(EOagent *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    
    return(p->config.proxy);
}

extern eOresult_t eo_agent_InpROPprocess(EOagent *p, EOrop *ropin, eOipv4addr_t fromipaddr, EOrop *replyrop)
{
    uint8_t ropc = eo_ropcode_none;
    eOropconfinfo_t confinfo = eo_ropconf_none;
    eOresult_t res;

    if((NULL == p) || (NULL == ropin) || (NULL == replyrop))
    {
        return(eores_NOK_nullpointer);
    }


    // reset a possible replyrop. responsibility of full preparation is on other functions: s_eo_agent_rop_process() and called ones
    eo_rop_Reset(replyrop);

    // init the ropc of the input rop
    ropc = eo_rop_GetROPcode(ropin);
    confinfo = eo_rop_GetROPconfinfo(ropin);


    // normal commands
    // a simple node which only knows about its own netvars must use eo_nv_ownership_local
    // when receives ask<>, set<>, rst<>, upd<>.
    // a smart node who receives say<> and sig<> must search into eo_nv_ownership_remote.
    
    // ack/nak commands
    // the simple node just process: ack-nak-sig<>. 
    // the smart node can also process: nak-ask<>, ack-nak-set<>, ack-nak-rst<>, ack-nak-upd<>
    

    // can process only valid commands
    if(eobool_false == eo_rop_ropcode_is_valid(ropc))
    {
        return(eores_NOK_generic);
    }


    // if it is a confirmation, then ... call the confirmation engine
    if(eo_ropconf_none != confinfo)
    {   // received a confirmation ack/nak: execute the callback
			
        if(NULL != p->config.confman)
        {
            eo_confman_Confirmation_Received(p->config.confman, fromipaddr, &ropin->ropdes);
        }

        return(eores_OK); 
    }
    else 
    {   // we have a normal rop to be processed with eo_ropconf_none
        eOnvOwnership_t ownership = eo_rop_get_ownership(ropc, eo_ropconf_none, eo_rop_dir_received); // local if we receive a set/get. remote if we receive a sig
        

        res = eo_nvset_NV_Get(  p->config.nvset, 
                                (eo_nv_ownership_local == ownership) ? (eok_ipv4addr_localhost) : (fromipaddr),
                                ropin->stream.head.id32,  
                                &ropin->netvar
                                );
        
        if(eores_OK != res)
        {
            eo_nv_Clear(&ropin->netvar);    
        }
        
        // process the rop even if the netvar is not found (res is not eores_OK)
        // because we may need to send back a nack. 
        s_eo_agent_rop_process(ropin, replyrop, p->config.proxy);

        return(eores_OK);
    }

}



extern eOresult_t eo_agent_OutROPprepare(EOagent* p, EOnv* nv, eOropdescriptor_t* ropdescr, EOrop* rop, uint16_t* requiredbytes)
{
    eOrophead_t rophead;

    if((NULL == p) || (NULL == rop) || (NULL == nv) || (NULL == ropdescr))
    {
        return(eores_NOK_nullpointer);
    } 
    
    // reset the rop
    eo_rop_Reset(rop);  
        
    // put in rophead all the options
    memcpy(&rophead.ctrl, &ropdescr->control, sizeof(eOropctrl_t));
    rophead.ctrl.confinfo   = eo_ropconf_none;  // cannot do a ack/ack
    rophead.ctrl.version    = 0;                // it must be zero
       
    
    rophead.ropc            = ropdescr->ropcode;
    rophead.dsiz            = ropdescr->size;
    rophead.id32            = ropdescr->id32;


    // check validity of ropc 
    if(eobool_false == eo_rop_ropcode_is_valid(ropdescr->ropcode))
    {
        return(eores_NOK_generic);
    }

   
    // --- ok: now we are ready to prepare the rop
    
    // 1. the netvar
    memcpy(&rop->netvar, nv, sizeof(EOnv));
    
    // 2. the stream
    
    // head
    memcpy(&rop->stream.head, &rophead, sizeof(eOrophead_t));
    
    // data
    if(eobool_true == eo_rop_datafield_is_required(&rophead))
    {
        // ropdescr->data is not NULL ... use it
        if(NULL != ropdescr->data)
        {
            rop->stream.head.dsiz   = ropdescr->size;
            memcpy(rop->stream.data, ropdescr->data, rop->stream.head.dsiz);            
        }
        else
        {
            uint16_t sss;
            eo_nv_Get(nv, eo_nv_strg_volatile, rop->stream.data, &sss);
            rop->stream.head.dsiz = sss;
        }
    }
    else
    {
        rop->stream.head.dsiz = 0;
    }
    
    // assign sign
    rop->stream.sign = (1 == ropdescr->control.plussign) ? (ropdescr->signature) : (EOK_uint32dummy);

    // assign time
    rop->stream.time = (0 == ropdescr->control.plustime) ? (EOK_uint64dummy) : (eov_sys_LifeTimeGet(eov_sys_GetHandle()));

    if(NULL != requiredbytes)
    {
        //uint16_t dataeffectivebytes = (rop->stream.head.dsiz + 3) / 4;
        //dataeffectivebytes *= 4;   
        //*requiredbytes = sizeof(eOrophead_t) + dataeffectivebytes + (4*rop->stream.head.ctrl.plussign) + (8*rop->stream.head.ctrl.plustime);
        *requiredbytes = eo_rop_compute_size(rop->stream.head.ctrl, rop->stream.head.ropc, rop->stream.head.dsiz);
    }

    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static eOresult_t s_eo_agent_rop_process(EOrop *p, EOrop *replyrop, EOproxy* proxy) 
{
    EOrop *rop_o = NULL;
    EOnv *thenv = &p->netvar;

    if((NULL == p) || (NULL == replyrop))
    {
        return(eores_NOK_nullpointer);
    }


    // reset the replyrop
    eo_rop_Reset(replyrop);

    // verify if we need a reply rop and prepare it. if we need it, then rop_o is not NULL
    rop_o = s_eo_agent_rop_prepare_reply(p, replyrop);

    // also ... filter out the operations that we cannot surely perform, such any operation on a not-existing nv
    // or as a write on a ro var etc
    if(eobool_true == s_eo_agent_rop_cannot_manage(p))
    {
        return(eores_OK);
    }
    
    // we are in here only if the var exists and the operation can be done

    if((eobool_true == eo_nv_IsProxied(thenv) && (NULL != proxy) && (eo_nv_ownership_local == eo_nv_GetOwnership(thenv))))
    {
        eo_proxy_ROP_Forward(proxy, p, rop_o);
    }
    else
    {   
        s_eo_agent_rop_exec(p, rop_o);
    }


    return(eores_OK);
}


static void s_eo_agent_rop_exec(EOrop *rop_in, EOrop *rop_o)
{
    eOresult_t res = eores_NOK_generic;
    const uint8_t *source = NULL;
    uint8_t *destin = NULL;
    uint16_t size = 0;
    EOnv *thenv = NULL;
    eOropdescriptor_t* theropdes = NULL;


    thenv = &rop_in->netvar;
    theropdes = &rop_in->ropdes;
    

    switch(rop_in->stream.head.ropc)
    {
        case eo_ropcode_set:
        case eo_ropcode_rst:
        {   // the receiver owns the nv locally

            // in here we manage operations which ask to write a value into the netvar.
            // the value can be contained in the data field of the remote operation (set)
            // or can be taken from a constant local memory location (rst).
            // the destination is always a ram location (and in addition can also be a remote device in case of cached variable).
            // the writing operation is done only if the netvar has writeable properties.
            // we have a reply rop only if there is an explicit ack/nak request
            // also ... we call the update function to propagate the value to the peripheral (or to the attached remote device)


            if(eo_ropcode_rst == rop_in->stream.head.ropc)
            {   
                res = eo_nv_hid_ResetROP(thenv, eo_nv_upd_ifneeded, theropdes);
            }
            else 
            {   // set
                
                // get the data to be set and its size.
                source = rop_in->stream.data;
                if(rop_in->stream.head.dsiz == thenv->rom->capacity)
                {
                    res = eo_nv_hid_SetROP(thenv, source, eo_nv_upd_ifneeded, theropdes);
                }
                else
                {   // if the rop is badly formed we dont write ...
                    res = eores_NOK_generic;
                }
            }
                        
            if(eores_OK == res)  
            {                
                if((1 == rop_in->stream.head.ctrl.rqstconf) && (NULL != rop_o))
                {   // mark the ropcode of the reply (if any) to be ack
                    rop_o->stream.head.ctrl.confinfo = eo_ropconf_ack;
                } 
            }

        } break;

        
        case eo_ropcode_ask:
        {   // the receiver owns the nv locally
            
            // in here we manage operations which ask to retrieve the value of the netvar and to create a reply rop.
            // the value for reply rop is taken from ram            
 
            // we use datasize and not the progressive index because by doing so we update the rop_o as soon as we write
            destin = rop_o->stream.data;

            eo_nv_Get(thenv, eo_nv_strg_volatile, destin, &size);

            // datasize is set to size
            rop_o->stream.head.dsiz = size;
            // need to update the ropdes as well
            rop_o->ropdes.size = size;
            rop_o->ropdes.data = rop_o->stream.data;
                    
            // never mark it as a nak or ack, even if we received the confirmation request.
            // the confirmation is the reply message itself 
            rop_o->stream.head.ctrl.confinfo = eo_ropconf_none;

        } break; 
                     

        case eo_ropcode_sig:
        case eo_ropcode_say:
        {   // the receiver owns the nv remotely.


            // in here we need to copy the received data into the volatile data of the netvar.
            // then, the processing of such data is done in the appropriate update() function 
    

            // force write also if an input, force update.
            source = rop_in->stream.data;
            eo_nv_hid_remoteSetROP(thenv, source, eo_nv_upd_always, theropdes);

            // in here we manage the case in which the sig message that we have received required an ack.
            if((1 == rop_in->stream.head.ctrl.rqstconf) && (NULL != rop_o) && (eo_ropcode_sig == rop_in->stream.head.ropc))
            {
                rop_o->stream.head.ctrl.confinfo = eo_ropconf_ack;
            } 

        } break;

                  
        default:
        {
        } break;
    }

}


static EOrop * s_eo_agent_rop_prepare_reply(EOrop *ropin, EOrop *ropout)
{
    // in here we fill the reply rop.

    // if the nv is not found given a [ip, id32], (thus the condition is netvar.rom NULL), then we reply only if ctrl.rqstconf is 1.    
    // if the nv is an existing one we reply only if (a) we have a ask or if (b) the ctrl.rqstconf is 1. 
    // in case of (a) we reply with a say.
    // in case of (b) we may reply with a nak/ack. ack if the rop can be performed, nak if it cannot be performed (or nv does not exist) 
    
    EOrop *r = NULL;
    eObool_t nvisnotexisting = (NULL == ropin->netvar.rom) ? eobool_true : eobool_false;

    if(eobool_true == nvisnotexisting)
    {   // we did not find a netvar w/ that (ip,id32) but if we have been asked for a confirmation then we must send a nak
        if(1 == ropin->stream.head.ctrl.rqstconf)
        {
            r = ropout;
            r->stream.head.ctrl.confinfo 	        = eo_ropconf_nak;            
            r->stream.head.dsiz 					= 0;
            r->stream.head.id32 					= ropin->stream.head.id32;
            r->stream.head.ropc 					= ropin->stream.head.ropc;
        }
        else
        {
            r = NULL;
        }

    }
    else if(eo_ropcode_ask == ropin->stream.head.ropc)
    {   // in case we have a ask, then we have two cases: (1) the var is fully owned, (2) the varaible is proxied.
        // we surely send a say only in first case.          
        r = ropout;
        r->stream.head.ropc = eo_ropcode_say;

        if(1 == ropin->stream.head.ctrl.rqstconf)
        {   // by default we assign a nak, but we SHALL transform it in a none if we process the ask later on
            r->stream.head.ctrl.confinfo = eo_ropconf_nak;  
        }
    }
    else if(1 == ropin->stream.head.ctrl.rqstconf)
    {   // with any other opcode with a confirmation request we must send a reply			
        // we may send a ack or a nak, depending on success of operation (insuccess is write on ro netvars). 
        // we assign a nak ropcode which we SHALL transform in a ack if operation is successful.
        r = ropout;
        r->stream.head.ctrl.confinfo 	= eo_ropconf_nak;
        r->stream.head.ropc 		    = ropin->stream.head.ropc;
    }

    // now we complete the reply
    if(NULL != r)
    {
        // but we surely assign the same netvar, as the reply is about the very same netvar
        memcpy(&r->netvar, &ropin->netvar, sizeof(EOnv));

        r->stream.head.ctrl.plussign = ropin->stream.head.ctrl.plussign;
        r->stream.head.ctrl.plustime = ropin->stream.head.ctrl.rqsttime;
        
        r->stream.head.dsiz = 0; // if it must be non-zero then some function shall fill it        
        r->stream.head.id32 = ropin->stream.head.id32;

        // data will be filled by some other function
        r->stream.sign = (1 == r->stream.head.ctrl.plussign) ? (ropin->stream.sign) : (EOK_uint32dummy);
#ifndef EODEF_DONT_USE_THE_VSYSTEM
        r->stream.time = (1 == r->stream.head.ctrl.plustime) ? (eov_sys_LifeTimeGet(eov_sys_GetHandle())) : (EOK_uint64dummy);
#else
        r->stream.time = (1 == r->stream.head.ctrl.plustime) ? (0xf1f2f3f4f5f6f7f8) : (EOK_uint64dummy);
#endif

        eo_rop_hid_fill_ropdes(&r->ropdes, &r->stream, 0, NULL);			
    }

    return(r);
}


static eObool_t s_eo_agent_rop_cannot_manage(EOrop *ropin)
{
    // we surely cannot do: 
    // (1) operations on a NULL nv
    // (2) a write into a ro: inp or con
    // (3) an update to what is not updateable: con, cfg, beh.
    // DO NOT filter out the sig/say operations

    eObool_t ret = eobool_false;
    eObool_t nvisnotexisting = (NULL == ropin->netvar.rom) ? eobool_true : eobool_false;

    
    if(eobool_true == nvisnotexisting)
    {   // if the nv was not found ... we surely cannot operate on that
        return(eobool_true);
    }


    switch(ropin->stream.head.ropc)
    {
        case eo_ropcode_set:
        case eo_ropcode_rst:
        {   // only if we can write the netvar
            ret = (eobool_false == eo_nv_hid_isWritable(&ropin->netvar)) ? eobool_true : eobool_false;
        } break;

        default:
        {
        } break;
    }


    return(ret);
}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




