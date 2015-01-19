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



#include "EOconstvector.h" 



#ifdef USE_PROTO_PROXY
#include "EOlist.h"
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

#define s_appTheDB_canportconnected2motorboards     eOcanport1 
#ifdef USE_PROTO_PROXY
// marco.accame:
// it is the number of max proxy requests for each instance of joint or motor or any other entity  
// thus: if we have 12 joints and 12 motors, then we have 24 lists of db_numOfEthProtoReq_for_entity items (for a max of 48 request items)
// but the proxy object is configured to accept a maximum of EOMTHEEMSAPPLCFG_PROXY_MAXNUMOFREPLYROPS (now 16) requests.
// maybe it is better to change a bit. 
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

static eObool_t s_appTheDB_checkConfiguration(eOappTheDB_cfg_t *cfg);
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


static eOresult_t s_appTheDB_virtualStrainData_init(EOappTheDB *p);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOappTheDB s_eo_appTheDB = 
{
    .cfg            = 
    {
        .canboardsList      = NULL,
        .jointsList         = NULL,
        .motorsList         = NULL,
        .snsrMaisList       = NULL,
        .snsrStrainList     = NULL,
        .skinList           = NULL
    },
    .isinitted              = eobool_false,
    .canboardsInfo          = NULL,
    .jointsInfo             = NULL,
    .motorsInfo             = NULL,
    .maisesInfo             = NULL,
    .strainsInfo            = NULL,
    .skinsInfo              = NULL,
    .canaddressLookuptbl    =
    {   
        .capacity   = 0,
        .tbl        = NULL
    },
    .virtualStrainData      = NULL
};


static const char s_eobj_ownname[] = "EOappTheDB";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOappTheDB* eo_appTheDB_Initialise(eOappTheDB_cfg_t *cfg)
{
    eOresult_t res;
    EOappTheDB  *retptr = &s_eo_appTheDB;
    
    if(NULL == cfg)
    {
        return(NULL);
    }
    
    if(eobool_false == s_appTheDB_checkConfiguration(cfg))
    {
        return(NULL);
    }

    
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
    return((s_eo_appTheDB.isinitted) ? (&s_eo_appTheDB) : NULL);
}


extern uint16_t eo_appTheDB_GetNumberOfConnectedJoints(EOappTheDB *p)
{   // use s_eo_appTheDB so that i skip control of p not being NULL
    return(eo_array_Size(s_eo_appTheDB.jointsInfo));
}


extern uint16_t eo_appTheDB_GetNumberOfConnectedMotors(EOappTheDB *p)
{   // use s_eo_appTheDB so that i skip control of p not being NULL
    return(eo_array_Size(s_eo_appTheDB.motorsInfo));
}


extern uint16_t eo_appTheDB_GetNumberOfCanboards(EOappTheDB *p)
{   // use s_eo_appTheDB so that i skip control of p not being NULL
    return(eo_array_Size(s_eo_appTheDB.canboardsInfo));
}


extern eOresult_t eo_appTheDB_GetTypeOfCanboard(EOappTheDB *p, eObrd_boardId_t bid, eObrd_cantype_t *type_ptr)
{
    eOappTheDB_hid_canBoardInfo_t   *b_ptr;
    
    if((NULL == p) || (NULL == type_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(bid >= eo_array_Size(p->canboardsInfo))
    {
        return(eores_NOK_nodata);
    }
    
    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsInfo, bid);
    
    *type_ptr = (eObrd_cantype_t)b_ptr->basicboardinfo.type;
    
    return(eores_OK);
}


extern eObool_t eo_appTheDB_isSkinConnected(EOappTheDB *p)
{
    if((NULL == p) || (NULL == p->skinsInfo))
    {
        return(eobool_false);
    }
    return(eobool_true);
}


extern eOresult_t eo_appTheDB_GetJointCanLocation(EOappTheDB *p, eOmc_jointId_t jId, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eObrd_cantype_t *type_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr = NULL;
    eOappTheDB_hid_jointInfo_t *j_ptr = NULL;
    uint16_t indexincanboardarray = 0;
    
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= eo_array_Size(p->jointsInfo))
    {   // i attempt to retrieve a position beyond its size.
        // for instance: position 4 or 5 or 6 or higher number when the size of array is only 4
        return(eores_NOK_nodata);
    }
    // eo_array_At() returns NULL only if jId is beyond capacity, not size.
    j_ptr = (eOappTheDB_hid_jointInfo_t *)eo_array_At(p->jointsInfo, jId);
  
    indexincanboardarray = j_ptr->mapping2canboard.indexofcanboard;
    
    // now i must verify that we have     
    if(indexincanboardarray >= eo_array_Size(p->canboardsInfo))
    {
        return(eores_NOK_nodata);
    }

    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsInfo, indexincanboardarray);
    
    // ok, now i can fill the return values    
    canloc_ptr->indexinsidecanboard     = (eOicubCanProto_jm_indexinBoard_t)j_ptr->mapping2canboard.indexinsidecanboard;    
    canloc_ptr->addr                    = b_ptr->basicboardinfo.addr;
    canloc_ptr->emscanport              = (eOcanport_t)b_ptr->basicboardinfo.port;
    if(NULL != type_ptr)
    {
        *type_ptr = (eObrd_cantype_t)b_ptr->basicboardinfo.type;
    }
    
    return(eores_OK);
}

