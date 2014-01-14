
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

#define JOINTS(j)   for (uint8_t j=0; j<s_emsc->n_joints; ++j)
#define MOTORS(m)   for (uint8_t m=0; m<s_emsc->n_joints; ++m)
#define ENCODERS(e) for (uint8_t e=0; e<s_emsc->n_joints; ++e)

//#define LIMIT(x,L) if (x>(L)) x=(L); else if (x<-(L)) x=-(L)

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

const float   EMS_PERIOD           = 0.001f;
const int32_t EMS_FREQUENCY_INT32  = 1000;
const float   EMS_FREQUENCY_FLOAT  = 1000.0f;
const int32_t TICKS_PER_REVOLUTION = 65536;




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

//static const char s_eobj_ownname[] = "EOemsController";

static EOemsController *s_emsc = NULL;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOemsController* eo_emsController_Init(emsBoardType_t board_type) 
{    
    if (board_type == EMS_NULL) return NULL;

    s_emsc = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOemsController), 1);

    if (s_emsc)
    {
        s_emsc->boardType  = board_type;
        s_emsc->defcon     = EMS_PRUDENT;
        s_emsc->n_joints   = MAX_JOINTS;
        s_emsc->motors     = NULL;
        
        s_emsc->limited_motors_mask_changed = eobool_false;
        s_emsc->limited_motors_mask = 0;
        
        s_emsc->n_calibrated = 0;
        
        s_emsc->cable_length_alarm = eobool_false;
        
        JOINTS(j)
        {
            s_emsc->axis_controller[j] = NULL;
            s_emsc->enc_speedometer[j] = NULL;
        }
        
        switch (s_emsc->boardType)
        {
            case EMS_SHOULDER:
                s_emsc->axis_controller[0] = eo_axisController_New(CUT_FREQ_1_1_Hz, 0);
                s_emsc->axis_controller[1] = eo_axisController_New(CUT_FREQ_1_1_Hz, 1);
                s_emsc->axis_controller[2] = eo_axisController_New(CUT_FREQ_1_1_Hz, 2);
                s_emsc->axis_controller[3] = eo_axisController_New(CUT_FREQ_1_1_Hz, 3);
                s_emsc->n_joints = 4;
                break;
            
            case EMS_WAIST:    
                s_emsc->axis_controller[0] = eo_axisController_New(CUT_FREQ_3_0_Hz, 0);
                s_emsc->axis_controller[1] = eo_axisController_New(CUT_FREQ_3_0_Hz, 1);
                s_emsc->axis_controller[2] = eo_axisController_New(CUT_FREQ_3_0_Hz, 2);
                s_emsc->n_joints = 3;          
                break;

            case EMS_UPPERLEG:
                s_emsc->axis_controller[0] = eo_axisController_New(CUT_FREQ_1_1_Hz, 0);
                s_emsc->axis_controller[1] = eo_axisController_New(CUT_FREQ_1_1_Hz, 1);
                s_emsc->axis_controller[2] = eo_axisController_New(CUT_FREQ_1_1_Hz, 2);
                s_emsc->axis_controller[3] = eo_axisController_New(CUT_FREQ_1_1_Hz, 3);
                s_emsc->n_joints = 4;          
                break;
            
            case EMS_ANKLE:
                s_emsc->axis_controller[0] = eo_axisController_New(CUT_FREQ_3_0_Hz, 0);
                s_emsc->axis_controller[1] = eo_axisController_New(CUT_FREQ_3_0_Hz, 1);
                s_emsc->n_joints = 2;          
                break;
            
            default:           
                s_emsc->n_joints = 0;
        }
        
        s_emsc->motors = eo_motors_New(s_emsc->n_joints);
        
        JOINTS(j)
        {
            s_emsc->enc_speedometer[j] = eo_speedmeter_New();
        }
    }
    
    return s_emsc;
}

extern void eo_emsController_SetEncSign(uint16_t jxx, int32_t enc_sign)
{
    if (!s_emsc) return;
    
    eo_speedometer_SetEncSign(s_emsc->enc_speedometer[jxx], enc_sign);
}

