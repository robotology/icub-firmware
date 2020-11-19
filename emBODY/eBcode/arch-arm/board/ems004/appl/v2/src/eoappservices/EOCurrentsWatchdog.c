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

#include "EOtheServices_hid.h"
#include "EoMotionControl.h"
#include "EOtheEntities.h"
#include "EOtheMemoryPool.h"
#include "EOtheMotionController.h"
//to signal errors
#include "Controller.h"
#include "EOtheErrorManager.h"
#include "EoError.h"
#include "EoProtocol.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOCurrentsWatchdog.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOCurrentsWatchdog_hid.h"

#if defined(EOTHESERVICES_disable_CurrentsWatchdog)


    extern EOCurrentsWatchdog* eo_currents_watchdog_Initialise(void)
    {
        return NULL;
    }

    extern EOCurrentsWatchdog* eo_currents_watchdog_GetHandle(void)
    {
        return NULL;
    }

    extern void eo_currents_watchdog_Tick(EOCurrentsWatchdog* p, int16_t voltage, int16_t *currents)
    {
    }
        

    extern eOresult_t eo_currents_watchdog_UpdateCurrentLimits(EOCurrentsWatchdog* p, uint8_t motor)
    {
        return eores_NOK_generic;
    }
    
#elif !defined(EOTHESERVICES_disable_CurrentsWatchdog)

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

#define REMOVE_TO_AVOID_COMPILATION_WARNING


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
//static void s_eo_currents_watchdog_CheckSpike(uint8_t joint, int16_t value);
static void s_eo_currents_watchdog_CheckI2T(uint8_t joint, int16_t value);
//static void s_eo_currents_watchdog_UpdateMotorCurrents(uint8_t joint, int16_t value);
#if defined(REMOVE_TO_AVOID_COMPILATION_WARNING)
#else
EO_static_inline uint32_t s_eo_currents_watchdog_averageCalc_addValue(uint8_t motor, int16_t value);
EO_static_inline void s_eo_currents_watchdog_averageCalc_reset(uint8_t motor);
EO_static_inline eObool_t s_eo_currents_watchdog_averageCalc_collectDataIsCompleted(uint8_t motor);
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOCurrentsWatchdog s_eo_currents_watchdog = 
{
    EO_INIT(.themotors)         NULL,
    EO_INIT(.numberofmotors)    0,
    //EO_INIT(.filter_reg)       NULL,
    EO_INIT(.nominalCurrent2)   NULL,
    EO_INIT(.I2T_threshold)     NULL,
    EO_INIT(.avgCurrent)        NULL,
    EO_INIT(.accomulatorEp)     NULL,
    EO_INIT(.initted)           eobool_false,
    EO_INIT(.motorinI2Tfault)   NULL
};

static uint16_t suppliedVoltage_counter = 0;
static eOmc_controller_t *nv_controller_ptr = NULL;
#define SUMMPLIED_VOLTAGE_COUNTER_MAX 500
//static const char s_eobj_ownname[] = "EOCurrentsWatchdog";

static int16_t maxReadCurrent[4] ={15, 15, 15, 15};//15 mA is the cureent with no load see datasheet of motor (debug only)


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

    s_eo_currents_watchdog.themotors = (eOmc_motor_t**) eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(eOmc_motor_t*), s_eo_currents_watchdog.numberofmotors);
    // retrieve pointers to motors   
    for(m=0; m<s_eo_currents_watchdog.numberofmotors; m++)
    {
        s_eo_currents_watchdog.themotors[m] = eo_entities_GetMotor(eo_entities_GetHandle(), m);
    }
    
    
    s_eo_currents_watchdog.nominalCurrent2 = (float*) eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(float), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.nominalCurrent2, 0, s_eo_currents_watchdog.numberofmotors*sizeof(float));
    
    s_eo_currents_watchdog.I2T_threshold = (float*) eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(float), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.I2T_threshold, 0, s_eo_currents_watchdog.numberofmotors*sizeof(float));
    
    s_eo_currents_watchdog.avgCurrent = (eoCurrentWD_averageData_t*) eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(eoCurrentWD_averageData_t), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.avgCurrent, 0, s_eo_currents_watchdog.numberofmotors*sizeof(eoCurrentWD_averageData_t));

    s_eo_currents_watchdog.accomulatorEp = (float*) eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(float), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.accomulatorEp, 0, s_eo_currents_watchdog.numberofmotors*sizeof(float));
    
    s_eo_currents_watchdog.motorinI2Tfault = (eObool_t*) eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_auto, sizeof(eObool_t), s_eo_currents_watchdog.numberofmotors);
    memset(s_eo_currents_watchdog.motorinI2Tfault, 0, s_eo_currents_watchdog.numberofmotors*sizeof(eObool_t)); //all motors are not in I2t fault

    s_eo_currents_watchdog.initted = eobool_true;
    
    suppliedVoltage_counter = 0;
    nv_controller_ptr = (eOmc_controller_t*) eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_controller, 0);
    
    if(nv_controller_ptr == NULL)
        return NULL;
    
    
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
    s_eo_currents_watchdog.nominalCurrent2[motor] = (float)nc*(float)nc;
    static const float I2TIME = 3.0f;
    s_eo_currents_watchdog.I2T_threshold[motor] = I2TIME*((float)pc*(float)pc - s_eo_currents_watchdog.nominalCurrent2[motor]); 
    
