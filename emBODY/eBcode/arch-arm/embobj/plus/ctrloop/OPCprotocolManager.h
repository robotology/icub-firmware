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
#ifndef _OPCPROTOCOLMANAGER_H_
#define _OPCPROTOCOLMANAGER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       OPCprotocolManager.h
	@brief      This header file implements public interface to the opc protocol manager.
 	@author     marco.accame@iit.it
	@date       03/19/2013
 **/

/** @defgroup doxy_opcprotman OPCprotocolManager singleton 
    
    The OPC protocol is formed by the following fields: header [+ data].
    
    header: 8 bytes 
    
    B0      B1      B2      B3      B4      B5      B6      B7       
    [SGN   ][OPC   ][DBV           ][VAR           ][LEN           ]
        |                               |               |
                                        |               |--> size of the data field
                                        |
                                        |--> identifier of the variable
                        
                        |--> identifier of the database containing the variable
                |
                |--> the opcode to apply to the variable
        |
        |--> the signature (it is always 0xDE)
 

    data: LEN bytes + 0 padding to reach multiple of four 
    
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum { opcprotman_signature = 0xDE };

/** @typedef    typedef struct OPCprotocolManager_hid OPCprotocolManager
    @brief      OPCprotocolManager is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct OPCprotocolManager_hid OPCprotocolManager;


typedef enum  
{
    opcprotman_OK_withreply =  1,   /**< correct result: with reply */
    opcprotman_OK           =  0,   /**< correct result: generic */
    opcprotman_NOK_generic  = -1    /**< error: generic */
} opcprotman_res_t;


typedef enum
{
    opcprotman_opc_ask      = 0,
    opcprotman_opc_say      = 1,
    opcprotman_opc_set      = 2,
    opcprotman_opc_sig      = 3,
    opcprotman_opc_none     = 127,
} opcprotman_opc_t;

typedef uint16_t opcprotman_dbv_t;
typedef uint16_t opcprotman_var_t;


typedef struct
{
    uint8_t                 sgn;    /**< contains a signature for the opc. it is teh first byte of the packet and must be opcprotman_signature  */
    uint8_t                 opc;    /**< use opcprotman_opc_t values */
    uint16_t                dbv;    /**< version of the used database */
    uint32_t                seqnum;
    uint32_t                dummy;
    uint16_t                var;    /**< identifier of the variable to be manipulated */
    uint16_t                len;    /**< length of the data field */
} opcprotman_header_t; 


// use only as a pointer
typedef struct
{
    opcprotman_header_t     head;
    uint32_t                data[1];
} opcprotman_message_t; 


typedef struct opcprotman_var_map_T opcprotman_var_map_t;


typedef void (*opcprotman_fp_onrec_t)(opcprotman_opc_t opc, opcprotman_var_map_t* map, void* recdata);

struct opcprotman_var_map_T
{
    uint16_t                var;
    uint16_t                size;
    void*                   ptr;
    opcprotman_fp_onrec_t   onrec;  // calls the opc, the opcprotman_var_map_t*, and data received in message
};



/**	@typedef    typedef struct opcprotman_cfg_t 
 	@brief      Contains the configuration for the OPCprotocolManager. 
 **/
typedef struct
{
    uint16_t                databaseversion;    /**< it shall match the head.dbv field in the message */
    uint16_t                numberofvariables;  /**< it tells how many variables are inside the database */
    opcprotman_var_map_t*   arrayofvariablemap; /**< contains a C array of numberofvariables items */
} opcprotman_cfg_t;



    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


// - declaration of extern public functions ---------------------------------------------------------------------------

// it must be defined in an external configuration file. see examples in OPCprotocolManager_Cfg.template.c
extern opcprotman_res_t opcprotman_personalize_database(OPCprotocolManager *p);


/** @fn         extern OPCprotocolManager * opcprotman_New(const opcprotman_cfg_t *cfg)
    @brief      Creates the OPCprotocolManager object 
    @arg        cfg         The configuration. NULL is NOT OK.
    @return     The handle to the OPCprotocolManager
 **/
extern OPCprotocolManager * opcprotman_New(const opcprotman_cfg_t *cfg);



// opcprotman_OK if it has signature, opcprotman_NOK_generic otherwise
extern opcprotman_res_t opcprotman_Has_Signature(OPCprotocolManager* p, opcprotman_message_t* msg);

extern opcprotman_res_t opcprotman_Has_Same_Database(OPCprotocolManager* p, opcprotman_message_t* msg);

extern opcprotman_res_t opcprotman_Is_Managed(OPCprotocolManager* p, opcprotman_message_t* msg);

// opcprotman_OK_withreplyif there is a reply message, 0 in case of no reply but correct message, -1 in case of error
extern opcprotman_res_t opcprotman_Parse(OPCprotocolManager* p, opcprotman_message_t* msg, opcprotman_message_t* reply, uint16_t* replysize);


extern opcprotman_res_t opcprotman_Form(OPCprotocolManager* p, opcprotman_opc_t opc, opcprotman_var_t var, const void* setdata, opcprotman_message_t* msg, uint16_t* size);

extern opcprotman_var_map_t* opcprotman_find(OPCprotocolManager* p, uint16_t var);

extern opcprotman_res_t opcprotman_personalize_var(OPCprotocolManager* p, uint16_t var, uint8_t* var_ram, opcprotman_fp_onrec_t fn);

extern uint32_t opcprotman_getSeqNum(OPCprotocolManager* p, opcprotman_message_t* msg);

/** @}            
    end of group doxy_opcprotman  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

