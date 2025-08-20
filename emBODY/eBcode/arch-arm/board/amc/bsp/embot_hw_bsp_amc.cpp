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




// - support map: begin of embot::hw::testpoint

#include "embot_hw_testpoint_bsp.h"

#if !defined(EMBOT_ENABLE_hw_testpoint)

namespace embot { namespace hw { namespace testpoint {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::TESTPOINT h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace testpoint {
    
    #if   defined(STM32HAL_BOARD_AMC)

    
    constexpr PROP tp1 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::five}  };
    constexpr PROP tp2 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::A, embot::hw::GPIO::PIN::eight}  };
    constexpr PROP tp3 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::zero}  };
    constexpr PROP tp4 = { .on = embot::hw::gpio::State::RESET, .off = embot::hw::gpio::State::SET, .gpio = {embot::hw::GPIO::PORT::C, embot::hw::GPIO::PIN::thirteen}}; 
    
    constexpr BSP thebsp {
        // maskofsupported
        mask::pos2mask<uint32_t>(TESTPOINT::one) | mask::pos2mask<uint32_t>(TESTPOINT::two) | 
        mask::pos2mask<uint32_t>(TESTPOINT::three) | mask::pos2mask<uint32_t>(TESTPOINT::four) 
        ,
        // properties
        {{
            &tp1, &tp2, &tp3, &tp4
        }}
    };
    
    void clock_enable_A() { __HAL_RCC_GPIOA_CLK_ENABLE(); }
    void clock_enable_C() { __HAL_RCC_GPIOC_CLK_ENABLE(); }
    void BSP::init(embot::hw::TESTPOINT h) const 
    {
        // activate the clock if cube-mx didn't do that
        uint8_t i = embot::core::tointegral(h);

        clock_enable_A();
        clock_enable_C();
        
        // init the gpio
        const embot::hw::GPIO &g = thebsp.properties[i]->gpio;
        embot::hw::gpio::configure(g, embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::high);
    } 

    #else
        #error embot::hw::testpoint::thebsp must be defined
    #endif
    
    const BSP& getBSP()
    {
        return thebsp;
    }

}}} // namespace embot { namespace hw {  namespace testpoint {

#endif // testpoint

// - support map: end of embot::hw::testpoint






// - support map: begin of embot::hw::flash

#include "embot_hw_flash_bsp.h"

#if !defined(EMBOT_ENABLE_hw_flash)

namespace embot { namespace hw { namespace flash {
    
    constexpr BSP thebsp { };
    void BSP::init() const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else
    
namespace embot { namespace hw { namespace flash { namespace bsp {
     
#if   defined(STM32HAL_BOARD_AMC)
    
    constexpr uint8_t numbanks {2};
    constexpr uint32_t banksize {1024*1024};
    constexpr uint32_t pagesize {128*1024};
    constexpr BankDescriptor bank01 { Bank::ID::one, 0x08000000, banksize, pagesize };
    constexpr BankDescriptor bank02 { Bank::ID::two, 0x08100000, banksize, pagesize };
    constexpr theBanks thebanks 
    {
        numbanks, 
        { &bank01, &bank02 }
    }; 
    
    // on Bank::one
    constexpr Partition ldr {Partition::ID::eloader,        &bank01,    bank01.address,         128*1024}; 
    constexpr Partition upd {Partition::ID::eupdater,       &bank01,    ldr.address+ldr.size,   256*1024};
    constexpr Partition a00 {Partition::ID::eapplication00, &bank01,    upd.address+upd.size,   512*1024};  
    constexpr Partition b00 {Partition::ID::buffer00,       &bank01,    a00.address+a00.size,   128*1024};
    
    // on Bank::two
    constexpr Partition a01 {Partition::ID::eapplication01, &bank02,    bank02.address,         512*1024};     
    constexpr Partition b01 {Partition::ID::buffer01,       &bank02,    a01.address+a01.size,   512*1024};
    
    constexpr thePartitions thepartitions
    {
        { &ldr, &upd, &a00, &b00, &a01, &b01 }
    };

    constexpr BSP thebsp {        
        thebanks,
        thepartitions
    };   
            
#else
    #error embot::hw::flash::thebsp must be defined    
#endif   
     
    
    void BSP::init() const {}
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
              
}}}} // namespace embot { namespace hw { namespace flash { namespace bsp {

#endif // flash

// - support map: end of embot::hw::flash


// - support map: begin of embot::hw::eeprom

#include "embot_hw_eeprom.h"
#include "embot_hw_eeprom_bsp.h"

#if !defined(EMBOT_ENABLE_hw_eeprom)

namespace embot { namespace hw { namespace eeprom {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::EEPROM h) const {}
    void BSP::deinit(embot::hw::EEPROM h) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

namespace embot { namespace hw { namespace eeprom {
    
    #if defined(STM32HAL_BOARD_AMC)
        
