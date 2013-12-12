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
#include "EOtheErrorManager.h"

#include "EOrop_hid.h"





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOropframe.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOropframe_hid.h" 


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
// empty-section


EO_static_inline EOropframeHeader_t* s_eo_ropframe_header_get(EOropframe *p)
{
    return( (EOropframeHeader_t *)&p->headropsfooter->header );
}

EO_static_inline uint8_t* s_eo_ropframe_rops_get(EOropframe *p)
{
    return( (uint8_t *)(&p->headropsfooter->ropsfooter[0]) );
}

EO_static_inline uint16_t s_eo_ropframe_sizeofrops_get(EOropframe *p)
{
    return( p->headropsfooter->header.ropssizeof );
}

EO_static_inline uint16_t s_eo_ropframe_numberofrops_get(EOropframe *p)
{
    return( p->headropsfooter->header.ropsnumberof );
}

EO_static_inline EOropframeFooter_t* s_eo_ropframe_footer_get(EOropframe *p)
{
    return( (EOropframeFooter_t *)(&p->headropsfooter->ropsfooter[s_eo_ropframe_sizeofrops_get(p)]) );
}


static void s_eo_ropframe_header_addrop(EOropframe *p, uint16_t sizeofrop);
static void s_eo_ropframe_header_remrop(EOropframe *p, uint16_t sizeofrop);
static void s_eo_ropframe_header_addrops(EOropframe *p, uint16_t numofrops, uint16_t sizeofrops);
static void s_eo_ropframe_header_clr(EOropframe *p);
static void s_eo_ropframe_footer_adjust(EOropframe *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOropframe";

//static const uint16_t s_eo_ropframe_minimum_framesize = eo_ropframe_sizeforZEROrops;
//(sizeof(EOropframeHeader_t)+sizeof(EOropframeFooter_t));


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOropframe* eo_ropframe_New(void)
{
    EOropframe *retptr = NULL;    

    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOropframe), 1);
    
    retptr->capacity                = 0;
    retptr->size                    = 0;
    retptr->index2nextrop2beparsed  = 0;
    retptr->headropsfooter          = NULL;

    return(retptr);
}


extern eOresult_t eo_ropframe_Load(EOropframe *p, uint8_t *framedata, uint16_t framesize, uint16_t framecapacity)
{
    if((NULL == p) || (NULL == framedata)) 
    {
        return(eores_NOK_nullpointer);
    }
    
    if((framecapacity < eo_ropframe_sizeforZEROrops) || (framesize > framecapacity) || (framesize < eo_ropframe_sizeforZEROrops))
    {
        return(eores_NOK_generic);
    }

    p->capacity                 = framecapacity;
    p->size                     = framesize;
    p->index2nextrop2beparsed   = 0;
    p->headropsfooter           = (EOropframeHeaderRopsFooter_t*)framedata;
    
    return(eores_OK);
}

extern eOresult_t eo_ropframe_Unload(EOropframe *p)
{
    if(NULL == p) 
    {
        return(eores_NOK_nullpointer);
    }

    p->capacity                 = 0;
    p->size                     = 0;
    p->index2nextrop2beparsed   = 0;
    p->headropsfooter           = NULL;

    return(eores_OK);
}

extern eOresult_t eo_ropframe_Get(EOropframe *p, uint8_t **framedata, uint16_t* framesize, uint16_t* framecapacity)  
{
    if((NULL == p) || (NULL == framedata) || (NULL == framesize) || (NULL == framecapacity)) 
    {
        return(eores_NOK_nullpointer);
    }

    *framedata          = (uint8_t*)p->headropsfooter;
    *framesize          = p->size;
    *framecapacity      = p->capacity;

    return(eores_OK);
}

extern eOresult_t eo_ropframe_Size_Get(EOropframe *p, uint16_t* framesize)
{
    if((NULL == p) || (NULL == framesize)) 
    {
        return(eores_NOK_nullpointer);
    }

    *framesize          = p->size;

    return(eores_OK);
}


extern eOresult_t eo_ropframe_Clear(EOropframe *p)
{
    if(NULL == p) 
    {
        return(eores_NOK_nullpointer);
    }
   
    p->size                     = (0 == p->capacity) ? (0) : (eo_ropframe_sizeforZEROrops); // if capacity is zero then we dont have buffer ... else we have and size must be eo_ropframe_sizeforZEROrops
    p->index2nextrop2beparsed   = 0;
    
    if(NULL != p->headropsfooter)
    {
        s_eo_ropframe_header_clr(p);    
        s_eo_ropframe_footer_adjust(p);
    }

    return(eores_OK);
}


