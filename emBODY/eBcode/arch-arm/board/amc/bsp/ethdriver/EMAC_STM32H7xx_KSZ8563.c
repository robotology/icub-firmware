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
 * Driver:       Driver_ETH_MAC0
 * Configured:   via STM32CubeMx configuration tool
 * Project:      Ethernet Media Access (MAC) Driver for STM32H7xx
 * --------------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                     Value
 *   ---------------------                     -----
 *   Connect to hardware via Driver_ETH_MAC# = 0
 * -------------------------------------------------------------------------- */

/* History:
 *  Version 1.3
 *    Added data cache enable bit check
 *  Version 1.2
 *    Updated GetRxFrameSize and ReadFrame functions to correctly use HAL 
 *  Version 1.1
 *    Updated __MEMORY_AT macro
 *  Version 1.0
 *    Initial release
 */
 
/*! \page stm32h7_emac CMSIS-Driver Ethernet MAC Setup

The CMSIS-Driver EMAC requires:
  - Setup of AHB1 clock to 200MHz or lower
  - Setup of ETH in MII or RMII mode
  - Setup of NVIC to enable Ethernet global interrupt
  - Setup of DMA descriptors and data buffers to AXI SRAM (D1) or AHB SRAM (D2 or D3)
  - Setup MPU to disable CPU DCache when accessing Ethernet DMA descriptors

\note The User Label name is used to connect the CMSIS-Driver to the GPIO pin.

The example below uses correct settings for STM32H743I-EVAL:
  - ETH Mode: RMII

For different boards, refer to the hardware schematics to reflect correct setup values.

The STM32CubeMX configuration steps for Pinout, Clock, and System Configuration are 
listed below. Enter the values that are marked \b bold.

Pinout tab
----------
  1. Configure ETH mode
     - Peripherals \b ETH: Mode=<b>RMII</b>

Clock Configuration tab
-----------------------
  1. Configure AHB1 Clock: To AHB1,2 Peripheral Clocks (MHz) = <b>200MHz or lower</b>

Configuration tab
-----------------
  1. Under Connectivity open \b ETH Configuration:
     - <b>GPIO Settings</b>: review settings, no changes required
          Pin Name | Signal on Pin | GPIO mode | GPIO Pull-up/Pull..| Maximum out | User Label
          :--------|:--------------|:----------|:-------------------|:------------|:----------
          PA1      | ETH_REF_CLK   | Alternate | No pull-up and no..| High        |.
          PA2      | ETH_MDIO      | Alternate | No pull-up and no..| High        |.
          PA7      | ETH_CRS_DV    | Alternate | No pull-up and no..| High        |.
          PC1      | ETH_MDC       | Alternate | No pull-up and no..| High        |.
          PC4      | ETH_RXD0      | Alternate | No pull-up and no..| High        |.
          PC5      | ETH_RXD1      | Alternate | No pull-up and no..| High        |.
          PG11     | ETH_TX_EN     | Alternate | No pull-up and no..| High        |.
          PG13     | ETH_TXD0      | Alternate | No pull-up and no..| High        |.
          PG14     | ETH_TXD1      | Alternate | No pull-up and no..| High        |.

     - <b>NVIC Settings</b>: enable interrupts
          Interrupt Table                      | Enable | Preemption Priority | Sub Priority
          :------------------------------------|:-------|:--------------------|:------------
          Ethernet global interrupt            |\b ON   | 0                   | 0

     - <b>Parameter Settings</b>: configure descriptor and RX buffer memory locations
          General:Ethernet Configuration | Value
          :------------------------------|:-------
          Ethernet MAC Address           | unused
          Tx Descriptor Length           | \b 4
          First Tx Descriptor Address    | \b 0x30040060
          Rx Descriptor Length           | \b 4
          First Rx Descriptor Address    | \b 0x30040000
          Rx Buffers Address             | \b 0x30040200
          Rx Buffers Length              | \b 1524          

     \n\note
     Add <b>"RxDecripSection", "TxDecripSection" and "RxArraySection" </b> sections to the Scatter file if GNU Compiler or Arm Compiler 6 is used.\n      
     Example:
     \code{.sct}
       RW_ETH_RX_DESC 0x30040000 0x00000060 {
         *(.RxDecripSection)
       }
       RW_ETH_TX_DESC 0x30040060 0x000001A0 {
         *(.TxDecripSection)
       }
       RW_ETH_RX_BUF  0x30040200 0x00001800 {
         *(.RxArraySection)
       }
     \endcode
     \endnote

     - <b>User Constants</b>: not used

     Click \b OK to close the ETH Configuration dialog
 
  2. Under System open \b CORTEX_M7 Configuration
     - <b>Parameter Settings</b>: optionally enable cache and configure MPU
          Cortex Interface Settings    | Value
          :----------------------------|:-------
          CPU ICache                   | Enabled (optional)
          CPU DCache Length            | Enabled (optional)
          
          Cortex MPU Control Settings  | Value
          :----------------------------|:-------
          MPU Control Mode             | Background Region Privileged accesses only
          
          Cortex MPU Region 0 Settings | Value
          :----------------------------|:-------
          MPU Region                   | \b Enabled
          MPU Region Base Address      | \b 0x30040000
          MPU Region Size              | \b 256B
          MPU SubRegion Disable        | \b 0x0
          MPU TEX field level          | \b level 0
          MPU Access Permission        | \b ALL ACCESS PERMITTED
          MPU Instruction Access       | \b ENABLE
          MPU Shareability Permission  | \b ENABLE
          MPU Cacheable Permission     | \b DISABLE
          MPU Bufferable Permission    | \b ENABLE
*/



