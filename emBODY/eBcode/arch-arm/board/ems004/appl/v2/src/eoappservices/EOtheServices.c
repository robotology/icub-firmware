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

#include "EoError.h"

#include "EOMtheEMSappl.h"

#include "EoProtocolMN.h"

#include "EOtheBOARDtransceiver.h"

//#include "hal_trace.h"

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
static eOresult_t s_after_verify_mais(EOaService* p, eObool_t operationisok);
static eOresult_t s_after_verify_strain(EOaService* p, eObool_t operationisok);
static eOresult_t s_after_verify_skin(EOaService* p, eObool_t operationisok);
static eOresult_t s_after_verify_inertials(EOaService* p, eObool_t operationisok);


static eOresult_t s_eo_services_verifyactivate(EOtheServices *p, eOmn_serv_category_t category, eOmn_serv_configuration_t* config);

static eOresult_t s_eo_services_process_start(EOtheServices *p, eOmn_serv_category_t category);
static eOresult_t s_eo_services_process_stop(EOtheServices *p, eOmn_serv_category_t category);

static eOresult_t s_eo_services_process_regsig_manage(EOtheServices *p, eOmn_serv_category_t category, eOmn_serv_arrayof_id32_t* arrayofid32);
    
static eOresult_t s_eo_services_process_failure(EOtheServices *p, eOmn_service_operation_t operation, eOmn_serv_category_t category);

static eOresult_t s_eo_services_start(EOtheServices *p, eOmn_serv_category_t category);
static eOresult_t s_eo_services_stop(EOtheServices *p, eOmn_serv_category_t category);


static void send_rop_command_result(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
    
static EOtheServices s_eo_theservices = 
{
    .initted            = eobool_false,
    .nvset              = NULL,
    .timer              = NULL,
    .board              = eo_prot_BRDdummy,
    .allactivated       = eobool_false,
    .failedservice      = eo_service_none,
    .mnservice          = NULL,
    .running            = {eobool_false}
};

static const char s_eobj_ownname[] = "EOtheServices";


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
    
    s_eo_theservices.allactivated = eobool_false;
    s_eo_theservices.failedservice = eo_service_none;
    s_eo_theservices.mnservice = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_management, eoprot_entity_mn_service, 0);
    
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



extern eOmn_serv_state_t eo_service_GetState(eOmn_serv_category_t category)
{
    eOmn_serv_state_t state = eomn_serv_state_notsupported;
    
    if(eobool_true == s_eo_theservices.initted)
    {
        return(state);
    }  

    switch(category)
    {
        
        case eomn_serv_category_mais:
        {
            state = eo_mais_GetServiceState(eo_mais_GetHandle());
        } break;
        
        case eomn_serv_category_mc:
        {
            state = eo_motioncontrol_GetServiceState(eo_motioncontrol_GetHandle());
        } break;
        
        case eomn_serv_category_strain:
        {
            state = eo_strain_GetServiceState(eo_strain_GetHandle());   
        } break;
        
        case eomn_serv_category_inertials:
        {
            state = eo_inertials_GetServiceState(eo_inertials_GetHandle());   
        } break;
        
        case eomn_serv_category_skin:
        {
            state = eo_skin_GetServiceState(eo_skin_GetHandle());
        } break;
        
        default:
        {
            
        } break;

    }

    return(state);
    
}

