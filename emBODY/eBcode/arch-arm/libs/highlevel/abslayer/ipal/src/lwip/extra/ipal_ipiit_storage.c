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

/* @file       ipal_ipiit_storage.c
	@brief      This file contains the allocation of variables and the definition of functions which are required by 
                the lwip library.
	@author     valentina.gaggero@iit.it
    @date       01/10/2013
**/

// - modules to be built: contains the IPAL_USE_* macros --------------------------------------------------------------
#include "ipal_modules.h"

// - sizes of modules: contains the IPAL_NUMBEROF_* macros ----------------------------------------------------------------
#include "ipal_numberof.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "time.h"

#include "lwip/mem.h"

#include "ipal_base_hid.h"
#include "ipal_sys_hid.h"
#include "ipal_fifo_hid.h"
#include "ipal_eth_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_ipiit_storage.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_ipiit_storage_hid.h"

// --------------------------------------------------------------------------------------------------------------------
// - ordered by module
// --------------------------------------------------------------------------------------------------------------------


#if defined(IPAL_USE_ARP)

// - lwip: variables required -------------------------------------------------------------------------------------- 


// - lwip: functions required --------------------------------------------------------------------------------------


// - ipal: functions required ----------------------------------------------------------------------------------------

extern ipal_result_t ipal_ipiit_storage_arp_init(const ipal_cfg_t *cfg)
{
    return(ipal_res_OK);
}

#endif//IPAL_USE_ARP



#if defined(IPAL_USE_DHCP)

// - lwip: variables required -------------------------------------------------------------------------------------- 


// - lwip: functions required --------------------------------------------------------------------------------------
// none

// - ipal: functions required ----------------------------------------------------------------------------------------
extern ipal_result_t ipal_ipiit_storage_dhcp_init(const ipal_cfg_t *cfg)
{
    return(ipal_res_OK);
}

#endif//IPAL_USE_DHCP


#if defined(IPAL_USE_ETH)

// - lwip: variables required -------------------------------------------------------------------------------------- 

// - lwip: functions required --------------------------------------------------------------------------------------

typedef struct
{
    void     *frame;
    uint32_t length;
}ipal_genericframe_t;


typedef struct
{
    uint32_t (*move2higherlayer)(uint8_t *inputbuffer, uint32_t size);
    uint32_t (*move2lowerlayer)(ipal_genericframe_t *genframe, uint8_t *outputbuffer);
    void (*alerthigherlayer)(void);
} ipal_network_hw_init_arg0_t;

typedef struct
{
    uint8_t                         priority;
    ipal_network_hw_init_arg0_t*    onframerx;
    uint64_t                        macaddress;
} ipal_network_hw_init_cfg_t;

static uint32_t s_ipal_eth_moveframe2higherlayer(uint8_t *inputbuffer, uint32_t size)
{
    ipal_eth_hid_netif *netif =  ipal_eth_hid_getnetif();
    err_t err;
    struct pbuf * pbuf_ptr, *q;
    ipal_result_t res;
    uint32_t l=0;
    
    if(0 == size)
    {
        ipal_base_hid_on_fatalerror(ipal_error_internal0, "in moveframe2higherlayer required 0 size");
    }
    //trsformo da hal_buf in lwip_buf
    pbuf_ptr = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
    if(NULL == pbuf_ptr)
    {
        ipal_base_hid_on_fatalerror(ipal_error_internal0, "pbuf_alloc returns NULL in moveframe2higherlayer");
        return(0);
    }
    

    for (q = pbuf_ptr; q != NULL; q = q->next)
    {
        memcpy((u8_t*)q->payload, (u8_t*)&inputbuffer[l], q->len);
        l = l + q->len;
    } 


    
    ipal_fifo_item_t item =(void*) pbuf_ptr;
    
    //ipal_base_hid_cfg.extfn.hal_eth_disable(); non dovrebbero servirmi perche' la funz e' chiamata da dentro la isr
    res = ipal_fifo_hid_put((ipal_fifo_t*)(netif->netif.state), &item);
    //ipal_base_hid_cfg.extfn.hal_eth_enable();
    
    if(ipal_res_OK != res)
    {
        ipal_base_hid_on_fatalerror(ipal_error_internal0, "no more space in netif fifo");
        return(0);
    }
    
    return(size);
}

static uint32_t s_ipal_eth_moveframe2lowerlayer(ipal_genericframe_t *genframe, uint8_t *outputbuffer)
{
    u16_t numofcopiedbytes = 0;
    struct pbuf *p = (struct pbuf *)genframe->frame;
    struct pbuf *q;
    char str[150];
    uint32_t l = 0;
    
    for(q = p; q != NULL; q = q->next) 
    {
      memcpy((u8_t*)&outputbuffer[l], q->payload, q->len);
      l = l + q->len;
    }
        
    return(p->tot_len);
    
}

