/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "stdio.h"
#include "string.h"
#include "EoCommon.h"
#include "eEcommon.h"

#include "EOtheErrorManager.h"

#include "EOVtheEnvironment_hid.h"

#include "hal.h"


#include "hal_uniqueid.h"

#include "eEsharedServices.h" 

#include "eEmemorymap.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheARMenvironment.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheARMenvironment_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

 // --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOarmenv_cfg_t eo_armenv_DefaultCfg = 
{
    .dummy              = 0
};



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_armenv_prepare(const eEmoduleInfo_t *modinfo, const eEboardInfo_t *brdinfo);
static void s_eo_armenv_sharedservices_init_sync(void);
static void s_eo_armenv_sharedservices_init(void);
static void s_eo_armenv_sharedservices_synchronise(void);

static eOresult_t s_eo_armenv_shareddata_sync(EOVtheEnvironmentDerived *p);
static eOresult_t s_eo_armenv_code_proc_offset_get(EOVtheEnvironmentDerived *p, uint32_t *offset);
static eOresult_t s_eo_armenv_shareddata_ipnet_get(EOVtheEnvironmentDerived *p, const void **ipnet);
static eOresult_t s_eo_armenv_shareddata_cannets_get(EOVtheEnvironmentDerived *p, const void **cannets, uint8_t *numnets);
static eOresult_t s_eo_armenv_eproc_get(EOVtheEnvironmentDerived *p, eEprocess_t *eproc);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOtheARMenvironment";

static EOtheARMenvironment s_the_armenv = 
{
    // the base object
    .env            = NULL,
    // other stuff
    .modinfo        = NULL,
    .brdinfo        = NULL,
    .devinfo        = NULL,
    .codeprocoffset = 0,
    .eprocess       = ee_procNone
};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheARMenvironment * eo_armenv_Initialise(const eEmoduleInfo_t *modinfo, const eEboardInfo_t *brdinfo)
{
    
    
    if(NULL == s_the_armenv.env)
    {
        // allow initialisation with null cfg ..... but not without the moduleinfo
        
        eo_errman_Assert(eo_errman_GetHandle(), (NULL != modinfo), "eo_armenv_Initialise(): NULL modinfo", s_eobj_ownname, &eo_errman_DescrWrongParamLocal);
         
        s_eo_armenv_prepare(modinfo, brdinfo);
        
        // i initialise the base environment
        s_the_armenv.env = eov_env_hid_Initialise(s_eo_armenv_shareddata_sync, s_eo_armenv_code_proc_offset_get,
                                                                               s_eo_armenv_shareddata_ipnet_get,
                                                                               s_eo_armenv_shareddata_cannets_get,
                                                                               s_eo_armenv_eproc_get);
        
        
//         #warning --> acemor says: we make sure that the oui is not the infamous 0x112233 but ...
//         
//         
//         shalinfo_deviceinfo_t deviceinfo;
//         const uint64_t thenewoui = 0x0B9A70; //0xabcdef; 0x332211; 
//         
//         if(ee_res_OK != shalinfo_deviceinfo_get(&s_the_armenv.devinfo))
//         {
//             eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "cannot read deviceinfo", s_eobj_ownname, NULL);
//             //return(eores_NOK_generic);
//         }
//         
//         if(thenewoui != (0x0000000000ffffff & s_the_armenv.devinfo->ipnetwork.macaddress))
//         {        
//             memcpy(&deviceinfo, s_the_armenv.devinfo, sizeof(shalinfo_deviceinfo_t));
//             
//             deviceinfo.ipnetwork.macaddress = 0x0000FFFFFF000000 & s_the_armenv.devinfo->ipnetwork.macaddress;
//             deviceinfo.ipnetwork.macaddress |= thenewoui;
//             
//             shalinfo_deviceinfo_set(&deviceinfo);           
//         }

        
        
    }
    

    return(&s_the_armenv);
}
 
    
extern EOtheARMenvironment* eo_armenv_GetHandle(void) 
{
    return((NULL != s_the_armenv.env) ? (&s_the_armenv) : (NULL));
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_armenv_prepare(const eEmoduleInfo_t *modinfo, const eEboardInfo_t *brdinfo)
{
    eEprocess_t proc = (eEprocess_t)modinfo->info.entity.signature;

    // eval modinfo
    if((ee_entity_process == modinfo->info.entity.type) && (ee_procNone != proc))
    {
        s_the_armenv.modinfo = modinfo;
    }
    else
    {
         eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "s_eo_armenv_prepare(): wrong modinfo", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
    }
    
    // load brdinfo. it is non-NULL only in the loader
    s_the_armenv.brdinfo = brdinfo;


    s_the_armenv.eprocess = proc;
    
   

    switch(proc)
    {
        case ee_procLoader:
        { 
            s_the_armenv.codeprocoffset = (EENV_MEMMAP_ELOADER_ROMADDR-EENV_ROMSTART);  
        } break; 
        
        case ee_procUpdater:        
        {
            s_the_armenv.codeprocoffset = (EENV_MEMMAP_EUPDATER_ROMADDR-EENV_ROMSTART);    
        } break;
    
        case ee_procApplication:        
        {
            s_the_armenv.codeprocoffset = (EENV_MEMMAP_EAPPLICATION_ROMADDR-EENV_ROMSTART);    
        } break;  

        default:
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "s_eo_armenv_prepare(): unsupported proc", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
        } break;
    
    }
 
// moved the forceoffset EENV_EAPPLICATION_FORCE_CODE_OFFSET_TO_ZERO isnide the eEmemorymap.h  file     
//    // however, if we have the following macro defined we want the e-proc to work at base address
//#ifdef EE_FORCE_CODE_PROC_OFFSET_TO_ZERO
//    s_the_armenv.codeprocoffset = 0;
//#endif

}

