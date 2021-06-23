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

/* @file       ipal_cfg.c
	@brief      This file keeps internal implementation of the ipal.
	@author     marco.accame@iit.it
    @date       06/07/2010
**/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#undef DONTUSE_EOtheFatalError

#if !defined(DONTUSE_EOtheFatalError)
    #include "EOtheFatalError.h"
#else
    #include "EOtheErrorManager.h"
    #include "EoError.h"
    #include "hal_trace.h"
    #include "hal_led.h"
#endif


#include "ipal.h"
#include "hal.h"
#include "osal.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "ipal_cfg.h"

static void s_ipal_cfg_on_fatal_error(ipal_fatalerror_t errorcode, const char * errormsg);

static void onethframerx(void);


uint64_t macnn = IPAL_mac48addr(0x1e, 0x30, 0x6c, 0xa2, 0x45, 0x5e);  



const ipal_cfg_t ipal_cfg = 
{ 
    // arch
    EO_INIT(.arch_ipstack)          (ipal_ipstack_t)IPAL_TCPIPTYPE,
    EO_INIT(.memorymodel)           (ipal_memorymodel_t)IPAL_MEMMODEL,
    
    // sys
    EO_INIT(.sys_timetick)          IPAL_TIMETICK,          // uint32_t        sys_timetick;       // in usec, but multiple of 10 ms. upto 200 ms.  
    EO_INIT(.sys_mempoolsize)       IPAL_MEMPOOLSIZE,       // uint16_t        sys_mempoolsize;

    // eth
    EO_INIT(.eth_isrpriority)       IPAL_ETH_ISRPRIO,    
    EO_INIT(.eth_mac)               IPAL_mac48addr(0xFF&(IPAL_MACOUI>>16), 0xFF&(IPAL_MACOUI>>8), 0xFF&(IPAL_MACOUI), 0xFF&(IPAL_MAC3OCT>>16), 0xFF&(IPAL_MAC3OCT>>8), 0xFF&(IPAL_MAC3OCT)),
    EO_INIT(.eth_ip)                IPAL_ipv4addr(IPAL_IP0, IPAL_IP1, IPAL_IP2, IPAL_IP3),    //uint8_t         eth_ip[4];
    EO_INIT(.eth_mask)              IPAL_ipv4addr(IPAL_MSK0, IPAL_MSK1, IPAL_MSK2, IPAL_MSK3),        //uint8_t         eth_mask[4];
    
    // arp
    EO_INIT(.arp_cachesize)         IPAL_ARP_CACHESIZE,     // uint8_t         arp_cachesize;
    EO_INIT(.arp_retrymaxnum)       IPAL_ARP_RETRYMAXNUM,   //uint8_t         arp_retrymaxnum;
    EO_INIT(.arp_autonotify)        IPAL_ARP_AUTONOTIFY,    //uint8_t         arp_autonotify;    
    EO_INIT(.arp_cachetimeout)      IPAL_ARP_CACHETIMEOUT,  // uint32_t        arp_cachetimeout;   // in usec, but multiple of seconds
    EO_INIT(.arp_retrytimeout)      IPAL_ARP_RETRYTIMEOUT,  //uint32_t        arp_retrytimeout;   // in usec, but multiple of seconds

    
    // igmp
    EO_INIT(.igmp_enable)           IPAL_IGMP_ENABLE,       //uint8_t         igmp_enable;
    EO_INIT(.igmp_groupsnum)        IPAL_IGMP_GROUPSNUM,    // uint8_t         igmp_groupsnum;
    

    // dhcp
    EO_INIT(.dhcp_enable)           IPAL_DHCP_ENABLE,   // uint8_t         dhcp_enable;
    
    // udp
    EO_INIT(.udp_enable)            IPAL_UDP_ENABLE,    //uint8_t         udp_enable;
    EO_INIT(.udp_socketnum)         IPAL_UDP_SOCKETNUM, //uint8_t         udp_socketnum;
    
    // tcp sockets
    EO_INIT(.tcp_enable)            IPAL_TCP_ENABLE,            // uint8_t         tcp_enable;                     
    EO_INIT(.tcp_socketnum)         IPAL_TCP_SOCKETNUM,         //uint8_t         tcp_socketnum;                   
    EO_INIT(.tcp_retrymaxnum)       IPAL_TCP_RETRYMAXNUM,       //uint8_t         tcp_retrymaxnum;                   
    EO_INIT(.tcp_retrytimeout)      IPAL_TCP_RETRYTOUT,         // uint32_t        tcp_retrytimeout;           // in usec, but multiple of seconds                  
    EO_INIT(.tcp_connectiontimeout) IPAL_TCP_CONNECTIONTIMEOUT, //uint32_t        tcp_connectiontimeout;      // in usec, but multiple of seconds, upto 64 secs

  
    // tftp
    EO_INIT(.tftp_enable)           IPAL_TFTP_ENABLE,                    
    EO_INIT(.tftp_retrymaxnum)      IPAL_TFTP_RETRYMAXNUM,    
    EO_INIT(.tftp_port)             IPAL_TFTP_PORT,        
    EO_INIT(.tftp_timeout)          IPAL_TFTP_TIMEOUT,       // in usec but multiple of seconds, upto 60 secs     

    // ftp
    EO_INIT(.ftp_enable)            IPAL_FTP_ENABLE,            // uint8_t         ftp_enable;                     
    EO_INIT(.ftp_port)              IPAL_FTP_PORT,              // uint16_t        ftp_port;                       
    EO_INIT(.ftp_authenable)        IPAL_FTP_AUTHENABLE,        // uint8_t         ftp_authenable;                 
    EO_INIT(.ftp_user)              IPAL_FTP_USER,              // uint8_t         ftp_user[8];                
    EO_INIT(.ftp_pass)              IPAL_FTP_PASS,              // uint8_t         ftp_pass[8]; 

    // telnet
    EO_INIT(.tel_enable)            IPAL_TEL_ENABLE,            //uint8_t         tel_enable;                    
    EO_INIT(.tel_port)              IPAL_TEL_PORT,              //uint16_t        tel_port;                      
    EO_INIT(.tel_authenable)        IPAL_TEL_AUTHENABLE,        // uint8_t         tel_authenable;                
    EO_INIT(.tel_user)              IPAL_TEL_USER,              // uint8_t         tel_user[8];               
    EO_INIT(.tel_pass)              IPAL_TEL_PASS,              // uint8_t         tel_pass[8]; 

    EO_INIT(.extfn)
    { 
        EO_INIT(.usr_on_fatal_error)        s_ipal_cfg_on_fatal_error,

        EO_INIT(.osal_mutex_new)            NULL, 
        EO_INIT(.osal_mutex_take)           NULL, 
        EO_INIT(.osal_mutex_release)        NULL, 
        EO_INIT(.osal_param_tout_forever)   0, 
        EO_INIT(.osal_system_scheduling_suspend) NULL, //osal_system_scheduling_suspend,
        EO_INIT(.osal_system_scheduling_restart) NULL, //osal_system_scheduling_restart,


        EO_INIT(.hal_eth_init)              (ipal_result_t (*)(void*)) hal_eth_init,
        EO_INIT(.hal_eth_enable)            (ipal_result_t (*)(void))  hal_eth_enable,
        EO_INIT(.hal_eth_disable)           (ipal_result_t (*)(void))  hal_eth_disable,
        EO_INIT(.hal_eth_sendframe)         (ipal_result_t (*)(void*)) hal_eth_sendframe,

        EO_INIT(.usr_on_ethframe_received)  onethframerx,

        EO_INIT(.fopen)                     NULL,
        EO_INIT(.fclose)                    NULL,
        EO_INIT(.fread)                     NULL,
        EO_INIT(.fwrite)                    NULL,
        EO_INIT(.ftell)                     NULL,
        EO_INIT(.fseek)                     NULL,

        EO_INIT(.fsal_delete)               NULL, 
        EO_INIT(.fsal_rename)               NULL,
        EO_INIT(.fsal_find)                 NULL,

        EO_INIT(.usr_tnet_exec)             NULL,
        EO_INIT(.usr_tnet_login_msg)        NULL,              
        EO_INIT(.usr_tnet_welcome_msg)      NULL,
        EO_INIT(.usr_tnet_prompt)           NULL
    } 
};

 
const ipal_cfg_t *ipal_cfgMINE = &ipal_cfg;



