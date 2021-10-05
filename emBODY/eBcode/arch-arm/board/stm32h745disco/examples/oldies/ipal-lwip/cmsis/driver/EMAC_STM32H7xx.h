/* -----------------------------------------------------------------------------
 * Copyright (c) 2013-2020 Arm Limited.
 *
 * This software is provided 'as-is', without any express or implied warranty.
 * In no event will the authors be held liable for any damages arising from
 * the use of this software. Permission is granted to anyone to use this
 * software for any purpose, including commercial applications, and to alter
 * it and redistribute it freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software in
 *    a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * $Date:        9. April 2020
 * $Revision:    V1.3
 *
 * Project:      Ethernet Media Access (MAC) Definitions for STM32H7xx
 * -------------------------------------------------------------------------- */

#ifndef __EMAC_STM32F7XX_H
#define __EMAC_STM32F7XX_H

#include <string.h>

#if defined(modifiedCMSISETH)
#warning modifiedCMSISETH removed it because it does not find it (it is inside stm32hal.h)
#else
#include "cmsis_compiler.h"
#endif

#include "Driver_ETH_MAC.h"

#if defined(modifiedCMSISETH)
#warning modifiedCMSISETH modified it because we use stm32hal.h
#if defined(USE_STM32HAL)
#include "stm32hal.h"
#else
#include "stm32h7xx_hal.h"
#endif
#else
#include "stm32h7xx_hal.h"
#endif

#if defined(modifiedCMSISETH)
#warning modifiedCMSISETH removed it because we use cube-mx files
#else
#include "RTE_Components.h"
#include "MX_Device.h"
#endif

#ifndef      __MEMORY_AT
  #if     (defined (__CC_ARM))
    #define  __MEMORY_AT(x)     __attribute__((at(x)))
  #elif   (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
    #define  __MEMORY_AT__(x)   __attribute__((section(".bss.ARM.__at_"#x)))
    #define  __MEMORY_AT(x)     __MEMORY_AT__(x)
  #else
    #define  __MEMORY_AT(x)
    #warning Position memory containing __MEMORY_AT macro at absolute address!
  #endif
#endif

/* EMAC Media Interface definition */
#if defined(MX_ETH_TXD2_Pin)   && defined(MX_ETH_TXD3_Pin)   && \
    defined(MX_ETH_RXD2_Pin)   && defined(MX_ETH_RXD3_Pin)   && \
    defined(MX_ETH_TX_CLK_Pin) && defined(MX_ETH_RX_CLK_Pin)
  /* MII pins are defined */
  #define ETH_MII             1
#else
  #define ETH_MII             0
#endif

/* EMAC Driver state flags */
#define EMAC_FLAG_INIT      (1 << 0)    // Driver initialized
#define EMAC_FLAG_POWER     (1 << 1)    // Driver power on

/* EMAC driver control structure */
typedef struct {
  ARM_ETH_MAC_SignalEvent_t cb_event;   // Event callback
  uint8_t       flags;                  // Control and state flags
  ETH_BufferTypeDef tx_buf;             // Transmit buffer pointers
  ETH_BufferTypeDef rx_buf;             // Receive buffer pointers  
  ETH_HandleTypeDef *h;                 // Ethernet handle
} EMAC_CTRL;


/* Exported driver */
extern ARM_DRIVER_ETH_MAC Driver_ETH_MAC0;

#endif /* __EMAC_STM32H7XX_H */
