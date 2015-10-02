/*
 * Copyright (C) 2015 iCub Facility - Istituto Italiano di Tecnologia
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
#ifndef _EOTHECANSERVICE_H_
#define _EOTHECANSERVICE_H_

#ifdef __cplusplus
extern "C" {
#endif

/** @file       EOtheCANservice.h
	@brief      This header file implements public interface to the singleton which parses and form can frames in iCub.
 	@author     marco.accame@iit.it
	@date       12 mar 2015
 **/

/** @defgroup eo_thecanservice Singleton EOtheCANservice 
    
    It manages can protocol in icub.  
  
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"    
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"
#include "EoProtocol.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 

/** @typedef    typedef struct EOtheCANservice_hid EOtheCANservice
    @brief      EOtheCANservice is an opaque struct.
 **/  
typedef struct EOtheCANservice_hid EOtheCANservice;



typedef enum
{
    eocanserv_mode_straight     = 0,    /**< in tx the canframe is put inside the can tx buffer and transmitted straight away. in rx a frame reception triggers the callback */
    eocanserv_mode_ondemand     = 1     /**< in tx the canframe is put insize can tx buffer but transmitted only at the call of eo_canserv_TXstart. in rx the frame is extracted from the rx fifo only */     
} eOcanserv_mode_t;



/**	@typedef    typedef struct eOcanserv_cfg_t 
 	@brief      Contains the configuration for the EOtheCANservice. 
 **/
typedef struct
{    
    eOcanserv_mode_t    mode;
    eOreltime_t         canstabilizationtime;           /**< in usec [0, 10*OSAL_reltime1sec], it is the time required for can boards to stabilize
                                                             so that they are able to process can messages. 
                                                             for sensor boards which are powered on by the CAN bus: 5 seconds for bootloader permanence 
                                                             plus some more time for powerup and starting application (use 7*OSAL_reltime1sec).
                                                             for mc boards which are powered on externally: the same as before minus the bootloader time.
                                                             
                                                             function eo_canserv_Initialise() locks the calling task for all this time.
                                                         **/    
    uint8_t             rxqueuesize[eOcanports_number];
    uint8_t             txqueuesize[eOcanports_number];
    eOcallback_t        onrxcallback[eOcanports_number];
    void*               onrxargument[eOcanports_number];
} eOcanserv_cfg_t;


    
// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------

extern const eOcanserv_cfg_t eo_canserv_DefaultCfg;


// - declaration of extern public functions ---------------------------------------------------------------------------



/** @fn         extern EOtheCANservice * eo_canserv_Initialise(const eOcanserv_cfg_t *canprotcfg)
    @brief      Initialise the EOtheCANservice singleton. It also powers on the CAN bus.
    @arg        canprotcfg       The configuration. NULL is OK.
    @return     The handle to the EOtheCANservice
 **/
extern EOtheCANservice * eo_canserv_Initialise(const eOcanserv_cfg_t *canprotcfg);


/** @fn         extern EOtheCANservice * eo_canserv_GetHandle(void)
    @brief      Retrieve the EOtheCANservice singleton 
    @return     The handle to the EOtheCANservice
 **/
extern EOtheCANservice * eo_canserv_GetHandle(void);

extern eOresult_t eo_canserv_SetMode(EOtheCANservice *p, eOcanserv_mode_t mode); // tx-on-event or tx-on-demand

extern eOresult_t eo_canserv_ParseAll(EOtheCANservice *p);

extern uint8_t eo_canserv_NumberOfFramesInRXqueue(EOtheCANservice *p, eOcanport_t port);

extern eOresult_t eo_canserv_Parse(EOtheCANservice *p, eOcanport_t canport, uint8_t maxnumofcanframes, uint8_t *numofreadcanframes); 


// must specify all the entries in eOcanprot_descriptor_t
extern eOresult_t eo_canserv_SendCommandToLocation(EOtheCANservice *p, eOcanprot_command_t *command, eOcanmap_location_t loc);

/** @fn         extern eOresult_t eo_canserv_Parse(EOtheCANservice *p, eOcanframe_t *frame, eOcanport_t port) 
    @brief      It parses a can frame and executes associated actions. 
    @param      p               The singleton
    @param      frame           The input can frame.
    @param      port            The can port
    @return     eores_OK if the parsing is successful, eores_NOK_nullpointer in case of NULL parameters, eores_NOK_generic if the frame is not recognised.  
 **/
// of eOcanprot_descriptor_t we need fill only ...class, type, and value
extern eOresult_t eo_canserv_SendCommandToEntity(EOtheCANservice *p, eOcanprot_command_t *command, eOprotID32_t id32); 

extern eOresult_t eo_canserv_SendCommandToAllBoardsInEntity(EOtheCANservice *p, eOcanprot_command_t *command, eOprotID32_t id32);


extern eOresult_t eo_canserv_TXstartAll(EOtheCANservice *p);

extern eOresult_t eo_canserv_TXwaitAllUntilDone(EOtheCANservice *p, uint32_t timeout);

// it is meaningful only if the mode is ... otherwise .... 
extern eOresult_t eo_canserv_TXstart(EOtheCANservice *p, eOcanport_t port, uint8_t *sizeofTXqueue);
// it is meaningful only if the mode is ... otherwise ....
extern eOresult_t eo_canserv_TXwaituntildone(EOtheCANservice *p, eOcanport_t port, uint32_t timeout);

//extern eOresult_t eo_canserv_AlertOnRX(EOtheCANservice *p, taskid); // tx-on-event or tx-on-demand

/** @}            
    end of group eo_thecanservice  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

