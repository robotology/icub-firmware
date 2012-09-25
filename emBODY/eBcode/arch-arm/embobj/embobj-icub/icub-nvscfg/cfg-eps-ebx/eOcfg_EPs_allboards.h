/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOCFG_EPS_ALLBOARDS_H_
#define _EOCFG_EPS_ALLBOARDS_H_

#ifdef __cplusplus
extern "C" {
#endif

// This file collect all header files from all the boards, so that including this file all EP_cfg are available.
// Now it's used only by pc104, move it in the arch-host folder ??

#define FIRST_BOARD		1
#define LAST_BOARD		9


#include "eOcfg_EPs_eb1.h"
#include "eOcfg_EPs_eb2.h"
#include "eOcfg_EPs_eb3.h"
#include "eOcfg_EPs_eb4.h"
#include "eOcfg_EPs_eb5.h"
#include "eOcfg_EPs_eb6.h"
#include "eOcfg_EPs_eb7.h"
#include "eOcfg_EPs_eb8.h"
#include "eOcfg_EPs_eb9.h"


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif

#endif
