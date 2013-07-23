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
#ifndef _EOPROTOCOLMN_H_
#define _EOPROTOCOLMN_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoProtocolMN.h
    @brief      This header file gives the ...
    @author     marco.accame@iit.it
    @date       06/05/2013
**/

/** @defgroup eo_EoProtocolMN Protocol for management endpoint 
    This file contains tags, rw mode, and callback functions specific of the eoprot_endpoint_management endpoint.
    
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EoProtocol.h"
#include "EoManagement.h"
#include "EOnv.h"
#include "EOrop.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum { eoprot_entities_mn_numberof = eomn_entities_numberof };

// - entity comm


/** @typedef    typedef enum eOprot_tag_mn_comm_t
    @brief      It contains the tags for all variables of the comm entity.
                See definition of eOmn_comm_t (and its fields) in file EoManagement.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_tag_mn_comm_wholeitem                                    = 0,    
    eoprot_tag_mn_comm_cmmnds_ropsigcfg                             = 1
} eOprot_tag_mn_comm_t;

enum { eoprot_tags_mn_comm_numberof = 2 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_rwm_mn_comm_t
    @brief      It contains the rw modes for all variables of the comm entity. There must be a one-to-one
                correspondence to the values in eOprot_tag_mn_comm_t.
 **/
typedef enum
{
    eoprot_rwm_mn_comm_wholeitem                                    = eo_nv_rwmode_RO,
    eoprot_rwm_mn_comm_cmmnds_ropsigcfg                             = eo_nv_rwmode_RW
} eOprot_rwm_mn_comm_t; 

enum { eoprot_rwms_mn_comm_numberof = 2 };  // it MUST be equal to the number of rw modes. 



// - entity appl
  

/** @typedef    typedef enum eOprot_tag_mn_appl_t
    @brief      It contains the tags for all variables of the appl entity.
                See definition of eOmn_appl_t (and its fields) in file EoManagement.h for explanation of the variables.
 **/
typedef enum
{
    eoprot_tag_mn_appl_wholeitem                                    = 0,
    eoprot_tag_mn_appl_config                                       = 1,
    eoprot_tag_mn_appl_status                                       = 2,
    eoprot_tag_mn_appl_cmmnds_go2state                              = 3
} eOprot_tag_mn_appl_t;

enum { eoprot_tags_mn_appl_numberof = 4 };  // it MUST be equal to the number of tags. 


/** @typedef    typedef enum eOprot_rwm_mn_appl_t
    @brief      It contains the rw modes for all variables of the appl entity. There must be a one-to-one
                correspondence to the values in eOprot_tag_mn_appl_t.
 **/
typedef enum
{
    eoprot_rwm_mn_appl_wholeitem                                    = eo_nv_rwmode_RO,
    eoprot_rwm_mn_appl_config                                       = eo_nv_rwmode_RW,
    eoprot_rwm_mn_appl_status                                       = eo_nv_rwmode_RO,
    eoprot_rwm_mn_appl_cmmnds_go2state                              = eo_nv_rwmode_RW
} eOprot_rwm_mn_appl_t; 

enum { eoprot_rwms_mn_appl_numberof = 4 };  // it MUST be equal to the number of rw modes. 


// - memory organization in the endpoint
  
  
/** @typedef    typedef struct eOprot_template_mn_t;
    @brief      It is a template for the organisation of comm and appl entities in the management endpoint.
                The effective number may depend on the board.
 **/
typedef struct                  // 104+24+0 = 128              
{
    eOmn_comm_t                 communication; 
    eOmn_appl_t                 application;
} eOprot_template_mn_t;         //EO_VERIFYsizeof(eOprot_template_mn_t, 128);  

  
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------
// empty-section


// - declaration of extern overridable functions ----------------------------------------------------------------------

extern void eoprot_fun_INITIALISE_mn(eOprotIP_t ip, void *ram);

// - comm

extern void eoprot_fun_INIT_mn_comm_wholeitem(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_comm_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mn_comm_cmmnds_ropsigcfg(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_comm_cmmnds_ropsigcfg(const EOnv* nv, const eOropdescriptor_t* rd);

// - appl

extern void eoprot_fun_INIT_mn_appl_config(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_appl_config(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mn_appl_wholeitem(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_appl_wholeitem(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mn_appl_status(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_appl_status(const EOnv* nv, const eOropdescriptor_t* rd);

extern void eoprot_fun_INIT_mn_appl_cmmnds_go2state(const EOnv* nv);
extern void eoprot_fun_UPDT_mn_appl_cmmnds_go2state(const EOnv* nv, const eOropdescriptor_t* rd);


/** @}            
    end of group eo_EoProtocolMN  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


