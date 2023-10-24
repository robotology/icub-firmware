
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
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EOVtheCallbackManager.h"
#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtimer.h"
#include "EOtheCANdiscovery2.h"

#include "embot_core_binary.h"
#include "embot_app_eth_CANmonitor.h"


#if defined(USE_EMBOT_theServices)
#include "embot_app_eth_theServices.h"
#include "embot_app_eth_Service_legacy.h"
#else 
#include "EOtheServices_hid.h"
#endif

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

namespace embot::app::eth {
    
// it implements the request of the fullscales (6 values for each FT board) 
struct FSquery2
{
    struct Item
    {
        eOprotIndex_t index {0};
        eObrd_canlocation_t loc {};
        eOas_ft_t *ft {nullptr}; 
        Item(eOprotIndex_t i, eObrd_canlocation_t c) : index(i), loc(c) 
        {
            ft = reinterpret_cast<eOas_ft_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_ft, static_cast<eOprotIndex_t>(index))); 
        } 
        ~Item() 
        {
            index = 0; loc = {}; ft = nullptr;
        }    
    };
    
    std::vector<Item> items;
    uint8_t rxfullscales {0};
    uint8_t maxfullscales {0};
    bool querying {false};
    uint8_t currchannel {0};
    uint8_t curritem {0};
    EOtimer *tmr {nullptr};
    EOaction_strg astrg = {0};
    EOaction *act = {nullptr};
    embot::core::Callback ONok {};
    embot::core::Callback ONtimeout {};
    embot::core::relTime timeout {100*embot::core::time1millisec};
    static constexpr size_t maxboards {theFTservice::maxSensors};
    
    static constexpr bool parallelmode {false};
    static constexpr bool fakequery {false};
    
    FSquery2() = default;
    
    ~FSquery2() 
    {
        if(nullptr != tmr)
        {
            eo_timer_Delete(tmr);
        }
    }
    
    void initialise()
    {
        items.reserve(maxboards);
        tmr = eo_timer_New();
        act = reinterpret_cast<EOaction*>(&astrg);
    }
    
    void reset()
    {
        items.clear();
        maxfullscales = rxfullscales = 0;
        currchannel = curritem = 0;
        querying = false;
    }
    
    void add(const Item &item)
    {
        if(nullptr != item.ft)
        {
            items.push_back(item);
        }
    }
    
    static void ontout(void* p)
    {
        FSquery2 *fsq = reinterpret_cast<FSquery2*>(p);
        fsq->querying = false;
        eo_timer_Stop(fsq->tmr);
        fsq->ONtimeout.execute();
    }        
    
    void start(const embot::core::Callback &onOK, const embot::core::Callback &onTOUT, 
                embot::core::relTime tout = 100*embot::core::time1millisec)
    {
        ONok = onOK;
        ONtimeout = onTOUT;
        timeout = tout;        
        
        // set the number of fullscales i will receive
        rxfullscales = 0;
        maxfullscales = 6 * items.size();
    
        querying = true;
        
        if((0 == items.size()) || (true == fakequery))
        {
            ontermination();
            return;
        }

        if(true == parallelmode)
        {
            // send all request now
            askfullscales(); 
        }  
        else
        {    
            // sequential mode: send the requests one at a time
            currchannel = curritem = 0;   
            askfullscale(curritem, currchannel);
        }  

        
        // init timer w/ timeout 
        eo_action_SetCallback(act, ontout, this, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
        eo_timer_Start(tmr, eok_abstimeNOW, timeout, eo_tmrmode_ONESHOT, act);        
    }


    void askfullscale(uint8_t i, uint8_t c)
    {
        // send a message
        eOcanprot_command_t command {};
        uint8_t channel = c;
        command.clas = eocanprot_msgclass_pollingAnalogSensor;
        command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
        command.value = &channel;
        eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, items[i].loc);               
    }
    
    void askfullscales()
    {
        for(uint8_t i=0; i<items.size(); i++)
        {
            for(uint8_t c=0; c<eoas_ft_6axis; c++)
            {
                askfullscale(i, c);
            }
        }
    }
 
    void ontermination()
    {
        eo_timer_Stop(tmr);
        querying = false;
        ONok.execute();
        reset();
    }  

    bool isactive() const
    {
        return querying;
    }        
    
    bool tick(eObrd_canlocation_t loc, uint8_t chnl, uint16_t fsvalue)
    {
        if(false == querying)
        {
            return false;
        }
        
        if(chnl >= eoas_ft_6axis)
        {
            return false;
        }
        
        // search for loc
        uint8_t index {255};
        for(uint8_t i=0; i< items.size(); i++)
        {
            if((items[i].loc.port == loc.port) && (items[i].loc.addr == loc.addr))
            {
                index = i;
                break;
            }
        }
        
        if(255 == index)
        {            
            return false;
        }
            
        // ok, i use the value
        items[index].ft->status.fullscale[chnl] = fsvalue;

        // check if it is over
        rxfullscales++;        
        if(rxfullscales == maxfullscales)
        {
            ontermination();
        }
        else if(false == parallelmode)
        {
            currchannel++;
            if(eoas_ft_6axis == currchannel)
            {
                currchannel = 0;
                curritem++;
                // by uncommenting it we make continuous ... boh
//                    if(curritem == items.size())
//                    {
//                        curritem = 0;
//                    }
            }
            
            // i ask again ...
            if(curritem < items.size())
            {
                askfullscale(curritem, currchannel); 
            }                    
            
        }
                
        return true;
    }        
};


} // namespace embot::app::eth {

namespace embot::app::eth {
    
