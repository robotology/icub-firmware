
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_bldc_theMC2agent.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"
#include "embot_core_binary.h"

#include "embot_app_application_CANagentMC2.h"
#include "embot_app_application_theCANparserMC2.h"

#include <cstdio>

#include "embot_prot_can_motor_board.h"

//#include "embot_app_application_theCANtracer.h"


#define DEBUG_print_decoded_CAN_frames

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

    
struct embot::app::bldc::theMC2agent::Impl : public embot::app::application::CANagentMC2
{   
    Config _config {};
      
    Impl() = default;     


    bool initialise(const Config &config);
        
    bool tick(const std::vector<embot::prot::can::Frame> &input, const HWinfo &hwinfo, std::vector<embot::prot::can::Frame> &output);    

    void update(const HWinfo &hwinfo);        

    bool loadSIG(std::vector<embot::prot::can::Frame> &output);    

    // interface to CANagentMC2
    bool get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info) override;
    
    bool get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info) override;
    bool get(const embot::prot::can::motor::polling::Message_SET_CURRENT_LIMIT::Info &info) override; 
    bool get(const embot::prot::can::motor::polling::Message_SET_CURRENT_PID::Info &info) override;          
    bool get(const embot::prot::can::motor::polling::Message_SET_VELOCITY_PID::Info &info) override;  
    bool get(const embot::prot::can::motor::polling::Message_SET_MOTOR_CONFIG::Info &info) override; 
    bool get(const embot::prot::can::motor::polling::Message_SET_TEMPERATURE_LIMIT::Info &info) override;          
        
    bool get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo) override;
    bool get(const embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT::ReplyInfo &replyinfo) override; 
    bool get(const embot::prot::can::motor::polling::Message_GET_CURRENT_PID::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_PID::ReplyInfo &replyinfo) override;   
    bool get(const embot::prot::can::motor::polling::Message_GET_VELOCITY_PID::Info &info, embot::prot::can::motor::polling::Message_GET_VELOCITY_PID::ReplyInfo &replyinfo) override;
    bool get(const embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG::Info &info, embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG::ReplyInfo &replyinfo) override;
    bool get(const embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT::Info &info, embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT::ReplyInfo &replyinfo) override;
            
};


bool embot::app::bldc::theMC2agent::Impl::initialise(const Config &config)
{
    _config =  config;
    
    // initialise the CAN parser MC2, moved to mbd namespace    
    embot::app::application::theCANparserMC2::getInstance().initialise({this, _config.address});
    
    // init hw::motor
    
    // assign the callback on currents
    
    // and now calls whatever the MBD needs to be initialised
    
    embot::app::bldc::mbd::interface::init(&_config.io2);
    
    return true;
}

bool embot::app::bldc::theMC2agent::Impl::tick(const std::vector<embot::prot::can::Frame> &input, const HWinfo &hwinfo, std::vector<embot::prot::can::Frame> &output)
{
    // we do:
    // 1. parse the input w/ FILTER::dontGET
    //    w/ this filter the parser will parse only the SIG<> and SET<> and call the related agent but not the
    //    frames w/ GET<> for later action after the mbd::interce::tick().
    //    the associated agent (this Impl) will compute actions that are: 
    //    copy the relevant data from the SET<> to the mbd object
    // 2. fill the mbd object w/ information retrieved by the hw, for instance the fault status etc.
    // 3. tick the mbd w/ embot::app::bldc::mbd::interface::tick()
    // 4. call the parser w/ FILTER::onlyGET
    //    the parser will call the agent (this Impl) and fill the replies w/ data updated by the recent embot::app::bldc::mbd::interface::tick()
    // 5. call the agent so that any SIG<> emitted by the mbd can be converted in can frames.
      
    _config.io2.event_clearall();
    
    // 1. parse all but GET<>
    embot::app::application::theCANparserMC2::Result result {};
    embot::app::application::theCANparserMC2::FILTER filter {embot::app::application::theCANparserMC2::FILTER::dontGET};
    for(const auto &f : input)
    {
        embot::app::application::theCANparserMC2::getInstance().process(f, output, result, filter);
    } 

    // 2. update the mbd data w/ hwinfo
    update(hwinfo);
    
    // 3. tick mbd
    embot::app::bldc::mbd::interface::tick();
    
    // 4. parse the GET<>
    filter = embot::app::application::theCANparserMC2::FILTER::onlyGET;
    for(const auto &f : input)
    {
        embot::app::application::theCANparserMC2::getInstance().process(f, output, result, filter);
    }

    // 5. get the SIG<>        
    loadSIG(output);
    
    return true;
}

