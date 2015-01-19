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
#include "EOarray.h"

#include "EoAnalogSensors.h"


#include "EOMtheEMSappl.h"
#include "EOtheEMSapplBody.h"
#include "EOicubCanProto_specifications.h"


#include "EoProtocol.h"


#include "EOtheProtocolWrapper.h"



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
static void s_signalGetFullScales(eOas_strainId_t strainId, eObool_t signaloncefullscale);



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


extern void eoprot_fun_INIT_as_mais_config(const EOnv* nv)
{
    eOas_mais_config_t* maiscfg = eo_nv_RAM(nv);
    
    maiscfg->datarate = 10;
    maiscfg->mode = eoas_maismode_txdatacontinuously;
    maiscfg->resolution = eoas_maisresolution_08;
}


extern void eoprot_fun_INIT_as_mais_status(const EOnv* nv)
{
    eOas_mais_status_t *status = eo_nv_RAM(nv);  
    
    // marco.accame: i init as for eoas_maisresolution_08 
    //               the array the15values can be initted for size 0 or 16 as i now use teh proper eo_array_Assign() method
    uint8_t capacity    = 16;
    uint8_t itemsize    = 1;
    uint8_t size        = 16;
    EOarray* array = eo_array_New(capacity, itemsize, &status->the15values);
    eo_array_Resize(array, size);
}



extern void eoprot_fun_UPDT_as_mais_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_mais_status_t                  *status = NULL;
    eOsmStatesEMSappl_t                 currentstate;
    eOicubCanProto_msgDestination_t     msgdest;
    eOappTheDB_board_canlocation_t      canLoc;
    eOas_maisId_t                       maisId = (eOas_maisId_t)eoprot_ID2index(rd->id32); // this should be always 0
    eOicubCanProto_msgCommand_t         msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) 0
    };

    eOas_mais_config_t                  *maiscfg = (eOas_mais_config_t*)rd->data;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eo_appTheDB_GetSnsrMaisCanLocation(eo_appTheDB_GetHandle(), maisId, &canLoc);
    
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
            eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&(maiscfg->mode));
        }
    }
    
    
    msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&(maiscfg->datarate));

    msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&(maiscfg->resolution));
    

    status = eo_protocolwrapper_GetMaisStatus(eo_protocolwrapper_GetHandle(), maisId);
    if(NULL == status)
    {
        return; //error
    }     

    s_process_mais_resolution((eOas_maisresolution_t)maiscfg->resolution, status);
}


extern void eoprot_fun_UPDT_as_mais_config_mode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOsmStatesEMSappl_t           currentstate;
    eOas_maisId_t                 maisId = (eOas_maisId_t)eoprot_ID2index(rd->id32); //this should be always 0
    eOicubCanProto_msgCommand_t   msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
    };

    eOas_maismode_t                *maismode = (eOas_maismode_t*)rd->data;
    
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
           eo_appCanSP_SendCmd2SnrMais(appCanSP_ptr, maisId, msgCmd, (void*)maismode);
        }
    }
    else
    {
        eo_appCanSP_SendCmd2SnrMais(appCanSP_ptr, maisId, msgCmd, (void*)maismode);
    }
}


extern void eoprot_fun_UPDT_as_mais_config_datarate(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_maisId_t                  maisId = (eOas_maisId_t)eoprot_ID2index(rd->id32); //this should be always 0
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE
    };

    uint8_t               *maisdatarate = (uint8_t*)rd->data;

	EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eo_appCanSP_SendCmd2SnrMais(appCanSP_ptr, maisId, msgCmd, (void*)maisdatarate);

}




extern void eoprot_fun_UPDT_as_mais_config_resolution(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_mais_status_t              *status = NULL;
    eOas_maisId_t                   maisId = (eOas_maisId_t)eoprot_ID2index(rd->id32); //this should be always 0
    eOicubCanProto_msgCommand_t     msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_RESOLUTION
    };

    eOas_maisresolution_t           *maisresolution = (eOas_maisresolution_t*)rd->data;

	EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eo_appCanSP_SendCmd2SnrMais(appCanSP_ptr, maisId, msgCmd, (void*)maisresolution);
    
    status = eo_protocolwrapper_GetMaisStatus(eo_protocolwrapper_GetHandle(), maisId);
    if(NULL == status)
    {
        return; //error
    } 

    s_process_mais_resolution(*maisresolution, status);    
}


extern void eoprot_fun_INIT_as_strain_config(const EOnv* nv)
{
    eOas_strain_config_t* straincfg = eo_nv_RAM(nv);
    
    straincfg->datarate = 10;
    straincfg->mode = eoas_strainmode_acquirebutdonttx;
    straincfg->signaloncefullscale = eobool_false;
}


