/*
 * Copyright (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or  FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License foFITNESSr more details
*/

#ifndef MC_JOINT_SET___
#define MC_JOINT_SET___

#ifdef WRIST_MK2
#include "wrist_decoupler.h"
#endif

#include "EoCommon.h"

#include "EOemsControllerCfg.h"

#include "Joint.h"
#include "Motor.h"
#include "AbsEncoder.h"

typedef struct JointSet_hid JointSet;

extern JointSet* JointSet_new(uint8_t n); //

extern void JointSet_init(JointSet* o); //

extern void JointSet_config //
(
    JointSet* o,
    uint8_t* pN,
    uint8_t* pE,
    uint8_t* joints_of_set,
    uint8_t* motors_of_set,
    uint8_t* encoders_of_set,
    Joint* joint,
    Motor* motor,
    AbsEncoder *absEncoder,
    float** Jjm, float** Jjm_hard,
    float** Jmj,
    float** Sje,
    float** Sjm
);

extern void JointSet_do_odometry(JointSet* o); //
extern BOOL JointSet_do_check_faults(JointSet* o); //
extern void JointSet_do_control(JointSet* o); //

extern void JointSet_do(JointSet* o);
    
extern BOOL JointSet_set_control_mode(JointSet* o, eOmc_controlmode_command_t control_mode_cmd);
extern void JointSet_set_interaction_mode(JointSet* o, eOmc_interactionmode_t interaction_mode);

extern void JointSet_calibrate(JointSet* o, uint8_t e, eOmc_calibrator_t *calibrator);

extern void JointSet_do_pwm_control(JointSet* o);
    
extern void JointSet_send_debug_message(char *message, uint8_t jid, uint16_t par16, uint64_t par64);
extern void JointSet_enabled_sw_limit_protection(Joint* pJoint);

#ifdef WRIST_MK2
extern BOOL JointSet_set_pos_ref(JointSet* o, int j, CTRL_UNITS pos_ref, CTRL_UNITS vel_ref); //used only for WRIST_MK2
extern void JointSet_get_state(JointSet* o, int j, eOmc_joint_status_t* joint_state);  //used only for WRIST_MK2
extern void JointSet_stop(JointSet* o, int j);  //used only for WRIST_MK2
extern void JointSet_update_status_reference(JointSet* o, Joint* j_ptr, int j); //used only for WRIST_MK2
#endif
extern void JointSet_set_constraints(JointSet* o, const eOmc_jointSet_constraints_t *constraints);
extern void JointSet_init_wrist_decoupler(JointSet* o);
 
#endif  // include-guard


