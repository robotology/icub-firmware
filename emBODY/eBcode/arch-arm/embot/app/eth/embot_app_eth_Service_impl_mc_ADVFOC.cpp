
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_app_eth_Service_impl_mc_ADVFOC.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_app_eth_theErrorManager.h"
#include "embot_os_theScheduler.h"


#include "EoProtocolMC.h"
#include "EOtheEntities.h"
#include "Controller.h"
#include "embot_app_eth_theEncoderReader.h"
#include "embot_app_eth_theICCmapping.h"
#include "embot_app_eth_theServices.h"

#include "EOarray.h"

namespace embot::app::eth::service::impl::mc {    



void mcOBJadvfoc::initialise(embot::app::eth::service::impl::Core *c)
{
    p2core = c;
    
    size_t nboards {4};
    size_t nentities {4};
    size_t ntargets {1};
    cantools.initialise(nboards, nentities, ntargets);
    
    iccactuators.reserve(nboards);
    canactuators.reserve(nboards);
    
    // and now i init several things related to legacy foc mode
    std::memset(&focservconfig, 0, sizeof(eOmn_serv_configuration_t));
    focservconfig.type = eomn_serv_MC_advfoc;
    eOmn_serv_config_data_mc_foc_t *foc = &focservconfig.data.mc.foc_based;
    focjomodescriptors = eo_array_New(4, sizeof(eOmc_jomo_descriptor_t), &foc->arrayofjomodescriptors);    
    
    clear();        
}

void mcOBJadvfoc::emit(embot::app::eth::theErrorManager::Severity s, eOerror_code_t errorcode)
{
    desc.code = errorcode;
    embot::app::eth::theErrorManager::Caller cllr {"mcOBJadvfoc", embot::os::theScheduler::getInstance().scheduled()};
    embot::app::eth::theErrorManager::getInstance().emit(s, cllr, desc, "");
}  

embot::app::eth::Service::Type mcOBJadvfoc::type() const
{ 
    return embot::app::eth::Service::Type::MC_foc; 
}

bool mcOBJadvfoc::clear()
{
    bool r {true};
    
    servconfig = nullptr;
    advfoc = nullptr;
    advjomodescriptors = nullptr;
    numofjomos = 0;
    cantools.clear();
    
    iccactuators.clear();
    canactuators.clear();
    
    std::memset(&foc->jomocoupling, 0, sizeof(eOmc_4jomo_coupling_t));
    eo_array_Reset(focjomodescriptors);      
    
    triggeractivate = false;
    
    desc.clear();
    return r;
}
   
void mcOBJadvfoc::synch(embot::app::eth::Service::State s)
{
    p2core->state(s);
    embot::app::eth::theServices::getInstance().synch(embot::app::eth::Service::Category::mc, s);     
}

bool mcOBJadvfoc::load(embot::app::eth::Service *serv, const eOmn_serv_configuration_t *sc)
{
    bool r {true};
    
    if((nullptr == serv) || (nullptr == sc))
    {
        return false;
    }
    
    // at first i clear, then i init
    clear();
    
    service = serv;
    
    servconfig = sc;
    advfoc = &sc->data.mc.advfoc;         
    advjomodescriptors = eo_constarray_Load(reinterpret_cast<const EOarray*>(&advfoc->arrayof4advjomodescriptors));
    numofjomos = eo_constarray_Size(advjomodescriptors);
    
    if((0 == numofjomos) || (numofjomos > maxjomos))
    {
        clear();
        return false;
    } 

    
    // copy into the foc jomocoupling
    std::memmove(&foc->jomocoupling, &advfoc->adv4jomocoupling.data.traditional, sizeof(eOmc_4jomo_coupling_t));
    
    // now i iterate advjomodescriptors to build up suitable data structure for actuators an encoders:
    for(uint8_t i=0; i<numofjomos; i++)
    {
        const eOmc_adv_jomo_descriptor_t *advjomodes = reinterpret_cast<const eOmc_adv_jomo_descriptor_t*>(eo_constarray_At(advjomodescriptors, i));

//        // every advjomodes->actuator.type must be eomc_act_foc
//        if(eomc_act_foc != advjomodes->actuator.type)
//        {
//            break;
//        }
             
        // actuators: i need the icc and the can based on a separate container 
        if((eobus_icc1 == advjomodes->actuator.location.bus) || (eobus_icc2 == advjomodes->actuator.location.bus))
        {
            iccactuators.push_back({i, advjomodes->actuator});
        }
        else if((eobus_can1 == advjomodes->actuator.location.bus) || (eobus_can2 == advjomodes->actuator.location.bus))
        {
            canactuators.push_back({i, advjomodes->actuator});
        }
        
        // i fill focjomodescriptors for the encoderreader and mcontroller
        eOmc_jomo_descriptor_t jd {};              
        jd.actuator.gen.location = advjomodes->actuator.location;   
        jd.encoder1 = {advjomodes->encoder1.type, advjomodes->encoder1.port, advjomodes->encoder1.pos};   
        jd.encoder2 = {advjomodes->encoder2.type, advjomodes->encoder2.port, advjomodes->encoder2.pos};               
        eo_array_PushBack(focjomodescriptors, &jd);                  
    } 

    // we need all valid actuators
    if((canactuators.size()+iccactuators.size()) != numofjomos)
    {
        clear();
        return false;        
    }
    
    

    // prepare the can discovery for can boards
    // in this service type we could have 4 different types of CAN boards 
    // but for now i dont manage that and i use one type only: the first one
    
    if(false == canactuators.empty())
    {        
        eOcandiscovery_target_t trgt = {0};
        trgt.info = canactuators[0].actua.board;  
                
        for(uint8_t i=0; i<canactuators.size(); i++)
        {
            uint8_t port = (eobus_can1 == canactuators[i].actua.location.bus) ? eOcanport1 : eOcanport2; // we already know it is can
            uint8_t addr = canactuators[i].actua.location.adr;
            eo_common_hlfword_bitset(&trgt.canmap[port], addr);
        }

        
        // force a cleaned discoverytargets before we add the target
        eo_array_Reset(cantools.discoverytargets); 

        // i push back only once because i have only one type of board to search for    
        // in case of more types of board ... we need something else but for now it is OK        
        eo_array_PushBack(cantools.discoverytargets, &trgt);

        // then i assign the params of end of discovery: tha callback and it argument 
        cantools.ondiscoverystop.function = mcOBJadvfoc::verify_step03_onENDof_candiscovery;
        cantools.ondiscoverystop.parameter = this;
    }
    
    // we dont do anything else for the discovery of ICC boards. we just need the vector
    
    return r;        
}


size_t mcOBJadvfoc::numberofjomos() const
{
   return numofjomos; 
}


bool mcOBJadvfoc::verify(embot::app::eth::Service::OnEndOfOperation onend, bool andactivate)
{
    bool r {true};
    
    afterverifyactivate = onend;
    triggeractivate = andactivate;
    
    // i assign ... and bla bla bla.
    // i call 
    eOmn_serv_diagn_cfg_t dc = {0, 0};
    const embot::core::Confirmer conf {verify_step01_onENDof_verifyencoders, this};
    embot::app::eth::theEncoderReader::getInstance().Verify({focjomodescriptors, dc}, true, conf);
    
    return r;
    
}

bool mcOBJadvfoc::deactivate()
{
    bool r {true};
    
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, cantools.entitydescriptor); 
    eo_canmap_DeconfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, cantools.entitydescriptor); 
    
          
    // we should make sure that embot::app::eth::theICCservices unloads its entities
    #warning TODO: much better doing embot::app::eth::theICCmapping::getInstance().clear(motors && joints);
    embot::app::eth::theICCmapping::getInstance().clear();
    
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


    synch(embot::app::eth::Service::State::idle);
    
    // maybe put in clear() also entities etc. 
    clear();  
    
    return r;           
}