static void s_ipal_eth_signal_higherlayer(void)
{
    if(NULL != ipal_base_hid_cfg.extfn.usr_on_ethframe_received)
    {
        ipal_base_hid_cfg.extfn.usr_on_ethframe_received();
    }
}


static const ipal_network_hw_init_arg0_t s_ipal_eth_ntw_hw_init_arg0 =
{
    s_ipal_eth_moveframe2higherlayer,
    s_ipal_eth_moveframe2lowerlayer,
    s_ipal_eth_signal_higherlayer
};




//ext func
extern void low_level_init(struct netif *netif)
{
    ipal_network_hw_init_cfg_t cfg = 
    {
        .priority       = 1, 
        .onframerx      = NULL,
        .macaddress     = 0
    };

    ipal_result_t res;
    ipal_eth_hid_netif *netif_ptr = ipal_eth_hid_getnetif();
    
    cfg.priority    = ipal_base_hid_cfg.eth_isrpriority;
    cfg.onframerx   = (ipal_network_hw_init_arg0_t*)&s_ipal_eth_ntw_hw_init_arg0;
    cfg.macaddress  = IPAL_mac48addr(netif_ptr->netif.hwaddr[0], netif_ptr->netif.hwaddr[1], netif_ptr->netif.hwaddr[2], netif_ptr->netif.hwaddr[3], netif_ptr->netif.hwaddr[4], netif_ptr->netif.hwaddr[5]);

    
    res = ipal_base_hid_cfg.extfn.hal_eth_init((void*)&cfg);
    if(ipal_res_OK != res)
    {
        ipal_base_hid_on_fatalerror(ipal_error_generic, "low_level_init: extfn.hal_eth_init failed ");
    }

}

/* questa funzione dovrebbe essere chimata appena arriva un pkt nel dma e dovrebbe
copiare i dati da dma a pbuf. dato che la hal_eth driver fa la copia da dma a una zona di 
memoria passata da utente ==> questa funzione qui si limita a tirare fuori il puntatore al 
primo pbuf salvato nella coda di input */
extern struct pbuf *low_level_input(struct netif *netif)
{
    ipal_fifo_t         *rxpkt_fifo_ptr = NULL;
    ipal_fifo_item_t    *rxpkt_fifoitem_ptr = NULL;
    struct pbuf         *pbuf_ptr;
//    uint8_t             *u8payload_ptr;
    
    ipal_eth_hid_netif *netif_ptr = ipal_eth_hid_getnetif();
    
    rxpkt_fifo_ptr = (ipal_fifo_t*)netif_ptr->netif.state;
    if(NULL == rxpkt_fifo_ptr)
    {
        ipal_base_hid_on_fatalerror(ipal_error_generic, "low_level_input: norxpkt fifo!!! ");
        return(NULL);
    }
    
    ipal_base_hid_cfg.extfn.hal_eth_disable();
    rxpkt_fifoitem_ptr = ipal_fifo_hid_front(rxpkt_fifo_ptr);
    ipal_fifo_hid_pop(rxpkt_fifo_ptr);
    ipal_base_hid_cfg.extfn.hal_eth_enable();
    
    if(NULL == rxpkt_fifoitem_ptr)
    {
        return(NULL); //non ci sono pacchetti
    }
    
    pbuf_ptr = (struct pbuf*)(*rxpkt_fifoitem_ptr);
    
    return(pbuf_ptr);
}



extern err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    ipal_result_t res;

    ipal_genericframe_t genframe;
    genframe.frame = (void*)p;
    genframe.length = p->tot_len;
    
    res = ipal_base_hid_cfg.extfn.hal_eth_sendframe(&genframe);
    
    //pbuf_free(p); spostato in ipal_udpsocket_sendto
    
    return((ipal_res_OK == res)? ERR_OK : ERR_IF);
}


