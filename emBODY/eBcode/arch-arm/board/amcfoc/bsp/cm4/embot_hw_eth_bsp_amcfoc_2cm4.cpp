
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

#include "embot_hw_bsp_config.h"


// --------------------------------------------------------------------------------------------------------------------
// - support maps
// --------------------------------------------------------------------------------------------------------------------



// - support map: begin of embot::hw::eth

#include "embot_hw_eth.h"
#include "embot_hw_eth_bsp.h"

#if !defined(EMBOT_ENABLE_hw_eth) || !defined(HAL_ETH_MODULE_ENABLED)   

namespace embot::hw::eth::bsp {
    
    constexpr BSP thebsp { };
    
    void BSP::init(ipal_hal_eth_cfg_t *cfg) const {}
    
    void BSP::enable() const {}
    
    void BSP::disable() const {}
        
    void BSP::sendframe(ipal_hal_eth_frame_t *frame) const {}
    
    size_t BSP::get_frame_size() const { return 0; }
    
    void BSP::get_frame(size_t length, uint8_t* frame) const {}

    uint64_t BSP::get_mac() const { return 0; }
    
}

#elif defined(EMBOT_ENABLE_hw_eth)

#include "ipal_hal_eth_stm32h7.h"
#include "embot_hw_chip_KSZ8563.h"

#include "ethram.h"

void MX_ETH_Init(void);

namespace embot::hw::eth::bsp {
                   
    constexpr PROP eth1p = { &heth }; 
    

    constexpr BSP thebsp {        
        // maskofsupported
        mask::pos2mask<uint32_t>(EtH::one),        
        // properties
        {{
            &eth1p            
        }}        
    }; 
    
    constexpr embot::hw::chip::KSZ8563::Config ecfg
    {
        // embot::hw::SPI ->
        embot::hw::SPI::five,
        // embot::hw::spi::Config ->
        {
            embot::hw::spi::Prescaler::two,
            embot::hw::spi::DataSize::eight,
            embot::hw::spi::Mode::zero,
            {}
        },
        // embot::hw::chip::KSZ8563::PinControl -> default is OK
        // ns = PF6, config = 
        {            
            { embot::hw::GPIO::PORT::F, embot::hw::GPIO::PIN::six }, 
            { embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::veryhigh }            
        }        
    };
    
    embot::hw::chip::KSZ8563 *ethswitch {nullptr};    
    
    
    void BSP::init(ipal_hal_eth_cfg_t *cfg) const
    {
        static bool _initted {false};
        
        if(true == _initted)
        {   // already initted
            return;
        }
        
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_GPIOD_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_GPIOE_CLK_ENABLE();
        __HAL_RCC_GPIOG_CLK_ENABLE();
        __HAL_RCC_GPIOC_CLK_ENABLE();
        __HAL_RCC_GPIOH_CLK_ENABLE();
        __HAL_RCC_GPIOF_CLK_ENABLE();        
        
        // prepare GPIOs for ETH
        
        HAL_GPIO_WritePin(GPIOF, GPIO_PIN_6, GPIO_PIN_SET); // ETH_nSEL_Pin
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET); // ETH_nRST_GPIO_Port, ETH_nRST_Pin
            
        
        GPIO_InitTypeDef GPIO_InitStruct = {0};
  
        GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;    // ETH_nPME_Pin|ETH_nIRQ_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
        GPIO_InitStruct.Pull = GPIO_PULLUP;
        HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);      


        GPIO_InitStruct.Pin = GPIO_PIN_8; // TP2_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF0_MCO;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);   // TP2_GPIO_Port     
        
        GPIO_InitStruct.Pin = GPIO_PIN_6; // ETH_nSEL_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);      


        GPIO_InitStruct.Pin = GPIO_PIN_13; // ETH_nRST_Pin;
        GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); // ETH_nRST_GPIO_Port
 
        // we need to:
        // 1. init the eth switch chip ...
        // 2. init the eth peripheral
        // 3. init the ipal driver that links the eth peripheral to ipal
        
        //ethswitch = new embot::hw::chip::KSZ8563;
        //ethswitch->init(ecfg);
        
        
        
        MX_ETH_Init();
        
        ipal_hal_eth_stm32h7_init(cfg);
        
        _initted = true;
    }
    
    void BSP::enable() const
    {
        NVIC_EnableIRQ(ETH_IRQn); 
    }
    
    void BSP::disable() const
    {
        NVIC_DisableIRQ(ETH_IRQn);
    }
        
    void BSP::sendframe(ipal_hal_eth_frame_t *frame) const
    {
        ipal_hal_eth_stm32h7_sendframe(frame);
    }
    
    size_t BSP::get_frame_size() const
    {
        size_t s {0};
        s = ipal_hal_eth_stm32h7_get_frame_size();
        return s;
    }
    
    void BSP::get_frame(size_t length, uint8_t* frame) const
    {
        ipal_hal_eth_stm32h7_get_frame(length, frame);
    }    

    uint64_t BSP::get_mac() const
    {
        return 0;
    }    

    static constexpr embot::hw::chip::KSZ8563::PHY phys[2] = 
    {
        embot::hw::chip::KSZ8563::PHY::one, embot::hw::chip::KSZ8563::PHY::two
    };
    
    static constexpr embot::hw::chip::KSZ8563::PORT ports[2] = 
    {
        embot::hw::chip::KSZ8563::PORT::one, embot::hw::chip::KSZ8563::PORT::two
    };    
        
    bool BSP::islinkup(embot::hw::PHY phy) const
    {
        if(nullptr == ethswitch)
        {            
            return true;
        }
               
        embot::hw::chip::KSZ8563::Link lnk { embot::hw::chip::KSZ8563::Link::DOWN };
        ethswitch->read(phys[embot::core::tointegral(phy)], lnk);
        
        return (embot::hw::chip::KSZ8563::Link::UP == lnk) ? true : false;       
    }
    
    uint64_t BSP::errors(embot::hw::PHY phy, ERR e) const
    {
        if(nullptr == ethswitch)
        {            
            return 0;
        }  
                
        static constexpr embot::hw::chip::KSZ8563::MIB mibs[1] =
        {
            embot::hw::chip::KSZ8563::MIB::RxCRCerror
        };
        static constexpr embot::hw::chip::KSZ8563::PORT ports[3] =
        {
            embot::hw::chip::KSZ8563::PORT::one, 
            embot::hw::chip::KSZ8563::PORT::two,
            embot::hw::chip::KSZ8563::PORT::three
        };
        static embot::hw::chip::KSZ8563::MIBdata data {};
        ethswitch->read(ports[embot::core::tointegral(phy)], mibs[embot::core::tointegral(e)], data);
        
        return data.value();
    }

    const BSP& getBSP() 
    {
        return thebsp;
    }
    
} // namespace embot::hw::eth::bsp {   


//namespace embot::hw::eth::bsp { 
    
    ETH_TxPacketConfig TxConfig {};    
    
    void MX_ETH_Init(void)
    {
        static uint8_t MACAddr[6];

        heth.Instance = ETH;
        MACAddr[0] = 0x70;
        MACAddr[1] = 0x9A;
        MACAddr[2] = 0x0B;
        MACAddr[3] = 0x00;
        MACAddr[4] = 0x00;
        MACAddr[5] = 0x00;
        heth.Init.MACAddr = &MACAddr[0];
        heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
        heth.Init.TxDesc = DMATxDscrTab;
        heth.Init.RxDesc = DMARxDscrTab;
        heth.Init.RxBuffLen = 1524;

        if (HAL_ETH_Init(&heth) != HAL_OK)
        {
            for(;;);
        }

        memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
        TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
        TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
        TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
    }    
              
//} // namespace embot::hw::eth::bsp {
    

