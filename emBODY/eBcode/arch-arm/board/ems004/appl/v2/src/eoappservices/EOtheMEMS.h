/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOTHEMEMS_H_
#define _EOTHEMEMS_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheMEMS.h
    @brief      this object implements what is required for managing the SKIN.                
    @author     marco.accame@iit.it
    @date       12/12/2015
**/

/** @defgroup eo_EOtheMEMS Object EOtheMEMS
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMtask.h"

#include "EoAnalogSensors.h"

#include "hal_gyroscope.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheMEMS_hid EOtheMEMS;

typedef struct
{
    uint8_t         priority;       // of the worker task (evt-based)    
    uint16_t        stacksize;      // of the worker task (evt-based)  
} eOmems_cfg_t;

typedef enum
{
    mems_gyroscope_l3g4200    = 0
//    mems_accelerometer    = 1   for future use
} eOmems_sensor_t;

enum { eomems_numberofthem = 1 };

typedef struct
{
    hal_gyroscope_range_t    range;     
} eOmems_properties_gyroscope_l3g4200_t;

typedef union 
{
    eOmems_properties_gyroscope_l3g4200_t   gyroscope;    
} eOmems_properties_t;

typedef struct
{
    eOmems_sensor_t         sensor;    
    eOmems_properties_t     properties;
    eOreltime_t             acquisitionrate;
} eOmems_sensor_cfg_t;

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const eOmems_cfg_t eo_mems_DefaultCfg;

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOtheMEMS* eo_mems_Initialise(const eOmems_cfg_t *cfg);

extern EOtheMEMS* eo_mems_GetHandle(void);

extern eObool_t eo_mems_IsSensorSupported(EOtheMEMS *p, eOmems_sensor_t sensor);

extern eOresult_t eo_mems_Config(EOtheMEMS *p, eOmems_sensor_cfg_t *cfg);

extern eOresult_t eo_mems_Start(EOtheMEMS *p);

extern eOresult_t eo_mems_SetAlert(EOtheMEMS *p, EOMtask *task, eOevent_t event);

extern eOresult_t eo_mems_Get(EOtheMEMS *p, eOas_inertial3_data_t* data, eOreltime_t timeout, eOmems_sensor_t *sensor, uint16_t* remaining);

extern eOresult_t eo_mems_Stop(EOtheMEMS *p);






/** @}            
    end of group eo_EOtheMEMS
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


