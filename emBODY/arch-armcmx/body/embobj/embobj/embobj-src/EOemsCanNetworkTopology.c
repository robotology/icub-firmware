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

/* @file       EOemsCanNetworkTopology.c
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/
// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "EoCommon.h"
#include "EOtheMemoryPool.h"
#include "EOconstLookupTbl.h"
#include "EOconstLookupTbl_hid.h"
#include "EONV_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOemsCanNetworkTopology.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOemsCanNetworkTopology_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#ifdef EO_NVSCFG_USE_CACHED_NVS
    #define EO_EMSCANNETTOPO_USE_CACHED_NVS
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_emsCanNetTopo_hashTbls_init(EOemsCanNetTopo *p);
static void s_eo_emsCanNetTopo_nvsPtr_TEST_init(EOemsCanNetTopo *p);

static eOresult_t s_eo_emsCanNetTopo_GetMotorBoardNV_Status_ByCanLocation(EOemsCanNetTopo *p, eOcanport_t canPort, eo_icubCanProto_canBoardAddress_t boardAddr,
                                                                             eo_icubCanProto_motorAxis_t axis, EOnv** nvt_ptr);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static EOnv *nvPtr_status_TEST[1] = {0};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOemsCanNetTopo* eo_emsCanNetTopo_New(/*const*/ eo_emsCanNetTopo_cfg_t *cfg)
{
    EOemsCanNetTopo *retptr = NULL;  

    if(NULL == cfg)
    {    
        return(retptr);
    }

    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOemsCanNetTopo), 1);
    
    memcpy(&retptr->cfg, cfg, sizeof(eo_emsCanNetTopo_cfg_t));
    
    s_eo_emsCanNetTopo_hashTbls_init(retptr);  

#ifdef EO_EMSCANNETTOPO_USE_CACHED_NVS
    s_eo_emsCanNetTopo_nvsPtr_TEST_init(retptr);
#endif

    return(retptr);
} 

extern eOresult_t eo_emsCanNetTopo_GetMotorBoardNV_Status_ByCanLocation(EOemsCanNetTopo *p, eOcanport_t canPort, eo_icubCanProto_canBoardAddress_t boardAddr,
                                                                        eo_icubCanProto_motorAxis_t axis, EOnv** nvt_ptr)
{
    return (s_eo_emsCanNetTopo_GetMotorBoardNV_Status_ByCanLocation(p, canPort, boardAddr,
                                                                    axis,nvt_ptr));
}




extern eOresult_t eo_emsCanNetTopo_GetCanLocation_ByJointUniqueId(EOemsCanNetTopo *p, eOmc_jointUniqueId_t jUniqueId, eOcanport_t *canPort, eo_icubCanProto_canBoardAddress_t *boardAddr, eo_icubCanProto_motorAxis_t *axis)
{
    const EOconstLookupTbl* tbl_ptr;
    eo_emsCanNetTopo_hid_LUTbl_item_joint2BoardCanLocation_t *itemsList;
    
    if(NULL == p)
    {
        return(eores_OK);
    }


    tbl_ptr = p->cfg.joint2BoardCanLocation_LUTbl_ptr;

    if((jUniqueId - tbl_ptr->offset) >= tbl_ptr->capacity)
    {
        return(eores_NOK_generic);
    }

    itemsList = ( eo_emsCanNetTopo_hid_LUTbl_item_joint2BoardCanLocation_t *)tbl_ptr->itemsList;
    *canPort = itemsList[jUniqueId - tbl_ptr->offset].canPort;
    *boardAddr = itemsList[jUniqueId - tbl_ptr->offset].boardAddr; 
    *axis = itemsList[jUniqueId - tbl_ptr->offset].axis4board;
    return(eores_OK); 


}

extern eOresult_t eo_emsCanNetTopo_GetCanLocation_ByMotorUniqueId(EOemsCanNetTopo *p, eOmc_motorUniqueId_t mUniqueId, eOcanport_t *canPort, eo_icubCanProto_canBoardAddress_t *boardAddr, eo_icubCanProto_motorAxis_t *axis)
{
//    const EOconstLookupTbl* tbl_ptr;
//    eo_emsCanNetTopo_hid_LUTbl_item_joint2BoardCanLocation_t *itemsList;
//    
//    if(NULL == p)
//    {
//        return(eores_OK);
//    }
//
//
//    tbl_ptr = p->cfg.joint2BoardCanLocation_LUTbl_ptr;
//
//    if((jUniqueId - tbl_ptr->offset) >= tbl_ptr->capacity)
//    {
//        return(eores_NOK_generic);
//    }
//
//    itemsList = ( eo_emsCanNetTopo_hid_LUTbl_item_joint2BoardCanLocation_t *)tbl_ptr->itemsList;
//    *canPort = itemsList[jUniqueId - tbl_ptr->offset].canPort;
//    *boardAddr = itemsList[jUniqueId - tbl_ptr->offset].boardAddr; 
//    *axis = itemsList[jUniqueId - tbl_ptr->offset].axis4board;
    return(eores_OK); 


}



