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

/* @file       eOtheEthLowLevelParser.c
    @brief      
    @author     valentina.gaggero@iit.it
    @date       03/18/2013
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "EoCommon.h"
#include "string.h"

#include "EOropframe_hid.h"
#include "EOrop_hid.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
#include "eOtheEthLowLevelParser.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
#include "eOtheEthLowLevelParser_hid.h"



// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define ROPFRAME_HEADER_SIZE        sizeof(OropframeHeader_t)
#define ROP_HEADER_SIZE             sizeof(eOrophead_t)

/*
 * From Protocols (RFC 1700)
 */
#define IPPROTO_IP              0               /* dummy for IP */
#define IPPROTO_UDP             17              /* user datagram protocol */
#define IPPROTO_TCP             6               /* tcp */

//net2host conversion order
#define ntohs(A) ((((uint16_t)(A) & 0xff00) >> 8) | \
(((uint16_t)(A) & 0x00ff) << 8))

#define htonl(A) ((((uint32_t)(A) & 0xff000000) >> 24) | \
(((uint32_t)(A) & 0x00ff0000) >> 8) | \
(((uint32_t)(A) & 0x0000ff00) << 8) | \
(((uint32_t)(A) & 0x000000ff) << 24))

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------
static eOresult_t s_eo_EthLowLewParser_GetUDPpayload(eOTheEthLowLevParser *p, uint8_t *packet, eOethLowLevParser_packetInfo_t *pktInfo_ptr);
static uint8_t s_eo_EthLowLewParser_applyFilters(eOTheEthLowLevParser *p, eOethLowLevParser_packetInfo_t *pktInfo_ptr);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
static eOTheEthLowLevParser s_ethLowLevParser_singleton = 
{
    EO_INIT(.initted)   0
};



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern eOTheEthLowLevParser * eo_ethLowLevParser_Initialise(const eOethLowLevParser_cfg_t *cfg)
{
    //se gia configurato fatto qc????
    
    if(NULL == cfg)
    {
        //cfg = &eo_ethLowLevParser_DefaultCfg;
        return(NULL);
    }
    
    memcpy(&s_ethLowLevParser_singleton.cfg, cfg, sizeof(eOethLowLevParser_cfg_t));
    s_ethLowLevParser_singleton.initted = 1;
    
    return(&s_ethLowLevParser_singleton);
}

extern eOTheEthLowLevParser * eo_ethLowLevParser_GetHandle(void)
{
    return( (s_ethLowLevParser_singleton.initted == 1)? &s_ethLowLevParser_singleton : NULL);
}


extern eOresult_t eOTheEthLowLevParser_GetUDPdatagramPayload(eOTheEthLowLevParser *p, uint8_t *packet, eOethLowLevParser_packetInfo_t *pktInfo_ptr)
{
    return(s_eo_EthLowLewParser_GetUDPpayload(p, packet, pktInfo_ptr));
}



extern eOresult_t eOTheEthLowLevParser_DissectPacket(eOTheEthLowLevParser *p, uint8_t *packet)
{
    eOethLowLevParser_packetInfo_t pktInfo;
    eOresult_t res = s_eo_EthLowLewParser_GetUDPpayload(p, packet, &pktInfo);
    if(eores_OK != res)
    {
        return(res);
    }

    if(NULL != p->cfg.appParserData.func)
    {
        res = p->cfg.appParserData.func(p->cfg.appParserData.arg, &pktInfo);
    }
    
    return(res);
}



// extern eOresult_t eOTheEthLowLevParser_RopFrameParse(eOTheEthLowLevParser *p, eOethLowLevParser_packetInfo_t *pktInfo_ptr)
// {

//     if((NULL == p) || (NULL == pktInfo_ptr))
//     {
//         return(eores_NOK_nullpointer);
//     }


//     if(!(s_eo_EthLowLewParser_isvalidropframe(pktInfo_ptr->payload_ptr)))
//     {
//         if(p->cfg.checks.invalidRopFrame.cbk != NULL)
//         {
//             p->cfg.checks.invalidRopFrame.cbk(pktInfo_ptr);
//         }
//         return(eores_NOK_generic); 
//     }
//     
//     if(p->cfg.checks.seqNum.cbk_onErrSeqNum != NULL)
//     {
//         if(!s_eo_EthLowLewParser_CheckSeqnum(p, pktInfo_ptr, &rec_seqnum, &expeted_seqnum))
//         {
//             p->cfg.checks.seqNum.cbk_onErrSeqNum(p, pktInfo_ptr, rec_seqnum, expeted_seqnum);
//         }
//     }
//     
//     if(p->cfg.checks.nv.cbk_onNVfound != NULL)
//     {
//         //cerco se nel pacchetto e' presente una delle rop richieste
//         s_eo_EthLowLewParser_CheckNV(p, pktInfo_ptr);
//     }
//     
//     return(eores_OK);
// }





// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static eOresult_t s_eo_EthLowLewParser_GetUDPpayload(eOTheEthLowLevParser *p, uint8_t *packet, eOethLowLevParser_packetInfo_t *pktInfo_ptr)
{
    eOresult_t res;
   
    /* declare pointers to packet headers */
    const eo_lowLevParser_ethernetHeader         *ethernet;  /* The ethernet header [1] */
    const eo_lowLevParser_IPHeader               *ip;              /* The IP header */
    const eo_lowLevParser_UDPHeader              *udp_h;         /* The UDP header */
    /*const*/ uint8_t                            *payload = NULL;                    /* Packet payload */

    int32_t size_ip;
    int32_t size_udp = sizeof(eo_lowLevParser_UDPHeader);
    int32_t size_payload;


    if((NULL == p) || (packet == NULL) || (pktInfo_ptr == NULL))
    {
        return(eores_NOK_nullpointer);
    }

    //reset return values
    pktInfo_ptr->payload_ptr = NULL;
    pktInfo_ptr->size = 0;
    pktInfo_ptr->src_addr = 0;
    pktInfo_ptr->dst_addr = 0;   
    
    /* define ethernet header */
    ethernet = (eo_lowLevParser_ethernetHeader*)(packet);

    /* define/compute ip header offset */
    ip = (eo_lowLevParser_IPHeader*)(packet + ETHERNET_HEADER_SIZE);
    size_ip = GET_IP_HEADER_LENGTH(ip)*4;
    if (size_ip < 20) 
    {
        //Invalid IP header length
        return(eores_NOK_generic);
    }


    /* determine protocol */
    switch(ip->ip_p)
    {
        case IPPROTO_TCP:
        {
            pktInfo_ptr->prototype = protoType_tcp;
            res = eores_NOK_unsupported;
        }break;
        case IPPROTO_UDP:
        {
            udp_h = (eo_lowLevParser_UDPHeader*)(packet + ETHERNET_HEADER_SIZE + size_ip); 

            /* define/compute udp payload (segment) offset */
            pktInfo_ptr->payload_ptr = (uint8_t *)(packet + ETHERNET_HEADER_SIZE + size_ip + size_udp);

            /* compute udp payload (segment) size */
            pktInfo_ptr->size = ntohs(ip->ip_len) - (size_ip + size_udp);
            
            pktInfo_ptr->prototype = protoType_udp;
            
            pktInfo_ptr->src_addr = htonl(ip->ip_src.s_addr);
            pktInfo_ptr->src_port = ntohs(udp_h->srcport);
            
            pktInfo_ptr->dst_addr = htonl(ip->ip_dst.s_addr);
            pktInfo_ptr->dst_port = ntohs(udp_h->dstport);
            
            if(p->cfg.conFiltersData.filtersEnable)
            {
                if(s_eo_EthLowLewParser_applyFilters(p, pktInfo_ptr))
                {
                    res = eores_OK;
                }
                else
                {
                    res = eores_NOK_nodata;
                }
            }
       
            res = eores_OK;
        }break;
//         case IPPROTO_ICMP:
//         {
//             res = eores_NOK_unsupported;
//         }break;

        default: //Protocol: unknown
        {
            res = eores_NOK_unsupported;
        }break;

    }

    return(res);
}



static uint8_t s_eo_EthLowLewParser_applyFilters(eOTheEthLowLevParser *p, eOethLowLevParser_packetInfo_t *pktInfo_ptr)
{
    eOethLowLevParser_cfg_connectionfilters_t *filters_ptr = &(p->cfg.conFiltersData.filters);

    if((filters_ptr->src_addr!= 0) && (filters_ptr->src_addr != pktInfo_ptr->src_addr))
    {
        return 0;
    }
    if((filters_ptr->dst_addr!= 0) && (filters_ptr->dst_addr != pktInfo_ptr->dst_addr))
    {
        return 0;
    }

    if((filters_ptr->src_port!= 0) && (filters_ptr->src_port != pktInfo_ptr->src_port))
    {
        return 0;
    }
    if((filters_ptr->dst_port!= 0) && (filters_ptr->dst_port != pktInfo_ptr->dst_port))
    {
        return 0;
    }
    
    return(1);

}


// static eOresult_t s_eo_EthLowLewParser_CheckNV(eOTheEthLowLevParser *p, eOethLowLevParser_packetInfo_t *pktInfo_ptr)
// {
//     EOropframeHeader_t *ropframeheader;
// 	eOrophead_t *ropheader;
// 	uint8_t *rop_ptr;
//     uint16_t i;


// 	//get ropframe header
// 	ropframeheader = (EOropframeHeader_t *)payload;

//     //get rop to first rop
// 	rop_ptr = &payload[ROPFRAME_HEADER_SIZE];
// 	ropheader = (eOrophead_t*)rop_ptr;

