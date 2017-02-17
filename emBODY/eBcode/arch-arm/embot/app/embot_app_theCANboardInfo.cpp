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
   
 
bool embot::app::theCANboardInfo::synch(Type type, std::uint8_t adr)
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


bool embot::app::theCANboardInfo::write(std::uint32_t address, std::uint32_t size, const void *data)
{
    return pImpl->flashstorage->write(pImpl->userdataaddress, size, data);    
}

bool embot::app::theCANboardInfo::read(std::uint32_t address, std::uint32_t size, void *data)
{
    return pImpl->flashstorage->read(pImpl->userdataaddress, size, data);    
}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


