/**
 * MX Resources
 *
 * This file is needed to "link" resources defined by STCubeMX to Keil Drivers
 */
#ifndef MX_RESOURCES
#define MX_RESOURCES


/* ============================================================================
 * MACROS
 * ============================================================================ */
 
//#warning marco.accame: removed the SPI_USES_DMA part 
//#define SPI_USES_DMA
 
/* ----------------------------------------------------------------------------
 * DMA
 * ---------------------------------------------------------------------------- */

#ifdef SPI_USES_DMA

	#define MX_DMA                                  1
	
	#define SPI_DCACHE_MAINTENANCE       						(0U)

	/* NVIC Configuration */

	/* NVIC DMA1_Stream1_IRQn */
	#define MX_DMA1_Stream1_IRQn_interruptPremptionPriority 0
	#define MX_DMA1_Stream1_IRQn_PriorityGroup      NVIC_PRIORITYGROUP_4
	#define MX_DMA1_Stream1_IRQn_Subriority         0

	/* NVIC DMA1_Stream0_IRQn */
	#define MX_DMA1_Stream0_IRQn_interruptPremptionPriority 0
	#define MX_DMA1_Stream0_IRQn_PriorityGroup      NVIC_PRIORITYGROUP_4
	#define MX_DMA1_Stream0_IRQn_Subriority         0
	
#endif

/*-------------------------------- ETH        --------------------------------*/

#if (1)
#define MX_ETH                                  1


/* GPIO Configuration */

/* Pin PA1 */
#define MX_ETH_REF_CLK_GPIO_Speed               GPIO_SPEED_FREQ_VERY_HIGH
#define MX_ETH_REF_CLK_Pin                      PA1
#define MX_ETH_REF_CLK_GPIOx                    GPIOA
#define MX_ETH_REF_CLK_GPIO_PuPd                GPIO_NOPULL
#define MX_ETH_REF_CLK_GPIO_Pin                 GPIO_PIN_1
#define MX_ETH_REF_CLK_GPIO_AF                  GPIO_AF11_ETH
#define MX_ETH_REF_CLK_GPIO_Mode                GPIO_MODE_AF_PP

/* Pin PA7 */
#define MX_ETH_CRS_DV_GPIO_Speed                GPIO_SPEED_FREQ_VERY_HIGH
#define MX_ETH_CRS_DV_Pin                       PA7
#define MX_ETH_CRS_DV_GPIOx                     GPIOA
#define MX_ETH_CRS_DV_GPIO_PuPd                 GPIO_NOPULL
#define MX_ETH_CRS_DV_GPIO_Pin                  GPIO_PIN_7
#define MX_ETH_CRS_DV_GPIO_AF                   GPIO_AF11_ETH
#define MX_ETH_CRS_DV_GPIO_Mode                 GPIO_MODE_AF_PP

/* Pin PC4 */
#define MX_ETH_RXD0_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_ETH_RXD0_Pin                         PC4
#define MX_ETH_RXD0_GPIOx                       GPIOC
#define MX_ETH_RXD0_GPIO_PuPd                   GPIO_NOPULL
#define MX_ETH_RXD0_GPIO_Pin                    GPIO_PIN_4
#define MX_ETH_RXD0_GPIO_AF                     GPIO_AF11_ETH
#define MX_ETH_RXD0_GPIO_Mode                   GPIO_MODE_AF_PP

/* Pin PC5 */
#define MX_ETH_RXD1_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_ETH_RXD1_Pin                         PC5
#define MX_ETH_RXD1_GPIOx                       GPIOC
#define MX_ETH_RXD1_GPIO_PuPd                   GPIO_NOPULL
#define MX_ETH_RXD1_GPIO_Pin                    GPIO_PIN_5
#define MX_ETH_RXD1_GPIO_AF                     GPIO_AF11_ETH
#define MX_ETH_RXD1_GPIO_Mode                   GPIO_MODE_AF_PP

/* Pin PB11 */
#define MX_ETH_TX_EN_GPIO_Speed                 GPIO_SPEED_FREQ_VERY_HIGH
#define MX_ETH_TX_EN_Pin                        PB11
#define MX_ETH_TX_EN_GPIOx                      GPIOB
#define MX_ETH_TX_EN_GPIO_PuPd                  GPIO_NOPULL
#define MX_ETH_TX_EN_GPIO_Pin                   GPIO_PIN_11
#define MX_ETH_TX_EN_GPIO_AF                    GPIO_AF11_ETH
#define MX_ETH_TX_EN_GPIO_Mode                  GPIO_MODE_AF_PP