extern eOresult_t eo_ropframe_Append(EOropframe *p, EOropframe *rfr, uint16_t *remainingbytes)  
{
    uint16_t rfr_sizeofrops;
    uint16_t p_sizeofrops;

// removed because the control vs NULL is done inside _Isvalid() method
//    if((NULL == p) || (NULL == rfr)) 
//    {
//        return(eores_NOK_nullpointer);
//    }

    // both must be valid
    if((eobool_false == eo_ropframe_IsValid(p)) || (eobool_false == eo_ropframe_IsValid(rfr)))
    {
        return(eores_NOK_generic);
    }

    // get the ropstream starting from the end of rops. call the parser

    rfr_sizeofrops = s_eo_ropframe_sizeofrops_get(rfr);
    
    if(0 == rfr_sizeofrops)
    {   // the second ropframe is empty
        return(eores_OK);
    }

    p_sizeofrops = s_eo_ropframe_sizeofrops_get(p);

    // first must be able to accept the rops contained in teh second
    if(p->capacity < (eo_ropframe_sizeforZEROrops+p_sizeofrops+rfr_sizeofrops))
    {
        return(eores_NOK_generic);
    }

    // ok, now i can concatenate ...

    // copy
    memcpy(s_eo_ropframe_rops_get(p)+p_sizeofrops, s_eo_ropframe_rops_get(rfr), rfr_sizeofrops);

    // advance the internal index2nextrop2beparsed ... are w sure we need it in here? i dont think so
    //#warning -> remove ??
    //p->index2nextrop2beparsed += rfr_sizeofrops;

    // advance the size
    p->size  += rfr_sizeofrops;     // if we append a ropframe the size of the resulting frame will be the sum of the old size (head+rops+foot) plus the size of the appended rops.
    
    // adjust the header
    s_eo_ropframe_header_addrops(p, s_eo_ropframe_numberofrops_get(rfr), rfr_sizeofrops);

    // adjust the footer
    s_eo_ropframe_footer_adjust(p);
    
    // fill the retrun value
    if(NULL != remainingbytes)
    {
        *remainingbytes = p->capacity - eo_ropframe_sizeforZEROrops - s_eo_ropframe_sizeofrops_get(p);
    }


    return(eores_OK);
}

extern eObool_t eo_ropframe_IsValid(EOropframe *p)
{
    EOropframeHeader_t *header;
    EOropframeFooter_t *footer;
    
    if((NULL == p) || (NULL == p->headropsfooter)) 
    {
        return(eobool_false);
    }

    header = s_eo_ropframe_header_get(p);
    footer = s_eo_ropframe_footer_get(p);
    
    if(EOFRAME_START != header->startofframe)
    {
        return(eobool_false);
    }
       
    if(EOFRAME_END != footer->endoframe)
    {
        return(eobool_false);
    }
    
    return(eobool_true);
}

extern uint16_t eo_ropframe_ROP_NumberOf(EOropframe *p)
{
    if(eobool_false == eo_ropframe_IsValid(p))
    {
        return(0);
    }
    else
    {
        return(s_eo_ropframe_numberofrops_get(p));
    }
}

extern uint16_t eo_ropframe_ROP_NumberOf_quickversion(EOropframe *p)
{
    return( p->headropsfooter->header.ropsnumberof );
}