extern eOresult_t eo_emsCanNetTopo_GetCanLocation_ByJointBoardId(EOemsCanNetTopo *p, eOmc_jointBoardId_t jBoardId, eOcanport_t *canPort, eo_icubCanProto_canBoardAddress_t *boardAddr, eo_icubCanProto_motorAxis_t *axis)
{

    const EOconstLookupTbl  *tbl_ptr;
    eo_emsCanNetTopo_hid_LUTbl_item_joint2BoardCanLocation_t *itemsList;


    if(NULL == p)
    {
        return(eores_OK);
    }
    tbl_ptr = p->cfg.joint2BoardCanLocation_LUTbl_ptr;
    
    if(jBoardId >= tbl_ptr->capacity)
    {
        return(eores_NOK_generic);
    }

    itemsList = ( eo_emsCanNetTopo_hid_LUTbl_item_joint2BoardCanLocation_t *)tbl_ptr->itemsList;

    *canPort = itemsList[jBoardId].canPort;
    *boardAddr = itemsList[jBoardId].boardAddr; 
    *axis = itemsList[jBoardId].axis4board;
    return(eores_OK); 
}



extern eOresult_t eo_emsCanNetTopo_GetCanLocation_BySensorId(EOemsCanNetTopo *p, eOsnsr_sensorId_t sensorId, eOcanport_t *canPort, eo_icubCanProto_canBoardAddress_t *boardAddr)
{
//    const eo_emsCanNetTopo_sensor2BoardCanLocation_lookupTbl_t   *tbl_ptr = eo_emsCanNetTopo_sensor2BoardCanLocation_tbl_ptr;
//
//    if((sensorId - tbl_ptr->hdr.offset) > tbl_ptr->hdr.capacity)
//    {
//        return(eores_NOK_generic);
//    }
//
//    *canPort = tbl_ptr->tbl[sensorId - tbl_ptr->hdr.offset].canPort;
//    *boardAddr = tbl_ptr->tbl[sensorId - tbl_ptr->hdr.offset].boardAddr; 
    return(eores_OK); 
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_emsCanNetTopo_hashTbls_init(EOemsCanNetTopo *p)
{
    uint8_t i; //i indexes the motorBoardCanLoc2NvsRef lookup tbl
        eo_emsCanNetTopo_hid_LUTbl_item_boardNvsReferences_t *itemsList;


    //reset hashtbl
    memset(p->motorBoardHashTbl, 0xFF, sizeof(p->motorBoardHashTbl));    
    memset(p->sensorBoardHashTbl, 0xFF, sizeof(p->sensorBoardHashTbl)); 
    
   

    const EOconstLookupTbl* MBLUTbl_p1 = p->cfg.motorBoardCanLoc2NvsRef_LUTbl_ptr[eOutil_canport1];
    const EOconstLookupTbl* MBLUTbl_p2 = p->cfg.motorBoardCanLoc2NvsRef_LUTbl_ptr[eOutil_canport2];
    
    itemsList = (eo_emsCanNetTopo_hid_LUTbl_item_boardNvsReferences_t *)MBLUTbl_p1->itemsList;
    for(i=0; i<MBLUTbl_p1->capacity; i++)
    {
        p->motorBoardHashTbl[eOutil_canport1][itemsList[i].boardAddr] = i;            
    }   

    itemsList = (eo_emsCanNetTopo_hid_LUTbl_item_boardNvsReferences_t *)MBLUTbl_p2->itemsList;
    for(i=0; i<MBLUTbl_p2->capacity; i++)
    {
        p->motorBoardHashTbl[eOutil_canport2][itemsList[i].boardAddr] = i;            
    } 
   //TODO: setssa cosa sulle sensor board
}



static void s_eo_emsCanNetTopo_motorBoardNvsPtr_init(EOemsCanNetTopo *p)
{

//    uint8_t index_board, index_nvType;
//    eOnvEPID_t *nvEPID_ptr = NULL;
//    uint8_t ipindex, epindex, idindex;
//    EOnv nvtarget;
//    eOresult_t res;
//
//    const eo_emsCanNetTopo_hid_LUTbl_boardCanLocation2NvsRef_t* LUTbl = eo_emsCanNetTopo_LUTbl_motorBoardCanLoc2NvsRef_ptr;
//    for(index_board=0; index_board<LUTbl->hdr[eOutil_canport1].capacity; index_board++)
//    {
//        //set nv_ptr for joint
//        for(index_nvType=0; index_nvType<nvType_joint_max; index_nvType++)
//        {
//             nvEPID_ptr =&(LUTbl->tbl[eOutil_canport1][index_board].jointNvs_list[index_nvType]);
//             
//             res = eo_nvscfg_GetIndices(eo_my_nvsCfg, eok_ipv4addr_localhost, nvEPID_ptr->ep, nvEPID_ptr->id, 
//                                        &ipindex, &epindex, &idindex); 
//    
//            //suppongo di usare: EO_NVSCFG_USE_CACHED_NVS
//            s_eo_emsCanNetTopo_motorBoardNvsPtr_port1_list[index_board].jointNvsPtr_list[index_nvType] = eo_nvscfg_GetNV(eo_my_nvsCfg, ipindex, epindex, idindex, NULL, &nvtarget);    
//        }
//
//        //TODO: set nv_ptr for board
//    }

}




static void s_eo_emsCanNetTopo_nvsPtr_TEST_init(EOemsCanNetTopo *p)
{

    const eOnvEPID_t *nvEPID_ptr;
    uint8_t ipindex, epindex, idindex;
    EOnv nvtarget;
    eo_emsCanNetTopo_hid_LUTbl_item_boardNvsReferences_t *itemsList;
    const EOconstLookupTbl* MBLUTbl_p1 = p->cfg.motorBoardCanLoc2NvsRef_LUTbl_ptr[eOutil_canport1];

    itemsList = (eo_emsCanNetTopo_hid_LUTbl_item_boardNvsReferences_t *)MBLUTbl_p1->itemsList;

    nvEPID_ptr =&(itemsList[0].joint_nvid_list[nvsType_joint_status]);
    
    eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, nvEPID_ptr->ep, nvEPID_ptr->id, 
                            &ipindex, &epindex, &idindex); 
    
    //suppongo di usare: EO_NVSCFG_USE_CACHED_NVS
    nvPtr_status_TEST[0] = eo_nvscfg_GetNV(p->cfg.nvsCfg, ipindex, epindex, idindex, NULL, &nvtarget);    

}

