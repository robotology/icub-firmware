
/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
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

#include "embot_hw_bsp_amc_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------

#if     !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   

    #if   defined(STM32HAL_BOARD_AMC)
    
       
    bool embot::hw::bsp::specialize()
    {
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
 
    #if defined(STM32HAL_BOARD_AMC)
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
    
    #if   defined(STM32HAL_BOARD_AMC)
       
    
    constexpr PROP led1p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::H, embot::hw::GPIO::PIN::thirteen}  };  
    constexpr PROP led2p = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::H, embot::hw::GPIO::PIN::fifteen}  };  
    constexpr PROP led3p = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::H, embot::hw::GPIO::PIN::two}  }; 
    constexpr PROP led4p = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::H, embot::hw::GPIO::PIN::three}  };     
    constexpr PROP led5p = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::H, embot::hw::GPIO::PIN::four}  }; 
    constexpr PROP led6p = { .on = embot::hw::gpio::State::SET, .off = embot::hw::gpio::State::RESET, .gpio = {embot::hw::GPIO::PORT::H, embot::hw::GPIO::PIN::five}  }; 
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(LED::one) | mask::pos2mask<uint32_t>(LED::two) | mask::pos2mask<uint32_t>(LED::three) |
        mask::pos2mask<uint32_t>(LED::four) | mask::pos2mask<uint32_t>(LED::five) | mask::pos2mask<uint32_t>(LED::six),        
        // properties
        {{
            &led1p, &led2p, &led3p, &led4p, &led5p, &led6p, nullptr, nullptr            
        }}        
    };
    
    void clock_enable_H() { __HAL_RCC_GPIOH_CLK_ENABLE(); }
    void BSP::init(embot::hw::LED h) const 
    {
        // activate the clock if cube-mx didn't do that
        //constexpr std::array<embot::core::fpWorker, 6> clockenable { clock_enable_H, clock_enable_H, clock_enable_H, clock_enable_H, clock_enable_H, clock_enable_H};        
        uint8_t i = embot::core::tointegral(h);
        //if(i < clockenable.size())
        //{
        //    clockenable[i]();
        //}
        clock_enable_H(); // no need of the above as every LED is on bus H
        
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
    
#error support for hw_button in embot::hw::bsp is yet to be done

namespace embot { namespace hw { namespace button {
    
    #if   defined(STM32HAL_BOARD_AMC)
        
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
    
#error support for hw_can in embot::hw::bsp is yet to be done

namespace embot { namespace hw { namespace can {
               
    #if   defined(STM32HAL_BOARD_AMC)
    
    
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

    #if defined(STM32HAL_BOARD_AMC)

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
    
#error support for hw_flash in embot::hw::bsp is yet to be done

namespace embot { namespace hw { namespace flash {
     
   #if   defined(STM32HAL_BOARD_AMC)

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


// - support map: begin of embot::hw::eth

//#include "embot_hw_can_eth.h"

#if   !defined(EMBOT_ENABLE_hw_eth)

namespace embot { namespace hw { namespace eth {
    
    
}}}

#else
    
#error support for hw_eth in embot::hw::bsp is yet to be done

namespace embot { namespace hw { namespace eth {
               
    #if   defined(STM32HAL_BOARD_AMC)
    
    // it has HAL_ETH_MODULE_ENABLED  
    // in here we add ....

    #else
        #error embot::hw::eth::thebsp must be defined    
    #endif

              
}}} // namespace embot { namespace hw { namespace eth {
    

    #if defined(HAL_ETH_MODULE_ENABLED)


    #endif //#if defined(HAL_ETH_MODULE_ENABLED)
            
#endif // EMBOT_ENABLE_hw_eth


// - support map: end of embot::hw::eth


// - support map: begin of embot::hw::eeprom

#include "embot_hw_eeprom.h"
#include "embot_hw_eeprom_bsp.h"

#if !defined(EMBOT_ENABLE_hw_eeprom)

namespace embot { namespace hw { namespace eeprom {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::EEPROM h) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace eeprom {
    
    #if defined(STM32HAL_BOARD_AMC)
        
    // ...
    constexpr PROP ee1p = { embot::hw::eeprom::Type::spiM95512DF, 
                            {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::thirteen}, 
                            {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::eight}, 
                            {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::twelve},
                             embot::hw::SPI::six
                          };  
    constexpr PROP ee2p = { embot::hw::eeprom::Type::chipM95512DF, 
                            {   
                                embot::hw::SPI::six,
                                {
                                    {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::eight},    // nS
                                    {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::twelve},   // nW
                                    {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::thirteen}, // nHOLD
                                    {
                                        embot::hw::gpio::Mode::OUTPUTpushpull,
                                        embot::hw::gpio::Pull::nopull,
                                        embot::hw::gpio::Speed::veryhigh
                                    }
                                }
                            }   
                          };  
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(EEPROM::one) | mask::pos2mask<uint32_t>(EEPROM::two),        
        // properties
        {{
            &ee1p, &ee2p            
        }}        
    };
    
    
    void BSP::init(embot::hw::EEPROM h) const {}
    
    #else
        #error embot::hw::bsp::eeprom::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#endif // eeprom

// - support map: end of embot::hw::eeprom


// - support map: begin of embot::hw::spi

#include "embot_hw_spi.h"
#include "embot_hw_spi_bsp.h"

#if !defined(EMBOT_ENABLE_hw_spi)

namespace embot { namespace hw { namespace spi {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::SPI h) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace spi {
    
    #if defined(STM32HAL_BOARD_AMC)
        
    // ...
    constexpr PROP spi6p = { &hspi6, 12500000 };  
    
    static_assert(spi6p.speed == 12.5*1000000, "SPI::six is now 12.5Mhz and must be changed inside MX_SPI6_Init()");
    // SPI::six is used @ 12.Mhz by a M95512-DFMC6 EEPROM and must be < 16MHz
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(SPI::six),        
        // properties
        {{
            nullptr, nullptr, nullptr, nullptr, nullptr, &spi6p            
        }}        
    };
    
    
    bool BSP::init(embot::hw::SPI h) const
    {
        // marco.accame: in here ... MX_SPI6_Ini() calls HAL_SPI_Init() and imposes the speed 
        // and the low level configuration specified inside cube-mx. 
        // it is quick and easy BUT: if we want to attach to the same bus more than one type 
        // of spi sensors (e.g., aea, aea3, AksIM-2, ...) then we must be able to call HAL_SPI_Init()
        // with the parameters we want.
        // conclusion: we shall move HAL_SPI_Init() out of BSP::init() and inside embot::hw::spi::init()
        if(h == SPI::six)
        {            
            MX_SPI6_Init();
            // HAL_SPI_MspInit(&hspi6); // it is called inside HAL_SPI_Init()
            return true;
        } 

        return false;
        // the new rule could be:
        // if we return true .... non extra init is required inside embot::hw::spi
        // else ... we call HAL_SPI_Init() inside embot::hw::spi::init()
        // w/ SPI_InitTypeDef values from embot::hw::spi::Config. we dont need to 
        // have the pins in there as they are initted by HAL_SPI_MspInit.       
    }

    bool BSP::deinit(embot::hw::SPI h) const
    {
        if(h == SPI::six)
        {
            HAL_SPI_DeInit(&hspi6);
            // HAL_SPI_MspDeInit(&hspi6); // // it is called inside HAL_SPI_DeInit()
            return true;
        }  

        return false;        
    }
    
    #else
        #error embot::hw::bsp::spi::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

extern "C"
{
    void SPI6_IRQHandler(void)
    {
        HAL_SPI_IRQHandler(&hspi6);
    }
}

#endif // spi

// - support map: end of embot::hw::spi


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

