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

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANmappingCfg.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// - the can boards: none 

// - the can mapping

static const eOcanmap_canboard_t * const s_boards_can1[] =
{   // they must be 15
    NULL,   // the ems has address 0
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL, 
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};  EO_VERIFYsizeof(s_boards_can1, 15*sizeof(eOcanmap_canboard_t *)); 

static const eOcanmap_canboard_t * const s_boards_can2[] =
{   // they must be 15
    NULL,   // the ems has address 0
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL, 
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL
};  EO_VERIFYsizeof(s_boards_can2, 15*sizeof(eOcanmap_canboard_t *));



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

// this variable contains the pointers of the canboards which are in the two can buses in each of the [0, 14] address
// if no can board is present at a given address, then we have NULL.  
eOcanmap_canboard_t * const * const eo_canmapcfg_boards[] =
{   
    (eOcanmap_canboard_t * const *)&s_boards_can1,   
    (eOcanmap_canboard_t * const *)&s_boards_can2
};  EO_VERIFYsizeof(eo_canmapcfg_boards, 2*sizeof(eOcanmap_canboard_t * const *));  


// - joints

// this variable contains the number of joints.
const uint8_t eo_canmapcfg_joints_numberof = 0;

// this variable contains the pointers of the boards which serve a given joint in number [0, eo_canmapcfg_joints_numberof-1]
const eOcanmap_canboard_t * const eo_canmapcfg_joints[] =
{
    NULL
};



// - motors

// this variable contains the number of motors.
const uint8_t eo_canmapcfg_motors_numberof = 0;

// this variable contains the pointers of the boards which serve a given motor in number [0, eo_canmapcfg_motors_numberof-1]
const eOcanmap_canboard_t * const eo_canmapcfg_motors[] =
{
    NULL
};    



// - strains

// this variable contains the number of strains.
const uint8_t eo_canmapcfg_strains_numberof = 0;

// this variable contains the pointers of the boards which serve a given strain in number [0, eo_canmapcfg_strains_numberof-1]
const eOcanmap_canboard_t * const eo_canmapcfg_strains[] =
{
    NULL
};



// - maises

// this variable contains the number of maises
const uint8_t eo_canmapcfg_maises_numberof = 0;

// this variable contains the pointers of the boards which serve a given mais in number [0, eo_canmapcfg_maises_numberof-1]
const eOcanmap_canboard_t * const eo_canmapcfg_maises[] =
{
    NULL
};




// - skins

// this variable tells how many skins there are
const uint8_t eo_canmapcfg_skins_numberof = 0;

// this array tells for each skin in value [0, eo_canmapcfg_skins_numberof-1] how many boards are used to implement it
const uint8_t eo_canmapcfg_skins_boardsinside[] = 
{
    0
};

// this variable contains for each skin in value [0, eo_canmapcfg_skins_numberof-1] the array of the skin boards  
const eOcanmap_canboard_t * const * const eo_canmapcfg_skins_boards[] =
{
    NULL
};


 

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