extern eOresult_t eo_services_ActivateAll(EOtheServices *p, eOprotBRD_t brd)
{
    if((NULL == p))
    {
        return(eores_NOK_nullpointer);
    }  


    return(eores_OK);
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

            
        // inside eo_canserv_Initialise() it is called hal_can_supported_is(canx) to see if we can init the can bus as requested.
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


extern eObool_t eo_services_AllActivated(EOtheServices *p)
{    
    if(NULL == p)
    {
        return(eobool_false);
    }    
        
    return(s_eo_theservices.allactivated);    
}


extern eOresult_t eo_services_SendFailureReport(EOtheServices *p)
{    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_true == p->allactivated)
    {
        // so far dont send any OK report
        return(eores_OK);
    }
        
    switch(s_eo_theservices.failedservice)
    {
        case eo_service_mc:
        {
            eo_motioncontrol_SendReport(eo_motioncontrol_GetHandle());
        } break;

        case eo_service_strain:
        {
            eo_strain_SendReport(eo_strain_GetHandle());
        } break;
        
        case eo_service_mais:
        {
            eo_mais_SendReport(eo_mais_GetHandle());            
        } break; 

        case eo_service_skin:
        {
            eo_skin_SendReport(eo_skin_GetHandle());
        } break;   


        case eo_service_inertial:
        {
            eo_inertials_SendReport(eo_inertials_GetHandle());   
        } break;        
        
        case eo_service_none:
        {
            // send message that the services are not verified yet.           
            eOerrmanDescriptor_t errorDescriptor = {0};
            errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
            errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_services_not_verified_yet);
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errorDescriptor);            
        } break;
        
        default:
        {
            
        } break;
        
    }
        
    return(eores_OK);    
}


extern eOresult_t eo_services_ProcessCommand(EOtheServices *p, eOmn_service_cmmnds_command_t *command)
{
    if((NULL == p) || (NULL == command))
    {
        return(eores_NOK_nullpointer);
    }
    
    
    eOmn_service_operation_t operation = (eOmn_service_operation_t)command->operation;
    eOmn_serv_category_t category = (eOmn_serv_category_t)command->category;
    eOmn_serv_configuration_t *config = &command->parameter.configuration;
    eOmn_serv_arrayof_id32_t *arrayofid32 = &command->parameter.arrayofid32;
    
    
    // if we can do it, then we ... do it and then the callback of ... will sig<command.result, OK>
    // if we cannot do it, then send sig<command.result, KO>
    
    // the operation can be: eomn_serv_operation_verifyactivate, eomn_serv_operation_start, eomn_serv_operation_stop
    // the category can be mc, mais, strain, skin, inertials
            
    switch(operation)
    {
        case eomn_serv_operation_verifyactivate:
        {
            eo_errman_Trace(eo_errman_GetHandle(), eo_errortype_info, "COMMAND: verifiactivate()", s_eobj_ownname);
            s_eo_services_verifyactivate(p, category, config);            
        } break;
        
        case eomn_serv_operation_start:
        {
            eo_errman_Trace(eo_errman_GetHandle(), eo_errortype_info, "COMMAND: start()", s_eobj_ownname);
            s_eo_services_process_start(p, category);
        } break;
        
        case eomn_serv_operation_stop:
        {
            eo_errman_Trace(eo_errman_GetHandle(), eo_errortype_info, "COMMAND: stop()", s_eobj_ownname);            
            s_eo_services_process_stop(p, category);
        } break;
        
        case eomn_serv_operation_regsig_load:
        {
            eo_errman_Trace(eo_errman_GetHandle(), eo_errortype_info, "COMMAND: load regsig()", s_eobj_ownname);            
            s_eo_services_process_regsig_manage(p, category, arrayofid32);
        } break;        

        case eomn_serv_operation_regsig_clear:
        {
            eo_errman_Trace(eo_errman_GetHandle(), eo_errortype_info, "COMMAND: clear regsig()", s_eobj_ownname);            
            s_eo_services_process_regsig_manage(p, category, NULL);
        } break;  
        
        default:
        {
            eo_errman_Trace(eo_errman_GetHandle(), eo_errortype_info, "COMMAND: failure()", s_eobj_ownname);
            // send failure
            s_eo_services_process_failure(p, operation, category);
        } break;
        
    }
    
    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern void eoprot_fun_INIT_mn_service_wholeitem(const EOnv* nv)
{    
    eOmn_service_t* mnservice = eo_nv_RAM(nv);
    
    // we init the stateofservice with eomn_serv_state_notsupported. we shall change later on.
    memset(&mnservice->status.stateofservice, eomn_serv_state_notsupported, sizeof(mnservice->status.stateofservice));    
    
    memset(&mnservice->status.filler, 3, sizeof(mnservice->status.filler));
 
    mnservice->status.commandresult.operation = eomn_serv_operation_none;
}






extern eOresult_t eo_service_hid_SynchServiceState(EOtheServices *p, eOmn_serv_category_t category, eOmn_serv_state_t servstate)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }   
    
    if((eomn_serv_category_none == category) || (eomn_serv_category_all == category))
    {
        return(eores_NOK_generic);
    }
    
    p->mnservice->status.stateofservice[category] = servstate;
    
    return(eores_OK);
}


