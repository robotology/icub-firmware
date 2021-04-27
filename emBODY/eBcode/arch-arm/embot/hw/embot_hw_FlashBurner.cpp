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
        uint32_t    page;
        uint32_t    offset;
    };
    
    uint32_t start;
    uint32_t size;
    
    Buffer buffer;
    
    bool isallerased;

    // by this, the PAGEsize is made dependent on the bsp. it is initted in the ctor 
    uint32_t PAGEsize = 2048; 
    
    static const uint32_t noPAGE = 0xffffffff;
        
    bool eraseall()
    {
        if(isallerased)
        {
            return true;
        }
        
        uint32_t firstpage = embot::hw::flash::address2page(start);
        uint32_t numofpages = embot::hw::flash::address2page(start+size) - firstpage;
            
        FLASH_EraseInitTypeDef erase = {0};
        erase.TypeErase = FLASH_TYPEERASE_PAGES;
        erase.Banks = FLASH_BANK_1;
        erase.Page = firstpage;
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
        
        uint32_t tmpadr = embot::hw::flash::page2address(buffer.page);
        uint32_t n64bitwords = PAGEsize / 8;
        for(uint32_t i=0; i<n64bitwords; i++)
        {
#if !defined(TEST_DONT_USE_FLASH)            
            HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, tmpadr, buffer.data[i]);
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
        start = _start; // dont do any control ... just > embot::hw::sys::startOfFLASH
        if(start < embot::hw::flash::getpartition(embot::hw::FLASH::whole).address) // embot::hw::sys::startOfFLASH
        {
             start = embot::hw::flash::getpartition(embot::hw::FLASH::application).address; //embot::hw::sys::addressOfApplication;
        }
        
        // init the page size used by the flash 
        PAGEsize = embot::hw::flash::getBSP().getPROP(embot::hw::FLASH::whole)->partition.pagesize;
        
        size = _size;
        
        buffer.page = noPAGE;
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
        //buffer.datamirror08 = static_cast<uint8_t*>(buffer.data);
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
    if((address < pImpl->start) || (address > (pImpl->start+pImpl->size)) )
    {
        return false;
    }
    return true;
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
    
    uint32_t currpagenum = embot::hw::flash::address2page(address);
    uint32_t curroffset = embot::hw::flash::address2offset(address); 
    
    bool dataallinside1page = (embot::hw::flash::address2page(address+size-1) == currpagenum);
    bool dataisinanewpage = (currpagenum != pImpl->buffer.page);
    
    
    if(dataisinanewpage)
    {   // must process the old page   
        if(Impl::noPAGE == pImpl->buffer.page)
        {   // first (valid) data ever. we just erase
            pImpl->eraseall();
        }
        else
        {   // all other times means that in buffer there is data.
            pImpl->writebuffer();
        }
        
        // erase the buffer.data, assign the new page to the buffer
        std::memset(pImpl->buffer.data, 0xff, pImpl->buffer.size);
        pImpl->buffer.page = currpagenum;    
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
        pImpl->buffer.page = currpagenum+1; 
        std::memmove(&pImpl->buffer.datamirror08[0], data2, size2);         
    }    
        
    return true;

}

bool embot::hw::FlashBurner::flush()
{
    if(Impl::noPAGE == pImpl->buffer.page)
    {   // never had data inside. cannot write
        return true;
    }
    else
    {   // all other times means that in buffer there is data.
        pImpl->writebuffer();
    }  

    return true;    
}


#if 0
bool embot::hw::FlashBurner::erase()
{ 
    return pImpl->eraseall();
}
#endif




 
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

