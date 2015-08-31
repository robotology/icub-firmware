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
#ifndef _EOTHEINERTIAL_H_
#define _EOTHEINERTIAL_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheInertial.h
    @brief      this object implements what is required for managing the inertial sensor.                
    @author     marco.accame@iit.it
    @date       08/19/2015
**/

/** @defgroup eo_EOtheInertial Object EOtheInertial
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheInertial_hid EOtheInertial;

//typedef enum 
//{
//    eoinertial_sensor_none          = 0,
//    eoinertial_sensor_accelerometer = 1,
//    eoinertial_sensor_gyroscope     = 2
//} eOinertial_sensor_type_t;


//typedef enum
//{
//    // simmetry left or rigth: must all be different
//    eoinertial_position_hand            = 2,

//    eoinertial_position_lowerarm_01     = 3,
//    eoinertial_position_lowerarm_02     = 4,
//    
//    eoinertial_position_upperarm_01     = 5,
//    eoinertial_position_upperarm_02     = 6,
//    eoinertial_position_upperarm_03     = 7,
//    eoinertial_position_upperarm_04     = 8,
//    
//    eoinertial_position_foot_01         = 8,
//    eoinertial_position_foot_02         = 9,
//    
//    eoinertial_position_lowerleg_01     = 10,
//    eoinertial_position_lowerleg_02     = 11,
//    eoinertial_position_lowerleg_03     = 12,
//    eoinertial_position_lowerleg_04     = 13,
//    eoinertial_position_lowerleg_05     = 14,
//    
//    eoinertial_position_upperleg_01     = 15,
//    eoinertial_position_upperleg_02     = 16,
//    eoinertial_position_upperleg_03     = 17,
//    eoinertial_position_upperleg_04     = 18,
//    eoinertial_position_upperleg_05     = 19,
//    eoinertial_position_upperleg_06     = 20,
//    
//    // simmetry central: the can start from 1
//    eoinertial_position_chest_01        = 1,
//    eoinertial_position_chest_02        = 2,
//    eoinertial_position_chest_03        = 3,
//    eoinertial_position_chest_04        = 4    
//    
//} eOintertial_sensor_position_t;


//typedef enum 
//{
//    eoinertial_simmetry_left            = 0,
//    eoinertial_simmetry_rigth           = 1,
//    eoinertial_simmetry_central         = 2
//} eOinertial_sensor_simmetry_t;


//typedef enum
//{
//    eoinertial_name_none                = 0,       
//    eoinertial_name_lhand               = 2,

//    eoinertial_position_lowerarm_01     = 3,
//    eoinertial_position_lowerarm_02     = 4,
//    
//    eoinertial_position_upperarm_01     = 5,
//    eoinertial_position_upperarm_02     = 6,
//    eoinertial_position_upperarm_03     = 7,
//    eoinertial_position_upperarm_04     = 8,
//    
//    eoinertial_position_foot_01         = 8,
//    eoinertial_position_foot_02         = 9,
//    
//    eoinertial_position_lowerleg_01     = 10,
//    eoinertial_position_lowerleg_02     = 11,
//    eoinertial_position_lowerleg_03     = 12,
//    eoinertial_position_lowerleg_04     = 13,
//    eoinertial_position_lowerleg_05     = 14,
//    
//    eoinertial_position_upperleg_01     = 15,
//    eoinertial_position_upperleg_02     = 16,
//    eoinertial_position_upperleg_03     = 17,
//    eoinertial_position_upperleg_04     = 18,
//    eoinertial_position_upperleg_05     = 19,
//    eoinertial_position_upperleg_06     = 20,
//    
//    // simmetry central: the can start from 1
//    eoinertial_position_chest_01        = 1,
//    eoinertial_position_chest_02        = 2,
//    eoinertial_position_chest_03        = 3,
//    eoinertial_position_chest_04        = 4    
//    
//} eOintertial_sensor_name_t;

 
//typedef struct
//{
//    uint64_t        timestamp;  // the time in usec of reception of this data inside the object EOtheInertial
//    uint8_t         type;       // use eOinertial_sensor_type_t. it is always accelerometer apart from eoinertial_position_hand_gyros
//    uint8_t         position;   // in 1100-0000b put eOinertial_sensor_simmetry_t. in 0011-1111b put eOintertial_sensor_position_t.
//    uint16_t        x;          // the x data
//    uint16_t        y;          // the y data
//    uint16_t        z;          // the z data
//} eOinertial_sensor_data_t;     EO_VERIFYsizeof(eOinertial_sensor_data_t, 16);



//typedef struct
//{
//    uint32_t        enableleft;     // use the bitwise OR of eOintertial_sensor_position_t values
//    uint32_t        enablerigth;    // use the bitwise OR of eOintertial_sensor_position_t values
//    uint32_t        enablecentral;  // use the bitwise OR of eOintertial_sensor_position_t values
//    uint8_t         datarate;       // the datarate of all the sensors. it is in milliseconds and in range [10, 200]
//    uint8_t         filler[3];
//} eOinertial_sensor_config_t;   EO_VERIFYsizeof(eOinertial_sensor_config_t, 16);


typedef struct
{
    eOas_inertial_position_t    canmapofsupportedsensors[2][15];  // contains in pos i,j: eoas_inertial_pos_none if no mtb is at that address, else another value of eOas_inertial_position_t
} eOinertial_cfg_t;


// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const eOinertial_cfg_t eo_inertial_cfg_eb2;
extern const eOinertial_cfg_t eo_inertial_cfg_eb4;
extern const eOinertial_cfg_t eo_inertial_cfg_eb10;
extern const eOinertial_cfg_t eo_inertial_cfg_eb11;

// - declaration of extern public functions ---------------------------------------------------------------------------

// only one fixed 
//extern EOtheInertial* eo_inertial_Initialise(eOas_inertialidentifier_t id, const eOinertial_cfg_t* cfg);

extern EOtheInertial* eo_inertial_Initialise2(const eOinertial_cfg_t* cfg);

extern EOtheInertial* eo_inertial_GetHandle(void);


//extern eOresult_t eo_inertial_Config(EOtheInertial *p, eOas_inertial_config_t* inertialcfg);

extern eOresult_t eo_inertial_Config2(EOtheInertial *p, eOas_inertial_config_t* config);

//extern eOresult_t eo_inertial_Start(EOtheInertial *p, eOas_inertial_enableflags_t enableflags);
extern eOresult_t eo_inertial_Start2(EOtheInertial *p);

//extern eOresult_t eo_inertial_Stop(EOtheInertial *p);
extern eOresult_t eo_inertial_Stop2(EOtheInertial *p);

//extern eOresult_t eo_inertial_Receive(EOtheInertial *p, eOinertial_sensor_type_t type, eOcanframe_t *frame, eOcanport_t port);
extern eOresult_t eo_inertial_Receive2(EOtheInertial *p, eOas_inertial_type_t type, eOcanframe_t *frame, eOcanport_t port);


extern eOresult_t eo_inertial_RefreshStatusOfEntity(EOtheInertial *p);


/** @}            
    end of group eo_EOtheInertial
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


