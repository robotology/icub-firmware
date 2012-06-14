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
#ifndef _EOMTHEEMSRUNNER_H_
#define _EOMTHEEMSRUNNER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOMtheEMSrunner.h
	@brief      This header file implements public interface to the EMS runner singleton.
 	@author     marco.accame@iit.it
	@date       05/21/2012
 **/

/** @defgroup eom_EOMtheEMSrunner Singleton EOMtheEMSrunner 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtransceiver.h"
#include "EOMtask.h"

#include "eOcfg_sm_EMSappl.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOMtheEMSrunner_hid EOMtheEMSrunner
    @brief      EOMtheEMSrunner is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct EOMtheEMSrunner_hid EOMtheEMSrunner;

typedef enum
{
    eo_emsrunner_taskid_runRX       = 0,
    eo_emsrunner_taskid_runDO       = 1,
    eo_emsrunner_taskid_runTX       = 2   
} eOemsrunner_taskid_t;

enum { eo_emsrunner_task_numberof   = 3 };


typedef enum
{
    eo_emsrunner_evt_enable       = 0x00000001,
    eo_emsrunner_evt_execute      = 0x00000002
} eOemsrunner_event_t;


/**	@typedef    typedef struct eOemsrunner_cfg_t 
 	@brief      Contains the configuration for the EOMtheEMSrunner. 
 **/
typedef struct
{
    uint8_t         taskpriority[eo_emsrunner_task_numberof];
    uint16_t        taskstacksize[eo_emsrunner_task_numberof];   
    eOreltime_t     period;                 /**< The period of the cycle rx-do-tx. The rx task receive the execute evt at beginning of the cycle.*/
    eOreltime_t     timeRX;                 /**< maximum time allocated to task RX before the task DO is triggered  */  
    eOreltime_t     timeDO;                 /**< maximum time allocated to task DO before the task TX is triggered  */ 
    eOreltime_t     timeTX;                 /**< maximum time allocated to task TX. task RX is triggered at beginning of cycle */ 
    eOreltime_t     safetyGAP;              /**< The time between two consecutive tasks before which the previous task must have finished its execution. */ 
    uint16_t        maxnumofRXpackets;      /**< It allows to receive and parse up to a given number of packets */ 
    uint16_t        maxnumofTXpackets;      /**< so far it can be only 0 or 1 */
} eOemsrunner_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOemsrunner_cfg_t eom_emsrunner_DefaultCfg; // = {.taskpriority = {250, 251, 252}, .taskstacksize = {1024, 1024, 1024}, 
                                                         //    .execDOafter = 500, .execTXafter = 750, .safetyGAP = 50,
                                                         //    .maxnumofRXpackets = 3, maxnumofTXpackets = 1};


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOMtheEMSrunner * eom_emsrunner_Initialise(const eOemsrunner_cfg_t *emsrunnercfg)
    @brief      Initialise the EOMtheEMSrunner singleton 
    @arg        emsrunnercfg       The configuration. NULL is OK.
    @return     The handle to the EOMtheEMSrunner
 **/
extern EOMtheEMSrunner * eom_emsrunner_Initialise(const eOemsrunner_cfg_t *emsrunnercfg);


/** @fn         extern EOMtheEMSrunner * eom_emsrunner_GetHandle(void)
    @brief      Retrieve the EOMtheEMSrunner singleton 
    @return     The handle to the EOMtheEMSrunner
 **/
extern EOMtheEMSrunner * eom_emsrunner_GetHandle(void);


extern eOresult_t eom_emsrunner_Start(EOMtheEMSrunner *p);

extern eOresult_t eom_emsrunner_StopAndGoTo(EOMtheEMSrunner *p, eOsmEventsEMSappl_t ev);

extern EOMtask * eom_emsrunner_GetTask(EOMtheEMSrunner *p, eOemsrunner_taskid_t id);

eObool_t eom_emsrunner_SafetyGapTouched(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid);

eObool_t eom_emsrunner_SafetyGapBroken(EOMtheEMSrunner *p, eOemsrunner_taskid_t taskid);


/** @}            
    end of group eom_EOMtheEMSrunner  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

