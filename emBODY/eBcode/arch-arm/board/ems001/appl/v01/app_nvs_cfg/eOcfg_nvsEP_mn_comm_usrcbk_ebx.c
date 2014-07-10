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

#if !defined(EOPROT_USE_MN_VERSION_1_0) & !defined(EOPROT_USE_MN_VERSION_2_0)
    #define EOPROT_USE_MN_VERSION_1_0
#endif

#if     !defined(EOPROT_USE_MN_VERSION_1_0)
        // marco.accame:    keep the double mode just for some days, until the blue robot is done.
        //                  forgive me for the double code block in the file 
        
        #include "EoProtocolEPmn_fun_overridden.c"

        
#else//!defined(EOPROT_USE_MN_VERSION_1_0)

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h" 
#include "string.h"
#include "stdio.h"

#include "EoCommon.h"

//#include "eOcfg_nvsEP_mn_comm_con.h"

#include "EOarray.h"
#include "EOnv_hid.h"

#include "EOtheBOARDtransceiver.h"
#include "EoProtocol.h"

//application
// #include "EOMtheEMSconfigurator.h"
#include "EOMtheEMSappl.h"
#include "EOtheEMSapplBody.h"

#include "EOtheEMSapplDiagnostics.h"

#include "EOtheErrorManager.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


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



extern void eoprot_fun_INIT_mn_comm_cmmnds_ropsigcfg(const EOnv* nv)
{
    
    eOmn_ropsigcfg_command_t* cmdloc = (eOmn_ropsigcfg_command_t*)nv->ram;
    
    if(eobool_true == eo_nv_hid_isLocal(nv))
    {   // function is called from within the local board
        s_eo_cfg_nvsEP_mngmnt_usr_ebx_generic_ropsigcfgcommand(cmdloc);  
    }
    else
    {   // function is called from within the remote host because it has initialised its data
        // it is possible to know which board owns the cmdrem from the ip address
        eOipv4addr_t ipaddress_of_remote_board = nv->ip;
        
        ipaddress_of_remote_board = ipaddress_of_remote_board;

    }    
}

extern void eoprot_fun_UPDT_mn_comm_cmmnds_ropsigcfg(const EOnv* nv, const eOropdescriptor_t* rd)
{
    eOmn_ropsigcfg_command_t* cmdloc = (eOmn_ropsigcfg_command_t*)nv->ram;
    
    if(eobool_true == eo_nv_hid_isLocal(nv))
    {   // function is called from within the local board
        s_eo_cfg_nvsEP_mngmnt_usr_ebx_generic_ropsigcfgcommand(cmdloc);  
    }
    else
    {   // function is called from within the remote host because it has received a say or a sig
        // it is possible to know which board has sent the say/sig by the ipaddress
        eOipv4addr_t ipaddress_of_remote_board = nv->ip;
        
        ipaddress_of_remote_board = ipaddress_of_remote_board;
    }    
}


