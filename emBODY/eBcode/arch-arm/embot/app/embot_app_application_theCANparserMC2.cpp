
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theCANparserMC2.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include <new>



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot::app::utils
{       
    using fpParser = bool (*)(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);       
    struct ParserItem
    {
        using CMD = uint8_t; 
        using OPC = embot::prot::can::OPC;        
        CMD cmd {0};
        OPC opc {OPC::none};
        fpParser parser {nullptr};
        
        constexpr ParserItem() = default;
        constexpr ParserItem(CMD c) : cmd(c), opc(OPC::none), parser(nullptr) {};
        constexpr ParserItem(CMD c, OPC o, fpParser p) : cmd(c), opc(o), parser(p) {};              
    };
}
    
// the implementation uses a LUT but we keep also a non LUT code
#define Impl_useLUT
        
struct embot::app::application::theCANparserMC2::Impl
{ 
    using fpParser = embot::app::utils::fpParser;
    using ParserItem = embot::app::utils::ParserItem;
        
    dummyCANagentMC2 dummyagent {};
        
    static constexpr uint8_t defaultaddress {1};
    Config config {&dummyagent, {defaultaddress, defaultaddress+1}};
        
    bool txframe {false};
    bool recognised {false};
    
    embot::prot::can::Clas cls {embot::prot::can::Clas::none};
    std::uint8_t cmd {0};    
        

    Impl() = default;
    
    bool initialise(const Config &cfg);
           
    bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies, Result &result, FILTER filter);
       

    // this parser assigns the motor index not on the 8-th bit of the CMD but on the address of the received frame.   
    void updatemotorindex(uint8_t destination, embot::prot::can::motor::polling::MotIndex &motorindex);
        
    fpParser getparserLUT(embot::prot::can::Clas cls, uint8_t cmd, FILTER filter, Result &result);
    
    // streaming parse functions
    static bool parse_ems2foc_desired_current(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);  
    // polling parse functions, set<>
    static bool parse_set_motorparam(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set_controlmode(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set_currentlimit(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set_pospid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set_pospidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set_currentpid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set_currentpidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set_velocitypid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set_velocitypidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set_motorconfig(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set_temperaturelimit(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_set_pid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    
    // polling parse function, get<>
    static bool parse_get_motorparam(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_get(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_get_controlmode(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_get_pospid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);  
    static bool parse_get_pospidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);    
    static bool parse_get_currentpid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);  
    static bool parse_get_currentpidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_get_velocitypid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_get_velocitypidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_get_temperaturelimit(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_get_motorconfig(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_get_currentlimit(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    static bool parse_get_pid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
       
    // the lookup table
    // each item has an integer code expressed as a number, its associated OPC (set or get) and the parse function
    // however, it is a sparse table w/ dummy items
    // the dummy items use a ctor that accepts only a number and loads opc none and nullptr as parse function     
    static constexpr std::array<ParserItem, 128> lutMCparserPOLLING
    {
          0,  
        { 1, embot::prot::can::OPC::set, parse_set_motorparam},         // SET_MOTOR_PARAM
        { 2, embot::prot::can::OPC::get, parse_get_motorparam},         // GET_MOTOR_PARAM
          3,  4,  
        { 5, embot::prot::can::OPC::set, parse_set },                   // SET
        { 6, embot::prot::can::OPC::get, parse_get },                   // GET
        { 7, embot::prot::can::OPC::get, parse_get_controlmode},        // GET_CONTROL_MODE
          8,
        { 9, embot::prot::can::OPC::set, parse_set_controlmode},        // SET_CONTROL_MODE
         10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 
         20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 
         30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 
         40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 
         50, 
        {50,  embot::prot::can::OPC::set, nullptr},                     // SET_BOARD_ID = 51 is not managed 
                 52, 53, 54, 55, 56, 57, 58, 59,                                      
         60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 
         70, 71, 
        {72, embot::prot::can::OPC::set, parse_set_currentlimit},       // SET_CURRENT_LIMIT
                     73, 74, 75, 76, 77, 78, 79,
         80, 81, 
        
        {82, embot::prot::can::OPC::set, parse_set_pospid},             // SET_POS_PID   
        {83, embot::prot::can::OPC::get, parse_get_pospid},             // GET_POS_PID 
        {84, embot::prot::can::OPC::set, parse_set_pospidlimits},       // SET_POS_PIDLIMITS
        {85, embot::prot::can::OPC::get, parse_get_pospidlimits},       // GET_POS_PIDLIMITS
        
                                 86, 87, 88, 89, 
         90,         
        {91, embot::prot::can::OPC::get, nullptr},                      // GET_FIRMWARE_VERSION = 91 is not parsed in here
                 92, 93, 94, 95, 96, 97, 98, 99,
        100,
        {101, embot::prot::can::OPC::set, parse_set_currentpid},        // SET_CURRENT_PID   
        {102, embot::prot::can::OPC::get, parse_get_currentpid},        // GET_CURRENT_PID 
        {103, embot::prot::can::OPC::set, parse_set_currentpidlimits},  // SET_CURRENT_PIDLIMITS
        {104, embot::prot::can::OPC::get, parse_get_currentpidlimits},  // GET_CURRENT_PIDLIMITS
        {105, embot::prot::can::OPC::set, parse_set_velocitypid},       // SET_VELOCITY_PID 
        {106, embot::prot::can::OPC::get, parse_get_velocitypid},       // GET_VELOCITY_PID
        {107, embot::prot::can::OPC::set, parse_set_velocitypidlimits}, // SET_VELOCITY_PIDLIMITS
        {108, embot::prot::can::OPC::get, parse_get_velocitypidlimits}, // GET_VELOCITY_PIDLIMITS        
                                            109,
        110,111,112,113,114,115,116,117,118,
        {119, embot::prot::can::OPC::set, parse_set_motorconfig},       // SET_MOTOR_CONFIG
        120,
        {121, embot::prot::can::OPC::set, parse_set_temperaturelimit},  // SET_TEMPERATURE_LIMIT  
        {122, embot::prot::can::OPC::get, parse_get_temperaturelimit},  // GET_TEMPERATURE_LIMIT
        {123, embot::prot::can::OPC::get, parse_get_motorconfig},       // GET_MOTOR_CONFIG
        {124, embot::prot::can::OPC::get, parse_get_currentlimit},      // GET_CURRENT_LIMIT
        {125, embot::prot::can::OPC::set, parse_set_pid},               // SET_PID
        {126, embot::prot::can::OPC::get, parse_get_pid},               // GET_PID
        {127, embot::prot::can::OPC::none, nullptr}                     // FFU127                                                
    };
    
};

bool embot::app::application::theCANparserMC2::Impl::initialise(const Config &cfg)
{
    if(!cfg.isvalid())
    {
        return false;
    }
    
    config = cfg;    
  
    return true;
}


bool embot::app::application::theCANparserMC2::Impl::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies, Result &result, FILTER filter)  
{
    txframe = false;
    result.recognised = result.processed = false;
    
    fpParser parser = nullptr;
    
    bool pollingMOTone = embot::prot::can::frameis4board(frame, config.address[0]);
    bool pollingMOTtwo = embot::prot::can::frameis4board(frame, config.address[1]);
    
    if((false == pollingMOTone) && (false == pollingMOTtwo))
    {
        // if we are in here we may have received a message w/ Clas::periodicMotorControl  
        // and we want to decode its CMD::EMSTO2FOC_DESIRED_CURRENT         
        if(embot::prot::can::Clas::periodicMotorControl != embot::prot::can::frame2clas(frame))
        {
            result.recognised = false;
            return result.recognised;
        }
    }
        
    // now get cls and cmd
    cls = embot::prot::can::frame2clas(frame);
    cmd = embot::prot::can::frame2cmd(frame);
    
    // now i get the pointer to parser function and Result

    parser = getparserLUT(cls, cmd, filter, result);
    
    if((true == result.processed) && (nullptr != parser))
    {
        txframe = parser(this, frame, replies);
    }
    
    return result.recognised;    
}


embot::app::application::theCANparserMC2::Impl::fpParser embot::app::application::theCANparserMC2::Impl::getparserLUT(embot::prot::can::Clas cls, uint8_t cmd, FILTER filter, Result &result)
{    
    fpParser fp = nullptr;
    result.recognised = result.processed =  false;
    // for now we handle only some messages ... using nested switch-case 
    
    switch(cls)
    {       
        default:
        {
            // txframe = result.recognised = result.processed = false;
        } break;
        
        case embot::prot::can::Clas::periodicMotorControl:
        { 
            switch(cmd)
            {
                default:
                {
                    // txframe = result.recognised = result.processed = false;
                } break;

                case static_cast<std::uint8_t>(embot::prot::can::motor::periodic::CMD::EMSTO2FOC_DESIRED_CURRENT):
                {
                    result.processed = (filter != FILTER::onlyGET);
                    fp = parse_ems2foc_desired_current;
                } break;               
            } // switch(cmd)   
 
        } break; // case embot::prot::can::Clas::periodicMotorControl:

        case embot::prot::can::Clas::pollingMotorControl:
        {
            if((cmd < 128) && (lutMCparserPOLLING[cmd].cmd == cmd) && (nullptr != lutMCparserPOLLING[cmd].parser))
            {
                fp = lutMCparserPOLLING[cmd].parser;
                if(embot::prot::can::OPC::get == lutMCparserPOLLING[cmd].opc)
                {
                    result.processed = (filter != FILTER::dontGET);
                }
                else
                {
                    result.processed = (filter != FILTER::onlyGET);
                }                                
            }
            
        } break; // case embot::prot::can::Clas::pollingMotorControl:
        
    } // switch(cls)   
    
    
    if(nullptr != fp)
    {
        result.recognised = true;
    }

    return fp;        
}


// the processing / parse functions


void embot::app::application::theCANparserMC2::Impl::updatemotorindex(uint8_t destination, embot::prot::can::motor::polling::MotIndex &motorindex)
{
    if(destination == config.address[0])
    {
        motorindex = embot::prot::can::motor::polling::MotIndex::one;
    }
    else if(destination == config.address[1])
    {
        motorindex = embot::prot::can::motor::polling::MotIndex::two;
    }    
}


bool embot::app::application::theCANparserMC2::Impl::parse_ems2foc_desired_current(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::periodic::Message_EMSTO2FOC_DESIRED_CURRENT msg;
    msg.load(frame);   
    impl->config.agent->get(msg.info);    
    return hasreply;        
}

// mc polling, set<>  

bool embot::app::application::theCANparserMC2::Impl::parse_set_motorparam(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_MOTOR_PARAM msg;
    msg.load(frame); 
    // msg.info.motorindex contains the information about the target motor and we MAY want to use the destination address for it     
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info);    
    return hasreply;        
}


bool embot::app::application::theCANparserMC2::Impl::parse_set(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET msg;
    msg.load(frame); 
    // msg.info.motorindex contains the information about the target motor and we MAY want to use the destination address for it     
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info);    
    return hasreply;        
}

bool embot::app::application::theCANparserMC2::Impl::parse_set_controlmode(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_CONTROL_MODE msg;
    msg.load(frame); 
    // msg.info.motorindex contains the information about the target motor and we MAY want to use the destination address for it     
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info);    
    return hasreply;        
}


bool embot::app::application::theCANparserMC2::Impl::parse_set_currentlimit(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_CURRENT_LIMIT msg;
    msg.load(frame);
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info);    
    return hasreply;        
}

bool embot::app::application::theCANparserMC2::Impl::parse_set_pospid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_POS_PID msg;
    msg.load(frame);
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info);    
    return hasreply;        
}

