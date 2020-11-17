
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
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

#include "embot_hw_bsp_stm32h745disco_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------

#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   

    #if   defined(STM32HAL_BOARD_STM32H745DISCO)
    
    // clock @ 400 mhz
            
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
      *            PLL_Q                          = 40
      *            PLL_R                          = 2
      *            VDD(V)                         = 3.3
      *            Flash Latency(WS)              = 4
      * @param  None
      * @retval None
      */
    static void my_SystemClock_Config(void)
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
      RCC_OscInitStruct.PLL.PLLQ = 20;  /* fdcan_ker_ck = 40 MHz */

      RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
      RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_2;
      ret = HAL_RCC_OscConfig(&RCC_OscInitStruct);
      if(ret != HAL_OK)
      {
        Error_Handler();
      }

      /* Select PLL as system clock source and configure  bus clocks dividers */
      RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_D1PCLK1 | RCC_CLOCKTYPE_PCLK1 | \
                                     RCC_CLOCKTYPE_PCLK2  | RCC_CLOCKTYPE_D3PCLK1);

      RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
      RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
      RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
      RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
      RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2; /* fdcan_pclk = 100 MHz */
      RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
      RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;
      ret = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4);
      if(ret != HAL_OK)
      {
        Error_Handler();
      }

      /*
      Note : The activation of the I/O Compensation Cell is recommended with communication  interfaces
              (GPIO, SPI, FMC, QSPI ...)  when  operating at  high frequencies(please refer to product datasheet)
              The I/O Compensation Cell activation  procedure requires :
            - The activation of the CSI clock
            - The activation of the SYSCFG clock
            - Enabling the I/O Compensation Cell : setting bit[0] of register SYSCFG_CCCSR
      */
      __HAL_RCC_CSI_ENABLE() ;

      __HAL_RCC_SYSCFG_CLK_ENABLE() ;

      HAL_EnableCompensationCell();
    }   

    /**
      * @brief  CPU L1-Cache enable.
      * @param  None
      * @retval None
      */
    static void my_CPU_CACHE_Enable(void)
    {
      /* Enable I-Cache */
      SCB_EnableICache();

      /* Enable D-Cache */
      SCB_EnableDCache();
    }  
       
    bool embot::hw::bsp::specialize()
    {
        my_CPU_CACHE_Enable();
        my_SystemClock_Config();
        
        return true;
    }

   
    #else
        #error embot::hw::bsp::specialize() must be defined    
    #endif  
#endif  //EMBOT_ENABLE_hw_bsp_specialize


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------



// - support map: begin of embot::hw::gpio

#include "embot_hw_gpio_bsp.h"

#if   !defined(HAL_GPIO_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_gpio)

#error CAVEAT: embot::hw requires GPIO. pls enable it!

namespace embot { namespace hw { namespace gpio {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::GPIO h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else
    
namespace embot { namespace hw { namespace gpio {
 
    #if defined(STM32HAL_BOARD_STM32H745DISCO)
    static const BSP thebsp {        
        // supportmask2d
        {{
            0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff 
        }},            
        // ports
        {{
            GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI, GPIOJ, GPIOK
        }},
        {{
            [](){__HAL_RCC_GPIOA_CLK_ENABLE();}, [](){__HAL_RCC_GPIOB_CLK_ENABLE();}, [](){__HAL_RCC_GPIOC_CLK_ENABLE();}, 
            [](){__HAL_RCC_GPIOD_CLK_ENABLE();}, [](){__HAL_RCC_GPIOE_CLK_ENABLE();}, [](){__HAL_RCC_GPIOF_CLK_ENABLE();},
            [](){__HAL_RCC_GPIOG_CLK_ENABLE();}, [](){__HAL_RCC_GPIOH_CLK_ENABLE();}, [](){__HAL_RCC_GPIOI_CLK_ENABLE();},
            [](){__HAL_RCC_GPIOJ_CLK_ENABLE();}, [](){__HAL_RCC_GPIOK_CLK_ENABLE();}
        }}
    };      
    #else
        #error embot::hw::gpio::thebsp must be defined
    #endif
    
    
    void BSP::init(embot::hw::GPIO h) const {}        
        
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}} // namespace embot { namespace hw { namespace gpio {

#endif // gpio

// - support map: end of embot::hw::gpio



// - support map: begin of embot::hw::led

#include "embot_hw_led_bsp.h"

#if !defined(EMBOT_ENABLE_hw_led)

namespace embot { namespace hw { namespace led {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::LED h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace led {         
    
    #if   defined(STM32HAL_BOARD_STM32H745DISCO)
       
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::J, embot::hw::GPIO::PIN::two}  };  
    constexpr PROP led2p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::I, embot::hw::GPIO::PIN::thirteen}  };  

        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one) | mask::pos2mask<uint32_t>(LED::two),        
        // properties
        {{
            &led1p, &led2p, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr            
        }}        
    };
    
