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

#include "embot_hw_flash.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_flash_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif

using namespace std;

#if defined(STM32HAL_STM32H7)
extern "C" {
void iitext_FLASH_ClearFlag(uint32_t flag);
// to be defined in stm32h7xx_hal_flash.c because ... we had to undef FLASH macro and we cannot call __HAL etc
//{ // FLASH_FLAG_PGSERR
//    __HAL_FLASH_CLEAR_FLAG(flag);
//}
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_flash)


namespace embot { namespace hw { namespace flash {

    const embot::hw::Partition& getpartition(embot::hw::FLASH fl) { static constexpr embot::hw::Partition p {}; return p; }
    
    // the following functions use embot::hw::FLASH::whole
    
    bool isaddressvalid(std::uint32_t address) { return false; }    
    Bank address2bank(std::uint32_t address) { return Bank::one; }
    std::uint32_t address2page(std::uint32_t address) { return 0; }
    std::uint32_t page2address(std::uint32_t page, Bank bank) { return 0; }
    std::uint32_t address2offset(std::uint32_t address){ return 0; }
    
    bool erase(std::uint32_t page, Bank bank) { return false; }
    bool erase(std::uint32_t address, std::uint32_t size) { return false; }
    bool read(std::uint32_t address, std::uint32_t size, void *data) { return false; }
    bool write(std::uint32_t address, std::uint32_t size, const void *data) { return false; }
    
}}} // namespace embot { namespace hw { namespace flash {

#else

namespace embot { namespace hw { namespace flash {
    
//    const embot::hw::Partition& getpartition(embot::hw::FLASH fl)
//    {
//        return embot::hw::flash::getBSP().getPROP(fl)->partition;         
//    }
    
    const embot::hw::flash::Partition& getpartition(embot::hw::flash::ID id)
    {
        return *embot::hw::flash::getBSP().get(id);
    }
           
    bool isaddressvalid(std::uint32_t address)
    {
        return nullptr != embot::hw::flash::getBSP().find(address);
    }
    
    Bank address2bank(std::uint32_t address)
    {        
        const BankDescriptor * bd = embot::hw::flash::getBSP().find(address);       
        return (nullptr == bd) ? Bank::none : bd->bank;     
    }
    
    std::uint32_t address2page(std::uint32_t address)
    {
        const BankDescriptor * bd = embot::hw::flash::getBSP().find(address);               
        return (nullptr == bd) ? InvalidPAGE : bd->topage(address); 
    }
    
    std::uint32_t page2address(std::uint32_t page, Bank bank)
    {
        const BankDescriptor * bd = embot::hw::flash::getBSP().get(bank);             
        return (nullptr == bd) ? InvalidADDR : bd->toaddress(page); 
    }
    
    std::uint32_t address2offset(std::uint32_t address)
    {
        const BankDescriptor * bd = embot::hw::flash::getBSP().find(address);             
        return (nullptr == bd) ? InvalidADDR : bd->tooffset(address);         
    }
    
//    Bank page2bank(std::uint32_t page)
//    {        
//        if(Version::one == embot::hw::flash::getBSP().version)
//        {
//            return Bank::one;
//        }
//        
//        if(Version::two == embot::hw::flash::getBSP().version)
//        {
//            if(0 == embot::hw::flash::getBSP().banks.pagesinonebank)
//            {
//                return Bank::one;
//            }    
//            return static_cast<Bank>(page/embot::hw::flash::getBSP().banks.pagesinonebank);
//        }
//        
//        return Bank::one;
//    }    
    
