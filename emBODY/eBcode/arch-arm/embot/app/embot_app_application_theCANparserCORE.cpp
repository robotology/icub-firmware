
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_application_theCANparserCORE.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_core_binary.h"

#include <new>



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::application::theCANparserCORE::Impl
{  
    dummyCANagentCORE _dummyagent {};
        
    Config _config {};     

    Impl() {}
    
    bool initialise(const Config &config);      
    bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    
    bool process_bl_broadcast_appl(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);  
    bool process_bl_board_appl(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);    
    bool process_bl_getadditionalinfo(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    bool process_bl_setadditionalinfo(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    bool process_bl_setcanaddress(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    bool process_bl_gettimeoflife(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
    
    bool process_getfirmwareversion(const embot::prot::can::Clas cl, const std::uint8_t cm, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);    
    bool process_setid(const embot::prot::can::Clas cl, const std::uint8_t cm, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);
};

bool embot::app::application::theCANparserCORE::Impl::initialise(const Config &config)
{
    _config = config;
    
    if(false == _config.isvalid())
    {
        _config.agent = &_dummyagent;
    }   
         
    return true;
}
    
bool embot::app::application::theCANparserCORE::Impl::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    bool txframe = false;
    bool recognised = false;

    embot::prot::can::Clas cls {embot::prot::can::Clas::none};
    std::uint8_t cmd {0};
    
    if(false == embot::prot::can::frameis4board(frame, _config.agent->address()))
    {
        recognised = false;
        return recognised;
    }
        
    // now get _cls and _cmd
    cls = embot::prot::can::frame2clas(frame);
    cmd = embot::prot::can::frame2cmd(frame);
    
    switch(cls)
    {
        case embot::prot::can::Clas::bootloader:
        {
            // only CMD::BROADCAST, CMD::BOARD, CMD::SETCANADDRESS,  CMD::GET_ADDITIONAL_INFO,  CMD::SET_ADDITIONAL_INFO 

            if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::BOARD) == cmd)
            {
                txframe = process_bl_board_appl(frame, replies);   
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::BROADCAST) == cmd)
            {
                txframe = process_bl_broadcast_appl(frame, replies);   
                recognised = true;                
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::SETCANADDRESS) == cmd)
            {
                txframe = process_bl_setcanaddress(frame, replies);  
                recognised = true;                
            } 
            else if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::GET_ADDITIONAL_INFO) == cmd)
            {
                txframe = process_bl_getadditionalinfo(frame, replies);   
                recognised = true;                
            } 
            else if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::SET_ADDITIONAL_INFO) == cmd)
            {
                txframe = process_bl_setadditionalinfo(frame, replies);
                recognised = true;                
            }                     
            else if(static_cast<std::uint8_t>(embot::prot::can::bootloader::CMD::GET_TIMEOFLIFE) == cmd)
            {
                txframe = process_bl_gettimeoflife(frame, replies);
                recognised = true;                
            }   
            
        } break;
        

        case embot::prot::can::Clas::pollingAnalogSensor:
        {
            // only embot::prot::can::analog::polling::CMD::SET_BOARD_ADX, GET_FIRMWARE_VERSION, ??
            if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::SET_BOARD_ADX) == cmd)
            {
                txframe = process_setid(cls, cmd, frame, replies);
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::analog::polling::CMD::GET_FIRMWARE_VERSION) == cmd)
            {
                txframe = process_getfirmwareversion(cls, cmd, frame, replies);
                recognised = true;
            }
 
        } break;

        case embot::prot::can::Clas::pollingMotorControl:
        {
            // only embot::prot::can::mcpollCMD::SET_BOARD_ID, GET_FIRMWARE_VERSION, ??
            if(static_cast<std::uint8_t>(embot::prot::can::motor::polling::CMD::SET_BOARD_ID) == cmd)
            {
                txframe = process_setid(cls, cmd, frame, replies);
                recognised = true;
            }
            else if(static_cast<std::uint8_t>(embot::prot::can::motor::polling::CMD::GET_FIRMWARE_VERSION) == cmd)
            {
                txframe = process_getfirmwareversion(cls, cmd, frame, replies);
                recognised = true;
            }
 
        } break;
        
        default:
        {
            txframe = false;
            recognised = false;
        } break;
    }    
        
    return recognised;
}


