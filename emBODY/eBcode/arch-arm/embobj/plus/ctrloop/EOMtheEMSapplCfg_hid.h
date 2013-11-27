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
#ifndef _EOMTHEEMSAPPLCFG_HID_H_
#define _EOMTHEEMSAPPLCFG_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       EOMtheEMSapplCfg_hid.h
    @brief      This header file implements hidden interface to the EMS application configuration singleton.
    @author     marco.accame@iit.it
    @date       06/25/2009
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

#include "EOtheBOARDtransceiver.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheEMSapplCfg.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------


// - derived #define -------------------------------------------------------------------------------------------------

#if 0



#if     defined(EOMTHEEMSAPPLCFG_USE_EB2) || defined(EOMTHEEMSAPPLCFG_USE_EB4)
    #define EOMTHEEMSAPPLCFG_EBX_hasSKIN    eobool_true
    #define EOMTHEEMSAPPLCFG_EBX_hasMC4     eobool_true
    #define EOMTHEEMSAPPLCFG_EBX_has2FOC    eobool_false
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB1) || defined(EOMTHEEMSAPPLCFG_USE_EB3) || defined(EOMTHEEMSAPPLCFG_USE_EB5) || defined(EOMTHEEMSAPPLCFG_USE_EB6) || defined(EOMTHEEMSAPPLCFG_USE_EB7) || defined(EOMTHEEMSAPPLCFG_USE_EB8) || defined(EOMTHEEMSAPPLCFG_USE_EB9)
    #define EOMTHEEMSAPPLCFG_EBX_hasSKIN    eobool_false
    #define EOMTHEEMSAPPLCFG_EBX_hasMC4     eobool_false
    #define EOMTHEEMSAPPLCFG_EBX_has2FOC    eobool_true
#else
    #error --> you must define an EBx
#endif



//encoders configuration
#define ENC_ENA     1   /* If encoder is connected*/
#define ENC_DISA    0   /* If encoder is NOT connected*/
#if     defined(EOMTHEEMSAPPLCFG_USE_EB1) || defined(EOMTHEEMSAPPLCFG_USE_EB3)  || defined(EOMTHEEMSAPPLCFG_USE_EB6)  || defined(EOMTHEEMSAPPLCFG_USE_EB8)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (ENC_ENA << eOeOappEncReader_encoder0)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder1)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder2)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder3)  |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder4) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder5)     \
                                                    )
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB7) || defined(EOMTHEEMSAPPLCFG_USE_EB9)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (ENC_ENA << eOeOappEncReader_encoder0)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder1)  |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder2) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder3) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder4) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder5)     \
                                                    )
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB5)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (ENC_ENA << eOeOappEncReader_encoder0)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder1)  |   \
                                                        (ENC_ENA << eOeOappEncReader_encoder2)  |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder3) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder4) |   \
                                                        (ENC_DISA << eOeOappEncReader_encoder5)     \
                                                    )
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB2) || defined(EOMTHEEMSAPPLCFG_USE_EB4)
        #define EOMTHEEMSAPPLCFG_EBX_encodersMASK   (   (ENC_DISA << eOeOappEncReader_encoder0) |  \
                                                        (ENC_DISA << eOeOappEncReader_encoder1) |  \
                                                        (ENC_DISA << eOeOappEncReader_encoder2) |  \
                                                        (ENC_DISA << eOeOappEncReader_encoder3) |  \
                                                        (ENC_DISA << eOeOappEncReader_encoder4) |  \
                                                        (ENC_DISA << eOeOappEncReader_encoder5)    \
                                                    )

#else
    #error --> you must define an EBx
#endif


//ems controller configuration
#if     defined(EOMTHEEMSAPPLCFG_USE_EB1) || defined(EOMTHEEMSAPPLCFG_USE_EB3)
        #define EOMTHEEMSAPPLCFG_EBX_emscontroller_EMSTYPE          EMS_SHOULDER
        
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB6) || defined(EOMTHEEMSAPPLCFG_USE_EB8)
        #define EOMTHEEMSAPPLCFG_EBX_emscontroller_EMSTYPE          EMS_UPPERLEG
        
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB7) || defined(EOMTHEEMSAPPLCFG_USE_EB9)
        #define EOMTHEEMSAPPLCFG_EBX_emscontroller_EMSTYPE          EMS_ANKLE
        
#elif   defined(EOMTHEEMSAPPLCFG_USE_EB5)
        #define EOMTHEEMSAPPLCFG_EBX_emscontroller_EMSTYPE          EMS_WAIST

#elif   defined(EOMTHEEMSAPPLCFG_USE_EB2) || defined(EOMTHEEMSAPPLCFG_USE_EB4)
        #define EOMTHEEMSAPPLCFG_EBX_emscontroller_EMSTYPE          EMS_GENERIC


#else
    #error --> you must define an EBx
#endif



#endif

// - definition of the hidden struct implementing the object ----------------------------------------------------------
// empty-section






// - declaration of extern hidden functions ---------------------------------------------------------------------------

extern void eom_emsapplcfg_hid_userdef_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