    constexpr PROP ee1p = { embot::hw::eeprom::Type::chipM95512DF, 
                            {   
                                embot::hw::SPI::six, 
                                { 
                                    embot::hw::spi::Prescaler::eight, 
                                    embot::hw::spi::DataSize::eight, 
                                    embot::hw::spi::Mode::zero,
                                    { {embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Pull::none} }
                                },
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
        mask::pos2mask<uint32_t>(EEPROM::one),        
        // properties
        {{
            &ee1p, nullptr            
        }}        
    };
    
    
    void BSP::init(embot::hw::EEPROM h) const {}
    void BSP::deinit(embot::hw::EEPROM h) const {}    
    
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


// - support map: begin of embot::hw::encoder

#include "embot_hw_encoder.h"
#include "embot_hw_encoder_bsp.h"

#if !defined(EMBOT_ENABLE_hw_encoder)

namespace embot { namespace hw { namespace encoder {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::ENCODER h) const {}
    void BSP::deinit(embot::hw::ENCODER h) const {}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#else

#include "embot_hw_chip_AS5045.h"
#include "embot_hw_chip_MA730.h"
#include "embot_hw_chip_MB049.h"

namespace embot::hw::encoder::bsp{
    
    #if defined(STM32HAL_BOARD_AMC)
    
    
    constexpr embot::hw::spi::Config spiCFGchipMA730  {embot::hw::chip::MA730::standardspiconfig};
    constexpr embot::hw::spi::Config spiCFGchipAS5045 {embot::hw::chip::AS5045::standardspiconfig};
    constexpr embot::hw::spi::Config spiCFGchipMB049  {embot::hw::chip::MB049::standardspiconfig};
        
    
    // encoder one --> SPI1
    constexpr PROP e1p = { embot::hw::SPI::one, { spiCFGchipAS5045, spiCFGchipMA730, spiCFGchipMB049 } };

    // encoder one --> SPI2
    constexpr PROP e2p = { embot::hw::SPI::two, { spiCFGchipAS5045, spiCFGchipMA730, spiCFGchipMB049 } };
    
    // encoder one --> SPI3
    constexpr PROP e3p = { embot::hw::SPI::three, { spiCFGchipAS5045, spiCFGchipMA730, spiCFGchipMB049 } };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(ENCODER::one) | mask::pos2mask<uint32_t>(ENCODER::two) | mask::pos2mask<uint32_t>(ENCODER::three),
        // properties
        {{ &e1p, &e2p, &e3p }}
    };
    
    
    void BSP::init(embot::hw::ENCODER h) const {}
    void BSP::deinit(embot::hw::ENCODER h) const {}    
    
    #else
        #error embot::hw::bsp::encoder::thebsp must be defined    
    #endif
    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}

#endif // encoder

// - support map: end of embot::hw::encoder


// - support map: begin of embot::hw::spi

#include "embot_hw_spi.h"
#include "embot_hw_spi_bsp.h"

#if !defined(EMBOT_ENABLE_hw_spi)

namespace embot { namespace hw { namespace spi { namespace bsp {
    
    constexpr BSP thebsp { };
    bool BSP::init(embot::hw::SPI h, const Config &config) const { return true;}
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}}

#else

namespace embot { namespace hw { namespace spi { namespace bsp {
    
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

    // we need this extconfig because we want to pass information to HAL_SPI_MspInit() and HAL_SPI_MspDeInit()
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
    
}}}}

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

// - support map: end of embot::hw::spi

// - support map: begin of embot::hw::i2c

#include "embot_hw_i2c.h"
#include "embot_hw_i2c_bsp.h"

#if !defined(EMBOT_ENABLE_hw_i2c)

namespace embot::hw::i2c 
{
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::I2C h) const { ;}
    const BSP& getBSP() 
    {
        return thebsp;
    }
}

#else

namespace embot::hw::i2c {

    #if defined(STM32HAL_BOARD_AMC)
    
    DMA_HandleTypeDef hdma_i2c1_tx;
    DMA_HandleTypeDef hdma_i2c1_rx;
    DMA_HandleTypeDef hdma_i2c2_rx;
    DMA_HandleTypeDef hdma_i2c2_tx;
    DMA_HandleTypeDef hdma_i2c3_rx;
    DMA_HandleTypeDef hdma_i2c3_tx;  
    
    I2C_HandleTypeDef hi2c1;
    constexpr PROP i2c1p = { &hi2c1, embot::hw::i2c::Speed::standard100 }; 
    
    I2C_HandleTypeDef hi2c2;
    constexpr PROP i2c2p = { &hi2c2, embot::hw::i2c::Speed::standard100 }; 
     
    I2C_HandleTypeDef hi2c3;
    constexpr PROP i2c3p = { &hi2c3, embot::hw::i2c::Speed::standard100 };     
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(I2C::one) | mask::pos2mask<uint32_t>(I2C::two) | mask::pos2mask<uint32_t>(I2C::three),
        // properties
        {{
            &i2c1p, &i2c2p, &i2c3p, nullptr            
        }}        
    };
    
    
    void s_J5_I2Cpinout(embot::hw::I2C h, bool enable)
    {
        static constexpr embot::hw::gpio::Config out { embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::medium };    
        static constexpr embot::hw::gpio::State stateI2C[2] = {embot::hw::gpio::State::SET, embot::hw::gpio::State::RESET};
        static constexpr embot::hw::gpio::State stateNONE[2] = {embot::hw::gpio::State::RESET, embot::hw::gpio::State::RESET};
    
        static constexpr size_t i2cnum {embot::core::tointegral(embot::hw::I2C::three)+1};
        static constexpr embot::hw::GPIO X1ENspi[i2cnum][2] = 
        {
            {   // i2c1
                {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::zero},   // i2c1 ENABLE
                {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::eight}   // spi1 ENABLE
            },
            {   // i2c2
                {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::one},   // i2c2 ENABLE
                {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::nine}   // spi2 ENABLE
            },
            {   // i2c3
                {embot::hw::GPIO::PORT::G, embot::hw::GPIO::PIN::two},   // i2c3 ENABLE
                {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::ten}    // spi3 ENABLE 
            }
        };
        
        uint8_t x = embot::core::tointegral(h);
        if(x <= i2cnum)
        {
            // i2cx
            for(uint8_t i=0; i<2; i++)
            {
                embot::hw::gpio::init(X1ENspi[x][i], out);
                embot::hw::gpio::set(X1ENspi[x][i], enable ? stateI2C[i] : stateNONE[i]);
            }  
        }
    }  
    
    // we need this extconfig because we want to pass information to HAL_I2C_MspInit() and HAL_I2C_MspDeInit()
//    utils::ExtendedConfig extconfig {};
        
    void s_I2Cinit(embot::hw::I2C h)
    {   
        // I2C_specific handle --> this is i2c* handle 
        embot::hw::i2c::I2C_Handle * hi2c = embot::hw::i2c::getBSP().getPROP(h)->handle;
        switch(h)
        {
            case I2C::one:
            {
                hi2c->Instance = I2C1;
            } break;
            
            case I2C::two:
            {
                hi2c->Instance = I2C2;
            } break;
            case I2C::three:
            {
                hi2c->Instance = I2C3;
            } break;
            case I2C::four:
            {
                // nothing to do
            } break;
            
            default: {} break;    
        }
        
        // prepare and then call HAL_SPI_Init()
        // These specs are equal for all hi2c, thus we do not need to add them to the switch case
        hi2c->Init.Timing = 0x10C0ECFF;
        hi2c->Init.OwnAddress1 = 0;
        hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
        hi2c->Init.OwnAddress2 = 0;
        hi2c->Init.OwnAddress2Masks = I2C_OA2_NOMASK;
        hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
        hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
        if (HAL_I2C_Init(hi2c) != HAL_OK)
        {
            Error_Handler();
        }
        
        // TODO: japo. Check if we really need filters configuration or we can remove these lines
        /** Configure Analogue filter
        */
        if (HAL_I2CEx_ConfigAnalogFilter(hi2c, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
        {
            Error_Handler();
        }
        /** Configure Digital filter
        */
        if (HAL_I2CEx_ConfigDigitalFilter(hi2c, 0) != HAL_OK)
        {
            Error_Handler();
        }     
        
        // clear extconfig
//        extconfig.clear(); 
    }  
    

    void s_I2Cdeinit(embot::hw::I2C h)
    {
        embot::hw::i2c::I2C_Handle * hi2c = embot::hw::i2c::getBSP().getPROP(h)->handle;    
        
        HAL_I2C_DeInit(hi2c); 
        
//        // clear extconfig
//        extconfig.clear();            
    }  

    void i2c_MX_DMA_Init(void)
    {

      static bool called {false};
      
      if(true == called)
      {
          return;
      }

      called = true;
      
      /* DMA controller clock enable */
      __HAL_RCC_DMA2_CLK_ENABLE();
      __HAL_RCC_DMA1_CLK_ENABLE();

      /* DMA interrupt init */
      /* DMA1_Stream0_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream0_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream0_IRQn);
      /* DMA1_Stream1_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
      /* DMA1_Stream2_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream2_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream2_IRQn);
      /* DMA1_Stream3_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream3_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
      /* DMA1_Stream5_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
      /* DMA1_Stream6_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);
      /* DMA2_Stream0_IRQn interrupt configuration */
      HAL_NVIC_SetPriority(DMA2_Stream0_IRQn, 5, 0);
      HAL_NVIC_EnableIRQ(DMA2_Stream0_IRQn);

    }    
    
    
    void BSP::init(embot::hw::I2C h) const
    {        
        switch(h)
        {
            case I2C::one:
            case I2C::two:
            case I2C::three:
            {
                // we are on J5: enable the port x1
                s_J5_I2Cpinout(h, true); 
                i2c_MX_DMA_Init();
                // and call SPI init              
                s_I2Cinit(h);                
            } break;
            
            case I2C::four:
            {
                // nothing to do
            } break;
            
            default: {} break;                            
        }
    }

    bool BSP::deinit(embot::hw::I2C h) const
    {       
        switch(h)
        {
            case I2C::one:
            case I2C::two:
            case I2C::three:
            {
                // call SPI deinit              
                s_I2Cdeinit(h);  
                // we are on J5: disable port x1
                s_J5_I2Cpinout(h, false);                
            } break;
            
            case I2C::four:
            {
                // nothing to do
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
    
// I2C INIT Define
#define TIMING_CLEAR_MASK   (0xF0FFFFFFU)  /*!< I2C TIMING clear register Mask */
#define I2C_STATE_NONE            ((uint32_t)(HAL_I2C_MODE_NONE)) 

// SDA-SCL I2C1 PINs
#define I2C1_SDA_Pin GPIO_PIN_7
#define I2C1_SDA_GPIO_Port GPIOB
#define I2C1_SCL_Pin GPIO_PIN_8
#define I2C1_SCL_GPIO_Port GPIOB
#define I2C1_XEN_Pin GPIO_PIN_0
#define I2C1_XEN_GPIO_Port GPIOG

// SDA-SCL I2C2 PINs
#define I2C2_SDA_Pin GPIO_PIN_0
#define I2C2_SDA_GPIO_Port GPIOF
#define I2C2_SCL_Pin GPIO_PIN_1
#define I2C2_SCL_GPIO_Port GPIOF
#define I2C2_XEN_Pin GPIO_PIN_1
#define I2C2_XEN_GPIO_Port GPIOG

// SDA-SCL I2C3 PINs
#define I2C3_SDA_Pin GPIO_PIN_8
#define I2C3_SDA_GPIO_Port GPIOH
#define I2C3_SCL_Pin GPIO_PIN_7
#define I2C3_SCL_GPIO_Port GPIOH
#define I2C3_XEN_Pin GPIO_PIN_2
#define I2C3_XEN_GPIO_Port GPIOG    
    

    
    void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c)
    {
        GPIO_InitTypeDef GPIO_InitStruct = {0};
        RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};
        if(hi2c->Instance==I2C1)
        {
            /* USER CODE BEGIN I2C1_MspInit 0 */

            /* USER CODE END I2C1_MspInit 0 */
            /** Initializes the peripherals clock
            */
            PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
            PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
            if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
            {
              Error_Handler();
            }

            __HAL_RCC_GPIOB_CLK_ENABLE();
            /**I2C1 GPIO Configuration
            PB8     ------> I2C1_SCL
            PB7     ------> I2C1_SDA
            */
            GPIO_InitStruct.Pin = I2C1_SCL_Pin|I2C1_SDA_Pin;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
            GPIO_InitStruct.Pull = GPIO_PULLUP;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

            /* Peripheral clock enable */
            __HAL_RCC_I2C1_CLK_ENABLE();

            /* I2C1 DMA Init */
            /* I2C1_TX Init */
            embot::hw::i2c::hdma_i2c1_tx.Instance = DMA1_Stream0;
            embot::hw::i2c::hdma_i2c1_tx.Init.Request = DMA_REQUEST_I2C1_TX;
            embot::hw::i2c::hdma_i2c1_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
            embot::hw::i2c::hdma_i2c1_tx.Init.PeriphInc = DMA_PINC_DISABLE;
            embot::hw::i2c::hdma_i2c1_tx.Init.MemInc = DMA_MINC_ENABLE;
            embot::hw::i2c::hdma_i2c1_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            embot::hw::i2c::hdma_i2c1_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
            embot::hw::i2c::hdma_i2c1_tx.Init.Mode = DMA_NORMAL;
            embot::hw::i2c::hdma_i2c1_tx.Init.Priority = DMA_PRIORITY_LOW;
            embot::hw::i2c::hdma_i2c1_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
            if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c1_tx) != HAL_OK)
            {
              Error_Handler();
            }

            __HAL_LINKDMA(hi2c,hdmatx,embot::hw::i2c::hdma_i2c1_tx);

            /* I2C1_RX Init */
            embot::hw::i2c::hdma_i2c1_rx.Instance = DMA1_Stream1;
            embot::hw::i2c::hdma_i2c1_rx.Init.Request = DMA_REQUEST_I2C1_RX;
            embot::hw::i2c::hdma_i2c1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
            embot::hw::i2c::hdma_i2c1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
            embot::hw::i2c::hdma_i2c1_rx.Init.MemInc = DMA_MINC_ENABLE;
            embot::hw::i2c::hdma_i2c1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
            embot::hw::i2c::hdma_i2c1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
            embot::hw::i2c::hdma_i2c1_rx.Init.Mode = DMA_NORMAL;
            embot::hw::i2c::hdma_i2c1_rx.Init.Priority = DMA_PRIORITY_LOW;
            embot::hw::i2c::hdma_i2c1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
            if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c1_rx) != HAL_OK)
            {
              Error_Handler();
            }

            __HAL_LINKDMA(hi2c,hdmarx,embot::hw::i2c::hdma_i2c1_rx);
            
            /* I2C1 interrupt Init */
            HAL_NVIC_SetPriority(I2C1_EV_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
            HAL_NVIC_SetPriority(I2C1_ER_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
            /* USER CODE BEGIN I2C1_MspInit 1 */

            /* USER CODE END I2C1_MspInit 1 */
        }
        else if(hi2c->Instance==I2C2)
        {
        /* USER CODE BEGIN I2C2_MspInit 0 */

        /* USER CODE END I2C2_MspInit 0 */

        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C2;
        PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_RCC_GPIOF_CLK_ENABLE();
        /**I2C2 GPIO Configuration
        PF1     ------> I2C2_SCL
        PF0     ------> I2C2_SDA
        */
        GPIO_InitStruct.Pin = I2C2_SCL_Pin|I2C2_SDA_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C2;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __HAL_RCC_I2C2_CLK_ENABLE();

        /* I2C2 DMA Init */
        /* I2C2_RX Init */
        embot::hw::i2c::hdma_i2c2_rx.Instance = DMA1_Stream2;
        embot::hw::i2c::hdma_i2c2_rx.Init.Request = DMA_REQUEST_I2C2_RX;
        embot::hw::i2c::hdma_i2c2_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        embot::hw::i2c::hdma_i2c2_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        embot::hw::i2c::hdma_i2c2_rx.Init.MemInc = DMA_MINC_ENABLE;
        embot::hw::i2c::hdma_i2c2_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c2_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c2_rx.Init.Mode = DMA_NORMAL;
        embot::hw::i2c::hdma_i2c2_rx.Init.Priority = DMA_PRIORITY_LOW;
        embot::hw::i2c::hdma_i2c2_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c2_rx) != HAL_OK)
        {
          Error_Handler();
        }
        
        __HAL_LINKDMA(hi2c,hdmarx,embot::hw::i2c::hdma_i2c2_rx);
        
        /* I2C2_TX Init */
        embot::hw::i2c::hdma_i2c2_tx.Instance = DMA1_Stream3;
        embot::hw::i2c::hdma_i2c2_tx.Init.Request = DMA_REQUEST_I2C2_TX;
        embot::hw::i2c::hdma_i2c2_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        embot::hw::i2c::hdma_i2c2_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        embot::hw::i2c::hdma_i2c2_tx.Init.MemInc = DMA_MINC_ENABLE;
        embot::hw::i2c::hdma_i2c2_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c2_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c2_tx.Init.Mode = DMA_NORMAL;
        embot::hw::i2c::hdma_i2c2_tx.Init.Priority = DMA_PRIORITY_LOW;
        embot::hw::i2c::hdma_i2c2_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c2_tx) != HAL_OK)
        {
          Error_Handler();
        }
        
        __HAL_LINKDMA(hi2c,hdmatx,embot::hw::i2c::hdma_i2c2_tx);

        
        
        /* I2C2 interrupt Init */
        HAL_NVIC_SetPriority(I2C2_EV_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
        HAL_NVIC_SetPriority(I2C2_ER_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);
        /* USER CODE BEGIN I2C2_MspInit 1 */

        /* USER CODE END I2C2_MspInit 1 */
        }
        else if(hi2c->Instance==I2C3)
        {
        /* USER CODE BEGIN I2C3_MspInit 0 */

        /* USER CODE END I2C3_MspInit 0 */

        /** Initializes the peripherals clock
        */
        PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2C3;
        PeriphClkInitStruct.I2c123ClockSelection = RCC_I2C123CLKSOURCE_D2PCLK1;
        if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
        {
            Error_Handler();
        }

        __HAL_RCC_GPIOH_CLK_ENABLE();
        /**I2C3 GPIO Configuration
        PH7     ------> I2C3_SCL
        PH8     ------> I2C3_SDA
        */
        GPIO_InitStruct.Pin = I2C3_SCL_Pin|I2C3_SDA_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_I2C3;
        HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

        /* Peripheral clock enable */
        __HAL_RCC_I2C3_CLK_ENABLE();
        

        /* I2C3 DMA Init */
        /* I2C3_RX Init */
        embot::hw::i2c::hdma_i2c3_rx.Instance = DMA1_Stream5;
        embot::hw::i2c::hdma_i2c3_rx.Init.Request = DMA_REQUEST_I2C3_RX;
        embot::hw::i2c::hdma_i2c3_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
        embot::hw::i2c::hdma_i2c3_rx.Init.PeriphInc = DMA_PINC_DISABLE;
        embot::hw::i2c::hdma_i2c3_rx.Init.MemInc = DMA_MINC_ENABLE;
        embot::hw::i2c::hdma_i2c3_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c3_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c3_rx.Init.Mode = DMA_NORMAL;
        embot::hw::i2c::hdma_i2c3_rx.Init.Priority = DMA_PRIORITY_LOW;
        embot::hw::i2c::hdma_i2c3_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c3_rx) != HAL_OK)
        {
          Error_Handler();
        }
        
        __HAL_LINKDMA(hi2c,hdmarx,embot::hw::i2c::hdma_i2c3_rx);
        
        /* I2C3_TX Init */
        embot::hw::i2c::hdma_i2c3_tx.Instance = DMA1_Stream6;
        embot::hw::i2c::hdma_i2c3_tx.Init.Request = DMA_REQUEST_I2C3_TX;
        embot::hw::i2c::hdma_i2c3_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
        embot::hw::i2c::hdma_i2c3_tx.Init.PeriphInc = DMA_PINC_DISABLE;
        embot::hw::i2c::hdma_i2c3_tx.Init.MemInc = DMA_MINC_ENABLE;
        embot::hw::i2c::hdma_i2c3_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c3_tx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
        embot::hw::i2c::hdma_i2c3_tx.Init.Mode = DMA_NORMAL;
        embot::hw::i2c::hdma_i2c3_tx.Init.Priority = DMA_PRIORITY_LOW;
        embot::hw::i2c::hdma_i2c3_tx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
        if (HAL_DMA_Init(&embot::hw::i2c::hdma_i2c3_tx) != HAL_OK)
        {
          Error_Handler();
        }
        
        __HAL_LINKDMA(hi2c,hdmatx,embot::hw::i2c::hdma_i2c3_tx);
        
        
        /* I2C3 interrupt Init */
        HAL_NVIC_SetPriority(I2C3_EV_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C3_EV_IRQn);
        HAL_NVIC_SetPriority(I2C3_ER_IRQn, 5, 0);
        HAL_NVIC_EnableIRQ(I2C3_ER_IRQn);
        /* USER CODE BEGIN I2C3_MspInit 1 */

        /* USER CODE END I2C3_MspInit 1 */
        }

    }
    