extern eOresult_t eo_ropframe_ROP_Parse(EOropframe *p, EOrop *rop, uint16_t *unparsedbytes)
{
    uint16_t consumedbytes = 0;
    eOresult_t res = eores_NOK_generic;
    uint16_t unparsed = 0;
    uint8_t * ropstream = NULL;
    eOparserResult_t parsres = eo_parser_res_ok;
    
    if((NULL == p) || (NULL == rop)) 
    {
        return(eores_NOK_nullpointer);
    }
        
    unparsed = s_eo_ropframe_sizeofrops_get(p) - p->index2nextrop2beparsed;
    
    if(0 == unparsed)
    {
        // cannot parse anymore ...
        eo_rop_Reset(rop);

        if(NULL != unparsedbytes)
        {
            *unparsedbytes = 0;
        }        
        return(eores_NOK_generic);
    }
    
    // get the ropstream starting from p->index2nextrop2beparsed. call the parser
    
    ropstream = s_eo_ropframe_rops_get(p);
    ropstream += p->index2nextrop2beparsed;
   
    // this function fills the rop only if everything is ok. it returns error if it cannot prepare a valid rop
    // in consumedbytes it tells how many bytes it has used. in some case if the ropstream is strongly illegal
    // an it cannot go to next rop, consumedbytes is equal to unparsed, so that we have to quit.
    res = eo_parser_GetROP(eo_parser_GetHandle(), ropstream, unparsed, rop, &consumedbytes, &parsres);
    
    if(eores_OK != res)
    {
        if(eores_NOK_nullpointer == res)
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "eo_ropframe_ROP_Parse() called w/ NULL params");          
        }
       
        eo_rop_Reset(rop);
        
        // for all errors the parser tells us how many bytes consume. 
        // they may be those of the single rop or also those of the entire stream.  

        // put in here diagnostics of parsing error ?
    }
    
    // advance the internal index2nextrop2beparsed
    p->index2nextrop2beparsed += consumedbytes;
  
    // returns the number of un-parsed bytes
    if(NULL != unparsedbytes)
    {
        *unparsedbytes = s_eo_ropframe_sizeofrops_get(p) - p->index2nextrop2beparsed;
    }

    // then ... returns the result: OK or NOK.

    return(res);
}

extern eOresult_t eo_ropframe_ROP_Add(EOropframe *p, const EOrop *rop, uint16_t* addedinpos, uint16_t* consumedbytes, uint16_t *remainingbytes)
{    
    uint8_t* ropstream = NULL;
    int32_t remaining = 0;
    uint16_t streamsize = 0;
    uint16_t streamindex = 0;
    eOresult_t res = eores_NOK_generic;
    
    if((NULL == p) || (NULL == p->headropsfooter) || (NULL == rop)) 
    {
        return(eores_NOK_nullpointer);
    }
     
    // verify that the rop is valid
    if(eobool_false == eo_rop_IsValid((EOrop*)rop))
    {
        return(eores_NOK_generic);
    }
    
    // verify that we have bytes enough to convert the rop to stream 
    
    streamsize = eo_rop_GetSize((EOrop*)rop);
    // remaining can be also negative. for example when capacity is eo_ropframe_sizeforZEROrops+1 (thus only one byte for rops) and the target rop requires 8 bytes.
    // we have eo_ropframe_sizeforZEROrops+1 - eo_ropframe_sizeforZEROrops - 8 = -7 ...
    remaining = p->capacity - eo_ropframe_sizeforZEROrops - s_eo_ropframe_sizeofrops_get(p);
    if(remaining < ((int32_t)streamsize))
    {   // not enough space in ...
        return(eores_NOK_generic);
    }
  
    // get the ropstream starting from the end of rops. call the parser
    ropstream = s_eo_ropframe_rops_get(p);
    streamindex = s_eo_ropframe_sizeofrops_get(p);
    ropstream += streamindex;
    
    // convert the rop and put it inside the stream;    
    res = eo_former_GetStream(eo_former_GetHandle(), rop, remaining, ropstream, &streamsize); 
 
    if(eores_OK != res)
    {
        // the rop is incorrect or it simply contains too much data to be fit inside the stream ...
        return(eores_NOK_generic);
    }
    
//    // advance the internal index2nextrop2beparsed: are we sure that we must advance it?
//#warning --> remove it
//    p->index2nextrop2beparsed += streamsize;

    // advance the size with what is sued by the added stream
    p->size  += streamsize;
    
    // adjust the header
    s_eo_ropframe_header_addrop(p, streamsize);

    // adjust the footer
    s_eo_ropframe_footer_adjust(p);


    if(NULL != addedinpos)
    {
        *addedinpos = streamindex;
    }

    if(NULL != consumedbytes)
    {
        *consumedbytes = streamsize;
    }
        
    if(NULL != remainingbytes)
    {
        *remainingbytes = p->capacity - eo_ropframe_sizeforZEROrops - s_eo_ropframe_sizeofrops_get(p);
    }
    
    // ... returns ok

    return(eores_OK);
}


