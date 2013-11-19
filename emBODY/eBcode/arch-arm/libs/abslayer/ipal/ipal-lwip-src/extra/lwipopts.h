/**
  ******************************************************************************
  * @file    lwipopts.h
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    31-October-2011
  * @brief   lwIP Options Configuration.
  *          This file is based on Utilities\lwip_v1.3.2\src\include\lwip\opt.h 
  *          and contains the lwIP configuration for the STM32F4x7 demonstration.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

#ifndef __LWIPOPTS_H__
#define __LWIPOPTS_H__

#include "ipal_modules.h"
#include "ipal_numberof.h"
/*******************************************************************************/
/*                  stack configuration options                                */
/*******************************************************************************/


#define MEM_LIBC_MALLOC 1 //non compilia dei file!!
/**
 * SYS_LIGHTWEIGHT_PROT==1: if you want inter-task protection for certain
 * critical regions during buffer allocation, deallocation and memory
 * allocation and deallocation.
 */
#define SYS_LIGHTWEIGHT_PROT    0

/**
 * NO_SYS==1: Provides VERY minimal functionality. Otherwise,
 * use lwIP facilities.
 */
#define NO_SYS                  1


/**
 * LWIP_NETCONN==1: Enable Netconn API (require to use api_lib.c)
 */
#define LWIP_NETCONN                    0

/**
 * LWIP_SOCKET==1: Enable Socket API (require to use sockets.c)
 */
#define LWIP_SOCKET                     0


/**
 * Statistics options
 */
#define LWIP_STATS 0
#define LWIP_STATS_DISPLAY 0 
#define LWIP_PROVIDE_ERRNO 0



#define LWIP_RAW 0

#define MEMP_NUM_RAW_PCB 0
/**
 * Lwip Debug options
 */
#define LWIP_DEBUG                      0

/**
 * Checksum options
 */
/* 
The STM32F4x7 allows computing and verifying the IP, UDP, TCP and ICMP checksums by hardware:
 - To use this feature let the following define uncommented.
 - To disable it and process by CPU comment the  the checksum.
*/
#undef CHECKSUM_BY_HARDWARE 


#ifdef CHECKSUM_BY_HARDWARE
  /* CHECKSUM_GEN_IP==0: Generate checksums by hardware for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 0
  /* CHECKSUM_GEN_UDP==0: Generate checksums by hardware for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                0
  /* CHECKSUM_GEN_TCP==0: Generate checksums by hardware for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                0 
  /* CHECKSUM_CHECK_IP==0: Check checksums by hardware for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               0
  /* CHECKSUM_CHECK_UDP==0: Check checksums by hardware for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              0
  /* CHECKSUM_CHECK_TCP==0: Check checksums by hardware for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              0
#else
  /* CHECKSUM_GEN_IP==1: Generate checksums in software for outgoing IP packets.*/
  #define CHECKSUM_GEN_IP                 1
  /* CHECKSUM_GEN_UDP==1: Generate checksums in software for outgoing UDP packets.*/
  #define CHECKSUM_GEN_UDP                1
  /* CHECKSUM_GEN_TCP==1: Generate checksums in software for outgoing TCP packets.*/
  #define CHECKSUM_GEN_TCP                1
  /* CHECKSUM_CHECK_IP==1: Check checksums in software for incoming IP packets.*/
  #define CHECKSUM_CHECK_IP               1
  /* CHECKSUM_CHECK_UDP==1: Check checksums in software for incoming UDP packets.*/
  #define CHECKSUM_CHECK_UDP              1
  /* CHECKSUM_CHECK_TCP==1: Check checksums in software for incoming TCP packets.*/
  #define CHECKSUM_CHECK_TCP              1
#endif


/*******************************************************************************/
/*                             memory  options                                 */
/*******************************************************************************/

/* MEM_ALIGNMENT: should be set to the alignment of the CPU for which
   lwIP is compiled. 4 byte alignment -> define MEM_ALIGNMENT to 4, 2
   byte alignment -> define MEM_ALIGNMENT to 2. */
#define MEM_ALIGNMENT           8

/* MEM_SIZE: the size of the heap memory. If the application will send
   a lot of data that needs to be copied, this should be set high. */
/* vale: the heap memory is used for dns, dhcp, autoip services. 
   Since currently these services are not avilables, no allocate memory form them*/
#define MEM_SIZE                 0//512 //(1*1024)


/* MEMP_NUM_PBUF: the number of memp struct pbufs. If the application
   sends a lot of data out of ROM (or other static memory), this
   should be set high. */
/* vale: since pbuf_ref and pool are not used ==> MEMP_NUM_PBUF = 0*/
#define MEMP_NUM_PBUF           0


/* ---------- Pbuf options ---------- */
/*vale: pbuf is used to memorize input and output packet. 
        it is quite similar to IPAL_NUMBEROF_SYSMEMPOOLMAXSIZE ig tcpnet,
        but in tcpnet stack sys-stack is used to allocate any data, while pbufs 
        here are used to save input and output packet */