extern eOresult_t eo_appTheDB_GetMotorCanLocation(EOappTheDB *p, eOmc_motorId_t mId,  eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eObrd_cantype_t *type_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr = NULL;
    eOappTheDB_hid_motorInfo_t *m_ptr = NULL;
    uint16_t indexincanboardarray = 0;
    
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(mId >= eo_array_Size(p->motorsInfo))
    {   // i attempt to retrieve a position beyond its size.
        // for instance: position 4 or 5 or 6 or higher number when the size of array is only 4
        return(eores_NOK_nodata);
    }
    // eo_array_At() returns NULL only if mId is beyond capacity, not size.
    m_ptr = (eOappTheDB_hid_motorInfo_t *)eo_array_At(p->motorsInfo, mId);
  
    indexincanboardarray = m_ptr->mapping2canboard.indexofcanboard;
    
    // now i must verify that we have     
    if(indexincanboardarray >= eo_array_Size(p->canboardsInfo))
    {
        return(eores_NOK_nodata);
    }

    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsInfo, indexincanboardarray);
    
    // ok, now i can fill the return values    
    canloc_ptr->indexinsidecanboard     = (eOicubCanProto_jm_indexinBoard_t)m_ptr->mapping2canboard.indexinsidecanboard;    
    canloc_ptr->addr                    = b_ptr->basicboardinfo.addr;
    canloc_ptr->emscanport              = (eOcanport_t)b_ptr->basicboardinfo.port;
    if(NULL != type_ptr)
    {
        *type_ptr = (eObrd_cantype_t)b_ptr->basicboardinfo.type;
    }
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetMotorId_ByMotorCanLocation(EOappTheDB *p, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eOmc_motorId_t *mId_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr;
    uint16_t indexincanboardarray = 0;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == mId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    if((s_appTheDB_canportconnected2motorboards != canloc_ptr->emscanport) || (canloc_ptr->addr >= p->canaddressLookuptbl.capacity))
    {
        return(eores_NOK_generic);
    }
    
    indexincanboardarray = eo_appTheDB_hid_GetBoardIdWithAddress(p, canloc_ptr->addr);
    
    if(indexincanboardarray >= eo_array_Size(p->canboardsInfo))
    {
        return(eores_NOK_nodata);
    }

    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsInfo, indexincanboardarray);

    
    // now fill the return value
    *mId_ptr = b_ptr->connectedmotors[canloc_ptr->indexinsidecanboard];
    if(*mId_ptr == EOK_uint08dummy)
    {
        return(eores_NOK_nodata);
    }

    return(eores_OK);
}



extern eOresult_t eo_appTheDB_GetJointId_ByJointCanLocation(EOappTheDB *p, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eOmc_jointId_t *jId_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr;
    uint16_t indexincanboardarray = 0;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == jId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    if((s_appTheDB_canportconnected2motorboards != canloc_ptr->emscanport) || (canloc_ptr->addr >= p->canaddressLookuptbl.capacity))
    {
        return(eores_NOK_generic);
    }
    
    indexincanboardarray = eo_appTheDB_hid_GetBoardIdWithAddress(p, canloc_ptr->addr);
    
    if(indexincanboardarray >= eo_array_Size(p->canboardsInfo))
    {
        return(eores_NOK_nodata);
    }

    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsInfo, indexincanboardarray);

    
    // now fill the return value
    *jId_ptr = b_ptr->connectedjoints[canloc_ptr->indexinsidecanboard];
    if(*jId_ptr == EOK_uint08dummy)
    {
        return(eores_NOK_nodata);
    }

    return(eores_OK);
}




