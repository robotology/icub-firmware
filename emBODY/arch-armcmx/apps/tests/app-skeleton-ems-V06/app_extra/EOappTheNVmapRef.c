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

#include "EoMotionControl.h"
#include "EoSkin.h"
          

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
static eOresult_t s_eo_appTheNVmapRef_tables_create(eOappTheNVmapRef_cfg_t *cfg);
static eOresult_t s_eo_appTheNVmapRef_tables_initialise(void);
static eOresult_t s_eo_appTheNVmapRef_table_motors_initialise(void);
static eOresult_t s_eo_appTheNVmapRef_table_joints_initialise(void);
static eOresult_t s_eo_appTheNVmapRef_table_skin_initialise(void);

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
    eOresult_t res;
    EOappTheNVmapRef* retptr = NULL;
    if(NULL == cfg)
    {
        return(retptr);
    }
    
    retptr = &s_eo_appTheNVmapRef;
    retptr->nvsCfg = cfg->nvsCfg;
    retptr->mc_endpoint = cfg->mc_endpoint;
    retptr->as_endpoint = cfg->as_endpoint;
    retptr->sk_endpoint = cfg->sk_endpoint;

    res = s_eo_appTheNVmapRef_tables_create(cfg);
    if(eores_OK != res)
    {
        return(NULL);    
    }
    res = s_eo_appTheNVmapRef_tables_initialise();
    if(eores_OK != res)
    {
        return(NULL);    
    }

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
    nv_id = eo_cfg_nvsEP_mc_joint_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)p->mc_endpoint, (eOcfg_nvsEP_mc_jointNumber_t)jId, (eOcfg_nvsEP_mc_jointNVindex_t) nvindex);
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


extern eOresult_t eo_appTheNVmapRef_GetSkinNVMemoryRef(EOappTheNVmapRef* p, eOsk_skinId_t skId, uint8_t nvindex, void**memRef)
{
    *memRef = p->skinList[skId][nvindex];
    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static eOresult_t s_eo_appTheNVmapRef_tables_create(eOappTheNVmapRef_cfg_t *cfg)
{

    uint8_t         i;
    eOsizecntnr_t  size;
    EOappTheNVmapRef* p = &s_eo_appTheNVmapRef;
    eOmc_jointId_t *jid_ptr;
    eOmc_motorId_t *mid_ptr;
    eOsk_skinId_t  *skid_ptr;
    

    //reset joint list
    memset(p->jointsList, 0, sizeof(p->jointsList));
    //reset motors list
    memset(p->motorsList, 0, sizeof(p->motorsList));
    //reset sensors list
    memset(p->sensorsList, 0, sizeof(p->sensorsList));
    //reset skin list
    memset(p->skinList, 0, sizeof(p->skinList));



    size = 0;
    //create joint-NV table
    if(NULL != cfg->jointsList)
    {
        size = eo_array_Size(cfg->jointsList); //eo_fifoword_Size(cfg->jointsList, &size, 0);
      
        for(i = 0; i<size; i++)
        {
            jid_ptr = (eOmc_jointId_t*) eo_array_At(cfg->jointsList, i);
            if(NULL == jid_ptr)
            {
                return(eores_NOK_generic);
            }
            
            p->jointsList[(*jid_ptr)] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                             sizeof(void*), jointNVindex_TOTALnumber);    
        }
    }

    size = 0;
    //create motor-NV table
    if(NULL != cfg->motorsList)
    {
        size = eo_array_Size(cfg->motorsList); //eo_fifoword_Size(cfg->motorsList, &size, 0);
      
        for(i = 0; i<size; i++)
        {
            mid_ptr = (eOmc_motorId_t*) eo_array_At(cfg->motorsList, i);
            if(NULL == mid_ptr)
            {
                return(eores_NOK_generic);
            }

            p->motorsList[(*mid_ptr)] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                               sizeof(void*), motorNVindex_TOTALnumber);
                
        }
    }


    size = 0;
    //create skin-NV table
    if(NULL != cfg->skinList)
    {
        size = eo_array_Size(cfg->skinList); //eo_fifoword_Size(cfg->skinList, &size, 0);
      
        for(i = 0; i<size; i++)
        {
            skid_ptr = (eOsk_skinId_t*) eo_array_At(cfg->skinList, i);
            if(NULL == skid_ptr)
            {
                return(eores_NOK_generic);
            }
            p->skinList[*skid_ptr] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                               sizeof(void*), skinNVindex_TOTALnumber);    
        }
    }

    return(eores_OK);

}

static eOresult_t s_eo_appTheNVmapRef_tables_initialise(void)
{
    eOresult_t res;

    res = s_eo_appTheNVmapRef_table_motors_initialise();
    if(eores_OK != res)
    {
        return(res);
    }

    res = s_eo_appTheNVmapRef_table_joints_initialise();
    if(eores_OK != res)
    {
        return(res);
    }

    res = s_eo_appTheNVmapRef_table_skin_initialise();
    if(eores_OK != res)
    {
        return(res);
    }

    return(eores_OK);
}


