
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

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_FLASH_H_
#define _EMBOT_HW_FLASH_H_

#include "embot_core.h"
#include "embot_hw_types.h"

namespace embot { namespace hw { namespace flash {
    
    enum class Bank : uint8_t { one = 0, two = 1, all = 30, none = 31, maxnumberof = 2 }; 
    
    enum class ID : uint8_t {   bootloader = 0, application = 1, sharedstorage = 2, applicationstorage = 3,     // for use by can boards
                                eloader = 0, eupdater = 1, eapplication00 = 2, eapplication01 = 3, buffer = 4,  // for use by eth boards
                                none = 31, maxnumberof = 5 }; 
    
    struct Partition
    {
        Bank bank {Bank::none};
        uint32_t address {0};
        uint32_t size {0}; 
        constexpr Partition() = default;
        constexpr Partition(const Bank b, const uint32_t a, const uint32_t s) 
            : bank(b), address(a), size(s) {}       
        bool isvalid() const { if((0 == address) || (0 == size) || (Bank::none == bank)) { return false; } else { return true; } }
        constexpr bool isinside(const uint32_t adr) const { return (adr >= address) && (adr < (address+size)); }
    };
    
    constexpr uint32_t InvalidPAGE {0xffffffff};
    constexpr uint32_t InvalidADDR {0xffffffff};

    const embot::hw::flash::Partition& getpartition(embot::hw::flash::ID id);    
        
    bool isaddressvalid(std::uint32_t address);  
    Bank address2bank(std::uint32_t address);  
    std::uint32_t address2page(std::uint32_t address); 
    std::uint32_t page2address(std::uint32_t page, Bank bank = Bank::one);
    std::uint32_t address2offset(std::uint32_t address);
 
    
    bool erase(std::uint32_t page, Bank bank = Bank::one);
    bool erase(std::uint32_t address, std::uint32_t size);
    bool read(std::uint32_t address, std::uint32_t size, void *data);
    bool write(std::uint32_t address, std::uint32_t size, const void *data);
    
}}} // namespace embot { namespace hw { namespace flash {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


