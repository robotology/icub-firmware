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

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const char s_eobj_ownname[] = "EOappTheMCNVmap";

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
//    s_eo_appTheMCNVmap_tables_create(cfg);
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
    void *l;
    uint32_t *a;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    l = p->jointsList[jUiniqueId];
    if(NULL == l)
    {
        return(eores_NOK_nodata);   
    }
    a =(uint32_t *)l; //a memory address is in 32bit
     
    *memRef =(void*) a[nvindex];

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
    nv_id = eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_joint_var_Get(jUiniqueId, nvindex);
//    nv_id = EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(jUiniqueId, nvindex);  
    eo_nvscfg_GetIndices(p->nvsCfg, eok_ipv4addr_localhost, p->endpoint, nv_id, 
                         &ipindex, &epindex, &idindex);  
    eo_nvscfg_GetNV(p->nvsCfg, ipindex, epindex, idindex, NULL, &nv);    
    *memRef = nv.loc;

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
        //p->jointsList[jUId] = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, 
        //                                           sizeof(void*), EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber);    
    }


}

static void s_eo_appTheMCNVmap_tables_initialise(void)
{

    uint8_t         j;
    eo_cfg_nvsEP_mc_any_con_bodypart_jointNVindex_t nvindex;
    EOnv nv;
    uint8_t ipindex, epindex, idindex;
    eOnvID_t nv_id;
    eOresult_t res;
    EOappTheMCNVmap* p = &s_eo_appTheMCNVmap;
    void *addr;
    EOnv* nv_res_ptr;
 
 
    //init joints-NV table   
    for(j=0; j<10 /*eOmc_maxnumof_joints_perBodypart*/; j++)
    {
        if(NULL == p->jointsList[j])
        {
            continue;
        }
        for(nvindex=0; nvindex<EOK_cfg_nvsEP_mc_any_con_jxx_jnvindex_TOTALnumber; nvindex++)
        {
            nv_id = eo_cfg_nvsEP_mc_any_con_bodypart_NVID_for_joint_var_Get(j, nvindex);
//            nv_id = EOK_cfg_nvsEP_mc_any_con_bodypart_NVIDoff_of_joint(j, nvindex);
              
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
            addr = nv.loc;
            p->jointsList[j][nvindex] = addr;
               
        }   
    }


}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




