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
static eOresult_t s_eo_appTheNVmapRef_table_sensorsStrain_initialise(void);
static eOresult_t s_eo_appTheNVmapRef_table_sensorsMais_initialise(void);
static eOresult_t s_eo_appTheNVmapRef_checkConfiguration(eOappTheNVmapRef_cfg_t *cfg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const char s_eobj_ownname[] = "EOappTheNVmapRef";

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
    
    s_eo_appTheNVmapRef_checkConfiguration(cfg);
    retptr = &s_eo_appTheNVmapRef;
    
    memcpy(&retptr->cfg, cfg, sizeof(eOappTheNVmapRef_cfg_t));

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

    if(NULL == p->jointsList[jUiniqueId])
    {
        return(eores_NOK_generic);
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
    nv_id = eo_cfg_nvsEP_mc_joint_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)p->cfg.mc_endpoint, (eOcfg_nvsEP_mc_jointNumber_t)jId, (eOcfg_nvsEP_mc_jointNVindex_t) nvindex);
    eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.mc_endpoint, nv_id, 
                         &ipindex, &epindex, &idindex);  
    eo_nvscfg_GetNV(p->cfg.nvsCfg, ipindex, epindex, idindex, NULL, &nv);    
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

    if(NULL == p->motorsList[mId])
    {
        return(eores_NOK_generic);
    }

    aux = p->motorsList[mId];
    addr = aux[nvindex];
    *memRef = p->motorsList[mId][nvindex];

    addr = addr;
    return(eores_OK);

}


extern eOresult_t eo_appTheNVmapRef_GetSensorsMaisNVMemoryRef(EOappTheNVmapRef* p, eOsnsr_sensorId_t sId, uint8_t nvindex, void**memRef)
{

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if(NULL == p->sensorsMaisList[sId])
    {
        return(eores_NOK_generic);
    }

    *memRef = p->sensorsMaisList[sId][nvindex];

     return(eores_OK);
}


extern eOresult_t eo_appTheNVmapRef_GetSensorsStrainNVMemoryRef(EOappTheNVmapRef* p, eOsnsr_sensorId_t sId, uint8_t nvindex, void**memRef)
{

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(NULL == p->sensorsStrainList[sId])
    {
        return(eores_NOK_generic);
    }

    *memRef = p->sensorsStrainList[sId][nvindex];

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
static eOresult_t s_eo_appTheNVmapRef_checkConfiguration(eOappTheNVmapRef_cfg_t *cfg)
{
    if(eo_array_Size(cfg->jointsList) == 0)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "no joints are config"); 
        return(eores_OK);
    }
    
    if(eo_array_Size(cfg->motorsList) == 0)
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_weak, s_eobj_ownname, "no motorss are config"); 
        return(eores_OK);
    }
    
    //if array cfg contains a mais or board, but cfg hasn't ep_as, then error!
    if(((eo_array_Size(cfg->sensorsStrainList) > 0) || (eo_array_Size(cfg->sensorsStrainList) > 0)) &&
       (cfg->as_endpoint == EOK_uint16dummy))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "mismatch in sesnsor cfg"); 
        return(eores_NOK_generic);
    }
    //if array cfg contains a mais or board, but cfg hasn't ep_as, then error!
    if((eo_array_Size(cfg->skinList) > 0) && (cfg->sk_endpoint == EOK_uint16dummy))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "mismatch in skin cfg"); 
        return(eores_NOK_generic);
    }
    
    return(eores_OK);
    
}




