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
#ifndef _EOCFG_ICUBCNAPROTO_SENSORBOARDMSGFUNCTION_H_
#define _EOCFG_ICUBCNAPROTO_SENSORBOARDMSGFUNCTION_H_




/** @file       eOcfg_icubCanProto_sensorBoardMsgFunctions.h
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/

/** @defgroup eo_icubCanProto Configuation of the messages lookup tables.
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------
#include "EOicubCanProto.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

//********************** P A R S E R       POLLING     F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_parser_pol_sb_unexpected_cmd(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_pol_sb_cmd__getFullScales(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);

//********************** F O R M E R       POLLING      F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_former_pol_sb_unexpected_cmd(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setTxMode(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setCanDatarate(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setResolution(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__getFullScales(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);



//********************** P A R S E R       PERIODIC     F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__forceVector(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__torqueVector(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes0to6(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes7to14(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);


extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__uncalibForceVectorDebugmode(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__uncalibTorqueVectorDebugmode(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);


//*********************************** S K I N *************************************************************
extern eOresult_t eo_icubCanProto_former_per_sb_cmd__forceVector(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_per_sb_cmd__torqueVector(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);


//*********************************** S K I N *************************************************************
extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__tactSetup(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__tactSetup2(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_parser_per_sk_cmd__allSkinMsg(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__setBrdCfg(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__setTriangCfg(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame);
extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__accGyroSetup(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame); 

//unused
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes0to3(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes4to7(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes8to11(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes12to15(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);


/** @}            
    end of group eo_icubCanProto 
 **/

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