extern eOresult_t eo_service_hid_SetRegulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, eObool_t (*isID32relevant)(uint32_t), uint8_t* numberofthem)
{
    EOarray* id32array = (EOarray*)arrayofid32;    
        
    uint8_t size = 0;
    uint8_t i = 0;
    
    EOtransceiver* boardtransceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle());
    
    eOropdescriptor_t ropdesc;
    memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
    ropdesc.control.plustime        = eobool_false;
    ropdesc.control.plussign        = eobool_false;
    ropdesc.ropcode                 = eo_ropcode_sig;
    ropdesc.id32                    = eo_prot_ID32dummy;    
    ropdesc.signature               = eo_rop_SIGNATUREdummy;  
    uint32_t* id32 = NULL;
    
    // at first we remove all regulars inside id32ofregulars and we reset it
    size = eo_array_Size(id32ofregulars);
    for(i=0; i<size; i++)
    {
        id32 = (uint32_t*)eo_array_At(id32ofregulars, i);
        if(NULL != id32)
        {
            ropdesc.id32 = *id32;
            eo_transceiver_RegularROP_Unload(boardtransceiver, &ropdesc);
        }       
    }    
    eo_array_Reset(id32ofregulars);
    
    
    // then i load the new id32s ... if there are any
    if((NULL != id32array) && (0 != eo_array_Size(id32array)))
    {
        // get all the id32 from id32array (but not more than ...) and: 1. push back into id32ofregulars, 2. load the regular
        size = eo_array_Size(id32array);
        
        eOropdescriptor_t ropdesc;
        memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
        ropdesc.control.plustime    = 0;
        ropdesc.control.plussign    = 0;
        ropdesc.ropcode             = eo_ropcode_sig;
        ropdesc.signature           = eo_rop_SIGNATUREdummy;   
        
        for(i=0; i<size; i++)
        {
            id32 = (uint32_t*)eo_array_At(id32array, i);
            if(NULL != id32)
            { 
                // filter them 
                eObool_t itisrelevant = eobool_true;
                if(NULL != isID32relevant)
                {
                    itisrelevant = isID32relevant(*id32);                   
                }
                
                if(eobool_true == itisrelevant)
                {
                    ropdesc.id32 = *id32;     
                    if(eores_OK == eo_transceiver_RegularROP_Load(boardtransceiver, &ropdesc))
                    {
                        eo_array_PushBack(id32ofregulars, id32);
                        if(eobool_true == eo_array_Full(id32ofregulars))
                        {   // cannot add any more regulars
                            break;
                        }
                    }
                    else
                    {
                        
                    }
                   
                }                
            }           
        }
    }
    
    if(NULL != numberofthem)
    {
        *numberofthem = eo_array_Size(id32ofregulars);
    }
    
    return(eores_OK);     
}



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
    // at first we do motion control, then strain, then skin, then inertials.

    
    const eOmn_serv_configuration_t * servcfg = NULL;
    
    if(NULL != (servcfg = eoboardconfig_code2motion_serv_configuration(s_eo_theservices.board)))
    {
        eo_motioncontrol_Verify(eo_motioncontrol_GetHandle(), servcfg, s_after_verify_motion, eobool_true); 
    }
    else if(NULL != (servcfg = eoboardconfig_code2mais_serv_configuration(s_eo_theservices.board)))
    {
        eo_mais_Verify(eo_mais_GetHandle(), servcfg, s_after_verify_mais, eobool_true); 
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
    else
    {   // nothing else to verify .. actually none is activated but in here tehre was nothing to activate
        s_eo_theservices.allactivated = eobool_true;
    }    
}