    bool erase(std::uint32_t page, Bank bank)
    {
        const BankDescriptor * bd = embot::hw::flash::getBSP().get(bank);
        if((nullptr == bd) || (page >= bd->pages.number))
        {
            return false;
        }
               
        HAL_StatusTypeDef r {HAL_OK};

#if defined(STM32HAL_STM32H7)   
        uint32_t error_code {0};       
        FLASH_EraseInitTypeDef er = {0};
        er.TypeErase = FLASH_TYPEERASE_SECTORS;
        er.Banks = (Bank::one == bank) ? FLASH_BANK_1 : FLASH_BANK_2;
        er.Sector = page;
        er.NbSectors = 1;
        er.VoltageRange = FLASH_VOLTAGE_RANGE_3;
        uint32_t eraseErrorIndex = 0;
        
        HAL_FLASH_Unlock();
        // __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_PGSERR); but i cannot use it, so ... i must add it into a function somewhere
//        iitext_FLASH_ClearFlag(FLASH_FLAG_PGSERR);
        r = HAL_FLASHEx_Erase(&er, &eraseErrorIndex);
        HAL_FLASH_Lock();
        
        if(HAL_OK != r)
        {
            error_code = HAL_FLASH_GetError();
            iitext_FLASH_ClearFlag(error_code);
        }
#else        
        FLASH_EraseInitTypeDef er = {0};
        er.TypeErase = FLASH_TYPEERASE_PAGES;
        er.Banks = FLASH_BANK_1;
        er.Page = page;
        er.NbPages = 1;
        uint32_t pagenum = 0;
        HAL_FLASH_Unlock();
        r = HAL_FLASHEx_Erase(&er, &pagenum);
        HAL_FLASH_Lock();
        
        if(HAL_OK != r)
        {
            error_code = HAL_FLASH_GetError();
            //iitext_FLASH_ClearFlag(error_code);
        }
        
#endif 

        
        return (HAL_OK == r) ? true : false;                
    }
    
    bool erase(std::uint32_t address, std::uint32_t size)
    {
        if(0 == size)
        {
            return false;
        }
        if(false == isaddressvalid(address))
        {
            return false;
        }
        if(false == isaddressvalid(address+size-1))
        {
            return false;
        }
        
        // if the limits of [address, address+size) are both inside the same bank, we erase in one shot.
        // otherwise we erase by page ...

        Bank ba = address2bank(address);
        Bank b1 = address2bank(address+size-1);
        if((Bank::none == ba) || (Bank::none == b1))
        {
            return false;
        }
        
        static volatile uint32_t nerrors {0};
        
        if(ba == b1)
        {
            HAL_StatusTypeDef r {HAL_ERROR};
            
            uint32_t firstpage = address2page(address);
            uint32_t lastpage = address2page(address+size-1);
            uint32_t npages = lastpage - firstpage + 1;   

#if defined(STM32HAL_STM32H7)   
            uint32_t error_code {0};       
            FLASH_EraseInitTypeDef er = {0};
            er.TypeErase = FLASH_TYPEERASE_SECTORS;
            er.Banks = (Bank::one == ba) ? FLASH_BANK_1 : FLASH_BANK_2;
            er.Sector = firstpage;
            er.NbSectors = npages;
            er.VoltageRange = FLASH_VOLTAGE_RANGE_3;
            uint32_t eraseErrorIndex = 0;
            
            HAL_FLASH_Unlock();
            iitext_FLASH_ClearFlag(FLASH_FLAG_PGSERR);
            r = HAL_FLASHEx_Erase(&er, &eraseErrorIndex);
            HAL_FLASH_Lock();
            
            
//            error_code = HAL_FLASH_GetError();
//            if(0 != error_code)
//            {
//                nerrors++;                
//            }
            
            if(HAL_OK != r)
            {
                nerrors++; 
                error_code = HAL_FLASH_GetError();
                iitext_FLASH_ClearFlag(error_code);
            }
#else        
            FLASH_EraseInitTypeDef er = {0};
            er.TypeErase = FLASH_TYPEERASE_PAGES;
            er.Banks = FLASH_BANK_1;
            er.Page = firstpage;
            er.NbPages = npages;
            uint32_t pagenum = 0;
            HAL_FLASH_Unlock();
            r = HAL_FLASHEx_Erase(&er, &pagenum);
            HAL_FLASH_Lock();
            
            if(HAL_OK != r)
            {
                nerrors++; 
                error_code = HAL_FLASH_GetError();
                iitext_FLASH_ClearFlag(error_code);
            }            
#endif   

            return (HAL_OK == r) ? true : false; 
            
        }
        else
        {
            
            // in here we assume that we have consecutive pages and increasing banks
            // so we iterate by address, until we reach
            uint32_t addressaligned2page = page2address(address2page(address), ba);
            uint32_t lastaddressaligned2page = page2address(address2page(address+size-1), b1);
            Bank b = ba;
            const BankDescriptor * bd = embot::hw::flash::getBSP().get(b);
            const uint32_t pagesize = bd->pages.size;
            bool rr {false};
            for(;;)
            {
                rr = erase(addressaligned2page, b);
                if(false == rr)
                {
                    break;
                }
                addressaligned2page += pagesize;
                b = address2bank(addressaligned2page);
                if(addressaligned2page > lastaddressaligned2page)
                {
                    break;
                }
            }
            
            return rr;            
        }
        
        return false;
    }
    
//    bool erase2(std::uint32_t address, std::uint32_t size)
//    {
//        // we still erase by page. we need first page and number of them
//        if(false == isaddressvalid(address))
//        {
//            return false;
//        }
//        if(false == isaddressvalid(address+size))
//        {
//            return false;
//        }
//        
//        uint32_t firstpage = address2page(address);
//        uint32_t lastpage = address2page(address+size-1);
//        uint32_t npages = lastpage - firstpage + 1;
//        
//        // if evry page is on the same bank we erase all of them w/ single operation.
//        // otherwise we erase each of them individually
//        
//        
//        
//        for(uint32_t page=firstpage; page<=lastpage; page++)
//        {
//            if(false == erase(page))
//            {
//                return false;
//            }
//        }
//        
//        return true;
//    }
    
