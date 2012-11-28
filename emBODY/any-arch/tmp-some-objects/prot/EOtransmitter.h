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
#ifndef _EOTRANSMITTER_H_
#define _EOTRANSMITTER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtransmitter.h
    @brief      This header file implements public interface to a frame.
    @author     marco.accame@iit.it
    @date       01/11/2010
**/

/** @defgroup eo_transmitter Object EOtransmitter
    The EOtransmitter object is used as ...
         
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOropframe.h"
#include "EOpacket.h"
#include "EOnvsCfg.h"
#include "EOVmutex.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct EOtransmitter_hid EOtransmitter
    @brief      EOtransmitter is an opaque struct. It is used to implement data abstraction for the datagram 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOtransmitter_hid EOtransmitter;


typedef enum
{
    eo_transmitter_protection_none      = 0,
    eo_transmitter_protection_total     = 1
} eOtransmitter_protection_t;

typedef struct
{
    uint16_t                        capacityoftxpacket;  
    uint16_t                        capacityofropframeregulars; 
    uint16_t                        capacityofropframeoccasionals;
    uint16_t                        capacityofropframereplies;
    uint16_t                        capacityofrop;
    uint16_t                        maxnumberofregularrops;
    EOnvsCfg*                       nvscfg;
    eOipv4addr_t                    ipv4addr;
    eOipv4port_t                    ipv4port;
    eov_mutex_fn_mutexderived_new   mutex_fn_new;
    eOtransmitter_protection_t      protection;    
} eo_transmitter_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eo_transmitter_cfg_t eo_transmitter_cfg_default; 


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOtransmitter* eo_transmitter_New(uint16_t capacity)
    @brief      Creates a new frame object and allocates memory able to store @e capacity bytes. If @e capacity is
                zero, then the object shall have external storage mode.
    @param      capacity   The max size of the packet.
    @return     The pointer to the required object.
 **/
 
 
 // gestisce 1 solo indirizzo ip di destinazione in modo da avere 1 solo EOpacket in uscita.
 // 
extern EOtransmitter* eo_transmitter_New(const eo_transmitter_cfg_t *cfg);

extern eOresult_t eo_transmitter_outpacket_Get(EOtransmitter *p, EOpacket **outpkt, uint16_t *numberofrops); 

// the rops in regular_rops stay forever unless unloaded one by one or all cleared. at each eo_transmitter_outpacket_Get() they are placed inside the
// packet. they however need an explicit refresh of their values. 
extern eOresult_t eo_transmitter_regular_rops_Load(EOtransmitter *p, eOropdescriptor_t* ropdesc); 
extern eOresult_t eo_transmitter_regular_rops_Unload(EOtransmitter *p, eOropdescriptor_t* ropdesc); 
extern eOresult_t eo_transmitter_regular_rops_Clear(EOtransmitter *p); 
extern eOresult_t eo_transmitter_regular_rops_Refresh(EOtransmitter *p);

// the rops in occasional_ropss are inserted with following functions, put inside the packet by function eo_transmitter_outpacket_Get()
// and after that they are cleared.

// obsolete ...
extern eOresult_t eo_transmitter_occasional_rops_Load_without_data(EOtransmitter *p, eOropdescriptor_t* ropdesc, uint8_t itisobsolete); 

extern eOresult_t eo_transmitter_occasional_rops_Load(EOtransmitter *p, eOropdescriptor_t* ropdesc);

extern eOresult_t eo_transmitter_reply_ropframe_Load(EOtransmitter *p, EOropframe* ropframe);





/** @}            
    end of group eo_transmitter  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

