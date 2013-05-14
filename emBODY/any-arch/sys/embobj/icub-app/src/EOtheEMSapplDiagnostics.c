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

#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"
#include "EoDiagnostics.h"
#include "EOMtheEMSbackdoor.h"

#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSrunner.h"
#include "EOMtheIPnet.h"





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheEMSapplDiagnostics.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheEMSapplDiagnostics_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
eOdgn_commands_t                    eo_dgn_cmds;
eOdgn_emsperipheralstatus_t         eo_dgn_emsperiph;
eOdgn_emsapplication_common_t       eo_dgn_emsapplcore;
eOdgn_emsapplication_emswithmc_t    eo_dgn_emsappmc;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOTheEMSdiagnostics_t s_thedgn = {0};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOTheEMSdiagnostics_t* eo_theEMSdgn_Initialize(void)
{
    memset(&eo_dgn_cmds, 0, sizeof(eOdgn_commands_t));
    memset(&eo_dgn_emsperiph, 0, sizeof(eOdgn_emsperipheralstatus_t));
    memset(&eo_dgn_emsapplcore, 0, sizeof(eOdgn_emsapplication_common_t));
    memset(&eo_dgn_emsappmc, 0, sizeof(eOdgn_emsapplication_emswithmc_t));
    
    return(&s_thedgn);
}


extern EOTheEMSdiagnostics_t* eo_theEMSdgn_GetHandle(void)
{
    return(&s_thedgn);
}


extern eOresult_t eo_theEMSdgn_Signalerror(EOTheEMSdiagnostics_t* p, uint16_t var , eOreltime_t timeout)
{
    if(eo_dgn_cmds.enable)
    {    
        return (eom_emsbackdoor_Signal(eom_emsbackdoor_GetHandle(), var , timeout));
    }
    else
    {
        return(eores_OK);
    }
}

extern eOresult_t eo_theEMSdgn_UpdateApplCore(EOTheEMSdiagnostics_t* p)
{

//core
    eOemsrunner_diagnosticsinfo_t* dgn_run_ptr =  eom_emsrunner_GetDiagnosticsInfoHandle(eom_emsrunner_GetHandle());
    if(NULL != dgn_run_ptr)
    {    
        eo_dgn_emsapplcore.core.runst.numberofperiods = dgn_run_ptr->numberofperiods;
        eo_dgn_emsapplcore.core.runst.cumulativeabsoluteerrorinperiod = dgn_run_ptr->cumulativeabsoluteerrorinperiod;
        eo_dgn_emsapplcore.core.runst.meanofabsoluteerrorinperiod = dgn_run_ptr->meanofabsoluteerrorinperiod;
        eo_dgn_emsapplcore.core.runst.movingmeanofabsoluteerrorinperiod = dgn_run_ptr->movingmeanofabsoluteerrorinperiod;
        eo_dgn_emsapplcore.core.runst.maxabsoluteerrorinperiod = dgn_run_ptr->maxabsoluteerrorinperiod;
        eo_dgn_emsapplcore.core.runst.minabsoluteerrorinperiod = dgn_run_ptr->minabsoluteerrorinperiod;
        eo_dgn_emsapplcore.core.runst.executionoverflows[0] = dgn_run_ptr->executionoverflows[0];
        eo_dgn_emsapplcore.core.runst.executionoverflows[1] = dgn_run_ptr->executionoverflows[1];
        eo_dgn_emsapplcore.core.runst.executionoverflows[2] = dgn_run_ptr->executionoverflows[2];
        eo_dgn_emsapplcore.core.runst.datagrams_failed_to_go_in_txsocket = dgn_run_ptr->datagrams_failed_to_go_in_txsocket;
    }
    #warning sistema eo_dgn_emsapplcore.core.runst.cantxfailuretimeoutsemaphore;
    
    
//ipnet
    eOmipnet_diagnosticsinfo_t *dgn_ipnet_ptr = eom_ipnet_GetDiagnosticsInfoHandle(eom_ipnet_GetHandle());
    if(NULL != dgn_ipnet_ptr)
    {  
        eo_dgn_emsapplcore.ipnet.datagrams_failed_to_go_in_rxfifo = dgn_ipnet_ptr->datagrams_failed_to_go_in_rxfifo;
        eo_dgn_emsapplcore.ipnet.datagrams_failed_to_go_in_txosalqueue = dgn_ipnet_ptr->datagrams_failed_to_go_in_txosalqueue;
        eo_dgn_emsapplcore.ipnet.datagrams_failed_to_be_retrieved_from_txfifo = dgn_ipnet_ptr->datagrams_failed_to_be_retrieved_from_txfifo;
        eo_dgn_emsapplcore.ipnet.datagrams_failed_to_be_sent_by_ipal = dgn_ipnet_ptr->datagrams_failed_to_be_sent_by_ipal;  
    }
//transceiver
    eOemstransceiver_diagnosticsinfo_t* dgn_tr_ptr = eom_emstransceiver_GetDiagnosticsInfoHandle(eom_emstransceiver_GetHandle());
    if(NULL != dgn_tr_ptr)
    {  
        eo_dgn_emsapplcore.transceiver.rxinvalidropframes = dgn_tr_ptr->rxinvalidropframes;
        eo_dgn_emsapplcore.transceiver.rxseqnumwrong = dgn_tr_ptr->rxseqnumwrong;
        eo_dgn_emsapplcore.transceiver.lostreplies = dgn_tr_ptr->lostreplies;
        eo_dgn_emsapplcore.transceiver.failuresinloadofreplyropframe = dgn_tr_ptr->failuresinloadofreplyropframe;
        eo_dgn_emsapplcore.transceiver.txropframeistoobigforthepacket = dgn_tr_ptr->txropframeistoobigforthepacket;
        eo_dgn_emsapplcore.transceiver.cannotloadropinregulars = dgn_tr_ptr->cannotloadropinregulars;
        eo_dgn_emsapplcore.transceiver.cannotloadropinoccasionals = dgn_tr_ptr->cannotloadropinoccasionals;
    }
    
    return(eores_OK);
}

extern eOresult_t eo_theEMSdgn_UpdateApplWithMc(EOTheEMSdiagnostics_t* p, EOappEncReader* encreader_ptr)
{
    uint8_t i;
    eOappEncReader_diagnosticsinfo_t* dgn_ptr = eo_appEncReader_GetDiagnosticsHandle(encreader_ptr);
    if(dgn_ptr == NULL)
    {
        return(eores_NOK_nullpointer);
    }
    
    for(i=0; i<6; i++)
    {
        eo_dgn_emsappmc.encreads.encList[i].err_onReadFromSpi = dgn_ptr->enclist[i][err_onReadFromSpi];
        eo_dgn_emsappmc.encreads.encList[i].err_onInvalidValue = dgn_ptr->enclist[i][err_onInvalidValue];
        eo_dgn_emsappmc.encreads.encList[i].err_onParityError = dgn_ptr->enclist[i][err_onParityError];
    }
    return(eores_OK);
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





