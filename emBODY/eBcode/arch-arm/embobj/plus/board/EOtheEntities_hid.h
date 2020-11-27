/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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

#ifndef _EOTHEENTITIES_HID_H_
#define _EOTHEENTITIES_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

// - external dependencies --------------------------------------------------------------------------------------------
// empty-section

// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheEntities.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

enum 
{  
    eoprotwrap_max_joints       = 12, 
    eoprotwrap_max_motors       = 12, 
    eoprotwrap_max_skins        = 2,
    eoprotwrap_max_strains      = 1,
    eoprotwrap_max_maises       = 1,
    eoprotwrap_max_temperatures = 1,
    eoprotwrap_max_inertials    = 1,
    eoprotwrap_max_inertials3   = 1,
    eoprotwrap_max_pscs         = 1,
    eoprotwrap_max_poss         = 1
};

struct EOtheEntities_hid
{
    eObool_t        initted;
    eOmc_joint_t*   joints[eoprotwrap_max_joints];
    eOmc_motor_t*   motors[eoprotwrap_max_motors];
    eOsk_skin_t*    skins[eoprotwrap_max_skins];
    eOas_strain_t*  strains[eoprotwrap_max_strains];
    eOas_mais_t*    maises[eoprotwrap_max_maises];
    eOas_temperature_t* temperatures[eoprotwrap_max_temperatures];
    eOas_inertial_t* inertials[eoprotwrap_max_inertials];
    eOas_inertial3_t* inertials3[eoprotwrap_max_inertials3];
    eOas_psc_t*     pscs[eoprotwrap_max_pscs];
    eOas_pos_t*     poss[eoprotwrap_max_poss];
    uint8_t         numofjoints;
    uint8_t         numofmotors;
    uint8_t         numofskins;
    uint8_t         numofstrains;
    uint8_t         numofmaises;
    uint8_t         numoftemperatures;
    uint8_t         numofinertials;
    uint8_t         numofinertials3;
    uint8_t         numofpscs;
    uint8_t         numofposs;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



