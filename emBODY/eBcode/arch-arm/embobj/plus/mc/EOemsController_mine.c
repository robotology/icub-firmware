
/*  @file       EOdecoupler.c
    @brief      This file implements internal implementation of motor-joint decoupling matrix.
    @author     alessandro.scalzo@iit.it
    @date       07/05/2012
**/

// ciao2
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "EOtheMemoryPool.h"

#include <string.h>

// keep it before any evaluation of macros ...
#include "EOemsControllerCfg.h"


#if !defined(DONT_USE_2FOC) 
 // we need to communicate over can 
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

#define SCAN(s)     for (uint8_t s=0; s<NAXLES; ++s)
#define JOINTS(j)   SCAN(j)
#define MOTORS(m)   SCAN(m)
#define ENCODERS(e) SCAN(e)

#define AXIS_TORQUE_SENS_FAULT   0x0100
#define AEA_ABS_ENC_INVALID_DATA 0x4000
#define AEA_ABS_ENC_TIMEOUT      0x8000

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
void set_2FOC_running(uint8_t joint);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOemsController *ems = NULL;

//static const char s_eobj_ownname[] = "EOemsController";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOemsController* eo_emsController_Init() 
{    
#ifndef NO_LOCAL_CONTROL
    ems = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOemsController), 1);
#endif
    
    if (ems)
    {        
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
            ems->gearbox_ratio[j]  = 1;
            ems->rotorencoder[j]  = 1;
        }
        
        ems->motors = eo_motors_New(NAXLES);
    }
    
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
    eo_motors_reset_wdog(ems->motors, motor);
    
    ems->motor_current [motor] = (2*current)/3; // Iq = sqrt(3)/2*Imeas, 32768 = 25000 mA ==> 0.66 scale factor
    ems->motor_velocity[motor] = 1000*velocity;
    ems->motor_velocity_gbx[motor] = ems->motor_velocity[motor]/ems->gearbox_ratio[motor];
    ems->motor_position[motor] = position;
    
    //change the sign of motor position and motor velocity according to the sign of rotorencoder
    if (ems->rotorencoder[motor]<0)
    {
       ems->motor_velocity[motor]     = - ems->motor_velocity[motor];
       ems->motor_velocity_gbx[motor] = - ems->motor_velocity_gbx[motor];
       ems->motor_position[motor]     = - ems->motor_position[motor];
    }
}

