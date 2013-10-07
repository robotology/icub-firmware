/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _EOMDATACONTAINER_HID_H_
#define _EOMDATACONTAINER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif
// - doxy -------------------------------------------------------------------------------------------------------------

/* @file       EOMdataContainer_hid.h
    @brief      This header file implements hidden interface to an entity.
    @author     valentina.gaggero@iit.it
    @date       16/12/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EOvector.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOMdataContainer.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct 
{
    eOsizeitem_t  datasize;
    void          *dataptr;
}EOMdataContainer_itemStruct_hid_t;


/** @struct     Object_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/   
struct EOMdataContainer_hid 
{
    EOMmutex                *readers_mutex;     /**< mutex used by readers */
    EOMmutex                *obj_mutex;         /**< mutex for the obj */
    uint16_t                readers_num;        /**< num of readers that are reading data */
    EOvector                *vector;
//    eOsizeitem_t            datasize;           /**< size of data */
//    uint8_t                 *data;              /**< pointer to data */
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



