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

// - the can boards: 6 4dc, 1 mais, 7 skins 

static const eOcanmap_canboard_t s_board_c1a03_mc4 = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_mc4,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 1, .minor = 2}        
    },
    .location   =
    {
        .port       = eOcanport1,
        .addr       = 3         
    },
    .indexentity0     = 0,      // jomo #0
    .indexentity1     = 1       // jomo #1
};

static const eOcanmap_canboard_t s_board_c1a04_mc4 = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_mc4,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 1, .minor = 2}        
    },
    .location   =
    {
        .port       = eOcanport1,
        .addr       = 4         
    },
    .indexentity0     = 2,      // jomo #2
    .indexentity1     = 3       // jomo #3
};

static const eOcanmap_canboard_t s_board_c1a05_mc4 = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_mc4,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 1, .minor = 2}        
    },
    .location   =
    {
        .port       = eOcanport1,
        .addr       = 5         
    },
    .indexentity0     = 4,      // jomo #4
    .indexentity1     = 5       // jomo #5
};

static const eOcanmap_canboard_t s_board_c1a06_mc4 = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_mc4,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 1, .minor = 2}        
    },
    .location   =
    {
        .port       = eOcanport1,
        .addr       = 6         
    },
    .indexentity0     = 6,      // jomo #6
    .indexentity1     = 7       // jomo #7
};

static const eOcanmap_canboard_t s_board_c1a07_mc4 = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_mc4,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 1, .minor = 2}        
    },
    .location   =
    {
        .port       = eOcanport1,
        .addr       = 7         
    },
    .indexentity0     = 8,      // jomo #8
    .indexentity1     = 9       // jomo #9
};

static const eOcanmap_canboard_t s_board_c1a08_mc4 = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_mc4,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 1, .minor = 2}        
    },
    .location   =
    {
        .port       = eOcanport1,
        .addr       = 8         
    },
    .indexentity0     = 10,     // jomo #10
    .indexentity1     = 11      // jomo #11
};

static const eOcanmap_canboard_t s_board_c1a14_mais = 
{   
    .board      =
    {
        .boardtype              = eobrd_cantype_mais,
        .firmwarebuildnumber    = 0,
        .firmwareversion        = {.major = 0, .minor = 0},
        .protocolversion        = {.major = 0, .minor = 0}        
    },
    .location   =
    {
        .port       = eOcanport1,
        .addr       = 14         
    },
    .indexentity0     = 0,      // mais #0
    .indexentity1     = 15      // none
};


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
    .indexentity0     = 0,      // skin #0
    .indexentity1     = 15      // none
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
    .indexentity0     = 0,      // skin #0
    .indexentity1     = 15      // none
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
    .indexentity0     = 0,      // skin #0
    .indexentity1     = 15      // none
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
    .indexentity0     = 0,      // skin #0
    .indexentity1     = 15      // none
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
    .indexentity0     = 0,      // skin #0
    .indexentity1     = 15      // none
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
    .indexentity0     = 0,      // skin #0
    .indexentity1     = 15      // none
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
    .indexentity0     = 0,      // skin #0
    .indexentity1     = 15      // none
};


// - the can mapping

static const eOcanmap_canboard_t * const s_boards_can1[] =
{   // they must be 15
    NULL,   // the ems has address 0
    NULL,
    NULL,
    &s_board_c1a03_mc4,
    &s_board_c1a04_mc4,
    &s_board_c1a05_mc4,
    &s_board_c1a06_mc4,
    &s_board_c1a07_mc4,
    &s_board_c1a08_mc4, 
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    &s_board_c1a14_mais
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
    &s_board_c2a08_skin,
    &s_board_c2a09_skin,
    &s_board_c2a10_skin,
    &s_board_c2a11_skin,
    &s_board_c2a12_skin,
    &s_board_c2a13_skin,
    &s_board_c2a14_skin
};  EO_VERIFYsizeof(s_boards_can2, 15*sizeof(eOcanmap_canboard_t *));


#elif   defined(BOARD_EB1)


// - the can boards: 4 1foc, 1 strain

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
    .indexentity0     = 0,      // jomo #0
    .indexentity1     = 15      // none
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
    .indexentity0     = 1,      // jomo #1
    .indexentity1     = 15      // none
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
    .indexentity0     = 2,      // jomo #2
    .indexentity1     = 15      // none
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
    .indexentity0     = 3,      // jomo #3
    .indexentity1     = 15      // none
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
    .indexentity0   = 0,        // strain #0
    .indexentity1   = 15        // none    
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



#endif

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


// - joints

