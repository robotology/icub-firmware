/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Alessandro Scalzo
 * email:   marco.accame@iit.it, alessandro.scalzo@iit.it
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
#ifndef _EUPDATERPARSER_H_
#define _EUPDATERPARSER_H_

#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       eupdater_parser.h
    @brief      This header file implements ....
    @author     marco.accame@iit.it
    @date       01/11/2012
**/

/** @defgroup eupdaterinfo cedcew fcevw
    The embENV allows ...... 
 
    @todo acemor-facenda: do documentation.
    

    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"
#include "EOpacket.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section

// - declaration of public user-defined types ------------------------------------------------------------------------- 

#if 0
// all commands are either 4 bytes (and not 1 byte anymore) ... or more after the 4 ... there are exceptions ... dont touch those for programming

typedef enum 
{
    upd_CMD_SCAN            = 0xFF,
    upd_CMD_SCAN2           = 0x7F,     
    upd_CMD_START           = 0x01,
    upd_CMD_DATA            = 0x02,
    upd_CMD_JUMP            = 0x03,
    upd_CMD_END             = 0x04,
    upd_CMD_DEF2RUN         = 0x05, // former BOOT
    upd_CMD_SYS_RESTART     = 0x06, // former RESET
    upd_CMD_IP_ADDR_SET     = 0x07,
    upd_CMD_IP_MASK_SET     = 0x08,
    upd_CMD_PROCS           = 0x09,
    upd_CMD_PROCS2          = 0x79,
    upd_CMD_BLINK           = 0x0B,
    upd_CMD_UPD_ONCE        = 0x0C,
    upd_CMD_MACSET          = 0x11,
    upd_CMD_SYS_EEPROMERASE = 0x12,
    upd_CMD_CANGTW_START    = 0x20,
//    upd_CMD_CANGTW_STOP     = 0x21,
    
    upd_CMD_INFO_CLR        = 0x30,
    upd_CMD_INFO_SET        = 0x31,
    upd_CMD_INFO_GET        = 0x32
} eOupd_opcodes_t;


typedef enum 
{
    upd_RES_OK              = 0,
    upd_RES_ERR_PROT        = 1,
    upd_RES_ERR_FLASH       = 2,
    upd_RES_ERR_LOST        = 3,
    upd_RES_ERR_UNK         = 4
} eOupd_result_t;


typedef enum
{
    upd_programLOADER       = 0x55,
    upd_programUPDATER      = 0xAA,
    upd_programAPPLICATION  = 0x5A
} eOupd_proc2prog_t;

#define EOUPD_PROTOCOL_VERSION 1



typedef enum               
{
    eLoader             = 0,    /**< the eLoader: the one executed just after reset which loads any other eProcess */
    eUpdater            = 1,    /**< the eUpdater: the one responsible for performing FW update */
    eApplication        = 2,    /**< the eApplication: the standard application */
    eProcessNone        = 255   /**< none */
} eOupd_proctype_t;


typedef struct
{
    uint8_t             type;               // use eOupd_proctype_t
    uint8_t             dummy;              // if not used it must be 255
    eOversion_t         version;            // the version of the firmware
    eOdate_t            date;               // the date associated to the firmware as it is in repository
    eOdate_t            compilationdate;    // the compilation date, as set by the compiler with a macro.
} eOupd_procinfo_t;     EO_VERIFYsizeof(eOupd_procinfo_t, 12);


typedef struct 
{     
    uint8_t             numberofthem;       // number of processes. it typically is 3, but it can be 2 if the eApplication is not available   
    uint8_t             startup;            // it is the process launched by the eLoader at bootstrap. it typically is the eUpdater but it may be the eAppplication
    uint8_t             def2run;            // it is the process launched after the 5 second safe time is terminated
    uint8_t             runningnow;         // it is the process which is running at the time of processing teh reply. it may be eUpdater or eApplication 
    eOupd_procinfo_t    info[3];
} eOupd_proctable_t;    EO_VERIFYsizeof(eOupd_proctable_t, 40);

// - format of commands and of their replies (if any)

typedef struct
{
    uint8_t             opc;    // it must be upd_CMD_SCAN 
} eOupd_cmd_SCAN_t;     EO_VERIFYsizeof(eOupd_cmd_SCAN_t, 1);

