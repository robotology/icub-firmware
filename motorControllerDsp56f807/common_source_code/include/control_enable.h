#ifndef __control_enable_h__
#define __control_enable_h__

#include "dsp56f807.h"
extern bool _can_protocol_ack;

byte helper_controlmode_api_to_fw (byte mode);
byte helper_controlmode_fw_to_api (byte mode);

void can_set_control_mode       (byte axis);
void can_get_control_mode       (byte axis);
void can_set_interaction_mode   (byte axis);
void can_get_interaction_mode   (byte axis);

bool mode_is_impedance_position (byte axis);
bool mode_is_impedance_velocity (byte axis);
bool mode_is_idle               (byte axis);
bool mode_is_stiff              (byte axis);
bool mode_is_force_controlled   (byte axis);

void enable_motor_pwm           (byte axis, byte control_mode);
void disable_motor_pwm          (byte axis);
void switch_control_mode        (byte axis, byte value);
void switch_interaction_mode    (byte axis, byte value);
void put_motor_in_fault         (byte axis);

#endif