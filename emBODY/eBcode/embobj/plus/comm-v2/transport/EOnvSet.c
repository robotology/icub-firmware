/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"

#include "EOnv_hid.h" 

#include "EoProtocol.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOnvSet.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOnvSet_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#undef EO_NVSET_INIT_EVERY_NV

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_nvset_PushBackDEVholder(EOnvSet* p, eOnvsetOwnership_t ownership, eOipv4addr_t ipaddress, eOnvBRD_t boardnum, eOuint16_fp_uint8_t fptr_ep2index, uint16_t nendpoints);
static eOresult_t s_eo_nvset_onDEV_PushBackEP(EOnvSet* p, uint16_t ondevindex, eOnvset_EPcfg_t *cfgofep, eOnvBRD_t boardnum);


static EOVmutexDerived* s_eo_nvset_get_nvmutex(EOnvSet* p, eOnvset_dev_t* thedevice, eOnvset_ep_t* theendpoint, eOnvID32_t id32);
static eOresult_t s_eo_nvset_hid_get_device(EOnvSet* p, eOipv4addr_t ip, eOnvset_dev_t** thedevice);
static eOresult_t s_eo_nvset_hid_get_device_endpoint_faster(EOnvSet* p, eOipv4addr_t ip, eOnvEP8_t ep8, eOnvset_dev_t** thedevice, eOnvset_ep_t** theendpoint);

static void s_eo_nvset_devicesowneship_change(EOnvSet *p, eOnvsetOwnership_t ownership);


extern uint16_t eo_nvset_hid_ip2index(EOnvSet* p, eOipv4addr_t ip);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOnvSet";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

 
extern EOnvSet* eo_nvset_New(uint16_t ndevices, eOnvset_protection_t prot, eov_mutex_fn_mutexderived_new mtxnew)
{
    EOnvSet *p = NULL;  

    eo_errman_Assert(eo_errman_GetHandle(), (0 != ndevices), s_eobj_ownname, "ndevices is zero");    

    // i get the memory for the object
    p = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOnvSet), 1);
    
    p->thedevices               = eo_vector_New(sizeof(eOnvset_dev_t*), ndevices, NULL, 0, NULL, NULL);
    p->theipaddresses           = eo_vector_New(sizeof(eOipv4addr_t), ndevices, NULL, 0, NULL, NULL);
    p->devindexoflocaldevice    = EOK_uint16dummy;
    p->devicesowneship          = eo_nvset_devicesownership_none;
    p->mtxderived_new           = mtxnew; 
    p->protection               = (NULL == mtxnew) ? (eo_nvset_protection_none) : (prot); 
    p->mtx_object               = (eo_nvset_protection_one_per_object == p->protection) ? (p->mtxderived_new()) : (NULL);

    return(p);
}



extern eOresult_t eo_nvset_DEVpushback(EOnvSet* p, uint16_t ondevindex, eOnvset_DEVcfg_t* cfgofdev, eOnvsetOwnership_t ownership, eOipv4addr_t ipaddress)
{
    uint16_t i = 0;
    uint16_t nendpoints = eo_constvector_Size(cfgofdev->vectorof_epcfg);
    s_eo_nvset_PushBackDEVholder(p, ownership, ipaddress, cfgofdev->boardnum, cfgofdev->fptr_ep2indexofepcfg, nendpoints);
    
    if(NULL != cfgofdev->fptr_device_initialise)
    {
        eObool_t local = (eo_nvset_ownership_local == ownership) ? (eobool_true) : (eobool_false);
        cfgofdev->fptr_device_initialise(local);
    }
    
    for(i=0; i<nendpoints; i++)
    {
        eOnvset_EPcfg_t* pepcfg = (eOnvset_EPcfg_t*) eo_constvector_At(cfgofdev->vectorof_epcfg, i);
        s_eo_nvset_onDEV_PushBackEP(p, ondevindex, pepcfg, cfgofdev->boardnum);        
    }
    
    return(eores_OK); 
}

