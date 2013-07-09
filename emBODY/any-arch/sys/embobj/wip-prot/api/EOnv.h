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
// empty-section


// - declaration of public user-defined types -------------------------------------------------------------------------    


/** @typedef    typedef struct EOnv_hid EOnv
    @brief      EOaction is an opaque struct. It is used to implement data abstraction for the datagram 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOnv_hid EOnv;


typedef uint8_t eOnvBRD_t;
typedef uint32_t eOnvID32_t;
typedef uint32_t eOnvPROGnum_t;
typedef uint8_t eOnvEP8_t;
typedef uint8_t eOnvENT_t;

// typedef uint8_t eOnvENT_t;
// typedef uint8_t eOnvIND_t;
// typedef uint8_t eOnvTAG_t;




enum { eo_nv_IPdummy = 0, eo_nv_EP8dummy = EOK_uint08dummy }; 

#define eo_nv_ID32dummy     EOK_uint32dummy
#define eo_nv_PROGnumdummy  EOK_uint32dummy


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

extern uint16_t eo_nv_Size(const EOnv *nv);

extern uint16_t eo_nv_Capacity(const EOnv *nv);

extern eOresult_t eo_nv_Set(const EOnv *nv, const void *dat, eObool_t forceset, eOnvUpdate_t upd);

extern eOresult_t eo_nv_Reset(const EOnv *nv, eObool_t forcerst, eOnvUpdate_t upd);

extern eOresult_t eo_nv_Get(const EOnv *nv, eOnvStorage_t strg, void *data, uint16_t *size);

extern eOipv4addr_t eo_nv_GetIP(const EOnv *nv);

extern eOnvID32_t eo_nv_GetID32(const EOnv *nv);

extern eOnvEP8_t eo_nv_GetEP8(const EOnv *nv);


extern eOnvRWmode_t eo_nv_GetRWmode(const EOnv *nv);

extern eOnvOwnership_t eo_nv_GetOwnership(const EOnv *nv);





/** @}            
    end of group eo_nv 
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

