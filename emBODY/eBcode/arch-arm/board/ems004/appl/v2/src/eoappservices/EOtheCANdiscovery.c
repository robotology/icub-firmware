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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"

#include "EoCommon.h"
#include "EoError.h"
#include "EOtheErrorManager.h"


#include "EOtheCANservice.h"
#include "EOtheCANprotocol.h"

#include "EOaction.h"

#include "EOMtheEMSconfigurator.h"

#include "EOtheEntities.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANdiscovery.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheCANdiscovery_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define SET_BITS(mask,bits) mask |=  (bits)
#define RST_BITS(mask,bits) mask &= ~(bits)
#define CHK_BITS(mask,bits) (((mask) & (bits)) == (bits))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eo_candiscovery_sendGetFWVersion(uint32_t dontaskmask);
static eObool_t s_eo_candiscovery_IsProtocolVersionCompatible(const eObrd_version_t *required, const eObrd_version_t *detected);
static void s_eo_candiscovery_SetBitMask (uint32_t* mask, uint8_t position, uint8_t port);
static void s_eo_candiscovery_ResetBitMask (uint32_t* mask, uint8_t position, uint8_t port);
static eObool_t s_eo_candiscovery_CheckBitMask (uint32_t* mask, uint8_t position, uint8_t can_port);
static void s_eo_candiscovery_SendDiagDeadBoards (void);
static void s_eo_candiscovery_SendDiagIncorrectProtBoards (void);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheCANdiscovery s_eo_thecandiscovery = 
{
    .initted                = eobool_false,
    .discoverytimer         = NULL
};

static const char s_eobj_ownname[] = "EOtheCANdiscovery";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheCANdiscovery* eo_candiscovery_Initialise(void)
{
    if(eobool_true == s_eo_thecandiscovery.initted)
    {
        return(&s_eo_thecandiscovery);
    }
    

    s_eo_thecandiscovery.discoverytimer = eo_timer_New(); 

    //all these values are reset again and updated when eo_candiscovery_ResetAndStartProcedure is called
    s_eo_thecandiscovery.readymask          = 0;
    s_eo_thecandiscovery.tobecheckedmask    = 0;
    s_eo_thecandiscovery.incorrectprotmask  = 0;
    s_eo_thecandiscovery.isMCready          = eobool_false;
    s_eo_thecandiscovery.isMAISready        = eobool_false;
    s_eo_thecandiscovery.isSTRAINready      = eobool_false;
    s_eo_thecandiscovery.numberofattempts   = 0;
    s_eo_thecandiscovery.initted = eobool_true;
    
    return(&s_eo_thecandiscovery);   
}


extern EOtheCANdiscovery* eo_candiscovery_GetHandle(void)
{
    if (s_eo_thecandiscovery.initted == eobool_false)
        return NULL;
    
    return(&s_eo_thecandiscovery);
}


extern eOresult_t eo_candiscovery_Start(EOtheCANdiscovery *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    EOaction_strg astg = {0};
    EOaction *action = (EOaction*)&astg;
       
    eo_action_SetEvent(action, emsconfigurator_evt_userdef01, eom_emsconfigurator_GetTask(eom_emsconfigurator_GetHandle()));
    eo_timer_Start(s_eo_thecandiscovery.discoverytimer, eok_abstimeNOW, 250*eok_reltime1ms, eo_tmrmode_FOREVER, action);
    
    return(eores_OK);       
}


extern eOresult_t eo_candiscovery_Stop(EOtheCANdiscovery *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    s_eo_thecandiscovery.numberofattempts  = 0;
    
    eo_candiscovery_SignalDetectedCANboards(eo_candiscovery_GetHandle());
    
    return(eo_timer_Stop(s_eo_thecandiscovery.discoverytimer)); 
}

