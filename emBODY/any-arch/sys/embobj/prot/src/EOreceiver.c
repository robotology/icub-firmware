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





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOreceiver.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOreceiver_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
extern void eo_receiver_callback_incaseoferror_in_sequencenumberReceived(uint64_t rec_seqnum, uint64_t expected_seqnum);


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

//static const char s_eobj_ownname[] = "EOreceiver";

const eo_receiver_cfg_t eo_receiver_cfg_default = 
{
    EO_INIT(.capacityofropframereply)   256, 
    EO_INIT(.capacityofropinput)        128, 
    EO_INIT(.capacityofropreply)        128, 
    EO_INIT(.nvscfg)                    NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOreceiver* eo_receiver_New(const eo_receiver_cfg_t *cfg)
{
    EOreceiver *retptr = NULL;   

    if(NULL == cfg)
    {    
        cfg = &eo_receiver_cfg_default;
    }
    
    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOreceiver), 1);
    
    retptr->ropframeinput       = eo_ropframe_New();
    retptr->ropframereply       = eo_ropframe_New();
    retptr->ropinput            = eo_rop_New(cfg->capacityofropinput);
    retptr->ropreply            = eo_rop_New(cfg->capacityofropreply);
    retptr->nvscfg              = cfg->nvscfg;
    retptr->theagent            = eo_agent_Initialise(NULL);
    retptr->ipv4addr            = 0;
    retptr->ipv4port            = 0;
    retptr->bufferropframereply = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, cfg->capacityofropframereply, 1);
    retptr->rx_seqnum           = eok_uint64dummy;
    // now we need to allocate the buffer for the ropframereply
    
    eo_ropframe_Load(retptr->ropframereply, retptr->bufferropframereply, 0, cfg->capacityofropframereply);
    
    return(retptr);
}


