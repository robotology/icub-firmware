
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

#if !defined(V1_MECHANICS) && !defined(V2_MECHANICS)
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

#define MOTOR_HARD_FAULT         0x0001
#define MOTOR_CAN_NOT_RESPONDING 0x0080
#define MOTOR_WRONG_STATE        0x0002

#define AXIS_TORQUE_SENS_FAULT   0x0100
#define AEA_ABS_ENC_INVALID_DATA 0x4000
#define AEA_ABS_ENC_TIMEOUT      0x8000

#define MOTOR_EXTERNAL_FAULT     0x00000004
#define MOTOR_OVERCURRENT_FAULT  0x00000008
#define MOTOR_I2T_LIMIT_FAULT    0x00020000
#define MOTOR_HALLSENSORS_FAULT  0x00000040
#define MOTOR_QENCODER_FAULT     0x00100000
#define MOTOR_CAN_INVALID_PROT   0x00000080
#define MOTOR_CAN_GENERIC_FAULT  0x00003D00

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
        ems->motor_config_gearbox_ratio[j]  = 1;
        ems->motor_config_rotorencoder[j]  = 1;
        ems->motor_config_hasHallSensor[j] = eobool_false;
        ems->motor_config_hasRotorEncoder[j] = eobool_false;
        ems->motor_config_hasTempSensor[j] = eobool_false;
        ems->motor_config_maxcurrentofmotor[j] = 0;
        ems->motor_config_maxvelocityofmotor[j] = 0;
        ems->motor_config_motorPoles[j] = 0;
        ems->motor_config_rotorIndexOffset[j] = 0;
    }
        
    ems->motors = eo_motors_New(ems->naxles, ems->board);

    return ems;
}

extern void eo_emsController_set_Jacobian(int32_t **Ji32)
{
    eo_motors_set_Jacobian(ems->motors, Ji32);
}

extern void eo_emsController_SetAbsEncoderSign(uint8_t joint, int32_t sign)
{
    eo_absCalibratedEncoder_SetSign(ems->abs_calib_encoder[joint], sign);
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
    //if (ems->rotorencoder[motor]<0)
    //{
    //   ems->motor_velocity[motor]     = - ems->motor_velocity[motor];
    //   ems->motor_velocity_gbx[motor] = - ems->motor_velocity_gbx[motor];
    //   ems->motor_position[motor]     = - ems->motor_position[motor];
    //}
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
        
        if (eo_absCalibratedEncoder_IsHardFault(ems->abs_calib_encoder[j]))
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
        
        if (bNoFaultState)
        {
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
                if (j<3 && ((ems->board == emscontroller_board_SHOULDER) || (ems->board == emscontroller_board_WAIST)))
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
    
    #if defined(USE_JACOBIAN)
    
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
        else if (ems->board == emscontroller_board_ANKLE)
        {
            axle_virt_vel[0] = ems->motor_velocity_gbx[0];
            axle_virt_vel[1] = ems->motor_velocity_gbx[1];
        
            axle_virt_pos[0] = ems->motor_position[0];
            axle_virt_pos[1] = ems->motor_position[1];
        }
    
#endif //USE_2FOC_FAST_ENCODER
    
    JOINTS(j)
    {
        #ifdef USE_2FOC_FAST_ENCODER
            eo_axleVirtualEncoder_Acquire(ems->motor_config_gearbox_ratio[j], ems->axle_virt_encoder[j], axle_abs_pos[j], axle_virt_pos[j], axle_virt_vel[j]);
            eo_axisController_SetEncPos(ems->axis_controller[j], eo_axleVirtualEncoder_GetPos(ems->axle_virt_encoder[j]));
            eo_axisController_SetEncVel(ems->axis_controller[j], eo_axleVirtualEncoder_GetVel(ems->axle_virt_encoder[j]));
        #else
            eo_axisController_SetEncPos(ems->axis_controller[j], axle_abs_pos[j]);
            eo_axisController_SetEncVel(ems->axis_controller[j], axle_abs_vel[j]);
        #endif
    }
    
    if (ems->n_calibrated != ems->naxles)
    {
        eo_emsController_CheckCalibrations();
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
    
    eo_motors_check_wdog(ems->motors);
    
    //PWM computation (PID ecc)
    JOINTS(j)
    {
        torque_protection[j] = eobool_false;
        pwm_joint[j] = eo_axisController_PWM(ems->axis_controller[j], &stiffness[j]);
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
            float pwm_lim = eo_pid_PWM_p( ems->axis_controller[j]->pidP, ems->axis_controller[j]->pos_min - pos);
            if ((pwm_lim > 0) ^ (pwm_joint[j] > pwm_lim)) 
            {
               pwm_joint[j] = pwm_lim;
               for (int8_t k; k<ems->naxles; k++)
                   torque_protection[k] = eobool_true; //put in protection all motors
            }
        }
        else if (pos >  ems->axis_controller[j]->pos_max)
        {
            float pwm_lim = eo_pid_PWM_p( ems->axis_controller[j]->pidP,  ems->axis_controller[j]->pos_max - pos);
            if ((pwm_lim > 0) ^ (pwm_joint[j] > pwm_lim))
            {
               pwm_joint[j] = pwm_lim;
               for (int8_t k; k<ems->naxles; k++)
                   torque_protection[k] = eobool_true; //put in protection all motors
            }
        }
      }
    }

    //PWM decoupling
    float pwm_motor[MAX_NAXLES];
    eo_motors_decouple_PWM(ems->motors, pwm_joint, pwm_motor, stiffness);
    
    //Friction compensation after joints decoupling
    MOTORS(m)
    {
        if (!torque_protection[m])
            pwm_motor[m] = eo_axisController_FrictionCompensation(ems->axis_controller[m],pwm_motor[m],ems->motor_velocity[m]);
    }
    
    MOTORS(m) LIMIT(pwm_motor[m], NOMINAL_CURRENT);
    
    //save the pwm data after the decoupling
    MOTORS(m)  ems->axis_controller[m]->controller_output=pwm_motor[m];
    
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
    else if (ems->board == emscontroller_board_UPPERLEG)
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
          *torque_motor=ems->axis_controller[joint_id]->torque_meas_jnt;
    }
    #endif  
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
    else if (ems->board == emscontroller_board_UPPERLEG)
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
        *torque_motor=ems->axis_controller[joint_id]->torque_ref_jnt;
    }
}

