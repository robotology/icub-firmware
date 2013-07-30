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
#include "EOVtheSystem.h"
#include "EOtheErrorManager.h"
#include "EOvector.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtransmitter.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtransmitter_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if defined(EO_TAILOR_CODE_FOR_ARM)
//    #define EONV_DONT_USE_EOV_MUTEX_FUNCTIONS
#endif


#if defined(EONV_DONT_USE_EOV_MUTEX_FUNCTIONS)
    #define eov_mutex_Take(a, b)   
    #define eov_mutex_Release(a)
#endif



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

//static eOresult_t s_eo_transmitter_listmatching_rule(void *item, void *param);

static eOresult_t s_eo_transmitter_ropmatchingrule_rule(void *item, void *param);

static void s_eo_transmitter_list_updaterop_in_ropframe(void *item, void *param);

static void s_eo_transmitter_list_shiftdownropinfo(void *item, void *param);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOtransmitter";

const eo_transmitter_cfg_t eo_transmitter_cfg_default = 
{
    EO_INIT(.capacityoftxpacket)            512, 
    EO_INIT(.capacityofropframeregulars)    256, 
    EO_INIT(.capacityofropframeoccasionals) 256,
    EO_INIT(.capacityofropframereplies)     256,
    EO_INIT(.capacityofrop)                 128, 
    EO_INIT(.maxnumberofregularrops)        16,
    EO_INIT(.maxnumberofconfreqrops)        16,
    EO_INIT(.nvset)                         NULL,
    EO_INIT(.confmanager)                   NULL,
    EO_INIT(.ipv4addr)                      EO_COMMON_IPV4ADDR_LOCALHOST,
    EO_INIT(.ipv4port)                      10001,
    EO_INIT(.protection)                    eo_transmitter_protection_none,
    EO_INIT(.mutex_fn_new)                  NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

 
extern EOtransmitter* eo_transmitter_New(const eo_transmitter_cfg_t *cfg)
{
    EOtransmitter *retptr = NULL;   

    if(NULL == cfg)
    {    
        cfg = &eo_transmitter_cfg_default;
    }
    
    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOtransmitter), 1);
    
    retptr->txpacket                = eo_packet_New(cfg->capacityoftxpacket);
    retptr->ropframereadytotx       = eo_ropframe_New();
    retptr->ropframeregulars        = eo_ropframe_New();
    retptr->ropframeoccasionals     = eo_ropframe_New();
    retptr->ropframereplies         = eo_ropframe_New();
    retptr->roptmp                  = eo_rop_New(cfg->capacityofrop);
    retptr->nvset                   = cfg->nvset;
    retptr->theagent                = eo_agent_Initialise();
    retptr->confmanager             = cfg->confmanager;
    retptr->ipv4addr                = cfg->ipv4addr;
    retptr->ipv4port                = cfg->ipv4port;
    retptr->bufferropframeregulars  = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, cfg->capacityofropframeregulars, 1);
    retptr->bufferropframeoccasionals = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, cfg->capacityofropframeoccasionals, 1);
    retptr->bufferropframereplies   = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, cfg->capacityofropframereplies, 1);
    retptr->listofregropinfo        = (0 == cfg->maxnumberofregularrops) ? (NULL) : (eo_list_New(sizeof(eo_transm_regrop_info_t), cfg->maxnumberofregularrops, NULL, 0, NULL, NULL));
    retptr->currenttime             = 0;
    retptr->tx_seqnum               = 0;
    retptr->confrequests            = (NULL == retptr->confmanager) ? (NULL) : (eo_vector_New(sizeof(eOropdescriptor_t), cfg->maxnumberofconfreqrops, NULL, NULL, NULL, NULL));

    eo_ropframe_Load(retptr->ropframeregulars, retptr->bufferropframeregulars, eo_ropframe_sizeforZEROrops, cfg->capacityofropframeregulars);
    eo_ropframe_Clear(retptr->ropframeregulars);
    eo_ropframe_Load(retptr->ropframeoccasionals, retptr->bufferropframeoccasionals, eo_ropframe_sizeforZEROrops, cfg->capacityofropframeoccasionals);
    eo_ropframe_Clear(retptr->ropframeoccasionals);
    eo_ropframe_Load(retptr->ropframereplies, retptr->bufferropframereplies, eo_ropframe_sizeforZEROrops, cfg->capacityofropframereplies);
    eo_ropframe_Clear(retptr->ropframereplies);


    {   // we set the content of ropframereadytotx with the same memory used by txpacket, so that when we operate on 
        // ropframereadytotx then we prepare the txpacket.
        uint8_t *data;
        uint16_t size;
        uint16_t capacity;
        
        eo_packet_Payload_Get(retptr->txpacket, &data, &size);
        eo_packet_Capacity_Get(retptr->txpacket, &capacity);
    
        eo_ropframe_Load(retptr->ropframereadytotx, data, eo_ropframe_sizeforZEROrops, capacity); // dont use size because size is now zero.
        eo_ropframe_Clear(retptr->ropframereadytotx);
        
        if(eobool_true != eo_ropframe_IsValid(retptr->ropframereadytotx))
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "the ropframeready2tx is not valid... cannot continue");
        }

        // the destination ipv4addr and ipv4port are constant and are the ones passed through configuration
        eo_packet_Addressing_Set(retptr->txpacket, retptr->ipv4addr, retptr->ipv4port);
    } 

    if((NULL != cfg->mutex_fn_new) && (eo_transmitter_protection_total == cfg->protection))
    {
        retptr->mtx_replies     = cfg->mutex_fn_new();
        retptr->mtx_regulars    = cfg->mutex_fn_new();
        retptr->mtx_occasionals = cfg->mutex_fn_new();        
    }
    else
    {
        retptr->mtx_replies     = NULL;
        retptr->mtx_regulars    = NULL;
        retptr->mtx_occasionals = NULL;
    }
    
