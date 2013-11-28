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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOPROTOCOLSK_H_
#define _EOPROTOCOLSK_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolSK.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocolSK Protocol for skin endpoint 
    This file contains tags, rw mode, and callback functions specific of the eoprot_endpoint_skin endpoint.
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EoSkin.h"
#include "EOnv.h"
#include "EOrop.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 


enum { eoprot_entities_sk_numberof = eosk_entities_numberof };

// - entity skin


/** @typedef    typedef enum eOprot_tag_sk_skin_t
    @brief      It contains the tags for all variables of the skin entity.
                See definition of eOsk_skin_t (and its fields) in file EoSkin.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_tag_sk_skin_wholeitem                                     = 0,
    eoprot_tag_sk_skin_config_sigmode                                = 1,
    eoprot_tag_sk_skin_status_arrayof10canframes                     = 2
} eOprot_tag_sk_skin_t;

enum { eoprot_tags_sk_skin_numberof = 3 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_rwm_sk_skin_t
    @brief      It contains the rw modes for all variables of the skin entity. There must be a one-to-one
                correspondence to the values in eOprot_tag_sk_skin_t.
 **/
typedef enum
{
    eoprot_rwm_sk_skin_wholeitem                                  = eo_nv_rwmode_RO,
    eoprot_rwm_sk_skin_config_sigmode                             = eo_nv_rwmode_RW,
    eoprot_rwm_sk_skin_status_arrayof10canframes                  = eo_nv_rwmode_RO
} eOprot_rwm_sk_skin_t; 

enum { eoprot_rwms_sk_skin_numberof = 3 };  // it MUST be equal to the number of rw modes. 


// - structures implementing the endpoint
  

/** @typedef    typedef struct eOprot_template_sk_t;
    @brief      It is a template for the organisation of skin entities in the skin endpoint.
                The effective number may depend on the board.
 **/
typedef struct                  // 176+0 = 176              
{
    eOsk_skin_t                 skin;
} eOprot_template_sk_t;         //EO_VERIFYsizeof(eOprot_template_sk_t, 176);



// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


// - declaration of extern overridable functions ----------------------------------------------------------------------

extern void eoprot_fun_INITIALISE_sk(eOprotIP_t ip, void *ram);

// -- skin

extern void eoprot_fun_INIT_sk_skin_wholeitem(const EOnv* nv);
extern void eoprot_fun_UPDT_sk_skin_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_sk_skin_config_sigmode(const EOnv* nv);
extern void eoprot_fun_UPDT_sk_skin_config_sigmode(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_sk_skin_status_arrayof10canframes(const EOnv* nv);
extern void eoprot_fun_UPDT_sk_skin_status_arrayof10canframes(const EOnv* nv, const eOropdescriptor_t* rd);



/** @}            
    end of group eo_EoProtocolSK  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