extern eOresult_t eo_ropframe_ROP_Rem(EOropframe *p, uint16_t wasaddedinpos, uint16_t itsizewas)
{
    int16_t tmp = 0;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }


    // move memory
    tmp = s_eo_ropframe_sizeofrops_get(p)-wasaddedinpos-itsizewas; // howmanymove
    if(tmp > 0)
    {
        memmove(s_eo_ropframe_rops_get(p)+wasaddedinpos, s_eo_ropframe_rops_get(p)+wasaddedinpos+itsizewas, tmp);
    }

//    // decrement the internal index2nextrop2beparsed: are we sure we must update it?
//#warning -> remove it
//    p->index2nextrop2beparsed -= itsizewas;

    // decrement the size by the byte used by the removed stream
    p->size  -= itsizewas;
    
    // adjust the header
    s_eo_ropframe_header_remrop(p, itsizewas);

    // adjust the footer
    s_eo_ropframe_footer_adjust(p);

    // clear what stays beyond footer: instead or remaining i clear only what was non-zero (itsizewas)
    //tmp = p->capacity - eo_ropframe_sizeforZEROrops - s_eo_ropframe_sizeofrops_get(p);  // remaining
    memset(((uint8_t*)s_eo_ropframe_footer_get(p))+sizeof(EOropframeFooter_t), 0, itsizewas);

    return(eores_OK);
}


extern eOresult_t eo_ropframe_age_Set(EOropframe *p, eOabstime_t age)
{
    EOropframeHeader_t* header = NULL;
    
    if(NULL == p) 
    {
        return(eores_NOK_nullpointer);
    }

    header = s_eo_ropframe_header_get(p);

    header->ageofframe = age;
 
    return(eores_OK);
}

extern eOabstime_t eo_ropframe_age_Get(EOropframe *p)
{
    EOropframeHeader_t* header = NULL;
    
    if(NULL == p) 
    {
        return(eok_abstimeNOW);
    }

    header = s_eo_ropframe_header_get(p);

    return(header->ageofframe);
}

extern eOresult_t eo_ropframe_seqnum_Set(EOropframe *p, uint64_t seqnum)
{
    EOropframeHeader_t* header = NULL;
    
    if(NULL == p) 
    {
        return(eores_NOK_nullpointer);
    }

    header = s_eo_ropframe_header_get(p);

    header->sequencenumber = seqnum;
 
    return(eores_OK);
}

extern uint64_t eo_ropframe_seqnum_Get(EOropframe *p)
{
    EOropframeHeader_t* header = NULL;
    
    if(NULL == p) 
    {
        return(eok_uint64dummy);
    }

    header = s_eo_ropframe_header_get(p);

    return(header->sequencenumber);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

uint8_t* eo_ropframe_hid_get_pointer_offset(EOropframe *p, uint16_t offset)
{
    if(NULL == p)
    {
        return(NULL);
    }

    return(s_eo_ropframe_rops_get(p) + offset);
}





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_ropframe_header_addrop(EOropframe *p, uint16_t sizeofrop)
{
    EOropframeHeader_t* header = s_eo_ropframe_header_get(p);
    
    header->ropssizeof              += sizeofrop;
    header->ropsnumberof            += 1;
}


static void s_eo_ropframe_header_remrop(EOropframe *p, uint16_t sizeofrop)
{
    EOropframeHeader_t* header = s_eo_ropframe_header_get(p);
    
    header->ropssizeof              -= sizeofrop;
    header->ropsnumberof            -= 1;
}

static void s_eo_ropframe_header_addrops(EOropframe *p, uint16_t numofrops, uint16_t sizeofrops)
{
    EOropframeHeader_t* header = s_eo_ropframe_header_get(p);
    
    header->ropssizeof              += sizeofrops;
    header->ropsnumberof            += numofrops;
}

static void s_eo_ropframe_header_clr(EOropframe *p)
{
    EOropframeHeader_t* header = s_eo_ropframe_header_get(p);
    
    header->startofframe            = EOFRAME_START;
    header->ropssizeof              = 0;
    header->ropsnumberof            = 0;
    header->ageofframe              = 0;
}
    
static void s_eo_ropframe_footer_adjust(EOropframe *p)
{
    EOropframeFooter_t* footer = s_eo_ropframe_footer_get(p);
    
    footer->endoframe               = EOFRAME_END;
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