extern eOresult_t eo_candiscovery_SignalDetectedCANboards(EOtheCANdiscovery *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    eOerrmanDescriptor_t des = {0};
    des.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_board_detected);
    uint8_t i=0;
    
    //mc
    uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());
    for(i=0; i<numofjomos; i++)
    {        
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);        
        eOcanmap_location_t loc = {0};
        
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &loc, NULL, NULL))
        {
            // send a message... must retrieve the board info
                
            //print only the ready boards
            if (!s_eo_candiscovery_CheckBitMask(&s_eo_thecandiscovery.readymask, loc.addr, loc.port))
                continue;
            const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), loc);
            des.sourcedevice    = (eOcanport1 == loc.port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
            des.sourceaddress   = loc.addr;
            des.par16           = i; 
            memcpy(&des.par64, &board->detected, sizeof(eObrd_typeandversions_t));
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &des);
        }               
    }
    
    //mais
    uint8_t numofmaises = eo_entities_NumOfMaises(eo_entities_GetHandle());
    for(i=0; i<numofmaises; i++)
    {        
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_mais, i, 0);         
        eOcanmap_location_t loc = {0};
      
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &loc, NULL, NULL))
        {
            //print only the ready boards
            if (!s_eo_candiscovery_CheckBitMask(&s_eo_thecandiscovery.readymask, loc.addr, loc.port))
                continue;
            // send a message... must retrieve the board info 
            const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), loc);
            des.sourcedevice    = (eOcanport1 == loc.port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
            des.sourceaddress   = loc.addr;
            des.par16           = i; 
            memcpy(&des.par64, &board->detected, sizeof(eObrd_typeandversions_t));
            eo_errman_Error(eo_errman_GetHandle(), eo_errortype_info, NULL, s_eobj_ownname, &des);
        }               
    }
    
    return(eores_OK);
    
}


