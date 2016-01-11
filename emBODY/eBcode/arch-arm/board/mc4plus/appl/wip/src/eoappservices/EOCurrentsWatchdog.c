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
#include "EOmcService.h"
#include "EOtheEntities.h"
#include "EOtheMemoryPool.h"

//to signal errors
#include "EOemsController.h"

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

#define FILTER_WINDOW (float) 1000.0 //make it configurable? --> could become a parameter from XML


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


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOCurrentsWatchdog s_eo_currents_watchdog = 
{
    .cfg            = {0},
    .numberofmotors = 0,
    .filter_reg     = NULL,
	.initted	    = eobool_false,
};

//static const char s_eobj_ownname[] = "EOCurrentsWatchdog";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOCurrentsWatchdog* eo_currents_watchdog_Initialise(void)
{
    //reserve memory for the number of thresholds needed
    s_eo_currents_watchdog.numberofmotors = eo_entities_NumOfJoints(eo_entities_GetHandle());
    
    if (s_eo_currents_watchdog.numberofmotors == 0)
        return NULL;
    
    s_eo_currents_watchdog.cfg.i2t_thresh   = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(uint32_t), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.cfg.i2t_thresh, EOK_int08dummy, s_eo_currents_watchdog.numberofmotors*sizeof(uint32_t)); //highest value so that the threshold surely lowers it
    
    s_eo_currents_watchdog.cfg.spike_thresh = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(uint16_t), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.cfg.spike_thresh, EOK_int08dummy, s_eo_currents_watchdog.numberofmotors*sizeof(uint16_t)); //highest value so that the threshold surely lowers it
    
    s_eo_currents_watchdog.filter_reg = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(float), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.filter_reg, 0, s_eo_currents_watchdog.numberofmotors*sizeof(float));
    
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


extern eOresult_t eo_currents_watchdog_Configure(EOCurrentsWatchdog* p, eOcurrents_watchdog_cfg_t* cfg)
{
	if (p == NULL)
    {
        return(eores_NOK_nullpointer);
    }
    
    if((eobool_false == s_eo_currents_watchdog.initted) || (cfg == NULL))
    {
        return(eores_NOK_generic);
    }
        
    //save in reserved memory
    memcpy(&(s_eo_currents_watchdog.cfg), cfg, sizeof(eOcurrents_watchdog_cfg_t));
        
    return(eores_OK);
}

extern eOresult_t eo_currents_watchdog_SetSpikeThreshold(EOCurrentsWatchdog* p, uint8_t joint, uint16_t threshold)
{
    if (p == NULL)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(joint >= s_eo_currents_watchdog.numberofmotors)
    {
        return eores_NOK_generic;
    }
    
    //set the new threshold
    s_eo_currents_watchdog.cfg.spike_thresh[joint] = threshold;
    
    return(eores_OK);
}

extern eOresult_t eo_currents_watchdog_SetI2TThreshold(EOCurrentsWatchdog* p, uint8_t joint, uint32_t threshold)
{
    if (p == NULL)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(joint >= s_eo_currents_watchdog.numberofmotors)
    {
        return eores_NOK_generic;
    }
	    
    //set the new threshold
    s_eo_currents_watchdog.cfg.i2t_thresh[joint] = threshold;
    
    return(eores_OK);
}

extern void eo_currents_watchdog_Tick(EOCurrentsWatchdog* p)
{
	if (p == NULL)
    {
        return;
    }
    
    int16_t current_value = 0;
    //check spikes and I2T
    for (uint8_t i = 0; i < s_eo_currents_watchdog.numberofmotors; i++)
    {
        //get current value
        current_value = eo_mcserv_GetMotorCurrent (eo_mcserv_GetHandle(), i);
        
        //Update currents broadcasted
        s_eo_currents_watchdog_UpdateMotorCurrents(i, current_value);
        
        //error flags signalling is done internally
        s_eo_currents_watchdog_CheckSpike(i, current_value);
        s_eo_currents_watchdog_CheckI2T(i, current_value);
    }
    
}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_eo_currents_watchdog_UpdateMotorCurrents(uint8_t joint, int16_t value)
{
    eo_emsController_AcquireMotorCurrent(joint, value);  
}
static void s_eo_currents_watchdog_CheckSpike(uint8_t joint, int16_t value)
{
	// single sample cannot be above a certain treshold
    
    //change sign to check absolute value
    if (value < 0)
        value = -value;
    
    if (value > s_eo_currents_watchdog.cfg.spike_thresh[joint])
    {
        //signal the OVERCURRENT error EOemsController
        
        uint32_t current_state = eo_mcserv_GetMotorFaultMask(eo_mcserv_GetHandle(),joint);
        
        if((current_state & MOTOR_OVERCURRENT_FAULT) == 0) //overcurrent fault bit not set
        {
            //simulate the CANframe used by 2FOC to signal the status
            uint64_t fault_mask = (((uint64_t)(current_state | MOTOR_OVERCURRENT_FAULT)) << 32) & 0xFFFFFFFF00000000; //adding the error to the current state
            fault_mask |= icubCanProto_controlmode_hwFault; //setting the hard fault of the motor
            eo_motor_set_motor_status( eo_motors_GetHandle(), joint, (uint8_t*)&fault_mask);
        }
    }
    
    return;
}

static void s_eo_currents_watchdog_CheckI2T(uint8_t joint, int16_t value)
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
    int32_t i2 = (int32_t) (value * value);
    
    s_eo_currents_watchdog.filter_reg[joint] += (float) (i2 - s_eo_currents_watchdog.filter_reg[joint]) / FILTER_WINDOW;
    
    if (s_eo_currents_watchdog.filter_reg[joint] >= s_eo_currents_watchdog.cfg.i2t_thresh[joint])
    {
        //signal the I2T error to EOemsController
        
       uint32_t current_state = eo_mcserv_GetMotorFaultMask(eo_mcserv_GetHandle(),joint);
        
       if((current_state & MOTOR_I2T_LIMIT_FAULT) == 0) //I2T fault bit not set
       {
            //simulate the CANframe used by 2FOC to signal the status
            uint64_t fault_mask = (((uint64_t)(current_state | MOTOR_I2T_LIMIT_FAULT)) << 32) & 0xFFFFFFFF00000000; //adding the error to the current state
            fault_mask |= icubCanProto_controlmode_hwFault; //setting the hard fault of the motor
            eo_motor_set_motor_status( eo_motors_GetHandle(), joint, (uint8_t*)&fault_mask);
       }
    }        
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