    void HAL_I2C_MspDeInit(I2C_HandleTypeDef* hi2c)
    {

        if(hi2c->Instance==I2C1)
        {
            /* USER CODE BEGIN I2C1_MspDeInit 0 */

            /* USER CODE END I2C1_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_I2C1_CLK_DISABLE();

            /**I2C1 GPIO Configuration
            PB8     ------> I2C1_SCL
            PB7     ------> I2C1_SDA
            */
            HAL_GPIO_DeInit(I2C1_SCL_GPIO_Port, I2C1_SCL_Pin);

            HAL_GPIO_DeInit(I2C1_SDA_GPIO_Port, I2C1_SDA_Pin);

            /* I2C1 DMA DeInit */
            HAL_DMA_DeInit(hi2c->hdmatx);
            HAL_DMA_DeInit(hi2c->hdmarx);
            
            /* I2C1 interrupt DeInit */
            HAL_NVIC_DisableIRQ(I2C1_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C1_ER_IRQn);
            /* USER CODE BEGIN I2C1_MspDeInit 1 */

            /* USER CODE END I2C1_MspDeInit 1 */
        }
        else if(hi2c->Instance==I2C2)
        {
            /* USER CODE BEGIN I2C2_MspDeInit 0 */

            /* USER CODE END I2C2_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_I2C2_CLK_DISABLE();

            /**I2C2 GPIO Configuration
            PF1     ------> I2C2_SCL
            PF0     ------> I2C2_SDA
            */
            HAL_GPIO_DeInit(I2C2_SCL_GPIO_Port, I2C2_SCL_Pin);

            HAL_GPIO_DeInit(I2C2_SDA_GPIO_Port, I2C2_SDA_Pin);

            /* I2C2 DMA DeInit */
            HAL_DMA_DeInit(hi2c->hdmarx);
            HAL_DMA_DeInit(hi2c->hdmatx);
            
            /* I2C2 interrupt DeInit */
            HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C2_ER_IRQn);
            /* USER CODE BEGIN I2C2_MspDeInit 1 */

            /* USER CODE END I2C2_MspDeInit 1 */
        }
        else if(hi2c->Instance==I2C3)
        {
            /* USER CODE BEGIN I2C3_MspDeInit 0 */

            /* USER CODE END I2C3_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_I2C3_CLK_DISABLE();

            /**I2C3 GPIO Configuration
            PH7     ------> I2C3_SCL
            PH8     ------> I2C3_SDA
            */
            HAL_GPIO_DeInit(I2C3_SCL_GPIO_Port, I2C3_SCL_Pin);

            HAL_GPIO_DeInit(I2C3_SDA_GPIO_Port, I2C3_SDA_Pin);

            /* I2C3 DMA DeInit */
            HAL_DMA_DeInit(hi2c->hdmarx);
            HAL_DMA_DeInit(hi2c->hdmatx);
            
            /* I2C3 interrupt DeInit */
            HAL_NVIC_DisableIRQ(I2C3_EV_IRQn);
            HAL_NVIC_DisableIRQ(I2C3_ER_IRQn);
            /* USER CODE BEGIN I2C3_MspDeInit 1 */

            /* USER CODE END I2C3_MspDeInit 1 */
        }
    }
    
}

extern "C"
{
    // Event interrupt
    void I2C1_EV_IRQHandler(void)
    {
        HAL_I2C_EV_IRQHandler(&embot::hw::i2c::hi2c1);
    }

    void I2C2_EV_IRQHandler(void)
    {
        HAL_I2C_EV_IRQHandler(&embot::hw::i2c::hi2c2);
    }
 
    void I2C3_EV_IRQHandler(void)
    {
        HAL_I2C_EV_IRQHandler(&embot::hw::i2c::hi2c3);
    }
    
    // Error interrupt
    void I2C1_ER_IRQHandler(void)
    {
        HAL_I2C_ER_IRQHandler(&embot::hw::i2c::hi2c1);
    }

    void I2C2_ER_IRQHandler(void)
    {
        HAL_I2C_ER_IRQHandler(&embot::hw::i2c::hi2c2);
    }
 
    void I2C3_ER_IRQHandler(void)
    {
        HAL_I2C_ER_IRQHandler(&embot::hw::i2c::hi2c3);
    }
    
    
    void DMA1_Stream0_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c1_tx);
    }


