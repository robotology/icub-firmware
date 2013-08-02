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
#ifndef _EOTRANSCEIVER_H_
#define _EOTRANSCEIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtransceiver.h
    @brief      This header file implements public interface to a frame.
    @author     marco.accame@iit.it
    @date       01/11/2010
**/

/** @defgroup eo_transceiver Object EOtransceiver
    The EOtransceiver object is used as ...
         
    @{        
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOpacket.h"
#include "EOnvSet.h"
#include "EOconfirmationManager.h"
#include "EOrop.h"
#include "EOVmutex.h"




// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
  

// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct EOtransceiver_hid EOtransceiver
    @brief      EOtransceiver is an opaque struct. It is used to implement data abstraction for the datagram 
                object so that the user cannot see its private fields and he/she is forced to manipulate the
                object only with the proper public functions. 
 **/  
typedef struct EOtransceiver_hid EOtransceiver;


typedef enum
{
    eo_trans_protection_none                    = 0,
    eo_trans_protection_enabled                 = 1       
} eOtransceiver_protection_t;


typedef struct   
{
    uint16_t        capacityoftxpacket; 
    uint16_t        capacityofrop;    
    uint16_t        capacityofropframeregulars; 
    uint16_t        capacityofropframeoccasionals;
    uint16_t        capacityofropframereplies;
    uint16_t        maxnumberofregularrops;
    uint16_t        maxnumberofconfreqrops;
} eo_transceiver_sizes_t; 


typedef struct
{
    uint16_t                        capacityoftxpacket; 
    uint16_t                        capacityofrop;    
    uint16_t                        capacityofropframeregulars; 
    uint16_t                        capacityofropframeoccasionals;
    uint16_t                        capacityofropframereplies;
    uint16_t                        maxnumberofregularrops;
    uint16_t                        maxnumberofconfreqrops;
    eOipv4addr_t                    remipv4addr;           
    eOipv4port_t                    remipv4port;    
    EOnvSet*                        nvset; 
    eOconfman_cfg_t*                confmancfg;
    eov_mutex_fn_mutexderived_new   mutex_fn_new;
    eOtransceiver_protection_t      protection;
} eo_transceiver_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eo_transceiver_cfg_t eo_transceiver_cfg_default; //= {512, 128, 256, 128, 128, 16, EO_COMMON_IPV4ADDR_LOCALHOST, 10001, NULL, NULL};


// - declaration of extern public functions ---------------------------------------------------------------------------
 
 
/** @fn         extern EOtransceiver* eo_transceiver_New(uint16_t capacity)
    @brief      Creates a new frame object and allocates memory able to store @e capacity bytes. If @e capacity is
                zero, then the object shall have external storage mode.
    @param      capacity   The max size of the packet.
    @return     The pointer to the required object.
 **/
extern EOtransceiver* eo_transceiver_New(const eo_transceiver_cfg_t *cfg);

extern eOresult_t eo_transceiver_Receive(EOtransceiver *p, EOpacket *pkt, uint16_t *numberofrops, eOabstime_t* txtime); 

extern eOresult_t eo_transceiver_Transmit(EOtransceiver *p, EOpacket **pkt, uint16_t *numberofrops);

// if the variable is local then it is used the ram of the netvar. if it is remote, the ropdescr must contain data and size
extern eOresult_t eo_transceiver_OccasionalROP_Load(EOtransceiver *p, eOropdescriptor_t *ropdes);


extern eOresult_t eo_transceiver_RegularROPs_Clear(EOtransceiver *p);
extern eOresult_t eo_transceiver_RegularROP_Load(EOtransceiver *p, eOropdescriptor_t *ropdes); 
extern eOresult_t eo_transceiver_RegularROP_Unload(EOtransceiver *p, eOropdescriptor_t *ropdes); 



/** @}            
    end of group eo_transceiver  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