extern eOresult_t eo_candiscovery_ResetAndStartProcedure(EOtheCANdiscovery *p)
{
    //need to:
    //1 - ask all the CAN boards for the FW version
    //2 - fill the s_eo_thecandiscovery.tobecheckedmask so that I can use it later on to check the answers
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    //reset internal storage
    s_eo_thecandiscovery.tobecheckedmask    = 0;
    s_eo_thecandiscovery.readymask          = 0;
    s_eo_thecandiscovery.incorrectprotmask  = 0;
    s_eo_thecandiscovery.isMCready          = eobool_false;
    s_eo_thecandiscovery.isMAISready        = eobool_false;
    s_eo_thecandiscovery.isSTRAINready      = eobool_false;
    s_eo_thecandiscovery.numberofattempts   = 0;
    
    uint8_t i = 0;
    
    //mc
    uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());
    for(i=0; i<numofjomos; i++)
    {        
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);        
        eOcanmap_location_t location = {0};
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &location, NULL, NULL))
        {
            //update the mask adding the board to be checked           
            s_eo_candiscovery_SetBitMask(&s_eo_thecandiscovery.tobecheckedmask, location.addr, location.port);
            
            // send a message... must retrieve the board info 
            const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), location);
            eOcanprot_command_t command = {0};
            command.class = eocanprot_msgclass_pollingMotorControl;
            command.type  = ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION;
            command.value = (void*)&board->board.props.requiredprotocol;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);                                      
        }               
    }
    
    //mais   
    uint8_t numofmaises = eo_entities_NumOfMaises(eo_entities_GetHandle());
    for(i=0; i<numofmaises; i++)
    {        
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_analogsensors, eoprot_entity_as_mais, i, 0);        
        eOcanmap_location_t location = {0};
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &location, NULL, NULL))
        {
            //update the mask adding the board to be checked           
            s_eo_candiscovery_SetBitMask(&s_eo_thecandiscovery.tobecheckedmask, location.addr, location.port);
            
            // send a message... must retrieve the board info 
            const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), location);
            eOcanprot_command_t command = {0};
            command.class = eocanprot_msgclass_pollingAnalogSensor;
            command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION;
            command.value = (void*)&board->board.props.requiredprotocol;
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);                                   
        }               
    }
    
    //add similar parts for strain and skin...
    
    return eores_OK;
   
    //this strategy probably doesn't work for boards with multiple CAN addresses...
    /*
    
    eOcanmap_arrayof_locations_t* can_boards = (eOcanmap_arrayof_locations_t*)eo_canmap_GetBoardLocations(eo_canmap_GetHandle());
    uint8_t i;
    uint8_t shift;
    for(i=0 ; i<eocanmap_maxlocations; i++)
    {
        shift = can_boards->data[i].addr;
        //get info of the board
        eObrd_cantype_t brd_type = eo_canmap_GetBoardType(eo_canmap_GetHandle(),can_boards->data[i]);
        const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), can_boards->data[i]);
        eOcanprot_command_t command = {0};
        
        switch (brd_type)
        {
            //mc
            case eobrd_cantype_mc4 :
            case eobrd_cantype_1foc:
            {
                //prepare and send the command
                command.class = eocanprot_msgclass_pollingMotorControl;
                command.type  = ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION;
                command.value = (void*)&board->board.props.requiredprotocol;
                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, can_boards->data[i]);

                //update the mask
                s_eo_candiscovery_SetBitMask(&s_eo_thecandiscovery.tobecheckedmask, shift, can_boards->data[i].port);              
            } break;

            case eobrd_cantype_mais:
            {    
                //prepare and send the command
                command.class = eocanprot_msgclass_pollingAnalogSensor;
                command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION;
                command.value = (void*)&board->board.props.requiredprotocol;
                eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, can_boards->data[i]);

                //update the mask
                s_eo_candiscovery_SetBitMask(&s_eo_thecandiscovery.tobecheckedmask, shift, can_boards->data[i].port);
            } break;
            
            default:
                break;
            
        }
        
     }
     */
    
}
extern eOresult_t eo_candiscovery_EvaluateDiscoveredResources(EOtheCANdiscovery *p)
{
    // need to:
    //1 - check for all the canboards inside the mask (tobechecked) the detected major and minor
    //2 - compare with the wanted values and set their ready flag or not
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    eObool_t res = eores_OK;
    
    eOcanmap_arrayof_locations_t* can_boards = (eOcanmap_arrayof_locations_t*)eo_canmap_GetBoardLocations(eo_canmap_GetHandle());
    uint8_t boards_tot_num = can_boards->head.size;
    
    uint8_t numofjomos  = eo_entities_NumOfJoints(eo_entities_GetHandle());
    uint8_t numofmaises = eo_entities_NumOfMaises(eo_entities_GetHandle());
    
    //we'll update these values during the cycle
    uint8_t numofjomos_answers  = numofjomos;
    uint8_t numofmaises_answers = numofmaises;
     
    uint8_t i;
    uint8_t shift;
    for(i=0 ; i<boards_tot_num; i++)
    {
        //check if the current board should have replied
        shift = can_boards->data[i].addr;
        if (!s_eo_candiscovery_CheckBitMask(&s_eo_thecandiscovery.tobecheckedmask, shift, can_boards->data[i].port))
            continue;
        
        const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), can_boards->data[i]);
        //the board replied correctly
        if(eobool_true == s_eo_candiscovery_IsProtocolVersionCompatible(&board->board.props.requiredprotocol, &board->detected.protocolversion))
        {
            //update masks
            s_eo_candiscovery_SetBitMask(&s_eo_thecandiscovery.readymask, shift,can_boards->data[i].port);
            s_eo_candiscovery_ResetBitMask(&s_eo_thecandiscovery.tobecheckedmask, shift,can_boards->data[i].port);
            continue;
        }
        // the board replied but with an incorrect protocol version
        else if ((board->detected.protocolversion.major != 0) || (board->detected.protocolversion.minor != 0))
        {
            //update masks
            //N.B.an incorrect protocol version let the board go in RUN state, but it will produce a warning message when RobotInterface try to send the board in the RUN state           
            s_eo_candiscovery_SetBitMask(&s_eo_thecandiscovery.incorrectprotmask, shift,can_boards->data[i].port);
            s_eo_candiscovery_ResetBitMask(&s_eo_thecandiscovery.tobecheckedmask, shift,can_boards->data[i].port);
            continue;
        }
        
        //the board didn't reply
        else
        {
            eObrd_cantype_t brd_type = eo_canmap_GetBoardType(eo_canmap_GetHandle(),can_boards->data[i]);
            switch (brd_type)
            {
                //mc
                case eobrd_cantype_mc4 :
                case eobrd_cantype_1foc:
                {
                    numofjomos_answers--;
                } break;

                //as
                case eobrd_cantype_mais:
                {    
                    numofmaises_answers--;
                } break;
            
                default:
                    break;
            }
        }

    }
    
    //evaluate flags
    if (numofjomos_answers  ==   numofjomos)
            s_eo_thecandiscovery.isMCready      = eobool_true;
    if (numofmaises_answers ==  numofmaises)
            s_eo_thecandiscovery.isMAISready    = eobool_true;
       
    return res;
}
extern eOresult_t eo_candiscovery_CheckRemainingCanBoards(EOtheCANdiscovery *p)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if (s_eo_thecandiscovery.tobecheckedmask == 0)
        return(eores_OK);
    
    s_eo_thecandiscovery.numberofattempts++;
    
    eOcanmap_arrayof_locations_t* can_boards = (eOcanmap_arrayof_locations_t*)eo_canmap_GetBoardLocations(eo_canmap_GetHandle());
    uint8_t boards_tot_num = can_boards->head.size;
    
    uint8_t i;
    uint8_t shift;
    for(i=0 ; i<boards_tot_num; i++)
    {
        //check if the current board is still to be checked
        shift = can_boards->data[i].addr;
        if(s_eo_candiscovery_CheckBitMask(&s_eo_thecandiscovery.tobecheckedmask, shift, can_boards->data[i].port))
        {
            eObrd_cantype_t brd_type = eo_canmap_GetBoardType(eo_canmap_GetHandle(),can_boards->data[i]);
            const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), can_boards->data[i]);   
            
            switch (brd_type)
            {
                //mc
                case eobrd_cantype_mc4 :
                case eobrd_cantype_1foc:
                {
                    eOcanprot_command_t command = {0};
                    command.class = eocanprot_msgclass_pollingMotorControl;
                    command.type  = ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION;
                    command.value = (void*)&board->board.props.requiredprotocol;
                    eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, can_boards->data[i]);
                } break;

                //as
                case eobrd_cantype_mais:
                {    
                    eOcanprot_command_t command = {0};
                    command.class = eocanprot_msgclass_pollingAnalogSensor;
                    command.type  = ICUBCANPROTO_POL_AS_CMD__GET_FW_VERSION;
                    command.value = (void*)&board->board.props.requiredprotocol;
                    eo_canserv_SendCommandToLocation(eo_canserv_GetHandle(), &command, can_boards->data[i]);
                } break;
            
                default:
                    break;
            }
                       
        }
    }
    
    //If for 5 seconds I'm retrying to finish the procedure, than I send a message up saying which boards are missing
    if (s_eo_thecandiscovery.numberofattempts == 20)
    {
        s_eo_candiscovery_SendDiagDeadBoards();
    }
    return(eores_OK);
}

