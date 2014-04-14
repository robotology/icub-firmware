// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOEMSCONTROLLER_CFG_H_
#define _EOEMSCONTROLLER_CFG_H_

#ifdef __cplusplus
extern "C" {
#endif

/*  @file       EOemsControllerCfg.h
    @brief      This header file implements hidden interface to motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
 **/
 
 // - external dependencies --------------------------------------------------------------------------------------------
 
 #include "EOMtheEMSapplCfg_cfg.h"
 
 // - public #define  --------------------------------------------------------------------------------------------------
 
 #if   (7==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD) || (9==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
    #define ANKLE_BOARD
    #define NAXLES 2
#elif (5==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
    #define WAIST_BOARD
    #define NAXLES 3
#elif (1==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD) || (3==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
    #define SHOULDER_BOARD
    #define NAXLES 4
#elif (6==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD) || (8==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
    #define UPPERLEG_BOARD
    #define NAXLES 4
#elif (2==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD) || (4==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD) || (10==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD) || (11==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
    #define NO_2FOC_BOARD
    #define NAXLES 1
#else
    #error invalid board
#endif
 
 
 #ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
