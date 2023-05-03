/*******************************************************************************************************************//**
 * @file    adcm.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 September, 27
 * @brief   A/D converter management for CM4 core
 **********************************************************************************************************************/

// CODE SHAPER
#if defined(USE_STM32HAL)
#include "motorhal_config.h"
#endif
 
#include "embot_core.h"
 
#if defined(USE_STM32HAL) 
// API
#include "adcm.h"
#endif

#if defined(USE_STM32HAL)
#include "stm32hal.h"
#else
/* Includes ***********************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "adcm.h"
#include "main.h"
#include "stm32h7xx_hal_adc.h"
#include "led.h"
#include "console.h"
#include "FreeRTOS.h"
#endif // #if defined(USE_STM32HAL)

/* Private macros *****************************************************************************************************/

/* Couples of ADC channels */
#define NUMBER_OF_ADC_CHANNELS  (3)



/* Extenal variables **************************************************************************************************/

#if defined(MOTORHAL_changes)

#include "embot_hw_motor_bsp_amc2c.h"
#define hadc1 (embot::hw::motor::bsp::amc2c::hADC1)
#define hadc2 (embot::hw::motor::bsp::amc2c::hADC2)

#else
/* Declared in main.h */
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
#endif

#if defined(MOTORHAL_changes)

// --------------------------------------------------------------------------------------------------------------------
// add in here minimal changes
// --------------------------------------------------------------------------------------------------------------------

void s_adcm_on_acquisition_of_currents(int16_t c1, int16_t c2, int16_t c3); 

#endif // #if defined(MOTORHAL_changes)


/* Private typedefs ***************************************************************************************************/

/* Structure to hold the ADC1/ADC2 DMA data */
typedef struct
{
    int16_t adc1;
    uint16_t adc2;
} __attribute__((aligned(4),packed)) DualModeAdcData_t;


/* Private variables **************************************************************************************************/

/* DMA buffer for ADC1/ADC2 raw data */
static DualModeAdcData_t AdcMotRawData[2*NUMBER_OF_ADC_CHANNELS];

/* Unpacked data */
static int16_t adcMotCurrents[NUMBER_OF_ADC_CHANNELS];
static uint16_t adcMotVoltages[NUMBER_OF_ADC_CHANNELS];

/* Channels assigned to phase current measurement */
static const uint32_t AdcMotOffsetChannel[] =
{
    LL_ADC_CHANNEL_0, 
    LL_ADC_CHANNEL_1, 
    LL_ADC_CHANNEL_3
};

/* Offset to be applied to phase current measurement */
static const uint32_t AdcMotOffsetNumber[] =
{
    LL_ADC_OFFSET_1, 
    LL_ADC_OFFSET_2, 
    LL_ADC_OFFSET_3
};


/* Local functions ****************************************************************************************************/

static void adcMotGetSamples(const DualModeAdcData_t *pBuf)
{
    unsigned i;
#if defined(MOTORHALCONFIG_TESTPOINT_DEBUG_remove) 
#else    
/*DEBUG*/HAL_GPIO_WritePin(TP4_GPIO_Port, TP4_Pin, GPIO_PIN_SET);
#endif    
    for (i = 0; i < NUMBER_OF_ADC_CHANNELS; i++)
    {
        adcMotCurrents[i] = pBuf[i].adc1;
        adcMotVoltages[i] = pBuf[i].adc2;
    }
#if defined(MOTORHALCONFIG_TESTPOINT_DEBUG_remove) 
#else    
/*DEBUG*/HAL_GPIO_WritePin(TP4_GPIO_Port, TP4_Pin, GPIO_PIN_RESET);
#endif
    
#if defined(MOTORHAL_changes)    
    s_adcm_on_acquisition_of_currents(adcMotCurrents[0], adcMotCurrents[1], adcMotCurrents[2]);
#endif    
}

