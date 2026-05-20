/*
 * Copyright (c) 2013-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * -----------------------------------------------------------------------
 *
 * $Date:        6. May 2019
 * $Revision:    V1.3
 *  
 * Driver:       Driver_ETH_PHYn (default: Driver_ETH_PHY0)
 * Project:      Ethernet Physical Layer Transceiver (PHY)
 *               Driver for LAN8742A
 * -----------------------------------------------------------------------
 * Use the following configuration settings in the middleware component
 * to connect to this driver.
 *
 *   Configuration Setting                     Value
 *   ---------------------                     -----
 *   Connect to hardware via Driver_ETH_PHY# = n (default: 0)
 * -------------------------------------------------------------------- */

/* History:
 *  Version 1.3
 *    Corrected power down bit definition
 *  Version 1.2
 *    Updated for ARM compiler 6
 *  Version 1.1
 *    Added driver flow control flags
 *  Version 1.0
 *    Initial release
 */
#include "PHY_KSZ8563.h"
// acemor-rem-ksz8563 #include "itf_reg_ksz8563.h"
#include <stdint.h>

#define ARM_ETH_PHY_DRV_VERSION ARM_DRIVER_VERSION_MAJOR_MINOR(1,3) /* driver version */


#ifndef ETH_PHY_NUM
	#define ETH_PHY_NUM     0        /* Default driver number */
#endif

/* ===========================================================================
 * STATIC VARIABLES AND CONSTANTS
 * =========================================================================== */
 
/* Basic control register */
//static const uint16_t phy_bc_reg[2] = 
//{
//	PORT1_BC_REG,
//	PORT2_BC_REG
//};

/* Basic status register */
//static const uint16_t phy_bs_reg[2] = 
//{
//	PORT1_BS_REG,
//	PORT2_BS_REG,
//};

/* Phy ID Low */
//static const uint16_t phy_id_low_reg[2] = 
//{
//	PORT1_ID_LOW_REG,
//	PORT2_ID_LOW_REG
//};

/* Phy ID High */
//static const uint16_t phy_id_high_reg[2] = 
//{
//	PORT1_ID_HIGH_REG,
//	PORT2_ID_HIGH_REG
//};

/* Remote loopback */
//static const uint16_t phy_remote_loopback_reg[2] = 
//{
//	PORT1_REMOTE_LOOPBACK_REG,
//	PORT2_REMOTE_LOOPBACK_REG
//};

/* Autonegotiation */
//static const uint16_t phy_autoneg_reg[2] = 
//{
//	PORT1_AUTONEG_ADV_REG,
//	PORT2_AUTONEG_ADV_REG
//};

/* Auto MIDI */
//static const uint16_t phy_autoMIDI_reg[2] = 
//{
//	PORT1_AUTOMIDI_REG,
//	PORT2_AUTOMIDI_REG
//};

/* ===========================================================================
 * PRIVATE FUNCTIONS
 * =========================================================================== */


/* Driver Version */
static const ARM_DRIVER_VERSION DriverVersion = {
  ARM_ETH_PHY_API_VERSION,
  ARM_ETH_PHY_DRV_VERSION
};

/* Ethernet PHY control structure */
static PHY_CTRL PHY = { NULL, NULL, 0, 0, 0 };


/**
  \fn          ARM_DRIVER_VERSION GetVersion (void)
  \brief       Get driver version.
  \return      \ref ARM_DRIVER_VERSION
*/
static ARM_DRIVER_VERSION GetVersion (void) {
  return DriverVersion;
}


