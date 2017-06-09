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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _emBODYrobot_H_
#define _emBODYrobot_H_

#ifdef __cplusplus
extern "C" {
#endif



/** @file       emBODYrobot.h
    @brief      This header file implements public interface for robot definition
    @author     marco.accame@iit.it
    @date       6/25/2014
**/

/** @defgroup eo_robot robot definition of robot and boards inside emBODY
    the user must define in it environment the following macros:
    EMB
    
    
    
    @todo doxy documentation
    
   @{        
 */
 
  

// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"

// - public #define  --------------------------------------------------------------------------------------------------

// - the environment must define the following macros ... otherwise they are set to NONE values
//   -DemBODYrobot_ROBOT_NAME=iCubXXX, -DemBODYrobot_BOARD_NAME=boardYYY, -DemBODYrobot_BOARD_NUMBER=number
//   w/ number is 1, 2, etc

// - list of supported robots. the iCub* strings are equal to YARP_ROBOT_NAME and values are equal to their serial numbers.

#define iCubGenova01            0
#define iCubGenova04            21
#define iCubDarmstadt01         25

// must be the value of most recent robot + 1
#define iCubNumberOf            26

#define iCubANY                 254
#define iCubNONE                255


// - list of supported boards.

#define boardMCBSTM32           0
#define boardMCBSTM32C          1
#define boardMCBSTM32F400       2

#define boardEMS001             10
#define boardEMS4RD             11
#define boardMC4PLUS            12
#define boardMC2PLUS            13

// must be the value of most recent board + 1
#define boardNumberOf           13

#define boardANY                254
#define boardNONE               255

// - converts a number coming from a robot macro defined as an integer into a serial number
#define emBODYrobot_ROBOT_NAME2SERIAL(n)         (n)

// - converts a number coming from a robot macro defined as an integer into a string
#define emBODYrobot_ROBOT_NAME2STRING(n)         ( (n<iCubNumberOf) ? (embody_robot_robotnames[n]) : ( (n==iCubANY)?(embody_robot_robotnames[iCubNumberOf]):(embody_robot_robotnames[iCubNumberOf+1]) ) ) 


// - converts a number coming from a board macro defined as an integer into a serial number
#define emBODYrobot_BOARD_NAME2SERIAL(n)         (n)

// - converts a number coming from a board macro defined as an integer into a string
#define emBODYrobot_BOARD_NAME2STRING(n)         ( (n<boardNumberOf) ? (embody_robot_boardnames[n]) : ( (n==boardANY)?(embody_robot_boardnames[boardNumberOf]):(embody_robot_boardnames[boardNumberOf+1]) ) ) 


// - list of board numbers: it starts from 1 and goes up to 253

#define boardnumberANY          254
#define boardnumberNONE         255

// - the project must define the following macros

#if !defined(emBODYrobot_ROBOT_NAME)
    #define emBODYrobot_ROBOT_NAME          iCubNONE 
#endif

#if !defined(emBODYrobot_BOARD_NAME)
    #define emBODYrobot_BOARD_NAME          boardNONE 
#endif

#if !defined(emBODYrobot_BOARD_NUMBER)
    #define emBODYrobot_BOARD_NUMBER        boardnumberNONE 
#endif

// - declaration of public user-defined types -------------------------------------------------------------------------



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const char * embody_robot_robotnames[];
extern const char * embody_robot_boardnames[];

// - declaration of extern public functions ---------------------------------------------------------------------------




// - definition of extern public macros ------------------------------------------------------------------------------


/** @}            
    end of group eo_robot  
 **/
 

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



