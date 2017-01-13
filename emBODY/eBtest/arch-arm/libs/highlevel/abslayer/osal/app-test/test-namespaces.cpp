/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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





#include "test-namespaces.h"


#include "stdlib.h" 
#include "stdint.h" 
#include "osal.h"

#include <cstring>

#include <vector>


struct embot::sys::theStorage::Impl
{
    uint32_t one;
    uint32_t two; 
    embot::if2hw::storage *hwstrg;
    Impl() 
    {
        one = 0;
        two = 0;
        hwstrg = nullptr;
    }
};

embot::sys::theStorage::theStorage()
: pImpl(new Impl)
{   

}

bool embot::sys::theStorage::load(embot::if2hw::storage &strg)
{   
    pImpl->hwstrg = &strg;
    return true;    
}

bool embot::sys::theStorage::read(uint32_t address, uint32_t size, uint8_t *data)
{
    if(NULL == pImpl->hwstrg)
    {
        return false;
    }
    
    return pImpl->hwstrg->read(address, size, data);
}    

bool embot::sys::theStorage::write(uint32_t address, uint32_t size, const uint8_t *data)
{
    if(nullptr == pImpl->hwstrg)
    {
        return false;
    }
    
    return pImpl->hwstrg->write(address, size, data);
}  

bool embot::sys::theStorage::erase(uint32_t address, uint32_t size)
{
    if(nullptr == pImpl->hwstrg)
    {
        return false;
    }
    
    return pImpl->hwstrg->erase(address, size);
}  


bool embot::sys::theStorage::erase()
{
    if(nullptr == pImpl->hwstrg)
    {
        return false;
    }
    
    return pImpl->hwstrg->erase();
}  


struct embot::hw::emulEEPROM::Impl
{
    enum class Value: std::uint8_t { erasedbyte = 0xff };
    uint8_t *buffer;
    Config config;
    Impl() 
    {
        buffer = nullptr;
    }
    ~Impl()
    {
        reset();
    }
    void reset()
    {
        config.reset();
        if(!buffer)
        {
            delete[] buffer;
            buffer = nullptr;
        }        
    }
};

embot::hw::emulEEPROM::emulEEPROM()
: pImpl(new Impl)
{
}

embot::hw::emulEEPROM::~emulEEPROM()
{
    delete pImpl;
}

bool embot::hw::emulEEPROM::init(const emulEEPROM::Config &config)
{
    if((Config::emulPlace::nowhere == config.place) || (0 == config.capacity) || (0 == config.pagesize))
    {
        return false;
    }
    
    pImpl->config = config;
    pImpl->buffer = new uint8_t[pImpl->config.capacity];
    
    // now we must:
    // 1. init the flash partition we need (set rw etc ...)
    // 2. make sure it is erased ... for instance ... read it all and check a crc32 place da the end of partition 
    // 3. read from flash and put into buffer.
    
    
    return true;  
}


bool embot::hw::emulEEPROM::uninit()
{    
    // now we must:
    // 1. uninit the flash partition

    pImpl->config.reset();

    return true;  
}

bool embot::hw::emulEEPROM::read(uint32_t address, uint32_t size, uint8_t *data)
{  
    if(nullptr == pImpl->buffer)
    {   // not initted yet
        return false; 
    }
    
    if( ((address) > pImpl->config.capacity) || ((address+size) > pImpl->config.capacity) )
    {   // out of bound
        return false;         
    }
    
    if(nullptr == data)
    {   // NULL destination
        return false;
    }

    
    bool ret = false;
    switch(pImpl->config.place)
    {
        case Config::emulPlace::inRAM:
        {
            const void * from = (const void*) (pImpl->config.addressofzero + address);            
            std::memmove(data, from, size);
            ret =  true;
        } break;

        case Config::emulPlace::inFLASH:
        {   // must call flash funtions to copy from to .... 
            //const void * from = (const void*) (pImpl->config.addressofzero + address);  
            ret = false;
        } break;
        
        case Config::emulPlace::nowhere:
        default:
        {
            ret = false;
        } break;
                   
    }

    return ret;  
}

bool embot::hw::emulEEPROM::write(uint32_t address, uint32_t size, const uint8_t *data)
{  
    if(nullptr == pImpl->buffer)
    {   // not initted yet
        return false; 
    }
    
    if( ((address) > pImpl->config.capacity) || ((address+size) > pImpl->config.capacity) )
    {   // out of bound
        return false;         
    }
    
    if(nullptr == data)
    {   // NULL destination
        return false;
    }


    bool ret = false;
    switch(pImpl->config.place)
    {
        case Config::emulPlace::inRAM:
        {
            void * to = (void*) (pImpl->config.addressofzero + address);            
            std::memmove(to, data, size);
            ret =  true;
        } break;

        case Config::emulPlace::inFLASH:
        {   // must call flash funtions to copy entire page, change the page, write back.
            // read and verify ...            
            //const void * from = (const void*) (pImpl->config.addressofzero + address);  
            ret = false;
        } break;
        
        case Config::emulPlace::nowhere:
        default:
        {
            ret = false;
        } break;
                   
    }

    return ret;  
}


bool embot::hw::emulEEPROM::erase(uint32_t address, uint32_t size)
{
    if(nullptr == pImpl->buffer)
    {   // not initted yet
        return false; 
    }
    
    if( ((address) > pImpl->config.capacity) || ((address+size) > pImpl->config.capacity) )
    {   // out of bound
        return false;         
    }
    
    bool ret = false;
    switch(pImpl->config.place)
    {
        case Config::emulPlace::inRAM:
        {
            void * to = (void*) (pImpl->config.addressofzero + address);  
            std::memset(to, static_cast<int>(Impl::Value::erasedbyte), size);            
            ret =  true;
        } break;

        case Config::emulPlace::inFLASH:
        {   // must call flash funtions to copy entire page, change the page, write back.
            // read and verify ...            
            //const void * from = (const void*) (pImpl->config.addressofzero + address);  
            ret = false;
        } break;
        
        case Config::emulPlace::nowhere:
        default:
        {
            ret = false;
        } break;
                   
    }

    return ret;  
}

bool embot::hw::emulEEPROM::erase()
{
    return erase(0, pImpl->config.capacity);
}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






