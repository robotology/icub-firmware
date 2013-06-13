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
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrorManager.h"

#include "EOnv_hid.h" 
//#include "EOmatrix3d.h"






// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOnvsCfg.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOnvsCfg_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// #if defined(EO_TAILOR_CODE_FOR_LINUX)
//     // put in here definition of macros which must be used with linux
//     
//     // 1. always use cached nvs, at least until we find why it crashes if not defined
//     #define EO_NVSCFG_USE_CACHED_NVS

// #else
//     // put in here definition of macros which are used in any other environment
//     #undef EO_NVSCFG_USE_CACHED_NVS

// #endif


#define EO_NVSCFG_INIT_EVERY_NV
#undef  EO_NVSCFG_USE_HASHTABLE


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static EOVmutexDerived* s_eo_nvscfg_get_nvmutex(EOnvsCfg* p, EOnvsCfg_device_t* thedevice, EOnvsCfg_ep_t* theendpoint, eOnvID_t id);
static eOresult_t s_eo_nvscfg_hid_get_device_endpoint(EOnvsCfg* p, eOipv4addr_t ip, eOnvEP_t ep, EOnvsCfg_device_t** thedevice, EOnvsCfg_ep_t** theendpoint);
static eOresult_t s_eo_nvscfg_IPEP2indices(EOnvsCfg* p, eOipv4addr_t ip, eOnvEP_t ep, uint16_t* ondevindex, uint16_t* onendpointindex);
static eOresult_t s_eo_nvscfg_hid_get_device_endpoint_faster(EOnvsCfg* p, eOipv4addr_t ip, eOnvEP_t ep, EOnvsCfg_device_t** thedevice, EOnvsCfg_ep_t** theendpoint);

static void s_eo_nvscfg_devicesowneship_change(EOnvsCfg *p,  eOnvscfgOwnership_t ownership);


static uint16_t s_eo_nvscfg_ondevice_endpoint2index(EOnvsCfg* p, uint16_t ondevindex, eOnvEP_t endpoint);
static uint16_t s_eo_nvscfg_ondevice_onendpoint_id2progressivenumber(EOnvsCfg* p, uint16_t ondevindex, uint16_t onendpointindex, eOnvID_t id);

// #if defined(EO_NVSCFG_USE_HASHTABLE)
// static uint16_t s_nvscfg_hashing(uint16_t ep, uint16_t sizeofhashtable);
// #endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static const char s_eobj_ownname[] = "EOnvsCfg";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


 
extern EOnvsCfg* eo_nvscfg_New(uint16_t ndevices, /*EOVstorageDerived* stg,*/  eOnvscfg_protection_t prot, eov_mutex_fn_mutexderived_new mtxnew)
{
    EOnvsCfg *p = NULL;  

    eo_errman_Assert(eo_errman_GetHandle(), (0 != ndevices), s_eobj_ownname, "ndevices is zero");    

    // i get the memory for the object
    p = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOnvsCfg), 1);
    
    p->thedevices           = eo_vector_New(sizeof(EOnvsCfg_device_t*), ndevices, NULL, 0, NULL, NULL);
    p->ip2index             = eo_vector_New(sizeof(eOipv4addr_t), ndevices, NULL, 0, NULL, NULL);
    p->indexoflocaldevice   = EOK_uint16dummy;
    p->devicesowneship      = eo_nvscfg_devicesownership_none;
//    p->storage              = stg;
//    p->allnvs               = NULL;
    p->mtxderived_new       = mtxnew; 
    p->protection           = (NULL == mtxnew) ? (eo_nvscfg_protection_none) : (prot); 
    p->mtx_object           = (eo_nvscfg_protection_one_per_object == p->protection) ? p->mtxderived_new() : NULL;

    return(p);
}

extern eOnvscfgDevicesOwnership_t eo_nvscfg_GetDevicesOwnership(EOnvsCfg* p)
{
 	if(NULL == p) 
	{
		return(eo_nvscfg_devicesownership_none); 
	}

    return(p->devicesowneship);
}


extern uint16_t eo_nvscfg_GetIndexOfLocalDevice(EOnvsCfg* p)
{
 	if(NULL == p) 
	{
		return(EOK_uint16dummy); 
	}

    return(p->indexoflocaldevice);
}

extern eOresult_t eo_nvscfg_PushBackDevice(EOnvsCfg* p, eOnvscfgOwnership_t ownership, eOipv4addr_t ipaddress, eOuint16_fp_uint16_t hashfn_ep2index, uint16_t nendpoints)
{
    EOnvsCfg_device_t *dev = NULL;
 	if(NULL == p) 
	{
		return(eores_NOK_nullpointer); 
	}
    
    eo_errman_Assert(eo_errman_GetHandle(), (0 != nendpoints), s_eobj_ownname, "nendpoints is zero");
    
    eo_errman_Assert(eo_errman_GetHandle(), (eobool_false == eo_vector_Full(p->thedevices)), s_eobj_ownname, "->thedevices is full");

    eo_errman_Assert(eo_errman_GetHandle(), (eobool_true != eo_vector_Find(p->ip2index, &ipaddress, NULL)), s_eobj_ownname, "ip already inside");

    s_eo_nvscfg_devicesowneship_change(p, ownership);

    if(eo_nvscfg_ownership_local == ownership)
    {
        p->indexoflocaldevice   = eo_vector_Size(p->thedevices);
    }
    
    dev = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOnvsCfg_device_t), 1);
    
    dev->ipaddress              = ipaddress;
    dev->theendpoints           = eo_vector_New(sizeof(EOnvsCfg_ep_t*), nendpoints, NULL, 0, NULL, NULL);
    dev->ownership              = ownership;
    dev->theendpoints_numberof  = nendpoints;
    dev->hashfn_ep2index        = hashfn_ep2index;
    dev->mtx_device             = (eo_nvscfg_protection_one_per_device == p->protection) ? p->mtxderived_new() : NULL;
