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

#include "EOappTheDataBase.h"
          

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOappTheNVmapRef-NEW.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOappTheNVmapRef_hid-NEW.h" 


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
static eOresult_t s_eo_appTheNVmapRef_tables_create(EOappTheNVmapRef* p, eOappTheNVmapRef_cfg_t *cfg);
static eOresult_t s_eo_appTheNVmapRef_tables_initialise(EOappTheNVmapRef* p);
static eOresult_t s_eo_appTheNVmapRef_table_motors_initialise(EOappTheNVmapRef* p);
static eOresult_t s_eo_appTheNVmapRef_table_joints_initialise(EOappTheNVmapRef* p);
static eOresult_t s_eo_appTheNVmapRef_table_skin_initialise(void);
static eOresult_t s_eo_appTheNVmapRef_table_sensorsStrain_initialise(void);
static eOresult_t s_eo_appTheNVmapRef_table_sensorsMais_initialise(void);

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
    
    retptr = &s_eo_appTheNVmapRef;
    
    memcpy(&retptr->cfg, cfg, sizeof(eOappTheNVmapRef_cfg_t));

    res = s_eo_appTheNVmapRef_tables_create(retptr, cfg);
    if(eores_OK != res)
    {
        return(NULL);    
    }
    res = s_eo_appTheNVmapRef_tables_initialise(retptr);
    if(eores_OK != res)
    {
        return(NULL);    
    }

    retptr->isInited = eobool_true;

    return(retptr);
}



extern EOappTheNVmapRef* eo_appTheNVmapRef_GetHandle(void)
{
    return((s_eo_appTheNVmapRef.isInited == eobool_true) ? (&s_eo_appTheNVmapRef):  NULL);
}

extern eOresult_t eo_appTheNVmapRef_GetJointNVMemoryRef(EOappTheNVmapRef* p, eOmc_jointId_t jId, uint8_t nvindex, void**memRef)
{

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if((jId >= p->jointMaxNumber) || (nvindex >= eOappNVmapRef_jointNVindex_TOTALnumber))
    {
        return(eores_NOK_generic);
    }
     
    *memRef = p->jointsList[jId][nvindex];

     return(eores_OK);
}


extern eOresult_t eo_appTheNVmapRef_GetMotorNVMemoryRef(EOappTheNVmapRef* p, eOmc_motorId_t mId, eOappNVmapRef_motorNVindex_t nvindex, void**memRef)
{
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if((mId >= p->motorNumberMAX) || (nvindex >= eOappNVmapRef_motorNVindex_TOTALnumber))
    {
        return(eores_NOK_generic);
    }
        
    *memRef = p->motorsList[mId][nvindex];

    return(eores_OK);

}


extern eOresult_t eo_appTheNVmapRef_GetSensorsMaisNVMemoryRef(EOappTheNVmapRef* p, eOsnsr_sensorId_t sId, uint8_t nvindex, void**memRef)
{

//     if(NULL == p)
//     {
//         return(eores_NOK_nullpointer);
//     }

//     if(NULL == p->sensorsMaisList[sId])
//     {
//         return(eores_NOK_generic);
//     }

//     *memRef = p->sensorsMaisList[sId][nvindex];

     return(eores_OK);
}


extern eOresult_t eo_appTheNVmapRef_GetSensorsStrainNVMemoryRef(EOappTheNVmapRef* p, eOsnsr_sensorId_t sId, uint8_t nvindex, void**memRef)
{

//     if(NULL == p)
//     {
//         return(eores_NOK_nullpointer);
//     }
//     
//     if(NULL == p->sensorsStrainList[sId])
//     {
//         return(eores_NOK_generic);
//     }

//     *memRef = p->sensorsStrainList[sId][nvindex];

     return(eores_OK);
}

extern eOresult_t eo_appTheNVmapRef_GetSkinNVMemoryRef(EOappTheNVmapRef* p, eOsk_skinId_t skId, uint8_t nvindex, void**memRef)
{
//    *memRef = p->skinList[skId][nvindex];
    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_appTheNVmapRef_tables_create(EOappTheNVmapRef* p, eOappTheNVmapRef_cfg_t *cfg)
{

    uint8_t         i;

//joint    
    p->jointMaxNumber = eo_appTheDB_GetNumeberOfConnectedJoints(eo_appTheDB_GetHandle);

    p->jointsList = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(void**), 
                                         p->jointMaxNumber);
      
    for(i = 0; i<p->jointMaxNumber; i++)
    {
        p->jointsList[i] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                sizeof(void*), eOappNVmapRef_jointNVindex_TOTALnumber);    
    }
    