    // fills a given eOropdescriptor_t w/ whatever is required to be given to theFTservice::process()   
    const eOropdescriptor_t * fill(eOropdescriptor_t &rd, eOnvID32_t id32, void *data, uint16_t size, eOropcode_t rpc = eo_ropcode_set);
    
const eOropdescriptor_t * fill(eOropdescriptor_t &rd, eOnvID32_t id32, void *data, uint16_t size, eOropcode_t rpc)
{
    std::memset(&rd, 0, sizeof(rd));
    rd.id32 = id32;
    rd.data = reinterpret_cast<uint8_t*>(data);
    rd.size = size;
    rd.ropcode = rpc;
    return &rd;
}
 
constexpr eOservice_core_t dummy_service_core 
{
    .initted = eobool_false,
    .active = eobool_false,
    .activateafterverify = eobool_false,
    .started = eobool_false,
    .onverify = nullptr,
    .onverifyarg = nullptr,
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


// finally the implementation struct
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
    // BEWARE: or nullptr if the protocol configure less than maxSensors ft sensors. 
    // theFTboards[i] contains the relenat board type    
    std::array<eOas_ft_t*, maxSensors> theFTnetvariables {nullptr, nullptr, nullptr, nullptr};    
    std::array<eObrd_cantype_t, maxSensors> theFTboards {eobrd_cantype_none, eobrd_cantype_none, eobrd_cantype_none, eobrd_cantype_none};
    
    // this object performs the query of the full scales for all the FT sensors
    FSquery2 fullscalequery2 {};
        
    // this object performs the monitoring of presence of CAN boards
    CANmonitor canmonitor {};
    static constexpr CANmonitor::Config defaultcanmonitorconfig { 
        {}, // the map is left empty
        100*embot::core::time1millisec, 
        CANmonitor::Report::ALL,  
        10*embot::core::time1second,
        s_eobj_ownname,
        eomn_serv_category_ft
    };
            
    embot::core::Time debugtimeft {0};    
    
    // methods used by theFTservice 
    
    Impl() = default;      
    
    eOresult_t initialise();
    eOresult_t SendReport();   
    eOresult_t Verify(const eOmn_serv_configuration_t * servcfg, 
                eOservice_onendofoperation_fun_t onverify, 
                void *arg,
                bool activateafterverify);
    eOresult_t Activate(const eOmn_serv_configuration_t * servcfg);
    eOresult_t Deactivate();  
    eOmn_serv_state_t GetServiceState() const;   
    eOresult_t SetRegulars(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem); 
    eOresult_t Start(); 
    eOresult_t Stop();
    eOresult_t Tick(); 
    eOresult_t AcceptCANframe(const canFrameDescriptor &canframedescriptor);
    eOresult_t process(const eOropdescriptor_t* rd, const EOnv* nv);  

    // methods used to manage ETH commands. called by process() but also by the theFTservice
    bool set(eOprotIndex_t index, const eOas_ft_config_t *ftc);
    bool enable(eOprotIndex_t index, const uint8_t *cmdenable);
    
    const theServiceTester::Config & servicetesterconfig() const;

private:  
    
    // service methods used to tx commands to CAN             
    void can_forcetorque_Config(eOprotIndex_t index, uint8_t ratemillisec);
    void can_forcetorque_TX(eOprotIndex_t index,  bool on);
    void can_forcetorque_TXstop();
    void can_temperature_Config(eOprotIndex_t index);
    void can_temperature_TX(eOprotIndex_t index, bool on);
    void can_temperature_TXstop(); 
    
    // service methods used to handle can data
    eOresult_t can_forcetorque_processfullscale(const canFrameDescriptor &cfd); 
    void can_forcetorque_saturation_handler(const canFrameDescriptor &cfd, eOprotIndex_t index);
    
    // service methods used to handle the verifying state
    // some must stay static because are used as callbacks
    // step 1: discovery of all required CAN boards
    // step 2: we start full scales query
    // step3: if query is OK we activate and send reply back to yarprobotinterface. if KO just reply back.
    eOresult_t verifyingstate_step1_discovery(const eOmn_serv_configuration_t * servcfg, 
                                              eOservice_onendofoperation_fun_t onverify, 
                                              void *arg,
                                              bool activateafterverify);
    static eOresult_t s_verifyingstate_step2_onstop_search_for_ft_boards_we_get_fullscale(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok);
    static void s_verifyingstate_step3_onFSqueryOK(void *par);
    static void s_verifyingstate_step3_onFSqueryTIMEOUT(void *par);
    
    
    static void synchservice(eOmn_serv_state_t servstate);

    
    // other static ones used for callbacks
    static void s_send_periodic_error_report(void *par);    
    static constexpr eObool_t s_ft_isID32relevant(uint32_t id32);
    
    // debug funtions
    void debug_CANMAPPING(EOconstarray *carray);

};

//embot::app::eth::theFTservice::Impl::Impl()
//{
//    // we need it because ... we dont have default values for the C structs and better doing  
//}
      
eOresult_t embot::app::eth::theFTservice::Impl::initialise()
{
    if(true == initted)
    {
        return eores_OK;
    }
    
    embot::core::print("embot::app::eth::theFTservice::Impl::initialise()");
    
    service.servconfig.type = eomn_serv_NONE;
    sharedcan.boardproperties = eo_vector_New(sizeof(eObrd_canproperties_t), theFTservice::maxSensors, nullptr, 0, nullptr, nullptr);
    sharedcan.entitydescriptor = eo_vector_New(sizeof(eOcanmap_entitydescriptor_t), theFTservice::maxSensors, nullptr, 0, nullptr, nullptr);
    sharedcan.discoverytargets = eo_array_New(theFTservice::maxSensors, sizeof(eOcandiscovery_target_t),  nullptr);
    id32ofregulars = eo_array_New(embot::app::eth::theFTservice::maxRegulars, sizeof(uint32_t), nullptr);
    
    diagnostics.reportTimer = eo_timer_New();
    diagnostics.errorType = eo_errortype_error;
    diagnostics.errorDescriptor.sourceaddress = eo_errman_sourcedevice_localboard;
    diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_ft_not_verified_yet);      
        
    service.initted = eobool_true;
    service.active = eobool_false;
    service.started = eobool_false;    
    service.state = eomn_serv_state_idle;  
    synchservice(service.state);

    // clear data used by this class
    for(auto &item : theFTnetvariables) item = nullptr;
    for(auto &item : theFTboards) item = eobrd_cantype_none;
      
    fullscalequery2.initialise();
    
    initted = true;
    return eores_OK;
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
    canmonitor.tick();
              
    return eores_OK;
}


eOresult_t embot::app::eth::theFTservice::Impl::SendReport()
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
        
    return eores_OK;
}