typedef struct
{
    uint8_t             opc;            // it must be upd_CMD_SCAN 
    uint8_t             version_major;  // of the running process
    uint8_t             version_minor;  // of the running process
    uint8_t             dummy;          // in legacy boards it contains 0x0A
    uint8_t             ipmask[4];      // fixed at 255.255.255.0
    uint8_t             mac48[6];          
} eOupd_cmd_SCAN_REPLY_t; EO_VERIFYsizeof(eOupd_cmd_SCAN_REPLY_t, 14);



// typical reply
typedef struct 
{   // keep opc | res as first for backwards compatibility with old protocol
    uint8_t             opc;            // use eOupd_opcodes_t
    uint8_t             res;            // use eOupd_result_t
    uint8_t             protversion;    // use EOUPD_PROTOCOL_VERSION
    uint8_t             sizeofextra;    // put in here the size of the extra data in the reply
} eOupd_cmdREPLY_t;     EO_VERIFYsizeof(eOupd_cmdREPLY_t, 4);


typedef struct
{
    uint8_t             opc;    // it must be upd_CMD_SCAN to keep the backwards compatibility
    uint8_t             opc2;   // it must be upd_CMD_SCAN2
    uint8_t             dummy[2];
} eOupd_cmd_SCAN2_t;    EO_VERIFYsizeof(eOupd_cmd_SCAN2_t, 4);

typedef struct
{
    eOupd_cmdREPLY_t    reply;              // use reply.cmd = upd_CMD_SCAN2
    uint8_t             boardtype;          // use a eObrd_type_t value ... which must be of eth type
    uint8_t             dummy;              // if not used it must be 255
    uint8_t             mac48[6];           // the mac address so that positions 0, 1, 2 hold the oui (Organizationally Unique Identifier) positions 3, 4, 5 the others
    eOupd_proctable_t   processes;
    uint8_t             boardinfo32[32];    // where: boardinfo32[0] = strlen(&boardinfo32[1]) but if boardinfo32[0] is 255 then it means that it is not used
} eOupd_cmd_SCAN2_REPLY_t; EO_VERIFYsizeof(eOupd_cmd_SCAN2_REPLY_t, 84);


typedef struct
{
    uint8_t             opc;        // it must be upd_CMD_PROCS to keep the backwards compatibility
    uint8_t             opc2;       // it must be upd_CMD_PROCS2
    uint8_t             sendextrastring;
    uint8_t             dummy;
} eOupd_cmd_PROCS2_t;   EO_VERIFYsizeof(eOupd_cmd_PROCS2_t, 4);


typedef struct
{   // it is equal to eOupd_cmd_SCAN2_REPLY_t and then there is 
    eOupd_cmdREPLY_t    reply;   
    uint8_t             boardtype;          // use a eObrd_type_t value ... which must be of eth type
    uint8_t             dummy;              // if not used it must be 255
    uint8_t             mac48[6];           // the mac address so that positions 0, 1, 2 hold the oui (Organizationally Unique Identifier) positions 3, 4, 5 the others
    eOupd_proctable_t   processes;
    uint8_t             boardinfo32[32];    // where: boardinfo32[0] = strlen(&boardinfo32[1]) but if boardinfo32[0] is 255 then it means that it is not used
    uint8_t             hasextrastring; // tells if there is a string which follows. the string can have at most 384 chars
    uint8_t             extrastring[3];
} eOupd_cmd_PROCS2_REPLY_t; //EO_VERIFYsizeof(eOupd_cmd_PROCS2_REPLY_t, 88);


typedef struct
{
    uint8_t             opc;    // it must be upd_CMD_START 
    uint8_t             proc;   // use eOupd_proc2prog_t
    uint8_t             filler[2];
} eOupd_cmd_START_t;    EO_VERIFYsizeof(eOupd_cmd_START_t, 4);


typedef struct
{   
    eOupd_cmdREPLY_t    reply;   
} eOupd_cmd_START_REPLY_t; EO_VERIFYsizeof(eOupd_cmd_START_REPLY_t, 4);