static eOresult_t s_after_verify_motion(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        s_eo_theservices.failedservice = eo_service_mc;
        return(eores_OK);
    }
    
    const eOmn_serv_configuration_t * servcfg = NULL;
    
    if(NULL != (servcfg = eoboardconfig_code2mais_serv_configuration(s_eo_theservices.board)))
    {
        eo_mais_Verify(eo_mais_GetHandle(), servcfg, s_after_verify_mais, eobool_true); 
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
    else
    {   // nothing else to verify ..
        s_eo_theservices.allactivated = eobool_true;
    }
    
    return(eores_OK);
}


static eOresult_t s_after_verify_mais(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        s_eo_theservices.failedservice = eo_service_mais;
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
    else
    {   // nothing else to verify ..
        s_eo_theservices.allactivated = eobool_true;
    }    
   
    return(eores_OK);
}


static eOresult_t s_after_verify_strain(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        s_eo_theservices.failedservice = eo_service_strain;
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
    else
    {   // nothing else to verify ..
        s_eo_theservices.allactivated = eobool_true;
    }    
   
    return(eores_OK);
}


static eOresult_t s_after_verify_skin(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        s_eo_theservices.failedservice = eo_service_skin;
        return(eores_OK);
    }
    
    const eOmn_serv_configuration_t * servcfg = NULL;
    
    if(NULL != (servcfg = eoboardconfig_code2inertials_serv_configuration(s_eo_theservices.board)))
    {
        eo_inertials_Verify(eo_inertials_GetHandle(), servcfg, s_after_verify_inertials, eobool_true); 
    }              
    else
    {   // nothing else to verify .. 
        s_eo_theservices.allactivated = eobool_true;
    }        
 
    return(eores_OK);
}

static eOresult_t s_after_verify_inertials(EOaService* p, eObool_t operationisok)
{
    if(eobool_false == operationisok)
    {
        s_eo_theservices.failedservice = eo_service_inertial;
        return(eores_OK);
    }
    
    // nothing else to verify .. 
    s_eo_theservices.allactivated = eobool_true;
  
  
    return(eores_OK);
}


static eOresult_t s_eo_services_verifyactivate(EOtheServices *p, eOmn_serv_category_t category, eOmn_serv_configuration_t* config)
{
    // check if the category is ok and if it is supported. 
    
    // in here we must put for for two phases: 
    // phase1: all services are verified and activated at bootstrap. teh command verifyactivate does nothing apart sending a ok/nok back to robotInterface
    // phase2: at bootstratp service are just _Initalise()-ed and every verification-activation is done in here. 
    
    // if not supported or not already activated send up a negative reply.
    if((eomn_serv_category_none == category) || (eomn_serv_category_all == category))
    {
        // send a failure about wrong param
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
        p->mnservice->status.commandresult.category = category;
        p->mnservice->status.commandresult.operation = eomn_serv_operation_verifyactivate;
        p->mnservice->status.commandresult.type = config->type;
        p->mnservice->status.commandresult.data[0] = eomn_serv_state_notsupported;
        
        send_rop_command_result();   
        
        return(eores_OK);
    }
    
    eOmn_serv_state_t state = (eOmn_serv_state_t)p->mnservice->status.stateofservice[category];
    
    // now have a look at state. it must be eomn_serv_state_activated.
    
    if((eomn_serv_state_activated == state) || (eomn_serv_state_running == state))
    {
        // ok ....
    }
    else
    {       
        // JUST for NOW: send the failure report      
        eo_services_SendFailureReport(eo_services_GetHandle());        
        
        // send a failure about wrong state of the board
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
        p->mnservice->status.commandresult.category = category;
        p->mnservice->status.commandresult.operation = eomn_serv_operation_verifyactivate;
        p->mnservice->status.commandresult.type = config->type;        
        p->mnservice->status.commandresult.data[0] = state;
        
        send_rop_command_result();   
        
        return(eores_OK);
    }
    
    // ok, we have it activated. is the config coherent with what we already have? for instance ... the eOmn_serv_type_t, the number of ...?
    // we decide for now to avoid that.
    
    // we just send the command up telling that things are ok.
    
    p->mnservice->status.commandresult.latestcommandisok = eobool_true;
    p->mnservice->status.commandresult.category = category;
    p->mnservice->status.commandresult.operation = eomn_serv_operation_verifyactivate;
    p->mnservice->status.commandresult.type = config->type;
    p->mnservice->status.commandresult.data[0] = state;
    
    send_rop_command_result();    
    
    return(eores_OK);
}



