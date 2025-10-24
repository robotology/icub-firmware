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
//#define TEST_ANALOGS

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
    float getDriver1Temp() { return 25.0f; }
    float getDriver2Temp() { return 25.0f; }
    float getVaux()  { return 5.0f; }
    float getVcc()   { return 3.3f; }
    float getVcore() { return 1.2f; }
       
}

#elif defined(EMBOT_ENABLE_hw_analog_ish)  

namespace embot::hw::analog {
    
    #if defined(STM32HAL_BOARD_AMCFOC_1CM7)
    #define hadcAnIn (embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS)
    #else 
    #warning bsp of AMCFOC_1CM7
    #endif  
    
    /* ADC related constants */
    constexpr float    ADC_VREF = 2.5;     /*V*/
    constexpr uint8_t  ADC_NUMBER_OF_BITS = 12;
    constexpr uint16_t ADC_RESOLUTION = (1U << ADC_NUMBER_OF_BITS);
    constexpr float    ADC_LSB = ADC_VREF / static_cast<float>(ADC_RESOLUTION);
    constexpr uint8_t  NUMBER_OF_ADC3_CHANNELS = 10;
    
    
    //values taken from stm32h7xx_ll_adc.h
    #define TS_CAL1                     *TEMPSENSOR_CAL1_ADDR    
    #define TS_CAL2                     *TEMPSENSOR_CAL2_ADDR    
    
    
    /* Currents, voltages and temperature limits */
    constexpr float AIN_MAX_INPUT_CURRENT  = 0.400;
    constexpr float AIN_MAX_INPUT_VOLTAGE  = 58.0;
    constexpr float AIN_MIN_INPUT_VOLTAGE  = 9.5;
    constexpr float AIN_MAX_VCORE          = 1.21*1.05;
    constexpr float AIN_MIN_VCORE          = 1.21*0.95;
    constexpr float AIN_MAX_VCC            = 3.33*1.05;
    constexpr float AIN_MIN_VCC            = 3.33*0.95;
    constexpr float AIN_MAX_VAUX           = 5.00*1.05;
    constexpr float AIN_MIN_VAUX           = 5.00*0.95;
    constexpr float AIN_MAX_CORE_TEMP      = 125.0;
    constexpr float AIN_MAX_PTC1_TEMP      = 90.0;
    constexpr float AIN_MAX_PTC2_TEMP      = 90.0;

       
    /* AMC-FOC schematics related constants */
    constexpr float R73  = 33000.0;
    constexpr float R84  = 22000.0;
    constexpr float R85  = 22000.0;
    constexpr float R74  = 33000.0;
    constexpr float R139 = 560000.0;
    constexpr float R75  = 24000.0;
    constexpr float R140 = 0.1;
    constexpr float U18_GAIN  = 20.0;
    constexpr float VAUX_ATTEN  = ((R73+R84)/R84);
    constexpr float VCC_ATTEN   = ((R85+R74)/R74);
    constexpr float VCORE_ATTEN = 1.0;
    constexpr float VIN_ATTEN   = ((R139+R75)/R75);
    constexpr float CIN_ATTEN   = (1.0/(R140*U18_GAIN));
    constexpr float PTC_GAIN    = (132.0/2.5);
    constexpr float PTC_OFFS    = (-12.0);

        
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
     
    /* LSB voltage of all ADC */
    volatile float    AinLsb = ADC_LSB;
    
    volatile float    AinInputCurrent;           /* Input current (A) */
    volatile float    AinInputVoltage;           /* Input voltage (V) */
    volatile float    AinVaux;                   /* Auxiliary +5V voltage (V) */
    volatile float    AinVcc;                    /* Main 3.3V voltage (V) */
    volatile float    AinVcore;                  /* Core 1.2V voltage (V) */
    volatile float    AinCoreTemp;               /* Core temperature (C) */
    volatile float    AinDriverMot1Temp;         /* Motor 1 temperature (C) */    //temperature of the motor driver of T-drive
    volatile float    AinDriverMot2Temp;         /* Motor 2 temperature (C) */    //temperature of the motor driver of T-drive


    /* Calculate the VREFINT value given by the manufacturer */
    static const float AinVrefintCal = static_cast<float>(*VREFINT_CAL_ADDR) * static_cast<float>(VREFINT_CAL_VREF) / 65536000.0f;