static void onethframerx(void)
{
    static volatile uint8_t b = 0;
    b++;
    b = b;
}


static void s_ipal_cfg_on_fatal_error(ipal_fatalerror_t errorcode, const char * errormsg)
{
#if !defined(DONTUSE_EOtheFatalError)        
    fatal_error_descriptor_t *des = eo_fatalerror_GetDescriptor(eo_fatalerror_GetHandle());
    des->handlertype = fatalerror_handler_ipal;
    des->handlererrorcode = errorcode;
    des->param = NULL;
    eo_fatalerror_Restart(eo_fatalerror_GetHandle(), des);
#else
    #warning DONTUSE_EOtheFatalError is defined, are you sure?
    char str[256];
    
    if(eobool_true == eo_errman_IsErrorHandlerConfigured(eo_errman_GetHandle()))
    {
        // ok ... use the error manager, either in its simple form or in its networked form
        eOerrmanDescriptor_t errdes = {0};
        errdes.code             = eoerror_code_get(eoerror_category_System, eoerror_value_SYS_halerror);
        errdes.par16            = errorcode;
        errdes.par64            = 0;
        errdes.sourcedevice     = eo_errman_sourcedevice_localboard;
        errdes.sourceaddress    = 0;
        
        snprintf(str, sizeof(str), "ipal_fatalerror_t %d: %s", errorcode, errormsg);

        eo_errman_Error(eo_errman_GetHandle(), eo_errortype_fatal, str, "IPAL", &errdes);
    }
    else
    {    
        char str[256];
        snprintf(str, sizeof(str), "ipal_fatalerror_t %d: %s", errorcode, errormsg);
        
        // in case of fatal error we blink all leds but led2
        hal_led_init(hal_led0, NULL);
        hal_led_init(hal_led1, NULL);
        hal_led_init(hal_led2, NULL);
        hal_led_init(hal_led3, NULL);
        hal_led_init(hal_led4, NULL);
        hal_led_init(hal_led5, NULL);
    
        hal_led_off(hal_led0);
        hal_led_off(hal_led1);
        hal_led_off(hal_led2);
        hal_led_off(hal_led3);
        hal_led_off(hal_led4);
        hal_led_off(hal_led5);   

        for(;;)
        {
            hal_sys_delay(100);
            
            hal_led_toggle(hal_led0);
            hal_led_toggle(hal_led1);
            //hal_led_toggle(hal_led2);
            hal_led_toggle(hal_led3);
            hal_led_toggle(hal_led4);
            hal_led_toggle(hal_led5);  
        }
    }
#endif    
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



