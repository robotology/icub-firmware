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

#include "embot_hw_FlashBurner.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include "stm32hal.h" 
#include "embot_hw_bsp.h"
#include "embot_hw_flash.h"
#include "embot_hw_flash_bsp.h"


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

  
//#define TEST_DONT_USE_FLASH
  
struct embot::hw::FlashBurner::Impl
{    

    struct Buffer
    {
        bool        isexternal;
        uint32_t    size;
        uint64_t*   data;
        uint8_t*    datamirror08;
        embot::hw::flash::Page::Index pageindex; 
        uint32_t offset;
    };
    
    uint32_t start;
    uint32_t size;
    
    Buffer buffer;
    
    bool isallerased;

    // by this, the PAGEsize is made dependent on the bsp. it is initted in the ctor 
    uint32_t PAGEsize = 2048; 
            
    bool eraseall()
    {
        if(isallerased)
        {
            return true;
        }

        embot::hw::flash::Page firstpage = embot::hw::sys::page(start);
        embot::hw::flash::Page lastpage = embot::hw::sys::page(start+size-1);
        embot::hw::flash::Page::Index firstpageIndex = firstpage.index;        
        embot::hw::flash::Page::Index lastpageIndex = lastpage.index; 
 
        uint32_t numofpages = lastpageIndex - firstpageIndex + 1;
        
        FLASH_EraseInitTypeDef erase = {0};
        erase.TypeErase = FLASH_TYPEERASE_PAGES;
        erase.Banks = FLASH_BANK_1;
        erase.Page = firstpageIndex;
        erase.NbPages = numofpages;
        uint32_t pagenum = 0;
#if !defined(TEST_DONT_USE_FLASH)        
        HAL_FLASH_Unlock();
        int a = HAL_FLASHEx_Erase(&erase, &pagenum);
        a = a;
        HAL_FLASH_Lock();
#endif   
     
        isallerased = true;
     
        return true;        
    }
    
    bool writebuffer()
    {
        if(false == isallerased)
        {
            eraseall();
        }
        
        HAL_FLASH_Unlock();
        
        constexpr embot::hw::flash::Bank::ID bankid {embot::hw::flash::Bank::ID::one};
        embot::hw::flash::ADDR tmpadr = embot::hw::sys::address(bankid, buffer.pageindex);
        uint32_t n64bitwords = PAGEsize / 8;
        for(uint32_t i=0; i<n64bitwords; i++)
        {
#if !defined(TEST_DONT_USE_FLASH)            
            if(isaddressvalid(tmpadr))
            {                
                HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, tmpadr, buffer.data[i]);
            }
#endif
            tmpadr += 8;
        }

#if !defined(TEST_DONT_USE_FLASH)
        volatile uint32_t xx =  HAL_FLASH_GetError();       
        xx= xx;
#endif        
        HAL_FLASH_Lock();  

        return true;
    }
    
    
    Impl(std::uint32_t _start, std::uint32_t _size, std::uint32_t _buffersize = 2048, std::uint64_t * _buffer = nullptr) 
    {
        start = _start; // dont do any control ... just that start is a valid address
        if(false == embot::hw::flash::isvalid(start)) 
        {
            start = embot::hw::sys::partition(embot::hw::flash::Partition::ID::application).address; 
        }
        
        // init the page size used by the flash at that address. we assume that it stays the same
        embot::hw::flash::Page firstpage = embot::hw::sys::page(start); 
                
        PAGEsize = firstpage.size;
        _buffersize = PAGEsize;
        size = _size;
        
        buffer.pageindex = embot::hw::flash::Page::InvalidIndex;
        buffer.offset = 0;
        
        buffer.size = _buffersize;
        if(buffer.size > PAGEsize)
        {
            buffer.size = PAGEsize;
        }
        if(0 == buffer.size)
        {
             buffer.size = 64;
        }
        
        if(nullptr == _buffer)
        {
            buffer.isexternal = false;
            buffer.data = new std::uint64_t[buffer.size/sizeof(std::uint64_t)];
        }
        else
        {
            buffer.isexternal = true;
            buffer.data = _buffer;
        }

        buffer.datamirror08 = reinterpret_cast<uint8_t*>(buffer.data);
        
        isallerased = false;
        

        HAL_FLASH_Unlock();
    }
    
    ~Impl()
    {
        if(false == buffer.isexternal)
        {
            delete[] buffer.data;
        }
    }

    bool isaddressvalid(std::uint32_t address)
    {
        if((address < start) || (address > (start+size)) )
        {
            return false;
        }
        return true;
    }    
};


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::hw::FlashBurner::FlashBurner(std::uint32_t start, std::uint32_t size, std::uint32_t buffersize, std::uint64_t *buffer)
: pImpl(new Impl(start, size, buffersize, buffer))
{   

}

