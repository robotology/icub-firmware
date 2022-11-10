
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace hw { namespace flash {  
    
    constexpr Page InvalidPage { nullptr, InvalidADDR, InvalidSize, Page::InvalidIndex };    
    
    constexpr Partition InvalidPartition { Partition::ID::none, nullptr, InvalidADDR, 0 };
    
namespace bsp {
    constexpr BankDescriptor InvalidBank { Bank::ID::none, InvalidADDR, InvalidSize, {} };
}
    
}}} // namespace embot { namespace hw { namespace flash {    

#if !defined(EMBOT_ENABLE_hw_flash)


namespace embot { namespace hw { namespace flash {
   
namespace bsp {      
    
    const embot::hw::flash::Partition& bsp::partition(const embot::hw::flash::Partition::ID id) { return InvalidPartition; }    
    const embot::hw::flash::Partition& bsp::partition(const ADDR address) { return InvalidPartition; }
    
    const embot::hw::flash::Bank & bank(const ADDR address) { return InvalidBank;}
    const embot::hw::flash::Bank & bank(const Bank::ID id) { return InvalidBank;}

    Page page(const ADDR address) { return {}; }
    ADDR address(const Bank::ID bank, const Page::Index pageindex) { return InvalidADDR; } 
    
} // namespace bsp {

    bool isvalid(const ADDR address) { return false; } 
    bool erase(const ADDR address, const size_t size) { return false; }
    bool read(const ADDR address, const size_t size, void *data) { return false; }
    bool write(const ADDR address, const size_t size, const void *data) { return false; }
    
    bool isvalid(const Bank::ID id, const Page::Index pageindex) { return false; }        
    bool erase(const Bank::ID id, const Page::Index pageindex) { return false; }    
    
}}} // namespace embot { namespace hw { namespace flash {

#else



namespace embot { namespace hw { namespace flash {
    
    
    // declation of static functions

    static uint32_t tostm32bank(embot::hw::flash::Bank::ID id);    
    static void clearflag(uint32_t flag);

    // operations by ADDR
    
    bool isvalid(const ADDR address)
    {
        const bsp::BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(address);
        return nullptr != bd;
    }
            
    bool erase(const ADDR address, const size_t size)
    {
        if(0 == size)
        {
            return false;
        }
        
        if(false == isvalid(address))
        {
            return false;
        }
        
        if(false == isvalid(address+size-1))
        {
            return false;
        }
        
        // if the limits of [address, address+size) are both inside the same bank, we erase in one shot.
        // otherwise we erase by page ...

        Bank::ID ba = bsp::bank(address).id;
        Bank::ID b1 = bsp::bank(address+size-1).id;
        if((Bank::ID::none == ba) || (Bank::ID::none == b1))
        {
            return false;
        }
        
        static volatile uint32_t nerrors {0};
                
        Page firstpage = bsp::page(address);            
        Page lastpage = bsp::page(address+size-1);
          
              
        if(ba == b1)
        {   // we stay on the same bank  
            
            HAL_StatusTypeDef r {HAL_ERROR};
            uint32_t error_code {0};
            
            Page::Index firstpageIndex = firstpage.index;   
            Page::Index lastpageIndex = lastpage.index;                
            uint32_t npages = lastpageIndex - firstpageIndex + 1;   

#if defined(STM32HAL_STM32H7)          
            FLASH_EraseInitTypeDef er = {0};
            er.TypeErase = FLASH_TYPEERASE_SECTORS;
            er.Banks = tostm32bank(ba);
            er.Sector = firstpageIndex;
            er.NbSectors = npages;
            er.VoltageRange = FLASH_VOLTAGE_RANGE_3;
            uint32_t eraseErrorIndex = 0;
            
            HAL_FLASH_Unlock();
//            clearflag(FLASH_FLAG_PGSERR);
            r = HAL_FLASHEx_Erase(&er, &eraseErrorIndex);
            HAL_FLASH_Lock();
                                   
            if(HAL_OK != r)
            {
                nerrors++; 
                error_code = HAL_FLASH_GetError();
                clearflag(error_code);
            }
#else        
            FLASH_EraseInitTypeDef er = {0};
            er.TypeErase = FLASH_TYPEERASE_PAGES;
            er.Banks = tostm32bank(ba);
            er.Page = firstpageIndex;
            er.NbPages = npages;
            uint32_t pagenum = 0;
            HAL_FLASH_Unlock();
            r = HAL_FLASHEx_Erase(&er, &pagenum);
            HAL_FLASH_Lock();
            
            if(HAL_OK != r)
            {
                nerrors++; 
                error_code = HAL_FLASH_GetError();
                clearflag(error_code);
            }            
#endif   

            return (HAL_OK == r) ? true : false; 
            
        }
        else
        {            
            // in here we assume that we have consecutive pages and increasing banks
            // so we iterate by address, starting from addressaligned2page until we reach lastaddressaligned2page
            // warning: marco accame says that this code was NOT TESTED 
  
            ADDR addressaligned2page = firstpage.address;
            ADDR lastaddressaligned2page = lastpage.address; 
                        
            Bank::ID b = ba;
        
            bool rr {false};
            for(;;)
            {
                const bsp::BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(b);
                
                if(nullptr == bd)
                {
                    break;
                }
                
                Page currpage = bsp::page(addressaligned2page);
                
                rr = erase(b, currpage.index);
                if(false == rr)
                {
                    break;
                }
                addressaligned2page += currpage.size;
                b = bsp::bank(addressaligned2page).id;
                if(addressaligned2page > lastaddressaligned2page)
                {
                    break;
                }
            }
            
            return rr;            
        }
        
        return false;
    }
    
    
    bool read(const ADDR address, const size_t size, void *data)
    { 
        if((0 == size) || (nullptr == data))
        {
            return false;
        }
        
        if(false == isvalid(address))
        {
            return false;
        }
        
        if(false == isvalid(address+size-1))
        {
            return false;
        }
        
        // can read directly from flash
        HAL_FLASH_Unlock();
        void *st = (void*) address;
        std::memmove(data, st, size); 
        HAL_FLASH_Lock();
        
        return true;
    }
    
