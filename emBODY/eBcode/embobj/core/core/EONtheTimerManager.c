/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
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

#include "EOVtheTimerManager_hid.h"
#include "EOtimer_hid.h"
#include "EONmutex.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EONtheTimerManager.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EONtheTimerManager_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eon_timerman_OnNewTimer(EOVtheTimerManager* tm, EOtimer *t);

static eOresult_t s_eon_timerman_AddTimer(EOVtheTimerManager* tm, EOtimer *t);


static eOresult_t s_eon_timerman_RemTimer(EOVtheTimerManager* tm, EOtimer *t);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
 
//static const char s_eobj_ownname[] = "EONtheTimerManager";
 
static EONtheTimerManager s_eon_thetimermanager = 
{
    .tmrman     = NULL,         
    .none       = NULL          
}; 



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

 
extern EONtheTimerManager * eon_timerman_Initialise(void) 
{

    if(NULL != s_eon_thetimermanager.tmrman) 
    {
        // already initialised
        return(&s_eon_thetimermanager);
    }
    // i get a basic timer manager with add and rem functions proper for nee. and an EONmutex (by the way ... eon_mutex_New() never returns NULL).
    s_eon_thetimermanager.tmrman = eov_timerman_hid_Initialise(s_eon_timerman_OnNewTimer, s_eon_timerman_AddTimer, s_eon_timerman_RemTimer, eon_mutex_New()); 

    // i prepare the task able to process actions associated to expiry of the timers 
    s_eon_thetimermanager.none = NULL;                              
       
   
    return(&s_eon_thetimermanager);
}    

    
extern EONtheTimerManager* eon_timerman_GetHandle(void) 
{
    if(NULL == s_eon_thetimermanager.tmrman) 
    {
        return(NULL);
    }
    
    return(&s_eon_thetimermanager);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



static eOresult_t s_eon_timerman_OnNewTimer(EOVtheTimerManager* tm, EOtimer *t) 
{
    return(eores_NOK_generic);
}


static eOresult_t s_eon_timerman_AddTimer(EOVtheTimerManager* tm, EOtimer *t) 
{
    return(eores_NOK_generic);
}


static eOresult_t s_eon_timerman_RemTimer(EOVtheTimerManager* tm, EOtimer *t) 
{
    return(eores_NOK_generic);
}


   

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





