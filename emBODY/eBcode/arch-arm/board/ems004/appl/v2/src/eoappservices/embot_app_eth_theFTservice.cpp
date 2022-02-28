
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theFTservice.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <array>

#include "EoProtocolAS.h"
#include "EOtheServices_hid.h"
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EOVtheCallbackManager.h"
#include "EOtheCANservice.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

namespace embot::app::eth {
    
    
constexpr eOservice_core_t dummy_service_core 
{
    .initted = eobool_false,
    .active = eobool_false,
    .activateafterverify = eobool_false,
    .started = eobool_false,
    .onverify = nullptr,
    .state = eomn_serv_state_notsupported,
    .tmpcfg = nullptr,
    .servconfig = { .type = eomn_serv_NONE }
};    

constexpr eOservice_diagnostics_t dummy_service_diagnostics
{
    .reportTimer = nullptr,
    .reportPeriod = 0, // 10*EOK_reltime1sec, // with 0 we dont periodically report
    .errorDescriptor = {0},
    .errorType = eo_errortype_info,
    .errorCallbackCount =  0, 
    .repetitionOKcase =  0 // 10 // with 0 we transmit report only once at succesful activation    
};

constexpr eOservice_cantools_t dummy_service_cantools
{
    .boardproperties = nullptr,
    .entitydescriptor = nullptr,
    .discoverytargets = nullptr,
    .ondiscoverystop = {0},
    .command = {0}, 
};

struct theFSdiscovery
{
    struct Item
    {
        eOprotIndex_t index {0};
        eObrd_canlocation_t loc {};
        eOas_ft_t *ft {nullptr};            
    };
    std::vector<Item> items;
    uint8_t rxfullscales {0};
    uint8_t maxfullscales {0};
    bool querying {false};
    uint8_t fsindex {0};
    embot::core::Callback ONok {};
    embot::core::Callback ONko {};
    embot::core::relTime timeout {50*embot::core::time1millisec};
    static constexpr size_t maxboards {theFTservice::maxSensors};
    theFSdiscovery() = default;
    
    void initialise(const embot::core::Callback &onOK, const embot::core::Callback &onTOUT, 
                    embot::core::relTime tout = 50*embot::core::time1millisec)
    {
        items.reserve(maxboards);
        ONok = onOK;
        ONko = onTOUT;
        timeout = tout;
    }
    
    void reset()
    {
        items.clear();
        maxfullscales = rxfullscales = 0;
        fsindex = 0;
        querying = false;
    }
    
    void add(const Item &item)
    {
        items.push_back(item);
    }
    
    void start()
    {
        // init timer w/ timeout (i will do it ...)
        
        // set the number of fullscales i will receive
        rxfullscales = 0;
        maxfullscales = 6 * items.size();
    
        querying = true;
        // send the first burst
        fsindex = 0;
        askfullscales();
    }
    
    void askfullscales()
    {
        for(uint8_t i=0; i<items.size(); i++)
        {
            eOprotIndex_t protindex = items[i].index;
            eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, protindex, eoprot_tag_none);
            // for each entity i ask all of four in a row. 
            // note that in case we have a strain2 i could just send one single message w/ channel = 0x0f
            for(uint8_t c=0; c<6; c++)
            {
                // send a message
                eOcanprot_command_t command {};
                uint8_t channel = c;
                command.clas = eocanprot_msgclass_pollingAnalogSensor;
                command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
                command.value = &channel;

                eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);                                
            }
        }
    }
        
    
    bool tick(eOprotIndex_t index, eOas_ft_t *ft, uint8_t chnl, uint16_t fsvalue)
    {
        if(false == querying)
        {
            return false;
        }
        
        ft->status.fullscale[chnl] = fsvalue;
        rxfullscales++;
        if(rxfullscales == maxfullscales)
        {
            querying = false;
            ONok.execute();
        }
        
        return true;
    }        
};

struct embot::app::eth::theFTservice::Impl
{
    Config config {};
    bool initted {false};
    
    // data structures used in the embobj implementation
    // we can surely reuse them
    eOservice_core_t service {dummy_service_core};    
    eOservice_diagnostics_t diagnostics {dummy_service_diagnostics};
    eOservice_cantools_t sharedcan {dummy_service_cantools};   
    EOarray* id32ofregulars {nullptr};    
    
    constexpr static const char s_eobj_ownname[] = "theFTservice";

    const eOmn_serv_configuration_t * tmpservcfg {nullptr};   
    // theFTnetvariables[i] holds pointer to the network variable of the whole eOas_ft_t w/ i index
    // BEWARE: or nullptr if the protocol configure less than maxSensors ft sensors. see xxx    
    std::array<eOas_ft_t*, maxSensors> theFTnetvariables {nullptr, nullptr, nullptr, nullptr};    
    
    theFSdiscovery theFSdiscodance {};
    
    // methods
    
    Impl() = default;      
    
    // done
    bool initialise();
    
    // done
    bool SendReport();
      
    // done BUT .... must do request of fullscales before we call onverify()
    // the idea is to have a struct FSquery w/ .start(cfg), processcanframe(),
    // success(), failure() plus a timer w/ a timeout of 100 ms
    // now we have the fake .... getfullscale_start() etc.    
    eOresult_t Verify(const eOmn_serv_configuration_t * servcfg, 
                eOservice_onendofoperation_fun_t onverify, 
                bool activateafterverify);
    // done
    eOresult_t Activate(const eOmn_serv_configuration_t * servcfg);
    