extern eOresult_t eo_appTheDB_GetSnsrMaisId_BySensorCanLocation(EOappTheDB *p, eOappTheDB_board_canlocation_t *canloc_ptr, eOas_maisId_t *sId_ptr)
{

    eOappTheDB_hid_canBoardInfo_t *b_ptr = NULL;
    eOappTheDB_hid_snsrMaisInfo_t *s_ptr = NULL;
    uint16_t i;

    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == sId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    
    for(i=0; i<eo_array_Size(p->maisesInfo); i++)
    {
        s_ptr = (eOappTheDB_hid_snsrMaisInfo_t*) eo_array_At(p->maisesInfo, i);
        if(NULL == s_ptr->cfg_ptr)
        {
            return(eores_NOK_nodata);
        }
        uint16_t indexofcanboard = s_ptr->cfg_ptr->indexofcanboard;
        if(indexofcanboard >= eo_array_Size(p->canboardsInfo))
        {
            return(eores_NOK_nodata);
        } 
        b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsInfo, indexofcanboard);

        if( (canloc_ptr->emscanport == b_ptr->basicboardinfo.port) &&
             (canloc_ptr->addr == b_ptr->basicboardinfo.addr) )
        {
            // ok, i have found the mais with can location matching canloc_ptr
            *sId_ptr = i;
            return(eores_OK);
        }
    }
    
    return(eores_NOK_nodata);
}



extern eOresult_t eo_appTheDB_GetSnsrMaisCanLocation(EOappTheDB *p, eOas_maisId_t sId, eOappTheDB_board_canlocation_t *canloc_ptr)
{
    eOappTheDB_hid_snsrMaisInfo_t       *s_ptr = NULL;
    eOappTheDB_hid_canBoardInfo_t       *b_ptr = NULL;
       
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(sId >= eo_array_Size(p->maisesInfo))
    {
        return(eores_NOK_nodata);
    }
    
    s_ptr = (eOappTheDB_hid_snsrMaisInfo_t *)eo_array_At(p->maisesInfo, sId);
    if(NULL == s_ptr->cfg_ptr)
    {
        return(eores_NOK_nodata);
    }
    uint16_t indexofcanboard = s_ptr->cfg_ptr->indexofcanboard;
    if(indexofcanboard >= eo_array_Size(p->canboardsInfo))
    {
        return(eores_NOK_nodata);
    } 

    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsInfo, indexofcanboard);

    // now fill the return value
    canloc_ptr->emscanport  = b_ptr->basicboardinfo.port;
    canloc_ptr->addr        = b_ptr->basicboardinfo.addr;
    canloc_ptr->unused      = 0;
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(EOappTheDB *p, eOappTheDB_board_canlocation_t *canloc_ptr, eOas_strainId_t *sId_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr = NULL;
    eOappTheDB_hid_snsrStrainInfo_t  *s_ptr = NULL;
    uint16_t i;

    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == sId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    
    for(i=0; i<eo_array_Size(p->strainsInfo); i++)
    {
        s_ptr = (eOappTheDB_hid_snsrStrainInfo_t*)eo_array_At(p->strainsInfo, i);
        if(NULL == s_ptr->cfg_ptr)
        {
            return(eores_NOK_nodata);
        }
        uint16_t indexofcanboard = s_ptr->cfg_ptr->indexofcanboard;
        if(indexofcanboard >= eo_array_Size(p->canboardsInfo))
        {
            return(eores_NOK_nodata);
        } 
        b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsInfo, indexofcanboard);
 
        if( (canloc_ptr->emscanport == b_ptr->basicboardinfo.port) &&
            (canloc_ptr->addr == b_ptr->basicboardinfo.addr) )
        {
            // ok, i have found the strain with can location canloc_ptr
            *sId_ptr = i;
            return(eores_OK);
        }
    }
    
    return(eores_NOK_nodata);
}



