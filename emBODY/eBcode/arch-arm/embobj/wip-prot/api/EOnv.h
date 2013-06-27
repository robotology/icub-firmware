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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EONV_H_
#define _EONV_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOnv.h
    @brief      This header file implements public interface to a netvar.
    @author     marco.accame@iit.it
    @date       09/06/2011
**/

/** @defgroup eo_n Objevct EOnv
    Offers methods for reading and writing some data on RAM, ROM and storage and for doing some
    actions upon such operations. It is to be manipulated only via its public methods.
    It is not created but loaded as a constant object from a configuration mapped in ROM
      
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"




// - public #define  --------------------------------------------------------------------------------------------------


#define EO_nv_FUNTYP(fun, typ) 	    ((uint8_t)(  (((fun)&0x07)<<5) | (((typ)&0x07)<<2) )  )
#define EO_nv_FUN(funtyp)           ((uint8_t)(  (((uint8_t)(funtyp))>>5) & 0x07)         )
#define EO_nv_TYP(funtyp)           ((uint8_t)(  (((uint8_t)(funtyp))>>2) & 0x07)         )


// - declaration of public user-defined types -------------------------------------------------------------------------    


/** @typedef    typedef struct EOnv_hid EOnv
    @brief      EOaction is an opaque struct. It is used to implement data abstraction for the datagram 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOnv_hid EOnv;


typedef uint16_t eOnvEP_t;

typedef uint16_t eOnvID_t;

enum { eo_nv_IPdummy = 0, eo_nv_EPdummy = EOK_uint16dummy, eo_nv_IDdummy = EOK_uint16dummy }; 

//typedef uint8_t eOnvIDfuntyp_t;


typedef struct      // 04 bytes
{
    eOnvEP_t        ep;
    eOnvID_t        id;
} eOnvEPID_t;       EO_VERIFYsizeof(eOnvEPID_t, 4);


typedef enum
{
    eo_nv_ownership_local       = 0,
    eo_nv_ownership_remote      = 1
} eOnvOwnership_t; 



typedef enum
{
    eo_nv_strg_volatile     = 0,
    eo_nv_strg_default      = 1
} eOnvStorage_t;


// typedef enum
// {
//     eo_nv_FUN_NO0               = 0,
//     eo_nv_FUN_NO1               = 1,
//     eo_nv_FUN_mix               = 2,
//     eo_nv_FUN_con               = 3,
//     eo_nv_FUN_cfg               = 4,
//     eo_nv_FUN_beh               = 5,
//     eo_nv_FUN_inp               = 6,
//     eo_nv_FUN_out               = 7
// } eOnvFunc_t; 

// enum {eonv_fun_bits_numberof = 3 };


// typedef enum
// {
//     eo_nv_TYP_b08                = 0,
//     eo_nv_TYP_b16                = 1,
//     eo_nv_TYP_b32                = 2,
//     eo_nv_TYP_b64                = 3,
//     eo_nv_TYP_NO4                = 4,
//     eo_nv_TYP_NO5                = 5,
//     eo_nv_TYP_arr                = 6,   // packed as a EOarray object: 2B+1B+1B+[data]
//     eo_nv_TYP_pkd                = 7
// } eOnvType_t; 

// enum {eonv_typ_bits_numberof = 3 };


typedef enum
{
    eo_nv_rwmode_RO         = 1,
    eo_nv_rwmode_RW         = 3,
    eo_nv_rwmode_WO         = 2,    // maybe we can use it for commands ...
    eo_nv_rwmode_NONE       = 0
} eOnvRWmode_t; 


typedef enum
{
    eo_nv_upd_dontdo        = 0,
    eo_nv_upd_ifneeded      = 1,
    eo_nv_upd_always        = 2
} eOnvUpdate_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------





// - declaration of extern public functions ---------------------------------------------------------------------------



extern EOnv* eo_nv_New(void);

extern eOresult_t eo_nv_Clear(EOnv *nv);

extern eOresult_t eo_nv_Init(const EOnv *nv);

//extern eOresult_t eo_nv_Update(const EOnv *nv);

extern uint16_t eo_nv_Size(const EOnv *nv);

extern uint16_t eo_nv_Capacity(const EOnv *nv);

extern eOresult_t eo_nv_Set(const EOnv *nv, const void *dat, eObool_t forceset, eOnvUpdate_t upd);

extern eOresult_t eo_nv_Reset(const EOnv *nv, eObool_t forcerst, eOnvUpdate_t upd);

extern eOresult_t eo_nv_Get(const EOnv *nv, eOnvStorage_t strg, void *data, uint16_t *size);

//extern eOresult_t eo_nv_remoteSet(const EOnv *nv, const void *dat, eOnvUpdate_t upd);
//extern eOresult_t eo_nv_remoteGet(const EOnv *nv, void *data, uint16_t *size);


extern eOipv4addr_t eo_nv_GetIP(const EOnv *nv);
extern eOnvEP_t eo_nv_GetEP(const EOnv *nv);
extern eOnvID_t eo_nv_GetID(const EOnv *nv);

extern eOnvRWmode_t eo_nv_GetRWmode(const EOnv *nv);

extern eOnvOwnership_t eo_nv_GetOwnership(const EOnv *nv);

//extern eOnvFunc_t eo_nv_GetFUN(const EOnv *nv);
//extern eOnvType_t eo_nv_GetTYP(const EOnv *nv);






/** @}            
    end of group eo_nv 
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

