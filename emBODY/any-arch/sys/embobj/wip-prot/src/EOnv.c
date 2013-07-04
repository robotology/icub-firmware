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

#include "EOVmutex.h"


#include "EOrop.h" 
#include "EOarray.h" 





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOnv.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOnv_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#if defined(EO_TAILOR_CODE_FOR_ARM)
    #define EONV_DONT_USE_EOV_MUTEX_FUNCTIONS
#endif


#if defined(EONV_DONT_USE_EOV_MUTEX_FUNCTIONS)
    #define eov_mutex_Take(a, b)   
    #define eov_mutex_Release(a)
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_nv_SetROP(const EOnv *nv, const void *dat, void *dst, eOnvUpdate_t upd, const eOropdescriptor_t *ropdes);
static eOresult_t s_eo_nv_Set(const EOnv *nv, const void *dat, void *dst, eOnvUpdate_t upd);



EO_static_inline uint16_t s_eo_nv_get_size2(const EOnv *nv)
{
#define USECAPACITYFORARRAY

#if     defined(USECAPACITYFORARRAY)
    return(nv->rom->capacity);
#else
    eObool_t typisarray = (eo_nv_TYP_arr == EO_nv_TYP(nv->rom->funtyp)) ? (eobool_true) : (eobool_false);
    void* dat = nv->ram;
    if(eobool_false == typisarray)
    {
        return(nv->rom->capacity);
    }
    else
    {
        // 4 bytes are for the capacity and the size fields, whcih are always present. head->size are the othres
        return(eo_array_UsedBytes((EOarray*)dat));
    }        
#endif      
    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOnv";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOnv * eo_nv_New(void)
{
    EOnv *retptr = NULL;    

    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOnv), 1);

    eo_nv_Clear(retptr);
      
    return(retptr);
}


extern eOresult_t eo_nv_Clear(EOnv *nv)
{
    if(NULL == nv)
    {
        return(eores_NOK_nullpointer);
    }
    
    nv->ip          = eo_nv_IPdummy;
    nv->ep          = eo_nv_EPdummy;
    nv->id          = eo_nv_IDdummy;
    nv->rom         = NULL;       
    nv->ram         = NULL;  
    nv->mtx         = NULL;
      
    return(eores_OK);
}


extern eOresult_t eo_nv_Set(const EOnv *nv, const void *dat, eObool_t forceset, eOnvUpdate_t upd)
{
    if(NULL == nv)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_true == forceset)
    {
        // ok. go on even if the nv is not writeable
    }
    else if(eobool_false == eo_nv_hid_isWritable(nv))
    {
        return(eores_NOK_generic);
    }
    return(s_eo_nv_Set(nv, dat, nv->ram, upd));
}



extern eOresult_t eo_nv_Reset(const EOnv *nv, eObool_t forcerst, eOnvUpdate_t upd)
{
    if(NULL == nv)
    {
        return(eores_NOK_nullpointer);
    }    
    return(eo_nv_Set(nv, nv->rom->resetval, forcerst, upd));
}


extern uint16_t eo_nv_Capacity(const EOnv *nv)
{
    if(NULL == nv)
    {
        return(0);
    }   
    return(nv->rom->capacity);  
}


extern uint16_t eo_nv_Size(const EOnv *nv)
{
    if(NULL == nv)
    {
        return(0);
    }
    
    return(s_eo_nv_get_size2(nv));
}

// fills data with capacity bytes, ... but if an array typ then size is not capacity ...
extern eOresult_t eo_nv_Get(const EOnv *nv, eOnvStorage_t strg, void *data, uint16_t *size)
{
    eOresult_t res = eores_NOK_generic;
    void *source = NULL;
    
    if((NULL == data) || (NULL == size) || (NULL == nv))
    {
        return(eores_NOK_nullpointer);
    }


    switch(strg)
    {
        case eo_nv_strg_volatile:
        {   // better to protect so that the copy is atomic and not interrupted by other tasks which write 
            source = nv->ram;       
            *size = s_eo_nv_get_size2(nv);  
            eov_mutex_Take(nv->mtx, eok_reltimeINFINITE);
            memcpy(data, source, *size); 
            eov_mutex_Release(nv->mtx);
            res = eores_OK;
        } break;

        case eo_nv_strg_default:
        {   // no need to protect as the data is read only
            source = (void*)nv->rom->resetval;   
            //*size = s_eo_nv_get_size1(nv, source);
            *size = s_eo_nv_get_size2(nv);
            memcpy(data, source, *size);
            res = eores_OK;
        } break;

//         case eo_nv_strg_permanent:
//         {   // protection is done inside eo_nv_hid_GetPERMANENT()
//             //res = eo_nv_hid_GetPERMANENT(nv, data, size);
//         } break;
        default:
        {
            res = eores_NOK_generic;
        } break;
    }
   
    return(res);  
}

extern eOresult_t eo_nv_Init(const EOnv *nv)
{
    eOresult_t res = eores_NOK_generic;
	
    if((NULL == nv) || (NULL == nv->rom))
    {
            return(eores_NOK_nullpointer);
    }
    
    // call the init function if existing
    if(NULL != nv->rom->init)
    {   // protect ...
        eov_mutex_Take(nv->mtx, eok_reltimeINFINITE);
        nv->rom->init(nv);
        eov_mutex_Release(nv->mtx);
        res = eores_OK;
    }

    return(res);
}



