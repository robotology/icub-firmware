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
#include "Faults.h"
#include "Park.h"
#include "System.h"

volatile static char bDriveTestEnabled = FALSE;
volatile driveTestParm_t driveTestParams;

static void initTestMode(driveTestMode_t testMode);
static void terminateTestMode(void);


void EnableDriveTest(driveTestMode_t testMode)
{
    initTestMode(testMode);
    // I2T will run on behalf of 2FOC loop. Stop running it in behalf of Timer 3
    Timer3Disable();
    // Arm all the automation (PWM->ADC->DMA->FOC) but don't start yet!
    ADCInterruptAndDMAEnable(DMAchannel_DMA3);
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
        ADCInterruptAndDMADisable(DMAchannel_DMA3);
        bDriveTestEnabled = FALSE;
        // I2T and encoder traking will run in behalf of timer 3 when 2FOC loop is not active
        Timer3Enable();
    }
    terminateTestMode();
}

void __attribute__((__interrupt__, no_auto_psv)) _DMA3Interrupt(void)
{
    switch (driveTestParams.mode)
    {
        case driveTestMode_HES_ADC_OFFSET:
            MeasAndCompIaIcCalculateIb(); // Read the calibrated currents Ia, Ic
            driveTestParams.ADC.currOffsetA += (long)ParkParm.qIa; // cumulated current measurements
            driveTestParams.ADC.currOffsetC += (long)ParkParm.qIc; // cumulated current measurements
            break;
            
        case driveTestMode_HES_ADC_GAINS:
            MeasAndCompIaIcCalculateIb(); // Read the calibrated currents Ia, Ic
            long Ia_plus_Ic = (long)ParkParm.qIa + (long)ParkParm.qIc;
            long Ia_plus_Ic_squared = __builtin_mulss(Ia_plus_Ic,Ia_plus_Ic);
            driveTestParams.ADC.diffActPhaseCurrMean += Ia_plus_Ic; // cumulated Ia+Ic
            driveTestParams.ADC.diffActPhaseCurrSTD += Ia_plus_Ic_squared; // cumulated (Ia+Ic)^2)
            break;
            
        default:
            // Unsupported test mode
            SysError.ADCCalFailure = 1;
            break;
    }

    /*
     * Clear the DMA0 Interrupt Flag
     */
    IFS2bits.DMA3IF = 0;
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

static void initTestMode(driveTestMode_t testMode) {
    driveTestParams.mode = testMode;
    switch (testMode)
    {
        case driveTestMode_HES_ADC_OFFSET:
            driveTestParams.ADC.nSamples = 0;
            driveTestParams.ADC.currOffsetA = 0;
            driveTestParams.ADC.currOffsetC = 0;
            break;
            
        case driveTestMode_HES_ADC_GAINS:
            driveTestParams.ADC.nSamples = 0;
            driveTestParams.ADC.diffActPhaseCurrMean = 0;
            driveTestParams.ADC.diffActPhaseCurrSTD = 0;
            break;
            
        default:
            // Unsupported test mode
            driveTestParams.mode = driveTestMode_NONE;
            SysError.ADCCalFailure = 1;
            break;
    }
}

static void terminateTestMode()
{
    switch (driveTestParams.mode)
    {
        case driveTestMode_HES_ADC_OFFSET:
            driveTestParams.ADC.currOffsetA = __builtin_divsd(driveTestParams.ADC.currOffsetA,driveTestParams.ADC.nSamples);
            driveTestParams.ADC.currOffsetC = __builtin_divsd(driveTestParams.ADC.currOffsetC,driveTestParams.ADC.nSamples);
            break;
            
        case driveTestMode_HES_ADC_GAINS:
            driveTestParams.ADC.diffActPhaseCurrSTD = __builtin_divsd(
                    driveTestParams.ADC.diffActPhaseCurrSTD-driveTestParams.ADC.diffActPhaseCurrMean,
                    driveTestParams.ADC.nSamples);
            driveTestParams.ADC.diffActPhaseCurrMean = __builtin_divsd(driveTestParams.ADC.diffActPhaseCurrMean,driveTestParams.ADC.nSamples);
            break;
            
        default:
            // Unsupported test mode
            SysError.ADCCalFailure = 1;
            break;
    }
    
    driveTestParams.mode = driveTestMode_NONE;
}

