#ifndef __control_enable_h__
#define __control_enable_h__

#include "dsp56f807.h"
extern bool _can_protocol_ack;

void enable_motor_pwm (char axis);
void disable_motor_pwm (char axis);
void enable_control (char axis);
void disable_control (char axis);
void set_control_mode(char axis);

#endif