bool embot::app::application::theCANparserMC2::Impl::parse_set_pospidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_POS_PIDLIMITS msg;
    msg.load(frame);
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
//    impl->config.agent->get(msg.info);    
    return hasreply;        
}

bool embot::app::application::theCANparserMC2::Impl::parse_set_currentpid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_CURRENT_PID msg;
    msg.load(frame);
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info);    
    return hasreply;        
}

bool embot::app::application::theCANparserMC2::Impl::parse_set_currentpidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_CURRENT_PIDLIMITS msg;
    msg.load(frame);
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
//    impl->config.agent->get(msg.info);    
    return hasreply;        
}


bool embot::app::application::theCANparserMC2::Impl::parse_set_velocitypid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_VELOCITY_PID msg;
    msg.load(frame);
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info);    
    return hasreply;        
}


bool embot::app::application::theCANparserMC2::Impl::parse_set_velocitypidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_VELOCITY_PIDLIMITS msg;
    msg.load(frame);
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
//    impl->config.agent->get(msg.info);    
    return hasreply;        
}

bool embot::app::application::theCANparserMC2::Impl::parse_set_motorconfig(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_MOTOR_CONFIG msg;
    msg.load(frame);
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info);    
    return hasreply;        
}

bool embot::app::application::theCANparserMC2::Impl::parse_set_temperaturelimit(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_TEMPERATURE_LIMIT msg;
    msg.load(frame); 
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info);    
    return hasreply;        
}