extern err_t ethernetif_init(struct netif *netif)
{

#if LWIP_NETIF_HOSTNAME
  /* Initialize interface hostname */
  netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

  /*
   * Initialize the snmp variables and counters inside the struct netif.
   * The last argument should be replaced with your link speed, in units
   * of bits per second.
   */
  //VALE commentata linea sottostante perche' non usp snmp
//  NETIF_INIT_SNMP(netif, snmp_ifType_ethernet_csmacd, LINK_SPEED_OF_YOUR_NETIF_IN_BPS);

/* Define those to better describe your network interface. */
#define IFNAME0 'e'
#define IFNAME1 'n'
  netif->name[0] = IFNAME0;
  netif->name[1] = IFNAME1;
  /* We directly use etharp_output() here to save a function call.
   * You can instead declare your own function an call etharp_output()
   * from it if you have to do some checks before sending (e.g. if link
   * is available...) */
  netif->output = etharp_output;
  netif->linkoutput = low_level_output;
  
  //VALE commentata linea sottostante perche non serve (in prog stm32 non c'e')
  //ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);
  
  /* initialize the hardware */
  low_level_init(netif);

  return ERR_OK;
}
// - ipal: functions required ----------------------------------------------------------------------------------------

extern ipal_result_t ipal_ipiit_storage_eth_init(const ipal_cfg_t *cfg)
{

    return(ipal_res_OK);
}

#endif//IPAL_USE_ETH

//#if SYS_LIGHTWEIGHT_PROT
extern int ethisr_isrunning; //this variable is defined in hl_etc.c
                             /*I use it in order to know is i'm in isr rx func or not */
sys_prot_t sys_arch_protect(void)
{
    if(!ethisr_isrunning)
    {
        ipal_base_hid_cfg.extfn.hal_eth_disable();
        return(1);
    }
    else
    {
        return(0);
    }
}


void sys_arch_unprotect(sys_prot_t pval)
{
    if(1==pval)
        ipal_base_hid_cfg.extfn.hal_eth_enable(); 
}
//#endif








#if defined(IPAL_USE_FTP)

#include "ipal_ftp_hid.h"



// - lwip: variables required -------------------------------------------------------------------------------------- 



// - lwip: functions required --------------------------------------------------------------------------------------



// - ipal: functions required ----------------------------------------------------------------------------------------
extern ipal_result_t ipal_ipiit_storage_ftp_init(const ipal_cfg_t *cfg)
{
    return(ipal_res_OK);
}

#endif//IPAL_USE_FTP




#if defined(IPAL_USE_IGMP)

// - lwip: variables required -------------------------------------------------------------------------------------- 


// - lwip: functions required --------------------------------------------------------------------------------------


// - ipal: functions required ----------------------------------------------------------------------------------------
extern ipal_result_t ipal_ipiit_storage_igmp_init(const ipal_cfg_t *cfg)
{
    return(ipal_res_OK);
}


#endif//IPAL_USE_IGMP


#if defined(IPAL_USE_SYS)

// - lwip: variables required -------------------------------------------------------------------------------------- 



// - lwip: functions required --------------------------------------------------------------------------------------




// - ipal: functions required ----------------------------------------------------------------------------------------
extern ipal_result_t ipal_ipiit_storage_sys_init(const ipal_cfg_t *cfg)
{
    return(ipal_res_OK);
}


#endif//IPAL_USE_SYS






#if defined(IPAL_USE_TCP)

// - lwip: variables required -------------------------------------------------------------------------------------- 

// - lwip: functions required --------------------------------------------------------------------------------------


// - ipal: functions required ----------------------------------------------------------------------------------------

extern ipal_result_t ipal_ipiit_storage_tcp_init(const ipal_cfg_t *cfg)
{
    return(ipal_res_OK);
}


#endif//IPAL_USE_TCP




#if defined(IPAL_USE_TELNET)

// - lwip: variables required --------------------------------------------------------------------------------------



// - lwip: functions required --------------------------------------------------------------------------------------


// - ipal: functions required ----------------------------------------------------------------------------------------

extern ipal_result_t ipal_ipiit_storage_telnet_init(const ipal_cfg_t *cfg)
{
    return(ipal_res_OK);
}


#endif//IPAL_USE_TELNET






#if defined(IPAL_USE_TFTP)

// - lwip: variables required -------------------------------------------------------------------------------------- 



// - lwip: functions required --------------------------------------------------------------------------------------



// - ipal: functions required ----------------------------------------------------------------------------------------

extern ipal_result_t ipal_ipiit_storage_tftp_init(const ipal_cfg_t *cfg)
{

    return(ipal_res_OK);
}

#endif//IPAL_USE_TFTP




#if defined(IPAL_USE_UDP)

// - lwip: variables required -------------------------------------------------------------------------------------- 




// - lwip: functions required --------------------------------------------------------------------------------------


// - ipal: functions required ----------------------------------------------------------------------------------------

extern ipal_result_t ipal_ipiit_storage_udp_init(const ipal_cfg_t *cfg)
{
    return(ipal_res_OK);
}


#endif//IPAL_USE_UDP



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------