extern void eo_emsController_ReadEncoders(int32_t *enc_pos)
{
    if (!s_emsc) return;
    
    int32_t enc_vel[MAX_JOINTS];
    
    ENCODERS(e)
    {
        eo_speedometer_SlowEncoderRead(s_emsc->enc_speedometer[e], enc_pos[e]);
        
        enc_pos[e] = eo_speedometer_GetDistance(s_emsc->enc_speedometer[e]);
        enc_vel[e] = eo_speedometer_GetVelocity(s_emsc->enc_speedometer[e]);
    }

    switch (s_emsc->boardType)
    {
    case EMS_SHOULDER:
    {
        
        // |J0|   |  1     0    0   |   |E0|     
		// |J1| = |  0     1    0   | * |E1|     with a=40/65
		// |J2|   |  1    -1    a   |   |E2|
    
        //int32_t jnt_pos_2_ = eo_axisController_EncRangeAdj(s_emsc->axis_controller[2], enc_pos);

        int32_t jnt_pos_2_ = enc_pos[0]-enc_pos[1]+(40*enc_pos[2])/65;
        
//         static uint8_t led = 0;        
//         s_emsc->cable_length_alarm = eo_motors_CableLimitAlarm(enc_pos[0], enc_pos[1], jnt_pos_2_);
//         
//         if (s_emsc->cable_length_alarm)
//         {
//             if (!led)
//             {
//                 led = 1;
//                 hal_led_on(hal_led0);
//             }
//         }
//         else
//         {
//             if (led)
//             {
//                 led = 0;
//                 hal_led_off(hal_led0);
//             }
//         }
        
        eo_axisController_SetEncPos(s_emsc->axis_controller[0], enc_pos[0]);
        eo_axisController_SetEncPos(s_emsc->axis_controller[1], enc_pos[1]);
        eo_axisController_SetEncPos(s_emsc->axis_controller[2], jnt_pos_2_);
        eo_axisController_SetEncPos(s_emsc->axis_controller[3], enc_pos[3]);

        #ifdef USE_2FOC_FAST_ENCODER
        ERROR: fast encoder shoulder geometry TBD!!!
        #else
        eo_axisController_SetEncVel(s_emsc->axis_controller[0], enc_vel[0]);
        eo_axisController_SetEncVel(s_emsc->axis_controller[1], enc_vel[1]);
        eo_axisController_SetEncVel(s_emsc->axis_controller[2], enc_vel[0]-enc_vel[1]+(40*enc_vel[2])/65);
        eo_axisController_SetEncVel(s_emsc->axis_controller[3], enc_vel[3]);
        #endif
    
        break;
    }
    case EMS_UPPERLEG:
    case EMS_WAIST:
    case EMS_ANKLE:
    default:
        JOINTS(j)
        {
            eo_axisController_SetEncPos(s_emsc->axis_controller[j], enc_pos[j]);
            eo_axisController_SetEncVel(s_emsc->axis_controller[j], enc_vel[j]);
        }
    }
    
    if (s_emsc->n_joints != s_emsc->n_calibrated)
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
extern void eo_emsController_ReadSpeed(uint8_t axis, int32_t speed)
{
    if (s_emsc) eo_speedometer_FastEncoderRead(s_emsc->enc_speedometer[axis], speed);
}
#endif

extern eObool_t eo_emsController_GetLimitedCurrentMask(uint8_t* mask)
{
    if (!s_emsc || !s_emsc->limited_motors_mask_changed) return eobool_false; 
        
    *mask = s_emsc->limited_motors_mask;
    
    return eobool_true;
}

extern void eo_emsController_PWM(int16_t* pwm_motor)
{
    if (!s_emsc)
    {
        for (uint8_t m=0; m<MAX_JOINTS; ++m) pwm_motor[m] = 0;
        
        return;
    }
    
    if (s_emsc->defcon == EMS_ALARM)
    {   
        MOTORS(m) pwm_motor[m] = 0;
        
        return;
    }
    
    EMSdefcon_t defcon = EMS_ALL_OK;
    
    uint8_t alarm_mask = s_emsc->cable_length_alarm ?  0x07 : 0x00;
    
    JOINTS(j)
    {
        /*
        if (eo_speedometer_IsHardFault(s_emsc->enc_speedometer[j]))
        {
            //static uint8_t time=0;
            
            //if (!++time) hal_led_toggle(hal_led2);
            
            //s_emsc->defcon = EMS_ALARM;
            //MOTORS(m) pwm_motor[m] = 0;
            //return;
        }
        */
           
        if (!eo_speedometer_IsOk(s_emsc->enc_speedometer[j]) || !eo_axisController_HasLimits(s_emsc->axis_controller[j]))
        {            
            alarm_mask |= 1<<j;
            s_emsc->defcon = defcon = EMS_PRUDENT;
        }
    }
    
    if (s_emsc->defcon == EMS_PRUDENT && defcon == EMS_ALL_OK)
    {
        JOINTS(j) eo_axisController_Stop(s_emsc->axis_controller[j]);
        
        s_emsc->defcon = EMS_ALL_OK;
    }
       
    int32_t pwm_joint[MAX_JOINTS];

    JOINTS(j)
    {
        eObool_t big_error_flag = eobool_false;
        
        pwm_joint[j] = eo_axisController_PWM(s_emsc->axis_controller[j], &big_error_flag);
        
        if (big_error_flag) alarm_mask |= 1<<j;
    }
    
    uint8_t stop_mask = eo_motors_PWM(s_emsc->motors, s_emsc->boardType, pwm_joint, pwm_motor, &alarm_mask);
    
    // alarm mask is now the mask of current-limited motors
    
    s_emsc->limited_motors_mask_changed = (s_emsc->limited_motors_mask != alarm_mask);
    
    s_emsc->limited_motors_mask = alarm_mask;
    
    JOINTS(j)
    {
        if (stop_mask & (1<<j))
        {
            eo_emsController_SetControlMode(j, eomc_controlmode_cmd_switch_everything_off, eobool_false);
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

/*
extern void eo_emsController_GetPosRef(int32_t* pos, int32_t* avg_vel)
{
    if (!s_emsc) return;
    
    JOINTS(j) eo_axisController_GetPosRef(s_emsc->axis_controller[j], &(pos[j]), &(avg_vel[j])); 
}
*/

extern void eo_emsController_SetPosRef(uint8_t joint, int32_t pos, int32_t avg_vel)
{
    if (s_emsc) 
    {        
        eo_axisController_SetPosRef(s_emsc->axis_controller[joint], pos, avg_vel);       
    }
}

extern void eo_emsController_SetPosRaw(uint8_t joint, int32_t pos)
{
    if (s_emsc) 
    {        
        eo_axisController_SetPosRaw(s_emsc->axis_controller[joint], pos);       
    }
}

extern void eo_emsController_SetVelRef(uint8_t joint, int32_t vel, int32_t avg_acc)
{
    if (s_emsc) eo_axisController_SetVelRef(s_emsc->axis_controller[joint], vel, avg_acc);
}

extern void eo_emsController_SetTrqRef(uint8_t joint, int32_t trq)
{
    if (s_emsc) eo_axisController_SetTrqRef(s_emsc->axis_controller[joint], trq);
}

extern void eo_emsController_SetControlMode(uint8_t joint, eOmc_controlmode_command_t mode, eObool_t twoFOC_off)
{
    if (!s_emsc) return;
    
    static eObool_t motors_not_configured = eobool_true;
    
    if (motors_not_configured)
    {
        motors_not_configured = eobool_false;
        MOTORS(m) config_2FOC(m);   
    }
    
    eo_axisController_SetControlMode(s_emsc->axis_controller[joint], mode);
    
    if (mode == eomc_controlmode_cmd_switch_everything_off)
    {
        if (twoFOC_off)
        {
            switch (s_emsc->boardType)
            {
            case EMS_GENERIC: return;
        
            case EMS_UPPERLEG:
            case EMS_ANKLE:
                set_2FOC_idle(joint);
                break;
    
            case EMS_SHOULDER:
                if (joint == 3)
                {
                    set_2FOC_idle(3);
                    break;
                }
        
            case EMS_WAIST:
                if (eo_emsController_GetControlMode(0) == eomc_controlmode_idle &&
                    eo_emsController_GetControlMode(1) == eomc_controlmode_idle &&
                    eo_emsController_GetControlMode(2) == eomc_controlmode_idle)
                {
                    set_2FOC_idle(0);
                    set_2FOC_idle(1);
                    set_2FOC_idle(2);
                }
                break;
            }
        }
    }
    else
    {
        switch (s_emsc->boardType)
        {
        case EMS_GENERIC: return;
        
        case EMS_UPPERLEG:
        case EMS_ANKLE:
            set_2FOC_running(joint, mode);
            break;

        case EMS_SHOULDER:
            if (joint == 3)
            {
                set_2FOC_running(3, mode);
                break;
            }
    
        case EMS_WAIST:
            set_2FOC_running(0, mode);
            set_2FOC_running(1, mode);
            set_2FOC_running(2, mode);
            break;
        }
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
    
    switch (s_emsc->boardType)
    {    
    case EMS_UPPERLEG:
    case EMS_ANKLE:
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
        return;
   
    case EMS_SHOULDER:
        if (eo_axisController_IsCalibrated(s_emsc->axis_controller[3]))
        {
            s_emsc->n_calibrated++;
        }
        else if (eo_speedometer_IsOk(s_emsc->enc_speedometer[3]))
        {    
            s_emsc->n_calibrated++;
            
            eo_axisController_SetCalibrated(s_emsc->axis_controller[3]);
        }
    case EMS_WAIST:
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
    }    
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
extern void eo_emsController_SetPosPid(uint8_t joint, float K, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff)
{
    if (s_emsc) eo_axisController_SetPosPid(s_emsc->axis_controller[joint], K, Kd, Ki, Imax, Ymax, 0);    
}   
extern void eo_emsController_SetTrqPid(uint8_t joint, float K, float Kd, float Ki, float Imax, int32_t Ymax, int32_t Yoff)
{
  //if (s_emsc) eo_axisController_SetTrqPid(s_emsc->axis_controller[joint], K, Kd,  Ki,  Imax, Ymax, Yoff); 
    if (s_emsc) eo_axisController_SetTrqPid(s_emsc->axis_controller[joint], K, 0.f, 0.f, 0.0f, Ymax, Yoff);
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
    if (s_emsc) eo_motor_set_motor_error_status(s_emsc->motors, motor, motorerror || canerror);
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

    msgCmd.cmdId = ICUBCANPROTO_POL_MC_CMD__CONTROLLER_IDLE;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, NULL);
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