extern eOresult_t eo_appTheDB_GetSnsrStrainCanLocation(EOappTheDB *p, eOas_strainId_t sId, eOappTheDB_board_canlocation_t *canloc_ptr)
{
    eOappTheDB_hid_snsrStrainInfo_t     *s_ptr;
    eOappTheDB_hid_canBoardInfo_t       *b_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    if(sId >= eo_array_Size(p->strainsInfo))
    {
        return(eores_NOK_nodata);
    }
    
    s_ptr = (eOappTheDB_hid_snsrStrainInfo_t *)eo_array_At(p->strainsInfo, sId);
    if(NULL == s_ptr->cfg_ptr)
    {
        return(eores_NOK_nodata);
    }
    uint16_t indexofcanboard = s_ptr->cfg_ptr->indexofcanboard;
    if(indexofcanboard >= eo_array_Size(p->canboardsInfo))
    {
        return(eores_NOK_nodata);
    }  
    b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsInfo, s_ptr->cfg_ptr->indexofcanboard);

    // now i fill the return value
    canloc_ptr->emscanport  = b_ptr->basicboardinfo.port;
    canloc_ptr->addr        = b_ptr->basicboardinfo.addr;
    canloc_ptr->unused      = 0;
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetSkinCanLocation(EOappTheDB *p, eOsk_skinId_t skId, eOappTheDB_SkinCanLocation_t *canloc_ptr)
{
    eOappTheDB_hid_skinInfo_t *sk_ptr = NULL;
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    if(skId >= eo_array_Size(p->skinsInfo))
    {
        return(eores_NOK_nodata);
    }
    
    sk_ptr = (eOappTheDB_hid_skinInfo_t *)eo_array_At(p->skinsInfo, skId);
    if(NULL == sk_ptr->cfg_ptr)
    {
        return(eores_NOK_nodata);
    }

    // now i fill the return value
    canloc_ptr->emscanport = sk_ptr->cfg_ptr->connected2emsport;
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetSkinId_BySkinCanLocation(EOappTheDB *p, eOappTheDB_SkinCanLocation_t *canloc_ptr, eOsk_skinId_t *skId_ptr)
{
    uint16_t i;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == skId_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    for(i=0; i<eo_array_Size(p->skinsInfo); i++)
    {
        eOappTheDB_hid_skinInfo_t *sk_ptr = (eOappTheDB_hid_skinInfo_t*)eo_array_At(p->skinsInfo, i);
        if((NULL != sk_ptr->cfg_ptr) && (canloc_ptr->emscanport == sk_ptr->cfg_ptr->connected2emsport))
        {
            // ok, i have found the skin with can location canloc_ptr
            *skId_ptr = i;
            return(eores_OK);
        }
    }
    
    return(eores_NOK_nodata);

}


extern eOresult_t eo_appTheDB_GetShiftValuesOfJointPtr(EOappTheDB *p, eOmc_jointId_t jId, eOappTheDB_jointShiftValues_t **shiftval_ptr)
{   
    if((NULL == p) || (NULL == shiftval_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= eo_array_Size(p->jointsInfo))
    {
        return(eores_NOK_nodata);
    }
    
    eOappTheDB_hid_jointInfo_t *j_ptr = (eOappTheDB_hid_jointInfo_t *)eo_array_At(p->jointsInfo, jId);
    *shiftval_ptr = j_ptr->shiftvalues_ptr;
      
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetJointBcastpolicyPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOicubCanProto_bcastpolicy_t **bcast_ptr)
{
    if((NULL == p) || (NULL == bcast_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(jId >= eo_array_Size(p->jointsInfo))
    {
        return(eores_NOK_nodata);
    }

    eOappTheDB_hid_jointInfo_t *j_ptr = (eOappTheDB_hid_jointInfo_t *)eo_array_At(p->jointsInfo, jId);
    *bcast_ptr = j_ptr->bcastpolicy_ptr;

    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetSkinConfigPtr(EOappTheDB *p, eOsk_skinId_t sId, eOappTheDB_cfg_skinInfo_t **skconfig_ptr)
{
    if((NULL == p) || (NULL == skconfig_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(sId >= eo_array_Size(p->skinsInfo))
    {
        return(eores_NOK_nodata);
    }
    
    eOappTheDB_hid_skinInfo_t *sk_ptr = (eOappTheDB_hid_skinInfo_t*) eo_array_At(p->skinsInfo, sId);
    *skconfig_ptr = sk_ptr->cfg_ptr;
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetCanBoardInfo(EOappTheDB *p, eObrd_boardId_t bid, eOappTheDB_canboardinfo_t **ppcanboardinfo)
{
    if((NULL == p) || (NULL == ppcanboardinfo))
    {
        return(eores_NOK_nullpointer);
    }
    if(bid >= eo_array_Size(p->canboardsInfo))
    {
        return(eores_NOK_nodata);
    }
    eOappTheDB_hid_canBoardInfo_t *b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsInfo, bid);
    *ppcanboardinfo = &b_ptr->basicboardinfo;
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetCanBoardId_ByCanLocation(EOappTheDB *p, eOappTheDB_board_canlocation_t *canloc_ptr, eObrd_boardId_t *bid_ptr)
{    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == bid_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    uint16_t i;
    uint16_t numofboards = eo_array_Size(p->canboardsInfo);    
    for(i=0; i<numofboards; i++)
    {
        eOappTheDB_hid_canBoardInfo_t *b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsInfo, i);
        if( (b_ptr->basicboardinfo.port == canloc_ptr->emscanport) && (b_ptr->basicboardinfo.addr == canloc_ptr->addr) )
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
        } break;

        case 1:
        {
            p->virtualStrainData.values[0] = torquevalue; // wrist yaw
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


extern eOresult_t eo_appTheDB_setCanBoardReady(EOappTheDB *p, eOappTheDB_board_canlocation_t *canloc_ptr)
{
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    uint16_t i;
    uint16_t numofboard = eo_array_Size(p->canboardsInfo);
    for(i=0; i<numofboard; i++)
    {
        eOappTheDB_hid_canBoardInfo_t *b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsInfo, i);
        if( (b_ptr->basicboardinfo.port == canloc_ptr->emscanport) && (b_ptr->basicboardinfo.addr == canloc_ptr->addr) )
        {
            b_ptr->isready = eobool_true; 
            return(eores_OK);
        }
    }
    
    return(eores_NOK_nodata);    
}


extern eObool_t eo_appTheDB_areConnectedCanBoardsReady(EOappTheDB *p, uint32_t *canBoardsReady)
{
    eObool_t res = eobool_true;
    
    if((NULL == p) && (NULL == canBoardsReady))
    {
        return(eobool_false);
    }
    

    *canBoardsReady = 0;
    
    uint16_t i;
    for(i=0; i<eo_array_Size(p->canboardsInfo); i++)
    {        
        eOappTheDB_hid_canBoardInfo_t *b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsInfo, i);
        if((eobrd_mc4 != b_ptr->basicboardinfo.type) && (eobrd_1foc != b_ptr->basicboardinfo.type))
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
        // se sono qui allora non ho ricevuto tutte le risposte can che mi aspettavo...e quindi e' rimansta una entri sporca...
        // marco.accame: corrected the use of the list object, so that we dont use its internals anymore
        // .... however in here i just set the entry to 0 . is it what it is wanted ???
        eOappTheDB_hid_ethProtoRequest_t *item = (eOappTheDB_hid_ethProtoRequest_t*) eo_list_At(ethProtoReq_list, li);
        item->numOfREceivedResp = 0;
        //  ((eOappTheDB_hid_ethProtoRequest_t *)li->data)->numOfREceivedResp = 0;
        #warning --> marco.accame: if in here .... then what? maybe we .... THINK of it
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
    eOprotEndpoint_t ep = eoprot_ID2endpoint(id32);
    eOprotEntity_t entity = eoprot_ID2entity(id32);
    eOprotIndex_t index  = eoprot_ID2index(id32); 
   
    EOlist *ethProtoReq_list = s_appTheDB_getEthProtoReqList(p, eoprot_endpoint_motioncontrol, entity, index);
    
    // if ethProtoReq_list is NULL ... then eo_list_Find() returns NULL

    return(eo_list_Find(ethProtoReq_list, s_appTheDB_searchEthProtoReq_matchingRule, &key));
}


extern eOresult_t eo_appTheDB_removeEthProtoRequest(EOappTheDB *p, eOprotEntity_t entity, eOprotIndex_t index, EOlistIter* li)
{
    eo_list_Erase(s_appTheDB_getEthProtoReqList(p, eoprot_endpoint_motioncontrol, entity, index), li);
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

static eObool_t s_appTheDB_checkConfiguration(eOappTheDB_cfg_t *cfg)
{
    if((NULL == cfg->jointsList) || (NULL == cfg->motorsList) || (NULL == cfg->snsrMaisList) ||
       (NULL == cfg->snsrStrainList) || (NULL == cfg->skinList))
    {
        return(eobool_false);
    }

    //check if ep cfg and db cfg are consistent
    if(eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint) != eo_constvector_Size(cfg->jointsList))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "joints cfg mismatch", s_eobj_ownname, &eo_errman_DescrTobedecided);
        return(eobool_false);
    }

    if(eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_motioncontrol, eoprot_entity_mc_motor) != eo_constvector_Size(cfg->motorsList))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "motors cfg mismatch", s_eobj_ownname, &eo_errman_DescrTobedecided);
        return(eobool_false);
    }

    if(eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_mais) != eo_constvector_Size(cfg->snsrMaisList))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "mais cfg mismatch", s_eobj_ownname, &eo_errman_DescrTobedecided);
        return(eobool_false);
    }
    
    if(eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_analogsensors, eoprot_entity_as_strain) != eo_constvector_Size(cfg->snsrStrainList))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "strain cfg mismach", s_eobj_ownname, &eo_errman_DescrTobedecided);
        return(eobool_false);
    }
    
    if(eoprot_entity_numberof_get(eoprot_board_localboard, eoprot_endpoint_skin, eoprot_entity_sk_skin) != eo_constvector_Size(cfg->skinList))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, "skin cfg mismach", s_eobj_ownname, &eo_errman_DescrTobedecided);
        return(eobool_false);
    }
    
    return(eobool_true);
}


