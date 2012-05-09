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


typedef struct              // size is 4+10*16+0 = 164
{
    eOarray_head_t          head;
    uint8_t                 data[10*sizeof(eOutil_canframe_t)];
} EOarray_of_10canframes;   EO_VERIFYsizeof(EOarray_of_10canframes, 164);


/** @typedef    typedef struct eOskin_array_data_t
    @brief      eOskin_array_data_t contains up to 10 can frames as received by hal placed in an EOarray  object
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 164+4+0 = 168 
{
    EOarray_of_10canframes  array;
    uint8_t                 filler04[4];
} eOskin_array_data_t;      EO_VERIFYsizeof(eOskin_array_data_t, 168);


typedef enum
{
    skin_sigmode_dontsignal                         = 0,
    snsr_sigmode_signal                             = 1  
} eOskin_sigmode_t;

typedef struct                      
{
    eOenum08_t                  sigmode;                                   /**< use values from eOskin_sigmode_t */
    uint8_t                     filler03[3];                           
} eOskin_config_t;              EO_VERIFYsizeof(eOskin_config_t, 4);


typedef struct                      
{
    uint8_t                     filler04[4];                           
} eOskin_inputs_t;              EO_VERIFYsizeof(eOskin_inputs_t, 4);


typedef struct                  // size is: 164+4+0 = 168                     
{
    EOarray_of_10canframes      arrayof10canframes;
    uint8_t                     filler04[4];                           
} eOskin_status_t;              EO_VERIFYsizeof(eOskin_status_t, 168);


typedef struct                      // size is: 4+4+168+0 = 176
{
    eOskin_config_t                 sconfig;
    eOskin_inputs_t                 sinputs;    
    eOskin_status_t                 sstatus;
} eOskin_someskin_t;                EO_VERIFYsizeof(eOskin_someskin_t, 176);

    
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