// others

void embot::app::bldc::theMC2agent::Impl::update(const HWinfo &hwinfo)
{
    // in here we just use the hwinfo to update the MBD interface
    
     _config.io2.set_fault(hwinfo.faultpressed);   
    
}

bool embot::app::bldc::theMC2agent::Impl::loadSIG(std::vector<embot::prot::can::Frame> &output)
{
    bool emitted {false};
    
    for(const auto &m : themotors)
    {
        bool transmit = _config.io2.get_transmitstatus(embot::core::tointegral(m));
        bool emitFOC {transmit};
        bool emitSTATUS {transmit};
        bool emitADDITIONAL_STATUS {false};
            
        // each motor transmits using a different address        
        uint8_t canaddress = _config.getaddress(m);
        
        #warning read that
        // i need methods from mbd interface that tell me what is the content to transmit
        // the likes of the method 
        // bool IO2::get_transmitstatus(uint8_t motor) const 
        // that i have just used 
        // 
        // or i just use the pointers directly. for now i do that.
        //
        // the best way is however the former because it moves changes of field names inside the .h file
        
        if(true == emitFOC)
        {
            emitted = true;
            embot::prot::can::motor::periodic::Message_FOC msg {};
            embot::prot::can::Frame f {};
            embot::prot::can::motor::periodic::Message_FOC::Info info {};
            
            info.canaddress = canaddress;
            // fill foc info from the MBD: i get and load directly because ... the setXXX() methods accept normal measures
            float current = _config.io2.get_current(embot::core::tointegral(m));
            info.setCurrent(current);
            float velocity = _config.io2.get_velocity(embot::core::tointegral(m));
            info.setVelocity(velocity);
            float position = _config.io2.get_position(embot::core::tointegral(m));
            info.setPosition(position); 
                
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif 
            
            // load                 
            msg.load(info);
            // get frame
            msg.get(f);
            // push it back to vector
            output.push_back(f);            
        }        

        if(true == emitSTATUS)
        {
            emitted = true;
            embot::prot::can::motor::periodic::Message_STATUS msg {};
            embot::prot::can::Frame f {};
            embot::prot::can::motor::periodic::Message_STATUS::Info info {};
            info.canaddress = canaddress;
                
            // fill info from the MBD: i get, convert to can format, assign to info
                
            // 1. controlmode
            auto ctrlmodecan {embot::prot::can::motor::ControlMode::none};
            auto ctrlmodembd = _config.io2.get_controlmode(embot::core::tointegral(m));
            embot::app::bldc::mbd::interface::Converter::tocan(ctrlmodembd, ctrlmodecan);
            info.controlmode = embot::prot::can::motor::Converter::convert(ctrlmodecan);
            
            // 2. quadencoderstate
            embot::prot::can::motor::board::foc::QEstate qescan {0};
            uint8_t qesmbd = _config.io2.get_quadencoderstate(embot::core::tointegral(m));
            embot::app::bldc::mbd::interface::Converter::tocan(qesmbd, qescan);
            info.quadencoderstate = qescan.get();
            
            // 3. pwmfeedback
            float pwmperc =  _config.io2.get_pwmfeedback(embot::core::tointegral(m));
            info.pwmfeedback = embot::prot::can::motor::Converter::to_can_percentage(pwmperc);;  
       
            // 4 motorfaultstate
            embot::prot::can::motor::board::foc::MotorFaultState mfscan {0};
            uint8_t mfsmbd = _config.io2.get_motorfaultstate(embot::core::tointegral(m));
            embot::app::bldc::mbd::interface::Converter::tocan(mfsmbd, mfscan);
            info.motorfaultstate = mfscan.get();
            
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif            
            
            // load                 
            msg.load(info);
            // get frame
            msg.get(f);
            // push it back to vector
            output.push_back(f);        
        }

        if(true == emitADDITIONAL_STATUS)
        {
            emitted = true;
            embot::prot::can::motor::periodic::Message_ADDITIONAL_STATUS msg {};
            embot::prot::can::Frame f {};
            embot::prot::can::motor::periodic::Message_ADDITIONAL_STATUS::Info info {};
            // fill info info from the MBD
            info.canaddress = canaddress;
                
            float temp = _config.io2.get_temperature(embot::core::tointegral(m));
            int16_t t = static_cast<int16_t>(0.1*temp);
            info.temperature = t;
            // load                 
            msg.load(info);
            // get frame
            msg.get(f);
            // push it back to vector
            output.push_back(f);        
        }          
    }
    
    // in here we form the can frames spontaneously emitted by the MBD.
    // they belong to the periodic motor class
    
//    #warning find out how to see if we need to emit for a given motor
//        
//    for(uint8_t m=0; m<2; m++)
//    {
//        bool emitFOC {true};
//        bool emitSTATUS {true};
//        bool emitADDITIONAL_STATUS {false};
//        
//        if(1 == m)
//        {
//            emitFOC = emitSTATUS = false;
//        }
//        
//        if(true == emitFOC)
//        {
//            emitted = true;
//            embot::prot::can::motor::periodic::Message_FOC msg {};
//            embot::prot::can::Frame f {};
//            embot::prot::can::motor::periodic::Message_FOC::Info info {};
//            // fill foc info from the MBD
//            info.canaddress = _config.address[m];
//            info.current = 0;
//            info.position = 0;
//            info.velocity = 0;
//            // load                 
//            msg.load(info);
//            // get frame
//            msg.get(f);
//            // push it back to vector
//            output.push_back(f);            
//        }        

//        if(true == emitSTATUS)
//        {
//            emitted = true;
//            embot::prot::can::motor::periodic::Message_STATUS msg {};
//            embot::prot::can::Frame f {};
//            embot::prot::can::motor::periodic::Message_STATUS::Info info {};
//            info.canaddress = _config.address[m];
//            // fill info info from the MBD
//            info.controlmode = 0;
//            embot::prot::can::motor::board::foc::QEstate qes(0);
//            info.quadencoderstate = qes.get();
//            info.pwmfeedback = 0;
//            embot::prot::can::motor::board::foc::MotorFaultState mfs(0);
//            info.motorfaultstate = mfs.get();
//            // load                 
//            msg.load(info);
//            // get frame
//            msg.get(f);
//            // push it back to vector
//            output.push_back(f);        
//        }

//        if(true == emitADDITIONAL_STATUS)
//        {
//            emitted = true;
//            embot::prot::can::motor::periodic::Message_ADDITIONAL_STATUS msg {};
//            embot::prot::can::Frame f {};
//            embot::prot::can::motor::periodic::Message_ADDITIONAL_STATUS::Info info {};
//            // fill info info from the MBD
//            info.canaddress = _config.address[m];
//            info.temperature = 0;
//            // load                 
//            msg.load(info);
//            // get frame
//            msg.get(f);
//            // push it back to vector
//            output.push_back(f);        
//        }                  
//    }
//    
//    
   
    return emitted;    
}


