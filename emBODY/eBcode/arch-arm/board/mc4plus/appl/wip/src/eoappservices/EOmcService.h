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
#ifndef _EOCANSERVICE_H_
#define _EOCANSERVICE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOmcService.h
    @brief      This file ...                
    @author     marco.accame@iit.it
    @date       01/09/2015
**/

/** @defgroup eo_EOmcService Object EOmcService
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "eOcommon.h"
#include "EoProtocol.h"
#include "EoMotionControl.h"
#include "EoAnalogSensors.h"
#include "EoSkin.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOmcService_hid EOmcService;

typedef enum
{
    eomcserv_type_undefined     = 0,
    eomcserv_type_2foc          = 1,
    eomcserv_type_mc4can        = 2,
    eomcserv_type_mc4plus       = 3        
} eOmcserv_type_t;

typedef struct
{   // mapped as eOcanmap_entitylocation_t
    uint8_t     port    : 1;        // 0 is can1, 1 is can2. not used if onboard is 1.
    uint8_t     addr    : 4;        // if onboard is 1, then it is the index used by hal, otherwise the can address
    uint8_t     insideindex : 2;    // in case of can it is the inside index (0 or 1) of where the motor/joint is. otherwise it is 2
    uint8_t     onboard : 1;        // if 1 is onboard pwm otherwise is on can
} eOmcserv_pwm_mapping_t;


typedef struct
{  
    uint8_t     dummy0   : 1;       
    uint8_t     index    : 4;       // it is the index used by hal,
    uint8_t     dummy1   : 2;       
    uint8_t     type     : 1;       // if 1 is local otherwise is on can
} eOmcserv_act_local_mapping_t;


typedef struct
{   // mapped as eOcanmap_entitylocation_t
    uint8_t     port    : 1;        // 0 is can1, 1 is can2. not used if onboard is 1.
    uint8_t     addr    : 4;        // if onboard is 1, then it is the index used by hal, otherwise the can address
    uint8_t     insideindex : 2;    // in case of can it is the inside index (0 or 1) of where the motor/joint is. otherwise it is 2
    uint8_t     type    : 1;        // if 1 is onboard  otherwise is on can
} eOmcserv_act_oncan_mapping_t;

typedef struct
{   // mapped as eOcanmap_entitylocation_t
    uint8_t     dummy   : 7;       
    uint8_t     type    : 1;         // if 1 is onboard  otherwise is on can
} eOmcserv_act_any_mapping_t;

typedef union
{   // mapped as eOcanmap_entitylocation_t
    eOmcserv_act_any_mapping_t      any;
    eOmcserv_act_local_mapping_t    local;
    eOmcserv_act_oncan_mapping_t    oncan;    
} eOmcserv_act_mapping_t;           EO_VERIFYsizeof(eOmcserv_act_mapping_t, 1); 

typedef struct
{
    uint8_t     etype : 4;      // 0 aea, 1 amo, 2 incr, 3 adc, 4 mais, etc.
    uint8_t     index : 4;      // 0, 1, 2, 3, 4, etc where the number is referred to the hal enum specified by the type.   
    // example: aea encoder on port p6 or the ems is etype=0, index=0
} eOmcserv_enc_mapping_t;

typedef struct
{
    eOmcserv_enc_mapping_t      encoder;   
    eOmcserv_act_mapping_t      actuator; 
} eOmcserv_jomo_cfg_t;          EO_VERIFYsizeof(eOmcserv_jomo_cfg_t, 2);

enum { eomcserv_jomo_maxnumberof = 12 };

typedef struct
{
    eOmcserv_type_t         type;
    uint8_t                 jomosnumber; // so far we have number of motors = number of joints 
    uint16_t                ffu;
    eOmcserv_jomo_cfg_t     jomos[eomcserv_jomo_maxnumberof];
} eOmcserv_cfg_t;           //EO_VERIFYsizeof(eOmcserv_cfg_t, 28);

   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

extern EOmcService* eo_mcserv_Initialise(eOmcserv_cfg_t *cfg);

extern EOmcService* eo_mcserv_GetHandle(void);



extern uint8_t eo_mcserv_NumberOfJoints(EOmcService *p);

extern eOmc_joint_t* eo_mcserv_GetJoint(EOmcService *p, uint8_t index);

extern uint8_t eo_mcserv_NumberOfMotors(EOmcService *p);

extern eOmc_motor_t* eo_mcserv_GetMotor(EOmcService *p, uint8_t index);



// used to verify if all required resources are available: can boards (mc4 or 2foc or mais discovery, etc.)
// it may start a discovery process (e.g., the serach for all mc4-mais or 2foc boards).
extern eOresult_t eo_mcserv_CheckResources(EOmcService *p);

// tells if all resources are ready.
// use it when robotinterface attempt to start the run mode.
extern eObool_t eo_mcserv_AreResourcesReady(EOmcService *p, uint32_t *readyresourcesdescriptor);


// call it in on-entry of run mode
extern eOresult_t eo_mcserv_Start(EOmcService *p);
// call it in on-exit or run mode
extern eOresult_t eo_mcserv_Stop(EOmcService *p);

// call it inside the protocol callbacks ... to be refined along the way.
extern eOresult_t eo_mcserv_Set(EOmcService *p, uint8_t jomo, uint32_t whatisrequired);



// call it inside the DO step of run mode
// it performs differently according to its eOmcserv_type_t type.
// basically: it reads encoders, compute target pwm, send pwm to actuators (pwm peripherals or put inside can tx buffer 2foc boards), update jomo status
extern eOresult_t eo_mcserv_Actuate(EOmcService *p);




/** @}            
    end of group eo_EOmcService
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