/**
  \fn          int32_t Initialize (ARM_ETH_PHY_Read_t  fn_read,
                                   ARM_ETH_PHY_Write_t fn_write)
  \brief       Initialize Ethernet PHY Device.
  \param[in]   fn_read   Pointer to \ref ARM_ETH_MAC_PHY_Read
  \param[in]   fn_write  Pointer to \ref ARM_ETH_MAC_PHY_Write
  \return      \ref execution_status
*/
static int32_t Initialize (ARM_ETH_PHY_Read_t fn_read, ARM_ETH_PHY_Write_t fn_write) {

  if ((fn_read == NULL) || (fn_write == NULL)) { return ARM_DRIVER_ERROR_PARAMETER; }

  if ((PHY.flags & PHY_INIT) == 0U) {
    /* Register PHY read/write functions. */
    PHY.reg_rd = fn_read;
    PHY.reg_wr = fn_write;

    PHY.bcr    = 0U;
    PHY.flags  = PHY_INIT;
  }
	
    // acemor-rem-ksz8563
	/* Initialize SPI peripheral */
	// ksz8563_init();
	
  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t Uninitialize (void)
  \brief       De-initialize Ethernet PHY Device.
  \return      \ref execution_status
*/
static int32_t Uninitialize (void) {

  PHY.reg_rd = NULL;
  PHY.reg_wr = NULL;
  PHY.bcr    = 0U;
  PHY.flags  = 0U;

  return ARM_DRIVER_OK;
}

/**
  \fn          int32_t PowerControl (ARM_POWER_STATE state)
  \brief       Control Ethernet PHY Device Power.
  \param[in]   state  Power state
  \return      \ref execution_status
*/
static int32_t PowerControl (ARM_POWER_STATE state) {
    return ARM_DRIVER_OK;
//  uint16_t val;
//	uint8_t data_8;
//	uint16_t data_16;
////	ksz8563_drv_err_t drv_err;
//    // acemor-rem-ksz8563
//    #warning acemor-rem-ksz8563
//	uint16_t reg_addr;

//  switch ((int32_t)state) 
//	{
//		/* ARM Power OFF */
//    case ARM_POWER_OFF:
//      if ((PHY.flags & PHY_INIT) == 0U) {
//        /* Initialize must provide register access function pointers */
//        return ARM_DRIVER_ERROR;
//      }

//      PHY.flags &= ~PHY_POWER;
//      PHY.bcr    =  BCR_POWER_DOWN;
//			
////			/* Power down PHY1 */
////			drv_err = ksz8563_reg_16_write(phy_bc_reg[KSZ8563_PHY_1_ID], PHY.bcr);
////			
////			if (drv_err.status == END_NOK)
////			{
////				return ARM_DRIVER_ERROR;
////			}
//			
////			/* Power down PHY2 */
////			drv_err = ksz8563_reg_16_write(phy_bc_reg[KSZ8563_PHY_2_ID], PHY.bcr);
////			
////			if (drv_err.status == END_NOK)
////			{
////				return ARM_DRIVER_ERROR;
////			}
//			
//      return ARM_DRIVER_OK;

//		/* ARM Power FULL */
//    case ARM_POWER_FULL:
//      if ((PHY.flags & PHY_INIT) == 0U) {
//        return ARM_DRIVER_ERROR;
//      }
//      if (PHY.flags & PHY_POWER) {
//        return ARM_DRIVER_OK;
//      }
//			
//			/* ------------------------------------------------------------------------
//			 * Check PHY1 ID
//			 * ------------------------------------------------------------------------ */
//      #warning TODO: verify if this code is called  .......
////      /* Check Device Identification (HI) */
////			ksz8563_reg_16_read(phy_id_high_reg[KSZ8563_PHY_1_ID], &val);
//      
////      if (val != KSZ8563_REGID_PHYID_H) {
////        /* Invalid PHY ID */
////        return ARM_DRIVER_ERROR_UNSUPPORTED;
////      }

//			/* Check Device Identification (LO) */
////			ksz8563_reg_16_read(phy_id_low_reg[KSZ8563_PHY_1_ID], &val);

////      if (val != KSZ8563_REGID_PHYID_L) {
////        /* Invalid PHY ID */
////        return ARM_DRIVER_ERROR_UNSUPPORTED;
////      }
//			
//			/* ------------------------------------------------------------------------
//			 * Check PHY2 ID
//			 * ------------------------------------------------------------------------ */

////      /* Check Device Identification (HI) */
////			ksz8563_reg_16_read(phy_id_high_reg[KSZ8563_PHY_2_ID], &val);
////      
////      if (val != KSZ8563_REGID_PHYID_H) {
////        /* Invalid PHY ID */
////        return ARM_DRIVER_ERROR_UNSUPPORTED;
////      }

////			/* Check Device Identification (LO) */
////			ksz8563_reg_16_read(phy_id_low_reg[KSZ8563_PHY_2_ID], &val);

////      if (val != KSZ8563_REGID_PHYID_L) {
////        /* Invalid PHY ID */
////        return ARM_DRIVER_ERROR_UNSUPPORTED;
////      }
//					
////			/* Enable ETH_nPME from KSZ8563R to microcontroller */
////			drv_err = ksz8563_reg_8_write(PME_CONTROL_REG, PME_PIN_ENABLE);
//			
//			/* Enable IRQ here */

////			/* Set Driver strength */
////			drv_err = ksz8563_reg_8_write(SERIAL_CONTROL_REG, 0x41);
////			drv_err = ksz8563_reg_8_write(DRIVER_STRENGTH_REG, 0x77);
//			
//      PHY.flags |=  PHY_POWER;

//      return ARM_DRIVER_OK;

//    case ARM_POWER_LOW:
//    default:
//      return ARM_DRIVER_ERROR_UNSUPPORTED;
//  }
}

/**
  \fn          int32_t SetInterface (uint32_t interface)
  \brief       Set Ethernet Media Interface.
  \param[in]   interface  Media Interface type
  \return      \ref execution_status
*/
static int32_t SetInterface (uint32_t interface) {

  if ((PHY.flags & PHY_POWER) == 0U) { return ARM_DRIVER_ERROR; }

  switch (interface) {
    case ARM_ETH_INTERFACE_RMII:
      break;
    case ARM_ETH_INTERFACE_MII:
    default:
      return ARM_DRIVER_ERROR_UNSUPPORTED;
  }
  return(0);
}

/**
  \fn          int32_t SetMode (uint32_t mode)
  \brief       Set Ethernet PHY Device Operation mode.
  \param[in]   mode  Operation Mode
  \return      \ref execution_status
*/
static int32_t SetMode (uint32_t mode) {
  uint16_t val;
	uint8_t val_8;
	uint16_t addr;
//	ksz8563_drv_err_t drv_err;

//  if ((PHY.flags & PHY_POWER) == 0U) 
//	{ 
//		return ARM_DRIVER_ERROR; 
//	}

//  val = PHY.bcr & BCR_POWER_DOWN;

//  switch (mode & ARM_ETH_PHY_SPEED_Msk) {
//    case ARM_ETH_PHY_SPEED_10M:
//      break;
//    case ARM_ETH_PHY_SPEED_100M:
//      val |=  BCR_SPEED_SELECT;
//      break;
//    default:
//      return ARM_DRIVER_ERROR_UNSUPPORTED;
//  }

//  switch (mode & ARM_ETH_PHY_DUPLEX_Msk) {
//    case ARM_ETH_PHY_DUPLEX_HALF:
//      break;
//    case ARM_ETH_PHY_DUPLEX_FULL:
//      val |=  BCR_DUPLEX_MODE;
//      break;
//  }

//  if (mode & ARM_ETH_PHY_AUTO_NEGOTIATE) {
//    val |= BCR_AUTONEG;
//  }

//  if (mode & ARM_ETH_PHY_LOOPBACK) {
//    val |= BCR_LOCAL_LOOPBACK;
//  }

//  if (mode & ARM_ETH_PHY_ISOLATE) {
//    val |= BCR_ISOLATE;
//  }

//  PHY.bcr = val;
	
	/* ----------------------------------------------------------------------
	 * Set mode on PHY1
	 * ---------------------------------------------------------------------- */
	
//	drv_err = ksz8563_reg_16_write(phy_bc_reg[KSZ8563_PHY_1_ID], PHY.bcr);
//	
//	if (drv_err.status==END_NOK)
//	{
//		return ARM_DRIVER_ERROR;
//	}
	
	/* ----------------------------------------------------------------------
	 * Set mode on PHY2
	 * ---------------------------------------------------------------------- */
	
//	drv_err = ksz8563_reg_16_write(phy_bc_reg[KSZ8563_PHY_2_ID], PHY.bcr);
//	
//	if (drv_err.status==END_NOK)
//	{
//		return ARM_DRIVER_ERROR;
//	}
	
	/* MARTINO - Do not wait for autonegotiation */
	#if (0)
		/* Wait for autonegotiation */
		if (mode & ARM_ETH_PHY_AUTO_NEGOTIATE)
		{
			do 
			{	
				ksz8563_reg_16_read(phy_bs_reg[phy_id], &val);
			} while (0 == (val & BSR_AUTONEG_COMPLETE));
		}
	#endif
	
	return ARM_DRIVER_OK;
}

/**
  \fn          ARM_ETH_LINK_STATE GetLinkState (void)
  \brief       Get Ethernet PHY Device Link state.
  \return      current link status \ref ARM_ETH_LINK_STATE
*/
static ARM_ETH_LINK_STATE GetLinkState (void) {
	
	/* MARTINO - Deprecated */
	#if (0)
		ARM_ETH_LINK_STATE state = ARM_ETH_LINK_DOWN;
		uint16_t val = 0U;

		if (PHY.flags & PHY_POWER) 
		{
			/* Check */
			ksz8563_reg_16_read(phy_bs_reg[phy_id], &val);
		}

		state = (val & BSR_LINK_STATUS) ? ARM_ETH_LINK_UP : ARM_ETH_LINK_DOWN;

		return (state);
	#else
	
		return  ARM_ETH_LINK_DOWN;
	
	#endif
}

/**
  \fn          ARM_ETH_LINK_INFO GetLinkInfo (void)
  \brief       Get Ethernet PHY Device Link information.
  \return      current link parameters \ref ARM_ETH_LINK_INFO
*/
static ARM_ETH_LINK_INFO GetLinkInfo (void) {
  ARM_ETH_LINK_INFO info;
	
	/* Always returns 100M, full duplex */
	info.speed = ARM_ETH_SPEED_100M;
	info.duplex = ARM_ETH_DUPLEX_FULL;

  return (info);
}


/* PHY Driver Control Block */
extern
ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_PHY_NUM);
ARM_DRIVER_ETH_PHY ARM_Driver_ETH_PHY_(ETH_PHY_NUM) = {
  GetVersion,
  Initialize,
  Uninitialize,
  PowerControl,
  SetInterface,
  SetMode,
  GetLinkState,
  GetLinkInfo
};
