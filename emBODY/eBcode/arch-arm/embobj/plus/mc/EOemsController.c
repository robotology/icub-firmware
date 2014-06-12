
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
//#include "hal_led.h"

//#include <string.h>


#include "EOicubCanProto_specifications.h"
#include "EOtheEMSapplBody.h"

//#include "EOMtheEMSbackdoor.h"
//#include "OPCprotocolManager_Cfg.h"

#if !defined(V1_MECHANICS) && !defined(V2_MECHANICS)
#error mechanics is undefined
#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOemsController.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOemsController_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define SCAN(s)     for (uint8_t s=0; s<NAXLES; ++s)
#define JOINTS(j)   SCAN(j)
#define MOTORS(m)   SCAN(m)
#define ENCODERS(e) SCAN(e)

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
void set_2FOC_running(uint8_t joint, eOmc_controlmode_command_t mode);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOemsController *ems = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOemsController* eo_emsController_Init() 
{    
#ifndef NO_2FOC_BOARD
    ems = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOemsController), 1);
#endif
    
    if (ems)
    {        
        ems->n_calibrated = 0;
        ems->state_mask = EMS_OK;
        
        JOINTS(j)
        {
            ems->axis_controller[j] = eo_axisController_New(j);
            ems->abs_calib_encoder[j] = eo_absCalibratedEncoder_New();
            ems->axle_virt_encoder[j] = eo_axleVirtualEncoder_New();
            
            ems->motor_current [j] = 0;
            ems->motor_velocity[j] = 0;
            ems->motor_position[j] = 0;
        }
        
        ems->motors = eo_motors_New(NAXLES);
    }
    
    return ems;
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

#ifdef USE_2FOC_FAST_ENCODER
extern void eo_emsController_AcquireMotorEncoder(uint8_t motor, int16_t current, int32_t velocity, int32_t position)
{
    ems->motor_current [motor] = current;
    ems->motor_velocity[motor] = velocity;
    ems->motor_position[motor] = position;
}
#endif

