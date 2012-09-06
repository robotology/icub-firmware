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
#ifndef _EOICUBCANPROTO_HID_H_
#define _EOICUBCANPROTO_HID_H_

#ifdef __cplusplus
extern "C" {
#endif
// - doxy -------------------------------------------------------------------------------------------------------------

/* @file       EOicubCanProto_hid.h
    @brief      This header file implements hidden interface to EOicubCanProto.
    @author     valentina.gaggero@iit.it
    @date       03/08/2012
 **/


// - external dependencies --------------------------------------------------------------------------------------------
#include "Eocommon.h"
#include "EoUtilities.h"

#include "EOconstLookupTbl.h"
#include "EOconstLookupTbl_hid.h"

#include "EOemsCanNetworkTopology.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
#include "EOicubCanProto.h"



// - definition of the hidden struct implementing the object ---------------------------------------------------------- 
struct EOicubCanProto_hid 
{
    const EOconstLookupTbl  *msgClasses_LUTbl__ptr;  /**< ....             */      
    EOemsCanNetTopo         *emsCanNetTopo_ptr;      /**< ....             */
};

typedef enum
{
  eo_icubCanProto_hid_opParse = 0,
  eo_icubCanProto_hid_opForm = 1  
} eo_icubCanProto_hid_operation_t;


/*this struct contains the needed information to exception function */
typedef struct
{
    eo_icubCanProto_hid_operation_t op;
    EOicubCanProto                  *iproto_ptr;
    union
    {
        struct 
        {
            eo_icubCanProto_msgCommand_cmdId_t  cmdId;
            eOcanframe_t                   *frame;
            eOcanport_t                    canPort;        
        }p_arg;
        
        struct
        {
            eo_icubCanProto_msgCommand_cmdId_t  cmdId;
            eo_icubCanProto_msgDestination_t    dest;
            void                                *value;                              
            eOcanframe_t                   *frame;      
        }f_arg;    
    }u;            
} eo_icubCanProto_hid_exceptionFn_argument_t;

/** @typedef     eo_icubCanProto_parser_fn_t
    @brief       ptr to parser-function. 
 **/
typedef eOresult_t  (*eo_icubCanProto_hid_parserFn_t)  (EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPort);


/** @typedef     eo_icubCanProto_former_fn_t
    @brief       ptr to former-function.
 **/
typedef eOresult_t  (*eo_icubCanProto_hid_formerFn_t)  (EOicubCanProto* p, void*, eo_icubCanProto_msgDestination_t, eOcanframe_t*);

/** @typedef     eo_icubCanProto_hid_LUTbl_item_parserFnHandling_t
    @brief       it is an item in parser-table. it contains fn ptr to parser function. 
                 Note: if you need insert here other stuff regarding sigle command parser  
 **/
typedef struct
{
    eo_icubCanProto_hid_parserFn_t   parser;
} eo_icubCanProto_hid_LUTbl_item_parserFnHandling_t;



/** @typedef     eo_icubCanProto_hid_LUTbl_item_formerFnHandling_t
    @brief       it is an item in former-table. it contains fn ptr to former function. 
                 Note: if you need insert here other stuff regarding sigle command parser  
 **/
typedef struct
{
    eo_icubCanProto_hid_formerFn_t   former;
} eo_icubCanProto_hid_LUTbl_item_formerFnHandling_t;


/** @typedef     eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t
    @brief       it is an item message class table. it contains fn ptr to former and parser table for each class. 
 **/
typedef struct
{
    const EOconstLookupTbl           *const parserTbl;
    const EOconstLookupTbl           *const formerTbl;
    eo_icubCanProto_msgCommand_cmdId_t (*eo_icubCanProto_getCmdId_fromFrame) (eOcanframe_t *canFrame);
} eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t;


// - declaration of extern hidden functions ---------------------------------------------------------------------------
extern __inline eOboolvalues_t eo_icubCanProto_hid_LUTbl_indexIsInTbl(const EOconstLookupTbl *tbl, uint8_t index)
{
    return((eOboolvalues_t)((index >= tbl->offset) &&((index - tbl->offset)< tbl->capacity)));
}

extern __inline eOresult_t eo_icubCanProto_hid_LUTbl_excParse(EOicubCanProto* p,
                                                              const EOconstLookupTbl *tbl, // lookup table
                                                              eo_icubCanProto_msgCommand_cmdId_t cmdId,                     //command used to select parser func
                                                              eOcanframe_t *frame,                          //frame to parser
                                                              eOcanport_t canPortRX)                        //port where was received
{

     eo_icubCanProto_hid_exceptionFn_argument_t exFn_arg = 
     {
        EO_INIT(.op)          eo_icubCanProto_hid_opParse,
        EO_INIT(.iproto_ptr)    p,
        EO_INIT(.u)
        {
            EO_INIT(.p_arg)
            {
                EO_INIT(.cmdId)     cmdId,
                EO_INIT(.frame)     frame,
                EO_INIT(.canPort)   canPortRX,
            }      
        }
     
     };
    return(tbl->exceptionMngFn((void*)&exFn_arg));    
}
                                                             
extern __inline eOresult_t eo_icubCanProto_hid_LUTbl_excForm(EOicubCanProto* p,
                                                             const EOconstLookupTbl *tbl, // lookup table
                                                             eo_icubCanProto_msgCommand_cmdId_t cmdId,                     //command used to select parser func 
                                                             void *value,                                       //value to set in frame
                                                             eo_icubCanProto_msgDestination_t dest,                  //axis of joint
                                                             eOcanframe_t *frame)                          //output frame
{
     eo_icubCanProto_hid_exceptionFn_argument_t exFn_arg = 
     {
        EO_INIT(.op)          eo_icubCanProto_hid_opForm,
        EO_INIT(.iproto_ptr)    p,
        EO_INIT(.u)
        {
            EO_INIT(.f_arg)
            {
                EO_INIT(.cmdId)     cmdId,
                EO_INIT(.dest)      dest,
                EO_INIT(.value)     value,
                EO_INIT(.frame)     frame,
            }
         }
     };
    return(tbl->exceptionMngFn((void*)&exFn_arg));      
}


extern __inline eo_icubCanProto_msgCommand_class_t eo_icubCanProto_hid_getMsgClassFromFrameId(uint32_t frame_id)
{
    return((eo_icubCanProto_msgCommand_class_t)((frame_id & 0x700) >> 8));
}

extern __inline eo_icubCanProto_canBoardAddress_t eo_icubCanProto_hid_getSourceBoardAddrFromFrameId(uint32_t frame_id)
{
    return((eo_icubCanProto_canBoardAddress_t)((frame_id & 0x0F0) >> 4));
}

extern __inline eo_icubCanProto_motorAxis_t eo_icubCanProto_hid_getmotorAxisFromFrame(eOcanframe_t *frame)
{
    return((eo_icubCanProto_motorAxis_t)((frame->data[0] & 0x8) >> 4));
}


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



