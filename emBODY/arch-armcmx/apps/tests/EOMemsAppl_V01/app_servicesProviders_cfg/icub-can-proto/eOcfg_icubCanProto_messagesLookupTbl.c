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


/* @file       eOcfg_icubCanProto_messagesLookupTbl.c
    @brief      This file keeps ...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "EOcommon.h"
#include "EOicubCanProto.h"
#include "EOicubCanProto_specifications.h"
#include "EOicubCanProto_hid.h"

#include "EOconstLookupTbl.h"
#include "EOconstLookupTbl_hid.h"

#include "eOcfg_icubCanProto_motorBoardMsgLookupTbl.h"
#include "eOcfg_icubCanProto_sensorBoardMsgLookupTbl.h"
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "eOcfg_icubCanProto_messagesLookupTbl.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
/* max num of msg classes managed by lookup tbl.Currently loader class is not used*/
#define eo_icubCanProto_msgClasses_inTbl_max                       5 

/* max num of existing msg classes. (see eo_icubCanProtocol_messages.h)*/
#define eo_icubCanProto_msgClasses_maxNum                          icubCanProto_CLASS_MAXNUM +1 //skin class



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static eOicubCanProto_msgCommand_cmdId_t s_eo_icubCanProto_getCmdId_fromFrame_ofPeriodicClass(eOcanframe_t *frame);
static eOicubCanProto_msgCommand_cmdId_t s_eo_icubCanProto_getCmdId_fromFrame_ofPollingClass(eOcanframe_t *frame);
static eOicubCanProto_msgCommand_cmdId_t s_eo_icubCanProto_getCmdId_fromFrame_ofSkinClass(eOcanframe_t *frame);

static eOresult_t s_eo_icubCanProto_msgClasses_excFn(void *arg);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

/* DECLARATION OF MSG CLASS BODY TBL */                           
static const eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t s_msgClasses_tblsReference_list[eo_icubCanProto_msgClasses_inTbl_max] = 
{
    {   // 0  ==> ICUBCANPROTO_CLASS_POLLING_MOTORBOARD
        EO_INIT(.parserTbl)                           &icubCanProto_pollingMotorBoardMsg_parser_LUTbl,
        EO_INIT(.formerTbl)                           &icubCanProto_pollingMotorBoardMsg_former_LUTbl,
        EO_INIT(.eo_icubCanProto_getCmdId_fromFrame)  s_eo_icubCanProto_getCmdId_fromFrame_ofPollingClass
    },

    {   // 1
        EO_INIT(.parserTbl)                           &icubCanProto_periodicMotorBoardMsg_parser_LUTbl,
        EO_INIT(.formerTbl)                           &icubCanProto_periodicMotorBoardMsg_former_LUTbl,
        EO_INIT(.eo_icubCanProto_getCmdId_fromFrame)  s_eo_icubCanProto_getCmdId_fromFrame_ofPeriodicClass
    },


    {   // 2
        EO_INIT(.parserTbl)                           &icubCanProto_pollingSensorBoardMsg_parser_LUTbl,
        EO_INIT(.formerTbl)                           &icubCanProto_pollingSensorBoardMsg_former_LUTbl,
        EO_INIT(.eo_icubCanProto_getCmdId_fromFrame)  s_eo_icubCanProto_getCmdId_fromFrame_ofPollingClass
    },


    {   // 3
        EO_INIT(.parserTbl)                           &icubCanProto_periodicSensorBoardMsg_parser_LUTbl,
        EO_INIT(.formerTbl)                           NULL,
        EO_INIT(.eo_icubCanProto_getCmdId_fromFrame)  s_eo_icubCanProto_getCmdId_fromFrame_ofPeriodicClass 
    },
    //in order to use the same mecchanism of other messages i used class message id = 4,
    //currently not used, to parse/former skin's messages.
    {   // 4
        EO_INIT(.parserTbl)                           &icubCanProto_periodicSkinMsg_parser_LUTbl,
        EO_INIT(.formerTbl)                           &icubCanProto_pollingSkinMsg_former_LUTbl, 
        EO_INIT(.eo_icubCanProto_getCmdId_fromFrame)  s_eo_icubCanProto_getCmdId_fromFrame_ofSkinClass  
    }
/*   uncomment when loader msg will be use
    {   // 5
        EO_INIT(.parserTbl)                           NULL,
        EO_INIT(.formerTbl)                           NULL,
        EO_INIT(.eo_icubCanProto_getCmdId_fromFrame)  NULL 
    },

    {   // 6
        EO_INIT(.parserTbl)                           NULL,
        EO_INIT(.formerTbl)                           NULL,
        EO_INIT(.eo_icubCanProto_getCmdId_fromFrame)  NULL 
    },

    {   // 7   Loader!!!
        EO_INIT(.parserTbl)                           NULL,
        EO_INIT(.formerTbl)                           NULL,
        EO_INIT(.eo_icubCanProto_getCmdId_fromFrame)  NULL 
    },
*/
};



/* DECLARATION OF MSG CLASS LOOKUP TBL */
static const EOconstLookupTbl s_msgClasses_LUTbl = 
{
    EO_INIT(.capacity)                    eo_icubCanProto_msgClasses_inTbl_max,
    EO_INIT(.offset)                      0,
    EO_INIT(.exceptionMngFn)              s_eo_icubCanProto_msgClasses_excFn,
    EO_INIT(.itemsList)                   &s_msgClasses_tblsReference_list[0]
};

extern const EOconstLookupTbl* const icubCanProto_msgClasses_lookupTbl__ptr = &s_msgClasses_LUTbl;
// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

EO_static_inline eOicubCanProto_msgCommand_cmdId_t s_eo_icubCanProto_getCmdId_fromFrame_ofPeriodicClass(eOcanframe_t *frame)
{
    return((eOicubCanProto_msgCommand_cmdId_t)(frame->id & 0x00F));    

}



EO_static_inline eOicubCanProto_msgCommand_cmdId_t s_eo_icubCanProto_getCmdId_fromFrame_ofPollingClass(eOcanframe_t *frame)
{
    if(frame->size == 0)
    {
        return(0xFF);
    }
    
    return((eOicubCanProto_msgCommand_cmdId_t)(frame->data[0]&0x7F));
}



EO_static_inline eOicubCanProto_msgCommand_cmdId_t s_eo_icubCanProto_getCmdId_fromFrame_ofSkinClass(eOcanframe_t *frame)
{
    return(0); //use only this command to acees to skin's functions       
}


static eOresult_t s_eo_icubCanProto_msgClasses_excFn(void *arg)
{

    return(eores_OK);
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



