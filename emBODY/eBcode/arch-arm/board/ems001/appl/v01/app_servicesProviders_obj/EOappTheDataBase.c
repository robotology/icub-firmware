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

// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/* @file       EOappDataBase.c
    @brief      This file implements data base obj.
    @author    valentina.gaggero@iit.it
    @date       09/11/2012
**/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"
#include "string.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
//#include "EOnvsCfg.h"

#include "EOconstvector_hid.h" 
#include "EOnv_hid.h"

// #include "eOcfg_nvsEP_mc.h"
// #include "eOcfg_nvsEP_as.h"
// #include "eOcfg_nvsEP_sk.h"

#include "EOMtheEMSapplCfg_cfg.h" //here is define type of ems used
#ifdef USE_PROTO_PROXY
#include "EOlist_hid.h"
#endif
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOappTheDataBase.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOappTheDataBase_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define db_emscanportconnected2motorboard     eOcanport1 
//#define DB_NULL_VALUE_U16                     0xFFFF
#define DB_NULL_VALUE_U08                     0xFF
#ifdef USE_PROTO_PROXY
#define db_numOfEthProtoReq_for_entity        2
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static eObool_t s_appTheDB_checkConfiguaration(eOappTheDB_cfg_t *cfg);
static eOresult_t s_appTheDB_canboardslist_init(EOappTheDB *p);
static eOresult_t s_appTheDB_jointslist_init(EOappTheDB *p);
static eOresult_t s_appTheDB_motorslist_init(EOappTheDB *p);
static eOresult_t s_appTheDB_snsrMaislist_init(EOappTheDB *p);
static eOresult_t s_appTheDB_snsrStrainlist_init(EOappTheDB *p);
static eOresult_t s_appTheDB_skinlist_init(EOappTheDB *p);
static eOresult_t s_appTheDB_canaddressLookuptbl_init(EOappTheDB *p);
#ifdef USE_PROTO_PROXY
static EOlist * s_appTheDB_getEthProtoReqList(EOappTheDB *p, eOprotEndpoint_t ep, eOprotEntity_t entity, eOprotIndex_t index);
static eOresult_t s_appTheDB_searchEthProtoReq_matchingRule(void *item, void *param);
#endif

//static eOresult_t s_appTheDB_nvsramref_init(EOappTheDB *p);

static eOresult_t s_appTheDB_virtualStrainData_init(EOappTheDB *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static EOappTheDB s_db = 
{
    EO_INIT(.isinitted)    eobool_false
};

static const char s_eobj_ownname[] = "EOappTheDB";

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOappTheDB* eo_appTheDB_Initialise(eOappTheDB_cfg_t *cfg)
{
    eOresult_t res;
    EOappTheDB  *retptr;
    
    if(NULL == cfg)
    {
        return(NULL);
    }
    
    if(!s_appTheDB_checkConfiguaration(cfg))
    {
        return(NULL);
    }
    
    retptr = &s_db;
    
    memcpy(&retptr->cfg, cfg, sizeof(eOappTheDB_cfg_t));
    
    
    res = s_appTheDB_canboardslist_init(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }
    
    res = s_appTheDB_canaddressLookuptbl_init(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }
    
    res = s_appTheDB_jointslist_init(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }
    
    res = s_appTheDB_motorslist_init(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }

    res = s_appTheDB_snsrMaislist_init(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }

    res = s_appTheDB_snsrStrainlist_init(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }
    
    res = s_appTheDB_skinlist_init(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }

//     //res = s_appTheDB_nvsrefmaps_init(retptr);
//     res = s_appTheDB_nvsramref_init(retptr);
//     if(eores_OK != res)
//     {
//         return(NULL);
//     }
    
    s_appTheDB_virtualStrainData_init(retptr);

    retptr->isinitted = eobool_true;
    
    return(retptr);
}

extern EOappTheDB* eo_appTheDB_GetHandle(void)
{
    return((s_db.isinitted) ? (&s_db) : NULL);
}


extern uint16_t eo_appTheDB_GetNumeberOfConnectedJoints(EOappTheDB *p)
{
    return(eo_array_Capacity(p->jointsList));
}


extern uint16_t eo_appTheDB_GetNumeberOfConnectedMotors(EOappTheDB *p)
{
    return(eo_array_Capacity(p->motorsList));
}


extern uint16_t eo_appTheDB_GetNumeberOfCanboards(EOappTheDB *p)
{
    return(eo_array_Capacity(p->canboardsList));
}

extern eOresult_t eo_appTheDB_GetTypeOfCanboard(EOappTheDB *p, eObrd_boardId_t bid, eObrd_types_t *type_ptr)
{
    eOappTheDB_hid_canBoardInfo_t   *b_ptr;
    
    if((NULL == p) || (NULL == type_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, bid);
    if(NULL == b_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    *type_ptr = b_ptr->cfg_ptr->type;
    
    return(eores_OK);
}


extern eObool_t eo_appTheDB_isSkinConnected(EOappTheDB *p)
{

    if((NULL == p) || (NULL == p->skinList))
    {
        return(0);
    }
    return(1);

}





extern eOresult_t eo_appTheDB_GetJointCanLocation(EOappTheDB *p, eOmc_jointId_t jId,  eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eObrd_types_t *type_ptr)
{
    eOappTheDB_hid_canBoardInfo_t   *b_ptr;
    eOappTheDB_hid_jointInfo_t      *j_ptr;
    
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    j_ptr = (eOappTheDB_hid_jointInfo_t *)eo_array_At(p->jointsList, jId);
    if(NULL == j_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    canloc_ptr->indexinboard = j_ptr->cfg_ptr->canLoc.indexinboard;

    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, j_ptr->cfg_ptr->canLoc.belong2board);
    if(NULL == b_ptr)
    {
        return(eores_NOK_nodata);
    }
    canloc_ptr->addr = b_ptr->cfg_ptr->canLoc.addr;
    canloc_ptr->emscanport = b_ptr->cfg_ptr->canLoc.emscanport;
    
    if(NULL != type_ptr)
    {
        *type_ptr = b_ptr->cfg_ptr->type;
    }
    
    return(eores_OK);
}

extern eOresult_t eo_appTheDB_GetMotorCanLocation(EOappTheDB *p, eOmc_motorId_t mId,  eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eObrd_types_t *type_ptr)
{
    eOappTheDB_hid_canBoardInfo_t       *b_ptr;
    eOappTheDB_hid_motorInfo_t      *m_ptr;
    
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    m_ptr = (eOappTheDB_hid_motorInfo_t *)eo_array_At(p->motorsList, mId);
    if(NULL == m_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    canloc_ptr->indexinboard = m_ptr->cfg_ptr->canLoc.indexinboard;

    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, m_ptr->cfg_ptr->canLoc.belong2board);
    if(NULL == b_ptr)
    {
        return(eores_NOK_nodata);
    }
    canloc_ptr->addr = b_ptr->cfg_ptr->canLoc.addr;
    canloc_ptr->emscanport = b_ptr->cfg_ptr->canLoc.emscanport;

    if(NULL != type_ptr)
    {
        *type_ptr = b_ptr->cfg_ptr->type;
    }
        
    return(eores_OK);

}


extern eOresult_t eo_appTheDB_GetMotorId_ByMotorCanLocation(EOappTheDB *p, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eOmc_motorId_t *mId_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == mId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    if((db_emscanportconnected2motorboard != canloc_ptr->emscanport) || (canloc_ptr->addr >= p->canaddressLookuptbl.capacity))
    {
        return(eores_NOK_generic);
    }

    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, eo_appTheDB_hid_GetBoardIdWithAddress(p, canloc_ptr->addr));
    if(NULL == b_ptr)
    {
       return(eores_NOK_nodata);
    }
    
    *mId_ptr = b_ptr->s.jm.connectedmotors[canloc_ptr->indexinboard];
    if(*mId_ptr == DB_NULL_VALUE_U08)
    {
        return(eores_NOK_nodata);
    }

    return(eores_OK);
}



extern eOresult_t eo_appTheDB_GetJointId_ByJointCanLocation(EOappTheDB *p, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eOmc_jointId_t *jId_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == jId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    if((db_emscanportconnected2motorboard != canloc_ptr->emscanport) || (canloc_ptr->addr >= p->canaddressLookuptbl.capacity))
    {
        return(eores_NOK_generic);
    }

    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, eo_appTheDB_hid_GetBoardIdWithAddress(p, canloc_ptr->addr));
    if(NULL == b_ptr)
    {
        return(eores_NOK_nodata);
    }
    *jId_ptr = b_ptr->s.jm.connectedjoints[canloc_ptr->indexinboard];

    return(eores_OK);
}




extern eOresult_t eo_appTheDB_GetSnsrMaisId_BySensorCanLocation(EOappTheDB *p, eOappTheDB_sensorCanLocation_t *canloc_ptr, eOas_maisId_t *sId_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr;
    eOappTheDB_hid_snsrMaisInfo_t  *s_ptr;
    uint8_t i, size;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == sId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    size = eo_array_Capacity(p->snsrMaisList); //i used capacity instead of size, because array is not filled with pushback function
    
    for(i=0; i<size; i++)
    {
        s_ptr = eo_array_At(p->snsrMaisList, i);
        if(NULL == s_ptr)
        {
            return(eores_NOK_nodata); //error somethis is wrong...
        }
        b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, s_ptr->cfg_ptr->belong2board);
        if(NULL == b_ptr)
        {
            return(eores_NOK_nodata);
        }
         if( (canloc_ptr->emscanport == b_ptr->cfg_ptr->canLoc.emscanport) &&
             (canloc_ptr->addr == b_ptr->cfg_ptr->canLoc.addr) )
        {
            //ok, i found the mais with can location canloc_ptr!!!
            *sId_ptr = i;
            return(eores_OK);
        }
    }
    
    return(eores_NOK_nodata);
}