/* Tx buffer address (must be AXI SRAM (D1) or AHB SRAM (D2/D3) */
#ifndef EMAC_TXBUF_ADDRESS
#define EMAC_TXBUF_ADDRESS      0x30042000   
#endif

/* Receive/transmit Checksum offload enable */
#ifndef EMAC_CHECKSUM_OFFLOAD
#define EMAC_CHECKSUM_OFFLOAD   1
#endif

/* IEEE 1588 time stamping enable (PTP) */
#ifndef EMAC_TIME_STAMP
#define EMAC_TIME_STAMP         0
#endif

#include "EMAC_STM32H7xx_KSZ8563.h"
#include "arm_acle.h"
// acemor-rem-ksz8563 #include "itf_reg_ksz8563.h"

/* Driver version */
#define ARM_ETH_MAC_DRV_VERSION         ARM_DRIVER_VERSION_MAJOR_MINOR(1,4)

/* External HAL variables */
extern ETH_HandleTypeDef    heth;
extern ETH_DMADescTypeDef   DMARxDscrTab[ETH_RX_DESC_CNT];
extern ETH_DMADescTypeDef   DMATxDscrTab[ETH_TX_DESC_CNT];

#if defined(STM32HAL_DRIVER_V1B1)
#warning acemor removes Rx_Buff[] for version 1B1  
//extern uint8_t              Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE];
#else
extern uint8_t              Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE];
#endif

/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_ETH_MAC_API_VERSION,
  ARM_ETH_MAC_DRV_VERSION
};

/* Driver Capabilities */
static const ARM_ETH_MAC_CAPABILITIES DriverCapabilities = {
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,   /* checksum_offload_rx_ip4  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,   /* checksum_offload_rx_ip6  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,   /* checksum_offload_rx_udp  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,   /* checksum_offload_rx_tcp  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,   /* checksum_offload_rx_icmp */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,   /* checksum_offload_tx_ip4  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,   /* checksum_offload_tx_ip6  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,   /* checksum_offload_tx_udp  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,   /* checksum_offload_tx_tcp  */
  (EMAC_CHECKSUM_OFFLOAD != 0) ? 1U : 0U,   /* checksum_offload_tx_icmp */
  (ETH_MII != 0) ?
  ARM_ETH_INTERFACE_MII :
  ARM_ETH_INTERFACE_RMII,                   /* media_interface          */
  0U,                                       /* mac_address              */
  1U,                                       /* event_rx_frame           */
  1U,                                       /* event_tx_frame           */
  1U,                                       /* event_wakeup             */
  0U                                        /* precision_timer          */
#if (defined(ARM_ETH_MAC_API_VERSION) && (ARM_ETH_MAC_API_VERSION >= 0x201U))
, 0U                                        /* reserved bits            */
#endif
};

/* Local variables */
static EMAC_CTRL Emac;
static ETH_MACConfigTypeDef       MAC_Config;
static ETH_MACFilterConfigTypeDef MAC_Filter;
static ETH_TxPacketConfig         TX_Config;
static __ALIGNED(32) uint8_t        TX_Buff[ETH_TX_DESC_CNT][ETH_MAX_PACKET_SIZE] __MEMORY_AT(EMAC_TXBUF_ADDRESS); 


