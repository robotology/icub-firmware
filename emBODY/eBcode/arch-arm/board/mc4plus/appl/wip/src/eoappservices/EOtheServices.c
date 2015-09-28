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


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheServices s_eo_theserv = 
{
    .initted                 = eobool_false
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
    eo_candiscovery_Initialise();
    eo_candiscovery_Start(eo_candiscovery_GetHandle());
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


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



