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
#include "EoCommon.h"
#include "string.h"
#include "stdio.h"
#include "math.h"

#include "FeatureInterface.h"


#include "EOtheErrorManager.h"
#include "EOVtheSystem_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOYtheSystem.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOYtheSystem_hid.h" 


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

static eOresult_t s_eoy_sys_start(void (*init_fn)(void));

static void s_eoy_thecreation(void);

static EOVtaskDerived* s_eoy_sys_gettask(void);

static eOabstime_t s_eoy_sys_abstime_get(void);
static void s_eoy_sys_abstime_set(uint64_t time);
static uint64_t s_eoy_sys_nanotime_get(void);
static void s_eoy_sys_stop(void);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const char s_eobj_ownname[] = "EOYtheSystem";

static const eOysystem_cfg_t s_eoy_sys_defaultconfig = 
{
    EO_INIT(nothing)        0
};

static EOYtheSystem s_eoy_system = 
{
    EO_INIT(thevsys)        NULL,               
    EO_INIT(user_init_fn)   NULL,
    EO_INIT(.start)         0
};

//#if   defined(EO_TAILOR_CODE_FOR_LINUX)
//static struct timespec start_time;
//#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern EOYtheSystem * eoy_sys_Initialise(const eOysystem_cfg_t *syscfg, 
                                         const eOmempool_cfg_t *mpoolcfg, 
                                         const eOerrman_cfg_t *errmancfg)
{

    if(NULL != s_eoy_system.thevsys) 
    {
        // already initialised
        return(&s_eoy_system);
    }

    if(NULL == syscfg)
    {
        syscfg = &s_eoy_sys_defaultconfig;
    }

    //eo_errman_Assert(eo_errman_GetHandle(), (NULL != syscfg), s_eobj_ownname, "syscfg is NULL");
 

    // mpoolcfg can be NULL: in such a case we use eo_mempool_alloc_dynamic mode
    // errmancfg can be NULL
    // tmrmancfg can be NULL: in such a case we use default values
    // cbkmancfg can be NULL: in such a case we use default values

    
    // mempool and error manager initialised inside here.
    s_eoy_system.thevsys = eov_sys_hid_Initialise(mpoolcfg,
                                                  errmancfg,        // error man 
                                                  (eOres_fp_voidfpvoid_t)s_eoy_sys_start, s_eoy_sys_gettask, 
                                                  s_eoy_sys_abstime_get, s_eoy_sys_abstime_set, 
                                                  (eOuint64_fp_void_t)s_eoy_sys_nanotime_get,
                                                  s_eoy_sys_stop);


    // initialise y-environment
    
    s_eoy_system.start = yarp_time_now();

    return(&s_eoy_system);  
}


extern EOYtheSystem* eoy_sys_GetHandle(void)
{
    if(NULL == s_eoy_system.thevsys)
    {
        return(NULL);
    }

    return(&s_eoy_system);
}    


extern void eoy_sys_Start(EOYtheSystem *p, eOvoid_fp_void_t userinit_fn)
{
    s_eoy_sys_start(userinit_fn);
}

extern eOabstime_t eoy_sys_abstime_get(EOYtheSystem *p)
{
	if(p==NULL)
	{
		return(0);
	}

	return(s_eoy_sys_abstime_get());
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static eOresult_t s_eoy_sys_start(eOvoid_fp_void_t userinit_fn)
{

    s_eoy_system.user_init_fn = userinit_fn;
    s_eoy_thecreation();
 
    return(eores_OK);
}


static void s_eoy_thecreation(void)
{
    // run user defined initialisation ...
    if(NULL != s_eoy_system.user_init_fn)
    {
        s_eoy_system.user_init_fn();
    }
}


static EOVtaskDerived* s_eoy_sys_gettask(void)
{
    return(NULL);
}

static eOabstime_t s_eoy_sys_abstime_get(void)
{
    eOabstime_t time = 0xEABABABABABABABF;

    double delta = yarp_time_now() - s_eoy_system.start;

    delta *= (1e6);
    time = (eOabstime_t)floor(delta);

    return(time);

}


static void s_eoy_sys_abstime_set(eOabstime_t time)
{
    s_eoy_system.start = ((double) time)/ 1e6;
}

static eOnanotime_t s_eoy_sys_nanotime_get(void)
{
    eOnanotime_t nanotime = 0;

    double delta = yarp_time_now() - s_eoy_system.start;

    delta *= 1e9;
    nanotime = (eOnanotime_t)floor(delta);

    return(nanotime);
}

static void s_eoy_sys_stop(void)
{
    // do nothing
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