extern void eo_emsController_AcquireAbsEncoders(int32_t *abs_enc_pos, uint8_t error_mask)
{
#if defined(USE_2FOC_FAST_ENCODER)    
    //static const int16_t FOC_2_EMS_SPEED = 1000/GEARBOX_REDUCTION;
#endif
    
    int32_t axle_abs_pos[NAXLES];
    #ifndef USE_2FOC_FAST_ENCODER
    int32_t axle_abs_vel[NAXLES];
    #endif
    
    ENCODERS(e)
    {
        axle_abs_pos[e] = eo_absCalibratedEncoder_Acquire(ems->abs_calib_encoder[e], abs_enc_pos[e], 0x03 & (error_mask>>(e<<1)));
        #ifndef USE_2FOC_FAST_ENCODER
        axle_abs_vel[e] = eo_absCalibratedEncoder_GetVel(ems->abs_calib_encoder[e]);
        #endif
    }
    
    ////////////////////////////////////////////////////
    // CHECK HARDWARE AEA ENCODER & MOTOR FAULT
    
    static uint32_t ems_fault_mask_old[4];
           uint32_t ems_fault_mask_new[4];
    
    JOINTS(j)
    {
        if (eo_axisController_IsTorqueSensorFault(ems->axis_controller[j]))
        {
            ems_fault_mask_new[j] = AXIS_TORQUE_SENS_FAULT;
        }
        else
        {
            ems_fault_mask_new[j] = 0;
        }
    }
    
    #if defined(USE_JACOBIAN)
    
    uint8_t  ef[NAXLES];
    uint16_t mf[NAXLES];
    
    JOINTS(j)
    {
        ef[j] = eo_absCalibratedEncoder_IsHardFault(ems->abs_calib_encoder[j]);
        mf[j] = eo_get_motor_fault_mask(ems->motors, j);
    }
    
    JOINTS(f)
    {
        if (ef[f] || (mf[f] & MOTOR_HARDWARE_FAULT))
        {    
            JOINTS(j)
            {
                if (eo_motors_are_coupled(ems->motors, f, j)) 
                {
                    eo_axisController_SetHardwareFault(ems->axis_controller[j]);
                    if (!mf[j]) set_2FOC_idle(j);
                    ems_fault_mask_new[j] |= ((uint16_t)ef[j]) << 8;
                    ems_fault_mask_new[j] |= mf[j];
                }
            }
        }
    }
    
    JOINTS(f)
    {
        if (mf[f] & MOTOR_EXTERNAL_FAULT)
        {    
            eo_axisController_SetControlMode(ems->axis_controller[f], eomc_controlmode_cmd_idle);
            ems_fault_mask_new[f] |= mf[f];
        }
    }
    
    #else
    
    #if defined(SHOULDER_BOARD) || defined(WAIST_BOARD)
    
        uint8_t  ef0 = eo_absCalibratedEncoder_IsHardFault(ems->abs_calib_encoder[0]);
        uint8_t  ef1 = eo_absCalibratedEncoder_IsHardFault(ems->abs_calib_encoder[1]);
        uint8_t  ef2 = eo_absCalibratedEncoder_IsHardFault(ems->abs_calib_encoder[2]);
    
        uint16_t mf0 = eo_get_motor_fault_mask(ems->motors, 0);
        uint16_t mf1 = eo_get_motor_fault_mask(ems->motors, 1);
        uint16_t mf2 = eo_get_motor_fault_mask(ems->motors, 2);
    
        if (ef0 || ef1 || ef2 || ((mf0 | mf1 | mf2) & MOTOR_HARDWARE_FAULT))
        {            
            eo_axisController_SetHardwareFault(ems->axis_controller[0]);
            eo_axisController_SetHardwareFault(ems->axis_controller[1]);
            eo_axisController_SetHardwareFault(ems->axis_controller[2]);
            
            if (!mf0) set_2FOC_idle(0);
            if (!mf1) set_2FOC_idle(1);
            if (!mf2) set_2FOC_idle(2);
            
            ems_fault_mask_new[0] |= ((uint16_t)ef0) << 8;
            ems_fault_mask_new[1] |= ((uint16_t)ef1) << 8;
            ems_fault_mask_new[2] |= ((uint16_t)ef2) << 8;
            
            ems_fault_mask_new[0] |= mf0;
            ems_fault_mask_new[1] |= mf1;
            ems_fault_mask_new[2] |= mf2;
        }
        else if ((mf0 | mf1 | mf2) & MOTOR_EXTERNAL_FAULT) // external fault
        {
            eo_axisController_SetControlMode(ems->axis_controller[0], eomc_controlmode_cmd_idle);
            eo_axisController_SetControlMode(ems->axis_controller[1], eomc_controlmode_cmd_idle);
            eo_axisController_SetControlMode(ems->axis_controller[2], eomc_controlmode_cmd_idle);
            
            ems_fault_mask_new[0] |= mf0;
            ems_fault_mask_new[1] |= mf1;
            ems_fault_mask_new[2] |= mf2;
        }
    #endif
        
    #if defined(UPPERLEG_BOARD) || defined(ANKLE_BOARD) || defined(SHOULDER_BOARD)
        
        #if defined(SHOULDER_BOARD)
        static const uint8_t j = 3;
        #else
        JOINTS(j)
        #endif
        {
            uint8_t  efj = eo_absCalibratedEncoder_IsHardFault(ems->abs_calib_encoder[j]);
            uint16_t mfj = eo_get_motor_fault_mask(ems->motors, j);
            
            if (efj || (mfj & MOTOR_HARDWARE_FAULT)) // hard fault
            {                
                eo_axisController_SetHardwareFault(ems->axis_controller[j]);
                
                if (!mfj) set_2FOC_idle(j);
                
                ems_fault_mask_new[j] |= ((uint16_t)efj) << 8;
                ems_fault_mask_new[j] |= mfj;
            }
            else if (mfj & MOTOR_EXTERNAL_FAULT) // ext fault
            {
                eo_axisController_SetControlMode(ems->axis_controller[j], eomc_controlmode_cmd_idle);
                
                ems_fault_mask_new[j] |= mfj;
            }
        }
        
    #endif
    #endif

    //uint8_t changed = 0;
        
    static uint16_t transmit_time = 0;
        
    if (++transmit_time >= 5000)
    {
        transmit_time = 0;
        
        JOINTS(j) ems_fault_mask_old[j] = 0;
    }
        
    JOINTS(j)
    {    
        if (ems_fault_mask_new[j] != ems_fault_mask_old[j])
        {
            ems_fault_mask_old[j] = ems_fault_mask_new[j];
         
            //changed = 1;
            
            if (ems_fault_mask_new[j])
            {
                /*
                MOTOR_EXTERNAL_FAULT     0x0001
                MOTOR_OVERCURRENT_FAULT  0x0002
                MOTOR_I2T_LIMIT_FAULT    0x0004
                MOTOR_HALLSENSORS_FAULT  0x0008
                MOTOR_QENCODER_FAULT     0x0010
                MOTOR_CAN_INVALID_PROT   0x0020
                MOTOR_CAN_GENERIC_FAULT  0x0040
                MOTOR_CAN_NOT_RESPONDING 0x0080
                AXIS_TORQUE_SENS_FAULT   0x0100
                AEA_ABS_ENC_INVALID_DATA 0x4000
                AEA_ABS_ENC_TIMEOUT      0x8000
                */
                
                // 2FOC ERRORS
                if (ems_fault_mask_new[j] & MOTOR_EXTERNAL_FAULT)
                {
                    eOerrmanDescriptor_t descriptor = {0};
                    descriptor.par16 = j;
                    descriptor.par64 = 0;
                    descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; 
                    descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
                    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_external_fault);
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, NULL, NULL, &descriptor);
                }
                
                if (ems_fault_mask_new[j] & MOTOR_OVERCURRENT_FAULT)
                {
                    eOerrmanDescriptor_t descriptor = {0};
                    descriptor.par16 = j; // unless required
                    descriptor.par64 = 0;
                    descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
                    descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
                    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_overcurrent);
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
                }
                
                if (ems_fault_mask_new[j] & MOTOR_I2T_LIMIT_FAULT)
                {
                    eOerrmanDescriptor_t descriptor = {0};
                    descriptor.par16 = j; // unless required
                    descriptor.par64 = 0;
                    descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
                    descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
                    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_i2t_limit);
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
                }
                
                if (ems_fault_mask_new[j] & MOTOR_HALLSENSORS_FAULT)
                {
                    eOerrmanDescriptor_t descriptor = {0};
                    descriptor.par16 = j; // unless required
                    descriptor.par64 = 0;
                    descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
                    descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
                    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_hallsensors);
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
                }
                
                if (ems_fault_mask_new[j] & MOTOR_QENCODER_FAULT)
                {
                    eOerrmanDescriptor_t descriptor = {0};
                    descriptor.par16 = j; // unless required
                    descriptor.par64 = 0;
                    descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
                    descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
                    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_qencoder);
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
                }
                
                if (ems_fault_mask_new[j] & MOTOR_CAN_INVALID_PROT)
                {
                    eOerrmanDescriptor_t descriptor = {0};
                    descriptor.par16 = j; // unless required
                    descriptor.par64 = 0;
                    descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
                    descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
                    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_can_invalid_prot);
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
                }
                
                if (ems_fault_mask_new[j] & MOTOR_CAN_GENERIC_FAULT)
                {
                    eOerrmanDescriptor_t descriptor = {0};
                    descriptor.par16 = j; // unless required
                    descriptor.par64 = 0;
                    descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
                    descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
                    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_can_generic);
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
                }
                
                if (ems_fault_mask_new[j] & MOTOR_CAN_NOT_RESPONDING)
                {
                    eOerrmanDescriptor_t descriptor = {0};
                    descriptor.par16 = j; // unless required
                    descriptor.par64 = 0;
                    descriptor.sourcedevice = eo_errman_sourcedevice_canbus1; // 0 e' board, 1 can1, 2 can2
                    descriptor.sourceaddress = j+1; // oppure l'id del can che ha dato errore
                    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_motor_can_no_answer);
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
                }
                
                // BOARD ERRORS
                if (ems_fault_mask_new[j] & AXIS_TORQUE_SENS_FAULT)
                {
                    eOerrmanDescriptor_t descriptor = {0};
                    descriptor.par16 = j; // unless required
                    descriptor.par64 = 0;
                    descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
                    descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
                    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_axis_torque_sens);
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
                }
                
                if (ems_fault_mask_new[j] & AEA_ABS_ENC_INVALID_DATA)
                {
                    eOerrmanDescriptor_t descriptor = {0};
                    descriptor.par16 = j; // unless required
                    descriptor.par64 = 0;
                    descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
                    descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
                    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_aea_abs_enc_invalid);
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
                }
                
                if (ems_fault_mask_new[j] & AEA_ABS_ENC_TIMEOUT)
                {
                    eOerrmanDescriptor_t descriptor = {0};
                    descriptor.par16 = j; // unless required
                    descriptor.par64 = 0;
                    descriptor.sourcedevice = eo_errman_sourcedevice_localboard; // 0 e' board, 1 can1, 2 can2
                    descriptor.sourceaddress = 0; // oppure l'id del can che ha dato errore
                    descriptor.code = eoerror_code_get(eoerror_category_MotionControl, eoerror_value_MC_aea_abs_enc_timeout);
                    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, NULL, &descriptor);
                }
            }
        }
    }
    
    /*
    if (changed)
    {
        static char msg[31];
        sprintf(msg, "*** ERROR %04X %04X %04X %04X\n", ems_fault_mask_new[0], ems_fault_mask_new[1], ems_fault_mask_new[2], ems_fault_mask_new[3]);

        send_diagnostics_to_server(msg, 0xffffffff, 1);
    }
    */
    
    // CHECK HARDWARE AEA ENCODER & MOTOR FAULT
    ////////////////////////////////////////////////////
        
    #if defined(SHOULDER_BOARD) && defined(V1_MECHANICS)
        // |J0|   |  1     0    0   |   |E0|     
        // |J1| = |  0     1    0   | * |E1|
        // |J2|   |  1    -1  40/65 |   |E2|
        axle_abs_pos[2] = axle_abs_pos[0]-axle_abs_pos[1]+(40*axle_abs_pos[2])/65;
        #ifndef USE_2FOC_FAST_ENCODER
        axle_abs_vel[2] = axle_abs_vel[0]-axle_abs_vel[1]+(40*axle_abs_vel[2])/65;
        #endif
    #endif
    
