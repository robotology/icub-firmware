/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_analog.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include <array>

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_motor_bldc)


#elif defined(EMBOT_ENABLE_hw_motor_bldc)

#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
#include "embot_hw_motor_bldc_bsp_amcfoc_1cm7.h"
#endif


#if !defined(EMBOT_ENABLE_hw_analog_ish)

namespace embot::hw::analog {

    void deinit() {}
    void init(const Configuration &config) {}
    float getVin() { return 18.0f; }
    float getCin() { return 1.0f; } 
       
}

#elif defined(EMBOT_ENABLE_hw_analog_ish)  


namespace embot::hw::analog::bsp {
    
    constexpr uint8_t NUMBER_OF_ADC3_CHANNELS = {6}; // for ADC3

    /* Hardware related constants */
    constexpr uint32_t VREFINT_CAL = {24243};    /* (*VREFINT_CAL_ADDR) is the 16 bits calibration value stored by the manufacturer at location 0x1FF1E860 */
    constexpr double VREF_NOM      = {3300.0};   /* mV  */
    constexpr double VREFINT_NOM   = {1212.0};   /* mV  */
    constexpr double R110          = {560000.0}; /* Ohm */
    constexpr double R115          = {33000.0};  /* Ohm */
//    constexpr uint32_t VIN_GAIN    = static_cast<uint32_t>((R110+R115)/R115 + 0.5);
}



namespace embot::hw::analog {
     
    
#warning need to do a bsp
    
    
#if defined(STM32HAL_BOARD_AMC1CM7) 
#define hadc3 (embot::hw::motor::bsp::amc1cm7::hADC3)
#define htim6 (embot::hw::motor::bsp::amc1cm7::hTIM6)

#elif defined(STM32HAL_BOARD_AMC2CM4)
#define hadc3 (embot::hw::motor::bsp::amc2cm4::hADC3)
#define htim6 (embot::hw::motor::bsp::amc2cm4::hTIM6)

#endif  
    

#warning check if this is valid for amcfoc

#if 0
    enum AdcSignals 
    {
        Cin,        /* ADC3_INP0 */
        Vin,        /* ADC3_INP1 */
        Vaux,       /* ADC3_INP2 */
        Vcore,      /* ADC3_INP3 */
        Vtemp,      /* Temperature sensor */
        Vrefint     /* Internal voltage reference 1.212V */
    };
#endif
    


    
    
    //could be a class?
    typedef struct
    {
        int32_t avg;
        uint32_t idx;
        std::array<int32_t, 128> buf;
    } AdcMovingAverageFilter_t;
    
    
    /* VREF constant: VREFINT_CAL * VREFINT_CAL_VREF */
    static uint32_t Kvrefcal = static_cast<uint32_t>(VREFINT_NOM * VREF_NOM + 0.5);

    /* Measured VREF in mV. Equal to VCC */
    static volatile uint16_t Vcc_mV = VREF_NOM;
    static volatile uint16_t Vin_mV = 18000;
    static AdcMovingAverageFilter_t VrefFilter {0};
    //static AdcMovingAverageFilter_t CinFilter {0};

    static volatile int16_t Cin_mA = +1000;


    /* DMA buffer for ADC3 raw data */
    //static uint16_t AdcPwrRawData[16]; // TODO: re-check (compare) this parameter w/ the one used by GZini (it is different)
    alignas(32) static std::array<uint16_t, 16> AdcPwrRawData;

    /*******************************************************************************************************************//**
     * @brief
     * @param
     * @return
     */
    int32_t AdcMovingAverageFilter(AdcMovingAverageFilter_t *pFilter, int32_t sample)
    {
        pFilter->avg -= pFilter->buf[pFilter->idx];
        pFilter->avg += (pFilter->buf[pFilter->idx] = sample);
        if (++(pFilter->idx) >= pFilter->buf.size()) pFilter->idx = 0;
        return (pFilter->avg + (pFilter->buf.size() >> 1)) / pFilter->buf.size();
    }


