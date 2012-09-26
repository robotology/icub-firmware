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
#include "eOcfg_nvsEP_mc.h"
#include "eOcfg_nvsEP_as.h"
#include "eOcfg_nvsEP_sk.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOappDataBase.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOappDataBase_hid.h"


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
eObool_t s_appTheDB_checkConfiguaration(const eOappTheDB_cfg_t *cfg);
eOresult_t s_appTheDB_canboardslist_init(EOappTheDB *p);
eOresult_t s_appTheDB_jointslist_init(EOappTheDB *p);
eOresult_t s_appTheDB_motorslist_init(EOappTheDB *p);
eOresult_t s_appTheDB_snrMaislist_init(EOappTheDB *p);
eOresult_t s_appTheDB_snrStrainlist_init(EOappTheDB *p);
eOresult_t s_appTheDB_skinlist_init(EOappTheDB *p);
eOresult_t s_appTheDB_canaddressLookuptbl_init(EOappTheDB *p);
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
EOappTheDB s_db = 
{
    .EO_INIT(.isinitted)    eobool_false
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOappTheDB* eo_appTheDB_Initialise(const eOappTheDB_cfg_t *cfg)
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
    
    res = s_appTheDB_canaddressLookuptbl_init(p);
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

    res = s_appTheDB_snrMaislist_init(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }

    res = s_appTheDB_snrStrainlist_init(retptr);
    if(eores_OK != res)
    {
        return(NULL);
    }
    
    res = s_appTheDB_skinlist_init(retptr);
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

extern eOresult_t eo_appTheDB_GetJointCanLocation(EOappTheDB *p, eOmc_jointId_t jId,  eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr)
{
    eOappTheDB_hid_canBoardInfo		*b_ptr;
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
    
    canloc_ptr->indexinboard = j_ptr->cfg->indexinboard;

    b_ptr = (eOappTheDB_hid_canBoardInfo *)eo_array_At(p->canboardsList, j_ptr->cfg->belong2board);
	
    canloc_ptr->addr = b_ptr->cfg->addr;
    canloc_ptr->emscanport = b_ptr->cfg->emscanport;
    
    return(eores_OK);
}

extern eOresult_t eo_appTheDB_GetMotorCanLocation(EOappTheDB *p, eOmc_motorId_t mId,  eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr)
{
    eOappTheDB_hid_canBoardInfo		*b_ptr;
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
    
    canloc_ptr->indexinboard = m_ptr->cfg->indexinboard;

    b_ptr = (eOappTheDB_hid_canBoardInfo *)eo_array_At(p->canboardsList, m_ptr->cfg->belong2board);
	
    canloc_ptr->addr = b_ptr->cfg->addr;
    canloc_ptr->emscanport = b_ptr->cfg->emscanport;
    
    return(eores_OK);

}


extern eOresult_t eo_appTheDB_GetMotorId_ByMotorCanLocation(EOappTheDB *p, eOappTheDB_jointOrMotorCanLocation_t *canloc_ptr, eOmc_motorId_t *mId_ptr)
{
    eOappTheDB_hid_canBoardInfo *b_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr) || (NULL == mId_ptr))
    {
        return(eores_NOK_nullpointer);
    }

    if((db_emscanportconnected2motorboard != canloc_ptr->emscanport) || (canloc_ptr->addr >= p->canaddressLookuptbl.capacity))
    {
        return(eores_NOK_general);
    }

	b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, eo_appTheDB_hid_GetBoardIdWithAddress(p, canloc_ptr->addr));
    *mId_ptr = b_ptr->u.jm.connectedmotors[canloc_ptr->indexinboard];

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
        return(eores_NOK_general);
    }

	b_ptr = (eOappTheDB_hid_canBoardInfo_t *)eo_array_At(p->canboardsList, eo_appTheDB_hid_GetBoardIdWithAddress(p, canloc_ptr->addr));
    *jId_ptr = b_ptr->u.jm.connectedjoints[canloc_ptr->indexinboard];

    return(eores_OK);
}



extern eOresult_t eo_appTheDB_GetSnrMaisCanLocation(EOappTheDB *p, eOmc_sensorId_t sId, eOappTheDB_sensorCanLocation_t *canloc_ptr)
{
    eOappTheDB_hid_snrMaisInfo_t		*s_ptr;
	eOappTheDB_hid_canBoardInfo_t		*b_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    s_ptr = (eOappTheDB_hid_snrMaisInfo_t *)eo_array_At(p->snrMaisList, sId);
    if(NULL == m_ptr)
    {
        return(eores_NOK_nodata);
    }

    b_ptr = (eOappTheDB_hid_canBoardInfo *)eo_array_At(p->canboardsList, s_ptr->cfg->belong2board);
	
    canloc_ptr->addr = b_ptr->cfg->addr;
    canloc_ptr->emscanport = b_ptr->cfg->emscanport;
    
    return(eores_OK);
}