// #if defined(EO_NVSCFG_USE_HASHTABLE)
//     dev->ephashtable            = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(eOnvsCfgEPhash_t), nendpoints);
//     // teh entries of teh hastable are all {0xffff, 0xffff} to tell that they are still invalid.
//     memset(dev->ephashtable, 0xff,  sizeof(eOnvsCfgEPhash_t)*nendpoints); 
// #endif

    eo_vector_PushBack(p->thedevices, &dev);
    eo_vector_PushBack(p->ip2index, &ipaddress);

    return(eores_OK);
}


extern eOresult_t eo_nvscfg_ondevice_PushBackEP(EOnvsCfg* p, uint16_t ondevindex, eOnvscfg_EP_t *cfgofep)
{
    EOnvsCfg_device_t** thedev = NULL;
    EOnvsCfg_ep_t *theendpoint = NULL;
    uint16_t nnvs = 0;
    
    eOnvEP_t endpoint;
    eOuint16_fp_uint16_uint16_t hashfn_epid2nvprogressivenumber = NULL;
    eOvoidp_fp_voidp_uint16_uint16_t hashfn_dataepid2nvram =  NULL;
    eOvoidp_fp_uint16_uint16_t hashfn_epid2nvrom = NULL;
    uint32_t datanvs_size;
    eOvoid_fp_uint16_voidp_voidp_t datanvs_init;
    eOvoid_fp_uint16_voidp_voidp_t datanvs_retrieve;
 
    if((NULL == p) || (NULL == cfgofep)) 
    {
        return(eores_NOK_nullpointer); 
    }
        
    endpoint                        = cfgofep->endpoint;
    datanvs_size                    = cfgofep->sizeofdata;
    hashfn_dataepid2nvram  	        = cfgofep->dataepid2nvram;
    hashfn_epid2nvrom               = cfgofep->epid2nvrom;
    hashfn_epid2nvprogressivenumber = cfgofep->epid2nvprogressivenumber;
    datanvs_init                    = cfgofep->data_init;
    datanvs_retrieve                = cfgofep->data_retrieve;
    nnvs                            = cfgofep->ep2nvsnumberof(cfgofep->endpoint);

    
    eo_errman_Assert(eo_errman_GetHandle(), (0 != nnvs), s_eobj_ownname, "nnvs is zero");
   
    thedev = (EOnvsCfg_device_t**) eo_vector_At(p->thedevices, ondevindex);
    
    eo_errman_Assert(eo_errman_GetHandle(), (NULL != thedev), s_eobj_ownname, "->thedevices is indexed in wrong pos");    
    eo_errman_Assert(eo_errman_GetHandle(), (eobool_false == eo_vector_Full((*thedev)->theendpoints)), s_eobj_ownname, "one of ->theendpoints is full");
     
    theendpoint = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOnvsCfg_ep_t), 1);
    
    theendpoint->endpoint           = endpoint;
    theendpoint->initted            = eobool_false;
    theendpoint->thenvs_numberof    = nnvs;
    theendpoint->thenvs_vol         = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, datanvs_size, 1);
    if(eo_nvscfg_ownership_remote == (*thedev)->ownership)
    {
        theendpoint->thenvs_rem     = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, datanvs_size, 1);    
    }
    else
    {
        theendpoint->thenvs_rem     = NULL;
    }
    theendpoint->thenvs_initialise  		        = datanvs_init;
    theendpoint->thenvs_ramretrieve 		        = datanvs_retrieve;
    theendpoint->thenvs_sizeof      		        = datanvs_size;
    theendpoint->hashfn_epid2nvprogressivenumber    = hashfn_epid2nvprogressivenumber;
    theendpoint->hashfn_epid2nvrom                  = hashfn_epid2nvrom;
    theendpoint->hashfn_dataepid2nvram			    = hashfn_dataepid2nvram;
    theendpoint->mtx_endpoint       		        = (eo_nvscfg_protection_one_per_endpoint == p->protection) ? p->mtxderived_new() : NULL;
    
    // now add the vector of mtx if needed.
    if(eo_nvscfg_protection_one_per_netvar == p->protection)
    {
        uint16_t i;
        theendpoint->themtxofthenvs = eo_vector_New(sizeof(EOVmutexDerived*), nnvs, NULL, 0, NULL, NULL);
        for(i=0; i<nnvs; i++)
        {
            EOVmutexDerived* mtx = p->mtxderived_new();
            eo_vector_PushBack(theendpoint->themtxofthenvs, &mtx);           
        }
    }

// #if defined(EO_NVSCFG_USE_HASHTABLE)
//     {   // ok ... i must hash this endpoint with the used index.
//         eOnvsCfgEPhash_t *hashentry;
//
//         uint16_t hashindex = s_nvscfg_hashing(endpoint, (*thedev)->theendpoints_numberof);
//         if(EOK_uint16dummy != hashindex)
//         {
//             hashentry = &((*thedev)->ephashtable[hashindex]);
//             hashentry->index    = eo_vector_Size((*thedev)->theendpoints);
//             hashentry->ephashed = endpoint;
//         }
//         else
//         {
//             // cannot fill in hashtable --> need to do an exhaustive search.
//         }
//     }
// #endif    
    
    eo_vector_PushBack((*thedev)->theendpoints, &theendpoint);

    return(eores_OK);
}



