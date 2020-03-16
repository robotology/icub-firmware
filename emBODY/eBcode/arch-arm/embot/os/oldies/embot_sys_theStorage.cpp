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

#error dont use me [ i am not a true embot::os ]

// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_sys_theStorage.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::sys::theStorage::Impl
{    
    bool initted;
    embot::common::Storage *storage;
    
    Impl() 
    {              
        initted = false;
        storage = nullptr;
    }
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::sys::theStorage::theStorage()
: pImpl(new Impl)
{   

}


bool embot::sys::theStorage::init(embot::common::Storage *storage)
{   
    if(nullptr != storage)
    {
        pImpl->storage = storage;    
        pImpl->initted = true;
    }
    
    return pImpl->initted;    
}

bool embot::sys::theStorage::isInitted()
{
    return pImpl->initted;
}  

bool embot::sys::theStorage::isAddressValid(std::uint32_t address)
{
    if(false == pImpl->initted)
    {
        return false;
    }
    
    return pImpl->storage->isAddressValid(address);
}  

std::uint32_t embot::sys::theStorage::getBaseAddress()
{
    if(false == pImpl->initted)
    {
        return 0;
    }
    
    return pImpl->storage->getBaseAddress();
}  

std::uint32_t embot::sys::theStorage::getSize()
{
    if(false == pImpl->initted)
    {
        return 0;
    }
    
    return pImpl->storage->getSize();
} 

 
bool embot::sys::theStorage::fullerase()
{
    if(false == pImpl->initted)
    {
        return false;
    }
    
    return pImpl->storage->fullerase();
}  
 
bool embot::sys::theStorage::erase(std::uint32_t address, std::uint32_t size)
{
    if(false == pImpl->initted)
    {
        return false;
    }
    
    return pImpl->storage->erase(address, size);
}  

bool embot::sys::theStorage::read(std::uint32_t address, std::uint32_t size, void *data)
{
    if(false == pImpl->initted)
    {
        return false;
    }
    
    return pImpl->storage->read(address, size, data);
} 

bool embot::sys::theStorage::write(std::uint32_t address, std::uint32_t size, const void *data)
{
    if(false == pImpl->initted)
    {
        return false;
    }
    
    return pImpl->storage->write(address, size, data);
} 
        

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


