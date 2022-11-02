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

#if defined(WRIST_MK2) || defined(ERGOJOINT)
#include "JointSet.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include "EOemsControllerCfg.h"

#if !defined(WRIST_MK2) && !defined(ERGOJOINT)
#include "JointSet.h"
#endif

extern BOOL JointSet_do_wait_calibration_3(JointSet* o);
extern BOOL JointSet_do_wait_calibration_5(JointSet* o);
extern BOOL JointSet_do_wait_calibration_8(JointSet* o);
extern BOOL JointSet_do_wait_calibration_9(JointSet* o);
extern BOOL JointSet_do_wait_calibration_10(JointSet* o);
extern BOOL JointSet_do_wait_calibration_11(JointSet* o);
extern BOOL JointSet_do_wait_calibration_12(JointSet* o);
extern BOOL JointSet_do_wait_calibration_13(JointSet* o);
extern BOOL JointSet_do_wait_calibration_mixed(JointSet* o); //calib type 6 and 7


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


