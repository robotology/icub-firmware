
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
// #define DEBUG_print_decoded_CAN_frames_periodic

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
    
    bool get(const embot::prot::can::motor::polling::Message_SET_MOTOR_PARAM::Info &info) override;
    bool get(const embot::prot::can::motor::polling::Message_SET::Info &info) override;
    bool get(const embot::prot::can::motor::polling::Message_SET_CONTROL_MODE::Info &info) override;
    bool get(const embot::prot::can::motor::polling::Message_SET_CURRENT_LIMIT::Info &info) override; 
    bool get(const embot::prot::can::motor::polling::Message_SET_POS_PID::Info &info) override;
    bool get(const embot::prot::can::motor::polling::Message_SET_POS_PIDLIMITS::Info &info) override;
    bool get(const embot::prot::can::motor::polling::Message_SET_CURRENT_PID::Info &info) override; 
    bool get(const embot::prot::can::motor::polling::Message_SET_CURRENT_PIDLIMITS::Info &info) override;    
    bool get(const embot::prot::can::motor::polling::Message_SET_VELOCITY_PID::Info &info) override; 
    bool get(const embot::prot::can::motor::polling::Message_SET_VELOCITY_PIDLIMITS::Info &info) override;    
    bool get(const embot::prot::can::motor::polling::Message_SET_MOTOR_CONFIG::Info &info) override; 
    bool get(const embot::prot::can::motor::polling::Message_SET_TEMPERATURE_LIMIT::Info &info) override; 
    bool get(const embot::prot::can::motor::polling::Message_SET_PID::Info &info) override;
    
    bool get(const embot::prot::can::motor::polling::Message_GET_MOTOR_PARAM::Info &info, embot::prot::can::motor::polling::Message_GET_MOTOR_PARAM::ReplyInfo &replyinfo) override;
    bool get(const embot::prot::can::motor::polling::Message_GET::Info &info, embot::prot::can::motor::polling::Message_GET::ReplyInfo &replyinfo) override;    
    bool get(const embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::Info &info, embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo &replyinfo) override;
    bool get(const embot::prot::can::motor::polling::Message_GET_POS_PID::Info &info, embot::prot::can::motor::polling::Message_GET_POS_PID::ReplyInfo &replyinfo) override;
    bool get(const embot::prot::can::motor::polling::Message_GET_POS_PIDLIMITS::Info &info, embot::prot::can::motor::polling::Message_GET_POS_PIDLIMITS::ReplyInfo &replyinfo) override;
    bool get(const embot::prot::can::motor::polling::Message_GET_CURRENT_PID::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_PID::ReplyInfo &replyinfo) override; 
    bool get(const embot::prot::can::motor::polling::Message_GET_CURRENT_PIDLIMITS::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_PIDLIMITS::ReplyInfo &replyinfo) override;    
    bool get(const embot::prot::can::motor::polling::Message_GET_VELOCITY_PID::Info &info, embot::prot::can::motor::polling::Message_GET_VELOCITY_PID::ReplyInfo &replyinfo) override;
    bool get(const embot::prot::can::motor::polling::Message_GET_VELOCITY_PIDLIMITS::Info &info, embot::prot::can::motor::polling::Message_GET_VELOCITY_PIDLIMITS::ReplyInfo &replyinfo) override;
    bool get(const embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT::Info &info, embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT::ReplyInfo &replyinfo) override;
    bool get(const embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG::Info &info, embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG::ReplyInfo &replyinfo) override;
    bool get(const embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT::ReplyInfo &replyinfo) override; 
    bool get(const embot::prot::can::motor::polling::Message_GET_PID::Info &info, embot::prot::can::motor::polling::Message_GET_PID::ReplyInfo &replyinfo) override;
  

    // motor params that are temporarily in here and not in the MBD
    
    std::array<embot::prot::can::motor::PIDlimits, 2> posPIDlimits { {{embot::prot::can::motor::PIDtype::POS, 0, 1, 2}} };
    std::array<embot::prot::can::motor::PIDlimits, 2> velPIDlimits { {{embot::prot::can::motor::PIDtype::VEL, 0, 11, 12}} };
    std::array<embot::prot::can::motor::PIDlimits, 2> curPIDlimits { {{embot::prot::can::motor::PIDtype::CURR, 0, 21, 31}} };
    
    std::array<embot::prot::can::motor::motorparam::Data<embot::prot::can::motor::motorparam::vGENERIC>, 2> vvmGENERIC {};
    std::array<embot::prot::can::motor::motorparam::Data<embot::prot::can::motor::motorparam::vBEMF>, 2> vvmBEMF {};
    std::array<embot::prot::can::motor::motorparam::Data<embot::prot::can::motor::motorparam::vHALL>, 2> vvmHALL {};
    std::array<embot::prot::can::motor::motorparam::Data<embot::prot::can::motor::motorparam::vELECTR_VMAX>, 2> vvmELECTR_VMAX {}; 

    std::array<embot::prot::can::motor::motorparam::Data<embot::prot::can::motor::generic::vGENERIC>, 2> vvgGENERIC {};        
    std::array<embot::prot::can::motor::generic::Data<embot::prot::can::motor::generic::vTargetPOS>, 2> vvgTargetPOS {};
};