static eOresult_t s_eo_armenv_shareddata_sync(EOVtheEnvironmentDerived *p)
{
    // ok, get back my type.
//    EOVtheEnvironment *armenv = (EOVtheEnvironment *)p;
    
//    #warning: we could remove teh check vs argument not NULL, as long as it is called correctly
//    if(NULL == armenv) 
//    {
//        return(eores_NOK_nullpointer);
//    }

    // it is a singleton boys ... thus i dont need using armenv and i can use s_the_armenv
    
    s_eo_armenv_sharedservices_init_sync();
   
    return(eores_OK);
}

static eOresult_t s_eo_armenv_code_proc_offset_get(EOVtheEnvironmentDerived *p, uint32_t *offset)
{
    // ok, get back my type.
//    EOVtheEnvironment *armenv = (EOVtheEnvironment *)p;
    
//    #warning: we could remove teh check vs argument not NULL, as long as it is called correctly
//    if(NULL == armenv) 
//    {
//        return(eores_NOK_nullpointer);
//    }

    // it is a singleton boys ... thus i dont need using armenv s_the_armenv
    
    *offset = s_the_armenv.codeprocoffset;
    
    return(eores_OK);
}

static eOresult_t s_eo_armenv_shareddata_ipnet_get(EOVtheEnvironmentDerived *p, const void **ipnet)
{
    // ok, get back my type.
    //EOVtheEnvironment *armenv = (EOVtheEnvironment *)p;

//    #warning: we could remove teh check vs argument not NULL, as long as it is called correctly
//    if(NULL == armenv) 
//    {
//        return(eores_NOK_nullpointer);
//    }

    // it is a singleton boys ... thus i dont need using armenv s_the_armenv
    
    if(ee_res_OK != ee_sharserv_info_deviceinfo_get(&s_the_armenv.devinfo))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "s_eo_armenv_shareddata_ipnet_get(): cannot read devinfo", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
        return(eores_NOK_generic);
    }
    
    *ipnet = &s_the_armenv.devinfo->ipnetwork;
    
    return(eores_OK);
}

static eOresult_t s_eo_armenv_shareddata_cannets_get(EOVtheEnvironmentDerived *p, const void **cannets, uint8_t *numnets)
{
    // ok, get back my type.
    //EOVtheEnvironment *armenv = (EOVtheEnvironment *)p;
    
//    #warning: we could remove teh check vs argument not NULL, as long as it is called correctly
//    if(NULL == armenv) 
//    {
//        return(eores_NOK_nullpointer);
//    }

    // it is a singleton boys ... thus i dont need using armenv s_the_armenv
    
    if(ee_res_OK != ee_sharserv_info_deviceinfo_get(&s_the_armenv.devinfo))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "s_eo_armenv_shareddata_cannets_get(): cannot read devinfo", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
        return(eores_NOK_generic);
    }
    
    *cannets = &s_the_armenv.devinfo->can1network;
    *numnets = 2;
    
    return(eores_OK);
}

