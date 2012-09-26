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
#include "EOnvsCfg.h"

#include "EOconstvector_hid.h" 
#include "EOnv_hid.h"

#include "eOcfg_nvsEP_mc.h"
#include "eOcfg_nvsEP_as.h"
#include "eOcfg_nvsEP_sk.h"

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

static eOresult_t s_appTheDB_nvsrefmaps_init(EOappTheDB *p);
static eOresult_t s_appTheDB_nvsrefmaps_jmap_fill(EOappTheDB *p, eOmc_jointId_t jId);
static eOresult_t s_appTheDB_nvsrefmaps_mmap_fill(EOappTheDB *p, eOmc_motorId_t mId);
static eOresult_t s_appTheDB_nvsrefmaps_snsrMaismap_fill(EOappTheDB *p, eOsnsr_maisId_t sId);
static eOresult_t s_appTheDB_nvsrefmaps_snsrStrainmap_fill(EOappTheDB *p, eOsnsr_strainId_t sId);
static eOresult_t s_appTheDB_nvsrefmaps_skinmap_fill(EOappTheDB *p, eOsk_skinId_t skId);
static void* s_eo_appTheDB_GetJointNVreference(EOappTheDB *p, eOmc_jointId_t jId, eOappTheDB_hid_jointNVindex_t nvindex);
static void* s_eo_appTheDB_GetMotorNVreference(EOappTheDB *p, eOmc_motorId_t mId, eOappTheDB_hid_motorNVindex_t nvindex);
static void* s_eo_appTheDB_GetSnsrMaisNVreference(EOappTheDB *p, eOsnsr_maisId_t sId, eOappTheDB_hid_snsrMaisNVindex_t nvindex);
static void* s_eo_appTheDB_GetSnsrStrainNVreference(EOappTheDB *p, eOsnsr_strainId_t sId, eOappTheDB_hid_snsrStrainNVindex_t nvindex);
static void* s_eo_appTheDB_GetSkinNVreference(EOappTheDB *p, eOsk_skinId_t skId, eOappTheDB_hid_skinNVindex_t nvindex);


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

    res = s_appTheDB_nvsrefmaps_init(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }

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


extern eObool_t	eo_appTheDB_isSkinConnected(EOappTheDB *p)
{

    if((NULL == p) || (NULL == p->skinList))
    {
        return(0);
    }
    return(1);

}


