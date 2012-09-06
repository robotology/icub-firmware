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

/* @file       EOicubCanProto.c
    @brief      This file implements...
    @author     valentina.gaggero@iit.it
    @date       03/12/2012
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "EoCommon.h"
#include "EOtheMemoryPool.h"
#include "EOconstLookupTbl.h"
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOicubCanProto.h"
#include "EOicubCanProto_specifications.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOicubCanProto_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static eo_icubCanProto_msgCommand_cmdId_t s_eo_icubCanProto_LUTbl_GetCmdIdFromFrame(EOicubCanProto* p,
                                                                                    eo_icubCanProto_msgCommand_class_t class,
                                                                                    eOcanframe_t *frame );
static const EOconstLookupTbl* s_eo_icubCanProto_LUTbl_GetParserTbl(EOicubCanProto* p,
                                                                    eo_icubCanProto_msgCommand_class_t class);

static const EOconstLookupTbl*  s_eo_icubCanProto_LUTbl_GetFormerTbl(EOicubCanProto* p,
                                                                     eo_icubCanProto_msgCommand_class_t class);

static eOresult_t s_eo_icubCanProto_ParseCanFrame(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPortRX);

static eOresult_t s_eo_icubCanProto_FormCanFrame(EOicubCanProto* p,
                                                 eo_canProto_msgCommand_t command, 
                                                 eo_canProto_msgDestination_t dest,
                                                 void *value,
                                                 eOcanframe_t *frame);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOicubCanProto* eo_icubCanProto_New(eo_icubCanProto_cfg_t *cfg)
{
    EOicubCanProto *retptr = NULL;  

    if(NULL == cfg)
    {    
        return(retptr);
    }

    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOicubCanProto), 1);
    
    retptr->emsCanNetTopo_ptr = cfg->emsCanNetTopo__ptr;
    retptr->msgClasses_LUTbl__ptr = cfg->msgClasses_LUTbl__ptr;

    return(retptr);
} 


extern eOresult_t eo_icubCanProto_ParseCanFrame(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPortRX)
{
 
    if((NULL == p) || (NULL == frame))
    {
        return(eores_NOK_nullpointer);
    }

    return(s_eo_icubCanProto_ParseCanFrame(p, frame, canPortRX));
}



extern eOresult_t eo_icubCanProto_FormCanFrame(EOicubCanProto* p,
                                               eo_icubCanProto_msgCommand_t command, 
                                               eo_icubCanProto_msgDestination_t dest,
                                               void *value,
                                               eOcanframe_t *frame)
{

    if((NULL == p) || (NULL == frame))
    {
        return(eores_NOK_nullpointer);
    }

    return(s_eo_icubCanProto_FormCanFrame(p, *((eo_canProto_msgCommand_t*)&command), 
                                          *((eo_canProto_msgDestination_t*)&dest),
                                          value, frame));
}



extern eOresult_t eo_icubCanProto_ParseSkinCanFrame(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPortRX)
{
    const EOconstLookupTbl                             *tbl;
    eo_icubCanProto_hid_LUTbl_item_parserFnHandling_t  *itemList;
 
    if((NULL == p) || (NULL == frame))
    {
        return(eores_NOK_nullpointer);
    }

    tbl = s_eo_icubCanProto_LUTbl_GetParserTbl(p, eo_icubCanProto_msgCmdClass_skinBoard);

    if(NULL == tbl)
    {
        return(eores_NOK_nullpointer);
    }

    itemList = (eo_icubCanProto_hid_LUTbl_item_parserFnHandling_t*)tbl->itemsList;
    return(itemList[0].parser(p, frame, canPortRX));

}
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static eOresult_t s_eo_icubCanProto_ParseCanFrame(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPortRX)
{
    const EOconstLookupTbl                             *tbl;
    eo_icubCanProto_hid_LUTbl_item_parserFnHandling_t  *itemList;
    eo_icubCanProto_msgCommand_cmdId_t                  cmdId;
    eo_icubCanProto_msgCommand_class_t                  msgClass;
    eo_emsCanNetTopo_sensorCanLocation_t                canLoc;
    eOresult_t                                          res;


    //NOTE: when skin messages will belong to a particular class message,(not to analog sensor message class)
    //remove following if-else and leave only eo_icubCanProto_hid_getMsgClassFromFrameId function
    res = eo_emsCanNetTopo_GetskinCanLocation_BySkinId(p->emsCanNetTopo_ptr, 0, &canLoc);
        
    if((eores_OK == res) && (canPortRX == canLoc.emscanport))
    {
        msgClass = eo_icubCanProto_msgCmdClass_skinBoard;
    }
    else
    {
        msgClass = eo_icubCanProto_hid_getMsgClassFromFrameId(frame->id);    
    }


    tbl = s_eo_icubCanProto_LUTbl_GetParserTbl(p, msgClass);

    if(NULL == tbl)
    {
        return(eores_NOK_nullpointer);
    }

    cmdId = s_eo_icubCanProto_LUTbl_GetCmdIdFromFrame(p, msgClass, frame);

    if(!eo_icubCanProto_hid_LUTbl_indexIsInTbl(tbl, cmdId))    
    {
        return(eo_icubCanProto_hid_LUTbl_excParse(p, tbl, cmdId, frame, canPortRX));
    }

    itemList = (eo_icubCanProto_hid_LUTbl_item_parserFnHandling_t*)tbl->itemsList;
    return(itemList[cmdId - tbl->offset].parser(p, frame, canPortRX));
}



static eOresult_t s_eo_icubCanProto_FormCanFrame(EOicubCanProto* p,
                                                 eo_canProto_msgCommand_t command, 
                                                 eo_canProto_msgDestination_t dest,
                                                 void *value,
                                                 eOcanframe_t *frame)
{

    const EOconstLookupTbl                            *tbl;
    eo_icubCanProto_hid_LUTbl_item_formerFnHandling_t *itemsList;
    eo_icubCanProto_msgCommand_t *icommand = (eo_icubCanProto_msgCommand_t*)&command; 
    eo_icubCanProto_msgDestination_t *idest = (eo_icubCanProto_msgDestination_t*)&dest;

    tbl = s_eo_icubCanProto_LUTbl_GetFormerTbl(p, (eo_icubCanProto_msgCommand_class_t)icommand->class);

    if(NULL == tbl)
    {
        return(eores_NOK_nullpointer);
    }

    if(!eo_icubCanProto_hid_LUTbl_indexIsInTbl(tbl, icommand->cmdId))    
    {
        return(eo_icubCanProto_hid_LUTbl_excForm(p, tbl, icommand->cmdId, value, *idest, frame));
    }

    itemsList = (eo_icubCanProto_hid_LUTbl_item_formerFnHandling_t*)tbl->itemsList;
    return(itemsList[icommand->cmdId - tbl->offset].former(p, value, *idest, frame));

}





static eo_icubCanProto_msgCommand_cmdId_t s_eo_icubCanProto_LUTbl_GetCmdIdFromFrame(EOicubCanProto* p,
                                                                                    eo_icubCanProto_msgCommand_class_t class,
                                                                                    eOcanframe_t *frame )
{
    eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t* itemsList;

    itemsList = (eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t*)p->msgClasses_LUTbl__ptr->itemsList;
    return(itemsList[class].eo_icubCanProto_getCmdId_fromFrame(frame));    
}




static const EOconstLookupTbl*  s_eo_icubCanProto_LUTbl_GetParserTbl(EOicubCanProto* p,
                                                                     eo_icubCanProto_msgCommand_class_t class)
{
    eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t* itemsList;

    /*Nota: probabilmente e' un controllo inutile, ma meglio farlo visto che si accedono a loc di memoria*/
    if((class >= p->msgClasses_LUTbl__ptr->offset) && ((((uint8_t)class) - p->msgClasses_LUTbl__ptr->offset) < p->msgClasses_LUTbl__ptr->capacity))
    {
        itemsList = (eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t*)p->msgClasses_LUTbl__ptr->itemsList;
        return(itemsList[class].parserTbl);   
    }
    else
    {
        return(NULL);
    }
}

static const EOconstLookupTbl*  s_eo_icubCanProto_LUTbl_GetFormerTbl(EOicubCanProto* p,
                                                                     eo_icubCanProto_msgCommand_class_t class)
{
    eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t* itemsList;

    /*Nota: probabilmente e' un controllo inutile, ma meglio farlo visto che si accedono a loc di memoria*/
    if((class >= p->msgClasses_LUTbl__ptr->offset) && ((((uint8_t)class) - p->msgClasses_LUTbl__ptr->offset) < p->msgClasses_LUTbl__ptr->capacity))
    {
        itemsList = (eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t*)p->msgClasses_LUTbl__ptr->itemsList;
        return(itemsList[class].formerTbl);   
    }
    else
    {
        return(NULL);
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

