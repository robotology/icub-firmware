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

#ifndef _EOTHEInertial_HID_H_
#define _EOTHEInertial_HID_H_




// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOtheCANprotocol.h"


// - declaration of extern public interface ---------------------------------------------------------------------------

#include "EOtheInertial.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------

typedef enum 
{
    gyro_none       = 0,
    gyro_ext        = 2
} eOinertial_gyro_type_t;

typedef enum 
{
    accel_none      = 0,
    accel_int       = 1,
    accel_ext       = 2,
    accel_intext    = 3
} eOinertial_accel_type_t;


struct EOtheInertial_hid
{
    eObool_t                    initted;
    eObool_t                    thereisinertial;
    uint8_t                     protindex;
    eOas_inertialidentifier_t   id;
    uint8_t                     supportedsensors;
    eOinertial_accel_type_t     acceltype;
    eOinertial_gyro_type_t      gyrotype;
    eObool_t                    istransmitting;
    eOprotID32_t                id32;
    eOas_inertial_config_t      config;
    eOcanprot_command_t         command;
}; 


// - declaration of extern hidden functions ---------------------------------------------------------------------------
// empty section


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