extern eOresult_t eo_nvscfg_data_Initialise(EOnvsCfg* p)
{
#if 1
		#warning -----> write eo_nvscfg_data_Initialise()
#else	
    EOnvsCfg_device_t** thedev = NULL;
    EOnvsCfg_ep_t **theendpoint = NULL;
    uint16_t i, j, k;
    uint16_t ndev;
    uint16_t nendpoints;
    uint16_t nvars;
    eOvoid_fp_uint16_voidp_voidp_t initialise = NULL;
    eOvoid_fp_uint16_voidp_voidp_t ramretrieve = NULL;
    EOnv tmpnv;
    EOnv_con_t* tmpnvcon = NULL;

    EOVmutexDerived* mtx2use = NULL;

 	if(NULL == p) 
	{
		return(eores_NOK_nullpointer); 
	}

    ndev = eo_vector_Size(p->thedevices);
    
    
    mtx2use = (eo_nvscfg_protection_one_per_object == p->protection) ? (p->mtx_object) : (NULL);



// #if defined(EO_NVSCFG_USE_CACHED_NVS)
//     p->allnvs  = eo_matrix3d_New(sizeof(EOnv), ndev);
// #endif


    for(i=0; i<ndev; i++)
    {
        thedev = (EOnvsCfg_device_t**) eo_vector_At(p->thedevices, i);
        nendpoints = eo_vector_Size((*thedev)->theendpoints);
        
        mtx2use = (eo_nvscfg_protection_one_per_device == p->protection) ? ((*thedev)->mtx_device) : (mtx2use);

// #if defined(EO_NVSCFG_USE_CACHED_NVS)
//         eo_matrix3d_Level1_PushBack(p->allnvs, nendpoints);
// #endif

        for(j=0; j<nendpoints; j++)
        {
            theendpoint = (EOnvsCfg_ep_t**) eo_vector_At((*thedev)->theendpoints, j);
            
            mtx2use = (eo_nvscfg_protection_one_per_endpoint == p->protection) ? ((*theendpoint)->mtx_endpoint) : (mtx2use);                   

            if(eobool_false == ((*theendpoint)->initted))
            {
                initialise  = (*theendpoint)->thenvs_initialise;
                ramretrieve = (*theendpoint)->thenvs_ramretrieve;
                (*theendpoint)->initted = eobool_true;
                
                if(NULL != initialise)
                {
                    initialise((*theendpoint)->endpoint, (*theendpoint)->thenvs_vol, (*theendpoint)->thenvs_rem);
                }
                if(NULL != ramretrieve)
                {
                    ramretrieve((*theendpoint)->endpoint, (*theendpoint)->thenvs_vol, (*theendpoint)->thenvs_rem);
                }
            }

#if defined(EO_NVSCFG_INIT_EVERY_NV) || defined(EO_NVSCFG_USE_CACHED_NVS)

            nvars = (*theendpoint)->thenvs_numberof;

// #if defined(EO_NVSCFG_USE_CACHED_NVS)
//             eo_matrix3d_Level2_PushBack(p->allnvs, i, nvars);
// #endif
            for(k=0; k<nvars; k++)
            {
                uint8_t *u8ptrvol = (uint8_t*) (*theendpoint)->thenvs_vol;
                uint8_t *u8ptrrem = (uint8_t*) (*theendpoint)->thenvs_rem;

                treenode = (EOtreenode*) eo_constvector_At((*theendpoint)->thetreeofnvs_con, k);
                tmpnvcon = (EOnv_con_t*) eo_treenode_GetData(treenode);

                
                if(eo_nvscfg_protection_one_per_netvar == p->protection)
                {
                    uint32_t** addr = eo_vector_At((*theendpoint)->themtxofthenvs, k);
                    mtx2use = (EOVmutexDerived*) (*addr);
                }
                

                eo_nv_hid_Load(     &tmpnv,
                                    treenode,
                                    (*thedev)->ipaddress,
                                    (*theendpoint)->endpoint,
                                    tmpnvcon,
                                    (EOnv_usr_t*) eo_constvector_At((*theendpoint)->thenvs_usr, k),
                                    (void*) (&u8ptrvol[tmpnvcon->offset]),
                                    (eo_nvscfg_ownership_remote == (*thedev)->ownership) ? ( (void*) (&u8ptrrem[tmpnvcon->offset]) ) : (NULL),
                                    //(eo_nvscfg_ownership_remote == (*thedev)->ownership) ? ((void*) ((uint32_t)((*theendpoint)->thenvs_rem) + tmpnvcon->offset)) : (NULL),
                                    mtx2use//, // was : (*theendpoint)->mtx_endpoint,
                                   // p->storage
                              );
                
//                 tmpnv.ep  = (*theendpoint)->endpoint;
//                 tmpnv.con = (EOnv_con_t*) eo_treenode_GetData(treenode);
//                 tmpnv.usr = (EOnv_usr_t*) eo_constvector_At((*theendpoint)->thenvs_usr, k);
//                 tmpnv.loc = (void*) ((uint32_t)((*theendpoint)->thenvs_vol) + tmpnv.con->offset);
//                 if(eo_nvscfg_ownership_remote == (*thedev)->ownership)
//                 {
//                     tmpnv.rem = (void*) ((uint32_t)((*theendpoint)->thenvs_rem) + tmpnv.con->offset);   
//                 }
//                 else
//                 {
//                     tmpnv.rem = NULL;
//                 }
//                 tmpnv.mtx = (*theendpoint)->mtx_endpoint;
//                 tmpnv.stg = p->storage;

#if defined(EO_NVSCFG_INIT_EVERY_NV)
                eo_nv_Init(&tmpnv); 
#endif  
                
// #if defined(EO_NVSCFG_USE_CACHED_NVS)
//                 eo_matrix3d_Level3_PushBack(p->allnvs, i, j, &tmpnv);
// #endif
                             
            }
#endif //EO_NVSCFG_INIT_EVERY_NV  EO_NVSCFG_USE_CACHED_NVS         
              
        }
            
    }

#endif		
    return(eores_OK);
}



