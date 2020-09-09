/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _STM32HAL_H_
#define _STM32HAL_H_



#ifdef __cplusplus
extern "C" {
#endif


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"   

// - public #define  --------------------------------------------------------------------------------------------------

// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef struct
{
    void (*tick1ms_init)(void);     // called by stm32 internals whenever it changes the clock
    uint32_t (*tick1ms_get)(void);  // required by some stm32 functions to generate a proper 1 ms delay. 
} stm32hal_config_t;


typedef enum 
{ 
    stm32hal_res_OK     = 0, 
    stm32hal_res_NOK    = -1 
} stm32hal_res_t;


// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern stm32hal_res_t stm32hal_init(const stm32hal_config_t *cfg);



// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section
 
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


