/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _HL_CFG_PLUS_MODULES_H_
#define _HL_CFG_PLUS_MODULES_H_


// -- definitions of macros used in hl

#include "hl_common.h"

#if     defined(HL_CFG_TARGET_BY_FILE)
    #include "hl_cfg_plus_target.h"
#endif


// whatever is required by hl_cfg_plus_modules is inside a proper section of hal_brdcfg.h
#include "hal_brdcfg.h"



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