typedef struct
{
    uint8_t             opc;            // it must be upd_CMD_DATA 
    uint8_t             address[4];     // contains the flash address where to write the data. we use little endian convention (i.e., address[0] is the lsb).
    uint8_t             size[2];        // contains the size of the data to be written. we use little endian convention (i.e., size[0] is the lsb). 
    uint8_t             data[512];        // data is of variable size and up to 512 bytes
    uint8_t             filler;
} eOupd_cmd_DATA_t;     EO_VERIFYsizeof(eOupd_cmd_DATA_t, 520);


typedef struct
{   
    eOupd_cmdREPLY_t    reply;   
} eOupd_cmd_DATA_REPLY_t; EO_VERIFYsizeof(eOupd_cmd_DATA_REPLY_t, 4);


typedef struct
{
    uint8_t             opc;            // it must be upd_CMD_START 
    uint8_t             number[2];      // number of packets of type eOupd_cmd_DATA_t + 1. we use little endian convention.
    uint8_t             filler;
} eOupd_cmd_END_t;    EO_VERIFYsizeof(eOupd_cmd_END_t, 4);


typedef struct
{   
    eOupd_cmdREPLY_t    reply;   
} eOupd_cmd_END_REPLY_t; EO_VERIFYsizeof(eOupd_cmd_END_REPLY_t, 4);

// former BOOT
typedef struct
{
    uint8_t             opc;        // it must be upd_CMD_DEF2RUN 
    uint8_t             proc;       // it can be either eUpdater or eApplication
    uint8_t             filler[2];
} eOupd_cmd_DEF2RUN_t;  EO_VERIFYsizeof(eOupd_cmd_DEF2RUN_t, 4);

// former RESET
typedef struct
{
    uint8_t             opc;        // it must be upd_CMD_SYS_RESTART 
    uint8_t             filler[3];
} eOupd_cmd_SYS_RESTART_t;  EO_VERIFYsizeof(eOupd_cmd_SYS_RESTART_t, 4);

// former IPSET
typedef struct
{
    uint8_t             opc;            // it must be upd_CMD_IP_ADDR_SET 
    uint8_t             address[4];     // from 0 to 3: 10.0.1.99
    uint8_t             sysrestart;     // if 1 the system also restarts so that the IP change is effective immediately.
    uint8_t             filler[2];
} eOupd_cmd_IP_ADDR_SET_t;  EO_VERIFYsizeof(eOupd_cmd_IP_ADDR_SET_t, 8);

// former MASKSET
typedef struct
{
    uint8_t             opc;            // it must be upd_CMD_IP_MASK_SET 
    uint8_t             mask[4];        // from 0 to 3: 255.255.255.0
    uint8_t             sysrestart;     // if 1 the system also restarts so that the IP change is effective immediately.
    uint8_t             filler[2];
} eOupd_cmd_IP_MASK_SET_t;  EO_VERIFYsizeof(eOupd_cmd_IP_MASK_SET_t, 8);


typedef struct
{
    uint8_t             opc;            // it must be upd_CMD_BLINK 
    uint8_t             filler[3];
} eOupd_cmd_BLINK_t;  EO_VERIFYsizeof(eOupd_cmd_BLINK_t, 4);


// former CMD_UPD_ONCE
typedef struct
{
    uint8_t             opc;            // it must be upd_JUMP2UPDATER
    uint8_t             filler[3];
} eOupd_cmd_JUMP2UPDATER_t;  EO_VERIFYsizeof(eOupd_cmd_JUMP2UPDATER_t, 4);

#endif

// - declaration of extern public variables, ... but better using use _get/_set instead -------------------------------



// - declaration of extern public functions ---------------------------------------------------------------------------


extern void eupdater_parser_evalwhenjumping(void);

extern eObool_t eupdater_parser_process_ethcmd(EOpacket *rxpkt, EOpacket *txpkt);

extern eObool_t eupdater_parser_stay_forever(void);

extern void eupdater_parser_cangtw_activate(void);

extern eObool_t eupdater_parser_cangtw_isactivated(void);


extern void eupdater_parser_download_blinkled_start(void);
extern void eupdater_parser_download_toggleled(void);
extern void eupdater_parser_download_blinkled_stop(void);
 

/** @}            
    end of group eupdaterparser 
 **/

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
