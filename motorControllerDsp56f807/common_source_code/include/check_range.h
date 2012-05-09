//# Copyright: (C) 2007-2010 RobotCub Consortium
//# Authors: Marco Maggiali, Marco Randazzo
//# CopyPolicy: Released under the terms of the GNU GPL v2.0.

#ifndef __CHECK_RANGE_H__
#define __CHECK_RANGE_H__

#include "dsp56f807.h"

void check_range        (byte, Int16, Int32 *);
void check_range_torque (byte, Int16, Int32 *);
void check_desired_within_limits(byte i, Int32 previous_desired);

#endif 