    /* Core temperaturecalibration */
    static const float AinTempGain = static_cast<float>(TEMPSENSOR_CAL2_TEMP - TEMPSENSOR_CAL1_TEMP) / static_cast<float>(TS_CAL2 - TS_CAL1);
    static const float AinTempOffs = static_cast<float>(TEMPSENSOR_CAL1_TEMP) - AinTempGain * static_cast<float>(TS_CAL1);
    /* Compensate the resolution and VREF */
    static const float AinTempGain_comp = AinTempGain * 65536000.0f / static_cast<float>(TEMPSENSOR_CAL_VREFANALOG);

    /* DMA circular buffers for ADC3: Double buffer */
    alignas(32) static std::array<volatile uint16_t, 2 * NUMBER_OF_ADC3_CHANNELS> AinAdc3Buffer;


    /*******************************************************************************************************************//**
     * @brief
     * @param
     * @return
     */
    void Adc3DmaComplete_callback(volatile uint16_t sample[])
    {       
        
        AinLsb = (0 == sample[embot::core::tointegral(Ain3Channels::VREFINT)]) ? ADC_LSB : AinVrefintCal/static_cast<float>(sample[embot::core::tointegral(Ain3Channels::VREFINT)]);
        AinInputCurrent =   CIN_ATTEN *         static_cast<float>(sample[embot::core::tointegral(Ain3Channels::CIN)])   * AinLsb;
        AinInputVoltage =   VIN_ATTEN *         static_cast<float>(sample[embot::core::tointegral(Ain3Channels::VIN)])   * AinLsb;
        AinVaux =           VAUX_ATTEN *        static_cast<float>(sample[embot::core::tointegral(Ain3Channels::VAUX)])  * AinLsb;
        AinVcc =            VCC_ATTEN *         static_cast<float>(sample[embot::core::tointegral(Ain3Channels::VCC)])   * AinLsb;
        AinVcore =          VCORE_ATTEN *       static_cast<float>(sample[embot::core::tointegral(Ain3Channels::VCORE)]) * AinLsb;
        AinCoreTemp =       AinTempGain_comp *  static_cast<float>(sample[embot::core::tointegral(Ain3Channels::TEMP)])  * AinLsb + AinTempOffs;
        AinDriverMot1Temp = PTC_GAIN *          static_cast<float>(sample[embot::core::tointegral(Ain3Channels::PTC1)])  * AinLsb + PTC_OFFS;
        AinDriverMot2Temp = PTC_GAIN *          static_cast<float>(sample[embot::core::tointegral(Ain3Channels::PTC2)])  * AinLsb + PTC_OFFS;

#ifdef TEST_ANALOGS
        MaxMin_analog_reads();
#endif
        
//        //test code
//        uint64_t t = embot::core::now(); 
//        embot::core::TimeFormatter tf{t};   
//        embot::core::print(tf.to_string() +" "+ std::to_string(AinInputVoltage)
//                            + " sample " + std::to_string(sample[embot::core::tointegral(Ain3Channels::VIN)]));
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
        Adc3DmaComplete_callback(&(AinAdc3Buffer[0]));
    }


