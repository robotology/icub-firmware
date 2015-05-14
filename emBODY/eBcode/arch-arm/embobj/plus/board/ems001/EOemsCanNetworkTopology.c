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
#include "eOcfg_nvsEP_sk.h"

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
static eOresult_t s_eo_emsCanNetTopo_hashTbl_skin_init(EOemsCanNetTopo *p);
static eOresult_t s_eo_emsCanNetTopo_checkConfiguration(eo_emsCanNetTopo_cfg_t *cfg);

//old functions
//static eOboolvalues_t s_eo_emsCanNetTopo_jointcfgIsValid(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointTopoInfo_t *j_topoinfo_ptr);
//static eOboolvalues_t s_eo_emsCanNetTopo_motorcfgIsValid(EOemsCanNetTopo *p, eo_emsCanNetTopo_motorTopoInfo_t *m_topoinfo_ptr);
//static void s_eo_emsCanNetTopo_addInfo_check(EOemsCanNetTopo *p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static const char s_eobj_ownname[] = "EOamsCanNetTopo";



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOemsCanNetTopo* eo_emsCanNetTopo_New(eo_emsCanNetTopo_cfg_t *cfg)
{
    eOresult_t res; 
    EOemsCanNetTopo *retptr = NULL;  
    
    if(NULL == cfg)
    {    
        return(retptr);
    }

    
    res = s_eo_emsCanNetTopo_checkConfiguration(cfg);
    if(eores_OK != res)
    {
        return(NULL);
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
    
    if( NULL == p->joint_Id2CanLoc_hTbl[jId].j_ptr)
    {
        return(eores_NOK_nodata);
    }

    location_ptr->canaddr = p->joint_Id2CanLoc_hTbl[jId].b_ptr->canaddr;
    location_ptr->jm_idInBoard = p->joint_Id2CanLoc_hTbl[jId].j_ptr->j_idInBoard;
    location_ptr->emscanport = p->joint_Id2CanLoc_hTbl[jId].b_ptr->emscanport;

    if(NULL !=boardType)
    {
        *boardType = p->joint_Id2CanLoc_hTbl[jId].b_ptr->boardtype;
    }
    return(eores_OK);
}

extern eOresult_t eo_emsCanNetTopo_GetMotorCanLocation_ByMotorId(EOemsCanNetTopo *p, eOmc_motorId_t mId, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eObrd_types_t *boardType)
{
    if((NULL == p) || (NULL == location_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if( NULL == p->motor_Id2CanLoc_hTbl[mId].m_ptr)
    {
        return(eores_NOK_nodata);
    }

    location_ptr->canaddr = p->motor_Id2CanLoc_hTbl[mId].b_ptr->canaddr;
    location_ptr->jm_idInBoard = p->motor_Id2CanLoc_hTbl[mId].m_ptr->m_idInBoard;
    location_ptr->emscanport = p->motor_Id2CanLoc_hTbl[mId].b_ptr->emscanport;

    if(NULL !=boardType)
    {
        *boardType = p->motor_Id2CanLoc_hTbl[mId].b_ptr->boardtype;
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
    
    
    if( (NULL != p->sensorStrain_hTbl.s_ptr) && (sId < strainNumberMAX) )
    {
        s_item_ptr = &p->sensorStrain_hTbl;
    }
    else if( (NULL != p->sensorMais_hTbl.s_ptr) && (sId < maisNumberMAX) )
    {
        s_item_ptr = &p->sensorMais_hTbl;
    }
    else
    {
        return(eores_NOK_nodata);
    }
    
    location_ptr->canaddr = s_item_ptr->b_ptr->canaddr;
    location_ptr->emscanport = s_item_ptr->b_ptr->emscanport;

    if(NULL !=boardType)
    {
        *boardType = s_item_ptr->b_ptr->boardtype;;
    }
    return(eores_OK);
}




extern eOresult_t eo_emsCanNetTopo_GetJointId_ByJointCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eOmc_jointId_t *jId_ptr)
{
    if((NULL == p) || (NULL == location_ptr) || (NULL == jId_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    

    *jId_ptr = p->joint_CanLoc2Id_hTbl[location_ptr->emscanport][location_ptr->canaddr][location_ptr->jm_idInBoard];
    if(joint_idNULL == *jId_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    return(eores_OK);
}



extern eOresult_t eo_emsCanNetTopo_GetMotorId_ByMotorCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eOmc_motorId_t *mId_ptr)
{
    if((NULL == p) || (NULL == location_ptr) || (NULL == mId_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    *mId_ptr = p->motor_CanLoc2Id_hTbl[location_ptr->emscanport][location_ptr->canaddr][location_ptr->jm_idInBoard];
    if(motor_idNULL == *mId_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    return(eores_OK);

}


extern eOresult_t eo_emsCanNetTopo_GetSensorId_BySensorCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_sensorCanLocation_t *location_ptr, eOsnsr_sensorId_t *sId_ptr)
{

    eo_emsCanNetTopo_hashTbl_s_item_t *s_item_ptr = NULL;
    
    
    if((NULL == p) || (NULL == location_ptr) || (NULL == sId_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    

    if( NULL != p->sensorStrain_hTbl.s_ptr )
    {
        s_item_ptr = &p->sensorStrain_hTbl;
    }
    else if( NULL != p->sensorMais_hTbl.s_ptr )
    {
        s_item_ptr = &p->sensorMais_hTbl;
    }
    else
    {
        return(eores_NOK_nodata);
    }

    if((s_item_ptr->b_ptr->emscanport == location_ptr->emscanport) && (s_item_ptr->b_ptr->canaddr == location_ptr->canaddr))
    {
        *sId_ptr = s_item_ptr->s_ptr->sid;
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

    eo_emsCanNetTopo_jointTopoInfo_t *jTInfo;
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

    jTInfo = (eo_emsCanNetTopo_jointTopoInfo_t *)p->cfg.emsCanNetTopo_joints__ptr->item_array_data;
    size = p->cfg.emsCanNetTopo_joints__ptr->size;

    for(i=0; i< size; i++)
    {
        eo_array_PushBack(connectedJointsList, &(jTInfo[i].jid));    
    }

    return(eores_OK);

}
extern eOresult_t eo_emsCanNetTopo_GetConnectedMotors(EOemsCanNetTopo *p, EOarray *connectedMotorsList)
{
    eOsizecntnr_t size;
    uint8_t i;

    eo_emsCanNetTopo_motorTopoInfo_t *mTInfo;
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

    mTInfo = (eo_emsCanNetTopo_motorTopoInfo_t *)p->cfg.emsCanNetTopo_motors__ptr->item_array_data;
    size = p->cfg.emsCanNetTopo_motors__ptr->size;

    for(i=0; i< size; i++)
    {
        eo_array_PushBack(connectedMotorsList, &(mTInfo[i].mid));       
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
        eo_array_PushBack(connectedSensorsList, &(sTInfo[i].sid));  
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
        eo_array_PushBack(connectedSkinList, &(skTInfo[i].sid));    
    }

    return(eores_OK);

}



extern eOresult_t eo_emsCanNetTopo_GetskinCanLocation_BySkinId(EOemsCanNetTopo *p, eOsk_skinId_t skId, 
                                                               eo_emsCanNetTopo_sensorCanLocation_t *location_ptr)
{
    eo_emsCanNetTopo_sensorTopoInfo_t *sk_topoinfo_ptr = (eo_emsCanNetTopo_sensorTopoInfo_t*)(p->cfg.emsCanNetTopo_sensors__ptr->item_array_data);
    eo_emsCanNetTopo_boardTopoInfo_t  *b_topoinfo_ptr = (eo_emsCanNetTopo_boardTopoInfo_t*)(p->cfg.emsCanNetTopo_canBoards__ptr->item_array_data);

    if((NULL == p) || (NULL == location_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if( NULL == p->cfg.emsCanNetTopo_skin__ptr)
    {
        return(eores_NOK_nodata);
    }

    sk_topoinfo_ptr = (eo_emsCanNetTopo_sensorTopoInfo_t*)(p->cfg.emsCanNetTopo_skin__ptr->item_array_data);

    if(sk_topoinfo_ptr->bid < p->cfg.emsCanNetTopo_canBoards__ptr->size)
    {
        location_ptr->emscanport = b_topoinfo_ptr[sk_topoinfo_ptr->bid].emscanport;
    }
    else
    {
     return(eores_NOK_generic);
    }

    /*Note: board addres is meanless*/
    location_ptr->canaddr = 0;

    return(eores_OK);

}


extern const EOconstvector* eo_emsCanNetTopo_GetCfgCanBoards(EOemsCanNetTopo *p)
{
    if(NULL == p)
    {
        return(NULL);
    }
    return(p->cfg.emsCanNetTopo_canBoards__ptr);
}




extern eOresult_t eo_emsCanNetTopo_GetBoardOfJoint(EOemsCanNetTopo *p, eOmc_jointId_t jId, eObrd_types_t *boardType)
{
    if((NULL == p) || (NULL == boardType))
    {
        return(eores_NOK_nullpointer);
    }
    
    if( NULL == p->joint_Id2CanLoc_hTbl[jId].j_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    *boardType = p->joint_Id2CanLoc_hTbl[jId].b_ptr->boardtype;

    return(eores_OK);
}

extern eOresult_t eo_emsCanNetTopo_GetBoardOfMotor(EOemsCanNetTopo *p, eOmc_motorId_t mId, eObrd_types_t *boardType)
{
    if((NULL == p) || (NULL == boardType))
    {
        return(eores_NOK_nullpointer);
    }
    
    if( NULL == p->motor_Id2CanLoc_hTbl[mId].m_ptr)
    {
        return(eores_NOK_nodata);
    }

    *boardType = p->motor_Id2CanLoc_hTbl[mId].b_ptr->boardtype;

    return(eores_OK);
}



extern eOresult_t eo_emsCanNetTopo_GetBoardOfSensor(EOemsCanNetTopo *p, eOsnsr_sensorId_t sId, eObrd_types_t *boardType)
{
    eo_emsCanNetTopo_hashTbl_s_item_t *s_item_ptr = NULL;
    
    if((NULL == p) || (NULL == boardType))
    {
        return(eores_NOK_nullpointer);
    }
    
    
    if( (NULL != p->sensorStrain_hTbl.s_ptr) && (sId < strainNumberMAX) )
    {
        s_item_ptr = &p->sensorStrain_hTbl;
    }
    else if( (NULL != p->sensorMais_hTbl.s_ptr) && (sId < maisNumberMAX) )
    {
        s_item_ptr = &p->sensorMais_hTbl;
    }
    else
    {
        return(eores_NOK_nodata);
    }
    
    *boardType = s_item_ptr->b_ptr->boardtype;;

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
    
    res = s_eo_emsCanNetTopo_hashTbl_skin_init(p);
    if(eores_OK != res)
    {
        return(res);
    }
    
    
    return(eores_OK);
}


static eOresult_t s_eo_emsCanNetTopo_hashTbl_joint_init(EOemsCanNetTopo *p)
{
    uint8_t i,j,k; //i indexes the motorBoardCanLoc2NvsRef lookup tbl
    eo_emsCanNetTopo_jointTopoInfo_t *j_topoinfo_ptr;
    eo_emsCanNetTopo_boardTopoInfo_t *b_topoinfo_ptr;
    eo_emsCanNetTopo_boardTopoInfo_t *b_aux_ptr;
    
    // 1) reset hashtbl
    memset(p->joint_Id2CanLoc_hTbl, 0x0, sizeof(p->joint_Id2CanLoc_hTbl));  
    for(i=0; i<eo_emsCanNetTopo_canports_num; i++)
    {
        for(j=0; j<MAX_CAN_ADDRESS; j++)
        {
            for(k=0; k<eo_emsCanNetTopo_jm_indexInBoard_max; k++)
            {
                p->joint_CanLoc2Id_hTbl[i][j][k] = joint_idNULL;
            }
        }
    }

   
    // 2) init hastbl   if joints are cfg
    if((NULL == p->cfg.emsCanNetTopo_joints__ptr) || (0 == p->cfg.emsCanNetTopo_joints__ptr->size)) 
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "no joints are config"); 
        return(eores_OK);
    } 
 
    j_topoinfo_ptr = (eo_emsCanNetTopo_jointTopoInfo_t*)(p->cfg.emsCanNetTopo_joints__ptr->item_array_data);
    b_topoinfo_ptr = (eo_emsCanNetTopo_boardTopoInfo_t*)(p->cfg.emsCanNetTopo_canBoards__ptr->item_array_data);

    for(i = 0; i< p->cfg.emsCanNetTopo_joints__ptr->size; i++)
    {
        b_aux_ptr = &b_topoinfo_ptr[j_topoinfo_ptr[i].bid];
        p->joint_Id2CanLoc_hTbl[j_topoinfo_ptr[i].jid].j_ptr = &j_topoinfo_ptr[i];
        p->joint_Id2CanLoc_hTbl[j_topoinfo_ptr[i].jid].b_ptr = b_aux_ptr;
        p->joint_CanLoc2Id_hTbl[b_aux_ptr->emscanport][b_aux_ptr->canaddr][j_topoinfo_ptr[i].j_idInBoard] = j_topoinfo_ptr[i].jid; 
    }
    
    return(eores_OK);
}

// static eOboolvalues_t s_eo_emsCanNetTopo_jointcfgIsValid(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointTopoInfo_t *j_topoinfo_ptr)
// {
//     if( (j_topoinfo_ptr->boardAddr > 0xF) || (j_topoinfo_ptr->id >= jointNumberMAX)  
//         || (j_topoinfo_ptr->bid >= p->cfg.emsCanNetTopo_canBoards__ptr->size) )
//     {
//         return(eobool_false);
//     }

//     return(eobool_true);
// }
static eOresult_t s_eo_emsCanNetTopo_hashTbl_motor_init(EOemsCanNetTopo *p)
{
    uint8_t i, j, k; //i indexes the motorBoardCanLoc2NvsRef lookup tbl
    eo_emsCanNetTopo_motorTopoInfo_t *m_topoinfo_ptr;
    eo_emsCanNetTopo_boardTopoInfo_t *b_topoinfo_ptr;
    eo_emsCanNetTopo_boardTopoInfo_t *b_aux_ptr;

    // 1) reset hashtbl
    memset(p->motor_Id2CanLoc_hTbl, 0x0, sizeof(p->motor_Id2CanLoc_hTbl)); 
    for(i=0; i<eo_emsCanNetTopo_canports_num; i++)
    {
        for(j=0; j<MAX_CAN_ADDRESS; j++)
        {
            for(k=0; k<eo_emsCanNetTopo_jm_indexInBoard_max; k++)
            {
                p->motor_CanLoc2Id_hTbl[i][j][k] = motor_idNULL;
            }
        }
    }

    // 2) init hastbl if motors are cfg   
    if((NULL == p->cfg.emsCanNetTopo_motors__ptr) || (0 == p->cfg.emsCanNetTopo_motors__ptr->size)) 
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "no motors are config"); 
        return(eores_OK);
    }

    m_topoinfo_ptr = (eo_emsCanNetTopo_motorTopoInfo_t*)(p->cfg.emsCanNetTopo_motors__ptr->item_array_data);
    b_topoinfo_ptr = (eo_emsCanNetTopo_boardTopoInfo_t*)(p->cfg.emsCanNetTopo_canBoards__ptr->item_array_data);

    for(i = 0; i< p->cfg.emsCanNetTopo_motors__ptr->size; i++)
    {
        b_aux_ptr = &b_topoinfo_ptr[m_topoinfo_ptr[i].bid];
        p->motor_Id2CanLoc_hTbl[m_topoinfo_ptr[i].mid].m_ptr = &m_topoinfo_ptr[i];
        p->motor_Id2CanLoc_hTbl[m_topoinfo_ptr[i].mid].b_ptr = b_aux_ptr;
        p->motor_CanLoc2Id_hTbl[b_aux_ptr->emscanport][b_aux_ptr->canaddr][m_topoinfo_ptr[i].m_idInBoard]= m_topoinfo_ptr[i].mid; 
    }
    
    return(eores_OK);
}

// static eOboolvalues_t s_eo_emsCanNetTopo_motorcfgIsValid(EOemsCanNetTopo *p, eo_emsCanNetTopo_motorTopoInfo_t *m_topoinfo_ptr)
// {
//     if( (m_topoinfo_ptr->boardAddr > 0xF) || (m_topoinfo_ptr->id >= motorNumberMAX) 
//         || (j_topoinfo_ptr->bid >= p->cfg.emsCanNetTopo_canBoards__ptr->size) )
//     {
//         return(eobool_false);
//     }

//     return(eobool_true);
// }



static eOresult_t s_eo_emsCanNetTopo_checkConfiguration(eo_emsCanNetTopo_cfg_t *cfg)
{
    uint16_t i;
    eo_emsCanNetTopo_boardTopoInfo_t *b_topoinfo_ptr;
    eo_emsCanNetTopo_motorTopoInfo_t *m_topoinfo_ptr;
    eo_emsCanNetTopo_jointTopoInfo_t *j_topoinfo_ptr;
    eo_emsCanNetTopo_sensorTopoInfo_t *s_topoinfo_ptr;

    // 1) verify almost one bord is connected to ems ==> else no emsCanNetTopo shuld be exist
    if( (NULL == cfg->emsCanNetTopo_canBoards__ptr) || (0 == cfg->emsCanNetTopo_canBoards__ptr->size) )
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "no can boards are cofig");
        return(eores_NOK_generic);
    }
    b_topoinfo_ptr = (eo_emsCanNetTopo_boardTopoInfo_t*)(cfg->emsCanNetTopo_canBoards__ptr->item_array_data);
    
    // 2) verify can boards cfg
    for(i=0; i<cfg->emsCanNetTopo_canBoards__ptr->size; i++)
    {
        if( (b_topoinfo_ptr[i].canaddr == 0) || (b_topoinfo_ptr[i].canaddr >= 0xF)) //check can addr range
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "can addr is not valid");
            return(eores_NOK_generic);
        }                
    }
    
    // 3) verify joint cfg
    if(NULL != cfg->emsCanNetTopo_joints__ptr)
    {
        j_topoinfo_ptr = (eo_emsCanNetTopo_jointTopoInfo_t*)(cfg->emsCanNetTopo_joints__ptr->item_array_data);
        for(i=0; i<cfg->emsCanNetTopo_joints__ptr->size; i++)
        {
            if( (j_topoinfo_ptr[i].jid >= jointNumberMAX ) || (j_topoinfo_ptr[i].bid >= cfg->emsCanNetTopo_canBoards__ptr->size ) )
            {
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "joint cfg is not valid");
                return(eores_NOK_generic);
            }                
        }
    }

    // 4) verify motor cfg
    if(NULL != cfg->emsCanNetTopo_motors__ptr)
    {
        m_topoinfo_ptr = (eo_emsCanNetTopo_motorTopoInfo_t*)(cfg->emsCanNetTopo_motors__ptr->item_array_data);
        for(i=0; i<cfg->emsCanNetTopo_motors__ptr->size; i++)
        {
            if( (m_topoinfo_ptr[i].mid >= motorNumberMAX ) || (m_topoinfo_ptr[i].bid >= cfg->emsCanNetTopo_canBoards__ptr->size ) )
            {
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "motor cfg is not valid");
                return(eores_NOK_generic);
            }                
        }
    }
    
    // 5) verify sensor cfg
    if(NULL != cfg->emsCanNetTopo_sensors__ptr)
    {
        s_topoinfo_ptr = (eo_emsCanNetTopo_sensorTopoInfo_t*)(cfg->emsCanNetTopo_sensors__ptr->item_array_data);
        if( (cfg->emsCanNetTopo_sensors__ptr->size > 1) ||  (s_topoinfo_ptr[0].bid > cfg->emsCanNetTopo_canBoards__ptr->size) )
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "error in sensor cfg"); 
            return(eores_NOK_generic);
        }
        
        //check if sensor is mais or strain
        if((eobrd_strain == b_topoinfo_ptr[s_topoinfo_ptr[0].bid].boardtype) && (s_topoinfo_ptr[0].sid >= strainNumberMAX) )
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "error in strain cfg"); 
            return(eores_NOK_generic);
        }
        
        if((eobrd_mais == b_topoinfo_ptr[s_topoinfo_ptr[0].bid].boardtype) && (s_topoinfo_ptr[0].sid >= maisNumberMAX) )
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "error in mais cfg"); 
            return(eores_NOK_generic);
        }
        
    }

    
    // 6) verify skin cfg
    if(NULL != cfg->emsCanNetTopo_skin__ptr)
    {
        s_topoinfo_ptr = (eo_emsCanNetTopo_sensorTopoInfo_t*)(cfg->emsCanNetTopo_skin__ptr->item_array_data);
        if( (cfg->emsCanNetTopo_skin__ptr->size > 1) || (s_topoinfo_ptr[0].sid > skinNumberMAX) || 
            (s_topoinfo_ptr[0].bid > cfg->emsCanNetTopo_canBoards__ptr->size) || (b_topoinfo_ptr[s_topoinfo_ptr[0].bid].boardtype !=  eobrd_skin) )
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "error in skin cfg"); 
            return(eores_NOK_generic);
        }
    }

    return(eores_OK);
}