bool embot::app::application::theCANparserCORE::Impl::process_bl_broadcast_appl(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_BROADCAST msg {};
    embot::prot::can::bootloader::Message_BROADCAST::ReplyInfo replyinfo {};
        
    msg.load(frame);        
    _config.agent->get(msg.info, replyinfo);
        
    embot::prot::can::Frame frame0 {};
    if(true == msg.reply(frame0, _config.agent->address(), replyinfo))
    {
        replies.push_back(frame0);
        return true;
    }        

    return false;         
}


bool embot::app::application::theCANparserCORE::Impl::process_bl_board_appl(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_BOARD msg {};
        
    msg.load(frame);    
    _config.agent->set(msg.info);
    
    // we dont reply
    return false;       
}


bool embot::app::application::theCANparserCORE::Impl::process_bl_getadditionalinfo(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO msg {};
    embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO::ReplyInfo replyinfo {};
    
    msg.load(frame);    
    _config.agent->get(msg.info, replyinfo);
   
    bool r {false};
    
    std::uint8_t nreplies = msg.numberofreplies();
    embot::prot::can::Frame frame0 {};
    for(std::uint8_t i=0; i<nreplies; i++)
    {
        if(true == msg.reply(frame0, _config.agent->address(), replyinfo))
        {
            replies.push_back(frame0);
            r = true;
        }
    }
    
    return r;   
}


bool embot::app::application::theCANparserCORE::Impl::process_bl_setadditionalinfo(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_SET_ADDITIONAL_INFO2 msg {};
        
    msg.load(frame);        
    _config.agent->set(msg.info);
        
    return false;        
}


bool embot::app::application::theCANparserCORE::Impl::process_setid(const embot::prot::can::Clas cl, const std::uint8_t cm, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::shared::Message_SET_ID msg {cl, cm};
    
    msg.load(frame);
    _config.agent->set(msg.info);
            
    return msg.reply();        
}

bool embot::app::application::theCANparserCORE::Impl::process_getfirmwareversion(const embot::prot::can::Clas cl, const std::uint8_t cm, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{    
    embot::prot::can::shared::Message_GET_VERSION msg {cl, cm};
    embot::prot::can::shared::Message_GET_VERSION::ReplyInfo replyinfo {};
    
    msg.load(frame);
    _config.agent->get(msg.info, replyinfo);  
    
    embot::prot::can::Frame frame0 {};
    if(true == msg.reply(frame0, _config.agent->address(), replyinfo))
    {            
        replies.push_back(frame0);
        return true;
    }
    
    return false;
}

bool embot::app::application::theCANparserCORE::Impl::process_bl_setcanaddress(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_SETCANADDRESS msg {};
        
    msg.load(frame);
    _config.agent->set(msg.info);   
                
    return msg.reply();        
}

bool embot::app::application::theCANparserCORE::Impl::process_bl_gettimeoflife(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{
    embot::prot::can::bootloader::Message_GET_TIMEOFLIFE msg {};
    embot::prot::can::bootloader::Message_GET_TIMEOFLIFE::ReplyInfo replyinfo {};
        
    msg.load(frame);
    _config.agent->get(msg.info, replyinfo);    

    embot::prot::can::Frame frame0 {};
    if(true == msg.reply(frame0, _config.agent->address(), replyinfo))
    {            
        replies.push_back(frame0);
        return true;
    }
    
    return false;       
}



// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------




embot::app::application::theCANparserCORE& embot::app::application::theCANparserCORE::getInstance()
{
    static theCANparserCORE* p = new theCANparserCORE();
    return *p;
}

embot::app::application::theCANparserCORE::theCANparserCORE()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::application::theCANparserCORE::~theCANparserCORE() { }
   
        
bool embot::app::application::theCANparserCORE::initialise(const Config &config)
{
    return pImpl->initialise(config);
}
  
bool embot::app::application::theCANparserCORE::process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies)
{    
    return pImpl->process(frame, replies);
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


