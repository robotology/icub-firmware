/* 
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

#ifndef _BOOTLOADER_PARSER_H_
#define _BOOTLOADER_PARSER_H_

// - doxy begin -------------------------------------------------------------------------------------------------------

/** @file       bootloader_parser.h
    @brief      this file provides parser module's interface. 
                the parser verifies if a received message is correct and asks the caller to do appropriate action.
    @author     valentina.gaggero@iit.it / marco.accame@iit.it
    @date       10/03/2011
**/


/** @defgroup bl_parser parser for bootloader of boards with dspic33
    The ... allows ... 
 
    @todo acemor-facenda: do documentation.
    
    
    @{        
 **/


// - external dependencies --------------------------------------------------------------------------------------------

#include "stdint.h"
#include "eEcommon.h"

#include "hal_can.h"
#include "bootloader_sm.h"


// - public #define  --------------------------------------------------------------------------------------------------
// empty-section
 
// - declaration of public user-defined types ------------------------------------------------------------------------- 


/** @typedef    typedef struct blparser_cfg_t
    @brief      blparser_cfg_t contains configuration of the parser module
 **/ 
typedef struct
{
    uint8_t     dummy;
} blparser_cfg_t;


/** @typedef    typedef enum blparser_opc_t
    @brief      blparser_opc_t contains the opcodes of the messages managed by the parser.
 **/ 
typedef enum
{
    blparser_opc_unknown              = 0,  /**< unknown message */
    blparser_opc_setboardaddress,           /**< it sets a can address to the device */
    blparser_opc_getadditionalinfo,         /**< it requests additional information about the device */
    blparser_opc_broadcast,                 /**< it puts the device in update state for fw upgrade */
    blparser_opc_board,                     /**< it moves the device in a substate ready for receiving the fw address and data */                    
    blparser_opc_address,                   /**< it contains the address of bytes being sent with the data command */ 
    blparser_opc_data,                      /**< it contains the bytes of the new fw image */  
    blparser_opc_start,                     /**< it moves into a substate in which some fw update operations can be completed */
    blparser_opc_end,                       /**< it forces a restart of the device with the new fw */ 
    blparser_opc_enablelog                  /**< it enables or disable some feedback messages to the updating host */
} blparser_opc_t;


/** @typedef    typedef struct blparser_senderinfo_t
    @brief      blparser_senderinfo_t contains information about the sender of the command
 **/ 
typedef struct
{
    uint32_t            canid;              /**< the can id of the sender */
    uint8_t             cmd;                /**< the received cmd code */
    uint8_t             errorinframe;       /**< tells if the can frame is badly formed */
} blparser_senderinfo_t;


/** @typedef    typedef struct parser_cmd_any_t
    @brief      parser_cmd_any_t is the generic cmd that the parser decodes for the caller.
                its use is as a cast to the various blparser_cmd_*_t types.
 **/ 
typedef struct
{
    blparser_senderinfo_t   sender;         /**< information about the sender of the message */
    blparser_opc_t          opc;            /**< the opc of the decoded command */
    uint8_t                 mem[15];        /**< memory holder for the fields of the specialised command types */
} blparser_cmd_any_t;


/** @typedef    typedef struct blparser_cmd_setboardaddress_t
    @brief      blparser_cmd_setboardaddress_t contains the fields specifics to the blparser_opc_setboardaddress opcode.
 **/ 
typedef struct
{
    blparser_senderinfo_t   sender;         /**< information about the sender of the message */
    blparser_opc_t          opc;            /**< the opc of the decoded command */
    uint8_t                 adr;            /**< the target address */
} blparser_cmd_setboardaddress_t;


/** @typedef    typedef struct blparser_cmd_getadditionalinfo_t
    @brief      blparser_cmd_getadditionalinfo_t contains the fields specifics to the blparser_opc_getadditionalinfo opcode.
 **/
typedef struct
{
    blparser_senderinfo_t   sender;         /**< information about the sender of the message */
    blparser_opc_t          opc;            /**< the opc of the decoded command */
} blparser_cmd_getadditionalinfo_t;