/**
  \fn          uint32_t crc32_8bit_rev (uint32_t crc32, uint8_t val)
  \brief       Calculate 32-bit CRC (Polynomial: 0x04C11DB7, data bit-reversed).
  \param[in]   crc32  CRC initial value
  \param[in]   val    Input value
  \return      Calculated CRC value
*/
static uint32_t crc32_8bit_rev (uint32_t crc32, uint8_t val) {
  uint32_t n;

  crc32 ^= __RBIT (val);
  for (n = 8; n; n--) {
    if (crc32 & 0x80000000U) {
      crc32 <<= 1;
      crc32  ^= 0x04C11DB7U;
    } else {
      crc32 <<= 1;
    }
  }
  return (crc32);
}

/**
  \fn          uint32_t crc32_data (const uint8_t *data, uint32_t len)
  \brief       Calculate standard 32-bit Ethernet CRC.
  \param[in]   data  Pointer to buffer containing the data
  \param[in]   len   Data length in bytes
  \return      Calculated CRC value
*/
static uint32_t crc32_data (const uint8_t *data, uint32_t len) {
  uint32_t cnt, crc;

  crc = 0xFFFFFFFFU;
  for (cnt = len; cnt; cnt--) {
    crc = crc32_8bit_rev (crc, *data++);
  }
  return (crc ^ 0xFFFFFFFFU);
}

/* Ethernet Driver functions */

/**
  \fn          ARM_DRIVER_VERSION GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          ARM_ETH_MAC_CAPABILITIES GetCapabilities (void)
  \brief       Get driver capabilities.
  \return      \ref ARM_ETH_MAC_CAPABILITIES
*/
static ARM_ETH_MAC_CAPABILITIES GetCapabilities (void) {
  return DriverCapabilities;
}


