
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
    Config config {};  
    EOnvSet* nvset {nullptr};   
    eOmn_service_t *mnservice {nullptr};    

    Impl(); 
    
    bool initialise(const Config &cfg);    

    bool process(eOmn_service_cmmnds_command_t *command);
    embot::app::eth::Service* get(Service::Category cat); 

    static bool onendverifyactivate(void *caller, Service *s, eOmn_serv_configuration_t *sc, bool ok);
    
    bool setregulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, fpIsID32relevant fpISOK, uint8_t* numberofthem);
    bool synch(Service::Category category, Service::State state);
};

embot::app::eth::theServices::Impl::Impl()
{
}

bool embot::app::eth::theServices::Impl::initialise(const Config &cfg)
{
    config = cfg;    
    nvset = eom_emstransceiver_GetNVset(eom_emstransceiver_GetHandle()); 
    mnservice = reinterpret_cast<eOmn_service_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_management, eoprot_entity_mn_service, 0));
    
    return true;
} 


bool embot::app::eth::theServices::Impl::onendverifyactivate(void *caller, Service *s, eOmn_serv_configuration_t *sc, bool ok)
{
    if(false == ok)
    {
        s->report();
        s->deactivate();
    }
    
    // now i must send back a rop w/ the result of the request
    // i need
    eOmn_serv_category_t category {};
    eOmn_serv_type_t type {};    
        
    Impl *impl = reinterpret_cast<Impl*>(caller);
    // mnservice can be retrieved w/ ...
    // mnservice = reinterpret_cast<eOmn_service_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_management, eoprot_entity_mn_service, 0));
    impl->mnservice->status.commandresult.category = category;
    impl->mnservice->status.commandresult.operation = eomn_serv_operation_verifyactivate;
    impl->mnservice->status.commandresult.type = type;   

    impl->mnservice->status.commandresult.latestcommandisok = ok ? eobool_true : eobool_false;    
    impl->mnservice->status.commandresult.data[0] = ok ? eomn_serv_state_activated: eomn_serv_state_failureofverify;        
    
//    transmit_rop();  

    eOropdescriptor_t ropdesc {};

    memcpy(&ropdesc, &eok_ropdesc_basic, sizeof(eok_ropdesc_basic));
    ropdesc.ropcode = eo_ropcode_sig;
    ropdesc.id32 = eoprot_ID_get(eoprot_endpoint_management, eoprot_entity_mn_service, 0, eoprot_tag_mn_service_status_commandresult);
    ropdesc.data = nullptr; // so that data from the EOnv is retrieved (which is: p->mnservice->status.commandresult)          

    embot::app::eth::theHandler::getInstance().transmit(ropdesc);
        

    #warning caller may be removed ...        
    
    return true;
}


bool embot::app::eth::theServices::Impl::process(eOmn_service_cmmnds_command_t *command)
{
    if(nullptr == command)
    {
        return false;
    }
       
    eOmn_serv_operation_t operation = (eOmn_serv_operation_t)command->operation;
    eOmn_serv_category_t category = (eOmn_serv_category_t)command->category;
    const eOmn_serv_configuration_t *config = &command->parameter.configuration;
    eOmn_serv_arrayof_id32_t *arrayofid32 = &command->parameter.arrayofid32;
    
    embot::app::eth::Service::Category cat = static_cast<embot::app::eth::Service::Category>(command->category);
    
    // if category is one which has an associated object we proceed
    // else if it is category_all and we have stop or deactivate we go on   
    // else .... no good
    
    #warning terminate_all();

    embot::app::eth::Service *service = embot::app::eth::get(cat);
    if(nullptr == service)
    {   // category not supported or not available yet
        // send up ROP w/ failure 
        return false;
    }
    
    switch(operation)
    {
        case eomn_serv_operation_verifyactivate:
        {

            service->verify(config, true, onendverifyactivate, this);
//            s_eo_services_process_verifyactivate(category, config);            
        } break;
        
        case eomn_serv_operation_start:
        {
            // if not ok, send up ROP w/ failure
            // else: service->Start(), mark service started
            // send in RUN mode, 
            // send ROP w/ ok
            
//            service->Start();
 //           s_eo_services_process_start(category);
        } break;
        
        case eomn_serv_operation_stop:
        { 
            // if not ok, send up ROP w/ failure
            // else: service->Stop(), service->SetRegulars(0), if requested ->Deactivate(), unmark service started
            // if none is running send in IDLE mode, 
            // send ROP w/ ok            
//            s_eo_services_process_stop(category, eobool_true);
        } break;
        
        case eomn_serv_operation_deactivate:
        {           
//            s_eo_services_process_deactivate(category);
        } break;        
        
        case eomn_serv_operation_regsig_load:
        {          
//            s_eo_services_process_regsig(category, arrayofid32);
        } break;        

        case eomn_serv_operation_regsig_clear:
        {           
//            s_eo_services_process_regsig(category, NULL);
        } break;  
        
        default:
        {
//            s_eo_services_process_failure(operation, category);
        } break;
        
    }    
    
    return true;
}

embot::app::eth::Service* embot::app::eth::theServices::Impl::get(Service::Category cat)
{
    #warning TBD
    return nullptr;
   
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
    #warning TBD
    return true;
}

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

bool embot::app::eth::theServices::process(eOmn_service_cmmnds_command_t *command)
{
    return pImpl->process(command);
}
  
embot::app::eth::Service * embot::app::eth::theServices::get(Service::Category cat)
{
    return pImpl->get(cat);
}


bool embot::app::eth::theServices::setregulars(EOarray* id32ofregulars, eOmn_serv_arrayof_id32_t* arrayofid32, fpIsID32relevant fpISOK, uint8_t* numberofthem)
{
    return pImpl->setregulars(id32ofregulars, arrayofid32, fpISOK, numberofthem);
}

bool embot::app::eth::theServices::synch(Service::Category category, Service::State state)
{
    return pImpl->synch(category, state);
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