static eOresult_t s_eo_nvset_PushBackDEVholder(EOnvSet* p, eOnvsetOwnership_t ownership, eOipv4addr_t ipaddress, eOnvBRD_t boardnum, eOuint16_fp_uint8_t fptr_ep2index, uint16_t nendpoints)
{
    eOnvset_dev_t *dev = NULL;
 	if(NULL == p) 
	{
		return(eores_NOK_nullpointer); 
	}
    
    eo_errman_Assert(eo_errman_GetHandle(), (0 != nendpoints), s_eobj_ownname, "nendpoints is zero");
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != fptr_ep2index), s_eobj_ownname, "fptr_ep2index() is NULL");
    
    eo_errman_Assert(eo_errman_GetHandle(), (eobool_false == eo_vector_Full(p->thedevices)), s_eobj_ownname, "->thedevices is full");

    eo_errman_Assert(eo_errman_GetHandle(), (eobool_true != eo_vector_Find(p->theipaddresses, &ipaddress, NULL)), s_eobj_ownname, "ip already inside");

    s_eo_nvset_devicesowneship_change(p, ownership);

    if(eo_nvset_ownership_local == ownership)
    {
        p->devindexoflocaldevice   = eo_vector_Size(p->thedevices);
    }
    
    dev = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOnvset_dev_t), 1);
    
    dev->ipaddress              = ipaddress;
    dev->boardnum               = boardnum;
    dev->ownership              = ownership;
    dev->theendpoints_numberof  = nendpoints;
    dev->theendpoints           = eo_vector_New(sizeof(eOnvset_ep_t*), nendpoints, NULL, 0, NULL, NULL);    
    dev->fptr_ep2index          = fptr_ep2index;
    dev->mtx_device             = (eo_nvset_protection_one_per_device == p->protection) ? p->mtxderived_new() : NULL;

    eo_vector_PushBack(p->thedevices, &dev);
    eo_vector_PushBack(p->theipaddresses, &ipaddress);

    return(eores_OK);
}


static eOresult_t s_eo_nvset_onDEV_PushBackEP(EOnvSet* p, uint16_t ondevindex, eOnvset_EPcfg_t *cfgofep, eOnvBRD_t boardnum)
{
    eOnvset_dev_t** thedev = NULL;
    eOnvset_ep_t *theendpoint = NULL;
    uint16_t epnvsnumberof = 0;
   
 
    if((NULL == p) || (NULL == cfgofep)) 
    {
        return(eores_NOK_nullpointer); 
    }
    
    epnvsnumberof = cfgofep->protif->getvarsnumberof(boardnum, cfgofep->endpoint);
    
    eo_errman_Assert(eo_errman_GetHandle(), (0 != epnvsnumberof), s_eobj_ownname, "epnvsnumberof is zero"); 
    
    // ok: i go on
        
    thedev = (eOnvset_dev_t**) eo_vector_At(p->thedevices, ondevindex);
    
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != thedev), s_eobj_ownname, "->thedevices is indexed in wrong pos");    
    eo_errman_Assert(eo_errman_GetHandle(), (eobool_false == eo_vector_Full((*thedev)->theendpoints)), s_eobj_ownname, "one of ->theendpoints is full");
     
    theendpoint = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOnvset_ep_t), 1);
    
    memcpy(&theendpoint->epcfg, cfgofep, sizeof(eOnvset_EPcfg_t));   
    theendpoint->epnvsnumberof      = epnvsnumberof;
    theendpoint->initted            = eobool_false;    
    theendpoint->epram              = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, theendpoint->epcfg.epram_sizeof, 1);
    theendpoint->mtx_endpoint       = (eo_nvset_protection_one_per_endpoint == p->protection) ? p->mtxderived_new() : NULL;
        
    // now we must load the ram in the endpoint
    eOres_fp_uint8_uint8_voidp_uint16_t fptr_loadram = theendpoint->epcfg.protif->loadram;
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != fptr_loadram), s_eobj_ownname, "fptr_loadram is NULL"); 
    fptr_loadram((*thedev)->boardnum, theendpoint->epcfg.endpoint, theendpoint->epram, theendpoint->epcfg.epram_sizeof);
    
    // now add the vector of mtx if needed.
    if(eo_nvset_protection_one_per_netvar == p->protection)
    {
        uint16_t i;
        theendpoint->themtxofthenvs = eo_vector_New(sizeof(EOVmutexDerived*), epnvsnumberof, NULL, 0, NULL, NULL);
        for(i=0; i<epnvsnumberof; i++)
        {
            EOVmutexDerived* mtx = p->mtxderived_new();
            eo_vector_PushBack(theendpoint->themtxofthenvs, &mtx);           
        }
    }
   
    eo_vector_PushBack((*thedev)->theendpoints, &theendpoint);

    return(eores_OK);
}



