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
#ifndef _EOROP_H_
#define _EOROP_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOrop.h
    @brief      This header file implements public interface to a remote oepration rop.
    @author     marco.accame@iit.it
    @date       09/06/2011
**/

/** @defgroup eo_rop Object EOrop
    The EOrop object contains a remote operation as received by the network.

     
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOnv.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct EOrop_hid EOrop
    @brief      EOaction is an opaque struct. It is used to implement data abstraction for the datagram 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOrop_hid EOrop;


typedef uint8_t eOropcode_t;

/** @typedef    typedef enum eOropcodevalue_t
    @brief      eOropcodevalue_t contains possible ROP codes.
 **/
typedef enum
{
    eo_ropcode_none         = 0,
    eo_ropcode_ask          = 1,
    eo_ropcode_say          = 2,
    eo_ropcode_set          = 3,
    eo_ropcode_sig          = 4,
    eo_ropcode_rst          = 5
} eOropcodevalues_t;

enum { eo_ropcodevalues_numberof = 6 };


/** @typedef    typedef struct eOropctrl_t
    @brief      eOropctrl_t contains the configuration for optional fields of a ROP. It is also used as first byte of a ROP.
 **/
typedef struct
{
    uint8_t         confinfo    : 2;        /**< it is formed by bits 0 and 1. it contains confirmation info as ine eOropconfinfo_t */
    uint8_t         plustime    : 1;        /**< if 1 the ROP has the time field of 8 bytes */
    uint8_t         plussign    : 1;        /**< if 1 the ROP has the signature field of 4 bytes   */
    uint8_t         rqsttime    : 1;        /**< if 1 the ROP requests that a reply shall have the time field */
    uint8_t         rqstconf    : 1;        /**< if 1 the ROP requests that the received shall send confirmation */
    uint8_t         version     : 2;        /**< it is formed by bits 7 and 6 of the first byte of the ROP. It must be zero  */
} eOropctrl_t;      EO_VERIFYsizeof(eOropctrl_t, 1);




/** @typedef    typedef enum eOropconfinfo_t
    @brief      eOropconfinfo_t contains possible confirmation values that a ROP may have.
 **/
typedef enum
{
    eo_ropconf_none = 0,
    eo_ropconf_nak  = 1,
    eo_ropconf_ack  = 2
} eOropconfinfo_t;



typedef struct      // 04 bytes
{
    eOnvID32_t      id32;
//     eObool_t        plustime;
//     uint8_t         filler[3];
} eOropSIGcfg_t;    EO_VERIFYsizeof(eOropSIGcfg_t, 4);


typedef struct      // 24 bytes
{
    eOropctrl_t             control;            // 1B
    eOropcode_t             ropcode;            // 1B
    uint16_t                size;               // 2B
    eOnvID32_t              id32;               // 4B
    uint8_t*                data;               // 4B
    uint32_t                signature;          // 4B
    uint64_t                time;               // 4B
} eOropdescriptor_t;        //EO_VERIFYsizeof(eOropdescriptor_t, 24); 


// the minimum size is just an header (e.g., the ask command)
enum { eo_rop_minimumsize = 8 };

    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------


extern const eOropctrl_t eok_ropctrl_basic; // no time, no sign, no conf request, no time request, ... nothing

// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOrop* eo_rop_New(void)
    @brief      Creates a new rop object. 
    @return     The pointer to the required object.
 **/
extern EOrop* eo_rop_New(uint16_t capacity);


/** @fn         extern eOresult_t eo_rop_Reset(EOrop *p)
    @brief      Resets a rop object. 
    @return     Always success unless argument is NULL.
 **/
extern eOresult_t eo_rop_Reset(EOrop *p);


/** @fn         extern eOresult_t eo_rop_Process(EOrop *p, EOrop *replyrop)
    @brief      Operates on a EOrop and executes whatever is specified by the ROP. If needed it also prepares a 
                ROP to send back to the sender. If a reply ROP is needed then the ropcode field shall be a valid one
                and different from eo_ropcode_none
    @param      p           The input ROP
    @param      replyrop    Pointer to a ROP to be sent back to the sender of @e p. If a reply is needed this shall contain
                            the ROP to use, otherwise a resetted EOrop.
    @return     Always success unless any argument is NULL.
 **/
extern eOresult_t eo_rop_Process(EOrop *p, EOrop *replyrop);


/** @fn         extern eOropcode_t eo_rop_GetROPCode(EOrop *p)
    @brief      Returns the ROP code.
    @param      p           The EOrop object 
    @return     The ropcode if the object is valid, or eo_ropcode_none if NULL
 **/
extern eOropcode_t eo_rop_GetROPcode(EOrop *p);

extern uint8_t* eo_rop_GetROPdata(EOrop *p);

extern uint16_t eo_rop_ComputeSize(eOropctrl_t ropctrl, eOropcode_t ropc, uint16_t sizeofdata);

extern eOropcode_t eo_rop_ropcode_Get(EOrop *p);

extern eOropconfinfo_t eo_rop_ropconfinfo_Get(EOrop *p);



/** @}            
    end of group eo_rop  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

