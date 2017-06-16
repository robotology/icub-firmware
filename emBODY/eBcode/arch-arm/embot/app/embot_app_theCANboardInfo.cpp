/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_theCANboardInfo.h"

#include "embot.h"

#include <cstring>



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_FlashStorage.h"
#include "embot_sys_theStorage.h"
#include "embot_hw.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



struct embot::app::theCANboardInfo::Impl
{    
    embot::hw::FlashStorage *flashstorage;
    const std::uint32_t userdataaddress = embot::hw::sys::addressOfStorage + 1024 - embot::app::theCANboardInfo::sizeOfUserSpace;
    std::uint64_t buffer[1024/sizeof(std::uint64_t)];
    Impl() 
    {              
        flashstorage = new embot::hw::FlashStorage(embot::hw::sys::addressOfStorage, 1024, buffer); 
        embot::sys::theStorage &thestrg = embot::sys::theStorage::getInstance();
        thestrg.init(flashstorage);   
    }
};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::app::theCANboardInfo::theCANboardInfo()
: pImpl(new Impl)
{       

}



bool embot::app::theCANboardInfo::erase()
{    
    return pImpl->flashstorage->fullerase();
}
   
 
bool embot::app::theCANboardInfo::synch(embot::app::canprotocol::Board type, embot::app::canprotocol::versionOfBOOTLOADER version, std::uint8_t adr, const char *defInfo32)
{
    bool ret = false;
    // at first we read. if what we read is not nice, then we write again
    StoredInfo strd = {0};
    ret = get(strd);
    
    if(validityKey != strd.key)
    {
        std::memset(&strd, 0, sizeof(strd));
        strd.key = validityKey;
        strd.boardtype = static_cast<std::uint8_t>(type);
        strd.canaddress = adr;  
        strd.bootloaderVmajor = version.major;
        strd.bootloaderVminor = version.minor;
        if(nullptr != defInfo32)
        {
            std::memmove(strd.info32, defInfo32, sizeof(strd.info32));
        }
        
        ret = set(strd);
    }
    else if((strd.boardtype != static_cast<std::uint8_t>(type)) || (version.major != strd.bootloaderVmajor) || (version.minor != strd.bootloaderVminor))
    {
        // make sure that the board type is the one we want. with the address we give
        strd.boardtype = static_cast<std::uint8_t>(type);
        strd.canaddress = adr;  
        strd.bootloaderVmajor = version.major;
        strd.bootloaderVminor = version.minor;
        if(nullptr != defInfo32)
        {
            std::memmove(strd.info32, defInfo32, sizeof(strd.info32));
        }
        
        ret = set(strd);        
    }
    
    return ret;
}


bool embot::app::theCANboardInfo::synch(embot::app::canprotocol::versionOfAPPLICATION application, embot::app::canprotocol::versionOfCANPROTOCOL protocol)
{
    bool ret = false;
    // at first we read. if what we read is not nice, then we write again
    StoredInfo strd = {0};
    ret = get(strd);
    
    if(validityKey != strd.key)
    {
        std::memset(&strd, 0, sizeof(strd));
        strd.key = validityKey;
        strd.boardtype = static_cast<std::uint8_t>(embot::app::canprotocol::Board::unknown);
        strd.canaddress = 1;  
        strd.bootloaderVmajor = 0;
        strd.bootloaderVminor = 0;
        strd.applicationVmajor = application.major;
        strd.applicationVminor = application.minor;
        strd.applicationVbuild = application.build;
        strd.protocolVmajor = protocol.major;
        strd.protocolVminor = protocol.minor;
        
        ret = set(strd);
    }
    else if((protocol.major != strd.protocolVmajor) || (protocol.minor != strd.protocolVminor) || 
            (application.major != strd.applicationVmajor) || (application.minor != strd.applicationVminor) || (application.build != strd.applicationVbuild))
    {
        strd.applicationVmajor = application.major;
        strd.applicationVminor = application.minor;
        strd.applicationVbuild = application.build;
        strd.protocolVmajor = protocol.major;
        strd.protocolVminor = protocol.minor;
        
        ret = set(strd);        
    }
    
    return ret;
}



bool embot::app::theCANboardInfo::get(StoredInfo &info)
{
    return pImpl->flashstorage->read(embot::hw::sys::addressOfStorage, sizeof(StoredInfo), &info);
}


bool embot::app::theCANboardInfo::set(const StoredInfo &info)
{
    return pImpl->flashstorage->write(embot::hw::sys::addressOfStorage, sizeof(StoredInfo), &info);
}
 
 
std::uint8_t embot::app::theCANboardInfo::getCANaddress()
{
    StoredInfo strd = {0};
    get(strd); 
    return strd.canaddress;
}


bool embot::app::theCANboardInfo::setCANaddress(std::uint8_t adr)
{
    StoredInfo strd = {0};
    get(strd); 
    strd.canaddress = adr;
    return set(strd);   
}


bool embot::app::theCANboardInfo::userdatawrite(std::uint32_t address, std::uint32_t size, const void *data)
{
    return pImpl->flashstorage->write(pImpl->userdataaddress+address, size, data);    
}

bool embot::app::theCANboardInfo::userdataread(std::uint32_t address, std::uint32_t size, void *data)
{
    return pImpl->flashstorage->read(pImpl->userdataaddress+address, size, data);    
}

bool embot::app::theCANboardInfo::userdataerase()
{
    return false;
    #warning TOBEDONE: implement selected parts of a flash page or ... chose a different page for user-def storage ....
    //pImpl->flashstorage->erase(pImpl->userdataaddress, sizeOfUserSpace);    
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


