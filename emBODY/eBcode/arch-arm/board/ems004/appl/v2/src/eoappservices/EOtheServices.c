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

#include "EoCommon.h"

#include "EOMtheEMStransceiver.h"

#include "EOconstvector_hid.h"

#include "EOtheCANservice.h"
#include "EOtheCANprotocol.h"

#include "EOtheEntities.h"
#include "EOtheMotionController.h"
#include "EOtheSTRAIN.h"
#include "EOtheMAIS.h"
#include "EOtheSKIN.h"
#include "EOtheInertials.h"

#include "EOtheETHmonitor.h"

#include "EOMtheEMSconfigurator.h"

#include "EOVtheCallbackManager.h"

#include "EOtheBoardConfig.h"

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

static void s_can_cbkonrx(void *arg);

static void s_activate_services_deferred(void);

static void s_activate_services_now(void *p);

static eOresult_t s_after_verify_motion(EOaService* p, eObool_t operationisok);
static eOresult_t s_after_verify_strain(EOaService* p, eObool_t operationisok);
static eOresult_t s_after_verify_skin(EOaService* p, eObool_t operationisok);
static eOresult_t s_after_verify_inertials(EOaService* p, eObool_t operationisok);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
    
static EOtheServices s_eo_theservices = 
{
    .initted    = eobool_false,
    .nvset      = NULL,
    .timer      = NULL,
    .board      = eo_prot_BRDdummy
};

//static const char s_eobj_ownname[] = "EOtheServices";


static const eOprot_EPcfg_t s_eo_theservices_theEPcfgsOthersMAX[] =
{  
    {           
        .endpoint           = eoprot_endpoint_motioncontrol,
        .numberofentities   = {12, 12, 1, 0, 0, 0, 0}     
    },     
    {        
        .endpoint           = eoprot_endpoint_analogsensors,
        .numberofentities   = {1, 1, 1, 1, 0, 0, 0}        
    },
    {        
        .endpoint           = eoprot_endpoint_skin,
        .numberofentities   = {2, 0, 0, 0, 0, 0, 0}        
    }     
};

static const EOconstvector s_eo_theservices_vectorof_EPcfg_max = 
{
    .capacity       = sizeof(s_eo_theservices_theEPcfgsOthersMAX)/sizeof(eOprot_EPcfg_t),
    .size           = sizeof(s_eo_theservices_theEPcfgsOthersMAX)/sizeof(eOprot_EPcfg_t),
    .item_size      = sizeof(eOprot_EPcfg_t),
    .dummy          = 0,
    .stored_items   = (void*)s_eo_theservices_theEPcfgsOthersMAX,
    .functions      = NULL   
};

static const EOconstvector * s_eo_theservices_the_vectorof_EPcfgs = &s_eo_theservices_vectorof_EPcfg_max;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheServices* eo_services_Initialise(void)
{
    if(eobool_true == s_eo_theservices.initted)
    {
        return(&s_eo_theservices);
    }
    
    s_eo_theservices.initted = eobool_true;
    s_eo_theservices.nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
    s_eo_theservices.timer = eo_timer_New();
    
    return(&s_eo_theservices);
}
 

    
extern EOtheServices* eo_services_GetHandle(void)
{
    if(eobool_true == s_eo_theservices.initted)
    {
        return(&s_eo_theservices);
    }
    
    return(NULL);
}


extern eOresult_t eo_services_StartLegacyMode(EOtheServices *p, eOprotBRD_t brd)
{
    if((NULL == p))
    {
        return(eores_NOK_nullpointer);
    }

    {   // A. protocol: max capabilities
        
        // 1. set the board number. the value of the generic board is 99. 
        //    the correct value is used only for retrieving it later on and perform specific actions based on the board number
        p->board = brd;
        eo_nvset_BRDlocalsetnumber(p->nvset, brd);
        
        // 2. i initialise the nvset at max capabilities
        uint16_t numofepcfgs = eo_constvector_Size(s_eo_theservices_the_vectorof_EPcfgs);
        uint8_t i = 0;
        for(i=0; i<numofepcfgs; i++)
        {
            eOprot_EPcfg_t* epcfg = (eOprot_EPcfg_t*) eo_constvector_At(s_eo_theservices_the_vectorof_EPcfgs, i);
            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
            {
                eo_nvset_LoadEP(p->nvset, epcfg, eobool_true);
            }                        
        }
        
        // 3. the callbacks
        // marco.accame on 30 sept 2015: so far i define all the callbacks. however:
        // a. we may decide to define EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME and thus we must later on to load a proper callback. 
        //    BUT maybe better not.
        // b. if not, i MUST later on re-write the callbacks, so that:
        //    a. we can understand if a service is configured (use proper object) and we something only if it configured.
        //    b. make sure that when we use a get entity, we use EOtheEntities which does not address joints beyond those configured
        
    }   // A.
    
    {   // B. the entities: only initted but not started or activated yets
        
        eo_entities_Initialise();
        eo_canmap_Initialise(NULL);
        eo_canprot_Initialise(NULL);
        eo_strain_Initialise();  
        eo_mais_Initialise();        
        eo_motioncontrol_Initialise();    
        eo_skin_Initialise(); 
        eo_inertials_Initialise();               
    }
    
    {   // C.  can services and discovery.
        // so far i do in here what i need without any container
             
        // can-services
        eOcanserv_cfg_t config = {.mode = eocanserv_mode_straight};   
        
        config.mode                 = eocanserv_mode_straight;
        config.canstabilizationtime = 7*eok_reltime1sec;
        config.rxqueuesize[0]       = 64;
        config.rxqueuesize[1]       = 64;
        config.txqueuesize[0]       = 64;
        config.txqueuesize[1]       = 64;  
        config.onrxcallback[0]      = s_can_cbkonrx; 
        config.onrxargument[0]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle());    
        config.onrxcallback[1]      = s_can_cbkonrx; 
        config.onrxargument[1]      = eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()); 

        
