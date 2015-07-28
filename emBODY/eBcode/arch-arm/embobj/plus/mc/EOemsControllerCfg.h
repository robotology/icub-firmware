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

//#include "EoCommon.h"
//#include "EOMtheEMSapplCfg_cfg.h"
    
//#include "EOappEncodersReader.h"
 
 // - public #define  --------------------------------------------------------------------------------------------------
 
//#define FORCE_ZERO_PWM_OUT
//#define EXPERIMENTAL_MOTOR_TORQUE
//#define EXPERIMENTAL_SPEED_CONTROL

//#define USE_ONLY_QE
//#define USE_4BIT_INC_ENC_PRECISION
    
//#define SHOULDER_3rd_JOINT_COUPLING
//#define USE_JACOBIAN

#if     defined(ICUB_MEC_V1) | defined(ICUB_GENOVA04)
    #define V1_MECHANICS
    #define USE_2FOC_FAST_ENCODER
#elif   defined(ICUB_MEC_V2) | defined(ICUB_DARMSTADT01)
    #define V2_MECHANICS
    #define USE_2FOC_FAST_ENCODER
#else
    #error -> specify either ICUB_MEC_V1 or ICUB_MEC_V2
#endif

#if defined(HEAD_V3)
    #undef USE_2FOC_FAST_ENCODER
#endif
//#warning in v3 fare in modo che NON venga definito USE_2FOC_FAST_ENCODER 

#ifdef USE_2FOC_FAST_ENCODER
//#define USE_4BIT_INC_ENC_PRECISION
#endif

#define EMS_PERIOD           0.001f
#define EMS_FREQUENCY_INT32  1000
#define EMS_FREQUENCY_FLOAT  1000.0f

#define TICKS_PER_REVOLUTION      65536
#define TICKS_PER_HALF_REVOLUTION 32768
#define ENCODER_QUANTIZATION      16
    
#define VELOCITY_CMD_TIMEOUT      100 // cycles
#define TORQUE_CMD_TIMEOUT        100 // cycles
#define TORQUE_SENSOR_TIMEOUT     100 // cycles
#define ENCODER_TIMEOUT            50 // cycles

#define NOMINAL_CURRENT     30000

#define MAX_NAXLES 4

//#if   (7==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD) 
//    #define ANKLE_BOARD // LEFT
//    #define NAXLES 2
//#elif (9==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
//    #define ANKLE_BOARD // RIGHT
//    #define NAXLES 2
//#elif (5==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
//    #define WAIST_BOARD
//    #define NAXLES 3
//#elif (1==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
//    #define SHOULDER_BOARD // LEFT
//    #define NAXLES 4
//#elif (3==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
//    #define SHOULDER_BOARD // RIGHT
//    #define NAXLES 4
//#elif (6==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
//    #define UPPERLEG_BOARD // LEFT
//    #define NAXLES 4
//#elif (8==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
//    #define UPPERLEG_BOARD // RIGHT
//    #define NAXLES 4
//#elif (2==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD) || (4==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD) || (10==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD) || (11==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
//    #define NO_LOCAL_CONTROL
//    #define NAXLES 1
//#elif (12==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
//    #define NAXLES 2
//    #define EMSCONTROLLER_DONT_USE_2FOC
//#elif (13==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
//    #define NAXLES 2
//    #define EMSCONTROLLER_DONT_USE_2FOC  
//#elif (15==EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
//    #define NAXLES 1
//    #define EMSCONTROLLER_DONT_USE_2FOC  
//#else
//    #error invalid board
//#endif
 
// utilities
    
#define SET_BITS(mask,bits) mask |=  (bits)
#define RST_BITS(mask,bits) mask &= ~(bits)
#define CHK_BITS(mask,bits) (((mask) & (bits)) == (bits))
#define LIMIT(x,L) { if (x>(L)) x=(L); else if (x<-(L)) x=-(L); }
#define LIMIT2(min, x, max) { if (x < (min)) x = (min); else if (x > (max)) x = (max); }

// - declaration of extern public functions ---------------------------------------------------------------------------
/* NOT USED
extern int32_t joint2ticksperrevolution (uint8_t joint_n);

extern eo_appEncReader_enc_type_t joint2encodertype (uint8_t joint_n);
*/
 #ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
