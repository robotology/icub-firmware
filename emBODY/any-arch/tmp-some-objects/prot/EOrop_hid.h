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
#include "EOtreenode.h"
#include "EOnvsCfg.h"

//typedef EOtreenode EOnvnodeTMP;

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOrop.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------





typedef struct
{
    uint8_t         ffu         : 1;
    uint8_t         confinfo    : 2;       
    uint8_t         plustime    : 1;
    uint8_t         plussign    : 1;
    uint8_t         rqsttime    : 1;
    uint8_t         rqstconf    : 1;
    uint8_t         userdefn    : 1; 
} eOropctrl_t;      EO_VERIFYsizeof(eOropctrl_t, 1);


typedef struct
{
    eOropctrl_t     ctrl;
    eOropcode_t     ropc;
    eOnvEP_t        endp;
    eOnvID_t        nvid;
    uint16_t        dsiz;
} eOrophead_t;      EO_VERIFYsizeof(eOrophead_t, 8);

typedef enum
{
    eo_rop_dir_received     = 0,
    eo_rop_dir_outgoing     = 1
} eOropDirection;


// - definition of the hidden struct implementing the object ----------------------------------------------------------




typedef struct
{   // temporary data used for various purposes, mainly for computing the netvar
    EOnvsCfg*           nvscfg;                 // the used nvscfg
//    eOipv4addr_t        ipaddress;              // the ip address of the ... is not necessary to be stored
    eOnvOwnership_t     nvownership;            // the ownership of the netvar with respect to the user of the rop
    uint16_t            ondevindex;             // the index of the device owning the netvar inside nvscfg 
    uint16_t            onendpointindex;        // the index of the endpoint owning the netvar on the specified device
    uint16_t            onidindex;              // the index of the treenode inside the specified endpoint
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
} EOrop_stream_t;


/* @struct     EOrop_hid
    @brief      Hidden definition. 
 **/
struct EOrop_hid 
{
    EOrop_stream_t      stream;                     // contains the representation of the binary rop. it is created by parser or use by rop
    EOnv                netvar;                     // it is used by the agent in reception phase but also in transmission phase. it
    uint16_t            capacityofstreamdatafield;  // it is the capacity of the stream.data field 
    uint16_t            curindexofstreamdatafield;  // contains an index used to navigate the stream.data field in case the set of values are to be done on each leaf of the netvar
    EOrop_tmpdata_t     tmpdata;                    // temporary data used for various purposes, mainly for computing the netvar    
};    



// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern EOnv* eo_rop_hid_NV_Get(EOrop *p);

extern eOnvOwnership_t eo_rop_hid_GetOwnership(eOropcode_t ropc, eOropconfinfo_t confinfo, eOropDirection direction);

extern eObool_t eo_rop_hid_DataField_is_Present(const eOrophead_t *head);

extern eObool_t eo_rop_hid_DataField_is_Required(const eOrophead_t *head);

EO_extern_inline uint16_t eo_rop_hid_DataField_EffectiveSize(uint16_t ropdatasize)
{
    return(((ropdatasize + 3) >> 2) << 2);
}

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




