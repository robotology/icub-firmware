
/* @file       EOnetvar.c
    @brief      This file implements internal implementation of a netvar object.
    @author     marco.accame@iit.it
    @date       09/03/2011
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"
#include "EOtheMemoryPool.h"

#include "EOtheNVs.h"

#include "EOrop.h" 





// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOnetvar.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOnetvar_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_netvar_Set(const EOnetvar *netvar, const void *dat, void *dst, eOnetvarUpdate_t upd);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//static const char s_eobj_ownname[] = "EOnetvar";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#warning -> document all the extern eo_netvar_* and verify if they all are needed

extern eOnetvarFunc_t eo_netvar_fromIDtoFUN(eOnetvarID_t id)
{
    return((eOnetvarFunc_t)eo_nv_getFUN(id));
}  

extern eOnetvarType_t eo_netvar_fromIDtoTYP(eOnetvarID_t id)
{
    return((eOnetvarType_t)eo_nv_getTYP(id));
} 

extern uint16_t eo_netvar_fromIDtoOFF(eOnetvarID_t id)
{
    return(eo_nv_getOFF(id));
}

extern eOresult_t eo_netvar_Set(const EOnetvar *netvar, const void *dat, eObool_t forceset, eOnetvarUpdate_t upd)
{
    if(NULL == netvar)
    {
        return(eores_NOK_nullpointer);
    }


    if(eobool_true == forceset)
    {
        // ok. go on.
    }
    else if(eobool_false == eo_netvar_hid_isWritable(netvar))
    {
        return(eores_NOK_generic);
    }

   return(s_eo_netvar_Set(netvar, dat, netvar->data.valuevol, upd));
}


extern eOresult_t eo_netvar_Reset(const EOnetvar *netvar, eObool_t forcerst, eOnetvarUpdate_t upd)
{
    if(NULL == netvar)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_true == forcerst)
    {
        // ok. go on.
    }
    else if(eobool_false == eo_netvar_hid_isWritable(netvar))
    {
        return(eores_NOK_generic);
    }

    return(s_eo_netvar_Set(netvar, netvar->data.valuedef, netvar->data.valuevol, upd));

}


extern uint16_t eo_netvar_Capacity(const EOnetvar *netvar)
{
    if(NULL == netvar)
    {
        return(0);
    }
    
    return(netvar->data.capacity);  
}


extern uint16_t eo_netvar_Size(const EOnetvar *netvar, const void *data)
{
    uint16_t size = 0;
    eObool_t typisarray = eobool_false;
    void *dat = NULL;

    if(NULL == netvar)
    {
        return(0);
    }

    typisarray = (eo_nv_TYP_arr == eo_netvar_fromIDtoTYP(netvar->id)) ? eobool_true : eobool_false;
    dat = (NULL != data) ? ((void*)data) : (netvar->data.valuevol);

    size = (eobool_false == typisarray) ? (netvar->data.capacity) : (2 + *((uint16_t*)dat));
    
    return(size);  
}

// fills data with capacity bytes, ... but if an array typ then size is not capacity ...
extern eOresult_t eo_netvar_Get(const EOnetvar *netvar, eOnetvarStorage_t strg, void *data, uint16_t *size)
{
    eOresult_t res = eores_NOK_generic;
    void *source = NULL;
    eObool_t typisarray = eobool_false;
    
    if((NULL == data) || (NULL == size) || (NULL == netvar))
    {
        return(eores_NOK_nullpointer);
    }


    typisarray = (eo_nv_TYP_arr == eo_netvar_fromIDtoTYP(netvar->id)) ? eobool_true : eobool_false;    


    switch(strg)
    {
        case eo_nv_strg_volatile:
        {   // better to protect so that the copy is atomic and not interrupted by other tasks which write 
            eo_nvs_Take(eo_nvs_GetHandle(), eok_reltimeINFINITE);
            source = netvar->data.valuevol;
            *size = (eobool_false == typisarray) ? (netvar->data.capacity) : (2 + *((uint16_t*)source));
            memcpy(data, source, *size); 
            eo_nvs_Release(eo_nvs_GetHandle());
            res = eores_OK;
        } break;

        case eo_nv_strg_default:
        {   // no need to protect as the data is read only
            source = (void*)netvar->data.valuedef;
            *size = (eobool_false == typisarray) ? (netvar->data.capacity) : (2 + *((uint16_t*)source));
            memcpy(data, source, *size);
            res = eores_OK;
        } break;

        case eo_nv_strg_permanent:
        {   // protection is done inside eo_netvar_hid_GetPERMANENT()
            res = eo_netvar_hid_GetPERMANENT(netvar, data, size);
        } break;
    }

    
    return(res);  
}

extern eOresult_t eo_netvar_Init(const EOnetvar *netvar)
{
    eOresult_t res = eores_NOK_generic;
    // call the init function if existing
    if((NULL != netvar->peripheralinterface) && (NULL != netvar->peripheralinterface->init))
    {   // protect ...
        eo_nvs_Take(eo_nvs_GetHandle(), eok_reltimeINFINITE);
        netvar->peripheralinterface->init((void*)netvar);
        eo_nvs_Release(eo_nvs_GetHandle());

        res = eores_OK;
    }

    return(res);
}

extern eOresult_t eo_netvar_Update(const EOnetvar *netvar)
{
    eOresult_t res = eores_NOK_generic;
    // call the update function if necessary, to propagate the change of data to the peripheral (out)
    if(eobool_true == eo_netvar_hid_isUpdateable(netvar)) 
    {
        if((NULL != netvar->peripheralinterface) && (NULL != netvar->peripheralinterface->update))
        {   // protect
            eo_nvs_Take(eo_nvs_GetHandle(), eok_reltimeINFINITE);
            netvar->peripheralinterface->update((void*)netvar);
            eo_nvs_Release(eo_nvs_GetHandle());

            res = eores_OK;
        }
    }

    return(res);
}

extern eOnetvarID_t eo_netvar_GetID(const EOnetvar *netvar)
{
    if(NULL == netvar)
    {
        return(0);
    }
    return(netvar->id);

}



extern eOresult_t eo_netvar_remoteSet(const EOnetvar *netvar, const void *dat, eOnetvarUpdate_t upd)
{
    if(NULL == netvar)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_true == eo_netvar_hid_isLocal(netvar))
    {
        return(eores_NOK_generic);
    }

    return(s_eo_netvar_Set(netvar, dat, netvar->data.flex.rem_valuerec, upd));
}

extern eOresult_t eo_netvar_remoteGet(const EOnetvar *netvar, void *data, uint16_t *size)
{
    void *source = NULL;
    eObool_t typisarray = eobool_false;
    
    if((NULL == data) || (NULL == size) || (NULL == netvar))
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_true == eo_netvar_hid_isLocal(netvar))
    {
        return(eores_NOK_generic);
    }


    typisarray = (eo_nv_TYP_arr == eo_netvar_fromIDtoTYP(netvar->id)) ? eobool_true : eobool_false;    


    eo_nvs_Take(eo_nvs_GetHandle(), eok_reltimeINFINITE);
    source = netvar->data.flex.rem_valuerec;
    *size = (eobool_false == typisarray) ? (netvar->data.capacity) : (2 + *((uint16_t*)source));
    memcpy(data, source, *size);
    eo_nvs_Release(eo_nvs_GetHandle());

    
    return(eores_OK);  
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

//extern EOnetvar * eo_netvar_hid_New(uint8_t fun, uint8_t typ, uint32_t otherthingsmaybe)
//{
//    EOnetvar *retptr = NULL;    
//
//    // i get the memory for the object
//    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOnetvar), 1);
//    
//    
//    return(retptr);
//}

#warning -> document all the extern eo_netvar_hid* and verify if they all are needed

extern eObool_t eo_netvar_hid_OnBefore_ROP(const EOnetvar *nv, eOropcode_t ropcode)
{
    eObool_t ret = eobool_false;
    eOvoid_fp_voidp_t fn_before_rop = NULL;

    if((NULL == nv) || (NULL == nv->on_rop_reception))
    {
        return(ret);
    }

    switch(ropcode)
    {
        case eo_ropcode_ask:
        {
            fn_before_rop = nv->on_rop_reception->loc.ask.bef;
        } break;

        case eo_ropcode_say:
        {
            fn_before_rop = nv->on_rop_reception->rem.say.bef;
        } break;

        case eo_ropcode_set:
        {
            fn_before_rop = nv->on_rop_reception->loc.set.bef;
        } break;

        case eo_ropcode_sig:
        {
            fn_before_rop = nv->on_rop_reception->rem.sig.bef;
        } break;

        case eo_ropcode_rst:
        {
            fn_before_rop = nv->on_rop_reception->loc.rst.bef;
        } break;

        case eo_ropcode_upd:
        {
            fn_before_rop = nv->on_rop_reception->loc.upd.bef;

        } break;
    }

    if(NULL != fn_before_rop)
    {
        eo_nvs_Take(eo_nvs_GetHandle(), eok_reltimeINFINITE);
        fn_before_rop((void*)nv);
        eo_nvs_Release(eo_nvs_GetHandle());
        ret = eobool_true;
    }

    return(ret);
   
}

extern eObool_t eo_netvar_hid_OnAfter_ROP(const EOnetvar *nv, eOropcode_t ropcode)
{
    eObool_t ret = eobool_false;
    eOvoid_fp_voidp_t fn_after_rop = NULL;

    if((NULL == nv) || (NULL == nv->on_rop_reception))
    {
        return(ret);
    }

    switch(ropcode)
    {
        case eo_ropcode_ask:
        {
            fn_after_rop = nv->on_rop_reception->loc.ask.aft;
        } break;

        case eo_ropcode_say:
        {
            fn_after_rop = nv->on_rop_reception->rem.say.aft;
        } break;

        case eo_ropcode_set:
        {
            fn_after_rop = nv->on_rop_reception->loc.set.aft;
        } break;

        case eo_ropcode_sig:
        {
            fn_after_rop = nv->on_rop_reception->rem.sig.aft;
        } break;

        case eo_ropcode_rst:
        {
            fn_after_rop = nv->on_rop_reception->loc.rst.aft;
        } break;

        case eo_ropcode_upd:
        {
            fn_after_rop = nv->on_rop_reception->loc.upd.aft;
        } break;
    }

    if(NULL != fn_after_rop)
    {
        eo_nvs_Take(eo_nvs_GetHandle(), eok_reltimeINFINITE);
        fn_after_rop((void*)nv);
        eo_nvs_Release(eo_nvs_GetHandle());
        ret = eobool_true;
    }
        
    return(ret);
}



extern eObool_t eo_netvar_hid_isWritable(const EOnetvar *netvar)
{
    eOnetvarFunc_t fun = eo_netvar_fromIDtoFUN(netvar->id);

    if((fun == eo_nv_FUN_out) || (fun == eo_nv_FUN_cfg) || (fun == eo_nv_FUN_beh))
    {
        return(eobool_true);
    }
    else
    {
        return(eobool_false);
    }
}


extern eObool_t eo_netvar_hid_isLocal(const EOnetvar *netvar)
{
    return((eo_nv_ownership_local == netvar->nvownership) ? (eobool_true) : (eobool_false));
} 

extern eObool_t eo_netvar_hid_isPermanent(const EOnetvar *netvar)
{
    eOnetvarFunc_t fun = eo_netvar_fromIDtoFUN(netvar->id);

    if(fun == eo_nv_FUN_cfg)
    {
        return(eobool_true);
    }
    else
    {
        return(eobool_false);
    }
} 

extern eObool_t eo_netvar_hid_isUpdateable(const EOnetvar *netvar)
{
    eOnetvarFunc_t fun = eo_netvar_fromIDtoFUN(netvar->id);

    if((fun == eo_nv_FUN_inp) || (fun == eo_nv_FUN_out) || (fun == eo_nv_FUN_cfg) || (fun == eo_nv_FUN_beh))
    {
        return(eobool_true);
    }
    else
    {
        return(eobool_false);
    }
} 


extern eObool_t eo_netvar_hid_isMirrorable(const EOnetvar *netvar)
{
    if(NULL == netvar->data.mirror)
    {
        return(eobool_false);
    }

    return(eobool_true);
}

extern eObool_t eo_netvar_hid_isMirrored(const EOnetvar *netvar)
{
    if(NULL == netvar->data.mirror)
    {
        return(eobool_false);
    }

    return((NULL == *(netvar->data.mirror)) ? (eobool_false) : (eobool_true));
}


extern eOresult_t eo_netvar_hid_loadMirror(const EOnetvar *netvar, void *mirror)
{
    if((NULL == netvar) || (NULL == mirror))
    {
        return(eores_NOK_nullpointer);
    }

    if(NULL != netvar->data.mirror)
    {
        // load mirror
        eo_nvs_Take(eo_nvs_GetHandle(), eok_reltimeINFINITE);
        *(netvar->data.mirror) = mirror;
        eo_nvs_Release(eo_nvs_GetHandle());
        return(eores_OK);
    }

    return(eores_NOK_generic);
}


extern eOresult_t eo_netvar_hid_updateMirror(const EOnetvar *netvar)
{
    if(NULL == netvar)
    {
        return(eores_NOK_nullpointer);
    }

    if(NULL != netvar->data.mirror)
    {
        eo_nvs_Take(eo_nvs_GetHandle(), eok_reltimeINFINITE);
        memcpy(*(netvar->data.mirror), netvar->data.valuevol, netvar->data.capacity); 
        eo_nvs_Release(eo_nvs_GetHandle());  
        return(eores_OK);
    }

    return(eores_NOK_generic);
}


extern const void* eo_netvar_hih_GetDEFAULT(const EOnetvar *netvar)
{
    if(NULL == netvar)
    {
        return(NULL);
    }

    return(netvar->data.valuedef);
}

extern void* eo_netvar_hid_GetVOLATILE(const EOnetvar *netvar)
{
    if(NULL == netvar)
    {
        return(NULL);
    }

    return(netvar->data.valuevol);
}

extern void * eo_netvar_hid_GetMIRROR(const EOnetvar *netvar)
{
    if((NULL == netvar) ||(NULL == netvar->data.mirror))
    {
        return(NULL);
    }

    return(*(netvar->data.mirror));
}

extern eOresult_t eo_netvar_hid_GetPERMANENT(const EOnetvar *netvar, void *dat, uint16_t *size)
{
    eOresult_t res = eores_NOK_generic;
    eObool_t typisarray = eobool_false;

    if((NULL == netvar) || (NULL == dat))
    {
        return(eores_NOK_nullpointer);
    }


    typisarray = (eo_nv_TYP_arr == eo_netvar_fromIDtoTYP(netvar->id)) ? eobool_true : eobool_false;

    *size = 0;

    if((eobool_true == eo_netvar_hid_isPermanent(netvar)) && (eobool_true == eo_netvar_hid_isLocal(netvar)))
    {
        if(EONETVAR_NOVALUEPER != netvar->data.flex.loc_addrperm)
        {   // protection is inside EOVstorage
            eov_strg_Get(eo_nvs_GetLocalStorage(eo_nvs_GetHandle()), netvar->data.flex.loc_addrperm, netvar->data.capacity, dat);
            *size = (eobool_false == typisarray) ? (netvar->data.capacity) : (2 + *((uint16_t*)dat));
            res = eores_OK;
        }
    }

    return(res);
}



extern uint16_t eo_netvar_hid_GetCAPACITY(const EOnetvar *netvar)
{
    if(NULL == netvar)
    {
        return(NULL);
    }

    return(netvar->data.capacity);
}






// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_netvar_Set(const EOnetvar *netvar, const void *dat, void *dst, eOnetvarUpdate_t upd)
{
    uint16_t size = 0;
    eObool_t typisarray = eobool_false;


    eo_nvs_Take(eo_nvs_GetHandle(), eok_reltimeINFINITE);

    typisarray = (eo_nv_TYP_arr == eo_netvar_fromIDtoTYP(netvar->id)) ? eobool_true : eobool_false;

    size = (eobool_false == typisarray) ? (netvar->data.capacity) : (2 + *((uint16_t*)dat));

    memcpy(dst, dat, size);

    #warning -> eo_netvar_hid_updateMirror() uses the valuevol 
    if(eobool_true == eo_netvar_hid_isMirrored(netvar))
    {
        eo_netvar_hid_updateMirror(netvar);
    }


    if((eobool_true == eo_netvar_hid_isPermanent(netvar)) && (eobool_true == eo_netvar_hid_isLocal(netvar)))
    {
        if(EONETVAR_NOVALUEPER != netvar->data.flex.loc_addrperm)
        {
            eov_strg_Set(eo_nvs_GetLocalStorage(eo_nvs_GetHandle()), netvar->data.flex.loc_addrperm, size, dat);
        }
    }


    // call the update function if necessary, to propagate the change of data to the peripheral
    if(eo_nv_upd_dontdo != upd)
    {
        if((eo_nv_upd_always == upd) || (eobool_true == eo_netvar_hid_isUpdateable(netvar))) 
        {
            if((NULL != netvar->peripheralinterface) && (NULL != netvar->peripheralinterface->update))
            {
                netvar->peripheralinterface->update((void*)netvar);
            }
        }
    }


    eo_nvs_Release(eo_nvs_GetHandle());

    return(eores_OK);
}




extern eOresult_t eo_netvar_SetSAFE(const EOnetvar *netvar, const void *dat, eObool_t forceset, eOnetvarUpdate_t upd)
{
    uint16_t size = 0;
    eObool_t typisarray = eobool_false;

    if(NULL == netvar)
    {
        return(eores_NOK_nullpointer);
    }


    if(eobool_true == forceset)
    {
        // ok. go on.
    }
    else if(eobool_false == eo_netvar_hid_isWritable(netvar))
    {
        return(eores_NOK_generic);
    }


    eo_nvs_Take(eo_nvs_GetHandle(), eok_reltimeINFINITE);

    typisarray = (eo_nv_TYP_arr == eo_netvar_fromIDtoTYP(netvar->id)) ? eobool_true : eobool_false;

    size = (eobool_false == typisarray) ? (netvar->data.capacity) : (2 + *((uint16_t*)dat));

    memcpy(netvar->data.valuevol, dat, size);

    if(NULL != netvar->data.mirror)
    {
        memcpy(*(netvar->data.mirror), dat, size);   
    }

    if((eobool_true == eo_netvar_hid_isPermanent(netvar)) && (eobool_true == eo_netvar_hid_isLocal(netvar)))
    {
        if(EONETVAR_NOVALUEPER != netvar->data.flex.loc_addrperm)
        {
            eov_strg_Set(eo_nvs_GetLocalStorage(eo_nvs_GetHandle()), netvar->data.flex.loc_addrperm, size, dat);
        }
    }


    // call the update function if necessary, to propagate the change of data to the peripheral
    if(eo_nv_upd_dontdo != upd)
    {
        if((eo_nv_upd_always == upd) || (eobool_true == eo_netvar_hid_isUpdateable(netvar))) 
        {
            if((NULL != netvar->peripheralinterface) && (NULL != netvar->peripheralinterface->update))
            {
                netvar->peripheralinterface->update((void*)netvar);
            }
        }
    }


    eo_nvs_Release(eo_nvs_GetHandle());

    return(eores_OK);
}


extern eOresult_t eo_netvar_ResetSAFE(const EOnetvar *netvar, eObool_t forcerst, eOnetvarUpdate_t upd)
{
    eOresult_t res = eores_NOK_generic;

    if(NULL == netvar)
    {
        return(eores_NOK_nullpointer);
    }

    if(eobool_true == forcerst)
    {
        // ok. go on.
    }
    else if(eobool_false == eo_netvar_hid_isWritable(netvar))
    {
        return(eores_NOK_generic);
    }

    eo_nvs_Take(eo_nvs_GetHandle(), eok_reltimeINFINITE);

    memcpy(netvar->data.valuevol, netvar->data.valuedef, netvar->data.capacity);

    if(NULL != netvar->data.mirror)
    {
        memcpy(*(netvar->data.mirror), netvar->data.valuedef, netvar->data.capacity);   
    }


    if((eobool_true == eo_netvar_hid_isPermanent(netvar)) && (eobool_true == eo_netvar_hid_isLocal(netvar)))
    {
        if(EONETVAR_NOVALUEPER != netvar->data.flex.loc_addrperm)
        {
            eov_strg_Set(eo_nvs_GetLocalStorage(eo_nvs_GetHandle()), netvar->data.flex.loc_addrperm, netvar->data.capacity, netvar->data.valuedef);
            res = eores_OK;
        }
    }

    // call the update function if necessary, to propagate the change of data to the peripheral (out)
    if(eo_nv_upd_dontdo != upd)
    {
        if((eo_nv_upd_always == upd) || (eobool_true == eo_netvar_hid_isUpdateable(netvar))) 
        {
            if((NULL != netvar->peripheralinterface) && (NULL != netvar->peripheralinterface->update))
            {
                netvar->peripheralinterface->update((void*)netvar);
            }
        }
    }

    eo_nvs_Release(eo_nvs_GetHandle());
    // ok, but which one ??

    return(res);
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




