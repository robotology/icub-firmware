
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_BSP_CONFIG_H_
#define __EMBOT_HW_BSP_CONFIG_H_

  
#if defined(STM32HAL_BOARD_AMCFOC_1CM7)

    #include "embot_hw_bsp_amcfoc_1cm7_config.h"    
    
#else
    #error wrong board
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