extern eOresult_t eo_nvset_NVSinitialise(EOnvSet* p)
{
    eOnvset_dev_t** theDevice = NULL;
    eOnvset_ep_t** theEndpoint = NULL;
    EOVmutexDerived* mtx2use = NULL;
    
    uint16_t i, j;
    uint16_t ndev;
    uint16_t nendpoints;
    eOnvEP8_t ep08 = EOK_uint08dummy;
    eOvoid_fp_uint32_voidp_t initialise = NULL;


 	if(NULL == p) 
	{
		return(eores_NOK_nullpointer); 
	}

    ndev = eo_vector_Size(p->thedevices);
    mtx2use = (eo_nvset_protection_one_per_object == p->protection) ? (p->mtx_object) : (NULL);


    for(i=0; i<ndev; i++)
    {
        theDevice = (eOnvset_dev_t**) eo_vector_At(p->thedevices, i);
        nendpoints = eo_vector_Size((*theDevice)->theendpoints);
        
        mtx2use = (eo_nvset_protection_one_per_device == p->protection) ? ((*theDevice)->mtx_device) : (mtx2use);


        for(j=0; j<nendpoints; j++)
        {
            theEndpoint = (eOnvset_ep_t**) eo_vector_At((*theDevice)->theendpoints, j);
            
            ep08 = (*theEndpoint)->epcfg.endpoint;
            
            mtx2use = (eo_nvset_protection_one_per_endpoint == p->protection) ? ((*theEndpoint)->mtx_endpoint) : (mtx2use);                   

            if(eobool_false == ((*theEndpoint)->initted))
            {
                initialise  = (*theEndpoint)->epcfg.fptr_ram_initialise;
                (*theEndpoint)->initted = eobool_true;
                
                if(NULL != initialise)
                {
                    initialise((*theDevice)->ipaddress, (*theEndpoint)->epram);
                }
            }
#define EO_NVSET_INIT_EVERY_NV
#if defined(EO_NVSET_INIT_EVERY_NV)
            
            EOnv thenv;
            uint16_t k = 0;
            EOnv_rom_t* rom = NULL;
            uint16_t nvars = (*theEndpoint)->epnvsnumberof;
            eOipv4addr_t ip = (*theDevice)->ipaddress;
            uint8_t brd =  (*theDevice)->boardnum;
            eOnvID32_t id32 = EOK_uint32dummy;     
            uint32_t prog = 0;
            eObool_t cached = eobool_false;

            for(k=0; k<nvars; k++)
            {

                if(eo_nvset_protection_one_per_netvar == p->protection)
                {
                    uint32_t** addr = eo_vector_At((*theEndpoint)->themtxofthenvs, k);
                    mtx2use = (EOVmutexDerived*) (*addr);
                }
                
                prog = k;
                // - 0. the id32 
                id32 = (*theEndpoint)->epcfg.protif->getid(brd, ep08, prog);    
                if(EOK_uint32dummy == id32)
                {
                    continue;
                }

                // - 0+. cached?
                cached = (*theEndpoint)->epcfg.protif->isvarcached(brd, id32);               
                // - 1. the rom
                rom = (EOnv_rom_t*) (*theEndpoint)->epcfg.protif->getrom(brd, id32);
                // - 2. the ram
                uint8_t* ram = (*theEndpoint)->epcfg.protif->getram(brd, id32);
                // - 3. the mtx
                mtx2use = s_eo_nvset_get_nvmutex(p, (*theDevice), (*theEndpoint), id32);

                // - load everything into the nv
                eo_nv_hid_Load(     &thenv,
                                    ip, //(*theDevice)->ipaddress,
                                    brd,
                                    cached,
                                    id32,
                                    rom,
                                    ram,
                                    mtx2use
                              );                    
                
             
                eo_nv_Init(&thenv);                             
            }
#endif //EO_NVSET_INIT_EVERY_NV                   
        }
            
    }

    return(eores_OK);
}


