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

#ifdef __cplusplus
extern "C" {
#endif


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


// -- all the possible enum


typedef enum
{
    snsr_strainmode_txcalibrateddatacontinuously          = 0,
    snsr_strainmode_acquirebutdonttx                      = 1,
    snsr_strainmode_txuncalibrateddatacontinuously        = 3,
    snsr_strainmode_txalldatacontinuously                 = 4   
} eOsnsr_strainmode_t;

typedef enum
{
    snsr_strain_formatofvalues_fullscale            = 0,
    snsr_strain_formatofvalues_calibrated           = 1,
    snsr_strain_formatofvalues_uncalibrated         = 2
} eOsnsr_strain_formatofvalues_t;

typedef enum
{
    snsr_maismode_txdatacontinuously                    = 0,
    snsr_maismode_acquirebutdonttx                      = 1,
    snsr_maismode_dontacquiredonttx                     = 5  
} eOsnsr_maismode_t;


typedef enum
{
    snsr_maisresolution_08                              = 0,
    snsr_maisresolution_16                              = 1,
//    snsr_maisresolution_debug                           = 2       //this mode set mais board in debug mode: it sends triangulas and sqaure waves.
} eOsnsr_maisresolution_t;


// -- verification of enum values vs icub can protocol

EO_VERIFYproposition(ismaismodeCompatiblewithicubcanproto, (snsr_maismode_txdatacontinuously == 0));
EO_VERIFYproposition(ismaismodeCompatiblewithicubcanproto, (snsr_maismode_acquirebutdonttx == 1));
EO_VERIFYproposition(ismaismodeCompatiblewithicubcanproto, (snsr_maismode_dontacquiredonttx == 5));


// -- all the possible data service structures

/** @typedef    typedef struct eOsnsr_upto15analog08vals_t
    @brief      eOsnsr_upto15analog08vals_t contains measurements of generic analog sensors with 8 bit resolution, as the default behaviour of mais 
 **/
typedef struct                  // size is: 1+1*15+0 = 16 
{
    uint8_t                     number;             /**< the number of values */ 
    uint8_t                     value[15];          /**< the values */
} eOsnsr_upto15analog08vals_t;  EO_VERIFYsizeof(eOsnsr_upto15analog08vals_t, 16);


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


// -- the definition of a strain entity

/** @typedef    typedef uint8_t  eOsnsr_strainId_t
    @brief      eOsnsr_strainId_t contains the values required to identify a strain sensor board in robot.
 **/
typedef uint16_t  eOsnsr_strainId_t;


typedef struct                      
{
    eOenum08_t                      mode;                                   /**< use values from eOsnsr_strainmode_t */
    uint8_t                         datarate;                               /**< in ms from 1 upto 210 */
    eObool_t                        signaloncefullscale;                    /**< if eobool_true, then the strains signals its fullscale*/
    uint8_t                         filler01[1];                           
} eOsnsr_strain_config_t;           EO_VERIFYsizeof(eOsnsr_strain_config_t, 4);


typedef struct                      
{
    uint8_t                         filler04[4];                               
} eOsnsr_strain_inputs_t;           EO_VERIFYsizeof(eOsnsr_strain_inputs_t, 4);


typedef struct                      // size is: 16+16+16+0 = 48                     
{
    eOsnsr_arrayofupto12bytes_t     fullscale;                              /**< the full scale as an array of three forces and three torques each of 2 bytes */
    eOsnsr_arrayofupto12bytes_t     calibratedvalues;                       /**< the calibrated values as an array of three forces and three torques each of 2 bytes */
    eOsnsr_arrayofupto12bytes_t     uncalibratedvalues;                     /**< the uncalibrated values as an array of three forces and three torques each of 2 bytes */                  
} eOsnsr_strain_status_t;           EO_VERIFYsizeof(eOsnsr_strain_status_t, 48);


typedef struct                      // size is: 4+4+48+0 = 56
{
    eOsnsr_strain_config_t          sconfig;
    eOsnsr_strain_inputs_t          sinputs;    
    eOsnsr_strain_status_t          sstatus;
} eOsnsr_strain_t;                  EO_VERIFYsizeof(eOsnsr_strain_t, 56);



// -- the definition of a mais entity


/** @typedef    typedef uint8_t  eOsnsr_maisId_t
    @brief      eOsnsr_maisId_t contains the values required to identify a mais sensor board in robot.
 **/
typedef uint16_t  eOsnsr_maisId_t;

typedef struct                      
{
    eOenum08_t                      mode;                               /**< use values from eOsnsr_maismode_t */
    uint8_t                         datarate;                           /**< in ms from 1 upto 210 */
    eOenum08_t                      resolution;                         /**< use values from eOsnsr_maisresolution_t */               
    uint8_t                         filler01[1];                           
} eOsnsr_mais_config_t;             EO_VERIFYsizeof(eOsnsr_mais_config_t, 4);


typedef struct                      
{
    uint8_t                         filler04[4];                               
} eOsnsr_mais_inputs_t;             EO_VERIFYsizeof(eOsnsr_mais_inputs_t, 4);


/* 
   NOTE: mais board sends 2 can frame: the first contains values of channels between 0 to 6 (7 values) and the second contains values of channel between 7 to 14 (8 vals); 
         they are saved the15values.data[]. 
*/

typedef struct                      // size is: 40+0 = 40                     
{
    eOsnsr_arrayofupto36bytes_t     the15values;                        /**< the 15 values of the mais, either at 1 byte or 2 bytes resolution. */                  
} eOsnsr_mais_status_t;             EO_VERIFYsizeof(eOsnsr_mais_status_t, 40);

typedef struct                      // size is: 4+4+40+0 = 48
{
    eOsnsr_mais_config_t            mconfig;
    eOsnsr_mais_inputs_t            minputs;    
    eOsnsr_mais_status_t            mstatus;
} eOsnsr_mais_t;                    EO_VERIFYsizeof(eOsnsr_mais_t, 48);





// - others unused possible entities

/** @typedef    typedef struct eOsnsr_aea_t
    @brief      eOsnsr_aea_t contains measurements of the absolute encoder whcih so far is managed only by teh ems 
    @warning    This struct must be of fixed and size and multiple of 4.
 **/
typedef struct                  // size is: 4+0 = 4
{
    uint32_t                    value;            // used with ... see hal_encoder.h for bitfield formatting  
} eOsnsr_aea_t;                 EO_VERIFYsizeof(eOsnsr_aea_t, 4);


/** @typedef    typedef struct eOsnsr_forcetorque_t
    @brief      eOsnsr_forcetorque_t contains measurements of force on xyz and of torque on 3 components 
 **/
typedef struct                  // size is: 4+2*3+2*3+0 = 16 
{
    uint32_t                    identifier;         /**< the identifier of the origin of measure: so far only strain */ 
    eOmeas_force_t              force[3];           /**< the force split in x, y, and z components */
    eOmeas_torque_t             torque[3];          /**< the torque split in three components*/
} eOsnsr_forcetorque_t;         EO_VERIFYsizeof(eOsnsr_forcetorque_t, 16);



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section



/** @}            
    end of group eo_cevcwervcrev5555  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