bool mcOBJadvfoc::activate()
{
    bool r {true};
    
    // we should activate only if the state of p2core is ::verified ... but for now we just drop this check

    eo_entities_SetNumOfJoints(eo_entities_GetHandle(), numofjomos);
    eo_entities_SetNumOfMotors(eo_entities_GetHandle(), numofjomos);
    
    // load the can mapping of boards and of entities, if any can board    
    if(false == canactuators.empty())
    {
        for(uint8_t i=0; i<canactuators.size(); i++)
        {
            eObrd_canproperties_t prop = {0};

            prop.type = canactuators[i].actua.board.type;
            prop.location.port = (eobus_can1 == canactuators[i].actua.location.bus) ? eOcanport1 : eOcanport2;
            prop.location.addr = canactuators[i].actua.location.adr;
            prop.location.insideindex = eobrd_caninsideindex_first;
            prop.requiredprotocol.major = canactuators[i].actua.board.protocol.major;
            prop.requiredprotocol.minor = canactuators[i].actua.board.protocol.minor;
            
            eo_vector_PushBack(cantools.boardproperties, &prop);            
        }
        eo_canmap_LoadBoards(eo_canmap_GetHandle(), cantools.boardproperties); 
        
        // load the can entity mapping.
        for(uint8_t i=0; i<canactuators.size(); i++)
        {
            eOcanmap_entitydescriptor_t des = {0};                
                            
            des.location.port = (eobus_can1 == canactuators[i].actua.location.bus) ? eOcanport1 : eOcanport2;
            des.location.addr = canactuators[i].actua.location.adr;
            des.location.insideindex = eobrd_caninsideindex_first;
            des.index = (eOcanmap_entityindex_t)canactuators[i].index;

            eo_vector_PushBack(cantools.entitydescriptor, &des);          
        }        
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, cantools.entitydescriptor); 
        eo_canmap_ConfigEntity(eo_canmap_GetHandle(), eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, cantools.entitydescriptor);        

    }
    
    
    // load the icc mapping of boards and of entities, if any icc board
    if(false == iccactuators.empty())
    {
        for(uint8_t i=0; i<canactuators.size(); i++)
        {        
            // this location is icc then ... we should tell object embot::app::eth::theICCmapping
            // that this location is associated to the i-th eoprot_entity_mc_motor / joint entity
            eOlocation_t location = canactuators[i].actua.location;
            uint8_t index = canactuators[i].index;
            embot::app::eth::theICCmapping::getInstance().load({{location}, {eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint}, index});
            embot::app::eth::theICCmapping::getInstance().load({{location}, {eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor}, index});
        }        
    }

    // init the encoders
    eOmn_serv_diagn_cfg_t dc = {0, 0};
    embot::app::eth::theEncoderReader::getInstance().Activate({focjomodescriptors, dc});
    

    // i use a eOmn_serv_configuration_t that is as close as possible to what MController_config_board() needs
    MController_config_board(&focservconfig);

    
    // synch 
    synch(embot::app::eth::Service::State::activated);   

    return r;                
}
    

