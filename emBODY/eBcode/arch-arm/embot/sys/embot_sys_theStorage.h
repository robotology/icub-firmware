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

#ifndef _EMBOT_SYS_THESTORAGE_H_
#define _EMBOT_SYS_THESTORAGE_H_

#include "embot_common.h"
#include "embot_sys.h"

namespace embot { namespace sys {
    
    
    
    class theStorage
    {
    public:
        static theStorage& getInstance()
        {
            static theStorage* p = new theStorage();
            return *p;
        }
        
    public:
      
        bool init(embot::common::Storage *storage);
        
        bool isInitted();
        bool isAddressValid(std::uint32_t address);
        std::uint32_t getBaseAddress();
        std::uint32_t getSize();
        bool fullerase();
        bool erase(std::uint32_t address, std::uint32_t size);
        bool read(std::uint32_t address, std::uint32_t size, void *data);
        bool write(std::uint32_t address, std::uint32_t size, const void *data);   

    private:
        theStorage();  

    public:
        // remove copy constructors and copy assignment operators
        theStorage(const theStorage&) = delete;
        theStorage(theStorage&) = delete;
        void operator=(const theStorage&) = delete;
        void operator=(theStorage&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };       


}} // namespace embot { namespace sys {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
