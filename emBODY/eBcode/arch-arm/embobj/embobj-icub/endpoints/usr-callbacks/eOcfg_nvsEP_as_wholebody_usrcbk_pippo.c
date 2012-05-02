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

/* @file       eOcfg_nvsEP_as_wholebody_usrcbk_pippo.c
    @brief      This file keeps the user-defined functions used in every ems board ebx for endpoint mngmnt
                The behaviour is the same in every ems because on reception of ropsigcfgassign, the transceiver must
                load the rop descriptors for regular signalling.                
    @author     marco.accame@iit.it
    @date       05/02/2012
**/





// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"

#include "eOcfg_nvsEP_as_wholebody_con.h"


#include "EOnv_hid.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_as_wholebody_usr_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section



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



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


extern void eo_cfg_nvsEP_as_wholebody_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    
    if(eobool_true == theOwnershipIsLocal)
    {   // function is called from within the local board
        memcpy(loc, &eo_cfg_nvsEP_as_wholebody_default, sizeof(eo_cfg_nvsEP_as_wholebody_t));
    }
    else
    {   // function is called from within the remote host to initialise the data of the endopoint.
        // there is one of such a call for each class of the pc104 which has the as_wholebody as a remotely owned endpoint
        // it is NOT possible to undestand who is the caller 
        memcpy(loc, &eo_cfg_nvsEP_as_wholebody_default, sizeof(eo_cfg_nvsEP_as_wholebody_t));
        if(NULL != rem)
        {   
            memcpy(rem, &eo_cfg_nvsEP_as_wholebody_default, sizeof(eo_cfg_nvsEP_as_wholebody_t));
        }        
    }
    
    // nothing else ...

}



extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Sxx_sconfig__datarate(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eo_cfg_nvsEP_as_wholebody_con_strainNumber_t sxx = (eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx;
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOipv4addr_t ip = nv->ip;
    eOnvEP_t ep = nv->ep;

    
    uint8_t milliseconds = 0;

    
    if(eobool_true == theOwnershipIsLocal)
    {   // function is called from within the local board when it has just received a set/rst of this variable
        
        milliseconds = *((uint8_t*)nv->loc);
        
        // use milliseconds to configure the data rate of strain number sxx (strainLeftArm_00, strainRigthArm_01, strainLeftLeg_02, strainRigthLeg_03).
        milliseconds = milliseconds;
    }
    else
    {   // function is called from within the remote host because it has received a say or a sig
        // there is one of such a call for each class of the pc104 which has the as_wholebody as a remotely owned endpoint
        // it IS possible to undestand who is the caller by evaluating the nv->ip 
        eOipv4addr_t ipaddress_of_remote_board = nv->ip;
        ipaddress_of_remote_board = ipaddress_of_remote_board;
        
        // the received values are in ...
        milliseconds = *((uint8_t*)nv->rem);
    }    
}





extern void eo_cfg_nvsEP_as_wholebody_usr_hid_UPDT_Sxx_sstatus__calibratedvalues(uint16_t xx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eo_cfg_nvsEP_as_wholebody_con_strainNumber_t sxx = (eo_cfg_nvsEP_as_wholebody_con_strainNumber_t)xx;
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    eOipv4addr_t ip = nv->ip;
    eOnvEP_t ep = nv->ep;

    
    eOsnsr_arrayofupto12bytes_t *calibratedvalues;

    
    if(eobool_true == theOwnershipIsLocal)
    {   // function is called from within the local board ... it does not happen
        

    }
    else
    {   // function is called from within the remote host because it has received a say or a sig
        // there is one of such a call for each class of the pc104 which has the as_wholebody as a remotely owned endpoint
        // it IS possible to undestand who is the caller by evaluating the nv->ip 
        eOipv4addr_t ipaddress_of_remote_board = nv->ip;
        ipaddress_of_remote_board = ipaddress_of_remote_board;
        
        // the received values are in ...
        calibratedvalues = (eOsnsr_arrayofupto12bytes_t*) nv->rem;
        uint8_t size = eo_array_Size((EOarray*)calibratedvalues);
        uint8_t itemsize = eo_array_ItemSize((EOarray*)calibratedvalues)); // must be 2 bytes.
        
        uint16_t *valueref = (uint16_t*) eo_array_At((EOarray*)calibratedvalues, 0); // the first of six values
    }    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



