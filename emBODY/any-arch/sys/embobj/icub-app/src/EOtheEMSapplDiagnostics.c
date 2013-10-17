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

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "stdlib.h"
#include "string.h"
#include "EoCommon.h"

#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"
#include "EoDiagnostics.h"
#include "EOMtheEMSbackdoor.h"

#include "EOMtheEMStransceiver.h"
#include "EOMtheEMSrunner.h"
#include "EOMtheIPnet.h"

#include "hal.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheEMSapplDiagnostics.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOtheEMSapplDiagnostics_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
eOdgn_commands_t                    eo_dgn_cmds;
eOdgn_errorlog_t                    eo_dgn_errorlog;
eOdgn_emsperipheralstatus_t         eo_dgn_emsperiph;
eOdgn_emsapplication_common_t       eo_dgn_emsapplcore;
eOdgn_emsapplication_emswithmc_t    eo_dgn_emsappmc;
eOdgn_motorstatusflags_t            eo_dgn_motorstflag;
eOdgn_rxCheckSetpoints_t            eo_dgn_rxchecksepoints;


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static EOTheEMSdiagnostics_t s_thedgn = {0};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOTheEMSdiagnostics_t* eo_theEMSdgn_Initialize(void)
{
    memset(&eo_dgn_cmds, 0, sizeof(eOdgn_commands_t));
    memset(&eo_dgn_errorlog, 0, sizeof(eOdgn_errorlog_t));
    memset(&eo_dgn_emsperiph, 0, sizeof(eOdgn_emsperipheralstatus_t));
    memset(&eo_dgn_emsapplcore, 0, sizeof(eOdgn_emsapplication_common_t));
    memset(&eo_dgn_emsappmc, 0, sizeof(eOdgn_emsapplication_emswithmc_t));
    memset(&eo_dgn_motorstflag, 0, sizeof(eOdgn_motorstatusflags_t));
    memset(&eo_dgn_rxchecksepoints, 0, sizeof(eOdgn_rxCheckSetpoints_t));
    
    return(&s_thedgn);
}


extern EOTheEMSdiagnostics_t* eo_theEMSdgn_GetHandle(void)
{
    return(&s_thedgn);
}


extern eOresult_t eo_theEMSdgn_Signalerror(EOTheEMSdiagnostics_t* p, uint16_t var , eOreltime_t timeout)
{
    if((eo_dgn_cmds.enable) || (eodgn_nvidbdoor_errorlog == var))
    {    
        return (eom_emsbackdoor_Signal(eom_emsbackdoor_GetHandle(), var , timeout));
    }
    else
    {
        return(eores_OK);
    }
}



extern eObool_t eo_theEMSdgn_IsExtFault2Signal(EOTheEMSdiagnostics_t* p)
{
    if(NULL == p)
    {
        return(0);
    }
    
    return(eo_dgn_cmds.signalExtFault);
}



extern eOresult_t eo_theEMSdgn_UpdateApplCore(EOTheEMSdiagnostics_t* p)
{

//core
    eOemsrunner_diagnosticsinfo_t* dgn_run_ptr =  eom_emsrunner_GetDiagnosticsInfoHandle(eom_emsrunner_GetHandle());
    if(NULL != dgn_run_ptr)
    {    
        eo_dgn_emsapplcore.core.runst.numberofperiods = dgn_run_ptr->numberofperiods;
        eo_dgn_emsapplcore.core.runst.cumulativeabsoluteerrorinperiod = dgn_run_ptr->cumulativeabsoluteerrorinperiod;
        eo_dgn_emsapplcore.core.runst.meanofabsoluteerrorinperiod = dgn_run_ptr->meanofabsoluteerrorinperiod;
        eo_dgn_emsapplcore.core.runst.movingmeanofabsoluteerrorinperiod = dgn_run_ptr->movingmeanofabsoluteerrorinperiod;
        eo_dgn_emsapplcore.core.runst.maxabsoluteerrorinperiod = dgn_run_ptr->maxabsoluteerrorinperiod;
        eo_dgn_emsapplcore.core.runst.minabsoluteerrorinperiod = dgn_run_ptr->minabsoluteerrorinperiod;
        eo_dgn_emsapplcore.core.runst.executionoverflows[0] = dgn_run_ptr->executionoverflows[0];
        eo_dgn_emsapplcore.core.runst.executionoverflows[1] = dgn_run_ptr->executionoverflows[1];
        eo_dgn_emsapplcore.core.runst.executionoverflows[2] = dgn_run_ptr->executionoverflows[2];
        eo_dgn_emsapplcore.core.runst.datagrams_failed_to_go_in_txsocket = dgn_run_ptr->datagrams_failed_to_go_in_txsocket;
    }
    
//ipnet
    eOmipnet_diagnosticsinfo_t *dgn_ipnet_ptr = eom_ipnet_GetDiagnosticsInfoHandle(eom_ipnet_GetHandle());
    if(NULL != dgn_ipnet_ptr)
    {  
        eo_dgn_emsapplcore.ipnet.datagrams_failed_to_go_in_rxfifo = dgn_ipnet_ptr->datagrams_failed_to_go_in_rxfifo;
        eo_dgn_emsapplcore.ipnet.datagrams_failed_to_go_in_txosalqueue = dgn_ipnet_ptr->datagrams_failed_to_go_in_txosalqueue;
        eo_dgn_emsapplcore.ipnet.datagrams_failed_to_be_retrieved_from_txfifo = dgn_ipnet_ptr->datagrams_failed_to_be_retrieved_from_txfifo;
        eo_dgn_emsapplcore.ipnet.datagrams_failed_to_be_sent_by_ipal = dgn_ipnet_ptr->datagrams_failed_to_be_sent_by_ipal;  
    }
//transceiver
    eOemstransceiver_diagnosticsinfo_t* dgn_tr_ptr = eom_emstransceiver_GetDiagnosticsInfoHandle(eom_emstransceiver_GetHandle());
    if(NULL != dgn_tr_ptr)
    {  
        eo_dgn_emsapplcore.transceiver.rxinvalidropframes = dgn_tr_ptr->rxinvalidropframes;
        eo_dgn_emsapplcore.transceiver.rxseqnumwrong = dgn_tr_ptr->rxseqnumwrong;
        eo_dgn_emsapplcore.transceiver.lostreplies = dgn_tr_ptr->lostreplies;
        eo_dgn_emsapplcore.transceiver.failuresinloadofreplyropframe = dgn_tr_ptr->failuresinloadofreplyropframe;
        eo_dgn_emsapplcore.transceiver.txropframeistoobigforthepacket = dgn_tr_ptr->txropframeistoobigforthepacket;
        eo_dgn_emsapplcore.transceiver.cannotloadropinregulars = dgn_tr_ptr->cannotloadropinregulars;
        eo_dgn_emsapplcore.transceiver.cannotloadropinoccasionals = dgn_tr_ptr->cannotloadropinoccasionals;
    }
    
    return(eores_OK);
}