extern eOresult_t eo_candiscovery_ManageDetectedBoard(EOtheCANdiscovery *p, eOcanmap_location_t loc, eObool_t match, eObrd_typeandversions_t *detected)
{
    eOresult_t res = eores_OK;
    
    // at first we send diagnostics
    if(eobool_false == match)
    {
        const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), loc);  
        eOerrmanDescriptor_t des = {0};
        des.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_board_wrongprotversion);
        des.sourcedevice    = (eOcanport1 == loc.port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        des.sourceaddress   = loc.addr;
        des.par16           = (board->board.props.requiredprotocol.major << 8) | (board->board.props.requiredprotocol.minor); 
        memcpy(&des.par64, detected, sizeof(eObrd_typeandversions_t));
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &des);        
    }
    
    // then we put the detected inside the EOtheCANmapping
    eo_canmap_BoardSetDetected(eo_canmap_GetHandle(), loc, detected); 
    
    return(res);
}

extern eObool_t eo_candiscovery_isMCReady(EOtheCANdiscovery *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    return s_eo_thecandiscovery.isMCready;
}

extern eObool_t eo_candiscovery_isMAISReady(EOtheCANdiscovery *p)
{
    if(NULL == p)
    {
        return(eobool_false);
    }
    
    return s_eo_thecandiscovery.isMAISready;
}

