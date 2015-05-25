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
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 3         
        },
        .requiredprotocol   = {.major = 1, .minor = 2}
    },  
    {
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 4         
        },
        .requiredprotocol   = {.major = 1, .minor = 2}
    },
    {
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 5         
        },
        .requiredprotocol   = {.major = 1, .minor = 2}
    },
    {
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 6         
        },
        .requiredprotocol   = {.major = 1, .minor = 2}
    },
    {
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 7         
        },
        .requiredprotocol   = {.major = 1, .minor = 2}
    },
    {
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 8         
        },
        .requiredprotocol   = {.major = 1, .minor = 2}
    },    
    {
        .type               = eobrd_cantype_mais,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 14         
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
    }, 
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 14         
        },
        .requiredprotocol   = {.major = 0, .minor = 0}
    }      
};



EOconstvector s_eo_vectorof_boardprops_eb2 = 
{
    .capacity       = sizeof(s_boardprops)/sizeof(eOcanmap_board_properties_t),
    .size           = sizeof(s_boardprops)/sizeof(eOcanmap_board_properties_t),
    .item_size      = sizeof(eOcanmap_board_properties_t),
    .dummy          = 0,
    .stored_items   = (void*)s_boardprops,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_boardprops_eb2 = &s_eo_vectorof_boardprops_eb2;



static const eOcanmap_entitydescriptor_t s_des_jomo[] = 
{
    {   // jomo 0
        .location   =
        {   
            .port           = eOcanport1,
            .addr           = 3,
            .insideindex    = eocanmap_insideindex_first
        },
        .index      = entindex00
    },        
    {   // jomo 1
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 3,
            .insideindex    = eocanmap_insideindex_second
        },
        .index      = entindex01,        
    },    
    {   // jomo 2
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 4,
            .insideindex    = eocanmap_insideindex_first
        },
        .index      = entindex02
    },
    {   // jomo 3
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 4,
            .insideindex    = eocanmap_insideindex_second
        },
        .index      = entindex03
    }, 
    {   // jomo 4
        .location   =
        {   
            .port           = eOcanport1,
            .addr           = 5,
            .insideindex    = eocanmap_insideindex_first
        },
        .index      = entindex04
    },        
    {   // jomo 5
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 5,
            .insideindex    = eocanmap_insideindex_second
        },
        .index      = entindex05,        
    },    
    {   // jomo 6
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 6,
            .insideindex    = eocanmap_insideindex_first
        },
        .index      = entindex06
    },
    {   // jomo 7
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 6,
            .insideindex    = eocanmap_insideindex_second
        },
        .index      = entindex07
    }, 
    {   // jomo 8
        .location   =
        {   
            .port           = eOcanport1,
            .addr           = 7,
            .insideindex    = eocanmap_insideindex_first
        },
        .index      = entindex08
    },        
    {   // jomo 9
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 7,
            .insideindex    = eocanmap_insideindex_second
        },
        .index      = entindex09,        
    },    
    {   // jomo 10
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 8,
            .insideindex    = eocanmap_insideindex_first
        },
        .index      = entindex10
    },
    {   // jomo 11
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 8,
            .insideindex    = eocanmap_insideindex_second
        },
        .index      = entindex11
    }    
};

EOconstvector s_eo_vectorof_des_jomo_eb2 = 
{
    .capacity       = sizeof(s_des_jomo)/sizeof(eOcanmap_entitydescriptor_t),
    .size           = sizeof(s_des_jomo)/sizeof(eOcanmap_entitydescriptor_t),
    .item_size      = sizeof(eOcanmap_entitydescriptor_t),
    .dummy          = 0,
    .stored_items   = (void*)s_des_jomo,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_descriptor_jomo_eb2 = &s_eo_vectorof_des_jomo_eb2;




EOconstvector s_eo_vectorof_des_strain_eb2 = 
{
    .capacity       = 0,
    .size           = 0,
    .item_size      = sizeof(eOcanmap_entitydescriptor_t),
    .dummy          = 0,
    .stored_items   = NULL,
    .functions      = NULL   
};


EOconstvector* eo_vectorof_descriptor_strain_eb2 = &s_eo_vectorof_des_strain_eb2;



static const eOcanmap_entitydescriptor_t s_des_mais[] = 
{
    {   // mais 0
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 14,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    }  
};

EOconstvector s_eo_vectorof_des_mais_eb2 = 
{
    .capacity       = sizeof(s_des_mais)/sizeof(eOcanmap_entitydescriptor_t),
    .size           = sizeof(s_des_mais)/sizeof(eOcanmap_entitydescriptor_t),
    .item_size      = sizeof(eOcanmap_entitydescriptor_t),
    .dummy          = 0,
    .stored_items   = (void*)s_des_mais,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_descriptor_mais_eb2 = &s_eo_vectorof_des_mais_eb2;



// in case, as in eb10 / eb11, there are two skins, in here we put in .index: entindex00 or entindex00 depending on the cases. 
// as i s_des_jomo ...
static const eOcanmap_entitydescriptor_t s_des_skin[] = 
{
    {   // skin 0
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 8,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    },
    {   // skin 0
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 9,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    }, 
    {   // skin 0
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 10,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    }, 
    {   // skin 0
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 11,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    },    
    {   // skin 0
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 12,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    }, 
    {   // skin 0
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 13,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    },
    {   // skin 0
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 14,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    }    
};

EOconstvector s_eo_vectorof_des_skin_eb2 = 
{
    .capacity       = sizeof(s_des_skin)/sizeof(eOcanmap_entitydescriptor_t),
    .size           = sizeof(s_des_skin)/sizeof(eOcanmap_entitydescriptor_t),
    .item_size      = sizeof(eOcanmap_entitydescriptor_t),
    .dummy          = 0,
    .stored_items   = (void*)s_des_skin,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_descriptor_skin_eb2 = &s_eo_vectorof_des_skin_eb2;




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


//// - skins

//// this variable tells how many skins there are
//const uint8_t eo_canmapcfg_skins_numberof = 1;

//// this array tells for each skin in value [0, eo_canmapcfg_skins_numberof-1] how many boards are used to implement it
//const uint8_t eo_canmapcfg_skins_boardsinside[] = 
//{
//    7
//};

//static const eOcanmap_board_extended_t * const s_eo_canmapcfg_skin0[] =
//{
//    NULL,
//    NULL,
//    NULL,
//    NULL,
//    NULL,
//    NULL,
//    NULL
//};


//// this variable contains for each skin in value [0, eo_canmapcfg_skins_numberof-1] the array of the skin boards  
//const eOcanmap_board_extended_t * const * const eo_canmapcfg_skins_boards[] =
//{
//    s_eo_canmapcfg_skin0
//};


 

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



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





