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
extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidposition(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidvelocity(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__pidtorque(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__impedance(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__minpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__maxpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__velocitysetpointtimeout(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__controlmode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__motionmonitormode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__des02FORjstatuschamaleon04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU01(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder01FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU03(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jconfig__holder02FFU04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__basic(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__ofpid(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jstatus__chamaleon04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__externallymeasuredtorque(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder02FFU01(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jinputs__holder04FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__calibration(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__setpoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__stoptrajectory(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__cmdcontrolxmode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Jxx_jcmmnds__holder01FFU03(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv);

// jxx-updt:

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidposition(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidvelocity(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__pidtorque(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__impedance(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__minpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__maxpositionofjoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__controlmode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__motionmonitormode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__des02FORjstatuschamaleon04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU01(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder01FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU03(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jconfig__holder02FFU04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__basic(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__ofpid(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jstatus__chamaleon04(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__externallymeasuredtorque(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder02FFU01(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jinputs__holder04FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__calibration(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__setpoint(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__stoptrajectory(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__cmdcontrolxmode(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU02(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Jxx_jcmmnds__holder01FFU03(eOcfg_nvsEP_mc_jointNumber_t jxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

// mxx-init:

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__pidcurrent(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxvelocityofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__maxcurrentofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mconfig__des02FORmstatuschamaleon04(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__basic(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Mxx_mstatus__chamaleon04(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv);

// mxx-updt:

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__pidcurrent(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxvelocityofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__maxcurrentofmotor(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mconfig__des02FORmstatuschamaleon04(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__basic(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Mxx_mstatus__chamaleon04(eOcfg_nvsEP_mc_motorNumber_t mxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);



// c00-init:    xx is 0
extern void eo_cfg_nvsEP_mc_hid_INIT_Cxx_cconfig(uint16_t xx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Cxx_cconfig__durationofctrlloop(uint16_t xx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Cxx_cstatus(uint16_t xx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Cxx_cstatus__alljomoinitted(uint16_t xx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_hid_INIT_Cxx_ccmmnds__go2stateofcontroller(uint16_t xx, const EOnv* nv);

// c00-updt:    xx is 0
extern void eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cconfig__durationofctrlloop(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cstatus(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Cxx_cstatus__alljomoinitted(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_hid_UPDT_Cxx_ccmmnds__go2stateofcontroller(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

/** @}            
    end of group eo_h2uilsdede2345er  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


