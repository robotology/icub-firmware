
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theServices.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EOnvSet.h"
#include "EOMtheEMStransceiver.h"
#include "EoProtocolMN.h"

#include "EOtheBOARDtransceiver.h"

#include "embot_app_eth_theHandler.h"

#include "embot_app_eth_theErrorManager.h"
#include "embot_os_theScheduler.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



struct embot::app::eth::theServices::Impl
{       
    Config _config {};  
    EOnvSet* _nvset {nullptr};   
    eOmn_service_t *_mnservice {nullptr};
    std::array<bool, Service::numberOfCategories> _running {false};     
    
    // i need a map of objects Service w/ a key given by the Service::Category
    
    std::array<Service *, Service::numberOfCategories> _mapfofservices {nullptr};   

    Impl(); 
    
    bool initialise(const Config &cfg);   
    
    void init_step2();

    bool load(embot::app::eth::Service *s);    
    
    embot::app::eth::Service* get(embot::app::eth::Service::Category cat);
    embot::app::eth::Service* get(const eOmn_service_cmmnds_command_t *command);
    
    bool process(eOmn_service_cmmnds_command_t *command);
    
    bool setregulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, fpIsID32relevant fpISOK, uint8_t* numberofthem);
    bool synch(Service::Category category, Service::State state);
    
    bool stop();
    bool tick();
    
    bool sendresult(Service *s, const eOmn_service_cmmnds_command_t *command, eOmn_serv_state_t state, bool ok); 
    
    // statics
    static bool onendverifyactivate(Service *s, const eOmn_serv_configuration_t *sc, bool ok);

};

embot::app::eth::theServices::Impl::Impl()
{
}

bool embot::app::eth::theServices::Impl::initialise(const Config &cfg)
{
    _config = cfg;    
    _nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
    
    for(auto &s : _mapfofservices) { s = nullptr; }
    for(auto &r : _running) { r = false; }
    
    
    init_step2();
    
    _mnservice = reinterpret_cast<eOmn_service_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_management, eoprot_entity_mn_service, 0));

    
    return true;
} 

#include "EOtheEntities.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"
#include "EOtheCANservice.h"


#include "EOconstvector_hid.h"
static const EOconstvector s_eo_theservices_vectorof_EPcfg_max = 
{
    EO_INIT(.capacity )     sizeof(eoprot_arrayof_maxEPcfgOthers)/sizeof(eOprot_EPcfg_t),
    EO_INIT(.size)          sizeof(eoprot_arrayof_maxEPcfgOthers)/sizeof(eOprot_EPcfg_t),
    EO_INIT(.item_size)     sizeof(eOprot_EPcfg_t),
    EO_INIT(.dummy)         0,
    EO_INIT(.stored_items)  (void*)eoprot_arrayof_maxEPcfgOthers,
    EO_INIT(.functions)     NULL   
};

static const EOconstvector * s_eo_theservices_the_vectorof_EPcfgs = &s_eo_theservices_vectorof_EPcfg_max;


#include "EOMtask.h"
#include "EOMtheEMSconfigurator.h"
static void s_can_cbkonrx(void *arg)
{
    EOMtask *task = (EOMtask *)arg;
    eom_task_isrSetEvent(task, emsconfigurator_evt_userdef00);
}


#include "EOtheCANdiscovery2.h"

#if 0
// to be used only if we have icub-firmware-shared w/ extra callbacks
#include "EoProtocolMC.h"

void OnAsk(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOprotEndpoint_t endpoint = eoprot_ID2endpoint(rd->id32);
    eOprotEntity_t entity = eoprot_ID2entity(rd->id32);
    eOprotIndex_t index = eoprot_ID2index(rd->id32);
    eOprotTag_t tag = eoprot_ID2tag(rd->id32);
    
    embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};    
    const char * tagstr = eoprot_TAG2string(endpoint, entity, tag);
    
    embot::app::eth::theErrorManager::getInstance().trace(
        std::string("ask<") + tagstr + ", " + std::to_string(index) + ">", {"the", thr}); 
     
    uint32_t idtarget = eoprot_ID_get(1, 0, 0, eoprot_tag_mc_joint_config);
     
    static volatile int ciao = 0;        
    if(idtarget == rd->id32)
    {
        ciao ++;
    }
        
}