extern eOresult_t eo_appTheDB_SetjointsShiftValues(EOappTheDB *p, eOappTheDB_jointShiftValues_t *shiftval_ptr)
{
    if((NULL == p) || (NULL == shiftval_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    return(eores_OK);
}

extern eOresult_t eo_appTheDB_SetJointsBcastpolicyPtr(EOappTheDB *p, eOicubCanProto_bcastpolicy_t *bcast_ptr)
{
    if((NULL == p) || (NULL == bcast_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    return(eores_OK);
}



extern eOresult_t eo_appTheDB_GetShiftValuesOfJoint(EOappTheDB *p, eOmc_jointId_t jId, eOappTheDB_jointShiftValues_t *shiftval_ptr)
{
    eOappTheDB_hid_jointInfo_t   *j_ptr;
	
    if((NULL == p) || (NULL == shiftval_ptr))
    {
        return(eores_NOK_nullpointer);
    }
	
    j_ptr = (eOappTheDB_hid_jointInfo_t *)eo_array_At(p->jointsList, jId);
    if(NULL == j_ptr)
    {
        return(eores_NOK_nodata);
    }

    memcpy(shiftval_ptr, j_ptr->shiftvalues_ptr, sizeof(eOappTheDB_jointShiftValues_t));
    
    return(eores_OK);
}


extern eOappEncReader_encoder_t eo_appTheDB_GetEncoderConnected2Joint(EOappTheDB *p, eOmc_jointId_t jId)
{
	eOappTheDB_hid_jointInfo_t *j_ptr;
    if(NULL == p)
    {
        return(eOeOappEncReader_encoderUnused);
    }
	
	j_ptr = (eOappTheDB_hid_jointInfo_t *)eo_array_At(p->jointsList, jId);
	
	if(NULL == j_ptr)
	{
		return(eOeOappEncReader_encoderUnused);
	}
	
    return(j_ptr->connectedEnc);
}

extern eOresult_t eo_appTheDB_GetJointCanLocation(EOappTheDB *p, eOmc_jointId_t jId,  eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eObrd_types_t *type_ptr)
{
    eOappTheDB_hid_canBoardInfo_t   *b_ptr;
	eOappTheDB_hid_jointInfo_t 		*j_ptr;
	
	
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
    eOappTheDB_hid_canBoardInfo_t		*b_ptr;
	eOappTheDB_hid_motorInfo_t 		*m_ptr;
	
	
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
    *mId_ptr = b_ptr->s.jm.connectedmotors[canloc_ptr->indexinboard];

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
    *jId_ptr = b_ptr->s.jm.connectedjoints[canloc_ptr->indexinboard];

    return(eores_OK);
}




extern eOresult_t eo_appTheDB_GetSnsrMaisId_BySensorCanLocation(EOappTheDB *p, eOappTheDB_sensorCanLocation_t *canloc_ptr, eOsnsr_maisId_t *sId_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr;
    eOappTheDB_hid_snsrMaisInfo_t  *s_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == sId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    s_ptr = eo_array_At(p->snsrMaisList, 0);
    
    /* currently only one mais is connetced to a ems.
       so i check if the can loc is of connected mais. */
    
    if(NULL == s_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    
	b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, s_ptr->cfg_ptr->belong2board);
    if( (canloc_ptr->emscanport == b_ptr->cfg_ptr->canLoc.emscanport) &&
        (canloc_ptr->addr == b_ptr->cfg_ptr->canLoc.addr) )
    {
        *sId_ptr = 0; 
    }

    return(eores_OK);

}



extern eOresult_t eo_appTheDB_GetSnsrMaisCanLocation(EOappTheDB *p, eOsnsr_maisId_t sId, eOappTheDB_sensorCanLocation_t *canloc_ptr)
{
    eOappTheDB_hid_snsrMaisInfo_t		*s_ptr;
	eOappTheDB_hid_canBoardInfo_t		*b_ptr;
    
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
	
    canloc_ptr->addr = b_ptr->cfg_ptr->canLoc.addr;
    canloc_ptr->emscanport = b_ptr->cfg_ptr->canLoc.emscanport;
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetSnsrStrainId_BySensorCanLocation(EOappTheDB *p, eOappTheDB_sensorCanLocation_t *canloc_ptr, eOsnsr_strainId_t *sId_ptr)
{
    eOappTheDB_hid_canBoardInfo_t *b_ptr;
    eOappTheDB_hid_snsrStrainInfo_t  *s_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == sId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    s_ptr = eo_array_At(p->snsrStrainList, 0);
    
    /* currently only one strain is connetced to a ems.
       so i check if the can loc is of connected mais. */
    
    if(NULL == s_ptr)
    {
        return(eores_NOK_nodata);
    }
    
    
	b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, s_ptr->cfg_ptr->belong2board);
    if( (canloc_ptr->emscanport == b_ptr->cfg_ptr->canLoc.emscanport) &&
        (canloc_ptr->addr == b_ptr->cfg_ptr->canLoc.addr) )
    {
        *sId_ptr = 0; 
    }

    return(eores_OK);

}



extern eOresult_t eo_appTheDB_GetSnsrStrainCanLocation(EOappTheDB *p, eOsnsr_strainId_t sId, eOappTheDB_sensorCanLocation_t *canloc_ptr)
{
    eOappTheDB_hid_snsrStrainInfo_t		*s_ptr;
	eOappTheDB_hid_canBoardInfo_t		*b_ptr;
    
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
	
    canloc_ptr->addr = b_ptr->cfg_ptr->canLoc.addr;
    canloc_ptr->emscanport = b_ptr->cfg_ptr->canLoc.emscanport;
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetSkinCanLocation(EOappTheDB *p, eOsk_skinId_t skId, eOappTheDB_SkinCanLocation_t *canloc_ptr)
{
    eOappTheDB_hid_skinInfo_t   		*sk_ptr;
    
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


extern eOresult_t eo_appTheDB_GetJointConfigPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_joint_config_t **jconfig_ptr)
{
	if((NULL == p) || (NULL == jconfig_ptr))
	{
        return(eores_NOK_nullpointer);
	}
    
    if(jId >= eo_array_Capacity(p->jointsList))
    {
        return(eores_NOK_nodata);
    }
    *jconfig_ptr = s_eo_appTheDB_GetJointNVreference(p, jId, eOappTheDB_hid_jointNVindex_jconfig);
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetJointStatusPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_joint_status_t **jstatus_ptr)
{
	if((NULL == p) || (NULL == jstatus_ptr))
	{
        return(eores_NOK_nullpointer);
	}
    
    if(jId >= eo_array_Capacity(p->jointsList))
    {
        return(eores_NOK_nodata);
    }
    *jstatus_ptr = s_eo_appTheDB_GetJointNVreference(p, jId, eOappTheDB_hid_jointNVindex_jstatus);
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetJointInputsPtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_joint_inputs_t **jinputs_ptr)
{
	if((NULL == p) || (NULL == jinputs_ptr))
	{
        return(eores_NOK_nullpointer);
	}
    
    if(jId >= eo_array_Capacity(p->jointsList))
    {
        return(eores_NOK_nodata);
    }
    *jinputs_ptr = s_eo_appTheDB_GetJointNVreference(p, jId, eOappTheDB_hid_jointNVindex_jinputs);
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetJointCmdControlmodePtr(EOappTheDB *p, eOmc_jointId_t jId,  eOmc_controlmode_t **jcmdcontrolmode_ptr)
{
	if((NULL == p) || (NULL == jcmdcontrolmode_ptr))
	{
        return(eores_NOK_nullpointer);
	}
    
    if(jId >= eo_array_Capacity(p->jointsList))
    {
        return(eores_NOK_nodata);
    }
    *jcmdcontrolmode_ptr = s_eo_appTheDB_GetJointNVreference(p, jId, eOappTheDB_hid_jointNVindex_jcmmnds__controlmode);
    
    return(eores_OK);
}



extern eOresult_t eo_appTheDB_GetShiftValuesOfJointPtr(EOappTheDB *p, eOmc_jointId_t jId, eOappTheDB_jointShiftValues_t **shiftval_ptr)
{
    eOappTheDB_hid_jointInfo_t 		*j_ptr;
    
    
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
    eOappTheDB_hid_jointInfo_t 		*j_ptr;
    

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



extern eOresult_t eo_appTheDB_GetMotorConfigPtr(EOappTheDB *p, eOmc_jointId_t mId,  eOmc_motor_config_t **mconfig_ptr)
{
	if((NULL == p) || (NULL == mconfig_ptr))
	{
        return(eores_NOK_nullpointer);
	}

    if(mId >= eo_array_Capacity(p->motorsList))
    {
        *mconfig_ptr = NULL;
        return(eores_NOK_nodata);
    }
    
    *mconfig_ptr = s_eo_appTheDB_GetMotorNVreference(p, mId, eOappTheDB_hid_motorNVindex_mconfig);
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetMotorStatusPtr(EOappTheDB *p, eOmc_motorId_t mId,  eOmc_motor_status_t **mstatus_ptr)
{
	if((NULL == p) || (NULL == mstatus_ptr))
	{
        return(eores_NOK_nullpointer);
	}
    
    if(mId >= eo_array_Capacity(p->motorsList))
    {
        return(eores_NOK_nodata);
    }
    *mstatus_ptr = s_eo_appTheDB_GetMotorNVreference(p, mId, eOappTheDB_hid_motorNVindex_mstaus);
    
    return(eores_OK);
}


extern eOresult_t eo_appTheDB_GetSnrMaisConfigPtr(EOappTheDB *p, eOsnsr_maisId_t sId,  eOsnsr_mais_config_t **sconfig_ptr)
{
	if((NULL == p) || (NULL == sconfig_ptr))
	{
        return(eores_NOK_nullpointer);
	}
    
    if(sId >= eo_array_Capacity(p->snsrMaisList))
    {
        return(eores_NOK_nodata);
    }
    *sconfig_ptr = s_eo_appTheDB_GetSnsrMaisNVreference(p, sId, eOappTheDB_hid_snsrMaisNVindex_mconfig);
    
    return(eores_OK);

}

extern eOresult_t eo_appTheDB_GetSnrMaisStatusPtr(EOappTheDB *p, eOsnsr_maisId_t sId,  eOsnsr_mais_status_t **sstatus_ptr)
{
	if((NULL == p) || (NULL == sstatus_ptr))
	{
        return(eores_NOK_nullpointer);
	}
    
    if(sId >= eo_array_Capacity(p->snsrMaisList))
    {
        return(eores_NOK_nodata);
    }
    *sstatus_ptr = s_eo_appTheDB_GetSnsrMaisNVreference(p, sId, eOappTheDB_hid_snsrMaisNVindex_mstatus);
    
    return(eores_OK);

}


extern eOresult_t eo_appTheDB_GetSnrStrainConfigPtr(EOappTheDB *p, eOsnsr_strainId_t sId,  eOsnsr_strain_config_t **sconfig_ptr)
{
	if((NULL == p) || (NULL == sconfig_ptr))
	{
        return(eores_NOK_nullpointer);
	}
    
    if(sId >= eo_array_Capacity(p->snsrStrainList))
    {
        return(eores_NOK_nodata);
    }
    *sconfig_ptr = NULL; s_eo_appTheDB_GetSnsrStrainNVreference(p, sId, eOappTheDB_hid_snsrStrainNVindex_sconfig);
    
    return(eores_OK);

}


extern eOresult_t eo_appTheDB_GetSnrStrainStatusPtr(EOappTheDB *p, eOsnsr_strainId_t sId,  eOsnsr_strain_status_t **sstatus_ptr)
{
	if((NULL == p) || (NULL == sstatus_ptr))
	{
        return(eores_NOK_nullpointer);
	}
    
    if(sId >= eo_array_Capacity(p->snsrStrainList))
    {
        return(eores_NOK_nodata);
    }
    *sstatus_ptr = NULL; s_eo_appTheDB_GetSnsrStrainNVreference(p, sId, eOappTheDB_hid_snsrStrainNVindex_sstatus);
    
    return(eores_OK);

}

extern eOresult_t eo_appTheDB_GetSkinCfgSigModePtr(EOappTheDB *p,eOsk_skinId_t skId,  eOskin_sigmode_t **sigmode_ptr)
{
	if((NULL == p) || (NULL == sigmode_ptr))
	{
        return(eores_NOK_nullpointer);
	}
    
    if(skId >= eo_array_Capacity(p->skinList))
    {
        return(eores_NOK_nodata);
    }
    *sigmode_ptr = s_eo_appTheDB_GetSkinNVreference(p, skId, eOappTheDB_hid_skinNVindex_sconfig__sigmode);
    
    return(eores_OK);

}
extern eOresult_t eo_appTheDB_GetSkinStArray10CanFramesPtr(EOappTheDB *p,eOsk_skinId_t skId,  EOarray_of_10canframes **arrayof10canframes_ptr)
{
	if((NULL == p) || (NULL == arrayof10canframes_ptr))
	{
        return(eores_NOK_nullpointer);
	}
    
    if(skId >= eo_array_Capacity(p->skinList))
    {
        return(eores_NOK_nodata);
    }
    *arrayof10canframes_ptr = s_eo_appTheDB_GetSkinNVreference(p, skId, eOappTheDB_hid_skinNVindex_sstaus__arrayof10canframe);
    
    return(eores_OK);

}


// extern eOresult_t eo_appTheDB_GetSkinConfigPtr(EOappTheDB *p,eOsk_skinId_t sId,  eOskin_config_t **skconfig_ptr)
// {
// 	if((NULL == p) || (NULL == skconfig_ptr))
// 	{
//         return(eores_NOK_nullpointer);
// 	}
//     
//     if(sId >= eo_array_Capacity(p->skinList))
//     {
//         return(eores_NOK_nodata);
//     }
//     *skconfig_ptr = NULL; //s_eo_appTheDB_GetSkinNVreference(p, mId, eOappTheDB_hid_skinNVindex_sconfig);
//     
//     return(eores_OK);

// }
// extern eOresult_t eo_appTheDB_GetSkinStatusPtr(EOappTheDB *p,eOsk_skinId_t sId,  eOskin_status_t **skstatus_ptr)
// {
// 	if((NULL == p) || (NULL == skstatus_ptr))
// 	{
//         return(eores_NOK_nullpointer);
// 	}
//     
//     if(sId >= eo_array_Capacity(p->skinList))
//     {
//         return(eores_NOK_nodata);
//     }
//     *skstatus_ptr = NULL; //s_eo_appTheDB_GetSkinNVreference(p, mId, eOappTheDB_hid_skinNVindex_sstatus);
//     
//     return(eores_OK);

// }


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
    if(eo_cfg_nvsEP_mc_joint_numbermax_Get((eOcfg_nvsEP_mc_endpoint_t)cfg->mc_endpoint) != cfg->jointsList->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "joints cfg mismach ");
        return(0);
    }

    if(eo_cfg_nvsEP_mc_motor_numbermax_Get((eOcfg_nvsEP_mc_endpoint_t)cfg->mc_endpoint) != cfg->motorsList->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "motors cfg mismach ");
        return(0);
    }

    if(eo_cfg_nvsEP_as_mais_numbermax_Get((eOcfg_nvsEP_as_endpoint_t)cfg->as_endpoint) != cfg->snsrMaisList->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "snr-mais cfg mismach ");
        return(0);
    }
    
    if(eo_cfg_nvsEP_as_strain_numbermax_Get((eOcfg_nvsEP_as_endpoint_t)cfg->as_endpoint) != cfg->snsrStrainList->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "snr-strain cfg mismach ");
        return(0);
    }
    if(eo_cfg_nvsEP_sk_sknumbermax_Get((eOcfg_nvsEP_sk_endpoint_t)cfg->sk_endpoint) != cfg->skinList->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "skin cfg mismach ");
        return(0);
    }
    
    return(1);
}


static eOresult_t s_appTheDB_canboardslist_init(EOappTheDB *p)
{
    eOsizecntnr_t 					i;
    eOappTheDB_cfg_canBoardInfo_t 	*b_cfg_ptr = NULL;	//pointer to configuration 
	eOappTheDB_hid_canBoardInfo_t 	*b_ptr = NULL;		//ponter to db memory
        
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
        }
//         else if((eobrd_mais == b_ptr->cfg_ptr->type) || (eobrd_skin == b_ptr->cfg_ptr->type) || (eobrd_strain == b_ptr->cfg_ptr->type))
//         {
//             b_ptr->s.connectedsensors =  eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
//                                                     sizeof(eOsnsr_sensorId_t), 1); //currently almost only one sensor per board
//         }
    }
    return(eores_OK);
}

