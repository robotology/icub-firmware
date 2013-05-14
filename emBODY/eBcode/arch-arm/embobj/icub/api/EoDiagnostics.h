/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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
#ifndef _EODISAGNOSTICS_H_
#define _EODISAGNOSTICS_H_

#ifdef __cplusplus
extern "C" {
#endif


/** @file       EoDiagnostics.h
	@brief      This header file gives 
	@author     valentina.gaggero@iit.it
	@date       09/06/2011
**/

/** @defgroup eo_cevcwervcrev5555 Configuation of the vrfverver
    Tcecece 
    
    @{		
 **/



// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
// #include "EOMtheIPnet.h"
// #include "EOMtheEMSrunner.h"



// - public #define  --------------------------------------------------------------------------------------------------





// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct eOsnsr_forcetorque_t
    @brief      eOsnsr_forcetorque_t contains measurements of force on xyz and of torque on 3 components 
 **/
typedef enum
{
    warning = 0,
    passive =1,
    busoff=2
} eOdgn_canerror_type_t;
 
typedef struct
{
    uint8_t  warning:1;     /**< This bit is set by hardware when the warning limit has been reached
                                            (Receive Error Counter or Transmit Error Counter >= 96).See standard for more information*/
    uint8_t  passive:1;     /**<  This bit is set by hardware when the Error Passive limit has been reached (Receive Error
                                             Counter or Transmit Error Counter>127). */
    uint8_t  busoff:1;      /**< This bit is set by hardware when it enters the bus-off state. The bus-off state is entered on
                                             TEC overflow, greater than 255*/
    uint8_t  rxqueueisfull:1; /**< if its value is 1 then the rx hw buffer is full.*/

    uint8_t dummy:4;
} eOdgn_canhwstatus_t; //1 B

typedef struct
{
    uint8_t txqueueisfull:1; 
    uint8_t rxqueueisfull:1;
    uint8_t dummy:6;
} eOdgn_canswstatus_t; //1 B

typedef struct 
{
    eOdgn_canhwstatus_t hw;
    eOdgn_canswstatus_t sw;
} eOdgn_canstatus_t; //2 B

typedef struct 
{
    uint8_t dummy;
} eOdgn_ethstatus_t;

typedef struct 
{
    uint8_t dummy;
} eOdgn_pwmstatus_t;

typedef struct
{
    eOdgn_canstatus_t   can_dev[2];
    eOdgn_ethstatus_t   eth_dev;
} eOdgn_emsperipheralstatus_t; //3 B

typedef struct
{
    uint8_t dummy;
} eOdgn_aea_encoder_t;

typedef struct
{
    eOdgn_aea_encoder_t   aea_enc;
} eOdgn_connecteddevices_t;


// typedef struct
// {
//     uint8_t type;
//     union
//     {
//         
//     }u;
//     
// } eOdgn_connecteddevice_t;

typedef struct
{
    uint64_t    numberofperiods;
    uint64_t    cumulativeabsoluteerrorinperiod;  
    uint32_t    meanofabsoluteerrorinperiod;
    uint32_t    movingmeanofabsoluteerrorinperiod;
    uint32_t    maxabsoluteerrorinperiod;
    uint32_t    minabsoluteerrorinperiod;  
    uint32_t    executionoverflows[3]; //enum { eo_emsrunner_task_numberof   = 3 };
    uint32_t    datagrams_failed_to_go_in_txsocket;
    uint32_t    cantxfailuretimeoutsemaphore;
} eOdgn_coreappl_runningst_t; //44 bytes


typedef struct
{
    uint32_t cantxfailuretimeoutsemaphore;
} eOdgn_coreappl_configst_t; //4 B


typedef struct
{
    eOdgn_coreappl_runningst_t    runst;
    eOdgn_coreappl_configst_t     cfgst;
} eOdgn_coreapplication_t; //48 bytes

typedef struct
{
    uint32_t    datagrams_failed_to_go_in_rxfifo;
    uint32_t    datagrams_failed_to_go_in_txosalqueue;
    uint32_t    datagrams_failed_to_be_retrieved_from_txfifo;
    uint32_t    datagrams_failed_to_be_sent_by_ipal;    
} eOdgn_ipnet_t; //equal to EOMtheIPnetDEBUG_t //16 B

typedef struct
{
    uint32_t    rxinvalidropframes;
    uint32_t    rxseqnumwrong;
    uint32_t    lostreplies;
    uint32_t    failuresinloadofreplyropframe;
    uint32_t    txropframeistoobigforthepacket;
    uint32_t    cannotloadropinregulars;
    uint32_t    cannotloadropinoccasionals;
} eOdgn_embObjtransceiver_t;  // 28 B

typedef struct
{
    uint8_t dummy;
} eOdgn_motorcontroller_t;


typedef struct
{
    uint16_t err_onReadFromSpi;
    uint16_t err_onParityError;
    uint16_t err_onInvalidValue;
} eOappEncReader_error_t;

enum{eOdgn_emsmaxnumofencoders = 6};

typedef struct
{
   eOappEncReader_error_t encList[eOdgn_emsmaxnumofencoders]; //6 = eOeOappEncReader_encoderMaxNum
} eOdgn_encoderreads_t;

typedef struct
{
    eOdgn_coreapplication_t         core;
    eOdgn_ipnet_t                   ipnet;
    eOdgn_embObjtransceiver_t       transceiver;
} eOdgn_emsapplication_common_t; // 48 + 16 + 28 = 92

typedef struct
{
    eOdgn_encoderreads_t            encreads;
    //eOdgn_motorcontroller_t         mc; si puo' mettere in EP mc??
} eOdgn_emsapplication_emswithmc_t;

typedef struct 
{
    eOdgn_emsperipheralstatus_t     periph;
    //eOdgn_connecteddevices_t     devices;
    eOdgn_emsapplication_common_t   appl;
} eOdgn_emsdiagnostic_common_t;


typedef struct 
{
    eOdgn_emsdiagnostic_common_t   appl;
    //eOdgn_connectedcanmotorboard_t  mcboards; PER ORA NON LO USEREI perche' gli unici dati importanti sono i fault sul can
    /* IO METTEREI UN EP PER I CONNECTED DEVICE: INFO TOPOLOGICHE PIU' FAULT */
} eOdgn_emsdiagnostic_emsgtwcan_t; //ems gateway can

typedef struct 
{
    eOdgn_emsdiagnostic_common_t        appl_com;
    eOdgn_emsapplication_emswithmc_t    appl_mc;
} eOdgn_emsdiagnostic_emswithmc_t;  //ems with motion controller




typedef struct
{
    uint8_t enable; //if true than all errrors are signaled else none!
}eOdgn_commands_t;

/** @}            
    end of group eo_cevcwervcrev5555  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

