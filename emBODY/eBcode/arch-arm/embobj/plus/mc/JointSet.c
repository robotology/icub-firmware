#include "EoCommon.h"

#include "EOtheMemoryPool.h"

#include "EOemsControllerCfg.h"

#include "Joint.h"
#include "Motor.h"
#include "AbsEncoder.h"
#include "Pid.h"

#include "JointSet.h"

#include "hal_led.h"

JointSet* JointSet_new(uint8_t n) //
{
    JointSet* o = NEW(JointSet, n);
    
    for (int i=0; i<n; ++i)
    {   
        JointSet_init(o+i);
    }
    
    return o;
}

void JointSet_init(JointSet* o) //
{   
    o->pN = NULL;

    o->joints_of_set = NULL;
    o->motors_of_set = NULL;
    
    o->joint = NULL;
    o->motor = NULL;
       
    o->Jjm = NULL;
    o->Jmj = NULL;
    
    o->Sje = NULL;
    o->Sjm = NULL;
    
    o->absEncoder = NULL;
    
    o->control_mode     = eomc_controlmode_notConfigured;
    o->interaction_mode = eOmc_interactionmode_stiff;
    
    o->MOTOR_CONTROL_TYPE = PWM_CONTROLLED_MOTOR; 
    
    o->pos_control_active = TRUE;
    o->trq_control_active = FALSE; 
    o->CAN_DO_TRQ_CTRL = TRUE;
    
    o->is_calibrated = FALSE;
}

void JointSet_config //
(
    JointSet* o,
    uint8_t* pN,
    uint8_t* joints_of_set,
    uint8_t* motors_of_set,
    uint8_t* encoders_of_set,
    Joint* joint, 
    Motor* motor, 
    float** Jjm,
    float** Jmj,
    float** Sje,
    float** Sjm,
    AbsEncoder *absEncoder
)
{
    o->pN = pN;
    o->joints_of_set = joints_of_set;
    o->motors_of_set = motors_of_set;
    o->encoders_of_set = encoders_of_set;
    o->joint = joint;
    o->motor = motor;
    o->Jjm = Jjm;
    o->Jmj = Jmj;
    o->Sje = Sje;
    o->Sjm = Sjm;
    o->absEncoder = absEncoder;
}

void JointSet_do_odometry(JointSet* o) //
{
    int js, j;
    int ms, m;

    int N = *(o->pN);
            
    float **Sjm = o->Sjm;
    
    if (Sjm)
    {
        for (js=0; js<N; ++js)
        {
            j = o->joints_of_set[js];

            o->joint[j].pos_fbk_from_motors = ZERO;
            o->joint[j].vel_fbk_from_motors = ZERO;
            
            for (ms=0; ms<N; ++ms)
            {
                m = o->motors_of_set[ms];
        
                o->joint[j].pos_fbk_from_motors += Sjm[j][m] * o->motor[m].pos_fbk;
                o->joint[j].vel_fbk_from_motors += Sjm[j][m] * o->motor[m].vel_fbk;
            }
        }
    }
    else
    {
        for (js=0; js<N; ++js)
        {
            j = o->joints_of_set[js];
            
            o->joint[j].pos_fbk_from_motors = o->motor[j].pos_fbk;
            o->joint[j].vel_fbk_from_motors = o->motor[j].vel_fbk;
        }
    }
    
    if (!o->Sje) // no encoder coupling
    {
        for (js=0; js<N; ++js)
        {    
            j = o->encoders_of_set[js];
        
            if (AbsEncoder_is_fake(o->absEncoder+j))
            {
                o->joint[j].pos_fbk = o->joint[j].pos_fbk_from_motors;
                o->joint[j].vel_fbk = o->joint[j].vel_fbk_from_motors;
            }
            else
            {
                o->joint[j].pos_fbk = AbsEncoder_position(o->absEncoder+j);
                
                #ifdef USE_SPEED_FBK_FROM_MOTORS
                o->joint[j].vel_fbk = o->joint[j].vel_fbk_from_motors;
                #else
                o->joint[j].vel_fbk = AbsEncoder_velocity(o->absEncoder+j);
                #endif
            }
        }
        
        return;
    }
    
    // encoder coupling
    {
        float pos[MAX_PER_BOARD];
        float vel[MAX_PER_BOARD];
        
        int es, e;
        
        for (es=0; es<N; ++es)
        {    
            e = o->encoders_of_set[es];
        
            if (AbsEncoder_is_fake(o->absEncoder+e))
            {
                pos[e] = o->joint[e].pos_fbk_from_motors;
                vel[e] = o->joint[e].vel_fbk_from_motors;
            }
            else
            {
                pos[e] = AbsEncoder_position(o->absEncoder+e);
                
                #ifdef USE_SPEED_FBK_FROM_MOTORS
                vel[e] = o->joint[e].vel_fbk_from_motors;
                #else
                vel[e] = AbsEncoder_velocity(o->absEncoder+e);
                #endif
            }
        }
    
        float **Sje = o->Sje;
        
        for (js=0; js<N; ++js)
        {
            j = o->joints_of_set[js];
        
            o->joint[j].pos_fbk = ZERO;
            o->joint[j].vel_fbk = ZERO;
            
            for (es=0; es<N; ++es)
            {
                e = o->encoders_of_set[es];
                
                o->joint[j].pos_fbk += Sje[j][e] * pos[e];
                o->joint[j].vel_fbk += Sje[j][e] * vel[e];
            }
        }
    }
}

