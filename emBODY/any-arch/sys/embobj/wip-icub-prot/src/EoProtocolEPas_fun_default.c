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

/* @file       EoProtocolEPas_fun_default.c
    @brief      This file keeps constant configuration for ...
    @author     marco.accame@iit.it
    @date       06/06/2013
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EoProtocolEPas_fun.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define EOPROT_EP_AS_FUN_OVERRIDE

#if defined(EOPROT_EP_AS_FUN_OVERRIDE)
#include "EoProtocolEPas_fun_overridden.h"
#endif




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables
// --------------------------------------------------------------------------------------------------------------------
// empty-section




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

#if !defined(OVERRIDE_eoprot_ep_as_fun_INITIALISE)
__weak extern void eoprot_ep_as_fun_INITIALISE(eOnvEP_t ep, void *ram) {}
#endif



#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_strain_config)
__weak extern void eoprot_ep_as_fun_INIT_strain_config(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_strain_config)
__weak extern void eoprot_ep_as_fun_UPDT_strain_config(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_strain_config__mode)
__weak extern void eoprot_ep_as_fun_INIT_strain_config__mode(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_strain_config__mode)
__weak extern void eoprot_ep_as_fun_UPDT_strain_config__mode(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_strain_config__datarate)
__weak extern void eoprot_ep_as_fun_INIT_strain_config__datarate(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_strain_config__datarate)
__weak extern void eoprot_ep_as_fun_UPDT_strain_config__datarate(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_strain_config__signaloncefullscale)
__weak extern void eoprot_ep_as_fun_INIT_strain_config__signaloncefullscale(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_strain_config__signaloncefullscale)
__weak extern void eoprot_ep_as_fun_UPDT_strain_config__signaloncefullscale(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_strain_status)
__weak extern void eoprot_ep_as_fun_INIT_strain_status(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_strain_status)
__weak extern void eoprot_ep_as_fun_UPDT_strain_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_strain_status__fullscale)
__weak extern void eoprot_ep_as_fun_INIT_strain_status__fullscale(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_strain_status__fullscale)
__weak extern void eoprot_ep_as_fun_UPDT_strain_status__fullscale(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_strain_status__calibratedvalues)
__weak extern void eoprot_ep_as_fun_INIT_strain_status__calibratedvalues(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_strain_status__calibratedvalues)
__weak extern void eoprot_ep_as_fun_UPDT_strain_status__calibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_strain_status__uncalibratedvalues)
__weak extern void eoprot_ep_as_fun_INIT_strain_status__uncalibratedvalues(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_strain_status__uncalibratedvalues)
__weak extern void eoprot_ep_as_fun_UPDT_strain_status__uncalibratedvalues(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_mais_config)
__weak extern void eoprot_ep_as_fun_INIT_mais_config(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_mais_config)
__weak extern void eoprot_ep_as_fun_UPDT_mais_config(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_mais_config__mode)
__weak extern void eoprot_ep_as_fun_INIT_mais_config__mode(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_mais_config__mode)
__weak extern void eoprot_ep_as_fun_UPDT_mais_config__mode(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_mais_config__datarate)
__weak extern void eoprot_ep_as_fun_INIT_mais_config__datarate(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_mais_config__datarate)
__weak extern void eoprot_ep_as_fun_UPDT_mais_config__datarate(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_mais_config__resolution)
__weak extern void eoprot_ep_as_fun_INIT_mais_config__resolution(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_mais_config__resolution)
__weak extern void eoprot_ep_as_fun_UPDT_mais_config__resolution(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_mais_status)
__weak extern void eoprot_ep_as_fun_INIT_mais_status(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_mais_status)
__weak extern void eoprot_ep_as_fun_UPDT_mais_status(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif

#if !defined(OVERRIDE_eoprot_ep_as_fun_INIT_mais_status__the15values)
__weak extern void eoprot_ep_as_fun_INIT_mais_status__the15values(const EOnv* nv) {}
#endif
#if !defined(OVERRIDE_eoprot_ep_as_fun_UPDT_mais_status__the15values)
__weak extern void eoprot_ep_as_fun_UPDT_mais_status__the15values(const EOnv* nv, const eOropdescriptor_t* rd) {}
#endif


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