static eOresult_t s_appTheDB_canboardslist_init(EOappTheDB *p)
{
    eOsizecntnr_t i;
    uint8_t k;
       
    // if no canboard is connected to board (skin only mode) ==> nothing to do and return ok 

    if(0 == eo_constvector_Size(p->cfg.canboardsList))
    {
        p->canboardsInfo = NULL;
        return(eores_OK);
    }
    
    
    // 1) create canboardsInfo
    p->canboardsInfo = eo_array_New(eo_constvector_Size(p->cfg.canboardsList), sizeof(eOappTheDB_hid_canBoardInfo_t), NULL);
    
   
    for(i=0; i<eo_constvector_Size(p->cfg.canboardsList); i++)
    {
        // marco.accame: correct use of constvector
        eOappTheDB_canboardinfo_t* p2canboardinfo = (eOappTheDB_canboardinfo_t*) eo_constvector_At(p->cfg.canboardsList, i);
        
        eOappTheDB_hid_canBoardInfo_t item = {0};
        
        item.basicboardinfo.addr    = p2canboardinfo->addr;
        item.basicboardinfo.port    = p2canboardinfo->port;
        item.basicboardinfo.type    = p2canboardinfo->type;

        // create array of "connected stuff" (joints or sensors)
        if((eobrd_mc4 == p2canboardinfo->type) || (eobrd_1foc == p2canboardinfo->type))
        {           
            for(k=0; k<eOicubCanProto_jm_indexInBoard_max; k++)
            {
                item.connectedjoints[k] = EOK_uint08dummy;
                item.connectedmotors[k] = EOK_uint08dummy;
            }                       
        }
        else
        {
            // put something in here ... for instance the same dummy values as before 
            for(k=0; k<eOicubCanProto_jm_indexInBoard_max; k++)
            {
                item.connectedjoints[k] = EOK_uint08dummy;
                item.connectedmotors[k] = EOK_uint08dummy;
            }            
        }
        
//         else if((eobrd_mais == b_ptr->cfg_ptr->type) || (eobrd_skin == b_ptr->cfg_ptr->type) || (eobrd_strain == b_ptr->cfg_ptr->type))
//         {
//             b_ptr->s.connectedsensors =  eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
//                                                     sizeof(eOas_sensorId_t), 1); //currently almost only one sensor per board
//         }
        
        // set the board as not ready
        item.isready = eobool_false;
               
        // now, finally pusk the item back.        
        eo_array_PushBack(p->canboardsInfo, &item);       
    }
    
    return(eores_OK);
}


