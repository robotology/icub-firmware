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

#ifndef MC_CALIBRATORS_H___
#define MC_CALIBRATORS_H___

#include "JointSet.h"
#include "EoMotionControl.h"

extern void Calibrator_calibrate(JointSet* o, uint8_t e, eOmc_calibrator_t *calibrator);
extern void Calibrator_do_wait_calibration(JointSet* o);


 
#endif  // include-guard


