#ifndef MC_CONTROLLER___
#define MC_CONTROLLER___

#include "EoCommon.h"

#include "EOemsControllerCfg.h"

#include "Joint.h"
#include "Motor.h"
#include "AbsEncoder.h"
#include "Pid.h"
#include "JointSet.h"

typedef struct //MController
{
    uint8_t nEncods;
    uint8_t nJoints;
    uint8_t nSets;
    
    JointSet *jointSet;
    
    uint8_t* set_dim;
    uint8_t* enc_set_dim;
    
    uint8_t multi_encs;
    
    uint8_t** jos;
    uint8_t** mos;
    uint8_t** eos;
    
    uint8_t *j2s;
    uint8_t *m2s;
    uint8_t *e2s;
    
    Motor *motor;
    Joint *joint;
    
    float **Jjm;
    float **Jmj;
    
    float **Sje;
    float **Sjm;
    
    uint8_t part_type;
    uint8_t actuation_type;
    
    AbsEncoder *absEncoder;
} MController;

extern MController* MController_new(uint8_t nJoints, uint8_t nEncoders); //

extern void MController_init(void); //
extern void MController_deinit(void); //
extern void MController_config_board(const eOmn_serv_configuration_t* brd_cfg);

extern void MController_config_joint(int j, eOmc_joint_config_t* config); //
extern void MController_config_motor(int m, eOmc_motor_config_t* config); //
extern void MController_config_Jjm(float **Jjm); //
extern void MController_config_Jje(float **Jje); //

extern void MController_update_motor_state_fbk(uint8_t m, void* state);
extern void MController_update_joint_torque_fbk(uint8_t j, CTRL_UNITS trq_fbk); //
//extern void MController_update_absEncoder_fbk(uint8_t e, int32_t position); //
extern void MController_update_absEncoder_fbk(uint8_t e, uint32_t* positions); //

extern void MController_invalid_absEncoder_fbk(uint8_t e, eOencoderreader_errortype_t error_type);

extern void MController_timeout_absEncoder_fbk(uint8_t e);

extern int32_t MController_get_absEncoder(uint8_t j); //

extern void MController_do(void); //

extern BOOL MController_set_control_mode(uint8_t j, eOmc_controlmode_command_t control_mode);
extern void MController_set_interaction_mode(uint8_t j, eOmc_interactionmode_t interaction_mode);

extern void MController_calibrate(uint8_t e, eOmc_calibrator_t *calibrator);

extern void MController_go_idle(void);

////////////////////////////////////////////////////////////////////////
//extern void MController_get_motor_control_state(uint8_t m, uint8_t* control_state, uint8_t* control_state_req);
extern void MController_get_joint_state(int j, eOmc_joint_status_t* joint_state);
extern void MController_get_pid_state(int j, eOmc_joint_status_ofpid_t* pid_state, BOOL decoupled_pwm);
extern void MController_get_motor_state(int m, eOmc_motor_status_t* motor_status);
extern void MController_update_motor_pos_fbk(int m, int32_t position_raw);
extern void MController_update_motor_current_fbk(int m, int16_t current);
extern void MController_config_motor_friction(int m, eOmc_motor_params_t* friction); //
extern void MController_config_joint_impedance(int j, eOmc_impedance_t* impedance); //
extern void MController_config_pos_pid(int j, eOmc_PID_t *pid_conf); //
extern void MController_config_vel_pid(int j, eOmc_PID_t *pid_conf); //
extern void MController_config_trq_pid(int m, eOmc_PID_t *pid_conf); //
extern void MController_config_joint_pos_limits(int j, int32_t pos_min, int32_t pos_max);
extern void MController_config_joint_vel_ref_timeout(int j, int32_t timeout_ms);
extern void MController_motor_config_current_PID(int m, eOmc_PID_t* pidcurrent);
extern void MController_motor_config_max_currents(int m, eOmc_current_limits_params_t* current_params);

extern BOOL MController_set_joint_pos_ref(int j, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref);
extern BOOL MController_set_joint_vel_ref(int j, CTRL_UNITS vel_ref, CTRL_UNITS acc_ref);
extern BOOL MController_set_joint_pos_raw(int j, CTRL_UNITS pos_ref);
extern BOOL MController_set_joint_trq_ref(int j, CTRL_UNITS trq_ref);
extern BOOL MController_set_joint_out_ref(int j, CTRL_UNITS out_ref);
extern void MController_stop_joint(int j);
extern void MController_config_motor_gearbox_ratio(int m, int32_t gearbox_ratio);
extern void MController_config_motor_encoder(int m, int32_t resolution);
extern int16_t MController_config_motor_pwm_limit(int m, int16_t pwm_limit);
extern void MController_update_motor_odometry_fbk_can(int m, void* data);

extern void MController_motor_raise_fault_i2t(int m);
extern void MController_motor_raise_fault_overcurrent(int m);
//extern void MController_motor_raise_fault_external(int m);
//extern BOOL MController_motor_is_external_fault(int m);

////////////////////////////////////////////////////////////////////////

//VALE: debug function. I'll remove it ASAP
//void MController_updated_debug_current_info(int j, int32_t avgCurrent, int32_t accum_Ep);
#endif
