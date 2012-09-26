/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
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


// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOAPPTHECANBOARDSMANAGER_HID_H_
#define _EOAPPTHECANBOARDSMANAGER_HID_H_


// - doxy -------------------------------------------------------------------------------------------------------------
/* @file       EOappTheCanBoardsManager.h
    @brief     This file provides hidden interface to TheCanBoardsManager singleton.
    @author    valentina.gaggero@iit.it
    @date       06/27/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
#include "EOappCanServicesProvider.h"



// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOappTheCanBoardsManager.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef enum
{
    eO_appTheCanBrdsMng_st__NOTinited   = 0,
    eO_appTheCanBrdsMng_st__inited      = 1,
} eO_appTheCanBrdsMng_states_hid_t;



struct EOappTheCanBrdsMng_hid
{
    eOappTheCanBrdsMng_cfg_t            cfg;
    eO_appTheCanBrdsMng_states_hid_t    st;
    uint16_t                            totalnumofcanboards; /*mi e' suff avere il numero totale dell board 
                                e non la lista dei vari bid perche il bid va da 0 a totalnumofcanboards-1*/
}; 
/*this struct contains data configuration for mc4 only: this data are not sent by pc104 and are equal for all mc4*/
// typedef struct
// {
//     eOmc_estimShift_t estimShiftFactor;
//     uint8_t           velShiftFactor;
// } eO_appTheCanBrdsMng_mc4Config_hid_t;


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty-section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