    // done
    eOresult_t Deactivate();  

    // done
    eOmn_serv_state_t GetServiceState() const;   
    
    // done
    eOresult_t SetRegulars(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem); 
    
    // done
    eOresult_t Start(); 
    
    // done
    eOresult_t Stop();
    
    // done
    eOresult_t Tick(); 
       
    
    eOresult_t AcceptCANframe(const canFrameDescriptor &canframedescriptor);
       
    
    bool process(const EOnv* nv, const eOropdescriptor_t* rd);
    
    // service functions
    
    void getfullscale_start();
    void getfullscale_continue();
    void getfullscale_stop();
    void getfullscale_failure();
        
    void s_eo_strain_SendTXmode();
    void s_eo_strain_TXstop();
    void s_ft_saturation_handler(const canFrameDescriptor &cfd, eOprotIndex_t index);
    
    // they must be static
    static eOresult_t onstop_search_for_ft_boards(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok);
    static eOresult_t onstop_search_for_ft_boards_we_get_fullscale(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok);
    static void send_periodic_error_report(void *par);    
    static constexpr eObool_t ft_isID32relevant(uint32_t id32);
    
    static void onFSdiscoveryOK(void *par);

};

//embot::app::eth::theFTservice::Impl::Impl()
//{
//    // we need it because ... we dont have default values for the C structs and better doing  
//}
      
bool embot::app::eth::theFTservice::Impl::initialise()
{
    if(true == initted)
    {
        return true;
    }
    
    embot::core::print("embot::app::eth::theFTservice::Impl::initialise()");
    
    service.servconfig.type = eomn_serv_NONE;
    sharedcan.boardproperties = eo_vector_New(sizeof(eObrd_canproperties_t), 1, nullptr, 0, nullptr, nullptr);
    sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), 1, nullptr, 0, nullptr, nullptr);
    sharedcan.discoverytargets = eo_array_New(1, sizeof(eOcandiscovery_target_t),  nullptr);
    id32ofregulars = eo_array_New(embot::app::eth::theFTservice::maxRegulars, sizeof(uint32_t), nullptr);
    
    diagnostics.reportTimer = eo_timer_New();
    diagnostics.errorType = eo_errortype_error;
    diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_ft_not_verified_yet);      
        
    service.initted = eobool_true;
    service.active = eobool_false;
    service.started = eobool_false;    
    service.state = eomn_serv_state_idle;  
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);   


    // build pointers to the eOas_ft_t used by protocol: theFTnetvariables    
    // in embobj standard they are managed by EOtheEntities, but ... if they have local scope i prefer to do it in here 
//    for(uint8_t i = 0; i<theFTnetvariables.size(); i++)
//    {
//        void *vpft = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_ft, static_cast<eOprotIndex_t>(i));
//        theFTnetvariables[i] = reinterpret_cast<eOas_ft_t*>(vpft);
//    }
    for(uint8_t i = 0; i<theFTnetvariables.size(); i++)
    {
        theFTnetvariables[i] = nullptr;
    }
    
    theFSdiscodance.initialise({}, {});
    
    initted = true;
    return initted;
}


eOresult_t embot::app::eth::theFTservice::Impl::Tick()
{
    if(eobool_false == service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    } 

    if(eobool_false == service.started)
    {   // not running, thus we do nothing
        return(eores_OK);
    }     

// add in here a check vs correct arrival of can frames.
              
    return eores_OK;
}


bool embot::app::eth::theFTservice::Impl::SendReport()
{
    eo_errman_Error(eo_errman_GetHandle(), diagnostics.errorType, NULL, s_eobj_ownname, &diagnostics.errorDescriptor);
            
    eOerror_value_t errorvalue = eoerror_code2value(diagnostics.errorDescriptor.code);
    
    switch(errorvalue)
    {
        case eoerror_value_CFG_ft_failed_candiscovery:
        {
            eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());            
        } break;
        
        default:
        {
            // dont send any additional info
        } break;
    }          
        
    return true;
}

eOresult_t embot::app::eth::theFTservice::Impl::Verify(const eOmn_serv_configuration_t * servcfg, 
                eOservice_onendofoperation_fun_t onverify, 
                bool activateafterverify)
{
    
    if(NULL == servcfg)
    {
        service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);
        if(NULL != onverify)
        {
            #warning USE a handle to the object w/ methods .... 
            onverify(nullptr, eobool_false); 
        }                 
        return(eores_NOK_nullpointer);
    }  
 
    if(eomn_serv_AS_ft != servcfg->type)
    {
        service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);
        if(NULL != onverify)
        {
            onverify(nullptr, eobool_false); 
        }          
        return(eores_NOK_generic);
    } 
     
    if(eobool_true == service.active)
    {
        Deactivate();        
    }  

    service.state = eomn_serv_state_verifying;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);

    // make sure the timer is not running
    eo_timer_Stop(diagnostics.reportTimer);  
    
    service.onverify = onverify;
    service.activateafterverify = activateafterverify;
    
    
    