static eOresult_t s_appTheDB_jointslist_init(EOappTheDB *p)
{
    eOsizecntnr_t                       i;
    eOappTheDB_cfg_jointInfo_t   		*j_cfg_ptr = NULL; 	//pointer to configuration
	eOappTheDB_hid_jointInfo_t			*j_ptr = NULL; 		//pointer to db memory
    eOappTheDB_hid_canBoardInfo_t       *b_ptr = NULL;
    eOappTheDB_jointShiftValues_t       *shiftvalues_ptr;
    eOicubCanProto_bcastpolicy_t        *bcastpolicy_ptr;    
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
        j_ptr->connectedEnc = (eOappEncReader_encoder_t)i; //currently the ith joint has connected to ith encoder
        j_ptr->ref2motor = (eOmc_motorId_t)i;			  //currently the ith joint has connected to ith motor
        j_ptr->shiftvalues_ptr = shiftvalues_ptr;
        j_ptr->bcastpolicy_ptr = bcastpolicy_ptr;
        
        //2.1)make a connection beetween board to joint also ==> fill "connected joints list" of baord
        b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, j_ptr->cfg_ptr->canLoc.belong2board);
        b_ptr->s.jm.connectedjoints[j_ptr->cfg_ptr->canLoc.indexinboard] = (eOmc_jointId_t)i;
    }
    return(eores_OK);
}