    /*******************************************************************************************************************//**
     * @brief
     * @param
     * @return
     */
    void Adc3DmaComplete_callback(volatile uint16_t sample[])
    {
        uint16_t Vref;
        /* Calculate average values of Vrefint */
        Vref = static_cast<uint16_t>(AdcMovingAverageFilter(&VrefFilter, static_cast<int32_t>(sample[Vrefint])));
        /* Update current analog values */
        Vcc_mV   = (0 < Vref)? Kvrefcal / Vref : 3300u;
    //    Vcore_mV = (uint32_t)sample[Vcore] * VCORE_GAIN * (uint32_t)Vcc_mV / 65536u;
    //    Vaux_mV  = (uint32_t)sample[Vaux]  * VAUX_GAIN  * (uint32_t)Vcc_mV / 65536u;
        Vin_mV   = static_cast<uint32_t>(sample[Vin]) * VIN_GAIN * static_cast<uint32_t>(Vcc_mV) / 65536u;
    //    Cin_mA   = (int16_t)AdcMovingAverageFilter(&CinFilter,
    //               (int32_t)(int16_t)sample[Cin] * CIN_GAINx4 * (int32_t)(int16_t)Vcc_mV / (65536*4));
    }

    /* Callback functions *************************************************************************************************/

    /*******************************************************************************************************************//**
     * @brief
     * @param
     * @return
     */
    void AdcPwrHalfTransfer_callback(ADC_HandleTypeDef *hadc)
    {
        /* Force the readback of the DMA buffer */
        //SCB_InvalidateDCache_by_Addr((void *)AdcPwrRawData, sizeof(AdcPwrRawData));
        /* First half of the buffer ready */
        Adc3DmaComplete_callback(&(AdcPwrRawData[0]));
    }


    void AdcPwrTransferComplete_callback(ADC_HandleTypeDef *hadc)
    {
        /* Force the readback of the DMA buffer */
        //SCB_InvalidateDCache_by_Addr((void *)AdcPwrRawData, sizeof(AdcPwrRawData));
        /* Second half of the buffer ready */
        Adc3DmaComplete_callback(&(AdcPwrRawData[NUMBER_OF_ADC3_CHANNELS]));
    }

        
    void init(const Configuration &config)
    {
        if (HAL_OK == HAL_TIM_Base_Start(&htim6))
        {
            /* Calibrate ADC3 */
            if (HAL_OK == HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED))
            {
                /* Register DMA callback functions */
                if ((HAL_OK == HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_HALF_CB_ID, AdcPwrHalfTransfer_callback)) &&
                    (HAL_OK == HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AdcPwrTransferComplete_callback)))
                {
                    
                    /* Calculate the proportionality constant */
                    Kvrefcal = static_cast<uint32_t>(VREFINT_CAL_VREF) * VREFINT_CAL;
                    
                    /* Init the Vref filter to the nominal value */
                    for (uint8_t i = 0; i < VrefFilter.buf.size(); i++)
                    {
                        VrefFilter.buf[i] = 3300;
                    }
                    VrefFilter.avg = VrefFilter.buf.size() * 3300;
                    VrefFilter.idx = 0;
                    
                    /* Clear the Cin filter */
    //                memset((void *)&CinFilter, 0, sizeof(CinFilter));
                    std::fill(std::begin(AdcPwrRawData), std::end(AdcPwrRawData), 0);
    //                SCB_CleanInvalidateDCache_by_Addr((void *)AdcDmaBuffer, sizeof(AdcDmaBuffer));
                    
                    /* Start the converter in DMA mode */
                    HAL_ADC_Start_DMA(&hadc3, (uint32_t *)AdcPwrRawData.data(), 2*NUMBER_OF_ADC3_CHANNELS);
                }
            }
        }
    }

    void deinit()
    {
        HAL_ADCEx_MultiModeStop_DMA(&hadc3);
        HAL_ADC_UnRegisterCallback(&hadc3, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
        HAL_ADC_UnRegisterCallback(&hadc3, HAL_ADC_CONVERSION_HALF_CB_ID);
    }

    float getVin()
    {
        // return Vin in Volts
        return Vin_mV * 0.001f;
    }

    float getCin()
    {
        return Cin_mA * 0.001f;
    }
    
} // namespace embot::hw::analog
 
#endif // #if defined(EMBOT_ENABLE_hw_analog_ish)

#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc)



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

