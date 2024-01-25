
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#ifndef __EMBOT_APP_ETH_SERVICE_IMPL_MC_FOC_H_
#define __EMBOT_APP_ETH_SERVICE_IMPL_MC_FOC_H_


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_Service_impl_mc_if.h"


    
namespace embot::app::eth::service::impl::mc {    

struct mcOBJfoc : public IFmcobj
{ 
    static const size_t maxjomos {4};
    
    embot::app::eth::Service *service {nullptr};
    embot::app::eth::service::impl::Core *p2core {nullptr};
    const eOmn_serv_configuration_t *servconfig {nullptr};  // points to a eOmn_serv_configuration_t object that stays in embot::app::eth::service::impl::Core::serviceconfig
    embot::app::eth::Service::OnEndOfOperation afterverifyactivate {nullptr};
    
    const eOmn_serv_config_data_mc_foc_t *foc {nullptr};    // points to ... servconfig that points to ...   
    EOconstarray* jomodescriptors {nullptr};                // points to ... servconfig that points to ...
    size_t numofjomos {0};
    
    embot::app::eth::service::impl::CANtools cantools {};
        
    embot::app::eth::theErrorManager::Descriptor desc {};
        
    
    mcOBJfoc() = default;
    
#if 0

    verification is done in two steps:
    - 01: encoders
    - 02: presence of can boards (foc or amcbldc)    
    
    
#endif
    
    void initialise(embot::app::eth::service::impl::Core *c)
    {
        p2core = c;
        
        size_t nboards {4};
        size_t nentities {4};
        size_t ntargets {1};
        cantools.initialise(nboards, nentities, ntargets);
        
        clear();        
    }
    
    void emit(embot::app::eth::theErrorManager::Severity s, eOerror_code_t errorcode)
    {
        desc.code = errorcode;
        embot::app::eth::theErrorManager::Caller cllr {"mcOBJfoc", embot::os::theScheduler::getInstance().scheduled()};
        embot::app::eth::theErrorManager::getInstance().emit(s, cllr, desc, "");
    }  

    embot::app::eth::Service::Type type() const override
    { 
        return embot::app::eth::Service::Type::MC_foc; 
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
            const eOmc_jomo_descriptor_t *jomodes = reinterpret_cast<const eOmc_jomo_descriptor_t*>(eo_constarray_At(jomodescriptors, i));
#if defined(useMCfoc_actuator_descriptor_generic)
            eObus_t bus = static_cast<eObus_t>(jomodes->actuator.gen.location.bus);
error            if((eobus_can1 != bus) && (eobus_can2 != bus))
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
            cantools.ondiscoverystop.function = mcOBJfoc::step02_onENDof_candiscovery;
            cantools.ondiscoverystop.parameter = this;
        }
        
        return r;        
    }

    
    size_t numberofjomos() const override
    {
       return numofjomos; 
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
        p2core->state(state);        
        
        // maybe put in clear() also entities etc. 
        #warning even better manage entities elsewhere ...
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
            const eOmc_jomo_descriptor_t *jomodes = reinterpret_cast<const eOmc_jomo_descriptor_t*>(eo_constarray_At(jomodescriptors, i));
            
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
            const eOmc_jomo_descriptor_t *jomodes = reinterpret_cast<const eOmc_jomo_descriptor_t*>(eo_constarray_At(jomodescriptors, i));            
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
 
        
        // synch 
        embot::app::eth::Service::State state {embot::app::eth::Service::State::activated};
        p2core->state(state);
        embot::app::eth::theServices::getInstance().synch(embot::app::eth::Service::Category::mc, state);     
    

        return r;                
    }
        
    
    // static methods used for callbacks: void *tHIS must be a pointer to this mcOBJfoc struct
    static void step01_onENDof_verifyencoders(void *tHIS, bool operationisok);
    static eOresult_t step02_onENDof_candiscovery(void *tHIS, EOtheCANdiscovery2* cd2, eObool_t searchisok);

}; 


void mcOBJfoc::step01_onENDof_verifyencoders(void *tHIS, bool operationisok)
{  
    mcOBJfoc *mcfoc = reinterpret_cast<mcOBJfoc*>(tHIS);  

    if(false == operationisok)
    {
        // the encoder reader fails. we dont even start the discovery of the foc boards.
        
        // 1. assign new state              
        embot::app::eth::Service::State state {embot::app::eth::Service::State::failureofverify};
        mcfoc->p2core->state(state);
        embot::app::eth::theServices::getInstance().synch(embot::app::eth::Service::Category::mc, state);


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

    
    // surely we have can boards to discover because we checked inside mcOBJfoc::load( ... )
    // so, we start can discovery and exit.
    // it will be mcfoc->cantools.ondiscoverystop and in particular 
    // step02_onENDof_candiscovery() that will be called next
    
    eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), mcfoc->cantools.discoverytargets, &mcfoc->cantools.ondiscoverystop);  
    
    return;

}  

eOresult_t mcOBJfoc::step02_onENDof_candiscovery(void *tHIS, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{
    eOresult_t r {eores_OK}; // always OK is fine as well, as nobody checks it    
    
    mcOBJfoc *mcfoc = reinterpret_cast<mcOBJfoc*>(tHIS);

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
    mcfoc->p2core->state(state);
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
    

} // namespace embot::app::eth::service::impl::mc { 


#endif // #define __EMBOT_APP_ETH_SERVICE_IMPL_MC_FOC_H_

 


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


