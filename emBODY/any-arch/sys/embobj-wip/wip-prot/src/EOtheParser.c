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


extern eOresult_t eo_parser_GetROP(EOtheParser *p, const uint8_t *streamdata, const uint16_t streamsize, EOrop *rop, uint16_t *consumedbytes, eOparserResult_t *result)
{   // this function requires the access to hidden types of EOrop
    eOrophead_t *rophead            = NULL;
    uint8_t     *ropdata            = NULL;
    uint8_t     *roptail            = NULL;
    uint16_t    dataeffectivesize   = 0; // multiple of four
    uint16_t    signeffectivesize   = 0;
    uint16_t    timeeffectivesize   = 0;
    uint16_t    parsedropsize       = 0;

    if((NULL == p) || (NULL == streamdata) || (NULL == rop) || (NULL == consumedbytes) || (NULL == result))
    {
        if(NULL != result)
        {
            *result = eo_parser_res_nok_fatal;
        }
        return(eores_NOK_nullpointer);
    }

    // reset return data: rop and consumed bytes
    eo_rop_Reset(rop);
    *consumedbytes = 0;
    *result = eo_parser_res_ok;

    if(streamsize < eo_rop_minimumsize)
    {
        *result = eo_parser_res_nok_nostreamdata;
        *consumedbytes = streamsize;
        return(eores_NOK_generic);
    }

    // get the head of the rop with ctrl, ropc, endp, nvid, dsiz. 
    // for now the roptail is just after the four bytes of the head
    rophead = (eOrophead_t*)(&streamdata[0]);
    roptail = (uint8_t*)(&streamdata[sizeof(eOrophead_t)]);
    roptail = roptail;  // there is this instruction to force roptail to have its correct value in debugger

    // check validity of ctrl
    if(0 != rophead->ctrl.version)
    {
        // not managed yet
        *result = eo_parser_res_nok_ropisillegal;    
        *consumedbytes = streamsize;        
        return(eores_NOK_generic);
    }

    // check validity of ropc 
    
    if(eobool_false == eo_rop_hid_ropcode_is_valid(rophead->ropc))
    {
        *result = eo_parser_res_nok_ropisillegal;
        *consumedbytes = streamsize;
        return(eores_NOK_generic);
    }
    
    // if the ropc requires data, the field datsize must be present
    
    // some ropcodes also have a data field
    if(eobool_true == eo_rop_hid_DataField_is_Required(rophead))
    {
        if(eobool_true == eo_rop_hid_DataField_is_Present(rophead))
        {
            ropdata = (uint8_t*)(&streamdata[sizeof(eOrophead_t)]);            
        }
        else
        {   // the rop is not well formed
            *result = eo_parser_res_nok_ropisillegal;
            *consumedbytes = streamsize;
            return(eores_NOK_generic);
        }
    }
    else
    {
        ropdata = NULL;
    }


    // in case there is data field, verify it, and sets its effective length 
    // remember that size and info must occupy 4, 8, 12, etc bytes.
    if(NULL != ropdata)
    {        
        // we compute the effective size of data as the next multiple of four of dsiz
        dataeffectivesize = eo_rop_hid_DataField_EffectiveSize(rophead->dsiz);
        
        // we make a first verification of size whcih at least tell us if the roptail is inside the stream
        if(streamsize < (sizeof(eOrophead_t) + dataeffectivesize))
        {   // the rop is too big to be contained inside the stream
            *result = eo_parser_res_nok_ropisillegal;
            *consumedbytes = streamsize;
            return(eores_NOK_generic);
        }
			
        // the roptail is after the bytes of the head and the bytes of the data
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
    
    // the total size of the rop acording to info contained in the header is ...
    parsedropsize = sizeof(eOrophead_t) + dataeffectivesize + signeffectivesize + timeeffectivesize;

    // if we dont have enough bytes in the stream to accomodate  leave with an error
    if(streamsize < parsedropsize)
    {   // not enough bytes in the passed packet to keep the data suggested by the header
        *result = eo_parser_res_nok_ropisillegal;
        *consumedbytes = streamsize;
        return(eores_NOK_generic);
    }
    
    // verify if we can accomodate the parsed rop in our buffer
    if(rop->stream.capacity < parsedropsize)
    {   // cannot handle the parsed rop in the EOrop object
        *result = eo_parser_res_nok_ropistoobig;
        *consumedbytes = parsedropsize;       
        return(eores_NOK_generic);
    }			          
    
    // ok ... fill all info
    *result = eo_parser_res_ok;
    *consumedbytes = parsedropsize;

    // copy head
    memcpy(&rop->stream.head, rophead, sizeof(eOrophead_t));

    // copy data
    if(NULL != ropdata)
    {
        rop->stream.head.dsiz = rophead->dsiz;
        memcpy(rop->stream.data, ropdata, dataeffectivesize);
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