#ifdef EO_EMSCANNETTOPO_USE_CACHED_NVS
static eOresult_t s_eo_emsCanNetTopo_GetMotorBoardNV_Status_ByCanLocation(EOemsCanNetTopo *p, eOcanport_t canPort, eo_icubCanProto_canBoardAddress_t boardAddr,
                                                                             eo_icubCanProto_motorAxis_t axis, EOnv** nvt_ptr)
{
    uint8_t i;

    if(NULL == p)
    {
        return(eores_OK);
    }


    i = p->motorBoardHashTbl[canPort][boardAddr];
    *nvt_ptr = nvPtr_status_TEST[i];
    return(eores_OK);
}

#else


//if not define EO_EMSCANNETTOPO_USE_CACHED_NVS: 
// I allacate a new nv, fill it with value and return its address.
//this solution is not goot (memory fragmented), but i use this only for test porpouse
static eOresult_t s_eo_emsCanNetTopo_GetMotorBoardNV_Status_ByCanLocation(EOemsCanNetTopo *p, eOcanport_t canPort, eo_icubCanProto_canBoardAddress_t boardAddr,
                                                                             eo_icubCanProto_motorAxis_t axis, EOnv** nvt_ptr)
{
   uint8_t                 i;
    const eOnvEPID_t        *nvEPID_ptr;
    uint8_t ipindex,        epindex, idindex;
    EOnv                    nvtarget;
    const EOconstLookupTbl  *LUTbl;
    eo_emsCanNetTopo_hid_LUTbl_item_boardNvsReferences_t *itemsList;
    eOresult_t  res;

    if(NULL == p)
    {
        return(eores_OK);
    }

    LUTbl = p->cfg.motorBoardCanLoc2NvsRef_LUTbl_ptr[canPort];
    itemsList = (eo_emsCanNetTopo_hid_LUTbl_item_boardNvsReferences_t *)LUTbl->itemsList;
////versione senza hashtable
//    for(i=0; i<LUTbl->hdr[canPort].capacity; i++)
//    {
//        if((boardAddr == LUTbl->tbl[canPort][i].boardAddr) && (axis == LUTbl->tbl[canPort][i].axis))
//        {
//            nvEPID_ptr = &(LUTbl->tbl[canPort][i].joint_nvid_list[nvsType_joint_status]);
//            break;       
//        }
//    }

//versione con hastbl
    i = p->motorBoardHashTbl[canPort][boardAddr];
    nvEPID_ptr = &(itemsList[i].joint_nvid_list[nvsType_joint_status]);

//da qui in poi rimane uguale!
    if(NULL == nvEPID_ptr)
    {
        return(eores_NOK_nodata); //non ho trovato niente!!! non dovrebbe capitare!!!tieni questo controllo solo per debug    
    }

    *nvt_ptr = eo_nv_New();
    res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, 
                               eok_ipv4addr_localhost, nvEPID_ptr->ep, nvEPID_ptr->id, 
                               &ipindex, &epindex, &idindex); 

    eo_nvscfg_GetNV(p->cfg.nvsCfg, ipindex, epindex, idindex, NULL, *nvt_ptr);
    return(res);


}

#endif
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