eOresult_t embot::app::eth::theFTservice::Impl::Verify(const eOmn_serv_configuration_t * servcfg, 
                eOservice_onendofoperation_fun_t onverify,
                void *arg,
                bool activateafterverify)
{
    
    eo_errman_Trace(eo_errman_GetHandle(), "::Verify()", s_eobj_ownname);
    
    if(NULL == servcfg)
    {
        service.state = eomn_serv_state_failureofverify;
        synchservice(service.state);

        if(NULL != onverify)
        {
            onverify(arg, eobool_false); 
        }                 
        return(eores_NOK_nullpointer);
    }  
 
    if(eomn_serv_AS_ft != servcfg->type)
    {
        service.state = eomn_serv_state_failureofverify;
        synchservice(service.state);        

        if(NULL != onverify)
        {
            onverify(arg, eobool_false); 
        }          
        return(eores_NOK_generic);
    } 
     
    if(eobool_true == service.active)
    {
        Deactivate();        
    }  

    service.state = eomn_serv_state_verifying;
    synchservice(service.state);

    // make sure the timer is not running
    eo_timer_Stop(diagnostics.reportTimer);  
    
    service.onverify = onverify;
    service.onverifyarg = arg;
    service.activateafterverify = activateafterverify;
    
    
    // from the passed service configuration:
    // the array of ft consiguration of ft sensors. it holds eOas_ft_sensordescriptor_t items
    EOconstarray *carray = eo_constarray_Load(reinterpret_cast<const EOarray*>(&servcfg->data.as.ft.arrayofsensors));    
    uint8_t numofsensors = eo_constarray_Size(carray);
    
    // do:
    // A. sanity check of the configuration
    // B. if DEBUG_CANMAPPING is defined we execute some debug
    // C. preparation and launch of verifying step 1: the discovery of the CAN boards
    // D. return 
    
        
    
    // A. sanity check
    
    if(0 == numofsensors)
    {
        // no sensors in descriptor ....
        service.state = eomn_serv_state_failureofverify;
        synchservice(service.state);

        if(NULL != onverify)
        {
            onverify(arg, eobool_false); 
        }          
        return(eores_NOK_generic);
    }
    
    // check that all the boards in the descriptor are ok: the must have the correct type but also there must not be doubles (same address)
    uint16_t canmap[2] = {0, 0};
    for(uint8_t s=0; s<numofsensors; s++)
    {
        const eOas_ft_sensordescriptor_t *sd = reinterpret_cast<const eOas_ft_sensordescriptor_t*>(eo_constarray_At(carray, s));
        bool alreadyused = embot::core::binary::bit::check(canmap[sd->canloc.port], sd->canloc.addr);
      
        if((true == alreadyused) || (eobool_false == eoas_ft_isboardvalid(static_cast<eObrd_cantype_t>(sd->boardinfo.type))))
        {
            // double board in descriptor or invalid type ....
            service.state = eomn_serv_state_failureofverify;
            synchservice(service.state);
           
            if(NULL != onverify)
            {
                onverify(arg, eobool_false); 
            }          
            return(eores_NOK_generic);            
        }
        embot::core::binary::bit::set(canmap[sd->canloc.port], sd->canloc.addr);
    }
    
    // B. if DEBUG_CANMAPPING is defined we execute some debug 
    
    debug_CANMAPPING(carray);  

    // C. preparation and launch of verifying step 1: the discovery of the CAN boards
    
    eOresult_t res = verifyingstate_step1_discovery(servcfg, onverify, arg, activateafterverify);
    
    
    // D. we return. the rest of operations required by verify-activate will be done by the callback manager
    //    in chain there will be: query of full scales, activation of the service.
    return res;

}

eOresult_t embot::app::eth::theFTservice::Impl::Activate(const eOmn_serv_configuration_t * servcfg)
{
    eo_errman_Trace(eo_errman_GetHandle(), "::Activate()", s_eobj_ownname);
    
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
    
    memcpy(&service.servconfig, servcfg, sizeof(eOmn_serv_configuration_t));
                
    // now... use the servcfg    
    EOconstarray *carray = eo_constarray_Load(reinterpret_cast<const EOarray*>(&servcfg->data.as.ft.arrayofsensors));    
    uint8_t numofsensors = eo_constarray_Size(carray);
    
    // marco.accame on 25 feb 2022: for now i dont do any more verification on the sanity of the servconfig.
    // we already did whatever we could inside Verify()
    
    // in here we do: 
    // - load boards in can mapping, and also in theFTboards
    // - load entities, one for board in strict order 
    // - init the theFTnetvariables
 
    // we must load all the boards into can mapping, entities etc    
    for(uint8_t s=0; s<numofsensors; s++)
    {
        const eOas_ft_sensordescriptor_t *sd = reinterpret_cast<const eOas_ft_sensordescriptor_t*>(eo_constarray_At(carray, s));
        
        if((nullptr != sd) && (eobool_true == eoas_ft_isboardvalid(static_cast<eObrd_cantype_t>(sd->boardinfo.type))))
        {  // even if it is a useless check because we did it before inside Verify()
            
            // preparation for loading can boards into EOtheCANmapping
            eObrd_canproperties_t prop = {0};
            prop.type = sd->boardinfo.type;
            prop.location.port = sd->canloc.port; 
            prop.location.addr = sd->canloc.addr; 
            prop.location.insideindex = eobrd_caninsideindex_none;
            prop.requiredprotocol = sd->boardinfo.protocol;            
            eo_vector_PushBack(sharedcan.boardproperties, &prop); 
            
            // preparation for loading entities into EOtheCANmapping
            eOcanmap_entitydescriptor_t des = {0};
            des.location.port = sd->canloc.port;
            des.location.addr = sd->canloc.addr;
            des.location.insideindex = eobrd_caninsideindex_none;
            des.index = static_cast<eOcanmap_entityindex_t>(s);  
            eo_vector_PushBack(sharedcan.entitydescriptor, &des);   

            // data used by this class         
            void *vpft = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_ft, static_cast<eOprotIndex_t>(s));
            theFTnetvariables[s] = reinterpret_cast<eOas_ft_t*>(vpft);
            theFTboards[s] = static_cast<eObrd_cantype_t>(sd->boardinfo.type);             
        }                
    }
    
    eo_canmap_LoadBoards(eo_canmap_GetHandle(), sharedcan.boardproperties);  
    eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_ft, sharedcan.entitydescriptor);
  
    
    // ok, service is active now

    service.active = eobool_true;
    service.state = eomn_serv_state_activated; 
    synchservice(service.state);

    return(eores_OK); 
}


