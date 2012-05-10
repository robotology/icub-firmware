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


/* @file       eOcfg_icubCanProto_sensorBoardMsgfunction.c
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "EOcommon.h"
#include "EOicubCanProto.h"
#include "EOicubCanProto_specifications.h"
#include "EOicubCanProto_hid.h"

#include "EoMotionControl.h"
#include "EOnv_hid.h"

#include "EOemsCanNetworkTopology.h"

#include "hal_debugPin.h"
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "eOcfg_icubCanProto_sensorBoardMsgFunctions.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define EMS_CAN_ADDR                                    0
#define ICUBCANPROTO_POL_MB_ID_DEFAULT                  ((ICUBCANPROTO_CLASS_POLLING_MOTORBOARD << 8) | (0x0<<4))
#define ICUBCANPROTO_POL_MB_CREATE_ID(destBoardAddr)    ((ICUBCANPROTO_CLASS_POLLING_MOTORBOARD << 8) | (EMS_CAN_ADDR<<4) | (destBoardAddr&0xF))



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

//********************** P A R S E R       POLLING     F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_parser_pol_sb_unexpected_cmd(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_sb_unexpected_cmd(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setTxMode(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setCanDatarate(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_sb_cmd__getFullScales(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__getFullScales(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


//********************** P A R S E R       PERIODIC     F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__forceVector(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__torqueVector(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes0to6(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes7to14(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


//unused
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes0to3(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}
//
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes4to7(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}
//
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes8to11(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}
//
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes12to15(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



