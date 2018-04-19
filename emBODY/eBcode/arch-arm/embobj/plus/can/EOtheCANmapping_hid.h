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
    eocanmap_skins_maxnumberof          = 2     // we may have up to 2 skins on an eth board
};



// maximum number of boards used to represent a single entity of the same type and same index
enum 
{  
    eocanmap_joint_index_boards_maxnumberof         = 1,    // we can use up to 1 can board (1foc or mc4) to represent a single joint index 
    eocanmap_motor_index_boards_maxnumberof         = 1,
    eocanmap_strain_index_boards_maxnumberof        = 1,    // we can use up to 1 can board (strain or strain2) to represent a single strain index
    eocanmap_mais_index_boards_maxnumberof          = 1,
    eocanmap_temperature_index_boards_maxnumberof   = 1,
    eocanmap_inertial_index_boards_maxnumberof      = 1,
    eocanmap_inertial3_index_boards_maxnumberof     = 1,
    eocanmap_skin_index_boards_maxnumberof          = 8     // we can use up to 8 can mtb boards to represent a single sk-skin index
};



enum { eocanmap_entities_maxnumberof = 8 };

extern uint8_t eocanmap_posOfEPEN(uint8_t ep, uint8_t en);

typedef struct
{
    uint8_t  bitmapOfPresence;      // it holds info about presence of a given endpoint-entity .... use eocanmap_posOfEPEN(ep, en) for bit position
    //uint8_t  indicesOfMC4;          // index for joint-motor for the case of mc4can. in nibble 0 we have the index if internalindex is 0. in nibble 1 the case of internalindex 1.
    //uint16_t dummy;
    //uint32_t indicesOf;             // it holds the index of an entity stored in a nibble .... use eocanmap_posOfEPEN(ep, en) for nibble position. for mc the case of mc4can is managed differently 
    uint8_t  compactIndicesOf;  // it holds the index of MC and SK in compact form ... nibble 0 contains either SK or MC. if mc4can: nibble 1 contains internalindex1
} eOcanmap_entities2_t;     EO_VERIFYsizeof(eOcanmap_entities2_t, 2)


/**	@typedef    typedef struct eOcanmap_board_t 
 	@brief      Contains the definition of a can board with properties and the index of the entity it holds. 
 **/
typedef struct
{
    eObrd_canproperties_t       props;
    eOcanmap_entities2_t        entities2;
} eOcanmap_board_t;             EO_VERIFYsizeof(eOcanmap_board_t, 6) 


/**	@typedef    typedef struct eOcanmap_board_extended_t 
 	@brief      Contains the definition of a can board and also the information detected over can discovery. 
 **/
typedef struct
{
    eOcanmap_board_t            board;
    eObrd_info_t                detected;
} eOcanmap_board_extended_t;    EO_VERIFYsizeof(eOcanmap_board_extended_t, 12) 




/** @struct     EOtheCANmapping_hid
    @brief      Hidden definition. 
 **/  
struct EOtheCANmapping_hid 
{
	eOcanmap_cfg_t                  config;
    eOcanmap_board_extended_t***    canmapping;         // [can][adr] -> pointer to 
    eOcanmap_board_extended_t****   entitylocation;     // [ep][ent][index]-> pointer to
    eOcanmap_board_extended_t***    skinlocation;       // [index]-> array[] of up to 8 pointers to 
    uint8_t                         numofskinboardsindex[eocanmap_skins_maxnumberof];
//    eOcanmap_arrayof_locations_t    arrayofboardlocations;
};


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