eOresult_t embot::app::eth::theFTservice::Impl::Deactivate()
{
    eo_errman_Trace(eo_errman_GetHandle(), "::Deactivate()", s_eobj_ownname);
    
    if(eobool_false == service.active)
    {
        // i force to eomn_serv_state_idle because it may be that state was eomn_serv_state_verified or eomn_serv_state_failureofverify
        service.state = eomn_serv_state_idle; 
        synchservice(service.state);
        return(eores_OK);        
    } 
    
    // send stop messages to the ft sensor board, unload the entity-can-mapping and the board-can-mapping, reset all things inside this object

    if(eobool_true == service.started)
    {
        Stop();
    }
    
    SetRegulars(nullptr, nullptr);
    
    // deconfig EOtheCANmapping
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_ft, sharedcan.entitydescriptor);     
    eo_canmap_UnloadBoards(eo_canmap_GetHandle(), sharedcan.boardproperties);
    
    // clear data used by this class
    for(auto &item : theFTnetvariables) 
    {
        if(nullptr != item)
        {
            // i reset the status to its default value
            std::memmove(&item->status, &defaultFTstatus, sizeof(eOas_ft_status_t));
            // i reset the config to its default value
            std::memmove(&item->config, &defaultFTconfig, sizeof(eOas_ft_config_t));
        }
        
        item = nullptr;
    }
    for(auto &item : theFTboards)
    {
        item = eobrd_cantype_none;
    }

         
    memset(&service.servconfig, 0, sizeof(eOmn_serv_configuration_t));
    service.servconfig.type = eomn_serv_NONE;
    
    eo_vector_Clear(sharedcan.boardproperties);
    eo_vector_Clear(sharedcan.entitydescriptor);
    eo_array_Reset(sharedcan.discoverytargets);
    
    // make sure the timer is not running
    eo_timer_Stop(diagnostics.reportTimer);       
    
    service.active = eobool_false;    
    service.state = eomn_serv_state_idle; 
    synchservice(service.state);
   
    return(eores_OK);
}

   
eOresult_t embot::app::eth::theFTservice::Impl::Start()
{
    eo_errman_Trace(eo_errman_GetHandle(), "::Start()", s_eobj_ownname);
    
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
    synchservice(service.state);
    
    // Start() does not force the tx from any CAN sensor boards.
    // the activation of transmission is done at reception of command set<eOas_ft_commands_t::enable, 1>
    // managed inside process(const EOnv* nv, const eOropdescriptor_t* rd)
    
    // we configure it w/ an empty target. and we start it. we shall add boards when the tx is enabled
    CANmonitor::Config cfg = defaultcanmonitorconfig;
    // use ...
    cfg.target.clear();
    cfg.periodofcheck = embot::core::time1millisec * service.servconfig.data.as.ft.canmonitorconfig.periodofcheck;
    cfg.periodofreport = embot::core::time1millisec * service.servconfig.data.as.ft.canmonitorconfig.periodofreport;
    cfg.reportmode = static_cast<CANmonitor::Report>(service.servconfig.data.as.ft.canmonitorconfig.reportmode);
    canmonitor.configure(cfg);
    canmonitor.start();
                
    return eores_OK;
}
 

