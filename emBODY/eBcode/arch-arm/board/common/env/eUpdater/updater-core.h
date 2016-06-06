/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Alessandro Scalzo
 * email:   alessandro.scalzo@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

#ifndef __UPDATER_CORE_H__
#define __UPDATER_CORE_H__

#include "osal.h"
#include "stdint.h"

#include "EoCommon.h"


extern void upd_core_init(void);

extern uint8_t upd_core_manage_cmd(uint8_t *pktin, uint16_t pktinsize, eOipv4addr_t remaddr, uint8_t *pktout, uint16_t capacityout, uint16_t *sizeout);

#endif
