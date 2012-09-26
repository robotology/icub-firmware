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


/* @file       eOcfg_icubCanProto_sensorBoardMsgfunction.c
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "EOcommon.h"
#include "EOicubCanProto.h"
#include "EOicubCanProto_specifications.h"
#include "EOicubCanProto_hid.h"

// #include "EoMotionControl.h"
// #include "EOnv_hid.h"
// #include "EOarray.h"



//icub api
#include "EoSensors.h"
#include "EOappTheDataBase.h"
//nv-cfg
// #include "eOcfg_nvsEP_as.h"
// #include "eOcfg_nvsEP_sk.h"

//#include "hal_debugPin.h"
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "eOcfg_icubCanProto_sensorBoardMsgFunctions.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define EMS_CAN_ADDR                                            0
#define ICUBCANPROTO_POL_SB_ID_DEFAULT                          ((ICUBCANPROTO_CLASS_POLLING_SENSORBOARD << 8) | (0x0<<4))
#define ICUBCANPROTO_POL_SB_CREATE_ID(destBoardAddr)            ((ICUBCANPROTO_CLASS_POLLING_SENSORBOARD << 8) | (EMS_CAN_ADDR<<4) | (destBoardAddr&0xF))
#define ICUBCANPROTO_POL_SK_CREATE_ID(destBoardAddr)            ((ICUBCANPROTO_CLASS_POLLING_SENSORBOARD << 8)|  (EMS_CAN_ADDR<<4) | (destBoardAddr&0xF))


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


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

//********************** P A R S E R       POLLING     F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_parser_pol_sb_unexpected_cmd(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_former_pol_sb_unexpected_cmd(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setTxMode(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOenum08_t *txmode = (eOenum08_t *)val_ptr;
    
    canFrame->id = ICUBCANPROTO_POL_SB_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 2;
    canFrame->data[0] = ICUBCANPROTO_POL_SB_CMD__SET_TXMODE;
    canFrame->data[1] = *txmode;
    
//     switch(*maismode)
//     {
//         case snsr_maismode_txdatacontinuously:
//         {
//                 canFrame->data[1] = 0;
//         }break;
//         
//         case snsr_maismode_acquirebutdonttx:
//         {
//                 canFrame->data[1] = 1;
//             
//         }break;

//         case snsr_maismode_dontacquiredonttx:
//         {
//                 canFrame->data[1] = 2;
//             
//         }break;

//     };
    
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setCanDatarate(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    uint8_t *datarate = (uint8_t*)val_ptr;
    canFrame->id = ICUBCANPROTO_POL_SB_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 2;
    canFrame->data[0] = ICUBCANPROTO_POL_SB_CMD__SET_CANDATARATE;
    canFrame->data[1] = *datarate;
    
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setResolution(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOsnsr_maisresolution_t *maisresolution = (eOsnsr_maisresolution_t*)val_ptr;
    
    canFrame->id = ICUBCANPROTO_POL_SB_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 2;
    canFrame->data[0] = ICUBCANPROTO_POL_SB_CMD__SET_RESOLUTION;
    switch(*maisresolution)
    {
        case snsr_maisresolution_08:
        {
            canFrame->data[1] = 2;
        }break;
        
        case snsr_maisresolution_16:
        {
            canFrame->data[1] = 0;
        }break;

#ifdef _MAIS_TEST_
        case snsr_maisresolution_debug:
        {
            canFrame->data[1] = 1;
        }break;
#endif

    }
    
    return(eores_OK);
}


extern eOresult_t eo_icubCanProto_parser_pol_sb_cmd__getFullScales(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__getFullScales(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


//********************** P A R S E R       PERIODIC     F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__forceVector(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_sensorCanLocation_t              canLoc;
    eOsnsr_strainId_t                           sId;
    eOsnsr_strain_config_t                      *sconfig_ptr;
    eOsnsr_strain_status_t                      *sstatus_ptr;

    /* 1) get can location */
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    
    res = eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(eo_appTheDB_GetHandle(), &canLoc, &sId);
    if(eores_OK != res)
    {
        return(res);
    }

    /* 2) get strain transmission mode, contained in straion config nv.
          (parsing depends on transmission mode)*/
    res = eo_appTheDB_GetSnrStrainConfigPtr(eo_appTheDB_GetHandle(), sId,  &sconfig_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    /* 3) get pointer to nv var where save incoming force values */
    res = eo_appTheDB_GetSnrStrainStatusPtr(eo_appTheDB_GetHandle(), sId,  &sstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    
    
    /* 4) set incoming force values */    
    switch(sconfig_ptr->mode)
    {
        case snsr_strainmode_txcalibrateddatacontinuously:
        case snsr_strainmode_txalldatacontinuously:
        {
            memcpy(&(sstatus_ptr->calibratedvalues.data[0]), &(frame->data[0]), 6);
        }break;

        case snsr_strainmode_txuncalibrateddatacontinuously:
        {
            memcpy(&(sstatus_ptr->uncalibratedvalues.data[0]), &(frame->data[0]), 6);
        }break;
        
        default:
        {
            //i must never be here!
            return(eores_NOK_generic);
        }
    };
    
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__torqueVector(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_sensorCanLocation_t              canLoc;
    eOsnsr_strainId_t                           sId;
    eOsnsr_strain_config_t                      *sconfig_ptr;
    eOsnsr_strain_status_t                      *sstatus_ptr;

    /* 1) get can location */
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    
    res = eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(eo_appTheDB_GetHandle(), &canLoc, &sId);
    if(eores_OK != res)
    {
        return(res);
    }

    /* 2) get strain transmission mode, contained in straion config nv.
          (parsing depends on transmission mode)*/
    res = eo_appTheDB_GetSnrStrainConfigPtr(eo_appTheDB_GetHandle(), sId,  &sconfig_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    /* 3) get pointer to nv var where save incoming force values */
    res = eo_appTheDB_GetSnrStrainStatusPtr(eo_appTheDB_GetHandle(), sId,  &sstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    
    
    /* 4) set incoming force values */
    
    switch(sconfig_ptr->mode)
    {
        case snsr_strainmode_txcalibrateddatacontinuously:
        case snsr_strainmode_txalldatacontinuously:
        {
            memcpy(&(sstatus_ptr->calibratedvalues.data[6]), &(frame->data[0]), 6);
        }break;

        case snsr_strainmode_txuncalibrateddatacontinuously:
        {
            memcpy(&(sstatus_ptr->uncalibratedvalues.data[6]), &frame->data[0], 6);
        }break;
        
        default:
        {
            //i must never be here!
            return(eores_NOK_generic);
        }
    };
    
    return(eores_OK);
}



extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__uncalibForceVectorDebugmode(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_sensorCanLocation_t              canLoc;
    eOsnsr_strainId_t                           sId;
    eOsnsr_strain_config_t                      *sconfig_ptr;
    eOsnsr_strain_status_t                      *sstatus_ptr;

    /* 1) get can location */
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    
    res = eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(eo_appTheDB_GetHandle(), &canLoc, &sId);
    if(eores_OK != res)
    {
        return(res);
    }

    /* 2) get strain transmission mode, contained in straion config nv.
          (parsing depends on transmission mode)*/
    res = eo_appTheDB_GetSnrStrainConfigPtr(eo_appTheDB_GetHandle(), sId,  &sconfig_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    /* 3) get pointer to nv var where save incoming force values */
    res = eo_appTheDB_GetSnrStrainStatusPtr(eo_appTheDB_GetHandle(), sId,  &sstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    memcpy(&(sstatus_ptr->uncalibratedvalues.data[0]), &frame->data[0], 6);

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__uncalibTorqueVectorDebugmode(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOappTheDB_sensorCanLocation_t              canLoc;
    eOsnsr_strainId_t                           sId;
    eOsnsr_strain_config_t                      *sconfig_ptr;
    eOsnsr_strain_status_t                      *sstatus_ptr;

    /* 1) get can location */
    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    
    res = eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(eo_appTheDB_GetHandle(), &canLoc, &sId);
    if(eores_OK != res)
    {
        return(res);
    }

    /* 2) get strain transmission mode, contained in straion config nv.
          (parsing depends on transmission mode)*/
    res = eo_appTheDB_GetSnrStrainConfigPtr(eo_appTheDB_GetHandle(), sId,  &sconfig_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    /* 3) get pointer to nv var where save incoming force values */
    res = eo_appTheDB_GetSnrStrainStatusPtr(eo_appTheDB_GetHandle(), sId,  &sstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    memcpy(&(sstatus_ptr->uncalibratedvalues.data[6]), &frame->data[0], 6);

    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes0to6(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOsnsr_maisId_t                             sId;
    eOappTheDB_sensorCanLocation_t              canLoc;
    eOsnsr_mais_status_t                        *sstatus_ptr;

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    res = eo_appTheDB_GetSnsrMaisId_BySensorCanLocation(eo_appTheDB_GetHandle(), &canLoc, &sId);
    if(eores_OK != res)
    {
        return(res);
    }
    
    res = eo_appTheDB_GetSnrMaisStatusPtr(eo_appTheDB_GetHandle(), sId,  &sstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    memcpy(&(sstatus_ptr->the15values.data[0]), &(frame->data[0]), 7); 

    return(eores_OK);    
}


extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes7to14(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    eOsnsr_maisId_t                             sId;
    eOappTheDB_sensorCanLocation_t              canLoc;
    eOsnsr_mais_status_t                        *sstatus_ptr;

    canLoc.emscanport = canPort;
    canLoc.addr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    res = eo_appTheDB_GetSnsrMaisId_BySensorCanLocation(eo_appTheDB_GetHandle(), &canLoc, &sId);
    if(eores_OK != res)
    {
        return(res);
    }
    
    res = eo_appTheDB_GetSnrMaisStatusPtr(eo_appTheDB_GetHandle(), sId,  &sstatus_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    memcpy(&(sstatus_ptr->the15values.data[7]), &(frame->data[0]), 8); 
    
    return(eores_OK);
}






/***********************************************************************************************************************************/
/*******************************  S K I N     F U N C T I O N S    *******************************************************************/
/***********************************************************************************************************************************/

extern eOresult_t eo_icubCanProto_parser_per_sk_cmd__allSkinMsg(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t          res;
    eOsk_skinId_t     sId = 0;
//    eOskin_status_t     *skstatus_ptr;
    EOarray_of_10canframes *arrayof10canframes_ptr = NULL;

//uncomment this code when status and config nvvar are used.
//     res = eo_appTheDB_GetSkinStatusPtr(eo_appTheDB_GetHandle(), sId,  &skstatus_ptr);
//     if(eores_OK != res)
//     {
//         return(res);
//     }

//     res =  eo_array_PushBack((EOarray*)(&skstatus_ptr->arrayof10canframes), frame);

    res = eo_appTheDB_GetSkinStArray10CanFramesPtr(eo_appTheDB_GetHandle(), sId,  &arrayof10canframes_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    res =  eo_array_PushBack((EOarray*)arrayof10canframes_ptr, frame);

    return(res);
}

extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__tactSetup(EOicubCanProto* p, void *val_ptr, eOicubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_SK_CREATE_ID(dest.s.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 8;

    /* 2) set can command (see SkinPrototype::calibrateSensor)*/
    canFrame->data[0] = ICUBCANPROTO_POL_SK_CMD__TACT_SETUP;
    canFrame->data[1]=0x01;  //==> risoluzione 8 bit   e 12 isure indipendenti
	canFrame->data[2]=0x01;  //==> invia ogni 40 milli
	canFrame->data[3]=0x01;
	canFrame->data[4]=0;
	canFrame->data[5]=0x22;
	canFrame->data[6]=0;
	canFrame->data[7]=0;

    return(eores_OK);
}

//unused
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes0to3(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}
//
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes4to7(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}
//
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes8to11(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}
//
//extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes12to15(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
//{
//    return(eores_OK);
//}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



