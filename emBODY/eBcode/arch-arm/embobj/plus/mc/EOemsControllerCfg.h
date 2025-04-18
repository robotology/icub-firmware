/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or  FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License foFITNESSr more details
*/

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOEMSCONTROLLER_CFG_H_
#define _EOEMSCONTROLLER_CFG_H_

        
// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtheMemoryPool.h"

// - public #define  --------------------------------------------------------------------------------------------------


// used by  MController::actuation_type  
typedef enum {
    ACT_TYPE_UNKNOWN    = 0,
    ACT_TYPE_2FOC       = 1,
    ACT_TYPE_MC4p       = 2 
#if 0
    // marco.accame: i keep it as an example of a mix actuation done by two boards: an mc4plus and a pmc   
   ,ACT_TYPE_MC4pPMC    = 3 
#endif    
} MC_ACTUATION_t;
    
// used by Motor::HARDWARE_TYPE
// typically, given a MController::actuation_type all the motors share the same Motor::HARDWARE_TYPE
// however, we may have mixed motor types. see note (xxx)
typedef enum {
    HARDWARE_UNKNOWN    = 0,
    HARDWARE_2FOC       = 1,
    HARDWARE_MC4p       = 2
#if 0
    // marco.accame: 
    // note (xxx) i keep it as an example. the ACT_TYPE_MC4pPMC uses motor hardware of mixed type: HARDWARE_MC4p but also of HARDWARE_PMC
   ,HARDWARE_PMC        = 3
#endif    
} MC_MOTOR_HARDWARE_t;


//#define NOT_AVAIL_MOTOR 0
//#define PWM_INPUT_MOTOR 1
//#define VEL_INPUT_MOTOR 2
//#define TRQ_INPUT_MOTOR 3
//#define POS_INPUT_MOTOR 4

///////#warning in v3 fare in modo che NON venga definito USE_2FOC_FAST_ENCODER 

#define AEA_DEFAULT_SPIKE_MAG_LIMIT   112 // 7*16 = 7*65536/resolution 
#define AEA_DEFAULT_SPIKE_CNT_LIMIT 32767 // no hardware error on spikes

// #define AEA3_DEFAULT_SPIKE_MAG_LIMIT   28 // 16+8+4 (the last three lsb byte could be invalid)
    
#define MAIS_DEFAULT_SPIKE_MAG_LIMIT   112 // 7*16 = 7*65536/resolution 
#define MAIS_DEFAULT_SPIKE_CNT_LIMIT 32767 // no hardware error on spikes
    
#define CTRL_LOOP_FREQUENCY_INT 1000  
#define CTRL_LOOP_FREQUENCY  1000.0f
#define CTRL_LOOP_PERIOD     0.001f
    
//#define EMS_PERIOD CTRL_LOOP_PERIOD
//#define EMS_FREQUENCY_INT32 1000
//#define EMS_FREQUENCY_FLOAT CTRL_LOOP_FREQUENCY


#define MAX_ENCODS_PER_BOARD 8
#define MAX_JOINTS_PER_BOARD 4
#define MAX_MOTORS_PER_BOARD 4


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

#define SPRING_MAX_TENSION    0x1F00

#define MAX_NAXLES 4

#define USE_FLOAT_CTRL_UNITS

//#define NO_CONSTRAINT        0
//#define CER_HAND_CONSTRAINT  2
//#define TRIFID_CONSTRAINT    3

#define WAIST_TRIFID_LIMIT  400000
#define WRIST_TRIFID_LIMIT  500000 //1130000

//#define HAND_CABLE_BACK_LIMIT 4500

#define POS_LIMIT_MARGIN 1820//364

#define CALIBRATION_TIMEOUT (30*CTRL_LOOP_FREQUENCY_INT)

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

static const CTRL_UNITS DEG2ICUB = 65536.0f/360.0f;
static const CTRL_UNITS ICUB2DEG = 360.0f/65536.0f;

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