extern eOresult_t eo_appTheDB_GetSnsrMaisCanLocation(EOappTheDB *p, eOas_maisId_t sId, eOappTheDB_sensorCanLocation_t *canloc_ptr)
{
    eOappTheDB_hid_snsrMaisInfo_t       *s_ptr;
    eOappTheDB_hid_canBoardInfo_t       *b_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    s_ptr = (eOappTheDB_hid_snsrMaisInfo_t *)eo_array_At(p->snsrMaisList, sId);
    if(NULL == s_ptr)
    {
        return(eores_NOK_nodata);
    }

    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, s_ptr->cfg_ptr->belong2board);
    if(NULL == b_ptr)
    {
        return(eores_NOK_nodata);
    }
    canloc_ptr->addr = b_ptr->cfg_ptr->canLoc.addr;
    canloc_ptr->emscanport = b_ptr->cfg_ptr->canLoc.emscanport;
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(EOappTheDB *p, eOappTheDB_sensorCanLocation_t *canloc_ptr, eOas_strainId_t *sId_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr;
    eOappTheDB_hid_snsrStrainInfo_t  *s_ptr;
    uint8_t i, size;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == sId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    size = eo_array_Capacity(p->snsrStrainList); //i used capacity instead of size, because array is not filled with pushback function
    
    for(i=0; i<size; i++)
    {
        s_ptr = eo_array_At(p->snsrStrainList, i);
        if(NULL == s_ptr)
        {
            return(eores_NOK_nodata); //error somethis is wrong...
        }
        b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, s_ptr->cfg_ptr->belong2board);
        if(NULL == b_ptr)
        {
            return(eores_NOK_nodata);
        }
         if( (canloc_ptr->emscanport == b_ptr->cfg_ptr->canLoc.emscanport) &&
             (canloc_ptr->addr == b_ptr->cfg_ptr->canLoc.addr) )
        {
            //ok, i found the strain with can location canloc_ptr!!!
            *sId_ptr = i;
            return(eores_OK);
        }
    }
    
    return(eores_NOK_nodata);
}



