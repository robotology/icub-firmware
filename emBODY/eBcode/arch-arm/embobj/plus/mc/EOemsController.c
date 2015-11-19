
/*  @file       EOdecoupler.c
    @brief      This file implements internal implementation of motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "EOtheMemoryPool.h"

#include <string.h>

// keep it before any evaluation of macros ...
#include "EOemsControllerCfg.h"

#if defined(USE_CANCOMM_V2)
    #include "EOtheCANservice.h"
#else
    #include "EOicubCanProto_specifications.h"
    #include "EOtheEMSapplBody.h"
#endif

//////////////////////////////////
#include "EOtheBOARDtransceiver.h"
#include "EOrop.h"
#include "EOprotocolMN.h"
#include "EOnv_hid.h"

#include "EoError.h"
#include "EOtheErrorManager.h"


#if !defined(V1_MECHANICS) && !defined(V2_MECHANICS) && !defined(V3_MECHANICS)
#error mechanics is undefined
#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOemsController.h"
#include "EOdecoupler.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOemsController_hid.h" 

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define SCAN(s)     for (uint8_t s=0; s<ems->naxles; ++s)
#define JOINTS(j)   SCAN(j)
#define MOTORS(m)   SCAN(m)
#define ENCODERS(e) SCAN(e)

#define GET_COUPLED(joint,j) JOINTS(j) if (eo_motors_are_coupled(ems->motors, joint, j))
//the MACROS below are now public (EOemsController.h), so that the application can set the error masks from other services (e.g. MC4plus application)
/*
#define MOTOR_HARD_FAULT         0x00000001
#define MOTOR_CAN_NOT_RESPONDING 0x00000080
#define MOTOR_WRONG_STATE        0x00000002

#define AXIS_TORQUE_SENS_FAULT   0x00000100
#define AEA_ABS_ENC_INVALID_DATA 0x00004000
#define AEA_ABS_ENC_TIMEOUT      0x00008000

#define MOTOR_EXTERNAL_FAULT     0x00000004
#define MOTOR_OVERCURRENT_FAULT  0x00000008
#define MOTOR_I2T_LIMIT_FAULT    0x00020000
#define MOTOR_HALLSENSORS_FAULT  0x00000040
#define MOTOR_QENCODER_FAULT     0x00100000
#define MOTOR_CAN_INVALID_PROT   0x00000080
#define MOTOR_CAN_GENERIC_FAULT  0x00003D00
*/
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

void config_2FOC(uint8_t joint);
void set_2FOC_idle(uint8_t joint);
void force_2FOC_idle(uint8_t motor);
void set_2FOC_running(uint8_t joint);
void sendErrorMessage(uint8_t j, uint32_t ems_fault_mask_new_j, uint32_t motor_fault_mask_j);
static void s_eo_emsController_ResetCalibrationCoupledJoints(uint8_t joint);
static void s_eo_emsController_InitTrajectoryVersionVergence(uint8_t j);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOemsController *ems = NULL;

//static const char s_eobj_ownname[] = "EOemsController";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOemsController* eo_emsController_Init(eOemscontroller_board_t board, eOemscontroller_actuation_t act, uint8_t nax) 
{
    if (board == emscontroller_board_NO_CONTROL) return NULL;

    if (nax == 0) return NULL;
    
    if (nax > MAX_NAXLES) nax = MAX_NAXLES;
    
#ifndef NO_LOCAL_CONTROL
    ems = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOemsController), 1);
#endif
    
    if (!ems) return NULL;
    
    ems->board = board;
    ems->act = act;
    ems->actuation_limit = eo_emsController_GetActuationLimit(); 
    ems->naxles = nax;
    ems->n_calibrated = 0;
        
    JOINTS(j)
    {
        ems->axis_controller[j] = eo_axisController_New(j);
        ems->abs_calib_encoder[j] = eo_absCalibratedEncoder_New(j);
        #ifdef USE_2FOC_FAST_ENCODER
        ems->axle_virt_encoder[j] = eo_axleVirtualEncoder_New();
        #endif
        ems->motor_current [j] = 0;
        ems->motor_velocity[j] = 0;
        ems->motor_velocity_gbx[j] = 0;
        ems->motor_acceleration[j] = 0;
        ems->motor_position[j] = 0;
        ems->motor_position_last[j] = 0;
        ems->motor_position_sign[j] = 0;
        ems->motor_config_gearbox_ratio[j]  = 1;
        ems->motor_config_rotorencoder[j]  = 1;
        ems->motor_config_hasHallSensor[j] = eobool_false;
        ems->motor_config_hasRotorEncoder[j] = eobool_false;
        ems->motor_config_hasTempSensor[j] = eobool_false;
        ems->motor_config_hasRotorEncoderIndex[j] = eobool_false;
        ems->motor_config_maxcurrentofmotor[j] = 0;
        ems->motor_config_maxvelocityofmotor[j] = 0;
        ems->motor_config_motorPoles[j] = 0;
        ems->motor_config_rotorIndexOffset[j] = 0;
        ems->new_motor_msg[j] = eobool_false;
    }
        
    ems->motors = eo_motors_New(ems->naxles, ems->board);

    return ems;
}

extern uint8_t eo_emsController_IsVirtualCalibrationInProgress(void)
{   
    if(emscontroller_board_HEAD_neckyaw_eyes != ems->board)
    {
        return 0;
    }
    
    
    if((eomc_controlmode_calib == ems->axis_controller[2]->control_mode) || (eomc_controlmode_calib == ems->axis_controller[3]->control_mode))
    {
        return 1;
    }
    
    return 0;
}

extern void eo_emsController_set_Jacobian(int32_t **Ji32)
{
    eo_motors_set_Jacobian(ems->motors, Ji32);
}

extern void eo_emsController_SetAbsEncoderSign(uint8_t joint, int32_t sign)
{
    eo_absCalibratedEncoder_SetSign(ems->abs_calib_encoder[joint], sign);
}

extern void eo_emsController_SetAxisCalibrationZero(uint8_t joint, int32_t calibration_zero)
{
    eo_axisController_SetAxisCalibrationZero(ems->axis_controller[joint], calibration_zero);
}

extern int32_t eo_emsController_GetAxisCalibrationZero(uint8_t joint)
{
    return eo_axisController_GetAxisCalibrationZero(ems->axis_controller[joint]);
}

//             | 1     0       0   |
// J = dq/dm = | 1   40/65     0   |
//             | 0  -40/65   40/65 | 

//        |    1       0       0   |
// J^-1 = | -65/40   65/40     0   |
//        | -65/40   65/40   65/40 |

//      | 1     1       0   |
// Jt = | 0   40/65  -40/65 |
//      | 0     0     40/65 | 

// speed_motor  = J^-1 * speed_axis
// torque_motor = Jt   * torque_axis

extern void eo_emsController_AcquireMotorEncoder(uint8_t motor, int16_t current, int32_t velocity, int32_t position)
{
    eo_motors_rearm_wdog(ems->motors, motor);
    
    ems->motor_current [motor] = current;//(2*current)/3; // Iq = sqrt(3)/2*Imeas, 32768 = 25000 mA ==> 0.66 scale factor
    ems->motor_velocity[motor] = 1000*velocity;
    ems->motor_velocity_gbx[motor] = ems->motor_velocity[motor]/ems->motor_config_gearbox_ratio[motor]; // motor_config_gearbox_ratio is a signed value
    ems->motor_position[motor] = position;
    
    //change the sign of motor position and motor velocity according to the sign of rotorencoder
    //if (ems->motor_config_rotorencoder[motor]<0)
    //{
    //   ems->motor_velocity[motor]     = - ems->motor_velocity[motor];
    //   ems->motor_velocity_gbx[motor] = - ems->motor_velocity_gbx[motor];
    //   ems->motor_position[motor]     = - ems->motor_position[motor];
    //}
}
extern void eo_emsController_AcquireMotorPosition(uint8_t motor, int32_t position)
{
    //don't need to rearm here cause this function is used for local actuators
    //eo_motors_rearm_wdog(ems->motors, motor);
    
    //valid for init
    if ((ems->motor_position[motor] == 0) && (ems->motor_position_last[motor] == 0))
    {
        ems->motor_position[motor] = position;
        ems->motor_position_last[motor] = position;
        return;
    }
    
    //direction of movement changes depending on the sign
    int32_t delta = position - ems->motor_position_last[motor];
    
    //normalize delta to avoid discontinuities
    while (delta < -TICKS_PER_HALF_REVOLUTION) delta += TICKS_PER_REVOLUTION;
    while (delta >  TICKS_PER_HALF_REVOLUTION) delta -= TICKS_PER_REVOLUTION;
        
    ems->motor_position[motor] += ems->motor_position_sign[motor]*delta;
    
    //update velocity
    ems->motor_velocity[motor] = (ems->motor_position_sign[motor] * delta)* 1000;
    
    //update last position for next iteration
    ems->motor_position_last[motor] = position;
    
    
    //ems->motor_position[motor] = position;
    //change the sign of motor position according to the sign of rotorencoder
    //if (ems->motor_config_rotorencoder[motor]<0)
    //{
    //  ems->motor_position[motor]     = - ems->motor_position[motor];
    //}
}

extern void eo_emsController_AcquireMotorCurrent(uint8_t motor, int16_t current)
{
    //don't need to rearm here cause this function is used for local actuators
    //eo_motors_rearm_wdog(ems->motors, motor);
    
    ems->motor_current [motor] = current; //(2*current)/3; // Iq = sqrt(3)/2*Imeas, 32768 = 25000 mA ==> 0.66 scale factor
}