extern void* eo_nvset_RAMofEndpoint_Get(EOnvSet* p, eOipv4addr_t ip, eOnvEP8_t ep8)
{
    eOnvset_dev_t* theDevice = NULL;
    eOnvset_ep_t* theEndpoint = NULL;
 
    if((NULL == p)) 
    {
        return(NULL); 
    }

    // - retrieve the device and the endpoint
    if(eores_OK != s_eo_nvset_hid_get_device_endpoint_faster(p, ip, ep8, &theDevice, &theEndpoint))   
    {
        return(NULL);
    }
    
    return(theEndpoint->epram); 
}

extern void* eo_nvset_RAMofEntity_Get(EOnvSet* p, eOipv4addr_t ip, eOnvEP8_t ep8, eOnvENT_t ent, uint8_t index)
{
    eOnvset_dev_t* theDevice = NULL;
    eOnvset_ep_t* theEndpoint = NULL;
 
    if((NULL == p)) 
    {
        return(NULL); 
    }

    // - retrieve the device and the endpoint
    if(eores_OK != s_eo_nvset_hid_get_device_endpoint_faster(p, ip, ep8, &theDevice, &theEndpoint))   
    {
        return(NULL);
    }
    

//    return(eoprot_ep_entity_ram_extract(theDevice->boardnum, ep8, ent, theEndpoint->epram));
    
    return(eoprot_entity_ramof_get(theDevice->boardnum, ep8, ent, index));

}


extern void* eo_nvset_RAMofVariable_Get(EOnvSet* p, eOipv4addr_t ip, eOnvID32_t id32)
{
    eOnvset_dev_t* theDevice = NULL;
    eOnvset_ep_t* theEndpoint = NULL;
 
    if((NULL == p)) 
    {
        return(NULL); 
    }

    // - retrieve the device and the endpoint
    if(eores_OK != s_eo_nvset_hid_get_device_endpoint_faster(p, ip, eoprot_ID2endpoint(id32), &theDevice, &theEndpoint))   
    {
        return(NULL);
    }
      
    return(eoprot_variable_ramof_get(theDevice->boardnum, id32));
}

extern eOresult_t eo_nvset_BRD_Get(EOnvSet* p, eOipv4addr_t ip, eOnvBRD_t* brd)
{
    eOnvset_dev_t* theDevice = NULL;
 
    if((NULL == p) || (NULL == brd)) 
    {
        return(eores_NOK_nullpointer); 
    }

    // - retrieve the device. if the ip is not recognised, then ... eores_NOK_generic
    if(eores_OK != s_eo_nvset_hid_get_device(p, ip, &theDevice))   
    {
        return(eores_NOK_generic);
    }

    *brd = theDevice->boardnum;    

    return(eores_OK);    
}


