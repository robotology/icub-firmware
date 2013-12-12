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
#include "EOnv_hid.h"
#include "EOtheErrorManager.h"
#include "EOnv_hid.h" 



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOrop.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOrop_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



const eOropctrl_t eok_ropctrl_basic = 
{
    EO_INIT(.confinfo)      eo_ropconf_none,
    EO_INIT(.plustime)      0,
    EO_INIT(.plussign)      0,
    EO_INIT(.rqsttime)      0,
    EO_INIT(.rqstconf)      0,
    EO_INIT(.version)       EOK_ROP_VERSION_0
};


const eOropdescriptor_t eok_ropdesc_basic = 
{
    EO_INIT(.control)
    {
        EO_INIT(.confinfo)      eo_ropconf_none,
        EO_INIT(.plustime)      0,
        EO_INIT(.plussign)      0,
        EO_INIT(.rqsttime)      0,
        EO_INIT(.rqstconf)      0,
        EO_INIT(.version)       EOK_ROP_VERSION_0        
    },
    EO_INIT(.ropcode)           eo_ropcode_none,
    EO_INIT(.size)              0,
    EO_INIT(.id32)              eo_nv_ID32dummy,
    EO_INIT(.data)              NULL,
    EO_INIT(.signature)         0,
    EO_INIT(.time)              0   
};



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

//static const char s_eobj_ownname[] = "EOrop";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOrop* eo_rop_New(uint16_t capacity)
{
    EOrop *retptr = NULL;    

    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOrop), 1);

    retptr->stream.capacity = capacity;

    if(0 != capacity)
    {
        retptr->stream.data = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_08bit, sizeof(uint8_t), capacity);
    }

    eo_rop_Reset(retptr);
    
    return(retptr);
}


extern eOresult_t eo_rop_Reset(EOrop *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
		
    // - stream ---------------------------------------------------------------------
		
    *((uint64_t*)(&(p->stream.head))) = 0;
    memset(p->stream.data, 0, p->stream.capacity);
    p->stream.sign          = EOK_uint32dummy;
    p->stream.time          = EOK_uint64dummy;		
		
    // - netvar ---------------------------------------------------------------------
		
    eo_nv_Clear(&p->netvar);
    
    // - ropdes ---------------------------------------------------------------------
    
    memset(&p->ropdes, 0, sizeof(eOropdescriptor_t));
    
			

    return(eores_OK);
}


extern uint16_t eo_rop_GetSize(EOrop *p)
{   
    uint16_t size = sizeof(eOrophead_t);

    if(NULL == p)
    {    
        return(0);
    }
		
    if(eobool_false == eo_rop_IsValid((EOrop *)p))
    {
        return(0);      
    }

    if(eobool_true == eo_rop_datafield_is_present(&p->stream.head))
    {
         size += eo_rop_datafield_effective_size(p->stream.head.dsiz);
    }

    if(1 == p->stream.head.ctrl.plussign)
    {
        size += 4;
    }

    if(1 == p->stream.head.ctrl.plustime)
    {
        size+= 8;
    }

    return(size);
}


extern eOropcode_t eo_rop_GetROPcode(EOrop *p)
{
    if(NULL == p)
    {
        return(eo_ropcode_none);
    }

    return((eOropcode_t)p->stream.head.ropc);
}


extern uint8_t* eo_rop_GetROPdata(EOrop *p)
{
    if(NULL == p)
    {
        return(NULL);
    }

    return(p->stream.data);
}



extern eOropconfinfo_t eo_rop_GetROPconfinfo(EOrop *p)
{
    if(NULL == p)
    {
        return(eo_ropconf_none);
    }
    return((eOropconfinfo_t)p->stream.head.ctrl.confinfo);	
}


extern EOnv* eo_rop_GetNV(EOrop *p)
{
    if(NULL == p)
    {
        return(NULL);
    }

    return(&p->netvar);
}


extern eObool_t eo_rop_IsValid(EOrop *p)
{
    // verify ropcode
    if(eobool_false == eo_rop_ropcode_is_valid(p->stream.head.ropc))
    {
        return(eobool_false);
    }
    
    eObool_t datasizeispresent = eo_rop_datafield_is_present(&p->stream.head);
    eObool_t datafieldisrequired = eo_rop_datafield_is_required(&p->stream.head);
    
    // verify datafield
    if(eobool_true == datafieldisrequired)
    {
        if(eobool_false == datasizeispresent)
        {
            return(eobool_false);
        }
    }
    else
    {
        if(eobool_true == datasizeispresent)
        {
            return(eobool_false);
        }        
    }
    
    return(eobool_true);
}


