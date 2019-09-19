/*
 * Copyright (C) 2019 Center for Robotics and Intelligent Systems - Istituto Italiano di Tecnologia
 * Author: Nuno Guedelha
 * email:   nuno.guedelha@iit.it
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/
#ifndef __2FOCTEST_H_
#define __2FOCTEST_H_

#include "UserTypes.h"

typedef enum {
    driveTestMode_HES_ADC_OFFSET = 0,
    driveTestMode_HES_ADC_GAINS,
    driveTestMode_NONE
} driveTestMode_t;

typedef struct {
    long currOffsetA;
    long currOffsetC;
    int nSamples;
    long diffActPhaseCurrMean;
    long diffActPhaseCurrSTD;
} ADCtestParm_t;

typedef struct {
    driveTestMode_t mode;
    ADCtestParm_t ADC;
} driveTestParm_t;

volatile extern driveTestParm_t driveTestParams;

void EnableDriveTest(driveTestMode_t testMode);
void DisableDriveTest(void);

#endif	/* 2FOCTEST_H */