extern eOresult_t eo_theEMSdgn_UpdateApplWithMc(EOTheEMSdiagnostics_t* p, EOappEncReader* encreader_ptr, uint16_t count)
{
    uint8_t i;
    eOappEncReader_diagnosticsinfo_t* dgn_ptr = eo_appEncReader_GetDiagnosticsHandle(encreader_ptr);
    if(dgn_ptr == NULL)
    {
        return(eores_NOK_nullpointer);
    }
    
    for(i=0; i<6; i++)
    {
        eo_dgn_emsappmc.encreads.encList[i].err_onReadFromSpi = dgn_ptr->enclist[i][err_onReadFromSpi];
        eo_dgn_emsappmc.encreads.encList[i].err_onInvalidValue = dgn_ptr->enclist[i][err_onInvalidValue];
        eo_dgn_emsappmc.encreads.encList[i].err_onParityError = dgn_ptr->enclist[i][err_onParityError];
    }
    eo_dgn_emsappmc.encreads.count = count;
    return(eores_OK);
}

extern eOresult_t eo_theEMSdgn_UpdateDummyFieldOfApplWithMc(EOTheEMSdiagnostics_t* p, uint32_t data)
{

    eo_dgn_emsappmc.encreads.dummy = data;
    return(eores_OK);
}



extern eOresult_t eo_theEMSdgn_UpdateMotorStFlags(EOTheEMSdiagnostics_t* p, eOmc_motorId_t mId, uint8_t flags)
{
    if(mId >= 12)
    {
        return(eores_NOK_generic);
    }

    eo_dgn_motorstflag.motorlist[mId] = flags;

    return(eores_OK);
}

extern eOresult_t eo_theEMSdgn_ClearMotorStFlags(EOTheEMSdiagnostics_t* p)
{
     memset(&eo_dgn_motorstflag, 0, sizeof(eOdgn_motorstatusflags_t));
    return(eores_OK);
}

extern eOresult_t eo_theEMSdgn_UpdateErrorLog(EOTheEMSdiagnostics_t* p, char *str_error, uint16_t size)
{
    uint16_t minsize = ((size < eOdgn_errorlog_str_size) ? size: eOdgn_errorlog_str_size);
    
    memcpy(&eo_dgn_errorlog.errorstate_str[0], str_error, minsize);
    return(eores_OK);
}


