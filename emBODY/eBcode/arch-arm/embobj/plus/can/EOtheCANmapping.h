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
	@brief      This header file implements public interface to the singleton which tells how the can boards are mapped onto the ETH board
 	@author     marco.accame@iit.it
	@date       12 mar 2015
 **/

/** @defgroup eo_thecanmapocol Singleton EOtheCANmapping 
    
    It manages can protocol in icub.  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EoBoards.h"
#include "EOconstvector.h"
#include "EOconstarray.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOtheCANmapping_hid EOtheCANmapping
    @brief      EOtheCANmapping is an opaque struct.
 **/  
typedef struct EOtheCANmapping_hid EOtheCANmapping;

// use these values for eOcanmap_entitydescriptor_t::index (and for other internal use)
typedef enum 
{
    entindex00 =  0, entindex01 =  1, entindex02 =  2, entindex03 =  3, entindex04 =  4, 
    entindex05 =  5, entindex06 =  6, entindex07 =  7, entindex08 =  8, entindex09 =  9, 
    entindex10 = 10, entindex11 = 11, entindex12 = 12, entindex13 = 13, entindex14 = 14, 
    entindexNONE = 15    
} eOcanmap_entityindex_t; 


/**	@typedef    typedef struct eOcanmap_entitydescriptor_t 
 	@brief      Contains the description of an entity with a given index mapped onto a can board location
                Example of if a 1foc offers motion control to joint 3 we have: 
                .location = { .port = eOcanport1, .addr = 2, .insideindex = eobrd_caninsideindex_first }
                .index = entindex03 
 **/
typedef struct
{
    eObrd_canlocation_t             location;
    eOcanmap_entityindex_t          index;
} eOcanmap_entitydescriptor_t;


/**	@typedef    typedef struct eOcanmap_cfg_t 
 	@brief      Contains the configuration for the EOtheCANmapping. 
 **/
typedef struct
{
    uint32_t            nothingsofar;
} eOcanmap_cfg_t;


enum { eocanmap_maxlocations = 32 };

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         EOtheCANmapping * eo_canmap_Initialise(const eOcanmap_cfg_t *canmapcfg)
    @brief      Initialise the EOtheCANmapping singleton 
    @arg        canmapcfg       The configuration. NULL is OK.
    @return     The handle to the EOtheCANmapping
 **/
extern EOtheCANmapping * eo_canmap_Initialise(const eOcanmap_cfg_t *canmapcfg);


/** @fn         EOtheCANmapping * eo_canmap_GetHandle(void)
    @brief      Retrieve the EOtheCANmapping singleton 
    @return     The handle to the EOtheCANmapping
 **/
extern EOtheCANmapping * eo_canmap_GetHandle(void);


/** @fn         eOresult_t eo_canmap_LoadBoards(EOtheCANmapping *p,  EOconstvector *vectorof_boardprops)
    @brief      Loads some boards onto the object. The function can be called only once with a vector containing all the boards,
                or it can be called multiple times to load some board at a time. if the same board is loaded multiple times, the
                last loaded wins. 
    @param      p                       The handle to the EOtheCANmapping                
    @param      vectorof_boardprops     A const vector of eObrd_canproperties_t, one item per board                 
    @return     eores_OK if successfull.
 **/
extern eOresult_t eo_canmap_LoadBoards(EOtheCANmapping *p,  EOconstvector *vectorof_boardprops);


extern eOresult_t eo_canmap_UnloadBoards(EOtheCANmapping *p,  EOconstvector *vectorof_boardprops);



/** @fn         eOresult_t eo_canmap_ConfigEntity(EOtheCANmapping *p,  eOprotEndpoint_t ep, eOprotEntity_t entity, EOconstvector *vectorof_entitydescriptors)
    @brief      After the boards are loaded, it configures an entity so that it is mapped into the boards. The function can be called only once with a vector containing 
                all the descriptors for the entity or it can be called multiple times to load some descriptors at a time. 
                As an example, suppose you want to load the 4 joints with indices 0, 1, 2, and 3 into 2foc boards on can1 with addresses 5, 6, 8, 10. 
                all at internal index 0. you can call this function with a vector of {.index = 0, .loc.can = 0, .loc.adr = 5, .loc.internalindx = 0} etc.                                 
    @param      p           The handle to the EOtheCANmapping
    @param      ep          the endpoint of the entity .... no management 
    @param      entity      the entity   
    @param      vectorof_entitydescriptors      the vector of eOcanmap_entitydescriptor_t items
    @return     eores_OK if successfull.
 **/