#ifdef USE_2FOC_FAST_ENCODER
    
    int32_t axle_virt_vel[NAXLES];
    int32_t axle_virt_pos[NAXLES];
    
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
    
    #elif defined(SHOULDER_BOARD)
    
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
    
    #elif defined(WAIST_BOARD)
    
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

    #elif defined(UPPERLEG_BOARD)
    
        axle_virt_vel[0] = (50*ems->motor_velocity_gbx[0])/75;
        axle_virt_vel[1] =     ems->motor_velocity_gbx[1]    ;
        axle_virt_vel[2] =     ems->motor_velocity_gbx[2]    ;
        axle_virt_vel[3] =     ems->motor_velocity_gbx[3]    ;
        
        axle_virt_pos[0] = (50*ems->motor_position[0])/75;
        axle_virt_pos[1] =     ems->motor_position[1]    ;
        axle_virt_pos[2] =     ems->motor_position[2]    ;
        axle_virt_pos[3] =     ems->motor_position[3]    ;
        
    #elif defined(ANKLE_BOARD)
        
        axle_virt_vel[0] = ems->motor_velocity_gbx[0];
        axle_virt_vel[1] = ems->motor_velocity_gbx[1];
        
        axle_virt_pos[0] = ems->motor_position[0];
        axle_virt_pos[1] = ems->motor_position[1];
        
    #else
        //#error undefined board type
    #endif
    
