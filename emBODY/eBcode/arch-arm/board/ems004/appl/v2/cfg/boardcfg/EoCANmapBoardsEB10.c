/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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

#if 0

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "EoCommon.h"
#include "EOtheCANmapping.h"

#include "EOconstvector_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const eOcanmap_board_properties_t s_boardprops[] = 
{
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 1         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    },  
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 2         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    }, 
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 3         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    }, 
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 4         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    },  
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 5         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    }, 
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 6         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    }, 
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 7         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    }, 


  
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 8         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    },  
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 9         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    }, 
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 10         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    }, 
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 11         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    },  
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 12         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    }, 
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 13         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    } 
};



EOconstvector s_eo_vectorof_boardprops_eb10 = 
{
    .capacity       = sizeof(s_boardprops)/sizeof(eOcanmap_board_properties_t),
    .size           = sizeof(s_boardprops)/sizeof(eOcanmap_board_properties_t),
    .item_size      = sizeof(eOcanmap_board_properties_t),
    .dummy          = 0,
    .stored_items   = (void*)s_boardprops,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_boardprops_eb10 = &s_eo_vectorof_boardprops_eb10;





// in case, as in eb10 / eb11, there are two skins, in here we put in .index: entindex00 or entindex00 depending on the cases. 
// as i s_des_jomo ...
static const eOcanmap_entitydescriptor_t s_des_skin[] = 
{
    {   // skin 0
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 1,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    },
    {   // skin 0
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 2,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    }, 
    {   // skin 0
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 3,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    }, 
    {   // skin 0
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 4,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    },    
    {   // skin 0
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 5,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    }, 
    {   // skin 0
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 6,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    },
    {   // skin 0
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 7,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    },


    {   // skin 1
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 8,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex01
    },
    {   // skin 1
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 9,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex01
    }, 
    {   // skin 1
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 10,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex01
    }, 
    {   // skin 1
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 11,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex01
    },    
    {   // skin 1
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 12,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex01
    }, 
    {   // skin 1
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 13,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex01
    } 
};

EOconstvector s_eo_vectorof_des_skin_eb10 = 
{
    .capacity       = sizeof(s_des_skin)/sizeof(eOcanmap_entitydescriptor_t),
    .size           = sizeof(s_des_skin)/sizeof(eOcanmap_entitydescriptor_t),
    .item_size      = sizeof(eOcanmap_entitydescriptor_t),
    .dummy          = 0,
    .stored_items   = (void*)s_des_skin,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_descriptor_skin_eb10 = &s_eo_vectorof_des_skin_eb10;



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------


 

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





