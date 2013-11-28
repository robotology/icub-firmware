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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOROP_HID_H_
#define _EOROP_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOrop_hid.h
    @brief      This header file implements hidden interface to a rop object.
    @author     marco.accame@iit.it
    @date       09/06/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv_hid.h"
#include "EOnvSet.h"

//typedef EOtreenode EOnvnodeTMP;

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOrop.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------






typedef struct
{
    eOropctrl_t     ctrl;
    eOropcode_t     ropc;
    uint16_t        dsiz;
    eOnvID32_t      id32;
} eOrophead_t;      EO_VERIFYsizeof(eOrophead_t, 8);


EO_VERIFYproposition(eo_rop_minimumsize_verify, (eo_rop_minimumsize == sizeof(eOrophead_t)));


typedef enum
{
    eo_rop_dir_received     = 0,
    eo_rop_dir_outgoing     = 1
} eOropDirection;


// - definition of the hidden struct implementing the object ----------------------------------------------------------




typedef struct
{   // temporary data used for various purposes, mainly for computing the netvar
    EOnvSet*            nvset;                  // the used nvset
    eOnvOwnership_t     nvownership;            // the ownership of the netvar with respect to the user of the rop
} EOrop_tmpdata_t;


/* @struct     EOrop_stream_t
    @brief      contains representation of the binary rop, whichi is formed by: 
                head (8B) + data (4nB, optional) + sign (4B, optional) + time (8B, optional)
 **/
typedef struct
{
    eOrophead_t         head; 
    uint8_t*            data;       
    uint32_t            sign;
    uint64_t            time;    
    uint16_t		    capacity;
    uint16_t	        dummy;			
} EOrop_stream_t;


/* @struct     EOrop_hid
    @brief      Hidden definition. 
 **/
struct EOrop_hid 
{
    EOrop_stream_t      stream;                     // contains the representation of the binary rop. it is created by parser or use by rop
    EOnv                netvar;                     // it is used by the agent in reception phase but also in transmission phase. it
    eOropdescriptor_t   ropdes;                     // contains a description of the rop used by the callback funtions of the netvar
};    



// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern EOnv* eo_rop_hid_NV_Get(EOrop *p);

extern eOnvOwnership_t eo_rop_hid_GetOwnership(eOropcode_t ropc, eOropconfinfo_t confinfo, eOropDirection direction);

extern eObool_t eo_rop_hid_DataField_is_Present(const eOrophead_t *head);

extern eObool_t eo_rop_hid_DataField_is_Required(const eOrophead_t *head);

extern eObool_t eo_rop_hid_OPChasData(eOropcode_t ropc);

extern void	eo_rop_hid_fill_ropdes(eOropdescriptor_t* ropdes, EOrop_stream_t* stream, uint16_t size, uint8_t* data);

// - declaration of extern inline hidden functions --------------------------------------------------------------------

EO_extern_inline uint16_t eo_rop_hid_DataField_EffectiveSize(uint16_t ropdatasize)
{
    return(((ropdatasize + 3) >> 2) << 2);
}

EO_extern_inline eObool_t eo_rop_hid_ropcode_is_valid(uint8_t bytewithropcode)
{
    if((bytewithropcode >= (uint8_t)eo_ropcodevalues_numberof) || (bytewithropcode == (uint8_t)eo_ropcode_none))
    {
        return(eobool_false);
    }
    
    return(eobool_true);
}


EO_extern_inline eObool_t eo_rop_hid_is_valid(EOrop *p)
{
    // verify ropcode
    if(eobool_false == eo_rop_hid_ropcode_is_valid(p->stream.head.ropc))
    {
        return(eobool_false);
    }
    
    eObool_t datasizeispresent = eo_rop_hid_DataField_is_Present(&p->stream.head);
    eObool_t datafieldisrequired = eo_rop_hid_DataField_is_Required(&p->stream.head);
    
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




#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