#if defined(USE_DEBUG_EOTRANSMITTER)
    // DEBUG
    retptr->DEBUG.txropframeistoobigforthepacket = 0;
#endif
    
    return(retptr);
}






extern eOresult_t eo_transmitter_regular_rops_Load(EOtransmitter *p, eOropdescriptor_t* ropdesc)
{
    eo_transm_regrop_info_t regropinfo;
    eOropdescriptor_t ropdescriptor;
    eOresult_t res;
    uint16_t usedbytes;
    uint16_t remainingbytes;
    uint16_t ropstarthere;
    uint16_t ropsize;
    EOnv nv;
    EOnv* tmpnvptr = NULL;
    eOnvOwnership_t nvownership;

    if((NULL == p) || (NULL == ropdesc)) 
    {
        return(eores_NOK_nullpointer);
    }  

    if(NULL == p->listofregropinfo)
    {
        // in such a case there is room for regular rops (for instance because the cfg->maxnumberofregularrops is zero)
        return(eores_NOK_generic);
    }
    
    
    eov_mutex_Take(p->mtx_regulars, eok_reltimeINFINITE);

    // work on the list ... 
    
    if(eobool_true == eo_list_Full(p->listofregropinfo))
    {
        eov_mutex_Release(p->mtx_regulars);
        return(eores_NOK_generic);
    }
    

    // for searching inside listofregropinfo we need only those three fields: ropcode, id, ep
    ropdescriptor.ropcode   = ropdesc->ropcode;
    ropdescriptor.id32      = ropdesc->id32;

    
    // search for ropcode+ep+id. if found, then ... return OK and dont do anything because it means that the rop is already inside
    if(NULL != eo_list_Find(p->listofregropinfo, s_eo_transmitter_ropmatchingrule_rule, &ropdescriptor))
    {   // it is already inside ...
        eov_mutex_Release(p->mtx_regulars);
        return(eores_NOK_generic);
    }    
    
    // else ... prepare a temporary variable eo_transm_regrop_info_t to be put inside the list.
    // and wait success of rop + insetrtion in frame
    
    memcpy(&ropdescriptor, ropdesc, sizeof(eOropdescriptor_t));
    ropdescriptor.control.rqstconf  = 0;                // VERY IMPORTANT: the regulars cannot ask for confirmation.
    ropdescriptor.control.confinfo  = eo_ropconf_none;  // VERY IMPORTANT: the regulars cannot be a ack/nack
    ropdescriptor.control.version   = EOK_ROP_VERSION_0;
    
    nvownership = eo_rop_hid_GetOwnership(ropdescriptor.ropcode, eo_ropconf_none, eo_rop_dir_outgoing);
      
    res = eo_nvset_NV_Get(  (p->nvset),  
                            (eo_nv_ownership_local == nvownership) ? (eok_ipv4addr_localhost) : (p->ipv4addr), 
                            ropdescriptor.id32,
                            &nv
                            );   

    // if the nvset does not have the triple (ip, ep, id) then we return an error because we cannot form the rop
    if(eores_OK != res)
    {
        return(eores_NOK_generic);
    } 

    // force size to be coherent with the nv. the size is always used, even if there is no data to transmit
    ropdescriptor.size = eo_nv_Size(&nv);    
    
    // now we have the nv. we set its value in local ram
    if(eobool_true == eo_rop_hid_OPChasData(ropdescriptor.ropcode))
    {  
        if(eo_nv_ownership_local == nvownership)
        {   // if the nv is local, then take data from nv, thus no need to write the data field of the nv using ropdescriptor.data.
            ropdescriptor.data = NULL;   // set ropdescriptor.data to NULL to force eo_agent_OutROPfromNV() to get data from EOnv
        }
        else
        {   // if the nv is remote, then the data must be passed inside ropdescriptor.data
            
            // so far we dont support that the device regularly sends commands such as set<remotevar, value>. it can send ask<remotevar> however.
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "eo_transmitter_regular_rops_Load() so far cannot load a ROP w/ payload onto a variable remotely owned");
            
            if(NULL == ropdescriptor.data)
            {
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "eo_transmitter_regular_rops_Load() cannot have a NULL ropdes->data with remote ownership");
            }          
        }
    }
    else
    {   // dont need to send data
        ropdescriptor.data = NULL;
    }

       
    res = eo_agent_OutROPprepare(p->theagent, &nv, &ropdescriptor, p->roptmp, &usedbytes);   
    
    // if we cannot prepare the rop ... we quit
    if(eores_OK != res)
    {
        eov_mutex_Release(p->mtx_regulars);
        return(res);
    }
    // extract the reference to the associated netvar
    tmpnvptr = eo_rop_hid_NV_Get(p->roptmp);
    

    // 2. put the rop inside the ropframe
    res = eo_ropframe_ROP_Add(p->ropframeregulars, p->roptmp, &ropstarthere, &ropsize, &remainingbytes);
    // if we cannot add the rop we quit
    if(eores_OK != res)
    {
        eov_mutex_Release(p->mtx_regulars);
        return(res);
    }
    
    
    // 3. prepare a regropinfo variable to be put inside the list    
    
    regropinfo.ropcode                  = ropdescriptor.ropcode;    
    regropinfo.hasdata2update           = eo_rop_hid_DataField_is_Present(&(p->roptmp->stream.head)); 
    regropinfo.ropstarthere             = ropstarthere;
    regropinfo.ropsize                  = ropsize;
    regropinfo.timeoffsetinsiderop      = (0 == p->roptmp->stream.head.ctrl.plustime) ? (EOK_uint16dummy) : (ropsize - 8); //if we have time, then it is in teh last 8 bytes
    memcpy(&regropinfo.thenv, tmpnvptr, sizeof(EOnv));


    // 4. finally push back regropinfo inside the list.
    eo_list_PushBack(p->listofregropinfo, &regropinfo);
    
    eov_mutex_Release(p->mtx_regulars);    
    return(eores_OK);   
}