extern void eoprot_fun_UPDT_mn_appl_cmmnds_go2state(const EOnv* nv, const eOropdescriptor_t* rd)
{   // n is always 0

    eOmn_appl_state_t *newstate_ptr = (eOmn_appl_state_t *)nv->ram;

    switch(*newstate_ptr)
    {
        case applstate_config:
        {
            eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STcfg);
        }break;

        case applstate_running:
        {
            ///*///
            uint32_t canBoardsReady;
            char str[60];
            if(!eo_appTheDB_areConnectedCanBoardsReady(eo_emsapplBody_GetDataBaseHandle(eo_emsapplBody_GetHandle()), &canBoardsReady))
            {
                snprintf(str, sizeof(str), "not all boards are ready mask=%u", canBoardsReady);
                eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, "on rec go2run cmd", str);
            }
            ///*///
            eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STrun);
        }break;
        
        case applstate_error:
        {
            //I don't expect to receive go to error cmd
            eom_emsappl_ProcessGo2stateRequest(eom_emsappl_GetHandle(), eo_sm_emsappl_STerr);
        }break;

    }

}



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static void s_eo_cfg_nvsEP_mngmnt_usr_ebx_generic_ropsigcfgcommand(eOmn_ropsigcfg_command_t* ropsigcfgcmd)
{
    uint8_t size, i;
    eOropSIGcfg_t *sigcfg;
    //eo_transceiver_ropinfo_t ropinfo;
    eOropdescriptor_t ropdesc;
    EOtransceiver* theems00transceiver; 
    EOarray *array = (EOarray*)&ropsigcfgcmd->array;
    eOmn_ropsigcfg_commandtype_t cmmnd = (eOmn_ropsigcfg_commandtype_t)ropsigcfgcmd->cmmnd;

    eOresult_t res;
    
    if(NULL == (theems00transceiver = eo_boardtransceiver_GetTransceiver(eo_boardtransceiver_GetHandle())))
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
            eo_transceiver_RegularROPs_Clear(theems00transceiver);
        } break;
        
        case ropsigcfg_cmd_assign:
        {   // clear and load all the sigcfg in the array
            eo_transceiver_RegularROPs_Clear(theems00transceiver);

            for(i=0; i<size; i++)
            {
                sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
                ropdesc.control.plustime        = (eobool_true == ropsigcfgcmd->plustime) ? (1) : (0);
                ropdesc.control.plussign        = (eobool_true == ropsigcfgcmd->plussign) ? (1) : (0);
                ropdesc.ropcode                 = eo_ropcode_sig;
                ropdesc.id32                    = sigcfg->id32;    
                ropdesc.signature               = ropsigcfgcmd->signature;   
                res = eo_transceiver_RegularROP_Load(theems00transceiver, &ropdesc);
                res = res;
                if(eores_OK != res)
                {
                    eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());
                    eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , 1000);
                }
            }        
        } break;
        
        case ropsigcfg_cmd_append:
        {   // dont clear and load all the sigcfg in the array
            for(i=0; i<size; i++)
            {
                sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
                ropdesc.control.plustime        = (eobool_true == ropsigcfgcmd->plustime) ? (1) : (0);
                ropdesc.control.plussign        = (eobool_true == ropsigcfgcmd->plussign) ? (1) : (0);
                ropdesc.ropcode                 = eo_ropcode_sig;
                ropdesc.id32                    = sigcfg->id32;
                ropdesc.signature               = ropsigcfgcmd->signature;
                res = eo_transceiver_RegularROP_Load(theems00transceiver, &ropdesc);
                res = res;
                if(eores_OK != res)
                {
                    eo_theEMSdgn_UpdateApplCore(eo_theEMSdgn_GetHandle());
                    eo_theEMSdgn_Signalerror(eo_theEMSdgn_GetHandle(), eodgn_nvidbdoor_emsapplcommon , 1000);
                }
            }         
        } break;        

        case ropsigcfg_cmd_remove:
        {   // remove all the sigcfg in the array
            for(i=0; i<size; i++)
            {
               sigcfg = (eOropSIGcfg_t*)eo_array_At(array, i);
                memcpy(&ropdesc.control, &eok_ropctrl_basic, sizeof(eOropctrl_t));
                ropdesc.control.plustime        = (eobool_true == ropsigcfgcmd->plustime) ? (1) : (0);
                ropdesc.control.plussign        = (eobool_true == ropsigcfgcmd->plussign) ? (1) : (0);
                ropdesc.ropcode                 = eo_ropcode_sig;
                ropdesc.id32                    = sigcfg->id32;
                ropdesc.signature               = ropsigcfgcmd->signature;
                res = eo_transceiver_RegularROP_Unload(theems00transceiver, &ropdesc);
                res = res;
            }         
        } break;                

        
        default:
        {
            
        } break;
    }


}


#endif//!defined(EOPROT_USE_MN_VERSION_1_0)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



