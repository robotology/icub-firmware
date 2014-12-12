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

/* @file       eEcommon.c
    @brief      This header file implements common parts of embENV.
    @author     marco.accame@iit.it
    @date       11/20/2013
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdint.h"
#include "string.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "eEcommon.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
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
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// empty-section

                                                                             
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions

extern eEresult_t ee_common_ipnetwork_clr(eEipnetwork_t* ntw, uint64_t uniqueid)
{
//     uint8_t ip3 = uniqueid & 0xff;
//     uint8_t ip4 = (uniqueid>>8) & 0xff;
    const uint8_t ip3 = EECOMMON_ipaddr_def3;
    const uint8_t ip4 = EECOMMON_ipaddr_def4;    
    if(NULL == ntw)
    {
        return(ee_res_NOK_generic);
    }
    
    if(0 == uniqueid)
    {
        uniqueid = 0x2222;
    }
    
    ntw->macaddress  = (((uint64_t)EECOMMON_mac_oui_iit)) | ((uint64_t)((uniqueid) & 0xFFFFFF)<<24);
    ntw->ipaddress   = (uint32_t)EECOMMON_ipaddr_base_iit | EECOMMON_ipaddr_from(0, 0, ip3, ip4); 
    ntw->ipnetmask   = (uint32_t)EECOMMON_ipmask_default_iit; 
    
    return(ee_res_OK);
}


extern const char* ee_common_get_month_string(eEdate_t date)
{ 
    static const char * themonths[13] = 
    {
        "???",
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    
    if(date.month > 12)
    {
        return(themonths[0]);
    }  
    
    return(themonths[date.month]);
}

extern eEresult_t ee_is_extendemoduleinfo_valid(eEmoduleExtendedInfo_t* extmodinfo)
{
    if(NULL == extmodinfo)
    {
        return(ee_res_NOK_generic);
    }
    
    if(0 == strcmp((const char*)extmodinfo->moduleinfo.extra, "EXT"))
    {
        return(ee_res_OK);
    }
    return(ee_res_NOK_generic);
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

