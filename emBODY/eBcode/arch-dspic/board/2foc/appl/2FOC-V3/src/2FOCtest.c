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

#include <p33FJ128MC802.h>
#include "2FOCtest.h"
#include "ADC.h"
#include "System.h"

volatile static char bDriveTestEnabled = FALSE;
volatile ADCtestParm_t ADCtestParm;
volatile driveTestMode_t driveTestMode;

void EnableDriveTest()
{
    // I2T will run on behalf of 2FOC loop. Stop running it in behalf of Timer 3
    Timer3Disable();
    // Arm all the automation (PWM->ADC->DMA->FOC) but don't start yet!
    ADCInterruptAndDMAEnable(DMAchannel_DMA1);
    bDriveTestEnabled = TRUE;
    // enable the overcurrent interrupt
    OverCurrentFaultIntEnable();
}

void DisableDriveTest()
// disable the drive functions:
// - disable FOC loop
{
    if (bDriveTestEnabled)
    {
        // Disable ADC IRQ and DMA
        ADCInterruptAndDMADisable(DMAchannel_DMA1);
        bDriveTestEnabled = FALSE;
        // I2T and encoder traking will run in behalf of timer 3 when 2FOC loop is not active
        Timer3Enable();
    }
}

void __attribute__((__interrupt__, no_auto_psv)) _DMA3Interrupt(void)
{
    int i;
}


//static BOOL Test_PWM_offsets(void)
//// PWM offsets test
//{
//    // Turn on PWM and set PWM = 0% for all terminals
//    pwmON();
//
//    // Measure average current on terminals A & C
//    int offsetA, offsetC; // in mA
//    //measureAverageCurrentOnSinglePhase(1,1000,&offsetA); // read phase A for 1s
//    //measureAverageCurrentOnSinglePhase(3,1000,&offsetC); // read phase C for 1s
//
//    // Log the offsets
//    I2Tdata.IQMeasured = offsetA;
//    CanIcubProtoTrasmitterSendPeriodicData();
//    I2Tdata.IQMeasured = offsetC;
//    CanIcubProtoTrasmitterSendPeriodicData();
//
//    // Check that offsets are null
//    //if () {}
//    return TRUE;
//}