extern void eo_emsController_AcquireAbsEncoders(int32_t *abs_enc_pos, uint8_t error_mask)
{   
    int32_t axle_abs_pos[MAX_NAXLES];
    #ifndef USE_2FOC_FAST_ENCODER
    int32_t axle_abs_vel[MAX_NAXLES];
    #endif
    
    ENCODERS(e)
    {
        axle_abs_pos[e] = eo_absCalibratedEncoder_Acquire(ems->abs_calib_encoder[e], abs_enc_pos[e], 0x03 & (error_mask>>(e<<1)));
        #ifndef USE_2FOC_FAST_ENCODER
        axle_abs_vel[e] = eo_absCalibratedEncoder_GetVel(ems->abs_calib_encoder[e]);
        #endif
    }
         
    #if defined(V1_MECHANICS)
    if (ems->board == emscontroller_board_SHOULDER)
    {
        // |J0|   |  1     0    0   |   |E0|     
        // |J1| = |  0     1    0   | * |E1|
        // |J2|   |  1    -1  40/65 |   |E2|
        axle_abs_pos[2] = axle_abs_pos[0]-axle_abs_pos[1]+(40*axle_abs_pos[2])/65;
        #ifndef USE_2FOC_FAST_ENCODER
        axle_abs_vel[2] = axle_abs_vel[0]-axle_abs_vel[1]+(40*axle_abs_vel[2])/65;
        #endif
    }
    #endif
    
#ifdef USE_2FOC_FAST_ENCODER
    
    int32_t axle_virt_vel[MAX_NAXLES];
    int32_t axle_virt_pos[MAX_NAXLES];
    
    #ifdef USE_JACOBIAN
    
    JOINTS(j)
    {
        axle_virt_pos[j] = 0;
        axle_virt_vel[j] = 0;
        
        MOTORS(m)
        {
            axle_virt_pos[j] += ems->motors->J[j][m]*ems->motor_position[m];
            axle_virt_vel[j] += ems->motors->J[j][m]*ems->motor_velocity_gbx[m];
        }
            
        axle_virt_pos[j] >>= 14;
        axle_virt_vel[j] >>= 14;
        axle_virt_vel[j] *= FOC_2_EMS_SPEED;
    }
    
    #else // USE_JACOBIAN
    if (ems->board == emscontroller_board_SHOULDER)
    {
        //             | 1     0       0    0 |
        // J = dq/dm = | 1   40/65     0    0 |
        //             | 0  -40/65   40/65  0 |
        //             | 0     0       0    1 |

        axle_virt_vel[0] = ems->motor_velocity_gbx[0];
        axle_virt_vel[1] = (ems->motor_velocity_gbx[0]+(40*ems->motor_velocity_gbx[1])/65);
        axle_virt_vel[2] = ((40*(-ems->motor_velocity_gbx[1]+ems->motor_velocity_gbx[2]))/65);
        axle_virt_vel[3] = ems->motor_velocity_gbx[3];

        axle_virt_pos[0] = ems->motor_position[0];
        axle_virt_pos[1] = ems->motor_position[0]+(40*ems->motor_position[1])/65;
        axle_virt_pos[2] = (40*(-ems->motor_position[1]+ems->motor_position[2]))/65;
        axle_virt_pos[3] = ems->motor_position[3];
    }
    else if (ems->board == emscontroller_board_WAIST)
    {
        //beware: this matrix is valid inside the firmare only
        //             |   0.5   0.5    0   |
        // J = dq/dm = |  -0.5   0.5    0   |
        //             | 22/80 22/80  22/40 |
    
        //at the user level, torso joints are swapped (0 1 2 -> 2 0 1) and the matrix is:
        //             | 22/40 22/80 22/80  |
        // J = dq/dm = |     0   0.5   0.5  |
        //             |     0  -0.5   0.5  |
    
        axle_virt_vel[0] = (    ems->motor_velocity_gbx[0] + ems->motor_velocity_gbx[1]) * 0.5;
        axle_virt_vel[1] = (   -ems->motor_velocity_gbx[0] + ems->motor_velocity_gbx[1]) * 0.5;
        axle_virt_vel[2] =      ems->motor_velocity_gbx[0]*0.022/0.08 + ems->motor_velocity_gbx[1]*0.022/0.08 + ems->motor_velocity_gbx[2]*0.022/0.04;
    
        axle_virt_pos[0] = (    ems->motor_position[0] + ems->motor_position[1]) * 0.5;
        axle_virt_pos[1] = (   -ems->motor_position[0] + ems->motor_position[1]) * 0.5;
        axle_virt_pos[2] =      ems->motor_position[0]*0.022/0.08 + ems->motor_position[1]*0.022/0.08 + ems->motor_position[2]*0.022/0.04;
    }
    else if (ems->board == emscontroller_board_UPPERLEG)
    {
        axle_virt_vel[0] = (50*ems->motor_velocity_gbx[0])/75;
        axle_virt_vel[1] =     ems->motor_velocity_gbx[1]    ;
        axle_virt_vel[2] =     ems->motor_velocity_gbx[2]    ;
        axle_virt_vel[3] =     ems->motor_velocity_gbx[3]    ;
    
        axle_virt_pos[0] = (50*ems->motor_position[0])/75;
        axle_virt_pos[1] =     ems->motor_position[1]    ;
        axle_virt_pos[2] =     ems->motor_position[2]    ;
        axle_virt_pos[3] =     ems->motor_position[3]    ;
    }
    else if (ems->board == emscontroller_board_CER_WAIST)
    {
        axle_virt_vel[0] = ems->motor_velocity_gbx[0];
        axle_virt_vel[1] = ems->motor_velocity_gbx[1];
        axle_virt_vel[2] = ems->motor_velocity_gbx[2];
        axle_virt_vel[3] = ems->motor_velocity_gbx[3];

        axle_virt_pos[0] = ems->motor_position[0];
        axle_virt_pos[1] = ems->motor_position[1];
        axle_virt_pos[2] = ems->motor_position[2];
        axle_virt_pos[3] = ems->motor_position[3];
    }
    else if (ems->board == emscontroller_board_ANKLE)
    {
        axle_virt_vel[0] = ems->motor_velocity_gbx[0];
        axle_virt_vel[1] = ems->motor_velocity_gbx[1];
    
        axle_virt_pos[0] = ems->motor_position[0];
        axle_virt_pos[1] = ems->motor_position[1];
    }
    #endif // USE_JACOBIAN

#endif //USE_2FOC_FAST_ENCODER
    
    JOINTS(j)
    {
        #ifdef USE_2FOC_FAST_ENCODER
            #ifdef EXPERIMENTAL_SPEED_CONTROL
            eo_axleVirtualEncoder_Acquire(ems->motor_config_gearbox_ratio[j], ems->axle_virt_encoder[j], axle_abs_pos[j], axle_virt_pos[0], axle_virt_vel[j]);
            #else
            eo_axleVirtualEncoder_Acquire(ems->motor_config_gearbox_ratio[j], ems->axle_virt_encoder[j], axle_abs_pos[j], axle_virt_pos[j], axle_virt_vel[j]);
            #endif
            eo_axisController_SetEncPos(ems->axis_controller[j], eo_axleVirtualEncoder_GetPos(ems->axle_virt_encoder[j]));
            eo_axisController_SetEncVel(ems->axis_controller[j], eo_axleVirtualEncoder_GetVel(ems->axle_virt_encoder[j]));
        #else
            if (ems->axis_controller[j]->calibration_type == eomc_calibration_type5_hard_stops_mc4plus)  
            { 
                if((emscontroller_board_HEAD_neckyaw_eyes == ems->board) && ((2==j)||(3==j))  && eo_axisController_IsCalibrated(ems->axis_controller[j]))
                {
                    // j0 and j1 are independent, j2 and j3 are not
                    
                    // in herr i do things for j2 and j3 ...
                    
                    // use following formula:
                    // m2: motor position of left eye (as read by encoder)              (L)
                    // m3: motor position of rigth eye (as read by encoder)             (R)
                    // e2: version of eyes                                              (Vs)
                    // e3: vergence of eyes                                             (Vg)
                    // e2 = 0.5*m2 + 0.5*m3             (we must move the motors in the same directions to move eyes to follow a direction)
                    // e3 = 1.0*m2 - 1.0*m3             (we must move the motors in the opposite directions to do convergence or divergence)
                    // or:
                    // E = A * M, E = [e2, e3], M = {m2, m3], A = [0.5, +0.5], [1.0, -1.0]]
                    // hence M = E * A^-1, where A^-1 = [[1, 0.5,], [1, -0.5]].
                    // m2 = +e2 + e3
                    // m3 = e2 - 0.5 * e3
                    

                    // i should use the direct matrix A, not the inverse.
                    //axle_abs_pos[2] = 0.333f * ( +p2 +p3 );
                    //axle_abs_pos[3] = 0.500f * ( -p2 +p3 );
                    
                    
                    // i must rescale the positions read by encoders into the version-vergence space.
                    // i do it inside following function. at first i apply an offset, so that both positions of eyes are in the range [+45, -45],
                    // where the correct mapping is: rigth eye -> [+45, -45], nose, [+45, -45] <- left eye (the +45 is toward the direction of motor and goes towards the rigth).
                    // then i must apply the transformation into values of version = [-30, +30] and vergence = [0, 45] (on the gui is 50, but for now i prefer to keep it smaller)
                    // the transformation matrix is M = [[+0.333, +0.333], [-0.500, +0.500]] (and its inverse is  M^1 = [[+1.500, -1.000], [+1.500, +1.000]]).                
                    
                    eo_axisController_RescaleAxisPositionToVersionVergence(ems->axis_controller[2], ems->axis_controller[3], axle_abs_pos[2], axle_abs_pos[3], j);                    
                }
                else
                {                
                    eo_axisController_RescaleAxisPosition(ems->axis_controller[j], axle_abs_pos[j]);
                }
            }
            else    
            {
                eo_axisController_SetEncPos(ems->axis_controller[j], axle_abs_pos[j]);
            }
            
            eo_axisController_SetEncVel(ems->axis_controller[j], axle_abs_vel[j]);
        #endif
    }
    
    if (ems->n_calibrated != ems->naxles)
    {
        eo_emsController_CheckCalibrations();
    }
}