void OnSet(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOprotEndpoint_t endpoint = eoprot_ID2endpoint(rd->id32);
    eOprotEntity_t entity = eoprot_ID2entity(rd->id32);
    eOprotIndex_t index = eoprot_ID2index(rd->id32);
    eOprotTag_t tag = eoprot_ID2tag(rd->id32);
    
    embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};    
    const char * tagstr = eoprot_TAG2string(endpoint, entity, tag);
    
    embot::app::eth::theErrorManager::getInstance().trace(
        std::string("set<") + tagstr + ", " + std::to_string(index) + ">", {"the", thr}); 
        
}
#endif

void embot::app::eth::theServices::Impl::init_step2()
{

    {   // A. protocol: max capabilities
        
        // 1. set the board number. the value of the generic board is 99. 
        //    the correct value is used only for retrieving it later on and perform specific actions based on the board number
        //eo_nvset_BRDlocalsetnumber(_nvset, 0);
        
        // 2. i initialise the nvset at max capabilities
        uint16_t numofepcfgs = eo_constvector_Size(s_eo_theservices_the_vectorof_EPcfgs);
        uint8_t i = 0;
        for(i=0; i<numofepcfgs; i++)
        {
            eOprot_EPcfg_t* epcfg = (eOprot_EPcfg_t*) eo_constvector_At(s_eo_theservices_the_vectorof_EPcfgs, i);
            if(eobool_true == eoprot_EPcfg_isvalid(epcfg))
            {
                eo_nvset_LoadEP(_nvset, epcfg, eobool_true);
            }                        
        }
        
//        eo_entities_Initialise();
        
        
        
        // 3. the callbacks
        // marco.accame on 30 sept 2015: so far i define all the callbacks. however:
        // a. we may decide to define EOPROT_CFG_OVERRIDE_CALLBACKS_IN_RUNTIME and thus we must later on to load a proper callback. 
        //    BUT maybe better not.
        // b. if not, i MUST later on re-write the callbacks, so that:
        //    a. we can understand if a service is configured (use proper object) and we something only if it configured.
        //    b. make sure that when we use a get entity, we use EOtheEntities which does not address joints beyond those configured
        
    }   // A.    
     
    {  // B. the entities: only initted but not started or activated yets  
        eo_entities_Initialise();
        eo_canmap_Initialise(NULL);
        eo_canprot_Initialise(NULL);
    }
    
    
    {   // C.  can services and discovery.
    // so far i do in here what i need without any container
         
    // can-services
    eOcanserv_cfg_t config;   
    
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
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetEvent(act, emsconfigurator_evt_userdef01, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    eo_candiscovery2_SetTicker(eo_candiscovery2_GetHandle(), act);
  
    }     


//    eoprot_config_onask_endpoint_set(eoprot_endpoint_management, OnAsk);  
//    eoprot_config_onask_endpoint_set(eoprot_endpoint_motioncontrol, OnAsk);  
//    eoprot_config_onask_endpoint_set(eoprot_endpoint_analogsensors, OnAsk);  

//    eoprot_config_onset_endpoint_set(eoprot_endpoint_management, OnSet);  
//    eoprot_config_onset_endpoint_set(eoprot_endpoint_motioncontrol, OnSet);  
//    eoprot_config_onset_endpoint_set(eoprot_endpoint_analogsensors, OnSet); 

}

bool embot::app::eth::theServices::Impl::load(Service *s)
{   
    embot::app::eth::Service::Category c = s->category();
    auto index = embot::core::tointegral(c);
    if(index >= embot::app::eth::Service::numberOfCategories)
    {
        return false;
    }
    _mapfofservices[index] = s;
        
    return true;
} 

embot::app::eth::Service* embot::app::eth::theServices::Impl::get(Service::Category cat)
{
    auto index = embot::core::tointegral(cat);
    if(index >= embot::app::eth::Service::numberOfCategories)
    {
        return nullptr;
    }
    return _mapfofservices[index];
}

embot::app::eth::Service* embot::app::eth::theServices::Impl::get(const eOmn_service_cmmnds_command_t *command)
{
    embot::app::eth::Service::Category cat = static_cast<embot::app::eth::Service::Category>(command->category);
    return get(cat);
}

bool embot::app::eth::theServices::Impl::process(eOmn_service_cmmnds_command_t *command)
{
    embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
    if(nullptr == command)
    {
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::process()", thr}, {}, "(nullptr)");
        return false;
    }
       
    eOmn_serv_operation_t operation = (eOmn_serv_operation_t)command->operation;
    eOmn_serv_category_t category = static_cast<eOmn_serv_category_t>(command->category);
    const eOmn_serv_configuration_t *config = &command->parameter.configuration;
    //eOmn_serv_arrayof_id32_t *arrayofid32 = &command->parameter.arrayofid32;
    
    embot::app::eth::Service::Category cat = static_cast<embot::app::eth::Service::Category>(command->category);
    
    // if category is one which has an associated object we proceed
    // else if it is category_all and we have stop or deactivate we go on   
    // else .... no good
    
    if((embot::app::eth::Service::Category::all == cat) && (eomn_serv_operation_stop == operation))
    {
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::process()", thr}, {}, "stop all");
        stop();        
        sendresult(nullptr, command, eomn_serv_state_idle, true);
        return true;
    }
    
    
    #if 0
    
    theftserv = new FTService;
    theftserv->init();
    theservices.load(theftserv);
    
    Service srv* theServices.get(FTcategory);
    
    srv->verify(arg) etc
    but also
    tehservices.verify(arg) ???
    
    sicuramente 
    theservices.stop() ferma tutto
    
    
    #endif
    
    embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::process()", thr}, {}, "some request");

    embot::app::eth::Service *service = get(cat);
    if(nullptr == service)
    {   // category not supported or not available yet
        #warning TODO: send up ROP w/ failure 
        embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::process()", thr}, {}, "cannot get service of type ...");
        sendresult(nullptr, command, eomn_serv_state_notsupported, false);
        return false;
    }
    
    eOmn_serv_state_t servstate = static_cast<eOmn_serv_state_t>(_mnservice->status.stateofservice[category]); 
    
    switch(operation)
    {
        case eomn_serv_operation_verifyactivate:
        {
            embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::process()", thr}, {}, "verifyactivate");
            service->verify(config, true, onendverifyactivate);
            // we dont send any result. the callback onendverifyactivate() will do it.
            // for reference it was: s_eo_services_process_verifyactivate(category, config);            
        } break;
        
        case eomn_serv_operation_start:
        {
            embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::process()", thr}, {}, "start");
            // if not ok, send up ROP w/ failure
            // else: service->Start(), mark service started
            // send in RUN mode, 
            // send ROP w/ ok
            
            // if state is not OK (it must be either started or activated) send error up
            // else start the service, put in RUN mode and finally send ok up
            if((eomn_serv_state_started != servstate) && (eomn_serv_state_activated != servstate))
            {
                sendresult(service, command, servstate, false);
            }
            else
            {
                bool ok = service->start();
                if(ok)
                {
                    _running[category] = true;
                    embot::app::eth::theHandler::getInstance().process(embot::app::eth::theHandler::Command::go2RUN);                        
                } 
                servstate = static_cast<eOmn_serv_state_t>(_mnservice->status.stateofservice[category]);                
                sendresult(service, command, servstate, ok);                
            }            
            
//            service->Start();
            // for reference it was: s_eo_services_process_start(category);
        } break;
        
        case eomn_serv_operation_stop:
        { 
            embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::process()", thr}, {}, "stop");
            // if not ok, send up ROP w/ failure
            // else: service->Stop(), service->SetRegulars(0), if requested ->Deactivate(), unmark service started
            // if none is running send in IDLE mode, 
            // send ROP w/ ok            
//            s_eo_services_process_stop(category, eobool_true);
            
            service->stop();
            _running[category] = false;
            uint8_t num {0};
            service->setregulars(nullptr, num);
            service->deactivate();                                       
            bool noserviceisrunning {true};
            for(auto r : _running) { if(r) noserviceisrunning = false; }
            if(true == noserviceisrunning)
            {
                embot::app::eth::theHandler::getInstance().process(embot::app::eth::theHandler::Command::go2IDLE);
            }
            // needed again             
            servstate = static_cast<eOmn_serv_state_t>(_mnservice->status.stateofservice[category]);              
            sendresult(service, command, servstate, true);                 
        } break;
        
        case eomn_serv_operation_deactivate:
        {
            embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::process()", thr}, {}, "deactivate");            
            service->deactivate();   
            bool noserviceisrunning {true};
            for(auto r : _running) { if(r) noserviceisrunning = false; }
            if(true == noserviceisrunning)
            {
                embot::app::eth::theHandler::getInstance().process(embot::app::eth::theHandler::Command::go2IDLE);
            } 
            // needed again            
            servstate = static_cast<eOmn_serv_state_t>(_mnservice->status.stateofservice[category]);              
            sendresult(service, command, servstate, true);             
            // for reference: s_eo_services_process_deactivate(category);
        } break;        
        
        case eomn_serv_operation_regsig_load:
        {
            embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::process()", thr}, {}, "regsigload");
            eOmn_serv_arrayof_id32_t *arrayofid32 = &command->parameter.arrayofid32;  
            uint8_t num {0};
            service->setregulars(arrayofid32, num);   
            servstate = static_cast<eOmn_serv_state_t>(_mnservice->status.stateofservice[category]);            
            sendresult(service, command, servstate, true);              
            // for reference: eOmn_serv_arrayof_id32_t *arrayofid32 = &command->parameter.arrayofid32;            
            // s_eo_services_process_regsig(category, arrayofid32);
        } break;        

        case eomn_serv_operation_regsig_clear:
        {
            embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::process()", thr}, {}, "regsigclear");            
            uint8_t num {0};
            service->setregulars(nullptr, num);
            servstate = static_cast<eOmn_serv_state_t>(_mnservice->status.stateofservice[category]);            
            sendresult(service, command, servstate, true);            
            // for reference: s_eo_services_process_regsig(category, NULL);
        } break;  
        
        default:
        {
            embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::process()", thr}, {}, "default ...");
            sendresult(service, command, eomn_serv_state_idle, false);
        } break;
        
    }    
    
    return true;
}


bool embot::app::eth::theServices::Impl::setregulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, fpIsID32relevant fpISOK, uint8_t* numberofthem)
{
    if((nullptr == id32ofregulars) || (nullptr == arrayofid32))
    {
        return false;
    }
    
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
                bool itisrelevant = true;
                if(nullptr != fpISOK)
                {
                    itisrelevant = fpISOK(*id32);                   
                }
                
                if(true == itisrelevant)
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
    
    if(nullptr != numberofthem)
    {
        *numberofthem = eo_array_Size(id32ofregulars);
    }
    
    return true;   
}

