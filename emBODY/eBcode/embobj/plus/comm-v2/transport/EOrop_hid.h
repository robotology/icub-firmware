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
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------


/* @typedef struct eOropstream_t
    @brief      contains representation of the binary rop, which is formed by: 
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
} eOropstream_t;


/* @struct     EOrop_hid
    @brief      Hidden definition. 
 **/
struct EOrop_hid 
{
    eOropstream_t       stream;                     // contains the representation of the binary rop. it is created by parser
    EOnv                netvar;                     // it is used by the agent in reception phase but also in transmission phase. 
    eOropdescriptor_t   ropdes;                     // contains a description of the rop
};    



// - declaration of extern hidden functions ---------------------------------------------------------------------------



extern void	eo_rop_hid_fill_ropdes(eOropdescriptor_t* ropdes, eOropstream_t* stream, uint16_t size, uint8_t* data);



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