BOOL JointSet_do_check_faults(JointSet* o)
{
    int N = *(o->pN);
    
    BOOL fault = FALSE;
    o->external_fault = FALSE;
    
    for (int k=0; k<N; ++k)
    {
        if (Joint_check_faults(o->joint+o->joints_of_set[k]))
        {
            fault = TRUE;
        }
        
        if (Motor_check_faults(o->motor+o->motors_of_set[k]))
        {
            fault = TRUE;
            
            // TODOALE
            // traditional confusion among joints, motors and encoders: I don't like it
            o->joint[o->joints_of_set[k]].control_mode = eomc_controlmode_hwFault;
        }
        
        if (Motor_is_external_fault(o->motor+o->motors_of_set[k]))
        {
            o->external_fault = TRUE;
        }
        
        if (AbsEncoder_is_in_fault(o->absEncoder+o->encoders_of_set[k]))
        {
            fault = TRUE;
            
            // TODOALE
            // traditional confusion among joints, motors and encoders: I don't like it
            o->joint[o->joints_of_set[k]].control_mode = eomc_controlmode_hwFault;
        }
    }
    
    if (fault)
    {
        for (int k=0; k<N; ++k)
        {
            Joint_set_control_mode(o->joint+o->joints_of_set[k], eomc_controlmode_cmd_idle);
            
            Motor_set_idle(o->motor+o->motors_of_set[k]);
        }
        
        o->control_mode = eomc_controlmode_hwFault;
    }
    else if (o->external_fault)
    {
        for (int k=0; k<N; ++k)
        {
            Joint_set_control_mode(o->joint+o->joints_of_set[k], eomc_controlmode_cmd_idle);
            
            Motor_set_idle(o->motor+o->motors_of_set[k]);
        }
        
        o->control_mode = eomc_controlmode_idle;    
    }
    
    return fault;
}

static void JointSet_do_pwm_control(JointSet* o);
static void JointSet_do_vel_control(JointSet* o);

void JointSet_do_control(JointSet* o)
{
    switch (o->MOTOR_CONTROL_TYPE)
    {
    case PWM_CONTROLLED_MOTOR:
        JointSet_do_pwm_control(o);
        return;
    
    case VEL_CONTROLLED_MOTOR:
        JointSet_do_vel_control(o);
        return;
    
    case IQQ_CONTROLLED_MOTOR:
        //JointSet_do_trq_control(o);
        return;
    }
}

static void JointSet_do_wait_calibration(JointSet* o);

void JointSet_do(JointSet* o)
{
    if (o->is_calibrated)
    {            
        JointSet_do_odometry(o);
    
        JointSet_do_check_faults(o);
    
        JointSet_do_control(o);
    }
    else
    {
        JointSet_do_wait_calibration(o);
        
        for (int ms=0; ms<*(o->pN); ++ms)
        {
            int m = o->motors_of_set[ms];
            
            o->motor[m].output = 0;
        }
    }
}

