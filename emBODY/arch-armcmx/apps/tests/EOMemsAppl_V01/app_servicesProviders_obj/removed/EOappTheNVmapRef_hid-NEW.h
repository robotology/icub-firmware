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
#ifndef _EOAPPTHEMCNVMAP_HID_H_
#define _EOAPPTHEMCNVMAP_HID_H_


/* @file       EOappTheMCNVmap_hid.h
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       04/17/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOMotionControl.h"
#include "eOcfg_nvsEP_sk.h"
#include "eOcfg_nvsEP_mc.h"
#include "eOcfg_nvsEP_as.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOappTheNVmapRef-NEW.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
/*NOTE: following definitions are not used in EOappNVmapRef source code, but they are usefull to verify
        nv indexes exported in EOappTheNVmapRef.h are comaptible with ones in eOcfg_nvsEP_mc.h*/

/**************************  J O I N T  **************************/
#define EOappNVmapRef_jointNVindexoffset_jconfig                                     0
#define EOappNVmapRef_jointNVindexoffset_jstatus                                     14
#define EOappNVmapRef_jointNVindexoffset_jinputs__externallymeasuredtorque           17
#define EOappNVmapRef_jointNVindexoffset_jcmmnds__calibration                        19
#define EOappNVmapRef_jointNVindexoffset_jcmmnds__setpoint                           19
#define EOappNVmapRef_jointNVindexoffset_jcmmnds__stoptrajectory                     19

//here verify if eoappNVmapRef-index is equal to nvindex-offset
EO_VERIFYproposition(EOappNVmapRef_NVindexjconfig_match, (jointNVindex_jconfig == (eOappNVmapRef_jointNVindex_jconfig + EOappNVmapRef_jointNVindexoffset_jconfig) ));
EO_VERIFYproposition(EOappNVmapRef_NVindexjstaus_match, (jointNVindex_jstatus == (eOappNVmapRef_jointNVindex_jstaus + EOappNVmapRef_jointNVindexoffset_jstatus) ));
EO_VERIFYproposition(EOappNVmapRef_NVindexjinputs__ext_match, (jointNVindex_jinputs__externallymeasuredtorque == (eOappNVmapRef_jointNVindex_jinputs__externallymeasuredtorque + EOappNVmapRef_jointNVindexoffset_jinputs__externallymeasuredtorque) ));
EO_VERIFYproposition(EOappNVmapRef_NVindexjcmmnds__calibration_match, (jointNVindex_jcmmnds__calibration == (eOappNVmapRef_jointNVindex_jcmmnds__calibration + EOappNVmapRef_jointNVindexoffset_jcmmnds__calibration) ));
EO_VERIFYproposition(EOappNVmapRef_NVindexjcmmnds__setpoint_match, (jointNVindex_jcmmnds__setpoint == (eOappNVmapRef_jointNVindex_jcmmnds__setpoint + EOappNVmapRef_jointNVindexoffset_jcmmnds__setpoint) ));
EO_VERIFYproposition(EOappNVmapRef_NVindexjcmmnds__stoptrajectory_match, (jointNVindex_jcmmnds__stoptrajectory == (eOappNVmapRef_jointNVindex_jcmmnds__stoptrajectory + EOappNVmapRef_jointNVindexoffset_jcmmnds__stoptrajectory) ));



/**************************  M O T O R  **************************/
#define EOappNVmapRef_motorNVindexoffset_mconfig                                     0
#define EOappNVmapRef_motorNVindexoffset_mstatus                                     4


//here verify if eoappNVmapRef-index is equal to nvindex-offset
EO_VERIFYproposition(EOappNVmapRef_NVindexmconfig_match, (motorNVindex_mconfig == (eOappNVmapRef_motorNVindex_mconfig + EOappNVmapRef_motorNVindexoffset_mconfig) ));
EO_VERIFYproposition(EOappNVmapRef_NVindexmstaus_match, (motorNVindex_mstatus == (eOappNVmapRef_motorNVindex_mstatus + EOappNVmapRef_motorNVindexoffset_mstatus) ));


// - definition of the hidden struct implementing the object ----------------------------------------------------------

/** @struct     EONtheSystem_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/  

struct EOappTheNVmapRef_hid 
{
    eOboolvalues_t          isInited; 
    eOappTheNVmapRef_cfg_t  cfg;
    uint16_t                jointMaxNumber;
    void***                 jointsList;
}; 

// - declaration of hidden constant variables ---------------------------------------------------------------------------

// const uint8_t eo_appTheNVmapRef_jointIndexMap[eOappNVmapRef_jointNVindex_TOTALnumber] = 
// {
//     EOappNVmapRef_jointNVindexoffset_jconfig,
//     EOappNVmapRef_jointNVindexoffset_jstatus,
//     EOappNVmapRef_jointNVindexoffset_jinputs__externallymeasuredtorque,
//     EOappNVmapRef_jointNVindexoffset_jcmmnds__calibration,
//     EOappNVmapRef_jointNVindexoffset_jcmmnds__setpoint,
//     EOappNVmapRef_jointNVindexoffset_jcmmnds__stoptrajectory
// };


const eOcfg_nvsEP_mc_jointNVindex_t eo_appTheNVmapRef_jointIndexMap[eOappNVmapRef_jointNVindex_TOTALnumber] = 
{
    jointNVindex_jconfig,
    jointNVindex_jstatus,
    jointNVindex_jinputs__externallymeasuredtorque,
    jointNVindex_jcmmnds__calibration,
    jointNVindex_jcmmnds__setpoint,
    jointNVindex_jcmmnds__stoptrajectory
};


const eOcfg_nvsEP_mc_motorNVindex_t eo_appTheNVmapRef_motorIndexMap[eOappNVmapRef_motorNVindex_TOTALnumber] = 
{
    motorNVindex_mconfig,
    motorNVindex_mstatus
};

// - declaration of extern hidden functions ---------------------------------------------------------------------------

EO_extern_inline eOcfg_nvsEP_mc_jointNVindex_t eo_appTheNVmapRef_GetMCjointNVindex(eOappNVmapRef_jointNVindex_t index)
{
    return(eo_appTheNVmapRef_jointIndexMap[index]);
}


EO_extern_inline eOcfg_nvsEP_mc_motorNVindex_t eo_appTheNVmapRef_GetMCmotorNVindex(eOappNVmapRef_motorNVindex_t index)
{
    return(eo_appTheNVmapRef_motorIndexMap[index]);
}


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