extern eOresult_t eo_canmap_ConfigEntity(EOtheCANmapping *p,  eOprotEndpoint_t ep, eOprotEntity_t entity, EOconstvector *vectorof_entitydescriptors);

extern eOresult_t eo_canmap_DeconfigEntity(EOtheCANmapping *p,  eOprotEndpoint_t ep, eOprotEntity_t entity, EOconstvector *vectorof_entitydescriptors);



/** @fn         eOresult_t eo_canmap_BoardSetDetected(EOtheCANmapping *p, eObrd_canlocation_t loc, eObrd_info_t *detected)
    @brief      change the fw version etc on a board. we read it from can and then we set it with this function       
    @param      p           The handle to the EOtheCANmapping
    @param      loc         teh can location
    @param      detected    the detected type and versions which we want to set inside the object
    @return     eores_OK if successfull.
**/
extern eOresult_t eo_canmap_BoardSetDetected(EOtheCANmapping *p, eObrd_canlocation_t loc, eObrd_info_t *detected);


/** @fn         eObrd_cantype_t eo_canmap_GetBoardType(EOtheCANmapping *p, eObrd_canlocation_t loc)
    @brief      i get type of board on a given location.
    @param      p           The handle to the EOtheCANmapping
    @param      loc         the can location
    @return     the index at the specified location or EOK_uint08dummy if no board is found or of the board type is not coherent with the entity type.
**/
extern eObrd_cantype_t eo_canmap_GetBoardType(EOtheCANmapping *p, eObrd_canlocation_t loc);


/** @fn         eOprotIndex_t eo_canmap_GetEntityIndex(EOtheCANmapping *p, eObrd_canlocation_t loc, eOprotEndpoint_t ep, eOprotEntity_t entity)
    @brief      i get the index of the entity on the board on a given location and also verify if the board type is coherent with the specified entity.
    @param      p           The handle to the EOtheCANmapping
    @param      loc         the can location
    @param      ep          the endpoint of the entity 
    @param      entity      the entity      
    @return     the index at the specified location or EOK_uint08dummy if no board is found or of the board type is not coherent with the entity type.
**/
extern eOprotIndex_t eo_canmap_GetEntityIndex(EOtheCANmapping *p, eObrd_canlocation_t loc, eOprotEndpoint_t ep, eOprotEntity_t entity);



/** @fn         eOresult_t eo_canmap_GetEntityLocation(EOtheCANmapping *p, eOprotID32_t id32, eObrd_canlocation_t *loc, uint8_t *numoflocs, eObrd_cantype_t *boardtype)
    @brief      it gets the location of an entity with a given index. but also tells how many can boards are dedicated to this entity (eg.g, skin uses several
                can boards), and the type of board.    
    @param      p           The handle to the EOtheCANmapping
    @param      id32        it specifies the entity. use for instance: eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, 2, eoprot_tag_none); 
    @param      loc         pointer to the wanted can location
    @param      numoflocs   if not NULL it will hold the number of can boards used by that entity (all use one except for skin whcih may use 7 or 8 or ...)
    @param      boardtype   if not NULL it will hold the type of can board used by that entity.
    @return     eores_OK if successfull.
**/
extern eOresult_t eo_canmap_GetEntityLocation(EOtheCANmapping *p, eOprotID32_t id32, eObrd_canlocation_t *loc, uint8_t *numoflocs, eObrd_cantype_t *boardtype);

// the array is of eObrd_canlocation_t items
extern eOresult_t eo_canmap_GetEntityArrayOfLocations(EOtheCANmapping *p, eOprotID32_t id32, EOarray *arrayoflocs, eObrd_cantype_t *boardtype);


extern eObool_t eocanmap_BRDisCompatible(eObrd_cantype_t brd, eOprotEndpoint_t ep, eOprotEntity_t en);


// it returns a eOcanmap_arrayof_locations_t*
/** @fn         EOconstarray* eo_canmap_GetBoardLocations(EOtheCANmapping *p)
    @brief      it gets the EOconstarray with elements of type eObrd_canlocation_t of all the can boards loaded in the object.
    @param      p           The handle to the EOtheCANmapping
    @return     the array, or NULL if p is NULL.
**/
//extern EOconstarray* eo_canmap_GetBoardLocations(EOtheCANmapping *p);


/** @}            
    end of group eo_thecanmapocol  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