eOresult_t embot::app::eth::theFTservice::Impl::Stop()
{ 
    eo_errman_Trace(eo_errman_GetHandle(), "::Stop()", s_eobj_ownname); 
    
    if(eobool_false == service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
    
    if(eobool_false == service.started)
    {   // it is already stopped
        return(eores_OK);
    }  
    
    // Stop() forces the end of the tx from all the CAN sensor boards
    // and of the activities of the CAN monitor
    canmonitor.stop();
    can_forcetorque_TXstop();   
    can_temperature_TXstop();

    service.started = eobool_false;
    service.state = eomn_serv_state_activated;
    synchservice(service.state);
    
    // we dont remove all regulars related to ft entity because Deactivate() does that.
    
    return eores_OK;
}

eOresult_t embot::app::eth::theFTservice::Impl::SetRegulars(eOmn_serv_arrayof_id32_t* arrayofid32, uint8_t* numberofthem)
{
    eo_errman_Trace(eo_errman_GetHandle(), "::SetRegulars()", s_eobj_ownname);
    
    if(eobool_false == service.active)
    {   // nothing to do because object must be first activated
        return(eores_OK);
    }  
#if defined(USE_EMBOT_theServices)
    auto isok = [](uint32_t id){ return eobool_true == s_ft_isID32relevant(id); };
    bool r = embot::app::eth::theServices::getInstance().setREGULARS(id32ofregulars, arrayofid32, isok, numberofthem);
    return r ? eores_OK : eores_NOK_generic;
#else    
    return(eo_service_hid_SetRegulars(id32ofregulars, arrayofid32, s_ft_isID32relevant, numberofthem));
#endif    
}

eOresult_t embot::app::eth::theFTservice::Impl::can_forcetorque_processfullscale(const canFrameDescriptor &cfd)
{
    if(false == fullscalequery2.isactive())        
    {   // we process only when fullscalequery2 is active
        return eores_OK;
    }
         
    eObrd_canlocation_t loc = {};
    loc.port = cfd.port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(cfd.frame);
    loc.insideindex = eobrd_caninsideindex_none;
        
    // now i get the channel and the data to be put inside the strain.  
    uint8_t channel = cfd.frame->data[1];
    uint16_t value = (static_cast<uint16_t>(cfd.frame->data[2]) << 8) + static_cast<uint16_t>(cfd.frame->data[3]);
    
    fullscalequery2.tick(loc, channel, value);
    
    return eores_OK;
}


eOresult_t embot::app::eth::theFTservice::Impl::AcceptCANframe(const canFrameDescriptor &cfd)
{
    // we accept can frames only if we are activated.
    // the only exception is for fullscales.
    
    canFrameDescriptor::Type type = cfd.type;
    
    // #warning process also Type::autodetect 
    if(canFrameDescriptor::Type::autodetect == type)
    {
        // eval the frame->id and frame->data[0]
        if(3 == cfd.frame->data[0])
        {
           type = canFrameDescriptor::Type::fullscale; 
        }
        else if(4 == (cfd.frame->id & 0xf))
        {
            type = canFrameDescriptor::Type::force; 
        }
        else if(5 == (cfd.frame->id & 0xf))
        {
            type = canFrameDescriptor::Type::torque; 
        }        
    }
    
    if(canFrameDescriptor::Type::fullscale == type)
    {
        return can_forcetorque_processfullscale(cfd);
    }
    
    
    
    // object must be first activated otherwise we cannot use EOtheCANmapping, theFTnetvariables, theFTboards  
    if(eobool_false == service.active)
    {   
        return(eores_OK);
    }   
    

    eObrd_canlocation_t loc = {};
    loc.port = cfd.port;
    loc.addr = EOCANPROT_FRAME_GET_SOURCE(cfd.frame);
    loc.insideindex = eobrd_caninsideindex_none;

    eOprotIndex_t index = eo_canmap_GetEntityIndex(eo_canmap_GetHandle(), loc, eoprot_endpoint_analogsensors, eoprot_entity_as_ft); 
    if(EOK_uint08dummy == index)
    {
        return eores_NOK_generic;
    }
    
    canmonitor.touch(loc);
    
    eOas_ft_t *ft = theFTnetvariables[index];
    if(nullptr == ft)
    {   // something is wrong
        return eores_NOK_generic;
    }
    
    switch(type)
    {
        case canFrameDescriptor::Type::force:
        case canFrameDescriptor::Type::torque:
        {
            uint8_t offset = (canFrameDescriptor::Type::force == type) ? 0 : 3;            
            for(uint8_t i=0; i<3; i++)
            {
                float32_t vv = (static_cast<uint16_t>(cfd.frame->data[2*i+1]) << 8) + static_cast<uint16_t>(cfd.frame->data[2*i]) - static_cast<uint16_t>(0x8000);
                if(eoas_ft_mode_calibrated == ft->config.mode) 
                {
                    vv = vv * static_cast<float32_t>(ft->status.fullscale[i+offset]) / static_cast<float32_t>(0x8000);
                }
                ft->status.timedvalue.values[i+offset] = vv;
            }
            
            ft->status.timedvalue.age = embot::core::now();
            can_forcetorque_saturation_handler(cfd, index);
                                   
        } break;   

        case canFrameDescriptor::Type::fullscale:
        { 
            // nothing to do because we process it elsewhere
        } break;  
        
        case canFrameDescriptor::Type::temperature:
        {
            ft->status.timedvalue.temperature = (static_cast<uint16_t>(cfd.frame->data[2]) << 8) + static_cast<uint16_t>(cfd.frame->data[1]);
            //embot::core::print("T = " + std::to_string(ft->status.timedvalue.temperature/10.0) + " C");        
        }

        default:
        {
            
        } break;        
    }

    
    return eores_OK;
}


void embot::app::eth::theFTservice::Impl::can_forcetorque_saturation_handler(const canFrameDescriptor &cfd, eOprotIndex_t index)
{
    static uint16_t upper_saturations[maxSensors][6] = {0};
    static uint16_t lower_saturations[maxSensors][6] = {0};
    
    static uint32_t counter[maxSensors] = {0};
    
    eOas_ft_t *ft = theFTnetvariables[index];
    
    counter[index] ++;
    
    uint8_t ftTXrate = ft->config.ftperiod; // in ms
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

eOresult_t embot::app::eth::theFTservice::Impl::process(const eOropdescriptor_t* rd, const EOnv* nv)
{
    if((nullptr == rd) || (nullptr == rd->data))
    {
        return eores_NOK_nullpointer;
    }
    
    eOprotIndex_t index = eoprot_ID2index(rd->id32);
    eOprot_tag_as_ft_t tag = static_cast<eOprot_tag_as_ft_t>(eoprot_ID2tag(rd->id32));
    
    switch(tag)
    {
        case eoprot_tag_as_ft_config: 
        {
            // in here we need to get the configuration and assign it
            eOas_ft_config_t *cfg = reinterpret_cast<eOas_ft_config_t*>(rd->data);
            set(index, cfg);            
        } break;
        
        case eoprot_tag_as_ft_cmmnds_enable:
        {
            // in here we either start or stop depending on value 1 or 0 of ...
            // eOas_ft_commands_t::enable
            uint8_t * cmdenable = reinterpret_cast<uint8_t*>(rd->data);
            enable(index, cmdenable);
        } break;
        
        default:
        {
            
        } break;
        
    }
            
    return eores_OK;
}


bool embot::app::eth::theFTservice::Impl::set(eOprotIndex_t index, const eOas_ft_config_t *ftc)
{
    // NOTE-x:    
    // ... the ETH protocol mechanism already writes inside the relevant memory the new values
    // inside ftc we just have have them, which are also inside theFTnetvariables[index]->config
    // however, ... if the method process() whuich calls set() is called by theServiceValidator 
    // and not by the ETH callbacks, we also must copy what inside ftc
    // but ... beware of nullptr
    
    if(nullptr == ftc)
    {
        return false;
    }  

    if(eobool_false == service.active)
    {   
        return false;
    }     
    
    eOas_ft_config_t *theNVptr = (nullptr != theFTnetvariables[index]) ? &theFTnetvariables[index]->config : nullptr;

    if(nullptr == theNVptr)
    {
        return false;
    }
    
    if(ftc != theNVptr)
    {   // in this case the caller was not the ETH handler, so we must update the internal memory
        std::memmove(theNVptr, ftc, sizeof(eOas_ft_config_t));
    }
    
    // impose datarate to the FT sensor board 
    can_forcetorque_Config(index, ftc->ftperiod);
    
    // copy mode into status
    theFTnetvariables[index]->status.mode = ftc->mode;
    
    
    // config the temperature sensor in teh FT sensor board, even if we shall no enable it
    if(eobrd_cantype_strain2 == theFTboards[index])
    {
        can_temperature_Config(index);
    }
        
    return true;
}


bool embot::app::eth::theFTservice::Impl::enable(eOprotIndex_t index, const uint8_t *cmdenable)
{
    // see NOTE-x above.
    
    if(nullptr == cmdenable)
    {
        return false;
    }    

    if(eobool_false == service.active)
    {   
        return false;
    } 
    
    uint8_t *theNVptr = (nullptr != theFTnetvariables[index]) ? &theFTnetvariables[index]->cmmnds.enable : nullptr;
    
    if(nullptr == theNVptr)
    {
        return false;
    }
    
    if(cmdenable != theNVptr)
    {   // in this case the caller was not the ETH handler, so we must update the internal memory
        std::memmove(theNVptr, cmdenable, sizeof(uint8_t));
    }  

    // in here i will start / stop tx of FT data
    
    bool on = (1 == *cmdenable) ? true : false;
    
    can_forcetorque_TX(index, on); 
    
    // and also of temperature data
    if(eobrd_cantype_strain2 == theFTboards[index])
    {
        can_temperature_TX(index, on);
    }  

    // i also need to add / rem the can location to the canmonitor. 
    // NOTE: i do that only for FT, not for temperature because it is FT che comanda
    eObrd_canlocation_t loc {};
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, index, eoprot_tag_none);
    if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &loc, NULL, NULL))
    {
        if(true == on)
        {
            canmonitor.add(loc);            
//            // i adjust rate according to actual rate w/ a safety factor
//            constexpr uint16_t percentgain = 100;
//            embot::core::relTime periodofcheck = 10*(100+percentgain) * theFTnetvariables[index]->config.ftperiod;
//            canmonitor.setcheckrate(periodofcheck);
        }
        else
        {
            canmonitor.rem(loc);
        }
    }    
    
    return true;
}


