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


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

#include "embot_core.h"
#include "embot_hw_bsp.h"



using namespace std;


namespace embot { namespace hw {
    
    static bool initted = false; 
    
    bool initialised()
    {
        return initted;
    }   
        

    bool init(const Config &config)
    {
        if(true == embot::hw::initialised())
        {
            return true;
        }
        
        embot::core::init({{nullptr, config.get1microtime}, {embot::hw::bsp::print}});        
        embot::hw::bsp::init(config);
                      
        initted = true;
        return true;
    }
       
}} // namespace embot { namespace hw {




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