extern eOresult_t eo_transmitter_regular_rops_Unload(EOtransmitter *p, eOropdescriptor_t* ropdesc)//eOropcode_t ropcode, eOnvEP_t nvep, eOnvID_t nvid)
{
    eo_transm_regrop_info_t regropinfo;
    eOropdescriptor_t ropdescriptor;
    EOlistIter *li = NULL;

    if(NULL == p) 
    {
        return(eores_NOK_nullpointer);
    }  

    if(NULL == p->listofregropinfo)
    {
        // in such a case there is room for regular rops (for instance because the cfg->maxnumberofregularrops is zero)
        return(eores_NOK_generic);
    }

    // work on the list ... 
    
    eov_mutex_Take(p->mtx_regulars, eok_reltimeINFINITE);
    
    if(eobool_true == eo_list_Empty(p->listofregropinfo))
    {
        eov_mutex_Release(p->mtx_regulars);
        return(eores_NOK_generic);
    }
    
    // need only ropcode, ep and id to search for inside the list listofregropinfo
    ropdescriptor.ropcode       = ropdesc->ropcode;
    ropdescriptor.id32          = ropdesc->id32;

      
    // search for ropcode+nvep+nvid. if not found, then ... return OK and dont do anything.
    li = eo_list_Find(p->listofregropinfo, s_eo_transmitter_ropmatchingrule_rule, &ropdescriptor);
    if(NULL == li)
    {   // it is not inside ...
        eov_mutex_Release(p->mtx_regulars);
        return(eores_NOK_generic);
    }
    
    // copy what is inside the list into a temporary variable
    memcpy(&regropinfo, eo_list_At(p->listofregropinfo, li), sizeof(eo_transm_regrop_info_t));
    
    // for each element after li: (name is afterli) retrieve it and modify its content so that ropstarthere is decremented by regropinfo.ropsize ...
    eo_list_FromIterForEach(p->listofregropinfo, eo_list_Next(p->listofregropinfo, li), s_eo_transmitter_list_shiftdownropinfo, &regropinfo);
    
    // remove the element indexedby li
    eo_list_Erase(p->listofregropinfo, li);
    
    // inside the p->ropframeregulars: remove a rop of regropinfo.ropsize whcih starts at regropinfo.ropstartshere. use a _friend method in here defined.
    //                                  you must: decrement the nrops by 1, decrement the size by regropinfo.ropsize, ... else in header and private variable ...
    //                                            and finally make a memmove down by regropinfo.ropsize.
    
    eo_ropframe_ROP_Rem(p->ropframeregulars, regropinfo.ropstarthere, regropinfo.ropsize);
  

    eov_mutex_Release(p->mtx_regulars);
    
    return(eores_OK);   
}


