
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#ifndef __EMBOT_APP_ETH_THESERVICEMC_IMPL_H_
#define __EMBOT_APP_ETH_THESERVICEMC_IMPL_H_

#include "embot_app_eth_theServiceMC.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_Service_impl.h"

#include "embot_prot_can_motor_periodic.h"

#include "EOtheCANprotocol.h"
#include "EOtheCANmapping.h"


#include "embot_app_eth_theErrorManager.h"
#include "EoError.h"
#include "EOtheErrorManager.h"

#include "embot_os_theScheduler.h"

#include "EoProtocolMC.h"
#include "EOtheEntities.h"
#include "Controller.h"
#include "embot_app_eth_theEncoderReader.h"
#include "embot_app_eth_theICCmapping.h"
#include "embot_app_eth_theServices.h"

    
namespace embot::app::eth {    

    // or other namespace such as embot::app::eth::service::impl::mc {  
 
struct IFmcobj 
{
    virtual bool clear() = 0;    
    virtual bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc) = 0;
    virtual bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend) = 0;
    virtual bool deactivate() = 0;
    
protected:
    virtual ~IFmcobj() {};    // cannot delete from interface but only from derived object 
};

struct MCnone : public IFmcobj
{       
    MCnone() = default;
        
    bool clear() override 
    { return false; }

    
    bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc) override
    { return false; }
    
    bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend) override
    { return false; }
    
    bool deactivate() override
    { return false; }
    
};
    
struct MCfoc : public IFmcobj
{ 
    static const uint8_t maxjomos {4};
    
    embot::app::eth::Service *service {nullptr};
    const eOmn_serv_configuration_t *servconfig {nullptr};  // points to ... external ram
    embot::app::eth::Service::OnEndOfOperation afterverifyactivate {nullptr};
    
    const eOmn_serv_config_data_mc_foc_t *foc {nullptr};    // points to ... servconfig that points to external ram   
    EOconstarray* jomodescriptors {nullptr};                // points to ... servconfig that points to external ram
    uint8_t numofjomos {0};
    
    embot::app::eth::service::impl::CANtools cantools {};
        
    embot::app::eth::theErrorManager::Descriptor desc {};
        
    
    MCfoc() = default;
    
#if 0

    verification is done in two steps:
    - 01: encoders
    - 02: presence of can boards (foc or amcbldc)    
    
    
#endif
    
    void initialise()
    {
        size_t nboards {4};
        size_t nentities {4};
        size_t ntargets {1};
        cantools.initialise(nboards, nentities, ntargets);
        
        clear();        
    }
    