/**
  \fn          int32_t Initialize (ARM_ETH_MAC_SignalEvent_t cb_event)
  \brief       Initialize Ethernet MAC Device.
  \param[in]   cb_event  Pointer to \ref ARM_ETH_MAC_SignalEvent
  \return      \ref execution_status
*/
static int32_t Initialize (ARM_ETH_MAC_SignalEvent_t cb_event) {
  static const uint8_t mac_def[6] = { 2, 0, 0, 0, 0, 0 };

  heth.Instance = ETH;
  heth.Init.TxDesc         = DMATxDscrTab;
  heth.Init.RxDesc         = DMARxDscrTab;
  heth.Init.RxBuffLen      = ETH_MAX_PACKET_SIZE;
  heth.Init.MACAddr        = (void *)mac_def;
  heth.Init.MediaInterface = (ETH_MII != 0) ? HAL_ETH_MII_MODE : HAL_ETH_RMII_MODE;

  /* Set Tx packet config common parameters */
  memset (&TX_Config, 0 , sizeof(ETH_TxPacketConfig));
  TX_Config.Attributes     = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TX_Config.ChecksumCtrl   = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TX_Config.CRCPadCtrl     = ETH_CRC_PAD_INSERT;

  /* Clear control structure */
  memset (&Emac, 0, sizeof (EMAC_CTRL));
  Emac.h        = &heth;
  Emac.cb_event = cb_event;
  Emac.flags    = EMAC_FLAG_INIT;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Uninitialize (void)
  \brief       De-initialize Ethernet MAC Device.
  \return      \ref execution_status
*/
static int32_t Uninitialize (void) {

  Emac.flags &= ~EMAC_FLAG_INIT;
  heth.Instance = NULL;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t PowerControl (ARM_POWER_STATE state)
  \brief       Control Ethernet MAC Device Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t PowerControl (ARM_POWER_STATE state) {
  uint32_t idx;

  if ((state != ARM_POWER_OFF)  && 
      (state != ARM_POWER_FULL) && 
      (state != ARM_POWER_LOW)) {
    return ARM_DRIVER_ERROR_UNSUPPORTED;
  }

  switch (state) {
    case ARM_POWER_OFF:

      if (heth.Instance != NULL) {
        HAL_ETH_DeInit(&heth);
      }

      Emac.flags &= ~EMAC_FLAG_POWER;
      break;

    case ARM_POWER_LOW:
      return ARM_DRIVER_ERROR_UNSUPPORTED;

    case ARM_POWER_FULL:
      if ((Emac.flags & EMAC_FLAG_INIT)  == 0U) {
        /* Driver not initialized */
        return ARM_DRIVER_ERROR;
      }
      if ((Emac.flags & EMAC_FLAG_POWER) != 0U) {
        /* Driver already powered */
        break;
      }

      if (HAL_ETH_Init (&heth) != HAL_OK) {
        return ARM_DRIVER_ERROR;
      }
#if defined(STM32HAL_DRIVER_V1B1)
#warning acemor removes Rx_Buff[] for version 1B1   
//      /* Assign memory for Rx packets */
//      for (idx = 0; idx < ETH_RX_DESC_CNT; idx++) {
//        HAL_ETH_DescAssignMemory (&heth, idx, Rx_Buff[idx], NULL);
//      }
#else
      /* Assign memory for Rx packets */
      for (idx = 0; idx < ETH_RX_DESC_CNT; idx++) {
        HAL_ETH_DescAssignMemory (&heth, idx, Rx_Buff[idx], NULL);
      }
#endif
      Emac.tx_buf.len = 0;
      Emac.flags     |= EMAC_FLAG_POWER;
      break;
  }

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t GetMacAddress (ARM_ETH_MAC_ADDR *ptr_addr)
  \brief       Get Ethernet MAC Address.
  \param[in]   ptr_addr  Pointer to address
  \return      \ref execution_status
*/
static int32_t GetMacAddress (ARM_ETH_MAC_ADDR *ptr_addr) {
  uint32_t val;

  if (ptr_addr == NULL) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  val = ETH->MACA0HR;
  ptr_addr->b[5] = (uint8_t)(val >>  8);
  ptr_addr->b[4] = (uint8_t)(val);
  val = ETH->MACA0LR;
  ptr_addr->b[3] = (uint8_t)(val >> 24);
  ptr_addr->b[2] = (uint8_t)(val >> 16);
  ptr_addr->b[1] = (uint8_t)(val >>  8);
  ptr_addr->b[0] = (uint8_t)(val);

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SetMacAddress (const ARM_ETH_MAC_ADDR *ptr_addr)
  \brief       Set Ethernet MAC Address.
  \param[in]   ptr_addr  Pointer to address
  \return      \ref execution_status
*/
static int32_t SetMacAddress (const ARM_ETH_MAC_ADDR *ptr_addr) {

  if (ptr_addr == NULL) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  /* Set Ethernet MAC Address registers */
  ETH->MACA0HR = ((uint32_t)ptr_addr->b[5] <<  8) |  (uint32_t)ptr_addr->b[4];
  ETH->MACA0LR = ((uint32_t)ptr_addr->b[3] << 24) | ((uint32_t)ptr_addr->b[2] << 16) |
                 ((uint32_t)ptr_addr->b[1] <<  8) |  (uint32_t)ptr_addr->b[0];

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SetAddressFilter (const ARM_ETH_MAC_ADDR *ptr_addr,
                                               uint32_t          num_addr)
  \brief       Configure Address Filter.
  \param[in]   ptr_addr  Pointer to addresses
  \param[in]   num_addr  Number of addresses to configure
  \return      \ref execution_status
*/
static int32_t SetAddressFilter (const ARM_ETH_MAC_ADDR *ptr_addr, uint32_t num_addr) {
  uint32_t crc;

  if ((ptr_addr == NULL) && (num_addr != 0)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  /* Use unicast address filtering for first 3 MAC addresses */
  ETH->MACPFR &= ~(ETH_MACPFR_HPF | ETH_MACPFR_HMC);
  ETH->MACHT0R = 0U; ETH->MACHT1R = 0U;

  if (num_addr == 0U) {
    ETH->MACA1HR = 0U; ETH->MACA1LR = 0U;
    ETH->MACA2HR = 0U; ETH->MACA2LR = 0U;
    ETH->MACA3HR = 0U; ETH->MACA3LR = 0U;
    return ARM_DRIVER_OK;
  }

  ETH->MACA1HR = ((uint32_t)ptr_addr->b[5] <<  8) |  (uint32_t)ptr_addr->b[4] | ETH_MACAHR_AE;
  ETH->MACA1LR = ((uint32_t)ptr_addr->b[3] << 24) | ((uint32_t)ptr_addr->b[2] << 16) |
                 ((uint32_t)ptr_addr->b[1] <<  8) |  (uint32_t)ptr_addr->b[0];
  num_addr--;
  if (num_addr == 0U) {
    ETH->MACA2HR = 0U; ETH->MACA2LR = 0U;
    ETH->MACA3HR = 0U; ETH->MACA3LR = 0U;
    return ARM_DRIVER_OK;
  }
  ptr_addr++;

  ETH->MACA2HR = ((uint32_t)ptr_addr->b[5] <<  8) |  (uint32_t)ptr_addr->b[4] | ETH_MACAHR_AE;
  ETH->MACA2LR = ((uint32_t)ptr_addr->b[3] << 24) | ((uint32_t)ptr_addr->b[2] << 16) |
                 ((uint32_t)ptr_addr->b[1] <<  8) |  (uint32_t)ptr_addr->b[0];
  num_addr--;
  if (num_addr == 0U) {
    ETH->MACA3HR = 0U; ETH->MACA3LR = 0U;
    return ARM_DRIVER_OK;
  }
  ptr_addr++;

  ETH->MACA3HR = ((uint32_t)ptr_addr->b[5] <<  8) |  (uint32_t)ptr_addr->b[4] | ETH_MACAHR_AE;
  ETH->MACA3LR = ((uint32_t)ptr_addr->b[3] << 24) | ((uint32_t)ptr_addr->b[2] << 16) |
                 ((uint32_t)ptr_addr->b[1] <<  8) |  (uint32_t)ptr_addr->b[0];
  num_addr--;
  if (num_addr == 0U) {
    return ARM_DRIVER_OK;
  }
  ptr_addr++;

  /* Calculate 64-bit Hash table for remaining MAC addresses */
  for ( ; num_addr; ptr_addr++, num_addr--) {
    crc = crc32_data (&ptr_addr->b[0], 6U) >> 26;
    if (crc & 0x20U) {
      ETH->MACHT0R |= (1U << (crc & 0x1FU));
    }
    else {
      ETH->MACHT1R |= (1U << crc);
    }
  }
  /* Enable both, unicast and hash address filtering */
  ETH->MACPFR |= ETH_MACPFR_HPF | ETH_MACPFR_HMC;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t SendFrame (const uint8_t *frame, uint32_t len, uint32_t flags)
  \brief       Send Ethernet frame.
  \param[in]   frame  Pointer to frame buffer with data to send
  \param[in]   len    Frame buffer length in bytes
  \param[in]   flags  Frame transmit flags (see ARM_ETH_MAC_TX_FRAME_...)
  \return      \ref execution_status
*/
static int32_t SendFrame (const uint8_t *frame, uint32_t len, uint32_t flags) {
  ETH_DMADescTypeDef *tx_desc;
  uint32_t tx_index;

	
  if ((frame == NULL) || (len == 0U)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }

  if (Emac.tx_buf.len == 0) {
    /* Start of a new transmit frame */
    tx_index = heth.TxDescList.CurTxDesc;
    tx_desc  = (ETH_DMADescTypeDef *)heth.TxDescList.TxDesc[tx_index];
    if (tx_desc->DESC3 & ETH_DMATXNDESCWBF_OWN) {
      /* Transmitter is busy, wait */
      return ARM_DRIVER_ERROR_BUSY;
    }
    Emac.tx_buf.buffer = TX_Buff[tx_index];
    Emac.tx_buf.next   = NULL;
  }
  /* Copy data fragments to ETH-DMA buffer */
  memcpy (Emac.tx_buf.buffer + Emac.tx_buf.len, frame, len);
  Emac.tx_buf.len += len;

  if (flags & ARM_ETH_MAC_TX_FRAME_FRAGMENT) {
    /* More data to come, remember current write position */
    return ARM_DRIVER_OK;
  }

  /* Last fragment, send the packet now */
  TX_Config.TxBuffer = &Emac.tx_buf;
  TX_Config.Length   =  Emac.tx_buf.len;

  /* Clean and invalidate data cache */
  HAL_ETH_Transmit_IT (&heth, &TX_Config);

  Emac.tx_buf.len = 0;
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t ReadFrame (uint8_t *frame, uint32_t len)
  \brief       Read data of received Ethernet frame.
  \param[in]   frame  Pointer to frame buffer for data to read into
  \param[in]   len    Frame buffer length in bytes
  \return      number of data bytes read or execution status
                 - value >= 0: number of data bytes read
                 - value < 0: error occurred, value is execution status as defined with \ref execution_status 
*/
static int32_t ReadFrame (uint8_t *frame, uint32_t len) {

  if ((frame == NULL) && (len != 0U)) {
    /* Invalid parameters */
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    /* Driver not yet powered */
    return ARM_DRIVER_ERROR;
  }

  if ((frame != NULL) && (Emac.rx_buf.buffer != NULL)) {
    memcpy (frame, Emac.rx_buf.buffer, len);
    Emac.rx_buf.buffer = NULL;
  }
#if defined(STM32HAL_DRIVER_V1B1)
#warning acemor does not know how to chamge it
    // ETH_UpdateDescriptor Emac.h); is called inside HAL_ETH_ReadData()
#else
//  /* Return block back to EMAC-DMA */
  HAL_ETH_BuildRxDescriptors (Emac.h);
#endif

  return (len);
}

#if defined(STM32HAL_DRIVER_V1B1)
struct pbuf {
  /** next pbuf in singly linked pbuf chain */
  struct pbuf *next;

  /** pointer to the actual data in the buffer */
  void *payload;

  /**
   * total length of this buffer and all next buffers in chain
   * belonging to the same packet.
   *
   * For non-queue packet chains this is the invariant:
   * p->tot_len == p->len + (p->next? p->next->tot_len: 0)
   */
  uint16_t tot_len;

  /** length of this buffer */
  uint16_t len;

  /** a bit field indicating pbuf type and allocation sources
      (see PBUF_TYPE_FLAG_*, PBUF_ALLOC_FLAG_* and PBUF_TYPE_ALLOC_SRC_MASK)
    */
  uint8_t type_internal;

  /** misc flags */
  uint8_t flags;

  /**
   * the reference count always equals the number of pointers
   * that refer to this pbuf. This can be pointers from an application,
   * the stack itself, or pbuf->next pointers from a chain.
   */
  uint8_t ref;

  /** For incoming packets, this contains the input netif's index */
  uint8_t if_idx;
};

#endif

/**
  \fn          uint32_t GetRxFrameSize (void)
  \brief       Get size of received Ethernet frame.
  \return      number of bytes in received frame
*/
static uint32_t GetRxFrameSize (void) {
  
    uint32_t len = 0;
    
#if defined(STM32HAL_DRIVER_V1B1)    
    struct pbuf *p = NULL;
    #warning changed api

    if(HAL_OK == HAL_ETH_ReadData(Emac.h, (void **)&p))
    {
        Emac.rx_buf.buffer = p->payload;
        Emac.rx_buf.len = p->len;
        len = p->len;
    }
    
    return len;
#else    
    
  /* Clean and invalidate data cache */
  if(HAL_ETH_GetRxDataBuffer(Emac.h, &Emac.rx_buf) == HAL_OK) {
    if (HAL_ETH_GetRxDataLength (Emac.h, &len) == HAL_OK) {
      return (len);
    }
  }
  /* No data available */
  return (0);
#endif  
}

/**
  \fn          int32_t GetRxFrameTime (ARM_ETH_MAC_TIME *time)
  \brief       Get time of received Ethernet frame.
  \param[in]   time  Pointer to time structure for data to read into
  \return      \ref execution_status
*/
static int32_t GetRxFrameTime (ARM_ETH_MAC_TIME *time) {
  (void)time;
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
  \fn          int32_t GetTxFrameTime (ARM_ETH_MAC_TIME *time)
  \brief       Get time of transmitted Ethernet frame.
  \param[in]   time  Pointer to time structure for data to read into
  \return      \ref execution_status
*/
static int32_t GetTxFrameTime (ARM_ETH_MAC_TIME *time) {
  (void)time;
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
  \fn          int32_t ControlTimer (uint32_t control, ARM_ETH_MAC_TIME *time)
  \brief       Control Precision Timer.
  \param[in]   control  operation
  \param[in]   time     Pointer to time structure
  \return      \ref execution_status
*/
static int32_t ControlTimer (uint32_t control, ARM_ETH_MAC_TIME *time) {
  (void)control;
  (void)time;
  return ARM_DRIVER_ERROR_UNSUPPORTED;
}

/**
  \fn          int32_t Control (uint32_t control, uint32_t arg)
  \brief       Control Ethernet Interface.
  \param[in]   control  operation
  \param[in]   arg      argument of operation (optional)
  \return      \ref execution_status
*/
static int32_t Control (uint32_t control, uint32_t arg) {

  if ((Emac.flags & EMAC_FLAG_POWER) == 0U) {
    return ARM_DRIVER_ERROR;
  }
  if ((control != ARM_ETH_MAC_CONFIGURE)   && 
      (control != ARM_ETH_MAC_CONTROL_TX)  && 
      (control != ARM_ETH_MAC_CONTROL_RX)  && 
      (control != ARM_ETH_MAC_FLUSH)       && 
      (control != ARM_ETH_MAC_SLEEP)       && 
      (control != ARM_ETH_MAC_VLAN_FILTER)) {
    return ARM_DRIVER_ERROR_PARAMETER;
  }

  switch (control) {
    case ARM_ETH_MAC_CONFIGURE:
      /* Read device configuration first */
      HAL_ETH_GetMACConfig       (Emac.h, &MAC_Config);
      HAL_ETH_GetMACFilterConfig (Emac.h, &MAC_Filter);

      /* Configure 100MBit/10MBit mode */
      switch (arg & ARM_ETH_MAC_SPEED_Msk) {
        case ARM_ETH_MAC_SPEED_10M:
          MAC_Config.Speed = ETH_SPEED_10M;		
          break;
        case ARM_ETH_SPEED_100M:
          MAC_Config.Speed = ETH_SPEED_100M;
          break;
        default:
          return ARM_DRIVER_ERROR_UNSUPPORTED;
      }

      /* Confige Half/Full duplex mode */
      switch (arg & ARM_ETH_MAC_DUPLEX_Msk) {
        case ARM_ETH_MAC_DUPLEX_FULL:
          MAC_Config.DuplexMode = ETH_FULLDUPLEX_MODE;
          break;
        case ARM_ETH_MAC_DUPLEX_HALF:
          MAC_Config.DuplexMode = ETH_HALFDUPLEX_MODE; 
          break;
        default:
          return ARM_DRIVER_ERROR;
      }

      if ((arg & ARM_ETH_MAC_LOOPBACK) != 0U) {
        /* Enable loopback mode */
        MAC_Config.LoopbackMode = ENABLE;
      } else {
        MAC_Config.LoopbackMode = DISABLE;
      }

#if (EMAC_CHECKSUM_OFFLOAD != 0)
      if ((arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX) != 0U) {
        /* Enable rx checksum verification */
        MAC_Config.ChecksumOffload = ENABLE;
      } else {
        MAC_Config.ChecksumOffload = DISABLE;
      }

      /* Enable tx checksum generation */
      if ((arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX) != 0U) {
      } else {
      }
#else
      if ((arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_RX) ||
          (arg & ARM_ETH_MAC_CHECKSUM_OFFLOAD_TX)) {
        /* Checksum offload is disabled in the driver */
        return ARM_DRIVER_ERROR;
      }
#endif

      if ((arg & ARM_ETH_MAC_ADDRESS_BROADCAST) != 0U) {
        /* Enable broadcast frame receive */
        MAC_Filter.BroadcastFilter = ENABLE;
      } else {
        MAC_Filter.BroadcastFilter = DISABLE;
      }

      if ((arg & ARM_ETH_MAC_ADDRESS_MULTICAST) != 0U) {
        /* Enable all multicast frame receive */
        MAC_Filter.PassAllMulticast = ENABLE;
      } else {
        MAC_Filter.PassAllMulticast = DISABLE;
      }

      if ((arg & ARM_ETH_MAC_ADDRESS_ALL) != 0U) {
        /* Enable promiscuous mode (no filtering) */
        MAC_Filter.PromiscuousMode = ENABLE;
      } else {
        MAC_Filter.PromiscuousMode = DISABLE;
      }
      HAL_ETH_SetMACConfig       (Emac.h, &MAC_Config);
      HAL_ETH_SetMACFilterConfig (Emac.h, &MAC_Filter);
      break;

    case ARM_ETH_MAC_CONTROL_TX:
      if (arg != 0U) {
        /* Enable MAC transmitter */
        // ETH->MACCR   |=  ETH_MACCR_TE;
      }
      else {
        /* Disable MAC transmitter */
        // ETH->MACCR   &= ~ETH_MACCR_TE;
      }
      return ARM_DRIVER_OK;

    case ARM_ETH_MAC_CONTROL_RX:
      if (arg != 0U) {
        /* Enable MAC receiver */
        HAL_ETH_Start_IT (Emac.h);
      } else {
        /* Disable MAC receiver */
        HAL_ETH_Stop_IT (Emac.h);
      }
      return ARM_DRIVER_OK;

    case ARM_ETH_MAC_FLUSH:
      /* Flush tx and rx buffers */
      if ((arg & ARM_ETH_MAC_FLUSH_RX) != 0U) {
        /* Flush the receive buffer */
        // Disable Rx if enabled
        // if (rx_en)
        //   ETH->MACCR &= ~ETH_MACCR_RE;
        //
        // Clear rx descriptors here
        //
        // Enable Rx if was disabled above
        // if (rx_en)
        //   ETH->MACCR |=  ETH_MACCR_RE;
      }

      if (arg & ARM_ETH_MAC_FLUSH_TX) {
        /* Flush the transmit buffer */
        // Disable Tx if enabled
        // if (tx_en)
        //   ETH->MACCR &= ~ETH_MACCR_TE;
        //
        // Clear tx descriptors here
        //
        // Enable Tx if was disabled above
        // if (tx_en)
        //   ETH->MACCR |=  ETH_MACCR_TE;
      }
      break;

    case ARM_ETH_MAC_VLAN_FILTER:
      /* Configure VLAN filter */
      if (arg == 0) {
        /* Disable VLAN filter */
      }
      else {
        /* arg bits [0-15] are VLAN tag value */
        if ((arg & ARM_ETH_MAC_VLAN_FILTER_ID_ONLY) != 0U) {
          /* Compare only the 12-bit VLAN identifier */
          HAL_ETH_SetRxVLANIdentifier (Emac.h, ETH_VLANTAGCOMPARISON_12BIT, (arg & 0xFFFF));
        } else {
          /* Compare the complete 16-bit VLAN tag value */
          HAL_ETH_SetRxVLANIdentifier (Emac.h, ETH_VLANTAGCOMPARISON_16BIT, (arg & 0xFFFF));
        }
      }
      break;
  }

  return ARM_DRIVER_OK;
}

/**
	\fn          int32_t PHY_Read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data)
	\brief       Read Ethernet PHY Register through Management Interface.
	\param[in]   phy_addr  5-bit device address
	\param[in]   reg_addr  5-bit register address
	\param[out]  data      Pointer where the result is written to
	\return      \ref execution_status
*/
static int32_t PHY_Read (uint8_t phy_addr, uint8_t reg_addr, uint16_t *data) 
{
	/* Avoid compiler warnings */
	phy_addr = phy_addr;
	reg_addr = reg_addr;
	data = data;
	
	/* UNUSED */
	return 0;
}


/**
	\fn          int32_t PHY_Write (uint8_t phy_addr, uint8_t reg_addr, uint16_t data)
	\brief       Write Ethernet PHY Register through Management Interface.
	\param[in]   phy_addr  5-bit device address
	\param[in]   reg_addr  5-bit register address
	\param[in]   data      16-bit data to write
	\return      \ref execution_status
*/
static int32_t PHY_Write (uint8_t phy_addr, uint8_t reg_addr, uint16_t data) 
{
	/* Avoid compiler warnings */
	phy_addr = phy_addr;
	reg_addr = reg_addr;
	data = data;
	
	/* UNUSED */
	return 0;
}

/* HAL callback: Tx transfer completed (frame sent) */
void HAL_ETH_TxCpltCallback(ETH_HandleTypeDef *heth) {

  if (Emac.cb_event != NULL) {
    Emac.cb_event (ARM_ETH_MAC_EVENT_TX_FRAME);
  }
}

/* HAL callback: Rx transfer completed (frame received) */
void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth) {

  if (Emac.cb_event != NULL) {
    Emac.cb_event (ARM_ETH_MAC_EVENT_RX_FRAME);
  }
}

/* HAL callback: Power management callback (Magic/WOL packet received) */
void HAL_ETH_PMTCallback(ETH_HandleTypeDef *heth) {

  if (Emac.cb_event != NULL) {
    Emac.cb_event (ARM_ETH_MAC_EVENT_WAKEUP);
  }
}


/* MAC Driver Control Block */
ARM_DRIVER_ETH_MAC Driver_ETH_MAC0 = {
  GetVersion,
  GetCapabilities,
  Initialize,
  Uninitialize,
  PowerControl,
  GetMacAddress,
  SetMacAddress,
  SetAddressFilter,
  SendFrame,
  ReadFrame,
  GetRxFrameSize,
  GetRxFrameTime,
  GetTxFrameTime,
  ControlTimer,
  Control,
  PHY_Read,
  PHY_Write
};

/*! \endcond */