extern eOresult_t eo_transmitter_regular_rops_Clear(EOtransmitter *p)
{
    if(NULL == p) 
    {
        return(eores_NOK_nullpointer);
    }  

    if(NULL == p->listofregropinfo)
    {
        // in such a case there is room for regular rops (for instance because the cfg->maxnumberofregularrops is zero)
        return(eores_OK);
    }
    
    eov_mutex_Take(p->mtx_regulars, eok_reltimeINFINITE);
    
    if(eobool_true == eo_list_Empty(p->listofregropinfo))
    {
        eov_mutex_Release(p->mtx_regulars);
        return(eores_OK);
    } 
    
    eo_list_Clear(p->listofregropinfo);
    
    eo_ropframe_Clear(p->ropframeregulars);

    eov_mutex_Release(p->mtx_regulars);
    
    return(eores_OK);   
}

extern eOresult_t eo_transmitter_regular_rops_Refresh(EOtransmitter *p)
{
    if(NULL == p) 
    {
        return(eores_NOK_nullpointer);
    }  

    if(NULL == p->listofregropinfo)
    {
        // in such a case there is room for regular rops (for instance because the cfg->maxnumberofregularrops is zero)
        return(eores_OK);
    }
    
    eov_mutex_Take(p->mtx_regulars, eok_reltimeINFINITE);
    
    if(eobool_true == eo_list_Empty(p->listofregropinfo))
    {
        eov_mutex_Release(p->mtx_regulars);
        return(eores_OK);
    } 
    
    p->currenttime = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    
    // for each element in the list ... i do: ... see function
    eo_list_ForEach(p->listofregropinfo, s_eo_transmitter_list_updaterop_in_ropframe, p);

    eov_mutex_Release(p->mtx_regulars);
    
    return(eores_OK);   
}