/** @typedef    typedef struct blparser_cmd_broadcast_t
    @brief      blparser_cmd_broadcast_t contains the fields specifics to the blparser_opc_broadcast opcode.
 **/
typedef struct
{
    blparser_senderinfo_t   sender;         /**< information about the sender of the message */
    blparser_opc_t          opc;            /**< the opc of the decoded command */
} blparser_cmd_broadcast_t;


/** @typedef    typedef struct blparser_cmd_board_t
    @brief      blparser_cmd_board_t contains the fields specifics to the blparser_opc_board opcode.
 **/
typedef struct
{
    blparser_senderinfo_t   sender;         /**< information about the sender of the message */
    blparser_opc_t          opc;            /**< the opc of the decoded command */
    uint8_t                 updateeeprom;   /**< tells if the eeprom (permanent application data) must be updated */
} blparser_cmd_board_t;


/** @typedef    typedef struct blparser_cmd_address_t
    @brief      blparser_cmd_address_t contains the fields specifics to the blparser_opc_address opcode.
 **/
typedef struct
{
    blparser_senderinfo_t   sender;         /**< information about the sender of the message */
    blparser_opc_t          opc;            /**< the opc of the decoded command */
    uint8_t                 numbytes;       /**< the data bytes following that address */
    uint8_t                 addressval[4];  /**< the address */
} blparser_cmd_address_t;


/** @typedef    typedef struct blparser_cmd_data_t
    @brief      blparser_cmd_data_t contains the fields specifics to the blparser_opc_data opcode.
 **/
typedef struct
{
    blparser_senderinfo_t   sender;         /**< information about the sender of the message */
    blparser_opc_t          opc;            /**< the opc of the decoded command */
    uint8_t                 size;           /**< the number of bytes contained in the data field */
    uint8_t                 data[8];        /**< the data bytes */
} blparser_cmd_data_t;


/** @typedef    typedef struct blparser_cmd_start_t
    @brief      blparser_cmd_start_t contains the fields specifics to the blparser_opc_start opcode.
 **/
typedef struct
{
    blparser_senderinfo_t   sender;         /**< information about the sender of the message */
    blparser_opc_t          opc;            /**< the opc of the decoded command */
} blparser_cmd_start_t;


/** @typedef    typedef struct blparser_cmd_end_t
    @brief      blparser_cmd_end_t contains the fields specifics to the blparser_opc_end opcode.
 **/
typedef struct
{
    blparser_senderinfo_t   sender;         /**< information about the sender of the message */
    blparser_opc_t          opc;            /**< the opc of the decoded command */
} blparser_cmd_end_t;


/** @typedef    typedef struct blparser_cmd_enablelog_t
    @brief      blparser_cmd_enablelog_t contains the fields specifics to the blparser_opc_enablelog opcode.
 **/
typedef struct
{
    blparser_senderinfo_t   sender;         /**< information about the sender of the message */
    blparser_opc_t          opc;            /**< the opc of the decoded command */
    uint8_t                 enable;         /**< boolean value indicating if the log is enabled or not */
} blparser_cmd_enablelog_t;


    
// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------

extern const blparser_cfg_t blparser_cfg_default; // = { .dummy = 0}


// - declaration of extern public functions ---------------------------------------------------------------------------

/** @fn         extern void blparser_init(const blparser_cfg_t *cfg);
    @brief      initialises the parser module 
    @param      cfg         the configuration (NULL takes the default value)
    @return     ee_res_OK
 **/
extern eEresult_t blparser_init(const blparser_cfg_t *cfg);



/** @fn         extern eEresult_t blparser_parse_canframe(hal_can_frame_t *frame, const blparser_cmd_any_t **cmd)
    @brief      parses a received can frame and gives back a decoded command. it does not filter away any command.
    @param      frame               the received CAN frame
    @param      cmd                 the command contained inside the CAN frame
    @return     ee_res_OK or ee_res_NOK_generic if the can frame is empty
 **/
extern eEresult_t blparser_parse_canframe(hal_can_frame_t *frame, const blparser_cmd_any_t **cmd);


// - doxy end ---------------------------------------------------------------------------------------------------------

/** @}            
    end of group bl_parser 
 **/

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