extern void eo_emsController_CheckFaults()
{
    if (!ems) return;
    
    static uint32_t ems_fault_mask_old[MAX_NAXLES];
           uint32_t ems_fault_mask_new[MAX_NAXLES];
    
    static uint16_t transmit_time = 0;
    if (++transmit_time >= 5000)
    {
        transmit_time = 0;
        JOINTS(j) ems_fault_mask_old[j] = 0;
    }
    
    JOINTS(j)
    {
        ems_fault_mask_new[j] = 0;
        
        if (eo_axisController_IsTorqueSensorFault(ems->axis_controller[j]))
        {
            ems_fault_mask_new[j] |= AXIS_TORQUE_SENS_FAULT;
        }
        
        if (eo_get_motor_timeout(ems->motors,j))
        {
            ems_fault_mask_new[j] |= MOTOR_CAN_NOT_RESPONDING;
        }
        
        if ((eo_absCalibratedEncoder_IsHardFault(ems->abs_calib_encoder[j])) || (eo_absCalibratedEncoder_AreThereTooManySpikes(ems->abs_calib_encoder[j])))
        {
            ems_fault_mask_new[j] |= eo_absCalibratedEncoder_IsHardFault(ems->abs_calib_encoder[j]);
        }
        
        eObool_t bNoFaultState = eobool_true;
        
        if (eo_is_motor_hard_fault(ems->motors,j))
        {
            ems_fault_mask_new[j] |= MOTOR_HARD_FAULT;
            
            bNoFaultState = eobool_false;
        }
        
        if (eo_is_motor_ext_fault(ems->motors,j))
        {
            ems_fault_mask_new[j] |= MOTOR_EXTERNAL_FAULT;
            
            bNoFaultState = eobool_false;
        }
                
        if (bNoFaultState && ems->new_motor_msg[j])
        {
            ems->new_motor_msg[j] = eobool_false;
            
            if (eo_motor_check_state_req(ems->motors,j))
            {
                ems_fault_mask_new[j] |= MOTOR_WRONG_STATE;
            }
        }
        
        if (ems_fault_mask_new[j] != ems_fault_mask_old[j]) // error state changed
        {
            ems_fault_mask_old[j] = ems_fault_mask_new[j];
            
            if (ems_fault_mask_new[j])
            {
                #if defined(USE_JACOBIAN)
                    GET_COUPLED(j,k)
                    {
                        set_2FOC_idle(k);
                        if (ems_fault_mask_new[j] & ~MOTOR_EXTERNAL_FAULT)
                        {
                            eo_axisController_SetHardwareFault(ems->axis_controller[k]);
                        }
                        else
                        {
                            eo_axisController_SetControlMode(ems->axis_controller[k], eomc_controlmode_cmd_idle);   
                        }
                    }
                #else // USE_JACOBIAN
                if (j<3 && (ems->board == emscontroller_board_SHOULDER 
                         || ems->board == emscontroller_board_WAIST 
                         || ems->board == emscontroller_board_CER_WAIST))
                {
                    set_2FOC_idle(0);
                    set_2FOC_idle(1);
                    set_2FOC_idle(2);
                    if (ems_fault_mask_new[j] & ~MOTOR_EXTERNAL_FAULT)
                    {
                        eo_axisController_SetHardwareFault(ems->axis_controller[0]);
                        eo_axisController_SetHardwareFault(ems->axis_controller[1]);
                        eo_axisController_SetHardwareFault(ems->axis_controller[2]);
                    }
                    else
                    {
                        eo_axisController_SetControlMode(ems->axis_controller[0], eomc_controlmode_cmd_idle);
                        eo_axisController_SetControlMode(ems->axis_controller[1], eomc_controlmode_cmd_idle); 
                        eo_axisController_SetControlMode(ems->axis_controller[2], eomc_controlmode_cmd_idle);                             
                    }
                }
                if (ems->board == emscontroller_board_HEAD_neckpitch_neckroll)
                {   //coupled joints (0,1) handling
                    set_2FOC_idle(0);
                    set_2FOC_idle(1);
                    if (ems_fault_mask_new[j] & ~MOTOR_EXTERNAL_FAULT)
                    {
                        eo_axisController_SetHardwareFault(ems->axis_controller[0]);
                        eo_axisController_SetHardwareFault(ems->axis_controller[1]);
                    }
                    else
                    {
                        eo_axisController_SetControlMode(ems->axis_controller[0], eomc_controlmode_cmd_idle);
                        eo_axisController_SetControlMode(ems->axis_controller[1], eomc_controlmode_cmd_idle);                              
                    }
                }
                if ((j > 1) && (ems->board == emscontroller_board_HEAD_neckyaw_eyes))
                {   //coupled joints (2,3) handling
                    set_2FOC_idle(2);
                    set_2FOC_idle(3);
                    if (ems_fault_mask_new[j] & ~MOTOR_EXTERNAL_FAULT)
                    {
                        eo_axisController_SetHardwareFault(ems->axis_controller[2]);
                        eo_axisController_SetHardwareFault(ems->axis_controller[3]);
                    }
                    else
                    {
                        eo_axisController_SetControlMode(ems->axis_controller[2], eomc_controlmode_cmd_idle);
                        eo_axisController_SetControlMode(ems->axis_controller[3], eomc_controlmode_cmd_idle);                              
                    }
                }
                else
                {
                    set_2FOC_idle(j);
                    if (ems_fault_mask_new[j] & ~MOTOR_EXTERNAL_FAULT)
                    {
                        eo_axisController_SetHardwareFault(ems->axis_controller[j]);
                    }
                    else
                    {
                        eo_axisController_SetControlMode(ems->axis_controller[j], eomc_controlmode_cmd_idle);   
                    }
                }
                #endif // USE_JACOBIAN
                    
                sendErrorMessage(j,ems_fault_mask_new[j],eo_get_motor_fault_mask(ems->motors,j));
            }
        }
    }
}
extern void eo_emsController_ReadTorque(uint8_t joint, eOmeas_torque_t trq_measure)
{
#ifndef EXPERIMENTAL_MOTOR_TORQUE
    if (!ems) return;
    
    eo_axisController_SetTorque(ems->axis_controller[joint], trq_measure);
#endif
}

extern void eo_emsController_PWM(int16_t* pwm_motor_16)
{
    if ((!ems))// || (ems->state_mask != EMS_OK) /*|| (ems->n_calibrated != ems->naxles)*/)
    {
        MOTORS(m) pwm_motor_16[m] = 0;
        return;
    }
    
    float pwm_joint[MAX_NAXLES];
    eObool_t torque_protection[MAX_NAXLES];
    eObool_t stiffness[MAX_NAXLES];
    
    //check watchdog only if not local actuation
    if(emscontroller_actuation_LOCAL != ems->act)
    {
        eo_motors_check_wdog(ems->motors);
    }
    
    //PWM computation (PID ecc)
    JOINTS(j)
    {
        torque_protection[j] = eobool_false;
        // davide: don'tcompute the PWM if the encoder is not calibrated, for calibration type 5
        // this should let to initialize the encoder (encoder_init() inside the speedmeter during the first cycles of the loop)
        if((!eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[j])) && (ems->axis_controller[j]->calibration_type == eomc_calibration_type5_hard_stops_mc4plus))
        {
            pwm_joint[j] = 0;
        }
        else
        {
            pwm_joint[j] = eo_axisController_PWM(ems->axis_controller[j], &stiffness[j]);
        }
    }
 
    if (ems->board == emscontroller_board_CER_WAIST)
    {
        int32_t rho0= ems->axis_controller[0]->position;
        int32_t rho1= ems->axis_controller[1]->position;
        int32_t rho2= ems->axis_controller[2]->position;
        
        static char stuck0 = 0;
        static char stuck1 = 0;
        static char stuck2 = 0;
        
        if (rho0*rho0+rho1*rho1+rho2*rho2-rho0*rho1-rho1*rho2-rho2*rho0>400000) // 25 deg limit
        {
            if ( !(rho0 <= rho1 && rho0 <= rho2 && pwm_joint[0]>=0.f) && !(rho0 >= rho1 && rho0 >= rho2 && pwm_joint[0]<=0.f))
            {
                pwm_joint[0] = 0.f;
                
                if (!stuck0)
                {
                    stuck0 = 1;
                    eo_axisController_Stop(ems->axis_controller[0]);
                }
            }
            
            if ( !(rho1 <= rho2 && rho1 <= rho0 && pwm_joint[1]>=0.f) && !(rho1 >= rho2 && rho1 >= rho0 && pwm_joint[1]<=0.f))
            {
                pwm_joint[1] = 0.f;
                
                if (!stuck1)
                {
                    stuck1 = 1;
                    eo_axisController_Stop(ems->axis_controller[1]);
                }
            }
            
            if ( !(rho2 <= rho0 && rho2 <= rho1 && pwm_joint[2]>=0.f) && !(rho2 >= rho0 && rho2 >= rho1 && pwm_joint[2]<=0.f))
            {
                pwm_joint[2] = 0.f;
                
                if (!stuck2)
                {
                    stuck2 = 1;
                    eo_axisController_Stop(ems->axis_controller[2]);
                }
            }
        }
        else
        {
            stuck0 = stuck1 = stuck2 = 0;
        }
    }

    //torque control limits protection mechanism
    JOINTS(j)
    {
      if ((ems->axis_controller[j]->control_mode==eomc_controlmode_torque) ||
          (ems->axis_controller[j]->interact_mode==eOmc_interactionmode_compliant &&
          (ems->axis_controller[j]->control_mode==eomc_controlmode_position ||
           ems->axis_controller[j]->control_mode==eomc_controlmode_velocity ||
           ems->axis_controller[j]->control_mode==eomc_controlmode_mixed ||
           ems->axis_controller[j]->control_mode==eomc_controlmode_direct)))
      {
        int32_t pos = ems->axis_controller[j]->position;
        if (pos <  ems->axis_controller[j]->pos_min)
        {
            #ifdef EXPERIMENTAL_SPEED_CONTROL
            
                if (pwm_joint[j]<0) pwm_joint[j] = 0;
            
            #else
            float pwm_lim = eo_pid_PWM_p( ems->axis_controller[j]->pidP, ems->axis_controller[j]->pos_min - pos);
            if ((pwm_lim > 0) ^ (pwm_joint[j] > pwm_lim)) 
            {
               pwm_joint[j] = pwm_lim;
               for (int8_t k; k<ems->naxles; k++)
                   torque_protection[k] = eobool_true; //put in protection all motors
            }
            #endif
        }
        else if (pos >  ems->axis_controller[j]->pos_max)
        {
            #ifdef EXPERIMENTAL_SPEED_CONTROL
            
                if (pwm_joint[j]>0) pwm_joint[j] = 0;
            
            #else
            float pwm_lim = eo_pid_PWM_p( ems->axis_controller[j]->pidP,  ems->axis_controller[j]->pos_max - pos);
            if ((pwm_lim > 0) ^ (pwm_joint[j] > pwm_lim))
            {
               pwm_joint[j] = pwm_lim;
               for (int8_t k; k<ems->naxles; k++)
                   torque_protection[k] = eobool_true; //put in protection all motors
            }
            #endif
        }
      }
    }

    //PWM decoupling
    float pwm_motor[MAX_NAXLES];
    eo_motors_decouple_PWM(ems->motors, pwm_joint, pwm_motor, stiffness);
    
    #ifndef EXPERIMENTAL_SPEED_CONTROL
    //Friction compensation after joints decoupling
    MOTORS(m)
    {
        if (!torque_protection[m])
            pwm_motor[m] = eo_axisController_FrictionCompensation(ems->axis_controller[m],pwm_motor[m],ems->motor_velocity[m]);
    }
    #endif

    //hardware limit
    MOTORS(m) LIMIT(pwm_motor[m], ems->actuation_limit);
    
    //save the pwm data after the decoupling   
    //the output should be between -PWM_OUTPUT_LIMIT and +PWM_OUTPUT_LIMIT, indipendently from the hardware
    //int16_t pwm_rescaled[MAX_NAXLES] = {0};
    //MOTORS(m) pwm_rescaled[m] = RESCALE2PWMLIMITS(pwm_motor[m], ems->actuation_limit);
    //MOTORS(m) ems->axis_controller[m]->controller_output = RESCALE2PWMLIMITS(pwm_motor[m], ems->actuation_limit);//pwm_rescaled[m];
    MOTORS(m) ems->axis_controller[m]->controller_output = pwm_motor[m]; //no more rescaled
    
    MOTORS(m) pwm_motor_16[m] = (int16_t)pwm_motor[m];
}