extern eOresult_t eo_nvset_NV_Get(EOnvSet* p, eOipv4addr_t ip, eOnvID32_t id32, EOnv* thenv)
{
    eOnvset_dev_t* theDevice = NULL;
    eOnvset_ep_t* theEndpoint = NULL;
    eOnvEP8_t ep8 = eo_nv_hid_id32_extract_ep8(id32); 

 
    if((NULL == p) || (NULL == thenv)) 
    {
        return(eores_NOK_nullpointer); 
    }

    // - retrieve the device and the endpoint. if the ep8 is not recognised, then ... eores_NOK_generic
    if(eores_OK != s_eo_nvset_hid_get_device_endpoint_faster(p, ip, ep8, &theDevice, &theEndpoint))   
    {
        return(eores_NOK_generic);
    }

    uint8_t brd = theDevice->boardnum;
    
    // - verify that on the given endpoint there is a valid id32. if the id32 is not recognised, then ... eores_NOK_generic
    if(eobool_false ==  theEndpoint->epcfg.protif->isidsupported(brd, id32))
    {
        return(eores_NOK_generic);       
    }
    
    // - retrieve from the device and endpoint what is required to form the netvar: con, ram, mtx, etc.   
    // - 0+. cached?
    eObool_t cached = theEndpoint->epcfg.protif->isvarcached(brd, id32);     
    // - 1. the rom
    EOnv_rom_t* rom = (EOnv_rom_t*) theEndpoint->epcfg.protif->getrom(brd, id32);
    // - 2. the ram
    uint8_t* ram = theEndpoint->epcfg.protif->getram(brd, id32);
    // - 3. the mtx
    EOVmutexDerived* mtx2use = s_eo_nvset_get_nvmutex(p, theDevice, theEndpoint, id32);
    
    
    // - final control about the validity of id32. it may be redundant but it is safer. for instance if the fptr_isepidsupported()
    //   does not take into account a removed tag and just checks that the tag-number is lower than the max allowed.
    
    if((NULL == rom) || (NULL == ram))  // mtx2use can be NULL
    {
        return(eores_NOK_generic); 
    }
    

    // - load everything into the nv
    eo_nv_hid_Load(     thenv,
                        ip, //(*theDevice)->ipaddress,
                        brd,
                        id32,
                        cached,    
                        rom,
                        ram,
                        mtx2use
                  );    

    return(eores_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern eOnvPROGnum_t eo_nvset_hid_NVprogressivenumber(EOnvSet* p, eOipv4addr_t ip, eOnvID32_t id32)
{
    eOnvset_dev_t* theDevice = NULL;
    eOnvset_ep_t* theEndpoint = NULL;
    
    eOnvEP8_t ep8 = eo_nv_hid_id32_extract_ep8(id32);
 
    if(NULL == p) 
    {
        return(EOK_uint32dummy); 
    }

    // - retrieve the device and the endpoint
    if(eores_OK != s_eo_nvset_hid_get_device_endpoint_faster(p, ip, ep8, &theDevice, &theEndpoint))   
    {
        return(EOK_uint32dummy);
    }
    
    if(NULL == theEndpoint->epcfg.protif->getprognumber)
    {
        return(EOK_uint32dummy);
    }
    
    return(theEndpoint->epcfg.protif->getprognumber(theDevice->boardnum, id32));
}

extern uint16_t eo_nvset_hid_ip2index(EOnvSet* p, eOipv4addr_t ip)
{
    uint16_t index = 0;
    if(NULL == p)
    {
        return(EOK_uint16dummy);
    }
    if(eobool_true == eo_vector_Find(p->theipaddresses, &ip, &index))
    {
        return(index);  
    }
    return(EOK_uint16dummy);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static EOVmutexDerived* s_eo_nvset_get_nvmutex(EOnvSet* p, eOnvset_dev_t* thedevice, eOnvset_ep_t* theendpoint, eOnvID32_t id32)
{
    EOVmutexDerived* mtx2use = NULL;
    
    if(eo_nvset_protection_none == p->protection)
    {
        mtx2use = NULL;
    }
    else
    {
        switch(p->protection)
        {
            case eo_nvset_protection_one_per_object:
            {
                mtx2use = p->mtx_object;
            } break;
            
            case eo_nvset_protection_one_per_device:
            {
                mtx2use = thedevice->mtx_device;
            } break;            
       
            case eo_nvset_protection_one_per_endpoint:
            {
                mtx2use = theendpoint->mtx_endpoint;
            } break;  
            
            case eo_nvset_protection_one_per_netvar:
            {
                //#warning .... i think of void* as a uint32_t*
                uint32_t nvprognumber = theendpoint->epcfg.protif->getprognumber(thedevice->boardnum, id32);
                uint32_t** addr = eo_vector_At(theendpoint->themtxofthenvs, nvprognumber);
                mtx2use = (EOVmutexDerived*) (*addr);
            } break;  
            
            default:
            {
                mtx2use = NULL;
            } break;
            
        }
    }
    
    return(mtx2use);  
}

static eOresult_t s_eo_nvset_hid_get_device(EOnvSet* p, eOipv4addr_t ip, eOnvset_dev_t** thedevice)
{
    // get the device   
    uint16_t ondevindex = (eok_ipv4addr_localhost == ip) ? (p->devindexoflocaldevice) : (eo_nvset_hid_ip2index(p, ip));   
    if(EOK_uint16dummy == ondevindex)
    {
        return(eores_NOK_generic);
    }    
    eOnvset_dev_t** thedev = (eOnvset_dev_t**) eo_vector_At(p->thedevices, ondevindex);
    if(NULL == thedev)
    {
        return(eores_NOK_nullpointer);
    }
 
   
    *thedevice      = *thedev;  

    return(eores_OK);
}

static eOresult_t s_eo_nvset_hid_get_device_endpoint_faster(EOnvSet* p, eOipv4addr_t ip, eOnvEP8_t ep8, eOnvset_dev_t** thedevice, eOnvset_ep_t** theendpoint)
{
    // get the device   
    uint16_t ondevindex = (eok_ipv4addr_localhost == ip) ? (p->devindexoflocaldevice) : (eo_nvset_hid_ip2index(p, ip));   
    if(EOK_uint16dummy == ondevindex)
    {
        return(eores_NOK_generic);
    }    
    eOnvset_dev_t** thedev = (eOnvset_dev_t**) eo_vector_At(p->thedevices, ondevindex);
    if(NULL == thedev)
    {
        return(eores_NOK_nullpointer);
    }
 
    // get the endpoint
    uint16_t onendpointindex = EOK_uint16dummy;
    if((NULL != (*thedev)->fptr_ep2index))
    {
        onendpointindex = (*thedev)->fptr_ep2index(ep8);
    }
    // we dont do a full search because we must rely on the given function !!!
//     // if the fptr_ep2index is not present or if it fails ... we must do a full search
//     if(EOK_uint16dummy == onendpointindex)
//     {   // cannot fill w/ hash --> need to do an exhaustive search.
//         uint16_t j = 0;
//         uint16_t nendpoints = eo_vector_Size((*thedev)->theendpoints);
//         for(j=0; j<nendpoints; j++)
//         {
//             eOnvset_ep_t **tmpendpoint = (eOnvset_ep_t**) eo_vector_At((*thedev)->theendpoints, j);
//     
//             if(ep8 == (*tmpendpoint)->epcfg.endpoint)
//             {
//                 onendpointindex = j;
//                 break;
//             }
//         }
//     }    
    if(EOK_uint16dummy == onendpointindex)
    {
        return(eores_NOK_generic);
    }        
    eOnvset_ep_t** theend = (eOnvset_ep_t**) eo_vector_At((*thedev)->theendpoints, onendpointindex);
    if(NULL == theend)
    {
        return(eores_NOK_nullpointer);
    }    

    
    *thedevice      = *thedev;
    *theendpoint    = *theend;
    

    return(eores_OK);
}


static void s_eo_nvset_devicesowneship_change(EOnvSet *p,  eOnvsetOwnership_t ownership)
{
    switch(p->devicesowneship)
    {
        case eo_nvset_devicesownership_none:
        {
            if(eo_nvset_ownership_local == ownership)
            {
                p->devicesowneship = eo_nvset_devicesownership_onelocal;    
            }
            else
            {
                p->devicesowneship = eo_nvset_devicesownership_someremote;
            }
        } break;

        case eo_nvset_devicesownership_onelocal:
        {
            if(eo_nvset_ownership_local == ownership)
            {
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "at most one local");
            }
            else
            {
                p->devicesowneship = eo_nvset_devicesownership_onelocalsomeremote;
            }
        } break;

        case eo_nvset_devicesownership_onelocalsomeremote:
        {
            if(eo_nvset_ownership_local == ownership)
            {
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "at most one local");
            }
            else
            {
                p->devicesowneship = eo_nvset_devicesownership_onelocalsomeremote;
            }
        } break;

        case eo_nvset_devicesownership_someremote:
        {
            if(eo_nvset_ownership_local == ownership)
            {
                 p->devicesowneship = eo_nvset_devicesownership_onelocalsomeremote;    
            }
            else
            {
                p->devicesowneship = eo_nvset_devicesownership_someremote;
            }
        } break;

        default:
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "verify your code");
        } break;

    }

}



