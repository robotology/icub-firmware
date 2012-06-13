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
#include "EOtheErrorManager.h"
#include "EOconstvector.h"
#include "EOconstvector_hid.h"

#include "eOcfg_nvsEP_mc.h"
#include "eOcfg_nvsEP_as.h"


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
static eOresult_t s_eo_emsCanNetTopo_hashTbls_init(EOemsCanNetTopo *p);
static eOresult_t s_eo_emsCanNetTopo_hashTbl_joint_init(EOemsCanNetTopo *p);
static eOresult_t s_eo_emsCanNetTopo_hashTbl_motor_init(EOemsCanNetTopo *p);
static eOresult_t s_eo_emsCanNetTopo_hashTbl_sensor_init(EOemsCanNetTopo *p);
static eOboolvalues_t s_eo_emsCanNetTopo_jointcfgIsValid(eo_emsCanNetTopo_jointOrMotorTopoInfo_t *j_topoinfo_ptr);
static eOboolvalues_t s_eo_emsCanNetTopo_motorcfgIsValid(eo_emsCanNetTopo_jointOrMotorTopoInfo_t *j_topoinfo_ptr);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static const char s_eobj_ownname[] = "EOamsCanNetTopo";



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
    eo_emsCanNetTopo_hashTbl_s_item_t *s_item_ptr = NULL;
    
    if((NULL == p) || (NULL == location_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    
    if( (NULL != p->sensorStrain_hTbl.ptr) && (sId < strainNumberMAX) )
    {
        s_item_ptr = &p->sensorStrain_hTbl;
    }
    else if( (NULL != p->sensorMais_hTbl.ptr) && (sId < maisNumberMAX) )
    {
        s_item_ptr = &p->sensorMais_hTbl;
    }
    else
    {
        return(eores_NOK_nodata);
    }
    
    location_ptr->canaddr = s_item_ptr->ptr->boardAddr;
    location_ptr->emscanport = s_item_ptr->ptr->canPort;

    if(NULL !=boardType)
    {
        *boardType = s_item_ptr->ptr->boardType;
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

    eo_emsCanNetTopo_hashTbl_s_item_t *s_item_ptr = NULL;
    
    
    if((NULL == p) || (NULL == location_ptr) || (NULL == sId_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    

    if( NULL != p->sensorStrain_hTbl.ptr )
    {
        s_item_ptr = &p->sensorStrain_hTbl;
    }
    else if( NULL != p->sensorMais_hTbl.ptr )
    {
        s_item_ptr = &p->sensorMais_hTbl;
    }
    else
    {
        return(eores_NOK_nodata);
    }

    if((s_item_ptr->ptr->canPort == location_ptr->emscanport) && (s_item_ptr->ptr->boardAddr == location_ptr->canaddr))
    {
        *sId_ptr = s_item_ptr->ptr->id;
        return(eores_OK);
    }
    else
    {
        return(eores_NOK_nodata);
    }

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
        eo_array_Reset(connectedJointsList);
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
        eo_array_Reset(connectedMotorsList);
        return(eores_OK);
    }

    mTInfo = (eo_emsCanNetTopo_jointOrMotorTopoInfo_t *)p->cfg.emsCanNetTopo_motors__ptr->item_array_data;
    size = p->cfg.emsCanNetTopo_motors__ptr->size;

    for(i=0; i< size; i++)
    {
        eo_array_PushBack(connectedMotorsList, &(mTInfo[i].id));       
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
        eo_array_Reset(connectedSensorsList); 
        return(eores_OK);
    }

    sTInfo = (eo_emsCanNetTopo_sensorTopoInfo_t *)p->cfg.emsCanNetTopo_sensors__ptr->item_array_data;
    size = p->cfg.emsCanNetTopo_sensors__ptr->size;

    for(i=0; i< size; i++)
    {
        eo_array_PushBack(connectedSensorsList, &(sTInfo[i].id));  
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


static eOresult_t s_eo_emsCanNetTopo_hashTbls_init(EOemsCanNetTopo *p)
{
    eOresult_t res;
    
    res = s_eo_emsCanNetTopo_hashTbl_joint_init(p);
    if(eores_OK != res)
    {
        return(res);
    }
    
    res = s_eo_emsCanNetTopo_hashTbl_motor_init(p);
    if(eores_OK != res)
    {
        return(res);
    }
    
    res = s_eo_emsCanNetTopo_hashTbl_sensor_init(p);
    if(eores_OK != res)
    {
        return(res);
    }
    
    return(eores_OK);
}


static eOresult_t s_eo_emsCanNetTopo_hashTbl_joint_init(EOemsCanNetTopo *p)
{
    uint8_t i; //i indexes the motorBoardCanLoc2NvsRef lookup tbl
    eo_emsCanNetTopo_jointOrMotorTopoInfo_t *j_topoinfo_ptr = (eo_emsCanNetTopo_jointOrMotorTopoInfo_t*)(p->cfg.emsCanNetTopo_joints__ptr->item_array_data);

    //reset hashtbl
    memset(p->joint_Id2CanLoc_hTbl, 0x0, sizeof(p->joint_Id2CanLoc_hTbl));  //probabilmente non e' suff==> da rivedere   
    memset(p->joint_CanLoc2Id_hTbl, 0x0, sizeof(p->joint_CanLoc2Id_hTbl));  //probabilmente non e' suff==> da rivedere   
    
    if((NULL == j_topoinfo_ptr) || (0 == p->cfg.emsCanNetTopo_joints__ptr->size)) 
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "no joints are config"); 
        return(eores_OK);
    }
    
    for(i = 0; i< p->cfg.emsCanNetTopo_joints__ptr->size; i++)
    {
        if( !s_eo_emsCanNetTopo_jointcfgIsValid(&j_topoinfo_ptr[i]))
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "joint id is bigger than jointNumberMAX");
            return(eores_NOK_generic);
        }
        p->joint_Id2CanLoc_hTbl[j_topoinfo_ptr[i].id].ptr =&j_topoinfo_ptr[i];
        p->joint_CanLoc2Id_hTbl[j_topoinfo_ptr[i].canPort][j_topoinfo_ptr[i].boardAddr][j_topoinfo_ptr[i].axis].ptr = &j_topoinfo_ptr[i]; 
    }
    
    return(eores_OK);
}

static eOboolvalues_t s_eo_emsCanNetTopo_jointcfgIsValid(eo_emsCanNetTopo_jointOrMotorTopoInfo_t *j_topoinfo_ptr)
{
    if( (j_topoinfo_ptr->boardAddr > 0xF) || (j_topoinfo_ptr->axis > 1) || (j_topoinfo_ptr->id >= jointNumberMAX) )
    {
        return(eobool_false);
    }

    return(eobool_true);
}
static eOresult_t s_eo_emsCanNetTopo_hashTbl_motor_init(EOemsCanNetTopo *p)
{
    uint8_t i; //i indexes the motorBoardCanLoc2NvsRef lookup tbl
    eo_emsCanNetTopo_jointOrMotorTopoInfo_t *m_topoinfo_ptr = (eo_emsCanNetTopo_jointOrMotorTopoInfo_t*)(p->cfg.emsCanNetTopo_motors__ptr->item_array_data);

    //reset hashtbl
    memset(p->motor_Id2CanLoc_hTbl, 0x0, sizeof(p->motor_Id2CanLoc_hTbl));  //probabilmente non e' suff==> da rivedere   
    memset(p->motor_CanLoc2Id_hTbl, 0x0, sizeof(p->motor_CanLoc2Id_hTbl));  //probabilmente non e' suff==> da rivedere   
    
    if((NULL == m_topoinfo_ptr) || (0 == p->cfg.emsCanNetTopo_motors__ptr->size)) 
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "no motors are config"); 
        return(eores_OK);
    }

    for(i = 0; i< p->cfg.emsCanNetTopo_motors__ptr->size; i++)
    {
        if( !s_eo_emsCanNetTopo_motorcfgIsValid(&m_topoinfo_ptr[i]))
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "motor id is bigger than motorNumberMAX");
            return(eores_NOK_generic);
        }

        p->motor_Id2CanLoc_hTbl[m_topoinfo_ptr[i].id].ptr =&m_topoinfo_ptr[i];
        p->motor_CanLoc2Id_hTbl[m_topoinfo_ptr[i].canPort][m_topoinfo_ptr[i].boardAddr][m_topoinfo_ptr[i].axis].ptr = &m_topoinfo_ptr[i]; 
    }
    
    return(eores_OK);
}