#warning SEE this new implem
#if 0
bool embot::app::eth::theServices::Impl::setregulars(EOarray* id32ofregulars, bool clearfirst, eOmn_serv_arrayof_id32_t* arrayofid32, fpIsID32relevant fpISOK, uint8_t* numberofthem)
{
    if(nullptr == id32ofregulars)
    {
        return false;
    }
             
    uint8_t size = 0;
    uint8_t i = 0;
    
    EOtransceiver* boardtransceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle());

    if(true == clearfirst)
    {    
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
    }    
    
    EOarray* id32array = (EOarray*)arrayofid32;   
        
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
        uint32_t* id32 = NULL;        
        
        for(i=0; i<size; i++)
        {
            id32 = (uint32_t*)eo_array_At(id32array, i);
            if(NULL != id32)
            { 
                // filter them 
                bool itisrelevant = true;
                if(nullptr != fpISOK)
                {
                    itisrelevant = fpISOK(*id32);                   
                }
                
                if(true == itisrelevant)
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
    
    if(nullptr != numberofthem)
    {
        *numberofthem = eo_array_Size(id32ofregulars);
    }
    
    return true;   
}

#endif

bool embot::app::eth::theServices::Impl::synch(Service::Category category, Service::State state)
{
    auto index = embot::core::tointegral(category);
    if(index >= Service::numberOfCategories)
    {
        return false;
    }
    
    _mnservice->status.stateofservice[index] = embot::core::tointegral(state);
    Service *s = get(category);
    if(nullptr != s)
    {
        s->set(state);
    }
    
    return true;
}

//extern eOresult_t eo_service_hid_SynchServiceState(EOtheServices *p, eOmn_serv_category_t category, eOmn_serv_state_t servstate)
//{
//    if(NULL == p)
//    {
//        return(eores_NOK_nullpointer);
//    }   
//    
//    if((eomn_serv_category_none == category) || (eomn_serv_category_all == category) || (eomn_serv_category_unknown == category))
//    {
//        return(eores_NOK_generic);
//    }
//    
//    p->mnservice->status.stateofservice[category] = servstate;
//    
//    return(eores_OK);
//}


bool embot::app::eth::theServices::Impl::stop()
{
    for(auto &s : _mapfofservices) 
    { 
        if(nullptr != s)
        {
            s->stop();
            uint8_t num {0};
            s->setregulars(nullptr, num);
            s->deactivate();
        }            
    }
    
    for(auto &r : _running) { r = false; }
    
    return true;
}

bool embot::app::eth::theServices::Impl::tick()
{
    for(auto &s : _mapfofservices) 
    { 
        if(nullptr != s)
        {
            s->tick();
        }            
    }   
    return true;
}

bool embot::app::eth::theServices::Impl::sendresult(Service *s, const eOmn_service_cmmnds_command_t *command, eOmn_serv_state_t state, bool ok)
{
    const eOmn_serv_configuration_t *sc = &command->parameter.configuration;
    eOmn_serv_operation_t operation = (eOmn_serv_operation_t)command->operation;
    eOmn_serv_category_t category = static_cast<eOmn_serv_category_t>(command->category);
    eOmn_serv_type_t type = static_cast<eOmn_serv_type_t>(sc->type);    
        
    _mnservice->status.commandresult.category = category;
    _mnservice->status.commandresult.operation = operation;
    _mnservice->status.commandresult.type = type;   

    _mnservice->status.commandresult.latestcommandisok = ok ? eobool_true : eobool_false;    
    _mnservice->status.commandresult.data[0] = state;        
    

    eOropdescriptor_t ropdesc {};

    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
    ropdesc.ropcode = eo_ropcode_sig;
    ropdesc.id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_service, 0, eoprot_tag_mn_service_status_commandresult);
    ropdesc.data = nullptr; // so that data from the EOnv is retrieved (which is: p->mnservice->status.commandresult)          

    embot::app::eth::theHandler::getInstance().transmit(ropdesc);
                
    return true;
}