#if defined(USE_MC4PLUS)       
        #warning TODO: think of how to config the can in the proper way if a board does not support can2. maybe using hal_can_supported_is().                
        config.rxqueuesize[1] = config.txqueuesize[1] = 0;
        config.onrxcallback[1] = config.onrxargument[1] = NULL;       
#endif
            
        // attenzione alla mc4cplus che ha 1 solo can ... potrei cambiare il eo_canserv_Initialise() in modo che
        // se hal_can_supported_is(canx) e' falso allora non retsituisco errore ma semplicemente non inizializzo.
        eo_canserv_Initialise(&config);   
        
        // can-discovery
        eo_candiscovery2_Initialise(NULL);        
    } 

    {   // D. ethmonitor: init and start
        eo_ethmonitor_Initialise(NULL);        
        eo_ethmonitor_Start(eo_ethmonitor_GetHandle());
    }
    
    // ok, now we can go. the rest of the things is done by a timer which will expire in 100 ms.
    
    s_activate_services_deferred();

    return(eores_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_can_cbkonrx(void *arg)
{
    EOMtask *task = (EOMtask *)arg;
    eom_task_isrSetEvent(task, emsconfigurator_evt_userdef00);
}

static void s_activate_services_deferred(void)
{
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    
    eo_action_SetCallback(act, s_activate_services_now, NULL, eov_callbackman_GetTask(eov_callbackman_GetHandle())); 
    
    eo_timer_Start(s_eo_theservices.timer, eok_abstimeNOW, 100*eok_reltime1ms, eo_tmrmode_ONESHOT, act);          
}

static void s_activate_services_now(void *p)
{
    // depending on the board number we have something to do which is different
    // at first we do motion control, then strain, then skin, then inertials
    
    const eOmn_serv_configuration_t * servcfg = NULL;
    
    if(NULL != (servcfg = eoboardconfig_code2motion_serv_configuration(s_eo_theservices.board)))
    {
        eo_motioncontrol_Verify(eo_motioncontrol_GetHandle(), servcfg, s_after_verify_motion, eobool_true); 
    } 
    else if(NULL != (servcfg = eoboardconfig_code2strain_serv_configuration(s_eo_theservices.board)))
    {
        eo_strain_Verify(eo_strain_GetHandle(), servcfg, s_after_verify_strain, eobool_true); 
    }
    else if(NULL != (servcfg = eoboardconfig_code2skin_serv_configuration(s_eo_theservices.board)))
    {
        eo_skin_Verify(eo_skin_GetHandle(), servcfg, s_after_verify_skin, eobool_true); 
    }
    else if(NULL != (servcfg = eoboardconfig_code2inertials_serv_configuration(s_eo_theservices.board)))
    {
        eo_inertials_Verify(eo_inertials_GetHandle(), servcfg, s_after_verify_inertials, eobool_true); 
    }       
}




static eOresult_t s_after_verify_motion(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        return(eores_OK);
    }
    
    const eOmn_serv_configuration_t * servcfg = NULL;
    
    if(NULL != (servcfg = eoboardconfig_code2strain_serv_configuration(s_eo_theservices.board)))
    {
        eo_strain_Verify(eo_strain_GetHandle(), servcfg, s_after_verify_strain, eobool_true); 
    }
    else if(NULL != (servcfg = eoboardconfig_code2skin_serv_configuration(s_eo_theservices.board)))
    {
        eo_skin_Verify(eo_skin_GetHandle(), servcfg, s_after_verify_skin, eobool_true); 
    }
    else if(NULL != (servcfg = eoboardconfig_code2inertials_serv_configuration(s_eo_theservices.board)))
    {
        eo_inertials_Verify(eo_inertials_GetHandle(), servcfg, s_after_verify_inertials, eobool_true); 
    }           
    

    return(eores_OK);
}


static eOresult_t s_after_verify_strain(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        return(eores_OK);
    }
    
    
    const eOmn_serv_configuration_t * servcfg = NULL;
    
    if(NULL != (servcfg = eoboardconfig_code2skin_serv_configuration(s_eo_theservices.board)))
    {
        eo_skin_Verify(eo_skin_GetHandle(), servcfg, s_after_verify_skin, eobool_true); 
    }
    else if(NULL != (servcfg = eoboardconfig_code2inertials_serv_configuration(s_eo_theservices.board)))
    {
        eo_inertials_Verify(eo_inertials_GetHandle(), servcfg, s_after_verify_inertials, eobool_true); 
    }              
    
   
    return(eores_OK);
}


static eOresult_t s_after_verify_skin(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        return(eores_OK);
    }
    
    const eOmn_serv_configuration_t * servcfg = NULL;
    
    if(NULL != (servcfg = eoboardconfig_code2inertials_serv_configuration(s_eo_theservices.board)))
    {
        eo_inertials_Verify(eo_inertials_GetHandle(), servcfg, s_after_verify_inertials, eobool_true); 
    }              
        
 
    return(eores_OK);
}

static eOresult_t s_after_verify_inertials(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        return(eores_OK);
    }
    
 
   
    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



