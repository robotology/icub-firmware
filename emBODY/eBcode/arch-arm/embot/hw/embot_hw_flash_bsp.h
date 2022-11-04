
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
    
//    struct PROP
//    { 
//        embot::hw::Partition partition {0, 0, 0};   
//    };
    
//    struct Banks
//    {
//        uint8_t number {1}; 
//        uint32_t size {0};
//        uint32_t pagesize {0}; 
//        uint32_t pagesinonebank {0};        
//        constexpr Banks() = default;
//        constexpr Banks(uint8_t n, uint32_t s, uint32_t p) 
//            : number(n), size(s), pagesize(p), pagesinonebank((0==p) ? 0 : (s/p)) {} 
//    };
//    
//    struct BankProp
//    {
//        Bank bank {Bank::none};
//        uint32_t address {0};
//        uint32_t size {0};
//        uint32_t pagesize {0};
//        uint32_t pagesnumber {0};
//        constexpr BankProp() = default; 
//        constexpr BankProp(Bank b, uint32_t a, uint32_t s, uint32_t ps)
//            : bank(b), address(a), size(s), pagesize(ps), pagesnumber((0==ps) ? 0 : (s/ps)) {}            
//    };
//    
//    struct Banks2
//    {
//        uint8_t number {1}; 
//        BankProp prop {};        
//        constexpr Banks2() = default;
//        constexpr Banks2(uint8_t n, const BankProp &p) 
//            : number(n), prop(p) {} 
//    };  

//    struct Banks3
//    {
//        static constexpr auto maxbanks {embot::core::tointegral(Bank::maxnumberof)};
//        uint8_t number {1}; 
//        std::array<const BankProp*, maxbanks> props {};        
//        constexpr Banks3() = default;
//        constexpr Banks3(uint8_t n, const std::array<const BankProp*, maxbanks> &p) 
//            : number(n), props(p) {} 
//    };   


    // the new one
    
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
    
    struct BankDescriptor
    {
        Bank bank {Bank::none};
        uint32_t address {0};
        uint32_t size {0};
        Pages pages {};
        constexpr BankDescriptor() = default; 
        constexpr BankDescriptor(const Bank b, const uint32_t a, const uint32_t s, const Pages& p)
            : bank(b), address(a), size(s), pages(p) {} 
        constexpr bool isinside(const uint32_t adr) const { return (adr >= address) && (adr < (address+size)); }            
        constexpr uint32_t topage(const uint32_t adr) const
        {
            if(true == isinside(adr))
            {
                return (adr - address) / pages.size;
            }
            return InvalidPAGE;
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
//        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::FLASH::maxnumberof);
//        
//        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro) 
//            : SUPP(msk), properties(pro), banks({}), version(Version::one) {}
//        constexpr BSP(std::uint32_t msk, std::array<const PROP*, maxnumberof> pro, const Banks &b) 
//            : SUPP(msk), properties(pro), banks(b), version(Version::two) {}        
//        constexpr BSP() 
//            : SUPP(0), properties({0}), banks({}), version(Version::one) {}   

        constexpr BSP(const theBanks &b, const thePartitions &p) 
            : SUPP(0xffffffff), version(Version::three), thebanks(b), thepartitions(p) {}     
                
          
        Version version {Version::one};
                
//        std::array<const PROP*, maxnumberof> properties {};             
//        Banks banks {};  

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
        
        constexpr const BankDescriptor * find(const uint32_t adr) const
        {
            return thebanks.find(adr);
        } 
        
//        constexpr const PROP * getPROP(embot::hw::FLASH h) const { return supported(h) ? properties[embot::core::tointegral(h)] : nullptr; }
        void init() const;
    };
    
    const BSP& getBSP();                                     
        
}}} // namespace embot { namespace hw { namespace flash {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