// - static

bool embot::app::eth::theServices::Impl::onendverifyactivate(Service *s, const eOmn_serv_configuration_t *sc, bool ok)
{
    embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
    
//    Service *ss = embot::app::eth::theServices::getInstance().get();
    // ok keeps the result of the verify
    if((false == ok) && (nullptr != s))
    {
        s->report();
        s->deactivate();
    }
    
    // now i must send back a rop w/ the result of the request
    // i need
    eOmn_serv_category_t category = static_cast<eOmn_serv_category_t>(s->category());
    eOmn_serv_type_t type = static_cast<eOmn_serv_type_t>(sc->type);    
        
//    Impl *impl = reinterpret_cast<Impl*>(caller);
    // mnservice can be retrieved w/ ...
    eOmn_service_t * mnservice = reinterpret_cast<eOmn_service_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_management, eoprot_entity_mn_service, 0));
    mnservice->status.commandresult.category = category;
    mnservice->status.commandresult.operation = eomn_serv_operation_verifyactivate;
    mnservice->status.commandresult.type = type;   

    mnservice->status.commandresult.latestcommandisok = ok ? eobool_true : eobool_false;    
    mnservice->status.commandresult.data[0] = ok ? eomn_serv_state_activated: eomn_serv_state_failureofverify;        
    