extern void eo_emsController_SetControlModeGroupJoints(uint8_t joint, eOmc_controlmode_command_t mode)
{
    eo_emsController_SetControlMode(joint, mode);
}

extern eObool_t eo_emsController_SetInteractionModeGroupJoints(uint8_t joint, eOmc_interactionmode_t mode)
{
    if (joint<3 && (ems->board == emscontroller_board_SHOULDER || ems->board == emscontroller_board_WAIST)) 
    {
        eo_emsController_SetInteractionMode(0, mode);
        eo_emsController_SetInteractionMode(1, mode);
        eo_emsController_SetInteractionMode(2, mode);
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
    
    static eObool_t motors_not_configured = eobool_true;
    
    if (motors_not_configured)
    {
        motors_not_configured = eobool_false;
        MOTORS(m) config_2FOC(m);   
    }
    
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
        if (joint<3 && ((ems->board == emscontroller_board_SHOULDER) || (ems->board == emscontroller_board_WAIST)))
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
        if (joint<3 && ((ems->board == emscontroller_board_SHOULDER) || (ems->board == emscontroller_board_WAIST)))
        {
            set_2FOC_idle(0);
            set_2FOC_idle(1);
            set_2FOC_idle(2); 
            
            eo_axisController_SetControlMode(ems->axis_controller[0], eomc_controlmode_cmd_idle);
            eo_axisController_SetControlMode(ems->axis_controller[1], eomc_controlmode_cmd_idle);
            eo_axisController_SetControlMode(ems->axis_controller[2], eomc_controlmode_cmd_idle);
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
        
        if (joint<3 && ((ems->board == emscontroller_board_SHOULDER) || (ems->board == emscontroller_board_WAIST)))
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

extern void eo_emsController_StartCalibration(uint8_t joint, int32_t pos, int32_t vel, int32_t offset)
{
    if (!ems) return;
    
    ems->n_calibrated = 0;
        
    eo_absCalibratedEncoder_Calibrate(ems->abs_calib_encoder[joint], offset);
    
    eo_axisController_StartCalibration(ems->axis_controller[joint]);
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
            eo_axisController_SetCalibrated(ems->axis_controller[j]);
            ems->n_calibrated++;
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
                ems->n_calibrated++;
                eo_axisController_SetCalibrated(ems->axis_controller[j]);
            }
        }
    }
    else if (ems->board == emscontroller_board_SHOULDER || ems->board == emscontroller_board_WAIST)
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
            ems->n_calibrated+=3;
            eo_axisController_SetCalibrated(ems->axis_controller[0]);
            eo_axisController_SetCalibrated(ems->axis_controller[1]);
            eo_axisController_SetCalibrated(ems->axis_controller[2]);
        }
  
        if (ems->board == emscontroller_board_SHOULDER)
        {
            if (eo_axisController_IsCalibrated(ems->axis_controller[3]))
            {
                ems->n_calibrated++;
            }
            else if (eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[3]))
            {    
                ems->n_calibrated++;
                eo_axisController_SetCalibrated(ems->axis_controller[3]);
            }
        }
    }
    #endif // ! USE_JACOBIAN        
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
        pidStatus->error     = 0;
        pidStatus->output    = 0;
        pidStatus->positionreference = 0;
        pidStatus->torquereference = 0;
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
        jointStatus->basic.jnt_position        = 0;  // the position of the joint           
        jointStatus->basic.jnt_velocity        = 0;  // the velocity of the joint          
        jointStatus->basic.jnt_acceleration    = 0;  // the acceleration of the joint       
        jointStatus->basic.jnt_torque          = 0;  // the torque of the joint when locally measured
        jointStatus->basic.motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_notmonitored;  // use eOmc_motionmonitorstatus_t. it is eomc_motionmonitorstatus_notmonitored unless the monitor is activated in jconfig.motionmonitormode  
        jointStatus->basic.controlmodestatus   = eomc_controlmode_idle;  // use eOmc_controlmode_t. it is a readonly shadow copy of jconfig.controlmode used to remind the host of teh current controlmode
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