    void clock_enable_J() { __HAL_RCC_GPIOJ_CLK_ENABLE(); }
    void clock_enable_I() { __HAL_RCC_GPIOI_CLK_ENABLE(); }
    void BSP::init(embot::hw::LED h) const 
    {
        // activate the clock if cube-mx didn't do that
        constexpr std::array<embot::core::fpWorker, 2> clockenable { clock_enable_J, clock_enable_I };
        
        uint8_t i = embot::core::tointegral(h);
        if(i < clockenable.size())
        {
            clockenable[i]();
        }
        
        // init the gpio
        const embot::hw::GPIO &g = thebsp.properties[i]->gpio;
        embot::hw::gpio::configure(g, embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::low);
                        
    } 

    #else
        #error embot::hw::led::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}} // namespace embot { namespace hw {  namespace led {

#endif // led

// - support map: end of embot::hw::led



// - support map: begin of embot::hw::button

#include "embot_hw_button.h"
#include "embot_hw_button_bsp.h"
#include "embot_hw_gpio_bsp.h"

#if !defined(EMBOT_ENABLE_hw_button)

namespace embot { namespace hw { namespace button {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::BTN h) const {}
    void BSP::onEXTI(const embot::hw::gpio::PROP &p) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace button {
    
    #if   defined(STM32HAL_BOARD_STM32H745DISCO)
        
    // this button is the blue one on the board
    constexpr PROP btn1p = { .pressed = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::thirteen}, 
                             .pull = embot::hw::gpio::Pull::nopull, .irqn = EXTI15_10_IRQn  };  
 
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(BTN::one),        
        // properties
        {{
            &btn1p, nullptr, nullptr            
        }}        
    };
    
    
    void BSP::init(embot::hw::BTN h) const {}
    
    void BSP::onEXTI(const embot::hw::gpio::PROP &p) const
    {
        const embot::hw::GPIO gpio = embot::hw::gpio::getBSP().getGPIO(p);
        switch(gpio.pin)
        {
            case embot::hw::GPIO::PIN::thirteen:
            {
                embot::hw::button::onexti(BTN::one);
            } break;
            

            default:
            {
            } break;           
        }              
    }
    
    // we put in here the IRQHandlers + the exti callback

    extern "C" {
            
        void EXTI15_10_IRQHandler(void)
        {
            HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_13);
        }
        
        void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
        {
             embot::hw::button::getBSP().onEXTI({nullptr, GPIO_Pin, nullptr});            
        }        
    }
        
    #else
        #error embot::hw::bsp::button::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#endif // button

// - support map: end of embot::hw::button



// - support map: begin of embot::hw::can

#include "embot_hw_can_bsp.h"

#if   !defined(EMBOT_ENABLE_hw_can)

namespace embot { namespace hw { namespace can {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::CAN h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace can {
               
    #if   defined(STM32HAL_BOARD_STM32H745DISCO)
    
    // it has HAL_FDCAN_MODULE_ENABLED
    
    FDCAN_HandleTypeDef hfdcan;
    