    void DMA1_Stream1_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c1_rx);
    }    
    

    void DMA1_Stream2_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c2_rx);
    }

    void DMA1_Stream3_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c2_tx);
    }

    void DMA1_Stream5_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c3_rx);
    }

    void DMA1_Stream6_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(&embot::hw::i2c::hdma_i2c3_tx);
    }    
}

#endif // i2c

// - support map: end of embot::hw::i2c



// - support map: begin of embot::hw::timer

#include "embot_hw_timer_bsp.h"

#if   !defined(HAL_TIM_MODULE_ENABLED) || !defined(EMBOT_ENABLE_hw_timer)

namespace embot { namespace hw { namespace timer {
    
    constexpr BSP thebsp { };
    void BSP::init(embot::hw::TIMER h) const {}    
    const BSP& getBSP() 
    {
        return thebsp;
    }
    
}}}

#elif defined(EMBOT_ENABLE_hw_timer_emulated)

namespace embot { namespace hw { namespace timer {
    
    #if defined(STM32HAL_BOARD_AMC)    
    
    constexpr PROP tim01p = { };
    constexpr PROP tim02p = { };
    constexpr PROP tim03p = { };
    constexpr PROP tim04p = { };  
    constexpr PROP tim05p = { };
    constexpr PROP tim06p = { };  
    
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TIMER::one) | mask::pos2mask<uint32_t>(TIMER::two) | 
        mask::pos2mask<uint32_t>(TIMER::three) | mask::pos2mask<uint32_t>(TIMER::four) |
        mask::pos2mask<uint32_t>(TIMER::four) | mask::pos2mask<uint32_t>(TIMER::five),        
        // properties
        {{
            &tim01p, &tim02p, &tim03p, &tim04p, &tim05p, &tim06p, nullptr, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr      // from 9 to 16             
        }}        
    };
    