    void AdcPwrTransferComplete_callback(ADC_HandleTypeDef *hadc)
    {
        /* Force the readback of the DMA buffer */
//        SCB_InvalidateDCache_by_Addr(reinterpret_cast<void*>(AinAdc3Buffer.data()), sizeof(AinAdc3Buffer));
        /* Second half of the buffer ready */
        Adc3DmaComplete_callback(&(AinAdc3Buffer[NUMBER_OF_ADC3_CHANNELS]));
    }

        
    void init(const Configuration &config)
    {
        
        static bool onceonly_initted {false};
                
        if(false == onceonly_initted)
        {
            HAL_ADC_Stop_DMA(&hadcAnIn);
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
        // return Core Temperature in �C
        return AinCoreTemp;
    }
    
    
    float getDriver1Temp()
    {
        // return Motor Driver 1 Temperature in �C
        return AinDriverMot1Temp;
    }
    
    float getDriver2Temp()
    {
        // return Motor Driver 2 Temperature in �C
        return AinDriverMot2Temp;
    }
    
    float getVaux()
    {
        // return Auxiliary +5V voltage
        return AinVaux;
    }
    
    float getVcc()
    {
        // return Main 3.3V voltage (V)
        return AinVcc;
    }
    
    float getVcore()
    {
        // return Core 1.2V voltage (V)
        return AinVcore;
    }
    
#ifdef TEST_ANALOGS
    
    volatile float AinInputVoltageMax = 5.0;
    volatile float AinInputVoltageMin = 60.0;
    
    volatile float AinInputCurrentMax = 0.0;
    
    volatile float AinCoreTempMax = 10.0;
    
    volatile float AinDriverMot1TempMax = 10.0;
    volatile float AinDriverMot2TempMax = 10.0;
    
    volatile float AinVauxMax = 4.0;
    volatile float AinVauxMin = 6.0;
    
    volatile float AinVccMax = 3.3;
    volatile float AinVccMin = 3.3;
    
    volatile float AinVcoreMax = 1.2;
    volatile float AinVcoreMin = 1.2;
    
    
    void MaxMin_analog_reads()
    {
        //Vin
        if (getVin() > AinInputVoltageMax) { AinInputVoltageMax = getVin(); }
        else if (getVin() < AinInputVoltageMin) { AinInputVoltageMin = getVin(); }
        if (getVin() >= AIN_MAX_INPUT_VOLTAGE)  embot::core::print("Vin Max reached!!!!!");
        if (getVin() <= AIN_MIN_INPUT_VOLTAGE)  embot::core::print("Vin Min reached!!!!!");
        //Cin
        if (getCin() > AinInputCurrentMax) { AinInputCurrentMax = getCin(); }
        if (getCin() >= AIN_MAX_INPUT_CURRENT)  embot::core::print("Cin Max reached!!!!!");
        //T-driver temp
        if (getDriver1Temp() > AinDriverMot1TempMax) { AinDriverMot1TempMax = getDriver1Temp(); }
        if (getDriver1Temp() >= AIN_MAX_PTC1_TEMP)  embot::core::print("TEMP MOT1 driver Max reached!!!!!");
        if (getDriver2Temp() > AinDriverMot2TempMax) { AinDriverMot2TempMax = getDriver2Temp(); }
        if (getDriver2Temp() >= AIN_MAX_PTC2_TEMP)  embot::core::print("TEMP MOT2 driver Max reached!!!!!");
        
        //Tcore
        if (getCoreTemp() > AinCoreTempMax) { AinCoreTempMax = getCoreTemp(); }
        if (getCoreTemp() >= AIN_MAX_CORE_TEMP)  embot::core::print("Tcore Max reached!!!!!");
        
        //Vaux,Vcc,Vcore
        if (getVaux() > AinVauxMax) { AinVauxMax = getVaux(); }
        else if (getVaux() < AinVauxMin) { AinVauxMin = getVaux(); }
        if (getVcc() > AinVccMax) { AinVccMax = getVcc(); }
        else if (getVcc() < AinVccMin) { AinVccMin = getVcc(); }
        if (getVcore() > AinVcoreMax) { AinVcoreMax = getVcore(); }
        else if (getVcore() < AinVcoreMin) { AinVcoreMin = getVcore(); }
        
        if(getVaux() >= AIN_MAX_VAUX)  embot::core::print("Vaux Max reached!!!!!");
        if(getVaux() <= AIN_MIN_VAUX)  embot::core::print("Vaux Min reached!!!!!");
        
        if(getVcc() >= AIN_MAX_VCC)  embot::core::print("Vcc Max reached!!!!!");
        if(getVcc() <= AIN_MIN_VCC)  embot::core::print("Vcc Min reached!!!!!");
        
        if(getVcore() >= AIN_MAX_VCORE)  embot::core::print("Vcore Max reached!!!!!");
        if(getVcore() <= AIN_MIN_VCORE)  embot::core::print("Vcore Min reached!!!!!");
        
        
    }
    
    
    void print_Analogs()
    {
        embot::core::print("Vin Max: "+ std::to_string(AinInputVoltageMax) + " Vin Min: "+ std::to_string(AinInputVoltageMin));
        embot::core::print("Cin Max: "+ std::to_string(AinInputCurrentMax));
        embot::core::print("Tcore Max: "+ std::to_string(AinCoreTempMax));
        embot::core::print("Tmp MOT1 driver Max: "+ std::to_string(AinDriverMot1TempMax) + " Tmp MOT2 driver Max: "+ std::to_string(AinDriverMot2TempMax));
        embot::core::print("Vaux Max: "+ std::to_string(AinVauxMax) + " Vaux Min: "+ std::to_string(AinVauxMin));
        embot::core::print("Vcc  Max: "+ std::to_string(AinVccMax) + " Vcc Min: "+ std::to_string(AinVccMin));
        embot::core::print("Vcore Max: "+ std::to_string(AinVcoreMax) + " Vcore Min: "+ std::to_string(AinVcoreMin));  
    }
    
    
#endif // TEST_ANALOGS
    
} // namespace embot::hw::analog
 
#endif // #if defined(EMBOT_ENABLE_hw_analog_ish)

#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc)



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