static eOresult_t s_eo_services_process_start(EOtheServices *p, eOmn_serv_category_t category)
{
    // check if the category is ok and if it is supported. 
    
    // in here we must put for for two phases: 
    // phase1: all services are verified and activated at bootstrap. teh command verifyactivate does nothing apart sending a ok/nok back to robotInterface
    // phase2: at bootstratp service are just _Initalise()-ed and every verification-activation is done in here. 
    
    // if not supported send up a negative reply.
    if((eomn_serv_category_none == category) || (eomn_serv_category_all == category))
    {
        // send a failure about wrong param
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
        p->mnservice->status.commandresult.category = category;
        p->mnservice->status.commandresult.operation = eomn_serv_operation_start;
        p->mnservice->status.commandresult.type = eomn_serv_NONE;
        p->mnservice->status.commandresult.data[0] = eomn_serv_state_notsupported;
        
        send_rop_command_result();   
        
        return(eores_OK);
    }
    
    // now have a look at state. it must be eomn_serv_state_activated or eomn_serv_state_running  
    if((eomn_serv_state_running == p->mnservice->status.stateofservice[category]) || (eomn_serv_state_activated == p->mnservice->status.stateofservice[category]))
    {     
        // ok, we attempt to start it again however
    } 
    else
    {
        // send a failure about wrong state of the board ... dont send it if the service is already started.
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
        p->mnservice->status.commandresult.category = category;
        p->mnservice->status.commandresult.operation = eomn_serv_operation_start;
        p->mnservice->status.commandresult.type = eomn_serv_NONE;        
        p->mnservice->status.commandresult.data[0] = p->mnservice->status.stateofservice[category];
        
        send_rop_command_result();   
        
        return(eores_OK);
    }
    
    
    
    // now we start the service.
    if(eores_OK == s_eo_services_start(p, category))
    {
        p->mnservice->status.commandresult.latestcommandisok = eobool_true;        
    }
    else
    {
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
    }
    
    // ok, we have it activated. is the config coherent with what we already have? for instance ... the eOmn_serv_type_t, the number of ...?
    // we decide for now to avoid that.
    
    // we just send the command up telling that things are ok.
    
    p->mnservice->status.commandresult.category = category;
    p->mnservice->status.commandresult.operation = eomn_serv_operation_start;
    p->mnservice->status.commandresult.type = eomn_serv_NONE;
    p->mnservice->status.commandresult.data[0] = p->mnservice->status.stateofservice[category];
    
    send_rop_command_result();    
    
    return(eores_OK);        
}