// - not used ------------------------------------------------------------------------------------------------------------

// static eOresult_t s_eo_nvset_hid_get_device_endpoint(EOnvSet* p, eOipv4addr_t ip, eOnvEP_t ep, eOnvset_dev_t** thedevice, eOnvset_ep_t** theendpoint)
// {
//     // look for the indices for ip and ep. they are required to find the suitable eOnvset_dev_t and eOnvset_ep_t
//     uint16_t ondevindex;
//     uint16_t onendpointindex;
//     
//     if(eores_OK != s_eo_nvset_IPEP2indices(p, ip, ep, &ondevindex, &onendpointindex))
//     {
//         return(eores_NOK_generic);
//     }
//        
//     // - retrieve the device and the endpoint
//
//     eOnvset_dev_t** thedev = (eOnvset_dev_t**) eo_vector_At(p->thedevices, ondevindex);
//     if(NULL == thedev)
//     {
//         return(eores_NOK_nullpointer);
//     }
//     eOnvset_ep_t** theend = (eOnvset_ep_t**) eo_vector_At((*thedev)->theendpoints, onendpointindex);
//     if(NULL == theend)
//     {
//         return(eores_NOK_nullpointer);
//     }
//     
//     *thedevice      = *thedev;
//     *theendpoint    = *theend;
//     
//
//     return(eores_OK);
// }


