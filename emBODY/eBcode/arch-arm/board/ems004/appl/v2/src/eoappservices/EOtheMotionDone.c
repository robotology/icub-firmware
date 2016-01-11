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

#error DOT USE IT ANYMORE

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

    #warning MERGE-> dont use EOtheMotionDone anymore ...
    //for(;;) {;}

    if(eobool_true == s_eo_themotiondone.initted)
    {
        return(&s_eo_themotiondone);
    }
    
    
    s_eo_themotiondone.currjoint = 0;
    s_eo_themotiondone.numofjoints = eo_entities_NumOfJoints(eo_entities_GetHandle());
    

    s_eo_themotiondone.motiondonecommand.class = eocanprot_msgclass_pollingMotorControl;    
    s_eo_themotiondone.motiondonecommand.type  = ICUBCANPROTO_POL_MC_CMD__MOTION_DONE;
    s_eo_themotiondone.motiondonecommand.value = NULL;
    
    s_eo_themotiondone.initted = eobool_true;
    
    return(&s_eo_themotiondone);   
}


extern EOtheMotionDone* eo_motiondone_GetHandle(void)
{
    if(eobool_true == s_eo_themotiondone.initted)
    {
        return(&s_eo_themotiondone);
    }
    return(NULL);
}

extern eOresult_t eo_motiondone_Tick(EOtheMotionDone *p)
{   
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    #warning MERGE-> removed the code inside eo_motiondone_Tick()
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