bool embot::app::bldc::theMC2agent::Impl::initialise(const Config &config)
{
    _config =  config;
    
    // initialise the CAN parser MC2, moved to mbd namespace    
    embot::app::application::theCANparserMC2::getInstance().initialise({this, _config.address});
    
    // init mbd
    embot::app::bldc::mbd::interface::init();
   
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
        bool emitDEBUG = io2handle.get_transmit(embot::prot::can::motor::periodic::CMD::DEBUG, embot::core::tointegral(m)); 
        
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


        if(true == emitDEBUG)
        {
            
            // we may emitted = true;
            embot::app::bldc::mbd::interface::IO2::canDEBUGqenccalibresult cc {};
            io2handle.get(cc, embot::core::tointegral(m));
            if(true == cc.calibrationdone)
            {
                // ok, we can emit. but we should do it every 1 sec.
                static std::array<size_t, 2> counts {0};
                constexpr size_t decimation {5000};
                
                counts[embot::core::tointegral(m)]++;
                if(counts[embot::core::tointegral(m)] >= decimation)
                {
                    counts[embot::core::tointegral(m)] = 0;
                    
                    // we can surely emit                   
                    emitted = true;                    

                    embot::prot::can::motor::periodic::Message_DEBUG msg {};
                    embot::prot::can::Frame f {};
                    embot::prot::can::motor::periodic::Message_DEBUG::Info info {};
                
                    info.canaddress = canaddress;
                    int16_t offset = cc.offset; // it is a signed i16 that may be complement two is negative
                    info.payload = static_cast<uint16_t>(offset); // i must put the two bytes into lowest two bytes of the payload.
                    
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



bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_MOTOR_PARAM::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif

    // 1. get the motor index
    uint8_t motor = embot::core::tointegral(info.motorindex);   
    
    embot::prot::can::motor::motorparam::ID id = info.descriptor.getID();
	  embot::app::bldc::mbd::interface::MotorConfigurationExtSet from {};
			
		from.key = embot::core::tointegral(id);
		from.value[0] = from.value[1] = 0;
		
    switch(id)
    {
        case embot::prot::can::motor::motorparam::ID::GENERIC:
        {
            vvmGENERIC[motor].load(info.descriptor.getdata());
            embot::core::print(vvmGENERIC[motor].to_string());
						
        } break;
        
        case embot::prot::can::motor::motorparam::ID::BEMF:
        {
            vvmBEMF[motor].load(info.descriptor.getdata());
						from.value[0] = vvmBEMF[motor].value().kbemf;
						embot::core::print(vvmBEMF[motor].to_string());   
						
        } break;

        case embot::prot::can::motor::motorparam::ID::HALL:
        {
            vvmHALL[motor].load(info.descriptor.getdata());
						from.value[0] = vvmHALL[motor].value().offset;
						from.value[1] = (embot::prot::can::motor::motorparam::vHALL::SwapMode::none == vvmHALL[motor].value().swapmode) ? 0.0 : 1.0;
						embot::core::print(vvmHALL[motor].to_string());  
        } break;

        case embot::prot::can::motor::motorparam::ID::ELECTR_VMAX:
        {
            vvmELECTR_VMAX[motor].load(info.descriptor.getdata());
						from.value[0] = vvmELECTR_VMAX[motor].value().vmax;
					  embot::core::print(vvmELECTR_VMAX[motor].to_string());  
        } break;
        
        default: {} break;
    }
		

		
		embot::app::bldc::mbd::interface::MotorConfigurationExtSet to {};   
    embot::app::bldc::mbd::interface::Converter::fromcan(from, to);
    
    // 3. load the object into mbd for a given motor
    io2handle.event_pushback(to, motor); 
#if 0      
    // 2. copy from info into the relevant mbd::interface type     
    embot::app::bldc::mbd::interface::XXX xxx {};    
    embot::app::bldc::mbd::interface::Converter::fromcan(vvGENERIC, xxx);
    
    // 3. load the object into mbd for a given motor
    io2handle.event_pushback(xxx, motor);    
#endif
    
    return true;
} 

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif

    // 1. get the motor index
    uint8_t motor = embot::core::tointegral(info.motorindex);   
    
    embot::prot::can::motor::generic::ID id = info.descriptor.getID();        
    switch(id)
    {
        case embot::prot::can::motor::generic::ID::GENERIC:
        {
            vvgGENERIC[motor].load(info.descriptor.getdata());
        } break;
        
        case embot::prot::can::motor::generic::ID::TargetPOS:
        {
            vvgTargetPOS[motor].load(info.descriptor.getdata());
            // but also it is passed to the MBD interface using a Targets item properly initted
            embot::app::bldc::mbd::interface::Targets t {};        
            t.position = vvgTargetPOS[motor].value().position;
            t.velocity = vvgTargetPOS[motor].value().withvelocity;
            io2handle.event_pushback(t, motor);            
        } break;
        
        default: {} break;
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

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_POS_PID::Info &info)
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
  

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_POS_PIDLIMITS::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    // 1. get the motor index
    uint8_t motor = embot::core::tointegral(info.motorindex);   
    posPIDlimits[motor] = info.pidlimits;
    
//    // 2. copy from info into the relevant mbd::interface type     
//    embot::app::bldc::mbd::interface::PID pid {};    
//    embot::app::bldc::mbd::interface::Converter::fromcan(info.pid, pid);
//    
//    // 3. load the object into mbd for a given motor
//    io2handle.event_pushback(pid, motor);    
        
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
   
bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_CURRENT_PIDLIMITS::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    // 1. get the motor index
    uint8_t motor = embot::core::tointegral(info.motorindex);   
    curPIDlimits[motor] = info.pidlimits;
    
//    // 2. copy from info into the relevant mbd::interface type     
//    embot::app::bldc::mbd::interface::PID pid {};    
//    embot::app::bldc::mbd::interface::Converter::fromcan(info.pid, pid);
//    
//    // 3. load the object into mbd for a given motor
//    io2handle.event_pushback(pid, motor);    
        
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

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_VELOCITY_PIDLIMITS::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    // 1. get the motor index
    uint8_t motor = embot::core::tointegral(info.motorindex);   
    velPIDlimits[motor] = info.pidlimits;
    
//    // 2. copy from info into the relevant mbd::interface type     
//    embot::app::bldc::mbd::interface::PID pid {};    
//    embot::app::bldc::mbd::interface::Converter::fromcan(info.pid, pid);
//    
//    // 3. load the object into mbd for a given motor
//    io2handle.event_pushback(pid, motor);    
        
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

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_SET_PID::Info &info)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    #if 0
        replyinfo.motorindex = info.motorindex;
    replyinfo.descriptor.clear();
    uint8_t motor = embot::core::tointegral(info.motorindex);
    embot::app::bldc::mbd::interface::PID pid {};
    bool found {true};
    switch(info.type)
    {
        case embot::prot::can::motor::pid::Type::POS:
        {
            io2handle.get_pos_pid(motor, pid);
        } break;
        case embot::prot::can::motor::pid::Type::VEL:
        {
            io2handle.get_velocity_pid(motor, pid);
        } break;        
        case embot::prot::can::motor::pid::Type::CURR:
        {
            io2handle.get_current_pid(motor, pid);
        } break; 
        default:
        {
            found = false;
        } break;            
    }
    
    if(false == found)
    {
        return true;
    }
    
    float value {0.0};
    
    switch(info.param)
    {
        case embot::prot::can::motor::pid::Param::KP:
        {   
            value = pid.P;            
        } break; 

        case embot::prot::can::motor::pid::Param::KI:
        {   
            value = pid.I;            
        } break; 

        case embot::prot::can::motor::pid::Param::KD:
        {   
            value = pid.D;            
        } break; 
                       
        default:
        {
            found = false;
        } break;            
    }
    
    if(false == found)
    {
        return true;
    }
    
    replyinfo.descriptor.load(info.type, info.param, value);
    #endif

    // to be done

    uint8_t motor = embot::core::tointegral(info.motorindex);
    embot::prot::can::motor::pid::Type t = info.descriptor.getType();
    embot::prot::can::motor::pid::Param p = info.descriptor.getParam();
    float value = info.descriptor.getValue();
    
    embot::app::bldc::mbd::interface::PID pid {};
        
    bool found {true};
    switch(t)
    {
        case embot::prot::can::motor::pid::Type::POS:
        {
            io2handle.get_pos_pid(motor, pid);
        } break;
        case embot::prot::can::motor::pid::Type::VEL:
        {
            io2handle.get_velocity_pid(motor, pid);
        } break;        
        case embot::prot::can::motor::pid::Type::CURR:
        {
            io2handle.get_current_pid(motor, pid);
        } break; 
        default:
        {
            found = false;
        } break;            
    }
    
    if(false == found)
    {
        return true;
    } 
    
    
    switch(p)
    {
        case embot::prot::can::motor::pid::Param::KP:
        {   
            pid.P = value;            
        } break; 

        case embot::prot::can::motor::pid::Param::KI:
        {   
            pid.I = value;            
        } break; 

        case embot::prot::can::motor::pid::Param::KD:
        {   
            pid.D = value;            
        } break; 
                       
        default:
        {
            found = false;
        } break;            
    }    
    
    // 3. load the object into mbd for a given motor
    io2handle.event_pushback(pid, motor);    

    return true;
} 


bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_MOTOR_PARAM::Info &info, embot::prot::can::motor::polling::Message_GET_MOTOR_PARAM::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif

    replyinfo.motorindex = info.motorindex;
    replyinfo.descriptor.clear();
    uint8_t motor = embot::core::tointegral(info.motorindex);
    
    embot::prot::can::motor::motorparam::Descriptor des { };

    // TO BE DONE
    
    embot::prot::can::motor::motorparam::ID id = info.id;        
    switch(id)
    {
        case embot::prot::can::motor::motorparam::ID::GENERIC:
        {
            des.load(id, vvmGENERIC[motor].serialize());
        } break;
        
        case embot::prot::can::motor::motorparam::ID::BEMF:
        {
            des.load(id, vvmBEMF[motor].serialize());                   
        } break;

        case embot::prot::can::motor::motorparam::ID::HALL:
        {
            des.load(id, vvmHALL[motor].serialize());
        } break;

        case embot::prot::can::motor::motorparam::ID::ELECTR_VMAX:
        {
            des.load(id, vvmELECTR_VMAX[motor].serialize());
        } break;
        
        default: {} break;
    }    

    replyinfo.load(des, info.motorindex);    
    
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(replyinfo.to_string());
#endif 
    
    return true;    
}


bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET::Info &info, embot::prot::can::motor::polling::Message_GET::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif

    replyinfo.motorindex = info.motorindex;
    replyinfo.descriptor.clear();
    uint8_t motor = embot::core::tointegral(info.motorindex);
    
    embot::prot::can::motor::generic::Descriptor des { };

    // TO BE DONE
    
    embot::prot::can::motor::generic::ID id = info.id;        
    switch(id)
    {
        case embot::prot::can::motor::generic::ID::GENERIC:
        {
            des.load(id, vvgGENERIC[motor].serialize());
        } break;
        
        case embot::prot::can::motor::generic::ID::TargetPOS:
        {
            des.load(id, vvgTargetPOS[motor].serialize());                   
        } break;
        
        default: {} break;
    }    

    replyinfo.load(des, info.motorindex);    
    
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(replyinfo.to_string());
#endif 
    
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

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_POS_PID::Info &info, embot::prot::can::motor::polling::Message_GET_POS_PID::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif

    replyinfo.motorindex = info.motorindex;
    uint8_t motor = embot::core::tointegral(info.motorindex);
    embot::app::bldc::mbd::interface::PID pid {};
    io2handle.get_pos_pid(motor, pid);  
    // marco.accame pid.type must be ControlModes_Position so that embot::app::bldc::mbd::interface::Converter::tocan() works fine
    // that is done inside get_pos_pid()
    embot::app::bldc::mbd::interface::Converter::tocan(pid, replyinfo.pid);
        
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(replyinfo.to_string());
#endif         
        
    return true;          
}

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_POS_PIDLIMITS::Info &info, embot::prot::can::motor::polling::Message_GET_POS_PIDLIMITS::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif

    replyinfo.motorindex = info.motorindex;
    uint8_t motor = embot::core::tointegral(info.motorindex);
    //replyinfo.pidlimits.load(embot::prot::can::motor::PIDtype::POS, 0, 1000, 2000);
    replyinfo.pidlimits = posPIDlimits[motor];
