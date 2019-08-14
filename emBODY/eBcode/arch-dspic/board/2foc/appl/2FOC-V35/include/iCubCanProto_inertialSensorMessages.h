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

#ifndef _ICUBCANPROTO_INERTIALSENSORMESSAGES_H_
#define _ICUBCANPROTO_INERTIALSENSORMESSAGES_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       iCubCanProto_inertialSensorMessages.h
    @brief      This header file gives the definitions of the iCub CAN protocols commands relating to inertial sensor messages class
    @author     marco.accame@iit.it
    @date       19/08/2015
    @ingroup    iCubCanProtocol
**/

/** @ingroup    iCubCanProtocol
    @{
 **/




// - external dependencies --------------------------------------------------------------------------------------------
// empty-section


// - public #define  --------------------------------------------------------------------------------------------------

/***************************************************************************/
// command messages of class ICUBCANPROTO_CLASS_POLLING_INERTIALSENSOR 
/***************************************************************************/

// none so far.

#define ICUBCANPROTO_POL_IS_CMD__NONE                               0x0

#define ICUBCANPROTO_POL_IS_CMD_MAXNUM                              (1)

/***************************************************************************/
// command messages of class ICUBCANPROTO_CLASS_PERIODIC_INERTIALSENSOR
/***************************************************************************/

#define ICUBCANPROTO_PER_IS_MSG__DIGITAL_GYROSCOPE                  0x00    
#define ICUBCANPROTO_PER_IS_MSG__DIGITAL_ACCELEROMETER              0x01
#define ICUBCANPROTO_PER_IS_MSG__ANALOG_ACCELEROMETER               0x02

#define ICUBCANPROTO_PER_IS_MSG_MAXNUM                              (3)



/** @} **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------