extern void eo_emsController_ReadMotorstatus(uint8_t motor, uint8_t* state)
{
    if (ems) eo_motor_set_motor_status(ems->motors, motor, state);
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
      ems->motor_config_rotorencoder[joint]=motorconfig.rotorencoder;
      ems->motor_config_gearbox_ratio[joint]=motorconfig.gearboxratio;
      //placeholder filler01
      ems->motor_config_maxvelocityofmotor[joint]=motorconfig.maxvelocityofmotor;
      ems->motor_config_maxcurrentofmotor[joint]=motorconfig.maxcurrentofmotor;
      ems->motor_config_rotorIndexOffset[joint]=motorconfig.rotorIndexOffset;
      ems->motor_config_motorPoles[joint]=motorconfig.motorPoles;
      ems->motor_config_hasHallSensor[joint]=motorconfig.hasHallSensor;
      ems->motor_config_hasTempSensor[joint]=motorconfig.hasTempSensor;
      ems->motor_config_hasRotorEncoder[joint]=motorconfig.hasRotorEncoder;
    }
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




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


void config_2FOC(uint8_t motor)
{
    if (ems->act != emscontroller_actuation_2FOC) return;
    
    //eOmc_i2tParams_t i2t;
    //icubCanProto_current_t max_current;
    
    int8_t KpKiKdKs[7];
    ((int16_t*)KpKiKdKs)[0] =  8; //Kp
    ((int16_t*)KpKiKdKs)[1] =  2; //Ki
    ((int16_t*)KpKiKdKs)[2] =  0; //Kd (unused in 2FOC)
               KpKiKdKs [6] = 10; // shift
    
    uint32_t max_current = 5000; // 5A
    
    #define HAS_QE      0x0001
    #define HAS_HALL    0x0002
    #define HAS_TSENS   0x0004

    uint8_t motor_config[6];
    motor_config[0] = HAS_QE|HAS_HALL;
    *(int16_t*)(motor_config+1) = -14400;
    *(int16_t*)(motor_config+3) = 155; // offset (degrees)
    motor_config[5] = 4; // num motor poles
    
    #if defined(USE_CANCOMM_V2)

    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;

    command.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    command.value = KpKiKdKs;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    
    command.type = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    command.value = &max_current;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);

    command.type = ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG;
    command.value = motor_config;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);      

    #else
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eOappTheDB_jointOrMotorCanLocation_t canLoc;
    eOicubCanProto_msgDestination_t msgdest;

    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };
    
    eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), motor,  &canLoc, NULL);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
     
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, KpKiKdKs);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &max_current);

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_MOTOR_CONFIG;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, motor_config);

    #endif
    
    eo_motors_new_state_req(ems->motors, motor, icubCanProto_controlmode_idle);
}

void set_2FOC_idle(uint8_t motor)
{
    if (ems->act != emscontroller_actuation_2FOC) return;
    
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_idle;

    #if defined(USE_CANCOMM_V2)
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &controlmode_2foc;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    #else
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle()); 
    eOappTheDB_jointOrMotorCanLocation_t canLoc;
    eOicubCanProto_msgDestination_t msgdest;
    
    eOicubCanProto_msgCommand_t msgCmd =
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };
    
    eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), motor,  &canLoc, NULL);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE; //+
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &controlmode_2foc);
    #endif
    
    eo_motors_new_state_req(ems->motors, motor, icubCanProto_controlmode_idle);
    
    // no need
    //eo_motor_set_motor_status(ems->motors, motor, 0);
}

