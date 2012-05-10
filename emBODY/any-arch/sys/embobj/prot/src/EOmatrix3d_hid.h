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
#ifndef _EOMATRIX3D_HID_H_
#define _EOMATRIX3D_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file        EOmatrix3d_hid.h
    @brief      This header file implements hidden interface to a matrix3d object.
    @author     marco.accame@iit.it
    @date       08/03/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOmatrix3d.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef struct
{
    uint16_t        capacity;
    uint16_t        itemsize;
    uint16_t        size;
    uint16_t        dummy;
} eOmatrix3d_head_t;   EO_VERIFYsizeof(eOmatrix3d_head_t, 8);

typedef struct eOmatrix3d_node_T
{
    eOmatrix3d_head_t           head;
    struct eOmatrix3d_node_T*   node[1];
} eOmatrix3d_node_t;

typedef struct
{
    eOmatrix3d_head_t           head;
    uint8_t                     data[4];
} eOmatrix3d_end_t;


/** @struct     EOmatrix3d_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
struct EOmatrix3d_hid 
{
    eOmatrix3d_head_t           head;
    eOmatrix3d_node_t*          node[1];
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------

//extern eOresult_t eo_matrix3d_hid_PushBackData(EOmatrix3d *p, const void *data, uint8_t sizedata);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