#if 0    

    eOcandiscovery_target_t trgt = {0};

    #warning for now I assume that we have only one board type ... 
    // in case we really have n board types, have a look at eo_inertials3_Verify()
    // basically: we must push multiple times eo_array_PushBack(sharedcan.discoverytargets, &trgt); 
    // w/ a trgt holding one type only of board.
    
    EOconstarray *carray = eo_constarray_Load(reinterpret_cast<const EOarray*>(&servcfg->data.as.ft.arrayofsensors));
   
    const eOas_ft_sensordescriptor_t *sd = reinterpret_cast<const eOas_ft_sensordescriptor_t*>(eo_constarray_At(carray, 0));
    trgt.info.type = sd->boardinfo.type;
    trgt.info.protocol = sd->boardinfo.protocol;
    trgt.info.firmware = sd->boardinfo.firmware;  
    trgt.canmap[sd->canloc.port] = 0x0001 << sd->canloc.addr; 
    
    // force a cleaned discoverytargets before we add the target
    eo_array_Reset(sharedcan.discoverytargets);
    eo_array_PushBack(sharedcan.discoverytargets, &trgt);
    
    tmpservcfg = servcfg;
    sharedcan.ondiscoverystop.function = onstop_search_for_ft_boards;
    sharedcan.ondiscoverystop.parameter = (void*) this; //(void*)servcfg;
    
    // start discovery
    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), sharedcan.discoverytargets, &sharedcan.ondiscoverystop);   
    
#else
    // from EOtheInertials3
    
    // now we deal with sensors on can:
    // step-1: check vs max number of can boards in config
    // step-2: do discovery of all boards
    
    
    // get the array of sensors
    EOconstarray *carray = eo_constarray_Load(reinterpret_cast<const EOarray*>(&servcfg->data.as.ft.arrayofsensors));
    
    // the number of sensor descriptors
    uint8_t numofsensors = eo_constarray_Size(carray);
    
    if(0 == numofsensors)
    {
        // no sensors in descriptor ....
        service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);
        if(NULL != onverify)
        {
            onverify(nullptr, eobool_false); 
        }          
        return(eores_NOK_generic);
    }
    
    // force a cleaned discoverytargets before we add the target
    eo_array_Reset(sharedcan.discoverytargets);

    // we go through all types of ft sensor boards to group the boards by type
    for(uint8_t b=0; b<eoas_ft_supportedboards_numberof(); b++)
    {
        eOcandiscovery_target_t trgt = {0};
        eObool_t found = eobool_false;
        eObrd_cantype_t brdtype = eoas_ft_supportedboards_gettype(b);
        trgt.info.type = eobrd_cantype_none;
        trgt.canmap[eOcanport1] = trgt.canmap[eOcanport2] = 0;
        // we go through inside the carray to find every sensor board whose type is = brdtype
        for(uint8_t s=0; s<numofsensors; s++)
        {
            const eOas_ft_sensordescriptor_t *sd = reinterpret_cast<const eOas_ft_sensordescriptor_t*>(eo_constarray_At(carray, s));
            if(sd->boardinfo.type == brdtype)
            {
                // ok, we have one.
                found = eobool_true;
                // i add boardinfo (type, protocol, firmware) and assume that the whole boardinfos stay the same for all
                // boards of the same type. in any case the last one wins
                trgt.info = sd->boardinfo;
                // i add the location
                eo_common_hlfword_bitset(&trgt.canmap[sd->canloc.port], sd->canloc.addr);                 
            }            
        }

        if(eobool_true == found)
        {   // we have found a valid discovery target for this kind of board. we add it to the array
            eo_array_PushBack(sharedcan.discoverytargets, &trgt);
        } 
        
    }

    if(0 == eo_array_Size(sharedcan.discoverytargets))
    {
        // the service configuration does not contain any board valid to offer ft service 
        service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);
        if(NULL != onverify)
        {
            onverify(nullptr, eobool_false); 
        }          
        return(eores_NOK_generic);
    }
    
    // ok, we launch discovery for all the boards 
    
    // init params
    tmpservcfg = servcfg;
    sharedcan.ondiscoverystop.function = onstop_search_for_ft_boards_we_get_fullscale; // onstop_search_for_ft_boards;
    sharedcan.ondiscoverystop.parameter = (void*) this; 
    
    // start discovery
    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), sharedcan.discoverytargets, &sharedcan.ondiscoverystop);    

#endif
    
    return eores_OK;
}

