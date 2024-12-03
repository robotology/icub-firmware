
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_APP_BLDC_MBD_INTERFACE_MOCK_H_
#define __EMBOT_APP_BLDC_MBD_INTERFACE_MOCK_H_

#include "embot_core.h"
#include "embot_app_bldc_MBD_interface.h"

namespace embot::app::bldc::mbd::interface::mock {
    
    void init(IO2 *io2);
    void process_01_input();
    void process_02_activity();
    void process_03_output();   
}



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
