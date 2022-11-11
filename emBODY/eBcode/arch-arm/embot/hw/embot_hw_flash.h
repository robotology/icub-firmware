
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_FLASH_H_
#define __EMBOT_HW_FLASH_H_

#include "embot_core.h"
#include "embot_hw_types.h"
#include <vector>

#if 0

---

# The `embot::hw::flash` driver

The driver can be used straight away, as long as its `embot::hw::flash::bsp` is configurated properly.



## Raw use

You can directly access the FLASH *as long as you know* the addresses and size of the banks and of the pages inside.

```c++
#include "embot_hw_flash.h"

using namespace embot::hw;

namespace test {
    
constexpr size_t datasize {32};
constexpr uint32_t datawrite[datasize/sizeof(uint32_t)] = {0, 1, 2, 3, 4, 5, 6, 7};
uint32_t dataread[datasize/sizeof(uint32_t)] = {0};

void APIflashBasic()
{
    flash::erase(0x08100000, 128*1024);             // must erase entire page
    flash::write(0x08100000, datasize, datawrite);  // write 32 bytes
    flash::read(0x08100000, datasize, dataread);    // read them back    
}

} // namespace test {

```
**Code Listing**. Basic usage of the `embot::hw::flash` driver. You must know where FLASH is and how it is formed.



## Practical use

But you can also access the FLASH using data structures such as `Bank`, `Page`, `Partition` which can be retrieved from the BSP by ID or by address.

- `Page`: it holds information (not the actual data) about the smallest erasable part of the FLASH. On the used MPUs it is called page or sector.
- `Bank`: it holds information (not the actual data) about the physical container of a number of consecutive pages. 
- `Partition`: it holds information (not the actual data) about a group of consecutive pages all inside a single bank. The pages inside a given `Partition`  contain code and/or data related to a single logical entity, such as the `bootloader`, the `shared storage`, the `application`, the `application storage`, etc.

In here is an example of use of the above data structures.


```c++
#include "embot_hw_flash.h"

using namespace embot::hw;

namespace test {
    
constexpr size_t datasize {32};
constexpr uint32_t datawrite[datasize/sizeof(uint32_t)] = {0, 1, 2, 3, 4, 5, 6, 7};
uint32_t dataread[datasize/sizeof(uint32_t)] = {0};

void APIflashBSP()
{
    // get the handler of the first Bank and operate on it by pages
    std::vector<embot::hw::flash::Page> thepages {};
    const embot::hw::flash::Bank &bank1 = flash::bsp::bank(flash::Bank::ID::one);
    if(!bank1.isvalid())
    {
        embot::core::print(
            "FLASH (bank): Bank::one is not configured in the BSP");
    }
    else    
    {
        // retrieves info of all the pages inside
        bank1.pages(thepages);
        embot::core::print(
            std::string("FLASH (pages): on Bank::one their number is = ") + 
            std::to_string(thepages.size())); 
           
        // retrieve info of the last page in the bank          
        flash::ADDR endofbank01 {bank1.address+bank1.size-1};
        flash::Page page = bank1.page(endofbank01);
        
        // and operate on it: erase, write, read back
        if(flash::erase(page.address, page.size))
        {
            size_t s = std::min(sizeof(datawrite), page.size);
            if(flash::write(page.address, s, datawrite))
            {
                flash::read(page.address, s, dataread);    
            } 
            const char *rr = (0 == std::memcmp(dataread, datawrite, s)) ? "OK" : "KO";
            embot::core::print(
                std::string("FLASH (erase, write, read): on last page of Bank::one = ") +
                rr);
        }
    }
    
    
    // get the handler of the partion containing the bootloader (CAN based boards)
    const flash::Partition& btl { flash::bsp::partition(flash::Partition::ID::bootloader) };  
    if(!btl.isvalid())
    {
        embot::core::print(
            "FLASH (partition): Partition::ID::bootloader is not configured in the BSP");
    }
    else
    {
        btl.pages(thepages);
        embot::core::print(
        "FLASH (partition): Partition::ID::bootloader has size = " + std::to_string(btl.size) +
            " and " + std::to_string(thepages.size()) + " pages");
        
    }
    
    // get the handler of the partion containing the eLoader (ETH based boards)
    const flash::Partition& ldr { flash::bsp::partition(flash::Partition::ID::eloader) };  
    if(!ldr.isvalid())
    {
        embot::core::print(
            "FLASH (partition): Partition::ID::eloader is not configured in the BSP");
    }
    else
    {
        ldr.pages(thepages);
        embot::core::print(
        "FLASH (partition): Partition::ID::eloader has size = " + std::to_string(ldr.size) +
            " and " + std::to_string(thepages.size()) + " pages");
    }  
}

} // namespace test {

```
**Code Listing**. Usage of the `embot::hw::flash` driver with `embot::hw::flash::bsp`.  You access FLASH through the following data structures: `Page`, `Bank`, `Partition`.



