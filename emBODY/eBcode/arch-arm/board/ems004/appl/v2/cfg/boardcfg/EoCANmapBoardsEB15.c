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





static const eOcanmap_board_properties_t s_boardprops[] = 
{
    {
        .type               = eobrd_cantype_1foc,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 1         
        },
        .requiredprotocol   = {.major = 1, .minor = 3}
    },  
    {
        .type               = eobrd_cantype_1foc,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 2         
        },
        .requiredprotocol   = {.major = 1, .minor = 3}
    },
    {
        .type               = eobrd_cantype_1foc,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 3         
        },
        .requiredprotocol   = {.major = 1, .minor = 3}
    },
    {
        .type               = eobrd_cantype_1foc,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 4         
        },
        .requiredprotocol   = {.major = 1, .minor = 3}
    }   
};

static EOconstvector s_eo_vectorof_boardprops_eb15 = 
{
    .capacity       = sizeof(s_boardprops)/sizeof(eOcanmap_board_properties_t),
    .size           = sizeof(s_boardprops)/sizeof(eOcanmap_board_properties_t),
    .item_size      = sizeof(eOcanmap_board_properties_t),
    .dummy          = 0,
    .stored_items   = (void*)s_boardprops,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_boardprops_eb15 = &s_eo_vectorof_boardprops_eb15;



static const eOcanmap_entitydescriptor_t s_des_jomo[] = 
{
    {   // jomo 0
        .location   =
        {   
            .port           = eOcanport1,
            .addr           = 1,
            .insideindex    = eocanmap_insideindex_first
        },
        .index      = entindex00
    },        
    {   // jomo 1
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 2,
            .insideindex    = eocanmap_insideindex_first
        },
        .index      = entindex01,        
    },    
    {   // jomo 2
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 3,
            .insideindex    = eocanmap_insideindex_first
        },
        .index      = entindex02
    },
    {   // jomo 3
        .location   =
        {
            .port           = eOcanport1,
            .addr           = 4,
            .insideindex    = eocanmap_insideindex_first
        },
        .index      = entindex03
    }     
};

EOconstvector s_eo_vectorof_des_jomo_eb15 = 
{
    .capacity       = sizeof(s_des_jomo)/sizeof(eOcanmap_entitydescriptor_t),
    .size           = sizeof(s_des_jomo)/sizeof(eOcanmap_entitydescriptor_t),
    .item_size      = sizeof(eOcanmap_entitydescriptor_t),
    .dummy          = 0,
    .stored_items   = (void*)s_des_jomo,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_descriptor_jomo_eb15 = &s_eo_vectorof_des_jomo_eb15;
/*
static const eOcanmap_entitydescriptor_t s_des_strain[] = 
{
    {   // strain 0
        .location   =
        {
            .port           = eOcanport2,
            .addr           = 13,
            .insideindex    = eocanmap_insideindex_none
        },
        .index      = entindex00
    }  
};

EOconstvector s_eo_vectorof_des_strain_eb6 = 
{
    .capacity       = sizeof(s_des_strain)/sizeof(eOcanmap_entitydescriptor_t),
    .size           = sizeof(s_des_strain)/sizeof(eOcanmap_entitydescriptor_t),
    .item_size      = sizeof(eOcanmap_entitydescriptor_t),
    .dummy          = 0,
    .stored_items   = (void*)s_des_strain,
    .functions      = NULL   
};

EOconstvector* eo_vectorof_descriptor_strain_eb6 = &s_eo_vectorof_des_strain_eb6;
*/

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





