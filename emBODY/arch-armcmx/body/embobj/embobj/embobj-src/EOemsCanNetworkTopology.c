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
#include "EOconstvector.h"
#include "EOconstvector_hid.h"


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
// empty-section



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
static void s_eo_emsCanNetTopo_hashTbl_joint_init(EOemsCanNetTopo *p);
static void s_eo_emsCanNetTopo_hashTbl_motor_init(EOemsCanNetTopo *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOemsCanNetTopo* eo_emsCanNetTopo_New(eo_emsCanNetTopo_cfg_t *cfg)
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

    return(retptr);
} 


extern eOresult_t eo_emsCanNetTopo_GetJointCanLocation_ByJointId(EOemsCanNetTopo *p, eOmc_jointId_t jId, 
                                                                        eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eObrd_types_t *boardType)
{
    if((NULL == p) || (NULL == location_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if( NULL == p->joint_Id2CanLoc_hTbl[jId].ptr)
    {
        return(eores_NOK_nodata);
    }

    location_ptr->canaddr = p->joint_Id2CanLoc_hTbl[jId].ptr->boardAddr;
    location_ptr->axis = p->joint_Id2CanLoc_hTbl[jId].ptr->axis;
    location_ptr->emscanport = p->joint_Id2CanLoc_hTbl[jId].ptr->canPort;

    if(NULL !=boardType)
    {
        *boardType = p->joint_Id2CanLoc_hTbl[jId].ptr->boardType;
    }
    return(eores_OK);
}

extern eOresult_t eo_emsCanNetTopo_GetMotorCanLocation_ByMotorId(EOemsCanNetTopo *p, eOmc_motorId_t mId, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eObrd_types_t *boardType)
{
    if((NULL == p) || (NULL == location_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if( NULL == p->motor_Id2CanLoc_hTbl[mId].ptr)
    {
        return(eores_NOK_nodata);
    }

    location_ptr->canaddr = p->motor_Id2CanLoc_hTbl[mId].ptr->boardAddr;
    location_ptr->axis = p->motor_Id2CanLoc_hTbl[mId].ptr->axis;
    location_ptr->emscanport = p->motor_Id2CanLoc_hTbl[mId].ptr->canPort;

    if(NULL !=boardType)
    {
        *boardType = p->motor_Id2CanLoc_hTbl[mId].ptr->boardType;
    }

    return(eores_OK);
}


extern eOresult_t eo_emsCanNetTopo_GetSensorCanLocation_BySensorId(EOemsCanNetTopo *p, eOsnsr_sensorId_t sId, 
                                                                        eo_emsCanNetTopo_sensorCanLocation_t *location_ptr, eObrd_types_t *boardType)
{
    if((NULL == p) || (NULL == location_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if( NULL == p->sensor_Id2CanLoc_hTbl[sId].ptr)
    {
        return(eores_NOK_nodata);
    }

    location_ptr->canaddr = p->sensor_Id2CanLoc_hTbl[sId].ptr->boardAddr;
    location_ptr->emscanport = p->sensor_Id2CanLoc_hTbl[sId].ptr->canPort;

    if(NULL !=boardType)
    {
        *boardType = p->sensor_Id2CanLoc_hTbl[sId].ptr->boardType;
    }
    return(eores_OK);
}




extern eOresult_t eo_emsCanNetTopo_GetJointId_ByJointCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eOmc_jointId_t *jId_ptr)
{

    eo_emsCanNetTopo_hashTbl_jm_item_t *item_ptr;
    if((NULL == p) || (NULL == location_ptr) || (NULL == jId_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    item_ptr = &(p->joint_CanLoc2Id_hTbl[location_ptr->emscanport][location_ptr->canaddr][location_ptr->axis]); 
    if( NULL == item_ptr->ptr)
    {
        return(eores_NOK_nodata);
    }

    *jId_ptr = item_ptr->ptr->id;
    return(eores_OK);
}



extern eOresult_t eo_emsCanNetTopo_GetMotorId_ByMotorCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eOmc_motorId_t *mId_ptr)
{
    eo_emsCanNetTopo_hashTbl_jm_item_t *item_ptr;
    if((NULL == p) || (NULL == location_ptr) || (NULL == mId_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    item_ptr = &(p->motor_CanLoc2Id_hTbl[location_ptr->emscanport][location_ptr->canaddr][location_ptr->axis]); 
    if( NULL == item_ptr->ptr)
    {
        return(eores_NOK_nodata);
    }

    *mId_ptr = item_ptr->ptr->id;
    return(eores_OK);

}


extern eOresult_t eo_emsCanNetTopo_GetSensorId_BySensorCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_sensorCanLocation_t *location_ptr, eOsnsr_sensorId_t *sId_ptr)
{

    eo_emsCanNetTopo_hashTbl_s_item_t *item_ptr;
    if((NULL == p) || (NULL == location_ptr) || (NULL == sId_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    item_ptr = &(p->sensor_CanLoc2Id_hTbl[location_ptr->emscanport][location_ptr->canaddr]); 
    if( NULL == item_ptr->ptr)
    {
        return(eores_NOK_nodata);
    }

    *sId_ptr = item_ptr->ptr->id;
    return(eores_OK);
}


extern eOresult_t eo_emsCanNetTopo_GetConnectedJoints(EOemsCanNetTopo *p, EOarray *connectedJointsList)
{
    eOsizecntnr_t size;
    uint8_t i;

    eo_emsCanNetTopo_jointOrMotorTopoInfo_t *jTInfo;
    if((NULL == p) || (NULL == connectedJointsList))
    {
        return(eores_NOK_nullpointer);
    }

    //if no joints are connected to this board return ok and empty list 
    if((NULL == p->cfg.emsCanNetTopo_joints__ptr) || (0 == p->cfg.emsCanNetTopo_joints__ptr->size))
    {
        eo_array_Reset(connectedJointsList); //eo_fifoword_Clear(connectedJointsList, 0);
        return(eores_OK);
    }

    jTInfo = (eo_emsCanNetTopo_jointOrMotorTopoInfo_t *)p->cfg.emsCanNetTopo_joints__ptr->item_array_data;
    size = p->cfg.emsCanNetTopo_joints__ptr->size;

    for(i=0; i< size; i++)
    {
        eo_array_PushBack(connectedJointsList, &(jTInfo[i].id));    
    }

    return(eores_OK);

}
extern eOresult_t eo_emsCanNetTopo_GetConnectedMotors(EOemsCanNetTopo *p, EOarray *connectedMotorsList)
{
    eOsizecntnr_t size;
    uint8_t i;

    eo_emsCanNetTopo_jointOrMotorTopoInfo_t *mTInfo;
    if((NULL == p) || (NULL == connectedMotorsList))
    {
        return(eores_NOK_nullpointer);
    }

    //if no Motors are connected to this board return ok and empty list
    if((NULL == p->cfg.emsCanNetTopo_motors__ptr) || (0 == p->cfg.emsCanNetTopo_motors__ptr->size))
    {
        eo_array_Reset(connectedMotorsList); //eo_fifoword_Clear(connectedMotorsList, 0);
        return(eores_OK);
    }

    mTInfo = (eo_emsCanNetTopo_jointOrMotorTopoInfo_t *)p->cfg.emsCanNetTopo_motors__ptr->item_array_data;
    size = p->cfg.emsCanNetTopo_motors__ptr->size;

    for(i=0; i< size; i++)
    {
        eo_array_PushBack(connectedMotorsList, &(mTInfo[i].id));   
        //eo_fifoword_Put(connectedMotorsList, mTInfo[i].id, 0);    
    }

    return(eores_OK);
}


extern eOresult_t eo_emsCanNetTopo_GetConnectedSensors(EOemsCanNetTopo *p, EOarray *connectedSensorsList)
{
    eOsizecntnr_t size;
    uint8_t i;

    eo_emsCanNetTopo_sensorTopoInfo_t *sTInfo;
    if((NULL == p) || (NULL == connectedSensorsList))
    {
        return(eores_NOK_nullpointer);
    }

    //if no Motors are connected to this board return ok and empty list
    if((NULL == p->cfg.emsCanNetTopo_sensors__ptr) || (0 == p->cfg.emsCanNetTopo_sensors__ptr->size))
    {
        eo_array_Reset(connectedSensorsList); //eo_fifoword_Clear(connectedSensorsList, 0);
        return(eores_OK);
    }

    sTInfo = (eo_emsCanNetTopo_sensorTopoInfo_t *)p->cfg.emsCanNetTopo_sensors__ptr->item_array_data;
    size = p->cfg.emsCanNetTopo_sensors__ptr->size;

    for(i=0; i< size; i++)
    {
        eo_array_PushBack(connectedSensorsList, &(sTInfo[i].id)); 
        //eo_fifoword_Put(connectedSensorsList, sTInfo[i].id, 0);    
    }

    return(eores_OK);


}




extern eOresult_t eo_emsCanNetTopo_GetConnectedSkin(EOemsCanNetTopo *p, EOarray *connectedSkinList)
{
    eOsizecntnr_t size;
    uint8_t i;

    eo_emsCanNetTopo_sensorTopoInfo_t *skTInfo;
    if((NULL == p) || (NULL == connectedSkinList))
    {
        return(eores_NOK_nullpointer);
    }

    //if no Motors are connected to this board return ok and empty list
    if((NULL == p->cfg.emsCanNetTopo_skin__ptr) || (0 == p->cfg.emsCanNetTopo_skin__ptr->size))
    {
        eo_array_Reset(connectedSkinList); // eo_fifoword_Clear(connectedSkinList, 0);
        return(eores_OK);
    }

    skTInfo = (eo_emsCanNetTopo_sensorTopoInfo_t *)p->cfg.emsCanNetTopo_skin__ptr->item_array_data;
    size = p->cfg.emsCanNetTopo_skin__ptr->size;

    for(i=0; i< size; i++)
    {
        eo_array_PushBack(connectedSkinList, &(skTInfo[i].id));
        //eo_fifoword_Put(connectedSkinList, skTInfo[i].id, 0);    
    }

    return(eores_OK);

}











//extern eOresult_t eo_emsCanNetTopo_GetSkinCanPort(EOemsCanNetTopo *p, eOsk_skinId_t skId)
//{
//
//
//}

extern eOresult_t eo_emsCanNetTopo_GetskinCanLocation_BySkinId(EOemsCanNetTopo *p, eOsk_skinId_t skId, 
                                                               eo_emsCanNetTopo_sensorCanLocation_t *location_ptr)
{
    eo_emsCanNetTopo_sensorTopoInfo_t *sk_topoinfo_ptr = (eo_emsCanNetTopo_sensorTopoInfo_t*)(p->cfg.emsCanNetTopo_sensors__ptr->item_array_data);
    if((NULL == p) || (NULL == location_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if( NULL == p->cfg.emsCanNetTopo_skin__ptr)
    {
        return(eores_NOK_nodata);
    }

    sk_topoinfo_ptr = (eo_emsCanNetTopo_sensorTopoInfo_t*)(p->cfg.emsCanNetTopo_skin__ptr->item_array_data);
    location_ptr->emscanport = sk_topoinfo_ptr->canPort;

    /*Note: board addres is meanless*/
    location_ptr->canaddr = 0;

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
    s_eo_emsCanNetTopo_hashTbl_joint_init(p);
    s_eo_emsCanNetTopo_hashTbl_motor_init(p);
}


static void s_eo_emsCanNetTopo_hashTbl_joint_init(EOemsCanNetTopo *p)
{
    uint8_t i; //i indexes the motorBoardCanLoc2NvsRef lookup tbl
    eo_emsCanNetTopo_jointOrMotorTopoInfo_t *j_topoinfo_ptr = (eo_emsCanNetTopo_jointOrMotorTopoInfo_t*)(p->cfg.emsCanNetTopo_joints__ptr->item_array_data);

    //reset hashtbl
    memset(p->joint_Id2CanLoc_hTbl, 0x0, sizeof(p->joint_Id2CanLoc_hTbl));  //probabilmente non e' suff==> da rivedere   
    memset(p->joint_CanLoc2Id_hTbl, 0x0, sizeof(p->joint_CanLoc2Id_hTbl));  //probabilmente non e' suff==> da rivedere   
    

    for(i = 0; i< p->cfg.emsCanNetTopo_joints__ptr->size; i++)
    {
        p->joint_Id2CanLoc_hTbl[j_topoinfo_ptr[i].id].ptr =&j_topoinfo_ptr[i];
        p->joint_CanLoc2Id_hTbl[j_topoinfo_ptr[i].canPort][j_topoinfo_ptr[i].boardAddr][j_topoinfo_ptr[i].axis].ptr = &j_topoinfo_ptr[i]; 
    }

}

static void s_eo_emsCanNetTopo_hashTbl_motor_init(EOemsCanNetTopo *p)
{
    uint8_t i; //i indexes the motorBoardCanLoc2NvsRef lookup tbl
    eo_emsCanNetTopo_jointOrMotorTopoInfo_t *m_topoinfo_ptr = (eo_emsCanNetTopo_jointOrMotorTopoInfo_t*)(p->cfg.emsCanNetTopo_motors__ptr->item_array_data);

    //reset hashtbl
    memset(p->motor_Id2CanLoc_hTbl, 0x0, sizeof(p->motor_Id2CanLoc_hTbl));  //probabilmente non e' suff==> da rivedere   
    memset(p->motor_CanLoc2Id_hTbl, 0x0, sizeof(p->motor_CanLoc2Id_hTbl));  //probabilmente non e' suff==> da rivedere   
    

    for(i = 0; i< p->cfg.emsCanNetTopo_motors__ptr->size; i++)
    {
        p->motor_Id2CanLoc_hTbl[m_topoinfo_ptr[i].id].ptr =&m_topoinfo_ptr[i];
        p->motor_CanLoc2Id_hTbl[m_topoinfo_ptr[i].canPort][m_topoinfo_ptr[i].boardAddr][m_topoinfo_ptr[i].axis].ptr = &m_topoinfo_ptr[i]; 
    }

}

static void s_eo_emsCanNetTopo_hashTbl_sensor_init(EOemsCanNetTopo *p)
{
    uint8_t i; //i indexes the motorBoardCanLoc2NvsRef lookup tbl
    eo_emsCanNetTopo_sensorTopoInfo_t *s_topoinfo_ptr = (eo_emsCanNetTopo_sensorTopoInfo_t*)(p->cfg.emsCanNetTopo_sensors__ptr->item_array_data);

    //reset hashtbl
    memset(p->sensor_Id2CanLoc_hTbl, 0x0, sizeof(p->sensor_Id2CanLoc_hTbl));  //probabilmente non e' suff==> da rivedere   
    memset(p->sensor_CanLoc2Id_hTbl, 0x0, sizeof(p->sensor_CanLoc2Id_hTbl));  //probabilmente non e' suff==> da rivedere   
    

    for(i = 0; i< p->cfg.emsCanNetTopo_sensors__ptr->size; i++)
    {
        p->sensor_Id2CanLoc_hTbl[s_topoinfo_ptr[i].id].ptr =&s_topoinfo_ptr[i];
        p->sensor_CanLoc2Id_hTbl[s_topoinfo_ptr[i].canPort][s_topoinfo_ptr[i].boardAddr].ptr = &s_topoinfo_ptr[i]; 
    }

}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