// 	for(i=0; i<ropframeheader->ropsnumberof; i++)
// 	{		
//         //if((cfg.checks.nv.reqnv.ep == ropheader->endp) && (cfg.checks.nv.reqnv.nvid == ropheader->nvid))
//         if(s_eo_EthLowLewParser_NVisrequired(p, ropheader->endp, ropheader->nvid))
// 		{
// 			uint8_t *enddata_ptr;
//             eOethLowLevParser_ropAdditionalInfo_t ropAddInfo = {0};
//            
//             //prepare rop additional info
//             ropAddInfo.desc.configuration = ropheader->ctrl;
//             ropAddInfo.desc.ropcode = ropheader->ropc;
//             ropAddInfo.desc.ep = ropheader->endp;
//             ropAddInfo.desc.id = ropheader->nvid;
//             ropAddInfo.desc.data = &rop_ptr[ROP_HEADER_SIZE];
//             ropAddInfo.desc.size = ropheader->dsiz;
//             ropAddInfo.desc.signature = EOK_uint32dummy;
//             ropAddInfo.time = EOK_uint64dummy;
//             
//             
// 			if(ropheader->ctrl.plussign == 1)
//             {
//                 int32_t fill= ropheader->dsiz%4;
//                 int32_t howmuch = 0;
//                 if(fill!=0)
//                 {
//                     fill=4-fill;
//                 }
//                 howmuch = (ropheader->dsiz) + fill;
//                 enddata_ptr = &rop_ptr[howmuch];
//                 ropAddInfo.desc.signature = *((uint32_t*)enddata_ptr);
//                 enddata_ptr = &rop_ptr[howmuch+4/*signature size*/];
//                 ropAddInfo.time = *((uint64_t*)enddata_ptr);
//             }
//             else if(ropheader->ctrl.plustime == 1)
//             {
//                 int32_t fill= ropheader->dsiz%4;
//                 int32_t howmuch = 0;
//                 if(fill!=0)
//                 {
//                     fill=4-fill;
//                 }
//                 howmuch = (ropheader->dsiz) + fill;
//                 enddata_ptr = &rop_ptr[howmuch];
//                 ropAddInfo.time = *((uint64_t*)enddata_ptr);  
//             }
//             
//             p->cfg.checks.nv.cbk_onNVfound(pktInfo_ptr, &ropAddInfo);
//  		}
// 		
//         //go to next rop
// 		rop_ptr = &rop_ptr[8+ropheader->dsize + 8];
// 		ropheader = (eOrophead_t*)rop_ptr;
// 	}

//     return(eores_OK);
// }

// static uint8_t s_eo_EthLowLewParser_NVisrequired(eOTheEthLowLevParser *p, eOnvEP_t ep, eOnvID_t nvid)
// {
//     uint8_t i, max;
//     eOethLowLevParser_nvidEp_couple_t *couple;
//     
//     max = p->cfg.checks.nv.NVsArray.head.size;
//     
//     for(i=0; i<max; i++)
//     {
//         couple  = (eOethLowLevParser_nvidEp_couple_t *)eo_array_At((EOarray*)&p->cfg.nv.NVsArray, i);
//         if((couple->nv == nvid)&&(couple->ep == ep))
//         {
//             return(1);
//         }
//     }
//     return(0);
// }


// static uint8_t s_eo_EthLowLewParser_CheckSeqnum(eOTheEthLowLevParser *p, eOethLowLevParser_packetInfo_t *pktInfo_ptr, uint32_t *rec_seqnum, uint32_t *expeted_seqnum)
// {
//     static uint8_t isfirstframe = 1;
//     static uint32_t curr_seqNum; //current sequence number
//     
//     EOropframeHeader_t *ropframeHdr = (EOropframeHeader_t *)pktInfo_ptr->payload_ptr;
//     uint32_t *u32ptr =  (uint32_t*)&ropframeHdr->sequencenumber;
//     *rec_seqnum = *u32ptr;


//     //if it is first pkt received i save start seqNum (initialized curr_seqNum with received seqnum)
//     if(isfirstframe)
//     {
//         curr_seqNum = *rec_seqnum;
//         *expeted_seqnum = curr_seqNum;
//         isfirstframe = 0;
//         return(1);
//     }
//     
//     //calculate expected seqnum
//     *expeted_seqnum = curr_seqNum+1;
//     
//     
//     if(*expeted_seqnum == *rec_seqnum)
//     {
//         curr_seqNum = *expeted_seqnum;
//         return(1);
//     }

//     //if i'm here i lost a packt or packets arrived not in order.
//     
//     if(*rec_seqnum > curr_seqNum)
//     {
//         //i lost a pkt, so i restart with received seqnum
//         curr_seqNum = *rec_seqnum;
//     }
//     
//     //if i received a pkt with seqnum<curr_seqNum then i mantein my curr_seqNum
//     
//     return(0);
// }


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