eOresult_t embot::app::eth::theFTservice::Impl::Activate(const eOmn_serv_configuration_t * servcfg)
{
    if(NULL == servcfg)
    {
        return(eores_NOK_nullpointer);
    }    

    if(eomn_serv_AS_ft != servcfg->type)
    {
        return(eores_NOK_generic);
    }  
    
    if(eobool_true == service.active)
    {
        Deactivate();        
    }   
    
    // we dont use EOtheEntities but .... theFTnetvariables
            
    memcpy(&service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));
                
    // now... use the servcfg    
    EOconstarray *carray = eo_constarray_Load(reinterpret_cast<const EOarray*>(&servcfg->data.as.ft.arrayofsensors));    
    uint8_t numofsensors = eo_constarray_Size(carray);
    
    // marco.accame on 25 feb 2022: for now i dont do any more verification on the sanity of the servconfig.
    // we already did it in Verify()
    
    // we must load all the boards into can mapping    
    for(uint8_t s=0; s<numofsensors; s++)
    {
        const eOas_ft_sensordescriptor_t *sd = reinterpret_cast<const eOas_ft_sensordescriptor_t*>(eo_constarray_At(carray, s));
        if((nullptr != sd) && (eobool_true == eoas_ft_isboardvalid(static_cast<eObrd_cantype_t>(sd->boardinfo.type))))
        {
            eObrd_canproperties_t prop = {0};
            prop.type = sd->boardinfo.type;
            prop.location.port = sd->canloc.port; 
            prop.location.addr = sd->canloc.addr; 
            prop.location.insideindex = eobrd_caninsideindex_none;
            prop.requiredprotocol = sd->boardinfo.protocol; 
           
            eo_vector_PushBack(sharedcan.boardproperties, &prop);                      
        }                
    }               
    eo_canmap_LoadBoards(eo_canmap_GetHandle(), sharedcan.boardproperties);  

    // load the entities: one sd is one index of entity ft. also prepare the memory
    for(uint8_t s=0; s<numofsensors; s++)
    {
        const eOas_ft_sensordescriptor_t *sd = reinterpret_cast<const eOas_ft_sensordescriptor_t*>(eo_constarray_At(carray, s));
        if((nullptr != sd) && (eobool_true == eoas_ft_isboardvalid(static_cast<eObrd_cantype_t>(sd->boardinfo.type))))
        {
            eOcanmap_entitydescriptor_t des = {0};
            des.location.port = sd->canloc.port;
            des.location.addr = sd->canloc.addr;
            des.location.insideindex = eobrd_caninsideindex_none;
            des.index = static_cast<eOcanmap_entityindex_t>(s);  
            eo_vector_PushBack(sharedcan.entitydescriptor, &des);   

            void *vpft = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_ft, static_cast<eOprotIndex_t>(s));
            theFTnetvariables[s] = reinterpret_cast<eOas_ft_t*>(vpft);            
        }                
    }
    eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_ft, sharedcan.entitydescriptor);
    
    
    // ok, service is active now

    service.active = eobool_true;
    service.state = eomn_serv_state_activated; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);        

    return(eores_OK); 
}


eOresult_t embot::app::eth::theFTservice::Impl::Deactivate()
{
    if(eobool_false == service.active)
    {
        // i force to eomn_serv_state_idle because it may be that state was eomn_serv_state_verified or eomn_serv_state_failureofverify
        service.state = eomn_serv_state_idle; 
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);
        return(eores_OK);        
    } 
    
    // send stop messages to the ft sensor board, unload the entity-can-mapping and the board-can-mapping, reset all things inside this object

    if(eobool_true == service.started)
    {
        Stop();
    }
    
    SetRegulars(nullptr, nullptr);
    
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_ft, sharedcan.entitydescriptor); 
    
    // clear the ...
    for(uint8_t i = 0; i<theFTnetvariables.size(); i++)
    {
        theFTnetvariables[i] = nullptr;
    }
    
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), sharedcan.boardproperties);
         
    memset(&service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    service.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(sharedcan.boardproperties);
    eo_vector_Clear(sharedcan.entitydescriptor);
    eo_array_Reset(sharedcan.discoverytargets);
    
    // make sure the timer is not running
    eo_timer_Stop(diagnostics.reportTimer);    
    
    
    service.active = eobool_false;    
    service.state = eomn_serv_state_idle; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);
    
    return(eores_OK);
}

   
eOresult_t embot::app::eth::theFTservice::Impl::Start()
{
    if(eobool_false == service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    if(eobool_true == service.started)
    {   // it is already running
        return(eores_OK);
    }   
    
    service.started = eobool_true;    
    service.state = eomn_serv_state_started; 
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);
    
    // start() does not force the tx of the board. 
    // this function just config the strain tx mode according to values in ft.config, whose initial values are asfidanken
    // unless someone has changed the strain.config with eth protocol messages 
    s_eo_strain_SendTXmode();   
          
    return eores_OK;
}
 
eOresult_t embot::app::eth::theFTservice::Impl::Stop()
{
     
    if(eobool_false == service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    if(eobool_false == service.started)
    {   // it is already stopped
        return(eores_OK);
    }  
    
    s_eo_strain_TXstop();   

    service.started = eobool_false;
    service.state = eomn_serv_state_activated;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);    
    
    // remove all regulars related to ft entity ... no, dont do that.
    //SetRegulars(NULL, NULL);  
    
    return eores_OK;
}

