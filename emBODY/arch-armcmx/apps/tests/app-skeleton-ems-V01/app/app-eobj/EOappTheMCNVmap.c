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



/* @file       EOappTheMCNVmap.c
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       04/17/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"
#include "EOnv.h"
#include "EOnv_hid.h"

#include "eOcfg_nvsEP_mc_any_con_bodypart.h"
          

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOappTheMCNVmap.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOappTheMCNVmap_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


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
static void s_eo_appTheMCNVmap_tables_create(eOappTheMCNVmap_cfg_t *cfg);
static void s_eo_appTheMCNVmap_tables_initialise(void);
static void s_eo_appTheMCNVmap_table_motors_initialise(void);
static void s_eo_appTheMCNVmap_table_joints_initialise(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


//static const char s_eobj_ownname[] = "EOappTheMCNVmap";

static EOappTheMCNVmap s_eo_appTheMCNVmap;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOappTheMCNVmap* eo_appTheMCNVmap_Initialise(eOappTheMCNVmap_cfg_t *cfg)
{
    EOappTheMCNVmap* retptr = NULL;
    if(NULL == cfg)
    {
        return(retptr);
    }
    
    retptr = &s_eo_appTheMCNVmap;
    retptr->nvsCfg = cfg->nvsCfg;
    retptr->endpoint = cfg->endpoint;
    s_eo_appTheMCNVmap_tables_create(cfg);
    s_eo_appTheMCNVmap_tables_initialise();

    retptr->isInited = eobool_true;

    return(retptr);
}



extern EOappTheMCNVmap* eo_appTheMCNVmap_GetHandle(void)
{
    return(&s_eo_appTheMCNVmap);
}

extern eOresult_t eo_appTheMCNVmap_GetJointNVMemoryRef(EOappTheMCNVmap* p, eOmc_jointUniqueId_t jUiniqueId, uint8_t nvindex, void**memRef)
{

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    *memRef = p->jointsList[jUiniqueId][nvindex];

     return(eores_OK);

}

extern eOresult_t eo_appTheMCNVmap_GetJointNVMemoryRef_test(EOappTheMCNVmap* p, eOmc_jointUniqueId_t jUiniqueId, uint8_t nvindex, void**memRef)
{
    eOnvID_t nv_id;
    EOnv nv;
    uint8_t ipindex, epindex, idindex;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    nv_id = eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_joint_var_Get((eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t) jUiniqueId,
                                                                    (eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t) nvindex);
    eo_nvscfg_GetIndices(p->nvsCfg, eok_ipv4addr_localhost, p->endpoint, nv_id, 
                         &ipindex, &epindex, &idindex);  
    eo_nvscfg_GetNV(p->nvsCfg, ipindex, epindex, idindex, NULL, &nv);    
    *memRef = nv.loc;

     return(eores_OK);
}


extern eOresult_t eo_appTheMCNVmap_GetMotorNVMemoryRef(EOappTheMCNVmap* p, eOmc_motorUniqueId_t mUiniqueId, uint8_t nvindex, void**memRef)
{
    void **aux;
    void *addr = 0;
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    aux = p->motorsList[mUiniqueId];
    addr = aux[nvindex];
    *memRef = p->motorsList[mUiniqueId][nvindex];

     return(eores_OK);

}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_appTheMCNVmap_tables_create(eOappTheMCNVmap_cfg_t *cfg)
{

    uint8_t         i;
    eOsizecntnr_t  size;
    eOmc_jointUniqueId_t jUId;
    eOmc_motorUniqueId_t mUId;
    EOappTheMCNVmap* p = &s_eo_appTheMCNVmap;
    

    //reset joint list
    memset(p->jointsList, 0, sizeof(p->jointsList));
    //reset motors list
    memset(p->motorsList, 0, sizeof(p->motorsList));
    //reset sensors list
    memset(p->sensorsList, 0, sizeof(p->sensorsList));


    //create joint-NV table
    eo_fifobyte_Size(cfg->jointsList, &size, 0);
  
    for(i = 0; i<size; i++)
    {
        eo_fifobyte_Get(cfg->jointsList, (uint8_t*)&jUId, 0);
        eo_fifobyte_Rem(cfg->jointsList, 0);
        p->jointsList[jUId] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                           sizeof(void*), jointNVindex_TOTALnumber);    
    }

    //create motor-NV table
    eo_fifobyte_Size(cfg->motorsList, &size, 0);
  
    for(i = 0; i<size; i++)
    {
        eo_fifobyte_Get(cfg->motorsList, (uint8_t*)&mUId, 0);
        eo_fifobyte_Rem(cfg->motorsList, 0);
        p->motorsList[mUId] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                           sizeof(void*), motorNVindex_TOTALnumber);    
    }


}

static void s_eo_appTheMCNVmap_tables_initialise(void)
{
    s_eo_appTheMCNVmap_table_motors_initialise();
    s_eo_appTheMCNVmap_table_joints_initialise();
}


static void s_eo_appTheMCNVmap_table_joints_initialise(void)
{

    uint8_t             j, ipindex, epindex, idindex;;
    EOnv                nv;
    EOnv                *nv_res_ptr;
    eOnvID_t            nv_id;
    eOresult_t          res;
    EOappTheMCNVmap     *p = &s_eo_appTheMCNVmap;

    eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t nvindex;
 
 
    //init joints-NV table   
    for(j=0; j<jointNumber_TOTALnumber; j++)
    {
        if(NULL == p->jointsList[j])
        {
            continue;
        }

        for(nvindex=jointNVindex_jconfig /*=0*/; nvindex<jointNVindex_TOTALnumber; nvindex++)
        {
            nv_id = eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_joint_var_Get((eo_cfg_nvsEP_mc_any_con_bodypart_jointNumber_t)j,
                                                                             nvindex);              
            res = eo_nvscfg_GetIndices(p->nvsCfg, eok_ipv4addr_localhost, p->endpoint, nv_id, 
                                       &ipindex, &epindex, &idindex); 
            if(eores_OK != res)
            {
                continue;
            }
            nv_res_ptr = eo_nvscfg_GetNV(p->nvsCfg, ipindex, epindex, idindex, NULL, &nv);
            if(NULL == nv_res_ptr)
            {
                continue;    
            }
            
            p->jointsList[j][nvindex] = nv.loc;               
        }   
    }


}