//    char str[eomn_info_status_extra_sizeof];    
//    eOerrmanDescriptor_t errdes = {0};
//    errdes.code                 = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
//    errdes.par16                = motor;
//    errdes.sourcedevice         = eo_errman_sourcedevice_localboard;
//    errdes.sourceaddress        = 0;  

//    snprintf(str, sizeof(str), "nc2=%.4f I2T=%.4f", s_eo_currents_watchdog.nominalCurrent2[motor], s_eo_currents_watchdog.I2T_threshold[motor]);
//    eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, str, NULL, &errdes);
    return(eores_OK);
}


extern void eo_currents_watchdog_Tick(EOCurrentsWatchdog* p, int16_t voltage, int16_t *currents)
{

    if (p == NULL)
    {
        return;
    }
    
    int16_t current_value = 0;

    for (uint8_t i = 0; i < s_eo_currents_watchdog.numberofmotors; i++)
    {
        //get current value
        current_value = currents[i];
        
        //Update currents broadcasted
        //s_eo_currents_watchdog_UpdateMotorCurrents(i, current_value);
        MController_update_motor_current_fbk(i, current_value);
        
        // moved into s_eo_currents_watchdog_UpdateMotorCurrents
        //error flags signalling is done internally
        //s_eo_currents_watchdog_CheckSpike(i, current_value);
        
        s_eo_currents_watchdog_CheckI2T(i, current_value);
        

        //added following code only 4 debug purpose
        if(current_value > maxReadCurrent[i])
        {
            maxReadCurrent[i] = current_value;
//            if(0 == i)//only for first joint
//            {
//                
//                eOerrmanDescriptor_t errdes = {0};
//                errdes.code                 = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
//                errdes.par16                = i;
//                errdes.sourcedevice         = eo_errman_sourcedevice_localboard;
//                errdes.sourceaddress        = 0;  
//                char str[eomn_info_status_extra_sizeof];
//                snprintf(str, sizeof(str), "NEW MAX CURR VAL: =%d ", current_value);
//                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, str, NULL, &errdes);
//            }
        }

    }
    
    
    // in here i proces the voltage
    {
        suppliedVoltage_counter++;
        
        if(suppliedVoltage_counter < SUMMPLIED_VOLTAGE_COUNTER_MAX)
            return;
        
        nv_controller_ptr->status.supplyVoltage = voltage;
        suppliedVoltage_counter = 0;               
    }
    
    
}

//extern void eo_currents_watchdog_TickSupplyVoltage(EOCurrentsWatchdog* p)
//{

//    if (p == NULL)
//    {
//        return;
//    }
//    
//    suppliedVoltage_counter++;
//    
//    if(suppliedVoltage_counter < SUMMPLIED_VOLTAGE_COUNTER_MAX)
//        return;
//    
//    nv_controller_ptr->status.supplyVoltage = eo_motioncontrol_extra_GetSuppliedVoltage(eo_motioncontrol_GetHandle());
//    suppliedVoltage_counter = 0;
//    return;
//    
//    
//}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

/*
static void s_eo_currents_watchdog_UpdateMotorCurrents(uint8_t motor, int16_t value)
{
    MController_update_motor_current_fbk(motor, value);
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
                
        MController_motor_raise_fault_overcurrent(motor);
    }
    else
    {
        MController_motor_reset_fault_overcurrent(motor);
    }
    
    return;
}
*/