    bool read(std::uint32_t address, std::uint32_t size, void *data)
    {        
        if(false == isaddressvalid(address))
        {
            return false;
        }
        
        if(false == isaddressvalid(address+size-1))
        {
            return false;
        }
        
        if((0 == size) || (nullptr == data))
        {
            return false;
        }
        
        #warning see note.
        // note: you could protect the std::memmove() w/ HAL_FLASH_Unlock(); HAL_FLASH_Lock();
        // can read directly from flash
        HAL_FLASH_Unlock();
        void *st = (void*) address;
        std::memmove(data, st, size); 
        HAL_FLASH_Lock();
        
        return true;
    }
    
    bool write(std::uint32_t address, std::uint32_t size, const void *data)
    {
        if(false == isaddressvalid(address))
        {
            return false;
        }
        if(false == isaddressvalid(address+size-1))
        {
            return false;
        }
        
        if(NULL == data)
        {
            return false;
        }
        
        if(0 != (address & 0x00000007))
        {   // address must be 8-aligned. size must be multiple of 8.
            return false;
        }

        //const embot::hw::Partition part = embot::hw::flash::getpartition(embot::hw::FLASH::whole);
        
        HAL_FLASH_Unlock();
        
        HAL_StatusTypeDef r {HAL_ERROR};
        
#if defined(STM32HAL_STM32H7)  
        uint32_t flashaddress = address;
        uint32_t n256bitwords = size / 32; //part.maxsize / 32;     // TODO: check this operation
        const uint32_t *buffer = reinterpret_cast<const std::uint32_t*>(data);        
        for(uint32_t i=0; i<n256bitwords; i++)
        {
            uint32_t dataaddress = (uint32_t)&buffer[i];
            r = HAL_FLASH_Program(FLASH_TYPEPROGRAM_FLASHWORD, flashaddress, dataaddress);
            flashaddress += 32;
            if(HAL_OK != r)
            {
                break;
            }
        }
        
        if(HAL_OK != r)
        {
            uint32_t error_code = HAL_FLASH_GetError();
            iitext_FLASH_ClearFlag(error_code);
        }        
#else        
        uint32_t tmpadr = address;
        uint32_t n64bitwords = size / 8; // part.maxsize / 8;
        const uint64_t *buffer = reinterpret_cast<const std::uint64_t*>(data);
        for(uint32_t i=0; i<n64bitwords; i++)
        {
            r = HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, tmpadr, buffer[i]);
            tmpadr += 8;
            if(HAL_OK != r)
            {
                break;
            }
        }
        
        if(HAL_OK != r)
        {
            uint32_t error_code = HAL_FLASH_GetError();
            //iitext_FLASH_ClearFlag(error_code);
        }        
#endif

        HAL_FLASH_Lock();
        
        return (HAL_OK == r) ? true : false;
    }        
    
     
}}} // namespace embot { namespace hw { namespace flash {
   

#endif //defined(EMBOT_ENABLE_hw_flash)

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

