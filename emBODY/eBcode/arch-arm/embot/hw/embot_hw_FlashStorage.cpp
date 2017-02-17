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

#include "embot_hw_FlashStorage.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>

#if defined(USE_STM32HAL)
#include "stm32hal.h" 
#else
#include "stm32l4xx_hal.h" 
#endif

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

    
struct embot::hw::FlashStorage::Impl
{
    uint64_t *buffer;
    uint32_t pagenumber;
    uint32_t pagestart;
    uint32_t pagesize;
    bool bufferisexternal;
    
    Impl(std::uint32_t _pagestart = embot::hw::sys::addressOfStorage, std::uint32_t _pagesize = 1024, std::uint64_t * _buffer = nullptr) 
    {
        pagestart = _pagestart; // dont do any control ... just > embot::hw::sys::startOfFLASH
        if(pagestart < embot::hw::sys::startOfFLASH)
        {
             pagestart = embot::hw::sys::addressOfStorage;
        }
        pagenumber = (pagestart - embot::hw::sys::startOfFLASH) / 2048; // pages in here are of size 2k.
        
        pagesize = _pagesize;
        if(pagesize > 2048)
        {
            pagesize = 2048;
        }
        if(0 == pagesize)
        {
             pagesize = 64;
        }
        
        if(nullptr == _buffer)
        {
            bufferisexternal = false;
            buffer = new std::uint64_t[pagesize/sizeof(std::uint64_t)];
        }
        else
        {
            bufferisexternal = true;
            buffer = _buffer;
        }

        HAL_FLASH_Unlock();
    }
    
    ~Impl()
    {
        if(false == bufferisexternal)
        {
            delete[] buffer;
        }
    }
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::hw::FlashStorage::FlashStorage(std::uint32_t pagestart, std::uint32_t pagesize, std::uint64_t *buffer)
: pImpl(new Impl(pagestart, pagesize, buffer))
{   

}

embot::hw::FlashStorage::~FlashStorage()
{   
    delete pImpl;
}

namespace embot { namespace hw {
    

        
bool embot::hw::FlashStorage::isInitted()
{   
    return true;
}


bool embot::hw::FlashStorage::isAddressValid(std::uint32_t address)
{
    if((address < pImpl->pagestart) || (address > (pImpl->pagestart+pImpl->pagesize)) )
    {
        return false;
    }
    return true;
}

std::uint32_t embot::hw::FlashStorage::getBaseAddress()
{
    return pImpl->pagestart;    
}

std::uint32_t embot::hw::FlashStorage::getSize()
{
    return pImpl->pagesize;    
}

bool embot::hw::FlashStorage::fullerase()
{
    // erase page
    FLASH_EraseInitTypeDef erase = {0};
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Banks = FLASH_BANK_1;
    erase.Page = pImpl->pagenumber;
    erase.NbPages = 1;
    uint32_t pagenum = 0;
    HAL_FLASH_Unlock();
    int a = HAL_FLASHEx_Erase(&erase, &pagenum);
    a = a;
    HAL_FLASH_Lock();
    return true;
}


bool embot::hw::FlashStorage::erase(std::uint32_t address, std::uint32_t size)
{
    if(false == isAddressValid(address))
    {
        return false;
    }
    if(false == isAddressValid(address+size))
    {
        return false;
    }
        
    return false; 
}


bool embot::hw::FlashStorage::read(std::uint32_t address, std::uint32_t size, void *data)
{
    if(false == isAddressValid(address))
    {
        return false;
    }
    if(false == isAddressValid(address+size))
    {
        return false;
    }
    
    if(NULL == data)
    {
        return false;
    }
    
    // can read directly from flash
    void *st = (void*) address;
    std::memmove(data, st, size); 
    
    return true;
}

bool embot::hw::FlashStorage::write(std::uint32_t address, std::uint32_t size, const void *data)
{
    if(false == isAddressValid(address))
    {
        return false;
    }
    if(false == isAddressValid(address+size))
    {
        return false;
    }
    
    if(NULL == data)
    {
        return false;
    }
    
    // read all flash with buffer.  
    if(false == read(pImpl->pagestart, pImpl->pagesize, pImpl->buffer))
    {
        return false;
    }
    
    // change the buffer
    uint32_t reducedaddress = address - pImpl->pagestart;
    std::memmove(&pImpl->buffer[reducedaddress/8], data, size);
    // erase page
    fullerase();
    
    HAL_FLASH_Unlock();
    
    uint32_t tmpadr = address;
    uint32_t n64bitwords = pImpl->pagesize / 8;
    for(uint16_t i=0; i<n64bitwords; i++)
    {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, tmpadr, pImpl->buffer[i]);
        tmpadr += 8;
    }

    volatile uint32_t xx =  HAL_FLASH_GetError();
    
    xx= xx;
    
    HAL_FLASH_Lock();
    
    return true;    
}


 
}} // namespace embot { namespace hw {
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