static void s_eo_currents_watchdog_CheckI2T(uint8_t motor, int16_t value)
{
    float I = value;
    
    s_eo_currents_watchdog.accomulatorEp[motor] += 0.001f*(I*I - s_eo_currents_watchdog.nominalCurrent2[motor]);
    
    if (s_eo_currents_watchdog.accomulatorEp[motor] < 0.0f) s_eo_currents_watchdog.accomulatorEp[motor] = 0.0f;

    // 2) check if current Ep is bigger than threshold then rais fault
    if( s_eo_currents_watchdog.accomulatorEp[motor] > s_eo_currents_watchdog.I2T_threshold[motor])
    {
        s_eo_currents_watchdog.motorinI2Tfault[motor] = eobool_true;
        
        MController_motor_raise_fault_i2t(motor);
    }
    else
    {
        //I need to raise I2T fault until the current system energy is not littler than I2T threshold divided 2. (we decided so....)
        if(s_eo_currents_watchdog.motorinI2Tfault[motor])
        {
            if(s_eo_currents_watchdog.accomulatorEp[motor] > (0.5f*s_eo_currents_watchdog.I2T_threshold[motor]))
            {
                MController_motor_raise_fault_i2t(motor);
            }
            else
            {
                eOerrmanDescriptor_t errdes = {0};
                errdes.code                 = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
                errdes.par16                = motor;
                errdes.sourcedevice         = eo_errman_sourcedevice_localboard;
                errdes.sourceaddress        = 0;  
                char str[100];
                snprintf(str, sizeof(str), "Ep < I2T/2: now it is possible put in idle the motor");
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, str, NULL, &errdes);
                
                s_eo_currents_watchdog.motorinI2Tfault[motor] = eobool_false;
            }
        }
    }
}

#if 0
static void s_eo_currents_watchdog_CheckI2T(uint8_t motor, int16_t value)
{    
    //change sign to check absolute value
    if (value < 0)
        value = -value;
    
    uint32_t averageCurrent = s_eo_currents_watchdog_averageCalc_addValue( motor, value);
   
    if(!s_eo_currents_watchdog_averageCalc_collectDataIsCompleted(motor))
    {
        return;
    }
    // 1) calculate Ep
    float Ep_aux= (averageCurrent*averageCurrent) - s_eo_currents_watchdog.nominalCurrent2[motor];
    
    
    s_eo_currents_watchdog.accomulatorEp[motor] += Ep_aux;
    
    if(s_eo_currents_watchdog.accomulatorEp[motor] < 0) //Ep could not be smaller than zero
    {
        s_eo_currents_watchdog.accomulatorEp[motor] = 0;
    }

    // 2) check if current Ep is bigger than threshold then rais fault
    if( s_eo_currents_watchdog.accomulatorEp[motor] > s_eo_currents_watchdog.I2T_threshold[motor])
    {
        s_eo_currents_watchdog.motorinI2Tfault[motor] = eobool_true;
        
        MController_motor_raise_fault_i2t(motor);
    }
    else
    {
        //I need to raise I2T fault until the current system energy is not littler than I2T threshold divided 2. (we decided so....)
        if(s_eo_currents_watchdog.motorinI2Tfault[motor])
        {
            if(s_eo_currents_watchdog.accomulatorEp[motor] > (s_eo_currents_watchdog.I2T_threshold[motor]/2))
            {
                MController_motor_raise_fault_i2t(motor);
            }
            else
            {
                eOerrmanDescriptor_t errdes = {0};
                errdes.code                 = eoerror_code_get(eoerror_category_Debug, eoerror_value_DEB_tag00);
                errdes.par16                = motor;
                errdes.sourcedevice         = eo_errman_sourcedevice_localboard;
                errdes.sourceaddress        = 0;  
                char str[100];
                snprintf(str, sizeof(str), "Ep < I2T/2: now it is possible put in idle the motor");
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_debug, str, NULL, &errdes);
                
                s_eo_currents_watchdog.motorinI2Tfault[motor] = eobool_false;
            }
        }
    }
    
    // 3) reset average data
    s_eo_currents_watchdog_averageCalc_reset(motor);

}
#endif


#if defined(REMOVE_TO_AVOID_COMPILATION_WARNING)
#else

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

#endif //REMOVE_TO_AVOID_COMPILATION_WARNING

#endif // #elif !defined(EOTHESERVICES_disable_CurrentsWatchdog)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



