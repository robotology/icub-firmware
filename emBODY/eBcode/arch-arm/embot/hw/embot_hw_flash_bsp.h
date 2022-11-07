
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_FLASH_BSP_H_
#define _EMBOT_HW_FLASH_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_flash.h"


namespace embot { namespace hw { namespace flash {
        
    enum class Version : uint8_t { one = 0, two = 1, three = 2 };
        
    struct Pages
    {   // the type can be different. the case allequalsize is the only one so far. but in case a partition holds pages of different sizes as the stm32f407 ...  
        enum class Type : uint8_t { allequalsize = 0 };
        Type type {Type::allequalsize};
        uint32_t size {0};
        uint32_t number {0};
        constexpr Pages() = default; 
        constexpr Pages(const uint32_t s, const uint32_t n)
            : type(Type::allequalsize), size(s), number(n) {}            
    };       
    
    struct BankDescriptor : public BankProperties
    {
        Pages pages {};
        constexpr BankDescriptor() = default; 
        constexpr BankDescriptor(const Bank b, const uint32_t a, const uint32_t s, const Pages& p)
            : BankProperties(b, a, s), pages(p) {} 
        constexpr bool isinside(const uint32_t adr) const { return (adr >= address) && (adr < (address+size)); }            
        constexpr uint32_t topage(const uint32_t adr) const
        {
            if(true == isinside(adr))
            {
                return (adr - address) / pages.size;
            }
            return InvalidPAGE;
        }
        constexpr uint32_t topagesize(const uint32_t adr) const
        {
            if(true == isinside(adr))
            {
                return pages.size;
            }
            return 0;
        }        
        constexpr uint32_t toaddress(const uint32_t page) const
        {
            if((InvalidPAGE != page) && (page < pages.number))
            {
                return page * pages.size;
            }
            return InvalidADDR;
        } 
        constexpr uint32_t tooffset(const uint32_t adr) const
        {
            if(true == isinside(adr))
            {
                return adr % pages.size;
            }
            return InvalidADDR;
        }
        
    };    

    struct theBanks
    {
        static constexpr auto maxnumber {embot::core::tointegral(Bank::maxnumberof)};
        uint8_t number {1}; 
        std::array<const BankDescriptor*, maxnumber> descr {};        
        constexpr theBanks() = default;
        constexpr theBanks(const uint8_t n, const std::array<const BankDescriptor*, maxnumber> &d) 
            : number(n), descr(d) {} 
        constexpr const BankDescriptor* get(const Bank b) const 
        { 
            uint8_t n = embot::core::tointegral(b); return (n<maxnumber) ? descr[n] : nullptr; 
        }
        constexpr const BankDescriptor* find(const uint32_t adr) const
        {
            for(const auto &i : descr) 
            { 
                if((nullptr != i) && (i->isinside(adr))) return i; 
            }
            return nullptr;
        }
        constexpr bool isinside(const uint32_t adr) const 
        { 
            for(const auto &i : descr) { if((nullptr != i) && (i->isinside(adr))) return true; }
            return false;
        }            

    };   


    struct thePartitions
    {
        static constexpr auto maxnumber {embot::core::tointegral(embot::hw::flash::ID::maxnumberof)};
        std::array<const embot::hw::flash::Partition*, maxnumber> part {};        
        constexpr thePartitions() = default;
        constexpr thePartitions(const std::array<const embot::hw::flash::Partition*, maxnumber> &p) 
            : part(p) {}
        constexpr const embot::hw::flash::Partition* get(const embot::hw::flash::ID id) const
        {
            uint8_t n = embot::core::tointegral(id); return (n<maxnumber) ? part[n] : nullptr;
        }  
        constexpr const embot::hw::flash::Partition* find(const uint32_t adr) const
        {
            for(const auto &i : part) { if((nullptr != i) && (i->isinside(adr))) return i; }
            return nullptr;
        }        
    };      
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr BSP() 
            : SUPP(0), version(Version::three) {}
        
        constexpr BSP(const theBanks &b, const thePartitions &p) 
            : SUPP(0xffffffff), version(Version::three), thebanks(b), thepartitions(p) {}     

                
        Version version {Version::one};
                
        theBanks thebanks {};
        thePartitions thepartitions {};
        
        constexpr const embot::hw::flash::Partition * get(const embot::hw::flash::ID id) const
        {
            return thepartitions.get(id);
        }  
        
        constexpr const BankDescriptor * get(const embot::hw::flash::Bank b) const
        {
            return thebanks.get(b);
        } 
        
        constexpr const embot::hw::flash::Partition * partition(const uint32_t adr) const
        {
            return thepartitions.find(adr);
        } 
        
        constexpr const BankDescriptor * find(const uint32_t adr) const
        {
            return thebanks.find(adr);
        } 
 
        void init() const;
    };
    
    const BSP& getBSP();                                     
        
}}} // namespace embot { namespace hw { namespace flash {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