eOresult_t embot::app::eth::theFTservice::Impl::SetRegulars(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    if(eobool_false == service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    return(eo_service_hid_SetRegulars(id32ofregulars, arrayofid32, ft_isID32relevant, numberofthem));
}
    

eOresult_t embot::app::eth::theFTservice::Impl::AcceptCANframe(const canFrameDescriptor &cfd)
{
    
    if(eobool_false == service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }   
    
    // 1. get a loc,
    // depending on cfd.type ....
    // fullscale
    // force
    // torque

    eObrd_canlocation_t loc = {};
    loc.port = cfd.port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(cfd.frame);
    loc.insideindex = eobrd_caninsideindex_first;

    eOprotIndex_t index = eo_canmap_GetEntityIndex(eo_canmap_GetHandle(), loc, eoprot_endpoint_analogsensors, eoprot_entity_as_ft); 
    if(EOK_uint08dummy == index)
    {
        return eores_NOK_generic;
    }
    
    eOas_ft_t *ft = theFTnetvariables[index];
    
    
    switch(cfd.type)
    {
        case canFrameDescriptor::Type::force:
        case canFrameDescriptor::Type::torque:
        {
            uint8_t offset = (canFrameDescriptor::Type::force == cfd.type) ? 0 : 3;
            
            #warning verifica che il fullscale sia a posto come little endianess ....
            for(uint8_t i=0; i<3; i++)
            {
                float32_t vv = (static_cast<uint16_t>(cfd.frame->data[2*i+1]) << 8) + static_cast<uint16_t>(cfd.frame->data[2*i]) - static_cast<uint16_t>(0x8000);
                if(eoas_ft_mode_calibrated == ft->config.mode) 
                {
                    vv = vv * static_cast<float32_t>(ft->status.fullscale[i+offset]) / static_cast<float32_t>(0x8000);
                }
                ft->status.timedvalue.values[i+offset] = vv;
            }
            
            s_ft_saturation_handler(cfd, index);

            
            
           
            // ora: vedi se e' modo calibrato oppure no e copia con o senza transpfrmazione
            // ma nella entity corretta.
            #warning add saturation handler ...
//                       eo_array_Assign((EOarray*)(&strain->status.calibratedvalues), (processForce == mode) ? 0 : 3, &(frame->data[0]), 3);

            
        } break;   

        case canFrameDescriptor::Type::fullscale:
        {
            
            #warning fullscaleEngine.tick(index, ft, cfd.frame);
            
        } break;  

        default:
        {
            
        } break;        
    }

    
    return eores_OK;
}

void embot::app::eth::theFTservice::Impl::s_ft_saturation_handler(const canFrameDescriptor &cfd, eOprotIndex_t index)
{
    static uint16_t upper_saturations[maxSensors][6] = {0};
    static uint16_t lower_saturations[maxSensors][6] = {0};
    
    static uint32_t counter[maxSensors] = {0};
    
    eOas_ft_t *ft = theFTnetvariables[index];
    
    counter[index] ++;
    
    uint8_t ftTXrate = ft->config.ftdatarate; // in ms
    uint32_t numberofmessagesin1second = 2000*ftTXrate; // ft sends 2 msgs (1 for force and 1 for torque) every ftTXrate milli.
    

    if(7 == cfd.frame->size)
    {
        uint8_t info = cfd.frame->data[6]; // byte containing info about saturation
    
        if(0 != info)
        {
            switch (cfd.type)
            {
                case canFrameDescriptor::Type::force:
                {
                    icubCanProto_strain_forceSaturationInfo_t* force_info = (icubCanProto_strain_forceSaturationInfo_t*) &info; 
                    
                    if (force_info->saturationInChannel_0 == saturationLOW)
                        lower_saturations[index][0]++;
                    else if (force_info->saturationInChannel_0 == saturationHIGH)
                        upper_saturations[index][0]++;
            
                    if (force_info->saturationInChannel_1 == saturationLOW)
                        lower_saturations[index][1]++;
                    else if (force_info->saturationInChannel_1 == saturationHIGH)
                        upper_saturations[index][1]++;
                   
                    if (force_info->saturationInChannel_2 == saturationLOW)
                         lower_saturations[index][2]++;
                    else if (force_info->saturationInChannel_2 == saturationHIGH)
                         upper_saturations[index][2]++;            
                } break;   
                
                case canFrameDescriptor::Type::torque:
                {
                    icubCanProto_strain_torqueSaturationInfo_t* torque_info = (icubCanProto_strain_torqueSaturationInfo_t*) &info;
                 
                    if (torque_info->saturationInChannel_3 == saturationLOW)
                        lower_saturations[index][3]++;
                    else if (torque_info->saturationInChannel_3 == saturationHIGH)
                        upper_saturations[index][3]++;
                    
                    if (torque_info->saturationInChannel_4 == saturationLOW)
                        lower_saturations[index][4]++;
                    else if (torque_info->saturationInChannel_4 == saturationHIGH)
                        upper_saturations[index][4]++;
                    
                    if (torque_info->saturationInChannel_5 == saturationLOW)
                        lower_saturations[index][5]++;
                    else if (torque_info->saturationInChannel_5 == saturationHIGH)
                        upper_saturations[index][5]++;
                } break;  
                
                default:
                {                   
                } break;                
            }
            

        }
        else
        {
            // send diag message about malformed message
            // uncomment if you need it for debugging
            /*
            eOerrmanDescriptor_t errdes = {0};
            errdes.sourcedevice         = (eOcanport1 == port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
            errdes.sourceaddress        = EOCANPROT_FRAME_GET_SOURCE(frame);                
            errdes.code                 = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag01);
            errdes.par16                = 0;
            errdes.par64                = 0;
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, "strain saturation byte 7 (if sent) should be different from 0!", NULL, &errdes);
            */
        }    
    
    }
    
 
    if((numberofmessagesin1second > 0) && (counter[index] >= numberofmessagesin1second))
    {   // ok, 1 second has expired (or better: so many messages have arrived for 1 second). 
            
        // reset counter    
        counter[index] = 0;
            
        // send saturation message for every channel, if any
        for(uint8_t i = 0; i < 6; i++)
        {
            eOerrmanDescriptor_t errdes = {0};
            if (upper_saturations[index][i] != 0 || lower_saturations[index][i] != 0)
            {
                errdes.sourcedevice         = (eOcanport1 == cfd.port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
                errdes.sourceaddress        = EOCANPROT_FRAME_GET_SOURCE(cfd.frame);                
                errdes.code                 = eoerror_code_get(eoerror_category_HardWare, eoerror_value_HW_strain_saturation);
                errdes.par16                = i; //channel involved
                errdes.par64                = (uint64_t) (upper_saturations[index][i]) << 32 | (uint64_t) lower_saturations[index][i]; //LSW->lower_sat, MSW->upper_sat
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &errdes);
                
                upper_saturations[index][i] = 0;
                lower_saturations[index][i] = 0;
            }
         }                     
    }    
}


eOmn_serv_state_t embot::app::eth::theFTservice::Impl::GetServiceState() const
{
    return service.state;
}

bool embot::app::eth::theFTservice::Impl::process(const EOnv* nv, const eOropdescriptor_t* rd)
{
    // it can be ... 
    // at first get index and tag.
    eOprotIndex_t index = eoprot_ID2index(rd->id32);
    eOprot_tag_as_ft_t tag = static_cast<eOprot_tag_as_ft_t>(eoprot_ID2tag(rd->id32));
    
    switch(tag)
    {
        case eoprot_tag_as_ft_config: 
        {
            // in here we need to get the configuration and ...
            eOas_ft_config_t *cfg = reinterpret_cast<eOas_ft_config_t*>(rd->data);
            // we use it for index 0 or 1 or 2 or 3
            #warning FILL IT
            
        } break;
        
        case eoprot_tag_as_ft_cmmnds_enable:
        {
            // in here we either start or stop ....
            // eOas_ft_commands_t::enable
            uint8_t enable = *reinterpret_cast<uint8_t*>(rd->data);
            #warning FILL IT
        } break;
        
        default:
        {
            
        } break;
        
    }
        
    
    return true;
}



eOresult_t embot::app::eth::theFTservice::Impl::onstop_search_for_ft_boards(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{   
    embot::app::eth::theFTservice::Impl *p = reinterpret_cast<embot::app::eth::theFTservice::Impl*>(par);
    const eOmn_serv_configuration_t * servcfg = p->tmpservcfg;

    if(eobool_true == searchisok)
    {
        p->service.state = eomn_serv_state_verified;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, p->service.state);
    }
    else
    {   
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, p->service.state);
    }
    
    if((eobool_true == searchisok) && (eobool_true == p->service.activateafterverify))
    {
        p->Activate(servcfg);        
    }
    
    p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.sourceaddress    = 0;
    p->diagnostics.errorDescriptor.par16            = 0;
    p->diagnostics.errorDescriptor.par64            = 0;    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
        
    if(eobool_true == searchisok)
    {        
        p->diagnostics.errorType = eo_errortype_debug;
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_ft_ok);
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if((0 != p->diagnostics.repetitionOKcase) && (0 != p->diagnostics.reportPeriod))
        {
            p->diagnostics.errorCallbackCount = p->diagnostics.repetitionOKcase;        
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }
    
    if(eobool_false == searchisok)
    {   
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_ft_failed_candiscovery);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
    }    
    
    if(NULL != p->service.onverify)
    {
        p->service.onverify(nullptr, searchisok); 
    }    
    
    return(eores_OK);   
}

