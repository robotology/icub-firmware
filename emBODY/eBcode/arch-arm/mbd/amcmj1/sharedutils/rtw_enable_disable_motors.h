/*
 * Copyright (C) 2021 iCub Tech Facility - Istituto Italiano di Tecnologia
 * Author:  German David Rodriguez Arenas
 * email:   german.rodriguez@iit.it
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#ifndef RTW_ENABLE_DISABLE_MOTORS
#define RTW_ENABLE_DISABLE_MOTORS

#ifdef __cplusplus
    extern "C" {
#endif /* __cplusplus */

void rtw_enableMotor();

void rtw_disableMotor();

#ifdef __cplusplus
    } /* extern "C" */
#endif /* __cplusplus */        

#endif /* RTW_ENABLE_DISABLE_MOTORS */