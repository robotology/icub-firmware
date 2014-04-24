
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
#include "hal_led.h"

#include <string.h>


#include "EOicubCanProto_specifications.h"
#include "EOtheEMSapplBody.h"

#include "EOMtheEMSbackdoor.h"
#include "OPCprotocolManager_Cfg.h"

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

static EOemsController *s_emsc = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOemsController* eo_emsController_Init() 
{    
#ifndef NO_2FOC_BOARD
    s_emsc = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOemsController), 1);
#endif
    
    if (s_emsc)
    {        
        s_emsc->n_calibrated = 0;
        s_emsc->state_mask = EMS_OK;
        
        JOINTS(j)
        {
            s_emsc->axis_controller[j] = eo_axisController_New(j);
            s_emsc->enc_speedometer[j] = eo_speedmeter_New();
        }
        
        s_emsc->motors = eo_motors_New(NAXLES);
    }
    
    return s_emsc;
}

extern void eo_emsController_SetEncSign(uint16_t jxx, int32_t enc_sign)
{
    if (!s_emsc) return;
    
    eo_speedometer_SetEncSign(s_emsc->enc_speedometer[jxx], enc_sign);
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

#if !defined(V1_MECHANICS) && !defined(V2_MECHANICS)
#error mechanics is undefined
#endif

extern void eo_emsController_ReadEncoders(int32_t *enc_pos)
{
    if (!s_emsc) return;
    
    int32_t enc_vel[NAXLES];
    
    ENCODERS(e)
    {
        eo_speedometer_SlowEncoderRead(s_emsc->enc_speedometer[e], enc_pos[e]);
        
        enc_pos[e] = eo_speedometer_GetDistance(s_emsc->enc_speedometer[e]);
        enc_vel[e] = eo_speedometer_GetVelocity(s_emsc->enc_speedometer[e]);
    }

    #if defined(SHOULDER_BOARD) && defined(V1_MECHANICS)
        // |J0|   |  1     0    0   |   |E0|     
		// |J1| = |  0     1    0   | * |E1|
		// |J2|   |  1    -1  40/65 |   |E2|
        
        int32_t jnt_pos_2_ = enc_pos[0]-enc_pos[1]+(40*enc_pos[2])/65;
        //s_emsc->cable_length_alarm = eo_motors_CableLimitAlarm(enc_pos[0], enc_pos[1], jnt_pos_2_);
        eo_axisController_SetEncPos(s_emsc->axis_controller[0], enc_pos[0]);
        eo_axisController_SetEncPos(s_emsc->axis_controller[1], enc_pos[1]);
        eo_axisController_SetEncPos(s_emsc->axis_controller[2], jnt_pos_2_);
        eo_axisController_SetEncPos(s_emsc->axis_controller[3], enc_pos[3]);
    
        #ifdef USE_2FOC_FAST_ENCODER
            eo_axisController_SetEncVel(s_emsc->axis_controller[0], enc_vel[0]);
            eo_axisController_SetEncVel(s_emsc->axis_controller[1], enc_vel[0] + (40*enc_vel[1])/65);
            eo_axisController_SetEncVel(s_emsc->axis_controller[2], (40*(enc_vel[2]-enc_vel[1]))/65);
            eo_axisController_SetEncVel(s_emsc->axis_controller[3], enc_vel[3]);
        #else
            eo_axisController_SetEncVel(s_emsc->axis_controller[0], enc_vel[0]);
            eo_axisController_SetEncVel(s_emsc->axis_controller[1], enc_vel[1]);
            eo_axisController_SetEncVel(s_emsc->axis_controller[2], enc_vel[0]-enc_vel[1]+(40*enc_vel[2])/65);
            eo_axisController_SetEncVel(s_emsc->axis_controller[3], enc_vel[3]);
        #endif
    #else
        JOINTS(j)
        {
            eo_axisController_SetEncPos(s_emsc->axis_controller[j], enc_pos[j]);
            eo_axisController_SetEncVel(s_emsc->axis_controller[j], enc_vel[j]);
        }
    #endif
    
    if (s_emsc->n_calibrated != NAXLES)
    {
        eo_emsController_CheckCalibrations();
    }
}

extern void eo_emsController_ReadTorque(uint8_t joint, eOmeas_torque_t trq_measure)
{
    if (!s_emsc) return;
    
    eo_axisController_SetTorque(s_emsc->axis_controller[joint], trq_measure);
}

#ifdef USE_2FOC_FAST_ENCODER
extern void eo_emsController_ReadSpeed(uint8_t axis, int32_t speed, int32_t pos)
{
    if (s_emsc) eo_speedometer_FastEncoderRead(s_emsc->enc_speedometer[axis], speed, pos);
}
#endif

extern void eo_emsController_PWM(int16_t* pwm_motor)
{
    if ((!s_emsc) || (s_emsc->state_mask != EMS_OK) /*|| (s_emsc->n_calibrated != NAXLES)*/)
    {
        MOTORS(m) pwm_motor[m] = 0;
        return;
    }
    
    ENCODERS(e)
    {
        if (eo_speedometer_IsHardFault(s_emsc->enc_speedometer[e]))
        {
            //o->state_mask |= ...
            MOTORS(m) pwm_motor[m] = 0;
            return;
        }
    }
       
    int32_t pwm_joint[NAXLES];

    eObool_t stiffness[NAXLES];
    
    JOINTS(j)
    {
        pwm_joint[j] = eo_axisController_PWM(s_emsc->axis_controller[j], &stiffness[j]);
    }
    
    uint8_t stop_mask = eo_motors_PWM(s_emsc->motors, pwm_joint, pwm_motor, stiffness);
    
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
    if (s_emsc) eo_axisController_SetOutput(s_emsc->axis_controller[joint], out);    
}

extern void eo_emsController_SetPosRef(uint8_t joint, int32_t pos, int32_t avg_vel)
{
    if (s_emsc) eo_axisController_SetPosRef(s_emsc->axis_controller[joint], pos, avg_vel);       
}

extern void eo_emsController_SetPosRaw(uint8_t joint, int32_t pos)
{
    if (s_emsc) eo_axisController_SetPosRaw(s_emsc->axis_controller[joint], pos);       
}

extern void eo_emsController_SetVelRef(uint8_t joint, int32_t vel, int32_t avg_acc)
{
    if (s_emsc) eo_axisController_SetVelRef(s_emsc->axis_controller[joint], vel, avg_acc);
}

extern void eo_emsController_SetTrqRef(uint8_t joint, int32_t trq)
{
    if (s_emsc) eo_axisController_SetTrqRef(s_emsc->axis_controller[joint], trq);
}

extern void eo_emsController_SetControlMode(uint8_t joint, eOmc_controlmode_command_t mode, eObool_t b2FOC_off)
{
    if (!s_emsc) return;
    
    static eObool_t motors_not_configured = eobool_true;
    
    if (motors_not_configured)
    {
        motors_not_configured = eobool_false;
        MOTORS(m) config_2FOC(m);   
    }
    
    eo_axisController_SetControlMode(s_emsc->axis_controller[joint], mode);
    
    if (mode == eomc_controlmode_cmd_idle)
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
    if (!s_emsc) return eomc_controlmode_idle;

    return eo_axisController_GetControlMode(s_emsc->axis_controller[joint]);
}

extern void eo_emsController_ResetPosPid(uint8_t joint)
{
    if (s_emsc) eo_pid_Reset(eo_axisController_GetPosPidPtr(s_emsc->axis_controller[joint]));
}

extern void eo_emsController_StartCalibration(uint8_t joint, int32_t pos, int32_t vel, int32_t offset)
{
    if (!s_emsc) return;
    
    s_emsc->n_calibrated = 0;
        
    eo_speedometer_Calibrate(s_emsc->enc_speedometer[joint], offset);
    
    eo_axisController_StartCalibration(s_emsc->axis_controller[joint]);
}
    
extern void eo_emsController_CheckCalibrations(void)
{
    if (!s_emsc) return;
    
    s_emsc->n_calibrated = 0;
    
    #if defined(UPPERLEG_BOARD) || defined(ANKLE_BOARD) || defined(WAIST_BOARD) || defined(V2_MECHANICS)
        JOINTS(j)
        {
            if (eo_axisController_IsCalibrated(s_emsc->axis_controller[j]))
            {
                s_emsc->n_calibrated++;
            }
            else if (eo_speedometer_IsOk(s_emsc->enc_speedometer[j]))
            {    
                s_emsc->n_calibrated++;
                eo_axisController_SetCalibrated(s_emsc->axis_controller[j]);
            }
        }
    #elif defined(SHOULDER_BOARD) 
        if (eo_axisController_IsCalibrated(s_emsc->axis_controller[0]) &&
            eo_axisController_IsCalibrated(s_emsc->axis_controller[1]) &&
            eo_axisController_IsCalibrated(s_emsc->axis_controller[2]))
        {
            s_emsc->n_calibrated+=3;
        }
        else if (eo_speedometer_IsOk(s_emsc->enc_speedometer[0]) && 
                 eo_speedometer_IsOk(s_emsc->enc_speedometer[1]) &&
                 eo_speedometer_IsOk(s_emsc->enc_speedometer[2]))
        {
            s_emsc->n_calibrated+=3;
            eo_axisController_SetCalibrated(s_emsc->axis_controller[0]);
            eo_axisController_SetCalibrated(s_emsc->axis_controller[1]);
            eo_axisController_SetCalibrated(s_emsc->axis_controller[2]);
        }

        if (eo_axisController_IsCalibrated(s_emsc->axis_controller[3]))
        {
            s_emsc->n_calibrated++;
        }
        else if (eo_speedometer_IsOk(s_emsc->enc_speedometer[3]))
        {    
            s_emsc->n_calibrated++;
            eo_axisController_SetCalibrated(s_emsc->axis_controller[3]);
        }
    #endif  
}

extern void eo_emsController_Stop(uint8_t joint)
{
    if (s_emsc) eo_axisController_Stop(s_emsc->axis_controller[joint]);
}

extern void eo_emsController_ResetTrqPid(uint8_t joint)
{
    if (s_emsc) eo_pid_Reset(eo_axisController_GetTrqPidPtr(s_emsc->axis_controller[joint]));
}

extern void eo_emsController_GetActivePidStatus(uint8_t joint, eOmc_joint_status_ofpid_t* pidStatus)
{
    if (s_emsc && s_emsc->axis_controller[joint])
    {
        eo_axisController_GetActivePidStatus(s_emsc->axis_controller[joint], pidStatus);
    }
    else
    {
        pidStatus->error     = 0;
        pidStatus->output    = 0;
        pidStatus->reference = 0;
    }
}

extern void eo_emsController_GetJointStatus(uint8_t joint, eOmc_joint_status_basic_t* jointStatus)
{
    if (s_emsc && s_emsc->axis_controller[joint])
    {
        eo_axisController_GetJointStatus(s_emsc->axis_controller[joint], jointStatus);
    }
    else
    {
        jointStatus->position            = 0;  // the position of the joint           
        jointStatus->velocity            = 0;  // the velocity of the joint          
        jointStatus->acceleration        = 0;  // the acceleration of the joint       
        jointStatus->torque              = 0;  // the torque of the joint when locally measured
        jointStatus->motionmonitorstatus = (eOenum08_t)eomc_motionmonitorstatus_notmonitored;  // use eOmc_motionmonitorstatus_t. it is eomc_motionmonitorstatus_notmonitored unless the monitor is activated in jconfig.motionmonitormode  
        jointStatus->controlmodestatus   = eomc_controlmode_idle;  // use eOmc_controlmode_t. it is a readonly shadow copy of jconfig.controlmode used to remind the host of teh current controlmode
    }
}

extern eObool_t eo_emsController_GetMotionDone(uint8_t joint)
{
    if (s_emsc && s_emsc->axis_controller[joint])
    {
        return eo_axisController_GetMotionDone(s_emsc->axis_controller[joint]);
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
    if (s_emsc) eo_axisController_SetPosPid(s_emsc->axis_controller[joint], Kp, Kd, Ki, Imax, Ymax, Yoff);    
}   
extern void eo_emsController_SetTrqPid(uint8_t joint, float Kp, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff)
{
    if (s_emsc) eo_axisController_SetTrqPid(s_emsc->axis_controller[joint], Kp, Kd,  Ki, Imax, Ymax, Yoff); 
}   
// PID configurations
///////////////////////

extern void eo_emsController_SetImpedance(uint8_t joint, int32_t stiffness, int32_t damping, int32_t offset)
{
    if (s_emsc) eo_axisController_SetImpedance(s_emsc->axis_controller[joint], stiffness, damping, offset);
}

extern void eo_emsController_GetImpedance(uint8_t joint, int32_t *stiffness, int32_t *damping, int32_t *offset)
{
    if (s_emsc) eo_axisController_GetImpedance(s_emsc->axis_controller[joint], stiffness, damping, offset);
}

extern void eo_emsController_SetVelTimeout(uint8_t joint, int32_t vel_timeout)
{
    if (s_emsc) eo_axisController_SetVelTimeout(s_emsc->axis_controller[joint], vel_timeout);
}

extern void eo_emsController_SetLimits(uint8_t joint, int32_t pos_min, int32_t pos_max, int32_t vel_max)
{
    if (s_emsc) eo_axisController_SetLimits(s_emsc->axis_controller[joint], pos_min, pos_max, vel_max);
}

extern void eo_emsController_SetPosMin(uint8_t joint, int32_t pos_min)
{
    if (s_emsc) eo_axisController_SetPosMin(s_emsc->axis_controller[joint], pos_min);
}

extern void eo_emsController_SetPosMax(uint8_t joint, int32_t pos_max)
{
    if (s_emsc) eo_axisController_SetPosMax(s_emsc->axis_controller[joint], pos_max);
}

extern void eo_emsController_SetVelMax(uint8_t joint, int32_t vel_max)
{
    if (s_emsc) eo_axisController_SetVelMax(s_emsc->axis_controller[joint], vel_max);
}

extern void eo_emsController_ReadMotorstatus(uint8_t motor, uint8_t motorerror, uint8_t canerror, eOmc_controlmode_t controlmode)
{
    if (s_emsc) eo_motor_set_motor_status(s_emsc->motors, motor, motorerror || canerror, controlmode);
}

extern void eo_emsMotorController_GoIdle(void)
{
    if (s_emsc)
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