static void s_eo_appTheMCNVmap_table_motors_initialise(void)
{
    uint8_t             j, ipindex, epindex, idindex;;
    EOnv                nv;
    EOnv                *nv_res_ptr;
    eOnvID_t            nv_id;
    eOresult_t          res;
    EOappTheMCNVmap     *p = &s_eo_appTheMCNVmap;

    eo_cfg_nvsEP_mc_any_con_bodypart_motorNVindex_t nvindex;
 
 
    //init motors-NV table   
    for(j=0; j<motorNumber_TOTALnumber; j++)
    {
        if(NULL == p->motorsList[j])
        {
            continue;
        }

        for(nvindex=motorNVindex_mconfig /*=0*/; nvindex<motorNVindex_TOTALnumber; nvindex++)
        {
            nv_id = eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_motor_var_Get((eo_cfg_nvsEP_mc_any_con_bodypart_motorNumber_t)j,
                                                                             nvindex);              
            res = eo_nvscfg_GetIndices(p->nvsCfg, eok_ipv4addr_localhost, p->endpoint, nv_id, 
                                       &ipindex, &epindex, &idindex); 
            if(eores_OK != res)
            {
                continue;
            }
            nv_res_ptr = eo_nvscfg_GetNV(p->nvsCfg, ipindex, epindex, idindex, NULL, &nv);
            if(NULL == nv_res_ptr)
            {
                continue;    
            }
            
            p->motorsList[j][nvindex] = nv.loc;               
        }   
    }


}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