static eOresult_t s_eo_appTheNVmapRef_table_joints_initialise(void)
{

    uint8_t             j;
    uint16_t            ipindex, epindex, idindex;
    EOnv                nv;
    EOnv                *nv_res_ptr;
    eOnvID_t            nv_id;
    eOresult_t          res;
    EOappTheNVmapRef     *p = &s_eo_appTheNVmapRef;

    eOcfg_nvsEP_mc_jointNVindex_t jnvindex;
 
 
    //init joints-NV table   
    for(j=0; j<jointNumberMAX; j++)
    {
        if(NULL == p->jointsList[j])
        {
            continue;
        }

        for(jnvindex=jointNVindex_jconfig /*=0*/; jnvindex<jointNVindex_TOTALnumber; jnvindex++)
        {
            nv_id = eo_cfg_nvsEP_mc_joint_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)p->mc_endpoint, (eOcfg_nvsEP_mc_jointNumber_t)j, jnvindex);
            res = eo_nvscfg_GetIndices(p->nvsCfg, eok_ipv4addr_localhost, p->mc_endpoint, nv_id, 
                                       &ipindex, &epindex, &idindex); 
            if(eores_OK != res)
            {
                return(res);
            }
            nv_res_ptr = eo_nvscfg_GetNV(p->nvsCfg, ipindex, epindex, idindex, NULL, &nv);
            if(NULL == nv_res_ptr)
            {
                return(eores_NOK_nullpointer);    
            }
            
            p->jointsList[j][jnvindex] = nv.loc;               
        }   
    }
   return(eores_OK);

}

static eOresult_t s_eo_appTheNVmapRef_table_motors_initialise(void)
{
    uint8_t             m;
    uint16_t            ipindex, epindex, idindex;;
    EOnv                nv;
    EOnv                *nv_res_ptr;
    eOnvID_t            nv_id;
    eOresult_t          res;
    EOappTheNVmapRef     *p = &s_eo_appTheNVmapRef;

    eOcfg_nvsEP_mc_motorNVindex_t mnvindex;
 
 
    //init motors-NV table   
    for(m=0; m<motorNumberMAX; m++)
    {
        if(NULL == p->motorsList[m])
        {
            continue;
        }

        for(mnvindex=motorNVindex_mconfig /*=0*/; mnvindex<motorNVindex_TOTALnumber; mnvindex++)
        {
            nv_id = eo_cfg_nvsEP_mc_motor_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)p->mc_endpoint, (eOcfg_nvsEP_mc_motorNumber_t) m, mnvindex);
            res = eo_nvscfg_GetIndices(p->nvsCfg, eok_ipv4addr_localhost, p->mc_endpoint, nv_id, 
                                       &ipindex, &epindex, &idindex); 
            if(eores_OK != res)
            {
                return(res);
            }
            nv_res_ptr = eo_nvscfg_GetNV(p->nvsCfg, ipindex, epindex, idindex, NULL, &nv);
            if(NULL == nv_res_ptr)
            {
                return(eores_NOK_nullpointer);    
            }
            
            p->motorsList[m][mnvindex] = nv.loc;               
        }   
    }
    return(eores_OK);
}

static eOresult_t s_eo_appTheNVmapRef_table_skin_initialise(void)
{
    uint8_t             sk;
    uint16_t            ipindex, epindex, idindex;;
    EOnv                nv;
    EOnv                *nv_res_ptr;
    eOnvID_t            nv_id;
    eOresult_t          res;
    EOappTheNVmapRef     *p = &s_eo_appTheNVmapRef;

    eOcfg_nvsEP_sk_skinNVindex_t sknvindex;
 
 
    //init motors-NV table   
    for(sk=0; sk<skinNumberMAX; sk++)
    {
        if(NULL == p->skinList[sk])
        {
            continue;
        }

        for(sknvindex=skinNVindex_sconfig__sigmode /*=0*/; sknvindex<skinNVindex_TOTALnumber; sknvindex++)
        {
            nv_id = eo_cfg_nvsEP_sk_NVID_Get((eOcfg_nvsEP_sk_endpoint_t)p->sk_endpoint, sk, sknvindex);
            res = eo_nvscfg_GetIndices(p->nvsCfg, eok_ipv4addr_localhost, p->sk_endpoint, nv_id, 
                                       &ipindex, &epindex, &idindex); 
            if(eores_OK != res)
            {
                return(res);
            }
            nv_res_ptr = eo_nvscfg_GetNV(p->nvsCfg, ipindex, epindex, idindex, NULL, &nv);
            if(NULL == nv_res_ptr)
            {
                return(eores_NOK_nullpointer);   
            }
            
            p->skinList[sk][sknvindex] = nv.loc;               
        }   
    }

    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