extern void eo_candiscovery_SendDiagnosticsAboutBoardsWithIssues (EOtheCANdiscovery *p)
{
    if(NULL == p)
    {
        return;
    }
    
    //this mask consists of the boards which should have replied but did not yet
    //uint32_t didntreplymask = (s_eo_thecandiscovery.readymask & s_eo_thecandiscovery.tobecheckedmask) | (~s_eo_thecandiscovery.tobecheckedmask);
    
    s_eo_candiscovery_SendDiagDeadBoards();
    s_eo_candiscovery_SendDiagIncorrectProtBoards();  
}

/* DEPRECATED APIs */
extern eOresult_t eo_candiscovery_CheckCanBoardsAreReady(EOtheCANdiscovery *p, uint32_t dontaskmask)
{
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    return(s_eo_candiscovery_sendGetFWVersion(dontaskmask));
}

// must return true only if all can boards have replied and their protocol version matches with target
extern eObool_t eo_candiscovery_areCanBoardsReady(EOtheCANdiscovery *p, uint32_t *canBoardsReady, uint32_t *canBoardsChecked)
{   
    eObool_t res = eobool_true;
    
    if((NULL == p) || (NULL == canBoardsReady))
    {
        return(eobool_false);
    }
    
    *canBoardsReady = 0;
    
    uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());
    uint8_t i=0;
    for(i=0; i<numofjomos; i++)
    {
        if(NULL != canBoardsChecked)
        {
            *canBoardsChecked |= (1<<i);
        }
        
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);        
        eOcanmap_location_t location = {0};
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &location, NULL, NULL))
        {
            // must retrieve the extended board info 
            const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), location);
            eObool_t ready = eobool_false;
            if(eobool_true == s_eo_candiscovery_IsProtocolVersionCompatible(&board->board.props.requiredprotocol, &board->detected.protocolversion))
            {
                ready = eobool_true;
                *canBoardsReady |= (1<<i);                
            }
            res &= ready;
        }               
    }
    
    return(res);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static eObool_t s_eo_candiscovery_IsProtocolVersionCompatible(const eObrd_version_t *required, const eObrd_version_t *detected)
{
    eObool_t ret = eobool_false;
    //if the required version is 0.0 I don't care about the answer
    if ((required->major == 0) && (required->minor == 0))
    {
        return (eobool_true);
    }
    
    if((required->major == detected->major) && (required->minor <= detected->minor))
    {
        ret = eobool_true;
    }    
    return(ret);    
}

static void s_eo_candiscovery_SetBitMask (uint32_t* mask, uint8_t position, uint8_t can_port)
{  
    //CAN1
    if (can_port == 0)
        //s_eo_thecandiscovery.tobecheckedmask |= (1 << can_boards->data[i].addr);
        SET_BITS(*mask, 1 << position);
    //CAN2
    else if (can_port == 1)
        //s_eo_thecandiscovery.tobecheckedmask |= (1 << (can_boards->data[i].addr+16));
        SET_BITS(*mask, 1 << (position+16));
    else
        return;
}
static void s_eo_candiscovery_ResetBitMask (uint32_t* mask, uint8_t position, uint8_t can_port)
{  
    //CAN1
    if (can_port == 0)
        //s_eo_thecandiscovery.tobecheckedmask |= (1 << can_boards->data[i].addr);
        RST_BITS(*mask, 1 << position);
    //CAN2
    else if (can_port == 1)
        //s_eo_thecandiscovery.tobecheckedmask |= (1 << (can_boards->data[i].addr+16));
        RST_BITS(*mask, 1 << (position+16));
    else
        return;
}


static eObool_t s_eo_candiscovery_CheckBitMask (uint32_t* mask, uint8_t position, uint8_t can_port)
{  
    //CAN1
    if (can_port == 0)
        //s_eo_thecandiscovery.tobecheckedmask |= (1 << can_boards->data[i].addr);
        return CHK_BITS(*mask, 1 << position);
    //CAN2
    else if (can_port == 1)
        //s_eo_thecandiscovery.tobecheckedmask |= (1 << (can_boards->data[i].addr+16));
        return CHK_BITS(*mask, 1 << (position+16));
    else
        return eobool_false;
}