static void JointSet_set_inner_control_flags(JointSet* o);

BOOL JointSet_set_control_mode(JointSet* o, eOmc_controlmode_command_t control_mode)
{
    if (control_mode == o->control_mode) return TRUE;
    
    if (o->control_mode == eomc_controlmode_calib) return FALSE;
    
    if (o->control_mode == eomc_controlmode_notConfigured) return FALSE;
    
    if (control_mode == eomc_ctrlmval_openloop && o->MOTOR_CONTROL_TYPE != PWM_CONTROLLED_MOTOR) return FALSE;
    
    if (control_mode == eomc_controlmode_torque && !o->CAN_DO_TRQ_CTRL) return FALSE;
    
    int N = *(o->pN);
    
    if (o->control_mode == eomc_controlmode_hwFault)
    {
        if (control_mode == eomc_controlmode_cmd_force_idle)
        {
            for (int k=0; k<N; ++k)
            {   
                Motor_force_idle(o->motor+o->motors_of_set[k]);
                
                AbsEncoder_clear_faults(o->absEncoder+o->encoders_of_set[k]);
                
                Joint_set_control_mode(o->joint+o->joints_of_set[k], eomc_controlmode_cmd_force_idle);
            }
        }
        else
        {
            return FALSE;
        }
    }
    
    switch (control_mode)
    {
    case eomc_controlmode_cmd_force_idle:
    case eomc_controlmode_cmd_idle:
        for (int k=0; k<N; ++k)
        { 
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);
            
            Motor_set_idle(o->motor+o->motors_of_set[k]);
            Joint_set_control_mode(o->joint+o->joints_of_set[k], control_mode);
        }
        break;
    
    case eomc_controlmode_cmd_openloop:
        if (o->external_fault) return FALSE;
        for (int k=0; k<N; ++k)
        { 
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);
            
            Motor_set_run(o->motor+o->motors_of_set[k]);
            Joint_set_control_mode(o->joint+o->joints_of_set[k], control_mode);
        }
        break;
        
    case eomc_controlmode_cmd_torque:
        if (o->external_fault) return FALSE;
        for (int k=0; k<N; ++k)
        { 
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);
            
            Motor_set_run(o->motor+o->motors_of_set[k]);
            Joint_set_control_mode(o->joint+o->joints_of_set[k], control_mode);
        }    
        break;
    
    case eomc_controlmode_direct:
    case eomc_controlmode_cmd_mixed:
    case eomc_controlmode_cmd_position:
    case eomc_controlmode_cmd_velocity:
        if (o->external_fault) return FALSE;
        for (int k=0; k<N; ++k)
        { 
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);
            
            Motor_set_run(o->motor+o->motors_of_set[k]);
            Joint_set_control_mode(o->joint+o->joints_of_set[k], control_mode);
        }
        break;
        
    default:
        return FALSE;
    }
    
    o->control_mode = (eOmc_controlmode_t)control_mode;
    
    JointSet_set_inner_control_flags(o);
    
    return TRUE;
}

void JointSet_set_interaction_mode(JointSet* o, eOmc_interactionmode_t interaction_mode)
{
    if (interaction_mode == o->interaction_mode) return;
    
    o->interaction_mode = interaction_mode;
    
    JointSet_set_inner_control_flags(o);
    
    int N = *(o->pN);
    
    if (interaction_mode == eOmc_interactionmode_stiff)
    {        
        for (int k=0; k<N; ++k)
        {
            Motor_motion_reset(o->motor+o->motors_of_set[k]);
            Joint_motion_reset(o->joint+o->joints_of_set[k]);
        }
    }
    
    for (int k=0; k<N; ++k)
    {
        Joint_set_interaction_mode(o->joint+o->joints_of_set[k], interaction_mode);
    }
}

//////////////////////////////////////////////////////////////////////////
// statics