static eOresult_t s_eo_emsCanNetTopo_hashTbl_sensor_init(EOemsCanNetTopo *p)
{
    //uint8_t i; //i indexes the motorBoardCanLoc2NvsRef lookup tbl
    eo_emsCanNetTopo_sensorTopoInfo_t *s_topoinfo_ptr;
    eo_emsCanNetTopo_boardTopoInfo_t *b_topoinfo_ptr;

    //reset hashtbl
     memset(&p->sensorStrain_hTbl, 0x0, sizeof(p->sensorStrain_hTbl));  
     memset(&p->sensorMais_hTbl, 0x0, sizeof(p->sensorMais_hTbl)); 

    if((NULL == p->cfg.emsCanNetTopo_sensors__ptr) || (0 == p->cfg.emsCanNetTopo_sensors__ptr->size))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, s_eobj_ownname, "no can-sensors are config"); 
        return(eores_OK); //some ems hasn't sensors
    }
    
    s_topoinfo_ptr = (eo_emsCanNetTopo_sensorTopoInfo_t*)(p->cfg.emsCanNetTopo_sensors__ptr->item_array_data);
    b_topoinfo_ptr = (eo_emsCanNetTopo_boardTopoInfo_t*)(p->cfg.emsCanNetTopo_canBoards__ptr->item_array_data);
    
    
    switch(b_topoinfo_ptr[s_topoinfo_ptr->bid].boardtype)
    {
        case eobrd_strain:
        {
            p->sensorStrain_hTbl.s_ptr =&s_topoinfo_ptr[0];
            p->sensorStrain_hTbl.b_ptr =&b_topoinfo_ptr[s_topoinfo_ptr[0].bid];
        }break;
        
        case eobrd_mais:
        {
            p->sensorMais_hTbl.s_ptr =&s_topoinfo_ptr[0];
            p->sensorMais_hTbl.b_ptr =&b_topoinfo_ptr[s_topoinfo_ptr[0].bid];   
        }break;
        
        default:
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "error in sensors' cfg");
            return(eores_NOK_generic);
        }
    };
    
