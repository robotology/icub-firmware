
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


namespace embot { namespace hw { namespace flash {
      
    extern const std::uint32_t startOfFLASH;
    extern const std::uint32_t sizeOfFLASH;
    extern const std::uint32_t sizeOfPAGE;
    extern const std::uint32_t maxNumOfPAGEs;
    
    bool isaddressvalid(std::uint32_t address);    
    std::uint32_t address2page(std::uint32_t address); // returns [0, maxNumOfPAGEs). if address not valid, it returns maxNumOfPAGEs.
    
    bool erase(std::uint32_t page);
    bool erase(std::uint32_t address, std::uint32_t size);
    bool read(std::uint32_t address, std::uint32_t size, void *data);
    bool write(std::uint32_t address, std::uint32_t size, const void *data);
    
}}} // namespace embot { namespace hw { namespace flash {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