#endif
    
    JOINTS(j)
    {
        #ifdef USE_2FOC_FAST_ENCODER
            eo_axleVirtualEncoder_Acquire(ems->gearbox_ratio[j], ems->axle_virt_encoder[j], axle_abs_pos[j], axle_virt_pos[j], axle_virt_vel[j]);
            eo_axisController_SetEncPos(ems->axis_controller[j], eo_axleVirtualEncoder_GetPos(ems->axle_virt_encoder[j]));
            eo_axisController_SetEncVel(ems->axis_controller[j], eo_axleVirtualEncoder_GetVel(ems->axle_virt_encoder[j]));
        #else
            eo_axisController_SetEncPos(ems->axis_controller[j], axle_abs_pos[j]);
            eo_axisController_SetEncVel(ems->axis_controller[j], axle_abs_vel[j]);
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

extern void eo_emsController_PWM(int16_t* pwm_motor_16)
{
    if ((!ems))// || (ems->state_mask != EMS_OK) /*|| (ems->n_calibrated != NAXLES)*/)
    {
        MOTORS(m) pwm_motor_16[m] = 0;
        return;
    }
    
    float pwm_joint[NAXLES];
    eObool_t torque_protection[NAXLES];
    eObool_t stiffness[NAXLES];
    
#if !defined (DONT_USE_2FOC)
    eo_motors_check_wdog(ems->motors);
#endif
    
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
               for (int8_t k; k<NAXLES; k++)
                   torque_protection[k] = eobool_true; //put in protection all motors
            }
        }
        else if (pos >  ems->axis_controller[j]->pos_max)
        {
            float pwm_lim = eo_pid_PWM_p( ems->axis_controller[j]->pidP,  ems->axis_controller[j]->pos_max - pos);
            if ((pwm_lim > 0) ^ (pwm_joint[j] > pwm_lim))
            {
               pwm_joint[j] = pwm_lim;
               for (int8_t k; k<NAXLES; k++)
                   torque_protection[k] = eobool_true; //put in protection all motors
            }
        }
      }
    }

    //PWM decoupling
    float pwm_motor[NAXLES];
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
    #if   defined(SHOULDER_BOARD)
          if (joint_id==0) {*torque_motor= ems->axis_controller[0]->torque_meas_jnt + ems->axis_controller[1]->torque_meas_jnt; return;}
          if (joint_id==1) {*torque_motor=(ems->axis_controller[1]->torque_meas_jnt - ems->axis_controller[2]->torque_meas_jnt)* 0.625; return;}
          if (joint_id==2) {*torque_motor=ems->axis_controller[2]->torque_meas_jnt * 0.625; return;}
          if (joint_id==3) {*torque_motor=ems->axis_controller[3]->torque_meas_jnt; return;}
    #elif defined(WAIST_BOARD)
          if (joint_id==0) {*torque_motor=(ems->axis_controller[0]->torque_meas_jnt - ems->axis_controller[1]->torque_meas_jnt)*0.5 + ems->axis_controller[2]->torque_meas_jnt*0.275; return;}
          if (joint_id==1) {*torque_motor=(ems->axis_controller[0]->torque_meas_jnt + ems->axis_controller[1]->torque_meas_jnt)*0.5 + ems->axis_controller[2]->torque_meas_jnt*0.275; return;}
          if (joint_id==2) {*torque_motor= ems->axis_controller[2]->torque_meas_jnt*0.55; return;}
    #elif defined(UPPERLEG_BOARD)
          if (joint_id==0) {*torque_motor=ems->axis_controller[0]->torque_meas_jnt; return;}
          if (joint_id==1) {*torque_motor=ems->axis_controller[1]->torque_meas_jnt; return;}
          if (joint_id==2) {*torque_motor=ems->axis_controller[2]->torque_meas_jnt; return;}
          if (joint_id==3) {*torque_motor=ems->axis_controller[3]->torque_meas_jnt; return;}
    #elif defined(ANKLE_BOARD)
          if (joint_id==0) {*torque_motor=ems->axis_controller[0]->torque_meas_jnt; return;}
          if (joint_id==1) {*torque_motor=ems->axis_controller[1]->torque_meas_jnt; return;}
    #else
          *torque_motor=ems->axis_controller[joint_id]->torque_meas_jnt;
    #endif  
}

