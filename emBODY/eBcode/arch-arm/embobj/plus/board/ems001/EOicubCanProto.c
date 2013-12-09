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
#include "EOappTheDataBase.h"
#include "EOtheEMSapplBody.h"
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
static icubCanProto_msgCommand_cmdId_t s_eo_icubCanProto_LUTbl_GetCmdIdFromFrame(EOicubCanProto* p,
                                                                                    icubCanProto_msgCommand_class_t msgclass,
                                                                                    eOcanframe_t *frame );
static const EOconstLookupTbl* s_eo_icubCanProto_LUTbl_GetParserTbl(EOicubCanProto* p,
                                                                    icubCanProto_msgCommand_class_t msgclass);

static const EOconstLookupTbl*  s_eo_icubCanProto_LUTbl_GetFormerTbl(EOicubCanProto* p,
                                                                     icubCanProto_msgCommand_class_t msgclass);

static eOresult_t s_eo_icubCanProto_ParseCanFrame(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPortRX);

static eObool_t s_eo_icubCanProto_isMaisBUGmsg(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPortRX);


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
                                               eOicubCanProto_msgCommand_t command, 
                                               eOicubCanProto_msgDestination_t dest,
                                               void *value,
                                               eOcanframe_t *frame)
{

    const EOconstLookupTbl                            *tbl;
    eo_icubCanProto_hid_LUTbl_item_formerFnHandling_t *itemsList;
    
    if((NULL == p) || (NULL == frame))
    {
        return(eores_NOK_nullpointer);
    }
    
    tbl = s_eo_icubCanProto_LUTbl_GetFormerTbl(p, (icubCanProto_msgCommand_class_t)(command.class));

    if(NULL == tbl)
    {
        return(eores_NOK_nullpointer);
    }

    if(!eo_icubCanProto_hid_LUTbl_indexIsInTbl(tbl, command.cmdId))    
    {
        return(eo_icubCanProto_hid_LUTbl_excForm(p, tbl, command.cmdId, value, dest, frame));
    }

    itemsList = (eo_icubCanProto_hid_LUTbl_item_formerFnHandling_t*)tbl->itemsList;
    return(itemsList[command.cmdId - tbl->offset].former(p, value, dest, frame));
}



extern eOresult_t eo_icubCanProto_ParseSkinCanFrame(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPortRX)
{
    const EOconstLookupTbl                             *tbl;
    eo_icubCanProto_hid_LUTbl_item_parserFnHandling_t  *itemList;
 
    if((NULL == p) || (NULL == frame))
    {
        return(eores_NOK_nullpointer);
    }

    tbl = s_eo_icubCanProto_LUTbl_GetParserTbl(p, icubCanProto_msgCmdClass_skinBoard);

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
    icubCanProto_msgCommand_cmdId_t                     cmdId;
    icubCanProto_msgCommand_class_t                     msgClass;
    eOappTheDB_SkinCanLocation_t                        canLoc;
    eOresult_t                                          res;
    
    if(s_eo_icubCanProto_isMaisBUGmsg(p, frame, canPortRX))
    {
        return(eores_OK);
    }
    //NOTE: when skin messages will belong to a particular class message,(not to analog sensor message class)
    //remove following if-else and leave only eo_icubCanProto_hid_getMsgClassFromFrameId function
    res = eo_appTheDB_GetSkinCanLocation(eo_appTheDB_GetHandle(), 0, &canLoc);
        
    if((eores_OK == res) && (canPortRX == canLoc.emscanport))
    {
        msgClass = icubCanProto_msgCmdClass_skinBoard;
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






static icubCanProto_msgCommand_cmdId_t s_eo_icubCanProto_LUTbl_GetCmdIdFromFrame(EOicubCanProto* p,
                                                                                    icubCanProto_msgCommand_class_t msgclass,
                                                                                    eOcanframe_t *frame )
{
    eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t* itemsList;

    itemsList = (eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t*)p->msgClasses_LUTbl__ptr->itemsList;
    return(itemsList[msgclass].eo_icubCanProto_getCmdId_fromFrame(frame));    
}




static const EOconstLookupTbl*  s_eo_icubCanProto_LUTbl_GetParserTbl(EOicubCanProto* p,
                                                                     icubCanProto_msgCommand_class_t msgclass)
{
    eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t* itemsList;

    /*Nota: probabilmente e' un controllo inutile, ma meglio farlo visto che si accedono a loc di memoria*/
    if((msgclass >= p->msgClasses_LUTbl__ptr->offset) && ((uint16_t)msgclass < (p->msgClasses_LUTbl__ptr->offset + p->msgClasses_LUTbl__ptr->capacity)))
    {
        itemsList = (eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t*)p->msgClasses_LUTbl__ptr->itemsList;
        return(itemsList[(msgclass - p->msgClasses_LUTbl__ptr->offset)].parserTbl);   
    }
    else
    {
        return(NULL);
    }
}

static const EOconstLookupTbl*  s_eo_icubCanProto_LUTbl_GetFormerTbl(EOicubCanProto* p,
                                                                     icubCanProto_msgCommand_class_t msgclass)
{
    eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t* itemsList;

    /*Nota: probabilmente e' un controllo inutile, ma meglio farlo visto che si accedono a loc di memoria*/
    if((msgclass >= p->msgClasses_LUTbl__ptr->offset) && ((uint16_t)msgclass < (p->msgClasses_LUTbl__ptr->offset + p->msgClasses_LUTbl__ptr->capacity)))
    {
        itemsList = (eo_icubCanProto_hid_LUTbl_item_classMsgTblReference_t*)p->msgClasses_LUTbl__ptr->itemsList;
        return(itemsList[(msgclass - p->msgClasses_LUTbl__ptr->offset)].formerTbl);   
    }
    else
    {
        return(NULL);
    }
}

static eObool_t s_eo_icubCanProto_isMaisBUGmsg(EOicubCanProto* p, eOcanframe_t *frame, eOcanport_t canPortRX)
{
    eOappTheDB_sensorCanLocation_t canloc = { .emscanport = eOcanport1, .addr = 0}; // default value 
//This msg is send by Mais board every time it has received an unknown msg.
//on dspic30 there is a bug hw in filter can, so some motor msg are reveived by Mais and read as unknown message

//Sinse mais is only on board with MC4 before check runmode
    eOmn_appl_runMode_t apprunmode = eo_emsapplBody_GetAppRunMode(eo_emsapplBody_GetHandle());
    if((applrunMode__2foc == apprunmode) || (applrunMode__default == apprunmode))
    {
        return (0);
    }
    
    eo_appTheDB_GetSnsrMaisCanLocation(eo_appTheDB_GetHandle(), 0, &canloc);
    
    if( (canloc.emscanport == canPortRX) &&  //if port rx frame is equal of mais can location
        ((frame->id & 0x700) >> 8 == 0x0 ) &&  //if message class is motor polling
        (((frame->id &0x0f0) >> 4) == canloc.addr)) //if sender adress is of mais board
    {
        return(1);
    }
    else
    {
        return(0);
    }
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

