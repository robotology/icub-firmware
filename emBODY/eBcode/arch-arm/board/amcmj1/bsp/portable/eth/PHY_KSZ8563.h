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
 * Project:      Ethernet Physical Layer Transceiver (PHY)
 *               Definitions for KSZ8563
 * -------------------------------------------------------------------- */

#ifndef __PHY_KSZ8563_H
#define __PHY_KSZ8563_H

#include "Driver_ETH_PHY.h"
// acemor-rem-ksz8563
//#include "ksz8563_registers.h"

/* ====================================================================================
 * MACROS
 * ==================================================================================== */
 
/* PHY Driver State Flags */
#define PHY_INIT              0x01U     /* Driver initialized                */
#define PHY_POWER             0x02U     /* Driver power is on   */

/* ====================================================================================
 * TYPE DEFINITIONS
 * ==================================================================================== */

/* PHY Driver Control Structure */
typedef struct phy_ctrl {
  ARM_ETH_PHY_Read_t  reg_rd;           /* PHY register read function        */
  ARM_ETH_PHY_Write_t reg_wr;           /* PHY register write function       */
  uint16_t            bcr;              /* BCR register value                */
  uint8_t             flags;            /* Control flags                     */
  uint8_t             rsvd;             /* Reserved                          */
} PHY_CTRL;

#endif