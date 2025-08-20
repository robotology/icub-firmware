/*
 * Copyright (C) 2025 iCub Tech - Istituto Italiano di Tecnologia
 * Author:    Marco Accame
 * email:     marco.accame@iit.it
 * Coauthor:  Kevin Sangalli
 * email:     kevin.sangalli@iit.it
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


#include "embot_hw_spi.h"
#include "embot_hw_spi_bsp.h"
#include "embot_hw_spi_bsp_amc.h"

#if !defined(EMBOT_ENABLE_hw_spi)

namespace embot::hw::spi::bsp {
    
    constexpr BSP thebsp { };
    bool BSP::init(embot::hw::SPI h, const Config &config) const { return true;}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#else

namespace embot::hw::spi::bsp {
    
    #if defined(STM32HAL_BOARD_AMC)
    
    SPI_Device* getDEVICE(embot::hw::SPI h)
    {
        static SPI_Device * spiDEVICE[6] = {SPI1, SPI2, SPI3, SPI4, SPI5, SPI6};
        return embot::hw::spi::supported(h) ? spiDEVICE[embot::core::tointegral(h)] : nullptr;        
    }
    
    
    SPI_HandleTypeDef hspi1;
    constexpr std::array<embot::hw::GPIO, SignalsNumberOf> pinoutspi1 = { {
        {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::nine},     // miso
        {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::seven},    // mosi
        {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::eleven},   // sckl
        {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::ten}       // ssel
    } };
    constexpr PROP spi1p = { &hspi1, 100*1000*1000, pinoutspi1 }; 
    
    
    SPI_HandleTypeDef hspi2;
    constexpr std::array<embot::hw::GPIO, SignalsNumberOf> pinoutspi2 = { {
        {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::fourteen}, // miso
        {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::fifteen},  // mosi
        {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::three},    // sckl
        {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::nine}      // ssel
    } };
    constexpr PROP spi2p = { &hspi2, 100*1000*1000, pinoutspi2 };     
    
    SPI_HandleTypeDef hspi3;
    constexpr std::array<embot::hw::GPIO, SignalsNumberOf> pinoutspi3 = { {
        {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::eleven},   // miso
        {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::six},      // mosi
        {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::ten},      // sckl
        {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::four}      // ssel
    } };
    constexpr PROP spi3p = { &hspi3, 100*1000*1000, pinoutspi3 };       
    

    constexpr std::array<embot::hw::GPIO, SignalsNumberOf> pinoutspi5 = { {
        {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::eight},    // miso
        {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::eleven},   // mosi
        {embot::hw::GPIO::PORT::H, embot::hw::GPIO::PIN::six},      // sckl
        {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none}   // ssel
    } };
    constexpr PROP spi5p = { &hspi5, 100*1000*1000, pinoutspi5 };   
    
    constexpr std::array<embot::hw::GPIO, SignalsNumberOf> pinoutspi6 = { {
        {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::four},     // miso
        {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::fourteen}, // mosi
        {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::thirteen}, // sckl
        {embot::hw::GPIO::PORT::none, embot::hw::GPIO::PIN::none}   // ssel
    } };
    constexpr PROP spi6p = { &hspi6, 100*1000*1000, pinoutspi6 };     
    
    static_assert(spi6p.clockrate == 100*1000*1000, "SPI::six is now 12.5Mhz and must be changed inside MX_SPI6_Init()");
    // SPI::six is used @ 12.Mhz by a M95512-DFMC6 EEPROM and must be < 16MHz
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(SPI::one) | mask::pos2mask<uint32_t>(SPI::two) | mask::pos2mask<uint32_t>(SPI::three) |
        mask::pos2mask<uint32_t>(SPI::five) | mask::pos2mask<uint32_t>(SPI::six),        
        // properties
        {{
            &spi1p, &spi2p, &spi3p, nullptr, &spi5p, &spi6p            
        }}        
    };
 
 
    void s_J5_SPIpinout(embot::hw::SPI h, bool enable)
    {
        static constexpr embot::hw::gpio::Config out { embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::medium };    
        static constexpr embot::hw::gpio::State stateSPI[2] = {embot::hw::gpio::State::RESET, embot::hw::gpio::State::SET};
        static constexpr embot::hw::gpio::State stateNONE[2] = {embot::hw::gpio::State::RESET, embot::hw::gpio::State::RESET};
    
        static constexpr size_t spinum {embot::core::tointegral(embot::hw::SPI::three)+1};
        static constexpr embot::hw::GPIO X1ENspi[spinum][2] = 
        {
            {   // spi1
                {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::zero},   // i2c1 ENABLE
                {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::eight}   // spi1 ENABLE
            },
            {   // spi2
                {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::one},   // i2c2 ENABLE
                {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::nine}   // spi2 ENABLE
            },
            {   // spi3
                {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::two},   // i2c3 ENABLE
                {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::ten}    // spi3 ENABLE 
            }
        };
        
        uint8_t x = embot::core::tointegral(h);
        if(x <= spinum)
        {
            // spix
            for(uint8_t i=0; i<2; i++)
            {
                embot::hw::gpio::init(X1ENspi[x][i], out);
                embot::hw::gpio::set(X1ENspi[x][i], enable ? stateSPI[i] : stateNONE[i]);
            }  
        }
    }  

    // we need this extconfig because we wamt to pass information to HAL_SPI_MspInit() and HAL_SPI_MspDeInit()
    utils::ExtendedConfig extconfig {};
        
    void s_SPIinit(embot::hw::SPI h, const Config &config)
    {
        embot::hw::spi::bsp::SPI_Handle * hspi = embot::hw::spi::bsp::getBSP().getPROP(h)->handle;
        
        // prepare and then call HAL_SPI_Init()
        hspi->Instance = getDEVICE(h);
        hspi->Init.Mode = SPI_MODE_MASTER;
        hspi->Init.Direction = SPI_DIRECTION_2LINES;
        hspi->Init.DataSize = embot::hw::spi::bsp::utils::stm32::todatasize(config.datasize);;
        hspi->Init.CLKPolarity = embot::hw::spi::bsp::utils::stm32::toCLKpolarity(config.mode);
        hspi->Init.CLKPhase = embot::hw::spi::bsp::utils::stm32::toCLKphase(config.mode);
        hspi->Init.NSS = SPI_NSS_SOFT;
        hspi->Init.BaudRatePrescaler = embot::hw::spi::bsp::utils::stm32::tobaudrateprescaler(config.prescaler);
        hspi->Init.FirstBit = SPI_FIRSTBIT_MSB;
        hspi->Init.TIMode = SPI_TIMODE_DISABLE;
        hspi->Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
        hspi->Init.CRCPolynomial = 0x0;
        hspi->Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
        hspi->Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
        hspi->Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
        hspi->Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
        hspi->Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
        hspi->Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
        hspi->Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
        hspi->Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
        hspi->Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
        hspi->Init.IOSwap = SPI_IO_SWAP_DISABLE;
        
        // load config and pinout into extconfig so that whatever HAL_SPI_Init() calls can use it
        extconfig.load(h, config);
        HAL_SPI_Init(hspi);         
        // clear extconfig
        extconfig.clear();
    }  
    

    void s_SPIdeinit(embot::hw::SPI h)
    {
        embot::hw::spi::bsp::SPI_Handle * hspi = embot::hw::spi::bsp::getBSP().getPROP(h)->handle;    
        
        // load pinout into extconfig so that whatever HAL_SPI_DeInit() calls can use it
        extconfig.load(h, {});
        HAL_SPI_DeInit(hspi); 
        // clear extconfig
        extconfig.clear();            
    }      
    
    
    bool BSP::init(embot::hw::SPI h, const Config &config) const
    {        
        switch(h)
        {
            case SPI::one:
            case SPI::two:
            case SPI::three:
            {
                // we are on J5: enable the port x1
                s_J5_SPIpinout(h, true); 
                // and call SPI init              
                s_SPIinit(h, config);                
            } break;
            
            case SPI::four:
            {
                // nothing to do
            } break;
            
            case SPI::five:
            {
                // ETH ... use what cube-mx has chosen
                MX_SPI5_Init();
            } break;
            
            case SPI::six:
            {
                // eeprom: just call SPI init
                s_SPIinit(h, config);
            } break;
            
            default: {} break;                            
        }
        
        return true; 
    }

    bool BSP::deinit(embot::hw::SPI h) const
    {       
        switch(h)
        {
            case SPI::one:
            case SPI::two:
            case SPI::three:
            {
                // call SPI deinit              
                s_SPIdeinit(h);  
                // we are on J5: disable port x1
                s_J5_SPIpinout(h, false);                
            } break;
            
            case SPI::four:
            {
                // nothing to do
            } break;
            
            case SPI::five:
            {
                // ETH ... use what cube-mx has chosen
                HAL_SPI_DeInit(&hspi5);
            } break;
            
            case SPI::six:
            {
                // eeprom: just call SPI deinit
                s_SPIdeinit(h);
            } break;
            
            default: {} break;                            
        }        
        
        return true;        
    }
    
    #else
        #error embot::hw::bsp::spi::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

extern "C"
{
    void HAL_SPI_MspInit(SPI_HandleTypeDef* spiHandle)
    {
       embot::hw::spi::bsp::utils::ExtendedConfig * extcfg = nullptr;
       extcfg = &embot::hw::spi::bsp::extconfig;   

      GPIO_InitTypeDef GPIO_InitStruct = {0};
      RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
      
      if(spiHandle->Instance == SPI1)
      {
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI1;
        PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
          Error_Handler();
        }

        /* SPI1 clock enable */
        __HAL_RCC_SPI1_CLK_ENABLE();

        // must prepare the clocks of the sckl, mosi, miso, (ssel ?): D and G in our case
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        
        /**SPI6 GPIO Configuration
        PG9      ------> SPI1_MISO
        PG11     ------> SPI1_SCK
        PD7     ------>  SPI1_MOSI
        */
        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::MISO, 0);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::MISO, GPIO_PULLUP);
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::MISO, nullptr), &GPIO_InitStruct);

        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::MOSI, 0);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::MOSI, GPIO_PULLUP);
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::MOSI, nullptr), &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::SCLK, 0); 
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::SCLK, GPIO_PULLUP);;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::SCLK, nullptr), &GPIO_InitStruct);        

        /* SPI1 interrupt Init */
        HAL_NVIC_SetPriority(SPI1_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI1_IRQn);          
      }
      else if(spiHandle->Instance == SPI2)
      {
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI2;
        PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
          Error_Handler();
        }

        /* SPI2 clock enable */
        __HAL_RCC_SPI2_CLK_ENABLE();

        // must prepare the clocks of the sckl, mosi, miso, (ssel ?): B and D in our case
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        
        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::MISO, 0);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::MISO, GPIO_NOPULL);
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::MISO, nullptr), &GPIO_InitStruct);

        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::MOSI, 0);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::MOSI, GPIO_NOPULL);
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::MOSI, nullptr), &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::SCLK, 0); 
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::SCLK, GPIO_NOPULL);;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::SCLK, nullptr), &GPIO_InitStruct);        

        /* SPI2 interrupt Init */
        HAL_NVIC_SetPriority(SPI2_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI2_IRQn);          
      }  
      else if(spiHandle->Instance == SPI3)
      {
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI3;
        PeriphClkInitStruct.Spi123ClockSelection = RCC_SPI123CLKSOURCE_PLL;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
          Error_Handler();
        }

        /* SPI3 clock enable */
        __HAL_RCC_SPI3_CLK_ENABLE();

        // must prepare the clocks of the sckl, mosi, miso, (ssel ?): C and D in our case
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        
        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::MISO, 0);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::MISO, GPIO_NOPULL);
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::MISO, nullptr), &GPIO_InitStruct);

        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::MOSI, 0);
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::MOSI, GPIO_NOPULL);
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI3;
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::MOSI, nullptr), &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::SCLK, 0); 
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::SCLK, GPIO_NOPULL);;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF6_SPI3;
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::SCLK, nullptr), &GPIO_InitStruct);        

        /* SPI3 interrupt Init */
        HAL_NVIC_SetPriority(SPI3_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI3_IRQn);          
      }        
      else if(spiHandle->Instance==SPI5)
      {
      /* USER CODE BEGIN SPI5_MspInit 0 */

      /* USER CODE END SPI5_MspInit 0 */
      /** Initializes the peripherals clock
      */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI5;
        PeriphClkInitStruct.Spi45ClockSelection = RCC_SPI45CLKSOURCE_D2PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
          Error_Handler();
        }

        /* SPI5 clock enable */
        __HAL_RCC_SPI5_CLK_ENABLE();

        __HAL_RCC_GPIOF_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();
        /**SPI5 GPIO Configuration
        PF8     ------> SPI5_MISO
        PF11     ------> SPI5_MOSI
        PH6     ------> SPI5_SCK
        */
        GPIO_InitStruct.Pin = ETH_MISO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLDOWN;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
        HAL_GPIO_Init(ETH_MISO_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = ETH_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
        HAL_GPIO_Init(ETH_MOSI_GPIO_Port, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = ETH_SCLK_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI5;
        HAL_GPIO_Init(ETH_SCLK_GPIO_Port, &GPIO_InitStruct);

        /* SPI5 interrupt Init */
        HAL_NVIC_SetPriority(SPI5_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI5_IRQn);
      /* USER CODE BEGIN SPI5_MspInit 1 */

      /* USER CODE END SPI5_MspInit 1 */
      }
      else if(spiHandle->Instance==SPI6)
      {
      /* USER CODE BEGIN SPI6_MspInit 0 */

      /* USER CODE END SPI6_MspInit 0 */

      /** Initializes the peripherals clock
      */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_SPI6;
        PeriphClkInitStruct.Spi6ClockSelection = RCC_SPI6CLKSOURCE_D3PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
          Error_Handler();
        }

        /* SPI6 clock enable */
        __HAL_RCC_SPI6_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        /**SPI6 GPIO Configuration
        PB4 (NJTRST)     ------> SPI6_MISO
        PG13     ------> SPI6_SCK
        PG14     ------> SPI6_MOSI
        */
        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::MISO, EE_MISO_Pin); //EE_MISO_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::MISO, GPIO_NOPULL);
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF8_SPI6;
        //HAL_GPIO_Init(EE_MISO_GPIO_Port, &GPIO_InitStruct);
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::MISO, EE_MISO_GPIO_Port), &GPIO_InitStruct);

        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::MOSI, EE_MOSI_Pin);//EE_MOSI_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::MOSI, GPIO_NOPULL);
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI6;
        //HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::MOSI, GPIOG), &GPIO_InitStruct);
        
        GPIO_InitStruct.Pin = extcfg->pin(embot::hw::spi::Signal::SCLK, EE_SCLK_Pin); //EE_SCLK_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = extcfg->pull(embot::hw::spi::Signal::SCLK, GPIO_NOPULL);;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI6;
        //HAL_GPIO_Init(GPIOG, &GPIO_InitStruct); 
        HAL_GPIO_Init(extcfg->port(embot::hw::spi::Signal::SCLK, GPIOG), &GPIO_InitStruct);        

        /* SPI6 interrupt Init */
        HAL_NVIC_SetPriority(SPI6_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(SPI6_IRQn);
      /* USER CODE BEGIN SPI6_MspInit 1 */

      /* USER CODE END SPI6_MspInit 1 */
      }
    }    


    void HAL_SPI_MspDeInit(SPI_HandleTypeDef* spiHandle)
    {
       embot::hw::spi::bsp::utils::ExtendedConfig * extcfg = nullptr;
       extcfg = &embot::hw::spi::bsp::extconfig;  
        
        if(spiHandle->Instance == SPI1)
        {
            __HAL_RCC_SPI1_CLK_DISABLE();
            
            embot::hw::gpio::deinit(extcfg->gpio(embot::hw::spi::Signal::MISO));
            embot::hw::gpio::deinit(extcfg->gpio(embot::hw::spi::Signal::MOSI));
            embot::hw::gpio::deinit(extcfg->gpio(embot::hw::spi::Signal::SCLK));

            HAL_NVIC_DisableIRQ(SPI1_IRQn);            
        }
        else if(spiHandle->Instance == SPI2)
        {
            __HAL_RCC_SPI2_CLK_DISABLE();
            
            embot::hw::gpio::deinit(extcfg->gpio(embot::hw::spi::Signal::MISO));
            embot::hw::gpio::deinit(extcfg->gpio(embot::hw::spi::Signal::MOSI));
            embot::hw::gpio::deinit(extcfg->gpio(embot::hw::spi::Signal::SCLK));

            HAL_NVIC_DisableIRQ(SPI2_IRQn);            
        }
        else if(spiHandle->Instance == SPI3)
        {
            __HAL_RCC_SPI3_CLK_DISABLE();
            
            embot::hw::gpio::deinit(extcfg->gpio(embot::hw::spi::Signal::MISO));
            embot::hw::gpio::deinit(extcfg->gpio(embot::hw::spi::Signal::MOSI));
            embot::hw::gpio::deinit(extcfg->gpio(embot::hw::spi::Signal::SCLK));

            HAL_NVIC_DisableIRQ(SPI3_IRQn);            
        }        
      else if(spiHandle->Instance==SPI5)
      {
      /* USER CODE BEGIN SPI5_MspDeInit 0 */

      /* USER CODE END SPI5_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SPI5_CLK_DISABLE();

        /**SPI5 GPIO Configuration
        PF8     ------> SPI5_MISO
        PF11     ------> SPI5_MOSI
        PH6     ------> SPI5_SCK
        */
        HAL_GPIO_DeInit(GPIOF, ETH_MISO_Pin|ETH_MOSI_Pin);

        HAL_GPIO_DeInit(ETH_SCLK_GPIO_Port, ETH_SCLK_Pin);

        /* SPI5 interrupt Deinit */
        HAL_NVIC_DisableIRQ(SPI5_IRQn);
      /* USER CODE BEGIN SPI5_MspDeInit 1 */

      /* USER CODE END SPI5_MspDeInit 1 */
      }
      else if(spiHandle->Instance==SPI6)
      {
      /* USER CODE BEGIN SPI6_MspDeInit 0 */

      /* USER CODE END SPI6_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_SPI6_CLK_DISABLE();

        /**SPI6 GPIO Configuration
        PB4 (NJTRST)     ------> SPI6_MISO
        PG13     ------> SPI6_SCK
        PG14     ------> SPI6_MOSI
        */
        //HAL_GPIO_DeInit(EE_MISO_GPIO_Port, EE_MISO_Pin);
        //HAL_GPIO_DeInit(GPIOG, EE_MOSI_Pin);
        //HAL_GPIO_DeInit(GPIOG, EE_SCLK_Pin);
          
        HAL_GPIO_DeInit(    extcfg->port(embot::hw::spi::Signal::MISO, EE_MISO_GPIO_Port), 
                            extcfg->pin(embot::hw::spi::Signal::MISO, EE_MISO_Pin));
        HAL_GPIO_DeInit(    extcfg->port(embot::hw::spi::Signal::MOSI, GPIOG), 
                            extcfg->pin(embot::hw::spi::Signal::MOSI, EE_MOSI_Pin));
        HAL_GPIO_DeInit(    extcfg->port(embot::hw::spi::Signal::SCLK, GPIOG),
                            extcfg->pin(embot::hw::spi::Signal::SCLK, EE_SCLK_Pin)); ///EE_SCLK_Pin);

        //HAL_GPIO_DeInit(GPIOG, EE_SCLK_Pin|EE_MOSI_Pin);

        /* SPI6 interrupt Deinit */
        HAL_NVIC_DisableIRQ(SPI6_IRQn);
      /* USER CODE BEGIN SPI6_MspDeInit 1 */

      /* USER CODE END SPI6_MspDeInit 1 */
      }
    }
    
}

extern "C"
{
    void SPI1_IRQHandler(void)
    {
        HAL_SPI_IRQHandler(&embot::hw::spi::bsp::hspi1);
    }

    void SPI2_IRQHandler(void)
    {
        HAL_SPI_IRQHandler(&embot::hw::spi::bsp::hspi2);
    }
 
    void SPI3_IRQHandler(void)
    {
        HAL_SPI_IRQHandler(&embot::hw::spi::bsp::hspi3);
    }

    void SPI5_IRQHandler(void)
    {
        HAL_SPI_IRQHandler(&hspi5);
    }
    
    void SPI6_IRQHandler(void)
    {
        HAL_SPI_IRQHandler(&hspi6);
    }
}

#endif // spi


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