extern eOnvID_t eo_nv_GetID(const EOnv *nv)
{
    if(NULL == nv)
    {
        return(eo_nv_IDdummy);
    }
    return(nv->id);
}

extern eOnvEP_t eo_nv_GetEP(const EOnv *nv)
{
    if(NULL == nv)
    {
        return(eo_nv_EPdummy);
    }
    return(nv->ep);
}

extern eOipv4addr_t eo_nv_GetIP(const EOnv *nv)
{
    if(NULL == nv)
    {
        return(eo_nv_IPdummy);
    }
    return(nv->ip);
}



extern eOnvRWmode_t eo_nv_GetRWmode(const EOnv *nv)
{
    if((NULL == nv) || (NULL == nv->rom))
    {
        return(eo_nv_rwmode_NONE);
    }

    return((eOnvRWmode_t)nv->rom->rwmode);    
}

extern eOnvOwnership_t eo_nv_GetOwnership(const EOnv *nv)
{
    return((eobool_true == eo_nv_hid_isLocal(nv)) ? (eo_nv_ownership_local) : (eo_nv_ownership_remote));
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern eOresult_t eo_nv_hid_Load(EOnv *nv, eOipv4addr_t ip, eOnvEP_t ep, eOnvID_t id, EOnv_rom_t* rom, void* ram, EOVmutexDerived* mtx/*, EOVstorageDerived* stg*/)
{
    nv->ip          = ip;
    nv->ep          = ep;
    nv->id          = id;
    nv->rom         = rom;
    nv->ram         = ram; 
    nv->mtx         = mtx;
           
    return(eores_OK);
}

extern void eo_nv_hid_Fast_LocalMemoryGet(EOnv *nv, void* dest)
{
    eov_mutex_Take(nv->mtx, eok_reltimeINFINITE);
    memcpy(dest, nv->ram, nv->rom->capacity);
    eov_mutex_Release(nv->mtx);    
}


extern eObool_t eo_nv_hid_isWritable(const EOnv *nv)
{   
    if((eo_nv_rwmode_RW == nv->rom->rwmode) || (eo_nv_rwmode_WO == nv->rom->rwmode))
    {
        return(eobool_true);
    }
    else
    {
        return(eobool_false);
    }    
}


extern eObool_t eo_nv_hid_isLocal(const EOnv *nv)
{
    return((eok_ipv4addr_localhost == nv->ip) ? (eobool_true) : (eobool_false));
} 



extern eObool_t eo_nv_hid_isUpdateable(const EOnv *nv)
{

    if(eobool_true == eo_nv_hid_isLocal(nv))
    {
        return(eo_nv_hid_isWritable(nv));
    }
    else
    {
        return(eobool_true);
    }
} 



extern eOresult_t eo_nv_hid_ResetROP(const EOnv *nv, eOnvUpdate_t upd, const eOropdescriptor_t *ropdes)
{
    if(NULL == nv)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == eo_nv_hid_isWritable(nv))
    {
        return(eores_NOK_generic);
    }

    return(s_eo_nv_SetROP(nv, nv->rom->resetval, nv->ram, upd, ropdes));		
}

extern eOresult_t eo_nv_hid_SetROP(const EOnv *nv, const void *dat, eOnvUpdate_t upd, const eOropdescriptor_t *ropdes)
{
    if(NULL == nv)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_false == eo_nv_hid_isWritable(nv))
    {
        return(eores_NOK_generic);
    }

    return(s_eo_nv_SetROP(nv, dat, nv->ram, upd, ropdes));
}


extern eOresult_t eo_nv_hid_remoteSetROP(const EOnv *nv, const void *dat, eOnvUpdate_t upd, const eOropdescriptor_t* ropdes)
{
    if((NULL == nv) || (NULL == dat))
    {
        return(eores_NOK_nullpointer);
    }

    return(s_eo_nv_SetROP(nv, dat, nv->ram, upd, ropdes));
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_nv_Set(const EOnv *nv, const void *dat, void *dst, eOnvUpdate_t upd)
{
    return(s_eo_nv_SetROP(nv, dat, dst, upd, NULL));
}



static eOresult_t s_eo_nv_SetROP(const EOnv *nv, const void *dat, void *dst, eOnvUpdate_t upd, const eOropdescriptor_t *ropdes)
{
    uint16_t size = s_eo_nv_get_size2(nv);// s_eo_nv_get_size1(nv, dat);

    eov_mutex_Take(nv->mtx, eok_reltimeINFINITE);
    memcpy(dst, dat, size);
    eov_mutex_Release(nv->mtx);

    // call the update function if necessary
    if(eo_nv_upd_dontdo != upd)
    {
        if((eo_nv_upd_always == upd) || (eobool_true == eo_nv_hid_isUpdateable(nv))) 
        {
            if(NULL != nv->rom->update)
            {
                eov_mutex_Take(nv->mtx, eok_reltimeINFINITE);
                nv->rom->update(nv, ropdes);
                eov_mutex_Release(nv->mtx);
            }
        }
    }

    return(eores_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




