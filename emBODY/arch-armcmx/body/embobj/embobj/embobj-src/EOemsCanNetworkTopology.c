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


extern eOresult_t eo_emsCanNetTopo_GetJointCanLocation_ByJointUniqueId(EOemsCanNetTopo *p, eOmc_jointUniqueId_t jUniqueId, 
                                                                        eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eObrd_types_t *boardType)
{
    if((NULL == p) || (NULL == location_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if( NULL == p->joint_Id2CanLoc_hTbl[jUniqueId].ptr)
    {
        return(eores_NOK_nodata);
    }

    location_ptr->canaddr = p->joint_Id2CanLoc_hTbl[jUniqueId].ptr->boardAddr;
    location_ptr->axis = p->joint_Id2CanLoc_hTbl[jUniqueId].ptr->axis;
    location_ptr->emscanport = p->joint_Id2CanLoc_hTbl[jUniqueId].ptr->canPort;

    if(NULL !=boardType)
    {
        *boardType = p->joint_Id2CanLoc_hTbl[jUniqueId].ptr->boardType;
    }
    return(eores_OK);
}

extern eOresult_t eo_emsCanNetTopo_GetMotorCanLocation_ByMotorUniqueId(EOemsCanNetTopo *p, eOmc_motorUniqueId_t mUniqueId, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eObrd_types_t *boardType)
{
    if((NULL == p) || (NULL == location_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    if( NULL == p->motor_Id2CanLoc_hTbl[mUniqueId].ptr)
    {
        return(eores_NOK_nodata);
    }

    location_ptr->canaddr = p->motor_Id2CanLoc_hTbl[mUniqueId].ptr->boardAddr;
    location_ptr->axis = p->motor_Id2CanLoc_hTbl[mUniqueId].ptr->axis;
    location_ptr->emscanport = p->motor_Id2CanLoc_hTbl[mUniqueId].ptr->canPort;

    if(NULL !=boardType)
    {
        *boardType = p->motor_Id2CanLoc_hTbl[mUniqueId].ptr->boardType;
    }

    return(eores_OK);
}


extern eOresult_t eo_emsCanNetTopo_GetJointUinqueId_ByJointCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eOmc_jointUniqueId_t *jUniqueId_ptr)
{

    eo_emsCanNetTopo_hashTbl_item_t *item_ptr;
    if((NULL == p) || (NULL == location_ptr) || (NULL == jUniqueId_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    item_ptr = &(p->joint_CanLoc2Id_hTbl[location_ptr->emscanport][location_ptr->canaddr][location_ptr->axis]); 
    if( NULL == item_ptr->ptr)
    {
        return(eores_NOK_nodata);
    }

    *jUniqueId_ptr = item_ptr->id;
    return(eores_OK);
}

extern eOresult_t eo_emsCanNetTopo_GetMotorUinqueId_ByMotorCanLocation(EOemsCanNetTopo *p, eo_emsCanNetTopo_jointOrMotorCanLocation_t *location_ptr, eOmc_motorUniqueId_t *mUniqueId_ptr)
{
    eo_emsCanNetTopo_hashTbl_item_t *item_ptr;
    if((NULL == p) || (NULL == location_ptr) || (NULL == mUniqueId_ptr))
    {
        return(eores_NOK_nullpointer);
    }
    
    item_ptr = &(p->motor_CanLoc2Id_hTbl[location_ptr->emscanport][location_ptr->canaddr][location_ptr->axis]); 
    if( NULL == item_ptr->ptr)
    {
        return(eores_NOK_nodata);
    }

    *mUniqueId_ptr = item_ptr->id;
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
        p->joint_Id2CanLoc_hTbl[j_topoinfo_ptr[i].id].id  = i;   
    
        p->joint_CanLoc2Id_hTbl[j_topoinfo_ptr[i].canPort][j_topoinfo_ptr[i].boardAddr][j_topoinfo_ptr[i].axis].id = i; 
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
        p->motor_Id2CanLoc_hTbl[m_topoinfo_ptr[i].id].id  = i;   
    
        p->motor_CanLoc2Id_hTbl[m_topoinfo_ptr[i].canPort][m_topoinfo_ptr[i].boardAddr][m_topoinfo_ptr[i].axis].id = i; 
        p->motor_CanLoc2Id_hTbl[m_topoinfo_ptr[i].canPort][m_topoinfo_ptr[i].boardAddr][m_topoinfo_ptr[i].axis].ptr = &m_topoinfo_ptr[i]; 
    }

}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

