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

#include "EoCANmapBoards.h"


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


// - the can boards: 4 1foc, 1 strain

static eOcanmap_canboard_t s_board_c1a01_1foc = 
{  
    .board =
    {
        .type               = eobrd_cantype_1foc,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 1         
        },
        .requiredprotocol   = {.major = 1, .minor = 2},
        .indexofentity = 
        {
            entindex00,     // jomo #0
            entindexNONE    // none            
        }
    },
    .detected   = {0} 
};


static const eOcanmap_canboard_t s_board_c1a02_1foc = 
{   
    .board =
    {
        .type               = eobrd_cantype_1foc,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 2         
        },
        .requiredprotocol   = {.major = 1, .minor = 2},
        .indexofentity = 
        {
            entindex01,     // jomo #1
            entindexNONE    // none            
        }
    },
    .detected   = {0} 
};


static const eOcanmap_canboard_t s_board_c1a03_1foc = 
{ 
    .board =
    {
        .type               = eobrd_cantype_1foc,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 3         
        },
        .requiredprotocol   = {.major = 1, .minor = 2},
        .indexofentity = 
        {
            entindex02,     // jomo #2
            entindexNONE    // none            
        }
    },
    .detected   = {0}  
};

static const eOcanmap_canboard_t s_board_c1a04_1foc = 
{
    .board =
    {
        .type               = eobrd_cantype_1foc,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 4         
        },
        .requiredprotocol   = {.major = 1, .minor = 2},
        .indexofentity = 
        {
            entindex03,     // jomo #3
            entindexNONE    // none            
        }
    },
    .detected   = {0}     
};


static const eOcanmap_canboard_t s_board_c2a13_strain = 
{
    .board =
    {
        .type               = eobrd_cantype_1foc,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 13         
        },
        .requiredprotocol   = {.major = 0, .minor = 0},
        .indexofentity = 
        {
            entindex00,     // strain #0
            entindexNONE    // none            
        }
    },
    .detected   = {0}   
};



// - the can mapping

static const eOcanmap_canboard_t * const s_boards_can1[] =
{   // they must be 15
    NULL,   // the ems has address 0
    &s_board_c1a01_1foc,
    &s_board_c1a02_1foc,
    &s_board_c1a03_1foc,
    &s_board_c1a04_1foc,
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
    &s_board_c2a13_strain,
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
eOcanmap_canboard_t * const * const eo_canmapping_boards[] =
{   
    (eOcanmap_canboard_t * const *)&s_boards_can1,   
    (eOcanmap_canboard_t * const *)&s_boards_can2
};  EO_VERIFYsizeof(eo_canmapping_boards, 2*sizeof(eOcanmap_canboard_t * const *));   


// - joints

// this variable contains the number of joints.
const uint8_t eo_canmapping_joints_numberof = 4;

// this variable contains the pointers of the boards which serve a given joint in number [0, eo_canmapping_joints_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_joints[] =
{
    &s_board_c1a01_1foc,    // j0
    &s_board_c1a02_1foc,    // j1
    &s_board_c1a03_1foc,    // j2
    &s_board_c1a04_1foc     // j3
};



// - motors

// this variable contains the number of motors.
const uint8_t eo_canmapping_motors_numberof = 4;

// this variable contains the pointers of the boards which serve a given motor in number [0, eo_canmapping_motors_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_motors[] =
{
    &s_board_c1a01_1foc,    // m0
    &s_board_c1a02_1foc,    // m1
    &s_board_c1a03_1foc,    // m2
    &s_board_c1a04_1foc     // m3
};



// - strains

// this variable contains the number of strains.
const uint8_t eo_canmapping_strains_numberof = 1;

// this variable contains the pointers of the boards which serve a given strain in number [0, eo_canmapping_strains_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_strains[] =
{
    &s_board_c2a13_strain
};


// - maises

// this variable contains the number of maises
const uint8_t eo_canmapping_maises_numberof = 0;

// this variable contains the pointers of the boards which serve a given mais in number [0, eo_canmapping_maises_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_maises[] =
{
    NULL
};




// - skins

// this variable tells how many skins there are
const uint8_t eo_canmapping_skins_numberof = 0;

// this array tells for each skin in value [0, eo_canmapping_skins_numberof-1] how many boards are used to implement it
const uint8_t eo_canmapping_skins_boardsinside[] = 
{
    0
};

// this variable contains for each skin in value [0, eo_canmapping_skins_numberof-1] the array of the skin boards 
const eOcanmap_canboard_t * const *const eo_canmapping_skins_boards[] =
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





