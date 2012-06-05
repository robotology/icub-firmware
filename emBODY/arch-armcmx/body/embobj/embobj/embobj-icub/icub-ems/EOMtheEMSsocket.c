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
#include "EOMtheIPnet.h"

#include "EOsocketDatagram.h"
#include "EOMmutex.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSsocket.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "EOMtheEMSsocket_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const eOemssocket_cfg_t eom_emssocket_DefaultCfg = 
{
    EO_INIT(.inpdatagramnumber)         3, 
    EO_INIT(.outdatagramnumber)         1, 
    EO_INIT(.inpdatagramsizeof)         768, 
    EO_INIT(.outdatagramsizeof)         1024,
    EO_INIT(.localport)                 12345, 
    EO_INIT(.usemutex)                  eobool_true
};



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

static const char s_eobj_ownname[] = "EOMtheEMSsocket";

 
static EOMtheEMSsocket s_emssocket_singleton = 
{
    EO_INIT(.socket)                        NULL,
	EO_INIT(.cfg) 
    {
        EO_INIT(.inpdatagramnumber)         3, 
        EO_INIT(.outdatagramnumber)         1, 
        EO_INIT(.inpdatagramsizeof)         768, 
        EO_INIT(.outdatagramsizeof)         1024,
        EO_INIT(.localport)                 12345,
        EO_INIT(.usemutex)                  eobool_true
    },
    EO_INIT(.rxpkt)                         NULL,
    EO_INIT(.txpkt)                         NULL,
    EO_INIT(.hostaddress)                   EO_COMMON_IPV4ADDR_LOCALHOST,
    EO_INIT(.connected2host)                eobool_false,
    EO_INIT(.active)                        eobool_false
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern EOMtheEMSsocket * eom_emssocket_Initialise(const eOemssocket_cfg_t *cfg)
{
    if(NULL != s_emssocket_singleton.socket)
    {
        return(&s_emssocket_singleton);
    }
    
    if(NULL == cfg)
    {
        cfg = &eom_emssocket_DefaultCfg;
    }
    
    memcpy(&s_emssocket_singleton.cfg, cfg, sizeof(eOemssocket_cfg_t));
    
    if(NULL == eom_ipnet_GetHandle())
    {
        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, s_eobj_ownname, "the EOMtheIPnet has not started yet");
    }

    s_emssocket_singleton.socket = eo_socketdtg_New(    cfg->inpdatagramnumber, cfg->inpdatagramsizeof, (eobool_true == cfg->usemutex) ? (eom_mutex_New()) : (NULL), 
                                                        cfg->outdatagramnumber, cfg->outdatagramsizeof, (eobool_true == cfg->usemutex) ? (eom_mutex_New()) : (NULL)
                                                    );
                                                    
    s_emssocket_singleton.rxpkt = eo_packet_New(cfg->inpdatagramsizeof);                                                
    //s_emssocket_singleton.txpkt = eo_packet_New(cfg->outdatagramsizeof); 
    
    s_emssocket_singleton.active = eobool_false;
    
    return(&s_emssocket_singleton);
}


extern EOMtheEMSsocket* eom_emssocket_GetHandle(void) 
{
    if(NULL != s_emssocket_singleton.socket)
    {
        return(&s_emssocket_singleton);
    }
    else
    {
        return(NULL);
    }
}


extern eOresult_t eom_emssocket_Open(EOMtheEMSsocket *p, EOaction* withactiononrx)
{
    eOresult_t res;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_true == p->active)
    {
        eo_socketdtg_SetActions(p->socket, withactiononrx, NULL);
    }
    else
    {
    
        res = eo_socketdtg_Open(p->socket, p->cfg.localport, eo_sktdir_TXRX, eobool_false, 
                                NULL, withactiononrx, NULL);
        
        if(eores_OK == res)
        {
            char str[96];
            p->active = eobool_true;
            snprintf(str, sizeof(str)-1, "started socket listeing on local port %d\n\r", p->cfg.localport);
            eo_errman_Info(eo_errman_GetHandle(), s_eobj_ownname, str);                        
        }
    }                        
                            
    return(res);
}


extern eOresult_t eom_emssocket_Close(EOMtheEMSsocket *p)
{
    eOresult_t res;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->active)
    {
        return(eores_OK);
    }
    
    res = eo_socketdtg_Close(p->socket);
    
    p->active = eobool_false;
                                                       
    return(res);
}


extern eOsizecntnr_t eom_emssocket_NumberOfReceivedPackets(EOMtheEMSsocket *p)
{
    eOsizecntnr_t numberof = 0;
    
    if(NULL == p)
    {
        return(numberof);
    } 

    if(eobool_true == p->active)
    {
        eo_socketdtg_Received_NumberOf(p->socket, &numberof);
    }
    
    return(numberof);    
}


extern eOresult_t eom_emssocket_Receive(EOMtheEMSsocket *p, EOpacket** rxpkt, eOsizecntnr_t* remaining)
{
    eOresult_t res;
    
    if(NULL != remaining)
    {
        *remaining = 0;
    }       
    
    if((NULL == p) || (NULL == rxpkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->active)
    { 
        return(eores_NOK_generic);
    }  

    res = eo_socketdtg_Get(p->socket, p->rxpkt, eok_reltimeZERO);
    
    if(eores_OK != res)
    {
        eo_packet_Size_Set(p->rxpkt, 0);
    }
    else
    {
        eo_socketdtg_Received_NumberOf(p->socket, remaining);
    }
    
    *rxpkt = p->rxpkt;
    
    return(res);  
}

extern eOresult_t eom_emssocket_Connect(EOMtheEMSsocket *p, eOipv4addr_t remaddr)
{
    eOresult_t res = eores_OK;
    
    if(NULL == p)
    {
        return(eores_NOK_nullpointer);
    }

    if((eobool_false == p->connected2host) || (remaddr != p->hostaddress))
    {
        p->connected2host = eobool_false;
        p->hostaddress    = remaddr;
        
        res = eom_ipnet_ResolveIP(eom_ipnet_GetHandle(), remaddr, 5*EOK_reltime1sec);
 
        if(eores_OK != res)
        {
            return(res);
        }
        p->connected2host = eobool_true;        
    }
    
    return(eores_OK);   
}


extern eOresult_t eom_emssocket_Transmit(EOMtheEMSsocket *p, EOpacket* txpkt)
{
    eOresult_t res;
    eOipv4addr_t remaddr;
    eOipv4port_t remport;
    
    if((NULL == p) || (NULL == txpkt))
    {
        return(eores_NOK_nullpointer);
    }
    
    if(eobool_false == p->active)
    {
        return(eores_NOK_generic);
    } 
    
    eo_packet_Addressing_Get(txpkt, &remaddr, &remport);
    

    if((eobool_false == p->connected2host) || (remaddr != p->hostaddress))
    {
        res = eom_emssocket_Connect(p, remaddr);
        if(eores_OK != res)
        {
            return(res);
        }
    }
    
    res = eo_socketdtg_Put(p->socket, txpkt);
    
    return(res);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------