//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////


extern void eo_emsController_SetOutput(uint8_t joint, int16_t out)
{
    if (ems) eo_axisController_SetOutput(ems->axis_controller[joint], out);    
}

extern void eo_emsController_SetPosRef(uint8_t joint, int32_t pos, int32_t avg_vel)
{
    if (ems) eo_axisController_SetPosRef(ems->axis_controller[joint], pos, avg_vel);       
}

extern void eo_emsController_SetPosRaw(uint8_t joint, int32_t pos)
{
    if (ems) eo_axisController_SetPosRaw(ems->axis_controller[joint], pos);       
}

extern void eo_emsController_SetVelRef(uint8_t joint, int32_t vel, int32_t avg_acc)
{
    if (ems) eo_axisController_SetVelRef(ems->axis_controller[joint], vel, avg_acc);
}

extern void eo_emsController_SetTrqRef(uint8_t joint, int32_t trq)
{
    if (ems) eo_axisController_SetTrqRef(ems->axis_controller[joint], trq);
}

extern void eo_emsController_GetDecoupledMeasuredTorque(uint8_t joint_id, int32_t * torque_motor)
{
    if (!ems) return;
    
    if (ems->board == emscontroller_board_SHOULDER)
    {
          if (joint_id==0) {*torque_motor= ems->axis_controller[0]->torque_meas_jnt + ems->axis_controller[1]->torque_meas_jnt; return;}
          if (joint_id==1) {*torque_motor=(ems->axis_controller[1]->torque_meas_jnt - ems->axis_controller[2]->torque_meas_jnt)* 0.625; return;}
          if (joint_id==2) {*torque_motor=ems->axis_controller[2]->torque_meas_jnt * 0.625; return;}
          if (joint_id==3) {*torque_motor=ems->axis_controller[3]->torque_meas_jnt; return;}
    }
    else if (ems->board == emscontroller_board_WAIST)
    {
          if (joint_id==0) {*torque_motor=(ems->axis_controller[0]->torque_meas_jnt - ems->axis_controller[1]->torque_meas_jnt)*0.5 + ems->axis_controller[2]->torque_meas_jnt*0.275; return;}
          if (joint_id==1) {*torque_motor=(ems->axis_controller[0]->torque_meas_jnt + ems->axis_controller[1]->torque_meas_jnt)*0.5 + ems->axis_controller[2]->torque_meas_jnt*0.275; return;}
          if (joint_id==2) {*torque_motor= ems->axis_controller[2]->torque_meas_jnt*0.55; return;}
    }
    else if (ems->board == emscontroller_board_UPPERLEG || ems->board == emscontroller_board_CER_WAIST)
    {
          if (joint_id==0) {*torque_motor=ems->axis_controller[0]->torque_meas_jnt; return;}
          if (joint_id==1) {*torque_motor=ems->axis_controller[1]->torque_meas_jnt; return;}
          if (joint_id==2) {*torque_motor=ems->axis_controller[2]->torque_meas_jnt; return;}
          if (joint_id==3) {*torque_motor=ems->axis_controller[3]->torque_meas_jnt; return;}
    }
    else if (ems->board == emscontroller_board_ANKLE)
    {
          if (joint_id==0) {*torque_motor=ems->axis_controller[0]->torque_meas_jnt; return;}
          if (joint_id==1) {*torque_motor=ems->axis_controller[1]->torque_meas_jnt; return;}
    }
    else    
    {
        // marco.accame: for head-v3 this case is ok. no need to differentiate according to used board
        // the reason is that ... there is no torque control
        *torque_motor=ems->axis_controller[joint_id]->torque_meas_jnt;
    }
}

extern void eo_emsController_GetDecoupledReferenceTorque(uint8_t joint_id, int32_t * torque_motor)
{
    if (!ems) return; 
    
    if (ems->board == emscontroller_board_SHOULDER)
    {
        if (joint_id==0) {*torque_motor= ems->axis_controller[0]->torque_ref_jnt + ems->axis_controller[1]->torque_ref_jnt; return;}
        if (joint_id==1) {*torque_motor=(ems->axis_controller[1]->torque_ref_jnt - ems->axis_controller[2]->torque_ref_jnt)* 0.625; return;}
        if (joint_id==2) {*torque_motor=ems->axis_controller[2]->torque_ref_jnt * 0.625; return;}
        if (joint_id==3) {*torque_motor=ems->axis_controller[3]->torque_ref_jnt; return;}
    }
    else if (ems->board == emscontroller_board_WAIST)
    {
        if (joint_id==0) {*torque_motor=(ems->axis_controller[0]->torque_ref_jnt - ems->axis_controller[1]->torque_ref_jnt)*0.5 + ems->axis_controller[2]->torque_ref_jnt*0.275; return;}
        if (joint_id==1) {*torque_motor=(ems->axis_controller[0]->torque_ref_jnt + ems->axis_controller[1]->torque_ref_jnt)*0.5 + ems->axis_controller[2]->torque_ref_jnt*0.275; return;}
        if (joint_id==2) {*torque_motor= ems->axis_controller[2]->torque_ref_jnt*0.55; return;}
    }
    else if (ems->board == emscontroller_board_UPPERLEG || ems->board == emscontroller_board_CER_WAIST)
    {
        if (joint_id==0) {*torque_motor=ems->axis_controller[0]->torque_ref_jnt; return;}
        if (joint_id==1) {*torque_motor=ems->axis_controller[1]->torque_ref_jnt; return;}
        if (joint_id==2) {*torque_motor=ems->axis_controller[2]->torque_ref_jnt; return;}
        if (joint_id==3) {*torque_motor=ems->axis_controller[3]->torque_ref_jnt; return;}
    }
    else if (ems->board == emscontroller_board_ANKLE)
    {
        if (joint_id==0) {*torque_motor=ems->axis_controller[0]->torque_ref_jnt; return;}
        if (joint_id==1) {*torque_motor=ems->axis_controller[1]->torque_ref_jnt; return;}
    }
    else 
    {       
        // marco.accame: for head-v3 this case is ok. no need to differentiate according to used board
        // the reason is that ... there is no torque control       
        *torque_motor=ems->axis_controller[joint_id]->torque_ref_jnt; 
    }
}

extern void eo_emsController_SetControlModeGroupJoints(uint8_t joint, eOmc_controlmode_command_t mode)
{
    eo_emsController_SetControlMode(joint, mode);
}

extern eObool_t eo_emsController_SetInteractionModeGroupJoints(uint8_t joint, eOmc_interactionmode_t mode)
{
    if (joint<3 && (ems->board == emscontroller_board_SHOULDER 
                 || ems->board == emscontroller_board_WAIST 
                 || ems->board == emscontroller_board_CER_WAIST)) 
    {
        eo_emsController_SetInteractionMode(0, mode);
        eo_emsController_SetInteractionMode(1, mode);
        eo_emsController_SetInteractionMode(2, mode);
    }
    else if(emscontroller_board_HEAD_neckpitch_neckroll == ems->board)  
    {
        // j0 and j1 are coupled
        eo_emsController_SetInteractionMode(0, mode);
        eo_emsController_SetInteractionMode(1, mode);
    }
    else if((joint > 1) && (emscontroller_board_HEAD_neckyaw_eyes == ems->board))
    {
        // j2 and j3 are coupled
        eo_emsController_SetInteractionMode(2, mode);
        eo_emsController_SetInteractionMode(3, mode);
    }
    else
    {
        eo_emsController_SetInteractionMode(joint, mode);
    }
  
    return eobool_true;
}

