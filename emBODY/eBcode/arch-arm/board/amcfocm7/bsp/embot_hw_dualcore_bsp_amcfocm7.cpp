
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>
#include <array>

#include "embot_core_binary.h"
#include "embot_core.h"

#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace std;
using namespace embot::core::binary;

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_amcfocm7_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------


// - support map: begin of embot::hw::dualcore

#include "embot_hw_dualcore_bsp.h"

#if !defined(EMBOT_ENABLE_hw_dualcore)

namespace embot::hw::dualcore::bsp {
    
    constexpr BSP thebsp {};  
    const BSP& getBSP() 
    {
        return thebsp;
    }    
    Config _config {};
    bool BSP::config(const Config &c) const { return true; }
    bool BSP::start() const { return true; }
    const Config& BSP::config() const { return _config; }
}

#else

namespace embot::hw::dualcore::bsp {         

#if defined(dualcore_BOOT_cm7master)      
    constexpr PROP _cm7 = { embot::hw::dualcore::CORE::cm7, embot::hw::dualcore::BOOT::cm7master, embot::hw::MTX::one};  
    Config _config {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate};
#elif defined(dualcore_BOOT_cm4master)
    constexpr PROP _cm7 = { embot::hw::dualcore::CORE::cm7, embot::hw::dualcore::BOOT::cm4master, embot::hw::MTX::one};
    Config _config {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::donothing};
#endif
    
    constexpr BSP thebsp {        

        // properties
        {{
            &_cm7         
        }}        
    };
    
   

    const BSP& getBSP() 
    {
        return thebsp;
    }
    
    bool BSP::config(const Config &c) const
    {
        _config = c;        
        return true;
    } 

    const Config& BSP::config() const
    {
        return _config;
    }        

    
    void mySystemClock_Config(void);
    void icub_SystemClock_Config(void);
    
    bool BSP::init() const
    {

        if(embot::hw::dualcore::BOOT::cm7master == _cm7.boot)
        {        
            // the cm7 is master, so it must call HAL_Init() and start the clocks
            
            //MPU_Config();
            HAL_MPU_Disable();
//            SCB_EnableICache();
//            SCB_EnableDCache();    
            HAL_Init();                    
           // mySystemClock_Config();  
            icub_SystemClock_Config();          
        }
        else
        {
        
            // the cm7 is slave, so the cm4 has already done everything.
            // only thing is to enable the cache as only the cm7 can do it
            // ... maybe add some MPU initialization
            
//            SCB_EnableICache();
//            SCB_EnableDCache();            
        }
            
        return true; 
    }




    
} // namespace embot::hw::dualcore::bsp {


// in here are the SystemClock_Config() that we use

namespace embot::hw::dualcore::bsp {

        // from icubtech
    
    void icub_SystemClock_Config(void)
    {
      RCC_OscInitTypeDef RCC_OscInitStruct = {0};
      RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

      /** Supply configuration update enable
      */
      HAL_PWREx_ConfigSupply(PWR_EXTERNAL_SOURCE_SUPPLY);

      /** Configure the main internal regulator output voltage
      */
      __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE0);

      while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

      /** Configure LSE Drive Capability
      */
      HAL_PWR_EnableBkUpAccess();
      __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

      /** Macro to configure the PLL clock source
      */
      __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);

      /** Initializes the RCC Oscillators according to the specified parameters
      * in the RCC_OscInitTypeDef structure.
      */
      RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE|RCC_OSCILLATORTYPE_LSE;
      RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
      RCC_OscInitStruct.LSEState = RCC_LSE_ON;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
      RCC_OscInitStruct.PLL.PLLM = 2;
      RCC_OscInitStruct.PLL.PLLN = 64;
      RCC_OscInitStruct.PLL.PLLP = 2;
      RCC_OscInitStruct.PLL.PLLQ = 8;
      RCC_OscInitStruct.PLL.PLLR = 2;
      RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
      RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
      RCC_OscInitStruct.PLL.PLLFRACN = 0;
      if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
      {
        for(;;);
      }

      /** Initializes the CPU, AHB and APB buses clocks
      */
      RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                  |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                  |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
      RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
      RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
      RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

      if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
      {
        for(;;);
      }
      HAL_RCC_MCOConfig(RCC_MCO1, RCC_MCO1SOURCE_PLL1QCLK, RCC_MCODIV_4);
      
      HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
    }  

    // from stm

    /**
      * @brief  System Clock Configuration
      *         The system Clock is configured as follow : 
      *            System Clock source            = PLL (HSE)
      *            SYSCLK(Hz)                     = 400000000 (Cortex-M7 CPU Clock)
      *            HCLK(Hz)                       = 200000000 (Cortex-M4 CPU, Bus matrix Clocks)
      *            AHB Prescaler                  = 2
      *            D1 APB3 Prescaler              = 2 (APB3 Clock  100MHz)
      *            D2 APB1 Prescaler              = 2 (APB1 Clock  100MHz)
      *            D2 APB2 Prescaler              = 2 (APB2 Clock  100MHz)
      *            D3 APB4 Prescaler              = 2 (APB4 Clock  100MHz)
      *            HSE Frequency(Hz)              = 25000000
      *            PLL_M                          = 5
      *            PLL_N                          = 160
      *            PLL_P                          = 2
      *            PLL_Q                          = 4
      *            PLL_R                          = 2
      *            VDD(V)                         = 3.3
      *            Flash Latency(WS)              = 4
      * @param  None
      * @retval None
      */
    void mySystemClock_Config(void)
    {
      RCC_ClkInitTypeDef RCC_ClkInitStruct;
      RCC_OscInitTypeDef RCC_OscInitStruct;
      HAL_StatusTypeDef ret = HAL_OK;
      
      /*!< Supply configuration update enable */
      HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);

      /* The voltage scaling allows optimizing the power consumption when the device is 
         clocked below the maximum system frequency, to update the voltage scaling value 
         regarding system frequency refer to product datasheet.  */
      __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

      while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
      
      /* Enable HSE Oscillator and activate PLL with HSE as source */
      RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
      RCC_OscInitStruct.HSEState = RCC_HSE_ON;
      RCC_OscInitStruct.HSIState = RCC_HSI_OFF;
      RCC_OscInitStruct.CSIState = RCC_CSI_OFF;
      RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
      RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;

      RCC_OscInitStruct.PLL.PLLM = 5;
      RCC_OscInitStruct.PLL.PLLN = 160;
      RCC_OscInitStruct.PLL.PLLFRACN = 0;
      RCC_OscInitStruct.PLL.PLLP = 2;
      RCC_OscInitStruct.PLL.PLLR = 2;
      RCC_OscInitStruct.PLL.PLLQ = 4;

      RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
      RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
      ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
      if(ret != HAL_OK)
      {
        for(;;);
      }
      
    /* Select PLL as system clock source and configure  bus clocks dividers */
      RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
                                     RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
      RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;  
      RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; 
      RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2; 
      RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2; 
      ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
      if(ret != HAL_OK)
      {
        for(;;);
      }
      
      /*
      Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
              (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to product datasheet)       
              The I/O Compensation Cell activation  procedure requires :
            - The activation of the CSI clock
            - The activation of the SYSCFG clock
            - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
      
              To do this please uncomment the following code 
      */
     
      /*  
      __HAL_RCC_CSI_ENABLE() ;
      
      __HAL_RCC_SYSCFG_CLK_ENABLE() ;
      
      HAL_EnableCompensationCell();
      */  
    }    
    
} // namespace embot::hw::dualcore::bsp {

#endif // dualcore

// - support map: end of embot::hw::dualcore



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