eOresult_t embot::app::eth::theFTservice::Impl::verifyingstate_step1_discovery(
                const eOmn_serv_configuration_t * servcfg,
                eOservice_onendofoperation_fun_t onverify, 
                void *arg,
                bool activateafterverify)
{
    EOconstarray *carray = eo_constarray_Load(reinterpret_cast<const EOarray*>(&servcfg->data.as.ft.arrayofsensors));    
    uint8_t numofsensors = eo_constarray_Size(carray);    
    
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
        synchservice(service.state);     
        if(NULL != onverify)
        {
            onverify(arg, eobool_false); 
        }          
        return(eores_NOK_generic);
    }
    
    // ok, we launch discovery for all the boards 
    
    // init params
    tmpservcfg = servcfg;
    sharedcan.ondiscoverystop.function = s_verifyingstate_step2_onstop_search_for_ft_boards_we_get_fullscale; 
    sharedcan.ondiscoverystop.parameter = (void*) this; 
        
    // start discovery
    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), sharedcan.discoverytargets, &sharedcan.ondiscoverystop); 

    return eores_OK;    
}


eOresult_t embot::app::eth::theFTservice::Impl::s_verifyingstate_step2_onstop_search_for_ft_boards_we_get_fullscale(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{   
    embot::app::eth::theFTservice::Impl *p = reinterpret_cast<embot::app::eth::theFTservice::Impl*>(par);
    const eOmn_serv_configuration_t * servcfg = p->tmpservcfg;    


    if(eobool_true == searchisok)
    {
        p->fullscalequery2.reset();
        
        EOconstarray *carray = eo_constarray_Load(reinterpret_cast<const EOarray*>(&servcfg->data.as.ft.arrayofsensors));    
        // the number of sensor descriptors 
        uint8_t numofsensors = eo_constarray_Size(carray);
 
        for(uint8_t s=0; s<numofsensors; s++)
        {
            const eOas_ft_sensordescriptor_t *sd = reinterpret_cast<const eOas_ft_sensordescriptor_t*>(eo_constarray_At(carray, s));            
            // just for debug: we add the board to the query of the fullscale only if the ffu is not 255
            if(sd->ffu != 255)
            {
                p->fullscalequery2.add({static_cast<eOprotIndex_t>(s), sd->canloc});
            }
        }
               
        embot::core::Callback onOK {s_verifyingstate_step3_onFSqueryOK, p};
        embot::core::Callback onTIMEOUT {s_verifyingstate_step3_onFSqueryTIMEOUT, p};
        p->fullscalequery2.start(onOK, onTIMEOUT);    
    }
    else
    {   
        p->service.state = eomn_serv_state_failureofverify;
        synchservice(p->service.state);

        p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
        p->diagnostics.errorDescriptor.sourceaddress    = 0;
        p->diagnostics.errorDescriptor.par16            = 0;
        p->diagnostics.errorDescriptor.par64            = 0;    
        EOaction_strg astrg = {0};
        EOaction *act = (EOaction*)&astrg;
        eo_action_SetCallback(act, s_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
        
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
            p->service.onverify(p->service.onverifyarg, eobool_false);            
        }
        
    }
    
    return(eores_OK);   
}


void embot::app::eth::theFTservice::Impl::s_send_periodic_error_report(void *par)
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


constexpr eObool_t embot::app::eth::theFTservice::Impl::s_ft_isID32relevant(uint32_t id32)
{
    constexpr uint32_t mask0 = (((uint32_t)eoprot_endpoint_analogsensors) << 24) | (((uint32_t)eoprot_entity_as_ft) << 16);
    
    if((id32 & mask0) == mask0)
    {
        return(eobool_true);
    }
    
    return(eobool_false); 
}


void embot::app::eth::theFTservice::Impl::can_forcetorque_Config(eOprotIndex_t index, uint8_t ratemillisec)
{ 
    uint8_t datarate = ratemillisec;    
    sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;    
    sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    sharedcan.command.value = &datarate;
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, index, eoprot_tag_none);
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &sharedcan.command, id32);
}


void embot::app::eth::theFTservice::Impl::can_forcetorque_TX(eOprotIndex_t index, bool on)
{ 
    if(nullptr == theFTnetvariables[index])
    {
        return;
    }
    eOas_ft_mode_t mode = static_cast<eOas_ft_mode_t>(theFTnetvariables[index]->config.mode);
    
    // now, depending on the pair {transmit, mode} we choose the eOas_strainmode_t value to send to the strain / strain2 board
    // amongst these three: 
    // eoas_strainmode_acquirebutdonttx (if transmit is false), 
    // eoas_strainmode_txuncalibrateddatacontinuously, 
    // eoas_strainmode_txcalibrateddatacontinuously
    
    eOas_strainmode_t strainmode = eoas_strainmode_acquirebutdonttx;
    
    if(true == on)
    {
        strainmode = (eoas_ft_mode_raw == mode) ? eoas_strainmode_txuncalibrateddatacontinuously : eoas_strainmode_txcalibrateddatacontinuously;
    }
    
    sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;    
    sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
    sharedcan.command.value = &strainmode;
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, index, eoprot_tag_none);
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &sharedcan.command, id32);
}

void embot::app::eth::theFTservice::Impl::can_forcetorque_TXstop()
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

void embot::app::eth::theFTservice::Impl::can_temperature_TX(eOprotIndex_t index, bool on) 
{
    if(nullptr == theFTnetvariables[index])
    {
        return;
    }

    if(true == on)
    {
        can_temperature_Config(index);
    }
    
    // start / stop its transmission
    icubCanProto_thermo_transmit_t ttransmit = {0};
    ttransmit.periodsec = (true == on) ? (static_cast<uint8_t>(theFTnetvariables[index]->config.temperatureperiod)) : (0);

    sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__THERMOMETER_TRANSMIT;
    sharedcan.command.value = &ttransmit; 
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, index, eoprot_tag_none);
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &sharedcan.command, id32);    
}


void embot::app::eth::theFTservice::Impl::can_temperature_Config(eOprotIndex_t index) 
{
    if(nullptr == theFTnetvariables[index])
    {
        return;
    }

    icubCanProto_thermo_config_t tconfig = {0};
    eo_common_byte_bitset(&tconfig.sensormask, icubCanProto_thermo_t1); // only the first (and only) sensor

    // configure sensor
    sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;    
    sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__THERMOMETER_CONFIG_SET;
    sharedcan.command.value = &tconfig;
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, index, eoprot_tag_none);
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &sharedcan.command, id32);
}


void embot::app::eth::theFTservice::Impl::can_temperature_TXstop()
{
    icubCanProto_thermo_transmit_t ttransmit = {0};
    ttransmit.periodsec = 0;
    sharedcan.command.clas = eocanprot_msgclass_pollingAnalogSensor;
    sharedcan.command.type  = ICUBCANPROTO_POL_AS_CMD__THERMOMETER_TRANSMIT;
    sharedcan.command.value = &ttransmit; 
    
    for(uint8_t index = 0; index<theFTboards.size(); index++) 
    {
        if(eobrd_cantype_strain2 == theFTboards[index])
        {            
            eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, index, eoprot_tag_none);
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &sharedcan.command, id32); 
        }
    }        
}