extern eOresult_t eo_transmitter_outpacket_Get(EOtransmitter *p, EOpacket **outpkt, uint16_t *numberofrops)
{
    uint16_t remainingbytes;
    uint16_t size;
    uint16_t rops2tx = 0;

    if((NULL == p) || (NULL == outpkt)) 
    {
        return(eores_NOK_nullpointer);
    }

    
    // clear the content of the ropframe to transmit which uses the same storage of the packet ...
    eo_ropframe_Clear(p->ropframereadytotx);
    
    // add to it the ropframe of regulars. keep it afterwards. dont clear it !!!
    eov_mutex_Take(p->mtx_regulars, eok_reltimeINFINITE);
    eo_ropframe_Append(p->ropframereadytotx, p->ropframeregulars, &remainingbytes);
    eov_mutex_Release(p->mtx_regulars);

    // add the ropframe of occasionals ... and then clear it
    eov_mutex_Take(p->mtx_occasionals, eok_reltimeINFINITE);
    eo_ropframe_Append(p->ropframereadytotx, p->ropframeoccasionals, &remainingbytes);
    eo_ropframe_Clear(p->ropframeoccasionals);
    eov_mutex_Release(p->mtx_occasionals);

    // add the ropframe of replies ... and then clear it
    eov_mutex_Take(p->mtx_replies, eok_reltimeINFINITE);
    eo_ropframe_Append(p->ropframereadytotx, p->ropframereplies, &remainingbytes);
    eo_ropframe_Clear(p->ropframereplies);
    eov_mutex_Release(p->mtx_replies);

    // now add the age of the frame
    eo_ropframe_age_Set(p->ropframereadytotx, eov_sys_LifeTimeGet(eov_sys_GetHandle()));
    
    // and get teh number of rops to tx
    
    rops2tx = eo_ropframe_ROP_NumberOf(p->ropframereadytotx);
    
    // add sequence number
    if(0 != rops2tx)
    {
    	p->tx_seqnum++;
    	eo_ropframe_seqnum_Set(p->ropframereadytotx, p->tx_seqnum);
    }
    // now set the size of the packet according to what is inside the ropframe.
    eo_ropframe_Size_Get(p->ropframereadytotx, &size);
    eo_packet_Size_Set(p->txpacket, size);

#if defined(USE_DEBUG_EOTRANSMITTER)    
    {   // DEBUG    
        uint16_t capacity = 0;
        eo_packet_Capacity_Get(p->txpacket, &capacity);   
        if(size > capacity)
        {
            p->DEBUG.txropframeistoobigforthepacket ++;
        }
    }
#endif
    
    // finally gives back the packet
    *outpkt = p->txpacket;

    // and the number of contained rops
    if(NULL != numberofrops)
    {
       *numberofrops = rops2tx;
    }
    
    // if the confirmation manager is active and there are rops which require a confirmation request ... call the on-tx function
    //if((NULL != p->confmanager) && (NULL != p->confrequests) && (0 != eo_vector_Size(p->confrequests)))
    if(0 != eo_vector_Size(p->confrequests))
    {
        uint16_t size = eo_vector_Size(p->confrequests);
        uint16_t i=0;
        for(i=0; i<size; i++)
        {
            eOropdescriptor_t *ropdes = (eOropdescriptor_t*) eo_vector_At(p->confrequests, i);
            eo_confman_Confirmation_Requested(p->confmanager, p->ipv4addr, ropdes);            
        }       
        eo_vector_Clear(p->confrequests);   // remove the conf requests
    }
    
    return(eores_OK);   
}


 
extern eOresult_t eo_transmitter_occasional_rops_Load(EOtransmitter *p, eOropdescriptor_t* ropdesc)
{
    eOnvOwnership_t nvownership;
    eOresult_t res;
    uint16_t usedbytes;
    uint16_t ropsize;
    uint16_t remainingbytes;   
    EOnv nv;
    
    if((NULL == p) || (NULL == ropdesc)) 
    {
        return(eores_NOK_nullpointer);
    }  
     
    
    nvownership = eo_rop_hid_GetOwnership(ropdesc->ropcode, eo_ropconf_none, eo_rop_dir_outgoing);
      
    res = eo_nvset_NV_Get(  (p->nvset),  
                            (eo_nv_ownership_local == nvownership) ? (eok_ipv4addr_localhost) : (p->ipv4addr), 
                            ropdesc->id32,
                            &nv
                            );   

    // if the nvset does not have the triple (ip, ep, id) then we return an error because we cannot form the rop
    if(eores_OK != res)
    {
        return(eores_NOK_generic);
    } 

    // force size to be coherent with the nv. the size is always used, even if there is no dat to transmit
    ropdesc->size = eo_nv_Size(&nv);    
    
    // now we have the nv. we set its value in local ram
    if(eobool_true == eo_rop_hid_OPChasData(ropdesc->ropcode))
    {  
        if(eo_nv_ownership_local == nvownership)
        {   // if the nv is local, then take data from nv, thus no need to write the data field of the nv using ropdesc->data.
            ropdesc->data = NULL;   // set ropdesc->data to NULL to force eo_agent_OutROPfromNV() to get data from EOnv
        }
        else
        {   // if the nv is remote, then the data must be passed inside ropdesc->data
            if(NULL == ropdesc->data)
            {
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "eo_transmitter_occasional_rops_Load() cannot have a NULL ropdes->data with remote ownership");
            }          
        }
    }
    else
    {   // dont need to send data
        ropdesc->data = NULL;
    }


    eov_mutex_Take(p->mtx_occasionals, eok_reltimeINFINITE);
       
    res = eo_agent_OutROPprepare(p->theagent, &nv, ropdesc, p->roptmp, &usedbytes);    
    
    if(eores_OK != res)
    {
        eov_mutex_Release(p->mtx_occasionals);
        return(res);
    }

    // put the rop inside the ropframe
    res = eo_ropframe_ROP_Add(p->ropframeoccasionals, p->roptmp, NULL, &ropsize, &remainingbytes);
    
    // if the confirmation manager is active and a conf request is flagged on ...
    if((NULL != p->confmanager) && (NULL != p->confrequests) && (eobool_false == eo_vector_Full(p->confrequests)))
    {
        eo_vector_PushBack(p->confrequests, ropdesc);
    }
    
    
    eov_mutex_Release(p->mtx_occasionals);
   
    
    return(res);   
}
 

