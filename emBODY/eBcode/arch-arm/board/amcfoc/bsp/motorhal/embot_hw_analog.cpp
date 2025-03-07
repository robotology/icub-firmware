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
    
    
    //size of ...
    constexpr uint8_t NUMBER_OF_ADC3_CHANNELS = {10}; // for ADC3

    /* Hardware related constants */
    constexpr uint32_t VREFINT_CAL = {24243};    /* (*VREFINT_CAL_ADDR) is the 16 bits calibration value stored by the manufacturer at location 0x1FF1E860 */
    constexpr double VREF_NOM      = {3300.0};   /* mV  */
    constexpr double VREFINT_NOM   = {1212.0};   /* mV  */
    constexpr double R139          = {560000.0}; /* Ohm */
    constexpr double R75          = {33000.0};  /* Ohm */
    constexpr uint32_t VIN_GAIN    = static_cast<uint32_t>((R139+R75)/R75 + 0.5);
    
    
/* AMC-FOC schematics related constants */
#define R73                 (33000.0)
#define R84                 (22000.0)
#define R85                 (22000.0)
#define R74                 (33000.0)
#define R139                (560000.0)
#define R75                 (24000.0)
#define R140                (0.1)
#define U18_GAIN            (20)
#define VAUX_ATTEN          ((R73+R84)/R84)
#define VCC_ATTEN           ((R85+R74)/R74)
#define VCORE_ATTEN         (1.0)
#define VIN_ATTEN           ((R139+R75)/R75)
#define CIN_ATTEN           (1.0/(R140*U18_GAIN))
#define PTC_GAIN            (132.0/2.5)
#define PTC_OFFS            (-12.0) 
    
    
    
/* ADC related constants */
#define ADC_VREF                (2.5/*V*/)
#define ADC_NUMBER_OF_BITS      (12)
#define ADC_RESOLUTION          (1U<<ADC_NUMBER_OF_BITS)
#define ADC_LSB                 (ADC_VREF/(double)ADC_RESOLUTION)
#define ADC1_NUMBER_OF_CHANNELS (3u)
#define ADC2_NUMBER_OF_CHANNELS (3u)
#define ADC3_NUMBER_OF_CHANNELS (10u)
#define ADC3_TIMEOUT            (20u)
    
//    
//    #define VREFINT_CAL                 VREFINT_CAL_ADDR
//    #define TS_CAL1                     TEMPSENSOR_CAL1_ADDR  
//    #define TS_CAL2                     TEMPSENSOR_CAL2_ADDR

//    
//    constexpr double AinVrefintCal = ((double)VREFINT_CAL)*(double)VREFINT_CAL_VREF/65536000.0;
//    double AinTempGain = (float)(TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP)/(float)(TS_CAL2 - TS_CAL1);
//    constexpr double AinTempOffs = (float)TEMPSENSOR_CAL1_TEMP - AinTempGain*(float)TS_CAL1;
//    /* Compensate the resolution and VREF */
//    AinTempGain *= 65536000.0/(float)TEMPSENSOR_CAL_VREFANALOG;
//    
}



namespace embot::hw::analog {
     
    
#warning need to do a bsp
    
    
//#if defined(STM32HAL_BOARD_AMC1CM7) 
//#define hadc3 (embot::hw::motor::bsp::amc1cm7::hADC3)
//#define htim6 (embot::hw::motor::bsp::amc1cm7::hTIM6)

//#elif defined(STM32HAL_BOARD_AMC2CM4)
//#define hadc3 (embot::hw::motor::bsp::amc2cm4::hADC3)
//#define htim6 (embot::hw::motor::bsp::amc2cm4::hTIM6)

//#el
#if defined(STM32HAL_BOARD_AMCFOC_1CM7)
#define hadc3 (embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS)
#define htim15 (embot::hw::motor::bldc::bsp::amcfoc::cm7::htimTriggerOfadcOTHERS)
#else 
#warning bsp of AMCFOC_1CM7

#endif  
    

//////////#warning check if this is valid for 
//////////    enum AdcSignals 
//////////    {
//////////        Cin,        /* ADC3_INP0 */
//////////        Vin,        /* ADC3_INP1 */
//////////        Vaux,       /* ADC3_INP2 */
//////////        Vcore,      /* ADC3_INP3 */
//////////        Vtemp,      /* Temperature sensor */
//////////        Vrefint     /* Internal voltage reference 1.212V */
//////////    };
    
typedef enum
{
    PTC1,
    PTC2,
    CIN,
    VIN,
    VAUX,
    VCC,
    VCORE,
    VBAT,
    VREFINT,
    TEMP
} Ain3Channels_t;

    
    
    //could be a class?
    typedef struct
    {
        int32_t avg;
        uint32_t idx;
        std::array<int32_t, 128> buf;
    } AdcMovingAverageFilter_t;
    
    
    /* VREF constant: VREFINT_CAL * VREFINT_CAL_VREF */
    static uint32_t Kvrefcal = static_cast<uint32_t>(bsp::VREFINT_NOM * bsp::VREF_NOM + 0.5);

