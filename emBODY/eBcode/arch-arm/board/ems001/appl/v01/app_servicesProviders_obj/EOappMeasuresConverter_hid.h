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
#ifndef _EOAPPMEASURESCONVERTER_HID_H_
#define _EOAPPMEASURESCONVERTER_HID_H_


// - doxy -------------------------------------------------------------------------------------------------------------
/* @file       EOappMeasuresConverter.h
    @brief     This file provides hidden interface to appMeasuresConverter obj.
    @author    valentina.gaggero@iit.it
    @date      08/24/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
//abstlayaer
#include "eOcommon.h"
#include "eOcfg_nvsEP_mc.h"


// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOappMeasuresConverter.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


//this is the default value of encoder's proportional ratio. (encoder about joint)
enum {eo_appMeasConv_hid_jntEncoderConvFactor_defaultValue = 1};

//this is the default value of encoder's zero. (encoder about joint)
enum {eo_appMeasConv_hid_jntEncoderConvOffset_defaultValue = 0};

typedef struct
{
    eOappMeasConv_encConversionFactor_t       factor;  /**< proportionalRatio between idg and encoder's ticks. */
    eOappMeasConv_encConversionOffset_t      offset;  /**< offset to move encoder's zero out form joint's limit range */
} eOappMeasConv_encoderConversionData_t;

struct EOappMeasConv_hid
{
    eOappMeasConv_cfg_t                         cfg;
    struct
    {
        uint8_t                                 jointVelocityShift;
        uint8_t                                 jointVelocityEstimationShift;
        uint8_t                                 jointAccEstimationShift;
    } shiftvalues;
    uint16_t                                    totalnumofjoint;
    eOappMeasConv_encoderConversionData_t       *jntEncConvDataList;  /**< forn each joint, encoder's propRatio and zero is differet from other 
                                                                                 So this list contains for each joint, managed by a given EMS, encoder's ratio and zero */ 
    //One day here there will be rotor ecoder info and torque info.
}; 



// - declaration of extern hidden functions ---------------------------------------------------------------------------
inline extern eOappMeasConv_encConversionOffset_t eo_appMeasConv_hid_GetEncConv_offset(EOappMeasConv *p, eOmc_jointId_t jId)
{
    return(p->jntEncConvDataList[jId].offset);
}


inline extern eOappMeasConv_encConversionFactor_t eo_appMeasConv_hid_GetEncConv_factor(EOappMeasConv *p, eOmc_jointId_t jId)
{
    return(p->jntEncConvDataList[jId].factor);
}

inline extern uint8_t eo_appMeasConv_hid_GetVelShift(EOappMeasConv *p, eOmc_jointId_t jId)
{
    return(p->shiftvalues.jointVelocityShift);
}


inline extern uint8_t eo_appMeasConv_hid_GetVelEstimShift(EOappMeasConv *p, eOmc_jointId_t jId)
{
    return(p->shiftvalues.jointVelocityEstimationShift);
}

inline extern uint8_t eo_appMeasConv_hid_GetAccEstimShift(EOappMeasConv *p, eOmc_jointId_t jId)
{
    return(p->shiftvalues.jointAccEstimationShift);
}


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



