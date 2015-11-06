
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EODECOUPLER_HID_H_
#define _EODECOUPLER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/*  @file       EOdecoupler_hid.h
    @brief      This header file implements hidden interface to motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------

//#include "EOmotorControl_hid.h"
    
// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOdecoupler.h"
#include "EOemsControllerCfg.h"

// - #define used with hidden struct ----------------------------------------------------------------------------------

// - definition of the hidden struct implementing the object ----------------------------------------------------------

/*  @struct     EOdecoupler_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  
 
//#define MAX_MOTORS 4
 
struct EOmotors_hid 
{
    eOemscontroller_board_t board;
    
    uint8_t nMotors;
    
    uint16_t motor_watchdog[MAX_NAXLES];
    
    eObool_t Jok;
    uint8_t motor_run_state[MAX_NAXLES];
    //uint8_t motor_run_state_last[MAX_NAXLES];
    uint8_t motor_run_state_req[MAX_NAXLES];
    uint16_t motor_run_state_req_wdog[MAX_NAXLES];
    
    uint8_t motor_qe_error[MAX_NAXLES];
    uint8_t motor_enc_not_calibrated[MAX_NAXLES];
    uint32_t motor_fault_mask[MAX_NAXLES];
    int32_t J[MAX_NAXLES][MAX_NAXLES];
    int32_t Ji[MAX_NAXLES][MAX_NAXLES];
}; 

// - declaration of extern hidden functions ---------------------------------------------------------------------------

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