//    embot::app::bldc::mbd::interface::PID pid {};
//    io2handle.get_pos_pid(motor, pid);  
//    // marco.accame pid.type must be ControlModes_Position so that embot::app::bldc::mbd::interface::Converter::tocan() works fine
//    // that is done inside get_pos_pid()
//    embot::app::bldc::mbd::interface::Converter::tocan(pid, replyinfo.pid);

        
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

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_CURRENT_PIDLIMITS::Info &info, embot::prot::can::motor::polling::Message_GET_CURRENT_PIDLIMITS::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif
    
    replyinfo.motorindex = info.motorindex;
    uint8_t motor = embot::core::tointegral(info.motorindex);
    //replyinfo.pidlimits.load(embot::prot::can::motor::PIDtype::CURR, 0, 1001, 2001);
    replyinfo.pidlimits = curPIDlimits[motor];
//    embot::app::bldc::mbd::interface::PID pid {};
//    io2handle.get_current_pid(motor, pid);   
//    // marco.accame pid.type must be ControlModes_Current so that embot::app::bldc::mbd::interface::Converter::tocan() works fine
//    // that is done inside get_current_pid()
//    embot::app::bldc::mbd::interface::Converter::tocan(pid, replyinfo.pid);
        
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

bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_VELOCITY_PIDLIMITS::Info &info, embot::prot::can::motor::polling::Message_GET_VELOCITY_PIDLIMITS::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif

    replyinfo.motorindex = info.motorindex;
    uint8_t motor = embot::core::tointegral(info.motorindex);
    //replyinfo.pidlimits.load(embot::prot::can::motor::PIDtype::VEL, 0, 1002, 2002);
    replyinfo.pidlimits = velPIDlimits[motor];