void mcOBJadvfoc::verify_step01_onENDof_verifyencoders(void *tHIS, bool operationisok)
{  
    mcOBJadvfoc *mcadvfoc = reinterpret_cast<mcOBJadvfoc*>(tHIS);  

    if(false == operationisok)
    {
        // the encoder reader fails. we dont even start the discovery of the foc boards.
        
        // 1. assign new state  
        mcadvfoc->synch(embot::app::eth::Service::State::failureofverify);        

        // 2. diagnostics
        embot::app::eth::theErrorManager::Severity s {embot::app::eth::theErrorManager::Severity::error};
        eOerror_code_t errorcode {eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_failed_encoders_verify)};      
        mcadvfoc->emit(s, errorcode);
        
        // 3. on verify
        if(nullptr != mcadvfoc->afterverifyactivate)
        {
            mcadvfoc->afterverifyactivate(mcadvfoc->service, false);            
        }
        
        // 4. return
                 
        return;
    }
    
    // now we may do icc discovery
    bool ICCdiscovery = (false == mcadvfoc->iccactuators.empty());  
    
    if(true == ICCdiscovery)
    {
        // nothing for now: i just call the verify_step02_onENDof_iccdiscovery w/ good params        
        verify_step02_onENDof_iccdiscovery(tHIS, true);
    }
    else
    {
        // i just call the verify_step02_onENDof_iccdiscovery w/ good params  
        verify_step02_onENDof_iccdiscovery(tHIS, true);
    }
    
    return;
}  

void mcOBJadvfoc::verify_step02_onENDof_iccdiscovery(void *tHIS, bool searchisok)
{
    mcOBJadvfoc *mcadvfoc = reinterpret_cast<mcOBJadvfoc*>(tHIS);  

    if(false == searchisok)
    {
        // the icc discovery fails. we dont even start the discovery of the can boards.
        
        // 1. assign new state  
        mcadvfoc->synch(embot::app::eth::Service::State::failureofverify);        

        // 2. diagnostics
        embot::app::eth::theErrorManager::Severity s {embot::app::eth::theErrorManager::Severity::error};
        eOerror_code_t errorcode {eoerror_code_get(eoerror_category_Config, eoerror_value_CFG_mc_foc_failed_encoders_verify)};      
        mcadvfoc->emit(s, errorcode);
        
        // 3. on verify
        if(nullptr != mcadvfoc->afterverifyactivate)
        {
            mcadvfoc->afterverifyactivate(mcadvfoc->service, false);            
        }
        
        // 4. return
                 
        return;
    }

    
    // if we have can boards to discover we start can discovery and exit.
    
    bool CANdiscovery = (false == mcadvfoc->canactuators.empty());

    if(true == CANdiscovery)
    {
        eo_candiscovery2_Start2(eo_candiscovery2_GetHandle(), mcadvfoc->cantools.discoverytargets, &mcadvfoc->cantools.ondiscoverystop);      
    }
    else
    {
        // else we just finish it in here  by calling verify_step03_onENDof_candiscovery()
        verify_step03_onENDof_candiscovery(tHIS, eo_candiscovery2_GetHandle(), true);        
    }
    
}

eOresult_t mcOBJadvfoc::verify_step03_onENDof_candiscovery(void *tHIS, EOtheCANdiscovery2* cd2, eObool_t searchisok)
{
    eOresult_t r {eores_OK}; // always OK is fine as well, as nobody checks it    
    
    mcOBJadvfoc *mcadvfoc = reinterpret_cast<mcOBJadvfoc*>(tHIS);

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
    mcadvfoc->synch(state);
    
    
    // 2. send diagnostics
    mcadvfoc->emit(s, errorcode);
    
    // 3. activate if needed
    if((true == OK) && (true == mcadvfoc->triggeractivate))
    {
        mcadvfoc->activate();
    }    
        
    // 4. callback ...
    if(nullptr != mcadvfoc->afterverifyactivate)
    {
        mcadvfoc->afterverifyactivate(mcadvfoc->service, OK);            
    }
                
    return r;
}
    

} // namespace embot::app::eth::service::impl::mc { 




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