    void emit(embot::app::eth::theErrorManager::Severity s, eOerror_code_t errorcode)
    {
        desc.code = errorcode;
        embot::app::eth::theErrorManager::Caller cllr {"MCfoc", embot::os::theScheduler::getInstance().scheduled()};
        embot::app::eth::theErrorManager::getInstance().emit(s, cllr, desc, "");
    }  

    
    bool clear() override
    {
        bool r {true};
        
        servconfig = nullptr;
        foc = nullptr;
        jomodescriptors = nullptr;
        numofjomos = 0;
        cantools.clear();
        
        desc.clear();
        return r;
    }
    
    
    
    
    bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc) override
    {
        bool r {true};
        
        #warning TOBEDONE: assign a dummy service if .... nullptr
        service = serv;
        
        servconfig = sc;
        foc = &sc->data.mc.foc_based;        
        jomodescriptors = eo_constarray_Load(reinterpret_cast<const EOarray*>(&foc->arrayofjomodescriptors));
        numofjomos = eo_constarray_Size(jomodescriptors);
        
        if((0 == numofjomos) || (numofjomos > maxjomos))
        {
            clear();
            return false;
        } 

        // prepare the can discovery for foc boards
        // in this service type we have only one type of board but up to 4 of them at different addresses         
        eOcandiscovery_target_t trgt = {0};
        trgt.info.type = foc->type;
        trgt.info.protocol.major = foc->version.protocol.major; 
        trgt.info.protocol.minor = foc->version.protocol.minor;
        trgt.info.firmware.major = foc->version.firmware.major; 
        trgt.info.firmware.minor = foc->version.firmware.minor;   
        trgt.info.firmware.build = foc->version.firmware.build;   
                
        // if it is NOT defined useMCfoc_actuator_descriptor_generic (as default)
        // then every jomo is managed by a can board (not an icc location). 
        // nevertheless we always count the numofcanjomos
        
        uint8_t numofcanjomos {0};
        
        for(uint8_t i=0; i<numofjomos; i++)
        {
            const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(jomodescriptors, i);
#if defined(useMCfoc_actuator_descriptor_generic)
            eObus_t bus = static_cast<eObus_t>(jomodes->actuator.gen.location.bus);
            if((eobus_can1 != bus) && (eobus_can2 != bus))
            {
                // we consider only actuators located on can
                continue;
            } 
            numofcanjomos++;            
            eo_common_hlfword_bitset(&trgt.canmap[jomodes->actuator.gen.location.bus], jomodes->actuator.gen.location.adr);    
#else
            numofcanjomos++;            
            eo_common_hlfword_bitset(&trgt.canmap[jomodes->actuator.foc.canloc.port], jomodes->actuator.foc.canloc.addr); 
#endif            
        }
        
        if(0 == numofcanjomos)
        {
            // we return because the foc mode must have at least one can board  and i want to be sure in here, so that 
            // when we call step01_onENDof_verifyencoders() we can go on without any other check
            clear();
            return false;                        
        }
        
        // force a cleaned discoverytargets before we add the target
        eo_array_Reset(cantools.discoverytargets); 
        if(numofcanjomos > 0)
        {
            // i push back only once because i have only one type of board to search for           
            eo_array_PushBack(cantools.discoverytargets, &trgt);
            // then i assign the params of end of discovery: tha callback and it argument 
            cantools.ondiscoverystop.function = MCfoc::step02_onENDof_candiscovery;
            cantools.ondiscoverystop.parameter = this;
        }
        
        return r;        
    }
    
    bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend) override
    {
        bool r {true};
        
        afterverifyactivate = onend;
        
        // i assign ... and bla bla bla.
        // i call 
        eOmn_serv_diagn_cfg_t dc = {0, 0};
        const embot::core::Confirmer conf {step01_onENDof_verifyencoders, this};
        embot::app::eth::theEncoderReader::getInstance().Verify({jomodescriptors, dc}, true, conf);
        
        return r;
        
    }
    
    bool deactivate() override
    {
        bool r {true};
        
        eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, cantools.entitydescriptor); 
        eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, cantools.entitydescriptor); 
        
#if defined(useMCfoc_actuator_descriptor_generic)            
        // we should make sure that embot::app::eth::theICCservices unloads its entities
        #warning TODO: much better doing embot::app::eth::theICCmapping::getInstance().clear(motors && joints);
        embot::app::eth::theICCmapping::getInstance().clear();
