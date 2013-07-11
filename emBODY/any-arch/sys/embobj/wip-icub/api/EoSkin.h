/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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
#ifndef _EOSKIN_H_
#define _EOSKIN_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoSkin.h
	@brief      This header file gives 
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_cevcwervcrev2345 Configuation of the vrfverver
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoMeasures.h"
#include "EoUtilities.h"
#include "EOarray.h"

// - public #define  --------------------------------------------------------------------------------------------------





// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum eOsk_entity_t;
    @brief      It contains all the possible entities in skin.
                See definition of eOsk_skin_t for explanations
 **/
typedef enum
{
    eosk_entity_skin                    = 0
} eOsk_entity_t; 

enum { eosk_entities_numberof = 1 };


// -- all the possible enum

typedef enum
{
    eosk_sigmode_dontsignal                         = 0,
    eosk_sigmode_signal                             = 1  
} eOsk_sigmode_t;


// -- all the possible data service structures

typedef struct              // size is 4+10*16+0 = 164
{
    eOarray_head_t          head;
    uint8_t                 data[10*sizeof(eOutil_canframe_t)];
} EOarray_of_10canframes;   EO_VERIFYsizeof(EOarray_of_10canframes, 164);


/** @typedef    typedef struct eOsk_array_data_t
    @brief      eOsk_array_data_t contains up to 10 can frames as received by hal placed in an EOarray  object
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 164+4+0 = 168 
{
    EOarray_of_10canframes  array;
    uint8_t                 filler04[4];
} eOsk_array_data_t;        EO_VERIFYsizeof(eOsk_array_data_t, 168);


// -- the definition of a skin entity 

/** @typedef    uint8_t  eOsk_skinId_t
    @brief      eOsk_skinId_t contains the values required to identify a skin;
		        currently a skin id identifies a set of 7 skin board attached on a single body part.
 **/
typedef uint8_t  eOsk_skinId_t;

typedef struct                      
{
    eOenum08_t                  sigmode;                                   /**< use values from eOsk_sigmode_t */
    uint8_t                     filler03[3];                           
} eOsk_config_t;                EO_VERIFYsizeof(eOsk_config_t, 4);


typedef struct                      
{
    uint8_t                     filler04[4];                           
} eOsk_inputs_t;                EO_VERIFYsizeof(eOsk_inputs_t, 4);


typedef struct                  // size is: 164+4+0 = 168                     
{
    EOarray_of_10canframes      arrayof10canframes;
    uint8_t                     filler04[4];                           
} eOsk_status_t;                EO_VERIFYsizeof(eOsk_status_t, 168);


typedef struct                  // size is: 4+4+168+0 = 176
{
    eOsk_config_t               config;
    eOsk_inputs_t               inputs;    
    eOsk_status_t               status;
} eOsk_skin_t;                  EO_VERIFYsizeof(eOsk_skin_t, 176);



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section



/** @}            
    end of group eo_cevcwervcrev2345  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