// static eOresult_t s_eo_nvset_IPEP2indices(EOnvSet* p, eOipv4addr_t ip, eOnvEP_t ep, uint16_t* ondevindex, uint16_t* onendpointindex)
// {
//    // --- search for the index of the ip
//
//     if(eok_ipv4addr_localhost == ip)
//     {
//         *ondevindex = p->devindexoflocaldevice;
//     }
//     else
//     {
//         *ondevindex = eo_nvset_hid_ip2index(p, ip);
//     }
//
//     if(EOK_uint16dummy == *ondevindex)
//     {
//         return(eores_NOK_generic);
//     }
//
//     // --- search for the index of the ep
//
//     *onendpointindex = s_eo_nvset_ondevice_endpoint2index(p, *ondevindex, ep);
//     if(EOK_uint16dummy == *onendpointindex)
//     {
//         return(eores_NOK_generic);
//     }
//        
//     return(eores_OK);
// }

// extern uint16_t eo_nvset_hid_ondevice_endpoint2index(EOnvSet* p, uint16_t ondevindex, eOnvEP_t endpoint);
// extern uint16_t eo_nvset_hid_ondevice_onendpoint_id2progressivenumber(EOnvSet* p, uint16_t ondevindex, uint16_t onendpointindex, eOnvID_t id);

// extern uint16_t eo_nvset_hid_ondevice_endpoint2index(EOnvSet* p, uint16_t ondevindex, eOnvEP_t endpoint)
// {
//     if(NULL == p)
//     {
//         return(EOK_uint16dummy);
//     }

//     return(s_eo_nvset_ondevice_endpoint2index(p, ondevindex, endpoint));
// }


// extern uint16_t eo_nvset_hid_ondevice_onendpoint_id2progressivenumber(EOnvSet* p, uint16_t ondevindex, uint16_t onendpointindex, eOnvID_t id)
// {
//     if(NULL == p)
//     {
//         return(EOK_uint16dummy);
//     }

