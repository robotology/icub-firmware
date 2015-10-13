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
#ifndef _EOTHEMOTIONCONTROLLER_H_
#define _EOTHEMOTIONCONTROLLER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       EOtheMotionController.h
    @brief      this object implements what is required for managing the STRAIN.                
    @author     marco.accame@iit.it
    @date       05/28/2015
**/

/** @defgroup eo_EOtheMotionController Object EOtheMotionController
    ...... 
    @{        
 **/

// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct EOtheMotionController_hid EOtheMotionController;



typedef eOresult_t (*eOmotcon_onendofoperation_fun_t) (EOtheMotionController* p, eObool_t operationisok);

enum { eo_motcon_maxCANboards = 13, eo_motcon_maxENTITIES = 12 };
   
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------


// so far only foc-based ...

extern EOtheMotionController* eo_motioncontrol_Initialise(void);

extern EOtheMotionController* eo_motioncontrol_GetHandle(void);



// it verifies if the service as defined in te configuration is possible (is there a good strain board or not?), it executes a callback
// (which may send a confirmation to the entity which asked fot verification), and then it may activate the strain service by calling  eo_motioncontrol_Activate().
extern eOresult_t eo_motioncontrol_Verify(EOtheMotionController *p, const eOmn_serv_configuration_t * servcfg, eOmotcon_onendofoperation_fun_t onverify, eObool_t activateafterverify);

// it activates the strain service by loading the service configuration
extern eOresult_t eo_motioncontrol_Activate(EOtheMotionController *p, const eOmn_serv_configuration_t * servcfg);

// it deactivates service.
extern eOresult_t eo_motioncontrol_Deactivate(EOtheMotionController *p);


// to be called once before to be ticked
extern eOresult_t eo_motioncontrol_Start(EOtheMotionController *p);

// it ticks every ms of the control-loop
extern eOresult_t eo_motioncontrol_Tick(EOtheMotionController *p);

extern eOresult_t eo_motioncontrol_Stop(EOtheMotionController *p);

/** @}            
    end of group eo_EOtheMotionController
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