    void BSP::init(embot::hw::TIMER h) const {}
    
    #else
        #error embot::hw::timer::thebsp must be defined    
    #endif
        
    const BSP& getBSP() 
    {
        return thebsp;
    }    
}}}

#else

namespace embot { namespace hw { namespace timer {
    #if   defined(STM32HAL_BOARD_AMC) 
    // sadly we cannot use constexpr because of the reinterpret_cast<> inside TIM6 etc.
    TIM_HandleTypeDef htim13;
    static const PROP tim13p = { .TIMx = TIM13,  .handle = &htim13,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };
        
    TIM_HandleTypeDef htim15;
    static const PROP tim15p = { .TIMx = TIM15,  .handle = &htim15,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };
        
    TIM_HandleTypeDef htim16;
    static const PROP tim16p = { .TIMx = TIM16,  .handle = &htim16,  .clock = embot::hw::CLOCK::none, .speed = 200 * 1000000, .isonepulse = false, .mastermode = true };
        
    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(TIMER::one) | mask::pos2mask<uint32_t>(TIMER::two) | mask::pos2mask<uint32_t>(TIMER::three),        
        // properties
        {{
            &tim13p, &tim15p, &tim16p, nullptr, nullptr, nullptr, nullptr, nullptr,     // from 1 to 8
            nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr      // from 9 to 16            
        }}        
    };
    
