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
#ifndef _EEBASICSTORAGE_H_
#define _EEBASICSTORAGE_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eEbasicStorage.c
    @brief      This header file implements public interface to ....
    @author     marco.accame@iit.it
    @date       12/12/2011
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section




// - declaration of public user-defined types ------------------------------------------------------------------------- 

typedef enum
{
        
    ee_basicstrg_res_ok           = 0,        /**< as the ee_res_OK */
    ee_basicstrg_res_ok_generic   = -1        /**< as the ee_res_NOK_generic */
} eEbasicstrg_res_t;

// - declaration of extern public functions ---------------------------------------------------------------------------



extern eEbasicstrg_res_t ee_basicstrg_init(void);

extern eEbasicstrg_res_t ee_basicstrg_canaddress_get(uint32_t *canaddr);


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



