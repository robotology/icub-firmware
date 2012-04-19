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
#ifndef _EOSENSORS_H_
#define _EOSENSORS_H_




/** @file       EoSensors.h
	@brief      This header file gives 
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_cevcwervcrev5555 Configuation of the vrfverver
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOarray.h"
#include "EoUtilities.h"
#include "EoMeasures.h"


// - public #define  --------------------------------------------------------------------------------------------------





// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct eOsnsr_forcetorque_t
    @brief      eOsnsr_forcetorque_t contains measurements of force on xyz and of torque on 3 components 
 **/
typedef struct                  // size is: 4+2*3+2*3+0 = 16 
{
    uint32_t                    identifier;         /**< the identifier of the origin of measure: so far only strain */ 
    eOmeas_force_t              force[3];           /**< the force split in x, y, and z components */
    eOmeas_torque_t             torque[3];          /**< the torque split in three components*/
} eOsnsr_forcetorque_t;         EO_VERIFYsizeof(eOsnsr_forcetorque_t, 16);


/** @typedef    typedef struct eOsnsr_upto15analog08vals_t
    @brief      eOsnsr_upto15analog08vals_t contains measurements of generic analog sensors with 8 bit resolution, as the default behaviour of mais 
 **/
typedef struct                  // size is: 1+1*15+0 = 16 
{
    uint8_t                     number;             /**< the number of values */ 
    uint8_t                     value[15];          /**< the values */
} eOsnsr_upto15analog08vals_t;  EO_VERIFYsizeof(eOsnsr_upto15analog08vals_t, 16);
// note: a more generic type would be an EOarray. however, 4 bytes are required for teh head, thus only 12 bytes are available to fit in  

/** @typedef    typedef struct eOsnsr_arrayofupto12bytes_t
    @brief      eOsnsr_arrayofupto12bytes_t contains measurements of generic analog sensors with 8/16/32 bit resolution and is able to contain the 12 bytes of the strain
 **/
typedef struct                  // size is: 4+12 = 16 
{
    eOarray_head_t              head;               /**< the head */ 
    uint8_t                     data[12];           /**< the data: upto 12 U8, or 6 U16, or 3 U32 */
} eOsnsr_arrayofupto12bytes_t;  EO_VERIFYsizeof(eOsnsr_arrayofupto12bytes_t, 16);


/** @typedef    typedef struct eOsnsr_arrayofupto20bytes_t
    @brief      eOsnsr_arrayofupto20bytes_t contains measurements of generic analog sensors with 8/16/32 bit resolution and is able to contain the 15 bytes of the mais
 **/
typedef struct                  // size is: 4+20 = 24 
{
    eOarray_head_t              head;               /**< the head */ 
    uint8_t                     data[20];           /**< the data: upto 20 U8, or 10 U16, or 5 U32 */
} eOsnsr_arrayofupto20bytes_t;  EO_VERIFYsizeof(eOsnsr_arrayofupto20bytes_t, 24);


/** @typedef    typedef struct eOsnsr_arrayofupto36bytes_t
    @brief      eOsnsr_arrayofupto20bytes_t contains measurements of generic analog sensors with 8/16/32 bit resolution and is able to contain the 15 halfwords of mais
 **/
typedef struct                  // size is: 4+36+0 = 40 
{
    eOarray_head_t              head;               /**< the head */ 
    uint8_t                     data[36];           /**< the data: upto 20 U8, or 10 U16, or 5 U32 */
} eOsnsr_arrayofupto36bytes_t;  EO_VERIFYsizeof(eOsnsr_arrayofupto36bytes_t, 40);


/** @typedef    typedef struct eOsnsr_aea_t
    @brief      eOsnsr_aea_t contains measurements of the absolute encoder whcih so far is managed only by teh ems 
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 4+0 = 4
{
    uint32_t                    value;            // used with ... see hal_encoder.h for bitfield formatting  
} eOsnsr_aea_t;                 EO_VERIFYsizeof(eOsnsr_aea_t, 4);



typedef struct                      // size is: 1+1+1+1+4+16+0 = 24
{
    eOenum08_t                      txmode;
    uint8_t                         datarate;
    eObool_t                        signaloncefullscale;
    uint8_t                         filler01[1];
    uint8_t                         filler04[4];    
    eOsnsr_arrayofupto12bytes_t     values;
} eOsnsr_strain_t;                  EO_VERIFYsizeof(eOsnsr_strain_t, 24);


typedef struct                      // size is: 1+1+1+1+4+24+0 = 32
{
    eOenum08_t                      txmode;
    uint8_t                         datarate;
    eOenum08_t                      resolutionmode;
    uint8_t                         filler01[1];
    uint8_t                         filler04[4];    
    eOsnsr_arrayofupto20bytes_t     values; 
} eOsnsr_mais_t;                    EO_VERIFYsizeof(eOsnsr_mais_t, 32);



/** @typedef    typedef uint8_t  eOsnsr_sensorId_t
    @brief      eOsnsr_sensorId_t contains the values required to identify a sensor board in robot.
 **/
typedef uint8_t  eOsnsr_sensorId_t;
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section



/** @}            
    end of group eo_cevcwervcrev5555  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