bool embot::app::application::theCANparserMC2::Impl::parse_set_pid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    constexpr bool hasreply {false};
    embot::prot::can::motor::polling::Message_SET_PID msg;
    msg.load(frame); 
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info);    
    return hasreply;        
}


// mc polling, get<>

bool embot::app::application::theCANparserMC2::Impl::parse_get_motorparam(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_MOTOR_PARAM msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_MOTOR_PARAM::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;         
}

bool embot::app::application::theCANparserMC2::Impl::parse_get(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;         
}

bool embot::app::application::theCANparserMC2::Impl::parse_get_controlmode(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_CONTROL_MODE msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_CONTROL_MODE::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;         
}

bool embot::app::application::theCANparserMC2::Impl::parse_get_pospid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_POS_PID msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_POS_PID::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;       
}

bool embot::app::application::theCANparserMC2::Impl::parse_get_pospidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_POS_PIDLIMITS msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_POS_PIDLIMITS::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
//    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;       
}

bool embot::app::application::theCANparserMC2::Impl::parse_get_currentpid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_CURRENT_PID msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_CURRENT_PID::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;       
}

bool embot::app::application::theCANparserMC2::Impl::parse_get_currentpidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_CURRENT_PIDLIMITS msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_CURRENT_PIDLIMITS::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
//    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;       
}

bool embot::app::application::theCANparserMC2::Impl::parse_get_velocitypid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_VELOCITY_PID msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_VELOCITY_PID::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;        
}


bool embot::app::application::theCANparserMC2::Impl::parse_get_velocitypidlimits(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_VELOCITY_PIDLIMITS msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_VELOCITY_PIDLIMITS::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
//    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;        
}

bool embot::app::application::theCANparserMC2::Impl::parse_get_temperaturelimit(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_TEMPERATURE_LIMIT::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;        
}

bool embot::app::application::theCANparserMC2::Impl::parse_get_motorconfig(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_MOTOR_CONFIG::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;         
}

bool embot::app::application::theCANparserMC2::Impl::parse_get_currentlimit(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_CURRENT_LIMIT::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;          
}

bool embot::app::application::theCANparserMC2::Impl::parse_get_pid(void *p, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    Impl *impl = reinterpret_cast<Impl*>(p);
    
    embot::prot::can::motor::polling::Message_GET_PID msg;
    msg.load(frame);
    
    embot::prot::can::motor::polling::Message_GET_PID::ReplyInfo replyinfo {};
    
    impl->updatemotorindex(embot::prot::can::frame2destination(frame), msg.info.motorindex);    
    impl->config.agent->get(msg.info, replyinfo);

    embot::prot::can::Frame frame0 {};
    uint8_t adr = impl->config.getaddress(static_cast<embot::hw::MOTOR>(msg.info.motorindex));
    if(true == msg.reply(frame0, adr, replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }    
    return false;          
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------



embot::app::application::theCANparserMC2& embot::app::application::theCANparserMC2::getInstance()
{
    static theCANparserMC2* p = new theCANparserMC2();
    return *p;
}

embot::app::application::theCANparserMC2::theCANparserMC2()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theCANparserMC2::~theCANparserMC2() { }
   
        
bool embot::app::application::theCANparserMC2::initialise(const Config &config)
{
    return pImpl->initialise(config);
}


bool embot::app::application::theCANparserMC2::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies, Result &result, FILTER filter)
{
    return pImpl->process(frame, replies, result, filter);    
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


