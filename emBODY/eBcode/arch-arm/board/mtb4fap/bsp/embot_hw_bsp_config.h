
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_HW_BSP_CONFIG_H_
#define _EMBOT_HW_BSP_CONFIG_H_

  
#if   defined(STM32HAL_BOARD_MTB4FAP)
    #include "embot_hw_bsp_mtb4fap_config.h"        
#else
    #error wrong board
#endif


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