// this variable contains the pointers of the boards which serve a given joint in number [0, eo_canmapping_joints_numberof-1]
#if     defined(BOARD_EB2)
const eOcanmap_canboard_t * const eo_canmapping_joints[] =
{
    &s_board_c1a03_mc4,     // j0
    &s_board_c1a03_mc4,     // j1
    &s_board_c1a04_mc4,     // j2
    &s_board_c1a04_mc4,     // j3
    &s_board_c1a05_mc4,     // j4
    &s_board_c1a05_mc4,     // j5
    &s_board_c1a06_mc4,     // j6
    &s_board_c1a06_mc4,     // j7
    &s_board_c1a07_mc4,     // j8
    &s_board_c1a07_mc4,     // j9
    &s_board_c1a08_mc4,     // j10
    &s_board_c1a08_mc4      // j11
};
#elif   defined(BOARD_EB1)
const eOcanmap_canboard_t * const eo_canmapping_joints[] =
{
    &s_board_c1a01_1foc,    // j0
    &s_board_c1a02_1foc,    // j1
    &s_board_c1a03_1foc,    // j2
    &s_board_c1a04_1foc     // j3
};
#endif

// this variable contains the size of the array eo_canmapping_joints, or the number of joints.
const uint8_t eo_canmapping_joints_numberof = sizeof(eo_canmapping_joints) / sizeof(const eOcanmap_canboard_t *);


// - motors

// this variable contains the pointers of the boards which serve a given motor in number [0, eo_canmapping_motors_numberof-1]
#if     defined(BOARD_EB2)
const eOcanmap_canboard_t * const eo_canmapping_motors[] =
{
    &s_board_c1a03_mc4,     // m0
    &s_board_c1a03_mc4,     // m1
    &s_board_c1a04_mc4,     // m2
    &s_board_c1a04_mc4,     // m3
    &s_board_c1a05_mc4,     // m4
    &s_board_c1a05_mc4,     // m5
    &s_board_c1a06_mc4,     // m6
    &s_board_c1a06_mc4,     // m7
    &s_board_c1a07_mc4,     // m8
    &s_board_c1a07_mc4,     // m9
    &s_board_c1a08_mc4,     // m10
    &s_board_c1a08_mc4      // m11
};    
#elif   defined(BOARD_EB1)
const eOcanmap_canboard_t * const eo_canmapping_motors[] =
{
    &s_board_c1a01_1foc,    // m0
    &s_board_c1a02_1foc,    // m1
    &s_board_c1a03_1foc,    // m2
    &s_board_c1a04_1foc     // m3
};
#endif

// this variable contains the size of the array eo_canmapping_motors, or the number of motors.
const uint8_t eo_canmapping_motors_numberof = sizeof(eo_canmapping_motors) / sizeof(const eOcanmap_canboard_t *);


// - strains

#if     defined(BOARD_EB2)
// this variable contains the pointers of the boards which serve a given strain in number [0, eo_canmapping_strains_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_strains[] =
{
    NULL
};
// this variable contains the size of the array eo_canmapping_strains, or the number of strains.
const uint8_t eo_canmapping_strains_numberof = 0;
#elif   defined(BOARD_EB1)
// this variable contains the pointers of the boards which serve a given strain in number [0, eo_canmapping_strains_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_strains[] =
{
    &s_board_c2a13_strain
};
// this variable contains the size of the array eo_canmapping_strains, or the number of strains.
const uint8_t eo_canmapping_strains_numberof = sizeof(eo_canmapping_strains) / sizeof(const eOcanmap_canboard_t *);
#endif


// - maises

#if     defined(BOARD_EB2)
// this variable contains the pointers of the boards which serve a given mais in number [0, eo_canmapping_maises_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_maises[] =
{
    &s_board_c1a14_mais
};

const uint8_t eo_canmapping_maises_numberof = sizeof(eo_canmapping_maises) / sizeof(const eOcanmap_canboard_t *);
#elif   defined(BOARD_EB1)
// this variable contains the pointers of the boards which serve a given mais in number [0, eo_canmapping_maises_numberof-1]
const eOcanmap_canboard_t * const eo_canmapping_maises[] =
{
    NULL
};

const uint8_t eo_canmapping_maises_numberof = 0;
#endif


// - skins

#if     defined(BOARD_EB2)

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
    s_eo_canmapping_skin0
};


#elif   defined(BOARD_EB1)

const uint8_t eo_canmapping_skins_numberof = 0;

const uint8_t eo_canmapping_skins_boardsinside[] = 
{
    0
};

const eOcanmap_canboard_t * const *const eo_canmapping_skins_boards[] =
{
    NULL
};

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