extern void eo_emsController_GetDecoupledReferenceTorque(uint8_t joint_id, int32_t * torque_motor)
{
    if (!ems) return; 
    #if   defined(SHOULDER_BOARD)
          if (joint_id==0) {*torque_motor= ems->axis_controller[0]->torque_ref_jnt + ems->axis_controller[1]->torque_ref_jnt; return;}
          if (joint_id==1) {*torque_motor=(ems->axis_controller[1]->torque_ref_jnt - ems->axis_controller[2]->torque_ref_jnt)* 0.625; return;}
          if (joint_id==2) {*torque_motor=ems->axis_controller[2]->torque_ref_jnt * 0.625; return;}
          if (joint_id==3) {*torque_motor=ems->axis_controller[3]->torque_ref_jnt; return;}
    #elif defined(WAIST_BOARD)
          if (joint_id==0) {*torque_motor=(ems->axis_controller[0]->torque_ref_jnt - ems->axis_controller[1]->torque_ref_jnt)*0.5 + ems->axis_controller[2]->torque_ref_jnt*0.275; return;}
          if (joint_id==1) {*torque_motor=(ems->axis_controller[0]->torque_ref_jnt + ems->axis_controller[1]->torque_ref_jnt)*0.5 + ems->axis_controller[2]->torque_ref_jnt*0.275; return;}
          if (joint_id==2) {*torque_motor= ems->axis_controller[2]->torque_ref_jnt*0.55; return;}
    #elif defined(UPPERLEG_BOARD)
          if (joint_id==0) {*torque_motor=ems->axis_controller[0]->torque_ref_jnt; return;}
          if (joint_id==1) {*torque_motor=ems->axis_controller[1]->torque_ref_jnt; return;}
          if (joint_id==2) {*torque_motor=ems->axis_controller[2]->torque_ref_jnt; return;}
          if (joint_id==3) {*torque_motor=ems->axis_controller[3]->torque_ref_jnt; return;}
    #elif defined(ANKLE_BOARD)
          if (joint_id==0) {*torque_motor=ems->axis_controller[0]->torque_ref_jnt; return;}
          if (joint_id==1) {*torque_motor=ems->axis_controller[1]->torque_ref_jnt; return;}
    #else
          *torque_motor=ems->axis_controller[joint_id]->torque_ref_jnt;
    #endif  
}

