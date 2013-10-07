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
#include "EOtheErrorManager.h"
#include "EOnv.h"
#include "EOrop_hid.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheParser.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheParser_hid.h" 


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



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


//static const char s_eobj_ownname[] = "EOtheParser";

static EOtheParser eo_theparser = 
{
    EO_INIT(.initted)       0
};




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

 
extern EOtheParser * eo_parser_Initialise(void) 
{
    if(1 == eo_theparser.initted)
    {
        return(&eo_theparser);
    }
 
    eo_theparser.initted = 1;

    return(&eo_theparser);        
}    


extern EOtheParser * eo_parser_GetHandle(void) 
{
    return( (1 == eo_theparser.initted) ? (&eo_theparser) : (eo_parser_Initialise()) );
}


extern eOresult_t eo_parser_GetROP(EOtheParser *p, const uint8_t *streamdata, const uint16_t streamsize, EOrop *rop, uint16_t *consumedbytes)
{   // this function requires the access to hidden types of EOrop
    eOrophead_t *rophead            = NULL;
    uint8_t     *ropdata            = NULL;
    uint8_t     *roptail            = NULL;
    uint16_t    dataeffectivesize   = 0; // multiple of four
    uint16_t    signeffectivesize   = 0;
    uint16_t    timeeffectivesize   = 0;

    if((NULL == p) || (NULL == streamdata) || (NULL == rop) || (NULL == consumedbytes))
    {
        return(eores_NOK_nullpointer);
    }

    // reset return data: rop and consumed bytes
    eo_rop_Reset(rop);
    *consumedbytes = 0;

    if(streamsize < eo_rop_minimumsize)
    {
        return(eores_NOK_generic);
    }

    // get the head of the rop with ctrl, ropc, endp, nvid, dsiz. 
    // for now the roptail is just after the four bytes of the head
    rophead = (eOrophead_t*)(&streamdata[0]);
    roptail = (uint8_t*)(&streamdata[4]);
    roptail = roptail;  // there is this instruction to force roptail to have its correct value in debugger

    // check validity of ctrl
    if(1 == rophead->ctrl.userdefn)
    {
       // not managed yet
        return(eores_NOK_generic);
    }

    // check validity of ropc 
    
    if(eobool_false == eo_rop_hid_ropcode_is_valid(rophead->ropc))
    {
        return(eores_NOK_generic);
    }



    // some ropcodes also have a data field
    if(eobool_true == eo_rop_hid_DataField_is_Present(rophead))
    {
        ropdata = (uint8_t*)(&streamdata[sizeof(eOrophead_t)]);
    }

    // in case there is data field, verify it, and sets its effective length 
    // remember that size and info must occupy 4, 8, 12, etc bytes.
    if(NULL != ropdata)
    {
			
        // at first we must be sure that the rop->stream can contain the data
        if(rophead->dsiz > rop->stream.capacity)
        {   // cannot handle the rop
            return(eores_NOK_generic);
        }			

        // then we compute the effective size of data as the next multiple of four of dsiz
        dataeffectivesize = eo_rop_hid_DataField_EffectiveSize(rophead->dsiz);

        // the roptail is now after the bytes of the head and the bytes of the data
        roptail = (uint8_t*)(&streamdata[sizeof(eOrophead_t) + dataeffectivesize]);
   
    }

    // in case there is sign and/or time presence, then sets the correct sizes

    if(1 == rophead->ctrl.plussign)
    {
        signeffectivesize = 4;
    }

    if(1 == rophead->ctrl.plustime)
    {
        timeeffectivesize = 8;
    }

    // if we dont have enough bytes in the packet ... leave with an error
    if(streamsize < (sizeof(eOrophead_t) + dataeffectivesize + signeffectivesize + timeeffectivesize))
    {
        // not enough bytes in the passed packet to keep the data sugegsted by the dsiz
        return(eores_NOK_generic);
    }

    // else ... fill the rop w/ the acquired info

    *consumedbytes = sizeof(eOrophead_t) + dataeffectivesize + signeffectivesize + timeeffectivesize;

    // copy head
    memcpy(&rop->stream.head, rophead, sizeof(eOrophead_t));

    // copy data
    if(NULL != ropdata)
    {
        rop->stream.head.dsiz = rophead->dsiz;
        memcpy(rop->stream.data, ropdata, rophead->dsiz);
    }
		
    
    // copy the signature
    if(0 != signeffectivesize)
    {
        rop->stream.sign = *( (uint32_t*) &roptail[0] );
    }

    // copy the time
    if(0 != timeeffectivesize)
    {
        rop->stream.time = *( (uint64_t*) &roptail[signeffectivesize] );
    }  
    

    // sets the ownership
    // asfidanken
    //rop->tmpdata.nvownership = eo_rop_hid_GetOwnership(rophead->ropc, (eOropconfinfo_t)rophead->ctrl.confinfo, eo_rop_dir_received);

    // prepare the ropdes
    eo_rop_hid_fill_ropdes(&rop->ropdes, &rop->stream, rop->stream.head.dsiz, rop->stream.data);
				
    
    return(eores_OK);
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