    void BSP::init(embot::hw::TIMER h) const {}    
    #else
        #error embot::hw::timer::thebsp must be defined    
    #endif
        
    const BSP& getBSP() 
    {
        return thebsp;
    }    
}}}

// in here it is implemented in the way the good old hal2 was doing: the handler directly manages the callback
// instead the stm hal make a lot of calls before actually calling the callback code, hence it is slower.

#include "embot_hw_timer.h"

void manageInterrupt(embot::hw::TIMER t, TIM_HandleTypeDef *htim)
{
    if(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_UPDATE) != RESET)
    {
        if(__HAL_TIM_GET_IT_SOURCE(htim, TIM_IT_UPDATE) !=RESET)
        {
            __HAL_TIM_CLEAR_IT(htim, TIM_IT_UPDATE);
            embot::hw::timer::execute(t);
        }
    }   
} 


extern "C" {
    
    void TIM8_UP_TIM13_IRQHandler(void)
    {
        //#warning TODO: cambiare il modo in cui si chiama la callback. usare le callback di stm32
        manageInterrupt(embot::hw::TIMER::one, &embot::hw::timer::htim13);
    }
    
    void TIM15_IRQHandler(void)
    {
        manageInterrupt(embot::hw::TIMER::two, &embot::hw::timer::htim15);
    }
    
    void TIM16_IRQHandler(void)
    {
        manageInterrupt(embot::hw::TIMER::three, &embot::hw::timer::htim16);
    }
}


