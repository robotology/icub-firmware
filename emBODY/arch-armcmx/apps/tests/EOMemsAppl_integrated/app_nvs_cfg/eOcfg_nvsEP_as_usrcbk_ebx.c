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

/* @file       eOcfg_nvsEP_sk_usrcbk_ebx.c
    @brief      This file keeps the user-defined functions used in every ems board ebx for endpoint as
    @author     valentina.gaggero@iit.it
    @date       06/11/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"
#include "hal.h"

#include "EoCommon.h"
#include "EOnv_hid.h"
#include "EOarray.h"

#include "EoSensors.h"
#include "eOcfg_nvsEP_as.h"

//application
#include "EOappTheServicesProvider.h"
//#include "EOappCanServicesProvider.h"
#include "EOicubCanProto_specifications.h"

//#include "EOappTheNVmapRef.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



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
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

//mais-init
// extern void eo_cfg_nvsEP_as_onemais_usr_hid_INIT_Mxx_mstatus__the15values(uint16_t xx, const EOnv* nv)
// {
//     eOsnsr_arrayofupto36bytes_t *maisArray = (eOsnsr_arrayofupto36bytes_t *)nv->loc;    
//     maisArray->head.capacity
// }

//mais-update

extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__mode(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_SB_CMD__SET_TXMODE
    };

    eOsnsr_maismode_t               *maismode = (eOsnsr_maismode_t*)nv->loc;

//     //if application is not in running state the mode is not forwarded
//     if(!eom_appTheSysController_AppIsRunningSt(eom_appTheSysController_GetHandle())) 
//     {
//         return;
//     }

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    if(NULL == appCanSP_ptr)
    {
        return;
    }

    res = eo_appCanSP_GetSensorCanLocation(appCanSP_ptr, n, &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    if(eobrd_mais != boardType)
    {
        return; //somethin is wrong....
    }

    // 1) send cmd
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)maismode);
}



extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__datarate(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
//    eOsnsr_maisresolution_t         maisresolution = snsr_maisresolution_debug;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_SB_CMD__SET_CANDATARATE
    };

    uint8_t               *maisdatarate = (uint8_t*)nv->loc;

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    if(NULL == appCanSP_ptr)
    {
        return;
    }

    res = eo_appCanSP_GetSensorCanLocation(appCanSP_ptr, n, &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    if(eobrd_mais != boardType)
    {
        return; //somethin is wrong....
    }

    // 1) send cmd
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)maisdatarate);
//     #warning VALE--> TEST MAIS!!!!!!!!!!!!!!!!!
//     msgCmd.cmdId = ICUBCANPROTO_POL_SB_CMD__SET_RESOLUTION;
//     eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)&maisresolution);
    
}




extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__resolution(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOresult_t                      res;
    eo_appCanSP_canLocation         canLoc;
    eObrd_types_t                   boardType;
    eo_icubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_SB_CMD__SET_RESOLUTION
    };

    eOsnsr_maisresolution_t         *maisresolution = (uint8_t*)nv->loc;

    EOappCanSP *appCanSP_ptr = eo_appTheSP_GetCanServiceHandle(eo_appTheSP_GetHandle());
    if(NULL == appCanSP_ptr)
    {
        return;
    }

    res = eo_appCanSP_GetSensorCanLocation(appCanSP_ptr, n, &canLoc, &boardType);
    if(eores_OK != res)
    {
        return;
    }

    if(eobrd_mais != boardType)
    {
        return; //somethin is wrong....
    }

    // 1) send cmd
    eo_appCanSP_SendCmd(appCanSP_ptr, &canLoc, msgCmd, (void*)maisresolution);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



