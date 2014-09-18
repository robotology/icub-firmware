/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

/* @file       hal_brdcfg.c
    @brief      This header file includes .c dependent on board
    @author     valentina.gaggero@iit.it
    @date       11/16/2010
 **/


#ifdef  HAL_BOARD_6SG
    #include "brdcfg/hal_brdcfg_6sg.c"
#elif defined HAL_BOARD_2FOC
    #include "brdcfg/hal_brdcfg_2foc.c"
#elif defined HAL_BOARD_2FOCAPPL
    #include "brdcfg/hal_brdcfg_2focappl.c"
#else
    #error "Please select board XXX w/ HAL_BOARD_XXX"
#endif                         


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------