extern eOresult_t eo_transmitter_reply_ropframe_Load(EOtransmitter *p, EOropframe* ropframe)
{
    eOresult_t res;
    uint16_t remainingbytes;

    if(NULL == p) 
    {
        return(eores_NOK_nullpointer);
    }  

    eov_mutex_Take(p->mtx_replies, eok_reltimeINFINITE);
    res = eo_ropframe_Append(p->ropframereplies, ropframe, &remainingbytes);
    eov_mutex_Release(p->mtx_replies);
    
    // replies cannot have a conf request flagged on, then there is no insertion inside the p->confrequests

    return(res);     
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static eOresult_t s_eo_transmitter_ropmatchingrule_rule(void *item, void *param)
{
    eo_transm_regrop_info_t *inside = (eo_transm_regrop_info_t*)item;
    eOropdescriptor_t *targetrop = (eOropdescriptor_t*)param;

    if((inside->thenv.id32 == targetrop->id32) && (inside->ropcode == targetrop->ropcode))
    {
        return(eores_OK);
    }
    else
    {
        return(eores_NOK_generic);
    }
}


static void s_eo_transmitter_list_updaterop_in_ropframe(void *item, void *param)
{
    eo_transm_regrop_info_t *inside = (eo_transm_regrop_info_t*)item;
    EOtransmitter *p = (EOtransmitter*)param;
    
    uint8_t *origofrop;
    uint8_t *dest;
    
    // retrieve the beginning of the ropstream inside the ropframe
    origofrop = eo_ropframe_hid_get_pointer_offset(p->ropframeregulars, inside->ropstarthere);
    dest = origofrop + sizeof(eOrophead_t);

    // if it has a data field ... copy from the nv to the ropstream
    if(eobool_true == inside->hasdata2update)
    {
        // by using eo_nv_hid_Fast_LocalMemoryGet() we use the protection which is configured
        // by the EOnvscfg object, and the concurrent access to the netvar is managed
        // internally the nv object.
        eo_nv_hid_Fast_LocalMemoryGet(&inside->thenv, dest);
        
        // with memcpy the copy from local buffer to dest is not protected, thus data format may be corrupt
        // in case any concurrent task is in the process of writing the local buffer.
//        memcpy(dest, inside->nvloc, inside->capacity);
//        // for debug: memset(dest, 0xaa, inside->capacity); 
    }
    
    
    // if it has a time field ... copy from the current time to the ropstream
    if(EOK_uint16dummy != inside->timeoffsetinsiderop)
    {
        eOabstime_t *time = (eOabstime_t*) (&origofrop[inside->timeoffsetinsiderop]);
        //*time = p->currenttime;
        memcpy(time, &p->currenttime, sizeof(eOabstime_t));
    }
    
//    // if it requires a confirmation ... but it never requires a confirmation. 
//     if(...)
//     {
//         
//     }

}


static void s_eo_transmitter_list_shiftdownropinfo(void *item, void *param)
{
    eo_transm_regrop_info_t *after = (eo_transm_regrop_info_t*)item;
    eo_transm_regrop_info_t *theone2beremoved = (eo_transm_regrop_info_t*)param;

    // for each element after: modify its content so that ropstarthere is decremented by theone2beremoved.ropsize

    after->ropstarthere -= theone2beremoved->ropsize;
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




