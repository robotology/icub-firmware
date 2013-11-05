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
#include "EoMotionControl.h"


// - public #define  --------------------------------------------------------------------------------------------------
#define DGN_MOTOR_FAULT_UNDERVOLTAGE    0x00000001
#define DGN_MOTOR_FAULT_OVERVOLTAGE     0x00000002
#define DGN_MOTOR_FAULT_EXTERNAL        0x00000004
#define DGN_MOTOR_FAULT_OVERCURRENT     0x00000008
#define DGN_MOTOR_FAULT_I2TFAILURE      0x00000010
#define DGN_MOTOR_FAULT_CANRECWARNING   0x00000020
#define DGN_MOTOR_FAULT_CANRECERROR     0x00000040
#define DGN_MOTOR_FAULT_CANRECHWOVERRUN 0x00000080

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
    uint8_t min;
    uint8_t max;
    uint8_t dummy1;
    uint8_t dummy2;
} eOdgn_can_stat_info_t; // 4B

typedef struct
{
    eOdgn_can_stat_info_t info_rx;
    eOdgn_can_stat_info_t info_tx;
} eOdgn_can_statistics_perPort_t; //8 B

typedef struct
{
    eOdgn_can_statistics_perPort_t stat[2];
} eOdgn_can_statistics_configMode_t; //16 B


typedef struct
{
    eOdgn_can_statistics_perPort_t stat[2];  //2 = num can port
} eOdgn_can_statistics_runMode_t; //16 B

typedef struct
{
    eOdgn_can_statistics_configMode_t config_mode;
    eOdgn_can_statistics_runMode_t    run_mode;
} eOdgn_can_statistics_t; //32 B



typedef struct 
{
    uint16_t linksmask;
    uint8_t crcErrorCnt_overflow;
    uint8_t crcErrorCnt_validVal;
    uint32_t crcErrorCnt[3];
} eOdgn_ethstatus_t; //4 B

typedef struct 
{
    uint8_t dummy;
} eOdgn_pwmstatus_t;

typedef struct
{
    eOdgn_canstatus_t   can_dev[2];
    eOdgn_ethstatus_t   eth_dev;
} eOdgn_emsperipheralstatus_t; //40 B

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
    uint32_t    dummy;
} eOdgn_coreappl_runningst_t; //48 bytes


typedef struct
{
    uint32_t cantxfailuretimeoutsemaphore;
    uint32_t dummy;
} eOdgn_coreappl_configst_t; //8 B


typedef struct
{
    eOdgn_coreappl_runningst_t    runst;
    eOdgn_coreappl_configst_t     cfgst;
} eOdgn_coreapplication_t; //56 bytes

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
    uint32_t    dummy;
} eOdgn_embObjtransceiver_t;  // 32 B

typedef struct
{
    uint8_t dummy;
} eOdgn_motorcontroller_t;


typedef struct
{
    uint16_t err_onReadFromSpi;
    uint16_t err_onParityError;
    uint16_t err_onInvalidValue;
    uint16_t dummy;
} eOappEncReader_error_t; //8B

enum{eOdgn_emsmaxnumofencoders = 6};

typedef struct
{
   eOappEncReader_error_t encList[eOdgn_emsmaxnumofencoders]; //6 = eOeOappEncReader_encoderMaxNum
   uint32_t count;
   uint32_t dummy;
} eOdgn_encoderreads_t; //56B

typedef struct
{
    eOdgn_coreapplication_t         core;
    eOdgn_ipnet_t                   ipnet;
    eOdgn_embObjtransceiver_t       transceiver;
} eOdgn_emsapplication_common_t; // 56 + 16 + 32 = 104

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
} eOdgn_emsdiagnostic_common_t; //104 + 8


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
    uint8_t enable; //if true then all errrors are signaled else none!
    uint8_t signalExtFault; //if true external fault is signaled
    uint8_t signalEthCounters; 
}eOdgn_commands_t;

typedef struct
{
    uint8_t motorlist[12]; //on ems there are 12 motor max.Each element of array contains falg about the motor with id = index of array
} eOdgn_motorstatusflags_t;


    enum{eOdgn_errorlog_str_size = 250};
typedef struct
{
    char errorstate_str[eOdgn_errorlog_str_size]; //in string contains the reason of switch to error state.
} eOdgn_errorlog_t;



typedef struct
{   // time is in milli-sec
//     uint8_t     flags;
//     uint8_t     type;
//     uint8_t     dummy;
//     uint8_t     dummy2;
    int32_t     deltaprognumber;
    uint32_t    deltarxtime; 
} eOdgn_rxSetPointsInfo_t;

typedef struct
{
    eOdgn_rxSetPointsInfo_t position[4];
    eOdgn_rxSetPointsInfo_t impedence[4];
} eOdgn_rxCheckSetpoints_t;

/** @}            
    end of group eo_cevcwervcrev5555  
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

