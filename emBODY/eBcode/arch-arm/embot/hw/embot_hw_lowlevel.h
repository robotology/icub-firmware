
/*
 * Copyright (C) 2018 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EMBOT_HW_LOWLEVEL_H_
#define _EMBOT_HW_LOWLEVEL_H_

#include "embot_core.h"
#include "embot_hw.h"



namespace embot { namespace hw { namespace lowlevel {
    
    void asmEXECcycles(std::uint32_t numcycles);
    
}}} // namespace embot { namespace hw { namespace lowlevel {




#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


