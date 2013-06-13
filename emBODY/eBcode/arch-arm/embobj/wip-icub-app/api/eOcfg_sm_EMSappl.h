/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _EOCFG_SM_EMSAPPL_H_
#define _EOCFG_SM_EMSAPPL_H_




/** @file       eOcfg_sm_EMSappl.h
	@brief      This header file implements public interface to the configuration of the example state machine Ex2.
	@author     marco.accame@iit.it
	@date       09/01/2011
**/

/** @defgroup eo_theex2smconfig Configuration eOcfg_sm_Ex2m for EOsm
    ...
    The user should export a proper eOsmEvents<name>_t, and a proper smcfg_<name>_GetHandle() function.
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOsm.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/**	@typedef    typedef enum eOsmEventsEMSappl_t
 	@brief 		The events to be used by a state machine configured with ...
 **/ 
typedef enum 
{
	eo_sm_emsappl_EVdummy           =  eo_sm_evNONE,        /**< the null event */
	eo_sm_emsappl_EVgo2run          = (eOsmEvent_t) 0,		/**< event emitted by the handler of ropreceived */
	eo_sm_emsappl_EVgo2err 	        = (eOsmEvent_t) 1,		/**< event emitted by the EOtheErrorManager */
	eo_sm_emsappl_EVgo2cfg 	        = (eOsmEvent_t) 2		/**< event emitted by the handler of ropreceived */
} eOsmEventsEMSappl_t;

enum { eo_sm_emsappl_EVnumberof = 3 };


/**	@typedef    typedef enum eOsmStatesEMSappl_t
 	@brief 		the states to be used by a state machine configured with ...
 **/ 
typedef enum 
{
	eo_sm_emsappl_STcfg             = (eOsmEvent_t) 0,		/**< state CFG */
	eo_sm_emsappl_STrun 	        = (eOsmEvent_t) 1,		/**< state RUN */
	eo_sm_emsappl_STerr 	        = (eOsmEvent_t) 2		/**< state ERR */
} eOsmStatesEMSappl_t;

enum { eo_sm_emsappl_STnumberof = 3 };

/** @typedef    typedef struct eOsmDynamicDataEMSappl_t
    @brief      contains the dynamic data structure used in the functions of this state machine. It will be responsibility 
                of the state machine object to allocate RAM for this struct.
 **/ 
typedef struct 
{
    uint32_t    dummy;
} eOsmDynamicDataEMSappl_t;
    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

 
/** @fn         extern const eOsm_cfg_t * eo_cfg_sm_EMSappl_Get(void) 
    @brief      Gets the pointer to the eOsm_cfg_t for a EOsm.
    @return     Pointer to the configuration. 
 **/
extern const eOsm_cfg_t * eo_cfg_sm_EMSappl_Get(void);




/** @}            
    end of group eo_theex2smconfig  
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




