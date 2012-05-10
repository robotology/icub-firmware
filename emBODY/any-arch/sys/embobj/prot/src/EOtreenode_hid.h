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
#ifndef _EOTREENODE_HID_H_
#define _EOTREENODE_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOtreenode_hid.h
    @brief      This header file implements hidden interface to a rop object.
    @author     marco.accame@iit.it
    @date       09/06/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtreenode.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

#define EOTREENODE_NCHILDREN      16


// - definition of the hidden struct implementing the object ----------------------------------------------------------


/** @struct     EOtreenode_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/
struct EOtreenode_hid 
{
    void*                   data;                               // pointer to the data 
    uint16_t                index;                              // index in the array
    uint8_t                 nchildren;                          // number of dependant netvars
    uint16_t                ichildren[EOTREENODE_NCHILDREN];    // using indices in a common array rather than pointers which would require more space  
    EOtreenode*             pchildren[EOTREENODE_NCHILDREN];    // using indices in a common array rather than pointers which would require more space  
};   
 



// - declaration of extern hidden functions ---------------------------------------------------------------------------

///** @fn         extern EOtreenode * eo_treenode_hid_New(void)
//    @brief      Creates a new nv object. 
//    @return     The pointer to the required object.
// **/
//extern EOtreenode * eo_treenode_hid_New(void);

 

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