extern eOresult_t eo_receiver_Process(EOreceiver *p, EOpacket *packet, EOnvsCfg *nvscfg, uint16_t *numberofrops, eObool_t *thereisareply, eOabstime_t *transmittedtime)
{
    uint16_t rxremainingbytes = 0;
    uint16_t txremainingbytes = 0;
    uint8_t* payload;
    uint16_t size;
    uint16_t capacity;
    uint16_t nrops;
    uint16_t i;
    eOresult_t res;
    EOnvsCfg *nvs2use = NULL;
    eOipv4addr_t remipv4addr;
    eOipv4port_t remipv4port;
    uint64_t rec_seqnum;

    
    if((NULL == p) || (NULL == packet)) 
    {
        return(eores_NOK_nullpointer);
    }
    
    if(NULL != nvscfg)
    {
        nvs2use = nvscfg;
    }
    else
    {
        nvs2use = p->nvscfg;
    }

    if(NULL == nvs2use)
    {
        return(eores_NOK_nullpointer);
    }
    
    // clear the ropframereply w/ eo_ropframe_Clear(). the clear operation also makes it safe to manipulate p->ropframereplay with *_quickversion
    
    eo_ropframe_Clear(p->ropframereply);
    
    
    // we get the ip address and port of the incoming packet.
    // the remaddr can be any. however, if the eo_receiver_Process() is called by the EOtransceiver, it will be only the one of the remotehost
    eo_packet_Addressing_Get(packet, &remipv4addr, &remipv4port);
    
   
    // then we assign them to the ones of the EOreceiver. by doing so we force the receive to accept packets from everyboby.

    //p->ipv4addr = remipv4addr;
    //p->ipv4port = remipv4port;
    
    // retrieve payload from the incoming packet and load the ropframe with it
    eo_packet_Payload_Get(packet, &payload, &size);
    eo_packet_Capacity_Get(packet, &capacity);
    eo_ropframe_Load(p->ropframeinput, payload, size, capacity);
    
    // verify if the ropframeinput is valid w/ eo_ropframe_IsValid()
    if(eobool_false == eo_ropframe_IsValid(p->ropframeinput))
    {
        if(NULL != thereisareply)
        {
            *thereisareply = eobool_false;
        }
        return(eores_NOK_generic);
    }
    
    //check sequnece number
    rec_seqnum = eo_ropframe_seqnum_Get(p->ropframeinput);
    
    if(p->rx_seqnum == eok_uint64dummy)
    {
        //this is the first received ropframe
        p->rx_seqnum = rec_seqnum;
    }
    else
    {
        if(rec_seqnum != (p->rx_seqnum+1))
        {
            eo_receiver_callback_incaseoferror_in_sequencenumberReceived(rec_seqnum, p->rx_seqnum+1);
        }
        p->rx_seqnum = rec_seqnum;
    }
    // for every rop inside with eo_ropframe_ROP_NumberOf() :
    //nrops = eo_ropframe_ROP_NumberOf(p->ropframeinput);
    // i have already verified validity of p->ropframeinput, thus i can use the quickversion
    nrops = eo_ropframe_ROP_NumberOf_quickversion(p->ropframeinput);
    
    for(i=0; i<nrops; i++)
    {
        // - get teh rop w/ eo_ropframe_ROP_Get()
        
        res = eo_ropframe_ROP_Get(p->ropframeinput, p->ropinput, &rxremainingbytes);
        
        if(eores_OK != res)
        {
            break;
        }        
        
        // - use the agent w/ eo_agent_InpROPprocess() and retrieve the ropreply. 
        //   we need to tell the agent what nvs database we are using and from where the rop is coming 
        
        eo_agent_InpROPprocess(p->theagent, p->ropinput, nvs2use, remipv4addr, p->ropreply);
        
        // - if ropreply is ok w/ eo_rop_GetROPcode() then add it to ropframereply w/ eo_ropframe_ROP_Set()
        
        if(eo_ropcode_none != eo_rop_GetROPcode(p->ropreply))
        {
            res = eo_ropframe_ROP_Set(p->ropframereply, p->ropreply, NULL, NULL, &txremainingbytes);
        }
        
        // we keep on decoding eve if we cannot put a reply into the ropframe 
        //if(0 == rxremainingbytes)
        //{
        //    break;
        //}
        
    }

    
    if(NULL != numberofrops)
    {
        *numberofrops = nrops;
    }

    // if any rop inside ropframereply w/ eo_ropframe_ROP_NumberOf() then sets thereisareply  
    if(NULL != thereisareply)
    {
        *thereisareply = (0 == eo_ropframe_ROP_NumberOf(p->ropframereply)) ? (eobool_false) : (eobool_true);
        // dont use the quickversion because it may be that ropframereply is dummy
        //*thereisareply = (0 == eo_ropframe_ROP_NumberOf_quickversion(p->ropframereply)) ? (eobool_false) : (eobool_true);
    } 
    
    if(NULL != transmittedtime)
    {
        *transmittedtime = eo_ropframe_age_Get(p->ropframeinput);
    }   
    
    return(eores_OK);   
}

#if !defined(OVERRIDE_eo_receiver_callback_incaseoferror_in_sequencenumberReceived)
__weak extern void eo_receiver_callback_incaseoferror_in_sequencenumberReceived(uint64_t rec_seqnum, uint64_t expected_seqnum)
{
;
}
#endif

extern eOresult_t eo_receiver_GetReply(EOreceiver *p, EOropframe **ropframereply)
{
    if((NULL == p) || (NULL == ropframereply)) 
    {
        return(eores_NOK_nullpointer);
    }
    
    if(0 == eo_ropframe_ROP_NumberOf(p->ropframereply))
    // dont use the quickversion because it may be that ropframereply is dummy
//    if(0 == eo_ropframe_ROP_NumberOf_quickversion(p->ropframereply))
    {
        *ropframereply  = p->ropframereply;
        return(eores_NOK_generic);
    }
    
    *ropframereply  = p->ropframereply;
    
    return(eores_OK);
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




