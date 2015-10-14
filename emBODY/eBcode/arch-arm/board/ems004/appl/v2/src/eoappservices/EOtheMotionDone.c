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
#include "EOtheErrorManager.h"
#include "EOtheEntities.h"

#include "EOtheCANservice.h"
#include "EOtheCANmapping.h"
#include "EOtheCANprotocol.h"

//#include "EOtheEMSapplBody.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMotionDone.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheMotionDone_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


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
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOtheMotionDone s_eo_themotiondone = 
{
    .initted            = eobool_false,
//    .itismc4can         = eobool_false,
    .currjoint          = 0,
    .numofjoints        = 0,
    .motiondonecommand  = {0}
};

//static const char s_eobj_ownname[] = "EOtheMotionDone";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOtheMotionDone* eo_motiondone_Initialise(void)
{
    if(eobool_true == s_eo_themotiondone.initted)
    {
        return(&s_eo_themotiondone);
    }
    
    
    s_eo_themotiondone.currjoint = 0;
    s_eo_themotiondone.numofjoints = eo_entities_NumOfJoints(eo_entities_GetHandle());
    
//    EOtheEMSapplBody* emsappbody_ptr = eo_emsapplBody_GetHandle();
//    eOmn_appl_runMode_t runmode = eo_emsapplBody_GetAppRunMode(emsappbody_ptr);
//    
//    if((applrunMode__mc4Only == runmode) || (applrunMode__skinAndMc4 == runmode))
//    {
//        s_eo_themotiondone.itismc4can = eobool_true;
//    }
//    else
//    {
//        s_eo_themotiondone.itismc4can = eobool_false;
//    }
    
    s_eo_themotiondone.motiondonecommand.class = eocanprot_msgclass_pollingMotorControl;    
    s_eo_themotiondone.motiondonecommand.type  = ICUBCANPROTO_POL_MC_CMD__MOTION_DONE;
    s_eo_themotiondone.motiondonecommand.value = NULL;
    
    s_eo_themotiondone.initted = eobool_true;
    
    return(&s_eo_themotiondone);   
}


extern EOtheMotionDone* eo_motiondone_GetHandle(void)
{
    return(eo_motiondone_Initialise());
}

extern eOresult_t eo_motiondone_Tick(EOtheMotionDone *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
//    if(eobool_false == s_eo_themotiondone.itismc4can)
//    {   // nothing to do because we dont have a mc4can board
//        return(eores_OK);
//    }
    
    // now, i do things. 
    // i cycle all the joints, one at a time. and in some cases i send a can frame to the relevant board
    
    eOmc_joint_t * joint = eo_entities_GetJoint(eo_entities_GetHandle(), s_eo_themotiondone.currjoint);
    
    if(NULL == joint)
    {   // but it should never happen
        return(eores_NOK_nullpointer);
    }
    
    // marco.accame on 28 may 2015: the motion done mechanism for mc4-based control is to be reviewed.
    // the mechanism works but marco.randazzo saw that too many motion done messages are sent over can.
    // so far, i have not changed the existing mechanism but we need to review it later on.
    // my proposal is to group in some functions called eomotiondone_* what is in here and in can rx handlers and
    // in eth-protocol callbacks so that we have everything in only one place.
    // it can be the EOtheMotionDone object ....
    if( (eomc_controlmode_position == joint->status.basic.controlmodestatus) ||
        (eomc_controlmode_mixed    == joint->status.basic.controlmodestatus) ||
        (eomc_controlmode_calib    == joint->status.basic.controlmodestatus) )
    {
        if(eomc_motionmonitorstatus_setpointnotreachedyet == joint->status.basic.motionmonitorstatus)
        {
            /* -- marco.accame on 28 may 2015: i have not changed the behaviour and i have found the following note.
                - if motionmonitorstatus is equal to _setpointnotreachedyet, i send motion done message. 
                - if (motionmonitorstatus == eomc_motionmonitorstatus_setpointisreached), i don't send
                  message because the setpoint is alredy reached. this means that:
                  - if monitormode is forever, no new set point has been configured 
                  - if monitormode is _untilreached, the joint reached the setpoint already.
                - if (motionmonitorstatus == eomc_motionmonitorstatus_notmonitored), i don't send
                  message because pc104 is not interested in getting motion done.
            */

            eOprotID32_t id32 = eoprot_ID_get(eoprot_endpoint_motioncontrol, eoprot_entity_mc_joint, s_eo_themotiondone.currjoint, eoprot_tag_none);
            eo_canserv_SendCommandToEntity(eo_canserv_GetHandle(), &s_eo_themotiondone.motiondonecommand, id32);
        }
    }
    
    // now i prepare for the next joint. i must cycle 0, 1, 2, (s_eo_themotiondone.numofjoints
    s_eo_themotiondone.currjoint ++;
    s_eo_themotiondone.currjoint %= s_eo_themotiondone.numofjoints;
    
    return(eores_OK); 
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



