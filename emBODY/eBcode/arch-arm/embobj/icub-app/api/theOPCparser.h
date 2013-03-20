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
#ifndef _THEOPCPARSER_H_
#define _THEOPCPARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       theOPCparser.h
	@brief      This header file implements public interface to the opc parser singleton.
 	@author     marco.accame@iit.it
	@date       03/19/2013
 **/

/** @defgroup doxy_opcparser theOPCparser singleton 
    
    The .....  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"



// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

enum { opcparser_signature = 0xDE };

/** @typedef    typedef struct theOPCparser_hid theOPCparser
    @brief      theOPCparser is an opaque struct. It is used to implement data abstraction for the 
                object so that the user cannot see its private fields so that he/she is forced to manipulate the
                object only with the proper public functions
 **/  
typedef struct theOPCparser_hid theOPCparser;


typedef enum  
{
    opcparser_OK_withreply      =  1,       /**< correct result: with replay */
    opcparser_OK                =  0,       /**< correct result: generic */
    opcparser_NOK_generic       = -1        /**< error: generic */
} opcparser_res_t;

typedef enum
{
    opcparser_opc_ask   = 0,
    opcparser_opc_say   = 1,
    opcparser_opc_set   = 2,
    opcparser_opc_sig   = 3,
    opcparser_opc_none  = 127,
} opcparser_opc_t;

typedef uint16_t opcparser_dbv_t;
typedef uint16_t opcparser_var_t;


typedef struct
{
    uint8_t             sgn;        /**< contains a signature for the opc. it is teh first byte of the packet and must be opcparser_signature  */
    uint8_t             opc;        /**< use opcparser_opc_t values */
    uint16_t            dbv;        /**< version of the used database */
    uint16_t            var;        /**< identifier of the variable to be manipulated */
    uint16_t            len;        /**< length of the data field */
} opcparser_header_t; 


// use only as a pointer
typedef struct
{
    opcparser_header_t  head;
    uint32_t            data[1];
} opcparser_message_t; 


typedef struct opcparser_var_map_T opcparser_var_map_t;

typedef void (*opcparser_fp_onset_t)(opcparser_var_map_t* map, void* recdata);
typedef void (*opcparser_fp_onsig_t)(opcparser_var_map_t* map, void* recdata);

struct opcparser_var_map_T
{
    uint16_t                var;
    uint16_t                size;
    void*                   ptr;
    opcparser_fp_onset_t    onset;  // calls a opcparser_var_map_t* and rx data
    opcparser_fp_onsig_t    onsig;  // calls a opcparser_var_map_t* and rx data
};



/**	@typedef    typedef struct opcparser_cfg_t 
 	@brief      Contains the configuration for the theOPCparser. 
 **/
typedef struct
{
    uint16_t                databaseversion;    /**< it shall match the head.dbv field in the message */
    uint16_t                numberofvariables;  /**< it tells how many variables are inside the database */
    opcparser_var_map_t*    arrayofvariablemap; /**< contains a C array of numberofvariables items */
} opcparser_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------
// empty-section


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern theOPCparser * opcparser_Initialise(const opcparser_cfg_t *cfg)
    @brief      Initialise the theOPCparser singleton 
    @arg        cfg         The configuration. NULL is NOT OK.
    @return     The handle to the theOPCparser
 **/
extern theOPCparser * opcparser_Initialise(const opcparser_cfg_t *cfg);


/** @fn         extern theOPCparser * opcparser_GetHandle(void)
    @brief      Retrieve the theOPCparser singleton 
    @return     The handle to the theOPCparser
 **/
extern theOPCparser * opcparser_GetHandle(void);


// opcparser_OK if it has signature, opcparser_NOK_generic otherwise
extern opcparser_res_t opcparser_Has_Signature(theOPCparser* p, opcparser_message_t* msg);

// opcparser_OK_withreplyif there is a reply message, 0 in case of no reply but correct message, -1 in case of error
extern opcparser_res_t opcparser_Parse(theOPCparser* p, opcparser_message_t* msg, opcparser_message_t* reply, uint16_t* replysize);


extern opcparser_res_t opcparser_Form(theOPCparser* p, opcparser_opc_t opc, opcparser_var_t var, opcparser_message_t* msg, uint16_t* size);

/** @}            
    end of group doxy_opcparser  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