extern "C"
{
    void HAL_TIM_Base_MspInit(TIM_HandleTypeDef* tim_baseHandle)
    {
      if(tim_baseHandle->Instance==TIM13)
      {
      /* USER CODE BEGIN TIM13_MspInit 0 */

      /* USER CODE END TIM13_MspInit 0 */
        /* TIM13 clock enable */
        __HAL_RCC_TIM13_CLK_ENABLE();

        /* TIM13 interrupt Init */
        HAL_NVIC_SetPriority(TIM8_UP_TIM13_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM8_UP_TIM13_IRQn);
      /* USER CODE BEGIN TIM13_MspInit 1 */

      /* USER CODE END TIM13_MspInit 1 */
      }
      else if(tim_baseHandle->Instance==TIM15)
      {
      /* USER CODE BEGIN TIM15_MspInit 0 */

      /* USER CODE END TIM15_MspInit 0 */
        /* TIM15 clock enable */
        __HAL_RCC_TIM15_CLK_ENABLE();

        /* TIM15 interrupt Init */
        HAL_NVIC_SetPriority(TIM15_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM15_IRQn);
      /* USER CODE BEGIN TIM15_MspInit 1 */

      /* USER CODE END TIM15_MspInit 1 */
      }
      else if(tim_baseHandle->Instance==TIM16)
      {
      /* USER CODE BEGIN TIM16_MspInit 0 */

      /* USER CODE END TIM16_MspInit 0 */
        /* TIM15 clock enable */
        __HAL_RCC_TIM16_CLK_ENABLE();

        /* TIM16 interrupt Init */
        HAL_NVIC_SetPriority(TIM16_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(TIM16_IRQn);
      /* USER CODE BEGIN TIM15_MspInit 1 */

      /* USER CODE END TIM15_MspInit 1 */
      }
    }

    void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef* tim_baseHandle)
    {

      if(tim_baseHandle->Instance==TIM13)
      {
      /* USER CODE BEGIN TIM13_MspDeInit 0 */

      /* USER CODE END TIM13_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM13_CLK_DISABLE();

        /* TIM13 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM8_UP_TIM13_IRQn);
      /* USER CODE BEGIN TIM13_MspDeInit 1 */

      /* USER CODE END TIM13_MspDeInit 1 */
      }
      else if(tim_baseHandle->Instance==TIM15)
      {
      /* USER CODE BEGIN TIM15_MspDeInit 0 */

      /* USER CODE END TIM15_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM15_CLK_DISABLE();

        /* TIM15 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM15_IRQn);
      /* USER CODE BEGIN TIM15_MspDeInit 1 */

      /* USER CODE END TIM15_MspDeInit 1 */
      }
      else if(tim_baseHandle->Instance==TIM16)
      {
      /* USER CODE BEGIN TIM16_MspDeInit 0 */

      /* USER CODE END TIM16_MspDeInit 0 */
        /* Peripheral clock disable */
        __HAL_RCC_TIM16_CLK_DISABLE();

        /* TIM16 interrupt Deinit */
        HAL_NVIC_DisableIRQ(TIM16_IRQn);
      /* USER CODE BEGIN TIM16_MspDeInit 1 */

      /* USER CODE END TIM16_MspDeInit 1 */
      }
    }
}

