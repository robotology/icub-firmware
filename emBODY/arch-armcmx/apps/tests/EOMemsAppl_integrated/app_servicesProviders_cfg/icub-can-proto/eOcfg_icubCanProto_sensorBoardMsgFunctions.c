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
#include "EOcommon.h"
#include "EOicubCanProto.h"
#include "EOicubCanProto_specifications.h"
#include "EOicubCanProto_hid.h"

#include "EoMotionControl.h"
#include "EOnv_hid.h"
#include "EOarray.h"

#include "EOappTheNVmapRef.h"
#include "EOemsCanNetworkTopology.h"

//icub api
#include "EoSensors.h"
//nv-cfg
#include "eOcfg_nvsEP_as.h"
#include "eOcfg_nvsEP_sk.h"

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


extern eOresult_t eo_icubCanProto_former_pol_sb_unexpected_cmd(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setTxMode(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOenum08_t *txmode = (eOenum08_t *)val_ptr;
    
    canFrame->id = ICUBCANPROTO_POL_SB_CREATE_ID(dest.canAddr);
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

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setCanDatarate(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    uint8_t *datarate = (uint8_t*)val_ptr;
    canFrame->id = ICUBCANPROTO_POL_SB_CREATE_ID(dest.canAddr);
    canFrame->id_type = 0; //standard id
    canFrame->frame_type = 0; //data frame
    canFrame->size = 2;
    canFrame->data[0] = ICUBCANPROTO_POL_SB_CMD__SET_CANDATARATE;
    canFrame->data[1] = *datarate;
    
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__setResolution(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    eOsnsr_maisresolution_t *maisresolution = (eOsnsr_maisresolution_t*)val_ptr;
    
    canFrame->id = ICUBCANPROTO_POL_SB_CREATE_ID(dest.canAddr);
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

extern eOresult_t eo_icubCanProto_former_pol_sb_cmd__getFullScales(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    return(eores_OK);
}


//********************** P A R S E R       PERIODIC     F U N C T I O N S  ******************************************************
extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__forceVector(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__torqueVector(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    return(eores_OK);
}

extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes0to6(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    uint8_t                                     i, index;
    eOsnsr_sensorId_t                           sId;
    void                                        *nv_mem_ptr;
    eOsnsr_arrayofupto36bytes_t                 *the15values_ptr;
    eo_emsCanNetTopo_sensorCanLocation_t        canLoc;

    canLoc.emscanport = canPort;
    canLoc.canaddr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    res = eo_emsCanNetTopo_GetSensorId_BySensorCanLocation(p->emsCanNetTopo_ptr, &canLoc, &sId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetSensorsMaisNVMemoryRef(eo_appTheNVmapRef_GetHandle(), sId, maisNVindex_mstatus__the15values, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    the15values_ptr = (eOsnsr_arrayofupto36bytes_t *)nv_mem_ptr;


    switch(the15values_ptr->data[0])
    {
        case 0: //array resetted
        {
            the15values_ptr->data[0] = 1;
            the15values_ptr->head.size = 8;
        }break;
            
        case 1: //somethig is wrong new values are avilable and i dont' send older ones.
        {
            ;//nothing to do ==> error count
        }break;
        
        case 8: //i'm the second message
        {
            the15values_ptr->data[0] = 1;
            //the15values_ptr->size = 16; the size does not change!!!  
        }break;
    };
    
    for(i=0,index = 1; i<frame->size; i++, index++)
    {
        the15values_ptr->data[index] = frame->data[i];
//         #warning VALE--> 1-non copio il valore della mais solo per test!!!!
//         the15values_ptr->data[index] = (0xC0|index);
    }

    return(eores_OK);
    
}


extern eOresult_t eo_icubCanProto_parser_per_sb_cmd__hes7to14(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t                                  res;
    uint8_t                                     i, index;
    eOsnsr_sensorId_t                           sId;
    void                                        *nv_mem_ptr;
    eOsnsr_arrayofupto36bytes_t                 *the15values_ptr;
    eo_emsCanNetTopo_sensorCanLocation_t        canLoc;

    canLoc.emscanport = canPort;
    canLoc.canaddr = eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(frame->id);
    
    res = eo_emsCanNetTopo_GetSensorId_BySensorCanLocation(p->emsCanNetTopo_ptr, &canLoc, &sId);
    if(eores_OK != res)
    {
        return(res);
    }

    res = eo_appTheNVmapRef_GetSensorsMaisNVMemoryRef(eo_appTheNVmapRef_GetHandle(), sId, maisNVindex_mstatus__the15values, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }
    
    the15values_ptr = (eOsnsr_arrayofupto36bytes_t *)nv_mem_ptr;
    
    switch(the15values_ptr->data[0])
    {
        case 0: //array resetted
        {
            the15values_ptr->data[0] = 8;
            the15values_ptr->head.size = 16;
        }break;
            
        case 1: //i'm the second message
        {
            the15values_ptr->head.size = 16;
        }break;
        
        case 8: //somethig is wrong new values are avilable and i dont' send older ones.
        {
            ;//nothing to do ==> error count
        }break;
    };
    
    for(i=0,index = 8; i<frame->size; i++, index++)
    {
        the15values_ptr->data[index] = frame->data[i];
//         #warning VALE--> 1-non copio il valore della mais solo per test!!!!
//         the15values_ptr->data[index] = (0xC0|index);
    }
    
    return(eores_OK);
}






/***********************************************************************************************************************************/
/*******************************  S K I N     F U N C T I O N S    *******************************************************************/
/***********************************************************************************************************************************/

extern eOresult_t eo_icubCanProto_parser_per_sk_cmd__allSkinMsg(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort)
{
    eOresult_t res;
    void *nv_mem_ptr;
//    EOarray_of_10canframes *array;
    EOarray * array;

    res = eo_appTheNVmapRef_GetSkinNVMemoryRef(eo_appTheNVmapRef_GetHandle(), 0, skinNVindex_sstatus__arrayof10canframe, &nv_mem_ptr);
    if(eores_OK != res)
    {
        return(res);
    }

    array = (EOarray*)nv_mem_ptr;
    res =  eo_array_PushBack(array, frame);

    return(res);
}

extern eOresult_t eo_icubCanProto_former_pol_sk_cmd__tactSetup(EOicubCanProto* p, void *val_ptr, eo_icubCanProto_msgDestination_t dest, eOcanframe_t *canFrame)
{
    /* 1) prepare base information*/
    canFrame->id = ICUBCANPROTO_POL_SK_CREATE_ID(dest.canAddr);
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