static eOboolvalues_t s_eo_emsCanNetTopo_motorcfgIsValid(eo_emsCanNetTopo_jointOrMotorTopoInfo_t *m_topoinfo_ptr)
{
    if( (m_topoinfo_ptr->boardAddr > 0xF) || (m_topoinfo_ptr->axis > 1) || (m_topoinfo_ptr->id >= motorNumberMAX) )
    {
        return(eobool_false);
    }

    return(eobool_true);
}


static eOresult_t s_eo_emsCanNetTopo_hashTbl_sensor_init(EOemsCanNetTopo *p)
{
    //uint8_t i; //i indexes the motorBoardCanLoc2NvsRef lookup tbl
    eo_emsCanNetTopo_sensorTopoInfo_t *s_topoinfo_ptr = (eo_emsCanNetTopo_sensorTopoInfo_t*)(p->cfg.emsCanNetTopo_sensors__ptr->item_array_data);

    //reset hashtbl
//     memset(&p->sensorStrain_hTbl, 0x0, sizeof(p->sensorStrain_hTbl));  //probabilmente non e' suff==> da rivedere   
//     memset(&p->sensorMais_hTbl, 0x0, sizeof(p->sensorMais_hTbl));  //probabilmente non e' suff==> da rivedere   
    
    p->sensorStrain_hTbl.ptr = NULL;
    p->sensorMais_hTbl.ptr = NULL;
    
    if((NULL == s_topoinfo_ptr) || (0 == p->cfg.emsCanNetTopo_sensors__ptr->size))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, s_eobj_ownname, "no can-sensors are config"); 
        return(eores_OK); //some ems hasn't sensors
    }

    //this obj works with only one sensor per ems. if more sensors are configured, returns error    
    if(1 != p->cfg.emsCanNetTopo_sensors__ptr->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "more than 1 sensors are config"); 
        return(eores_NOK_generic);
    }



    
    if(eobrd_strain == s_topoinfo_ptr->boardType)
    {
        if(s_topoinfo_ptr[0].id >= strainNumberMAX)
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "strain id bigger than strainMaxNum");
            return(eores_NOK_generic);
        }
            p->sensorStrain_hTbl.ptr =&s_topoinfo_ptr[0];
    }
        
    else if(eobrd_mais == s_topoinfo_ptr->boardType)
    {
        if(s_topoinfo_ptr[0].id >= maisNumberMAX)
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "mais id bigger than maisnMaxNum");
            return(eores_NOK_generic);
        }
        p->sensorMais_hTbl.ptr =&s_topoinfo_ptr[0];
    }
    else
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "sensor boards type unknown");
        return(eores_NOK_generic);
    }
    
    

    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