extern void eo_emsController_AcquireAbsEncoders(int32_t *abs_enc_pos)
{
    static const int16_t FOC_2_EMS_SPEED = 1000/GEARBOX_REDUCTION;
    
    int32_t axle_abs_pos[NAXLES];
    
    ENCODERS(e)
    {
        axle_abs_pos[e] = eo_absCalibratedEncoder_Acquire(ems->abs_calib_encoder[e], abs_enc_pos[e]);
        
        //if (eo_absCalibratedEncoder_IsHardFault(ems->abs_calib_encoder[e])
        //{
        //    SET_BITS(ems->state_mask,EMS_FAULT);
        //}
    }
    
    #if defined(SHOULDER_BOARD) && defined(V1_MECHANICS)
        // |J0|   |  1     0    0   |   |E0|     
		// |J1| = |  0     1    0   | * |E1|
		// |J2|   |  1    -1  40/65 |   |E2|
        axle_abs_pos[2] = axle_abs_pos[0]-axle_abs_pos[1]+(40*axle_abs_pos[2])/65;
    #endif
    
#ifdef USE_2FOC_FAST_ENCODER
    
    int16_t axle_virt_vel[NAXLES];
    int32_t axle_virt_pos[NAXLES];
    
    #if defined(SHOULDER_BOARD)
    
        //             | 1     0       0    0 |
        // J = dq/dm = | 1   40/65     0    0 |
        //             | 0  -40/65   40/65  0 |
        //             | 0     0       0    1 |
    
        axle_virt_vel[0] = FOC_2_EMS_SPEED*ems->motor_velocity[0];
        axle_virt_vel[1] = FOC_2_EMS_SPEED*(ems->motor_velocity[0]+(40*ems->motor_velocity[1])/65);
        axle_virt_vel[2] = FOC_2_EMS_SPEED*((40*(ems->motor_velocity[2]-ems->motor_velocity[1]))/65);
        axle_virt_vel[3] = FOC_2_EMS_SPEED*ems->motor_velocity[3];
    
        axle_virt_pos[0] = ems->motor_position[0];
        axle_virt_pos[1] = ems->motor_position[0]+(40*ems->motor_position[1])/65;
        axle_virt_pos[2] = (40*(ems->motor_position[2]-ems->motor_position[1]))/65;
        axle_virt_pos[3] = ems->motor_position[3];
    
    #elif defined(WAIST_BOARD)
    
        //             |   1     1     0   |
        // J = dq/dm = |  -1     1     0   |
        //             | 44/80 44/80 44/80 | 

        axle_virt_vel[0] = FOC_2_EMS_SPEED*(     ems->motor_velocity[0] + ems->motor_velocity[1]);
        axle_virt_vel[1] = FOC_2_EMS_SPEED*(    -ems->motor_velocity[0] + ems->motor_velocity[1]);
        axle_virt_vel[2] = FOC_2_EMS_SPEED*((44*(ems->motor_velocity[0] + ems->motor_velocity[1] + ems->motor_velocity[2]))/80);
        
        axle_virt_pos[0] =      ems->motor_position[0] + ems->motor_position[1];
        axle_virt_pos[1] =     -ems->motor_position[0] + ems->motor_position[1];
        axle_virt_pos[2] = (44*(ems->motor_velocity[0] + ems->motor_velocity[1] + ems->motor_velocity[2]))/80;
        
    #elif defined(UPPERLEG_BOARD)
    
        axle_virt_vel[0] = FOC_2_EMS_SPEED*(50*ems->motor_velocity[0])/75;
        axle_virt_vel[1] = FOC_2_EMS_SPEED*    ems->motor_velocity[1]    ;
        axle_virt_vel[2] = FOC_2_EMS_SPEED*    ems->motor_velocity[2]    ;
        axle_virt_vel[3] = FOC_2_EMS_SPEED*    ems->motor_velocity[3]    ;
        
        axle_virt_pos[0] = (50*ems->motor_position[0])/75;
        axle_virt_pos[1] =     ems->motor_position[1]    ;
        axle_virt_pos[2] =     ems->motor_position[2]    ;
        axle_virt_pos[3] =     ems->motor_position[3]    ;
        
    #elif defined(ANKLE_BOARD)
        
        axle_virt_vel[0] = FOC_2_EMS_SPEED*ems->motor_velocity[0];
        axle_virt_vel[1] = FOC_2_EMS_SPEED*ems->motor_velocity[1];
        
        axle_virt_pos[0] = ems->motor_position[0];
        axle_virt_pos[1] = ems->motor_position[1];
        
    #else
        //#error undefined board type
    #endif
    
#endif
    
    JOINTS(j)
    {
        #ifdef USE_2FOC_FAST_ENCODER
            eo_axleVirtualEncoder_Acquire(ems->axle_virt_encoder[j], axle_abs_pos[j], axle_virt_pos[j], axle_virt_vel[j]);
            eo_axisController_SetEncPos(ems->axis_controller[j], eo_axleVirtualEncoder_GetPos(ems->axle_virt_encoder[j]));
            eo_axisController_SetEncVel(ems->axis_controller[j], eo_axleVirtualEncoder_GetVel(ems->axle_virt_encoder[j]));
        #else
            eo_axisController_SetEncPos(ems->axis_controller[j], axle_abs_pos[j]);
        #endif
    }
    
    if (ems->n_calibrated != NAXLES)
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

extern void eo_emsController_PWM(int16_t* pwm_motor)
{
    if ((!ems) || (ems->state_mask != EMS_OK) /*|| (ems->n_calibrated != NAXLES)*/)
    {
        MOTORS(m) pwm_motor[m] = 0;
        return;
    }
    
    ENCODERS(e)
    {
        if (eo_absCalibratedEncoder_IsHardFault(ems->abs_calib_encoder[e]))
        {
            //o->state_mask |= ...
            MOTORS(m) pwm_motor[m] = 0;
            return;
        }
    }
       
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
        eo_axisController_SetTorque(ems->axis_controller[2], (80*ems->motor_current[3])/44);
        
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
    
    int32_t pwm_joint[NAXLES];

    eObool_t stiffness[NAXLES];
    
    JOINTS(j)
    {
        pwm_joint[j] = eo_axisController_PWM(ems->axis_controller[j], &stiffness[j]);
    }
    
    uint8_t stop_mask = eo_motors_PWM(ems->motors, pwm_joint, pwm_motor, stiffness);
    
    JOINTS(j)
    {
        if (stop_mask & (1<<j))
        {
            eo_emsController_SetControlMode(j, eomc_controlmode_cmd_idle, eobool_false);
        }
    }
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

extern void eo_emsController_SetControlMode(uint8_t joint, eOmc_controlmode_command_t mode, eObool_t b2FOC_off)
{
    if (!ems) return;
    
    static eObool_t motors_not_configured = eobool_true;
    
    if (motors_not_configured)
    {
        motors_not_configured = eobool_false;
        MOTORS(m) config_2FOC(m);   
    }
    
    eo_axisController_SetControlMode(ems->axis_controller[joint], mode);
    
    if ((mode == eomc_controlmode_cmd_idle) || (mode == eomc_controlmode_cmd_switch_everything_off))
    {
        if (b2FOC_off)
        {
            #if   defined(UPPERLEG_BOARD) || defined(ANKLE_BOARD)
                set_2FOC_idle(joint);
            #elif defined(SHOULDER_BOARD) || defined(WAIST_BOARD)
                if (joint < 3)
                {
                    if (eo_emsController_GetControlMode(0) == eomc_controlmode_idle &&
                        eo_emsController_GetControlMode(1) == eomc_controlmode_idle &&
                        eo_emsController_GetControlMode(2) == eomc_controlmode_idle)
                    {
                        set_2FOC_idle(0);
                        set_2FOC_idle(1);
                        set_2FOC_idle(2);
                    }
                }
            #endif
                
            #if defined(SHOULDER_BOARD)
                if (joint == 3) set_2FOC_idle(3);
            #endif
        }
    }
    else
    {
        #if   defined(UPPERLEG_BOARD) || defined(ANKLE_BOARD)
            set_2FOC_running(joint, mode);
        #elif defined(SHOULDER_BOARD) || defined(WAIST_BOARD)
            if (joint < 3)
            {
                set_2FOC_running(0, mode);
                set_2FOC_running(1, mode);
                set_2FOC_running(2, mode);
            }
        #endif
        
        #if defined(SHOULDER_BOARD)
            if (joint == 3) set_2FOC_running(3, mode);
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
    
    #if defined(UPPERLEG_BOARD) || defined(ANKLE_BOARD) //|| defined(WAIST_BOARD) || defined(V2_MECHANICS)
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
    #elif defined(SHOULDER_BOARD) || defined(WAIST_BOARD)
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
    #endif
        
    #if defined(SHOULDER_BOARD)
        if (eo_axisController_IsCalibrated(ems->axis_controller[3]))
        {
            ems->n_calibrated++;
        }
        else if (eo_absCalibratedEncoder_IsOk(ems->abs_calib_encoder[3]))
        {    
            ems->n_calibrated++;
            eo_axisController_SetCalibrated(ems->axis_controller[3]);
        }
    #endif  
}

extern void eo_emsController_Stop(uint8_t joint)
{
    if (ems) eo_axisController_Stop(ems->axis_controller[joint]);
}

extern void eo_emsController_ResetTrqPid(uint8_t joint)
{
    if (ems) eo_pid_Reset(eo_axisController_GetTrqPidPtr(ems->axis_controller[joint]));
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
        pidStatus->reference = 0;
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
        jointStatus->basic.position            = 0;  // the position of the joint           
        jointStatus->basic.velocity            = 0;  // the velocity of the joint          
        jointStatus->basic.acceleration        = 0;  // the acceleration of the joint       
        jointStatus->basic.torque              = 0;  // the torque of the joint when locally measured
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
extern void eo_emsController_SetPosPid(uint8_t joint, float Kp, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff)
{
    if (ems)
    {
        eo_axisController_SetPosPid(ems->axis_controller[joint], Kp, Kd, Ki, Imax, Ymax, Yoff);
        
        #ifdef USE_2FOC_FAST_ENCODER
            eo_axleVirtualEncoder_SetSign(ems->axle_virt_encoder[joint], (Kp >= 0.0f ? 1:-1));
        #endif
    }
}   
extern void eo_emsController_SetTrqPid(uint8_t joint, float Kp, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff)
{
    if (ems) eo_axisController_SetTrqPid(ems->axis_controller[joint], Kp, Kd,  Ki, Imax, Ymax, Yoff); 
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

extern void eo_emsController_ReadMotorstatus(uint8_t motor, uint8_t motorerror, uint8_t canerror, eOmc_controlmode_t controlmode)
{
    if (ems) eo_motor_set_motor_status(ems->motors, motor, motorerror || canerror, controlmode);
}

extern void eo_emsController_GetMotorStatus(uint8_t mId, eOmc_motor_status_t* motor_status)
{
    motor_status->basic.position = ems->motor_position[mId];
    motor_status->basic.velocity = ems->motor_velocity[mId];
    motor_status->basic.current  = ems->motor_current [mId];
}

extern void eo_emsMotorController_GoIdle(void)
{
    if (ems)
    {
        JOINTS(j)
        {
            eo_emsController_SetControlMode(j, eomc_controlmode_cmd_idle, eobool_true);
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
    eOmc_PID_t pid;
    eOmc_i2tParams_t i2t;
    icubCanProto_current_t max_current;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eOappTheDB_jointOrMotorCanLocation_t canLoc;
    eOicubCanProto_msgDestination_t msgdest;
    
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };
    
    eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), motor,  &canLoc, NULL);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
     
    pid.kp = 0x00C0;
    pid.ki = 0x0080;
    pid.kd = 0x3500; // it is integral anti windup
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &pid);
    
    i2t.time  = 668;
    i2t.tresh = 12000;
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &i2t);
    
    max_current = 5000; // 5A
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &max_current);
}

void set_2FOC_idle(uint8_t motor)
{
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eOappTheDB_jointOrMotorCanLocation_t canLoc;
    eOicubCanProto_msgDestination_t msgdest;
    
    eOicubCanProto_msgCommand_t msgCmd =
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };
    
    eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), motor,  &canLoc, NULL);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__DISABLE_PWM_PAD;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);

    //msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE;
    //eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
}

void set_2FOC_running(uint8_t motor, eOmc_controlmode_command_t mode)
{
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_openloop;
    eOappTheDB_jointOrMotorCanLocation_t canLoc;
    eOicubCanProto_msgDestination_t msgdest;
    
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };
    
    eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), motor,  &canLoc, NULL);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinboard, canLoc.addr);
    
    //controlmode_2foc = icubCanProto_controlmode_velocity;
    controlmode_2foc = icubCanProto_controlmode_openloop;
    //controlmode_2foc = icubCanProto_controlmode_current;
        
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__ENABLE_PWM_PAD;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
            
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_RUN;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &controlmode_2foc);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




