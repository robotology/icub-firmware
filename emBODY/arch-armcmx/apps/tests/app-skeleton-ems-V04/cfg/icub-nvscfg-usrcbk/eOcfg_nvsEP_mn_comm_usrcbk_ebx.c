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

/* @file       eOcfg_nvsEP_mn_usrcbk_ebx.c
    @brief      This file keeps the user-defined local ...
    @author     marco.accame@iit.it
    @date       09/06/2011
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"

#include "eOcfg_nvsEP_mn_comm_con.h"

#include "EOarray.h"
#include "EOnv_hid.h"

#include "EOtheBOARDtransceiver.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "eOcfg_nvsEP_mn_hid.h"

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

static void s_eo_cfg_nvsEP_mngmnt_usr_ebx_generic_ropsigcfgcommand(eOmn_ropsigcfg_command_t* ropsigcfgcmd);

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


extern void eo_cfg_nvsEP_mn_comm_hid_INITIALISE(eOnvEP_t ep, void *loc, void *rem)
{
    eObool_t theOwnershipIsLocal = (NULL == rem) ? eobool_true : eobool_false;
    
    theOwnershipIsLocal = theOwnershipIsLocal;
    
    // initialisation of loc and rem is always done in the usr part of the endpoint and can never be overridden
    
    // nothing else ...

}


extern void eo_cfg_nvsEP_mn_comm_hid_INIT__ropsigcfgcommand(uint16_t n, const EOnv* nv)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    
    eOmn_ropsigcfg_command_t* cmdloc = (eOmn_ropsigcfg_command_t*)nv->loc;
    eOmn_ropsigcfg_command_t* cmdrem = (eOmn_ropsigcfg_command_t*)nv->rem;
    
    if(eobool_true == theOwnershipIsLocal)
    {   // function is called from within the local board
        s_eo_cfg_nvsEP_mngmnt_usr_ebx_generic_ropsigcfgcommand(cmdloc);  
    }
    else
    {   // function is called from within the remote host because it has initialised its data
        // it is possible to know which board owns the cmdrem from the ip address
        eOipv4addr_t ipaddress_of_remote_board = nv->ip;
        
        ipaddress_of_remote_board = ipaddress_of_remote_board;
        cmdrem = cmdrem;

    }    
}

extern void eo_cfg_nvsEP_mn_comm_hid_UPDT__ropsigcfgcommand(uint16_t n, const EOnv* nv, const eOabstime_t time, const uint32_t sign)
{
    eObool_t theOwnershipIsLocal = (NULL == nv->rem) ? eobool_true : eobool_false;
    
    eOmn_ropsigcfg_command_t* cmdloc = (eOmn_ropsigcfg_command_t*)nv->loc;
    eOmn_ropsigcfg_command_t* cmdrem = (eOmn_ropsigcfg_command_t*)nv->rem;
    
    if(eobool_true == theOwnershipIsLocal)
    {   // function is called from within the local board
        s_eo_cfg_nvsEP_mngmnt_usr_ebx_generic_ropsigcfgcommand(cmdloc);  
    }
    else
    {   // function is called from within the remote host because it has received a say or a sig
        // it is possible to know which board has sent the say/sig by the ipaddress
        eOipv4addr_t ipaddress_of_remote_board = nv->ip;
        
        ipaddress_of_remote_board = ipaddress_of_remote_board;
        cmdrem = cmdrem;

    }    
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_cfg_nvsEP_mngmnt_usr_ebx_generic_ropsigcfgcommand(eOmn_ropsigcfg_command_t* ropsigcfgcmd)
{
    uint8_t size, i;
    eOropSIGcfg_t *sigcfg;
    eo_transceiver_ropinfo_t ropinfo;
    EOtransceiver* theems00transceiver; 
    EOarray *array = (EOarray*)&ropsigcfgcmd->array;
    eOmn_ropsigcfg_commandtype_t cmmnd = (eOmn_ropsigcfg_commandtype_t)ropsigcfgcmd->cmmnd;

    eOresult_t res;
    
    if(NULL == (theems00transceiver = eo_boardtransceiver_GetHandle()))
    {
        return;
    }
    
    
    if((eo_array_ItemSize(array) != sizeof(eOropSIGcfg_t)) || (NUMOFROPSIGCFG != eo_array_Capacity(array)) || ((size = eo_array_Size(array)) > NUMOFROPSIGCFG))
    {
        return;
    }
    
    
    
    switch(cmmnd)
    {
    
        case ropsigcfg_cmd_clear:
        {   // just clear
            eo_transceiver_rop_regular_Clear(theems00transceiver);
        } break;
        
        case ropsigcfg_cmd_assign:
        {   // clear and load all the sigcfg in the array
            eo_transceiver_rop_regular_Clear(theems00transceiver);

            for(i=0; i<size; i++)
            {
                sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                ropinfo.ropcfg              = eok_ropconfig_basic;
                ropinfo.ropcfg.plustime     = sigcfg->plustime;
                ropinfo.ropcode             = eo_ropcode_sig;
                ropinfo.nvep                = sigcfg->ep;    
                ropinfo.nvid                = sigcfg->id;
                res = eo_transceiver_rop_regular_Load(theems00transceiver, &ropinfo);
                res = res;
            }        
        } break;
        
        case ropsigcfg_cmd_append:
        {   // dont clear and load all the sigcfg in the array
            for(i=0; i<size; i++)
            {
                sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                ropinfo.ropcfg              = eok_ropconfig_basic;
                ropinfo.ropcfg.plustime     = sigcfg->plustime;
                ropinfo.ropcode             = eo_ropcode_sig;
                ropinfo.nvep                = sigcfg->ep;    
                ropinfo.nvid                = sigcfg->id;
                res = eo_transceiver_rop_regular_Load(theems00transceiver, &ropinfo);
                res = res;
            }         
        } break;        

        case ropsigcfg_cmd_remove:
        {   // remove all the sigcfg in the array
            for(i=0; i<size; i++)
            {
                sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                ropinfo.ropcfg              = eok_ropconfig_basic;
                ropinfo.ropcfg.plustime     = sigcfg->plustime;
                ropinfo.ropcode             = eo_ropcode_sig;
                ropinfo.nvep                = sigcfg->ep;    
                ropinfo.nvid                = sigcfg->id;
                res = eo_transceiver_rop_regular_Unload(theems00transceiver, &ropinfo);
                res = res;
            }         
        } break;                

        
        default:
        {
            
        } break;
    }


}




// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



