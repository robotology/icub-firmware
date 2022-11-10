
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_FLASH_BSP_H_
#define __EMBOT_HW_FLASH_BSP_H_


#include "embot_core.h"
#include "embot_hw_types.h"
#include "embot_hw_bsp.h"
#include "embot_hw_flash.h"

#if 0

## Configuration of the BSP

See inside file `embot_hw_flash_bsp.h`.


In here is a simple configuration to be placed somewhere, possibly inside `embot_hw_bsp_boardname.cpp`

```c++
namespace embot { namespace hw { namespace flash { namespace bsp {
         
    constexpr uint8_t numbanks {1};
    constexpr size_t banksize {512*1024};
    constexpr size_t pagesize {4*1024};
    constexpr BankDescriptor bank01 { Bank::ID::one, 0x08000000, banksize, pagesize };
    constexpr theBanks thebanks 
    {
        numbanks, 
        { &bank01, nullptr }
    }; 
    
    // on top of each other, with sizes:
    constexpr std::array<uint32_t, 4> ss = {124*1024, 4*1024, 380*1024, 4*1024};
    constexpr Partition btl {Partition::ID::bootloader,         &bank01,    bank01.address,         ss[0]}; 
    constexpr Partition sha {Partition::ID::sharedstorage,      &bank01,    btl.address+btl.size,   ss[1]};
    constexpr Partition app {Partition::ID::application,        &bank01,    sha.address+sha.size,   ss[2]}; 
    constexpr Partition stg {Partition::ID::applicationstorage, &bank01,    app.address+app.size,   ss[3]}; 
    
    constexpr thePartitions thepartitions
    {
        { &btl, &sha, &app, &stg }
    };
        
    constexpr BSP thebsp {        
        thebanks,
        thepartitions
    };        
    
    void BSP::init() const {}
    
    const BSP& getBSP() 
    {
        return thebsp;
    }       

              
}}}} // namespace embot { namespace hw { namespace flash { namespace bsp {
```

#endif


namespace embot { namespace hw { namespace flash { namespace bsp {

    
    struct BankDescriptor : public RegularBank
    {   // a RegularBank w/ some more methods
        size_t numberofpages {0};
        
        constexpr BankDescriptor() = default; 
        constexpr BankDescriptor(const Bank::ID i, const ADDR a, const size_t s, const size_t p)
            : RegularBank(i, a, s, p), numberofpages((0 != p) ? (s/p) : 0) {}              
       
    };
    
    
    struct theBanks
    {
        static constexpr auto maxnumber {embot::core::tointegral(Bank::ID::maxnumberof)};
        
        uint8_t number {1}; 
        std::array<const BankDescriptor*, maxnumber> descr {};   
            
        constexpr theBanks() = default;
        constexpr theBanks(const uint8_t n, const std::array<const BankDescriptor*, maxnumber> &d) 
            : number(n), descr(d) {} 
                
        constexpr bool isinside(const ADDR adr) const 
        { 
            for(const auto &i : descr) { if((nullptr != i) && (i->isinside(adr))) return true; }
            return false;
        }   
                
        constexpr const BankDescriptor* get(const Bank::ID id) const 
        { 
            for(const auto &i : descr) { if((nullptr != i) && (i->id == id)) return i; }
            return nullptr;
        }
        
        constexpr const BankDescriptor* get(const ADDR adr) const
        {
            for(const auto &i : descr) { if((nullptr != i) && (i->isinside(adr))) return i; }
            return nullptr;
        }                 

    };   


    struct thePartitions
    {
        static constexpr auto maxnumber {embot::core::tointegral(embot::hw::flash::Partition::ID::maxnumberof)};
        
        std::array<const embot::hw::flash::Partition*, maxnumber> part {};    
            
        constexpr thePartitions() = default;
        constexpr thePartitions(const std::array<const embot::hw::flash::Partition*, maxnumber> &p) 
            : part(p) {}
                
        constexpr const embot::hw::flash::Partition* get(const embot::hw::flash::Partition::ID id) const
        {
            for(const auto &i : part) { if((nullptr != i) && (id == i->id)) return i; }
            return nullptr;
        }  
        
        constexpr const embot::hw::flash::Partition* get(const ADDR adr) const
        {
            for(const auto &i : part) { if((nullptr != i) && (i->isinside(adr))) return i; }
            return nullptr;
        }        
    };      
    
    struct BSP
    {        
        theBanks thebanks {};
        thePartitions thepartitions {};
            
        constexpr BSP() {}            
        constexpr BSP(const theBanks &b, const thePartitions &p) 
            : thebanks(b), thepartitions(p) {}     
                        
        constexpr const embot::hw::flash::Partition * partition(const embot::hw::flash::Partition::ID id) const
        {
            return thepartitions.get(id);
        }  
        
        constexpr const embot::hw::flash::Partition * partition(const ADDR adr) const
        {
            return thepartitions.get(adr);
        } 
        
        constexpr const BankDescriptor * bank(const embot::hw::flash::Bank::ID id) const
        {
            return thebanks.get(id);
        } 
              
        constexpr const BankDescriptor * bank(const ADDR adr) const
        {
            return thebanks.get(adr);
        } 
 
        void init() const;
    };
    
    const BSP& getBSP();                                     
        
}}}} // namespace embot { namespace hw { namespace flash { namespace bsp {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