extern eOresult_t eo_appTheDB_GetSnsrStrainCanLocation(EOappTheDB *p, eOas_strainId_t sId, eOappTheDB_sensorCanLocation_t *canloc_ptr)
{
    eOappTheDB_hid_snsrStrainInfo_t     *s_ptr;
    eOappTheDB_hid_canBoardInfo_t       *b_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    s_ptr = (eOappTheDB_hid_snsrStrainInfo_t *)eo_array_At(p->snsrStrainList, sId);
    if(NULL == s_ptr)
    {
        return(eores_NOK_nodata);
    }

    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, s_ptr->cfg_ptr->belong2board);
    if(NULL == b_ptr)
    {
        return(eores_NOK_nodata);
    }
    canloc_ptr->addr = b_ptr->cfg_ptr->canLoc.addr;
    canloc_ptr->emscanport = b_ptr->cfg_ptr->canLoc.emscanport;
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetSkinCanLocation(EOappTheDB *p, eOsk_skinId_t skId, eOappTheDB_SkinCanLocation_t *canloc_ptr)
{
    eOappTheDB_hid_skinInfo_t           *sk_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    sk_ptr = (eOappTheDB_hid_skinInfo_t *)eo_array_At(p->skinList, skId);
    if(NULL == sk_ptr)
    {
        return(eores_NOK_nodata);
    }

    canloc_ptr->emscanport = sk_ptr->cfg_ptr->connected2emsport;
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetSkinId_BySkinCanLocation(EOappTheDB *p, eOappTheDB_SkinCanLocation_t *canloc_ptr, eOsk_skinId_t *skId_ptr)
{
    eOappTheDB_hid_skinInfo_t  *sk_ptr;
    uint8_t i, size;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == skId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    size = eo_array_Capacity(p->skinList); //i used capacity instead of size, because array is not filled with pushback function
    
    for(i=0; i<size; i++)
    {
        sk_ptr = eo_array_At(p->skinList, i);
        if(NULL == sk_ptr)
        {
            return(eores_NOK_nodata); //error somethis is wrong...
        }
         if(canloc_ptr->emscanport == sk_ptr->cfg_ptr->connected2emsport)
        {
            //ok, i found the strain with can location canloc_ptr!!!
            *skId_ptr = i;
            return(eores_OK);
        }
    }
    
    return(eores_NOK_nodata);

}
extern eOresult_t eo_appTheDB_GetJointConfigPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_joint_config_t **jconfig_ptr)
{
    if((NULL == p) || (NULL == jconfig_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    eOmc_joint_t *j_ptr = (eOmc_joint_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, (eOprotIndex_t)jId);
    
    if(NULL == j_ptr)
    {
        return(eores_NOK_nodata);
    }

    *jconfig_ptr = &(j_ptr->config);
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetJointStatusPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_joint_status_t **jstatus_ptr)
{
    if((NULL == p) || (NULL == jstatus_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    eOmc_joint_t *j_ptr = (eOmc_joint_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, (eOprotIndex_t)jId);
    
    if(NULL == j_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    *jstatus_ptr = &(j_ptr->status);
    return(eores_OK);
}


// extern eOresult_t eo_appTheDB_GetJointInputsPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_joint_inputs_t **jinputs_ptr)
// {
//  if((NULL == p) || (NULL == jinputs_ptr))
//  {
//         return(eores_NOK_nullpointer);
//  }
//     
//     if(jId >= p->cfg.jointsList->size)
//     {
//         return(eores_NOK_nodata);
//     }
//     
//     *jinputs_ptr = &(p->nvsram.jointsList_ptr[jId].inputs);
//     
//     return(eores_OK);
// }


// extern eOresult_t eo_appTheDB_GetJointCmdControlmodePtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_controlmode_t **jcmdcontrolmode_ptr)
// {
//  if((NULL == p) || (NULL == jcmdcontrolmode_ptr))
//  {
//         return(eores_NOK_nullpointer);
//  }
//     
//     if(jId >= p->cfg.jointsList->size)
//     {
//         return(eores_NOK_nodata);
//     }
//     
//     
//     *jcmdcontrolmode_ptr = (eOmc_controlmode_t*)&(p->nvsram.jointsList_ptr[jId].cmmnds.controlmode);
//     
//     return(eores_OK);
// }


// extern eOresult_t eo_appTheDB_GetJointCommandsPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_joint_commands_t **jcmds_ptr)
// {
//     if((NULL == p) || (NULL == jcmds_ptr))
//     {
//         return(eores_NOK_nullpointer);
//     }
//     
//     
//     eOmc_joint_t *j_ptr = (eOmc_joint_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, (eOprotIndex_t)jId);
//     
//     if(NULL == j_ptr)
//     {
//         return(eores_NOK_nodata);
//     }
//     
//     *jcmds_ptr = &(j_ptr->cmmnds);
//     return(eores_OK);
// }

extern eOresult_t eo_appTheDB_GetShiftValuesOfJointPtr(EOappTheDB *p, eOmc_jointId_t jId, eOappTheDB_jointShiftValues_t **shiftval_ptr)
{
    eOappTheDB_hid_jointInfo_t      *j_ptr;
    
    
    if((NULL == p) || (NULL == shiftval_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= eo_array_Capacity(p->jointsList))
    {
        return(eores_NOK_nodata);
    }
    
    j_ptr = (eOappTheDB_hid_jointInfo_t *)eo_array_At(p->jointsList, jId);
    *shiftval_ptr = j_ptr->shiftvalues_ptr;
      
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetJointBcastpolicyPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOicubCanProto_bcastpolicy_t **bcast_ptr)
{
    eOappTheDB_hid_jointInfo_t      *j_ptr;
    

    if((NULL == p) || (NULL == bcast_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= eo_array_Capacity(p->jointsList))
    {
        return(eores_NOK_nodata);
    }

    j_ptr = (eOappTheDB_hid_jointInfo_t *)eo_array_At(p->jointsList, jId);
    *bcast_ptr = j_ptr->bcastpolicy_ptr;

    return(eores_OK);
}



// extern eOresult_t eo_appTheDB_GetMotorConfigPtr(EOappTheDB *p, eOmc_motorId_t mId,  eOmc_motor_config_t **mconfig_ptr)
// {
//  if((NULL == p) || (NULL == mconfig_ptr))
//  {
//         return(eores_NOK_nullpointer);
//  }

//     if(mId >= p->cfg.motorsList->size)
//     {
//         return(eores_NOK_nodata);
//     }
//     
//     *mconfig_ptr =  &(p->nvsram.motorsList_ptr[mId].config);
//     
//     return(eores_OK);
// }


extern eOresult_t eo_appTheDB_GetMotorStatusPtr(EOappTheDB *p, eOmc_motorId_t mId,  eOmc_motor_status_t **mstatus_ptr)
{
    if((NULL == p) || (NULL == mstatus_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    eOmc_motor_t *m_ptr = (eOmc_motor_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor, (eOprotIndex_t)mId);
    
    if(NULL == m_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    *mstatus_ptr = &m_ptr->status;
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetSnrMaisConfigPtr(EOappTheDB *p, eOas_maisId_t sId,  eOas_mais_config_t **sconfig_ptr)
{
    if((NULL == p) || (NULL == sconfig_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    eOas_mais_t *mais_ptr = (eOas_mais_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, (eOprotIndex_t)sId);
    
    if(NULL == mais_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    *sconfig_ptr = &(mais_ptr->config);
    
    return(eores_OK);

}

extern eOresult_t eo_appTheDB_GetSnrMaisStatusPtr(EOappTheDB *p, eOas_maisId_t sId,  eOas_mais_status_t **sstatus_ptr)
{
    if((NULL == p) || (NULL == sstatus_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    eOas_mais_t *mais_ptr = (eOas_mais_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais, (eOprotIndex_t)sId);
    
    if(NULL == mais_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    *sstatus_ptr = &(mais_ptr->status);
    
    return(eores_OK);

}


extern eOresult_t eo_appTheDB_GetSnrStrainConfigPtr(EOappTheDB *p, eOas_strainId_t sId,  eOas_strain_config_t **sconfig_ptr)
{
    if((NULL == p) || (NULL == sconfig_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    eOas_strain_t *strain_ptr = (eOas_strain_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain, (eOprotIndex_t)sId);
    
    if(NULL == strain_ptr)
    {
        return(eores_NOK_nodata);
    }

    *sconfig_ptr = &(strain_ptr->config);
    
    return(eores_OK);

}


extern eOresult_t eo_appTheDB_GetSnrStrainStatusPtr(EOappTheDB *p, eOas_strainId_t sId,  eOas_strain_status_t **sstatus_ptr)
{
    if((NULL == p) || (NULL == sstatus_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    eOas_strain_t *strain_ptr = (eOas_strain_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain, (eOprotIndex_t)sId);
    
    if(NULL == strain_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    *sstatus_ptr = &(strain_ptr->status);
    
    return(eores_OK);

}

// extern eOresult_t eo_appTheDB_GetSkinCfgSigModePtr(EOappTheDB *p,eOsk_skinId_t skId,  eOsk_sigmode_t **sigmode_ptr)
// {
//  if((NULL == p) || (NULL == sigmode_ptr))
//  {
//         return(eores_NOK_nullpointer);
//  }
//     
//     eOsk_skin_t *sk_ptr = (eOsk_skin_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_skin, eosk_entity_skin, (eOprotIndex_t)skId);
//     
//     if(NULL == sk_ptr)
//     {
//         return(eores_NOK_nodata);
//     }
//     *sigmode_ptr = (eOsk_sigmode_t*)&(sk_ptr->config.sigmode);
//     
//     return(eores_OK);

// }

extern eOresult_t eo_appTheDB_GetSkinConfigPtr(EOappTheDB *p,eOsk_skinId_t sId,  eOappTheDB_cfg_skinInfo_t **skconfig_ptr)
{
    eOappTheDB_hid_skinInfo_t  *sk_ptr;
    if((NULL == p) || (NULL == skconfig_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    sk_ptr = eo_array_At(p->skinList, sId);
    if(NULL == sk_ptr)
    {
        return(eores_NOK_nodata);
    }

    *skconfig_ptr = sk_ptr->cfg_ptr;
    return(eores_OK);
}
extern eOresult_t eo_appTheDB_GetSkinStArray10CanFramesPtr(EOappTheDB *p,eOsk_skinId_t skId,  EOarray_of_10canframes **arrayof10canframes_ptr)
{
    if((NULL == p) || (NULL == arrayof10canframes_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    eOsk_skin_t *sk_ptr = (eOsk_skin_t *)eoprot_entity_ramof_get(eoprot_board_localboard, eoprot_endpoint_skin, eosk_entity_skin, (eOprotIndex_t)skId);
    
    if(NULL == sk_ptr)
    {
        return(eores_NOK_nodata);
    }
    *arrayof10canframes_ptr = &(sk_ptr->status.arrayof10canframes);
    
    return(eores_OK);

}

extern eOresult_t eo_appTheDB_GetCanBoardCfg(EOappTheDB *p, eObrd_boardId_t bid, eOappTheDB_cfg_canBoardInfo_t **cfg_canbrd_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr;
    if((NULL == p) || (NULL == cfg_canbrd_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(bid >= eo_array_Capacity(p->canboardsList))
    {
        return(eores_NOK_nodata);
    }
    b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, bid);
    *cfg_canbrd_ptr = b_ptr->cfg_ptr;
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetCanBoardId_ByCanLocation(EOappTheDB *p, eOappTheDB_canBoardCanLocation_t *canloc_ptr, eObrd_boardId_t *bid_ptr)
{
    eObrd_boardId_t i;
    eOappTheDB_hid_canBoardInfo_t *b_ptr;
    uint16_t numofboard = 0;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == bid_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    numofboard = eo_array_Capacity(p->canboardsList);    
    for(i=0; i<numofboard; i++)
    {
        b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, i);
        if( (b_ptr->cfg_ptr->canLoc.emscanport == canloc_ptr->emscanport) && (b_ptr->cfg_ptr->canLoc.addr == canloc_ptr->addr) )
        {
            *bid_ptr = i;
            return(eores_OK);
        }
    }
    
    return(eores_NOK_nodata);
}




extern eOresult_t eo_appTheDB_GetVirtualStrainDataPtr(EOappTheDB *p, uint16_t **virtStrain_ptr)
{
    if((NULL == p) || (NULL == virtStrain_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    *virtStrain_ptr = &p->virtualStrainData.values[0];
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_SetVirtualStrainValue(EOappTheDB *p, eOmc_jointId_t jId, uint16_t torquevalue)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    switch(jId)
    {
        case 0:
        {
            p->virtualStrainData.values[4] = torquevalue; //wrist pronosupination
            p->virtualStrainData.isupdated = 1;
        }break;

        case 1:
        {
            p->virtualStrainData.values[0] = torquevalue; //wrist yaw
            p->virtualStrainData.isupdated = 1;
        }break;
        
        case 2:
        {
            p->virtualStrainData.values[1] = torquevalue; //wrist pitch
            p->virtualStrainData.isupdated = 1;
        }break;
        default:
        {
            ;//nothing to do!!!
        }

    };

    return(eores_OK);
}

extern uint8_t eo_appTheDB_IsVirtualStrainDataUpdated(EOappTheDB *p)
{
    if(NULL == p)
    {
        return(0);
    }   
    return(p->virtualStrainData.isupdated);
}

extern void eo_appTheDB_ClearVirtualStrainDataUpdatedFlag(EOappTheDB *p)
{
    if(NULL == p)
    {
        return;
    }   
    p->virtualStrainData.isupdated = 0;
}


extern eOresult_t eo_appTheDB_setCanBoardReady(EOappTheDB *p, eOappTheDB_canBoardCanLocation_t *canloc_ptr)
{
    uint8_t                         numofboard, i;
    eOappTheDB_hid_canBoardInfo_t   *b_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    numofboard = eo_array_Capacity(p->canboardsList);
    for(i=0; i<numofboard; i++)
    {
        b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, i);
        if( (b_ptr->cfg_ptr->canLoc.emscanport == canloc_ptr->emscanport) && (b_ptr->cfg_ptr->canLoc.addr == canloc_ptr->addr) )
        {
            b_ptr->isready = eobool_true;
            return(eores_OK);
        }
    }
    
    return(eores_NOK_nodata);
    
}
extern eObool_t  eo_appTheDB_areConnectedCanBoardsReady(EOappTheDB *p, uint32_t *canBoardsReady)
{
    uint8_t                             numofboard, i;
    eOappTheDB_hid_canBoardInfo_t       *b_ptr;
    eObool_t                            res = eobool_true;
    
    if((NULL == p) && (NULL == canBoardsReady))
    {
        return(eobool_false);
    }
    
    numofboard = eo_array_Capacity(p->canboardsList);
    *canBoardsReady = 0;
    
    for(i=0; i<numofboard; i++)
    {
        
        b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, i);
        if((eobrd_mc4 != b_ptr->cfg_ptr->type) && (eobrd_1foc != b_ptr->cfg_ptr->type))
        {
            continue;
        }
        res &= b_ptr->isready;
        if(b_ptr->isready)
        {
            *canBoardsReady |= (1<<i);
        }
        
    }
    
    return(res);

}

#ifdef USE_PROTO_PROXY
extern eOresult_t eo_appTheDB_appendEthProtoRequest(EOappTheDB *p, eOprotEntity_t entity, eOprotIndex_t index, eOappTheDB_hid_ethProtoRequest_t *req)
{
    EOlist  *ethProtoReq_list = NULL;
    EOlistIter* li =NULL;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    ethProtoReq_list = s_appTheDB_getEthProtoReqList(p, eoprot_endpoint_motioncontrol, entity, index);
    
    if(NULL == ethProtoReq_list)
    {
        return(eores_NOK_nodata);
    }
    
    if(eo_list_Full(ethProtoReq_list))
    {
        return(eores_NOK_busy);
    }
    
    
    li = eo_list_Find(ethProtoReq_list, s_appTheDB_searchEthProtoReq_matchingRule, &req->id32);
    if(NULL == li)
    {
        eo_list_PushBack(ethProtoReq_list, req);
    }
    else
    {
        //se sono qui allora non ho ricevuto tutte le risposte can che mi aspettavo...e quindi e' rimansta una entri sporca...
        ((eOappTheDB_hid_ethProtoRequest_t *)li->data)->numOfREceivedResp = 0;
    }
    
    return(eores_OK);
}


extern EOlistIter* eo_appTheDB_searchEthProtoRequest(EOappTheDB *p, eOprotID32_t id32)
{
    if(NULL == p)
    {
        return(NULL);
    }
    eOprotID32_t key = id32;
    eOprotEndpoint_t ep     = eoprot_ID2endpoint(id32);
    eOprotEntity_t   entity = eoprot_ID2entity(id32);
    eOprotIndex_t    index  = eoprot_ID2index(id32); 
   
    EOlist  *ethProtoReq_list = s_appTheDB_getEthProtoReqList(p, eoprot_endpoint_motioncontrol, entity, index);

    return (eo_list_Find(ethProtoReq_list, s_appTheDB_searchEthProtoReq_matchingRule, &key));

}
extern eOresult_t eo_appTheDB_removeEthProtoRequest(EOappTheDB *p, eOprotEntity_t entity, eOprotIndex_t index, EOlistIter* li)
{
    eo_list_Erase( s_appTheDB_getEthProtoReqList(p, eoprot_endpoint_motioncontrol, entity, index), li);
    return(eores_OK);
    #warning VALE: mettere controllo in eo_appTheDB_removeEthProtoRequest????
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static eObool_t s_appTheDB_checkConfiguaration(eOappTheDB_cfg_t *cfg)
{
    if((NULL == cfg->jointsList) || (NULL == cfg->motorsList) || (NULL == cfg->snsrMaisList) ||
       (NULL == cfg->snsrStrainList) || (NULL == cfg->skinList))
    {
        return(0);
    }

    //check if ep cfg and db cfg are consistent
    //if(eo_cfg_nvsEP_mc_joint_numbermax_Get((eOcfg_nvsEP_mc_endpoint_t)cfg->mc_endpoint) != cfg->jointsList->size)
    if(eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint) != cfg->jointsList->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "joints cfg mismatch", s_eobj_ownname, &eo_errman_DescrTobedecided);
        return(0);
    }

    if(eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor) != cfg->motorsList->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "motors cfg mismatch", s_eobj_ownname, &eo_errman_DescrTobedecided);
        return(0);
    }

    if(eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais) != cfg->snsrMaisList->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "snr-mais cfg mismatch", s_eobj_ownname, &eo_errman_DescrTobedecided);
        return(0);
    }
    
    if(eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain) != cfg->snsrStrainList->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "snr-strain cfg mismach", s_eobj_ownname, &eo_errman_DescrTobedecided);
        return(0);
    }
    if(eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin) != cfg->skinList->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "skin cfg mismach", s_eobj_ownname, &eo_errman_DescrTobedecided);
        return(0);
    }
    
    return(1);
}


static eOresult_t s_appTheDB_canboardslist_init(EOappTheDB *p)
{
    eOsizecntnr_t                   i, k;
    eOappTheDB_cfg_canBoardInfo_t   *b_cfg_ptr = NULL;  // pointer to configuration 
    eOappTheDB_hid_canBoardInfo_t   *b_ptr = NULL;      // pointer to db memory
    
    
    //if no canboard is connected to board (skin only mode) ==> nothing to do and return ok 
    if(0 == p->cfg.canboardsList->size)
    {
        p->canboardsList = NULL;
        return(eores_OK);
    }
    
    
    //1) create canboardsList
    p->canboardsList = eo_array_New(p->cfg.canboardsList->size, sizeof(eOappTheDB_hid_canBoardInfo_t), NULL);
    if(NULL == p->canboardsList)
    {
        return(eores_NOK_generic);
    }
    
    b_cfg_ptr = (eOappTheDB_cfg_canBoardInfo_t*)(p->cfg.canboardsList->item_array_data);
    
    for(i = 0; i< p->cfg.canboardsList->size; i++)
    {

        b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, i);
        
        //1.1) save pointer to board cfg info
        b_ptr->cfg_ptr = &b_cfg_ptr[i];
        
        //create array of "connected stuff" (joints or sensors)
        if((eobrd_mc4 == b_ptr->cfg_ptr->type) || (eobrd_1foc == b_ptr->cfg_ptr->type))
        {
            b_ptr->s.jm.connectedjoints =  eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                    sizeof(eOmc_jointId_t), eOicubCanProto_jm_indexInBoard_max);
            b_ptr->s.jm.connectedmotors =  eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                    sizeof(eOmc_motorId_t), eOicubCanProto_jm_indexInBoard_max);
            for(k=0; k<eOicubCanProto_jm_indexInBoard_max; k++)
            {
                b_ptr->s.jm.connectedjoints[k] = DB_NULL_VALUE_U08;
                b_ptr->s.jm.connectedmotors[k] = DB_NULL_VALUE_U08;
            }
        }
//         else if((eobrd_mais == b_ptr->cfg_ptr->type) || (eobrd_skin == b_ptr->cfg_ptr->type) || (eobrd_strain == b_ptr->cfg_ptr->type))
//         {
//             b_ptr->s.connectedsensors =  eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
//                                                     sizeof(eOas_sensorId_t), 1); //currently almost only one sensor per board
//         }
        
        //set the board as not ready!
        b_ptr->isready = eobool_false;
        
    }
    return(eores_OK);
}

static eOresult_t s_appTheDB_jointslist_init(EOappTheDB *p)
{
    eOsizecntnr_t                       i;
    eOappTheDB_cfg_jointInfo_t          *j_cfg_ptr = NULL;  //pointer to configuration
    eOappTheDB_hid_jointInfo_t          *j_ptr = NULL;      //pointer to db memory
    eOappTheDB_hid_canBoardInfo_t       *b_ptr = NULL;
    eOappTheDB_jointShiftValues_t       *shiftvalues_ptr;
    eOicubCanProto_bcastpolicy_t        *bcastpolicy_ptr;    
    
    //if no joint is connected to board (skin only mode) ==> nothing to do and return ok 
    if(0 == p->cfg.jointsList->size)
    {
        p->jointsList = NULL;
        return(eores_OK);
    }
    
    
    //1) create jointList
    p->jointsList = eo_array_New(p->cfg.jointsList->size, sizeof(eOappTheDB_hid_jointInfo_t), NULL);
    if(NULL == p->jointsList)
    {
        return(eores_NOK_generic);
    }
    
    //2) fill jointsList
    j_cfg_ptr = (eOappTheDB_cfg_jointInfo_t*)(p->cfg.jointsList->item_array_data);
        
    //2.1) allocate memory where i save data shared between all joints
    shiftvalues_ptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOappTheDB_jointShiftValues_t), 1);
    bcastpolicy_ptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOicubCanProto_bcastpolicy_t), 1);
    
    for(i = 0; i< p->cfg.jointsList->size; i++)
    {
        j_ptr = (eOappTheDB_hid_jointInfo_t*)eo_array_At(p->jointsList, i);
        
        j_ptr->cfg_ptr = &j_cfg_ptr[i];
        j_ptr->shiftvalues_ptr = shiftvalues_ptr;
        j_ptr->bcastpolicy_ptr = bcastpolicy_ptr;
        
        #ifdef USE_PROTO_PROXY
            j_ptr->ethProtoReq_list = eo_list_New(sizeof(eOappTheDB_hid_ethProtoRequest_t), db_numOfEthProtoReq_for_entity, NULL, 0, NULL, NULL);
        #endif
        
        
        //2.1)make a connection beetween board to joint also ==> fill "connected joints list" of baord
        b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, j_ptr->cfg_ptr->canLoc.belong2board);
        b_ptr->s.jm.connectedjoints[j_ptr->cfg_ptr->canLoc.indexinboard] = (eOmc_jointId_t)i;
    }
    return(eores_OK);
}


static eOresult_t s_appTheDB_motorslist_init(EOappTheDB *p)
{
    eOsizecntnr_t                       i;
    eOappTheDB_cfg_motorInfo_t          *m_cfg_ptr = NULL;  //pointer to configuration
    eOappTheDB_hid_motorInfo_t          *m_ptr = NULL;      //pointer to db memory
    eOappTheDB_hid_canBoardInfo_t       *b_ptr = NULL;

    //if no motor is connected to board (skin only mode) ==> nothing to do and return ok 
    if(0 == p->cfg.motorsList->size)
    {
        p->motorsList = NULL;
        return(eores_OK);
    }
    

    //1) create jointList
    p->motorsList = eo_array_New(p->cfg.motorsList->size, sizeof(eOappTheDB_hid_motorInfo_t), NULL);
    if(NULL == p->motorsList)
    {
        return(eores_NOK_generic);
    }

 
    //2) fill motorsList
    m_cfg_ptr = (eOappTheDB_cfg_motorInfo_t*)(p->cfg.motorsList->item_array_data);

    for(i = 0; i< p->cfg.motorsList->size; i++)
    {
        m_ptr = (eOappTheDB_hid_motorInfo_t*)eo_array_At(p->motorsList, i);
        m_ptr->cfg_ptr = &m_cfg_ptr[i];
        #ifdef USE_PROTO_PROXY
            m_ptr->ethProtoReq_list = eo_list_New(sizeof(eOappTheDB_hid_ethProtoRequest_t), db_numOfEthProtoReq_for_entity, NULL, 0, NULL, NULL);
        #endif
        //2.1)make a connection beetween board to joint also ==> fill "connected joints list" of board
        b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, m_ptr->cfg_ptr->canLoc.belong2board);
        b_ptr->s.jm.connectedmotors[m_ptr->cfg_ptr->canLoc.indexinboard] = (eOmc_motorId_t)i; //set motor's id
    }
    
    return(eores_OK);
}


static eOresult_t s_appTheDB_snsrMaislist_init(EOappTheDB *p)
{
    eOsizecntnr_t                       i;
    eOappTheDB_cfg_snsrMaisInfo_t        *s_cfg_ptr = NULL;     //pointer to configuration
    eOappTheDB_hid_snsrMaisInfo_t       *s_ptr = NULL;      //pointer to db memory

    //if no sensor is connected to board ==> nothing to do and return ok
    if(0 == p->cfg.snsrMaisList->size)
    {
        p->snsrMaisList = NULL;
        return(eores_OK);
    }
    
    //1) create sensors mais List
    p->snsrMaisList = eo_array_New(p->cfg.snsrMaisList->size, sizeof(eOappTheDB_hid_snsrMaisInfo_t), NULL);
    if(NULL == p->snsrMaisList)
    {
        return(eores_NOK_generic);
    }
    
    
    //2) fill sensors mais List
    s_cfg_ptr = (eOappTheDB_cfg_snsrMaisInfo_t*)(p->cfg.snsrMaisList->item_array_data);

    for(i = 0; i<p->cfg.snsrMaisList->size; i++)
    {
        s_ptr = (eOappTheDB_hid_snsrMaisInfo_t*)eo_array_At(p->snsrMaisList, i);
        s_ptr->cfg_ptr = &s_cfg_ptr[i];
        
        /* Since almost only one sensor is connected to ems, i don't use a table to get sensor id(it will always 0) */
    }
    
    return(eores_OK);
}


static eOresult_t s_appTheDB_snsrStrainlist_init(EOappTheDB *p)
{
    eOsizecntnr_t                       i;
    eOappTheDB_cfg_snsrStrainInfo_t      *s_cfg_ptr = NULL;     //pointer to configuration
    eOappTheDB_hid_snsrStrainInfo_t     *s_ptr = NULL;      //pointer to db memory

    //if no sensor is connected to board ==> nothing to do and return ok
    if(0 == p->cfg.snsrStrainList->size)
    {
        p->snsrStrainList = NULL;
        return(eores_OK);
    }
    
    //1) create sensors mais List
    p->snsrStrainList = eo_array_New(p->cfg.snsrStrainList->size, sizeof(eOappTheDB_hid_snsrStrainInfo_t), NULL);
    if(NULL == p->snsrStrainList)
    {
        return(eores_NOK_generic);
    }
    
    
    //2) fill sensors mais List
    s_cfg_ptr = (eOappTheDB_cfg_snsrStrainInfo_t*)(p->cfg.snsrStrainList->item_array_data);

    for(i = 0; i<p->cfg.snsrStrainList->size; i++)
    {
        s_ptr = (eOappTheDB_hid_snsrStrainInfo_t*)eo_array_At(p->snsrStrainList, i);
        s_ptr->cfg_ptr = &s_cfg_ptr[i];
        
        /* Since almost only one sensor is connected to ems, i don't use a table to get sensor id(it will always 0) */
    }
    
    return(eores_OK);
}


static eOresult_t s_appTheDB_skinlist_init(EOappTheDB *p)
{
    int8_t                              i;
    eOappTheDB_cfg_skinInfo_t           *sk_cfg_ptr = NULL;
    eOappTheDB_hid_skinInfo_t           *sk_ptr = NULL;

    //if no sensor is connected to board ==> nothing to do and return ok
    if(0 == p->cfg.skinList->size)
    {
        p->skinList = NULL;
        return(eores_OK);
    }
    
    //1) create sensorsList
    p->skinList = eo_array_New(p->cfg.skinList->size, sizeof(eOappTheDB_hid_skinInfo_t), NULL);
    if(NULL == p->skinList)
    {
        return(eores_NOK_generic);
    }


    //2) fill skin List
    sk_cfg_ptr = (eOappTheDB_cfg_skinInfo_t*)(p->cfg.skinList->item_array_data);

    for(i = 0; i< p->cfg.skinList->size; i++)
    {
        sk_ptr = (eOappTheDB_hid_skinInfo_t*)eo_array_At(p->skinList, i);
        sk_ptr->cfg_ptr = &sk_cfg_ptr[i];
        /* Since skin is seen like a unique board, connected to a specific ems port 
            i don't need a method to get skin id by can location */
    }
    
    return(eores_OK);
}


static eOresult_t s_appTheDB_canaddressLookuptbl_init(EOappTheDB *p)
{
    eOsizecntnr_t                       i;
//  eObrd_boardId_t                     maxusedcanaddr = 0;
    icubCanProto_canBoardAddress_t      maxusedcanaddr = 0, addr;
    
    if(0 == p->cfg.canboardsList->size)
    {
        p->canaddressLookuptbl.capacity = 0;
        p->canaddressLookuptbl.tbl = NULL;
        return(eores_OK);
    }
    
    //1) get max used can address (it will be in range [1,E]. 0 is ems can port address and F is bradcast address)
    eOappTheDB_cfg_canBoardInfo_t *b_cfg_ptr = (eOappTheDB_cfg_canBoardInfo_t*)(p->cfg.canboardsList->item_array_data);
    
    for(i = 0; i< p->cfg.canboardsList->size; i++)
    {
        //select only the addresses that are:
        //  - connected 2 ems port used to commnd joint (joints and motors are connected to a different can bus of sensors) AND
        //  - belong to motor baord (1 foc and mc4)
        if( (db_emscanportconnected2motorboard == b_cfg_ptr[i].canLoc.emscanport) &&
            ((eobrd_1foc == b_cfg_ptr[i].type) || (eobrd_mc4 == b_cfg_ptr[i].type)) &&
            (maxusedcanaddr < b_cfg_ptr[i].canLoc.addr))
        {
            maxusedcanaddr = b_cfg_ptr[i].canLoc.addr;
        }   
    }

    // 2) create the lookup tbl
    p->canaddressLookuptbl.capacity = maxusedcanaddr+1; //cosi' lascio l'elemento 0-esimo, anche se non lo uso.    
    p->canaddressLookuptbl.tbl = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eObrd_boardId_t), 
                                                      p->canaddressLookuptbl.capacity);
    
    // 3) fill lookup tbl
    b_cfg_ptr = (eOappTheDB_cfg_canBoardInfo_t*)(p->cfg.canboardsList->item_array_data);
    for(i=0; i<p->cfg.canboardsList->size; i++)
    {
        if((db_emscanportconnected2motorboard != b_cfg_ptr[i].canLoc.emscanport) &&
           (eobrd_1foc != b_cfg_ptr[i].type) && (eobrd_mc4 != b_cfg_ptr[i].type))
        {
            continue;
        }        
        addr = b_cfg_ptr[i].canLoc.addr;
        p->canaddressLookuptbl.tbl[addr] = (eObrd_boardId_t)i;
    }
    
    return(eores_OK);
}



// static eOresult_t s_appTheDB_nvsramref_init(EOappTheDB *p)
// {
//     
//     eOmc_joint_t            *jointsList_ptr  = NULL;
//     eOmc_motor_t            *motorsList_ptr  = NULL;
//     eOmc_controller_t       *thecontroller   = NULL;
//     eOas_mais_t             *maisList_ptr    = NULL;
//     eOas_strain_t           *strainList_ptr  = NULL;
//     eOsk_skin_t             *skin_ptr        = NULL;

//     
// #if  defined(EOMTHEEMSAPPLCFG_USE_EB1)
//     //mc
//     eo_cfg_nvsEP_mc_upperarm_t *mc_ptr = (eo_cfg_nvsEP_mc_upperarm_t*)eo_cfg_nvsEP_eb1_Get_locallyownedRAM(p->cfg.mc_endpoint);
//     jointsList_ptr = &mc_ptr->joints[0];
//     motorsList_ptr = &mc_ptr->motors[0];
//     thecontroller = &mc_ptr->thecontroller;
//     
//     //strain 
//     eo_cfg_nvsEP_as_onestrain_t *as_ptr = (eo_cfg_nvsEP_as_onestrain_t *)eo_cfg_nvsEP_eb1_Get_locallyownedRAM(p->cfg.as_endpoint);
//     strainList_ptr = &as_ptr->strains[0];
//     

// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB2)
//     //mc
//     eo_cfg_nvsEP_mc_lowerarm_t *mc_ptr = (eo_cfg_nvsEP_mc_lowerarm_t*)eo_cfg_nvsEP_eb2_Get_locallyownedRAM(p->cfg.mc_endpoint);
//     jointsList_ptr = &mc_ptr->joints[0];
//     motorsList_ptr = &mc_ptr->motors[0];
//     thecontroller = &mc_ptr->thecontroller;
//     
//     //mais
//     eo_cfg_nvsEP_as_onemais_t *as_ptr = (eo_cfg_nvsEP_as_onemais_t *)eo_cfg_nvsEP_eb2_Get_locallyownedRAM(p->cfg.as_endpoint);
//     maisList_ptr = &as_ptr->maises[0];
//     
//     //skin
//     eo_cfg_nvsEP_sk_emsboard_t * sk_ptr = (eo_cfg_nvsEP_sk_emsboard_t *)eo_cfg_nvsEP_eb2_Get_locallyownedRAM(p->cfg.sk_endpoint);
//     skin_ptr  = &sk_ptr->skin;
//   
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB3)
//     //mc
//     eo_cfg_nvsEP_mc_upperarm_t *mc_ptr = (eo_cfg_nvsEP_mc_upperarm_t*)eo_cfg_nvsEP_eb3_Get_locallyownedRAM(p->cfg.mc_endpoint);
//     jointsList_ptr = &mc_ptr->joints[0];
//     motorsList_ptr = &mc_ptr->motors[0];
//     thecontroller = &mc_ptr->thecontroller;
//     
//     //strain 
//     eo_cfg_nvsEP_as_onestrain_t *as_ptr = (eo_cfg_nvsEP_as_onestrain_t *)eo_cfg_nvsEP_eb3_Get_locallyownedRAM(p->cfg.as_endpoint);
//     strainList_ptr = &as_ptr->strains[0];
//     
//     
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB4)
//     //mc
//     eo_cfg_nvsEP_mc_lowerarm_t *mc_ptr = (eo_cfg_nvsEP_mc_lowerarm_t*)eo_cfg_nvsEP_eb4_Get_locallyownedRAM(p->cfg.mc_endpoint);
//     jointsList_ptr = &mc_ptr->joints[0];
//     motorsList_ptr = &mc_ptr->motors[0];
//     thecontroller = &mc_ptr->thecontroller;
//     
//     //mais
//     eo_cfg_nvsEP_as_onemais_t *as_ptr = (eo_cfg_nvsEP_as_onemais_t *)eo_cfg_nvsEP_eb4_Get_locallyownedRAM(p->cfg.as_endpoint);
//     maisList_ptr = &as_ptr->maises[0];
//     
//     //skin
//     eo_cfg_nvsEP_sk_emsboard_t * sk_ptr = (eo_cfg_nvsEP_sk_emsboard_t *)eo_cfg_nvsEP_eb4_Get_locallyownedRAM(p->cfg.sk_endpoint);
//     skin_ptr  = &sk_ptr->skin;
//     
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB5)
//     //mc
//     eo_cfg_nvsEP_mc_torso_t *mc_ptr = (eo_cfg_nvsEP_mc_torso_t*)eo_cfg_nvsEP_eb5_Get_locallyownedRAM(p->cfg.mc_endpoint);
//     jointsList_ptr = &mc_ptr->joints[0];
//     motorsList_ptr = &mc_ptr->motors[0];
//     thecontroller = &mc_ptr->thecontroller;
//     
//     
// #elif  defined(EOMTHEEMSAPPLCFG_USE_EB6)
//     //mc
//     eo_cfg_nvsEP_mc_upperleg_t *mc_ptr = (eo_cfg_nvsEP_mc_upperleg_t*)eo_cfg_nvsEP_eb6_Get_locallyownedRAM(p->cfg.mc_endpoint);
//     jointsList_ptr = &mc_ptr->joints[0];
//     motorsList_ptr = &mc_ptr->motors[0];
//     thecontroller = &mc_ptr->thecontroller;
//     
//     //strain 
//     eo_cfg_nvsEP_as_onestrain_t *as_ptr = (eo_cfg_nvsEP_as_onestrain_t *)eo_cfg_nvsEP_eb6_Get_locallyownedRAM(p->cfg.as_endpoint);
//     strainList_ptr = &as_ptr->strains[0];

// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB7)
//     //mc
//     eo_cfg_nvsEP_mc_lowerleg_t *mc_ptr = (eo_cfg_nvsEP_mc_lowerleg_t*)eo_cfg_nvsEP_eb7_Get_locallyownedRAM(p->cfg.mc_endpoint);
//     jointsList_ptr = &mc_ptr->joints[0];
//     motorsList_ptr = &mc_ptr->motors[0];
//     thecontroller = &mc_ptr->thecontroller;
//     
//     
// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB8)
//     //mc
//     eo_cfg_nvsEP_mc_upperleg_t *mc_ptr = (eo_cfg_nvsEP_mc_upperleg_t*)eo_cfg_nvsEP_eb8_Get_locallyownedRAM(p->cfg.mc_endpoint);
//     jointsList_ptr = &mc_ptr->joints[0];
//     motorsList_ptr = &mc_ptr->motors[0];
//     thecontroller = &mc_ptr->thecontroller;
//     
//     //strain 
//     eo_cfg_nvsEP_as_onestrain_t *as_ptr = (eo_cfg_nvsEP_as_onestrain_t *)eo_cfg_nvsEP_eb8_Get_locallyownedRAM(p->cfg.as_endpoint);
//     strainList_ptr = &as_ptr->strains[0];

// #elif   defined(EOMTHEEMSAPPLCFG_USE_EB9)
//     //mc
//     eo_cfg_nvsEP_mc_lowerleg_t *mc_ptr = (eo_cfg_nvsEP_mc_lowerleg_t*)eo_cfg_nvsEP_eb9_Get_locallyownedRAM(p->cfg.mc_endpoint);
//     jointsList_ptr = &mc_ptr->joints[0];
//     motorsList_ptr = &mc_ptr->motors[0];
//     thecontroller = &mc_ptr->thecontroller;
// #else
//     #error --> you must define an EBx
// #endif
//     
//     
//     p->nvsram.jointsList_ptr = jointsList_ptr;
//     p->nvsram.motorsList_ptr = motorsList_ptr;
//     p->nvsram.thecontroller = thecontroller;
//     p->nvsram.maisList_ptr = maisList_ptr;
//     p->nvsram.strainList_ptr = strainList_ptr;
//     p->nvsram.skin_ptr = skin_ptr;


//     return(eores_OK);
// }


static eOresult_t s_appTheDB_virtualStrainData_init(EOappTheDB *p)
{
    memset(&p->virtualStrainData, 0, sizeof(eOappTheDB_hid_virtualStrainData_t));
    return(eores_OK);
}


#ifdef USE_PROTO_PROXY
static EOlist * s_appTheDB_getEthProtoReqList(EOappTheDB *p, eOprotEndpoint_t ep, eOprotEntity_t entity, eOprotIndex_t index)
{
    if((eoprot_endpoint_motioncontrol == ep) && (eoprot_entity_mc_joint == entity))
    {
        eOappTheDB_hid_jointInfo_t *j_ptr = (eOappTheDB_hid_jointInfo_t *)eo_array_At(p->jointsList, index);
        if(NULL != j_ptr)
        {
            return(j_ptr->ethProtoReq_list);
        }
    }
    
    if((eoprot_endpoint_motioncontrol == ep) && (eoprot_entity_mc_motor == entity))
    {
        eOappTheDB_hid_motorInfo_t * m_ptr = (eOappTheDB_hid_motorInfo_t *)eo_array_At(p->motorsList, index);
        if(NULL != m_ptr)
        {
            return (m_ptr->ethProtoReq_list);
        }
    }
    
    return(NULL);

}


static eOresult_t s_appTheDB_searchEthProtoReq_matchingRule(void *item, void *param)
{
    eOappTheDB_hid_ethProtoRequest_t *req = (eOappTheDB_hid_ethProtoRequest_t*)item;
    eOprotID32_t * id32 = (eOprotID32_t*)param;
    
    if(req->id32 == *id32)
    {
        return(eores_OK);
    }
    else
    {
        return(eores_NOK_generic);
    }

}
#endif
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