extern void eo_emsController_SetControlModeGroupJoints(uint8_t joint, eOmc_controlmode_command_t mode)
{
  #if   defined(SHOULDER_BOARD) || defined(WAIST_BOARD)
      if (joint <3) 
      {
        eo_emsController_SetControlMode(0, mode);
        eo_emsController_SetControlMode(1, mode);
        eo_emsController_SetControlMode(2, mode);
      }
      else
      {
        eo_emsController_SetControlMode(joint, mode);
      }
  #else
        eo_emsController_SetControlMode(joint, mode);
  #endif  
}

extern eObool_t eo_emsController_SetInteractionModeGroupJoints(uint8_t joint, eOmc_interactionmode_t mode)
{
  #if   defined(SHOULDER_BOARD) || defined(WAIST_BOARD)
      if (joint <3) 
      {
        eo_emsController_SetInteractionMode(0, mode);
        eo_emsController_SetInteractionMode(1, mode);
        eo_emsController_SetInteractionMode(2, mode);
      }
      else
      {
        eo_emsController_SetInteractionMode(joint, mode);
      }
  #else
        eo_emsController_SetInteractionMode(joint, mode);
  #endif
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
        
        if (eo_axisController_IsHardwareFault(ems->axis_controller[joint]))
        {
            JOINTS(j)
            {
                if (eo_motors_are_coupled(ems->motors, joint, j))
                {
                    set_2FOC_idle(j);
                    eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[j]);
                    eo_axisController_SetControlMode(ems->axis_controller[j], eomc_controlmode_cmd_force_idle);
                }
            }
        }
        else
        {
            eo_axisController_SetControlMode(ems->axis_controller[joint], eomc_controlmode_cmd_force_idle);
        }
        
        #else // ! USE_JACOBIAN
        
        #if defined(SHOULDER_BOARD)
        
            if (joint == 3)
            {
                set_2FOC_idle(3);
                eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[3]);
                eo_axisController_SetControlMode(ems->axis_controller[3], eomc_controlmode_cmd_force_idle);
            }
        
        #endif
        
        #if   defined(UPPERLEG_BOARD) || defined(ANKLE_BOARD)
        
            set_2FOC_idle(joint);
            eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[joint]);
            eo_axisController_SetControlMode(ems->axis_controller[joint], eomc_controlmode_cmd_force_idle);
        
        #elif defined(SHOULDER_BOARD) || defined(WAIST_BOARD)
            
            if (joint < 3)
            {
                if (eo_axisController_IsHardwareFault(ems->axis_controller[0]) && 
                    eo_axisController_IsHardwareFault(ems->axis_controller[1]) &&
                    eo_axisController_IsHardwareFault(ems->axis_controller[2]))
                {
                    // ENCODER OR MOTOR FAULT
                    set_2FOC_idle(0);
                    set_2FOC_idle(1);
                    set_2FOC_idle(2);
                
                    eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[0]);
                    eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[1]);
                    eo_absCalibratedEncoder_ClearFaults(ems->abs_calib_encoder[2]);
                    
                    eo_axisController_SetControlMode(ems->axis_controller[0], eomc_controlmode_cmd_force_idle);
                    eo_axisController_SetControlMode(ems->axis_controller[1], eomc_controlmode_cmd_force_idle);
                    eo_axisController_SetControlMode(ems->axis_controller[2], eomc_controlmode_cmd_force_idle);
                }
                else
                {
                    eo_axisController_SetControlMode(ems->axis_controller[joint], eomc_controlmode_cmd_force_idle);
                }
            }
            
        #endif
        #endif // ! USE_JACOBIAN
    }
    else if ((mode == eomc_controlmode_cmd_idle) || (mode == eomc_controlmode_cmd_switch_everything_off))
    {
        eo_axisController_SetControlMode(ems->axis_controller[joint], eomc_controlmode_cmd_idle);
        
        #if defined(USE_JACOBIAN)
        
        eObool_t all_idle = eobool_true;
        
        JOINTS(j)
        {
            if (eo_motors_are_coupled(ems->motors, joint, j))
            {
                if (eo_emsController_GetControlMode(j) != eomc_controlmode_idle && 
                    eo_emsController_GetControlMode(j) != eomc_controlmode_hwFault)
                {
                    all_idle = eobool_false;
                    break;
                }
            }
        }
        
        if (all_idle)
        {
            JOINTS(j)
            {
                if (eo_motors_are_coupled(ems->motors, joint, j))
                {
                    set_2FOC_idle(j);
                }
            }
        }
        
        #else // ! USE_JACOBIAN
        
        #if   defined(SHOULDER_BOARD)
        
            if (joint == 3) set_2FOC_idle(3);
        
        #endif
        
        #if   defined(UPPERLEG_BOARD) || defined(ANKLE_BOARD)
        
            set_2FOC_idle(joint);
        
        #elif defined(SHOULDER_BOARD) || defined(WAIST_BOARD)

            if (joint < 3)
            {
                if ((eo_emsController_GetControlMode(0) == eomc_controlmode_idle || 
                     eo_emsController_GetControlMode(0) == eomc_controlmode_hwFault ) &&
                    (eo_emsController_GetControlMode(1) == eomc_controlmode_idle || 
                     eo_emsController_GetControlMode(1) == eomc_controlmode_hwFault ) &&
                    (eo_emsController_GetControlMode(2) == eomc_controlmode_idle || 
                     eo_emsController_GetControlMode(2) == eomc_controlmode_hwFault ))
                {
                    set_2FOC_idle(0);
                    set_2FOC_idle(1);
                    set_2FOC_idle(2);
                }
            }
            
        #endif
        #endif // ! USE_JACOBIAN
    }
    else // not an idle mode
    {
        
        if (eo_axisController_SetControlMode(ems->axis_controller[joint], mode))
        {
        #if defined(USE_JACOBIAN)
        
        JOINTS(j)
        {
            if (eo_motors_are_coupled(ems->motors, joint, j))
            {
                if (eo_is_motor_ext_fault(ems->motors, j)) set_2FOC_idle(j);  
                set_2FOC_running(j);
            }
        }
        
        #else // ! USE_JACOBIAN
            
        #if   defined(SHOULDER_BOARD)
            
            if (joint == 3)
            {
                // external fault reset
                if (eo_is_motor_ext_fault(ems->motors, 3)) set_2FOC_idle(3);
                
                set_2FOC_running(3);
            }
            
        #endif
            
        #if   defined(UPPERLEG_BOARD) || defined(ANKLE_BOARD)
       
            // external fault reset
            if (eo_is_motor_ext_fault(ems->motors, joint)) set_2FOC_idle(joint);
            
            set_2FOC_running(joint);
        
        #elif defined(SHOULDER_BOARD) || defined(WAIST_BOARD)
        
            if (joint < 3)
            {
                // external fault reset
                if (eo_is_motor_ext_fault(ems->motors, 0)) set_2FOC_idle(0);
                if (eo_is_motor_ext_fault(ems->motors, 1)) set_2FOC_idle(1);
                if (eo_is_motor_ext_fault(ems->motors, 2)) set_2FOC_idle(2);
            
                set_2FOC_running(0);
                set_2FOC_running(1);
                set_2FOC_running(2);
            }
            
        #endif
        #endif // ! USE_JACOBIAN
        }
    }
    
    //eo_axisController_SetControlMode(ems->axis_controller[joint], mode);
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
    
    eObool_t joint_ok[NAXLES];
    
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
    if (ems) ems->gearbox_ratio[joint]=gearboxratio;
}