#endif 
        
        eo_canmap_UnloadBoards(eo_canmap_GetHandle(), cantools.boardproperties); 
        
        embot::app::eth::theEncoderReader::getInstance().Deactivate();
        
        // marco.accame.TODO: call deinit of mcontroller
        
        // now i reset the status of motors etc

        // Reset the communicated fault state to dummy for each motor
        uint8_t n_motors = eo_entities_NumOfMotors(eo_entities_GetHandle());
        
        for(uint8_t mId = 0; mId < n_motors; mId++)
        {
           eOmc_motor_status_t *mstatus = eo_entities_GetMotorStatus(eo_entities_GetHandle(), mId);
           if(NULL != mstatus)
           {
               mstatus->mc_fault_state = eoerror_code_dummy;
           }
        }
        
        // reset other things
        
        eo_entities_SetNumOfJoints(eo_entities_GetHandle(), 0);
        eo_entities_SetNumOfMotors(eo_entities_GetHandle(), 0);


        embot::app::eth::Service::State state {embot::app::eth::Service::State::idle};
        embot::app::eth::theServices::getInstance().synch(embot::app::eth::Service::Category::mc, state);  
        
        // maybe put in clear() also entities etc. 
        #warning even better mange entities elsewhere ...
        clear();  
        
        return r;           
    }
    
    bool activate()
    {
        bool r {true};

        eo_entities_SetNumOfJoints(eo_entities_GetHandle(), numofjomos);
        eo_entities_SetNumOfMotors(eo_entities_GetHandle(), numofjomos);
        
        // should maybe call p->service.active = eobool_true;
        
        // load the can mapping

        for(uint8_t i=0; i<numofjomos; i++)
        {
            const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(jomodescriptors, i);
            
            eObrd_canproperties_t prop = {0};

#if defined(useMCfoc_actuator_descriptor_generic)

            eObus_t bus = static_cast<eObus_t>(jomodes->actuator.gen.location.bus);
            if((eobus_can1 != bus) && (eobus_can2 != bus))
            {
                // object EOtheCANmapping treats only actuators located on can
                continue;
            }
            
            prop.type = p->service.servconfig.data.mc.foc_based.type;
            prop.location.port = jomodes->actuator.gen.location.bus;
            prop.location.addr = jomodes->actuator.gen.location.adr;
            prop.location.insideindex = eobrd_caninsideindex_first;
            prop.requiredprotocol.major = p->service.servconfig.data.mc.foc_based.version.protocol.major;
            prop.requiredprotocol.minor = p->service.servconfig.data.mc.foc_based.version.protocol.minor;
            
            eo_vector_PushBack(p->sharedcan.boardproperties, &prop);
#else 
            
            prop.type = foc->type;
            prop.location.port = jomodes->actuator.foc.canloc.port;
            prop.location.addr = jomodes->actuator.foc.canloc.addr;
            prop.location.insideindex = jomodes->actuator.foc.canloc.insideindex;
            prop.requiredprotocol.major = foc->version.protocol.major;
            prop.requiredprotocol.minor = foc->version.protocol.minor;
            
            eo_vector_PushBack(cantools.boardproperties, &prop);            
#endif
        }
        eo_canmap_LoadBoards(eo_canmap_GetHandle(), cantools.boardproperties); 
        
        // load the entity mapping.
        for(uint8_t i=0; i<numofjomos; i++)
        {
            const eOmc_jomo_descriptor_t *jomodes = (eOmc_jomo_descriptor_t*) eo_constarray_At(jomodescriptors, i);                
            eOcanmap_entitydescriptor_t des = {0};  
            
#if defined(useMCfoc_actuator_descriptor_generic)
            eOlocation_t location = jomodes->actuator.gen.location;
            eObus_t bus = static_cast<eObus_t>(jomodes->actuator.gen.location.bus);
            if((eobus_icc1 == bus) || (eobus_icc2 == bus))
            {
                // if this location is icc then ... we should tell some new object embot::app::eth::theICCservices
                // that this location is associated to the i-th eoprot_entity_mc_motor / joint entity
                embot::app::eth::theICCmapping::getInstance().load({{location}, {eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint}, i});
                embot::app::eth::theICCmapping::getInstance().load({{location}, {eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor}, i});
                continue;
            }

            des.location.port = location.bus;
            des.location.addr = location.adr;
            des.location.insideindex = eobrd_caninsideindex_first;
            des.index = (eOcanmap_entityindex_t)i;

            eo_vector_PushBack(cantools.entitydescriptor, &des); 
            
#else                
                            
            des.location.port = jomodes->actuator.foc.canloc.port;
            des.location.addr = jomodes->actuator.foc.canloc.addr;
            des.location.insideindex = jomodes->actuator.foc.canloc.insideindex;
            des.index = (eOcanmap_entityindex_t)i;

            eo_vector_PushBack(cantools.entitydescriptor, &des);  
#endif          
        }        
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, cantools.entitydescriptor); 
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, cantools.entitydescriptor);        

        // init the encoders
        eOmn_serv_diagn_cfg_t dc = {0, 0};
        embot::app::eth::theEncoderReader::getInstance().Activate({jomodescriptors, dc});
        
        
        MController_config_board(servconfig);
 
        #warning make sure that SErvice::set(State s) also updates internals        
