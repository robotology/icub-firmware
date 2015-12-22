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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "eOcommon.h"
#include "EOtheErrorManager.h"
#include "EOMtheEMSconfigurator.h"

#include "osal.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheServices.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheServices_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_theservices_cancbkonrx(void *arg);
static eOresult_t s_eo_theservices_onstop_search4mais(void* par, EOtheCANdiscovery2* p, eObool_t searchisok);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheServices s_eo_theserv = 
{
    .initted                    = eobool_false,
    .isASmais_ready             = eobool_false,
    .BOARDisreadyforcontrolloop = eobool_false
};

static eOcanserv_cfg_t eo_canserv_DefaultCfgMc4plus = 
{    
    .mode                   = eocanserv_mode_straight,
    .canstabilizationtime   = 7*OSAL_reltime1sec,
    .rxqueuesize            = {64, 0},
    .txqueuesize            = {64, 0},
    .onrxcallback           = {NULL, NULL},
    .onrxargument           = {NULL, NULL}
};

static const eOcandiscovery_target_t s_candiscoverytarget_mais_mc4plus =
{   
    .boardtype          = eobrd_cantype_mais,
    .filler             = {0},
    .firmwareversion    = {0, 0},
    .protocolversion    = {1, 0},
    .canmap             = {0x4000, 0x0000}
};

//static const char s_eobj_ownname[] = "EOtheServices";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheServices* eo_serv_Initialise(eOserv_cfg_t *cfg)
{
    s_eo_theserv.initted = eobool_true;    
    return(&s_eo_theserv);
}


extern EOtheServices* eo_serv_GetHandle(void)
{
    if(eobool_false == s_eo_theserv.initted)
    {
        return(NULL);
    }
    return(&s_eo_theserv);
}


extern eOresult_t eo_serv_ConfigMC(EOtheServices *p, eOmcconfig_cfg_t *mccfg)
{
//    eOresult_t res = eores_OK;
    
    if((NULL == p) || (NULL == mccfg))
    {
        return(eores_NOK_nullpointer);
    }
    
    eo_mcserv_Initialise(mccfg);
    
    return(eores_OK);
}



extern eOresult_t eo_serv_InitializeCurrentsWatchdog(EOtheServices *p)
{    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eo_currents_watchdog_Initialise();
    
    return(eores_OK);
}

extern eOresult_t eo_serv_ConfigCAN(EOtheServices *p, eOcanserv_cfg_t *cancfg)
{
//    eOresult_t res = eores_OK;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    //no config specified
    if (cancfg == NULL)
    {
        //use default config for MC4plus (can1 and NOT can2)
        cancfg = &eo_canserv_DefaultCfgMc4plus;
        cancfg->onrxcallback[0]  = s_eo_theservices_cancbkonrx; 
        cancfg->onrxargument[0]  = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
    }
    
    //using the EOtheCANservice to initialize the CAN
    eo_canserv_Initialise(cancfg);
    
    return(eores_OK);
}


extern eOresult_t eo_serv_StartCANdiscovery(EOtheServices *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    //Init and start the discovery
    eo_candiscovery2_Initialise(NULL);
    
    //start mais discovery
    const eOcandiscovery_target_t *mais_t = &s_candiscoverytarget_mais_mc4plus;
    eOcandiscovery_onstop_t onstop = {0};
    onstop.function = s_eo_theservices_onstop_search4mais;
    onstop.parameter = NULL;
    eo_candiscovery2_Start(eo_candiscovery2_GetHandle(),mais_t, &onstop);
    
    return(eores_OK);
}

extern eObool_t eo_serv_IsBoardReadyForControlLoop(EOtheServices *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    return(p->BOARDisreadyforcontrolloop);    
}

extern eOresult_t eo_serv_SetBoardReadyForControlLoop(EOtheServices *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
         
    p->BOARDisreadyforcontrolloop = eobool_true;
        
    return eores_OK;
}

extern eOresult_t eo_serv_SendDiscoveryFailureReport(EOtheServices *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());
    
    return(eores_OK);    
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_theservices_cancbkonrx(void *arg)
{
    EOMtask *task = (EOMtask *)arg;
    eom_task_isrSetEvent(task, emsconfigurator_evt_userdef00);
}

static eOresult_t s_eo_theservices_onstop_search4mais(void* par, EOtheCANdiscovery2* p, eObool_t searchisok)
{
    if(eobool_true == searchisok)
    {
        s_eo_theserv.isASmais_ready = eobool_true;
        eo_serv_SetBoardReadyForControlLoop(&s_eo_theserv);
        
        eo_mais_Initialise();
        
        uint8_t board_n = eoprot_board_local_get();
        //only the owners should start the mais (c-shape boards on the lower arms)
        if ((board_n == 15) || (board_n == 18))
            eo_mais_Start(eo_mais_GetHandle());          
    }     
    else
    {
//        const eOcandiscovery_detection_t* det = eo_candiscovery2_GetDetection(eo_candiscovery2_GetHandle());
//        memcpy(&s_applBody.failedDetection, det, sizeof(eOcandiscovery_detection_t));        
    }
    
    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