    /* Measured VREF in mV. Equal to VCC */
    static volatile uint16_t Vcc_mV = bsp::VREF_NOM;
    static volatile uint16_t Vin_mV = 18000;
    static volatile uint16_t AinVcc = 18000;
    static AdcMovingAverageFilter_t VrefFilter {0};
    //static AdcMovingAverageFilter_t CinFilter {0};

    static volatile int16_t Cin_mA = +1000;

//    /* DMA circular buffers for ADC3: Double buffer */
//    volatile uint16_t AinDma3Buffer[2 * ADC3_NUMBER_OF_CHANNELS] __attribute__((aligned (4)));
    /* DMA buffer for ADC3 raw data */
    //static uint16_t AdcPwrRawData[16]; // TODO: re-check (compare) this parameter w/ the one used by GZini (it is different)
    alignas(32) static std::array<uint16_t, 2 * ADC3_NUMBER_OF_CHANNELS> AdcPwrRawData;

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
////        uint16_t Vref;
////        /* Calculate average values of Vrefint */
////        Vref = static_cast<uint16_t>(AdcMovingAverageFilter(&VrefFilter, static_cast<int32_t>(sample[VREFINT])));
////        /* Update current analog values */
////        Vcc_mV   = (0 < Vref)? Kvrefcal / Vref : 3300u;
////    //    Vcore_mV = (uint32_t)sample[Vcore] * VCORE_GAIN * (uint32_t)Vcc_mV / 65536u;
////    //    Vaux_mV  = (uint32_t)sample[Vaux]  * VAUX_GAIN  * (uint32_t)Vcc_mV / 65536u;
////        Vin_mV   = static_cast<uint32_t>(sample[VIN]) * bsp::VIN_GAIN * static_cast<uint32_t>(Vcc_mV) / 65536u;
////    //    Cin_mA   = (int16_t)AdcMovingAverageFilter(&CinFilter,
////    //               (int32_t)(int16_t)sample[Cin] * CIN_GAINx4 * (int32_t)(int16_t)Vcc_mV / (65536*4));
        
        
        
        
//        double AinLsb = (0 == AinAdc3Buffer[VREFINT])? ADC_LSB : AinVrefintCal/(float)AinAdc3Buffer[VREFINT];
//        AinAdc1Lsb = AinLsb / (U9_GAIN * R68);
//        AinAdc2Lsb = AinLsb / (U12_GAIN * R72);
//        AinInputCurrent = CIN_ATTEN * (float)AinAdc3Buffer[CIN] * AinLsb;
//        AinInputVoltage = VIN_ATTEN * (float)AinAdc3Buffer[VIN] * AinLsb;
//        AinVaux = VAUX_ATTEN * (float)AinAdc3Buffer[VAUX] * AinLsb;
//        AinVcc = VCC_ATTEN * (float)AinAdc3Buffer[VCC] * AinLsb;
//        AinVcore = VCORE_ATTEN * (float)AinAdc3Buffer[VCORE] * AinLsb;
//        AinCoreTemp = AinTempGain * (float)AinAdc3Buffer[TEMP] * AinLsb + AinTempOffs;
//        AinMot1Temp = PTC_GAIN * (float)AinAdc3Buffer[PTC1] * AinLsb + PTC_OFFS;
//        AinMot2Temp = PTC_GAIN * (float)AinAdc3Buffer[PTC2] * AinLsb + PTC_OFFS;
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
        embot::core::print("qui: callback");
    }


    void AdcPwrTransferComplete_callback(ADC_HandleTypeDef *hadc)
    {
        /* Force the readback of the DMA buffer */
        //SCB_InvalidateDCache_by_Addr((void *)AdcPwrRawData, sizeof(AdcPwrRawData));
        /* Second half of the buffer ready */
        Adc3DmaComplete_callback(&(AdcPwrRawData[bsp::NUMBER_OF_ADC3_CHANNELS]));
    }

        
    void init(const Configuration &config)
    {
        
        static bool onceonly_initted {false};
                
        if(false == onceonly_initted)
        {
            HAL_TIM_Base_Stop(&htim15);
            HAL_ADC_Stop_DMA(&hadc3);
                  
            __HAL_TIM_SetCounter(&htim15, 0);
            if (HAL_OK == HAL_TIM_Base_Start(&htim15))
            {
                /* Calibrate ADC3 */
                if (HAL_OK == HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED))
                {
                    /* Register DMA callback functions */
                    if ((HAL_OK == HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_HALF_CB_ID, AdcPwrHalfTransfer_callback)) &&
                        (HAL_OK == HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AdcPwrTransferComplete_callback)))
                    {
                        /* Calculate the proportionality constant */
                        Kvrefcal = static_cast<uint32_t>(VREFINT_CAL_VREF) * bsp::VREFINT_CAL;

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
                        if (HAL_OK ==HAL_ADC_Start_DMA(&hadc3, (uint32_t *)AdcPwrRawData.data(), 2*bsp::NUMBER_OF_ADC3_CHANNELS))
                        {
                        embot::core::print("qui: HAL_ADC_Start_DMA tutto ok");
                        }
                    }   
                }
            }
            onceonly_initted = true;
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

