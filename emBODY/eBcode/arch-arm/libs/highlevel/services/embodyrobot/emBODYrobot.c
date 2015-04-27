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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "emBODYrobot.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const char * embody_robot_robotnames[] =
{
    "iCubGenova01",     // 00
    "iCubNONE",         // 01
    "iCubNONE",         // 02
    "iCubNONE",         // 03
    "iCubNONE",         // 04
    "iCubNONE",         // 05
    "iCubNONE",         // 06
    "iCubNONE",         // 07
    "iCubNONE",         // 08
    "iCubNONE",         // 09
    "iCubNONE",         // 10 
    "iCubNONE",         // 11
    "iCubNONE",         // 12
    "iCubNONE",         // 13
    "iCubNONE",         // 14
    "iCubNONE",         // 15
    "iCubNONE",         // 16
    "iCubNONE",         // 17
    "iCubNONE",         // 18
    "iCubNONE",         // 19
    "iCubNONE",         // 20     
    "iCubGenova04",     // 21
    "iCubNONE",         // 22
    "iCubNONE",         // 23
    "iCubNONE",         // 24
    "iCubDarmstadt01",  // 25

    "iCubANY",
    "iCubNONE"          // always keep them as the last
};


const char * embody_robot_boardnames[] =
{
    "boardMCBSTM32",        // 00
    "boardMCBSTM32C",       // 01
    "boardMCBSTM32F400",    // 02
    "boardNONE",            // 03
    "boardNONE",            // 04
    "boardNONE",            // 05
    "boardNONE",            // 06
    "boardNONE",            // 07
    "boardNONE",            // 08
    "boardNONE",            // 09
    "boardEMS001",          // 10 
    "boardEMS4RD",          // 11
    "boardMC4PLUS",         // 12

    "boardANY",
    "boardNONE"          // always keep them as the last
};


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
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