#if defined(HAL_ETH_MODULE_ENABLED)

    extern "C"
    {
        void ETH_IRQHandler(void)
        {
            HAL_ETH_IRQHandler(&heth);
        }

        void ETH_WKUP_IRQHandler(void)
        {
            HAL_ETH_IRQHandler(&heth);
        }
    }
    
    
    // see eth.c
    void HAL_ETH_MspInit(ETH_HandleTypeDef* ethHandle)
    {

        GPIO_InitTypeDef GPIO_InitStruct = {0};
        if(ethHandle->Instance==ETH)
        {
            /* USER CODE BEGIN ETH_MspInit 0 */

            /* USER CODE END ETH_MspInit 0 */
            /* ETH clock enable */
            __HAL_RCC_ETH1MAC_CLK_ENABLE();
            __HAL_RCC_ETH1TX_CLK_ENABLE();
            __HAL_RCC_ETH1RX_CLK_ENABLE();

            __HAL_RCC_GPIOG_CLK_ENABLE();
            __HAL_RCC_GPIOC_CLK_ENABLE();
            __HAL_RCC_GPIOB_CLK_ENABLE();
            __HAL_RCC_GPIOA_CLK_ENABLE();
            /**ETH GPIO Configuration
            PG14     ------> ETH_TXD1
            PC1     ------> ETH_MDC
            PC5     ------> ETH_RXD1
            PG13     ------> ETH_TXD0
            PA7     ------> ETH_CRS_DV
            PA1     ------> ETH_REF_CLK
            PG11     ------> ETH_TX_EN
            PA2     ------> ETH_MDIO
            PC4     ------> ETH_RXD0
            */
			
			//ETH_TX_EN|ETH_TXD0|ETH_TXD1
            GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
            HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_4;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
            HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

            GPIO_InitStruct.Pin = GPIO_PIN_7|GPIO_PIN_1|GPIO_PIN_2;
            GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
            GPIO_InitStruct.Pull = GPIO_NOPULL;
            GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
            GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

            /* ETH interrupt Init */
            HAL_NVIC_SetPriority(ETH_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(ETH_IRQn);
            HAL_NVIC_SetPriority(ETH_WKUP_IRQn, 0, 0);
            HAL_NVIC_EnableIRQ(ETH_WKUP_IRQn);
            /* USER CODE BEGIN ETH_MspInit 1 */

            /* USER CODE END ETH_MspInit 1 */
        }
    }

    void HAL_ETH_MspDeInit(ETH_HandleTypeDef* ethHandle)
    {

        if(ethHandle->Instance==ETH)
        {
            /* USER CODE BEGIN ETH_MspDeInit 0 */

            /* USER CODE END ETH_MspDeInit 0 */
            /* Peripheral clock disable */
            __HAL_RCC_ETH1MAC_CLK_DISABLE();
            __HAL_RCC_ETH1TX_CLK_DISABLE();
            __HAL_RCC_ETH1RX_CLK_DISABLE();

            /**ETH GPIO Configuration
            PG14     ------> ETH_TXD1
            PC1     ------> ETH_MDC
            PC5     ------> ETH_RXD1
            PG13     ------> ETH_TXD0
            PA7     ------> ETH_CRS_DV
            PA1     ------> ETH_REF_CLK
            PG11     ------> ETH_TX_EN
            PA2     ------> ETH_MDIO
            PC4     ------> ETH_RXD0
            */
            HAL_GPIO_DeInit(GPIOG, GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_14);

            HAL_GPIO_DeInit(GPIOC, GPIO_PIN_1|GPIO_PIN_5|GPIO_PIN_4);

            HAL_GPIO_DeInit(GPIOA, GPIO_PIN_7|GPIO_PIN_1|GPIO_PIN_2);

            /* ETH interrupt Deinit */
            HAL_NVIC_DisableIRQ(ETH_IRQn);
            HAL_NVIC_DisableIRQ(ETH_WKUP_IRQn);
            /* USER CODE BEGIN ETH_MspDeInit 1 */

            /* USER CODE END ETH_MspDeInit 1 */
        }
    }

    
#endif //#if defined(HAL_ETH_MODULE_ENABLED)
            
#endif // defined(EMBOT_ENABLE_hw_eth)


// - support map: end of embot::hw::eth    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

