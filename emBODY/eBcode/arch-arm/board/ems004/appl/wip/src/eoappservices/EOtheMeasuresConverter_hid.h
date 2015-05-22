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
#ifndef _EOTHEMEASURESCONVERTER_HID_H_
#define _EOTHEMEASURESCONVERTER_HID_H_


// - doxy -------------------------------------------------------------------------------------------------------------
/* @file       EOtheMeasuresConverter.h
    @brief     This file provides hidden interface to appMeasuresConverter obj.
    @author    valentina.gaggero@iit.it
    @date      08/24/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
//abstlayaer
#include "eOcommon.h"



// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOtheMeasuresConverter.h"


// - definition of the hidden struct implementing the object ----------------------------------------------------------


//this is the default value of encoder's proportional ratio. (encoder about joint)
enum {eo_measconv_hid_jntEncoderConvFactor_defaultValue = 1};

//this is the default value of encoder's zero. (encoder about joint)
enum {eo_measconv_hid_jntEncoderConvOffset_defaultValue = 0};

typedef struct
{
    eOmeasconv_encConversionFactor_t     factor;  /**< proportionalRatio between idg and encoder's ticks. */
    eOmeasconv_encConversionOffset_t     offset;  /**< offset to move encoder's zero out form joint's limit range */
} eOmeasconv_encoderConversionData_t;

struct EOtheMeasuresConverter_hid
{
    eOmeasconv_cfg_t                         cfg;
//    struct
//    {
//        uint8_t                                 jointVelocityShift;
//        uint8_t                                 jointVelocityEstimationShift;
//        uint8_t                                 jointAccEstimationShift;
//    } shiftvalues;
    uint8_t                                     jointVelocityShift;
    uint8_t                                     jointVelocityEstimationShift;
    uint8_t                                     jointAccEstimationShift;
    uint8_t                                     totalnumofjoint;
    eOmeasconv_encoderConversionData_t       *jntEncConvDataList;  /**< forn each joint, encoder's propRatio and zero is differet from other 
                                                                                 So this list contains for each joint, managed by a given EMS, encoder's ratio and zero */ 
    //One day here there will be rotor ecoder info and torque info.
}; 



// - declaration of extern hidden functions ---------------------------------------------------------------------------

inline extern eOmeasconv_encConversionOffset_t eo_measconv_hid_GetEncConv_offset(EOtheMeasuresConverter *p, eOmc_jointId_t jId)
{
    return(p->jntEncConvDataList[jId].offset);
}


inline extern eOmeasconv_encConversionFactor_t eo_measconv_hid_GetEncConv_factor(EOtheMeasuresConverter *p, eOmc_jointId_t jId)
{
    return(p->jntEncConvDataList[jId].factor);
}

inline extern uint8_t eo_measconv_hid_GetVelShift(EOtheMeasuresConverter *p, eOmc_jointId_t jId)
{
    return(p->jointVelocityShift);
}


inline extern uint8_t eo_measconv_hid_GetVelEstimShift(EOtheMeasuresConverter *p, eOmc_jointId_t jId)
{
    return(p->jointVelocityEstimationShift);
}

inline extern uint8_t eo_measconv_hid_GetAccEstimShift(EOtheMeasuresConverter *p, eOmc_jointId_t jId)
{
    return(p->jointAccEstimationShift);
}


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