static eOresult_t s_eo_services_process_stop(EOtheServices *p, eOmn_serv_category_t category)
{
    // check if the category is ok and if it is supported. 
    
    // in here we must put for for two phases: 
    // phase1: all services are verified and activated at bootstrap. teh command verifyactivate does nothing apart sending a ok/nok back to robotInterface
    // phase2: at bootstratp service are just _Initalise()-ed and every verification-activation is done in here. 
    
    // if not supported send up a negative reply.
    if(eomn_serv_category_none == category)
    {
        // send a failure about wrong param
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
        p->mnservice->status.commandresult.category = category;
        p->mnservice->status.commandresult.operation = eomn_serv_operation_stop;
        p->mnservice->status.commandresult.type = eomn_serv_NONE;
        p->mnservice->status.commandresult.data[0] = eomn_serv_state_notsupported;
        
        send_rop_command_result();   
        
        return(eores_OK);
    }
    
 
    
    // now we stop the service.
    if(eores_OK == s_eo_services_stop(p, category))
    {
        p->mnservice->status.commandresult.latestcommandisok = eobool_true;        
    }
    else
    {
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
    }
    
    // ok, we have it stopped. is the config coherent with what we already have? for instance ... the eOmn_serv_type_t, the number of ...?
    // we decide for now to avoid that.
    
    // we just send the command up telling that things are ok.
    
    p->mnservice->status.commandresult.category = category;
    p->mnservice->status.commandresult.operation = eomn_serv_operation_stop;
    p->mnservice->status.commandresult.type = eomn_serv_NONE;
    p->mnservice->status.commandresult.data[0] = p->mnservice->status.stateofservice[category];
    
    send_rop_command_result();    
    
    return(eores_OK);        
}


static eOresult_t s_eo_services_process_regsig_manage(EOtheServices *p, eOmn_serv_category_t category, eOmn_serv_arrayof_id32_t* arrayofid32)
{
    // check if the category is ok and if it is supported. 
    uint8_t number = 0;
    
    // in here we must put for for two phases: 
    // phase1: all services are verified and activated at bootstrap. teh command verifyactivate does nothing apart sending a ok/nok back to robotInterface
    // phase2: at bootstratp service are just _Initalise()-ed and every verification-activation is done in here. 
    
    // if not supported send up a negative reply.
    if(eomn_serv_category_none == category)
    {
        // send a failure about wrong param
        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
        p->mnservice->status.commandresult.category = category;
        p->mnservice->status.commandresult.operation = (NULL != arrayofid32) ? (eomn_serv_operation_regsig_load) : (eomn_serv_operation_regsig_clear);
        p->mnservice->status.commandresult.type = eomn_serv_NONE;
        p->mnservice->status.commandresult.data[0] = eomn_serv_state_notsupported;
        
        send_rop_command_result();   
        
        return(eores_OK);
    }
     
    eOresult_t res = eores_NOK_generic;
    
    // now we operate ... by calling the specific function of the service.
    switch(category)
    {
        case eomn_serv_category_mc:
        {
            res = eo_motioncontrol_SetRegulars(eo_motioncontrol_GetHandle(), arrayofid32, &number); 
        } break;
        
        case eomn_serv_category_strain:
        {
            res = eo_strain_SetRegulars(eo_strain_GetHandle(), arrayofid32, &number);
        } break;

        case eomn_serv_category_mais:
        {
            res = eo_mais_SetRegulars(eo_mais_GetHandle(), arrayofid32, &number);
        } break;    

        case eomn_serv_category_skin:
        {
            res = eo_skin_SetRegulars(eo_skin_GetHandle(), arrayofid32, &number);
        } break;        

        case eomn_serv_category_inertials:
        {
            res = eo_inertials_SetRegulars(eo_inertials_GetHandle(), arrayofid32, &number);
        } break;
        
        default:
        {
            res = eores_NOK_generic;
        } break;
    }

    p->mnservice->status.commandresult.latestcommandisok = (eores_OK == res) ? (eobool_true) : (eobool_false);
    
    
    // ok, we have it stopped. is the config coherent with what we already have? for instance ... the eOmn_serv_type_t, the number of ...?
    // we decide for now to avoid that.
    
    // we just send the command up telling that things are ok.
    
    p->mnservice->status.commandresult.category = category;
    p->mnservice->status.commandresult.operation = (NULL != arrayofid32) ? (eomn_serv_operation_regsig_load) : (eomn_serv_operation_regsig_clear);
    p->mnservice->status.commandresult.type = eomn_serv_NONE;
    p->mnservice->status.commandresult.data[0] = p->mnservice->status.stateofservice[category];
    
    send_rop_command_result();    
    
    return(eores_OK);        
}