extern void eo_emsController_SetControlMode(uint8_t joint, eOmc_controlmode_command_t mode)
{
    if (!ems) return;
    
    /*
    static eObool_t motors_not_configured = eobool_true;
    if (motors_not_configured)
    {
        motors_not_configured = eobool_false;
        MOTORS(m) config_2FOC(m);   
    }
    */
    
    if (mode == eomc_controlmode_cmd_force_idle)
    {
        if (!eo_axisController_IsHardwareFault(ems->axis_controller[joint]))
        {
            mode = eomc_controlmode_cmd_idle;
        }
    }
    
    if (mode == eomc_controlmode_cmd_force_idle)
    {
        #if defined(USE_JACOBIAN)
        GET_COUPLED(joint,j)
        {
            force_2FOC_idle(j);
            eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[j]);
            eo_axisController_SetControlMode(ems->axis_controller[j], eomc_controlmode_cmd_force_idle);
        }
        #else // USE_JACOBIAN
        if (joint<3 && (ems->board == emscontroller_board_SHOULDER 
                     || ems->board == emscontroller_board_WAIST 
                     || ems->board == emscontroller_board_CER_WAIST))
        {
            force_2FOC_idle(0);
            eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[0]);
            eo_axisController_SetControlMode(ems->axis_controller[0], eomc_controlmode_cmd_force_idle);
            
            force_2FOC_idle(1);
            eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[1]);
            eo_axisController_SetControlMode(ems->axis_controller[1], eomc_controlmode_cmd_force_idle);
            
            force_2FOC_idle(2);                
            eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[2]);
            eo_axisController_SetControlMode(ems->axis_controller[2], eomc_controlmode_cmd_force_idle);
        }
		else if(emscontroller_board_HEAD_neckpitch_neckroll == ems->board)  
  		{	
			force_2FOC_idle(0);
            eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[0]);
            eo_axisController_SetControlMode(ems->axis_controller[0], eomc_controlmode_cmd_force_idle);
            
            force_2FOC_idle(1);
            eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[1]);
            eo_axisController_SetControlMode(ems->axis_controller[1], eomc_controlmode_cmd_force_idle);

		}
        else if((joint > 1) && (emscontroller_board_HEAD_neckyaw_eyes == ems->board))
		{
			force_2FOC_idle(2);                
            eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[2]);
            eo_axisController_SetControlMode(ems->axis_controller[2], eomc_controlmode_cmd_force_idle);
		
			force_2FOC_idle(3);                
            eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[3]);
            eo_axisController_SetControlMode(ems->axis_controller[3], eomc_controlmode_cmd_force_idle);
		}
        else
        {
            force_2FOC_idle(joint);
            eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[joint]);
            eo_axisController_SetControlMode(ems->axis_controller[joint], eomc_controlmode_cmd_force_idle);
        }
        #endif // USE_JACOBIAN
    }
    else if ((mode == eomc_controlmode_cmd_idle) || (mode == eomc_controlmode_cmd_switch_everything_off))
    {
        #if defined(USE_JACOBIAN)
        GET_COUPLED(joint,j)
        {
            set_2FOC_idle(j);
            eo_axisController_SetControlMode(ems->axis_controller[j], eomc_controlmode_cmd_idle);
        }
        #else
        if (joint<3 && (ems->board == emscontroller_board_SHOULDER 
                     || ems->board == emscontroller_board_WAIST 
                     || ems->board == emscontroller_board_CER_WAIST))
        {
            set_2FOC_idle(0);
            set_2FOC_idle(1);
            set_2FOC_idle(2); 
            
            eo_axisController_SetControlMode(ems->axis_controller[0], eomc_controlmode_cmd_idle);
            eo_axisController_SetControlMode(ems->axis_controller[1], eomc_controlmode_cmd_idle);
            eo_axisController_SetControlMode(ems->axis_controller[2], eomc_controlmode_cmd_idle);
        }
		else if((emscontroller_board_HEAD_neckpitch_neckroll == ems->board))
        {	
            set_2FOC_idle(0);
            set_2FOC_idle(1);

            eo_axisController_SetControlMode(ems->axis_controller[0], eomc_controlmode_cmd_idle);
            eo_axisController_SetControlMode(ems->axis_controller[1], eomc_controlmode_cmd_idle);	
		}
		else if((joint > 1) && (emscontroller_board_HEAD_neckyaw_eyes == ems->board))
        {   
            set_2FOC_idle(2);
            set_2FOC_idle(3);

            eo_axisController_SetControlMode(ems->axis_controller[2], eomc_controlmode_cmd_idle);
            eo_axisController_SetControlMode(ems->axis_controller[3], eomc_controlmode_cmd_idle);               
        }
        else
        {
            set_2FOC_idle(joint);
            eo_axisController_SetControlMode(ems->axis_controller[joint], eomc_controlmode_cmd_idle);
        }
        #endif
    }
    else // not an idle mode
    {
        #if defined(USE_JACOBIAN)
        
        GET_COUPLED(joint,j)
        {
            if (eo_axisController_IsHardwareFault(ems->axis_controller[j])) return;
            if (eo_is_motor_ext_fault(ems->motors, j)) return;
        }
        
        GET_COUPLED(joint,j) if (!eo_axisController_SetControlMode(ems->axis_controller[j], mode)) return;
        
        GET_COUPLED(joint,m) set_2FOC_running(m);
        
        #else
        
        if (joint<3 && (ems->board == emscontroller_board_SHOULDER 
                     || ems->board == emscontroller_board_WAIST 
                     || ems->board == emscontroller_board_CER_WAIST))
        {
            if (eo_axisController_IsHardwareFault(ems->axis_controller[0])) return;
            if (eo_axisController_IsHardwareFault(ems->axis_controller[1])) return;
            if (eo_axisController_IsHardwareFault(ems->axis_controller[2])) return;
            
            if (eo_is_motor_ext_fault(ems->motors, 0)) return;
            if (eo_is_motor_ext_fault(ems->motors, 1)) return;
            if (eo_is_motor_ext_fault(ems->motors, 2)) return;
            
            if (!eo_axisController_SetControlMode(ems->axis_controller[0], mode)) return;
            if (!eo_axisController_SetControlMode(ems->axis_controller[1], mode)) return;
            if (!eo_axisController_SetControlMode(ems->axis_controller[2], mode)) return;
            
            set_2FOC_running(0);
            set_2FOC_running(1);
            set_2FOC_running(2);
        }

		else if((emscontroller_board_HEAD_neckpitch_neckroll == ems->board))
        {
			if (eo_axisController_IsHardwareFault(ems->axis_controller[0])) return;
            if (eo_axisController_IsHardwareFault(ems->axis_controller[1])) return;

            if (eo_is_motor_ext_fault(ems->motors, 0)) return;
            if (eo_is_motor_ext_fault(ems->motors, 1)) return;

			if (!eo_axisController_SetControlMode(ems->axis_controller[0], mode)) return;
            if (!eo_axisController_SetControlMode(ems->axis_controller[1], mode)) return;
        
            set_2FOC_running(0);
            set_2FOC_running(1);
        }
		else if((joint > 1) && (emscontroller_board_HEAD_neckyaw_eyes == ems->board))
        {
            if (eo_axisController_IsHardwareFault(ems->axis_controller[2])) return;
            if (eo_axisController_IsHardwareFault(ems->axis_controller[3])) return;

            if (eo_is_motor_ext_fault(ems->motors, 2)) return;
            if (eo_is_motor_ext_fault(ems->motors, 3)) return;

            if (!eo_axisController_SetControlMode(ems->axis_controller[2], mode)) return;
            if (!eo_axisController_SetControlMode(ems->axis_controller[3], mode)) return;
        
            set_2FOC_running(2);
            set_2FOC_running(3);                        
        }
        else
        {
            if (eo_axisController_IsHardwareFault(ems->axis_controller[joint])) return;
            if (eo_is_motor_ext_fault(ems->motors, joint)) return;
            if (!eo_axisController_SetControlMode(ems->axis_controller[joint], mode)) return;
            set_2FOC_running(joint);
        }
        #endif
    }
}

extern eOmc_controlmode_t eo_emsController_GetControlMode(uint8_t joint)
{
    if (!ems) return eomc_controlmode_idle;

    return eo_axisController_GetControlMode(ems->axis_controller[joint]);
}

extern eObool_t eo_emsController_SetInteractionMode(uint8_t joint, eOmc_interactionmode_t mode)
{
    return eo_axisController_SetInteractionMode(ems->axis_controller[joint], mode);
}

/*
extern eOmc_interactionmode_t eo_emsController_GetInteractionMode(uint8_t joint)
{
    return eo_axisController_GetInteractionMode(ems->axis_controller[joint]);
}
*/

extern void eo_emsController_ResetPosPid(uint8_t joint)
{
    if (ems) eo_pid_Reset(eo_axisController_GetPosPidPtr(ems->axis_controller[joint]));
}

extern eObool_t eo_emsMotorController_isMotorEncoderCalibrated(uint8_t motor)
{
    if (!ems) return eobool_false;
    
    return eo_motors_isEncCalibrated(ems->motors, motor);
}

extern void eo_emsController_StartCalibration_type3(uint8_t joint, int32_t pos, int32_t vel, int32_t offset)
{
    if (!ems) return;
    
    ems->n_calibrated = 0;
        
    ems->axis_controller[joint] ->calibration_type = eomc_calibration_type3_abs_sens_digital;
    
    //in case of coupled joints, I reset the calibration state of the others...they must always change their state together
    s_eo_emsController_ResetCalibrationCoupledJoints(joint);
    
    eo_absCalibratedEncoder_Calibrate(ems->abs_calib_encoder[joint], offset);
    
    eo_axisController_StartCalibration_type3(ems->axis_controller[joint]);
}

extern void eo_emsController_StartCalibration_type6(uint8_t joint, int32_t pos, int32_t vel, int32_t maxencoder)
{
    if (!ems) return;
    
    ems->n_calibrated = 0;
        
    //Set the right calibration type
    ems->axis_controller[joint] ->calibration_type = eomc_calibration_type6_mais_mc4plus;
    
    /*
    //in case of coupled joints, I reset the calibration state of the others...they must always change their state together
    s_eo_emsController_ResetCalibrationCoupledJoints(joint);
    
    //zero offset for this calib? maybe I could change it later, when the calib procedure is ended
    eo_absCalibratedEncoder_Calibrate(ems->abs_calib_encoder[joint], 0);
    
    //I should convert the position and velocity in iCubDegrees? probably yes
    eOmc_joint_t *joint_ref = eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, joint);
    float divider = joint_ref->config.jntEncoderResolution;
    if(divider < 0)
        divider = -divider;
    
    uint32_t pos_icubdeg = (float) pos * 65535.0 / divider; //icub degrees are always unsigned (0-65535)
    uint32_t vel_icubdeg = (float) vel * 65535.0 / divider;
    eo_axisController_StartCalibration_type6(ems->axis_controller[joint], pos_icubdeg, vel_icubdeg, maxencoder);
    */
}
extern void eo_emsController_StartCalibration_type5(uint8_t joint, int32_t pwmlimit, int32_t final_position)
{
    if (!ems) return;
    
    ems->n_calibrated = 0;
        
    //calibrating procedure
    //comments:
    //we don't need to set an offset to the encoder in this case! at the startup, the value is 0
  
    //Set the right calibration type
    ems->axis_controller[joint] ->calibration_type = eomc_calibration_type5_hard_stops_mc4plus;
    
    //in case of coupled joints, I reset the calibration state of the others...they must always change their state together
    s_eo_emsController_ResetCalibrationCoupledJoints(joint);
    //offset is 0 in this case
    eo_absCalibratedEncoder_Calibrate(ems->abs_calib_encoder[joint], 0);
    eo_axisController_StartCalibration_type5(ems->axis_controller[joint], pwmlimit,final_position);
    
}

