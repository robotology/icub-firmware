/*
 * Copyright (C) 2019 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EMBOT_APP_THESTORAGE_H_
#define _EMBOT_APP_THESTORAGE_H_

#include "embot_core.h"
#include "embot_sys.h"
#include <memory>


namespace embot { namespace app {
    
       
    class theStorage
    {
    public:
        static theStorage& getInstance();
                
    public:
    
        static const std::uint32_t capacity = 1024;
        
        // it must be at a page address
        bool init(const std::uint32_t address, const std::uint32_t size);
              
        bool erase();
                        
        // but we can read / write particular areas of max size ::capacity. we can use it for storing matrices or ... whatever we want, as long as we dont do mess
        bool write(const std::uint32_t atoffset, const std::uint32_t size, const void *data);
        bool read(const std::uint32_t atoffset, const std::uint32_t size, void *data);

        
    private:
        theStorage(); 
        ~theStorage(); 
  
    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;      
    };       


}} // namespace embot { namespace app {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