//        p->service.active = eobool_true;
//        p->service.state = eomn_serv_state_activated;
        
        //s_mc_synchservice(p->service.state);   
        embot::app::eth::Service::State state {embot::app::eth::Service::State::activated};
        embot::app::eth::theServices::getInstance().synch(embot::app::eth::Service::Category::mc, state);        
        
        return r;                
    }
        
    
    // static methods used for callbacks: void *p is this
    static void step01_onENDof_verifyencoders(void *tHIS, bool operationisok);
    static eOresult_t step02_onENDof_candiscovery(void *tHIS, EOtheCANdiscovery2* cd2, eObool_t searchisok);

}; 

void MCfoc::step01_onENDof_verifyencoders(void *tHIS, bool operationisok)
{  
    MCfoc *mcfoc = reinterpret_cast<MCfoc*>(tHIS);  

    if(false == operationisok)
    {
        // the encoder reader fails. we dont even start the discovery of the foc boards.
        
        // 1. assign new state        
        #warning VERIFY THAT IS OK
        //eOmn_serv_state_t state = eomn_serv_state_failureofverify;
//        p->service.state = state;
//        s_mc_synchservice(state);        
        embot::app::eth::Service::State state {embot::app::eth::Service::State::failureofverify};
        embot::app::eth::theServices::getInstance().synch(embot::app::eth::Service::Category::mc, state);
        //mcfoc->service->set(state);


        // 2. diagnostics
        embot::app::eth::theErrorManager::Severity s {embot::app::eth::theErrorManager::Severity::error};
        eOerror_code_t errorcode {eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_failed_encoders_verify)};      
        mcfoc->emit(s, errorcode);
        
        // 3. on verify
        if(nullptr != mcfoc->afterverifyactivate)
        {
            mcfoc->afterverifyactivate(mcfoc->service, false);            
        }
        
        // 4. return
                 
        return;
    }

    
    // surely we have can boards to discover because we checked inside MCfoc::load( ... )
    // so, we start can discovery and exit.
    // it will be mcfoc->cantools.ondiscoverystop and in particular 
    // step02_onENDof_candiscovery() that will be called next
    
    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), mcfoc->cantools.discoverytargets, &mcfoc->cantools.ondiscoverystop);  
    
    return;

}  

eOresult_t MCfoc::step02_onENDof_candiscovery(void *tHIS, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{
    eOresult_t r {eores_OK}; // always OK is fine as well, as nobody checks it    
    
    MCfoc *mcfoc = reinterpret_cast<MCfoc*>(tHIS);

    // it is the final step, so: i get the params ...
    
    //eOmn_serv_state_t state {eomn_serv_state_verified};
    embot::app::eth::Service::State state {embot::app::eth::Service::State::verified};
    bool OK {true};
    embot::app::eth::theErrorManager::Severity s {embot::app::eth::theErrorManager::Severity::debug};
    eOerror_code_t errorcode {eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_ok)};
 
    if(eobool_false == searchisok)
    {
        //state = eomn_serv_state_failureofverify;
        state = embot::app::eth::Service::State::failureofverify;
        OK = false;
        s = embot::app::eth::theErrorManager::Severity::error;
        errorcode = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_failed_candiscovery_of_foc);        
    }
      
    // ... and i use them
    
    // 1. assign new state
    #warning TOBECHECKED
//    p->service.state = state;
//    s_mc_synchservice(state);
    //mcfoc->service->set(state);  
    embot::app::eth::theServices::getInstance().synch(embot::app::eth::Service::Category::mc, state);
    
    
    // 2. send diagnostics
    mcfoc->emit(s, errorcode);
    
    // 3. activate if needed
    if(OK)
    {
        mcfoc->activate();
    }    
        
    // 4. callback ...
    if(nullptr != mcfoc->afterverifyactivate)
    {
        mcfoc->afterverifyactivate(mcfoc->service, OK);            
    }
        
        
    return r;
}
    

struct MCadvfoc : public IFmcobj
{    
    
    MCadvfoc() = default;
    
#if 0

    verification is done in two steps:
    - 01: encoders
    - 02: presence of icc resources
    - 03: presence of can boards 
    
    
#endif
    
    
    bool clear() override
    {
        bool r {true};
        
        return r;
    }
    
    
    bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc) override
    {
        bool r {true};
                       
        return r;        
    }
    
    bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend) override
    {
        bool r {true};
        
//        void *owner {nullptr};
//        // i assign ... and bla bla bla.
//        // i call 
//        EOconstarray *carrayofjomodes {nullptr};
//        eOmn_serv_diagn_cfg_t dc = {0, 0};
//        const embot::core::Confirmer conf {step01_onENDof_verifyencoders, owner};
//        embot::app::eth::theEncoderReader::getInstance().Verify({carrayofjomodes, dc}, true, conf);
        
        return r;
        
    }
    
    bool deactivate() override
    {
        bool r {true};
        
        
        
                       
        return r;           
    }

}; 