extern void eo_emsController_SetRotorEncoder(uint8_t joint, int32_t rotorencoder)
{
    if (ems) ems->rotorencoder[joint]=rotorencoder;
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
#if !defined(DONT_USE_2FOC)   
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
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
     
    pid.kp = 0x00C0;
    pid.ki = 0x0080;
    pid.kd = 0x3500; // it is integral anti windup
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_PID;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &pid);
    
    i2t.time  = 668;
    i2t.tresh = 12000;
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_I2T_PARAMS;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &i2t);
    
    max_current = 5000; // 5A
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CURRENT_LIMIT;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &max_current);
#endif    
}

void set_2FOC_idle(uint8_t motor)
{
#if !defined(DONT_USE_2FOC)
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle()); 
    eOappTheDB_jointOrMotorCanLocation_t canLoc;
    eOicubCanProto_msgDestination_t msgdest;
    icubCanProto_controlmode_t controlmode_2foc = icubCanProto_controlmode_idle; //+
    
    eOicubCanProto_msgCommand_t msgCmd =
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingMotorControl,
        EO_INIT(.cmdId) 0
    };
    
    eo_appTheDB_GetJointCanLocation(eo_appTheDB_GetHandle(), motor,  &canLoc, NULL);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE; //+
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &controlmode_2foc); //+
    
    eo_motor_set_motor_status(ems->motors, motor, 0);
#endif    
}

void set_2FOC_running(uint8_t motor)
{
#if !defined(DONT_USE_2FOC)    
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
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(canLoc.indexinsidecanboard, canLoc.addr);
    
    #ifdef EXPERIMENTAL_SPEED_CONTROL
    controlmode_2foc = icubCanProto_controlmode_velocity;
    #else
    controlmode_2foc = icubCanProto_controlmode_openloop;
    //controlmode_2foc = icubCanProto_controlmode_current;
    #endif
    
    
    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__SET_CONTROL_MODE;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, &controlmode_2foc);
#endif    
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




