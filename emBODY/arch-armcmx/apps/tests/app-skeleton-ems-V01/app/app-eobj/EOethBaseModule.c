/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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
// - doxy
// --------------------------------------------------------------------------------------------------------------------
// empty section


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"
//embobj
#include "EOtheErrormanager.h"
#include "EOtheMemoryPool.h"
#include "EOaction.h"
#include "EOpacket.h"
#include "EOMmutex.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "EOethBaseModule.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "EOethBaseModule_hid.h" 



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
//static const char s_eobj_ownname[] = "ethBaseModule";


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern EOethBaseModule* eo_ethBaseModule_New(EOethBaseModule_cfg_t *cfg)
{
    EOethBaseModule *retptr = NULL;
    if(NULL == cfg)
    {
        return(NULL);    
    }
  

    // i get the memory for the object
    retptr = eo_mempool_GetMemory(eo_mempool_GetHandle(), eo_mempool_align_32bit, sizeof(EOethBaseModule), 1);
    

    //Here I don't check (dtg_num > 0) and (dtg_size > 0), because it is already do in eo_socketdtg_New.
    //if the the check is not passed then the function calls EOerrorManager.
    
    retptr->socket = eo_socketdtg_New(cfg->dtagramQueue_itemNum, cfg->dtagramQueue_itemSize, eom_mutex_New(), // input queue: 4 datagram with mwax size = ETH_CONTROL_DGRAM_PAYLOAD_SIZE
                                        cfg->dtagramQueue_itemNum, cfg->dtagramQueue_itemSize, eom_mutex_New()  // input queue: 4 datagram with mwax size = ETH_CONTROL_DGRAM_PAYLOAD_SIZE
                                        );

    retptr->rxpkt = eo_packet_New(cfg->dtagramQueue_itemSize);
    retptr->txpkt = eo_packet_New(cfg->dtagramQueue_itemSize);
    retptr->localport = cfg->localport;
    retptr->remaddr = cfg->remaddr;
    retptr->remport = cfg->remport;
    retptr->periodTx = cfg->periodTx;

    retptr->action_onrx = eo_action_New();

    if(NULL != cfg->action_onRec)
    {
        eo_action_Copy(retptr->action_onrx, cfg->action_onRec);
    }
    
    return(retptr);
} 


extern eOresult_t eo_ethBaseModule_Activate(EOethBaseModule *p)
{
    eOresult_t res;
    eOsktdtgTXmode_t txmode;
    eOsktdtgTXmode_t *txmode_ptr = NULL;

    if(NULL == p)
    {
        return(eores_NOK_nullpointer);    
    }

    if(NULL == p->action_onrx)
    {
        return(eores_NOK_nodata);
    }

    //config txmode
    if(0 == p->periodTx)
    {
       txmode_ptr = NULL;  //tx-datagram will be sent immediately
    }
    else
    {
        //tx-datagram will be sent periodically
        txmode.startat = eok_abstimeNOW;
        txmode.after = p->periodTx;
        txmode.periodic = eobool_true;
        
        txmode_ptr = &txmode;     
    }

    /* Open the socket:
       - on port s_sysCntrl_port
       - in tx-rx 
       - in no-blocking mode 
       - execute s_action upon rx of a datagram
       - the task will be not advise on trasmission of a datagram.
     */
    res = eo_socketdtg_Open(p->socket, p->localport, eo_sktdir_TXRX, eobool_false, 
                            txmode_ptr, p->action_onrx, NULL);
    return(res);
}



extern eOresult_t eo_ethBaseModule_SetActionOnRec(EOethBaseModule *p, EOaction *action)
{
    if((NULL == p) || (NULL == action))
    {
        return(eores_NOK_nullpointer);    
    }

    eo_action_Copy(p->action_onrx, action);

    return(eores_OK);
}

extern eOresult_t eo_ethBaseModule_Deactivate(EOethBaseModule *p)
{
    eOresult_t res = eores_NOK_nullpointer;

    if(NULL == p) 
    {
        return(res);    
    }

    res = eo_socketdtg_Close(p->socket);

    return(res);
}

extern eOresult_t eo_ethBaseModule_Receive(EOethBaseModule *p, uint8_t **payload_ptr, uint16_t *payloadsize)
{
    eOresult_t res = eores_OK;
    
    if((NULL == p) || (NULL == payload_ptr) || (NULL == payloadsize))
    {
        return(eores_NOK_nullpointer);    
    }
        
    eo_packet_Full_Clear(p->rxpkt, 0); //clear the packet


    /*the third param is ignored, because the socket is not blocking*/
    res = eo_socketdtg_Get(p->socket, p->rxpkt, eok_reltimeZERO );
    //NOTE: the only reason I get error is caused by socket's fifo is emmpty
    if(eores_OK != res)
    {
        return(res);
    }

    eo_packet_Payload_Get(p->rxpkt, payload_ptr, payloadsize);
    
    if((0 == p->remaddr) || (0 == p->remport))
    {
        eo_packet_Destination_Get(p->rxpkt, &(p->remaddr), &(p->remport));
        eo_socketdtg_Connect(p->socket, p->remaddr, EOK_reltimeZERO); //arp force
    }

    return(res); 
}


extern eOresult_t eo_ethBaseModule_Transmit(EOethBaseModule *p, uint8_t *payload_ptr, uint16_t payloadsize)
{

    eOresult_t res = eores_OK;
    
    if((NULL == p) || (NULL == payload_ptr))
    {
        return(eores_NOK_nullpointer);    
    }

    if( (0 == payloadsize) || (0 == p->remaddr) || (0 == p->remport) )
    {
        return(eores_NOK_generic);
    }


    //set data in tx pkt
    eo_packet_Full_Set(p->txpkt, p->remaddr, p->remport, payloadsize, payload_ptr);
    /* TODO: ha piu' senso settare remaddr e remport una volta per tutte e poi settare solo i dati.
            i remaddr e remport si possono settare su conect.!!!
    */

    //send the pkt
    res = eo_socketdtg_Put(p->socket, p->txpkt);


    //prepare pkt for the next time (reinit with 0)
    eo_packet_Full_Clear(p->txpkt, 0);

    return(res);
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