static eOresult_t s_eo_appTheNVmapRef_tables_create(eOappTheNVmapRef_cfg_t *cfg)
{

    uint8_t         i;
    eOsizecntnr_t  size;
    EOappTheNVmapRef* p = &s_eo_appTheNVmapRef;
    eOmc_jointId_t *jid_ptr;
    eOmc_motorId_t *mid_ptr;
    eOsk_skinId_t  *skid_ptr;
    eOsnsr_sensorId_t *sid_ptr;
    

    //reset joint list
    memset(p->jointsList, 0, sizeof(p->jointsList));
    //reset motors list
    memset(p->motorsList, 0, sizeof(p->motorsList));
    //reset sensors-strain list
    memset(p->sensorsStrainList, 0, sizeof(p->sensorsStrainList));
    //reset sensors-mais list
    memset(p->sensorsMaisList, 0, sizeof(p->sensorsMaisList));
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
    //create sensorStrain-NV table
    if(NULL != cfg->sensorsStrainList)
    {
        size = eo_array_Size(cfg->sensorsStrainList); //eo_fifoword_Size(cfg->skinList, &size, 0);
      
        for(i = 0; i<size; i++)
        {
            sid_ptr = (eOsnsr_sensorId_t*) eo_array_At(cfg->sensorsStrainList, i);
            if(NULL == sid_ptr)
            {
                return(eores_NOK_generic);
            }
            p->sensorsStrainList[*sid_ptr] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                               sizeof(void*), strainNVindex_TOTALnumber);
            
        }
    }

    size = 0;
    //create sensorMais-NV table
    if(NULL != cfg->sensorsMaisList)
    {
        size = eo_array_Size(cfg->sensorsMaisList); //eo_fifoword_Size(cfg->skinList, &size, 0);
      
        for(i = 0; i<size; i++)
        {
            sid_ptr = (eOsnsr_sensorId_t*) eo_array_At(cfg->sensorsMaisList, i);
            if(NULL == sid_ptr)
            {
                return(eores_NOK_generic);
            }
            p->sensorsMaisList[*sid_ptr] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                               sizeof(void*), maisNVindex_TOTALnumber);
            
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

    
    res = s_eo_appTheNVmapRef_table_sensorsStrain_initialise();
    if(eores_OK != res)
    {
        return(res);
    }

    
    res = s_eo_appTheNVmapRef_table_sensorsMais_initialise();
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
            nv_id = eo_cfg_nvsEP_mc_joint_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)p->cfg.mc_endpoint, (eOcfg_nvsEP_mc_jointNumber_t)j, jnvindex);
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
            nv_id = eo_cfg_nvsEP_mc_motor_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)p->cfg.mc_endpoint, (eOcfg_nvsEP_mc_motorNumber_t) m, mnvindex);
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
            
            p->motorsList[m][mnvindex] = nv.loc;               
        }   
    }
    return(eores_OK);
}


static eOresult_t s_eo_appTheNVmapRef_table_sensorsStrain_initialise(void)
{

    uint8_t             s;
    uint16_t            ipindex, epindex, idindex;
    EOnv                nv;
    EOnv                *nv_res_ptr;
    eOnvID_t            nv_id;
    eOresult_t          res;
    EOappTheNVmapRef     *p = &s_eo_appTheNVmapRef;

    eOcfg_nvsEP_as_strainNVindex_t snvindex;
 
 
    if(NULL == p->cfg.sensorsStrainList)
    {
        return(eores_OK);
    }
    
    for(s=0; s<strainNumberMAX; s++)
    {
        if(NULL == p->sensorsStrainList[s])
        {
            continue;
        }

        for(snvindex=strainNVindex_sconfig__mode /*=0*/; snvindex<strainNVindex_TOTALnumber; snvindex++)
        {
            nv_id = eo_cfg_nvsEP_as_strain_NVID_Get((eOcfg_nvsEP_as_endpoint_t)p->cfg.as_endpoint, (eOcfg_nvsEP_as_strainNumber_t)s, snvindex);
            res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.as_endpoint, nv_id, 
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
            
            p->sensorsStrainList[s][snvindex] = nv.loc;               
        } 
    }        
   
   return(eores_OK);

}

static eOresult_t s_eo_appTheNVmapRef_table_sensorsMais_initialise(void)
{

    uint8_t             s;
    uint16_t            ipindex, epindex, idindex;
    EOnv                nv;
    EOnv                *nv_res_ptr;
    eOnvID_t            nv_id;
    eOresult_t          res;
    EOappTheNVmapRef     *p = &s_eo_appTheNVmapRef;

    eOcfg_nvsEP_as_maisNVindex_t snvindex;
 
 
    if(NULL == p->cfg.sensorsMaisList)
    {
        return(eores_OK);
    }
    
    for(s=0; s<maisNumberMAX; s++)
    {
        if(NULL == p->sensorsMaisList[s])
        {
            continue;
        }

        for(snvindex=maisNVindex_mconfig__mode /*=0*/; snvindex<maisNVindex_TOTALnumber; snvindex++)
        {
            nv_id = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)p->cfg.as_endpoint, (eOcfg_nvsEP_as_maisNumber_t)s, snvindex);
            res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.as_endpoint, nv_id, 
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
            
            p->sensorsMaisList[s][snvindex] = nv.loc;               
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
            nv_id = eo_cfg_nvsEP_sk_NVID_Get((eOcfg_nvsEP_sk_endpoint_t)p->cfg.sk_endpoint, sk, sknvindex);
            res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.sk_endpoint, nv_id, 
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
            
            p->skinList[sk][sknvindex] = nv.loc;               
        }   
    }

    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