struct MCobjects
{
    static constexpr size_t maxjomos {embot::app::eth::theServiceMC::maxJOMOs};
    
    std::array<eOmc_joint_t*, maxjomos> joints {nullptr};
    std::array<eOmc_motor_t*, maxjomos> motors {nullptr};

    MCnone mcnone {};    
    MCfoc mcfoc {};
    MCfoc mcadvfoc {};
    
    IFmcobj *mcobj {&mcnone};
    
    
    EOconstarray*                           jomodescriptors {nullptr};      // points to the jomodescriptor inside EOtheMotionController_hid::service::servconfig etc. 
    EOconstarray*                           advjomodescriptors {nullptr};   // point to the eOmc_arrayof_4advjomodescriptors_t inside ....
    std::vector<eObrd_info_t>               canboards {};
    std::vector<eObrd_info_t>               iccboards {};
    uint8_t                                 numofjomos {0}; 
    embot::app::eth::Service::Type          mcservicetype {embot::app::eth::Service::Type::none};
    
    MCobjects() = default;
    
    
    void initialise()
    {
        mcobj = &mcnone;
        
        mcfoc.initialise();
        
        numofjomos = 0;
        jomodescriptors = advjomodescriptors = nullptr;
        canboards.reserve(maxjomos);
        iccboards.reserve(maxjomos); 
        mcservicetype = embot::app::eth::Service::Type::none;
        for(size_t i=0; i<maxjomos; i++)
        {
            joints[i] = reinterpret_cast<eOmc_joint_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i));
            motors[i] = reinterpret_cast<eOmc_motor_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, i));
        }
    }
    
    void clear()
    {
        mcobj = &mcnone;
        
        numofjomos = 0;
        jomodescriptors = advjomodescriptors = nullptr;   
        canboards.clear();
        iccboards.clear(); 
        mcservicetype = embot::app::eth::Service::Type::none;        
    }
    
    bool load(embot::app::eth::Service::Type type, embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc)
    {
        bool r {true};
        
        mcservicetype = type;
        
        switch(mcservicetype)
        {
            case embot::app::eth::Service::Type::MC_foc:
            {
                mcobj = &mcfoc;             
            } break;

            case embot::app::eth::Service::Type::MC_advfoc:
            {
                mcobj = &mcadvfoc;                
            } break; 

            default:
            {
                mcobj = &mcnone;
            } break;
        }
        
        r = mcobj->load(serv, sc);        
                
        return r;
    }
    
    bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend)
    {
        return mcobj->verifyactivate(onend);        
    }   


    bool deactivate()
    {
        return mcobj->deactivate(); 
    }    
    
};

// it holds specific data for the service
struct MCservice
{
    embot::app::eth::service::impl::Core core {};
    MCobjects mcobjects {};
        
    MCservice() = default;
    
    void initialise(size_t nboards, size_t ntargets, size_t nentities, size_t maxregs)
    {
        if(true == core.paraphernalia.initted)
        {
            return;
        }
        
        // core
        //const size_t nboards {maxjomos};
        //const size_t nentities {maxjomos};
        //const size_t ntargets {maxjomos};
        core.initialise(nboards, nentities, ntargets, maxregs);
        
        // mcobjects
        mcobjects.initialise();       
    }

    void clear()
    {
        core.clear();
        mcobjects.clear();
    }

    bool active() const
    {
        return core.paraphernalia.active;
        //return supported(mcobjects.mcservicetype);
    }  
    
    void active(bool yes) 
    {
        core.paraphernalia.active = yes;
    }

    bool started() const
    {
        return core.paraphernalia.started;
        //return supported(mcobjects.mcservicetype);
    }  
    
    void started(bool yes) 
    {
        core.paraphernalia.started = yes;
    }
    
