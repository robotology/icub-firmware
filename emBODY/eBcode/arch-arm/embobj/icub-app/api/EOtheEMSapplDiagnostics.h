/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
#ifndef _EOTHEEMSAPPLDIAGNOTICS_H_
#define _EOTHEEMSAPPLDIAGNOTICS_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtheEMSapplDiagnostics.h
	@brief      This header file implements public interface to the EMS application diagnostics.
 	@author     valentina.gaggero@iit.it
	@date       05/10/2013
 **/

/** @defgroup bal bla
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoDiagnostics.h"
#include "OPCprotocolManager_Cfg.h"
#include "EOappEncodersReader.h"



// - public #define  --------------------------------------------------------------------------------------------------
// #define eodgn_nvidbdoor_cmds            1
// #define eodgn_nvidbdoor_emsperiph       2
// #define eodgn_nvidbdoor_emsapplcore     3
// #define eodgn_nvidbdoor_emsapplmc       4

// #define eodgn_nvidbdoor_max              5 //you can use until 10 see file OPCprotocolManager_Cfg.h!! 

// - declaration of public user-defined types ------------------------------------------------------------------------- 
typedef struct EOTheEMSdiagnostics_hid_t EOTheEMSdiagnostics_t;





    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern eOdgn_commands_t                    eo_dgn_cmds;
extern eOdgn_errorlog_t                    eo_dgn_errorlog;
extern eOdgn_emsperipheralstatus_t         eo_dgn_emsperiph;
extern eOdgn_emsapplication_common_t       eo_dgn_emsapplcore;
extern eOdgn_emsapplication_emswithmc_t    eo_dgn_emsappmc;
extern eOdgn_motorstatusflags_t            eo_dgn_motorstflag;
extern eOdgn_rxCheckSetpoints_t            eo_dgn_rxchecksepoints;
extern eOdgn_can_statistics_t              eo_dgn_canstatistics;



// - declaration of extern public functions ---------------------------------------------------------------------------
extern EOTheEMSdiagnostics_t* eo_theEMSdgn_Initialize(void);

extern EOTheEMSdiagnostics_t* eo_theEMSdgn_GetHandle(void);


extern eOresult_t eo_theEMSdgn_Signalerror(EOTheEMSdiagnostics_t* p, uint16_t var , eOreltime_t timeout);

extern eObool_t eo_theEMSdgn_IsExtFault2Signal(EOTheEMSdiagnostics_t* p);

extern eOresult_t eo_theEMSdgn_UpdateApplCore(EOTheEMSdiagnostics_t* p);

extern eOresult_t eo_theEMSdgn_UpdateApplWithMc(EOTheEMSdiagnostics_t* p, EOappEncReader* encreader_ptr, uint16_t count);

extern eOresult_t eo_theEMSdgn_UpdateMotorStFlags(EOTheEMSdiagnostics_t* p, eOmc_motorId_t mId, uint8_t flags);

extern eOresult_t eo_theEMSdgn_ClearMotorStFlags(EOTheEMSdiagnostics_t* p);

extern eOresult_t eo_theEMSdgn_UpdateErrorLog(EOTheEMSdiagnostics_t* p, char *str_error, uint16_t size);

extern eOresult_t eo_theEMSdgn_checkEthLinkStatus(EOTheEMSdiagnostics_t* p, uint8_t *link1_isup, uint8_t *link2_isup);

extern eOresult_t eo_theEMSdgn_resetSetpoints(EOTheEMSdiagnostics_t* p);

extern eOresult_t eo_theEMSdgn_UpdateDummyFieldOfApplWithMc(EOTheEMSdiagnostics_t* p, uint32_t data);

extern eOresult_t eo_theEMSdgn_UpdateCanQueuesStatistics(EOTheEMSdiagnostics_t* p, eOcanport_t canport, uint8_t numofRXcanframe, uint8_t numofXcanframe);

extern void eo_theEMSdgn_updateCanRXqueueStatisticsOnRunMode(eOcanport_t canport, uint8_t numofRXcanframe);

extern void eo_theEMSdgn_updateCanTXqueueStatisticsOnRunMode(eOcanport_t canport, uint8_t numofTXcanframe);

extern void eo_theEMSdgn_updateCanTXqueueStatisticsOnConfigMode(eOcanport_t canport, uint8_t numofTXcanframe);

extern void eo_theEMSdgn_updateCanRXqueueStatisticsOnConfigMode(eOcanport_t canport, uint8_t numofRXcanframe);

#if 0
extern void eo_theEMSdgn_updateCanQueuesStatistics(eOdgn_appl_phase_t phase, eOcanport_t canport);


extern eOresult_t eo_theEMSdgn_checkErrors(EOTheEMSdiagnostics_t* p);

extern eOresult_t eo_theEMSdgn_checkLinksStatus(EOTheEMSdiagnostics_t* p);
#endif
/** @}            
    end of group 
    
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