eOresult_t embot::app::eth::theFTservice::Impl::onstop_search_for_ft_boards_we_get_fullscale(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{   
    embot::app::eth::theFTservice::Impl *p = reinterpret_cast<embot::app::eth::theFTservice::Impl*>(par);
    const eOmn_serv_configuration_t * servcfg = p->tmpservcfg;
    

    if(eobool_true == searchisok)
    {
        p->getfullscale_start();
        
        p->theFSdiscodance.reset();
        p->theFSdiscodance.add({});
        p->theFSdiscodance.start();    
    }
    else
    {   
        p->service.state = eomn_serv_state_failureofverify;
        eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, p->service.state);

        p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
        p->diagnostics.errorDescriptor.sourceaddress    = 0;
        p->diagnostics.errorDescriptor.par16            = 0;
        p->diagnostics.errorDescriptor.par64            = 0;    
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
        
        // fill error description and transmit it
        p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_ft_failed_candiscovery);
        p->diagnostics.errorType = eo_errortype_error;                
        eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
        
        if(0 != p->diagnostics.reportPeriod)
        {
            p->diagnostics.errorCallbackCount = EOK_int08dummy;
            eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
        }
     
        // call onverify
        if(NULL != p->service.onverify)
        {
            p->service.onverify(p, eobool_false); 
        }
        
    }
    
    return(eores_OK);   
}


