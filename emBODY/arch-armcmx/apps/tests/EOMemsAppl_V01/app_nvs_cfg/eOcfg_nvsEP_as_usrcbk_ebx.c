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
#include "EOtheEMSapplBody.h"
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
//     maisArray->head.capacity = 
//     capacity;
//     uint8_t         itemsize;
//     uint8_t         size;
// }

//mais-update

extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOsnsr_mais_status_t                *sstatus_ptr;
    eOicubCanProto_msgDestination_t     msgdest;
    eOappTheDB_sensorCanLocation_t      canLoc;
    eOicubCanProto_msgCommand_t         msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) 0
    };

    eOsnsr_mais_config_t               *maiscfg = (eOsnsr_mais_config_t*)nv->loc;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eo_appTheDB_GetSnsrMaisCanLocation(eo_appTheDB_GetHandle(), (eOsnsr_maisId_t)n, &canLoc);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canLoc.addr); 
    msgCmd.cmdId =  ICUBCANPROTO_POL_SB_CMD__SET_TXMODE;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(maiscfg->mode));
    
    msgCmd.cmdId =  ICUBCANPROTO_POL_SB_CMD__SET_CANDATARATE;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(maiscfg->datarate));

    msgCmd.cmdId =  ICUBCANPROTO_POL_SB_CMD__SET_RESOLUTION;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(maiscfg->resolution));
    
    //set head of array "the15values" of status.
    eo_appTheDB_GetSnrMaisStatusPtr(eo_appTheDB_GetHandle(), (eOsnsr_maisId_t)n,  &sstatus_ptr); 

    if(snsr_maisresolution_08 == maiscfg->resolution)
    {
        sstatus_ptr->the15values.head.capacity = 16;
        sstatus_ptr->the15values.head.itemsize = 1;
        sstatus_ptr->the15values.head.size = 16;
    }
    else if(snsr_maisresolution_16 == maiscfg->resolution)
    {
        sstatus_ptr->the15values.head.capacity = 32;
        sstatus_ptr->the15values.head.itemsize = 2;
        sstatus_ptr->the15values.head.size = 32;
    }
}


extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__mode(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_SB_CMD__SET_TXMODE
    };

    eOsnsr_maismode_t               *maismode = (eOsnsr_maismode_t*)nv->loc;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eo_appCanSP_SendCmd2SnrMais(appCanSP_ptr, (eOsnsr_maisId_t)n, msgCmd, (void*)maismode);

}



extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__datarate(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_SB_CMD__SET_CANDATARATE
    };

    uint8_t               *maisdatarate = (uint8_t*)nv->loc;

	EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eo_appCanSP_SendCmd2SnrMais(appCanSP_ptr, (eOsnsr_maisId_t)n, msgCmd, (void*)maisdatarate);

}




extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__resolution(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOsnsr_mais_status_t                *sstatus_ptr;
    eOicubCanProto_msgCommand_t     msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_SB_CMD__SET_RESOLUTION
    };

    eOsnsr_maisresolution_t         *maisresolution = (uint8_t*)nv->loc;

	EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eo_appCanSP_SendCmd2SnrMais(appCanSP_ptr, (eOsnsr_maisId_t)n, msgCmd, (void*)maisresolution);
    
    //set head of array "the15values" of status.
    eo_appTheDB_GetSnrMaisStatusPtr(eo_appTheDB_GetHandle(), (eOsnsr_maisId_t)n,  &sstatus_ptr); 

    if(snsr_maisresolution_08 == *maisresolution)
    {
        sstatus_ptr->the15values.head.capacity = 16;
        sstatus_ptr->the15values.head.itemsize = 1;
        sstatus_ptr->the15values.head.size = 16;
    }
    else if(snsr_maisresolution_16 == *maisresolution)
    {
        sstatus_ptr->the15values.head.capacity = 32;
        sstatus_ptr->the15values.head.itemsize = 2;
        sstatus_ptr->the15values.head.size = 32;
    }

}



extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    uint8_t                             channel = 0;
    eOresult_t                          res;
    eOicubCanProto_msgDestination_t     msgdest;
    eOsnsr_strain_status_t              *sstatus_ptr;
    eOappTheDB_sensorCanLocation_t      canLoc;
    eOicubCanProto_msgCommand_t         msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) 0
    };

    eOsnsr_strain_config_t               *straincfg = (eOsnsr_strain_config_t*)nv->loc;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eo_appTheDB_GetSnsrStrainCanLocation(eo_appTheDB_GetHandle(), (eOsnsr_strainId_t)xx, &canLoc);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canLoc.addr); 
    
    msgCmd.cmdId =  ICUBCANPROTO_POL_SB_CMD__SET_TXMODE;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(straincfg->mode));
    

    msgCmd.cmdId =  ICUBCANPROTO_POL_SB_CMD__SET_CANDATARATE;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(straincfg->datarate));
    
    if(straincfg->signaloncefullscale)
    {
        //clear array in strainstatus
        res = eo_appTheDB_GetSnrStrainStatusPtr(eo_appTheDB_GetHandle(), xx,  &sstatus_ptr);
        if(eores_OK != res)
        {
            return;
        }
        
        sstatus_ptr->fullscale.head.size = 0;
        memset(&sstatus_ptr->fullscale.data[0], 0, 12);
        channel = 0;
        msgCmd.cmdId =  ICUBCANPROTO_POL_SB_CMD__GET_FULL_SCALES;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &channel);
    }
    
    /* Note: here i don't set head data of arrays in status, because they are init by constant default one strain 
    (see eOcfg_nvsEP_as_any_con_sxxdefault.c) */
}

extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig__mode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_SB_CMD__SET_TXMODE
    };

    eOsnsr_strainmode_t               *strainmode = (eOsnsr_strainmode_t*)nv->loc;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eo_appCanSP_SendCmd2SnrStrain(appCanSP_ptr, (eOsnsr_strainId_t)xx, msgCmd, (void*)strainmode);


}

extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig__datarate(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_SB_CMD__SET_CANDATARATE
    };

    uint8_t               *straindatarate = (uint8_t*)nv->loc;

	EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eo_appCanSP_SendCmd2SnrStrain(appCanSP_ptr, (eOsnsr_strainId_t)xx, msgCmd, (void*)straindatarate);
}


extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig__signaloncefullscale(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    uint8_t                     channel = 0;
    eOresult_t                  res;
    eOsnsr_strain_status_t      *sstatus_ptr;
    eObool_t                    *signaloncefullscale = (eObool_t*)nv->loc;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_SB_CMD__GET_FULL_SCALES
    };

    if(*signaloncefullscale)
    {
        EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
        
        //clear array in strainstatus
        res = eo_appTheDB_GetSnrStrainStatusPtr(eo_appTheDB_GetHandle(), xx,  &sstatus_ptr);
        if(eores_OK != res)
        {
            return;
        }
        
        sstatus_ptr->fullscale.head.size = 0;
        memset(&sstatus_ptr->fullscale.data[0], 0, 12);
        channel = 0;
        eo_appCanSP_SendCmd2SnrStrain(appCanSP_ptr, xx, msgCmd, &channel);
    }

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



