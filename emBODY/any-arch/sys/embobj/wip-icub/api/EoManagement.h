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
#ifndef _EOMANAGEMENT_H_
#define _EOMANAGEMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EoManagement.h
	@brief      This header file gives 
	@author     marco.accame@iit.it
	@date       05/15/2012
**/

/** @defgroup eo_management Management of the ems board
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOarray.h"
#include "EOrop.h"



// - public #define  --------------------------------------------------------------------------------------------------


#define EOMANAGEMENT_USE21ROPSIGCFG

#if     defined(EOMANAGEMENT_USE21ROPSIGCFG) 
#define NUMOFROPSIGCFG 21
#else
#error --> define a size for NUMOFROPSIGCFG
#endif


// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef enum eOmn_entity_t;
    @brief      It contains all the possible entities in management.
                See definition of eOmn_comm_t, and eOmn_appl_t for explanations
 **/
typedef enum
{
    eomn_entity_comm                        = 0,
    eomn_entity_appl                        = 1,
} eOmn_entity_t; 

enum { eomn_entities_numberof = 2 };

// -- all the possible enum


/** @typedef    typedef enum eOmn_ropsigcfg_commandtype_t;
    @brief      It contains all the operations which are possible to configure the regular EOrop objects in the transceiver.   
 **/
typedef enum 
{
    ropsigcfg_cmd_none      = 0,                /**< it does nothing */
    ropsigcfg_cmd_clear     = 1,                /**< it forces a clear of all the regular rops in the transceiver */
    ropsigcfg_cmd_assign    = 2,                /**< it copies in the reg rops all of the content of the delivered array */
    ropsigcfg_cmd_append    = 3,                /**< it appends to the reg rops all of the content of the delivered array (if size is 1 then it is a pushback)*/
    ropsigcfg_cmd_remove    = 5                 /**< it searches and removes from the reg rops all the items of the delivered array (its size must be > 0)*/
} eOmn_ropsigcfg_commandtype_t;


/** @typedef    typedef enum eOmn_appl_state_t;
    @brief      It contains the states of the state machine of the application.     
 **/
typedef enum 
{
    applstate_config        = 0,                /**< config */
    applstate_running       = 1,                /**< running */
    applstate_error         = 2                 /**< error */
} eOmn_appl_state_t;


// -- all the possible data service structures

/** @typedef    typedef struct eOmn_ropsigcfg_array_t;
    @brief      is an array of eOropSIGcfg_t structs used to configure the communication
 **/
typedef struct              // size is 4+21*4=88 bytes
{
    eOarray_head_t          head;
    uint8_t                 data[NUMOFROPSIGCFG*sizeof(eOropSIGcfg_t)];
} eOmn_ropsigcfg_array_t;   EO_VERIFYsizeof(eOmn_ropsigcfg_array_t, 88);


/** @typedef    typedef struct eOmn_ropsigcfg_command_t;
    @brief      is the command used to configure the communication
 **/
typedef struct              // size is 88+ 1+1+1+1+4 = 96 bytes
{
    eOmn_ropsigcfg_array_t          array;
    eOenum08_t                      cmmnd;              /**< but use eOmn_ropsigcfg_commandtype_t */
    uint8_t                         plustime;
    uint8_t                         plussign;
    uint8_t                         filler01;
    uint32_t                        signature;
} eOmn_ropsigcfg_command_t;         EO_VERIFYsizeof(eOmn_ropsigcfg_command_t, 96);


/** @typedef    typedef enum eOmn_appl_runMode_t;
    @brief      contains ems application's run mode.
                Currently runMode is not set to ems by pc104, but application itsself understands its runmode
                depending on connected CAN boards.
 **/
typedef enum
{
    applrunMode__default       = 0,
    applrunMode__skinOnly      = 1,
    applrunMode__mc4Only       = 2,
    applrunMode__skinAndMc4    = 3,
    applrunMode__2foc          = 4
} eOmn_appl_runMode_t;


// -- the definition of a comm entity

typedef struct
{
    uint8_t                         filler04[4];
} eOmn_comm_config_t;               EO_VERIFYsizeof(eOmn_comm_config_t, 4);

typedef struct
{
    uint8_t                         filler04[4];
} eOmn_comm_status_t;               EO_VERIFYsizeof(eOmn_comm_status_t, 4);

typedef struct
{
    eOmn_ropsigcfg_command_t        ropsigcfg;
} eOmn_comm_cmmnds_t;               EO_VERIFYsizeof(eOmn_comm_cmmnds_t, 96);


/** @typedef    typedef struct eOmn_comm_t;
    @brief      used to represent the communication with config, status, commands. so far config and status are not used
 **/
typedef struct                      // size is 4+4+96+0 = 104 bytes
{
    eOmn_comm_config_t              config;
    eOmn_comm_status_t              status;
    eOmn_comm_cmmnds_t              cmmnds;
} eOmn_comm_t;                      EO_VERIFYsizeof(eOmn_comm_t, 104);



// -- the definition of a appl entity

/** @typedef    typedef struct eOmn_appl_config_t;
    @brief      used to configure the application
 **/
typedef struct                      // size is 4+4 = 8 bytes
{
    eOreltime_t                     cycletime;
    uint8_t                         filler04[4];
} eOmn_appl_config_t;               EO_VERIFYsizeof(eOmn_appl_config_t, 8);


/** @typedef    typedef struct eOmn_appl_status_t;
    @brief      used to report status of the application
 **/
typedef struct                      // size is 1+7 = 8 bytes
{
    eOenum08_t                      currstate;          /**< use eOmn_appl_state_t  */
    eOenum08_t                      runmode;            /**< use eOmn_appl_runMode_t */
    uint8_t                         filler06[6];
} eOmn_appl_status_t;               EO_VERIFYsizeof(eOmn_appl_status_t, 8);

                                         
/** @typedef    typedef struct eOmn_appl_cmmnds_t;
    @brief      used to send commands to the application
 **/
typedef struct                      // size is 1+7 = 8 bytes
{
    eOenum08_t                      go2state;       /**< use eOmn_appl_state_t */
    uint8_t                         filler07[7];
} eOmn_appl_cmmnds_t;               EO_VERIFYsizeof(eOmn_appl_cmmnds_t, 8);


/** @typedef    typedef struct eOmn_appl_t;
    @brief      used to represent the application with config, status, commands
 **/
typedef struct                      // size is 8+8+8 = 24 bytes
{
    eOmn_appl_config_t              config;
    eOmn_appl_status_t              status;
    eOmn_appl_cmmnds_t              cmmnds;
} eOmn_appl_t;                      EO_VERIFYsizeof(eOmn_appl_t, 24);



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section




/** @}            
    end of group eo_management  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