extern eOresult_t eo_nvscfg_GetNV1(EOnvsCfg* p, eOipv4addr_t ip, eOnvEP_t ep, eOnvID_t id, EOnv* thenv)
{
    EOnvsCfg_device_t* theDevice = NULL;
    EOnvsCfg_ep_t* theEndpoint = NULL;
    EOnv_rom_t* rom = NULL;
    EOVmutexDerived* mtx2use = NULL;
 
    if((NULL == p) || (NULL == thenv)) 
    {
        return(eores_NOK_nullpointer); 
    }

    // - retrieve the device and the endpoint
    if(eores_OK != s_eo_nvscfg_hid_get_device_endpoint_faster(p, ip, ep, &theDevice, &theEndpoint))   
    {
        return(eores_NOK_generic);
    }

    // - retrieve from the device and endpoint what is required to form the netvar: con, ram, mtx, etc.    
    // - 1. the rom
    rom = (EOnv_rom_t*) theEndpoint->hashfn_epid2nvrom(ep, id);
    // - 2. the ram
    uint8_t* ram = theEndpoint->hashfn_dataepid2nvram(theEndpoint->thenvs_vol, ep, id);
    // - 3. the rem
    uint8_t* rem = (eo_nvscfg_ownership_remote == theDevice->ownership) ? (theEndpoint->hashfn_dataepid2nvram(theEndpoint->thenvs_rem, ep, id)) : (NULL);
    // - 4. the mtx
    mtx2use = s_eo_nvscfg_get_nvmutex(p, theDevice, theEndpoint, id);

    // - load everything into the nv
    eo_nv_hid_Load(     thenv,
                        ip, //(*thedev)->ipaddress,
                        ep, //(*theendpoint)->endpoint,
                        id,
                        rom,
                        ram,
                        rem,
                        mtx2use
                  );    

    return(eores_OK);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



extern uint16_t eo_nvscfg_hid_ip2index(EOnvsCfg* p, eOipv4addr_t ipaddress)
{
    uint16_t index = 0;

    if(NULL == p)
    {
        return(EOK_uint16dummy);
    }

    if(eobool_true == eo_vector_Find(p->ip2index, &ipaddress, &index))
    {
        return(index);  
    }

    return(EOK_uint16dummy);
}

extern uint16_t eo_nvscfg_hid_ondevice_endpoint2index(EOnvsCfg* p, uint16_t ondevindex, eOnvEP_t endpoint)
{
    if(NULL == p)
    {
        return(EOK_uint16dummy);
    }

    return(s_eo_nvscfg_ondevice_endpoint2index(p, ondevindex, endpoint));
}


extern uint16_t eo_nvscfg_hid_ondevice_onendpoint_id2progressivenumber(EOnvsCfg* p, uint16_t ondevindex, uint16_t onendpointindex, eOnvID_t id)
{
    if(NULL == p)
    {
        return(EOK_uint16dummy);
    }

    return(s_eo_nvscfg_ondevice_onendpoint_id2progressivenumber(p, ondevindex, onendpointindex, id));
}


// extern EOtreenode* eo_nvscfg_hid_ondevice_onendpoint_withID_GetTreeNode(EOnvsCfg* p, uint16_t ondevindex, uint16_t onendpointindex, eOnvID_t id)
// {
//     EOnvsCfg_device_t** thedev = NULL;
//     EOnvsCfg_ep_t **theendpoint = NULL;


//     thedev = (EOnvsCfg_device_t**) eo_vector_At(p->thedevices, ondevindex);
//     if(NULL == thedev)
//     {
//         return(NULL);
//     }
//     theendpoint = (EOnvsCfg_ep_t**) eo_vector_At((*thedev)->theendpoints, onendpointindex);
//     if(NULL == theendpoint)
//     {
//         return(NULL);
//     }

//     if((NULL != (*theendpoint)->hashfn_id2index))
//     {
//         uint16_t index = (*theendpoint)->hashfn_id2index(id);
//         if(EOK_uint16dummy != index)
//         {
//             return((EOtreenode*) eo_constvector_At((*theendpoint)->thetreeofnvs_con, index));
//         }        
//     }


//     if(1)
//     {   // cannot find w/ hash function thus i use exhaustive search   
//         EOnv_con_t *nvcon = NULL;
//         uint16_t nvars = 0;
//         uint16_t k = 0; 
//         EOtreenode *treenode = NULL; 

//         nvars = (*theendpoint)->thenvs_numberof;
//         for(k=0; k<nvars; k++)
//         {
//             treenode = (EOtreenode*) eo_constvector_At((*theendpoint)->thetreeofnvs_con, k);
//             nvcon = (EOnv_con_t*) eo_treenode_GetData(treenode);
//     
//             if(id == nvcon->id)
//             {
//                 return(treenode);
//             }
//         }
//     }


//     return(NULL);
// }



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static EOVmutexDerived* s_eo_nvscfg_get_nvmutex(EOnvsCfg* p, EOnvsCfg_device_t* thedevice, EOnvsCfg_ep_t* theendpoint, eOnvID_t id)
{
    EOVmutexDerived* mtx2use = NULL;
    
    if(eo_nvscfg_protection_none == p->protection)
    {
        mtx2use = NULL;
    }
    else
    {
        switch(p->protection)
        {
            case eo_nvscfg_protection_one_per_object:
            {
                mtx2use = p->mtx_object;
            } break;
            
            case eo_nvscfg_protection_one_per_device:
            {
                mtx2use = thedevice->mtx_device;
            } break;            
       
            case eo_nvscfg_protection_one_per_endpoint:
            {
                mtx2use = theendpoint->mtx_endpoint;
            } break;  
            
            case eo_nvscfg_protection_one_per_netvar:
            {
                //#warning .... i think of void* as a uint32_t*
                uint16_t nvprogindex = theendpoint->hashfn_epid2nvprogressivenumber(theendpoint->endpoint, id);
                uint32_t** addr = eo_vector_At(theendpoint->themtxofthenvs, nvprogindex);
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

static eOresult_t s_eo_nvscfg_hid_get_device_endpoint(EOnvsCfg* p, eOipv4addr_t ip, eOnvEP_t ep, EOnvsCfg_device_t** thedevice, EOnvsCfg_ep_t** theendpoint)
{
    // look for the indices for ip and ep. they are required to find the suitable EOnvsCfg_device_t and EOnvsCfg_ep_t
    uint16_t ondevindex;
    uint16_t onendpointindex;
    
    if(eores_OK != s_eo_nvscfg_IPEP2indices(p, ip, ep, &ondevindex, &onendpointindex))
    {
        return(eores_NOK_generic);
    }
       
    // - retrieve the device and the endpoint

    EOnvsCfg_device_t** thedev = (EOnvsCfg_device_t**) eo_vector_At(p->thedevices, ondevindex);
    if(NULL == thedev)
    {
        return(eores_NOK_nullpointer);
    }
    EOnvsCfg_ep_t** theend = (EOnvsCfg_ep_t**) eo_vector_At((*thedev)->theendpoints, onendpointindex);
    if(NULL == theend)
    {
        return(eores_NOK_nullpointer);
    }
    
    *thedevice      = *thedev;
    *theendpoint    = *theend;
    

    return(eores_OK);
}


static eOresult_t s_eo_nvscfg_hid_get_device_endpoint_faster(EOnvsCfg* p, eOipv4addr_t ip, eOnvEP_t ep, EOnvsCfg_device_t** thedevice, EOnvsCfg_ep_t** theendpoint)
{
    // get the device   
    uint16_t ondevindex = (eok_ipv4addr_localhost == ip) ? (p->indexoflocaldevice) : (eo_nvscfg_hid_ip2index(p, ip));   
    if(EOK_uint16dummy == ondevindex)
    {
        return(eores_NOK_generic);
    }    
    EOnvsCfg_device_t** thedev = (EOnvsCfg_device_t**) eo_vector_At(p->thedevices, ondevindex);
    if(NULL == thedev)
    {
        return(eores_NOK_nullpointer);
    }
 
    // get the endpoint
    uint16_t onendpointindex = EOK_uint16dummy;
    if((NULL != (*thedev)->hashfn_ep2index))
    {
        onendpointindex = (*thedev)->hashfn_ep2index(ep);
    }
    // if the hashfn_ep2index is not present or if it fails ... we must do a full search
    if(EOK_uint16dummy == onendpointindex)
    {   // cannot fill w/ hash --> need to do an exhaustive search.
        uint16_t j = 0;
        uint16_t nendpoints = eo_vector_Size((*thedev)->theendpoints);
        for(j=0; j<nendpoints; j++)
        {
            EOnvsCfg_ep_t **tmpendpoint = (EOnvsCfg_ep_t**) eo_vector_At((*thedev)->theendpoints, j);
    
            if(ep == (*tmpendpoint)->endpoint)
            {
                onendpointindex = j;
                break;
            }
        }
    }    
    if(EOK_uint16dummy == onendpointindex)
    {
        return(eores_NOK_generic);
    }        
    EOnvsCfg_ep_t** theend = (EOnvsCfg_ep_t**) eo_vector_At((*thedev)->theendpoints, onendpointindex);
    if(NULL == theend)
    {
        return(eores_NOK_nullpointer);
    }    

    
    *thedevice      = *thedev;
    *theendpoint    = *theend;
    

    return(eores_OK);
}

static eOresult_t s_eo_nvscfg_IPEP2indices(EOnvsCfg* p, eOipv4addr_t ip, eOnvEP_t ep, uint16_t* ondevindex, uint16_t* onendpointindex)
{
   // --- search for the index of the ip

    if(eok_ipv4addr_localhost == ip)
    {
        *ondevindex = p->indexoflocaldevice;
    }
    else
    {
        *ondevindex = eo_nvscfg_hid_ip2index(p, ip);
    }

    if(EOK_uint16dummy == *ondevindex)
    {
        return(eores_NOK_generic);
    }

    // --- search for the index of the ep

    *onendpointindex = s_eo_nvscfg_ondevice_endpoint2index(p, *ondevindex, ep);
    if(EOK_uint16dummy == *onendpointindex)
    {
        return(eores_NOK_generic);
    }
       
    return(eores_OK);
}



static void s_eo_nvscfg_devicesowneship_change(EOnvsCfg *p,  eOnvscfgOwnership_t ownership)
{
    switch(p->devicesowneship)
    {
        case eo_nvscfg_devicesownership_none:
        {
            if(eo_nvscfg_ownership_local == ownership)
            {
                p->devicesowneship = eo_nvscfg_devicesownership_onelocal;    
            }
            else
            {
                p->devicesowneship = eo_nvscfg_devicesownership_someremote;
            }
        } break;

        case eo_nvscfg_devicesownership_onelocal:
        {
            if(eo_nvscfg_ownership_local == ownership)
            {
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "at most one local");
            }
            else
            {
                p->devicesowneship = eo_nvscfg_devicesownership_onelocalsomeremote;
            }
        } break;

        case eo_nvscfg_devicesownership_onelocalsomeremote:
        {
            if(eo_nvscfg_ownership_local == ownership)
            {
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "at most one local");
            }
            else
            {
                p->devicesowneship = eo_nvscfg_devicesownership_onelocalsomeremote;
            }
        } break;

        case eo_nvscfg_devicesownership_someremote:
        {
            if(eo_nvscfg_ownership_local == ownership)
            {
                 p->devicesowneship = eo_nvscfg_devicesownership_onelocalsomeremote;    
            }
            else
            {
                p->devicesowneship = eo_nvscfg_devicesownership_someremote;
            }
        } break;

        default:
        {
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "verify your code");
        } break;

    }

}

// #if defined(EO_NVSCFG_USE_HASHTABLE) 
// static uint16_t s_nvscfg_hashing(uint16_t ep, uint16_t sizeofhashtable)
// {
//     uint16_t ret = (ep < sizeofhashtable) ? (ep) : (EOK_uint16dummy);
//     return(ret);
// }
// #endif

static uint16_t s_eo_nvscfg_ondevice_endpoint2index(EOnvsCfg* p, uint16_t ondevindex, eOnvEP_t endpoint)
{
    EOnvsCfg_device_t** thedev = NULL;

    thedev = (EOnvsCfg_device_t**) eo_vector_At(p->thedevices, ondevindex);
    if(NULL == thedev)
    {
        return(EOK_uint16dummy);
    }

// #if defined(EO_NVSCFG_USE_HASHTABLE)
//     {
//         eOnvsCfgEPhash_t *hashentry;
//         uint16_t hashindex;

//         hashindex = s_nvscfg_hashing(endpoint, (*thedev)->theendpoints_numberof);
//     
//         if((EOK_uint16dummy != hashindex))
//         {
//             hashentry = &((*thedev)->ephashtable[hashindex]);
//             if(endpoint == hashentry->ephashed)
//             {
//                 return(hashentry->index);
//             }
//         }
//     }
// #else

    if((NULL != (*thedev)->hashfn_ep2index))
    {
        uint16_t index = (*thedev)->hashfn_ep2index(endpoint);
        if(EOK_uint16dummy != index)
        {
            return(index);
        }        
    }

//#endif


    if(1)
    {   // cannot fill w/ hash --> need to do an exhaustive search.
        uint16_t j = 0;
        uint16_t nendpoints = eo_vector_Size((*thedev)->theendpoints);
        for(j=0; j<nendpoints; j++)
        {
            EOnvsCfg_ep_t **theendpoint = (EOnvsCfg_ep_t**) eo_vector_At((*thedev)->theendpoints, j);
    
            if(endpoint == (*theendpoint)->endpoint)
            {
                return(j);
            }
        }

    }

    return(EOK_uint16dummy);
}


static uint16_t s_eo_nvscfg_ondevice_onendpoint_id2progressivenumber(EOnvsCfg* p, uint16_t ondevindex, uint16_t onendpointindex, eOnvID_t id)
{
#if 1
		#warning ------------> write s_eo_nvscfg_ondevice_onendpoint_id2progressivenumber()
#else	
    EOnvsCfg_device_t** thedev = NULL;
    EOnvsCfg_ep_t **theendpoint = NULL;

    thedev = (EOnvsCfg_device_t**) eo_vector_At(p->thedevices, ondevindex);
    if(NULL == thedev)
    {
        return(EOK_uint08dummy);
    }
    theendpoint = (EOnvsCfg_ep_t**) eo_vector_At((*thedev)->theendpoints, onendpointindex);
    if(NULL == theendpoint)
    {
        return(EOK_uint16dummy);
    }

 
    if((NULL != (*theendpoint)->hashfn_id2index))
    {
        uint16_t index = (*theendpoint)->hashfn_id2index(id);
        if(EOK_uint16dummy != index)
        {
            return(index);
        }        
    }

    if(1)
    {   // cannot find with hash function, thus use exhaustive search
        uint16_t k = 0;
        uint16_t nvars = (*theendpoint)->thenvs_numberof;
    
        for(k=0; k<nvars; k++)
        {
            EOnv_con_t* nvcon = (EOnv_con_t*) eo_treenode_GetData((EOtreenode*) eo_constvector_At((*theendpoint)->thetreeofnvs_con, k));
    
            if(id == nvcon->id)
            {
                return(k);
            }
        }

    }
		
		
#endif
    return(EOK_uint16dummy);
}


// - oldies -----------------------------------------------------------------------------------------------------------

#if 0
extern void * eo_nvscfg_localdev_endpoint_GetRAM(EOnvsCfg* p, eOnvEP_t endpoint)
{
    uint16_t ondevindex, onendpointindex;
    EOnvsCfg_device_t** thedev = NULL;
    EOnvsCfg_ep_t **theendpoint = NULL;
 

    if((NULL == p) || (EOK_uint16dummy == p->indexoflocaldevice)) 
	{
		return(NULL); 
	}

    ondevindex = p->indexoflocaldevice;

    onendpointindex = eo_nvscfg_hid_ondevice_endpoint2index(p, ondevindex, endpoint);

    if(EOK_uint16dummy == onendpointindex)
    {
        return(NULL);
    }

    thedev = (EOnvsCfg_device_t**) eo_vector_At(p->thedevices, ondevindex);
    theendpoint = (EOnvsCfg_ep_t**) eo_vector_At((*thedev)->theendpoints, onendpointindex);

    return((*theendpoint)->thenvs_vol);
}
#endif

// ACEMOR_STEP1
#if 0
extern eOresult_t eo_nvscfg_GetIndices_nvscfgobsolete(EOnvsCfg* p, 
                                       eOipv4addr_t ip, eOnvEP_t ep, eOnvID_t id, 
                                       uint16_t *ipindex, uint16_t *epindex, uint16_t *idindex)
{
    if((NULL == p) || (NULL == ipindex) || (NULL == epindex) || (NULL == idindex))
    {
        return(eores_NOK_nullpointer);
    }

    // --- search for the index of the ip

    if(eok_ipv4addr_localhost == ip)
    {
        *ipindex = p->indexoflocaldevice;
    }
    else
    {
        *ipindex = eo_nvscfg_hid_ip2index(p, ip);
    }

    if(EOK_uint16dummy == *ipindex)
    {
        return(eores_NOK_generic);
    }

    // --- search for the index of the ep

    *epindex = s_eo_nvscfg_ondevice_endpoint2index(p, *ipindex, ep);
    if(EOK_uint16dummy == *epindex)
    {
        return(eores_NOK_generic);
    }

    // --- search for the index of the id

    *idindex = s_eo_nvscfg_ondevice_onendpoint_id2index(p, *ipindex, *epindex, id);
    if(EOK_uint16dummy == *idindex)
    {
        return(eores_NOK_generic);
    }

    return(eores_OK);
}

#endif

// extern EOtreenode* eo_nvscfg_GetTreeNode(EOnvsCfg* p, uint16_t ondevindex, uint16_t onendpointindex, uint16_t onidindex)
// {
//     EOnvsCfg_device_t** thedev = NULL;
//     EOnvsCfg_ep_t **theendpoint = NULL;


//     thedev = (EOnvsCfg_device_t**) eo_vector_At(p->thedevices, ondevindex);
//     if(NULL == thedev)
//     {
//         return(NULL);
//     }
//     theendpoint = (EOnvsCfg_ep_t**) eo_vector_At((*thedev)->theendpoints, onendpointindex);
//     if(NULL == theendpoint)
//     {
//         return(NULL);
//     }

//     return((EOtreenode*) eo_constvector_At((*theendpoint)->thetreeofnvs_con, onidindex));
// }

#if 0
extern eOresult_t eo_nvscfg_GetNV1(EOnvsCfg* p, eOipv4addr_t ip, eOnvEP_t ep, eOnvID_t id, EOnv* thenv)
{
#if 1
		#warning -----> write eo_nvscfg_GetNV1()
#else	
    uint16_t ondevindex;
    uint16_t onendpointindex;
    uint16_t onidindex;
    
    EOtreenode* treenode = NULL;

    EOnvsCfg_device_t** thedev = NULL;
    EOnvsCfg_ep_t **theendpoint = NULL;
    uint16_t k = 0;
    EOnv_con_t* tmpnvcon = NULL;
    EOVmutexDerived* mtx2use = NULL;
 
    if((NULL == p) || (NULL == thenv)) 
		{
			return(eores_NOK_nullpointer); 
		}
    
    
    // the indices
    
    // --- search for the index of the ip

    if(eok_ipv4addr_localhost == ip)
    {
        ondevindex = p->indexoflocaldevice;
    }
    else
    {
        ondevindex = eo_nvscfg_hid_ip2index(p, ip);
    }

    if(EOK_uint16dummy == ondevindex)
    {
        return(eores_NOK_generic);
    }

    // --- search for the index of the ep

    onendpointindex = s_eo_nvscfg_ondevice_endpoint2index(p, ondevindex, ep);
    if(EOK_uint16dummy == onendpointindex)
    {
        return(eores_NOK_generic);
    }

    // --- search for the index of the id

    onidindex = s_eo_nvscfg_ondevice_onendpoint_id2index(p, ondevindex, onendpointindex, id);
    if(EOK_uint16dummy == onidindex)
    {
        return(eores_NOK_generic);
    }    
     
    
    // the treenode
    
    if(NULL == treenode)
    {
        treenode = eo_nvscfg_GetTreeNode(p, ondevindex, onendpointindex, onidindex);
        if(NULL == treenode)
        {
            return(eores_NOK_nullpointer);
        }
    }

    thedev = (EOnvsCfg_device_t**) eo_vector_At(p->thedevices, ondevindex);
    if(NULL == thedev)
    {
        return(eores_NOK_nullpointer);
    }
    theendpoint = (EOnvsCfg_ep_t**) eo_vector_At((*thedev)->theendpoints, onendpointindex);
    if(NULL == theendpoint)
    {
        return(eores_NOK_nullpointer);
    }

    k = onidindex; // or eo_treenode_GetIndex(treenode);
    
    tmpnvcon = (EOnv_con_t*) eo_treenode_GetData(treenode);
    
    if(eo_nvscfg_protection_none == p->protection)
    {
        mtx2use = NULL;
    }
    else
    {
        switch(p->protection)
        {
            case eo_nvscfg_protection_one_per_object:
            {
                mtx2use = p->mtx_object;
            } break;
            
            case eo_nvscfg_protection_one_per_device:
            {
                mtx2use = (*thedev)->mtx_device;
            } break;            
       
            case eo_nvscfg_protection_one_per_endpoint:
            {
                mtx2use = (*theendpoint)->mtx_endpoint;
            } break;  
            
            case eo_nvscfg_protection_one_per_netvar:
            {
                //#warning .... i think of void* as a uint32_t*
                uint32_t** addr = eo_vector_At((*theendpoint)->themtxofthenvs, k);
                mtx2use = (EOVmutexDerived*) (*addr);
            } break;  
            
            default:
            {
                mtx2use = NULL;
            } break;
            
        }
    }
        
    uint8_t* startofloc = (uint8_t*) (*theendpoint)->thenvs_vol;
    uint8_t* startofrem = (uint8_t*) (*theendpoint)->thenvs_rem;
    uint16_t offset = (*theendpoint)->hashfn_id2dataoffset(id);
    if(EOK_uint16dummy == offset)
    {
        return(eores_NOK_generic);
    }
    uint8_t* locram = &startofloc[offset];
    uint8_t* remram = (eo_nvscfg_ownership_remote == (*thedev)->ownership) ? (&startofrem[offset]) : (NULL);
    
    
    eo_nv_hid_Load(     thenv,
                        treenode,
                        (*thedev)->ipaddress,
                        (*theendpoint)->endpoint,
                        tmpnvcon,
                        (EOnv_usr_t*) eo_constvector_At((*theendpoint)->thenvs_usr, k),
                        locram,
                        remram,
                        mtx2use//, // was: (*theendpoint)->mtx_endpoint,
                        //p->storage
                  );    

//    nv->ep  = (*theendpoint)->endpoint;
//    nv->con = (EOnv_con_t*) eo_treenode_GetData(treenode);
//    nv->usr = (EOnv_usr_t*) eo_constvector_At((*theendpoint)->thenvs_usr, k);
//    nv->loc = (void*) ((uint32_t)((*theendpoint)->thenvs_vol) + nv->con->offset);
//
//     if(eo_nvscfg_ownership_remote == (*thedev)->ownership)
//     {
//         nv->rem = (void*) ((uint32_t)((*theendpoint)->thenvs_rem) + nv->con->offset);   
//     }
//     else
//     {
//         nv->rem = NULL;
//     }

//     nv->mtx = (*theendpoint)->mtx_endpoint;
//     nv->stg = p->storage;


#endif

    return(eores_OK);
}

#endif

//hashfn_id2dataoffset

// ACEMOR_STEP1
#if 0
extern EOnv* eo_nvscfg_GetNV_nvscfgobsolete(EOnvsCfg* p, uint16_t ondevindex, uint16_t onendpointindex, uint16_t onidindex, EOtreenode* treenode, EOnv* nvtarget)
{

#if defined(EO_NVSCFG_USE_CACHED_NVS)

    // --- we use just the indices

    EOnv *nv;

    if(NULL == p) 
	{
		return(NULL); 
	}
    
    nv = (EOnv*) eo_matrix3d_At(p->allnvs, ondevindex, onendpointindex, onidindex);
    
    if((NULL != nv) && (NULL != nvtarget))
    {
        memcpy(nvtarget, nv, sizeof(EOnv));
    }

    return(nv);
    
#else

    // -- we use also the treenode

    EOnv* nv;
    EOnvsCfg_device_t** thedev = NULL;
    EOnvsCfg_ep_t **theendpoint = NULL;
    uint16_t k = 0;
    EOnv_con_t* tmpnvcon = NULL;
    EOVmutexDerived* mtx2use = NULL;
 
    if((NULL == p) || (NULL == nvtarget)) 
	{
		return(NULL); 
	}

    if(NULL == treenode)
    {
        treenode = eo_nvscfg_GetTreeNode(p, ondevindex, onendpointindex, onidindex);
        if(NULL == treenode)
        {
            return(NULL);
        }
    }

    thedev = (EOnvsCfg_device_t**) eo_vector_At(p->thedevices, ondevindex);
    if(NULL == thedev)
    {
        return(NULL);
    }
    theendpoint = (EOnvsCfg_ep_t**) eo_vector_At((*thedev)->theendpoints, onendpointindex);
    if(NULL == theendpoint)
    {
        return(NULL);
    }

    k = onidindex; // or eo_treenode_GetIndex(treenode);
    nv = nvtarget;
    
    tmpnvcon = (EOnv_con_t*) eo_treenode_GetData(treenode);
    
    if(eo_nvscfg_protection_none == p->protection)
    {
        mtx2use = NULL;
    }
    else
    {
        switch(p->protection)
        {
            case eo_nvscfg_protection_one_per_object:
            {
                mtx2use = p->mtx_object;
            } break;
            
            case eo_nvscfg_protection_one_per_device:
            {
                mtx2use = (*thedev)->mtx_device;
            } break;            
       
            case eo_nvscfg_protection_one_per_endpoint:
            {
                mtx2use = (*theendpoint)->mtx_endpoint;
            } break;  
            
            case eo_nvscfg_protection_one_per_netvar:
            {
                //#warning .... i think of void* as a uint32_t*
                uint32_t** addr = eo_vector_At((*theendpoint)->themtxofthenvs, k);
                mtx2use = (EOVmutexDerived*) (*addr);
            } break;  
            
            default:
            {
                mtx2use = NULL;
            } break;
            
        }
    }
        
    
    eo_nv_hid_Load(     nv,
                        treenode,
                        (*thedev)->ipaddress,
                        (*theendpoint)->endpoint,
                        tmpnvcon,
                        (EOnv_usr_t*) eo_constvector_At((*theendpoint)->thenvs_usr, k),
                        (void*) ((uint32_t)((*theendpoint)->thenvs_vol) + tmpnvcon->offset),
                        (eo_nvscfg_ownership_remote == (*thedev)->ownership) ? ((void*) ((uint32_t)((*theendpoint)->thenvs_rem) + tmpnvcon->offset)) : (NULL),
                        mtx2use//, // was: (*theendpoint)->mtx_endpoint,
                        //p->storage
                  );    

//    nv->ep  = (*theendpoint)->endpoint;
//    nv->con = (EOnv_con_t*) eo_treenode_GetData(treenode);
//    nv->usr = (EOnv_usr_t*) eo_constvector_At((*theendpoint)->thenvs_usr, k);
//    nv->loc = (void*) ((uint32_t)((*theendpoint)->thenvs_vol) + nv->con->offset);
//
//     if(eo_nvscfg_ownership_remote == (*thedev)->ownership)
//     {
//         nv->rem = (void*) ((uint32_t)((*theendpoint)->thenvs_rem) + nv->con->offset);   
//     }
//     else
//     {
//         nv->rem = NULL;
//     }

//     nv->mtx = (*theendpoint)->mtx_endpoint;
//     nv->stg = p->storage;


    return(nv);

#endif

}
#endif

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




