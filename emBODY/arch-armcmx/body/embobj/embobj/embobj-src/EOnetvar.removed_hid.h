
// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EONETVAR_HID_H_
#define _EONETVAR_HID_H_


/* @file       EOnetvar_hid.h
    @brief      This header file implements hidden interface to a netvar object.
    @author     marco.accame@iit.it
    @date       09/06/2011
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOrop.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "EOnetvar.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------



#define eo_nv_getFUN(id)            ((uint8_t) (((id)>>13)&0x0007) )
#define eo_nv_getTYP(id)            ((uint8_t) (((id)>>10)&0x0007) )
#define eo_nv_getOFF(id)            ((uint16_t) (((id)>>0)&0x03ff) )


#define EONETVAR_NOVALUEPER     0xffffffff


#define EONETVAR_ONROP_SET_2FN(fnbe, fnaf)      \
        .bef    = (fnbe),                       \
        .aft    = (fnaf) 
                

#define EONETVAR_ONROP_SET_LOC(fn)              \
    .ask =                                      \
    {                                           \
        .bef    = (fn),                         \
        .aft    = (fn)                          \
    },                                          \
    .set =                                      \
    {                                           \
        .bef    = (fn),                         \
        .aft    = (fn)                          \
    },                                          \
    .rst =                                      \
    {                                           \
        .bef    = (fn),                         \
        .aft    = (fn)                          \
    },                                          \
    .upd =                                      \
    {                                           \
        .bef    = (fn),                         \
        .aft    = (fn)                          \
    }                                                                       


#define EONETVAR_ONROP_SET_REM(fn)              \
    .say =                                      \
    {                                           \
        .bef    = (fn),                         \
        .aft    = (fn)                          \
    },                                          \
    .sig =                                      \
    {                                           \
        .bef    = (fn),                         \
        .aft    = (fn)                          \
    }                           


#define EONETVAR_ONROP_LOCSET(fn)               \
.loc =                                          \
{                                               \
    EONETVAR_ONROP_SET_LOC(fn)                  \
}                                  


#define EONETVAR_ONROP_REMSET(fn)               \
.rem =                                          \
{                                               \
    EONETVAR_ONROP_SET_REM(fn)                  \
}         


// - definition of the hidden struct implementing the object ----------------------------------------------------------


typedef struct                      // to be called w/ EOnetvar*
{
    eOvoid_fp_voidp_t           bef;
    eOvoid_fp_voidp_t           aft;
} eOnetvar_onrop_2fn_t;

typedef struct                      
{
    eOnetvar_onrop_2fn_t        ask;        // used by a simple node on reception of a query sent by a smart node
    eOnetvar_onrop_2fn_t        set;        // used by a simple node on reception of a command from a smart node
    eOnetvar_onrop_2fn_t        rst;        // used by a simple node on reception of a command from a smart node
    eOnetvar_onrop_2fn_t        upd;        // used by a simple node on reception of a command from a smart node
} eOnetvar_onrop_rx_loc_t;

typedef struct                      
{
    eOnetvar_onrop_2fn_t        say;        // used by a smart node on reception of a reply of a query previously sent
    eOnetvar_onrop_2fn_t        sig;        // used by a smart node on reception of a spontaneous signal from a node
} eOnetvar_onrop_rx_rem_t;

typedef union                      
{   // either local or remote
    eOnetvar_onrop_rx_loc_t     loc;        
    eOnetvar_onrop_rx_rem_t     rem;
} eOnetvar_onrop_rx_t;


typedef union
{
    uint32_t                    loc_addrperm;
    void*                       rem_valuerec;
} eOnetvar_flexi_mem_t;

// function used by an agent to synchronise values of netvar with an associated peripheral 
// for inp: it gets a proper value from the peripheral and place it into valvolatile.
//          used: by a periodic agent (trigger by user and regulated by cfg variables)
//          used: by the before_ask function if we prefer to retrieve the latest value from the peripheral
//          used: by the update rop.
// for out: it gets the value from valdefault and propagate it to the peripheral
//          used: by the after_set function to update the peripheral.
//          used: by the update rop.
// for cfg: cannot do ... if we need to propagate the effect to an algorithm better
//          to let this task to the proper before_* and after_* functions.

typedef struct                      // to be called w/ EOnetvar*
{
    eOvoid_fp_voidp_t  init;        // called at startup to init the link between the input or output netvar and the peripheral
    eOvoid_fp_voidp_t  update;      // used to propagate the value of the netvar towards the peripheral (if out)
                                    // or to place the value of the peripheral into the netvar (if inp)
} eOnetvar_fn_peripheralinterface_t;

typedef struct
{
    uint16_t                capacity;       // capacity in bytes of the data
    uint16_t                flags;          
    const void              *valuedef;      // default value of the data (in rom)
    void                    *valuevol;      // volatile value of the data (in ram)
    void                    **mirror;       // if its ptr is not NULL, then mirror must be a copy of valuevol
    //uint32_t                addrperm;       // address of permanent storage of value for requested cases (in eeprom).
    eOnetvar_flexi_mem_t    flex;
} eOnetvar_data_t;  

/** @struct     EOnetvar_hid
    @brief      Hidden definition. Implements private data used only internally by the 
                public or private (static) functions of the object and protected data
                used also by its derived objects.
 **/
