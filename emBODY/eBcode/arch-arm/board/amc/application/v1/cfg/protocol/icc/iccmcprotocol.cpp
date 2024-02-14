
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

//#include "embot_app_eth_mc_messaging.h"


// nothing in here for MC as it is inside file embot_app_eth_theServiceMC_impl.cpp

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

//#include "Controller.h"
//#include "EoProtocol.h"
//#include "embot_app_eth_theICCmapping.h"
////#include "EOtheCANprotocol.h"
//#include "embot_app_eth_theErrorManager.h"
//#include "embot_os_theScheduler.h"

//namespace embot::app::eth::mc::messaging::receiver {
//    
//    
//    class mcAgent : public Agent 
//    {
//    public:
//        
//        mcAgent() {}
//        virtual ~mcAgent() {}
//            
//        bool get(const sigFOCstatus1 &msg) override 
//        { 
//            embot::app::eth::theICCmapping::Index motorindex {0}; 
//            motorindex = embot::app::eth::theICCmapping::getInstance().toindex(msg.source, {eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor});
//  
//            
//            eOmc_motor_t *motor = reinterpret_cast<eOmc_motor_t*>(eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motorindex));
//            motor->status.basic.mot_current = msg.info.current;
//            motor->status.basic.mot_velocity = msg.info.velocity;
//            motor->status.basic.mot_position = msg.info.position;
//            MController_update_motor_odometry_fbk_can(motorindex, (void*)msg.info.payload);      
//            
//            return false; 
//        } 
//        
//        bool get(const sigFOCstatus2 &msg) override 
//        { 
//            embot::app::eth::theICCmapping::Index motorindex {0}; 
//            motorindex = embot::app::eth::theICCmapping::getInstance().toindex(msg.source, {eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor});
//            
//            if(embot::app::eth::theICCmapping::invalidindex != motorindex)
//            {
//                MController_update_motor_state_fbk(motorindex, (void*)msg.info.payload);
//            }
//            
//            return false; 
//        } 
//        
//        bool get(const sigPrint &msg) override 
//        { 
//            // the message may have history ...
//            // we however just print the substring
//            embot::os::Thread *thr {embot::os::theScheduler::getInstance().scheduled()};
//            embot::app::eth::theErrorManager::getInstance().emit(embot::app::eth::theErrorManager::Severity::trace, {"ICC agent received CAN print:", thr}, {}, std::string(msg.info.substring));        
// 
//            //embot::core::print("received CAN print: " + std::string(msg.info.substring));
//            return false; 
//        }
//    };

//    mcAgent mca {};    

//    Agent * agent()
//    {
//        return &mca;   
//    }

//} // namespace embot::app::eth::mc::messaging::receiver {



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