static void JointSet_do_pwm_control(JointSet* o)
{
    int N = *(o->pN);
        
    BOOL limits_torque_protection = FALSE;
        
    for (int js=0; js<N; ++js)
    {
        Joint *pJoint = o->joint+o->joints_of_set[js];
        
        Joint_do_pwm_control(pJoint);
       
        if (o->trq_control_active && Joint_pushing_limit(pJoint))
        {
            limits_torque_protection = TRUE;
        }
    }
    
    CTRL_UNITS motor_pwm_ref = ZERO;
    
    for (int ms=0; ms<N; ++ms)
    {
        int m = o->motors_of_set[ms];
    
        if (o->trq_control_active)
        {   
            if (o->Jjm)
            {
                CTRL_UNITS motor_trq_ref = ZERO;
                CTRL_UNITS motor_trq_fbk = ZERO;
            
                for (int js=0; js<N; ++js)
                {
                    int j = o->joints_of_set[js];
                
                    // mu = Jt Tau 
                    // transposed direct Jacobian
                    motor_trq_ref += o->Jjm[j][m]*o->joint[j].trq_ref;
                    motor_trq_fbk += o->Jjm[j][m]*o->joint[j].trq_fbk;
                }
                
                motor_pwm_ref = Motor_do_trq_control(o->motor+m, motor_trq_ref, motor_trq_fbk);
            }
            else
            {
                motor_pwm_ref = Motor_do_trq_control(o->motor+m, o->joint[m].trq_ref, o->joint[m].trq_fbk);
            }
        }
        else
        {
            if (o->Jmj)
            {
                motor_pwm_ref = ZERO;
            
                for (int js=0; js<N; ++js)
                {
                    int j = o->joints_of_set[js];
                
                    // inverse Jacobian
                    motor_pwm_ref += o->Jmj[m][j]*o->joint[j].output;
                }
            }
            else
            {
                motor_pwm_ref = o->joint[m].output;
            }
        }

        Motor_set_pwm_ref(o->motor+m, motor_pwm_ref);
    }
    
    if (limits_torque_protection)
    {
        CTRL_UNITS joint_pwm_ref[MAX_PER_BOARD];
        
        for (int js=0; js<N; ++js)
        {
            int j = o->joints_of_set[js];
            
            if (o->Jmj)
            {
                joint_pwm_ref[j] = ZERO;
            
                for (int ms=0; ms<N; ++ms)
                {
                    int m = o->motors_of_set[ms];
                
                    // transposed inverse Jacobian
                    joint_pwm_ref[j] += o->Jmj[m][j]*o->motor[m].pwm_ref;
                }
            }
            else
            {
                joint_pwm_ref[j] = o->motor[j].pwm_ref;
            }
            
            if (Joint_pushing_limit(o->joint+j))
            {
                if ((o->joint[j].output_lim > ZERO) ^ (o->joint[j].output_lim < joint_pwm_ref[j]))
                {
                    joint_pwm_ref[j] = o->joint[j].output_lim; 
                }
            }
        }
        
        for (int ms=0; ms<N; ++ms)
        {
            int m = o->motors_of_set[ms];
        
            if (o->Jjm)
            {
                motor_pwm_ref = ZERO;
            
                for (int js=0; js<N; ++js)
                {
                    int j = o->joints_of_set[js];
                
                    // transposed jacobian
                    motor_pwm_ref += o->Jjm[j][m]*joint_pwm_ref[j];
                }
            
                Motor_set_pwm_ref(o->motor+m, motor_pwm_ref);
            }
            else
            {
                Motor_set_pwm_ref(o->motor+m, joint_pwm_ref[m]);
            }
        }
    }
}

static void JointSet_do_vel_control(JointSet* o)
{
    int N = *(o->pN);
        
    for (int js=0; js<N; ++js)
    {
        Joint_do_vel_control(o->joint+o->joints_of_set[js]);
    }
    
    for (int ms=0; ms<N; ++ms)
    {
        int m = o->motors_of_set[ms];
    
        if (o->Jmj)
        {
            float **Ji = o->Jmj; // inverse Jacobian
            
            CTRL_UNITS motor_vel_ref = ZERO;
        
            for (int js=0; js<N; ++js)
            {
                int j = o->joints_of_set[js];
                
                motor_vel_ref += Ji[m][j]*o->joint[j].output;
            }
        
            Motor_set_vel_ref(o->motor+m, motor_vel_ref);
        }
        else
        {
            Motor_set_vel_ref(o->motor+m, o->joint[m].output);
        }
    }
}