## Configuration of the `embot::hw::flash::bsp`

See inside file `embot_hw_flash_bsp.h`.


---

#endif


namespace embot { namespace hw { namespace flash {
    
    using ADDR = uint32_t;
    constexpr ADDR InvalidADDR {0xffffffff};
    constexpr size_t InvalidSize {0xffffffff};  
    
}}} // namespace embot { namespace hw { namespace flash {    

// BASIC USAGE: functions which operate on FLASH by address. 
namespace embot { namespace hw { namespace flash {
           
    bool isvalid(const ADDR address);    
    bool erase(const ADDR address, const size_t size);
    bool read(const ADDR address, const size_t size, void *data);
    bool write(const ADDR address, const size_t size, const void *data);

}}} // namespace embot { namespace hw { namespace flash {


// ADVANCED USAGE: the following data structures allows to hold information about the structure of the FLASH
// so that we can get addresses and sizes
namespace embot { namespace hw { namespace flash {
    
    // the Page it is the minimum erasable element. in HW it can be called page (in stm32l/g) or sector (in stm32h)    
    struct Page;   
    // the Bank is formed by consecutive Pages, on stm32l/g/h all of the same size, but they may be also of different sizes.     
    struct Bank;  
    // the Partition is a section of a Bank formed by an integral number of consecutive Pages.   
    struct Partition;   
        
    struct Page
    {   
        using Index = uint32_t;     // keep the index of the page inside the Bank  w/ range [0, ...
        static constexpr Index InvalidIndex {0xffffffff};      
        
        const Bank *owner {nullptr};
        ADDR address {InvalidADDR};
        size_t size {InvalidSize};
        Page::Index index {InvalidIndex}; 
        
        constexpr Page() = default;
        constexpr Page(const Bank *o, const ADDR a, const size_t s, const Index i)
            : address(a), size(s), index(i), owner(o) {} 
                
        constexpr bool isvalid() const 
        {   
            bool ko { (nullptr == owner) || (InvalidSize == size) || (InvalidADDR == address) || (InvalidIndex == index) };
            return !ko;
        }
        
        constexpr const Bank* bank() { return owner; }
        
        void load(const Bank *o, const ADDR a, const size_t s, const Index i) 
        { 
            owner = o; address = a; size = s; index = i; 
        }                
    }; 
    
         
    struct Bank
    {   // this struct implements an abstract interface to the Bank
        enum class ID : uint8_t { one = 0, two = 1, none = 31, maxnumberof = 2 }; 
        
        Bank::ID id {Bank::ID::none};
        ADDR address {InvalidADDR};        
        size_t size {InvalidSize};

        constexpr Bank() = default;
        constexpr Bank(const Bank::ID i, const ADDR a, const size_t s)
            : id(i), address(a), size(s) {} 
                       
        constexpr bool isinside(const ADDR adr) const 
        { 
            return (adr >= address) && (adr < (address+size)); 
        } 
        
        constexpr bool baseisok() const 
        {
            bool ko {   (Bank::ID::none == id) || (Bank::ID::maxnumberof == id) || 
                        (InvalidADDR == address) || (InvalidSize == size) || (0 == size) };
            return !ko;      
        }
        
        // must implement them
        virtual bool isvalid() const = 0;           
        virtual Page page(const ADDR adr) const = 0; 
        virtual void pages(std::vector<Page> &pp) const = 0;
        virtual Page index2page(const Page::Index i) const = 0; 
    };

    
    struct RegularBank : public Bank
    {   // this struct is an implementation of Bank w/ pages of the same size
        
        size_t pagesize {InvalidSize}; 
        size_t numpages = {InvalidSize};
        
        constexpr RegularBank() = default;
        constexpr RegularBank(const Bank::ID i, const ADDR a, const size_t s, const size_t p)
            : Bank(i, a, s), pagesize(p), numpages((0==p) ? InvalidSize : (s/p)) {}     
        