// interface to CANagentMC2

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info)
{
    
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    // 1. we have info about all motors, so i must extract the targets of the two motors i need according to the address   
    for(const auto &m : themotors)
    {
        int16_t trg = info.target[_config.getaddress(m)-1];
        embot::app::bldc::mbd::interface::Targets t {};        
        embot::app::bldc::mbd::interface::Converter::fromcan(trg, t);
        _config.io2.event_pushback(t, embot::core::tointegral(m));
    }
     
    return true;        
}



bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    // 1. get the motor index
    uint8_t motor = embot::core::tointegral(info.motorindex);   
    
    // 2. copy from info into the relevant mbd::interface type     
    embot::app::bldc::mbd::interface::ControlModes cm {};    
    embot::app::bldc::mbd::interface::Converter::fromcan(info.controlmode, cm);
    
    // 3. load the object into mbd for a given motor
    _config.io2.event_pushback(cm, motor);        
    
    return true;    
}


bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_CURRENT_LIMIT::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    // 1. get the motor index
    uint8_t motor = embot::core::tointegral(info.motorindex);   
    
    // 2. copy from info into the relevant mbd::interface type     
    embot::app::bldc::mbd::interface::SupervisorInputLimits currlimits {};    
    embot::app::bldc::mbd::interface::Converter::fromcan(info.currents, currlimits);
    
    // 3. load the object into mbd for a given motor
    _config.io2.event_pushback(currlimits, motor);    
        
    return true;
} 

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_CURRENT_PID::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    // 1. get the motor index
    uint8_t motor = embot::core::tointegral(info.motorindex);   
    
    // 2. copy from info into the relevant mbd::interface type     
    embot::app::bldc::mbd::interface::PID pid {};    
    embot::app::bldc::mbd::interface::Converter::fromcan(info.pid, pid);
    
    // 3. load the object into mbd for a given motor
    _config.io2.event_pushback(pid, motor);    
        
    return true;
} 
        
bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_VELOCITY_PID::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    // 1. get the motor index
    uint8_t motor = embot::core::tointegral(info.motorindex);   
    
    // 2. copy from info into the relevant mbd::interface type     
    embot::app::bldc::mbd::interface::PID pid {};    
    embot::app::bldc::mbd::interface::Converter::fromcan(info.pid, pid);
    
    // 3. load the object into mbd for a given motor
    _config.io2.event_pushback(pid, motor);    
        
    return true;
} 
  
bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_MOTOR_CONFIG::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    // 1. get the motor index
    uint8_t motor = embot::core::tointegral(info.motorindex);   
    
    // 2. copy from info into the relevant mbd::interface type     
    embot::app::bldc::mbd::interface::MotorConfigurationExternal mc {};    
    embot::app::bldc::mbd::interface::Converter::fromcan(info.config, mc);
    
    // 3. load the object into mbd for a given motor
    _config.io2.event_pushback(mc, motor);    
        
    return true;    
} 

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_TEMPERATURE_LIMIT::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    // it does nothing
    return true;
} 


bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    replyinfo.motorindex = info.motorindex;
    uint8_t motor = embot::core::tointegral(info.motorindex);
    auto mbdcm = _config.io2.get_controlmode(motor);
    embot::app::bldc::mbd::interface::Converter::tocan(mbdcm, replyinfo.controlmode);
    
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(replyinfo.to_string());
#endif 
    
    return true;    
}


bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    replyinfo.motorindex = info.motorindex;
    uint8_t motor = embot::core::tointegral(info.motorindex);
    embot::app::bldc::mbd::interface::SupervisorInputLimits cl {};
    _config.io2.get_current_limits(motor, cl);    
    embot::app::bldc::mbd::interface::Converter::tocan(cl, replyinfo.currents);
        
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(replyinfo.to_string());
#endif 
        
    return true;          
}

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_CURRENT_PID::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_PID::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    replyinfo.motorindex = info.motorindex;
    uint8_t motor = embot::core::tointegral(info.motorindex);
    embot::app::bldc::mbd::interface::PID pid {};
    _config.io2.get_current_pid(motor, pid);    
    embot::app::bldc::mbd::interface::Converter::tocan(pid, replyinfo.pid);
        
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(replyinfo.to_string());
#endif         
        
    return true;        
}

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_VELOCITY_PID::Info &info, embot::prot::can::motor::polling::Message_GET_VELOCITY_PID::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif

    replyinfo.motorindex = info.motorindex;
    uint8_t motor = embot::core::tointegral(info.motorindex);
    embot::app::bldc::mbd::interface::PID pid {};
    _config.io2.get_velocity_pid(motor, pid);    
    embot::app::bldc::mbd::interface::Converter::tocan(pid, replyinfo.pid);
        
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(replyinfo.to_string());
#endif         
        
    return true;          
}


bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG::Info &info, embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
       
    replyinfo.motorindex = info.motorindex;
    uint8_t motor = embot::core::tointegral(info.motorindex);
    embot::app::bldc::mbd::interface::MotorConfigurationExternal mc {};
    _config.io2.get_motor_config(motor, mc);    
    embot::app::bldc::mbd::interface::Converter::tocan(mc, replyinfo.config);
        
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(replyinfo.to_string());
#endif 
        
    return true;     
}

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT::Info &info, embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    replyinfo.motorindex = info.motorindex;
    replyinfo.hardwarelimit = 666;
    
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(replyinfo.to_string());
#endif 
    
    return true;    
}



// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::bldc::theMC2agent& embot::app::bldc::theMC2agent::getInstance()
{
    static theMC2agent* p = new theMC2agent();
    return *p;
}

embot::app::bldc::theMC2agent::theMC2agent()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::bldc::theMC2agent::~theMC2agent() { }

         
bool embot::app::bldc::theMC2agent::initialise(const Config &config)
{
    return pImpl->initialise(config);
}

bool embot::app::bldc::theMC2agent::tick(const std::vector<embot::prot::can::Frame> &input, const HWinfo &hwinfo, std::vector<embot::prot::can::Frame> &output)
{
    return pImpl->tick(input, hwinfo, output);
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


