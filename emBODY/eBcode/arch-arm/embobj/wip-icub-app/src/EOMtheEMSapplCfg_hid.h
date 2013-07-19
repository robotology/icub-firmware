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
	
	
#include "EoProtocol.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOMtheEMSapplCfg.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------

#include "EOMtheEMSapplCfg_cfg.h"




// - derived #define -------------------------------------------------------------------------------------------------

#if     (1 == EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
        #define     EOMTHEEMSAPPLCFG_USE_EB1
        #define     EOMTHEEMSAPPLCFG_NAME                   "EOMemsApplEB1"
        #include    "eOprot_b01.h"
        #define     EOMTHEEMSAPPLCFG_nvsetdevcfg            &eoprot_b01_nvsetDEVcfg
#elif   (2 == EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
        #define     EOMTHEEMSAPPLCFG_USE_EB2
        #define     EOMTHEEMSAPPLCFG_NAME                   "EOMemsApplEB2"
        #include    "eOprot_b02.h"
        #define     EOMTHEEMSAPPLCFG_nvsetdevcfg            &eoprot_b02_nvsetDEVcfg        
#elif   (3 == EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
        #define     EOMTHEEMSAPPLCFG_USE_EB3
        #define     EOMTHEEMSAPPLCFG_NAME                   "EOMemsApplEB3"
        #include    "eOprot_b03.h"
        #define     EOMTHEEMSAPPLCFG_nvsetdevcfg            &eoprot_b03_nvsetDEVcfg         
#elif   (4 == EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
        #define     EOMTHEEMSAPPLCFG_USE_EB4
        #define     EOMTHEEMSAPPLCFG_NAME                   "EOMemsApplEB4"
        #include    "eOprot_b04.h"
        #define     EOMTHEEMSAPPLCFG_nvsetdevcfg            &eoprot_b04_nvsetDEVcfg  
#elif   (5 == EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
        #define     EOMTHEEMSAPPLCFG_USE_EB5
        #define     EOMTHEEMSAPPLCFG_NAME                   "EOMemsApplEB5"
        #include    "eOprot_b05.h"
        #define     EOMTHEEMSAPPLCFG_nvsetdevcfg            &eoprot_b05_nvsetDEVcfg   
#elif   (6 == EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
        #define     EOMTHEEMSAPPLCFG_USE_EB6
        #define     EOMTHEEMSAPPLCFG_NAME                   "EOMemsApplEB6"
        #include    "eOprot_b06.h"
        #define     EOMTHEEMSAPPLCFG_nvsetdevcfg            &eoprot_b06_nvsetDEVcfg   
#elif   (7 == EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
        #define     EOMTHEEMSAPPLCFG_USE_EB7
        #define     EOMTHEEMSAPPLCFG_NAME                   "EOMemsApplEB7"
        #include    "eOprot_b07.h"
        #define     EOMTHEEMSAPPLCFG_nvsetdevcfg            &eoprot_b07_nvsetDEVcfg   
#elif   (8 == EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
        #define     EOMTHEEMSAPPLCFG_USE_EB8
        #define     EOMTHEEMSAPPLCFG_NAME                   "EOMemsApplEB8"
        #include    "eOprot_b08.h"
        #define     EOMTHEEMSAPPLCFG_nvsetdevcfg            &eoprot_b08_nvsetDEVcfg   
#elif   (9 == EOMTHEEMSAPPLCFG_ID_OF_EMSBOARD)
        #define     EOMTHEEMSAPPLCFG_USE_EB9
        #define     EOMTHEEMSAPPLCFG_NAME                   "EOMemsApplEB9"
        #include    "eOprot_b09.h"
        #define     EOMTHEEMSAPPLCFG_nvsetdevcfg            &eoprot_b09_nvsetDEVcfg   
#endif



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

//definition on num of endpoint
#if     defined(EOMTHEEMSAPPLCFG_USE_EB1)
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_mc    eoprot_endpoint_motioncontrol       
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_as    eoprot_endpoint_analogsensors
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_sk    eoprot_endpoint_none 
        
#elif     defined(EOMTHEEMSAPPLCFG_USE_EB2)
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_mc    eoprot_endpoint_motioncontrol       
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_as    eoprot_endpoint_analogsensors 
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_sk    eoprot_endpoint_skin 
        
#elif     defined(EOMTHEEMSAPPLCFG_USE_EB3)
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_mc    eoprot_endpoint_mc_rightupperarm       
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_as    eoprot_endpoint_as_rightupperarm 
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_sk    eoprot_endpoint_none 

#elif     defined(EOMTHEEMSAPPLCFG_USE_EB4)
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_mc    eoprot_endpoint_mc_rightlowerarm       
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_as    eoprot_endpoint_as_rightlowerarm 
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_sk    eoprot_endpoint_sk_emsboard_rightlowerarm 

#elif     defined(EOMTHEEMSAPPLCFG_USE_EB5)
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_mc    eoprot_endpoint_mc_torso       
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_as    eoprot_endpoint_none 
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_sk    eoprot_endpoint_none 

#elif     defined(EOMTHEEMSAPPLCFG_USE_EB6)
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_mc    eoprot_endpoint_mc_leftupperleg       
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_as    eoprot_endpoint_as_leftupperleg 
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_sk    eoprot_endpoint_none 
        
#elif     defined(EOMTHEEMSAPPLCFG_USE_EB7)
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_mc    eoprot_endpoint_mc_leftlowerleg       
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_as    eoprot_endpoint_none 
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_sk    eoprot_endpoint_none 

#elif     defined(EOMTHEEMSAPPLCFG_USE_EB8)
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_mc    eoprot_endpoint_mc_rightupperleg       
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_as    eoprot_endpoint_as_rightupperleg 
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_sk    eoprot_endpoint_none 

#elif     defined(EOMTHEEMSAPPLCFG_USE_EB9)
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_mc    eoprot_endpoint_mc_rightlowerleg       
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_as    eoprot_endpoint_none 
        #define EOMTHEEMSAPPLCFG_EBX_endpoint_sk    eoprot_endpoint_none 
#else
    #error --> you must define an EBx
    //unless you have already added function to get mc, as and sk endpoint number and use them to initialise appTheNVmapRef obj!!!
    //Remember: remove from include eOcfg_nvsEP_sk.h 
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

