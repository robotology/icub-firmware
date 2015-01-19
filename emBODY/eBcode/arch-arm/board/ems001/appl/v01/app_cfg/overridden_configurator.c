
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

extern void eom_emsconfigurator_hid_userdef_ProcessUserdef00Event(EOMtheEMSconfigurator* p)
{
    eOresult_t  res;
    uint8_t     numofRXcanframe = 0;

    
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
    

    // se non ho la skin, allora entro dentro
    if(!(eo_emsapplBody_HasDevice(eo_emsapplBody_GetHandle(), eo_emsapplbody_deviceid_skin)))
    {
        numofRXcanframe = 0;
        res = eo_appCanSP_GetNumOfRecCanframe(appcanSP, eOcanport2, &numofRXcanframe);
        if(eores_OK != res)
        { // se c'e un errore sui parametri passati ovvero mai., 
            return;
        }
#ifdef _GET_CANQUEUE_STATISTICS_
        eo_theEMSdgn_updateCanRXqueueStatisticsOnConfigMode(eOcanport2, numofRXcanframe);
#endif
        // se non ho la skin leggo sul can2 tutti i canframe presenti e per ognuno chiamo le funzioni di callback 
        // sono i messaggi provenienti dai sensori strain
        eo_appCanSP_read(appcanSP, eOcanport2, numofRXcanframe, NULL);
    }
        
}


// marco.accame on Jan 15 2015: the user-defined emsconfigurator_evt_userdef01 is triggered by a timer inside the EOtheEMSapplBody to
// tell the that we must do again of verification of presence of can boards.

extern void eom_emsconfigurator_hid_userdef_ProcessUserdef01Event(EOMtheEMSconfigurator* p)
{
    uint32_t readyCanBoardsMask = 0;    // keeps the boards that are ready. if bit pos i-th is 1, then the board in list i-th is OK   

    EOappTheDB  *db = eo_emsapplBody_GetDataBaseHandle(eo_emsapplBody_GetHandle());


    // verifico che le board mc4 ed 1foc siano ready, ovvero che abbiamo mandato la loro fw version
    if(eobool_true == eo_appTheDB_areConnectedCanBoardsReady(db, &readyCanBoardsMask))
    {
        // se tutte le 1foc e le mc4 sono ready, allora setto che lo sono (stoppo il timer canBoardsReady_timer da 10 milli)
        // e poi mando la configurazione alle board can
        eo_emsapplBody_checkCanBoards_Stop(eo_emsapplBody_GetHandle());
        eo_emsapplBody_sendConfig2canboards(eo_emsapplBody_GetHandle());
    }
    else
    {  
        // i check again if the can boards are ready. however, i dont check the boards already ready
        eo_emsapplBody_checkCanBoardsAreReady(eo_emsapplBody_GetHandle(), readyCanBoardsMask);
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




