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

// - include guard ----------------------------------------------------------------------------------------------------
#ifndef _EOTHEETHLOWLEVELPARSER_HID_H_
#define _EOTHEETHLOWLEVELPARSER_HID_H_

#ifdef __cplusplus
extern "C" {
#endif

/* @file       eOtheEthLowLevelParser.h
    @brief      
    @author     valentina.gaggero@iit.it
    @date       03/18/2013
**/


// - external dependencies --------------------------------------------------------------------------------------------

#include "EoCommon.h"

// - declaration of extern public interface ---------------------------------------------------------------------------
 
#include "eOtheEthLowLevelParser.h"


// - #define used with hidden struct ----------------------------------------------------------------------------------
// empty-section


// - definition of the hidden struct implementing the object ----------------------------------------------------------

//* default snap length (maximum bytes per packet to capture) */
#define SNAP_LEN 1518

/* ethernet headers are always exactly 14 bytes [1] */
#define ETHERNET_HEADER_SIZE 14 //SIZE_ETHERNET

/* Ethernet addresses are 6 bytes */
#define ETHER_ADDR_LEN	6

  

// - declaration of public user-defined types ------------------------------------------------------------------------- 

//preso da <netinet/in.h> 
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

struct eOTheEthLowLevParser_hid
{
    eOethLowLevParser_cfg_t     cfg;
    uint8_t                     initted;
};
// - declaration of extern hidden functions ---------------------------------------------------------------------------

//extern eOresult_t eo_array_hid_PushBackData(EOarray *p, const void *data, uint16_t sizedata);


#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 
 
#endif  // include-guard

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------