/* Callback functions *************************************************************************************************/


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
static void AdcMotHalfTransfer_cb(ADC_HandleTypeDef *hadc)
{
    /* Record the first half of the buffer */
    adcMotGetSamples(&(AdcMotRawData[0]));
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
static void AdcMotTransferComplete_cb(ADC_HandleTypeDef *hadc)
{
    /* Record the second half of the buffer */
    adcMotGetSamples(&(AdcMotRawData[NUMBER_OF_ADC_CHANNELS]));
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
static void AdcMotTransferError_cb(ADC_HandleTypeDef *hadc)
{
    /* SHOULD NEVER HAPPEN */
}


/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcMotGetCurrents(int16_t *pU, int16_t *pV, int16_t *pW)
{
    *pU = adcMotCurrents[0];
    *pV = adcMotCurrents[1];
    *pW = adcMotCurrents[2];
}    


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcMotGetVoltages(uint16_t *pU, uint16_t *pV, uint16_t *pW)
{
    *pU = adcMotVoltages[0];
    *pV = adcMotVoltages[1];
    *pW = adcMotVoltages[2];
}    


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
uint32_t AdcMotGetOffset(AdcMotPhase_t Phase)
{
    return LL_ADC_GetOffsetLevel(ADC1, AdcMotOffsetNumber[Phase]);
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcMotSetOffset(AdcMotPhase_t Phase, uint16_t Offset)
{
    LL_ADC_SetOffset(ADC1, AdcMotOffsetNumber[Phase], AdcMotOffsetChannel[Phase], Offset);
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
HAL_StatusTypeDef AdcMotInit(void)
{
    /* Calibrate ADC1 and ADC2 */
    if ((HAL_OK == HAL_ADCEx_Calibration_Start(&hadc1, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED)) &&
        (HAL_OK == HAL_ADCEx_Calibration_Start(&hadc2, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED)))
    {
        /* Register DMA callback functions */
        if ((HAL_OK == HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AdcMotTransferComplete_cb)) &&
            (HAL_OK == HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_HALF_CB_ID, AdcMotHalfTransfer_cb)) &&
            (HAL_OK == HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_ERROR_CB_ID, AdcMotTransferError_cb)))
        {
            /* Start DMA operations */
#if defined(MOTORHAL_changes)
            return HAL_ADCEx_MultiModeStart_DMA(&hadc1, (uint32_t *)AdcMotRawData, sizeof(AdcMotRawData)/sizeof(DualModeAdcData_t));
#else            
            return HAL_ADCEx_MultiModeStart_DMA(&hadc1, (void *)AdcMotRawData, sizeof(AdcMotRawData)/sizeof(DualModeAdcData_t));
#endif            
        }
    }
    return HAL_ERROR;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcMotDeInit(void)
{
    HAL_ADCEx_MultiModeStop_DMA(&hadc1);
    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_CONVERSION_HALF_CB_ID);
    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_ERROR_CB_ID);
}



#if defined(MOTORHAL_changes)


// --------------------------------------------------------------------------------------------------------------------
// add in here all the new code required by MOTORHAL
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include <array>

struct Converter
{
    static int32_t raw2current(int32_t r)
    {
        // return CIN_GAIN * vref_mV * r >> 16u;
        return r;       
    }
    
    static int32_t current2raw(int32_t c)
    {
        // int32_t tmp = (curr << 16) / (CIN_GAIN * vref_mV);
        // return tmp;
        return c;       
    }    
    
    constexpr Converter() = default;
};


struct Calibrator
{
    static constexpr uint8_t _shift {10};
    static constexpr size_t _maxcount {1 << _shift};
    
    volatile bool _done {false};
    volatile size_t _count {0};
    adcm_Currents _currents {};
    std::array<int64_t, 3> cumulativerawvalues {0, 0, 0};
    
    constexpr Calibrator() = default;
    
    void init()
    {
       _count = 0;
       _done = false; 
       cumulativerawvalues.fill(0); 
       adcm_set_ADC_callback({oncurrents, this});
    }
    
    void stop()
    {
        adcm_set_ADC_callback({});
        _count = 0;
       _done = true;  
    }
    
    
    bool wait(const embot::core::relTime timeout = 100*embot::core::time1millisec)
    {
        bool ret {false};
        
        volatile embot::core::Time endtime = embot::core::now() + timeout;
        
        for(;;)
        {
            volatile embot::core::Time n = embot::core::now();
            if(true == _done)
            { 
                ret = true; 
                break; 
            }
            else if(n > endtime)
            {
                ret = false; 
                stop();
                break;
            }
        }

        return ret;
    }   


