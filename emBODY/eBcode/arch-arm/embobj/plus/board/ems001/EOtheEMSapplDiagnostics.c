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
eOdgn_can_statistics_t              eo_dgn_canstatistics;
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

static EOTheEMSdiagnostics_t s_thedgn = 
{
    .ethError2signal = 0
};


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
    memset(&eo_dgn_canstatistics, 0, sizeof(eOdgn_can_statistics_t));
    eo_dgn_canstatistics.config_mode.stat[0].info_rx.min = 255;
    eo_dgn_canstatistics.config_mode.stat[0].info_tx.min = 255;
    eo_dgn_canstatistics.config_mode.stat[1].info_rx.min = 255;
    eo_dgn_canstatistics.config_mode.stat[1].info_tx.min = 255;
    eo_dgn_canstatistics.run_mode.stat[0].info_rx.min = 255;
    eo_dgn_canstatistics.run_mode.stat[0].info_tx.min = 255;
    eo_dgn_canstatistics.run_mode.stat[1].info_rx.min = 255;
    eo_dgn_canstatistics.run_mode.stat[1].info_tx.min = 255;
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


extern eOresult_t eo_theEMSdgn_checkEthLinksStatus_quickly(EOTheEMSdiagnostics_t* p, uint8_t *link1_isup, uint8_t *link2_isup)
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


extern void eo_theEMSdgn_updateCanRXqueueStatisticsOnRunMode(eOcanport_t canport, uint8_t numofRXcanframe)
{
    if(eo_dgn_canstatistics.run_mode.stat[canport].info_rx.max < numofRXcanframe)
    {
        eo_dgn_canstatistics.run_mode.stat[canport].info_rx.max = numofRXcanframe;
    }
    if((eo_dgn_canstatistics.run_mode.stat[canport].info_rx.min > numofRXcanframe) && (numofRXcanframe >0))
    {
        eo_dgn_canstatistics.run_mode.stat[canport].info_rx.min = numofRXcanframe;
    }
}

extern void eo_theEMSdgn_updateCanTXqueueStatisticsOnRunMode(eOcanport_t canport, uint8_t numofTXcanframe)
{
    if(eo_dgn_canstatistics.run_mode.stat[canport].info_tx.max < numofTXcanframe)
    {
        eo_dgn_canstatistics.run_mode.stat[canport].info_tx.max = numofTXcanframe;
    }
    if( (eo_dgn_canstatistics.run_mode.stat[canport].info_tx.min > numofTXcanframe) && (numofTXcanframe >0))
    {
        eo_dgn_canstatistics.run_mode.stat[canport].info_tx.min = numofTXcanframe;
    }
}




extern void eo_theEMSdgn_updateCanRXqueueStatisticsOnConfigMode(eOcanport_t canport, uint8_t numofRXcanframe)
{
    if(eo_dgn_canstatistics.config_mode.stat[canport].info_rx.max < numofRXcanframe)
    {
        eo_dgn_canstatistics.config_mode.stat[canport].info_rx.max = numofRXcanframe;
    }
    if((eo_dgn_canstatistics.config_mode.stat[canport].info_rx.min > numofRXcanframe) && (numofRXcanframe >0))
    {
        eo_dgn_canstatistics.config_mode.stat[canport].info_rx.min = numofRXcanframe;
    }
}

extern void eo_theEMSdgn_updateCanTXqueueStatisticsOnConfigMode(eOcanport_t canport, uint8_t numofTXcanframe)
{
    if(eo_dgn_canstatistics.config_mode.stat[canport].info_tx.max < numofTXcanframe)
    {
        eo_dgn_canstatistics.config_mode.stat[canport].info_tx.max = numofTXcanframe;
    }
    if((eo_dgn_canstatistics.config_mode.stat[canport].info_tx.min > numofTXcanframe) && (numofTXcanframe >0))
    {
        eo_dgn_canstatistics.config_mode.stat[canport].info_tx.min = numofTXcanframe;
    }
}


extern eOresult_t eo_theEMSdgn_checkEthLinksErrors(EOTheEMSdiagnostics_t* p)
{
    
    hal_result_t res;
    hal_eth_phy_errorsinfo_t result = {0};
    char str[50];
    uint8_t i;
    

//     if(0 == eo_dgn_cmds.signalEthCounters)
//     {
//         return(eores_OK);
//     }
    
    
    //reset net var bdoor
    memset(&eo_dgn_emsperiph.eth_dev.crcErrorCnt[0], 0, sizeof(uint32_t)*3);
    eo_dgn_emsperiph.eth_dev.crcErrorCnt_validVal=0;
    eo_dgn_emsperiph.eth_dev.crcErrorCnt_overflow = 0;
    
    
    for(i=0; i<3; i++)
    {
        res = hal_eth_get_errors_info(i, rxCrcError, &result);
        if(res != hal_res_OK)
        {
            eo_dgn_emsperiph.eth_dev.i2c_error[i]++;
            snprintf(str, sizeof(str)-1, "error in hal_eth_get_errors_info for phy %d", i);
        }
        else
        {
            snprintf(str, sizeof(str)-1, "ERR ETH PHY %d: val=%d overflow=%d validVal=%d", i, result.value, result.counteroverflow, result.validvalue);
            if(result.validvalue)
            {   
                eo_dgn_emsperiph.eth_dev.crcErrorCnt_validVal |= 1<<i;
                eo_dgn_emsperiph.eth_dev.crcErrorCnt[i] = result.value;
                eo_dgn_emsperiph.eth_dev.crcErrorCnt_overflow |=  result.counteroverflow << i;
            }
        }
        hal_trace_puts(str);
    }
    eo_theEMSdgn_Signalerror(p, eodgn_nvidbdoor_emsperiph , 0);
    
    
    return(eores_OK);

}