static eOresult_t s_appTheDB_motorslist_init(EOappTheDB *p)
{
    eOsizecntnr_t                       i;
    eOappTheDB_cfg_motorInfo_t			*m_cfg_ptr = NULL; 	//pointer to configuration
	eOappTheDB_hid_motorInfo_t		    *m_ptr = NULL; 		//pointer to db memory
    eOappTheDB_hid_canBoardInfo_t       *b_ptr = NULL;

        
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
        m_ptr->ref2joint = (eOmc_jointId_t)i;
        
        //2.1)make a connection beetween board to joint also ==> fill "connected joints list" of board
		b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, m_ptr->cfg_ptr->canLoc.belong2board);
        b_ptr->s.jm.connectedmotors[m_ptr->cfg_ptr->canLoc.indexinboard] = (eOmc_motorId_t)i; //set motor's id
    }
    
    return(eores_OK);
}


static eOresult_t s_appTheDB_snsrMaislist_init(EOappTheDB *p)
{
    eOsizecntnr_t                       i;
    eOappTheDB_cfg_snsrMaisInfo_t        *s_cfg_ptr = NULL; 	//pointer to configuration
	eOappTheDB_hid_snsrMaisInfo_t		*s_ptr = NULL;		//pointer to db memory

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
    eOappTheDB_cfg_snsrStrainInfo_t      *s_cfg_ptr = NULL; 	//pointer to configuration
	eOappTheDB_hid_snsrStrainInfo_t		*s_ptr = NULL;		//pointer to db memory

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
	eOappTheDB_hid_skinInfo_t			*sk_ptr = NULL;

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
    eOsizecntnr_t 		     		 	i;
// 	eObrd_boardId_t						maxusedcanaddr = 0;
    eOicubCanProto_canBoardAddress_t 	maxusedcanaddr = 0, addr;

    //1) get max used can address (it will be in range [1,E]. 0 is ems can port address and F is bradcast address)
	eOappTheDB_cfg_canBoardInfo_t *b_cfg_ptr = (eOappTheDB_cfg_canBoardInfo_t*)(p->cfg.canboardsList->item_array_data);
	
	for(i = 0; i< p->cfg.canboardsList->size; i++)
	{
        //select only the addresses that are:
        //  - connected 2 ems port used to commnd joint (joints and motors are connected to a different can bus of sensors) AND
        //  - belong to motor baord (1 foc and mc4)
		if( (db_emscanportconnected2motorboard == b_cfg_ptr[i].canLoc.emscanport) &&
            (eobrd_1foc != b_cfg_ptr[i].type) && (eobrd_mc4 != b_cfg_ptr[i].type) &&
            (maxusedcanaddr < b_cfg_ptr[i].canLoc.addr))
		{
			maxusedcanaddr = b_cfg_ptr[i].canLoc.addr;
		}   
	}

    // 2) create the lookup tbl
	p->canaddressLookuptbl.capacity = maxusedcanaddr+1; //cosi' lascio l'emento 0-esimo, anche se non lo uso.    
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


static eOresult_t s_appTheDB_nvsrefmaps_init(EOappTheDB *p)
{
    eOresult_t res;
    uint16_t size, i;

//joint
    size = eo_array_Capacity(p->jointsList);
    p->nvsrefMaps.jnvMap = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(void**), size);
      
    for(i=0; i<size; i++)
    {
        p->nvsrefMaps.jnvMap[i] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                sizeof(void*), eOappTheDB_hid_jointNVindex_TOTALnumber); 
        
        res = s_appTheDB_nvsrefmaps_jmap_fill(p, (eOmc_jointId_t)i);
        if(eores_OK  != res)
        {
            return(res);
        }
    }
    
