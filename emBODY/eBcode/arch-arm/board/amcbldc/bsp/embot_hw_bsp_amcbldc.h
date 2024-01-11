
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_AMCBLDC_H_
#define _EMBOT_HW_BSP_AMCBLDC_H_

#include "embot_core.h"
#include "embot_hw_types.h"

namespace embot { namespace hw { namespace bsp { namespace amcbldc {
    
    Revision revision();
    
    embot::hw::BTN EXTFAULTbutton();
    
    float getVIN();
    float getCIN();
	  float getTEMP();
    
}}}}

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