/* Pin PA2 */
#define MX_ETH_MDIO_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_ETH_MDIO_Pin                         PA2
#define MX_ETH_MDIO_GPIOx                       GPIOA
#define MX_ETH_MDIO_GPIO_PuPd                   GPIO_NOPULL
#define MX_ETH_MDIO_GPIO_Pin                    GPIO_PIN_2
#define MX_ETH_MDIO_GPIO_AF                     GPIO_AF11_ETH
#define MX_ETH_MDIO_GPIO_Mode                   GPIO_MODE_AF_PP

/* Pin PG12 */
#define MX_ETH_TXD1_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_ETH_TXD1_Pin                         PG12
#define MX_ETH_TXD1_GPIOx                       GPIOG
#define MX_ETH_TXD1_GPIO_PuPd                   GPIO_NOPULL
#define MX_ETH_TXD1_GPIO_Pin                    GPIO_PIN_12
#define MX_ETH_TXD1_GPIO_AF                     GPIO_AF11_ETH
#define MX_ETH_TXD1_GPIO_Mode                   GPIO_MODE_AF_PP

/* Pin PB12 */
#define MX_ETH_TXD0_GPIO_Speed                  GPIO_SPEED_FREQ_VERY_HIGH
#define MX_ETH_TXD0_Pin                         PB12
#define MX_ETH_TXD0_GPIOx                       GPIOB
#define MX_ETH_TXD0_GPIO_PuPd                   GPIO_NOPULL
#define MX_ETH_TXD0_GPIO_Pin                    GPIO_PIN_12
#define MX_ETH_TXD0_GPIO_AF                     GPIO_AF11_ETH
#define MX_ETH_TXD0_GPIO_Mode                   GPIO_MODE_AF_PP

/* Pin PC1 */
#define MX_ETH_MDC_GPIO_Speed                   GPIO_SPEED_FREQ_VERY_HIGH
#define MX_ETH_MDC_Pin                          PC1
#define MX_ETH_MDC_GPIOx                        GPIOC
#define MX_ETH_MDC_GPIO_PuPd                    GPIO_NOPULL
#define MX_ETH_MDC_GPIO_Pin                     GPIO_PIN_1
#define MX_ETH_MDC_GPIO_AF                      GPIO_AF11_ETH
#define MX_ETH_MDC_GPIO_Mode                    GPIO_MODE_AF_PP

/* NVIC Configuration */

/* NVIC ETH_IRQn */
#define MX_ETH_IRQn_interruptPremptionPriority  0
#define MX_ETH_IRQn_PriorityGroup               NVIC_PRIORITYGROUP_4
#define MX_ETH_IRQn_Subriority                  0

#endif

//#warning marco.accame: removed the MX_SPI5 part 
/*-------------------------------- SPI5       --------------------------------*/

//#define MX_SPI5                                 1

#if defined(MX_SPI5)

/* GPIO Configuration */

/* Pin PF6 */
#define MX_SPI5_NSS_GPIO_Speed                  GPIO_SPEED_FREQ_LOW
#define MX_SPI5_NSS_Pin                         PF6
#define MX_SPI5_NSS_GPIOx                       GPIOF
#define MX_SPI5_NSS_GPIO_PuPd                   GPIO_NOPULL
#define MX_SPI5_NSS_GPIO_Pin                    GPIO_PIN_6
#define MX_SPI5_NSS_GPIO_AF                     GPIO_AF5_SPI5
#define MX_SPI5_NSS_GPIO_Mode                   GPIO_MODE_AF_PP

/* Pin PH6 */
#define MX_SPI5_SCK_GPIO_Speed                  GPIO_SPEED_FREQ_HIGH
#define MX_SPI5_SCK_Pin                         PH6
#define MX_SPI5_SCK_GPIOx                       GPIOH
#define MX_SPI5_SCK_GPIO_PuPd                   GPIO_NOPULL
#define MX_SPI5_SCK_GPIO_Pin                    GPIO_PIN_6
#define MX_SPI5_SCK_GPIO_AF                     GPIO_AF5_SPI5
#define MX_SPI5_SCK_GPIO_Mode                   GPIO_MODE_AF_PP

/* Pin PF8 */
#define MX_SPI5_MISO_GPIO_Speed                 GPIO_SPEED_FREQ_HIGH
#define MX_SPI5_MISO_Pin                        PF8
#define MX_SPI5_MISO_GPIOx                      GPIOF
#define MX_SPI5_MISO_GPIO_PuPd                  GPIO_NOPULL
#define MX_SPI5_MISO_GPIO_Pin                   GPIO_PIN_8
#define MX_SPI5_MISO_GPIO_AF                    GPIO_AF5_SPI5
#define MX_SPI5_MISO_GPIO_Mode                  GPIO_MODE_AF_PP

