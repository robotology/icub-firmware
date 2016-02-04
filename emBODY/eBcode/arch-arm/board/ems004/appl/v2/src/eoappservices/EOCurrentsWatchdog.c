/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Davide Pollarolo
 * email:   davide.pollarolo@iit.it
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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "EoMotionControl.h"
#include "EOtheEntities.h"
#include "EOtheMemoryPool.h"
#include "EOtheMotionController.h"
//to signal errors
#include "EOemsController.h"
#include "EOtheErrorManager.h"
#include "EoError.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOCurrentsWatchdog.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOCurrentsWatchdog_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define FILTER_WINDOW (float) 500.0 //make it configurable? --> could become a parameter from XML


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_currents_watchdog_CheckSpike(uint8_t joint, int16_t value);
static void s_eo_currents_watchdog_CheckI2T(uint8_t joint, int16_t value);
static void s_eo_currents_watchdog_UpdateMotorCurrents(uint8_t joint, int16_t value);
EO_static_inline uint32_t s_eo_currents_watchdog_averageCalc_addValue(uint8_t motor, int16_t value);
EO_static_inline void s_eo_currents_watchdog_averageCalc_reset(uint8_t motor);
EO_static_inline eObool_t s_eo_currents_watchdog_averageCalc_collectDataIsCompleted(uint8_t motor);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOCurrentsWatchdog s_eo_currents_watchdog = 
{
    .themotors       = NULL,
    .numberofmotors  = 0,
    //.filter_reg     = NULL,
    .nominalCurrent2 = NULL,
    .I2T_threshold   = NULL,
    .avgCurrent      = NULL,
    .accomulatorEp   = NULL,
    .initted         = eobool_false
};

//static const char s_eobj_ownname[] = "EOCurrentsWatchdog";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOCurrentsWatchdog* eo_currents_watchdog_Initialise(void)
{
    uint8_t m;
    //reserve memory for the number of thresholds needed
    s_eo_currents_watchdog.numberofmotors = eo_entities_NumOfMotors(eo_entities_GetHandle());
    
    if (s_eo_currents_watchdog.numberofmotors == 0)
        return NULL;

    s_eo_currents_watchdog.themotors = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(eOmc_motor_t*), s_eo_currents_watchdog.numberofmotors);
    // retrieve pointers to motors   
    for(m=0; m<s_eo_currents_watchdog.numberofmotors; m++)
    {
        s_eo_currents_watchdog.themotors[m] = eo_entities_GetMotor(eo_entities_GetHandle(), m);
    }
    
    
    s_eo_currents_watchdog.nominalCurrent2 = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(float), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.nominalCurrent2, 0, s_eo_currents_watchdog.numberofmotors*sizeof(float));
    
    s_eo_currents_watchdog.I2T_threshold = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(float), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.I2T_threshold, 0, s_eo_currents_watchdog.numberofmotors*sizeof(float));
    
    s_eo_currents_watchdog.avgCurrent = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(eoCurrentWD_averageData_t), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.avgCurrent, 0, s_eo_currents_watchdog.numberofmotors*sizeof(eoCurrentWD_averageData_t));

    s_eo_currents_watchdog.accomulatorEp = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(float), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.accomulatorEp, 0, s_eo_currents_watchdog.numberofmotors*sizeof(float));

    s_eo_currents_watchdog.initted = eobool_true;
    
    return(&s_eo_currents_watchdog);
}


extern EOCurrentsWatchdog* eo_currents_watchdog_GetHandle(void)
{
    if(eobool_false == s_eo_currents_watchdog.initted)
    {
        return(NULL);
    }
    return(&s_eo_currents_watchdog);
}

