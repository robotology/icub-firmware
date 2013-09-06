/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _EESHAREDSERVICES_H_
#define _EESHAREDSERVICES_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eEsharedServices.c
    @brief      This header file implements public interface to ....
    @author     marco.accame@iit.it
    @date       12/12/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "eEcommon.h"


// - public #define  --------------------------------------------------------------------------------------------------

// #define SHALS_MODE_STATIC
// #define SHALBASE_MODE_STATICLIBRARY
// #define SHALPART_MODE_STATICLIBRARY
// #define SHALINFO_MODE_STATICLIBRARY





// - declaration of public user-defined types ------------------------------------------------------------------------- 


// - declaration of extern public functions ---------------------------------------------------------------------------

// so far it is just a dummy wrapper. in the future we may use just ... ee_sharserv_xxxx calls
#include "shalBASE.h"
#include "shalPART.h"
#include "shalINFO.h"


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



