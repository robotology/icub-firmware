
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

#ifndef _EMBOT_HW_FLASH_STORAGE_H_
#define _EMBOT_HW_FLASH_STORAGE_H_

#include "embot_common.h"
#include "embot_i2h.h"
#include "embot_hw.h"

namespace embot { namespace hw {
    
    class FlashStorage : public embot::i2h::Storage
    {
    public:
        // for ease of life, we keep the address fixed. however, we could use it as a parameter. 
        //static const std::uint32_t PageStart = 0x0801F800; // @126k, page #63
        //static const std::uint32_t PageSize = 1024;
        
        FlashStorage(std::uint32_t pagestart = embot::hw::sys::addressOfStorage, std::uint32_t pagesize = 1024, std::uint64_t *buffer = nullptr);
        ~FlashStorage();
        
        virtual bool isInitted();
        virtual bool isAddressValid(std::uint32_t address);
        virtual std::uint32_t getBaseAddress();
        virtual std::uint32_t getSize();   
        virtual bool fullerase();  
        virtual bool erase(std::uint32_t address, std::uint32_t size);        
        virtual bool read(std::uint32_t address, std::uint32_t size, void *data);
        virtual bool write(std::uint32_t address, std::uint32_t size, const void *data);       

    private:        
        struct Impl;
        Impl *pImpl;         
    };

}} // namespace embot { namespace hw {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




