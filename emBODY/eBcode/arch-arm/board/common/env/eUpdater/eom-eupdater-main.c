/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Marco Accame, Alessandro Scalzo
 * email:   marco.accame@iit.it, alessandro.scalzo@iit.it
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

/* @file       eom-eupdater-main.c
	@brief      This file implements the e-updater using the embobj
	@author     marco.accame@iit.it, alessandro.scalzo@iit.it
    @date       01/11/2012
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdint.h"
#include "stdlib.h"
#include "stdio.h"

// abslayer 
#include "hal.h"
#include "hal_trace.h"
#include "osal.h"
#include "ipal.h"


// embobj  
#include "EoCommon.h"
#include "EOMtheSystem.h"
#include "EOVtheSystem.h"
#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"
#include "EOMtheIPnet.h"

#include "EOaction.h"
#include "EOpacket.h"
#include "EOMmutex.h"
#include "EOsocketDatagram.h"

#if !defined(_MAINTAINER_APPL_)
#include "eupdater-info.h"
#include "eupdater_cangtw.h"
#else
#include "emaintainer-info.h"
#endif

#include "EOtheARMenvironment.h"
#include "EOVtheEnvironment.h"

#include "updater-core.h"


#include "eupdater_parser.h"

#include "EOtimer.h"


#include "eEsharedServices.h" 

#include "emBODYrobot.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


extern void task_ethcommand(void *p);

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


#if     defined(_DEBUG_MODE_FULL_)
    #warning --> we are in _DEBUG_MODE_FULL_ and use macro _USE_IPADDR_FROM_IPAL_CFG_ 
    #define _USE_IPADDR_FROM_IPAL_CFG_
#endif

#if     defined(_USE_IPADDR_FROM_IPAL_CFG_)
    #warning --> the ip address is taken from ipal cfg, not from eeprom. to get it from eeprom undef _USE_IPADDR_FROM_IPAL_CFG_
#else
    #warning --> the ip address is taken from eeprom. to get it from ipal cfg please define _USE_IPADDR_FROM_IPAL_CFG_
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

#undef PARSE_ETH_ISR

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static void s_udpnode_errman_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des);

static void s_eom_eupdater_main_init(void);

static void s_ethcommand_startup(EOMtask *p, uint32_t t);
static void s_ethcommand_run(EOMtask *p, uint32_t t);


extern eObool_t eom_eupdater_main_connectsocket2host(eOipv4addr_t remaddr, EOsocketDatagram *skt, uint32_t usec);

#ifdef PARSE_ETH_ISR
static void s_verify_eth_isr(uint8_t* pkt_ptr, uint32_t size);
#endif    

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

// task eth command

static EOsocketDatagram*        s_skt_ethcmd                = NULL;
static EOpacket*                s_rxpkt_ethcmd              = NULL;
static EOpacket*                s_txpkt_ethcmd              = NULL;
static EOMtask*                 s_task_ethcommand           = NULL;
static EOaction*                s_action_ethcmd             = NULL;

static const eOipv4port_t       s_ethcmd_port               = 3333; 
static const eOmessage_t        s_message_from_skt_ethcmd   = 0x00000001;




static const eOerrman_cfg_t  errmancfg = 
{
    .extfn.usr_on_error = s_udpnode_errman_OnError
};

#if !defined(_MAINTAINER_APPL_)
static const eOmsystem_cfg_t* psyscfg = &eupdater_syscfg;
#else
static const eOmsystem_cfg_t* psyscfg = &emaintainer_syscfg;
#endif

static const eOipv4addr_t hostipaddr = EO_COMMON_IPV4ADDR(10, 0, 1, 104);


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


int main(void)
{
    eom_sys_Initialise( psyscfg,
                        &eom_thesystem_mempool_cfg_osal,    // mempool
                        &errmancfg,                         // errman
                        &eom_timerman_DefaultCfg,
                        &eom_callbackman_DefaultCfg
                      );  

    eom_sys_Start(eom_sys_GetHandle(), s_eom_eupdater_main_init);

}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


void osal_on_idle(void)
{
    static uint32_t cnt = 0;

    for(;;)
    {
        cnt++;
    }
}




extern void task_ethcommand(void *p)
{
    // do here whatever you like before startup() is executed and then forever()
    eom_task_Start(p);
}  


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static void s_udpnode_errman_OnError(eOerrmanErrorType_t errtype, const char *info, eOerrmanCaller_t *caller, const eOerrmanDescriptor_t *des)
{
    const char empty[] = "EO?";
    // const char *err = eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype);
    const char *eobjstr = (NULL == caller) ? (empty) : ((NULL == caller->eobjstr) ? (empty) : (caller->eobjstr));
    const uint32_t taskid = (NULL == caller) ? (0) : (caller->taskid);
    
    char text[128];
    uint64_t tt = eov_sys_LifeTimeGet(eov_sys_GetHandle());
    uint32_t sec = tt/(1000*1000);
    uint32_t tmp = tt%(1000*1000);
    uint32_t msec = tmp / 1000;
    uint32_t usec = tmp % 1000;    
    
    if(eo_errortype_trace == errtype)
    {   // it is a trace
        
        if(NULL != info)
        {
            snprintf(text, sizeof(text), "[TRACE] (%s @s%dm%du%d)-> %s.", eobjstr, sec, msec, usec, info); 
        }
        else
        {
            snprintf(text, sizeof(text), "[TRACE] (%s @s%dm%du%d)-> ...", eobjstr, sec, msec, usec); 
        }
        hal_trace_puts(text);
        return;            
    }    
    
    snprintf(text, sizeof(text), "[eobj: %s, tsk: %d] %s: %s", eobjstr, taskid, eo_errman_ErrorStringGet(eo_errman_GetHandle(), errtype), info);
    hal_trace_puts(text);

    if(errtype <= eo_errortype_error)
    {
        return;
    }

    for(;;);
}


static uint8_t *ipaddr = NULL;

static void s_eom_eupdater_main_init(void)
{
    const ipal_cfg_t* ipalcfg = NULL;
    eOmipnet_cfg_addr_t* eomipnet_addr;
#ifndef _USE_IPADDR_FROM_IPAL_CFG_
    const eEipnetwork_t *ipnet = NULL;
#endif    

    const eOmipnet_cfg_dtgskt_t eom_ipnet_dtgskt_MyCfg = 
    { 
#if !defined(_MAINTAINER_APPL_)    
        .numberofsockets            = 2,    // one for eth command and one for can gateway
        .maxdatagramenqueuedintx    = 32    // can gatweay needs to be able to manage many small packets
#else
        .numberofsockets            = 2, 
        .maxdatagramenqueuedintx    = 2
#endif
    };

#if !defined(_MAINTAINER_APPL_)
    ipalcfg = eupdater_ipal_cfg;
#else
    ipalcfg = emaintainer_ipal_cfg;
#endif    
    
    
    ipaddr  = (uint8_t*)&(ipalcfg->eth_ip);
    
    
//#if !defined(_MAINTAINER_APPL_)     
//    eupdater_info_trace("MAIN", "starting eUpdater:");
//#else
//    eupdater_info_trace("MAIN", "starting eMaintainer");
//#endif  

    // eeprom is used for shared services but is initted also inside there
    hal_eeprom_init(hal_eeprom_i2c_01, NULL);
    // flash MUST be unlocked if one wants to erase code space (program application, updater, loader)
    hal_flash_unlock();
    
    
#if !defined(_MAINTAINER_APPL_)    
    eo_armenv_Initialise((const eEmoduleInfo_t*)&eupdater_modinfo_extended, NULL);
#else
    eo_armenv_Initialise((const eEmoduleInfo_t*)&emaintainer_modinfo_extended, NULL);
#endif    
    eov_env_SharedData_Synchronise(eo_armenv_GetHandle());
    
    
#ifndef _USE_IPADDR_FROM_IPAL_CFG_
    if(eores_OK == eov_env_IPnetwork_Get(eo_armenv_GetHandle(), &ipnet))
    {
        eomipnet_addr = (eOmipnet_cfg_addr_t*)ipnet;   //they have the same memory layout

        ipaddr = (uint8_t*)&(eomipnet_addr->ipaddr);
    }
    else
#endif        
    {
        eomipnet_addr = NULL;
        ipaddr = (uint8_t*)&(ipalcfg->eth_ip);
    }
 
#ifdef PARSE_ETH_ISR   
    extern void (*hal_eth_lowLevelUsePacket_ptr)(uint8_t* pkt_ptr, uint32_t size);    
    hal_eth_lowLevelUsePacket_ptr = s_verify_eth_isr;
#endif

//    // start the ipnet
    eupdater_info_trace("MAIN", "starting ::ipnet with IP addr: %d.%d.%d.%d\n\r", ipaddr[0], ipaddr[1], ipaddr[2], ipaddr[3]); 

    eom_ipnet_Initialise(&eom_ipnet_DefaultCfg,
                         ipalcfg, 
                         eomipnet_addr,
                         &eom_ipnet_dtgskt_MyCfg
                         );
    
    // init the leds   
    hal_led_init(hal_led0, NULL);
    hal_led_init(hal_led1, NULL);
    hal_led_init(hal_led2, NULL);
    hal_led_init(hal_led3, NULL);
    hal_led_init(hal_led4, NULL);
    hal_led_init(hal_led5, NULL);   
    
    

    // the eth command task 
//    eupdater_info_trace("MAIN", "starting ::taskethcommand");                       

    s_task_ethcommand = eom_task_New(eom_mtask_MessageDriven, 101, 2*1024, s_ethcommand_startup, s_ethcommand_run,  16, 
                                    eok_reltimeINFINITE, NULL, 
                                    task_ethcommand, "ethcommand");
    s_task_ethcommand = s_task_ethcommand;
    
    upd_core_init();
    
#if !defined(_MAINTAINER_APPL_)
    // there is also the can gateway
//    eupdater_info_trace("MAIN", "starting ::cangateway");    
    eupdater_cangtw_init();
#endif

    // eval if and when jumping
//    eupdater_info_trace("MAIN", "calling ::evalwhenjumping");
    eupdater_parser_evalwhenjumping(); 
}


static void s_ethcommand_startup(EOMtask *p, uint32_t t)
{
    // init the rx and tx packets 
    s_rxpkt_ethcmd = eo_packet_New(1024);  
    s_txpkt_ethcmd = eo_packet_New(1024);

    // init the action used for various tasks
    s_action_ethcmd = eo_action_New();  

    // initialise the socket 
    s_skt_ethcmd = eo_socketdtg_New(  2, 1024, eom_mutex_New(), // input queue
                                      2, 1024, eom_mutex_New()  // output queue
                                   );   

//    eupdater_info_trace("MAIN", "opening a txrx socket on port %d for eth messages\n\r", s_ethcmd_port);


    // set the rx action on socket to be a message s_message_from_skt_ethcmd to this task object
    eo_action_SetMessage(s_action_ethcmd, s_message_from_skt_ethcmd, p);
    eo_socketdtg_Open(s_skt_ethcmd, s_ethcmd_port, eo_sktdir_TXRX, eobool_false, NULL, s_action_ethcmd, NULL);

#if !defined(_MAINTAINER_APPL_)
    // before we go on and start connection to server, we wait the task of cangateway is started and has initted its socket.
    // if we dont wait, then the init of the socket will be stopped until arp connection to host is achieved. 
    eupdater_cangtw_block_until_startup();
#endif

    eom_eupdater_main_connectsocket2host(hostipaddr, s_skt_ethcmd, 3000*eok_reltime1ms);    
}

static void s_ethcommand_run(EOMtask *p, uint32_t t)
{
    // read the packet.
    eOresult_t res;
    EOsocketDatagram *socket = NULL;
    eObool_t (*parser)(EOpacket*, EOpacket *) = NULL;

    // the message that we have received
    eOmessage_t msg = (eOmessage_t)t;

    switch(msg)
    {
        case s_message_from_skt_ethcmd:
        {
            socket = s_skt_ethcmd;
            parser = eupdater_parser_process_ethcmd;
        } break;

        default:
        {
            socket = NULL;
            parser = NULL;
        } break;

    }

    if((NULL == socket) || (NULL == parser))
    {
        return;
    }


    //hal_led_toggle(hal_led1);

    res = eo_socketdtg_Get(socket, s_rxpkt_ethcmd, eok_reltimeINFINITE);
    

    if(eores_OK == res)
    {
        eOipv4addr_t remaddr = 0;
        eOipv4port_t remport = 0;
        eo_packet_Addressing_Get(s_rxpkt_ethcmd, &remaddr, &remport);   

//        if(remaddr != hostipaddr)
//        {   // dont want anybody else but the host (pc104)
//            return;
//        }
        
        if(eobool_true == parser(s_rxpkt_ethcmd, s_txpkt_ethcmd))
        {   // if in here we have just stopped the countdown inside the parser
            
            // transmit a pkt back to the host. the call blocks until success or timeout expiry
            if(eobool_true == eom_eupdater_main_connectsocket2host(remaddr, socket, 1000*eok_reltime1ms))
            {
                eo_socketdtg_Put(socket, s_txpkt_ethcmd);
            }
        }
        else
        {   
            // this attempt is still-blocking, but it blocks for less time
            eom_eupdater_main_connectsocket2host(remaddr, socket, 250*eok_reltime1ms);
        } 
    }

}

static eObool_t     host_connected = eobool_false;
static eOipv4addr_t host_ipaddress = 0;

// blocking
extern eObool_t eom_eupdater_main_connectsocket2host(eOipv4addr_t remaddr, EOsocketDatagram *skt, uint32_t usec)
{
    //eOipv4addr_t remaddr = 0;
    //eOipv4port_t remport = 0;

    // print stats of rx packet
    //eo_packet_Destination_Get(rxpkt, &remaddr, &remport);
    
    if((eobool_false == host_connected) || (remaddr != host_ipaddress))
    {
        host_ipaddress = remaddr;
        host_connected = eobool_false;
        
//        uint32_t waitime = 100*ipaddr[3];
//        hal_sys_delay(waitime);
        
        // it is blocking
        if(eores_OK == eo_socketdtg_Connect(skt, remaddr, usec)) 
        {
            host_connected = eobool_true;
        }
        else
        {
            host_connected = eobool_false;
            //printf("not connecetd after %d ms. i shall try again at next reception\n\r", eok_reltime1sec/1000);
        }
    }
    
    if(eobool_true == host_connected)
    {
        hal_led_on(hal_led0);
    }

    return(host_connected);
}


#ifdef PARSE_ETH_ISR

// parse the packet

// -- init ddddddd
//* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define ETHERNET_HEADER_SIZE 14 //SIZE_ETHERNET

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

typedef uint32_t in_addr_t;

struct in_addr {
    in_addr_t s_addr;
};


/* Ethernet header */
typedef struct
{
        uint8_t  ether_dhost[ETHER_ADDR_LEN];    /* destination host address */
        uint8_t  ether_shost[ETHER_ADDR_LEN];    /* source host address */
        uint16_t ether_type;                     /* IP? ARP? RARP? etc */
}eo_lowLevParser_ethernetHeader;