//motor
    p->motortMaxNumber = eo_cfg_nvsEP_mc_motor_numbermax_Get((eOcfg_nvsEP_mc_endpoint_t)p->cfg.mc_endpoint);

    p->motorsList = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(void**), 
                                         p->motorMaxNumber);
      
    for(i = 0; i<p->motortMaxNumber; i++)
    {
        p->motortsList[i] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
                                                sizeof(void*), eOappNVmapRef_motorNVindex_TOTALnumber);    
    }
    
    
    return(eores_OK);

}

static eOresult_t s_eo_appTheNVmapRef_tables_initialise(EOappTheNVmapRef* p)
{
    eOresult_t res;

    res = s_eo_appTheNVmapRef_table_motors_initialise(p);
    if(eores_OK != res)
    {
        return(res);
    }

    res = s_eo_appTheNVmapRef_table_joints_initialise(p);
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


static eOresult_t s_eo_appTheNVmapRef_table_joints_initialise(EOappTheNVmapRef* p)
{

    uint8_t                         j;
    uint16_t                        ipindex, epindex, idindex;
    EOnv                            nv;
    EOnv                            *nv_res_ptr;
    eOnvID_t                        nv_id;
    eOresult_t                      res;
    eOappNVmapRef_jointNVindex_t    jnvindex;
 
 
    //init joints-NV table   
    for(j=0; j<p->jointMaxNumber; j++)
    {
        for(jnvindex=eOappNVmapRef_jointNVindex_jconfig /*=0*/; jnvindex<eOappNVmapRef_jointNVindex_TOTALnumber; jnvindex++)
        {
            nv_id = eo_cfg_nvsEP_mc_joint_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)p->cfg.mc_endpoint, 
                                                   (eOcfg_nvsEP_mc_jointNumber_t)j, 
                                                   eo_appTheNVmapRef_GetMCjointNVindex(jnvindex));
            
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

static eOresult_t s_eo_appTheNVmapRef_table_motors_initialise(EOappTheNVmapRef* p)
{
    uint8_t             m;
    uint16_t            ipindex, epindex, idindex;;
    EOnv                nv;
    EOnv                *nv_res_ptr;
    eOnvID_t            nv_id;
    eOresult_t          res;
    EOappTheNVmapRef     *p = &s_eo_appTheNVmapRef;

    eOappNVmapRef_jointNVindex_t mnvindex;

    //init joints-NV table   
    for(m=0; m<p->motorMaxNumber; m++)
    {
        for(mnvindex=eOappNVmapRef_motorNVindex_mconfig /*=0*/; mnvindex<eOappNVmapRef_motorNVindex_TOTALnumber; mnvindex++)
        {
            nv_id = eo_cfg_nvsEP_mc_motor_NVID_Get((eOcfg_nvsEP_mc_endpoint_t)p->cfg.mc_endpoint, 
                                                   (eOcfg_nvsEP_mc_motorNumber_t)m, 
                                                   eo_appTheNVmapRef_GetMCmotorNVindex(mnvindex));
            
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

//     uint8_t             s;
//     uint16_t            ipindex, epindex, idindex;
//     EOnv                nv;
//     EOnv                *nv_res_ptr;
//     eOnvID_t            nv_id;
//     eOresult_t          res;
//     EOappTheNVmapRef     *p = &s_eo_appTheNVmapRef;

//     eOcfg_nvsEP_as_strainNVindex_t snvindex;
//  
//  
//     if(NULL == p->cfg.sensorsStrainList)
//     {
//         return(eores_OK);
//     }
//     
//     for(s=0; s<strainNumberMAX; s++)
//     {
//         if(NULL == p->sensorsStrainList[s])
//         {
//             continue;
//         }

//         for(snvindex=strainNVindex_sconfig__mode /*=0*/; snvindex<strainNVindex_TOTALnumber; snvindex++)
//         {
//             nv_id = eo_cfg_nvsEP_as_strain_NVID_Get((eOcfg_nvsEP_as_endpoint_t)p->cfg.as_endpoint, (eOcfg_nvsEP_as_strainNumber_t)s, snvindex);
//             res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.as_endpoint, nv_id, 
//                                        &ipindex, &epindex, &idindex); 
//             if(eores_OK != res)
//             {
//                 return(res);
//             }
//             nv_res_ptr = eo_nvscfg_GetNV(p->cfg.nvsCfg, ipindex, epindex, idindex, NULL, &nv);
//             if(NULL == nv_res_ptr)
//             {
//                 return(eores_NOK_nullpointer);    
//             }
//             
//             p->sensorsStrainList[s][snvindex] = nv.loc;               
//         } 
//     }        
   
   return(eores_OK);

}

static eOresult_t s_eo_appTheNVmapRef_table_sensorsMais_initialise(void)
{

//     uint8_t             s;
//     uint16_t            ipindex, epindex, idindex;
//     EOnv                nv;
//     EOnv                *nv_res_ptr;
//     eOnvID_t            nv_id;
//     eOresult_t          res;
//     EOappTheNVmapRef     *p = &s_eo_appTheNVmapRef;

//     eOcfg_nvsEP_as_maisNVindex_t snvindex;
//  
//  
//     if(NULL == p->cfg.sensorsMaisList)
//     {
//         return(eores_OK);
//     }
//     
//     for(s=0; s<maisNumberMAX; s++)
//     {
//         if(NULL == p->sensorsMaisList[s])
//         {
//             continue;
//         }

//         for(snvindex=maisNVindex_mconfig__mode /*=0*/; snvindex<maisNVindex_TOTALnumber; snvindex++)
//         {
//             nv_id = eo_cfg_nvsEP_as_mais_NVID_Get((eOcfg_nvsEP_as_endpoint_t)p->cfg.as_endpoint, (eOcfg_nvsEP_as_maisNumber_t)s, snvindex);
//             res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.as_endpoint, nv_id, 
//                                        &ipindex, &epindex, &idindex); 
//             if(eores_OK != res)
//             {
//                 return(res);
//             }
//             nv_res_ptr = eo_nvscfg_GetNV(p->cfg.nvsCfg, ipindex, epindex, idindex, NULL, &nv);
//             if(NULL == nv_res_ptr)
//             {
//                 return(eores_NOK_nullpointer);    
//             }
//             
//             p->sensorsMaisList[s][snvindex] = nv.loc;               
//         } 
//     }        
   
   return(eores_OK);

}


static eOresult_t s_eo_appTheNVmapRef_table_skin_initialise(void)
{
//     uint8_t             sk;
//     uint16_t            ipindex, epindex, idindex;;
//     EOnv                nv;
//     EOnv                *nv_res_ptr;
//     eOnvID_t            nv_id;
//     eOresult_t          res;
//     EOappTheNVmapRef     *p = &s_eo_appTheNVmapRef;

//     eOcfg_nvsEP_sk_skinNVindex_t sknvindex;
//  
//  
//     //init motors-NV table   
//     for(sk=0; sk<skinNumberMAX; sk++)
//     {
//         if(NULL == p->skinList[sk])
//         {
//             continue;
//         }

//         for(sknvindex=skinNVindex_sconfig__sigmode /*=0*/; sknvindex<skinNVindex_TOTALnumber; sknvindex++)
//         {
//             nv_id = eo_cfg_nvsEP_sk_NVID_Get((eOcfg_nvsEP_sk_endpoint_t)p->cfg.sk_endpoint, sk, sknvindex);
//             res = eo_nvscfg_GetIndices(p->cfg.nvsCfg, eok_ipv4addr_localhost, p->cfg.sk_endpoint, nv_id, 
//                                        &ipindex, &epindex, &idindex); 
//             if(eores_OK != res)
//             {
//                 return(res);
//             }
//             nv_res_ptr = eo_nvscfg_GetNV(p->cfg.nvsCfg, ipindex, epindex, idindex, NULL, &nv);
//             if(NULL == nv_res_ptr)
//             {
//                 return(eores_NOK_nullpointer);   
//             }
//             
//             p->skinList[sk][sknvindex] = nv.loc;               
//         }   
//     }

    return(eores_OK);
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