//    transmit_rop();  

    eOropdescriptor_t ropdesc {};

    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
    ropdesc.ropcode = eo_ropcode_sig;
    ropdesc.id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_service, 0, eoprot_tag_mn_service_status_commandresult);
    ropdesc.data = nullptr; // so that data from the EOnv is retrieved (which is: p->mnservice->status.commandresult)          

    embot::app::eth::theHandler::getInstance().transmit(ropdesc);
        
    embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"theServices::onendverifyactivate()", thr}, {}, ok ? "ok": "false");
                
    return true;
}

//static eOresult_t s_eo_services_process_failure(EOtheServices *p, eOmn_serv_operation_t operation, eOmn_serv_category_t category)
//{
//    p->mnservice->status.commandresult.latestcommandisok = eobool_false;
//    p->mnservice->status.commandresult.category = category;
//    p->mnservice->status.commandresult.operation = operation;
//    p->mnservice->status.commandresult.type = eomn_serv_NONE;

//    send_rop_command_result(); 

//    return(eores_OK);    
//}


//static void send_rop_command_result(void)
//{
//    eOropdescriptor_t ropdesc;

//    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
//    ropdesc.ropcode = eo_ropcode_sig;
//    ropdesc.id32    = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_service, 0, eoprot_tag_mn_service_status_commandresult);
//    ropdesc.data    = NULL; // so that data from the EOnv is retrieved (which is: p->mnservice->status.commandresult)          
//    
//    eom_emsappl_Transmit_OccasionalROP(eom_emsappl_GetHandle(), &ropdesc);
//}