extern eOresult_t eo_currents_watchdog_UpdateCurrentLimits(EOCurrentsWatchdog* p, uint8_t motor)
{
    if (p == NULL)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(motor >= s_eo_currents_watchdog.numberofmotors)
    {
        return eores_NOK_generic;
    }
    
    //calculate nominalCurrent^2 and I2T_thershold
    eOmeas_current_t nc = s_eo_currents_watchdog.themotors[motor]->config.currentLimits.nominalCurrent;
    eOmeas_current_t pc = s_eo_currents_watchdog.themotors[motor]->config.currentLimits.peakCurrent;
    s_eo_currents_watchdog.nominalCurrent2[motor] = (float)(nc*nc);
    s_eo_currents_watchdog.I2T_threshold[motor] = (float)((pc*pc) - s_eo_currents_watchdog.nominalCurrent2[motor]); 
    
    return(eores_OK);
}


extern void eo_currents_watchdog_Tick(EOCurrentsWatchdog* p)
{

    if (p == NULL)
    {
        return;
    }
    
    int16_t current_value = 0;

    for (uint8_t i = 0; i < s_eo_currents_watchdog.numberofmotors; i++)
    {
        //get current value
        current_value = eo_motioncontrol_extra_GetMotorCurrent(eo_motioncontrol_GetHandle(), i);
        
        //Update currents broadcasted
        s_eo_currents_watchdog_UpdateMotorCurrents(i, current_value);
        
        //error flags signalling is done internally
        s_eo_currents_watchdog_CheckSpike(i, current_value);
        //VALE: I disabled check I2T until I have nominal and peack current values
        //s_eo_currents_watchdog_CheckI2T(i, current_value);
    }
    
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_currents_watchdog_UpdateMotorCurrents(uint8_t motor, int16_t value)
{
    eo_emsController_AcquireMotorCurrent(motor, value);  
}


static void s_eo_currents_watchdog_CheckSpike(uint8_t motor, int16_t value)
{
	// single sample cannot be above a certain treshold
    
    //change sign to check absolute value
    if (value < 0)
        value = -value;
    
    if (value > s_eo_currents_watchdog.themotors[motor]->config.currentLimits.overloadCurrent)
    {
        //signal the OVERCURRENT error EOemsController
        
        uint32_t current_state = eo_motioncontrol_extra_GetMotorFaultMask(eo_motioncontrol_GetHandle(),motor);
        
        if((current_state & MOTOR_OVERCURRENT_FAULT) == 0) //overcurrent fault bit not set
        {
            //simulate the CANframe used by 2FOC to signal the status
            uint64_t fault_mask = (((uint64_t)(current_state | MOTOR_OVERCURRENT_FAULT)) << 32) & 0xFFFFFFFF00000000; //adding the error to the current state
            fault_mask |= icubCanProto_controlmode_hwFault; //setting the hard fault of the motor
            eo_motor_set_motor_status( eo_motors_GetHandle(), motor, (uint8_t*)&fault_mask);
        }
    }
    
    return;
}

static void s_eo_currents_watchdog_CheckI2T(uint8_t motor, int16_t value)
{
	// apply a simple LOW-PASS filter and if the value is above a threshold signal the error
    
    // IMPLEMENTATION OF LOW-PASSFILTER TO CHECK I2T INSIDE 2FOC FW
    /*
    if (!MotorConfig.has_tsens)
    {
        //static const long I2T_LIMIT = ((long)(sMaxCurrent/100)*(long)(sMaxCurrent/100))*(UDEF_I2T_LIMIT*UDEF_I2T_LIMIT);
        long I2 = __builtin_mulss(I2Tdata.IQMeasured,I2Tdata.IQMeasured);

        I2 -= I2Tacc;
        I2 >>= 15;
        I2Tacc += I2;

        if (I2Tacc > sI2Tlimit)
        {
            //The temperature grew too much. Protect!
            SysError.I2TFailure = TRUE;
            FaultConditionsHandler();
        }
    }
    */
    //int32_t i2 = (int32_t) (value * value);
    
    //s_eo_currents_watchdog.filter_reg[motor] += (float) (i2 - s_eo_currents_watchdog.filter_reg[joint]) / FILTER_WINDOW;
    
    
    
    //change sign to check absolute value
    if (value < 0)
        value = -value;
    
    uint32_t averageCurrent = s_eo_currents_watchdog_averageCalc_addValue( motor, value);
   
    if(!s_eo_currents_watchdog_averageCalc_collectDataIsCompleted(motor))
    {
        return;
    }
    // 1) calculate Ep
    s_eo_currents_watchdog.accomulatorEp[motor] += (averageCurrent*averageCurrent) - s_eo_currents_watchdog.nominalCurrent2[motor];

    // 2) check if current Ep is bigger than thresholdeo_motioncontrol_extra_GetMotorFaultMask(eo_motioncontrol_GetHandle(),motor)
    if( s_eo_currents_watchdog.accomulatorEp[motor] > s_eo_currents_watchdog.I2T_threshold[motor])
    {
        //signal the I2T error to EOemsController
    
        uint32_t current_state = eo_motioncontrol_extra_GetMotorFaultMask(eo_motioncontrol_GetHandle(),motor);

        if((current_state & MOTOR_I2T_LIMIT_FAULT) == 0) //I2T fault bit not set
        {
            //simulate the CANframe used by 2FOC to signal the status
            uint64_t fault_mask = (((uint64_t)(current_state | MOTOR_I2T_LIMIT_FAULT)) << 32) & 0xFFFFFFFF00000000; //adding the error to the current state
            fault_mask |= icubCanProto_controlmode_hwFault; //setting the hard fault of the motor
            eo_motor_set_motor_status( eo_motors_GetHandle(), motor, (uint8_t*)&fault_mask);
        }
    }
    
    // 3) reset average data
    s_eo_currents_watchdog_averageCalc_reset(motor);

}
#define I2T_CHECK_USE_AVERAGE_CURRENT

#ifdef I2T_CHECK_USE_AVERAGE_CURRENT 
/* With this implementation I2T check is done using average current of FILETR_WINDOW samples.


The Average is calculated with following formula:

Cumulative moving average (cma)
cma_n: is Cumulative moving average on n samples;
x_n: is the nth sample
x_n+1 is n+1 th sample

cma_n+1 = cma_n + ( (x_n+1 -cma_n) / (n+1) )

see: https://en.wikipedia.org/wiki/Moving_average

*/
EO_static_inline uint32_t s_eo_currents_watchdog_averageCalc_addValue(uint8_t motor, int16_t value)
{
    s_eo_currents_watchdog.avgCurrent[motor].counter++;
    s_eo_currents_watchdog.avgCurrent[motor].commulativeAverage += (value - s_eo_currents_watchdog.avgCurrent[motor].commulativeAverage)/s_eo_currents_watchdog.avgCurrent[motor].counter ;
    return(s_eo_currents_watchdog.avgCurrent[motor].commulativeAverage); 
}

EO_static_inline void s_eo_currents_watchdog_averageCalc_reset(uint8_t motor)
{
    s_eo_currents_watchdog.avgCurrent[motor].counter = 0;
    s_eo_currents_watchdog.avgCurrent[motor].commulativeAverage = 0;
}

EO_static_inline eObool_t s_eo_currents_watchdog_averageCalc_collectDataIsCompleted(uint8_t motor)
{
    return(s_eo_currents_watchdog.avgCurrent[motor].counter==FILTER_WINDOW);
}

#else

/* With this implementation I2T check is done using istantaneous current read  each FILTER_WINDOW samples */

EO_static_inline uint32_t s_eo_currents_watchdog_averageCalc_addValue(uint8_t motor, int16_t value)
{
    s_eo_currents_watchdog.avgCurrent[motor].counter++;
    s_eo_currents_watchdog.avgCurrent[motor].commulativeAverage = value ;

}

EO_static_inline void s_eo_currents_watchdog_averageCalc_reset(uint8_t motor)
{
    s_eo_currents_watchdog.avgCurrent[motor].counter = 0;
    s_eo_currents_watchdog.avgCurrent[motor].commulativeAverage = 0;
}

EO_static_inline eObool_t s_eo_currents_watchdog_averageCalc_collectDataIsCompleted(uint8_t motor)
{
    return(s_eo_currents_watchdog.avgCurrent[motor].counter==FILTER_WINDOW);
}


#endif //I2T_CHECK_USE_AVERAGE_CURRENT 




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



