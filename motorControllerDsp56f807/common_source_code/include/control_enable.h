#ifndef __control_enable_h__
#define __control_enable_h__

#include "dsp56f807.h"
extern bool _can_protocol_ack;

void set_control_mode           (byte axis);
void get_control_mode           (byte axis);
void set_interaction_mode       (byte axis);
void get_interaction_mode       (byte axis);

bool mode_is_impedance_position (byte axis);
bool mode_is_impedance_velocity (byte axis);
bool mode_is_idle               (byte axis);
bool mode_is_stiff              (byte axis);
bool mode_is_force_controlled   (byte axis);

#endif