extern void eo_emsController_CheckCalibrations(void)
{
    if (!ems) return;
    
    ems->n_calibrated = 0;
    
    #if defined(USE_JACOBIAN)
    
    eObool_t joint_ok[MAX_NAXLES];
    
    JOINTS(j)
    {
        joint_ok[j] = eobool_true;
        
        ENCODERS(e)
        {
          if (eo_motors_are_coupled(ems->motors, j, e))
          {
            if (!eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[e]))
            {
                joint_ok[j] = eobool_false;
                break;
            }
           }
        }
    }
    
    JOINTS(j)
    {
        if (joint_ok[j])
        {
            //2FOC-based control
            if (ems->act == emscontroller_actuation_2FOC)
            {
                static eObool_t foc_running[MAX_NAXLES] = {eobool_false, eobool_false, eobool_false, eobool_false};
                if (!foc_running[j])
                {
                    foc_running[j] = eobool_true;
        
                    set_2FOC_running(j);
                }
                
                if (eo_motors_isEncCalibrated(ems->motors, j))
                {
                    ems->n_calibrated++;
                    eo_axisController_SetCalibrated(ems->axis_controller[j]);
        
                    foc_running[j] = eobool_false;
                    
                    eo_axisController_SetControlMode(ems->axis_controller[j], eomc_controlmode_cmd_position);
                    eo_axisController_SetInteractionMode(ems->axis_controller[j], eOmc_interactionmode_stiff);
                }	
            }
            //MC4plus-based control
            else if (ems->act == emscontroller_actuation_LOCAL)
            {
                if( (ems->axis_controller[j]->calibration_type == eomc_calibration_type3_abs_sens_digital)
                    ||
                    (ems->axis_controller[j]->calibration_type == eomc_calibration_type5_hard_stops_mc4plus && ems->axis_controller[j]->hardwarelimitisreached))
                {
                    ems->n_calibrated++;
                    eo_axisController_SetCalibrated(ems->axis_controller[j]);
              
                    eo_axisController_SetControlMode(ems->axis_controller[j], eomc_controlmode_cmd_position);
                    eo_axisController_SetInteractionMode(ems->axis_controller[j], eOmc_interactionmode_stiff);
                }
            }
        }
    }
    
    #else // ! USE_JACOBIAN

    if (ems->board == emscontroller_board_UPPERLEG || ems->board == emscontroller_board_ANKLE)
    {
        JOINTS(j)
        {
            if (eo_axisController_IsCalibrated(ems->axis_controller[j]))
            {
                ems->n_calibrated++;
            }
            else if (eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[j]))
            {                    
                static eObool_t foc_running[MAX_NAXLES] = {eobool_false, eobool_false, eobool_false, eobool_false};
            
                if (!foc_running[j])
                {
                    foc_running[j] = eobool_true;
                
                    set_2FOC_running(j);
                }
                
                if (eo_motors_isEncCalibrated(ems->motors, j))
                {
                    ems->n_calibrated++;
            
                    eo_axisController_SetCalibrated(ems->axis_controller[j]);
                    eo_axisController_SetControlMode(ems->axis_controller[j], eomc_controlmode_cmd_position);
                    eo_axisController_SetInteractionMode(ems->axis_controller[j], eOmc_interactionmode_stiff);
                
                    foc_running[j] = eobool_false;
                }
            }
        }
    }
    else if (emscontroller_board_FACE_eyelids_jaw == ems->board || emscontroller_board_FACE_lips == ems->board)
    {
        JOINTS(j)
        {
            if (eo_axisController_IsCalibrated(ems->axis_controller[j]))
            {
                ems->n_calibrated++;
            }
            else if (eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[j]))
            {
                if((ems->axis_controller[j]->calibration_type == eomc_calibration_type3_abs_sens_digital)
                    ||
                    (ems->axis_controller[j]->calibration_type == eomc_calibration_type5_hard_stops_mc4plus && ems->axis_controller[j]->hardwarelimitisreached))
                {    
                    ems->n_calibrated++;
                    eo_axisController_SetCalibrated(ems->axis_controller[j]);
                
                    set_2FOC_running(j);
                
                    eo_axisController_SetControlMode(ems->axis_controller[j], eomc_controlmode_cmd_position);
                    eo_axisController_SetInteractionMode(ems->axis_controller[j], eOmc_interactionmode_stiff);
                }
            }
        }
    }
    else if (ems->board == emscontroller_board_SHOULDER 
          || ems->board == emscontroller_board_WAIST 
          || ems->board == emscontroller_board_CER_WAIST)
    {
        if (eo_axisController_IsCalibrated(ems->axis_controller[0]) &&
            eo_axisController_IsCalibrated(ems->axis_controller[1]) &&
            eo_axisController_IsCalibrated(ems->axis_controller[2]))
        {
            ems->n_calibrated+=3;
        }
        else if (eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[0]) && 
                 eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[1]) &&
                 eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[2]))
        {
            static eObool_t foc_running = eobool_false;
            
            if (!foc_running)
            {
                foc_running = eobool_true;
                
                set_2FOC_running(0);
                set_2FOC_running(1);
                set_2FOC_running(2);
            }
            
            if (eo_motors_isEncCalibrated(ems->motors, 0) 
             && eo_motors_isEncCalibrated(ems->motors, 1) 
             && eo_motors_isEncCalibrated(ems->motors, 2))
            {
                ems->n_calibrated+=3;
            
                eo_axisController_SetCalibrated(ems->axis_controller[0]);
                eo_axisController_SetCalibrated(ems->axis_controller[1]);
                eo_axisController_SetCalibrated(ems->axis_controller[2]);
                
                eo_axisController_SetControlMode(ems->axis_controller[0], eomc_controlmode_cmd_position);
                eo_axisController_SetInteractionMode(ems->axis_controller[0], eOmc_interactionmode_stiff);
                eo_axisController_SetControlMode(ems->axis_controller[1], eomc_controlmode_cmd_position);
                eo_axisController_SetInteractionMode(ems->axis_controller[1], eOmc_interactionmode_stiff);
                eo_axisController_SetControlMode(ems->axis_controller[2], eomc_controlmode_cmd_position);
                eo_axisController_SetInteractionMode(ems->axis_controller[2], eOmc_interactionmode_stiff);
                
                foc_running = eobool_false;
            }
        }
  
        if (ems->board == emscontroller_board_SHOULDER || ems->board == emscontroller_board_CER_WAIST)
        {
            if (eo_axisController_IsCalibrated(ems->axis_controller[3]))
            {
                ems->n_calibrated++;
            }
            else if (eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[3]))
            {    
                static eObool_t foc_running = eobool_false;
            
                if (!foc_running)
                {
                    foc_running = eobool_true;
                
                    set_2FOC_running(3);
                }
                
                if (eo_motors_isEncCalibrated(ems->motors, 3))
                {
                    ems->n_calibrated++;
            
                    eo_axisController_SetCalibrated(ems->axis_controller[3]);
                    
                    eo_axisController_SetControlMode(ems->axis_controller[3], eomc_controlmode_cmd_position);
                    eo_axisController_SetInteractionMode(ems->axis_controller[3], eOmc_interactionmode_stiff);
                
                    foc_running = eobool_false;
                }
            }
        }
    }

   	else if(emscontroller_board_HEAD_neckpitch_neckroll == ems->board)
    {
        if (eo_axisController_IsCalibrated(ems->axis_controller[0]) &&
            eo_axisController_IsCalibrated(ems->axis_controller[1]) )
        {
            ems->n_calibrated+=2;
        }
        else if (eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[0]) && eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[1]))
        {
            if ((  (ems->axis_controller[0]->calibration_type == eomc_calibration_type3_abs_sens_digital)
                && (ems->axis_controller[1]->calibration_type == eomc_calibration_type3_abs_sens_digital))
                ||
                (  (ems->axis_controller[0]->calibration_type == eomc_calibration_type5_hard_stops_mc4plus && ems->axis_controller[0]->hardwarelimitisreached)
                && (ems->axis_controller[1]->calibration_type == eomc_calibration_type5_hard_stops_mc4plus && ems->axis_controller[1]->hardwarelimitisreached)))
            {
                ems->n_calibrated+=2;
                eo_axisController_SetCalibrated(ems->axis_controller[0]);
                eo_axisController_SetCalibrated(ems->axis_controller[1]);

                set_2FOC_running(0);
                set_2FOC_running(1);

                eo_axisController_SetControlMode(ems->axis_controller[0], eomc_controlmode_cmd_position);
                eo_axisController_SetInteractionMode(ems->axis_controller[0], eOmc_interactionmode_stiff);
                eo_axisController_SetControlMode(ems->axis_controller[1], eomc_controlmode_cmd_position);
                eo_axisController_SetInteractionMode(ems->axis_controller[1], eOmc_interactionmode_stiff);	
            }
        }            
    }
	else if(emscontroller_board_HEAD_neckyaw_eyes == ems->board)
    {
        //independent joints -> 0,1
        JOINTS(j)
        {
            if ( j > 1 )
                break;
            if (eo_axisController_IsCalibrated(ems->axis_controller[j]))
            {
                ems->n_calibrated++;
            }
            else if (eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[j]))
            {
                if  ((ems->axis_controller[j]->calibration_type == eomc_calibration_type3_abs_sens_digital)
                    ||
                    (ems->axis_controller[j]->calibration_type == eomc_calibration_type5_hard_stops_mc4plus && ems->axis_controller[j]->hardwarelimitisreached))
                {
                    ems->n_calibrated++;
                    eo_axisController_SetCalibrated(ems->axis_controller[j]);
                    
                    set_2FOC_running(j);
                    
                    eo_axisController_SetControlMode(ems->axis_controller[j], eomc_controlmode_cmd_position);
                    eo_axisController_SetInteractionMode(ems->axis_controller[j], eOmc_interactionmode_stiff);
                }
            }
        }
        
        //coupled joints -> 2,3
        if (eo_axisController_IsCalibrated(ems->axis_controller[2]) &&
            eo_axisController_IsCalibrated(ems->axis_controller[3]))
        {
            ems->n_calibrated+=2;
        }
        else if ( eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[2]) && eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[3]))
        {
            if ((  (ems->axis_controller[2]->calibration_type == eomc_calibration_type3_abs_sens_digital)
                && (ems->axis_controller[3]->calibration_type == eomc_calibration_type3_abs_sens_digital))
                ||
                (  (ems->axis_controller[2]->calibration_type == eomc_calibration_type5_hard_stops_mc4plus && ems->axis_controller[2]->hardwarelimitisreached)
                && (ems->axis_controller[3]->calibration_type == eomc_calibration_type5_hard_stops_mc4plus && ems->axis_controller[3]->hardwarelimitisreached)))
            {
                ems->n_calibrated+=2; 
                eo_axisController_SetCalibrated(ems->axis_controller[2]);
                eo_axisController_SetCalibrated(ems->axis_controller[3]);
               
                set_2FOC_running(2);
                set_2FOC_running(3);

                eo_axisController_SetControlMode(ems->axis_controller[2], eomc_controlmode_cmd_position);
                eo_axisController_SetInteractionMode(ems->axis_controller[2], eOmc_interactionmode_stiff);
                eo_axisController_SetControlMode(ems->axis_controller[3], eomc_controlmode_cmd_position);
                eo_axisController_SetInteractionMode(ems->axis_controller[3], eOmc_interactionmode_stiff);
                
                //eyes are virtually coupled, need to reset trajectory
                s_eo_emsController_InitTrajectoryVersionVergence(2);
                s_eo_emsController_InitTrajectoryVersionVergence(3);
            }                
       }
   }
   #endif // ! USE_JACOBIAN        
}

extern void eo_emsController_ResetCalibrationValues(uint8_t joint)
{
    if(ems)
    {
        eo_axisController_ResetCalibration(ems->axis_controller[joint]);
        eo_absCalibratedEncoder_ResetCalibration(ems->abs_calib_encoder[joint]);
    }
    
    return;
}

extern void eo_emsController_Stop(uint8_t joint)
{
    if (ems) eo_axisController_Stop(ems->axis_controller[joint]);
}

extern void eo_emsController_ResetTrqPid(uint8_t joint)
{
    if (ems) eo_pid_Reset(eo_axisController_GetTrqPidPtr(ems->axis_controller[joint]));
}

extern void eo_emsController_GetPWMOutput(uint8_t joint, int32_t* pwm)
{
    if (ems && ems->axis_controller[joint])
    {
        *pwm = ems->axis_controller[joint]->controller_output;
    }
}