extern eOresult_t eo_theEMSdgn_checkEthLinkErrors(EOTheEMSdiagnostics_t* p, uint8_t linknum)
{
    
    hal_result_t res;
    hal_eth_phy_errorsinfo_t result = {0};
    char str[50];
    uint8_t i = linknum;
    

    if(0 == eo_dgn_cmds.signalEthCounters)
    {
        return(eores_OK);
    }

    if(linknum>2)
    {
        return(eores_NOK_nodata);
    }
    
    
    //reset net var bdoor
    eo_dgn_emsperiph.eth_dev.crcErrorCnt[i] = 0;
    eo_dgn_emsperiph.eth_dev.crcErrorCnt_validVal &= ~(1<<i);
    eo_dgn_emsperiph.eth_dev.crcErrorCnt_overflow &= ~(1<<i);
    
    
   
    res = hal_eth_get_errors_info(i, (hal_eth_phy_errors_info_type_t)s_thedgn.ethError2signal, &result);
    if(res != hal_res_OK)
    {
        snprintf(str, sizeof(str)-1, "error in hal_eth_get_errors_info for phy %d", i);
    }
    else
    {
        snprintf(str, sizeof(str)-1, "ERR ETH PHY %d: val=%d overflow=%d validVal=%d", i, result.value, result.counteroverflow, result.validvalue);
        if(result.validvalue)
        {   
            eo_dgn_emsperiph.eth_dev.crcErrorCnt_validVal |= 1<<i;
            eo_dgn_emsperiph.eth_dev.crcErrorCnt[i] = result.value;
            eo_dgn_emsperiph.eth_dev.crcErrorCnt_overflow |=  result.counteroverflow << i;
        }
    }
    hal_trace_puts(str);
    
    return(eores_OK);

}


extern uint8_t eo_theEMSdgn_EthLinksInError(EOTheEMSdiagnostics_t* p)
{
    if( (eo_dgn_emsperiph.eth_dev.crcErrorCnt[0] == 0) &&
        (eo_dgn_emsperiph.eth_dev.crcErrorCnt[1] == 0) &&
        (eo_dgn_emsperiph.eth_dev.crcErrorCnt[2] == 0) )
    {
        return(0);
    }
    return(1);
}


extern eOresult_t eo_theEMSdgn_setEthError2check(EOTheEMSdiagnostics_t* p, eOdgn_ethCounters_type_t errortype)
{

    switch(errortype)
    {
        case dgn_rxCrcError:
        {
            s_thedgn.ethError2signal = rxCrcError;
        }break;
        case dgn_rxUnicast:
        {
            s_thedgn.ethError2signal = rxUnicast;
        }break;
        case dgn_rx64Octets:
        {
            s_thedgn.ethError2signal = rx64Octets;
        }break;
        case dgn_txUnicast:
        {
            s_thedgn.ethError2signal = txUnicast;
        }break;
        
        default:
        {
            s_thedgn.ethError2signal = 0; //disable
        }
    }
    
    return(eores_OK);
    

}


extern eOresult_t eo_theEMSdgn_checkEthLinksStatus(EOTheEMSdiagnostics_t* p)
{
//     hal_eth_phy_status_t link_list[2];
//     hal_eth_phy_status_t* l_ptr;
//     uint8_t links_num = 0, i;
//     char str[150];
//     

//     hal_eth_get_links_status(link_list, 2);

//     for(i=0; i<2; i++)
//     {
//         l_ptr = &link_list[i];
//         snprintf(str, sizeof(str), "LinkStatus %d: up=%d  autoNegDone=%d, good=%d, speed=%d, duplex=%d ", i, l_ptr->linkisup, l_ptr->autoNeg_done, l_ptr->linkisgood,  l_ptr->linkspeed, l_ptr->linkduplex);
//         hal_trace_puts(str);
//     }
    return(eores_OK);
}



#if 0
extern void eo_theEMSdgn_updateCanQueuesStatistics(eOdgn_appl_phase_t phase)
{
    eOresult_t res;
    uint8_t numofRXcanframe = 0;
    uint8_t numofTXcanframe = 0;
    eOdgn_can_statistics_perPort_t *stat_ptr = NULL;
    EOappCanSP *appCanSP_ptr = eo_emsapplBody_GetCanServiceHandle(eo_emsapplBody_GetHandle());

//port 1
    res = eo_appCanSP_GetNumOfRecCanframe(appCanSP_ptr, eOcanport1, &numofRXcanframe);
    res = eo_appCanSP_GetNumOfTxCanframe(appCanSP_ptr, eOcanport1, &numofTXcanframe);
    stat_ptr = &eo_dgn_canstatistics.run_mode.rxPhase_stat[phase][eOcanport1];
    s_theEMSdgn_calculateCanQueuesStatistics(stat_ptr, numofRXcanframe, numofTXcanframe);


//port 2    
    res = eo_appCanSP_GetNumOfRecCanframe(appCanSP_ptr, eOcanport1, &numofRXcanframe);
    res = eo_appCanSP_GetNumOfTxCanframe(appCanSP_ptr, eOcanport1, &numofTXcanframe);
    stat_ptr = &eo_dgn_canstatistics.run_mode.rxPhase_stat[phase][eOcanport1];
    s_theEMSdgn_calculateCanQueuesStatistics(stat_ptr, numofRXcanframe, numofTXcanframe);      
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





