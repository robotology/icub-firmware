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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOTHECANMAPPING_HID_H_
#define _EOTHECANMAPPING_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOtheCANmapping_hid.h
    @brief      This header file implements hidden interface to the can mapping 
    @author     marco.accame@iit.it
    @date       12 mar 2015
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOtheCANmapping.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section

// - definition of the hidden struct implementing the object ----------------------------------------------------------


// maximum number of each entity on can: mc-joint, mc-motor, as-strain, as-mais, as-inertial, as-temperature, sk-skin
// this is the upper limit + 1 of the possible index of each entity.
enum 
{  
    eocanmap_joints_maxnumberof         = 12,   // we may have up to 12 joints on an eth board (but typically 4)
    eocanmap_motors_maxnumberof         = eocanmap_joints_maxnumberof, 
    eocanmap_strains_maxnumberof        = 1,    // we may have up to 1 strain on an eth board 
    eocanmap_maises_maxnumberof         = 1,
    eocanmap_temperatures_maxnumberof   = 1,
    eocanmap_inertials_maxnumberof      = 1,
    eocanmap_inertials3_maxnumberof     = 1,
    eocanmap_pscs_maxnumberof           = 1,
    eocanmap_poses_maxnumberof          = 1,
    eocanmap_skins_maxnumberof          = 2     // we may have up to 2 skins on an eth board
};



// maximum number of boards used to represent a single entity of the same type and same index
enum 
{  
    eocanmap_joint_boards_maxnumberof         = 1,    // we can use up to 1 can board (1foc or mc4) to represent a single joint index 
    eocanmap_motor_boards_maxnumberof         = 1,
    eocanmap_strain_boards_maxnumberof        = 1,    // we can use up to 1 can board (strain or strain2) to represent a single strain index
    eocanmap_mais_boards_maxnumberof          = 1,
    eocanmap_temperature_boards_maxnumberof   = 1,
    eocanmap_inertial_boards_maxnumberof      = 1,
    eocanmap_inertial3_boards_maxnumberof     = 1,
    eocanmap_psc_boards_maxnumberof           = 3,    // we use exactly three psc boards to have the 6 values of the psc entity
    eocanmap_pos_boards_maxnumberof           = 3,    // we use only one pmc board but we may use 3 psc boards also
    eocanmap_skin_boards_maxnumberof          = 8     // we can use up to 8 can mtb/mtb4 boards to represent a single sk-skin index. we can increase this at cost of some ram
};


enum { eocanmap_entities_maxnumberof = 10 };

extern uint8_t eocanmap_posOfEPEN(eOprotEndpoint_t ep, eOprotEntity_t en);

extern uint8_t eocanmap_maxINDEX(eOprotEndpoint_t ep, eOprotEntity_t en); // eocanmap_joints_maxnumberof etc....

extern uint8_t eocanmap_maxBOARDnumber(eOprotEndpoint_t ep, eOprotEntity_t en); // eocanmap_joint_boards_maxnumberof etc....

typedef struct
{
    uint16_t bitmapOfPresence;      // it holds in its bits the info about presence of a given endpoint-entity .... use eocanmap_posOfEPEN(ep, en) for bit position
    uint8_t  dummy;
    uint8_t  compactIndicesOf;      // it holds the index of MC and SK in compact form ... nibble 0 contains either SK or MC. if mc4can: nibble 1 contains internalindex1
                                    // marco.accame on 02apr2019: we multiplex sk with mc information because a can board cannot do both. in case there will be a board 
                                    // with more entities with more than one index ... use the dummy byte  
} eOcanmap_entities2_t; EO_VERIFYsizeof(eOcanmap_entities2_t, 4)


/**	@typedef    typedef struct eOcanmap_board_t 
 	@brief      Contains the definition of a can board with properties and the index of the entity it holds. 
 **/
typedef struct
{
    eObrd_canproperties_t       props;
    eOcanmap_entities2_t        entities2;
} eOcanmap_board_t;             EO_VERIFYsizeof(eOcanmap_board_t, 8) 


/**	@typedef    typedef struct eOcanmap_board_extended_t 
 	@brief      Contains the definition of a can board and also the information detected over can discovery. 
 **/
typedef struct
{
    eOcanmap_board_t            board;
    eObrd_info_t                detected;
} eOcanmap_board_extended_t;    EO_VERIFYsizeof(eOcanmap_board_extended_t, 14) 



/** @struct     EOtheCANmapping_hid
    @brief      Hidden definition. 
 **/  
struct EOtheCANmapping_hid 
{
	eOcanmap_cfg_t                  config;
    eOcanmap_board_extended_t***    canmapping;         // [can][adr] -> pointer to 
    EOarray**                       arrayOfBRDEXTptr[eocanmap_entities_maxnumberof];  // [posEPEN][index] -> eoarray * which holds a variable number of possible pointers to board.     
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