extern eOresult_t eo_appTheDB_GetSnrStrainCanLocation(EOappTheDB *p, eOmc_sensorId_t sId, eOappTheDB_sensorCanLocation_t *canloc_ptr)
{
    eOappTheDB_hid_snrStrainInfo_t		*s_ptr;
	eOappTheDB_hid_canBoardInfo_t		*b_ptr;
    
    if((NULL == p) || (NULL == canloc_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    s_ptr = (eOappTheDB_hid_snrStrainInfo_t *)eo_array_At(p->snrStrainList, sId);
    if(NULL == m_ptr)
    {
        return(eores_NOK_nodata);
    }

    b_ptr = (eOappTheDB_hid_canBoardInfo *)eo_array_At(p->canboardsList, s_ptr->cfg->belong2board);
	
    canloc_ptr->addr = b_ptr->cfg->addr;
    canloc_ptr->emscanport = b_ptr->cfg->emscanport;
    
    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
eObool_t s_appTheDB_checkConfiguaration(const eOappTheDB_cfg_t *cfg)
{
    uint8_t asEP_numofsensors;
    
    if((NULL == p->cfg.jointsList) || (NULL == p->cfg.motorsList) || (NULL == p->cfg.snrMaisList) ||
       (NULL == p->cfg.snrStrainList) || (NULL == p->cfg.skinList))
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

    if(eo_cfg_nvsEP_as_mais_numbermax_Get((eOcfg_nvsEP_as_endpoint_t)cfg->as_endpoint) != cfg->snrMais->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "snr-mais cfg mismach ");
        return(0);
    }
    
    if(eo_cfg_nvsEP_as_strain_numbermax_Get((eOcfg_nvsEP_as_endpoint_t)cfg->as_endpoint) != cfg->snrStrain->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "snr-strain cfg mismach ");
        return(0);
    }
    
    if(eo_cfg_nvsEP_mc_skin_numbermax_Get((eOcfg_nvsEP_mc_endpoint_t)cfg->mc_endpoint) != cfg->skinList->size)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_warning, s_eobj_ownname, "skin cfg mismach ");
        return(0);
    }
    
    return(1);
}


eOresult_t s_appTheDB_canboardslist_init(EOappTheDB *p)
{
    eOsizecntnr_t 					i;
    eOappTheDB_cfg_canBoardInfo_t 	*b_cfg_ptr = NULL;	//pointer to configuration 
	eOappTheDB_hid_canBoardInfo_t 	*b_ptr = NULL;		//ponter to db memory
        
    //1) create canboardsList
    p->canboardsList = eo_array_New(p->cfg.canboardsList->size, sizeof(eOappTheDB_hid_canboardInfo_t), NULL);
	if(NULL == p->canboardsList)
	{
		return(eores_NOK_generic);
	}
    
    b_cfg_ptr = (eOappTheDB_cfg_canBoardInfo_t*)(p->cfg.canboardssList->item_array_data);
	
    for(i = 0; i< p->cfg.canboardtsList->size; i++)
    {
		b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, i);
        
		//1.1) save pointer to board cfg info
        b_ptr->cfg_ptr = &b_cfg_ptr[i];
        
        //create array of "connected stuff" (joints or sensors)
        if((eobrd_mc4 == b_ptr->cfg_ptr->type) || (eobrd_1foc == b_ptr->cfg_ptr->type))
        {
			b_ptr->u.jm.connectedjoints =  eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                    sizeof(eOmc_jointId_t), eOicubCanProto_jm_indexInBoard_max);
			b_ptr->u.jm.connectedmotors =  eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                    sizeof(eOmc_motorId_t), eOicubCanProto_jm_indexInBoard_max);
        }
        else if((eobrd_mais == b_ptr->cfg_ptr->type) || (eobrd_skin == b_ptr->cfg_ptr->type) || (eobrd_strain == b_ptr->cfg_ptr->type))
        {
            b_ptr->u.connectedsensors =  eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                    sizeof(eOsnsr_sensorId_t), 1); //currently almost only one sensor per board
        }
    }
    return(eores_OK);
}