extern void eo_emsController_GetActivePidStatus(uint8_t joint, eOmc_joint_status_ofpid_t* pidStatus)
{
    if (ems && ems->axis_controller[joint])
    {
        eo_axisController_GetActivePidStatus(ems->axis_controller[joint], pidStatus);
    }
    else
    {
        // but i would use: memset(pidStatus, 0, sizeof(eOmc_joint_status_ofpid_t)); 
        pidStatus->legacy.error     = 0;
        pidStatus->legacy.output    = 0;
        pidStatus->legacy.positionreference = 0;
        pidStatus->legacy.torquereference = 0;
    }
}

extern void eo_emsController_GetJointStatus(uint8_t joint, eOmc_joint_status_t* jointStatus)
{
    if (ems && ems->axis_controller[joint])
    {
        eo_axisController_GetJointStatus(ems->axis_controller[joint], jointStatus);
    }
    else
    {
        memset(&jointStatus->basic, 0, sizeof(eOmc_joint_status_basic_t)); 
        jointStatus->modes.controlmodestatus    = eomc_controlmode_idle;  // use eOmc_controlmode_t. it is a readonly shadow copy of jconfig.controlmode used to remind the host of the current controlmode
        jointStatus->modes.ismotiondone         = eobool_false;  
    }
}

extern eObool_t eo_emsController_GetMotionDone(uint8_t joint)
{
    if (ems && ems->axis_controller[joint])
    {
        return eo_axisController_GetMotionDone(ems->axis_controller[joint]);
    }
    else
    {
        return eobool_false;
    }
}

///////////////////////
// PID configurations
extern void eo_emsController_SetMotorParams(uint8_t joint, eOmc_motor_params_t params)
{
    if (ems)
    {
        eo_axisController_SetBemf(ems->axis_controller[joint], params.bemf_value/(float)(1<<params.bemf_scale));
        eo_axisController_SetKtau(ems->axis_controller[joint], params.ktau_value/(float)(1<<params.ktau_scale));
    }
}

extern void eo_emsController_SetTcFilterType(uint8_t joint, uint8_t filterType)
{
    if (ems)
    {
        eo_axisController_SetTcFilterType(ems->axis_controller[joint], filterType);
    }
}

extern void eo_emsController_SetPosPid(uint8_t joint, float Kp, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff, float stiction_up, float stiction_down)
{
    if (ems)
    {
        eo_axisController_SetPosPid(ems->axis_controller[joint], Kp, Kd, Ki, Imax, Ymax, Yoff, stiction_up, stiction_down);
    }
}

extern void eo_emsController_SetTrqPid(uint8_t joint, float Kp, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff, float Kff, float stiction_up, float stiction_down)
{
    if (ems) eo_axisController_SetTrqPid(ems->axis_controller[joint], Kp, Kd, Ki, Imax, Ymax, Yoff, Kff, stiction_up, stiction_down);     
}   
// PID configurations
///////////////////////

extern void eo_emsController_SetImpedance(uint8_t joint, int32_t stiffness, int32_t damping, int32_t offset)
{
    if (ems) eo_axisController_SetImpedance(ems->axis_controller[joint], stiffness, damping, offset);
}

extern void eo_emsController_GetImpedance(uint8_t joint, int32_t *stiffness, int32_t *damping, int32_t *offset)
{
    if (ems) eo_axisController_GetImpedance(ems->axis_controller[joint], stiffness, damping, offset);
}

extern void eo_emsController_SetVelTimeout(uint8_t joint, int32_t vel_timeout)
{
    if (ems) eo_axisController_SetVelTimeout(ems->axis_controller[joint], vel_timeout);
}

extern void eo_emsController_SetLimits(uint8_t joint, int32_t pos_min, int32_t pos_max, int32_t vel_max)
{
    if (ems) eo_axisController_SetLimits(ems->axis_controller[joint], pos_min, pos_max, vel_max);
}

extern void eo_emsController_SetPosMin(uint8_t joint, int32_t pos_min)
{
    if (ems) eo_axisController_SetPosMin(ems->axis_controller[joint], pos_min);
}

extern void eo_emsController_SetPosMax(uint8_t joint, int32_t pos_max)
{
    if (ems) eo_axisController_SetPosMax(ems->axis_controller[joint], pos_max);
}

extern void eo_emsController_SetVelMax(uint8_t joint, int32_t vel_max)
{
    if (ems) eo_axisController_SetVelMax(ems->axis_controller[joint], vel_max);
}

extern void eo_emsController_SetGearboxRatio(uint8_t joint, int32_t gearboxratio)
{
    if (ems) ems->motor_config_gearbox_ratio[joint]=gearboxratio;
}

extern void eo_emsController_SetRotorEncoder(uint8_t joint, int32_t rotorencoder)
{
    if (ems) ems->motor_config_rotorencoder[joint]=rotorencoder;
}
extern void eo_emsController_SetRotorEncoderSign(uint8_t motor, int32_t sign)
{
    if (!ems) return;
    
    if (ems->act == emscontroller_actuation_LOCAL)
    {        
        if ( sign > 0) 
            ems->motor_position_sign[motor] =  1; 
        else if (sign < 0)
            ems->motor_position_sign[motor] = -1; 
        else 
            ems->motor_position_sign[motor] =  0;
    }
    //retro compatibility
    else
    {
        ems->motor_position_sign[motor] =  1; 
    }
    
    return;
}

extern void eo_emsController_ReadMotorstatus(uint8_t motor, uint8_t* state)
{
    if (ems)
    {
        eo_motor_set_motor_status(ems->motors, motor, state);
        
        ems->new_motor_msg[motor] = eobool_true;
    }
}

extern void eo_emsController_GetMotorStatus(uint8_t mId, eOmc_motor_status_t* motor_status)
{
    motor_status->basic.mot_position = ems->motor_position[mId];
    motor_status->basic.mot_velocity = ems->motor_velocity[mId];
    motor_status->basic.mot_acceleration = ems->motor_acceleration[mId];
    motor_status->basic.mot_current  = ems->motor_current [mId];
}

extern void eo_emsController_SetMotorConfig(uint8_t joint, eOmc_motor_config_t motorconfig)
{
    if (ems)
    {
      ems->motor_config_rotorencoder[joint]=motorconfig.rotorEncoderResolution; //@@@ checkme
      ems->motor_config_gearbox_ratio[joint]=motorconfig.gearboxratio;
      //placeholder filler01
      ems->motor_config_maxvelocityofmotor[joint]=motorconfig.maxvelocityofmotor;
      ems->motor_config_maxcurrentofmotor[joint]=motorconfig.maxcurrentofmotor;
      ems->motor_config_rotorIndexOffset[joint]=motorconfig.rotorIndexOffset;
      ems->motor_config_motorPoles[joint]=motorconfig.motorPoles;
      ems->motor_config_hasHallSensor[joint]=motorconfig.hasHallSensor;
      ems->motor_config_hasTempSensor[joint]=motorconfig.hasTempSensor;
      ems->motor_config_hasRotorEncoder[joint]=motorconfig.hasRotorEncoder;
      ems->motor_config_hasRotorEncoderIndex[joint]=motorconfig.hasRotorEncoderIndex;
        
      config_2FOC(joint);
    }
}