static eOresult_t s_appTheDB_jointslist_init(EOappTheDB *p)
{
    eOsizecntnr_t                       i;
    eOappTheDB_jointShiftValues_t       *shiftvalues_ptr;
    eOicubCanProto_bcastpolicy_t        *bcastpolicy_ptr;    
    
    // if no joint is connected to board (skin only mode) ==> nothing to do and return ok 
    if(0 == eo_constvector_Size(p->cfg.jointsList))
    {
        p->jointsInfo = NULL;
        return(eores_OK);
    }
    
    
    //1) create jointList
    p->jointsInfo = eo_array_New(eo_constvector_Size(p->cfg.jointsList), sizeof(eOappTheDB_hid_jointInfo_t), NULL);
   
        
    //2.1) allocate memory where i save data shared between all joints
    // marco.accame: for now, every joint has the same shift values and the same broadcast policy.   
    shiftvalues_ptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOappTheDB_jointShiftValues_t), 1);
    bcastpolicy_ptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOicubCanProto_bcastpolicy_t), 1);
    
    //#warning marco.accame: can we use only one long list for the proxy requests of all joints/motors rather than many small lists one per jo/mo ??  
    for(i=0; i<eo_constvector_Size(p->cfg.jointsList); i++)
    {   // for all the joints in configuration
        
        eOappTheDB_mapping2canboard_t* map2can = (eOappTheDB_mapping2canboard_t*) eo_constvector_At(p->cfg.jointsList, i);
        eOappTheDB_hid_jointInfo_t jointinfoitem = {0};
        
        // build the item
        jointinfoitem.mapping2canboard.indexofcanboard      = map2can->indexofcanboard;
        jointinfoitem.mapping2canboard.indexinsidecanboard  = map2can->indexinsidecanboard;
        jointinfoitem.bcastpolicy_ptr                       = bcastpolicy_ptr;
        jointinfoitem.shiftvalues_ptr                       = shiftvalues_ptr;
#ifdef USE_PROTO_PROXY
        jointinfoitem.ethProtoReq_list  = eo_list_New(sizeof(eOappTheDB_hid_ethProtoRequest_t), db_numOfEthProtoReq_for_entity, NULL, 0, NULL, NULL);
#endif
        
        // put it inside the array
        eo_array_PushBack(p->jointsInfo, &jointinfoitem);
        
        // make connection between board and joint i-th
        if(map2can->indexofcanboard >= eo_array_Size(p->canboardsInfo))
        {
            #warning --> call error manager 
        }
        eOappTheDB_hid_canBoardInfo_t *boardinfoitem = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsInfo, map2can->indexofcanboard);
        boardinfoitem->connectedjoints[map2can->indexinsidecanboard] = (eOmc_jointId_t)i;

    }
    return(eores_OK);
}


