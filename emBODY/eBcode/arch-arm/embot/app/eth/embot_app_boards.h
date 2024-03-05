
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#ifndef __EMBOT_APP_BOARDS_H_
#define __EMBOT_APP_BOARDS_H_


#include "embot_core.h"


namespace embot::app::boards {
  
    enum class Board : uint8_t { 
    
        mtb = 5, strain = 6, mais = 7, mtb4 = 11, strain2 = 12, rfe = 13, sg3 = 14, psc = 15, mtb4w = 16, 
        pmc = 17, amcbldc = 18, bms = 19, mtb4c = 20, amc2c = 21, strain2c = 22, bat = 23,

        ems4 = 32, mc4plus = 33, mc2plus = 34, amc = 35,
        
        none = 254,
        unknown = 255
    };
    
}

 
#endif  // include-guard