/* IP header */
typedef struct
{
        uint8_t  ip_vhl;                 /* version << 4 | header length >> 2 */
        uint8_t  ip_tos;                 /* type of service */
        uint16_t ip_len;                 /* total length */
        uint16_t ip_id;                  /* identification */
        uint16_t ip_off;                 /* fragment offset field */
        #define IP_RF 0x8000            /* reserved fragment flag */
        #define IP_DF 0x4000            /* dont fragment flag */
        #define IP_MF 0x2000            /* more fragments flag */
        #define IP_OFFMASK 0x1fff       /* mask for fragmenting bits */
        uint8_t  ip_ttl;                 /* time to live */
        uint8_t  ip_p;                   /* protocol */
        uint16_t ip_sum;                 /* checksum */
        struct  in_addr ip_src,ip_dst;  /* source and dest address */
}eo_lowLevParser_IPHeader;
#define GET_IP_HEADER_LENGTH(ip)               (((ip)->ip_vhl) & 0x0f)
#define IP_V(ip)                (((ip)->ip_vhl) >> 4)

/* TCP header */
typedef uint32_t tcp_seq;

typedef struct{
        uint16_t th_sport;               /* source port */
        uint16_t th_dport;               /* destination port */
        tcp_seq th_seq;                 /* sequence number */
        tcp_seq th_ack;                 /* acknowledgement number */
        uint8_t  th_offx2;               /* data offset, rsvd */
#define TH_OFF(th)      (((th)->th_offx2 & 0xf0) >> 4)
        uint8_t  th_flags;
        #define TH_FIN  0x01
        #define TH_SYN  0x02
        #define TH_RST  0x04
        #define TH_PUSH 0x08
        #define TH_ACK  0x10
        #define TH_URG  0x20
        #define TH_ECE  0x40
        #define TH_CWR  0x80
        #define TH_FLAGS        (TH_FIN|TH_SYN|TH_RST|TH_ACK|TH_URG|TH_ECE|TH_CWR)
        uint16_t th_win;                 /* window */
        uint16_t th_sum;                 /* checksum */
        uint16_t th_urp;                 /* urgent pointer */
}eo_lowLevParser_TCPHeader;