static void JointSet_do_wait_calibration(JointSet* o)
{
    int N = *(o->pN);
    
    for (int ms=0; ms<N; ++ms)
    {
        if (!Motor_is_calibrated(o->motor+o->motors_of_set[ms])) return;
    }
    
    for (int es=0; es<N; ++es)
    {
        if (!AbsEncoder_is_calibrated(o->absEncoder+o->encoders_of_set[es])) return;
    }
    
    o->is_calibrated = TRUE;
    
    //if (o->control_mode == eomc_controlmode_calib)
    {
        for (int js=0; js<N; ++js)
        {
            o->joint[o->joints_of_set[js]].control_mode = eomc_controlmode_idle;
        }
    }
    
    o->control_mode = eomc_controlmode_idle;
    
    JointSet_set_control_mode(o, eomc_controlmode_cmd_position);
}

static void JointSet_set_inner_control_flags(JointSet* o)
{
    switch (o->control_mode)
    {
        case eomc_controlmode_position:
        case eomc_controlmode_velocity:
        case eomc_controlmode_mixed:
        case eomc_controlmode_direct:
            o->pos_control_active = TRUE;
        break;
        
        default:
            o->pos_control_active = FALSE;
    }
    
    if (o->control_mode==eomc_controlmode_torque)
    {
        o->trq_control_active = TRUE;
    }
    else if (o->pos_control_active && (o->interaction_mode==eOmc_interactionmode_compliant))
    {
        o->trq_control_active = TRUE;
    }
    else
    {
        o->trq_control_active = FALSE;
    }
}


#if 0
typedef struct                  // size is 1+3+4*4 = 20
{
    eOenum08_t                  type;                               /**< use eOmc_calibration_type_t */
    uint8_t                     filler03[3];
    union
    {
        uint32_t                                                any[6];
        eOmc_calibrator_params_type0_hard_stops_t               type0;
        eOmc_calibrator_params_type1_abs_sens_analog_t          type1;
        eOmc_calibrator_params_type2_hard_stops_diff_t          type2;
        eOmc_calibrator_params_type3_abs_sens_digital_t         type3;
        eOmc_calibrator_params_type4_abs_and_incremental_t      type4;
        eOmc_calibrator_params_type5_hard_stops_mc4plus_t       type5;
        eOmc_calibrator_params_type6_mais_t                     type6;
        eOmc_calibrator_params_type7_hall_sensor_t              type7;
        eOmc_calibration_type8_adc_and_incr_mc4plus_t           type8;
    } params;                                                       /**< the params of the calibrator */   
} eOmc_calibrator32_t;           EO_VERIFYsizeof(eOmc_calibrator32_t, 28);
typedef eOmc_calibrator32_t eOmc_calibrator_t;
#endif

void JointSet_calibrate(JointSet* o, uint8_t e, eOmc_calibrator_t *calibrator)
{
    switch (calibrator->type)
    {
        /*
        case eomc_calibration_type3_abs_sens_digital:
            AbsEncoder_calibrate(o->absEncoder+e, calibrator->params.type3.calibrationZero);
            Motor_config_pos_offset(o->motor+e, 0);
            break;
        
        case eomc_calibration_type9_motor_self_calibrated:
            AbsEncoder_calibrate(o->absEncoder+e, 0);
            Motor_config_pos_offset(o->motor+e, calibrator->params.type9.calibrationZero);
            break;
        */
        
        case eomc_calibration_type3_abs_sens_digital:
            AbsEncoder_calibrate(o->absEncoder+e, 0);
            Motor_calibrate(o->motor+e, calibrator->params.type3.calibrationZero);
            break;
        
        default:
            break;
    }
    
    for (int js=0; js<*(o->pN); ++js)
    {
        o->joint[o->joints_of_set[js]].control_mode = eomc_controlmode_calib;
    }
    
    o->control_mode = eomc_controlmode_calib;
}
