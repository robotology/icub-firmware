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
#ifndef _EOTHECANMAPPING_H_
#define _EOTHECANMAPPING_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtheCANmapping.h
	@brief      This header file implements public interface to the singleton which tells how the can boards are mapped onto the EMS boards
 	@author     marco.accame@iit.it
	@date       12 mar 2015
 **/

/** @defgroup eo_thecanmapocol Singleton EOtheCANmapping 
    
    It manages can protocol in icub.  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
    
#include "iCubCanProtocol.h"    

#include "EoProtocol.h"

#include "EoBoards.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOtheCANmapping_hid EOtheCANmapping
    @brief      EOtheCANmapping is an opaque struct.
 **/  
typedef struct EOtheCANmapping_hid EOtheCANmapping;

typedef enum
{
    eocanmap_insideindex_first      = 0,
    eocanmap_insideindex_second     = 1,
    eocanmap_insideindex_none       = 2
} eOcanmap_insideindex_t;

/** @typedef    typedef struct eOcanmap_entitylocation_t
    @brief      tells where an entity (joint or motor or strain etc.) is mapped into a can board
 **/ 
typedef struct
{
    uint8_t port : 1;               /**< use eOcanport_t */
    uint8_t addr : 4;               /**< use icubCanProto_canBoardAddress_t */ 
    uint8_t insideindex : 2;        /**< use eOcanmap_insideindex_t */
    uint8_t unused : 1;
} eOcanmap_entitylocation_t;

typedef struct
{
    uint8_t port : 1;               /**< use eOcanport_t */
    uint8_t addr : 4;               /**< use icubCanProto_canBoardAddress_t */ 
    uint8_t unused : 3;
} eOcanmap_boardlocation_t;

// used only as internal representation for eOcanmap_canboard_t which has only four bits available for the index. 
typedef enum 
{
    entindex00 =  0, entindex01 =  1, entindex02 =  2, entindex03 =  3, entindex04 =  4, 
    entindex05 =  5, entindex06 =  6, entindex07 =  7, entindex08 =  8, entindex09 =  9, 
    entindex10 = 10, entindex11 = 11, entindex12 = 12, entindex13 = 13, entindex14 = 14, 
    entindexNONE = 255    
} eOcanmap_entityindex_t;

//typedef struct
//{
//    eObrd_typeandversions_t     board;              /**< information about the can board */
//    eOcanmap_boardlocation_t    location;           /**< its can location */
//    uint8_t                     indexentity0: 4;    /**< the eoprot index of the entity0 it hosts. use eOcanmap_entityindex_t. used for joints, motors, strains, maises, skins */
//    uint8_t                     indexentity1: 4;    /**< the eoprot index of the entity1 it hosts. use eOcanmap_entityindex_t. used only for joints and motors in board eobrd_cantype_mc4 */
//} eOcanmap_canboard_t;          EO_VERIFYsizeof(eOcanmap_canboard_t, 8);       

typedef struct
{
    eOenum08_t                  type;               /**< use eObrd_cantype_t */
    eOcanmap_boardlocation_t    location;           /**< its can location */
    eObrd_version_t             requiredprotocol;   /**< so far, the only requirement is that the protocol version is ... */
    uint8_t                     indexofentity[2];   /**< at most two entities per board. use eOcanmap_insideindex_t for addressing array and use eOcanmap_entityindex_t as its value. */
} eOcanmap_board_t;             EO_VERIFYsizeof(eOcanmap_board_t, 6); 

typedef struct
{
    eOcanmap_board_t            board;
    eObrd_typeandversions_t     detected;
} eOcanmap_canboard_t;          EO_VERIFYsizeof(eOcanmap_canboard_t, 12); 


/**	@typedef    typedef struct eOcanmap_cfg_t 
 	@brief      Contains the configuration for the EOtheCANmapping. 
 **/
typedef struct
{
    uint32_t            nothingsofar;
} eOcanmap_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOtheCANmapping * eo_canmap_Initialise(const eOcanmap_cfg_t *canmapcfg)
    @brief      Initialise the EOtheCANmapping singleton 
    @arg        canmapcfg       The configuration. NULL is OK.
    @return     The handle to the EOtheCANmapping
 **/
extern EOtheCANmapping * eo_canmap_Initialise(const eOcanmap_cfg_t *canmapcfg);


/** @fn         extern EOtheCANmapping * eo_canmap_GetHandle(void)
    @brief      Retrieve the EOtheCANmapping singleton 
    @return     The handle to the EOtheCANmapping
 **/
extern EOtheCANmapping * eo_canmap_GetHandle(void);


// get a board given its can location, fro read only purposes
extern const eOcanmap_canboard_t * eo_canmap_GetBoard(EOtheCANmapping *p, eOcanmap_entitylocation_t loc);


// change the fw version etc on a board. we read it from can and then we set it with this function
extern eOresult_t eo_canmap_BoardSetDetected(EOtheCANmapping *p, eOcanmap_entitylocation_t loc, eObrd_typeandversions_t *detected);


// i get the index of the entity on the board on a given location without verifying type of board
extern eOprotIndex_t eo_canmap_GetEntityIndex(EOtheCANmapping *p, eOcanmap_entitylocation_t loc);


// i get the index of the entity on teh board on a given location, but i also verify that the wanted ep-entity is coherent with the found board type
// it does the same job as eo_canmap_GetEntityIndexSimple(0 but with some more checks and computation.
extern eOprotIndex_t eo_canmap_GetEntityIndexExtraCheck(EOtheCANmapping *p, eOcanmap_entitylocation_t loc, eOprotEndpoint_t ep, eOprotEntity_t entity);



// the id32 contains ep, entity, idex and tag = eoprot_tag_none
// it gets the location of a specified entity
extern eOresult_t eo_canmap_GetEntityLocation(EOtheCANmapping *p, eOprotID32_t id32, eOcanmap_entitylocation_t *loc, uint8_t *numoflocs, eObrd_cantype_t *boardtype);

extern eOresult_t eo_canmap_GetEntityLocation2(EOtheCANmapping *p, eOprotEndpoint_t ep, eOprotEntity_t entity, eOprotIndex_t index, eOcanmap_entitylocation_t *loc, uint8_t *numoflocs, eObrd_cantype_t *board);



/** @}            
    end of group eo_thecanmapocol  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