    embot::app::eth::Service::State state() const
    {
        return core.paraphernalia.state;
    }
    
    void state(embot::app::eth::Service::State s)
    {
        core.paraphernalia.state = s;
    }
    
    void synch()
    {
        embot::app::eth::Service::State ss {core.paraphernalia.state};
        embot::app::eth::theServices::getInstance().synch(embot::app::eth::Service::Category::mc, ss); 
    }
    
    
    embot::app::eth::Service::Type type() const
    {
        return static_cast<embot::app::eth::Service::Type>(core.paraphernalia.servconfig.type);
    }
    
     
    
    bool load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc)
    {
        // it gets a sc, checks if it is supported, inits all the required values in here
        // if everything is OK it returns true, else false
        bool r {false};
        
        if(false == supported(sc))
        {            
            r = false;
            clear();
            return r;            
        }
        
        // ok, we can proceed: copy service configuration into internal memory 
        std::memmove(&core.paraphernalia.servconfig, sc, sizeof(core.paraphernalia.servconfig));
        
        // then load what we need
        r = mcobjects.load(static_cast<embot::app::eth::Service::Type>(core.paraphernalia.servconfig.type), serv, &core.paraphernalia.servconfig);
        
        if(false == r)
        {
            clear();
        }
        
        return r;        
    }


    bool verifyactivate(embot::app::eth::Service::OnEndOfOperation onend)
    {
        // it gets a sc, checks if it is supported, inits all the required values in here
        // if everything is OK it returns true, else false
        bool r {false};
        
        r = mcobjects.mcobj->verifyactivate(onend);

        if(false == r)
        {
            // also call failure of verify ... maybe
            clear();
        }
        
        return r;        
    }    
    
    bool deactivate()
    {
        bool r {true};
        
        mcobjects.mcobj->deactivate();

        clear();
        
        return r;          
    }


    // static methods

    static bool supported(embot::app::eth::Service::Type t)
    {
        return (embot::app::eth::Service::Type::MC_advfoc == t) || (embot::app::eth::Service::Type::MC_foc == t);
    }
    
    static bool supported(const eOmn_serv_configuration_t *sc)
    {
        if(nullptr == sc)
        {
            return false;
        }

        return supported(static_cast<embot::app::eth::Service::Type>(sc->type));
    }    
      
};

} // namespace embot::app::eth {  


#endif // #define __EMBOT_APP_ETH_THESERVICEMC_IMPL_H_




//// static callbacks
//struct verifyFOC
//{
//    
//    static bool start(void *owner)
//    {
//        // i assign ... and bla bla bla.
//        // i call 
//        EOconstarray *carrayofjomodes {nullptr};
//        eOmn_serv_diagn_cfg_t dc = {0, 0};
//        const embot::core::Confirmer conf {step01_onENDof_verifyencoders, owner};
//        embot::app::eth::theEncoderReader::getInstance().Verify({carrayofjomodes, dc}, true, conf);
//        
//        return true;
//        
//    }

//    static void step01_onENDof_verifyencoders(void *p, bool operationisok)
//    {    
////            EOtheMotionController* p = &s_eo_themotcon;
////            
////            if(true == operationisok)
////            {
////                // do we have can boards to discover?
////                #warning SEE IF WE HAVE
////         
////                EOconstarray * a = reinterpret_cast<EOconstarray*>(p->sharedcan.discoverytargets);
////                if(0 != eo_constarray_Size(a))
////                {
////                    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), p->sharedcan.discoverytargets, &p->sharedcan.ondiscoverystop);  
////                } 
////                else 
////                {   // we must have at least one local actuators
////                    bool searchisok = checklocalactuators();
////                    p->service.state = (searchisok) ? (eomn_serv_state_verified) : (eomn_serv_state_failureofverify);
////                    s_mc_synchservice(p->service.state);    

////                    if((true == searchisok) && (eobool_true == p->service.activateafterverify))
////                    {
////                        const eOmn_serv_configuration_t * mcserv = &p->service.servconfig; // or also: (const eOmn_serv_configuration_t *)par;
////                        eo_motioncontrol_Activate(p, mcserv);
////                    }                

