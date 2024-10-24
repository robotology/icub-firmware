
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


#include "embot_hw_motor_bldc.h"

//#include "embot_app_application_theCANtracer.h"


#define DEBUG_print_decoded_CAN_frames
//#define DEBUG_print_decoded_CAN_frames_periodic

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

    
struct embot::app::bldc::theMC2agent::Impl : public embot::app::application::CANagentMC2
{   
    Config _config {};
        
    embot::app::bldc::mbd::interface::IO2 &io2handle {embot::app::bldc::mbd::interface::getIO2handle()};
    
    // use io2handle instead of _config.io2 or .... embot::app::bldc::mbd::interface::getIO2handle()
      
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
    
    // init mbd
    embot::app::bldc::mbd::interface::init();
    
    
    // init hw::motor et al
    
    // assign the callback on currents
    
    // and now calls whatever the MBD needs to be initialised
    
    
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
      
    io2handle.event_clearall();
    
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
    
    io2handle.set_fault(hwinfo.faultpressed);   

    for(const auto &m : themotors)
    {        
        embot::app::bldc::mbd::interface::getIO2handle().set_powersupply(hwinfo.vcc, embot::core::tointegral(m)); 
    }
    
}

bool embot::app::bldc::theMC2agent::Impl::loadSIG(std::vector<embot::prot::can::Frame> &output)
{
    bool emitted {false};
    
    for(const auto &m : themotors)
    {

        bool emitFOC = io2handle.get_transmit(embot::prot::can::motor::periodic::CMD::FOC, embot::core::tointegral(m));
        bool emitSTATUS = io2handle.get_transmit(embot::prot::can::motor::periodic::CMD::STATUS, embot::core::tointegral(m));
        bool emitADDITIONAL_STATUS = io2handle.get_transmit(embot::prot::can::motor::periodic::CMD::ADDITIONAL_STATUS, embot::core::tointegral(m)); // {false};
            
        // each motor transmits using a different address        
        uint8_t canaddress = _config.getaddress(m);
                
        if(true == emitFOC)
        {
            emitted = true;
            embot::prot::can::motor::periodic::Message_FOC msg {};
            embot::prot::can::Frame f {};
            embot::prot::can::motor::periodic::Message_FOC::Info info {};
            
            info.canaddress = canaddress;
            embot::app::bldc::mbd::interface::IO2::canFOCinfo cfi {};
            io2handle.get(cfi, embot::core::tointegral(m));
            // fill foc info from the MBD: i get and load directly because ... the setXXX() methods accept normal measures
            info.setCurrent(cfi.current);
            info.setVelocity(cfi.velocity);
            info.setPosition(cfi.position); 
                
#if defined(DEBUG_print_decoded_CAN_frames_periodic)
            embot::core::print(embot::hw::motor::bldc::to_string(m) + ": " + info.to_string());
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
                
            embot::app::bldc::mbd::interface::IO2::canSTATUSinfo csi {};
            io2handle.get(csi, embot::core::tointegral(m));
                
            // 1. controlmode
            auto ctrlmodecan {embot::prot::can::motor::ControlMode::none};
            auto ctrlmodembd = csi.controlmode; //io2handle.get_controlmode(embot::core::tointegral(m));
            embot::app::bldc::mbd::interface::Converter::tocan(ctrlmodembd, ctrlmodecan);
            info.controlmode = embot::prot::can::motor::Converter::convert(ctrlmodecan);
            
            // 2. quadencoderstate
            embot::prot::can::motor::board::foc::QEstate qescan {0};
            uint8_t qesmbd = csi.quadencoderstate; // io2handle.get_quadencoderstate(embot::core::tointegral(m));
            embot::app::bldc::mbd::interface::Converter::tocan(qesmbd, qescan);
            info.quadencoderstate = qescan.get();
            
            // 3. pwmfeedback
            float pwmperc =  csi.pwmfeedback; // io2handle.get_pwmfeedback(embot::core::tointegral(m));
            info.pwmfeedback = embot::prot::can::motor::Converter::to_can_percentage(pwmperc);;  
       
            // 4 motorfaultstate
            embot::prot::can::motor::board::foc::MotorFaultState mfscan {0};
            uint32_t mfsmbd = csi.motorfaultstate;// io2handle.get_motorfaultstate(embot::core::tointegral(m));
            embot::app::bldc::mbd::interface::Converter::tocan(mfsmbd, mfscan);
            info.motorfaultstate = mfscan.get();
            
#if defined(DEBUG_print_decoded_CAN_frames_periodic)
            embot::core::print(embot::hw::motor::bldc::to_string(m) + ": " + info.to_string());
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
                
            float temp = io2handle.get_temperature(embot::core::tointegral(m));
            int16_t t = static_cast<int16_t>(0.1*temp);
            info.temperature = t;

#if defined(DEBUG_print_decoded_CAN_frames_periodic)
            embot::core::print(embot::hw::motor::bldc::to_string(m) + ": " + info.to_string());
#endif                 
                
            // load                 
            msg.load(info);
            // get frame
            msg.get(f);
            // push it back to vector
            output.push_back(f);        
        }          
    }
    
   
    return emitted;    
}


// interface to CANagentMC2

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT::Info &info)
{
    
#if defined(DEBUG_print_decoded_CAN_frames_periodic)
    embot::core::print(info.to_string());
#endif
    
    // 1. we have info about all motors, so i must extract the targets of the two motors i need according to the address   
    for(const auto &m : themotors)
    {
        int16_t trg = info.target[_config.getaddress(m)-1];
        embot::app::bldc::mbd::interface::Targets t {};        
        embot::app::bldc::mbd::interface::Converter::fromcan(trg, t);
        io2handle.event_pushback(t, embot::core::tointegral(m));
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
    io2handle.event_pushback(cm, motor);        
    
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
    io2handle.event_pushback(currlimits, motor);    
        
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
    io2handle.event_pushback(pid, motor);    
        
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
    io2handle.event_pushback(pid, motor);    
        
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
    io2handle.event_pushback(mc, motor);    
        
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
    auto mbdcm = io2handle.get_controlmode(motor);
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
    io2handle.get_current_limits(motor, cl);    
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
    io2handle.get_current_pid(motor, pid);   
    // marco.accame pid.type must be ControlModes_Current so that embot::app::bldc::mbd::interface::Converter::tocan() works fine
    // that is done inside get_current_pid()
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
    io2handle.get_velocity_pid(motor, pid);  
    // marco.accame pid.type must be ControlModes_Velocity so that embot::app::bldc::mbd::interface::Converter::tocan() works fine
    // that is done inside get_velocity_pid()
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
    io2handle.get_motor_config(motor, mc);    
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