void embot::app::eth::theFTservice::Impl::s_verifyingstate_step3_onFSqueryOK(void *par)
{
    embot::app::eth::theFTservice::Impl *p = reinterpret_cast<embot::app::eth::theFTservice::Impl*>(par);
    
    const eOmn_serv_configuration_t * servcfg = p->tmpservcfg;

    p->service.state = eomn_serv_state_verified;
    synchservice(p->service.state);       
   
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
    eo_action_SetCallback(act, s_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
              
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
        p->service.onverify(p->service.onverifyarg, eobool_true); 
    }     
    
}


void embot::app::eth::theFTservice::Impl::s_verifyingstate_step3_onFSqueryTIMEOUT(void *par)
{
    embot::app::eth::theFTservice::Impl *p = reinterpret_cast<embot::app::eth::theFTservice::Impl*>(par);
    
    const eOmn_serv_configuration_t * servcfg = p->tmpservcfg;

    p->service.state = eomn_serv_state_failureofverify;
    synchservice(p->service.state);      
    
    p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
    p->diagnostics.errorDescriptor.sourceaddress    = 0;
    p->diagnostics.errorDescriptor.par16            = 0;
    p->diagnostics.errorDescriptor.par64            = 0;    
    EOaction_strg astrg = {0};
    EOaction *act = (EOaction*)&astrg;
    eo_action_SetCallback(act, s_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));
        
    
    p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_ft_failed_fullscales);
    p->diagnostics.errorType = eo_errortype_error;                
    eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
    
    if(0 != p->diagnostics.reportPeriod)
    {
        p->diagnostics.errorCallbackCount = EOK_int08dummy;
        eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
    }
          
    if(NULL != p->service.onverify)
    {
        p->service.onverify(p->service.onverifyarg, eobool_false); 
    }      
}

void embot::app::eth::theFTservice::Impl::synchservice(eOmn_serv_state_t s)
{
#if defined(USE_EMBOT_theServices)
    embot::app::eth::theServices::getInstance().synch(embot::app::eth::Service::Category::ft, static_cast<embot::app::eth::Service::State>(s));
#else        
    eo_service_hid_SynchServiceState(eo_services_GetHandle(), eomn_serv_category_ft, s);
#endif      
}


void embot::app::eth::theFTservice::Impl::debug_CANMAPPING(EOconstarray *carray)
{
#if defined(DEBUG_CANMAPPING) 
    
    uint8_t numofsensors = eo_constarray_Size(carray);
    
    // we must load all the boards into can mapping, entities etc    
    for(uint8_t s=0; s<numofsensors; s++)
    {
        const eOas_ft_sensordescriptor_t *sd = reinterpret_cast<const eOas_ft_sensordescriptor_t*>(eo_constarray_At(carray, s));
        
        if((nullptr != sd) && (eobool_true == eoas_ft_isboardvalid(static_cast<eObrd_cantype_t>(sd->boardinfo.type))))
        {  // even if it is a useless check because we did it before inside Verify()
            
            // preparation for loading can boards into EOtheCANmapping
            eObrd_canproperties_t prop = {0};
            prop.type = sd->boardinfo.type;
            prop.location.port = sd->canloc.port; 
            prop.location.addr = sd->canloc.addr; 
            prop.location.insideindex = eobrd_caninsideindex_none;
            prop.requiredprotocol = sd->boardinfo.protocol;            
            eo_vector_PushBack(sharedcan.boardproperties, &prop); 
            
            // preparation for loading entities into EOtheCANmapping
            eOcanmap_entitydescriptor_t des = {0};
            des.location.port = sd->canloc.port;
            des.location.addr = sd->canloc.addr;
            des.location.insideindex = eobrd_caninsideindex_none;
            des.index = static_cast<eOcanmap_entityindex_t>(s);  
            eo_vector_PushBack(sharedcan.entitydescriptor, &des);   

            // data used by this class         
            void *vpft = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_ft, static_cast<eOprotIndex_t>(s));
            theFTnetvariables[s] = reinterpret_cast<eOas_ft_t*>(vpft);
            theFTboards[s] = static_cast<eObrd_cantype_t>(sd->boardinfo.type);             
        }                
    }
    
    eo_canmap_LoadBoards(eo_canmap_GetHandle(), sharedcan.boardproperties);  
    eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_analogsensors, eoprot_entity_as_ft, sharedcan.entitydescriptor);
  
    
    
    #warning .... ok, verify that we have a good entity loaded ....
    volatile uint8_t ii = 7;
    for(uint8_t s=0; s<numofsensors; s++)
    {
        const eOas_ft_sensordescriptor_t *sd = reinterpret_cast<const eOas_ft_sensordescriptor_t*>(eo_constarray_At(carray, s));

        eObrd_canlocation_t loc = {};
        loc = sd->canloc;
        ii = eo_canmap_GetEntityIndex(eo_canmap_GetHandle(), sd->canloc, eoprot_endpoint_analogsensors, eoprot_entity_as_ft);
        ii = ii;
    }
    
    ii = ii;
    
    // stop here ... it is debug
    for(;;);
    
#endif // #if defined(DEBUG_CANMAPPING) 
}

#if defined(enableTHESERVICETESTER)

constexpr size_t numberofFTs2test {2};

