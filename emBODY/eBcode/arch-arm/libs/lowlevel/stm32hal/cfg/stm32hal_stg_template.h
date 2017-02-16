


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef __STM32HAL_STG_H_
#define __STM32HAL_STG_H_

#ifdef __cplusplus
extern "C" {
#endif
    

    
// --------------------------------------------------------------------------------------------------------------------
//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------


// <h> STACK & HEAP  
// <i> It contains stack and heap size and some externally functions

//   <o> stack size         <0x0-0xFFFFFFFF:8>
//   <i> define how much stack you want.
#ifndef STM32HAL_STG_STACKSIZE
    #define STM32HAL_STG_STACKSIZE      0x00002000
#endif

//   <o> heap size         <0x0-0xFFFFFFFF:8>
//   <i> define how much heap you want.
#ifndef STM32HAL_STG_HEAPSIZE
    #define STM32HAL_STG_HEAPSIZE      0x00018000
#endif

// </h>SYS module




// --------------------------------------------------------------------------------------------------------------------
//------------- <<< end of configuration section >>> ------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------------



#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