//motor
    size = eo_array_Capacity(p->motorsList);
    p->nvsrefMaps.mnvMap = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(void**), size);
      
    for(i=0; i<size; i++)
    {
        p->nvsrefMaps.mnvMap[i] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                sizeof(void*), eOappTheDB_hid_motorNVindex_TOTALnumber);

        res = s_appTheDB_nvsrefmaps_mmap_fill(p, (eOmc_motorId_t)i);
        if(eores_OK  != res)
        {
            return(res);
        }

    }
    
    
//mais
    size = eo_array_Capacity(p->snsrMaisList);
    p->nvsrefMaps.mnvMap = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(void**), size);
      
    for(i=0; i<size; i++)
    {
        p->nvsrefMaps.snsrMaisnvMap[i] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                sizeof(void*), eOappTheDB_hid_snsrMaisNVindex_TOTALnumber);

        res = s_appTheDB_nvsrefmaps_snsrMaismap_fill(p, (eOsnsr_maisId_t)i);
        if(eores_OK  != res)
        {
            return(res);
        }

    }


//strain
    size = eo_array_Capacity(p->snsrStrainList);
    p->nvsrefMaps.mnvMap = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(void**), size);
      
    for(i=0; i<size; i++)
    {
        p->nvsrefMaps.snsrStrainnvMap[i] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                sizeof(void*), eOappTheDB_hid_snsrStrainNVindex_TOTALnumber);

        res = s_appTheDB_nvsrefmaps_snsrStrainmap_fill(p, (eOsnsr_strainId_t)i);
        if(eores_OK  != res)
        {
            return(res);
        }

    }
    