    bool write(const ADDR address, const size_t size, const void *data)
    {
        if((0 == size) || (nullptr == data))
        {
            return false;
        }
        
        if(false == isvalid(address))
        {
            return false;
        }
        
        if(false == isvalid(address+size-1))
        {
            return false;
        }        
        
        if(0 != (address & 0x00000007))
        {   // address must be 8-aligned. size must be multiple of 8.
            return false;
        }
        
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
            clearflag(error_code);
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
            clearflag(error_code);
        }        
#endif

        HAL_FLASH_Lock();
        
        return (HAL_OK == r) ? true : false;
    }  



    // operations by Bank::ID and Page::Index
    
    bool isvalid(const Bank::ID id, const Page::Index index)
    {
        const bsp::BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(id);         
        return (nullptr == bd) ? false : bd->index2page(index).isvalid();        
    }
  
    bool erase(const Bank::ID id, const Page::Index index)
    {
        const bsp::BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(id);
        if((nullptr == bd) || (index >= bd->numberofpages))
        {
            return false;
        }
               
        HAL_StatusTypeDef r {HAL_OK};
        uint32_t error_code {0};

#if defined(STM32HAL_STM32H7)          
        FLASH_EraseInitTypeDef er = {0};
        er.TypeErase = FLASH_TYPEERASE_SECTORS;
        er.Banks = tostm32bank(id);
        er.Sector = index;
        er.NbSectors = 1;
        er.VoltageRange = FLASH_VOLTAGE_RANGE_3;
        uint32_t eraseErrorIndex = 0;
        
        HAL_FLASH_Unlock();
//        clearflag(FLASH_FLAG_PGSERR);
        r = HAL_FLASHEx_Erase(&er, &eraseErrorIndex);
        HAL_FLASH_Lock();
        
        if(HAL_OK != r)
        {
            error_code = HAL_FLASH_GetError();
            clearflag(error_code);
        }
#else        
        FLASH_EraseInitTypeDef er = {0};
        er.TypeErase = FLASH_TYPEERASE_PAGES;
        er.Banks = tostm32bank(id);
        er.Page = index;
        er.NbPages = 1;
        uint32_t pagenum = 0;
        HAL_FLASH_Unlock();
        r = HAL_FLASHEx_Erase(&er, &pagenum);
        HAL_FLASH_Lock();
        
        if(HAL_OK != r)
        {
            error_code = HAL_FLASH_GetError();
            //clearflag(error_code);
        }       
#endif 
        
        return (HAL_OK == r) ? true : false;                
    }
    
    
    // definition of static functions
    
    static uint32_t tostm32bank(embot::hw::flash::Bank::ID id)
    {
        // Bank::ID values srat from 0, macro FLASH_BANK_1 is i, macro FLASH_BANK_2 sometimes is not defined
        // in here we dont check much (Bank::ID::all, etc)
        return embot::core::tointegral(id) + 1;
    }
    
    static void clearflag(uint32_t flag)
    {
        __HAL_FLASH_CLEAR_FLAG(flag);
    }
        
    
}}} // }}}} // namespace embot { namespace hw { namespace flash {


// the BSP part

namespace embot { namespace hw { namespace flash { namespace bsp {
        
    const embot::hw::flash::Partition& partition(const embot::hw::flash::Partition::ID id)
    {
        const Partition *p = embot::hw::flash::bsp::getBSP().partition(id);
        return (nullptr == p) ? InvalidPartition : *p;
    }

    const embot::hw::flash::Partition& partition(const ADDR address)
    {
        const Partition *p = embot::hw::flash::bsp::getBSP().partition(address);
        return (nullptr == p) ? InvalidPartition : *p;
    }
    
    const embot::hw::flash::Bank & bank(const ADDR address)
    {
        const BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(address);             
        return (nullptr == bd) ? InvalidBank : *bd;         
    }
    
    const embot::hw::flash::Bank & bank(const Bank::ID id)
    {
        const BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(id);             
        return (nullptr == bd) ? InvalidBank : *bd; 
    }   
       
    Page page(const ADDR address) 
    {          
        const bsp::BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(address);   
        if(nullptr != bd)
        {
            return bd->page(address);
        }               
        return {}; 
    }
 
    ADDR address(const Bank::ID id, const Page::Index index) 
    {
        const bsp::BankDescriptor * bd = embot::hw::flash::bsp::getBSP().bank(id);
        return (nullptr == bd) ? InvalidADDR : bd->index2page(index).address;             
    }      
    
}}}} // }}}} // namespace embot { namespace hw { namespace flash { namespace bsp {     



#endif //defined(EMBOT_ENABLE_hw_flash)

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