struct EOnetvar_hid 
{
    eOnetvarID_t                        id;                 // keeps fun and typ in its inside
    eOnetvarOwnership_t                 nvownership;          // tells if the netvar is local or remote
    uint8_t                             forfutureuse;
    const eOnetvar_data_t               data;               // contains informations on the data and pointers to it
    const eOnetvar_fn_peripheralinterface_t*  peripheralinterface;      // contains function pointers used to interface data with the device
    const eOnetvar_onrop_rx_t*          on_rop_reception;   // contains operations to be done at reception of a netvar. they depend on ownership
};   
 



// - declaration of extern hidden functions ---------------------------------------------------------------------------

///** @fn         extern EOnetvar * eo_netvar_hid_New(uint8_t fun, uint8_t typ, uint32_t otherthingsmaybe)
//    @brief      Creates a new netvar object. 
//    @return     The pointer to the required object.
// **/
//extern EOnetvar * eo_netvar_hid_New(uint8_t fun, uint8_t typ, uint32_t otherthingsmaybe);



extern eObool_t eo_netvar_hid_OnBefore_ROP(const EOnetvar *nv, eOropcode_t ropcode);

extern eObool_t eo_netvar_hid_OnAfter_ROP(const EOnetvar *nv, eOropcode_t ropcode);




extern eObool_t eo_netvar_hid_isWritable(const EOnetvar *netvar);

extern eObool_t eo_netvar_hid_isLocal(const EOnetvar *netvar);

extern eObool_t eo_netvar_hid_isPermanent(const EOnetvar *netvar);

extern eObool_t eo_netvar_hid_isUpdateable(const EOnetvar *netvar);


extern eObool_t eo_netvar_hid_isMirrorable(const EOnetvar *netvar);

extern eObool_t eo_netvar_hid_isMirrored(const EOnetvar *netvar);

extern eOresult_t eo_netvar_hid_loadMirror(const EOnetvar *netvar, void *mirror);

extern eOresult_t eo_netvar_hid_updateMirror(const EOnetvar *netvar);


extern const void* eo_netvar_hid_GetDEFAULT(const EOnetvar *netvar);

extern void* eo_netvar_hid_GetVOLATILE(const EOnetvar *netvar);

extern void * eo_netvar_hid_GetMIRROR(const EOnetvar *netvar);



extern eOresult_t eo_netvar_hid_GetPERMANENT(const EOnetvar *netvar, void *dat, uint16_t *size);

extern uint16_t eo_netvar_hid_GetCAPACITY(const EOnetvar *netvar);





 

#endif  // include guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