//skin
    size = eo_array_Capacity(p->skinList);
    p->nvsrefMaps.mnvMap = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(void**), size);
      
    for(i=0; i<size; i++)
    {
        p->nvsrefMaps.skinnvMap[i] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                sizeof(void*), eOappTheDB_hid_skinNVindex_TOTALnumber);

        res = s_appTheDB_nvsrefmaps_skinmap_fill(p, (eOsk_skinId_t)i);
        if(eores_OK  != res)
        {
            return(res);
        }

    }

    return(eores_OK);
}


static eOresult_t s_appTheDB_nvsrefmaps_jmap_fill(EOappTheDB *p, eOmc_jointId_t jId)
{
    uint16_t                        ipindex, epindex, idindex;
    EOnv                            nv;
    EOnv                            *nv_res_ptr;
    eOnvID_t                        nv_id;
    eOresult_t                      res;
    eOappTheDB_hid_jointNVindex_t   jnvindex;

    for(jnvindex=eOappTheDB_hid_jointNVindex_jconfig /*=0*/; jnvindex<eOappTheDB_hid_jointNVindex_TOTALnumber; jnvindex++)
    {
        nv_id = eo_cfg_nvsEP_mc_joint_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)p->cfg.mc_endpoint, 
                                               (eOcfg_nvsEP_mc_jointNumber_t)jId, 
                                               eo_appTheDB_hid_GetMCjointNVindex(jnvindex));
            
        res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.mc_endpoint, nv_id, 
                                   &ipindex, &epindex, &idindex); 
        if(eores_OK != res)
        {
            return(res);
        }
        nv_res_ptr = eo_nvscfg_GetNV(p->cfg.nvsCfg, ipindex, epindex, idindex, NULL, &nv);
        if(NULL == nv_res_ptr)
        {
            return(eores_NOK_nullpointer);    
        }
        p->nvsrefMaps.jnvMap[jId][jnvindex] = nv.loc;  

    }
    return(eores_OK);
}