extern eOresult_t eo_theEMSdgn_checkEthLinkStatus(EOTheEMSdiagnostics_t* p, uint8_t *link1_isup, uint8_t *link2_isup)
{

//     uint8_t PHYaddr = 0x1;
//     uint8_t REGaddr = 0x1;
//     uint16_t status_link;
     char str[50];
//     #define LINK_IS_UP 0x00000004
    uint8_t linkst_mask, links_num;
    static uint8_t link_st[2] = {0};
    uint8_t state2notify = 0;
    
    
    if((NULL == p) || (NULL == link1_isup) || (NULL == link2_isup))
    {
        return(eores_NOK_nullpointer);
    }
    
    hal_eth_check_links(&linkst_mask, &links_num);
    
    if((linkst_mask & 0x01) == 0x01)
    {
        *link1_isup = 1;
        if(link_st[0] == 0)
        {
            snprintf(str, sizeof(str)-1, "Link 0 down --> up (mask=%d)", linkst_mask);
            hal_trace_puts(str);
            link_st[0] = 1;
            state2notify = 1;
        }
    }
    else
    {
         hal_led_on(hal_led1);
        if(link_st[0] == 1)
        {
            snprintf(str, sizeof(str)-1, "Link 0 up --> down (mask=%d)", linkst_mask);
            hal_trace_puts(str);
            link_st[0] = 0;
            state2notify = 1;
        }
 
        
    }

     if((linkst_mask & 0x02) == 0x02)
    {
        *link2_isup = 1;
         if(link_st[1] == 0)
        {
            snprintf(str, sizeof(str)-1, "Link 1 down --> up (mask=%d)", linkst_mask);
            hal_trace_puts(str);
            link_st[1] = 1;
            state2notify = 1;
        }
    }
    else
    {
        hal_led_on(hal_led2);
        if(link_st[1] == 1)
        {
            snprintf(str, sizeof(str)-1, "Link 1 up --> down (mask=%d)", linkst_mask);
            hal_trace_puts(str);
            link_st[1] = 0;
            state2notify = 1;
        }
    }
    
    if(state2notify)
    {
         eo_dgn_emsperiph.eth_dev.linksmask = linkst_mask;
        eo_theEMSdgn_Signalerror(&s_thedgn, eodgn_nvidbdoor_emsperiph , 0);
        snprintf(str, sizeof(str)-1, "Signal error %d ", linkst_mask);
        hal_trace_puts(str);
    }
//     snprintf(str, sizeof(str)-1, "LINKs mask %d ", linkst_mask);
//     hal_trace_puts(str);
 
    
    
//     *link1_isup = 0;
//     *link2_isup = 0;
//     
//     status_link = hal_eth_smi_read(PHYaddr, REGaddr);
//     if(EO_COMMON_CHECK_FLAG(status_link, LINK_IS_UP))
//     {
//         *link1_isup = 1;
//         snprintf(str, sizeof(str)-1, "LINK 1 is up!! %d", status_link);
//         hal_trace_puts(str);
//     }
//     
//     PHYaddr = 0x2;
//     status_link = hal_eth_smi_read(PHYaddr, REGaddr);
//     if(EO_COMMON_CHECK_FLAG(status_link, LINK_IS_UP))
//     {
//         *link2_isup = 1;
//         snprintf(str, sizeof(str)-1, "LINK 2 is up!! %d", status_link);
//         hal_trace_puts(str);
//     }
    
    return(eores_OK);

}


extern eOresult_t eo_theEMSdgn_resetSetpoints(EOTheEMSdiagnostics_t* p)
{
    uint8_t i;
    for(i=0; i<4; i++)
    {
        eo_dgn_rxchecksepoints.position[i].deltaprognumber = INT32_MAX;
        eo_dgn_rxchecksepoints.position[i].deltarxtime = UINT32_MAX;
        eo_dgn_rxchecksepoints.impedence[i].deltaprognumber = INT32_MAX;
        eo_dgn_rxchecksepoints.impedence[i].deltarxtime = UINT32_MAX;
    } 
    return(eores_OK);    
}
#if 0
extern eOresult_t eo_theEMSdgn_checkErrors(EOTheEMSdiagnostics_t* p)
{
    
    hal_result_t res;
    hal_eth_phy_errorsinfo_t result;
    char str[50];
    
    result.value = 0xAA;
    res = hal_eth_get_errors_info(0, rxCrcError, &result);
    if(res != hal_res_OK)
    {
        snprintf(str, sizeof(str)-1, "error in hal_eth_get_errors_info for phy 0");
    }
    else
    {
        snprintf(str, sizeof(str)-1, "ERR ETH PHY 0: val=%d overflow=%d validVal=%d", result.value, result.counteroverflow, result.invalidvalue);
    }
    hal_trace_puts(str);
    
    
    
    res = hal_eth_get_errors_info(1, rxCrcError, &result);
    if(res != hal_res_OK)
    {
        snprintf(str, sizeof(str)-1, "error in hal_eth_get_errors_info for phy 1");
    }
    else
    {
        snprintf(str, sizeof(str)-1, "ERR ETH PHY 1: val=%d overflow=%d validVal=%d", result.value, result.counteroverflow, result.invalidvalue);
    }
    hal_trace_puts(str);
    
    return(eores_OK);
}



extern eOresult_t eo_theEMSdgn_checkLinksStatus(EOTheEMSdiagnostics_t* p)
{
    hal_eth_phy_status_t* link_list[2];
    uint8_t links_num = 0;
    
    hal_eth_get_links_status(link_list, &links_num);
    
    return(eores_OK);
}

#endif
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