////                    if(NULL != p->service.onverify)
////                    {
////                        p->service.onverify(p->service.onverifyarg, searchisok ? eobool_true : eobool_false); 
////                    }              
////                }
////            }    
////            else
////            {
////                // the encoder reader fails. we dont even start the discovery of the foc boards. we just issue an error report and call onverify() w/ false argument
////                
////                p->service.state = eomn_serv_state_failureofverify;
////                s_mc_synchservice(p->service.state);
////                
////                // prepare things
////                p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
////                p->diagnostics.errorDescriptor.sourceaddress    = 0;
////                p->diagnostics.errorDescriptor.par16            = 0;
////                p->diagnostics.errorDescriptor.par64            = 0;    
////                EOaction_strg astrg = {0};
////                EOaction *act = (EOaction*)&astrg;
////                eo_action_SetCallback(act, s_eo_motioncontrol_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
////                

////                // fill error description. and transmit it
////                p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_failed_encoders_verify);
////                p->diagnostics.errorType = eo_errortype_error;                
////                eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
////                
////                if(0 != p->diagnostics.reportPeriod)
////                {
////                    p->diagnostics.errorCallbackCount = EOK_int08dummy;
////                    eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
////                }
////          
////                // call onverify
////                if(NULL != p->service.onverify)
////                {
////                    p->service.onverify(p->service.onverifyarg, eobool_false); 
////                }            
////                        
////            }   
//    }  
//    
//    static eOresult_t step02_onENDof_search4focs(void *par, EOtheCANdiscovery2* cd2, eObool_t searchisok)
//    {
//        
//        //par will be impl or something like that

////            EOtheMotionController* p = &s_eo_themotcon;
////            
////            if(eobool_true == searchisok)
////            {
////                p->service.state = eomn_serv_state_verified;
////                s_mc_synchservice(p->service.state);
////            }
////            else
////            {   
////                p->service.state = eomn_serv_state_failureofverify;
////                s_mc_synchservice(p->service.state);
////            }    
////            
////            if((eobool_true == searchisok) && (eobool_true == p->service.activateafterverify))
////            {
////                const eOmn_serv_configuration_t * mcserv = &p->service.servconfig; // or also: (const eOmn_serv_configuration_t *)par;
////                eo_motioncontrol_Activate(p, mcserv);
////            }
////            
////            p->diagnostics.errorDescriptor.sourcedevice     = eo_errman_sourcedevice_localboard;
////            p->diagnostics.errorDescriptor.sourceaddress    = 0;
////            p->diagnostics.errorDescriptor.par16            = 0;
////            p->diagnostics.errorDescriptor.par64            = 0;    
////            EOaction_strg astrg = {0};
////            EOaction *act = (EOaction*)&astrg;
////            eo_action_SetCallback(act, s_eo_motioncontrol_send_periodic_error_report, p, eov_callbackman_GetTask(eov_callbackman_GetHandle()));    
////            
////            if(eobool_true == searchisok)
////            {        
////                p->diagnostics.errorType = eo_errortype_debug;
////                p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_ok);
////                eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
////                
////                if((0 != p->diagnostics.repetitionOKcase) && (0 != p->diagnostics.reportPeriod))
////                {
////                    p->diagnostics.errorCallbackCount = p->diagnostics.repetitionOKcase;        
////                    eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
////                }
////            } 

////            if(eobool_false == searchisok)
////            {
////                p->diagnostics.errorDescriptor.code = eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_failed_candiscovery_of_foc);
////                p->diagnostics.errorType = eo_errortype_error;                
////                eo_errman_Error(eo_errman_GetHandle(), p->diagnostics.errorType, NULL, s_eobj_ownname, &p->diagnostics.errorDescriptor);
////                
////                if(0 != p->diagnostics.reportPeriod)
////                {
////                    p->diagnostics.errorCallbackCount = EOK_int08dummy;
////                    eo_timer_Start(p->diagnostics.reportTimer, eok_abstimeNOW, p->diagnostics.reportPeriod, eo_tmrmode_FOREVER, act);
////                }
////            }     
////                   
////            if(NULL != p->service.onverify)
////            {
////                p->service.onverify(p->service.onverifyarg, searchisok); 
////            }    
//        
//        return(eores_OK);   
//    }

//};     


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