//     if(eobrd_strain == b_topoinfo_ptr[s_topoinfo_ptr->bid].boardType) 
//     {
//         if(s_topoinfo_ptr[0].sid >= strainNumberMAX)
//         {
//             eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "strain id bigger than strainMaxNum");
//             return(eores_NOK_generic);
//         }
//             p->sensorStrain_hTbl.s_ptr =&s_topoinfo_ptr[0];
//             p->sensorStrain_hTbl.b_ptr =&b_topoinfo_ptr[s_topoinfo_ptr[0].bid];
//     }
//         
//     else if( (s_topoinfo_ptr[0].bid < p->cfg.emsCanNetTopo_canBoards__ptr->size) && (eobrd_mais == b_topoinfo_ptr[s_topoinfo_ptr->bid].boardType) )
//     {
//         if(s_topoinfo_ptr[0].sid >= maisNumberMAX)
//         {
//             eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "mais id bigger than maisnMaxNum");
//             return(eores_NOK_generic);
//         }
//         p->sensorMais_hTbl.s_ptr =&s_topoinfo_ptr[0];
//         p->sensorMais_hTbl.b_ptr =&b_topoinfo_ptr[s_topoinfo_ptr[0].bid];
//     }
//     else
//     {
//         eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "error in sensors' cfg");
//         return(eores_NOK_generic);
//     }
//     
    

    return(eores_OK);
}


