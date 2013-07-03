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

#include "EOtheAgent.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheAgent_hid.h" 


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

// static void s_eo_agent_on_rop_confirmation_received(eOipv4addr_t ipaddr, eOropcode_t ropc, eOnvEP_t endp, eOnvID_t nvid, 
//                                                     uint32_t sign, eOabstime_t time, eOropconfinfo_t confinfo);


// static void s_eo_agent_on_rop_confirmation_requested(eOipv4addr_t ipaddr, eOropcode_t ropc, eOnvEP_t endp, eOnvID_t nvid,
//                                                      uint32_t sign, eOabstime_t time);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOtheAgent";
 
static EOtheAgent eo_theagent = 
{
    EO_INIT(.initted)                   0,
    EO_INIT(.cfg)                       NULL
};


const eOagent_cfg_t eo_agent_cfg_default =
{
    EO_INIT(.on_rop_conf_received)      NULL, 
    EO_INIT(.on_rop_conf_requested)     NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOtheAgent* eo_agent_Initialise(const eOagent_cfg_t *cfg) 
{    
    if(1 == eo_theagent.initted)
    {
        return(&eo_theagent);
    }

    if(NULL == cfg)
    {
        cfg = &eo_agent_cfg_default;
    }

    eo_theagent.cfg     = cfg;
    eo_theagent.initted = 1;
    
    return(&eo_theagent);        
}    


extern EOtheAgent * eo_agent_GetHandle(void) 
{
    return( (1 == eo_theagent.initted) ? (&eo_theagent) : (eo_agent_Initialise(NULL)) );
}


extern eOresult_t eo_agent_InpROPprocess(EOtheAgent *p, EOrop *ropin, EOnvSet* nvset, eOipv4addr_t fromipaddr, EOrop *replyrop)
{
    uint8_t ropc = eo_ropcode_none;
    eOropconfinfo_t confinfo = eo_ropconf_none;
    eOresult_t res;

    if((NULL == p) || (NULL == nvset) || (NULL == ropin) || (NULL == replyrop))
    {
        return(eores_NOK_nullpointer);
    }


    // reset a possible replyrop. responsibility of full preparation is on other functions: eo_rop_Process() and called ones
    eo_rop_Reset(replyrop);

    // init the ropc of the input rop
    ropc = eo_rop_ropcode_Get(ropin);
    confinfo = eo_rop_ropconfinfo_Get(ropin);


    // normal commands
    // a simple node which only knows about its own netvars must use eo_nv_ownership_local
    // when receives ask<>, set<>, rst<>, upd<>.
    // a smart node who receives say<> and sig<> must search into eo_nv_ownership_remote.
    
    // ack/nak commands
    // the simple node just process: ack-nak-sig<>. 
    // the smart node can also process: nak-ask<>, ack-nak-set<>, ack-nak-rst<>, ack-nak-upd<>
    

    // can process only valid commands
    if(eobool_false == eo_rop_hid_ropcode_is_valid(ropc))
    {
        return(eores_NOK_generic);
    }


    // if it is a confirmation, then ... call the confirmation engine
    if(eo_ropconf_none != confinfo)
    {   // received a confirmation ack/nak: execute the callback
			
        if(NULL != eo_theagent.cfg->on_rop_conf_received)
        {
            eo_theagent.cfg->on_rop_conf_received(/*ropin->aboutip.ipaddr*/ fromipaddr, ropc, ropin->stream.head.endp, ropin->stream.head.nvid, ropin->stream.sign, ropin->stream.time, (eOropconfinfo_t)ropin->stream.head.ctrl.confinfo);
        }

        return(eores_OK); 
    }
    else 
    {   // we have a normal rop to be processed with eo_ropconf_none
        eOnvOwnership_t ownership = eo_rop_hid_GetOwnership(ropc, eo_ropconf_none, eo_rop_dir_received); // local if we receive a set/get. remote if we receive a sig
        
        // asfidanken
        //ropin->tmpdata.nvset = nvset;
        //ropin->tmpdata.nvownership = ownership;
        
        res = eo_nvset_NVget(   nvset, /*ropin->tmpdata.nvset,*/ 
                                (eo_nv_ownership_local == ownership) ? (eok_ipv4addr_localhost) : (fromipaddr),
                                ropin->stream.head.endp, ropin->stream.head.nvid,  
                                &ropin->netvar
                                );
        
        if(eores_OK != res)
        {
            eo_nv_Clear(&ropin->netvar);    
        }
        

        // process the rop even if the netvar is not found (res is not eores_OK)
        // because we may need to send back a nack. 
        eo_rop_Process(ropin, replyrop);

        return(eores_OK);
    }

}



extern eOresult_t eo_agent_OutROPprepare(EOtheAgent* p, EOnv* nv, eOropdescriptor_t* ropdescr, EOrop* rop, uint16_t* requiredbytes)
{
    eOrophead_t rophead;

    if((NULL == p) || (NULL == rop) || (NULL == nv) || (NULL == ropdescr))
    {
        return(eores_NOK_nullpointer);
    } 
    

    // reset the rop
    eo_rop_Reset(rop);  
        
    // put in rophead all the options
//     rophead.ctrl.ffu        = 0;
//     rophead.ctrl.confinfo   = eo_ropconf_none; // cannot do a ack/ack
//     rophead.ctrl.plustime   = ropdescr->configuration.plustime;
//     rophead.ctrl.plussign   = ropdescr->configuration.plussign;
//     rophead.ctrl.rqsttime   = ropdescr->configuration.timerqst;
//     rophead.ctrl.rqstconf   = ropdescr->configuration.confrqst;
//     rophead.ctrl.userdefn   = 0;
    
    memcpy(&rophead.ctrl, &ropdescr->control, sizeof(eOropctrl_t));
    rophead.ctrl.confinfo   = eo_ropconf_none;  // cannot do a ack/ack
    rophead.ctrl.ffu        = 0;                // it should be zero
    rophead.ctrl.userdefn   = 0;                // it must be zero
       
    
    rophead.ropc            = ropdescr->ropcode;
    rophead.endp            = ropdescr->ep;
    rophead.nvid            = ropdescr->id;
    rophead.dsiz            = ropdescr->size;


    // check validity of ropc 
    if(eobool_false == eo_rop_hid_ropcode_is_valid(ropdescr->ropcode))
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
    if(eobool_true == eo_rop_hid_DataField_is_Required(&rophead))
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
        rop->stream.head.dsiz = eo_nv_Size(nv);
    }
    
    // assign sign
    rop->stream.sign = (1 == ropdescr->control.plussign) ? (ropdescr->signature) : (EOK_uint32dummy);

    // assign time
    rop->stream.time = (0 == ropdescr->control.plustime) ? (EOK_uint64dummy) : (eov_sys_LifeTimeGet(eov_sys_GetHandle()));

    if(NULL != requiredbytes)
    {
        uint16_t dataeffectivebytes = (rop->stream.head.dsiz + 3) / 4;
        dataeffectivebytes *= 4;   
        *requiredbytes = sizeof(eOrophead_t) + dataeffectivebytes + (4*rop->stream.head.ctrl.plussign) + (8*rop->stream.head.ctrl.plustime);
    }

    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern eOresult_t eo_agent_hid_OutROPonTransmission(EOtheAgent *p, EOrop *rop)
{
    if(1 == rop->stream.head.ctrl.rqstconf)
    {
        if(NULL !=  eo_theagent.cfg->on_rop_conf_requested)
        {
//            #warning --> in eo_agent_hid_OutROPonTransmission() ho rimosso il ip di destinazione
            eo_theagent.cfg->on_rop_conf_requested(/*rop->aboutip.ipaddr*/0, rop->stream.head.ropc, rop->stream.head.endp, rop->stream.head.nvid, rop->stream.sign, rop->stream.time);
        }
        return(eores_OK);
    }

    return(eores_NOK_generic);   
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// - oldies ------


// OK: called by eo_transmitter_regular_rops_Load().
//extern eOresult_t eo_agent_OutROPinit(EOtheAgent *p, EOnvSet* nvset, eOipv4addr_t toipaddr, eOropdescriptor_t* ropdescr, EOrop *rop, uint16_t *requiredbytes);


// OK: called by eo_transmitter_occasional_rops_Load(). if data is required uses ropdescr->data/size if not NULL/0 otherwise uses nv.
//extern eOresult_t eo_agent_OutROPfromNV(EOtheAgent* p, EOnv* nv, eOropdescriptor_t* ropdescr, EOrop* rop, uint16_t* requiredbytes);

// if a payload is needed: if nv is local it uses nv->ram (nv is internally computed). it nv is remote it uses ropdescr->ram
//extern eOresult_t eo_agent_OutROPprepareOld(EOtheAgent* p, EOnvSet* nvset, eOipv4addr_t toipaddr, eOropdescriptor_t* ropdescr, EOrop* rop, uint16_t* requiredbytes);


// if a payload is needed: if nv is local it uses nv->ram (nv passed). it nv is remote it uses ropdescr->ram
//extern eOresult_t eo_agent_OutROPprepareNV(EOtheAgent* p, EOnv* nv, eOropdescriptor_t* ropdescr, EOrop* rop, uint16_t* requiredbytes);


// updates an existing rop
//extern eOresult_t eo_agent_OutROPrefresh(EOtheAgent *p, EOrop *rop);


#if 0
extern eOresult_t eo_agent_OutROPinit(EOtheAgent *p, EOnvSet* nvset, eOipv4addr_t toipaddr, eOropdescriptor_t* ropdescr, EOrop *rop, uint16_t *requiredbytes)
{
    eOrophead_t rophead;
    eOresult_t res;

    if((NULL == p) || (NULL == rop) || (NULL == ropdescr))
    {
        return(eores_NOK_nullpointer);
    } 

    // reset the rop
    eo_rop_Reset(rop);  
    
    // put in rophead all the options
//     rophead.ctrl.ffu        = 0;
//     rophead.ctrl.confinfo   = eo_ropconf_none; // cannot do a ack/ack
//     rophead.ctrl.plustime   = ropdescr->configuration.plustime;
//     rophead.ctrl.plussign   = ropdescr->configuration.plussign;
//     rophead.ctrl.rqsttime   = ropdescr->configuration.timerqst;
//     rophead.ctrl.rqstconf   = ropdescr->configuration.confrqst;
//     rophead.ctrl.userdefn   = 0;
    memcpy(&rophead.ctrl, &ropdescr->control, sizeof(eOropctrl_t)); 
    rophead.ctrl.confinfo   = eo_ropconf_none;  // cannot do a ack/ack
    rophead.ctrl.ffu        = 0;                // it should be 0
    rophead.ctrl.userdefn   = 0;                // it must be 0
    
    
    rophead.ropc            = ropdescr->ropcode;
    rophead.endp            = ropdescr->ep;
    rophead.nvid            = ropdescr->id;
    rophead.dsiz            = ropdescr->size;


    // check validity of ropdescr->ropcode     
    if(eobool_false == eo_rop_hid_ropcode_is_valid(ropdescr->ropcode))
    {
        return(eores_NOK_generic);
    }
          

    // get the ownership  
    eOnvOwnership_t ownership = eo_rop_hid_GetOwnership(ropdescr->ropcode, eo_ropconf_none, eo_rop_dir_outgoing);  

    res = eo_nvset_NVget(   nvset, 
                            (eo_nv_ownership_local == ownership) ? (eok_ipv4addr_localhost) : (toipaddr), 
                            ropdescr->ep, ropdescr->id,
                            &rop->netvar
                            );

    // if the nvset does not have the triple (ip, ep, id) then we return an error because we cannot form the rop
    if(eores_OK != res)
    {
        return(eores_NOK_generic);
    }

   
    // --- ok: now we are ready to prepare the rop
    
    // head
    memcpy(&rop->stream.head, &rophead, sizeof(eOrophead_t));
    
    // data
    if(eobool_true == eo_rop_hid_DataField_is_Required(&rophead))
    {
//         uint16_t sss;
//         eo_nv_Get(&rop->netvar, eo_nv_strg_volatile, rop->stream.data, &sss);
//         rop->stream.head.dsiz = sss;   
        
        // ropdescr->data is not NULL ... use it
        if(NULL != ropdescr->data)
        {
            rop->stream.head.dsiz   = ropdescr->size;
            memcpy(rop->stream.data, ropdescr->data, rop->stream.head.dsiz);            
        }
        else
        {
            uint16_t sss;
            eo_nv_Get(&rop->netvar, eo_nv_strg_volatile, rop->stream.data, &sss);
            rop->stream.head.dsiz = sss;
        }        
    }
    else
    {
        rophead.dsiz = eo_nv_Size(&rop->netvar);
    }
    
    
    // assign signature
    rop->stream.sign = (0 == ropdescr->configuration.plussign) ? (EOK_uint32dummy) : (ropdescr->signature);

    // assign time
    rop->stream.time = (0 == ropdescr->configuration.plustime) ? (EOK_uint64dummy) : (eov_sys_LifeTimeGet(eov_sys_GetHandle()));


    if(NULL != requiredbytes)
    {
        uint16_t dataeffectivebytes = (rop->stream.head.dsiz + 3) / 4;
        dataeffectivebytes *= 4;   
        *requiredbytes = sizeof(eOrophead_t) + dataeffectivebytes + (4*rop->stream.head.ctrl.plussign) + (8*rop->stream.head.ctrl.plustime);
    }


    return(eores_OK);
}
#endif

#if 0
// this function uses only the ram passed inside ropdescriptor
extern eOresult_t eo_agent_OutROPprepareOld(EOtheAgent* p, EOnvSet* nvset, eOipv4addr_t toipaddr, eOropdescriptor_t* ropdescr, EOrop* rop, uint16_t* requiredbytes)
{
    eOresult_t res = eores_NOK_generic;
    eOrophead_t rophead;

    if((NULL == p) || (NULL == nvset) || (NULL == ropdescr) || (NULL == rop))
    {
        return(eores_NOK_nullpointer);
    } 
    
    // check validity of ropc 
    if(eobool_false == eo_rop_hid_ropcode_is_valid(ropdescr->ropcode))
    {
        return(eores_NOK_generic);
    }
    
    // 1. reset the rop
    eo_rop_Reset(rop);  
    
    
    // 2. put in rophead all the options
    rophead.ctrl.ffu        = 0;
    rophead.ctrl.confinfo   = eo_ropconf_none; // cannot do a ack/ack
    rophead.ctrl.plustime   = ropdescr->configuration.plustime;
    rophead.ctrl.plussign   = ropdescr->configuration.plussign;
    rophead.ctrl.rqsttime   = ropdescr->configuration.timerqst;
    rophead.ctrl.rqstconf   = ropdescr->configuration.confrqst;
    rophead.ctrl.userdefn   = 0;
    rophead.ropc            = ropdescr->ropcode;
    rophead.endp            = ropdescr->ep;
    rophead.nvid            = ropdescr->id;
    rophead.dsiz            = 0;


   
    
    // 3. the netvar
    eOnvOwnership_t ownership = eo_rop_hid_GetOwnership(ropdescr->ropcode, eo_ropconf_none, eo_rop_dir_outgoing);  

    res = eo_nvset_NVget(   nvset, 
                            (eo_nv_ownership_local == ownership) ? (eok_ipv4addr_localhost) : (toipaddr), 
                            ropdescr->ep, ropdescr->id,
                            &rop->netvar
                            );

    // if the nvset does not have the triple (ip, ep, id) then we return an error because we cannot form the rop
    if(eores_OK != res)
    {
        return(eores_NOK_generic);
    }

    
    // 4. the stream
    
    // head
    memcpy(&rop->stream.head, &rophead, sizeof(eOrophead_t));
    
    // data
    if(eobool_true == eo_rop_hid_DataField_is_Required(&rophead))
    {
        rop->stream.head.dsiz = eo_nv_Size(&rop->netvar);
        if(NULL != ropdescr->data)
        {
            memcpy(rop->stream.data, ropdescr->data, rop->stream.head.dsiz);  
        }   
        else
        {
            return(eores_NOK_generic);
        }
    }
    
    // assign sign
    rop->stream.sign = (1 == ropdescr->configuration.plussign) ? (ropdescr->signature) : (EOK_uint32dummy);

    // assign time
    rop->stream.time = (0 == ropdescr->configuration.plustime) ? (EOK_uint64dummy) : (eov_sys_LifeTimeGet(eov_sys_GetHandle()));

    if(NULL != requiredbytes)
    {
        uint16_t dataeffectivebytes = (rop->stream.head.dsiz + 3) / 4;
        dataeffectivebytes *= 4;           
        *requiredbytes = sizeof(eOrophead_t) + dataeffectivebytes + (4*rop->stream.head.ctrl.plussign) + (8*rop->stream.head.ctrl.plustime);
    }


    return(eores_OK);
}


// this function uses only the ram passed inside ropdescriptor
extern eOresult_t eo_agent_OutROPprepareNV(EOtheAgent* p, EOnv* nv, eOropdescriptor_t* ropdescr, EOrop* rop, uint16_t* requiredbytes)
{
    eOrophead_t rophead;

    if((NULL == p) || (NULL == nv) || (NULL == ropdescr) || (NULL == rop))
    {
        return(eores_NOK_nullpointer);
    } 
    
    // check validity of ropc 
    if(eobool_false == eo_rop_hid_ropcode_is_valid(ropdescr->ropcode))
    {
        return(eores_NOK_generic);
    }
    
    // 1. reset the rop
    eo_rop_Reset(rop);  
    
    
    // 2. put in rophead all the options
    rophead.ctrl.ffu        = 0;
    rophead.ctrl.confinfo   = eo_ropconf_none; // cannot do a ack/ack
    rophead.ctrl.plustime   = ropdescr->configuration.plustime;
    rophead.ctrl.plussign   = ropdescr->configuration.plussign;
    rophead.ctrl.rqsttime   = ropdescr->configuration.timerqst;
    rophead.ctrl.rqstconf   = ropdescr->configuration.confrqst;
    rophead.ctrl.userdefn   = 0;
    rophead.ropc            = ropdescr->ropcode;
    rophead.endp            = ropdescr->ep;
    rophead.nvid            = ropdescr->id;
    rophead.dsiz            = 0;

    
    // 3. the netvar
    memcpy(&rop->netvar, nv, sizeof(EOnv));

    
    // 4. the stream
    
    // head
    memcpy(&rop->stream.head, &rophead, sizeof(eOrophead_t));
    
    // data
    if(eobool_true == eo_rop_hid_DataField_is_Required(&rophead))
    {
        eOnvOwnership_t ownership = eo_rop_hid_GetOwnership(ropdescr->ropcode, eo_ropconf_none, eo_rop_dir_outgoing);
        
        if(eo_nv_ownership_local == ownership)
        {   // use nv for data
            uint16_t sss;
            eo_nv_Get(nv, eo_nv_strg_volatile, rop->stream.data, &sss);     
            rop->stream.head.dsiz = sss;           
        }
        else
        {   // use ropdescr for data
            if(NULL != ropdescr->data)
            {
                rop->stream.head.dsiz = eo_nv_Size(nv);
                memcpy(rop->stream.data, ropdescr->data, rop->stream.head.dsiz);  
            }   
            else
            {
                return(eores_NOK_generic);
            }            
        }
    }
    
    // assign sign
    rop->stream.sign = (1 == ropdescr->configuration.plussign) ? (ropdescr->signature) : (EOK_uint32dummy);

    // assign time
    rop->stream.time = (0 == ropdescr->configuration.plustime) ? (EOK_uint64dummy) : (eov_sys_LifeTimeGet(eov_sys_GetHandle()));

    if(NULL != requiredbytes)
    {
        uint16_t dataeffectivebytes = (rop->stream.head.dsiz + 3) / 4;
        dataeffectivebytes *= 4;        
        *requiredbytes = sizeof(eOrophead_t) + dataeffectivebytes + (4*rop->stream.head.ctrl.plussign) + (8*rop->stream.head.ctrl.plustime);
    }

    return(eores_OK);
}
#endif


//#if 0
// extern eOresult_t eo_agent_OutROPrefresh(EOtheAgent *p, EOrop *rop)
// {

//     if((NULL == p) || (NULL == rop))
//     {
//         return(eores_NOK_nullpointer);
//     } 

// //     // if not found, then error because must have been searched before
// //     if(NULL == rop->tmpdata.nvtreenoderoot)
// //     {
// //         return(eores_NOK_generic);
// //     }
//     //#warning --> i remove a check ... think of how to put it back again. maybe vs. ropcode. 

//     
//     // ok, now we are ready to update the rop
//  
//     // datainfo
//     if(eobool_true == eo_rop_hid_DataField_is_Present(&rop->stream.head))
//     {   // update also rop->datasize
//         uint16_t sss;
//         eo_nv_Get(&rop->netvar, eo_nv_strg_volatile, rop->stream.data, &sss);
//         rop->stream.head.dsiz = sss;
//     }
//  
//     
// //    // signature
// //     if(1 == rop->stream.head.ctrl.plussign)
// //     {
// //         rop->stream.sign = (NULL != sign) ? (*sign) : (EOK_uint32dummy);
// //     }

//     // time
//     if(1 == rop->stream.head.ctrl.plustime)
//     {
//         rop->stream.time = eov_sys_LifeTimeGet(eov_sys_GetHandle());
//     }


//     return(eores_OK);

// }
// #endif


// extern eOresult_t eo_agent_OutROPfromNVold(EOtheAgent *p, EOnv* nv, eOropcode_t ropc, eOropconfig_t ropcfg, EOrop *rop, uint16_t *requiredbytes)
// {
// #if defined(EO_NV_EMBED_FUNTYP_IN_ID)
//     eOnvType_t typ = eo_nv_TYP_NO4;
//     eOnvFunc_t fun = eo_nv_FUN_NO0;
// #endif
//     eOrophead_t rophead;
// //    eOresult_t res;
//     uint16_t nvid;
//     uint16_t nvep;

//     if((NULL == p) || (NULL == rop) || (NULL == nv))
//     {
//         return(eores_NOK_nullpointer);
//     } 
//     
//     nvid = nv->con->id;
//     nvep = nv->ep;

//     // reset the rop
//     eo_rop_Reset(rop);  
//     
//     // put in rophead all the options
//     rophead.ctrl.ffu        = 0;
//     rophead.ctrl.confinfo   = eo_ropconf_none; // cannot do a ack/ack
//     rophead.ctrl.plustime   = (eobool_true == ropcfg.plustime) ? (1) : (0);
//     rophead.ctrl.plussign   = (eobool_true == ropcfg.plussign) ? (1) : (0);
//     rophead.ctrl.rqsttime   = (eobool_true == ropcfg.timerqst) ? (1) : (0);
//     rophead.ctrl.rqstconf   = (eobool_true == ropcfg.confrqst) ? (1) : (0);
//     rophead.ctrl.userdefn   = 0;
//     rophead.ropc            = ropc;
//     rophead.endp            = nvep;
//     rophead.nvid            = nvid;
//     rophead.dsiz            = 0;


//     // check validity of ropc 
//     if((ropc >= (uint8_t)eo_ropcodevalues_numberofthem) || (ropc == (uint8_t)eo_ropcode_none))
//     {
//         return(eores_NOK_generic);
//     }
//     else if((ropc == (uint8_t)eo_ropcode_usr))
//     {
//         // not managed yet
//         return(eores_NOK_generic);
//     }

// #if defined(EO_NV_EMBED_FUNTYP_IN_ID)
//     // check validity of nvid
//     fun = eo_nv_hid_fromIDtoFUN(nvid);
//     typ = eo_nv_hid_fromIDtoTYP(nvid);

//     if((eo_nv_FUN_NO0 == fun) || (eo_nv_FUN_NO1 == fun) || (eo_nv_TYP_NO4 == typ) ||(eo_nv_TYP_NO5 == typ))
//     {
//         // incorrect nvid
//         return(eores_NOK_generic);
//     }
// #endif

// //    rop->aboutip.ipaddr = toipaddr;

//     // get the ownership    
// //    rop->tmpdata.nvset = nvset;
// //    rop->tmpdata.nvownership = eo_rop_hid_GetOwnership(ropc, eo_ropconf_none, eo_rop_dir_outgoing);
// //
// //     // retrieve the indices inside the nvset given the triple (ip, ep, id)
// //      res = eo_nvset_GetIndices( rop->tmpdata.nvset,  
// //                                 (eo_nv_ownership_local == rop->tmpdata.nvownership) ? (eok_ipv4addr_localhost) : (toipaddr), 
// //                                 endpoint, nvid, 
// //                                 &rop->tmpdata.ondevindex,
// //                                 &rop->tmpdata.onendpointindex,
// //                                 &rop->tmpdata.onidindex); 
// //
// //     // if the nvset does not have the triple (ip, ep, id) then we return an error because we cannot form the rop
// //     if(eores_OK != res)
// //     {
// //         return(eores_NOK_generic);
// //     }
// //
// //    // we need a treenode of the nv
// //    rop->tmpdata.nvtreenoderoot = eo_nvset_GetTreeNode(rop->tmpdata.nvset, rop->tmpdata.ondevindex, rop->tmpdata.onendpointindex, rop->tmpdata.onidindex);

//    
//     // --- ok: now we are ready to prepare the rop
//     
//     // head
//     memcpy(&rop->stream.head, &rophead, sizeof(eOrophead_t));
//     
//     // data
//     if(eobool_true == eo_rop_hid_DataField_is_Required(&rophead))
//     {
//         // we also need the nv
//         uint16_t sss;
// //        eo_nvset_GetNV(rop->tmpdata.nvset, rop->tmpdata.ondevindex, rop->tmpdata.onendpointindex, rop->tmpdata.onidindex, rop->tmpdata.nvtreenoderoot, &rop->netvar);
// //        eo_nv_Get(&rop->netvar, eo_nv_strg_volatile, rop->stream.data, &sss);
//         eo_nv_Get(nv, eo_nv_strg_volatile, rop->stream.data, &sss);
//         rop->stream.head.dsiz = sss;
//     }
//     
//     // reset sign
//     rop->stream.sign = EOK_uint32dummy;

//     // reset time
//     rop->time = EOK_uint64dummy;

//     if(NULL != requiredbytes)
//     {
//         *requiredbytes = sizeof(eOrophead_t) + rop->stream.head.dsiz + (4*rop->stream.head.ctrl.plussign) + (8*rop->stream.head.ctrl.plustime);
//     }


//     return(eores_OK);
// }

// static void s_eo_agent_on_rop_confirmation_received(eOipv4addr_t ipaddr, eOropcode_t ropc, eOnvID_t nvid, eOnvEP_t endp
//                                                     uint32_t sign, eOabstime_t time, eOropconfinfo_t confinfo)
// {
//     char str[64];
//     snprintf(str, sizeof(str)-1, "rx %s for opc %d and nvid 0x%xd from ip %d", (eo_ropconf_ack == confinfo) ? ("ack") : ("nak"), ropc, nvid, ipaddr);
//     eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, s_eobj_ownname, str);    
// }


// static void s_eo_agent_on_rop_confirmation_requested(eOipv4addr_t ipaddr, eOropcode_t ropc, eOnvID_t nvid, eOnvEP_t endp
//                                                      uint32_t sign, eOabstime_t time)
// {
//     char str[64];
//     snprintf(str, sizeof(str)-1, "rqst conf for opc %d and nvid 0x%x from ip %d", ropc, nvid, ipaddr);
//     eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, s_eobj_ownname, str);        
// }





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




