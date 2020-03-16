
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

#ifndef _EMBOT_HW_FLASH_BURNER_H_
#define _EMBOT_HW_FLASH_BURNER_H_

#include "embot_core.h"
//#include "embot_i2h.h"
#include "embot_hw.h"
#include "embot_hw_sys.h"

namespace embot { namespace hw {
    
    class FlashBurner
    {
    public:
        
        FlashBurner(std::uint32_t start, std::uint32_t size, std::uint32_t buffersize = 2048, std::uint64_t *buffer = nullptr);
        ~FlashBurner();
     
        // it adds inside a buffer the specified data. the object will write according to internal rules.
        bool add(std::uint32_t address, std::uint32_t size, const void *data);
        // it forces a write of the content of the buffer.
        bool flush();
        
        //bool erase();
        
        bool isAddressValid(std::uint32_t address);
      

    private:        
        struct Impl;
        Impl *pImpl;         
    };

}} // namespace embot { namespace hw {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




