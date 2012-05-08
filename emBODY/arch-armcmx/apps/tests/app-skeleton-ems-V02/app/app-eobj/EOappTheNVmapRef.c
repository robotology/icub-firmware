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

#include "eOcfg_nvsEP_mc.h"
          

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOappTheNVmapRef.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOappTheNVmapRef_hid.h" 


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
static void s_eo_appTheNVmapRef_tables_create(eOappTheNVmapRef_cfg_t *cfg);
static void s_eo_appTheNVmapRef_tables_initialise(void);
static void s_eo_appTheNVmapRef_table_motors_initialise(void);
static void s_eo_appTheNVmapRef_table_joints_initialise(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


//static const char s_eobj_ownname[] = "EOappTheNVmapRef";

static EOappTheNVmapRef s_eo_appTheNVmapRef;



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOappTheNVmapRef* eo_appTheNVmapRef_Initialise(eOappTheNVmapRef_cfg_t *cfg)
{
    EOappTheNVmapRef* retptr = NULL;
    if(NULL == cfg)
    {
        return(retptr);
    }
    
    retptr = &s_eo_appTheNVmapRef;
    retptr->nvsCfg = cfg->nvsCfg;
    retptr->mc_endpoint = cfg->mc_endpoint;
    retptr->as_endpoint = cfg->as_endpoint;
    s_eo_appTheNVmapRef_tables_create(cfg);
    s_eo_appTheNVmapRef_tables_initialise();

    retptr->isInited = eobool_true;

    return(retptr);
}



extern EOappTheNVmapRef* eo_appTheNVmapRef_GetHandle(void)
{
    return(&s_eo_appTheNVmapRef);
}

extern eOresult_t eo_appTheNVmapRef_GetJointNVMemoryRef(EOappTheNVmapRef* p, eOmc_jointId_t jUiniqueId, uint8_t nvindex, void**memRef)
{

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    *memRef = p->jointsList[jUiniqueId][nvindex];

     return(eores_OK);

}

extern eOresult_t eo_appTheNVmapRef_GetJointNVMemoryRef_test(EOappTheNVmapRef* p, eOmc_jointId_t jId, uint8_t nvindex, void**memRef)
{
    eOnvID_t nv_id;
    EOnv nv;
    uint16_t ipindex, epindex, idindex;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    nv_id = eo_cfg_nvsEP_mc_joint_NVID_Get((eo_cfg_nvsEP_mc_endpoint_t)p->mc_endpoint, (eo_cfg_nvsEP_mc_jointNumber_t)jId, (eo_cfg_nvsEP_mc_jointNVindex_t) nvindex);
    eo_nvscfg_GetIndices(p->nvsCfg, eok_ipv4addr_localhost, p->mc_endpoint, nv_id, 
                         &ipindex, &epindex, &idindex);  
    eo_nvscfg_GetNV(p->nvsCfg, ipindex, epindex, idindex, NULL, &nv);    
    *memRef = nv.loc;

     return(eores_OK);
}


extern eOresult_t eo_appTheNVmapRef_GetMotorNVMemoryRef(EOappTheNVmapRef* p, eOmc_motorId_t mId, uint8_t nvindex, void**memRef)
{
    void **aux;
    void *addr = 0;
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    aux = p->motorsList[mId];
    addr = aux[nvindex];
    *memRef = p->motorsList[mId][nvindex];

    addr = addr;
     return(eores_OK);

}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_eo_appTheNVmapRef_tables_create(eOappTheNVmapRef_cfg_t *cfg)
{

    uint8_t         i;
    eOsizecntnr_t  size;
    eOmc_jointId_t jUId;
    eOmc_motorId_t mUId;
    EOappTheNVmapRef* p = &s_eo_appTheNVmapRef;
    

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

static void s_eo_appTheNVmapRef_tables_initialise(void)
{
    s_eo_appTheNVmapRef_table_motors_initialise();
    s_eo_appTheNVmapRef_table_joints_initialise();
}


static void s_eo_appTheNVmapRef_table_joints_initialise(void)
{

    uint8_t             j;
    uint16_t            ipindex, epindex, idindex;
    EOnv                nv;
    EOnv                *nv_res_ptr;
    eOnvID_t            nv_id;
    eOresult_t          res;
    EOappTheNVmapRef     *p = &s_eo_appTheNVmapRef;

    eo_cfg_nvsEP_mc_jointNVindex_t jnvindex;
 
 
    //init joints-NV table   
    for(j=0; j<jointEndpoint_TOTALnumber; j++)
    {
        if(NULL == p->jointsList[j])
        {
            continue;
        }

        for(jnvindex=jointNVindex_jconfig /*=0*/; jnvindex<jointNVindex_TOTALnumber; jnvindex++)
        {
            nv_id = eo_cfg_nvsEP_mc_joint_NVID_Get((eo_cfg_nvsEP_mc_endpoint_t)p->mc_endpoint, (eo_cfg_nvsEP_mc_jointNumber_t)j, jnvindex);
            res = eo_nvscfg_GetIndices(p->nvsCfg, eok_ipv4addr_localhost, p->mc_endpoint, nv_id, 
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
            
            p->jointsList[j][jnvindex] = nv.loc;               
        }   
    }


}

static void s_eo_appTheNVmapRef_table_motors_initialise(void)
{
    uint8_t             m;
    uint16_t            ipindex, epindex, idindex;;
    EOnv                nv;
    EOnv                *nv_res_ptr;
    eOnvID_t            nv_id;
    eOresult_t          res;
    EOappTheNVmapRef     *p = &s_eo_appTheNVmapRef;

    eo_cfg_nvsEP_mc_motorNVindex_t mnvindex;
 
 
    //init motors-NV table   
    for(m=0; m<motorEndpoint_TOTALnumber; m++)
    {
        if(NULL == p->motorsList[m])
        {
            continue;
        }

        for(mnvindex=motorNVindex_mconfig /*=0*/; mnvindex<motorNVindex_TOTALnumber; mnvindex++)
        {
            nv_id = eo_cfg_nvsEP_mc_motor_NVID_Get((eo_cfg_nvsEP_mc_endpoint_t)p->mc_endpoint, (eo_cfg_nvsEP_mc_motorNumber_t) m, mnvindex);
            res = eo_nvscfg_GetIndices(p->nvsCfg, eok_ipv4addr_localhost, p->mc_endpoint, nv_id, 
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
            
            p->motorsList[m][mnvindex] = nv.loc;               
        }   
    }


}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