    /**
      * @brief  Configures the FDCAN.
      * @param  None
      * @retval None
      */
    static void my_FDCAN1_Config(void)
    {
      FDCAN_FilterTypeDef sFilterConfig;

      /* Bit time configuration:
        fdcan_ker_ck               = 40 MHz
        Time_quantum (tq)          = 25 ns
        Synchronization_segment    = 1 tq
        Propagation_segment        = 23 tq
        Phase_segment_1            = 8 tq
        Phase_segment_2            = 8 tq
        Synchronization_Jump_width = 8 tq
        Bit_length                 = 40 tq = 1 µs
        Bit_rate                   = 1 MBit/s
      */
      hfdcan.Instance = FDCAN1;
      hfdcan.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
      hfdcan.Init.Mode = FDCAN_MODE_NORMAL;
      hfdcan.Init.AutoRetransmission = ENABLE;
      hfdcan.Init.TransmitPause = DISABLE;
      hfdcan.Init.ProtocolException = ENABLE;
      hfdcan.Init.NominalPrescaler = 0x1; /* tq = NominalPrescaler x (1/fdcan_ker_ck) */
      hfdcan.Init.NominalSyncJumpWidth = 0x8;
      hfdcan.Init.NominalTimeSeg1 = 0x1F; /* NominalTimeSeg1 = Propagation_segment + Phase_segment_1 */
      hfdcan.Init.NominalTimeSeg2 = 0x8;
      hfdcan.Init.MessageRAMOffset = 0;
      hfdcan.Init.StdFiltersNbr = 1;
      hfdcan.Init.ExtFiltersNbr = 0;
      hfdcan.Init.RxFifo0ElmtsNbr = 1;
      hfdcan.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
      hfdcan.Init.RxFifo1ElmtsNbr = 0;
      hfdcan.Init.RxBuffersNbr = 0;
      hfdcan.Init.TxEventsNbr = 0;
      hfdcan.Init.TxBuffersNbr = 0;
      hfdcan.Init.TxFifoQueueElmtsNbr = 1;
      hfdcan.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
      hfdcan.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
      if (HAL_FDCAN_Init(&hfdcan) != HAL_OK)
      {
        /* Initialization Error */
        Error_Handler();
      }

//#define DO_THE_REST_IN_embot_hw_can
#if !defined(DO_THE_REST_IN_embot_hw_can)
      /* Configure Rx filter */
      sFilterConfig.IdType = FDCAN_STANDARD_ID;
      sFilterConfig.FilterIndex = 0;
      sFilterConfig.FilterType = FDCAN_FILTER_MASK;
      sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
      sFilterConfig.FilterID1 = 0x321;
      sFilterConfig.FilterID2 = 0x7FF;
      if (HAL_FDCAN_ConfigFilter(&hfdcan, &sFilterConfig) != HAL_OK)
      {
        /* Filter configuration Error */
        Error_Handler();
      }

//      /* Start the FDCAN module */
//      if (HAL_FDCAN_Start(&hfdcan) != HAL_OK)
//      {
//        /* Start Error */
//        Error_Handler();
//      }

//      if (HAL_FDCAN_ActivateNotification(&hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
//      {
//        /* Notification Error */
//        Error_Handler();
//      }

//      /* Prepare Tx Header */
//      TxHeader.Identifier = 0x321;
//      TxHeader.IdType = FDCAN_STANDARD_ID;
//      TxHeader.TxFrameType = FDCAN_DATA_FRAME;
//      TxHeader.DataLength = FDCAN_DLC_BYTES_2;
//      TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
//      TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
//      TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
//      TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
//      TxHeader.MessageMarker = 0;
#endif      
      
    }        
    
    constexpr PROP can1p = { .handle = &hfdcan }; 
    //constexpr PROP can2p = { .handle = &hfdcan2 };    
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(CAN::one),        
        // properties
        {{
            &can1p           
        }}        
    };
    
    void BSP::init(embot::hw::CAN h) const 
    {
        if(h == CAN::one)
        {            
            my_FDCAN1_Config();
        }
    }
        
    #else
        #error embot::hw::can::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}} // namespace embot { namespace hw { namespace can {
    

#if defined(HAL_FDCAN_MODULE_ENABLED)

    #if defined(STM32HAL_BOARD_STM32H745DISCO)

        void FDCAN1_IT0_IRQHandler(void)
        {
            HAL_FDCAN_IRQHandler(&embot::hw::can::hfdcan);
        } 
    
    #else
        #error add the handler
    #endif        

#endif //
        
#endif // EMBOT_ENABLE_hw_can


// - support map: end of embot::hw::can



// - support map: begin of embot::hw::flash

#include "embot_hw_flash_bsp.h"

#if !defined(EMBOT_ENABLE_hw_flash)

namespace embot { namespace hw { namespace flash {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::FLASH h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace flash {
     
   #if   defined(STM32HAL_BOARD_STM32H745DISCO)

// acemor          
        // application @ tbdk we have 1024 k of flash. so far we use only 512 k
        constexpr PROP whole                {{0x08000000,               (1024)*1024,        2*1024}}; 
        constexpr PROP bootloader           {{0x08000000,               (126)*1024,         2*1024}};   // bootloader
        constexpr PROP sharedstorage        {{0x08000000+(126*1024),    (2)*1024,           2*1024}};   // sharedstorage: on top of bootloader
        constexpr PROP application          {{0x08000000+(128*1024),    (256+124)*1024,     2*1024}};   // application @ tbdk
        constexpr PROP applicationstorage   {{0x08000000+(508*1024),    (4)*1024,           2*1024}};   // applicationstorage: on top of application            

    #else
        #error embot::hw::flash::thebsp must be defined    
    #endif   


    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(FLASH::whole) | mask::pos2mask<uint32_t>(FLASH::bootloader) | mask::pos2mask<uint32_t>(FLASH::application) |
        mask::pos2mask<uint32_t>(FLASH::sharedstorage) | mask::pos2mask<uint32_t>(FLASH::applicationstorage),        
        // properties
        {{
            &whole, &bootloader, &application, &sharedstorage, &applicationstorage            
        }}        
    };
    
    void BSP::init(embot::hw::FLASH h) const {}
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}} // namespace embot { namespace hw { namespace flash {

#endif // flash

// - support map: end of embot::hw::flash



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