void embot::app::eth::theFTservice::Impl::send_periodic_error_report(void *par)
{    
    embot::app::eth::theFTservice::Impl *p = reinterpret_cast<embot::app::eth::theFTservice::Impl*>(par);
    
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    if(eoerror_value_CFG_ft_failed_candiscovery == eoerror_code2value(p->diagnostics.errorDescriptor.code))
    {   // if i dont find the can boards, i keep on sending the discovery results up. it is a temporary diagnostics tricks until we use the verification of services at bootstrap
        eo_candiscovery2_SendLatestSearchResults(eo_candiscovery2_GetHandle());
    }
    
    if(EOK_int08dummy != p->diagnostics.errorCallbackCount)
    {
        p->diagnostics.errorCallbackCount--;
    }
    if(0 == p->diagnostics.errorCallbackCount)
    {
        eo_timer_Stop(p->diagnostics.reportTimer);
    }
}

constexpr eObool_t embot::app::eth::theFTservice::Impl::ft_isID32relevant(uint32_t id32)
{
    constexpr uint32_t mask0 = (((uint32_t)eoprot_endpoint_analogsensors) << 24) | (((uint32_t)eoprot_entity_as_ft) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(eobool_true);
    }
    
    return(eobool_false); 
}

void embot::app::eth::theFTservice::Impl::s_eo_strain_SendTXmode()
{        
    // we send the relevant can message to all the boards. remember: they are either strain or strain2 so have the same commands
     
    // set txmode
    sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;    
    sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    
    for(uint8_t protindex = 0; protindex<theFTnetvariables.size(); protindex++)
    {
        if(nullptr != theFTnetvariables[protindex])
        {
            // it means that we have the entity index activated
            // we get the mode from the entity ram and we send the command
            eOas_ft_mode_t ftmode = static_cast<eOas_ft_mode_t>(theFTnetvariables[protindex]->config.mode);
            eOas_strainmode_t strainmode = (eoas_ft_mode_raw == ftmode) ? eoas_strainmode_txuncalibrateddatacontinuously : eoas_strainmode_txcalibrateddatacontinuously;
            sharedcan.command.value = &strainmode;
            eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, protindex, eoprot_tag_none);
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &sharedcan.command, id32);
        }
    }
}

void embot::app::eth::theFTservice::Impl::s_eo_strain_TXstop()
{             
    // set txmode
    eOenum08_t mode = eoas_strainmode_acquirebutdonttx; 
    sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;    
    sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    sharedcan.command.value = &mode;
    
    for(uint8_t protindex = 0; protindex<theFTnetvariables.size(); protindex++)
    {
        if(nullptr != theFTnetvariables[protindex])
        {
            // it means that we have the entity index activated
            // we get the mode from the entity ram and we send the command
            eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, protindex, eoprot_tag_none);
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &sharedcan.command, id32);
        }
    }
}

void embot::app::eth::theFTservice::Impl::onFSdiscoveryOK(void *par)
{
    embot::app::eth::theFTservice::Impl *p = reinterpret_cast<embot::app::eth::theFTservice::Impl*>(par);
    
    const eOmn_serv_configuration_t * servcfg = p->tmpservcfg;

    p->service.state = eomn_serv_state_verified;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, p->service.state);
   
    if(eobool_true == p->service.activateafterverify)
    {
        p->Activate(servcfg);        
    }
    
    p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.sourceaddress    = 0;
    p->diagnostics.errorDescriptor.par16            = 0;
    p->diagnostics.errorDescriptor.par64            = 0;    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
              
    p->diagnostics.errorType = eo_errortype_debug;
    p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_ft_ok);
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    if((0 != p->diagnostics.repetitionOKcase) && (0 != p->diagnostics.reportPeriod))
    {
        p->diagnostics.errorCallbackCount = p->diagnostics.repetitionOKcase;        
        eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
    }
         
    if(NULL != p->service.onverify)
    {
        p->service.onverify(nullptr, eobool_true); 
    }     
    
}


void embot::app::eth::theFTservice::Impl::getfullscale_start()
{
    // it must .... start a request to the first ft sensor board. when everything is over, we call onverify();
    
    
    // fake ....  for now i just assume we dont ask the fullscale.
    // we just set them to be = 10.

    for(uint8_t protindex = 0; protindex<theFTnetvariables.size(); protindex++)
    {
        if(nullptr != theFTnetvariables[protindex])
        {
            for(uint8_t x=0; x<eoas_ft_6axis; x++)
            {
                theFTnetvariables[protindex]->status.fullscale[x] = 10;
            }
        }

    }    
    
    getfullscale_stop();
    
}

void embot::app::eth::theFTservice::Impl::getfullscale_continue()
{
    
}

