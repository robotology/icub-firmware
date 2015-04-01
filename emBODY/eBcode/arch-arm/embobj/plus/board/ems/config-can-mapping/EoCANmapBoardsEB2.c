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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// - the can boards: 6 4dc, 1 mais, 7 skins 

static eOcanmap_canboard_t s_board_c1a03_mc4 = 
{   
    .board =
    {
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 3         
        },
        .requiredprotocol   = {.major = 1, .minor = 2},
        .indexofentity = 
        {
            entindex00,     // jomo #0
            entindex01      // jomo #1            
        }
    },
    .detected   = {0}
};

static eOcanmap_canboard_t s_board_c1a04_mc4 = 
{   
    .board =
    {
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 4         
        },
        .requiredprotocol   = {.major = 1, .minor = 2},
        .indexofentity = 
        {
            entindex02,     // jomo #2
            entindex03      // jomo #3            
        }
    },
    .detected   = {0}
};


static eOcanmap_canboard_t s_board_c1a05_mc4 = 
{   
    .board =
    {
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 5         
        },
        .requiredprotocol   = {.major = 1, .minor = 2},
        .indexofentity = 
        {
            entindex04,     // jomo #4
            entindex05      // jomo #5            
        }
    },
    .detected   = {0}
};

static eOcanmap_canboard_t s_board_c1a06_mc4 = 
{   
    .board =
    {
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 6         
        },
        .requiredprotocol   = {.major = 1, .minor = 2},
        .indexofentity = 
        {
            entindex06,     // jomo #6
            entindex07      // jomo #7            
        }
    },
    .detected   = {0}
};

static eOcanmap_canboard_t s_board_c1a07_mc4 = 
{   
    .board =
    {
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 7         
        },
        .requiredprotocol   = {.major = 1, .minor = 2},
        .indexofentity = 
        {
            entindex08,     // jomo #8
            entindex09      // jomo #9            
        }
    },
    .detected   = {0}
};


static eOcanmap_canboard_t s_board_c1a08_mc4 = 
{   
    .board =
    {
        .type               = eobrd_cantype_mc4,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 8         
        },
        .requiredprotocol   = {.major = 1, .minor = 2},
        .indexofentity = 
        {
            entindex10,     // jomo #10
            entindex11      // jomo #11            
        }
    },
    .detected   = {0}
};    

static eOcanmap_canboard_t s_board_c1a14_mais = 
{   
    .board =
    {
        .type               = eobrd_cantype_mais,
        .location =
        {
            .port   = eOcanport1,
            .addr   = 14         
        },
        .requiredprotocol   = {.major = 0, .minor = 0},
        .indexofentity = 
        {
            entindex00,     // mais #0
            entindexNONE    // none          
        }
    },
    .detected   = {0}
}; 


static eOcanmap_canboard_t s_board_c2a08_skin = 
{   
    .board =
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 8         
        },
        .requiredprotocol   = {.major = 0, .minor = 0},
        .indexofentity = 
        {
            entindex00,     // skin #0
            entindexNONE    // none          
        }
    },
    .detected   = {0}
}; 



static eOcanmap_canboard_t s_board_c2a09_skin = 
{   
    .board =
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 9         
        },
        .requiredprotocol   = {.major = 0, .minor = 0},
        .indexofentity = 
        {
            entindex00,     // skin #0
            entindexNONE    // none          
        }
    },
    .detected   = {0}
}; 



static eOcanmap_canboard_t s_board_c2a10_skin = 
{   
    .board =
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 10         
        },
        .requiredprotocol   = {.major = 0, .minor = 0},
        .indexofentity = 
        {
            entindex00,     // skin #0
            entindexNONE    // none          
        }
    },
    .detected   = {0}
}; 


static eOcanmap_canboard_t s_board_c2a11_skin = 
{   
    .board =
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 11         
        },
        .requiredprotocol   = {.major = 0, .minor = 0},
        .indexofentity = 
        {
            entindex00,     // skin #0
            entindexNONE    // none          
        }
    },
    .detected   = {0}
}; 


static eOcanmap_canboard_t s_board_c2a12_skin = 
{   
    .board =
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 12         
        },
        .requiredprotocol   = {.major = 0, .minor = 0},
        .indexofentity = 
        {
            entindex00,     // skin #0
            entindexNONE    // none          
        }
    },
    .detected   = {0}
}; 


static eOcanmap_canboard_t s_board_c2a13_skin = 
{   
    .board =
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 13         
        },
        .requiredprotocol   = {.major = 0, .minor = 0},
        .indexofentity = 
        {
            entindex00,     // skin #0
            entindexNONE    // none          
        }
    },
    .detected   = {0}
}; 

static eOcanmap_canboard_t s_board_c2a14_skin = 
{   
    .board =
    {
        .type               = eobrd_cantype_skin,
        .location =
        {
            .port   = eOcanport2,
            .addr   = 14         
        },
        .requiredprotocol   = {.major = 0, .minor = 0},
        .indexofentity = 
        {
            entindex00,     // skin #0
            entindexNONE    // none          
        }
    },
    .detected   = {0}
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
const uint8_t eo_canmapcfg_joints_numberof = 12;

// this variable contains the pointers of the boards which serve a given joint in number [0, eo_canmapcfg_joints_numberof-1]
const eOcanmap_canboard_t * const eo_canmapcfg_joints[] =
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



// - motors

// this variable contains the number of motors.
const uint8_t eo_canmapcfg_motors_numberof = 12;

// this variable contains the pointers of the boards which serve a given motor in number [0, eo_canmapcfg_motors_numberof-1]
const eOcanmap_canboard_t * const eo_canmapcfg_motors[] =
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
const uint8_t eo_canmapcfg_maises_numberof = 1;

// this variable contains the pointers of the boards which serve a given mais in number [0, eo_canmapcfg_maises_numberof-1]
const eOcanmap_canboard_t * const eo_canmapcfg_maises[] =
{
    &s_board_c1a14_mais
};




// - skins

// this variable tells how many skins there are
const uint8_t eo_canmapcfg_skins_numberof = 1;

// this array tells for each skin in value [0, eo_canmapcfg_skins_numberof-1] how many boards are used to implement it
const uint8_t eo_canmapcfg_skins_boardsinside[] = 
{
    7
};

static const eOcanmap_canboard_t * const s_eo_canmapcfg_skin0[] =
{
    &s_board_c2a08_skin,
    &s_board_c2a09_skin,
    &s_board_c2a10_skin,
    &s_board_c2a11_skin,
    &s_board_c2a12_skin,
    &s_board_c2a13_skin,
    &s_board_c2a14_skin
};


// this variable contains for each skin in value [0, eo_canmapcfg_skins_numberof-1] the array of the skin boards  
const eOcanmap_canboard_t * const * const eo_canmapcfg_skins_boards[] =
{
    s_eo_canmapcfg_skin0
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





