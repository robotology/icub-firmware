/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
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
#ifndef _EO_ICUBCANPROTOCOL_SPECIFICATIONS_H_
#define _EO_ICUBCANPROTOCOL_SPECIFICATIONS_H_


#ifdef __cplusplus
extern "C" {
#endif

// - doxy begin -------------------------------------------------------------------------------------------------------

/* @file       EOicubCanProto_specifications.h
    @brief      This file provides interface to ICUBCANPROTOcol
    @author    valentina.gaggero@iit.it
    @date       14/02/2012
**/


// - external dependencies --------------------------------------------------------------------------------------------
#include "iCubCanProtocol.h"
#include "iCubCanProto_types.h"

// - public #define  --------------------------------------------------------------------------------------------------
// empty-section


// - declaration of public user-defined types ------------------------------------------------------------------------- 




/** @typedef    typedef     enum             eOicubCanProto_motorAxis_t
    @brief      each CAN board can manage two axises.
                Note: a can board is different from a physical board: the seccond can combine
                two CAN board in a single board object. a Can board ca be identified by unique 
                CAN address on the same CAN bus.
                Each CAN board can manage two axis at most, 
                but a physical board can manage 4 axises at most.
 **/
typedef enum
{
    eo_icubCanProto_jm_index_first      = 0,
    eo_icubCanProto_jm_index_second     = 1
} eOicubCanProto_jm_indexinBoard_t;


/** @typedef    enum { eOicubCanProto_jm_indexInBoard_max = 2 };
    @brief      this is the max number of joint or motor for each canBoard
 **/
enum { eOicubCanProto_jm_indexInBoard_max = 2 };




/** @typedef    typedef     struct             eOicubCanProto_msgDestination_t
    @brief      The destination of message belong to icub can protocol is a 
                particuar axis managed by a board.
 **/
typedef union
{
    struct 
    {
        uint16_t canAddr:8;                 /**< use  eOicubCanProto_canBoardAddress_t */
        uint16_t jm_indexInBoard:8;         /**< use eOicubCanProto_jm_indexinBoard_t.
                                                if the destination is not joint or motor, this field values 0 */
    } s;
    
    uint16_t dest;
} eOicubCanProto_msgDestination_t;

//#define ICUBCANPROTO_MSGDEST_CREATE(jm_indexInBoard, canAddr)   ((eOicubCanProto_msgDestination_t)(jm_indexInBoard<<8 | canAddr))
#define ICUBCANPROTO_MSGDEST_CREATE(jm_indexInBoard, canAddr)   ((uint16_t)(jm_indexInBoard<<8 | canAddr))
#define ICUBCANPROTO_MSGDEST_GET_INDEXINBOARD(msgdest)          (((eOicubCanProto_msgDestination_t)(msgdest)).jm_indexInBoard)



typedef struct
{
    uint16_t class:8;                   /**< use  eOicubCanProto_msgCommand_class_t */
    uint16_t cmdId:8;                   /**< use  eOicubCanProto_msgCommand_cmdId_t */
} eOicubCanProto_msgCommand_t;

#define ICUBCANPROTO_MSGCOMMAND_CREATE(class, cmdId)   ((eOicubCanProto_msgCommand_t)(class<<8 | cmdId))

enum { eOicubCanProto_bcastpolicy_maxsize = 4 };

typedef struct
{
    uint8_t val2bcastList[eOicubCanProto_bcastpolicy_maxsize];           //list of value to broadcast
} eOicubCanProto_bcastpolicy_t;


typedef struct 
{
    uint8_t major;
    uint8_t minor;
} eOicubCanProto_protocolVersion_t;

typedef struct
{
    uint8_t estimShiftJointVel;
    uint8_t estimShiftJointAcc;
    uint8_t estimShiftMotorVel;
    uint8_t estimShiftMotorAcc;
} eOicubCanProto_estimShift_t;

// - declaration of extern public variables, ...deprecated: better using use _get/_set instead ------------------------
// empty-section

// - declaration of extern public functions ---------------------------------------------------------------------------

// - doxy end ---------------------------------------------------------------------------------------------------------
// empty-section

#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


