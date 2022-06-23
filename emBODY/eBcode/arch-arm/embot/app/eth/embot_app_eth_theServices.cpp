
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
    _mnservice = reinterpret_cast<eOmn_service_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_management, eoprot_entity_mn_service, 0));
    
    for(auto &s : _mapfofservices) { s = nullptr; }
    for(auto &r : _running) { r = false; }
    
    return true;
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
    if(nullptr == command)
    {
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

    embot::app::eth::Service *service = embot::app::eth::get(cat);
    if(nullptr == service)
    {   // category not supported or not available yet
        #warning TODO: send up ROP w/ failure 
        sendresult(nullptr, command, eomn_serv_state_notsupported, false);
        return false;
    }
    
    eOmn_serv_state_t servstate = static_cast<eOmn_serv_state_t>(_mnservice->status.stateofservice[category]); 
    
    switch(operation)
    {
        case eomn_serv_operation_verifyactivate:
        {
            service->verify(config, true, onendverifyactivate);
            // we dont send any result. the callback onendverifyactivate() will do it.
            // for reference it was: s_eo_services_process_verifyactivate(category, config);            
        } break;
        
        case eomn_serv_operation_start:
        {
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
            uint8_t num {0};
            service->setregulars(nullptr, num);
            servstate = static_cast<eOmn_serv_state_t>(_mnservice->status.stateofservice[category]);            
            sendresult(service, command, servstate, true);            
            // for reference: s_eo_services_process_regsig(category, NULL);
        } break;  
        
        default:
        {
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
                eObool_t itisrelevant = eobool_true;
                if(nullptr != fpISOK)
                {
                    itisrelevant = fpISOK(*id32);                   
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
    
    if(nullptr != numberofthem)
    {
        *numberofthem = eo_array_Size(id32ofregulars);
    }
    
    return true;   
}

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