#endif // timer


// - support map: end of embot::hw::timer



// --------------------------------------------------------------------------------------------------------------------
// - board specific methods
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_amc.h"
#include "embot_hw_icc_printer.h"

namespace embot::hw::bsp::amc {
    
    OnSpecialize defOnSpec {};
    
    void set(const OnSpecialize& onsp)
    {
        defOnSpec = onsp;
    }
    
}



// --------------------------------------------------------------------------------------------------------------------
// - specialize the bsp
// --------------------------------------------------------------------------------------------------------------------

//#if defined(EMBOT_ENABLE_hw_J5_powersupply)

#include "embot_hw_gpio.h"
void J5power(bool on)
{
    constexpr embot::hw::GPIO J5pc {embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::five};
    
    constexpr embot::hw::gpio::Config J5pcCfg {
        embot::hw::gpio::Mode::OUTPUTpushpull, 
        embot::hw::gpio::Pull::nopull, 
        embot::hw::gpio::Speed::medium
    };
    static bool initted {false};
    
    if(!initted)
    {
        embot::hw::gpio::init(J5pc, J5pcCfg);
        initted = true;
    }
    
    embot::hw::gpio::set(J5pc, on ? embot::hw::gpio::State::SET : embot::hw::gpio::State::RESET);        
    HAL_Delay(10); // wait for 10 ms to stabilize ...
}
//#endif

//#if defined(EMBOT_ENABLE_hw_spi_123_atstartup)    
#include "embot_hw_gpio.h"
#include "embot_hw_spi_bsp_amc.h"
// it selects spi1 / spi2 / spi3 in connector J5
void prepare_connector_j5_spi123()
{
    // ok, i know it does not compile... because:
    // todo: if we define EMBOT_ENABLE_hw_spi_123_atstartup then we must not call s_J5_SPIpinout() in runtime
    embot::hw::spi::bsp::s_J5_SPIpinout(embot::hw::SPI::one, true);
    embot::hw::spi::bsp::s_J5_SPIpinout(embot::hw::SPI::two, true);
    embot::hw::spi::bsp::s_J5_SPIpinout(embot::hw::SPI::three, true);
}
//#endif

#if   !defined(EMBOT_ENABLE_hw_bsp_specialize)
bool embot::hw::bsp::specialize() { return true; }
#else   

#if   defined(STM32HAL_BOARD_AMC)
    
       
    bool embot::hw::bsp::specialize()
    {
        embot::core::print("Called embot::hw::bsp::specialize");
        constexpr uint32_t hsem0 {0};
        constexpr uint32_t procID0 {0};
        
        switch(amc::defOnSpec.cm4mode)
        {
            case amc::OnSpecialize::CM4MODE::activateandhold:
            {
                // 1. init the hsems and take hsem-0
                __HAL_RCC_HSEM_CLK_ENABLE();
                HAL_HSEM_FastTake(hsem0);

                // 2. enable the second core
                HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);                
            } break;
 
            case amc::OnSpecialize::CM4MODE::release:
            {
                // 1. init the hsems (just in case) and release hsem-0
                __HAL_RCC_HSEM_CLK_ENABLE();
                HAL_HSEM_Release(hsem0, procID0);          

#if defined(EMBOT_ENABLE_hw_icc_printer)
                // and also activate the printer
                embot::hw::icc::printer::theServer::getInstance().initialise({});
#endif                
            } break;
            
            
            case amc::OnSpecialize::CM4MODE::activate:
            {
                // 1. init the hsems and take hsem-0
                __HAL_RCC_HSEM_CLK_ENABLE();
                HAL_HSEM_FastTake(hsem0);

                // 2. enable the second core
                HAL_RCCEx_EnableBootCore(RCC_BOOT_C2);   

                // 3. and release hsem-0
                HAL_HSEM_Release(hsem0, procID0);
                
            } break;            
            
            case amc::OnSpecialize::CM4MODE::donothing:
            default: 
            {
                break;
            }
        }
        
        if(true == amc::defOnSpec.enableJ5spi_123_at_startup)
        {
            prepare_connector_j5_spi123();
        }
        
        if(true == amc::defOnSpec.enableJ5powersupply_at_startup)
        {
            J5power(true);
        }
       
        return true;
    }

   
    #else
        #error embot::hw::bsp::specialize() must be defined    
    #endif  
#endif  //EMBOT_ENABLE_hw_bsp_specialize


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