extern uint16_t eo_rop_compute_size(eOropctrl_t ropctrl, eOropcode_t ropc, uint16_t sizeofdata)
{
    uint16_t size = sizeof(eOrophead_t);
    eOrophead_t rophead = 
    {
        EO_INIT(.ctrl)  0,
        EO_INIT(.ropc)  ropc,
        EO_INIT(.dsiz)  sizeofdata,           
        EO_INIT(.id32)  0     
    };

    if( (eo_ropcode_none == ropc) || (0 != ropctrl.version) )
    {
        return(0);      
    }
    

    memcpy(&rophead.ctrl, &ropctrl, sizeof(eOropctrl_t));
    
    
    if(eobool_true == eo_rop_datafield_is_required(&rophead))
    {
        size += eo_rop_datafield_effective_size(rophead.dsiz);
    }

    if(1 == rophead.ctrl.plussign)
    {
        size += 4;
    }

    if(1 == rophead.ctrl.plustime)
    {
        size+= 8;
    }

    return(size);    
    
}


// used when the rop is already built or when we already have a valid head from a stream
extern eObool_t eo_rop_datafield_is_present(const eOrophead_t *head)
{
    return( (0 != head->dsiz) ? (eobool_true) : (eobool_false));
}



// used when it is necessary to build a rop to transmit and the dsiz field is not yet assigned
extern eObool_t eo_rop_datafield_is_required(const eOrophead_t *head)
{
    eObool_t ret = eobool_false;

    if((eo_ropconf_none == head->ctrl.confinfo) && 
       ((eo_ropcode_set == head->ropc) || (eo_ropcode_say == head->ropc) || (eo_ropcode_sig == head->ropc))
      )
    {
        ret = eobool_true;
    }

    return(ret);
}




extern eObool_t eo_rop_ropcode_has_data(eOropcode_t ropc)
{
    eObool_t ret = eobool_false;

    if((eo_ropcode_set == ropc) || (eo_ropcode_say == ropc) || (eo_ropcode_sig == ropc))
    {
        ret = eobool_true;
    }

    return(ret);
}


extern uint16_t eo_rop_datafield_effective_size(uint16_t ropdatasize)
{
    return(((ropdatasize + 3) >> 2) << 2);
}

extern eObool_t eo_rop_ropcode_is_valid(uint8_t bytewithropcode)
{
    if((bytewithropcode >= (uint8_t)eo_ropcodevalues_numberof) || (bytewithropcode == (uint8_t)eo_ropcode_none))
    {
        return(eobool_false);
    }
    
    return(eobool_true);
}


// normal commands
// a simple node who only knows about its own netvars must use eo_nv_ownership_local
// when receives ask<>, set<>, rst<>.
// a smart node who receives say<> and sig<> must search into eo_nv_ownership_remote.

// normal commands
// a simple node who only knows about its own netvars must use eo_nv_ownership_local
// when receives ask<>, set<>, rst<>, upd<>.
// a smart node who receives say<> and sig<> must search into eo_nv_ownership_remote.

// ack/nak commands
// the simple node just process: ack-nak-sig<>. 
// the smart node can also process: nak-ask<>, ack-nak-set<>, ack-nak-rst<>, ack-nak-upd<>

extern eOnvOwnership_t eo_rop_get_ownership(eOropcode_t ropc, eOropconfinfo_t confinfo, eOropDirection direction)
{
    eOnvOwnership_t ownership = eo_nv_ownership_local;

    // set direction for received direction
    if((eo_ropcode_ask == ropc) || (eo_ropcode_set == ropc) || (eo_ropcode_rst == ropc))
    {
        ownership = eo_nv_ownership_local;
    }
    else // say, sig, 
    {
        ownership = eo_nv_ownership_remote;
    }

    // if direction is outgoing we exchange
    if(eo_rop_dir_outgoing == direction)
    {
        ownership = (eo_nv_ownership_local == ownership) ? (eo_nv_ownership_remote) : (eo_nv_ownership_local);
    }

    // if the ropc is a confirmation (ack/nak) we exchange again
    if(eo_ropconf_none != confinfo)
    {
        ownership = (eo_nv_ownership_local == ownership) ? (eo_nv_ownership_remote) : (eo_nv_ownership_local);
    }

    return(ownership);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



extern void	eo_rop_hid_fill_ropdes(eOropdescriptor_t* ropdes, eOropstream_t* stream, uint16_t size, uint8_t* data)
{
    memcpy(&ropdes->control, &stream->head.ctrl, sizeof(eOropctrl_t)); 
                            
    ropdes->ropcode			= stream->head.ropc; 
    ropdes->id32		    = stream->head.id32;
    ropdes->size		    = size;
    ropdes->data		    = data;
    ropdes->signature		= stream->sign;
    ropdes->time		    = stream->time;	
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





