    /*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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

/* @file       ipal_f_arp.c
	@brief      This file implements internal implementation of the fake ipal arpmodule
	@author     marco.accame@iit.it
    @date       11/03/2014
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
//#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
//#include "ipal_numberof.h"

#ifdef IPAL_USE_ARP


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "time.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal.h"




// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

//#include "ipal_f_arp_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern ipal_result_t ipal_arp_request(ipal_ipv4addr_t ip, ipal_arp_cachemode_t cm)
{   // legacy function
    return(ipal_res_NOK_generic);
}

extern ipal_result_t ipal_arp_resolve(ipal_ipv4addr_t ip, ipal_arp_cachemode_t cm, uint8_t forcearpframe)
{
    return(ipal_res_OK);
}

extern ipal_result_t ipal_arp_getmac(ipal_ipv4addr_t ip, uint8_t* mac)
{  
    if(NULL != mac)
    {
        static uint8_t rr[6] = {0};
        memcpy(mac, rr, 6);
        return(ipal_res_OK);
    }
    
    return(ipal_res_NOK_generic);
}

extern ipal_result_t ipal_arp_isresolved(ipal_ipv4addr_t ip)
{
    return(ipal_res_OK);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------



#endif//IPAL_USE_ARP

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