static eOresult_t s_appTheDB_motorslist_init(EOappTheDB *p)
{
    // if no motor is connected to board (skin only mode) ==> nothing to do and return ok 
    if(0 == eo_constvector_Size(p->cfg.motorsList))
    {
        p->motorsInfo = NULL;
        return(eores_OK);
    }
    

    //1) create jointList
    p->motorsInfo = eo_array_New(eo_constvector_Size(p->cfg.motorsList), sizeof(eOappTheDB_hid_motorInfo_t), NULL);
    
    eOappTheDB_hid_motorInfo_t motorinfoitem = {0};

    eOsizecntnr_t i;
    for(i=0; i<eo_constvector_Size(p->cfg.motorsList); i++)
    {   // for all the motors in configuration
        
        eOappTheDB_mapping2canboard_t* map2can = (eOappTheDB_mapping2canboard_t*) eo_constvector_At(p->cfg.motorsList, i);
        
        // build the item
        motorinfoitem.mapping2canboard.indexinsidecanboard = map2can->indexinsidecanboard;
        motorinfoitem.mapping2canboard.indexofcanboard =  map2can->indexofcanboard;
#ifdef USE_PROTO_PROXY
        motorinfoitem.ethProtoReq_list = eo_list_New(sizeof(eOappTheDB_hid_ethProtoRequest_t), db_numOfEthProtoReq_for_entity, NULL, 0, NULL, NULL);   
#endif     
        
        // put it inside the array
        eo_array_PushBack(p->motorsInfo, &motorinfoitem);     

        // make connection between board and motor i-th
        if(map2can->indexofcanboard >= eo_array_Size(p->canboardsInfo))
        {
            #warning --> call error manager 
        }
        eOappTheDB_hid_canBoardInfo_t *boardinfoitem = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsInfo, map2can->indexofcanboard);
        boardinfoitem->connectedmotors[map2can->indexinsidecanboard] = (eOmc_motorId_t)i; 
    }
    
    return(eores_OK);
}


static eOresult_t s_appTheDB_snsrMaislist_init(EOappTheDB *p)
{ 
    //if no sensor is connected to board ==> nothing to do and return ok
    if(0 == eo_constvector_Size(p->cfg.snsrMaisList))
    {
        p->maisesInfo = NULL;
        return(eores_OK);
    }
    
    //1) create sensors mais List
    p->maisesInfo = eo_array_New(eo_constvector_Size(p->cfg.snsrMaisList), sizeof(eOappTheDB_hid_snsrMaisInfo_t), NULL);
    
    eOsizecntnr_t i;
    for(i=0; i<eo_constvector_Size(p->cfg.snsrMaisList); i++)
    {       
        eOappTheDB_mapping2canboard_t* p2info = (eOappTheDB_mapping2canboard_t*) eo_constvector_At(p->cfg.snsrMaisList, i); 
        eOappTheDB_hid_snsrMaisInfo_t item = {NULL};
        item.cfg_ptr = p2info;
        eo_array_PushBack(p->maisesInfo, &item);        
    }
    
    return(eores_OK);
}


static eOresult_t s_appTheDB_snsrStrainlist_init(EOappTheDB *p)
{  
    //if no sensor is connected to board ==> nothing to do and return ok
    if(0 == eo_constvector_Size(p->cfg.snsrStrainList))
    {
        p->strainsInfo = NULL;
        return(eores_OK);
    }
    
    //1) create sensors strain List
    p->strainsInfo = eo_array_New(eo_constvector_Size(p->cfg.snsrStrainList), sizeof(eOappTheDB_hid_snsrStrainInfo_t), NULL);

    eOsizecntnr_t i;
    for(i=0; i<eo_constvector_Size(p->cfg.snsrStrainList); i++)
    {
        eOappTheDB_mapping2canboard_t* p2info = (eOappTheDB_mapping2canboard_t*) eo_constvector_At(p->cfg.snsrStrainList, i);
        eOappTheDB_hid_snsrStrainInfo_t item = {NULL};
        item.cfg_ptr = p2info;
        eo_array_PushBack(p->strainsInfo, &item);     
    }
    
    return(eores_OK);
}


