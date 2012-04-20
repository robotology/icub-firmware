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
#ifndef _EOCFG_NVSEP_MC_LEG_USR_HID_H_
#define _EOCFG_NVSEP_MC_LEG_USR_HID_H_


/* @file       eOcfg_nvsEP_mc_leg_usr_hid.h
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

extern void eo_cfg_nvsEP_mc_leg_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem);

// jxx-init:
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__pidposition(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__pidvelocity(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__pidtorque(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__minpositionofjoint(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__maxpositionofjoint(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__velocitysetpointtimeout(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx_jconfig__upto02descrforchameleon02(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx__jstatus(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx__calibrator(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx__setpoint(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx__controlmode(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx__signalwhenmotionisdone(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx__motionisdone(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Jxx__externalvalueoftorque(uint16_t n, const EOnv* nv);
// jxx-updt: all the nvs
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__pidposition(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__pidvelocity(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__pidtorque(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__minpositionofjoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__maxpositionofjoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__velocitysetpointtimeout(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx_jconfig__upto02descrforchameleon02(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx__jstatus(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx__calibrator(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx__setpoint(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx__controlmode(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx__signalwhenmotionisdone(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx__motionisdone(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Jxx__externalvalueoftorque(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
// mxx-init:
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mconfig(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mconfig__pidcurrent(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mconfig__maxvelocityofmotor(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mconfig__maxcurrentofmotor(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx_mconfig__upto02descrforchameleon06(uint16_t n, const EOnv* nv);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_INIT_Mxx__mstatus(uint16_t n, const EOnv* nv);
// mxx-updt: all the nvs
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mconfig(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mconfig__pidcurrent(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mconfig__maxvelocityofmotor(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mconfig__maxcurrentofmotor(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx_mconfig__upto02descrforchameleon06(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_mc_leg_usr_hid_UPDT_Mxx__mstatus(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign);


#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




