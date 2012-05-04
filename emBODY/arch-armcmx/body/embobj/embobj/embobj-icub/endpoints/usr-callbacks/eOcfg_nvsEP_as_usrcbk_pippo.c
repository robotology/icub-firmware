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

/* @file       eOcfg_nvsEP_as_usrcbk_pippo.c
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

#include "eOcfg_nvsEP_as.h"


#include "EOnv_hid.h"

#include "EOarray.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_as_hid.h"

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


extern void eo_cfg_nvsEP_as_usr_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    uint16_t s, m;

#if 0    
    eo_cfg_nvsEP_as_wholebody_t *lloc = (eo_cfg_nvsEP_as_wholebody_t*)loc;
    eo_cfg_nvsEP_as_wholebody_t *rrem = (eo_cfg_nvsEP_as_wholebody_t*)rem; 
    
    if(eobool_true == theOwnershipIsLocal)
    {   // function is called from within the local board
  
        for(s=0; s<strainWholeBody_TOTALnumber; s++)
        {
            memcpy(&lloc->strains[s], eo_cfg_nvsEP_as_wholebody_strain_defaultvalue, sizeof(eOsnsr_strain_t)); 
        }
        for(m=0; m<maisWholeBody_TOTALnumber; m++)
        {
            memcpy(&lloc->maises[m], eo_cfg_nvsEP_as_wholebody_mais_defaultvalue, sizeof(eOsnsr_mais_t)); 
        } 
                       
    }
    else
    {   // function is called from within the remote host to initialise the data of the endopoint.
        // there is one of such a call for each class of the pc104 which has the as_wholebody as a remotely owned endpoint
        // it is NOT possible to undestand who is the caller 
        // we can initialise both local and remote memory
        rrem = rrem;
        lloc = lloc;     
    }
#endif

#warning --> to rewrite teh example ....    
    // nothing else ...

}



extern void eo_cfg_nvsEP_as_usr_hid_UPDT_Sxx_sconfig__datarate(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
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





extern void eo_cfg_nvsEP_as_usr_hid_UPDT_Sxx_sstatus__calibratedvalues(eo_cfg_nvsEP_as_strainNumber_t sxx, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
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
        
        uint8_t size = 0;
        uint8_t itemsize = 0;
        
        uint16_t *valueref = NULL;
        
        // the received values are in ...
        calibratedvalues = (eOsnsr_arrayofupto12bytes_t*) nv->rem;
        size = eo_array_Size((EOarray*)calibratedvalues);
        itemsize = eo_array_ItemSize((EOarray*)calibratedvalues); // must be 2 bytes.
        
        valueref = (uint16_t*) eo_array_At((EOarray*)calibratedvalues, 0); // the first of six values
    }    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



