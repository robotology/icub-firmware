/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOCFG_SM_CANGTW_H_
#define _EOCFG_SM_CANGTW_H_



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOsm.h"

#include "EOMtask.h"
#include "EOsocketDatagram.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/**	@typedef    typedef enum eOsmEventsCanGtw_t
 	@brief 		The events to be used by a state machine configured with EOtheSMCfgCanGtw
 **/ 
typedef enum 
{
	eo_sm_cangtw_evdummy    =  eo_sm_evNONE,        /**< the null event derived from eOsmBasicEvents_t */
	eo_sm_cangtw_evstart 	= (eOsmEvent_t) 0,	    /**< another event managed by the example state machine */
	eo_sm_cangtw_evcanstable= (eOsmEvent_t) 1,		/**< another event managed by the example state machine */
	eo_sm_cangtw_evgo2run   = (eOsmEvent_t) 2,		/**< another event managed by the example state machine */
	eo_sm_cangtw_evrxeth    = (eOsmEvent_t) 3,		/**< another event managed by the example state machine */
	eo_sm_cangtw_evrxcan1   = (eOsmEvent_t) 4,		/**< another event managed by the example state machine */
	eo_sm_cangtw_evrxcan2   = (eOsmEvent_t) 5
} eOsmEventsCanGtw_t;


/** @typedef    typedef struct eOsmDynamicDataCanGtw_t
    @brief      eOsmDynamicDataCanGtw_t is the struct which contains the dynamic data structure used in the functions
                of this state machine. It will be responsibility of the state machine object to allocate
                RAM for this struct.
 **/ 
typedef struct 
{
    EOMtask*            taskCANgtw;
    EOsocketDatagram*   sockCANgtw;
    uint8_t             number_of_sent_canblmsg;
} eOsmDynamicDataCanGtw_t;
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

 
/** @fn         extern const eOsm_cfg_t * eo_cfg_sm_cangtw_Get(void) 
    @brief      Gets the pointer to the eOsm_cfg_t for a EOsm.
    @return     Pointer to the configuration. 
 **/
extern const eOsm_cfg_t * eo_cfg_sm_cangtw_Get(void);




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