extern uint16_t eo_emsController_GetActuationLimit(void)
{
    if (!ems) return 0;
    
    //2FOC is internally limited
    if (ems->act == emscontroller_actuation_2FOC)
        //return PWM_OUTPUT_LIMIT;
        return PWM_OUTPUT_LIMIT_2FOC;
        //return 32000; //input limit of the the CAN command containing the PWM applied to motors
    //MC4plus, hw-limited to 3360
    else if (ems->act == emscontroller_actuation_LOCAL)
        return 3360;
    
    return 0;
}
extern void eo_emsMotorController_GoIdle(void)
{
    if (ems)
    {
        JOINTS(j)
        {
            eo_emsController_SetControlMode(j, eomc_controlmode_cmd_idle);
        }
        
        MOTORS(m) set_2FOC_idle(m);
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

__weak extern void eo_emsController_hid_userdef_config_motor(EOemsController* ctrl,uint8_t motor)
{

}

__weak extern void eo_emsController_hid_userdef_set_motor_idle(EOemsController* ctrl,uint8_t motor)
{

}

__weak extern void eo_emsController_hid_userdef_force_motor_idle(EOemsController* ctrl,uint8_t motor)
{

}

__weak extern void eo_emsController_hid_userdef_set_motor_running(EOemsController* ctrl,uint8_t motor)
{

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


void config_2FOC(uint8_t motor)
{
    eo_emsController_hid_userdef_config_motor(ems, motor);
}

void set_2FOC_idle(uint8_t motor)
{
    eo_emsController_hid_userdef_set_motor_idle(ems, motor);
}

void force_2FOC_idle(uint8_t motor)
{
    eo_emsController_hid_userdef_force_motor_idle(ems, motor);
}

void set_2FOC_running(uint8_t motor)
{
    eo_emsController_hid_userdef_set_motor_running(ems, motor);
    
}

//#define MOTOR_QENCODER_FAULT     0x00100000

static void s_eo_emsController_ResetCalibrationCoupledJoints(uint8_t joint)
{
    uint8_t i;
    
    //check coupled joints
    if((emscontroller_board_SHOULDER == ems->board) 
    || (emscontroller_board_WAIST == ems->board) 
    || (ems->board == emscontroller_board_CER_WAIST))   
    {
      if (joint < 3)
      {
          for(i = 0; i < 3; i++)
          {
              if ((joint != i) && (eo_axisController_IsCalibrated(ems->axis_controller[i])))
              {
                  eo_emsController_ResetCalibrationValues(i);
              }   
          }
      }
    }
    else if(emscontroller_board_HEAD_neckpitch_neckroll == ems->board)  
    {
      for(i = 0; i < 2; i++)
      {
          if ((joint != i) && (eo_axisController_IsCalibrated(ems->axis_controller[i])))
          {
               eo_emsController_ResetCalibrationValues(i);
          }   
      }
    }
    else if(emscontroller_board_HEAD_neckyaw_eyes == ems->board) 
    {
      if ((joint == 2) || (joint == 3))
      {
          for(i = 2; i < 4; i++)
          {
              if ((joint != i) && (eo_axisController_IsCalibrated(ems->axis_controller[i])))
              {
                  eo_emsController_ResetCalibrationValues(i);
              }   
          }
      }
    }
    return;
}

static void s_eo_emsController_InitTrajectoryVersionVergence(uint8_t j)
{
    int32_t axis_pos2 = eo_axisController_GetAxisPos(ems->axis_controller[2]);
    int32_t axis_pos3 = eo_axisController_GetAxisPos(ems->axis_controller[3]);
    
    //version --> (pos3 + pos2)/2
    if (j == 2)   
    {
         eo_trajectory_Init(eo_axisController_GetTraj (ems->axis_controller[2]), (axis_pos3 + axis_pos2)/2, eo_axisController_GetAxisVel(ems->axis_controller[2]), 0);
         eo_trajectory_Stop(eo_axisController_GetTraj (ems->axis_controller[2]), (axis_pos3 + axis_pos2)/2);
         //set reference point to mantain --> it's the actual position! this seems to resolve the issue related to the eyes calibration
         eo_axisController_SetPosRef(ems->axis_controller[2],  (axis_pos3 + axis_pos2)/2,  eo_axisController_GetAxisVel(ems->axis_controller[2]));
         return;
    }
    
    //vergence --> (pos3 - pos2)/2
    if (j == 3)   
    {
         eo_trajectory_Init(eo_axisController_GetTraj (ems->axis_controller[3]), (axis_pos3 - axis_pos2)/2, eo_axisController_GetAxisVel(ems->axis_controller[3]), 0);
         eo_trajectory_Stop(eo_axisController_GetTraj (ems->axis_controller[3]), (axis_pos3 - axis_pos2)/2); // /2 or not?
         //set reference point to mantain --> it's the actual position! this seems to resolve the issue related to the eyes calibration
         eo_axisController_SetPosRef(ems->axis_controller[3], (axis_pos3 - axis_pos2)/2, eo_axisController_GetAxisVel(ems->axis_controller[3]));
         return;
    }
    
    #warning does it worth to put the virtually coupled joints in 0? it's a choice, discuss it 
}


/*
    BYTE 0

        b0  unsigned UnderVoltageFailure:1;      Undervoltage       
        b1  unsigned OverVoltageFailure:1;       Overvoltage
        b2  unsigned ExternalFaultAsserted:1;    External Fault
        b3  unsigned OverCurrentFailure:1;       OverCurrent
        b4  unsigned DHESInvalidValue:1;         an invalid value of HES has been detected
        b5  unsigned AS5045CSumError:1;          //ATTENTION IN MSG PUNT IN OR WITH    AS5045CalcError:1;
        b6  unsigned DHESInvalidSequence:1;      an invalid sequence of HES activation has been detected
        b7  unsigned CANInvalidProtocol:1;       can protocol incompatible with EMS

    BYTE 1

        b0  unsigned CAN_BufferOverRun:1;        A CAN fifo full event has happened. 
                                                 This should never happen because this FW on the 2FOC should 
                                                 be able to handle CAN with full load. This might happen in 
                                                 certain blocking operation are requested like save to eeprom 
                                                 parameters. TODO: verify it.
        b1  unsigned unused1:1;                  UNUSED: put here for mad in msg
        b2  unsigned CAN_TXIsPasv:1;             can IS is TX passive mode
        b3  unsigned CAN_RXIsPasv:1;             can IS in RX passive mode
        b4  unsigned CAN_IsWarnTX:1;             can IS in bus warn in tx (exist only one error for rx and tx, so the bits are used together)
        b5  unsigned CAN_IsWarnRX:1;             can IS in bus warn in rx
        b6  unsigned unused3:1;                  UNUSED: put here for mad in msg
        b7  unsigned unused4:1;                  UNUSED: put here for mad in msg

    BYTE 2
    
        b0  unsigned ADCCalFailure:1;            ADC calibration failure 
        b1  unsigned I2TFailure:1;               I2T protection                     
        b2  unsigned EMUROMFault:1;              EMUROM Fault
        b3  unsigned EMUROMCRCFault:1;           EMUROM CRC Fault
        b4  unsigned EncoderFault:1;             Encoder Fault. 1 when homing with zero signal fail 
        b5  unsigned FirmwareSPITimingError:1;   SPI reading has been interrupted before finishing by foc loop	
        b6  unsigned AS5045CalcError:1;
        b7  unsigned FirmwarePWMFatalError:1;    This is true when the FOC loop tried to delay the PWM update (see below)
                                                 but the wait for the PWM counter timed out (PWM counter freezed?)
                                                 This should never happen, and that may indicate
                                                 a firmware BUG or abnormal firmware behaviour due to
                                                 unpredictable and exotic reasons (malfunctions, voodoo
                                                 magic, hardware bugs, gravity inversions, alien invasions, 
                                                 end of the world).
                                                 In any case please consider that this is certainly NOT due
                                                 to the firmware developer, but more likely it's electronic 
                                                 eng. full responsibility :-)

    BYTE 3
    
        b0  unsigned CAN_TXWasPasv:1;           can has been in TX passive mode at least one time.
        b1  unsigned CAN_RXWasPasv:1;           can has been in RX passive
        b2  unsigned CAN_RTRFlagActive:1;       an RTR flag has been seen over the wire. This is not OK for LorCan
        b3  unsigned CAN_WasWarn:1;             can has been in bus warn at least one time
        b4  unsigned CAN_DLCError:1;            at least one DLC error has been seen
        b5  unsigned SiliconRevisionFault:1;    see comments below
        b6  unsigned PositionLimitUpper:1; 
        b7  unsigned PositionLimitLower:1; 
*/

void sendErrorMessage(uint8_t j, uint32_t ems_fault_mask_j, uint32_t motor_fault_mask_j)
{
    //eObool_t managed = eobool_false;
    uint64_t par64 = (((uint64_t)ems_fault_mask_j)<<32)|(uint64_t)motor_fault_mask_j;
     
    // 2FOC ERRORS
    if (motor_fault_mask_j & MOTOR_EXTERNAL_FAULT)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j;
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; 
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_external_fault);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &descriptor);
        motor_fault_mask_j &= ~MOTOR_EXTERNAL_FAULT;
        ems_fault_mask_j &= ~MOTOR_EXTERNAL_FAULT;
    }
        
    if (motor_fault_mask_j & MOTOR_OVERCURRENT_FAULT)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_overcurrent);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        motor_fault_mask_j &= ~MOTOR_OVERCURRENT_FAULT;
    }
    
    if (motor_fault_mask_j & MOTOR_I2T_LIMIT_FAULT)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_i2t_limit);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        motor_fault_mask_j &= ~MOTOR_I2T_LIMIT_FAULT;
    }
    
    if (motor_fault_mask_j & MOTOR_HALLSENSORS_FAULT)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_hallsensors);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        motor_fault_mask_j &= ~MOTOR_HALLSENSORS_FAULT;
    }
    
    if (motor_fault_mask_j & MOTOR_QENCODER_FAULT)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = eo_motors_getQEError(ems->motors, j);
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_qencoder);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        motor_fault_mask_j &= ~MOTOR_QENCODER_FAULT;
    }
    
    if (motor_fault_mask_j & MOTOR_CAN_GENERIC_FAULT)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_can_generic);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        motor_fault_mask_j &= ~MOTOR_CAN_GENERIC_FAULT;
    }
    
    // BOARD ERRORS
    
    if (ems_fault_mask_j & MOTOR_CAN_NOT_RESPONDING)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_can_no_answer);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        ems_fault_mask_j &= ~MOTOR_CAN_NOT_RESPONDING;
    }
    
    if (ems_fault_mask_j & MOTOR_CAN_INVALID_PROT)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_can_invalid_prot);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        ems_fault_mask_j &= ~MOTOR_CAN_INVALID_PROT;
    }
    
    if (ems_fault_mask_j & AXIS_TORQUE_SENS_FAULT)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_axis_torque_sens);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        ems_fault_mask_j &= ~AXIS_TORQUE_SENS_FAULT;
    }
    
    if (ems_fault_mask_j & AEA_ABS_ENC_INVALID_DATA)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_aea_abs_enc_invalid);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        ems_fault_mask_j &= ~AEA_ABS_ENC_INVALID_DATA;
    }
    
    if (ems_fault_mask_j & AEA_ABS_ENC_TIMEOUT)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_aea_abs_enc_timeout);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        ems_fault_mask_j &= ~AEA_ABS_ENC_TIMEOUT;
    }
    
    if (ems_fault_mask_j & MOTOR_WRONG_STATE)
    {
        //managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        uint8_t state; 
        uint8_t state_req;
        eo_motor_get_motor_status(ems->motors, j, &state, &state_req);
        descriptor.par64 = ((((uint64_t)MOTOR_WRONG_STATE))<<32)|(((uint64_t)state_req)<<8)|((uint64_t)state);
        descriptor.sourcedevice = 1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_generic_error);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
        ems_fault_mask_j &= ~MOTOR_WRONG_STATE;
    }
    
    if ((ems_fault_mask_j & ~MOTOR_HARD_FAULT) || motor_fault_mask_j)
    {
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_generic_error);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
}

#if 0
#ifdef EXPERIMENTAL_MOTOR_TORQUE
    #if defined(SHOULDER_BOARD)
        //         |    1       0       0   |
        // J^-1  = | -65/40   65/40     0   |
        //         | -65/40   65/40   65/40 |
    
        //         | 1  -65/40  -65/40 |
        // Jt^-1 = | 0   65/40   65/40 |
        //         | 0     0     65/40 |
    
        int16_t axle_trq_2 = (65*ems->motor_current[2])/40;
        int16_t axle_trq_1 = axle_trq_2 + (65*ems->motor_current[1])/40;
    
        eo_axisController_SetTorque(ems->axis_controller[0], ems->motor_current[0] - axle_trq_1);
        eo_axisController_SetTorque(ems->axis_controller[1], axle_trq_1);
        eo_axisController_SetTorque(ems->axis_controller[2], axle_trq_2);
        eo_axisController_SetTorque(ems->axis_controller[3], ems->motor_current[3]);
    
    #elif defined(WAIST_BOARD)
        //         | 1/2  -1/2    0   |
        // J^-1 =  | 1/2   1/2    0   |
        //         | -1     0   80/44 |

        //         |  1/2  1/2   -1   |
        // Jt^-1 = | -1/2  1/2    0   |
        //         |   0    0   80/44 |
        
        eo_axisController_SetTorque(ems->axis_controller[0], (ems->motor_current[0]+ems->motor_current[1])/2 - ems->motor_current[2]);
        eo_axisController_SetTorque(ems->axis_controller[1], (ems->motor_current[1]-ems->motor_current[0])/2);
        eo_axisController_SetTorque(ems->axis_controller[2], (80*ems->motor_current[2])/44);
        
    #elif defined(UPPERLEG_BOARD)
    
        eo_axisController_SetTorque(ems->axis_controller[0], (75*ems->motor_current[0])/50);
        eo_axisController_SetTorque(ems->axis_controller[1], ems->motor_current[1]);
        eo_axisController_SetTorque(ems->axis_controller[2], ems->motor_current[2]);
        eo_axisController_SetTorque(ems->axis_controller[3], ems->motor_current[3]);
    
    #elif defined(ANKLE_BOARD)
    
        eo_axisController_SetTorque(ems->axis_controller[0], ems->motor_current[0]);
        eo_axisController_SetTorque(ems->axis_controller[1], ems->motor_current[1]);
    
    #else
        #error undefined board type
    #endif
#endif
#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