        bool isvalid() const override
        {
            bool ko = { !baseisok() || (InvalidSize == pagesize) || (0 == pagesize) };
            return !ko;
        }  
        
        Page page(const ADDR adr) const override
        {        
            if(isvalid() && isinside(adr))
            {
                Page::Index i = (adr - address) / pagesize;
                return {this, address+i*pagesize, pagesize, i};  
            }
            return {};
        }; 
        
        void pages(std::vector<Page> &pp) const override
        {
            pp.clear();
            if(isvalid())
            {
                for(Page::Index i = 0; i<numpages; i++) { pp.push_back({this, address+i*pagesize, pagesize, i}); }
            }
        } 
        
        Page index2page(const Page::Index i) const override
        {                       
            if(isvalid() && (i < (size / pagesize)))
            {
                return {this, address+i*pagesize, pagesize, i};  
            }
            return {};            
        }        
    };
    
    
    struct Partition
    {   // it can have an ID so that we can easily retrieve its handle from the BSP. we can easily add IDs w/ any value
        
        enum class ID : uint8_t {
            generic = 0,                                                                // generic use. it may be inside BSP or not.
            bootloader = 1, application = 2, sharedstorage = 3, applicationstorage = 4, // used for BSP of CAN-based boards
            eloader = 5, eupdater = 6, eapplication00 = 7, eapplication01 = 8,          // used for BSP of ETH-based boards
            buffer00 = 9, buffer01 = 10, buffer02 = 11, buffer03 = 12, buffer04 = 13,   // others
            none = 31, maxnumberof = 14 };   
        
        Partition::ID id {Partition::ID::none};
        const Bank *owner {nullptr};
        ADDR address {InvalidADDR};
        size_t size {0}; 
                
        constexpr Partition() = default;
        constexpr Partition(const Bank *o, const ADDR a, const size_t s) 
            : id(ID::generic), owner(o), address(a), size(s) {} 
        constexpr Partition(const ID i, const Bank *o, const ADDR a, const size_t s) 
            : id(i), owner(o), address(a), size(s) {}  
                
        constexpr bool isvalid() const 
        {   // it must have a size and belong to a bank 
            if((0 == size) || (nullptr == owner)) { return false; } else { return true; } 
        }
        
        constexpr bool isinside(const ADDR adr) const 
        { 
            return (adr >= address) && (adr < (address+size)); 
        }
        
        constexpr const Bank* bank() { return owner; }
        
        Page page(const ADDR adr) const 
        {           
            if(isvalid() && isinside(adr)) { return owner->page(adr); }
            return {};            
        } 
        
        void pages(std::vector<Page> &pp) const
        {
            pp.clear();
            if(isvalid())
            {   
                for(ADDR a=address; a<(address+size); )
                {
                    Page p = owner->page(a);
                    pp.push_back(p);
                    a += p.size; 
                }
            }             
        }
    }; 
            

}}} // namespace embot { namespace hw { namespace flash {


// in here are functions used to retrieve info from the BSP
namespace embot { namespace hw { namespace flash { namespace bsp {
    
    // it gets the address of a given bank + pageindex. 
    // returns InvalidADDR if {id, index} are invalid, else a valid address
    ADDR address(const Bank::ID id, const Page::Index index);    
    
    // it gets a Page which contains a given address. if address is invalid, page.isvalid() is false
    Page page(const ADDR address);    
    
    // they retrieve a Partition specified inside the BSP by its ID or by ADDR
    // if not found inside the BSP they return the handle to a Partition where .isvalid() is false
    const embot::hw::flash::Partition& partition(const embot::hw::flash::Partition::ID id);    
    const embot::hw::flash::Partition& partition(const ADDR address);
    
    // they retrieve a Bank specified inside the BSP by its ID or by ADDR
    // if not found inside the BSP they return the handle to a Bank here .isvalid() is false
    const embot::hw::flash::Bank & bank(const ADDR address);
    const embot::hw::flash::Bank & bank(const Bank::ID id);
        
           
}}}} // namespace embot { namespace hw { namespace flash { namespace bsp {{


// functions which operate on FLASH by Bank::ID and Page::Index. 

namespace embot { namespace hw { namespace flash {
    
    bool isvalid(const Bank::ID id, const Page::Index index);
    bool erase(const Bank::ID id, const Page::Index index);

}}} // namespace embot { namespace hw { namespace flash {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


