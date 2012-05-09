/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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
#ifndef _EOCFG_NVSEP_MC_HID_H_
#define _EOCFG_NVSEP_MC_HID_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       eOcfg_nvsEP_mc_hid.h
	@brief      This header file gives the ...
	@author     marco.accame@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_h2uilsdede2345er Configuration of the bodypart
    In here there are constants for the max number of joints and motors in an endpoint 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

//#include "EoCommon.h"
//#include "EOnv.h"
//#include "EoMotionControl.h"



#include "eOcfg_nvsEP_mc.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
      


// - declaration of public user-defined types ------------------------------------------------------------------------- 
// empty-section

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------

extern void eo_cfg_nvsEP_mc_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem);

// jxx-init:
extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidposition(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidvelocity(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidtorque(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__impedance(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__minpositionofjoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__maxpositionofjoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__velocitysetpointtimeout(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__controlmode(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__motionmonitormode(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__des02FORjstatuschamaleon04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU03(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__basic(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__ofpid(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__chamaleon04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__externallymeasuredtorque(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder02FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder04FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__calibration(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__setpoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__stoptrajectory(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU03(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

// jxx-updt:

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidposition(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidvelocity(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidtorque(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__impedance(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__minpositionofjoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__maxpositionofjoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__controlmode(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__motionmonitormode(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__des02FORjstatuschamaleon04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU03(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__basic(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__ofpid(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__chamaleon04(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__externallymeasuredtorque(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder02FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder04FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__setpoint(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__stoptrajectory(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU01(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU02(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU03(eo_cfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

// mxx-init:

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__pidcurrent(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxvelocityofmotor(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxcurrentofmotor(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__des02FORmstatuschamaleon04(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__basic(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__chamaleon04(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

// mxx-updt:

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__pidcurrent(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxvelocityofmotor(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxcurrentofmotor(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__des02FORmstatuschamaleon04(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__basic(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__chamaleon04(eo_cfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);


/** @}            
    end of group eo_h2uilsdede2345er  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


