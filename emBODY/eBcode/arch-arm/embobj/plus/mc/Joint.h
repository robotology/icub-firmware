#ifndef MC_JOINT___
#define MC_JOINT___

#include "EoCommon.h"

#include "EOemsControllerCfg.h"

#include "Pid.h"
#include "Trajectory.h"
#include "WatchDog.h"

typedef union
{
    struct
    {
        uint8_t torque_sensor_timeout:1;
    } bits;
        
    uint8_t bitmask;
} JointFaultState;

typedef struct // Joint
{
    uint8_t ID;
    
    PID posPID;
    
    CTRL_UNITS pos_min;
    CTRL_UNITS pos_max;
    CTRL_UNITS vel_max;
    CTRL_UNITS acc_max;
    //CTRL_UNITS trq_max;
    //CTRL_UNITS out_max;
    
    CTRL_UNITS output_lim;
    
    CTRL_UNITS out_ref;
    CTRL_UNITS pos_ref;
    CTRL_UNITS vel_ref;
    CTRL_UNITS acc_ref;
    CTRL_UNITS trq_ref;
    
    CTRL_UNITS vel_dir_ref;
    
    CTRL_UNITS pos_fbk;
    CTRL_UNITS vel_fbk;
    CTRL_UNITS trq_fbk;    
    
    CTRL_UNITS pos_fbk_from_motors;
    CTRL_UNITS vel_fbk_from_motors;
    
    CTRL_UNITS pos_err;
    CTRL_UNITS vel_err;
    CTRL_UNITS trq_err;    
    
    CTRL_UNITS output;
        
    CTRL_UNITS tcKoffset;
    CTRL_UNITS tcKstiff;
    CTRL_UNITS tcKdamp;
    
    CTRL_UNITS scKpos;
    CTRL_UNITS scKvel;
    CTRL_UNITS scKstill;
    
    CTRL_UNITS Kadmitt;
    
    WatchDog trq_fbk_wdog;
    WatchDog vel_ref_wdog;
    
    Trajectory trajectory;
    
    eOmc_controlmode_t     control_mode;
    eOmc_interactionmode_t interaction_mode;
    
    uint8_t MOTOR_CONTROL_TYPE;
    
    int8_t pushing_limit;
    
    BOOL pos_control_active;
    BOOL trq_control_active;
    BOOL CAN_DO_TRQ_CTRL;
    
    
    
    JointFaultState fault_state_prec;
    JointFaultState fault_state;
    uint16_t diagnostics_refresh;
} Joint;

extern Joint* Joint_new(uint8_t n);
extern void Joint_init(Joint* o);
extern void Joint_destroy(Joint* o);

extern void Joint_config(Joint* o, uint8_t ID, eOmc_joint_config_t* config);

extern void Joint_motion_reset(Joint *o);

extern void Joint_update_odometry_fbk(Joint* o, CTRL_UNITS pos_fbk, CTRL_UNITS vel_fbk);
extern void Joint_update_torque_fbk(Joint* o, CTRL_UNITS trq_fbk);

extern BOOL Joint_check_faults(Joint* o);
extern void Joint_clear_faults(Joint* o);

extern int8_t Joint_check_limits(Joint* o);
extern int8_t Joint_pushing_limit(Joint* o);

extern void Joint_set_limits(Joint* o, CTRL_UNITS pos_min, CTRL_UNITS pos_max);

extern CTRL_UNITS Joint_do_pwm_control(Joint* o);
extern CTRL_UNITS Joint_do_vel_control(Joint* o);

extern BOOL Joint_set_control_mode(Joint* o, eOmc_controlmode_command_t control_mode);
extern BOOL Joint_set_interaction_mode(Joint* o, eOmc_interactionmode_t interaction_mode);

extern void Joint_set_impedance(Joint* o, eOmc_impedance_t* impedance);
extern void Joint_get_impedance(Joint* o, eOmc_impedance_t* impedance);

extern void Joint_get_state(Joint* o, eOmc_joint_status_t* joint_state);

extern BOOL Joint_get_pid_state(Joint* o, eOmc_joint_status_ofpid_t* pid_state);

extern BOOL Joint_set_pos_ref(Joint* o, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref);
extern BOOL Joint_set_vel_ref(Joint* o, CTRL_UNITS vel_ref, CTRL_UNITS acc_ref);
extern BOOL Joint_set_pos_raw(Joint* o, CTRL_UNITS pos_ref);
extern BOOL Joint_set_trq_ref(Joint* o, CTRL_UNITS trq_ref);
extern BOOL Joint_set_out_ref(Joint* o, CTRL_UNITS out_ref);
extern void Joint_stop(Joint* o);
#endif
