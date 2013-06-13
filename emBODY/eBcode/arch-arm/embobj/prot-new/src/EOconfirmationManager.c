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
#include "EOtheMemoryPool.h"
#include "EOtheParser.h"
#include "EOtheFormer.h"
#include "EOropframe_hid.h"
#include "EOnv_hid.h"
#include "EOrop_hid.h"
#include "EOVtheSystem.h"

#include "EOtheAgent_hid.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOconfirmationManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOconfirmationManager_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------



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

static void s_eo_confman_default_rop_conf_requested(EOrop *rop, eOipv4addr_t toipaddr);
static void s_eo_confman_default_rop_conf_received(EOrop *rop, eOipv4addr_t fromipaddr);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOconfirmationManager";

const eOconfman_cfg_t eOconfman_cfg_default = 
{
    EO_INIT(.on_rop_conf_requested)         s_eo_confman_default_rop_conf_requested, 
    EO_INIT(.on_rop_conf_received)          s_eo_confman_default_rop_conf_received
};




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOconfirmationManager* eo_confman_New(const eOconfman_cfg_t *cfg)
{
    EOconfirmationManager *retptr = NULL;   

    if(NULL == cfg)
    {    
        cfg = &eOconfman_cfg_default;
    }
    
    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOconfirmationManager), 1);
    
    memcpy(&retptr->config, cfg, sizeof(eOconfman_cfg_t));

    
    return(retptr);
}


extern eOresult_t eo_confman_Confirmation_Requested(EOconfirmationManager *p, EOrop *rop, eOipv4addr_t toipaddr)
{
    if((NULL == p) || (NULL == rop))
    {
        return(eores_NOK_generic);  
    }

    if(1 == rop->stream.head.ctrl.rqstconf)
    {
        if(NULL !=  p->config.on_rop_conf_requested)
        {
            p->config.on_rop_conf_requested(rop, toipaddr);
        }
        return(eores_OK);
    }

    return(eores_NOK_generic);   

}



extern eOresult_t eo_confman_Confirmation_Received(EOconfirmationManager *p, EOrop *rop, eOipv4addr_t fromipaddr)
{
    if((NULL == p) || (NULL == rop))
    {
        return(eores_NOK_generic);  
    }
    
    eOropconfinfo_t confinfo = (eOropconfinfo_t)rop->stream.head.ctrl.confinfo;

    if(eo_ropconf_none != confinfo)
    {
        // received a confirmation ack/nak: execute the callback
        if(NULL != p->config.on_rop_conf_received)
        {
            p->config.on_rop_conf_received(rop, fromipaddr);
        }

        return(eores_OK); 
    } 

    return(eores_NOK_generic);      

}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_confman_default_rop_conf_requested(EOrop *rop, eOipv4addr_t toipaddr)
{

   // of the rop it is useful to examinate the following which can be used to store info about the rop:
    
    // rop->stream.head.ropc 
    // rop->stream.head.endp 
    // rop->stream.head.nvid 
    // rop->stream.sign             (if any)
    // rop->stream.time             (if any)
    
}

static void s_eo_confman_default_rop_conf_received(EOrop *rop, eOipv4addr_t fromipaddr)
{

    // of the confirmation rop it is useful to examinate the following which can be used to mark the original rop delivered (if confinfo is eo_ropconf_ack)
    
    // rop->stream.head.ropc 
    // rop->stream.head.endp 
    // rop->stream.head.nvid 
    // rop->stream.sign             (if any) 
    // rop->stream.time             (if any)
    
    // and obviuosly ... the confirmation info:
    
    // (eOropconfinfo_t)rop->stream.head.ctrl.confinfo

}







// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