static eOresult_t s_eo_services_process_failure(EOtheServices *p, eOmn_service_operation_t operation, eOmn_serv_category_t category)
{
    // send a failure about wrong param
    p->mnservice->status.commandresult.latestcommandisok = eobool_false;
    p->mnservice->status.commandresult.category = category;
    p->mnservice->status.commandresult.operation = operation;
    p->mnservice->status.commandresult.type = eomn_serv_NONE;

    send_rop_command_result(); 

    return(eores_OK);    
}



//static eOresult_t s_eo_services_process_stop(EOtheServices *p, eOmn_serv_category_t category)
//{
//    // check if the category is ok and if it is supported. 
//    
//    // in here we must put for for two phases: 
//    // phase1: all services are verified and activated at bootstrap. teh command verifyactivate does nothing apart sending a ok/nok back to robotInterface
//    // phase2: at bootstratp service are just _Initalise()-ed and every verification-activation is done in here. 
//    
//    // if not supported send up a negative reply.
//    if(eomn_serv_category_none == category)
//    {
//        // send a failure about wrong param
//        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
//        p->mnservice->status.commandresult.category = category;
//        p->mnservice->status.commandresult.operation = eomn_serv_operation_start;
//        p->mnservice->status.commandresult.type = eomn_serv_NONE;
//        p->mnservice->status.commandresult.data[0] = eomn_serv_state_notsupported;
//        
//        send_rop_command_result();   
//        
//        return(eores_OK);
//    }
//    
//    // is it a single service or _all ? not for now ...
//    
//    
////    if(eomn_serv_category_all == category)
////    {
////        
////        
////        
////    }
//    
//    
//    // now have a look at state. it must be eomn_serv_state_running.    
//    if(eomn_serv_state_running != p->mnservice->status.stateofservice[category])
//    {
//        // send a failure about wrong state of the board
//        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
//        p->mnservice->status.commandresult.category = category;
//        p->mnservice->status.commandresult.operation = eomn_serv_operation_stop;
//        p->mnservice->status.commandresult.type = eomn_serv_NONE;        
//        p->mnservice->status.commandresult.data[0] = p->mnservice->status.stateofservice[category];
//        
//        send_rop_command_result();   
//        
//        return(eores_OK);
//    }
//    
//    // now we stop the service.
//    if(eores_OK == s_eo_services_start(p, category))
//    {
//        p->mnservice->status.commandresult.latestcommandisok = eobool_true;        
//    }
//    else
//    {
//        p->mnservice->status.commandresult.latestcommandisok = eobool_false;
//    }
//    
//    // ok, we have it activated. is the config coherent with what we already have? for instance ... the eOmn_serv_type_t, the number of ...?
//    // we decide for now to avoid that.
//    
//    // we just send the command up telling that things are ok.
//    
//    p->mnservice->status.commandresult.category = category;
//    p->mnservice->status.commandresult.operation = eomn_serv_operation_start;
//    p->mnservice->status.commandresult.type = eomn_serv_NONE;
//    p->mnservice->status.commandresult.data[0] = p->mnservice->status.stateofservice[category];
//    
//    send_rop_command_result();    
//    
//    return(eores_OK);        
//}


static void send_rop_command_result(void)
{
    eOropdescriptor_t ropdesc;
    // ok, prepare the occasional rops
    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
    ropdesc.ropcode = eo_ropcode_sig;
    ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_service, 0, eoprot_tag_mn_service_status_commandresult);
    ropdesc.data    = NULL; // so that dat from the EOnv is retrieved.            

    // now write what we want inside 
    // already done before this function call
    
    eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);
}