embot::hw::FlashBurner::~FlashBurner()
{   
    delete pImpl;
}
    

bool embot::hw::FlashBurner::isAddressValid(std::uint32_t address)
{
    return pImpl->isaddressvalid(address);
}


bool embot::hw::FlashBurner::add(std::uint32_t address, std::uint32_t size, const void *data)
{
    if(false == isAddressValid(address))
    {
        return false;
    }
    
    if(0 == size)
    {
        return false;
    } 
    
    // i DO NOT manage data so big that is amongst more pages, aka size > ...
    if(size >  pImpl->buffer.size)
    {
        return false;
    }
    
    // i put data inside the buffer.    
    embot::hw::flash::Page firstpage = embot::hw::sys::page(address);   
    embot::hw::flash::Page lastpage = embot::hw::sys::page(address+size-1); 
    
    embot::hw::flash::Page::Index currpageIndex = firstpage.index;        
    embot::hw::flash::Page::Index lastpageIndex = lastpage.index;
        
    bool dataallinside1page = (lastpageIndex == currpageIndex);
    bool dataisinanewpage = (currpageIndex != pImpl->buffer.pageindex);
     
    size_t curroffset = address - firstpage.address;    
            
    
    if(dataisinanewpage)
    {   // must process the old page   
        if(embot::hw::flash::Page::InvalidIndex == pImpl->buffer.pageindex)
        {   // first (valid) data ever. we just erase
            pImpl->eraseall();
        }
        else
        {   // all other times means that in buffer there is data.
            pImpl->writebuffer();
        }
        
        // erase the buffer.data, assign the new page to the buffer
        std::memset(pImpl->buffer.data, 0xff, pImpl->buffer.size);
        pImpl->buffer.pageindex = currpageIndex;    
    }    
        
    if(dataallinside1page)
    {   // simple...
                
        // copy data in the offset
        std::memmove(&pImpl->buffer.datamirror08[curroffset], data, size);            
    }
    else
    {   // we have some data inside the current page and some inside the next page. we only have two blocks at consecutive pages
        uint32_t size1 = pImpl->buffer.size - curroffset;
        uint32_t size2 = size - size1;
        uint8_t *data1 = (uint8_t*) data;
        uint8_t *data2 = &data1[size1];
        
        // first block: copy from offset and write buffer
        std::memmove(&pImpl->buffer.datamirror08[curroffset], data1, size1); 
        pImpl->writebuffer();
        // second block: erase buffer, set the new pagenumber, copy at the beginnig
        std::memset(pImpl->buffer.data, 0xff, pImpl->buffer.size);
        pImpl->buffer.pageindex = currpageIndex+1; 
        std::memmove(&pImpl->buffer.datamirror08[0], data2, size2);         
    }    
        
    return true;

}

bool embot::hw::FlashBurner::flush()
{
    if(embot::hw::flash::Page::InvalidIndex == pImpl->buffer.pageindex)
    {   // never had data inside. cannot write
        return true;
    }
    else
    {   // all other times means that in buffer there is data.
        pImpl->writebuffer();
    }  

    return true;    
}



bool embot::hw::FlashBurner::erase()
{ 
    return pImpl->eraseall();
}





 
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

