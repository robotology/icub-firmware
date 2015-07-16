
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOEMSCONTROLLER_HID_H_
#define _EOEMSCONTROLLER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*  @file       EOemsController_hid.h
    @brief      This header file implements hidden interface to motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "EOspeedmeter.h"
#include "EOaxisController.h"
#include "EOdecoupler.h"
#include "EOemsControllerCfg.h"
    
// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOemsController.h"

// - #define used with hidden struct ----------------------------------------------------------------------------------

//#define EMS_OK          0x0000
//#define EMS_FAULT       0x8000

/*
#define EMS_SPI_TIMEOUT 0x4000

#define EMS_AEA_FAULT_0 0x0001
#define EMS_AEA_FAULT_1 0x0002
#define EMS_AEA_FAULT_2 0x0004
#define EMS_AEA_FAULT_3 0x0008

#define EMS_TRQ_FAULT_0 0x0010
#define EMS_TRQ_FAULT_1 0x0020
#define EMS_TRQ_FAULT_2 0x0040
#define EMS_TRQ_FAULT_3 0x0080
*/

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/*  @struct     EOemsController_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  

struct EOemsController_hid 
{
    eOemscontroller_board_t board;
    eOemscontroller_actuation_t act;
    
    uint8_t n_calibrated;
    
    //uint16_t state_mask;
    
    //uint16_t motor_state_mask[4];
    
    uint8_t           naxles;
    EOmotors         *motors;
    EOaxisController *axis_controller[MAX_NAXLES];
    
    EOabsCalibratedEncoder *abs_calib_encoder[MAX_NAXLES];
    EOaxleVirtualEncoder   *axle_virt_encoder[MAX_NAXLES];
    
    int16_t motor_current[MAX_NAXLES];
    int32_t motor_acceleration[MAX_NAXLES];
    int32_t motor_velocity[MAX_NAXLES];
    int32_t motor_velocity_gbx[MAX_NAXLES];
    int32_t motor_position[MAX_NAXLES];
  
    int32_t           motor_config_gearbox_ratio[MAX_NAXLES];
    int32_t           motor_config_rotorencoder[MAX_NAXLES];
    eOmeas_velocity_t motor_config_maxvelocityofmotor[MAX_NAXLES];
    eOmeas_current_t  motor_config_maxcurrentofmotor[MAX_NAXLES];
    uint16_t          motor_config_rotorIndexOffset[MAX_NAXLES];
    uint8_t           motor_config_motorPoles[MAX_NAXLES];
    eObool_t          motor_config_hasHallSensor[MAX_NAXLES];
    eObool_t          motor_config_hasTempSensor[MAX_NAXLES];
    eObool_t          motor_config_hasRotorEncoder[MAX_NAXLES];
    
    //float torque_meas[NJOINTS];
}; 

// - declaration of extern hidden functions ---------------------------------------------------------------------------


#ifdef __MC_BACKDOOR__
extern void eo_emsController_hid_DEBUG_reset(void);

extern void eo_emsController_hid_DEBUG_evaltransmission(void);
#endif

extern void eo_emsController_hid_userdef_config_motor(EOemsController* ctrl,uint8_t motor);

extern void eo_emsController_hid_userdef_set_motor_idle(EOemsController* ctrl,uint8_t motor);
    
extern void eo_emsController_hid_userdef_force_motor_idle(EOemsController* ctrl,uint8_t motor);

extern void eo_emsController_hid_userdef_set_motor_running(EOemsController* ctrl,uint8_t motor);

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