/* UDP header */
typedef struct
{
		uint16_t srcport;
		uint16_t dstport;
		uint16_t len;
		uint16_t chksum;
}eo_lowLevParser_UDPHeader;

#define IPPROTO_UDP             17              /* user datagram protocol */

//net2host conversion order
#define ntohs(A) ((((uint16_t)(A) & 0xff00) >> 8) | \
(((uint16_t)(A) & 0x00ff) << 8))

#define htonl(A) ((((uint32_t)(A) & 0xff000000) >> 24) | \
(((uint32_t)(A) & 0x00ff0000) >> 8) | \
(((uint32_t)(A) & 0x0000ff00) << 8) | \
(((uint32_t)(A) & 0x000000ff) << 24))

static void s_verify_eth_isr(uint8_t* packet, uint32_t size)
{
    uint8_t* udp_data = NULL;
    uint32_t udp_size = 0;

    
    const eo_lowLevParser_ethernetHeader         *ethernet;  /* The ethernet header [1] */
    const eo_lowLevParser_IPHeader               *ip;              /* The IP header */
    const eo_lowLevParser_UDPHeader              *udp_h;         /* The UDP header */
//    /*const*/ uint8_t                            *payload = NULL;                    /* Packet payload */

    int32_t size_ip;
    int32_t size_udp = sizeof(eo_lowLevParser_UDPHeader);
    
    ip = (eo_lowLevParser_IPHeader*)(packet + ETHERNET_HEADER_SIZE);
    size_ip = GET_IP_HEADER_LENGTH(ip)*4;
    if (size_ip < 20) 
    {
        //Invalid IP header length
        return;
    }
    
    
    /* determine protocol */
    switch(ip->ip_p)
    {
 
        case IPPROTO_UDP:
        {
            udp_h = (eo_lowLevParser_UDPHeader*)(packet + ETHERNET_HEADER_SIZE + size_ip); 

            /* define/compute udp payload (segment) offset */
            udp_data = (uint8_t *)(packet + ETHERNET_HEADER_SIZE + size_ip + size_udp);

            /* compute udp payload (segment) size */
            udp_size = ntohs(ip->ip_len) - (size_ip + size_udp);
  
        } break;

        default: //Protocol: unknown
        {
        } break;

    } 

    if(NULL == udp_data)
    {
        return;
    }  


    if(0x12 == udp_data[0])
    {
        char str[64];
        uint32_t *nums = (uint32_t*)udp_data;
        snprintf(str, sizeof(str), "HALE %d", nums[1]);
        hal_trace_puts(str);        
    }

    
}

#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