eOresult_t s_appTheDB_jointslist_init(EOappTheDB *p)
{
    eOsizecntnr_t                       i;
    eOappTheDB_cfg_jointInfo_t   		*j_cfg_ptr = NULL; 	//pointer to configuration
	eOappTheDB_hid_jointInfo_t			*j_ptr = NULL; 		//pointer to db memory
    eOappTheDB_hid_canBoardInfo_t       *b_ptr = NULL;

    //1) create jointList
    p->jointsList = eo_array_New(p->cfg->jointsList->size, sizeof(eOappTheDB_hid_jointInfo_t), NULL);
	if(NULL == p->jointsList)
	{
		retunr(eores_NOK_generic);
	}
	
    //2) fill jointsList
    j_cfg_ptr = (eOappTheDB_cfg_jointInfo_t*)(p->cfg.jointsList->item_array_data);

    for(i = 0; i< p->cfg.jointsList->size; i++)
    {
		j_ptr = (eOappTheDB_hid_jointInfo_t*)eo_array_At(p->jointsList, i);
        j_cfg_ptr->cfg_ptr = &j_cfg_ptr[i];
        j_cfg_ptr->connectedEnc = (eOappEncReader_encoder_t)i; //currently the ith joint has connected to ith encoder
        j_cfg_ptr->ref2motor = (eOmc_motorId_t)i;			  //currently the ith joint has connected to ith motor
        
        //2.1)make a connection beetween board to joint also ==> fill "connected joints list" of baord
        b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, j_cfg_ptr->cfg_ptr->belong2board);
        b_ptr->u.jm.connectedjoints[j_cfg_ptr->cfg_ptr->indexinboard] = (eOmc_jointId_t)i;
    }
    
    return(eores_OK);
}


eOresult_t s_appTheDB_motorslist_init(EOappTheDB *p)
{
    int8_t                              i;
    eOappTheDB_cfg_motorInfo_t			*m_cfg_ptr = NULL; 	//pointer to configuration
	eOappTheDB_hid_motorInfo_t		    *m_ptr = NULL; 		//pointer to db memory
    eOappTheDB_hid_canBoardInfo         *b_ptr = NULL;

        
    //1) create jointList
    p->motorsList = eo_array_New(p->cfg->motorsList->size, sizeof(eOappTheDB_hid_motorInfo_t), NULL);
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
        b_ptr = &p->canboardsList[p->motorsList[i].cfg_ptr->belong2board]; //get pointer to motors's board 
		b_ptr = (eOappTheDB_hid_canBoardInfo_t*)eo_array_At(p->canboardsList, m_cfg_ptr->cfg_ptr->belong2board);
        b_ptr->u.jm.connectedmotors[m_ptr->cfg_ptr->indexinboard] = (eOmc_motorId_t)i; //set motor's id
    }
    
    return(eores_OK);
}


eOresult_t s_appTheDB_snrMaislist_init(EOappTheDB *p)
{
    int8_t                              i;
    eOappTheDB_cfg_snrMaisInfo_t      *s_cfg_ptr = NULL; 	//pointer to configuration
	eOappTheDB_hid_snrMaisInfo_t		*s_ptr = NULL;		//pointer to db memory
    eOappTheDB_hid_canBoardInfo         *b_ptr = NULL;

    //if no sensor is connected to board ==> nothing to do and return ok
    if(0 == p->cfg->snrMaisList->size)
    {
        p->snrMaisList = NULL;
        return(eores_OK);
    }
    
    //1) create sensors mais List
    p->snrMaisList = eo_array_New(p->cfg->snrMaisList->size, sizeof(eOappTheDB_hid_snrMaisInfo_t), NULL);
	if(NULL == p->snrMaisList)
	{
		return(eores_NOK_generic);
	}
	
	
	//2) fill sensors mais List
    s_cfg_ptr = (eOappTheDB_cfg_snrMaisInfo_t*)(p->cfg.snrMaisList->item_array_data);

    for(i = 0; i< p->generaldata.numberofconnectedsnrmais; i++)
    {
		s_ptr = (eOappTheDB_hid_snrMaisInfo_t*)eo_array_At(p->snrMaisList, i);
        s_ptr->cfg_ptr = &s_cfg_ptr[i];
		
        /* Since almost only one sensor is connected to ems, i don't use a table to get sensor id(it will always 0) */
    }
    
    return(eores_OK);
}