constexpr eOmn_serv_configuration_t s_serv_config_as_ft_001 =
{   
    .type = eomn_serv_AS_ft,
    .diagnosticsmode = eomn_serv_diagn_mode_NONE,
    .diagnosticsparam = 0,
    .data = 
    { .as = { .ft = {
        .canmonitorconfig = 
        {
            .periodofcheck = 100,
            .reportmode = eobrd_canmonitor_reportmode_ALL,
            .periodofreport = 10*1000        
        },
        .arrayofsensors = 
        {
            .head   = 
            {
                .capacity       = eOas_ft_sensors_maxnumber,
                .itemsize       = sizeof(eOas_ft_sensordescriptor_t),
                .size           = numberofFTs2test,
                .internalmem    = 0                    
            },
            .data   =
            {
                {   // 0
                    .boardinfo =
                    {
                        .type = eobrd_strain2, 
                        .firmware = {0, 0, 0},
                        .protocol = {2, 0}                      
                    },
                    .canloc = 
                    {
                        .port = eOcanport1, 
                        .addr = 13, 
                        .insideindex = eobrd_caninsideindex_none                    
                    },
                    .ffu = 0
                },
                {   // 1
                    .boardinfo =
                    {
                        .type = eobrd_strain2, 
                        .firmware = {0, 0, 0},
                        .protocol = {2, 0}                      
                    },
                    .canloc = 
                    {
                        .port = eOcanport2, 
                        .addr = 13, 
                        .insideindex = eobrd_caninsideindex_none                    
                    },
                    .ffu = 0
                },
                {   // 2
                    .boardinfo =
                    {
                        .type = eobrd_none, 
                        .firmware = {0, 0, 0},
                        .protocol = {0, 0}                    
                    },
                    .canloc = 
                    {
                        .port = eOcanport1, 
                        .addr = 2, 
                        .insideindex = eobrd_caninsideindex_none                    
                    },
                    .ffu = 0
                },
                {   // 3
                    .boardinfo =
                    {
                        .type = eobrd_none, 
                        .firmware = {0, 0, 0},
                        .protocol = {0, 0}                    
                    },
                    .canloc = 
                    {
                        .port = eOcanport1, 
                        .addr = 2, 
                        .insideindex = eobrd_caninsideindex_none                    
                    },
                    .ffu = 0
                }            
            }
        }
    }}}    
};

eOas_ft_config_t ftconfig[eOas_ft_sensors_maxnumber] =
{
    {
        .mode = eoas_ft_mode_calibrated,
        .ftperiod = 50,
        .calibrationset = 0,
        .temperatureperiod = 1
    },
    {
        .mode = eoas_ft_mode_raw,
        .ftperiod = 50,
        .calibrationset = 0,
        .temperatureperiod = 0
    }, 
    {
        .mode = eoas_ft_mode_raw,
        .ftperiod = 50,
        .calibrationset = 0,
        .temperatureperiod = 0
    },
    {
        .mode = eoas_ft_mode_raw,
        .ftperiod = 50,
        .calibrationset = 0,
        .temperatureperiod = 0
    }            
}; 

void setconfig()
{
    for(uint8_t i=0; i<numberofFTs2test; i++)
    {
        eOropdescriptor_t ropdes = {};
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, i, eoprot_tag_as_ft_config);
        embot::app::eth::fill(ropdes, id32, &ftconfig[i], sizeof(ftconfig));    
        embot::app::eth::theFTservice::getInstance().process(&ropdes);   
    }       
}

void txstart()
{
    for(uint8_t i=0; i<numberofFTs2test; i++)
    {
        uint8_t enable = 1;  
        eOropdescriptor_t ropdes = {};
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, i, eoprot_tag_as_ft_cmmnds_enable);
        embot::app::eth::fill(ropdes, id32, &enable, sizeof(enable));    
        embot::app::eth::theFTservice::getInstance().process(&ropdes);  
    }       
}

constexpr embot::app::eth::theServiceTester::Config FTservtestconfig =
{
    eomn_serv_category_ft,
    &s_serv_config_as_ft_001,
    setconfig,
    txstart,
    numberofFTs2test, // 2    
    {{ 
        EOPROT_ID_GET(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, 0, eoprot_tag_as_ft_status_timedvalue), 
        EOPROT_ID_GET(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, 1, eoprot_tag_as_ft_status_timedvalue),
        EOPROT_ID_GET(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, 2, eoprot_tag_as_ft_status_timedvalue),
        EOPROT_ID_GET(eoprot_endpoint_analogsensors, eoprot_entity_as_ft, 3, eoprot_tag_as_ft_status_timedvalue),
        eo_prot_ID32dummy, eo_prot_ID32dummy, eo_prot_ID32dummy, eo_prot_ID32dummy
    }}   
};

const theServiceTester::Config & embot::app::eth::theFTservice::Impl::servicetesterconfig() const
{
   return FTservtestconfig;
}

#else

const theServiceTester::Config & embot::app::eth::theFTservice::Impl::servicetesterconfig() const
{
   static constexpr embot::app::eth::theServiceTester::Config FTservtestconfig {};
   return FTservtestconfig;
}

#endif

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
        

eOresult_t embot::app::eth::theFTservice::initialise(const Config &config)
{
    pImpl->config = config;
    return pImpl->initialise();
}

eOresult_t embot::app::eth::theFTservice::Tick()
{
    return pImpl->Tick();
}


eOresult_t embot::app::eth::theFTservice::SendReport()
{
    return pImpl->SendReport();
}

eOresult_t embot::app::eth::theFTservice::Verify(const eOmn_serv_configuration_t * servcfg, 
                                                    eOservice_onendofoperation_fun_t onverify, 
                                                    void *arg,
                                                    bool activateafterverify)
{
    return pImpl->Verify(servcfg, onverify, arg, activateafterverify); 
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

eOresult_t embot::app::eth::theFTservice::process(const eOropdescriptor_t* rd, const EOnv* nv)
{
    return pImpl->process(rd, nv);
}

bool embot::app::eth::theFTservice::set(eOprotIndex_t index, const eOas_ft_config_t *ftc)
{
    return pImpl->set(index, ftc);
}

bool embot::app::eth::theFTservice::enable(eOprotIndex_t index, const uint8_t *cmdenable)
{
    return pImpl->enable(index, cmdenable);
}

const embot::app::eth::theServiceTester::Config & embot::app::eth::theFTservice::servicetesterconfig() const
{
    return pImpl->servicetesterconfig();
}

// other objects


// - extern C functions ....


extern "C"
{
    
    extern void eoprot_fun_UPDT_as_ft_config(const EOnv* nv, const eOropdescriptor_t* rd)
    {
        embot::app::eth::theFTservice::getInstance().process(rd, nv);
    }

    extern void eoprot_fun_UPDT_as_ft_cmmnds_enable(const EOnv* nv, const eOropdescriptor_t* rd)
    {
        embot::app::eth::theFTservice::getInstance().process(rd, nv);
    }  

    extern void eoprot_fun_INIT_as_ft_config(const EOnv* nv)
    {
        eOas_ft_config_t* config = reinterpret_cast<eOas_ft_config_t*>(eo_nv_RAM(nv));
        std::memmove(config, &embot::app::eth::theFTservice::defaultFTconfig, sizeof(eOas_ft_config_t));
    }

    extern void eoprot_fun_INIT_as_ft_status(const EOnv* nv)
    {
        eOas_ft_status_t *status = reinterpret_cast<eOas_ft_status_t*>(eo_nv_RAM(nv));        
        std::memmove(status, &embot::app::eth::theFTservice::defaultFTstatus, sizeof(eOas_ft_status_t));
    }    
    
} // extern "C"


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