static eOresult_t s_eo_armenv_eproc_get(EOVtheEnvironmentDerived *p, eEprocess_t *eproc)
{
    // ok, get back my type.
    //EOVtheEnvironment *armenv = (EOVtheEnvironment *)p;

//    #warning: we could remove teh check vs argument not NULL, as long as it is called correctly
//    if(NULL == armenv) 
//    {
//        return(eores_NOK_nullpointer);
//    }

    // it is a singleton boys ... thus i dont need using armenv s_the_armenv
       
    *eproc = s_the_armenv.eprocess;
    
    return(eores_OK);
}


static void s_eo_armenv_sharedservices_init_sync(void)
{
    s_eo_armenv_sharedservices_init();
  
    s_eo_armenv_sharedservices_synchronise();      
}


static void s_eo_armenv_sharedservices_synchronise(void)
{
//    eEresult_t res;
    eEprocess_t proc = (eEprocess_t)s_the_armenv.modinfo->info.entity.signature;
    
    if(ee_res_OK != ee_sharserv_part_proc_synchronise(proc, s_the_armenv.modinfo))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "s_eo_armenv_sharedservices_synchronise(): cannot sync modinfo", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
    }
    
    // only the e-loader has a non-null brdinfo
    if(NULL != s_the_armenv.brdinfo)
    {
        eEboardInfo_t boardinfo;
        memcpy(&boardinfo, s_the_armenv.brdinfo, sizeof(eEboardInfo_t));
        boardinfo.uniqueid = hal_uniqueid_id64bit_get();
        if(ee_res_OK != ee_sharserv_info_boardinfo_synchronise(&boardinfo))
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "s_eo_armenv_sharedservices_synchronise(): cannot sync brdinfo", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
        }
    }
    
    if(ee_res_OK != ee_sharserv_info_deviceinfo_get(&s_the_armenv.devinfo))
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "s_eo_armenv_sharedservices_synchronise(): cannot read devinfo", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
    }
 
}


static void s_eo_armenv_sharedservices_init(void)
{
//     eEresult_t res = ee_res_OK;
    
    if(ee_res_OK == ee_sharserv_isvalid())
    {
        // init sharserv
        sharserv_mode_t sharservmode = 
        {
            .onerror    = NULL,
            .initmode   = sharserv_base_initmode_forcestorageinit
        };
        
        if(ee_res_OK != ee_sharserv_init(&sharservmode))
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "s_eo_armenv_sharedservices_init(): cannot init sharserv", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);
        }        
    } 
    else
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "s_eo_armenv_sharedservices_init(): sharserv is not valid", s_eobj_ownname, &eo_errman_DescrRuntimeErrorLocal);    
    }
    
    
//     if((ee_res_OK == shalbase_isvalid()))
//     {
//         if(ee_res_OK != shalbase_init(shalbase_initmode_forcestorageinit))
//         {
//             eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "cannot init shalbase", s_eobj_ownname, NULL);
//         }
//     }
//     else
//     {
//         eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "shalbase is not present", s_eobj_ownname, NULL);
//     }
//     

//     if((ee_res_OK == shalpart_isvalid()))
//     {
//         if(ee_res_OK != shalpart_init())
//         {
//             eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "cannot init shalpart", s_eobj_ownname, NULL);
//         }
//     }
//     else
//     {
//         eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "shalpart is not present", s_eobj_ownname, NULL);
//     }    


//     if((ee_res_OK == shalinfo_isvalid()))
//     {
//         if(ee_res_OK != shalinfo_init())
//         {
//             eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "cannot init shalinfo", s_eobj_ownname, NULL);
//         }
//     }
//     else
//     {
//         eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "shalinfo is not present", s_eobj_ownname, NULL);
//     }        

}





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