extern void eoprot_fun_INIT_as_strain_status(const EOnv* nv)
{
    eOas_strain_status_t *status = eo_nv_RAM(nv);
    uint8_t capacity = 0;
    uint8_t itemsize = 0;
    uint8_t size = 0;
    EOarray* array = NULL;
    
    // marco.accame: cannot understand why in here size is zero and in others it was not zero. see why. 
    //               probably fullscale, being of itemsize = 2 is treated without the proper eo_array_ methods
    capacity    = 6;
    itemsize    = 2;
    size        = 0; 
    array = eo_array_New(capacity, itemsize, &status->fullscale);
    eo_array_Resize(array, size);


    // marco.accame: size can be 0 or 6. now i use teh proper eo_array_Assign method for calibratedvalues/uncalibratedvalues 
    //
    capacity    = 6;
    itemsize    = 2;
    size        = 6;
    array = eo_array_New(capacity, itemsize, &status->calibratedvalues);
    eo_array_Resize(array, size);   

    capacity    = 6;
    itemsize    = 2;
    size        = 6;
    array = eo_array_New(capacity, itemsize, &status->uncalibratedvalues);
    eo_array_Resize(array, size); 
}


extern void eoprot_fun_UPDT_as_strain_config(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOicubCanProto_msgDestination_t     msgdest;
    eOappTheDB_board_canlocation_t      canLoc;
    eOsmStatesEMSappl_t                 currentstate;
    eOas_strainId_t                     strainId = (eOas_strainId_t)eoprot_ID2index(rd->id32); //this should be always 0
    eOicubCanProto_msgCommand_t         msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) 0
    };

    eOas_strain_config_t               *straincfg = (eOas_strain_config_t*)rd->data;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    eo_appTheDB_GetSnsrStrainCanLocation(eo_appTheDB_GetHandle(), strainId, &canLoc);
    
    msgdest.dest = ICUBCANPROTO_MSGDEST_CREATE(0, canLoc.addr); 

    // only if the appl is in RUN state enable strain tx
    eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
    if(eo_sm_emsappl_STrun == currentstate)
    {
        msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_TXMODE;
        eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&(straincfg->mode));        
    }

    msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE;
    eo_appCanSP_SendCmd(appCanSP_ptr, (eOcanport_t)canLoc.emscanport, msgdest, msgCmd, (void*)&(straincfg->datarate));
    
    s_signalGetFullScales(strainId, straincfg->signaloncefullscale);

}

extern void eoprot_fun_UPDT_as_strain_config_mode(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOsmStatesEMSappl_t            currentstate;
    eOas_strainId_t                strainId = (eOas_strainId_t)eoprot_ID2index(rd->id32); // this should be always 0
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_TXMODE
    };

    eOas_strainmode_t *strainmode = (eOas_strainmode_t*)rd->data;
    
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());

    if(eoas_strainmode_acquirebutdonttx == *strainmode) 
    {
        eo_appCanSP_SendCmd2SnrStrain(appCanSP_ptr, strainId, msgCmd, (void*)strainmode);
    }
    else //if pc104 configures strain mode to send data
    {
        //only if the appl is in RUN state enable mais tx
        eom_emsappl_GetCurrentState(eom_emsappl_GetHandle(), &currentstate);
        if(eo_sm_emsappl_STrun == currentstate)
        {
            eo_appCanSP_SendCmd2SnrStrain(appCanSP_ptr, strainId, msgCmd, (void*)strainmode);
        }
    }        
}

extern void eoprot_fun_UPDT_as_strain_config_datarate(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOas_strainId_t                strainId = (eOas_strainId_t)eoprot_ID2index(rd->id32); //this should be always 0
    eOicubCanProto_msgCommand_t    msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__SET_CANDATARATE
    };

    uint8_t               *straindatarate = (uint8_t*)rd->data;

	EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
    
    eo_appCanSP_SendCmd2SnrStrain(appCanSP_ptr, strainId, msgCmd, (void*)straindatarate);
}


extern void eoprot_fun_UPDT_as_strain_config_signaloncefullscale(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eObool_t                    *signaloncefullscale = (eObool_t*)rd->data;
    eOas_strainId_t             strainId = (eOas_strainId_t)eoprot_ID2index(rd->id32); //this should be always 0

    s_signalGetFullScales(strainId, *signaloncefullscale);
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


static void s_signalGetFullScales(eOas_strainId_t strainId, eObool_t signaloncefullscale)
{
    eOas_strain_status_t        *status;
    uint8_t                     channel = 0;
    eOicubCanProto_msgCommand_t msgCmd = 
    {
        EO_INIT(.class) icubCanProto_msgCmdClass_pollingAnalogSensor,
        EO_INIT(.cmdId) ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES
    };

    if(eobool_true == signaloncefullscale)
    {
        EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());
        //clear array in strainstatus
        
        
        status = eo_protocolwrapper_GetStrainStatus(eo_protocolwrapper_GetHandle(), strainId);
        if(NULL != status)
        {
            // impose that fullscale is an empty array of itemsize 2 and capacity 6. 
            // we have already done it inside the eoprot_fun_INIT_as_strain_status() function, but we do it again with eo_array_New()
            EOarray *array = eo_array_New(6, 2, &status->fullscale);
            
            // then we send a command to strain to send us the value of channel 0. 
            // this oepration triggers a new request until we receive all the 6 values
            channel = 0;
            msgCmd.cmdId =  ICUBCANPROTO_POL_AS_CMD__GET_FULL_SCALES;
            eo_appCanSP_SendCmd2SnrStrain(appCanSP_ptr, strainId, msgCmd, &channel);
        }
    }

}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



