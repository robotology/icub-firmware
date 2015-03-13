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

#define BOARD_EB2


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


#if     defined(BOARD_EB2)
// puts the 7 skins

static const eOcanmap_canboard_t s_board_c2a08_skin = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_skin,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 0, .minor = 0}        
    },
    .location   =
    {
        .port       = eOcanport2,
        .addr       = 8         
    },
    .indexentity0     = 0,    // jomo 0
    .indexentity1     = 15    // jomo none
};


static const eOcanmap_canboard_t s_board_c2a09_skin = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_skin,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 0, .minor = 0}        
    },
    .location   =
    {
        .port       = eOcanport2,
        .addr       = 9         
    },
    .indexentity0     = 0,    // jomo 0
    .indexentity1     = 15    // jomo none
};


static const eOcanmap_canboard_t s_board_c2a10_skin = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_skin,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 0, .minor = 0}        
    },
    .location   =
    {
        .port       = eOcanport2,
        .addr       = 10         
    },
    .indexentity0     = 0,    // jomo 0
    .indexentity1     = 15    // jomo none
};

static const eOcanmap_canboard_t s_board_c2a11_skin = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_skin,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 0, .minor = 0}        
    },
    .location   =
    {
        .port       = eOcanport2,
        .addr       = 11         
    },
    .indexentity0     = 0,    // jomo 0
    .indexentity1     = 15    // jomo none
};


static const eOcanmap_canboard_t s_board_c2a12_skin = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_skin,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 0, .minor = 0}        
    },
    .location   =
    {
        .port       = eOcanport2,
        .addr       = 12         
    },
    .indexentity0     = 0,    // jomo 0
    .indexentity1     = 15    // jomo none
};


static const eOcanmap_canboard_t s_board_c2a13_skin = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_skin,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 0, .minor = 0}        
    },
    .location   =
    {
        .port       = eOcanport2,
        .addr       = 13         
    },
    .indexentity0     = 0,    // jomo 0
    .indexentity1     = 15    // jomo none
};

static const eOcanmap_canboard_t s_board_c2a14_skin = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_skin,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 0, .minor = 0}        
    },
    .location   =
    {
        .port       = eOcanport2,
        .addr       = 14        
    },
    .indexentity0     = 0,    // jomo 0
    .indexentity1     = 15    // jomo none
};


static const eOcanmap_canboard_t * const s_eo_canmapping_skin0[] =
{
    &s_board_c2a08_skin,
    &s_board_c2a09_skin,
    &s_board_c2a10_skin,
    &s_board_c2a11_skin,
    &s_board_c2a12_skin,
    &s_board_c2a13_skin,
    &s_board_c2a14_skin
};

#endif


static const eOcanmap_canboard_t s_board_c1a01_1foc = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_1foc,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 1, .minor = 2}        
    },
    .location   =
    {
        .port       = eOcanport1,
        .addr       = 1         
    },
    .indexentity0     = 0,    // jomo 0
    .indexentity1     = 15    // jomo none
};


static const eOcanmap_canboard_t s_board_c1a02_1foc = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_1foc,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 1, .minor = 2}        
    },
    .location   =
    {
        .port       = eOcanport1,
        .addr       = 2         
    }, 
    .indexentity0     = 1,    // jomo 1
    .indexentity1     = 15    // jomo none
};


static const eOcanmap_canboard_t s_board_c1a03_1foc = 
{  
    .board      =
    {
        .boardtype              = eobrd_cantype_1foc,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 1, .minor = 2}        
    },
    .location   =
    {
        .port       = eOcanport1,
        .addr       = 3         
    }, 
    .indexentity0     = 2,    // jomo 2
    .indexentity1     = 15    // jomo none
};

static const eOcanmap_canboard_t s_board_c1a04_1foc = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_1foc,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 1, .minor = 2}        
    },
    .location   =
    {
        .port       = eOcanport1,
        .addr       = 4         
    }, 
    .indexentity0     = 3,    // jomo 3
    .indexentity1     = 15    // jomo none
};


static const eOcanmap_canboard_t s_board_c2a13_strain = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_strain,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 0, .minor = 0}        
    },
    .location   =
    {
        .port       = eOcanport2,
        .addr       = 13         
    },
    .indexentity0   = 0,    // the first and only
    .indexentity1   = 15    // none    
};



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
};


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
};


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
const eOcanmap_canboard_t * const * const eo_canmapping_boards[] =
{   
    (const eOcanmap_canboard_t * const *)&s_boards_can1,   
    (const eOcanmap_canboard_t * const *)&s_boards_can2
};  EO_VERIFYsizeof(eo_canmapping_boards, 2*sizeof(const eOcanmap_canboard_t * const *));  


// this variable contains the pointers of the boards which serve a given joint in number [0, eo_canmapping_joints_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_joints[] =
{
    &s_board_c1a01_1foc,    // j0
    &s_board_c1a02_1foc,    // j1
    &s_board_c1a03_1foc,    // j2
    &s_board_c1a04_1foc     // j3
};

// this variable contains the size of the array eo_canmapping_joints, or the number of joints.
const uint8_t eo_canmapping_joints_numberof = sizeof(eo_canmapping_joints) / sizeof(const eOcanmap_canboard_t *);

// this variable contains the pointers of the boards which serve a given motor in number [0, eo_canmapping_motors_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_motors[] =
{
    &s_board_c1a01_1foc,    // m0
    &s_board_c1a02_1foc,    // m1
    &s_board_c1a03_1foc,    // m2
    &s_board_c1a04_1foc     // m3
};

// this variable contains the size of the array eo_canmapping_motors, or the number of motors.
const uint8_t eo_canmapping_motors_numberof = sizeof(eo_canmapping_motors) / sizeof(const eOcanmap_canboard_t *);


// this variable contains the pointers of the boards which serve a given strain in number [0, eo_canmapping_strains_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_strains[] =
{
    &s_board_c2a13_strain
};

// this variable contains the size of the array eo_canmapping_strains, or the number of strains.
const uint8_t eo_canmapping_strains_numberof = sizeof(eo_canmapping_strains) / sizeof(const eOcanmap_canboard_t *);

// this variable contains the pointers of the boards which serve a given mais in number [0, eo_canmapping_maises_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_maises[] =
{
    NULL
};

const uint8_t eo_canmapping_maises_numberof = 0;


#if     defined(BOARD_EB1)

const eOcanmap_canboard_t * const *const eo_canmapping_skins_boards[] =
{
    NULL,
    NULL
};

const uint8_t eo_canmapping_skins_numberof = 0;

#elif   defined(BOARD_EB2)
  
 
// this variable tells how many skins there are
const uint8_t eo_canmapping_skins_numberof = 1;

// this array tells for each skin in value [0, eo_canmapping_skins_numberof-1] how many boards are used to implement it
const uint8_t eo_canmapping_skins_boardsinside[] = 
{
    sizeof(s_eo_canmapping_skin0) / sizeof(const eOcanmap_canboard_t *)
};

// this variable contains for each skin in value [0, eo_canmapping_skins_numberof-1] the array of the skin boards  
const eOcanmap_canboard_t * const * const eo_canmapping_skins_boards[] =
{
    ///// non va bene (const eOcanmap_canboard_t **)&s_eo_canmapping_skin0
    s_eo_canmapping_skin0
};



#elif   defined(BOARD_EB10)


#endif

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





