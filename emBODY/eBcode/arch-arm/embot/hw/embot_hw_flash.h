
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

#include "embot_common.h"
#include "embot_hw.h"

namespace embot { namespace hw { namespace flash {
    
    struct Partition
    {
        std::uint32_t   address;
        std::uint32_t   maxsize; 
        std::uint32_t   pagesize; 
        bool isvalid() const { if((0 == address) || (0 == maxsize) || (0 == pagesize)) { return false; } else { return true; } }
    }; 
    
   
    const Partition& getpartition(embot::hw::FLASH fl);
    
    // the following functions use embot::hw::FLASH::whole
    
    bool isaddressvalid(std::uint32_t address);    
    std::uint32_t address2page(std::uint32_t address); // returns [0, maxNumOfPAGEs). if address not valid, it returns maxNumOfPAGEs.
    
    bool erase(std::uint32_t page);
    bool erase(std::uint32_t address, std::uint32_t size);
    bool read(std::uint32_t address, std::uint32_t size, void *data);
    bool write(std::uint32_t address, std::uint32_t size, const void *data);
    
}}} // namespace embot { namespace hw { namespace flash {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