    static void oncurrents(void *owner, const adcm_Currents * const currents)
    {
        Calibrator *o = reinterpret_cast<Calibrator*>(owner);   
        
        // i use Converter::current2raw() because technically adcm_Currents does not contain the raw ADC values but transformed values
        o->cumulativerawvalues[0] += Converter::current2raw(currents->u);
        o->cumulativerawvalues[1] += Converter::current2raw(currents->v);
        o->cumulativerawvalues[2] += Converter::current2raw(currents->w);
        
        o->_count++;
        
        if(o->_count >= Calibrator::_maxcount)
        {
            // time to do business: prepare average currents, impose the offset to ADC, stop the calibrator  
            
            // dont use the >> _shift because ... 1. this operation is done only once, so who bother. 
            //                                    2. cumulativerawvalues is int64_t. it may be (dont think so) negative so it is too complicate to optimize
            // impose offset to adc.
            volatile int64_t cc[3] {0, 0, 0};
            cc[0] = o->cumulativerawvalues[0] / _maxcount;
            cc[1] = o->cumulativerawvalues[1] / _maxcount;
            cc[2] = o->cumulativerawvalues[2] / _maxcount;
            AdcMotSetOffset(ADCMOT_PHASE_U, AdcMotGetOffset(ADCMOT_PHASE_U) + (o->cumulativerawvalues[0] / _maxcount));
            AdcMotSetOffset(ADCMOT_PHASE_V, AdcMotGetOffset(ADCMOT_PHASE_V) + (o->cumulativerawvalues[1] / _maxcount));
            AdcMotSetOffset(ADCMOT_PHASE_W, AdcMotGetOffset(ADCMOT_PHASE_W) + (o->cumulativerawvalues[2] / _maxcount));
            
            // stop
            o->stop();
            
            return;
        }                     
    }    
    
};

struct adcm_Internals
{
    adcm_Configuration config {};
    Calibrator calibrator {};
        
    static void dummy_adc_callback(void *owner, const adcm_Currents * const currents) {}  
    static constexpr adcm_ADC_callback dummyADCcbk { dummy_adc_callback, nullptr };         
        
    adcm_Internals() = default;    
};

adcm_Internals _adcm_internals {};
    
    
void s_adcm_on_acquisition_of_currents(int16_t c1, int16_t c2, int16_t c3)
{
    // important note: {c1, c2, c3} are the raw ADC acquisitions as in adcMotCurrents[0, 1, 2]
    // the callback requires converted currents. but to be fair in adcm.c there is no concept of how to convert them as in the amcbldc
    // so, boh ... for now i do nothing    
    adcm_Currents currents = {Converter::raw2current(c1), Converter::raw2current(c2), Converter::raw2current(c3)};
    _adcm_internals.config.onadc.execute(&currents);         
}    

    
bool adcm_Init(const adcm_Configuration &config)
{
    bool r {true};
    
    if(false == config.isvalid())
    {
        return false;
    }
    
    _adcm_internals.config = config;
    
    // 1. at first we want to be sure that everything is stopped, so:
    adcm_DeInit();  

    // 2. then we init w/out calibration or w/ calibration
    if(_adcm_internals.config.calibration == adcm_Configuration::CALIBRATION::none)        
    {
        HAL_StatusTypeDef hr = AdcMotInit();      
        r = (HAL_ERROR == hr) ? false : true;       
    }
    else
    {
        // 2.b w/ calibration
        _adcm_internals.calibrator.init();
        // in here we need to have zero pwm, so we force it
        PwmPhaseSet(0, 0, 0);
        PwmPhaseEnable(PWM_PHASE_NONE);
        
        AdcMotInit(); // this call starts the ADC acquisition. 
        // which in turn calls Calibrator::oncurrents() which eventally calls stop() and allows stop() to return
        r = _adcm_internals.calibrator.wait();
        r = true;
    }
    
    return r;   
}  

bool adcm_Calibrate(uint64_t usectimeout)
{
    bool r {true};
    
        _adcm_internals.calibrator.init();
        // in here we need to have zero pwm, so we force it
        PwmPhaseSet(0, 0, 0);
        PwmPhaseEnable(PWM_PHASE_NONE);
    
        r = _adcm_internals.calibrator.wait();

    
    return r;   
}    


void adcm_set_ADC_callback(const adcm_ADC_callback &cbk)
{
    // maybe better to protect execution of s_adcm_ADC_cbk by ...
    // temporarily suspending the AdcMotTransferComplete_cb() and AdcMotHalfTransfer_cb()  
    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_CONVERSION_HALF_CB_ID);
    
    _adcm_internals.config.onadc = cbk.isvalid() ? cbk : _adcm_internals.dummyADCcbk;
    
    HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AdcMotTransferComplete_cb);
    HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_HALF_CB_ID, AdcMotHalfTransfer_cb);    
}

// as AdcMotDeInit() but ... see the changes in analog_DeInit() of amcbldc
bool adcm_DeInit()
{
    bool r = true;    
    // AdcMotDeInit() unchanged is OK 
    AdcMotDeInit();
    // i also clear the callback
    adcm_set_ADC_callback({});    
    return r;    
}


#endif // #if defined(MOTORHAL_changes)


/* END OF FILE ********************************************************************************************************/