void force_2FOC_idle(uint8_t motor)
{
    if (ems->act != emscontroller_actuation_2FOC) return;
    
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_forceIdle;
    
    #if defined(USE_CANCOMM_V2)
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &controlmode_2foc;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    #else
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle()); 
    eOappTheDB_jointOrMotorCanLocation_t canLoc;
    eOicubCanProto_msgDestination_t msgdest;
    
    eOicubCanProto_msgCommand_t msgCmd =
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };
    
    eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), motor,  &canLoc, NULL);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE; //+
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &controlmode_2foc); //+
    #endif
    
    eo_motors_new_state_req(ems->motors, motor, icubCanProto_controlmode_idle);
    
    // no need
    //eo_motor_set_motor_status(ems->motors, motor, 0);
}

void set_2FOC_running(uint8_t motor)
{
    if (ems->act != emscontroller_actuation_2FOC) return;
    
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_openloop;
    
    #ifdef EXPERIMENTAL_SPEED_CONTROL
    controlmode_2foc = icubCanProto_controlmode_velocity;
    #endif
    
    #if defined(USE_CANCOMM_V2)
    eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, motor, 0);
    eOcanprot_command_t command = {0};
    command.class = eocanprot_msgclass_pollingMotorControl;
    command.type  = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    command.value = &controlmode_2foc;
    eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);
    #else
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eOappTheDB_jointOrMotorCanLocation_t canLoc;
    eOicubCanProto_msgDestination_t msgdest;
    
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };
    
    eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), motor,  &canLoc, NULL);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
   
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &controlmode_2foc);
    #endif

    eo_motors_new_state_req(ems->motors, motor, icubCanProto_controlmode_openloop);
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

    /*
    //@@@@MR: use this block for nice print debugging
    static int count =0;
    if (count == 1000)
    {
    eOerrmanDescriptor_t errdes = {0};
    errdes.code                 = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
    errdes.par16                = 0;
    errdes.par64                = 0;
    errdes.sourcedevice         = eo_errman_sourcedevice_localboard;
    errdes.sourceaddress        = 0;  
    //char *str = NULL;
    char str[eomn_info_status_extra_sizeof] = {0};
    snprintf(str, sizeof(str), "I%f T%f F%f R%f", pwm_input, o->Ktau, o->Kff, Tr);             
    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, str, NULL, &errdes);
    count =0;
    }
    count++;
    */
    
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


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
    eObool_t managed = eobool_false;
    uint64_t par64 = (((uint64_t)ems_fault_mask_j)<<32)|(uint64_t)motor_fault_mask_j;
     
    // 2FOC ERRORS
    if (motor_fault_mask_j & MOTOR_EXTERNAL_FAULT)
    {
        managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j;
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; 
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_external_fault);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &descriptor);
    }
        
    if (motor_fault_mask_j & MOTOR_OVERCURRENT_FAULT)
    {
        managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_overcurrent);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
    
    if (motor_fault_mask_j & MOTOR_I2T_LIMIT_FAULT)
    {
        managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_i2t_limit);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
    
    if (motor_fault_mask_j & MOTOR_HALLSENSORS_FAULT)
    {
        managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_hallsensors);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
    
    if (motor_fault_mask_j & MOTOR_QENCODER_FAULT)
    {
        managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_qencoder);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
    
    if (motor_fault_mask_j & MOTOR_CAN_GENERIC_FAULT)
    {
        managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_can_generic);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
    
    // BOARD ERRORS
    
    if (ems_fault_mask_j & MOTOR_CAN_NOT_RESPONDING)
    {
        managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_can_no_answer);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
    
    if (ems_fault_mask_j & MOTOR_CAN_INVALID_PROT)
    {
        managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_can_invalid_prot);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
    
    if (ems_fault_mask_j & AXIS_TORQUE_SENS_FAULT)
    {
        managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_axis_torque_sens);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
    
    if (ems_fault_mask_j & AEA_ABS_ENC_INVALID_DATA)
    {
        managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_aea_abs_enc_invalid);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
    
    if (ems_fault_mask_j & AEA_ABS_ENC_TIMEOUT)
    {
        managed = eobool_true;
        eOerrmanDescriptor_t descriptor = {0};
        descriptor.par16 = j; // unless required
        descriptor.par64 = par64;
        descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
        descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
        descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_aea_abs_enc_timeout);
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
    }
    
    if (!managed)
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