#ifdef IPAL_MEMORY_SMALL
    /* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
    #define PBUF_POOL_SIZE          8

    /* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
    #define PBUF_POOL_BUFSIZE       256

#elif IPAL_MEMORY_MEDIUM
    /* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
    #define PBUF_POOL_SIZE          12

    /* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
    #define PBUF_POOL_BUFSIZE       256

#elif IPAL_MEMORY_LARGE
    /* PBUF_POOL_SIZE: the number of buffers in the pbuf pool. */
    #define PBUF_POOL_SIZE          30

    /* PBUF_POOL_BUFSIZE: the size of each pbuf in the pbuf pool. */
    #define PBUF_POOL_BUFSIZE       256
#endif
/*******************************************************************************/
/*                         protocols   options                                 */
/*******************************************************************************/



#ifndef IPAL_USE_TCP

    /* disable TCP module in LWIP*/
    #define LWIP_TCP                0
    /* vale: Since tcp is not supported, then avoid to allocate mem by setting following options to 0*/ 

#else
    /* enable TCP module in LWIP*/
    #define LWIP_TCP                1
    
    /* ------ define tcp resources -------*/
    #error set TCP resources!!!!
    /* MEMP_NUM_TCP_PCB: the number of simulatenously active TCP connections. */
    #define MEMP_NUM_TCP_PCB        0
    /* MEMP_NUM_TCP_PCB_LISTEN: the number of listening TCP connections. */
    #define MEMP_NUM_TCP_PCB_LISTEN 0
    /* MEMP_NUM_TCP_SEG: the number of simultaneously queued TCP segments. */
    #define MEMP_NUM_TCP_SEG        0
    /* MEMP_NUM_SYS_TIMEOUT: the number of simulateously activetimeouts. */
    #define MEMP_NUM_SYS_TIMEOUT    0


    /* ---------- TCP options ---------- */
    #define TCP_TTL                 255

    /* Controls if TCP should queue segments that arrive out of
       order. Define to 0 if your device is low on memory. */
    #define TCP_QUEUE_OOSEQ         0

    /* TCP Maximum segment size. */
    #define TCP_MSS                 (1500 - 40)	  /* TCP_MSS = (Ethernet MTU - IP header size - TCP header size) */

    /* TCP sender buffer space (bytes). */
    #define TCP_SND_BUF             (2*TCP_MSS)

    /*  TCP_SND_QUEUELEN: TCP sender buffer space (pbufs). This must be at least
      as much as (2 * TCP_SND_BUF/TCP_MSS) for things to work. */

    #define TCP_SND_QUEUELEN        (2* TCP_SND_BUF/TCP_MSS)

    /* TCP receive window. */
    #define TCP_WND                 (2*TCP_MSS)

#endif





/* ---------- ICMP options ---------- */
#define LWIP_ICMP                       1


/* ---------- DHCP options ---------- */
/* Define LWIP_DHCP to 1 if you want DHCP configuration of
   interfaces. DHCP is not implemented in lwIP 0.5.1, however, so
   turning this on does currently not work. */
#ifdef IPAL_USE_DHCP
    #define LWIP_DHCP               1
    #define LWIP_AUTOIP             0
    #error chack autoip
#else
    #define LWIP_DHCP               0
    #define LWIP_AUTOIP             0
#endif

#if ((LWIP_DHCP==1) && (MEM_SIZE==0))
#error DHCP required memeory 
#endif

/* ---------- UDP options ---------- */
#ifdef IPAL_USE_UDP
    /* enable UDP module in LWIP*/
    #define LWIP_UDP                1
    
     /* ------ define udp resources -------*/
    /* MEMP_NUM_UDP_PCB: the number of UDP protocol control blocks. One per active UDP "connection". */
    #define MEMP_NUM_UDP_PCB        IPAL_NUMBEROF_UDPMAXSOCKETS
    
    
    /* ---------- UDP options ---------- */
    #define UDP_TTL                 255

#else
    /* disable UDP module in LWIP*/
    #define LWIP_UDP                0
#endif



 
/**
 * ARP_TABLE_SIZE: Number of active MAC-IP address pairs cached.
 */
// arp protocol is always anabled. if you want disabled it see op.h file
#define ARP_TABLE_SIZE          IPAL_NUMBEROF_ARPCACHETABLEMAXSIZE

/**
 * ARP_QUEUEING==1: Outgoing packets are queued during hardware address
 * resolution.
 */
#define ARP_QUEUEING                    1


/**
 * IP_REASSEMBLY==1: Reassemble incoming fragmented IP packets. Note that
 * this option does not affect outgoing packet sizes, which can be controlled
 * via IP_FRAG.
 */
#define IP_REASSEMBLY                   0

/**
 * IP_FRAG==1: Fragment outgoing IP packets if their size exceeds MTU. Note
 * that this option does not affect incoming packet sizes, which can be
 * controlled via IP_REASSEMBLY.
 */
#define IP_FRAG                         0



#endif /* __LWIPOPTS_H__ */

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