// --------------------------------------------------------------------------------------------------------------------


embot::app::eth::theServices& embot::app::eth::theServices::getInstance()
{
    static theServices* p = new theServices();
    return *p;
}

embot::app::eth::theServices::theServices()
{
    pImpl = std::make_unique<Impl>();
}  
    
embot::app::eth::theServices::~theServices() { }
        
bool embot::app::eth::theServices::initialise(const Config &cfg)
{
    return pImpl->initialise(cfg);
}

bool embot::app::eth::theServices::load(embot::app::eth::Service *s)
{
    return pImpl->load(s);
}

embot::app::eth::Service* embot::app::eth::theServices::get(embot::app::eth::Service::Category cat)
{
    return pImpl->get(cat);
}

embot::app::eth::Service* embot::app::eth::theServices::get(const eOmn_service_cmmnds_command_t *command)
{
    return pImpl->get(command);
}

bool embot::app::eth::theServices::process(eOmn_service_cmmnds_command_t *command)
{
    return pImpl->process(command);
}

bool embot::app::eth::theServices::setregulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, fpIsID32relevant fpISOK, uint8_t* numberofthem)
{
    return pImpl->setregulars(id32ofregulars, arrayofid32, fpISOK, numberofthem);
}

bool embot::app::eth::theServices::synch(Service::Category category, Service::State state)
{
    return pImpl->synch(category, state);
}

bool embot::app::eth::theServices::stop()
{
    return pImpl->stop();
}

bool embot::app::eth::theServices::tick()
{
    return pImpl->tick();
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