/* Pin PF11 */
#define MX_SPI5_MOSI_GPIO_Speed                 GPIO_SPEED_FREQ_HIGH
#define MX_SPI5_MOSI_Pin                        PF11
#define MX_SPI5_MOSI_GPIOx                      GPIOF
#define MX_SPI5_MOSI_GPIO_PuPd                  GPIO_NOPULL
#define MX_SPI5_MOSI_GPIO_Pin                   GPIO_PIN_11
#define MX_SPI5_MOSI_GPIO_AF                    GPIO_AF5_SPI5
#define MX_SPI5_MOSI_GPIO_Mode                  GPIO_MODE_AF_PP

#endif // #if defined(MX_SPI5)

#ifdef SPI_USES_DMA

/* DMA Configuration */

	/* DMA SPI5_RX */
	#define MX_SPI5_RX_DMA_Instance                 DMA1_Stream0
	#define MX_SPI5_RX_DMA_FIFOMode                 DMA_FIFOMODE_DISABLE
	#define MX_SPI5_RX_DMA_Priority                 DMA_PRIORITY_LOW
	#define MX_SPI5_RX_DMA_MemDataAlignment         DMA_MDATAALIGN_BYTE
	#define MX_SPI5_RX_DMA_Mode                     DMA_NORMAL
	#define MX_SPI5_RX_DMA_SyncRequestNumber        1
	#define MX_SPI5_RX_DMA_Request                  DMA_REQUEST_SPI5_RX
	#define MX_SPI5_RX_DMA_SyncPolarity             HAL_DMAMUX_SYNC_NO_EVENT
	#define MX_SPI5_RX_DMA_Direction                DMA_PERIPH_TO_MEMORY
	#define MX_SPI5_RX_DMA_SignalID                 NONE
	#define MX_SPI5_RX_DMA_MemInc                   DMA_MINC_ENABLE
	#define MX_SPI5_RX_DMA_IpInstance
	#define MX_SPI5_RX_DMA_RequestNumber            1
	#define MX_SPI5_RX_DMA_EventEnable              DISABLE
	#define MX_SPI5_RX_DMA_SyncEnable               DISABLE
	#define MX_SPI5_RX_DMA_DMA_Handle
	#define MX_SPI5_RX_DMA_PeriphDataAlignment      DMA_PDATAALIGN_BYTE
	#define MX_SPI5_RX_DMA_Polarity                 HAL_DMAMUX_REQ_GEN_RISING
	#define MX_SPI5_RX_DMA_SyncSignalID             NONE
	#define MX_SPI5_RX_DMA_PeriphInc                DMA_PINC_DISABLE

	/* DMA SPI5_TX */
	#define MX_SPI5_TX_DMA_Instance                 DMA1_Stream1
	#define MX_SPI5_TX_DMA_FIFOMode                 DMA_FIFOMODE_DISABLE
	#define MX_SPI5_TX_DMA_Priority                 DMA_PRIORITY_LOW
	#define MX_SPI5_TX_DMA_MemDataAlignment         DMA_MDATAALIGN_BYTE
	#define MX_SPI5_TX_DMA_Mode                     DMA_NORMAL
	#define MX_SPI5_TX_DMA_SyncRequestNumber        1
	#define MX_SPI5_TX_DMA_Request                  DMA_REQUEST_SPI5_TX
	#define MX_SPI5_TX_DMA_SyncPolarity             HAL_DMAMUX_SYNC_NO_EVENT
	#define MX_SPI5_TX_DMA_Direction                DMA_MEMORY_TO_PERIPH
	#define MX_SPI5_TX_DMA_SignalID                 NONE
	#define MX_SPI5_TX_DMA_MemInc                   DMA_MINC_ENABLE
	#define MX_SPI5_TX_DMA_IpInstance
	#define MX_SPI5_TX_DMA_RequestNumber            1
	#define MX_SPI5_TX_DMA_EventEnable              DISABLE
	#define MX_SPI5_TX_DMA_SyncEnable               DISABLE
	#define MX_SPI5_TX_DMA_DMA_Handle
	#define MX_SPI5_TX_DMA_PeriphDataAlignment      DMA_PDATAALIGN_BYTE
	#define MX_SPI5_TX_DMA_Polarity                 HAL_DMAMUX_REQ_GEN_RISING
	#define MX_SPI5_TX_DMA_SyncSignalID             NONE
	#define MX_SPI5_TX_DMA_PeriphInc                DMA_PINC_DISABLE

#endif

#if defined(MX_SPI5)
/* NVIC Configuration */

/* NVIC SPI5_IRQn */
#define MX_SPI5_IRQn_interruptPremptionPriority 0
#define MX_SPI5_IRQn_PriorityGroup              NVIC_PRIORITYGROUP_4
#define MX_SPI5_IRQn_Subriority                 0

#endif // #if defined(MX_SPI5)

#endif