static eOresult_t s_eo_emsCanNetTopo_hashTbl_skin_init(EOemsCanNetTopo *p)
{
    
//     /*NOTE: currently the application does not parse skin can frame, so it is not ncessary keep all can network information 
//     about skin board, but only emscanport. this function is useful to validate skin config */
//     eo_emsCanNetTopo_sensorTopoInfo_t *s_topoinfo_ptr = (eo_emsCanNetTopo_sensorTopoInfo_t*)(p->cfg.emsCanNetTopo_skin__ptr->item_array_data);
//     eo_emsCanNetTopo_boardTopoInfo_t *b_topoinfo_ptr = (eo_emsCanNetTopo_boardTopoInfo_t*)(p->cfg.emsCanNetTopo_canBoards__ptr->item_array_data);
//     
//     if((NULL == s_topoinfo_ptr) || (0 == p->cfg.emsCanNetTopo_sensors__ptr->size))
//     {
//         eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, s_eobj_ownname, "no skin is config"); 
//         return(eores_OK); //some ems hasn't skin
//     }

//     //this obj works with only one sensor per ems. if more sensors are configured, returns error    
//     if(1 != p->cfg.emsCanNetTopo_sensors__ptr->size)
//     {
//         eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "more than 1 skin are config"); 
//         return(eores_NOK_generic);
//     }
//     
//     
//     if( (s_topoinfo_ptr[0].bid < p->cfg.emsCanNetTopo_canBoards__ptr->size) && (eobrd_sskin == b_topoinfo_ptr[s_topoinfo_ptr->bid].boardType) )
//     {
//         return(eores_OK);
//     }
//     else
//     {
//         eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "error in skin's cfg");
//         return(eores_NOK_generic);
//     }
    return(eores_OK);
}

//-----------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