//    embot::app::bldc::mbd::interface::PID pid {};
//    io2handle.get_velocity_pid(motor, pid);  
//    // marco.accame pid.type must be ControlModes_Velocity so that embot::app::bldc::mbd::interface::Converter::tocan() works fine
//    // that is done inside get_velocity_pid()
//    embot::app::bldc::mbd::interface::Converter::tocan(pid, replyinfo.pid);
//        
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


bool embot::app::bldc::theMC2agent::Impl::get(const embot::prot::can::motor::polling::Message_GET_PID::Info &info, embot::prot::can::motor::polling::Message_GET_PID::ReplyInfo &replyinfo)
{
#if defined(DEBUG_print_decoded_CAN_frames)
    embot::core::print(info.to_string());
#endif

    replyinfo.motorindex = info.motorindex;
    replyinfo.descriptor.clear();
    uint8_t motor = embot::core::tointegral(info.motorindex);
    embot::app::bldc::mbd::interface::PID pid {};
    bool found {true};
    switch(info.type)
    {
        case embot::prot::can::motor::pid::Type::POS:
        {
            io2handle.get_pos_pid(motor, pid);
        } break;
        case embot::prot::can::motor::pid::Type::VEL:
        {
            io2handle.get_velocity_pid(motor, pid);
        } break;        
        case embot::prot::can::motor::pid::Type::CURR:
        {
            io2handle.get_current_pid(motor, pid);
        } break; 
        default:
        {
            found = false;
        } break;            
    }
    
    if(false == found)
    {
        return true;
    }
    
    float value {0.0};
    
    switch(info.param)
    {
        case embot::prot::can::motor::pid::Param::KP:
        {   
            value = pid.P;            
        } break; 

        case embot::prot::can::motor::pid::Param::KI:
        {   
            value = pid.I;            
        } break; 

        case embot::prot::can::motor::pid::Param::KD:
        {   
            value = pid.D;            
        } break; 
                       
        default:
        {
            found = false;
        } break;            
    }
    
    if(false == found)
    {
        return true;
    }
    
    replyinfo.descriptor.load(info.type, info.param, value);
  
    
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