static void s_eo_candiscovery_SendDiagDeadBoards(void)
{  
    //send up a diag message for each board that is missing, with info
    // N.B. maybe it could e good to have a more compact way to do it...
    
    eOcanmap_arrayof_locations_t* can_boards = (eOcanmap_arrayof_locations_t*)eo_canmap_GetBoardLocations(eo_canmap_GetHandle());
    uint8_t boards_tot_num = can_boards->head.size;
    
    uint8_t i;
    uint8_t shift;
    for(i = 0; i < boards_tot_num; i++)
    {
        shift = can_boards->data[i].addr;
        if (!s_eo_candiscovery_CheckBitMask(&s_eo_thecandiscovery.tobecheckedmask, shift, can_boards->data[i].port))
            continue;
        
        eObrd_cantype_t brd_type = eo_canmap_GetBoardType(eo_canmap_GetHandle(),can_boards->data[i]);
        
        eOerrmanDescriptor_t errdes = {0};
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_board_notfound);
        errdes.sourcedevice     = (eOcanport1 == can_boards->data[i].port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        errdes.sourceaddress    = shift;
        errdes.par16            = brd_type;
        errdes.par64            = 0;
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &errdes); 
        
    }
    
    //reset the counter
    s_eo_thecandiscovery.numberofattempts = 0;
    
    #warning davide:it could be nice to clear the CAN queue, cause I noticed that when the CAN bus is NOT connected a lot of message saying that TXfifo is overflow are coming up

}

static void s_eo_candiscovery_SendDiagIncorrectProtBoards (void)
{
    //send up a diag message for each board with an incorrect protocol version
    // N.B. maybe it could e good to have a more compact way to do it...
    
    eOcanmap_arrayof_locations_t* can_boards = (eOcanmap_arrayof_locations_t*)eo_canmap_GetBoardLocations(eo_canmap_GetHandle());
    uint8_t boards_tot_num = can_boards->head.size;
    
    uint8_t i;
    uint8_t shift;
    for(i = 0; i < boards_tot_num; i++)
    {
        shift = can_boards->data[i].addr;
        if (!s_eo_candiscovery_CheckBitMask(&s_eo_thecandiscovery.incorrectprotmask, shift, can_boards->data[i].port))
            continue;
        
        const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), can_boards->data[i]);  
        
        eOerrmanDescriptor_t des = {0};
        des.code = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_canservices_board_wrongprotversion);
        des.sourcedevice    = (eOcanport1 == can_boards->data[i].port) ? (eo_errman_sourcedevice_canbus1) : (eo_errman_sourcedevice_canbus2);
        des.sourceaddress   = can_boards->data[i].addr;
        des.par16           = (board->board.props.requiredprotocol.major << 8) | (board->board.props.requiredprotocol.minor);
        memcpy(&des.par64, &board->detected, sizeof(eObrd_typeandversions_t));
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_error, NULL, s_eobj_ownname, &des);               
    }
}

/* DEPRECATED static functions */
static eOresult_t s_eo_candiscovery_sendGetFWVersion(uint32_t dontaskmask)
{  
    // i check only mc. thus, i get the number of mc jomos, i search for the i-th board and i send a get fw version query to it.
    
    uint8_t numofjomos = eo_entities_NumOfJoints(eo_entities_GetHandle());
    uint8_t i=0;
    for(i=0; i<numofjomos; i++)
    {
        if( ((1<<i) & dontaskmask) == (1<<i))
        {
            continue;
        }
        
        eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, i, 0);        
        eOcanmap_location_t location = {0};
        if(eores_OK == eo_canmap_GetEntityLocation(eo_canmap_GetHandle(), id32, &location, NULL, NULL))
        {
            // send a message... must retrieve the board info 
            const eOcanmap_board_extended_t * board = eo_canmap_GetBoard(eo_canmap_GetHandle(), location);
            // i decide to send the request only if the board has not replied. if it replied but the protocol does
            // not match then i dont send the command anymore.
            if(0 == board->detected.protocolversion.major)
            {  // must ask if the board has not responded yet
                eOcanprot_command_t command = {0};
                command.class = eocanprot_msgclass_pollingMotorControl;
                command.type  = ICUBCANPROTO_POL_MC_CMD__GET_FIRMWARE_VERSION;
                command.value = (void*)&board->board.props.requiredprotocol;
                eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &command, id32);                  
            }                        
        }               
    }
    
    return(eores_OK);
}
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



