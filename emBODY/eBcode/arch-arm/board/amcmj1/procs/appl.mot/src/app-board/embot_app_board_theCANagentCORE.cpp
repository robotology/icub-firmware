
/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_theCANagentCORE.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include "embot_core_binary.h"

#include <new>


#include "embot_hw_sys.h"
#include "embot_hw_can.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::board::theCANagentCORE::Impl
{  
    
    static constexpr embot::prot::can::Board theboard {embot::prot::can::Board::none};
        
    Config _config {};
        
    static const std::uint32_t validityKey = 0x8888dead;
    struct StoredInfo
    {   // contains the basics stored inside some embot::i2h::Storage
        std::uint32_t       key {validityKey};
        std::uint8_t        address {3};
        std::uint8_t        boardtype { embot::core::tointegral(theboard) };
        std::uint8_t        bootloaderVmajor {0};
        std::uint8_t        bootloaderVminor {0};
        std::uint8_t        info32[32] {"boardnone"};
        std::uint8_t        applicationVmajor {0};
        std::uint8_t        applicationVminor {0};
        std::uint8_t        applicationVbuild {0};
        std::uint8_t        protocolVmajor {2};
        std::uint8_t        protocolVminor {0};
        
        std::uint8_t        tobefilled[3];  // to make the size of struct ... multiple of 8.
    };         
        
    StoredInfo _storedinfo = {};  
    
    embot::prot::can::applicationInfo _applicationinfo {};

    embot::prot::can::Board _board {theboard};
    embot::hw::CAN _canbus {embot::hw::CAN::two};

    
    Impl() { }
                    
    bool initialise(const Config &config);
    
    bool setcanaddress(const std::uint8_t adr, const std::uint16_t randominvalidmask);
};

bool embot::app::board::theCANagentCORE::Impl::initialise(const Config &config)
{
    _config = config;
            
    // retrieve board type, version of application, can address
    // we use the object theCANboardInfo which is based onto FlashStorage which manages 
    // the flash partition called Partition::ID::sharedstorage

    _storedinfo.address = _config.location.getadr();
    _storedinfo.boardtype = embot::core::tointegral(_config.board);
    _storedinfo.applicationVmajor = _config.applicationinfo.version.major;
    _storedinfo.applicationVminor = _config.applicationinfo.version.minor;
    _storedinfo.applicationVbuild = _config.applicationinfo.version.build;
    _storedinfo.protocolVmajor = _config.applicationinfo.protocol.major;
    _storedinfo.protocolVminor = _config.applicationinfo.protocol.minor;
  
    std::memmove(_storedinfo.info32, _config.boardinfo, sizeof(_storedinfo.info32)); 
           
    _board = _config.board;
    embot::app::msg::BUS bus = _config.location.getbus();
    if(true == _config.location.isCAN())
    {
        _canbus = (embot::app::msg::BUS::can1 == bus) ? (embot::hw::CAN::one) : (embot::hw::CAN::two);
    }
    else
    {
        _canbus = embot::hw::CAN::none;
    }
    _applicationinfo.version.major = _storedinfo.applicationVmajor;
    _applicationinfo.version.minor = _storedinfo.applicationVminor;
    _applicationinfo.version.build = _storedinfo.applicationVbuild;
    _applicationinfo.protocol.major = _storedinfo.protocolVmajor;
    _applicationinfo.protocol.minor = _storedinfo.protocolVminor;                

    // note: we could make it even more general by using inside _config a pure interface class which
    // does the job of theCANboardInfo, in such a way we just specialise this class ... but maybe later
         
    return true;
}
 

bool embot::app::board::theCANagentCORE::Impl::setcanaddress(const std::uint8_t adr, const std::uint16_t randominvalidmask)
{
    
    // i reinforce a reading from storage. just for safety. in here we are dealing w/ can address change and i want to be sure.
    std::uint8_t address = _storedinfo.address;
    
    std::uint8_t target = adr;
    
    if(0xff == adr)
    {
        // compute a new random address. use the randominvalidmask to filter out the undesired values. for sure 0x8001.
        std::uint16_t mask = randominvalidmask;
        mask |= 0x8001;
        if(0xffff == mask)
        {   // hei, nothing is good for you.
            return false;
        }
        
        bool ok = false;
        for(std::uint16_t i=0; i<250; i++)
        {
            target = (embot::hw::sys::random()-embot::hw::sys::minrandom()) & 0xf;
           
            if(false == embot::core::binary::bit::check(mask, target))
            {
                ok = true;
                break;                    
            }
        }
        
        if(!ok)
        {
            return false;
        }
    }
    
    // always check that is is not 0 or 0xf
    if((0 == target) || (0xf == target))
    {
        return false;
    }
        
    
    if(address != target)
    {
        if(embot::hw::CAN::none != _canbus)
        {
            if(embot::hw::resOK != embot::hw::can::setfilters(_canbus, target))
            {
                return false;
            }
        }
        _storedinfo.address = target;
        address = _storedinfo.address;
    }
                    
    return (target == address);
}


// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------


embot::app::board::theCANagentCORE& embot::app::board::theCANagentCORE::getInstance()
{
    static theCANagentCORE* p = new theCANagentCORE();
    return *p;
}

embot::app::board::theCANagentCORE::theCANagentCORE()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::board::theCANagentCORE::~theCANagentCORE() { }
   
        
bool embot::app::board::theCANagentCORE::initialise(const Config &config)
{
    return pImpl->initialise(config);
}
  

// its interfaces to CANagentCORE


const embot::prot::can::applicationInfo & embot::app::board::theCANagentCORE::applicationinfo() const
{    
    return pImpl->_applicationinfo;
}

embot::hw::CAN embot::app::board::theCANagentCORE::bus() const
{ 
    return pImpl->_canbus;
}

embot::prot::can::Address embot::app::board::theCANagentCORE::address() const
{    
    return pImpl->_storedinfo.address;
}


bool embot::app::board::theCANagentCORE::get(const embot::prot::can::bootloader::Message_BROADCAST::Info &info, embot::prot::can::bootloader::Message_BROADCAST::ReplyInfo &replyinfo) 
{   
    replyinfo.board = pImpl->_board;
    replyinfo.process = embot::prot::can::Process::application;
    replyinfo.firmware = {pImpl->_applicationinfo.version.major, pImpl->_applicationinfo.version.minor, pImpl->_applicationinfo.version.build};
                 
    return true;    
}


bool embot::app::board::theCANagentCORE::get(const embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO::Info &info, embot::prot::can::bootloader::Message_GET_ADDITIONAL_INFO::ReplyInfo &replyinfo)
{
    std::memmove(replyinfo.info32, pImpl->_storedinfo.info32, sizeof(replyinfo.info32)); 
    
    return true;
}


bool embot::app::board::theCANagentCORE::get(const embot::prot::can::shared::Message_GET_VERSION::Info &info, embot::prot::can::shared::Message_GET_VERSION::ReplyInfo &replyinfo)
{    
    replyinfo.board = pImpl->_board;
    replyinfo.firmware = {pImpl->_applicationinfo.version.major, pImpl->_applicationinfo.version.minor, pImpl->_applicationinfo.version.build};
    replyinfo.protocol = pImpl->_applicationinfo.protocol;
    
    return true;
}
    
bool embot::app::board::theCANagentCORE::get(const embot::prot::can::bootloader::Message_GET_TIMEOFLIFE::Info &info, embot::prot::can::bootloader::Message_GET_TIMEOFLIFE::ReplyInfo &replyinfo)
{
    replyinfo.timeoflife = embot::core::now();
    
    return true;
}


bool embot::app::board::theCANagentCORE::set(const embot::prot::can::bootloader::Message_BOARD::Info &info)
{
//    // we just restart so we cannot reply
//    embot::hw::sys::reset();
    
    return false;
}


bool embot::app::board::theCANagentCORE::set(const embot::prot::can::bootloader::Message_SET_ADDITIONAL_INFO2::Info &info)
{
    if(true == info.valid)
    {   // we have received all the 8 messages in order (important is that the one with data[1] = 0 is the first)
//        embot::app::theCANboardInfo::getInstance().get(pImpl->_storedinfo);    
        std::memmove(pImpl->_storedinfo.info32, info.info32, sizeof(pImpl->_storedinfo.info32));
//        embot::app::theCANboardInfo::getInstance().set(pImpl->_storedinfo);
    }   
    
    return false;
}


bool embot::app::board::theCANagentCORE::set(const embot::prot::can::shared::Message_SET_ID::Info &info)
{
    pImpl->setcanaddress(info.address, 0x0000);
    
    return false;
}


bool embot::app::board::theCANagentCORE::set(const embot::prot::can::bootloader::Message_SETCANADDRESS::Info &info)
{
    pImpl->setcanaddress(info.address, info.randominvalidmask);
    
    return false;
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


