/*
  * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologiaa
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

/* @file       hal_middleware_interface.c
    @brief      This file wraps the relevant system source file from middleware
    @author     marco.accame@iit.it
    @date       02/27/2013
 **/

 
// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------

#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_MIDDLEWARE_INTERFACE

// - the api ----------------------------------------------------------------------------------------------------------

#include "hal_middleware_interface.h"


// - the file ---------------------------------------------------------------------------------------------------------

// nothing, as we use the hl_core

#else
    #warning BEWARE --> you are not using the standard HAL middleware interface. are you sure? 
#endif//HAL_USE_MIDDLEWARE_INTERFACE


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
