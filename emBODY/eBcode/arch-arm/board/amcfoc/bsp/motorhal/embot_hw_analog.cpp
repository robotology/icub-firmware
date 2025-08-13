/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Kevin Sangalli
 * email:   marco.accame@iit.it, kevin.sanngalli@iit.it
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
    float getCoreTemp() { return 25.0f; }
    float getMot1Temp() { return 25.0f; }
    float getMot2Temp() { return 25.0f; }
       
}

#elif defined(EMBOT_ENABLE_hw_analog_ish)  

namespace embot::hw::analog {
    
    #if defined(STM32HAL_BOARD_AMCFOC_1CM7)
    #define hadcAnIn (embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS)
    #define htimAnIn (embot::hw::motor::bldc::bsp::amcfoc::cm7::htimTriggerOfadcOTHERS)
    #else 
    #warning bsp of AMCFOC_1CM7
    #endif  

     
    /* LSB voltage of all ADC */
    volatile float AinLsb = ADC_LSB;
    
    volatile float    AinInputCurrent;           /* Input current (A) */
    volatile float    AinInputVoltage;           /* Input voltage (V) */
    volatile float    AinVaux;                   /* Auxiliary +5V voltage (V) */
    volatile float    AinVcc;                    /* Main 3.3V voltage (V) */
    volatile float    AinVcore;                  /* Core 1.2V voltage (V) */
    volatile float    AinCoreTemp;               /* Core temperature (C) */
    volatile float    AinMot1Temp;               /* Motor 1 temperature (C) */    //temperature of the motor driver of T-drive
    volatile float    AinMot2Temp;               /* Motor 2 temperature (C) */

    
    /* Calculate the VREFINT value given by the manufacturer */
    static const float AinVrefintCal = ((float)(*VREFINT_CAL_ADDR))*(float)VREFINT_CAL_VREF/65536000.0;
    
    /* Core temperaturecalibration */
    float AinTempGain = (float)(TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP)/(float)(TS_CAL2 - TS_CAL1);
    float AinTempOffs = (float)TEMPSENSOR_CAL1_TEMP - AinTempGain*(float)TS_CAL1;
    /* Compensate the resolution and VREF */
    float AinTempGain_compensated = AinTempGain * 65536000.0/(float)TEMPSENSOR_CAL_VREFANALOG;
    




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
        AinCoreTemp = AinTempGain_compensated * (float)AinAdc3Buffer[embot::core::tointegral(Ain3Channels::TEMP)] * AinLsb + AinTempOffs;
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
            HAL_TIM_Base_Stop(&htimAnIn);
            HAL_ADC_Stop_DMA(&hadcAnIn);
                  
            __HAL_TIM_SetCounter(&htimAnIn, 0);
            if (HAL_OK == HAL_TIM_Base_Start(&htimAnIn))
            {
                /* Calibrate ADC3 */
                if (HAL_OK == HAL_ADCEx_Calibration_Start(&hadcAnIn, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED))
                {
                    /* Register DMA callback functions */
                    if ((HAL_OK == HAL_ADC_RegisterCallback(&hadcAnIn, HAL_ADC_CONVERSION_HALF_CB_ID, AdcPwrHalfTransfer_callback)) &&
                        (HAL_OK == HAL_ADC_RegisterCallback(&hadcAnIn, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AdcPwrTransferComplete_callback)))
                    {

                        std::fill(std::begin(AinAdc3Buffer), std::end(AinAdc3Buffer), 0);
                        //SCB_CleanInvalidateDCache_by_Addr((void *)AdcDmaBuffer, sizeof(AdcDmaBuffer));

                        /* Start the converter in DMA mode */
                        if (HAL_OK ==HAL_ADC_Start_DMA(&hadcAnIn, (uint32_t *)AinAdc3Buffer.data(), 2*NUMBER_OF_ADC3_CHANNELS))
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
        HAL_ADCEx_MultiModeStop_DMA(&hadcAnIn);
        HAL_ADC_UnRegisterCallback(&hadcAnIn, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
        HAL_ADC_UnRegisterCallback(&hadcAnIn, HAL_ADC_CONVERSION_HALF_CB_ID);
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
    
    float getCoreTemp()
    { 
        return AinCoreTemp;
    }
    
    
    float getMot1Temp()
    {
        return AinMot1Temp;
    }
    
    float getMot2Temp()
    {
        return AinMot2Temp;
    }
    
    
    
} // namespace embot::hw::analog
 
#endif // #if defined(EMBOT_ENABLE_hw_analog_ish)

#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc)



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

