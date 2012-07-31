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

#include "brd_mcbstm32x.h"

#if     defined(USE_BRD_MCBSTM32_C)

    #include "brd_mcbstm32c.c" 

#elif   defined(USE_BRD_MCBSTM32_F200)

    #error --> missing file
    #include "brd_mcbstm32f200.c" 

#elif   defined(USE_BRD_MCBSTM32_F400)
    
    #error --> missing file
    #include "brd_mcbstm32f400.c" 

#else
    #error --> define a MCBSTM32 board
#endif