void embot::app::eth::theFTservice::Impl::getfullscale_stop()
{
    const eOmn_serv_configuration_t * servcfg = tmpservcfg;

    service.state = eomn_serv_state_verified;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);
   
    if(eobool_true == service.activateafterverify)
    {
        Activate(servcfg);        
    }
    
    diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    diagnostics.errorDescriptor.sourceaddress    = 0;
    diagnostics.errorDescriptor.par16            = 0;
    diagnostics.errorDescriptor.par64            = 0;    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, send_periodic_error_report, this, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
              
    diagnostics.errorType = eo_errortype_debug;
    diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_ft_ok);
    eo_errman_Error(eo_errman_GetHandle(), diagnostics.errorType, NULL, s_eobj_ownname, &diagnostics.errorDescriptor);
    
    if((0 != diagnostics.repetitionOKcase) && (0 != diagnostics.reportPeriod))
    {
        diagnostics.errorCallbackCount = diagnostics.repetitionOKcase;        
        eo_timer_Start(diagnostics.reportTimer, eok_abstimeNOW, diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
    }
         
    if(NULL != service.onverify)
    {
        service.onverify(nullptr, eobool_true); 
    }     
    
}

void embot::app::eth::theFTservice::Impl::getfullscale_failure()
{
    const eOmn_serv_configuration_t * servcfg = tmpservcfg;

    service.state = eomn_serv_state_failureofverify;
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, service.state);
    
    diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    diagnostics.errorDescriptor.sourceaddress    = 0;
    diagnostics.errorDescriptor.par16            = 0;
    diagnostics.errorDescriptor.par64            = 0;    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, send_periodic_error_report, this, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
        
    
    diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_ft_failed_fullscales);
    diagnostics.errorType = eo_errortype_error;                
    eo_errman_Error(eo_errman_GetHandle(), diagnostics.errorType, NULL, s_eobj_ownname, &diagnostics.errorDescriptor);
    
    if(0 != diagnostics.reportPeriod)
    {
        diagnostics.errorCallbackCount = EOK_int08dummy;
        eo_timer_Start(diagnostics.reportTimer, eok_abstimeNOW, diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
    }
          
    if(NULL != service.onverify)
    {
        service.onverify(nullptr, eobool_false); 
    }      
}


} // namespace embot::app::eth {

// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theFTservice& embot::app::eth::theFTservice::getInstance()
{
    static theFTservice* p = new theFTservice();
    return *p;
}

embot::app::eth::theFTservice::theFTservice()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theFTservice::~theFTservice() { }
        

bool embot::app::eth::theFTservice::initialise(const Config &config)
{
    pImpl->config = config;
    return pImpl->initialise();
}

eOresult_t embot::app::eth::theFTservice::Tick()
{
    return pImpl->Tick();
}


bool embot::app::eth::theFTservice::SendReport()
{
    return pImpl->SendReport();
}

eOresult_t embot::app::eth::theFTservice::Verify(const eOmn_serv_configuration_t * servcfg, 
                                                    eOservice_onendofoperation_fun_t onverify, 
                                                    bool activateafterverify)
{
    return pImpl->Verify(servcfg, onverify, activateafterverify); 
}


eOmn_serv_state_t embot::app::eth::theFTservice::GetServiceState() const
{
    return pImpl->GetServiceState();
}

eOresult_t embot::app::eth::theFTservice::Activate(const eOmn_serv_configuration_t * servcfg)
{
    return pImpl->Activate(servcfg);
}

eOresult_t embot::app::eth::theFTservice::Deactivate()
{
    return pImpl->Deactivate();
}
   
eOresult_t embot::app::eth::theFTservice::Start()
{
    return pImpl->Start();
}
 
eOresult_t embot::app::eth::theFTservice::Stop()
{
    return pImpl->Stop();
}

eOresult_t embot::app::eth::theFTservice::SetRegulars(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    return pImpl->SetRegulars(arrayofid32, numberofthem);
}
    
eOresult_t embot::app::eth::theFTservice::AcceptCANframe(const canFrameDescriptor &canframedescriptor)
{
    return pImpl->AcceptCANframe(canframedescriptor);
}

bool embot::app::eth::theFTservice::process(const EOnv* nv, const eOropdescriptor_t* rd)
{
    return pImpl->process(nv, rd);
}


// - extern C functions ....


extern "C"
{
    
    extern void eoprot_fun_UPDT_as_ft_config(const EOnv* nv, const eOropdescriptor_t* rd)
    {
        embot::app::eth::theFTservice::getInstance().process(nv, rd);
    }

    extern void eoprot_fun_UPDT_as_ft_cmmnds_enable(const EOnv* nv, const eOropdescriptor_t* rd)
    {
        embot::app::eth::theFTservice::getInstance().process(nv, rd);
    }  



    extern void eoprot_fun_INIT_as_ft_config(const EOnv* nv)
    {
        eOas_ft_config_t* ftcfg = reinterpret_cast<eOas_ft_config_t*>(eo_nv_RAM(nv));
        
        ftcfg->ftdatarate = 10; // ms
        ftcfg->calibrationset = 0;
        ftcfg->mode = eoas_ft_mode_calibrated;
        ftcfg->tempdatarate = 1; // sec
    }


    extern void eoprot_fun_INIT_as_ft_status(const EOnv* nv)
    {
        eOas_ft_status_t *status = reinterpret_cast<eOas_ft_status_t*>(eo_nv_RAM(nv));
        
        for(uint8_t i=0; i<eoas_ft_6axis; i++)
        {
            status->fullscale[i] = 1;
            status->timedvalue.values[i] = 111.1;
        }
        status->mode = 0;
        status->timedvalue.age = 0; 
    }    
    
} // extern "C"

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