static eOresult_t s_appTheDB_nvsrefmaps_mmap_fill(EOappTheDB *p, eOmc_motorId_t mId)
{
    uint16_t                        ipindex, epindex, idindex;
    EOnv                            nv;
    EOnv                            *nv_res_ptr;
    eOnvID_t                        nv_id;
    eOresult_t                      res;
    eOappTheDB_hid_motorNVindex_t   mnvindex;

    for(mnvindex=eOappTheDB_hid_motorNVindex_mconfig /*=0*/; mnvindex<eOappTheDB_hid_motorNVindex_TOTALnumber; mnvindex++)
    {
        nv_id = eo_cfg_nvsEP_mc_motor_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)p->cfg.mc_endpoint, 
                                               (eOcfg_nvsEP_mc_motorNumber_t)mId, 
                                               eo_appTheDB_hid_GetMCmotorNVindex(mnvindex));
            
        res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.mc_endpoint, nv_id, 
                                   &ipindex, &epindex, &idindex); 
        if(eores_OK != res)
        {
            return(res);
        }
        nv_res_ptr = eo_nvscfg_GetNV(p->cfg.nvsCfg, ipindex, epindex, idindex, NULL, &nv);
        if(NULL == nv_res_ptr)
        {
            return(eores_NOK_nullpointer);    
        }
        p->nvsrefMaps.mnvMap[mId][mnvindex] = nv.loc;  

    }
    return(eores_OK);
}



static eOresult_t s_appTheDB_nvsrefmaps_snsrMaismap_fill(EOappTheDB *p, eOsnsr_maisId_t sId)
{
    uint16_t                            ipindex, epindex, idindex;
    EOnv                                nv;
    EOnv                                *nv_res_ptr;
    eOnvID_t                            nv_id;
    eOresult_t                          res;
    eOappTheDB_hid_snsrMaisNVindex_t   snvindex;

    for(snvindex=eOappTheDB_hid_snsrMaisNVindex_mconfig /*=0*/; snvindex<eOappTheDB_hid_snsrMaisNVindex_TOTALnumber; snvindex++)
    {
        nv_id = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)p->cfg.as_endpoint, 
                                               (eOcfg_nvsEP_as_maisNumber_t)sId, 
                                               eo_appTheDB_hid_GetASmaisNVindex(snvindex));
            
        res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.mc_endpoint, nv_id, 
                                   &ipindex, &epindex, &idindex); 
        if(eores_OK != res)
        {
            return(res);
        }
        nv_res_ptr = eo_nvscfg_GetNV(p->cfg.nvsCfg, ipindex, epindex, idindex, NULL, &nv);
        if(NULL == nv_res_ptr)
        {
            return(eores_NOK_nullpointer);    
        }
        p->nvsrefMaps.snsrMaisnvMap[sId][snvindex] = nv.loc;  

    }
    return(eores_OK);
}

