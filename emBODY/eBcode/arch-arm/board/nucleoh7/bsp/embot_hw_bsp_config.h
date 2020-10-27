
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_CONFIG_H_
#define _EMBOT_HW_BSP_CONFIG_H_

  
#if   defined(STM32HAL_BOARD_NUCLEOH7)
    #include "embot_hw_bsp_nucleoh7_config.h"        
#else
    #error wrong board
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