eOresult_t s_appTheDB_snrStrainlist_init(EOappTheDB *p)
{
    int8_t                              i;
    eOappTheDB_cfg_snrStrainInfo_t      *s_cfg_ptr = NULL; 	//pointer to configuration
	eOappTheDB_hid_snrMaisInfo_t		*s_ptr = NULL;		//pointer to db memory
    eOappTheDB_hid_canBoardInfo         *b_ptr = NULL;

    //if no sensor is connected to board ==> nothing to do and return ok
    if(0 == p->cfg->snrStrainList->size)
    {
        p->snrStrainList = NULL;
        return(eores_OK);
    }
    
    //1) create sensors mais List
    p->snrStrainList = eo_array_New(p->cfg->snrStrainList->size, sizeof(eOappTheDB_hid_snrStrainInfo_t), NULL);
	if(NULL == p->snrStrainList)
	{
		return(eores_NOK_generic);
	}
	
	
	//2) fill sensors mais List
    s_cfg_ptr = (eOappTheDB_cfg_snrStrainInfo_t*)(p->cfg.snrMaisList->item_array_data);

    for(i = 0; i< p->generaldata.numberofconnectedsnrmais; i++)
    {
		s_ptr = (eOappTheDB_hid_snrMaisInfo_t*)eo_array_At(p->snrMaisList, i);
        s_ptr->cfg_ptr = &s_cfg_ptr[i];
		
        /* Since almost only one sensor is connected to ems, i don't use a table to get sensor id(it will always 0) */
    }
    
    return(eores_OK);
}


eOresult_t s_appTheDB_skinlist_init(EOappTheDB *p)
{
    int8_t                              i;
    eOappTheDB_cfg_skinInfo_t           *sk_cfg_ptr = NULL;
	eOappTheDB_hid_skinInfo_t			*sk_ptr = NULL;
    eOappTheDB_hid_canBoardInfo         b_ptr = NULL;

    //if no sensor is connected to board ==> nothing to do and return ok
    if(0 == p->cfg->skinList->size)
    {
        p->skinList = NULL;
        return(eores_OK);
    }
    
    //1) create sensorsList
    p->skinList = eo_array_New(p->cfg->skinList->size, sizeof(eOappTheDB_hid_skinInfo_t), NULL);
	if(NULL == p->skinList)
	{
		return(eores_NOK_generic);
	}


    //2) fill sensorsList
    sk_cfg_ptr = (eOappTheDB_cfg_skinInfo_t*)(p->cfg.skinList->item_array_data);

    for(i = 0; i< p->cfg->skinList->size; i++)
    {
		sk_ptr = (eOappTheDB_hid_skinInfo_t*)eo_array_At(p->skinList, i);
        sk_ptr->cfg_ptr = &sk_cfg_ptr[i];
        /* Since skin is seen like a unique board, connected to a specific ems port 
            i don't need a method to get skin id by can location */
    }
    
    return(eores_OK);
}


eOresult_t s_appTheDB_canaddressLookuptbl_init(EOappTheDB *p)
{
    eOsizecntnr_t 		     		 	i;
	eObrd_boardId_t						maxusedcanaddr = 0;
    eOicubCanProto_canBoardAddress_t 	addr;
	
	eOappTheDB_cfg_canBoardInfo_t *b_cfg_ptr = (eOappTheDB_cfg_canBoardInfo_t*)(p->cfg.canboardssList->item_array_data);
	
	for(i = 0; i< p->cfg.canboardtsList->size; i++)
	{
		if((db_emscanportconnected2motorboard == p->b_cfg_ptr->canLoc.emscanport) &&(maxusedcanaddr < p->b_cfg_ptr->canLoc.addr))
		{
			maxusedcanaddr = b_cfg_ptr->canLoc.addr;
		}   
	}

	p->canaddressLookuptbl.capacity = maxusedcanaddr+1; //cosi' lascio l'emento 0-esimo, anche se non lo uso.    
    p->canaddressLookuptbl.tbl = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eObrd_boardId_t), 
													  p->canaddressLookuptbl.capacity);
    
    for(i=0; i<p->cfg.canboardsList->size; i++)
    {
        if((eobrd_1foc != p->canboardsList[i].cfg_ptr->type) && (eobrd_mc4 != p->canboardsList[i].cfg_ptr->type))
        {
            continue;
        }        
        addr = p->canboardsList[i].cfg_ptr->canLoc.addr;
        p->canaddressLookuptbl.tbl[addr] = (eObrd_boardId_t)i;
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