static eOresult_t s_appTheDB_nvsrefmaps_snsrStrainmap_fill(EOappTheDB *p, eOsnsr_strainId_t sId)
{
    uint16_t                            ipindex, epindex, idindex;
    EOnv                                nv;
    EOnv                                *nv_res_ptr;
    eOnvID_t                            nv_id;
    eOresult_t                          res;
    eOappTheDB_hid_snsrStrainNVindex_t   snvindex;

    for(snvindex=eOappTheDB_hid_snsrStrainNVindex_sconfig /*=0*/; snvindex<eOappTheDB_hid_snsrStrainNVindex_TOTALnumber; snvindex++)
    {
        nv_id = eo_cfg_nvsEP_as_strain_NVID_Get((eOcfg_nvsEP_as_endpoint_t)p->cfg.as_endpoint, 
                                               (eOcfg_nvsEP_as_strainNumber_t)sId, 
                                               eo_appTheDB_hid_GetASstrainNVindex(snvindex));
            
        res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.mc_endpoint, nv_id, 
                                   &ipindex, &epindex, &idindex); 
        if(eores_OK != res)
        {
            return(res);
        }
        nv_res_ptr = eo_nvscfg_GetNV(p->cfg.nvsCfg, ipindex, epindex, idindex, NULL, &nv);
        if(NULL == nv_res_ptr)
        {
            return(eores_NOK_nullpointer);    
        }
        p->nvsrefMaps.snsrStrainnvMap[sId][snvindex] = nv.loc;  

    }
    return(eores_OK);
}


static eOresult_t s_appTheDB_nvsrefmaps_skinmap_fill(EOappTheDB *p, eOsk_skinId_t skId)
{
    uint16_t                            ipindex, epindex, idindex;
    EOnv                                nv;
    EOnv                                *nv_res_ptr;
    eOnvID_t                            nv_id;
    eOresult_t                          res;
    eOappTheDB_hid_skinNVindex_t        sknvindex;

    for(sknvindex=eOappTheDB_hid_skinNVindex_sconfig__sigmode /*=0*/; sknvindex<eOappTheDB_hid_skinNVindex_TOTALnumber; sknvindex++)
    {
        nv_id = eo_cfg_nvsEP_sk_NVID_Get((eOcfg_nvsEP_sk_endpoint_t)p->cfg.sk_endpoint, 
                                         (eOcfg_nvsEP_sk_skinNumber_t)skId, 
                                          eo_appTheDB_hid_GetSKskinNVindex(sknvindex));
            
        res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.mc_endpoint, nv_id, 
                                   &ipindex, &epindex, &idindex); 
        if(eores_OK != res)
        {
            return(res);
        }
        nv_res_ptr = eo_nvscfg_GetNV(p->cfg.nvsCfg, ipindex, epindex, idindex, NULL, &nv);
        if(NULL == nv_res_ptr)
        {
            return(eores_NOK_nullpointer);    
        }
        p->nvsrefMaps.skinnvMap[skId][sknvindex] = nv.loc;  

    }
    return(eores_OK);
}

static void* s_eo_appTheDB_GetJointNVreference(EOappTheDB *p, eOmc_jointId_t jId, eOappTheDB_hid_jointNVindex_t nvindex)
{
	return(p->nvsrefMaps.jnvMap[jId][nvindex]);
}

static void* s_eo_appTheDB_GetMotorNVreference(EOappTheDB *p, eOmc_motorId_t mId, eOappTheDB_hid_motorNVindex_t nvindex)
{
	return(p->nvsrefMaps.mnvMap[mId][nvindex]);
}

static void* s_eo_appTheDB_GetSnsrMaisNVreference(EOappTheDB *p, eOsnsr_maisId_t sId, eOappTheDB_hid_snsrMaisNVindex_t nvindex)
{
	return(p->nvsrefMaps.snsrMaisnvMap[sId][nvindex]);
}

static void* s_eo_appTheDB_GetSnsrStrainNVreference(EOappTheDB *p, eOsnsr_strainId_t sId, eOappTheDB_hid_snsrStrainNVindex_t nvindex)
{
	return(p->nvsrefMaps.snsrStrainnvMap[sId][nvindex]);
}


static void* s_eo_appTheDB_GetSkinNVreference(EOappTheDB *p, eOsk_skinId_t skId, eOappTheDB_hid_skinNVindex_t nvindex)
{
	return(p->nvsrefMaps.skinnvMap[skId][nvindex]);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



