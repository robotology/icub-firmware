/*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
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

#include "embot_app_theStorage.h"

#include "embot.h"

#include <cstring>



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_FlashStorage.h"
//#include "embot_sys_theStorage.h"
#include "embot_hw.h"
#include "embot_hw_sys.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



struct embot::app::theStorage::Impl
{
    embot::hw::FlashStorage *flashstorage;
    std::uint32_t pageaddress;
    std::uint32_t pagesize;
    std::uint64_t buffer[capacity/sizeof(std::uint64_t)];
    Impl() 
    {    
        pageaddress = 0;        
        pagesize = 0;        
        flashstorage = nullptr; 
        std::memset(buffer, 0, sizeof(buffer));
//        embot::sys::theStorage &thestrg = embot::sys::theStorage::getInstance();
//        thestrg.init(flashstorage);    
    }
};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------




embot::app::theStorage& embot::app::theStorage::getInstance()
{
    static theStorage* p = new theStorage();
    return *p;
}

embot::app::theStorage::theStorage()
//    : pImpl(new Impl)
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::theStorage::~theStorage() { }


bool embot::app::theStorage::init(const std::uint32_t address, const std::uint32_t size)
{
    if(nullptr != pImpl->flashstorage)
    {
        return false;
    }
    if(size > capacity)
    {
        return false;
    }
    pImpl->pagesize = size;
    pImpl->pageaddress = address;
    pImpl->flashstorage = new embot::hw::FlashStorage(address, size, pImpl->buffer);
    
    return true;
}

bool embot::app::theStorage::erase()
{
    if(nullptr == pImpl->flashstorage)
    {
        return false;
    }
    return pImpl->flashstorage->fullerase();
}
   


bool embot::app::theStorage::write(const std::uint32_t atoffset, const std::uint32_t size, const void *data)
{
    if(nullptr == pImpl->flashstorage)
    {
        return false;
    }
    return pImpl->flashstorage->write(pImpl->pageaddress+atoffset, size, data);    
}

bool embot::app::theStorage::read(const std::uint32_t atoffset, const std::uint32_t size, void *data)
{
    if(nullptr == pImpl->flashstorage)
    {
        return false;
    }
    return pImpl->flashstorage->read(pImpl->pageaddress+atoffset, size, data);    
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


