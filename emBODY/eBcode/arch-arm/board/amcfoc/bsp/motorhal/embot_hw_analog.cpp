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


namespace embot::hw::analog {

    
    /* ADC related constants */
    constexpr float    ADC_VREF = 2.5;     /*V*/
    constexpr uint8_t  ADC_NUMBER_OF_BITS = 12;
    constexpr uint16_t ADC_RESOLUTION = (1U << ADC_NUMBER_OF_BITS);
    constexpr float    ADC_LSB = ADC_VREF / static_cast<float>(ADC_RESOLUTION);
    constexpr uint8_t  NUMBER_OF_ADC3_CHANNELS = 10;
    
    
#if 0 
    //These limits are not used right now
    /* Currents, voltages and temperature limits */
    #define AIN_MAX_INPUT_CURRENT   (0.400)
    #define AIN_MAX_INPUT_VOLTAGE   (58.0)
    #define AIN_MIN_INPUT_VOLTAGE   (9.5)
    #define AIN_MAX_VCORE           (1.21*1.05)
    #define AIN_MIN_VCORE           (1.21*0.95)
    #define AIN_MAX_VCC             (3.33*1.05)
    #define AIN_MIN_VCC             (3.33*0.95)
    #define AIN_MAX_VAUX            (5.00*1.05)
    #define AIN_MIN_VAUX            (5.00*0.95)
    #define AIN_MAX_CORE_TEMP       (125.0)
    #define AIN_MAX_PTC1_TEMP       (90.0)
    #define AIN_MAX_PTC2_TEMP       (90.0)
#endif

       
    /* AMC-FOC schematics related constants */
    constexpr float R73  = 33000.0;
    constexpr float R84  = 22000.0;
    constexpr float R85  = 22000.0;
    constexpr float R74  = 33000.0;
    constexpr float R139 = 560000.0;
    constexpr float R75  = 24000.0;
    constexpr float R140 = 0.1;
    constexpr uint8_t U18_GAIN  = 20;
    constexpr float VAUX_ATTEN  = ((R73+R84)/R84);
    constexpr float VCC_ATTEN   = ((R85+R74)/R74);
    constexpr float VCORE_ATTEN = 1.0;
    constexpr float VIN_ATTEN   = ((R139+R75)/R75);
    constexpr float CIN_ATTEN   = (1.0/(R140*U18_GAIN));
    constexpr float PTC_GAIN    = (132.0/2.5);
    constexpr float PTC_OFFS    = (-12.0);

        
    /* LSB voltage of all ADC */
    volatile float AinLsb = ADC_LSB;


    //values taken from stm32h7xx_ll_adc.h
    #define VREFINT_CAL_ADDR                   ((uint16_t*) (0x1FF1E860UL)) /* Internal voltage reference, address of parameter VREFINT_CAL: VrefInt ADC raw data acquired at temperature 30 DegC (tolerance: +-5 DegC), Vref+ = 3.3 V (tolerance: +-10 mV). */
    #define VREFINT_CAL_VREF                   (3300UL)                     /* Analog voltage reference (Vref+) value with which temperature sensor has been calibrated in production (tolerance: +-10 mV) (unit: mV). */
    //    #define TS_CAL1                     TEMPSENSOR_CAL1_ADDR  
    //    #define TS_CAL2                     TEMPSENSOR_CAL2_ADDR
    
    /* Calculate the VREFINT value given by the manufacturer */
    static const float AinVrefintCal = ((float)(*VREFINT_CAL_ADDR))*(float)VREFINT_CAL_VREF/65536000.0;
    
    
      //to do for measuring AinCoreTemp
      /* Core temperature and VREFINT calibration */
      //are constant, to be calculated
//    float AinTempGain;
//    float AinTempOffs;

//    AinTempGain = (float)(TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP)/(float)(TS_CAL2 - TS_CAL1);
//    AinTempOffs = (float)TEMPSENSOR_CAL1_TEMP - AinTempGain*(float)TS_CAL1;
//    /* Compensate the resolution and VREF */
//    AinTempGain *= 65536000.0/(float)TEMPSENSOR_CAL_VREFANALOG;


}



namespace embot::hw::analog {
    

    #warning need to do a bsp
        

    #if defined(STM32HAL_BOARD_AMCFOC_1CM7)
    #define hadc3 (embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS)
    #define htim15 (embot::hw::motor::bldc::bsp::amcfoc::cm7::htimTriggerOfadcOTHERS)
    #else 
    #warning bsp of AMCFOC_1CM7

