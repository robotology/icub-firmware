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
#include "EoCommon.h"


#include "stdlib.h"

#include "string.h"
//#include "stdio.h"


#include "EOtheErrorManager.h"
#include "EOVtheSystem_hid.h" 


#if     defined(EO_TAILOR_CODE_FOR_WINDOWS)
    #pragma message("WARNING-> cannot use linux api in the project... use ACE")
#elif   defined(EO_TAILOR_CODE_FOR_LINUX)
    #warning  WARNING-> cannot use linux api in the project... use ACE
#elif   defined(EO_TAILOR_CODE_FOR_ARM)
    #error("cannot use this file on ARM ....")
#endif

#if   defined(EO_TAILOR_CODE_FOR_LINUX)
#include <time.h>
#include <sys/time.h>
#endif


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

static uint64_t s_eoy_sys_abstime_get(void);
static void s_eoy_sys_abstime_set(uint64_t time);
static uint64_t s_eoy_sys_nanotime_get(void);
static void s_eoy_sys_stop(void);
static uint64_t s_eoy_sys_ticks_abstime_set(void);
#if   defined(EO_TAILOR_CODE_FOR_LINUX)
static int s_timeval_subtract(struct timespec *_result, struct timespec *_x, struct timespec *_y);
#endif

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
    EO_INIT(user_init_fn)   NULL               
};

#if   defined(EO_TAILOR_CODE_FOR_LINUX)
static struct timespec start_time;
#endif

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
    // add in here whatever is required.
    
    //#warning --> if needed initialise something related to yarp.

#if   defined(EO_TAILOR_CODE_FOR_LINUX)
    clock_gettime(CLOCK_REALTIME, &start_time);
#endif

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

extern uint64_t eoy_sys_abstime_get(EOYtheSystem *p)
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

static uint64_t s_eoy_sys_abstime_get(void)
{
    uint64_t time = 0xEABABABABABABABF;

#if   defined(EO_TAILOR_CODE_FOR_LINUX)
    struct timespec curr_time, diff;

    //#warning --> retrieve time with YARP

    clock_gettime(CLOCK_REALTIME, &curr_time);

    s_timeval_subtract(&diff, &curr_time, &start_time);

//    //trasformo in microsec
    time = ((diff.tv_sec * 1000 * 1000) + (diff.tv_nsec/1000));
//    time = diff.tv_sec;
#endif
    return(time);
}


#if   defined(EO_TAILOR_CODE_FOR_LINUX)
// risultato= stop - start, cioè _result = _x - _y
static int s_timeval_subtract(struct timespec *_result, struct timespec *_x, struct timespec *_y)
{
	//difftime();
	/* Perform the carry for the later subtraction by updating y. */
	static const long int MULTIPLIER = 1000 * 1000 * 1000;

	if(_x->tv_nsec < _y->tv_nsec)
	{
		int nsec = (_y->tv_nsec - _x->tv_nsec) / MULTIPLIER + 1;

		_y->tv_nsec -= MULTIPLIER * nsec;
		_y->tv_sec += nsec;
	}

	if(_x->tv_nsec - _y->tv_nsec > MULTIPLIER)
	{
		int nsec = (_x->tv_nsec - _y->tv_nsec) / MULTIPLIER;

		_y->tv_nsec += MULTIPLIER * nsec;
		_y->tv_sec -= nsec;
	}

	/* Compute the time remaining to wait. tv_usec is certainly positive. */

	_result->tv_sec = _x->tv_sec - _y->tv_sec;
	_result->tv_nsec = _x->tv_nsec - _y->tv_nsec;

	/* Return 1 if result is negative. */

	return _x->tv_sec < _y->tv_sec;
}
#endif


static uint64_t s_eoy_sys_ticks_abstime_set(void)
{
    uint64_t time = 0;
    
    //#warning --> retrieve time with YARP
    
    return(time);
}


static void s_eoy_sys_abstime_set(uint64_t time)
{
    // do nothing ...
}

static uint64_t s_eoy_sys_nanotime_get(void)
{
    uint64_t nanotime = 0;
    
    //#warning --> retrieve nanotime time with YARP
    
    return(nanotime);
}

static void s_eoy_sys_stop(void)
{
    // do nothing
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