static eOresult_t s_eo_services_start(EOtheServices *p, eOmn_serv_category_t category)
{
    eOresult_t res = eores_NOK_generic;
    
    // now we start the service.
    switch(category)
    {
        case eomn_serv_category_mc:
        {
            res = eo_motioncontrol_Start(eo_motioncontrol_GetHandle());
        } break;
        
        case eomn_serv_category_strain:
        {
            res = eo_strain_Start(eo_strain_GetHandle());
        } break;

        case eomn_serv_category_mais:
        {
            res = eo_mais_Start(eo_mais_GetHandle());
        } break;    

        case eomn_serv_category_skin:
        {
            res = eo_skin_Start(eo_skin_GetHandle());
        } break;        

        case eomn_serv_category_inertials:
        {
            res = eo_inertials_Start(eo_inertials_GetHandle());
        } break;
        
        default:
        {
            res = eores_NOK_generic;
        } break;
    }
    
    
    if(eores_OK == res)
    {
        // must send the application in run mode ...
        p->running[category] = eobool_true;
        res = eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STrun);        
    }
    
    return(res);
}


static eOresult_t s_eo_services_stop(EOtheServices *p, eOmn_serv_category_t category)
{
    eOresult_t res = eores_NOK_generic;
    
    // now we stop the service. and we also clear the regulars
    switch(category)
    {
        case eomn_serv_category_mc:
        {
            res = eo_motioncontrol_Stop(eo_motioncontrol_GetHandle());
            eo_motioncontrol_SetRegulars(eo_motioncontrol_GetHandle(), NULL, NULL);
        } break;
        
        case eomn_serv_category_strain:
        {
            res = eo_strain_Stop(eo_strain_GetHandle());
            eo_strain_SetRegulars(eo_strain_GetHandle(), NULL, NULL);
        } break;

        case eomn_serv_category_mais:
        {
            res = eores_OK; // prefer not to stop mais in order to avoid problems with the mc4can boards going in hw fault .... eo_mais_Stop(eo_mais_GetHandle());
            eo_mais_SetRegulars(eo_mais_GetHandle(), NULL, NULL);
        } break;    

        case eomn_serv_category_skin:
        {
            res = eo_skin_Stop(eo_skin_GetHandle());
            eo_skin_SetRegulars(eo_skin_GetHandle(), NULL, NULL);
        } break;        

        case eomn_serv_category_inertials:
        {
            res = eo_inertials_Stop(eo_inertials_GetHandle());
            eo_inertials_SetRegulars(eo_inertials_GetHandle(), NULL, NULL);
        } break;
        
        case eomn_serv_category_all:
        {
            eo_motioncontrol_Stop(eo_motioncontrol_GetHandle()); 
            eo_motioncontrol_SetRegulars(eo_motioncontrol_GetHandle(), NULL, NULL);

            
             // prefer not to stop mais in order to avoid problems with the mc4can boards going in hw fault .... eo_mais_Stop(eo_mais_GetHandle());
            eo_mais_SetRegulars(eo_mais_GetHandle(), NULL, NULL);
            
            eo_strain_Stop(eo_strain_GetHandle());
            eo_strain_SetRegulars(eo_strain_GetHandle(), NULL, NULL);
            
            eo_skin_Stop(eo_skin_GetHandle());
            eo_skin_SetRegulars(eo_skin_GetHandle(), NULL, NULL);
            
            eo_inertials_Stop(eo_inertials_GetHandle());
            eo_inertials_SetRegulars(eo_inertials_GetHandle(), NULL, NULL);            
            
            // if i dont use the service command to load the rop, then it is safe to remove all rops anyway.
            eo_transceiver_RegularROPs_Clear(eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle()));
            
            // also i send the application in config mode            
            res = eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STcfg);            
        } break;
        
        default:
        {
            res = eores_NOK_generic;
        } break;
    }
    
    
    if(eores_OK == res)
    {
        // must send the application in cfg mode ... only if there is no service running anymore
        p->running[category] = eobool_false; 
        eObool_t anyrunning = eobool_false;        
        for(uint8_t i=0; i<eomn_serv_categories_numberof; i++)
        {
            if(eobool_true == p->running[i])
            {
                anyrunning = eobool_true;
                break;
            }           
        }
        
        if(eobool_false == anyrunning)
        {
            res = eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STcfg);
        } 
    }
    
    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



