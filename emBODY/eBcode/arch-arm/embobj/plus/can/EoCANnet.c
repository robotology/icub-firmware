/*
 * Copyright (C) 2014 iCub Facility - Istituto Italiano di Tecnologia
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

/* @file       EoCANnet.c
    @brief      This file keeps ...
    @author     marco.accame@iit.it
    @date       Nov 10 2014
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"
#include "EOconstvector_hid.h"
#include "EOtheCANmapping.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoCANnet.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------

extern const EOconstvector * const eo_vectorof_descriptor_jomo_eb1;
extern const EOconstvector * const eo_vectorof_descriptor_strain_eb1;
extern const EOconstvector * const eo_vectorof_descriptor_mais_eb1;

extern EOconstvector s_eo_vectorof_des_jomo_eb1;
extern EOconstvector s_eo_vectorof_des_strain_eb1;
extern EOconstvector s_eo_vectorof_des_mais_eb1;

const EOconstvector s_eo_empty_constvector_board = 
{
    .capacity       = 0,
    .size           = 0,
    .item_size      = sizeof(eOcanmap_board_properties_t),
    .dummy          = 0,
    .stored_items   = NULL,
    .functions      = NULL   
};

const EOconstvector s_eo_empty_constvector_entity = 
{
    .capacity       = 0,
    .size           = 0,
    .item_size      = sizeof(eOcanmap_entitydescriptor_t),
    .dummy          = 0,
    .stored_items   = NULL,
    .functions      = NULL   
};

const EOconstvector * const entitiesmapB1[eoprot_endpoints_numberof][3] =
{
    { // mn
        &s_eo_empty_constvector_entity, &s_eo_empty_constvector_entity, &s_eo_empty_constvector_entity
    },
   
    { // mc
        &s_eo_vectorof_des_jomo_eb1, &s_eo_vectorof_des_jomo_eb1, &s_eo_empty_constvector_entity
    }, 

    { // as
        &s_eo_vectorof_des_strain_eb1, &s_eo_vectorof_des_mais_eb1, &s_eo_empty_constvector_entity
    },
    { // sk
        &s_eo_empty_constvector_entity, &s_eo_empty_constvector_entity, &s_eo_empty_constvector_entity
    }

};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOconstvector* eo_vectorof_boardprops_eb1;
extern EOconstvector* eo_vectorof_boardprops_eb2;

// of eOcanmap_board_properties_t
// the constvector contains all the boards in can1 and can2. or ... is empty.
extern EOconstvector * eocannet_code2boards(uint32_t code)
{
    EOconstvector *ret = (EOconstvector*) &s_eo_empty_constvector_board;
    
    // so far the code is just an index equal to board number but starting from 0. eb1 is 0, etc....
    switch(code)
    {
        case 0:     ret = eo_vectorof_boardprops_eb1; 
        break;
        
        case 1:     ret = eo_vectorof_boardprops_eb2; 
        break;        
    
        default:    ret = ret;
        break;
    
    }

    return(ret);
} 

// of eOcanmap_entitydescriptor_t
// teh constnector contains reference to the boards used to offer service to a given entity.
// or ... is empty in case of no such an entity on teh board or entity being served not by can
extern EOconstvector * eocannet_code2entitydescriptors(uint32_t code, eOprotEndpoint_t ep, eOprotEntity_t entity)
{
    EOconstvector *ret = (EOconstvector*) &s_eo_empty_constvector_entity;
    
    // so far the code is just an index equal to board number but starting from 0. eb1 is 0, etc....
    switch(code)
    {
        case 0:     ret = (EOconstvector*)entitiesmapB1[ep][entity]; 
        break;
        
        case 1:     ret = ret; 
        break;        
    
        default:    ret = ret;
        break;
    
    }

    return(ret);
}


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