static eOresult_t s_appTheDB_skinlist_init(EOappTheDB *p)
{
    //if no sensor is connected to board ==> nothing to do and return ok
    if(0 == eo_constvector_Size(p->cfg.skinList))
    {
        p->skinsInfo = NULL;
        return(eores_OK);
    }
    
    //1) create sensorsList
    p->skinsInfo = eo_array_New(eo_constvector_Size(p->cfg.skinList), sizeof(eOappTheDB_hid_skinInfo_t), NULL);

    eOsizecntnr_t i;
    for(i=0; i< eo_constvector_Size(p->cfg.skinList); i++)
    {
        eOappTheDB_cfg_skinInfo_t* p2info = (eOappTheDB_cfg_skinInfo_t*) eo_constvector_At(p->cfg.skinList, i);
        eOappTheDB_hid_skinInfo_t item = {NULL};
        item.cfg_ptr = p2info;
        eo_array_PushBack(p->skinsInfo, &item);     
    }
    
    return(eores_OK);
}

// marco.accame; it would be better to have a fixed array of 16 bytes rather than to compute max address and then allocate dymanic memory
static eOresult_t s_appTheDB_canaddressLookuptbl_init(EOappTheDB *p)
{
    icubCanProto_canBoardAddress_t maxusedcanaddr = 0;
    
    if(0 == eo_constvector_Size(p->cfg.canboardsList))
    {
        p->canaddressLookuptbl.capacity = 0;
        p->canaddressLookuptbl.tbl      = NULL;
        return(eores_OK);
    }
    
    //1) get max used can address (it will be in range [1,E]. 0 is ems can port address and F is bradcast address)        
    eOsizecntnr_t i;
    for(i=0, maxusedcanaddr=0; i<eo_constvector_Size(p->cfg.canboardsList); i++)
    {
        eOappTheDB_canboardinfo_t* p2info = (eOappTheDB_canboardinfo_t*) eo_constvector_At(p->cfg.canboardsList, i);
        
        //  select only the addresses that are:
        //  - connected 2 ems port used to commnd joint (joints and motors are connected to a different can bus of sensors) AND
        //  - belong to motor board (1 foc and mc4)
        if( (s_appTheDB_canportconnected2motorboards == p2info->port) &&
            ((eobrd_1foc == p2info->type) || (eobrd_mc4 == p2info->type)) &&
            (maxusedcanaddr < p2info->addr))
        {
            maxusedcanaddr = p2info->addr;
        }   
    }

    // 2) create the lookup tbl
    p->canaddressLookuptbl.capacity = maxusedcanaddr+1; //cosi' lascio l'elemento 0-esimo, anche se non lo uso.    
    p->canaddressLookuptbl.tbl = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eObrd_boardId_t), 
                                                      p->canaddressLookuptbl.capacity);
    
    // 3) fill lookup tbl
    for(i=0; i<eo_constvector_Size(p->cfg.canboardsList); i++)
    {
        eOappTheDB_canboardinfo_t* p2info = (eOappTheDB_canboardinfo_t*) eo_constvector_At(p->cfg.canboardsList, i);

        // marco.accame:    teh opposite of previous condition is not the following code. 
        //                  if we apply boolean logic it is: 
        //                  (portofmotors != port) || (! ((1foc==type) ||(mc4==type)) ) --->
        //                  (portofmotors != port) || ( ((1foc!=type) && (mc4!=type)) )
        // much better using a different mode

        // marco.accame: it was
#if 0        
        if((s_appTheDB_canportconnected2motorboards != p2info->port) &&
           (eobrd_1foc != p2info->type) && (eobrd_mc4 != p2info->type))
        {
            continue;
        }  
        addr = p2info->addr;
        p->canaddressLookuptbl.tbl[addr] = (eObrd_boardId_t)i;
#else
        if( (s_appTheDB_canportconnected2motorboards == p2info->port) &&
            ((eobrd_1foc == p2info->type) || (eobrd_mc4 == p2info->type)))  
        {        
            icubCanProto_canBoardAddress_t addr = p2info->addr;
            p->canaddressLookuptbl.tbl[addr] = (eObrd_boardId_t)i;
        }
#endif
    }
    
    return(eores_OK);
}



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
        if(index < eo_array_Size(p->jointsInfo))
        {
            eOappTheDB_hid_jointInfo_t *j_ptr = (eOappTheDB_hid_jointInfo_t *)eo_array_At(p->jointsInfo, index);
            return(j_ptr->ethProtoReq_list);            
        }
    }
    
    if((eoprot_endpoint_motioncontrol == ep) && (eoprot_entity_mc_motor == entity))
    {
        if(index < eo_array_Size(p->motorsInfo))
        {
            eOappTheDB_hid_motorInfo_t * m_ptr = (eOappTheDB_hid_motorInfo_t *)eo_array_At(p->motorsInfo, index);
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



