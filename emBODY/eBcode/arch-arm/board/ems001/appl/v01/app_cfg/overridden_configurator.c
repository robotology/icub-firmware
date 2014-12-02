
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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"
#include "EOMtask.h"

#include "EOMtheEMSappl.h"
#include "EOtheEMSApplBody.h"
#include "EOMtheEMSapplCfg.h"

#include "EOtheEMSapplDiagnostics.h"

#include "EoError.h"
#include "EOtheErrorManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSconfigurator.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSconfigurator_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define configurator_timeout_send_diagnostics   1000
#define MAX_WAITFOR2FOC                         300


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// marco.accame on Nov 26 2014: the user-defined emsconfigurator_evt_userdef is triggered by the EOtheEMSapplBody to
// tell the EOMtheEMSconfigurator that has received a CAN frame.

extern void eom_emsconfigurator_hid_userdef_ProcessUserdefEvent(EOMtheEMSconfigurator* p)
{
    static uint32_t waittimefor2foc = 0;
    eOresult_t  res;
    uint8_t     numofRXcanframe = 0;
    uint32_t    canBoardsReady = 0;
    EOappCanSP  *appcanSP = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    EOappTheDB  *db = eo_emsapplBody_GetDataBaseHandle(eo_emsapplBody_GetHandle());
   
    res = eo_appCanSP_GetNumOfRecCanframe(appcanSP, eOcanport1, &numofRXcanframe);
    if(eores_OK != res)
    {
        return;
    }
    
#ifdef _GET_CANQUEUE_STATISTICS_
    eo_theEMSdgn_updateCanRXqueueStatisticsOnConfigMode(eOcanport1, numofRXcanframe);
#endif
    eo_appCanSP_read(appcanSP, eOcanport1, numofRXcanframe, NULL);
    


    if(!(eo_emsapplBody_HasDevice(eo_emsapplBody_GetHandle(), eo_emsapplbody_deviceid_skin)))
    {
        numofRXcanframe = 0;
        res = eo_appCanSP_GetNumOfRecCanframe(appcanSP, eOcanport2, &numofRXcanframe);
        if(eores_OK != res)
        {
            return;
        }
#ifdef _GET_CANQUEUE_STATISTICS_
        eo_theEMSdgn_updateCanRXqueueStatisticsOnConfigMode(eOcanport2, numofRXcanframe);
#endif
        eo_appCanSP_read(appcanSP, eOcanport2, numofRXcanframe, NULL);
    }
    
    if(eo_appTheDB_areConnectedCanBoardsReady(db,&canBoardsReady))
    {
        eo_emsapplBody_setCanBoardsAreReady(eo_emsapplBody_GetHandle());
        eo_emsapplBody_sendConfig2canboards(eo_emsapplBody_GetHandle());
    }
    else
    {
        if(waittimefor2foc<MAX_WAITFOR2FOC)
        {
            waittimefor2foc++;
        }
        else
        {
            eo_emsapplBody_checkCanBoardsAreReady(eo_emsapplBody_GetHandle(), canBoardsReady);
        }
    }
    
}

// marco.accame on Nov 26 2014: this function is triggered if function eom_emssocket_Transmit() fails
// to transmit a udp packet.
extern void eom_emsconfigurator_hid_userdef_onemstransceivererror(EOMtheEMStransceiver* p)
{
    eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());
    // marco.accame: for now i remove the action of this object and i call the errormanager. for later we can have both the error handlers
    // eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , configurator_timeout_send_diagnostics);    
    
    eOerrmanDescriptor_t errdes = {0};
    errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_configurator_udptxfailure);
    errdes.param            = 0;
    errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress    = 0;    
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, "EOMtheEMSconfigurator", &errdes); 
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




