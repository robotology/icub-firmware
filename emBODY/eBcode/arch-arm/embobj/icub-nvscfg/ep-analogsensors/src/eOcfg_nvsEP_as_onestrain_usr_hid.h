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
#ifndef _EOCFG_NVSEP_AS_ONESTRAIN_USR_HID_H_
#define _EOCFG_NVSEP_AS_ONESTRAIN_USR_HID_H_


/* @file       eOcfg_nvsEP_as_onestrain_usr_hid.h
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

extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem);

// sxx-init:    xx is the strain number
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Sxx_sconfig(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Sxx_sconfig__mode(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Sxx_sconfig__datarate(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Sxx_sconfig__signaloncefullscale(uint16_t xx, const EOnv* nv);


extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Sxx_sstatus(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Sxx_sstatus__fullscale(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Sxx_sstatus__calibratedvalues(uint16_t xx, const EOnv* nv);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Sxx_sstatus__uncalibratedvalues(uint16_t xx, const EOnv* nv);


// sxx-updt:    xx is the strain number
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig__mode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig__datarate(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig__signaloncefullscale(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);


extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sstatus(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sstatus__fullscale(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sstatus__calibratedvalues(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sstatus__uncalibratedvalues(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);


// // mxx-init:    xx is the mais number
// extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Mxx_mconfig__mode(uint16_t xx, const EOnv* nv);
// extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Mxx_mconfig__datarate(uint16_t xx, const EOnv* nv);
// extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Mxx_mconfig__resolution(uint16_t xx, const EOnv* nv);
// extern void eo_cfg_nvsEP_as_onestrain_usr_hid_INIT_Mxx_mstatus__the15values(uint16_t xx, const EOnv* nv);


// // mxx-updt:    xx is the mais number
// extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Mxx_mconfig__mode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
// extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Mxx_mconfig__datarate(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
// extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Mxx_mconfig__resolution(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);
// extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Mxx_mstatus__the15values(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign);



#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