    #endif  
        
    
    enum class Ain3Channels
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
    };


    volatile float    AinInputCurrent;           /* Input current (A) */
    volatile float    AinInputVoltage;           /* Input voltage (V) */
    volatile float    AinVaux;                   /* Auxiliary +5V voltage (V) */
    volatile float    AinVcc;                    /* Main 3.3V voltage (V) */
    volatile float    AinVcore;                  /* Core 1.2V voltage (V) */
    volatile float    AinCoreTemp;               /* Core temperature (C) */
    volatile float    AinMot1Temp;               /* Motor 1 temperature (C) */    //temperature of the motor driver of T-drive
    volatile float    AinMot2Temp;               /* Motor 2 temperature (C) */
    


//    /* DMA circular buffers for ADC3: Double buffer */
//    volatile uint16_t AinDma3Buffer[2 * ADC3_NUMBER_OF_CHANNELS] __attribute__((aligned (4)));
    /* DMA buffer for ADC3 raw data */
    //static uint16_t AinAdc3Buffer[16]; // TODO: re-check (compare) this parameter w/ the one used by GZini (it is different)
    alignas(32) static std::array<uint16_t, 2 * NUMBER_OF_ADC3_CHANNELS> AinAdc3Buffer;
    /* DMA circular buffers for ADC3: Double buffer */




    /*******************************************************************************************************************//**
     * @brief
     * @param
     * @return
     */
    void Adc3DmaComplete_callback(volatile uint16_t sample[], uint8_t x)
    {       
        
        AinLsb = (0 == AinAdc3Buffer[embot::core::tointegral(Ain3Channels::VREFINT)])? ADC_LSB : AinVrefintCal/(float)AinAdc3Buffer[embot::core::tointegral(Ain3Channels::VREFINT)];
        AinInputCurrent = CIN_ATTEN * (float)AinAdc3Buffer[embot::core::tointegral(Ain3Channels::CIN)] * AinLsb;
        AinInputVoltage = VIN_ATTEN * (float)AinAdc3Buffer[embot::core::tointegral(Ain3Channels::VIN)] * AinLsb;
        
//        uint64_t t = embot::core::now(); 
//        embot::core::TimeFormatter tf{t};
//        embot::core::print(std::to_string(x) +" "+tf.to_string() +" "+ std::to_string(AinInputVoltage));

        AinVaux = VAUX_ATTEN * (float)AinAdc3Buffer[embot::core::tointegral(Ain3Channels::VAUX)] * AinLsb;
        AinVcc = VCC_ATTEN * (float)AinAdc3Buffer[embot::core::tointegral(Ain3Channels::VCC)] * AinLsb;
        AinVcore = VCORE_ATTEN * (float)AinAdc3Buffer[embot::core::tointegral(Ain3Channels::VCORE)] * AinLsb;
//        AinCoreTemp = AinTempGain * (float)AinAdc3Buffer[embot::core::tointegral(Ain3Channels::TEMP)] * AinLsb + AinTempOffs;
        AinMot1Temp = PTC_GAIN * (float)AinAdc3Buffer[embot::core::tointegral(Ain3Channels::PTC1)] * AinLsb + PTC_OFFS;
        AinMot2Temp = PTC_GAIN * (float)AinAdc3Buffer[embot::core::tointegral(Ain3Channels::PTC2)] * AinLsb + PTC_OFFS;
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
//        SCB_InvalidateDCache_by_Addr(reinterpret_cast<void*>(AinAdc3Buffer.data()), sizeof(AinAdc3Buffer));
        /* First half of the buffer ready */
        Adc3DmaComplete_callback(&(AinAdc3Buffer[0]),0);
    }


    void AdcPwrTransferComplete_callback(ADC_HandleTypeDef *hadc)
    {
        /* Force the readback of the DMA buffer */
//        SCB_InvalidateDCache_by_Addr(reinterpret_cast<void*>(AinAdc3Buffer.data()), sizeof(AinAdc3Buffer));
        /* Second half of the buffer ready */
        Adc3DmaComplete_callback(&(AinAdc3Buffer[NUMBER_OF_ADC3_CHANNELS]),1);
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

                        std::fill(std::begin(AinAdc3Buffer), std::end(AinAdc3Buffer), 0);
                        //SCB_CleanInvalidateDCache_by_Addr((void *)AdcDmaBuffer, sizeof(AdcDmaBuffer));

                        /* Start the converter in DMA mode */
                        if (HAL_OK ==HAL_ADC_Start_DMA(&hadc3, (uint32_t *)AinAdc3Buffer.data(), 2*NUMBER_OF_ADC3_CHANNELS))
                        {
//                        embot::core::print("qui: HAL_ADC_Start_DMA tutto ok");
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
        return AinInputVoltage;
    }

    float getCin()
    {   
        // return Ain in Ampere
        return AinInputCurrent;
    }
    
} // namespace embot::hw::analog
 
#endif // #if defined(EMBOT_ENABLE_hw_analog_ish)

#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc)



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