//     return(s_eo_nvset_ondevice_onendpoint_id2progressivenumber(p, ondevindex, onendpointindex, id));
// }

//static uint16_t s_eo_nvset_ondevice_onendpoint_id2progressivenumber(EOnvSet* p, uint16_t ondevindex, uint16_t onendpointindex, eOnvID_t id);
//static uint16_t s_eo_nvset_ondevice_onendpoint_id2progressivenumber(EOnvSet* p, uint16_t ondevindex, uint16_t onendpointindex, eOnvID_t id)
// {
// #if 1
// 		#warning ------------> write s_eo_nvset_ondevice_onendpoint_id2progressivenumber()
// #else	
//     eOnvset_dev_t** thedev = NULL;
//     eOnvset_ep_t **theendpoint = NULL;

//     thedev = (eOnvset_dev_t**) eo_vector_At(p->thedevices, ondevindex);
//     if(NULL == thedev)
//     {
//         return(EOK_uint08dummy);
//     }
//     theendpoint = (eOnvset_ep_t**) eo_vector_At((*thedev)->theendpoints, onendpointindex);
//     if(NULL == theendpoint)
//     {
//         return(EOK_uint16dummy);
//     }

//  
//     if((NULL != (*theendpoint)->hashfn_id2index))
//     {
//         uint16_t index = (*theendpoint)->hashfn_id2index(id);
//         if(EOK_uint16dummy != index)
//         {
//             return(index);
//         }        
//     }

//     if(1)
//     {   // cannot find with hash function, thus use exhaustive search
//         uint16_t k = 0;
//         uint16_t nvars = (*theendpoint)->epnvs_numberof;
//     
//         for(k=0; k<nvars; k++)
//         {
//             EOnv_con_t* nvcon = (EOnv_con_t*) eo_treenode_GetData((EOtreenode*) eo_constvector_At((*theendpoint)->thetreeofnvs_con, k));
//     
//             if(id == nvcon->id)
//             {
//                 return(k);
//             }
//         }

//     }
// 		
// 		
// #endif
//     return(EOK_uint16dummy);
// }

// static uint16_t s_eo_nvset_ondevice_endpoint2index(EOnvSet* p, uint16_t ondevindex, eOnvEP_t endpoint);
// static uint16_t s_eo_nvset_ondevice_endpoint2index(EOnvSet* p, uint16_t ondevindex, eOnvEP_t endpoint)
// {
//     eOnvset_dev_t** thedev = NULL;

//     thedev = (eOnvset_dev_t**) eo_vector_At(p->thedevices, ondevindex);
//     if(NULL == thedev)
//     {
//         return(EOK_uint16dummy);
//     }

//     if((NULL != (*thedev)->fptr_ep2index))
//     {
//         uint16_t index = (*thedev)->fptr_ep2index(endpoint);
//         if(EOK_uint16dummy != index)
//         {
//             return(index);
//         }        
//     }


//     if(1)
//     {   // cannot fill w/ hash --> need to do an exhaustive search.
//         uint16_t j = 0;
//         uint16_t nendpoints = eo_vector_Size((*thedev)->theendpoints);
//         for(j=0; j<nendpoints; j++)
//         {
//             eOnvset_ep_t **theendpoint = (eOnvset_ep_t**) eo_vector_At((*thedev)->theendpoints, j);
//     
//             if(endpoint == (*theendpoint)->endpoint)
//             {
//                 return(j);
//             }
//         }

//     }

//     return(EOK_uint16dummy);
// }

// extern eOnvsetDevicesOwnership_t eo_nvset_GetDevicesOwnership(EOnvSet* p)
// {
//  	if(NULL == p) 
// 	{
//         return(eo_nvset_devicesownership_none); 
// 	}
//     return(p->devicesowneship);
// }


// extern uint16_t eo_nvset_GetIndexOfLocalDEV(EOnvSet* p)
// {
//  	if(NULL == p) 
// 	{
// 		return(EOK_uint16dummy); 
// 	}
//     return(p->devindexoflocaldevice);
// }

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




