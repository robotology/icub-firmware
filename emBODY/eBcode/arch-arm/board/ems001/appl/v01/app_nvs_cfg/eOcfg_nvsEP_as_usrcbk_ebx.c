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

#include "EoAnalogSensors.h"
#include "eOcfg_nvsEP_as.h"

//application
#include "EOMtheEMSappl.h"
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

static void s_process_mais_resolution(eOas_maisresolution_t resolution, eOas_mais_status_t* status);



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
//     eOas_arrayofupto36bytes_t *maisArray = (eOas_arrayofupto36bytes_t *)nv->loc;    
//     maisArray->head.capacity = 
//     capacity;
//     uint8_t         itemsize;
//     uint8_t         size;
// }

//mais-update

extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOas_mais_status_t                  *sstatus_ptr;
    eOsmStatesEMSappl_t                 currentstate;
    eOicubCanProto_msgDestination_t     msgdest;
    eOappTheDB_sensorCanLocation_t      canLoc;
    eOicubCanProto_msgCommand_t         msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) 0
    };

    eOas_mais_config_t                  *maiscfg = (eOas_mais_config_t*)nv->loc;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eo_appTheDB_GetSnsrMaisCanLocation(eo_appTheDB_GetHandle(), (eOas_maisId_t)n, &canLoc);
    
    //if pc104 tell me to enable maistx, before to send cmd verify if i'm in RUN state:
    // if yes ==> ok no problem
    // if no ==> i'll send cmd when go to RUN state
    if(eoas_maismode_txdatacontinuously == maiscfg->mode)
    {
        //only if the appl is in RUN state enable mais tx
        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
        if(eo_sm_emsappl_STrun == currentstate)
        {
            msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canLoc.addr); 
            msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
            eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(maiscfg->mode));
        }
    }
    
    
    msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(maiscfg->datarate));

    msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION;
    eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(maiscfg->resolution));
    
    
    eo_appTheDB_GetSnrMaisStatusPtr(eo_appTheDB_GetHandle(), (eOas_maisId_t)n,  &sstatus_ptr); 
    

    s_process_mais_resolution((eOas_maisresolution_t)maiscfg->resolution, sstatus_ptr);
}


extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__mode(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOsmStatesEMSappl_t           currentstate;
    
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
    };

    eOas_maismode_t                *maismode = (eOas_maismode_t*)nv->loc;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    //if pc104 tell me to enable maistx, before to send cmd verify if i'm in RUN state:
    // if yes ==> ok no problem
    // if no ==> i'll send cmd when go to RUN state
    if(eoas_maismode_txdatacontinuously == *maismode)
    {
        //only if the appl is in RUN state enable mais tx
        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
        if(eo_sm_emsappl_STrun == currentstate)
        {
           eo_appCanSP_SendCmd2SnrMais(appCanSP_ptr, (eOas_maisId_t)n, msgCmd, (void*)maismode);
        }
    }
    else
    {
        eo_appCanSP_SendCmd2SnrMais(appCanSP_ptr, (eOas_maisId_t)n, msgCmd, (void*)maismode);
    }
}



extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__datarate(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE
    };

    uint8_t               *maisdatarate = (uint8_t*)nv->loc;

	EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eo_appCanSP_SendCmd2SnrMais(appCanSP_ptr, (eOas_maisId_t)n, msgCmd, (void*)maisdatarate);

}




extern void eo_cfg_nvsEP_as_onemais_usr_hid_UPDT_Mxx_mconfig__resolution(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOas_mais_status_t              *sstatus_ptr;
    eOicubCanProto_msgCommand_t     msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION
    };

    eOas_maisresolution_t           *maisresolution = (uint8_t*)nv->loc;

	EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eo_appCanSP_SendCmd2SnrMais(appCanSP_ptr, (eOas_maisId_t)n, msgCmd, (void*)maisresolution);
    

    eo_appTheDB_GetSnrMaisStatusPtr(eo_appTheDB_GetHandle(), (eOas_maisId_t)n,  &sstatus_ptr); 

    s_process_mais_resolution(*maisresolution, sstatus_ptr);
    
}



extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    uint8_t                             channel = 0;
    eOresult_t                          res;
    eOicubCanProto_msgDestination_t     msgdest;
    eOas_strain_status_t                *sstatus_ptr;
    eOappTheDB_sensorCanLocation_t      canLoc;
    eOsmStatesEMSappl_t                 currentstate;
    eOicubCanProto_msgCommand_t         msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) 0
    };

    eOas_strain_config_t               *straincfg = (eOas_strain_config_t*)nv->loc;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eo_appTheDB_GetSnsrStrainCanLocation(eo_appTheDB_GetHandle(), (eOas_strainId_t)xx, &canLoc);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canLoc.addr); 

    //only if the appl is in RUN state enable strain tx
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
    if(eo_sm_emsappl_STrun == currentstate)
    {
        msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, (void*)&(straincfg->mode));        
    }

    msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
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
        msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
        eo_appCanSP_SendCmd(appCanSP_ptr, canLoc.emscanport, msgdest, msgCmd, &channel);
    }
    
    /* Note: here i don't set head data of arrays in status, because they are init by constant default one strain 
    (see eOcfg_nvsEP_as_any_con_sxxdefault.c) */
}

extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig__mode(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOsmStatesEMSappl_t            currentstate;
    
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
    };

    eOas_strainmode_t               *strainmode = (eOas_strainmode_t*)nv->loc;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());

    if(eoas_strainmode_acquirebutdonttx == *strainmode) 
    {
        eo_appCanSP_SendCmd2SnrStrain(appCanSP_ptr, (eOas_strainId_t)xx, msgCmd, (void*)strainmode);
    }
    else //if pc104 configures strain mode to send data
    {
        //only if the appl is in RUN state enable mais tx
        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
        if(eo_sm_emsappl_STrun == currentstate)
        {
            eo_appCanSP_SendCmd2SnrStrain(appCanSP_ptr, (eOas_strainId_t)xx, msgCmd, (void*)strainmode);
        }
    }        
}

extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig__datarate(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE
    };

    uint8_t               *straindatarate = (uint8_t*)nv->loc;

	EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eo_appCanSP_SendCmd2SnrStrain(appCanSP_ptr, (eOas_strainId_t)xx, msgCmd, (void*)straindatarate);
}


extern void eo_cfg_nvsEP_as_onestrain_usr_hid_UPDT_Sxx_sconfig__signaloncefullscale(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    uint8_t                     channel = 0;
    eOresult_t                  res;
    eOas_strain_status_t        *sstatus_ptr;
    eObool_t                    *signaloncefullscale = (eObool_t*)nv->loc;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) eo_icubCanProto_msgCmdClass_pollingSensorBoard,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES
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

static void s_process_mais_resolution(eOas_maisresolution_t resolution, eOas_mais_status_t* status)
{
    uint8_t size = 0;
    uint8_t itemsize = 1;
    uint8_t capacity = 0;
    
    if(eoas_maisresolution_08 == resolution)
    {
        capacity    = 16;
        itemsize    = 1;
        size        = 16;
        //#warning acemor-> nella mais nel caso di risoluzione a 8 bit perche' la capacity di the15values e' 16 e non 15?
        //status->the15values.head.capacity = 16;
        //status->the15values.head.itemsize = 1;
        //status->the15values.head.size = 16;
    }
    else if(eoas_maisresolution_16 == resolution)
    {
        capacity    = 16;
        itemsize    = 2;
        size        = 16;
        //#warning acemor-> nella mais ho messo la capacity di the15values a 16 anche nel caso di risoluzione a 32 bit
        //status->the15values.head.capacity = 16;
        //status->the15values.head.itemsize = 2;
        status->the15values.head.size = 16;
    } 

    EOarray* array = eo_array_New(capacity, itemsize, &status->the15values);
    eo_array_Resize(array, size);    
   
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



