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
    
#include "EOtheMemoryPool.h"
 
 // - public #define  --------------------------------------------------------------------------------------------------
 
//#define FORCE_ZERO_PWM_OUT
//#define EXPERIMENTAL_MOTOR_TORQUE
//#define EXPERIMENTAL_SPEED_CONTROL
//#define USE_ONLY_QE
    
//#define SHOULDER_3rd_JOINT_COUPLING
//#define USE_JACOBIAN

#if     defined(ICUB_MEC_V1) | defined(ICUB_GENOVA04)
    #define V1_MECHANICS
    #define USE_2FOC_FAST_ENCODER
#elif   defined(ICUB_MEC_V2) | defined(ICUB_DARMSTADT01)
    #define V2_MECHANICS
    #define USE_2FOC_FAST_ENCODER
#elif defined(ICUB_MEC_V3)
    #define V3_MECHANICS
    #undef  USE_2FOC_FAST_ENCODER
#elif defined CER
    #define CER_MECHANICS
    #define CER_TICKS_CONTROL
#else 
    #error -> specify either ICUB_MEC_V1 or ICUB_MEC_V2 or ICUB_MEC_V3 or CER
#endif

#define UNKNOWN              0

#define HARDWARE_2FOC        1
#define HARDWARE_MC4p        2

#define PWM_CONTROLLED_MOTOR 1
#define VEL_CONTROLLED_MOTOR 2
#define IQQ_CONTROLLED_MOTOR 3
#define POS_CONTROLLED_MOTOR 4

///////#warning in v3 fare in modo che NON venga definito USE_2FOC_FAST_ENCODER 

#define AEA_DEFAULT_SPIKE_MAG_LIMIT   112 // 7*16 = 7*65536/resolution 
#define AEA_DEFAULT_SPIKE_CNT_LIMIT 32767 // no hardware error on spikes
    
#define CTRL_LOOP_FREQUENCY_INT 1000  
#define CTRL_LOOP_FREQUENCY  1000.0f
#define CTRL_LOOP_PERIOD     0.001f
    
#define EMS_PERIOD CTRL_LOOP_PERIOD
#define EMS_FREQUENCY_INT32 1000
#define EMS_FREQUENCY_FLOAT CTRL_LOOP_FREQUENCY
    
#define MAX_PER_BOARD 4
    
#define TICKS_PER_REVOLUTION      65536
#define TICKS_PER_HALF_REVOLUTION 32768
#define ENCODER_QUANTIZATION      16
    
#define VELOCITY_CMD_TIMEOUT      100 // cycles
#define TORQUE_CMD_TIMEOUT        100 // cycles
#define TORQUE_SENSOR_TIMEOUT     100 // cycles
#define ENCODER_TIMEOUT            50 // cycles

#define CAN_ALIVE_TIMEOUT  50
#define CTRL_REQ_TIMEOUT   50

#define ENCODER_TIMEOUT_LIMIT 50
#define ENCODER_INVALID_LIMIT 50

#define DEFAULT_WATCHDOG_TIME_MSEC 50

#define PWM_OUTPUT_LIMIT_2FOC 10000
#define PWM_OUTPUT_LIMIT      0x7FFF //32767 

#define MAX_NAXLES 4

#define USE_FLOAT_CTRL_UNITS
#define USE_SPEED_FBK_FROM_MOTORS

#ifdef USE_FLOAT_CTRL_UNITS
    typedef float   CTRL_UNITS;
    #define ZERO 0.0f
#else
    typedef int32_t CTRL_UNITS;
    #define ZERO 0
#endif


 
// utilities
    
#define DEFAULT_MOTOR_RUN_MODE icubCanProto_controlmode_openloop
    
#define CUT(x,max) ((x)<(-max))?(-max):(((x)>(max))?(max):(x))

#define SET_BITS(mask,bits) mask |=  (bits)
#define RST_BITS(mask,bits) mask &= ~(bits)
#define CHK_BITS(mask,bits) (((mask) & (bits)) == (bits))
#define LIMIT(x,L) { if (x>(L)) x=(L); else if (x<-(L)) x=-(L); }
#define LIMIT2(min, x, max) { if (x < (min)) x = (min); else if (x > (max)) x = (max); }
#define RESCALE2PWMLIMITS(x, hw_limit) (int16_t)((int32_t)(PWM_OUTPUT_LIMIT * (x + hw_limit))/hw_limit - PWM_OUTPUT_LIMIT) //linear rescaling

#define NEW(T,n) (T*)eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(T), n)
#define DELETE(p) { if (p!=NULL) eo_mempool_Delete(eo_mempool_GetHandle(), (void*)p); p=NULL; }

#define BOOL  eObool_t
#define FALSE eobool_false
#define TRUE  eobool_true

// - declaration of extern public functions ---------------------------------------------------------------------------

 #ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
