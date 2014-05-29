#ifndef __control_enable_h__
#define __control_enable_h__

#include "dsp56f807.h"
extern bool _can_protocol_ack;

//This are private functions to be used only inside control_enable.cpp
/*
void enable_motor_pwm (char axis);
void disable_motor_pwm (char axis);
void enable_control (char axis);
void disable_control (char axis);
*/

void set_control_mode(char axis, byte value);
void get_control_mode(char axis);
void set_control_mode_new(char axis);
void get_control_mode_new(char axis);
void set_interaction_mode(char axis);
void get_interaction_mode(char axis);
bool mode_is_impedance_position(char axis);
bool mode_is_impedance_velocity(char axis);
bool mode_is_idle (char axis);
#endif