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
#ifndef _EOCFG_NVSEP_MC_LOWERARM_USR_HID_H_
#define _EOCFG_NVSEP_MC_LOWERARM_USR_HID_H_


/* @file       eOcfg_nvsEP_mc_lowerarm_usr_hid.h
    @brief      This header file implements hidden interface to ...
    @author     marco.accame@iit.it
    @date       09/06/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"


// - declaration of extern public interface ---------------------------------------------------------------------------


// - #define used with hidden struct ----------------------------------------------------------------------------------



// - definition of the hidden struct implementing the object ----------------------------------------------------------




// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem);

// jxx-init:    xx is the joint number
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__pidposition(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__pidvelocity(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__pidtorque(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__impedance(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__minpositionofjoint(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__maxpositionofjoint(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__velocitysetpointtimeout(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__holder01FFU00(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__motionmonitormode(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__encoderconversionfactor(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__encoderconversionoffset(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__des02FORjstatuschamaleon04(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__holder01FFU01(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__holder01FFU02(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__holder02FFU03(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jconfig__holder02FFU04(uint16_t xx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jstatus(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jstatus__basic(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jstatus__ofpid(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jstatus__chamaleon04(uint16_t xx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jinputs__externallymeasuredtorque(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jinputs__holder02FFU01(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jinputs__holder04FFU02(uint16_t xx, const EOnv* nv);

extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jcmmnds__calibration(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jcmmnds__setpoint(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jcmmnds__stoptrajectory(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jcmmnds__controlmode(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jcmmnds__holder01FFU02(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Jxx_jcmmnds__holder01FFU03(uint16_t xx, const EOnv* nv);


// jxx-updt:    xx is the joint number
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__pidposition(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__pidvelocity(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__pidtorque(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__impedance(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__minpositionofjoint(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__maxpositionofjoint(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__holder01FFU00(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__motionmonitormode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__encoderconversionfactor(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__encoderconversionoffset(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__des02FORjstatuschamaleon04(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__holder01FFU01(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__holder01FFU02(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__holder02FFU03(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jconfig__holder02FFU04(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jstatus(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jstatus__basic(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jstatus__ofpid(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jstatus__chamaleon04(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jinputs__externallymeasuredtorque(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jinputs__holder02FFU01(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jinputs__holder04FFU02(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jcmmnds__calibration(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jcmmnds__setpoint(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jcmmnds__stoptrajectory(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jcmmnds__controlmode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jcmmnds__holder01FFU02(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Jxx_jcmmnds__holder01FFU03(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

// mxx-init:    xx is the motor number
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Mxx_mconfig(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Mxx_mconfig__pidcurrent(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Mxx_mconfig__maxvelocityofmotor(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Mxx_mconfig__maxcurrentofmotor(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Mxx_mconfig__des02FORmstatuschamaleon04(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Mxx_mstatus(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Mxx_mstatus__basic(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Mxx_mstatus__chamaleon04(uint16_t xx, const EOnv* nv);

// mxx-updt:    xx is the motor number
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Mxx_mconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Mxx_mconfig__pidcurrent(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Mxx_mconfig__maxvelocityofmotor(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Mxx_mconfig__maxcurrentofmotor(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Mxx_mconfig__des02FORmstatuschamaleon04(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Mxx_mstatus(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Mxx_mstatus__basic(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Mxx_mstatus__chamaleon04(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);

// cxx-init:    xx is 0
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Cxx_cconfig(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Cxx_cconfig__durationofctrlloop(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Cxx_cstatus(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Cxx_cstatus__alljomoinitted(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_INIT_Cxx_ccmmnds__go2stateofcontroller(uint16_t xx, const EOnv* nv);

// c00-updt:    xx is 0
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Cxx_cconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Cxx_cconfig__durationofctrlloop(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Cxx_cstatus(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Cxx_cstatus__alljomoinitted(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_lowerarm_usr_hid_UPDT_Cxx_ccmmnds__go2stateofcontroller(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




