/*
 * pcap.cpp
 *
 *  Created on: Jul 31, 2012
 *      Author: Alberto Cardellino
 *
 *  program for testing fuc...ing udp connection using the low-level pcap library
 */

#include <unistd.h>
#include <stdio.h>
#include <signal.h>

#include <pcap.h>
#include "header.h"
#include "debugFunctions.h"

// ACE stuff
#include <ace/ACE.h>
#include "ace/SOCK_Dgram.h"
#include "ace/Addr.h"

pcap_t *handle;

static void sighandler(int _signum)
{
  printf("Sighandler's got signal %d\n", _signum);
  pcap_breakloop(handle);
}

int main(int argc, char *argv[])
{
    char    dev[10];
    char    errbuf[PCAP_ERRBUF_SIZE];
    struct bpf_program fp;                  /* The compiled filter expression */
    char    filter_exp[64];                 /* The filter expression */
    bpf_u_int32 mask;                       /* The netmask of our sniffing device */
    bpf_u_int32 net;                        /* The IP of our sniffing device */

    struct pcap_pkthdr header;  /* The header that pcap gives us */
    const u_char *packet;       /* The actual packet */

    strcpy(dev, "eth1");
    if(argc > 1)
    {
        for(int i = 1; i < argc; i++)
        {
            if(strcmp("--dev", argv[i]) == 0 )
            {
                if(++i < (argc))
                    strcpy(dev, argv[i]);
                continue;
            }
            printf("command option %s not understood\n", argv[i]);
        }
    }

    //  Register handler to catch CTRL+C
    if(signal(SIGINT, sighandler) == SIG_IGN)
        signal(SIGINT, SIG_IGN);

    sprintf(filter_exp, "port %s", PORT);
    //  sprintf(filter_exp, "src host 10.0.1.104");

    printf("filter_exp = %s\n", filter_exp);
//  dev = pcap_lookupdev(errbuf);
//  if (dev == NULL) {
//      fprintf(stderr, "Couldn't find default device: %s\n", errbuf);
//      return(2);
//  }
    printf("Device: %s\n", dev);

    if(pcap_lookupnet(dev, &net, &mask, errbuf) == -1)
    {
        fprintf(stderr, "Can't get netmask for device %s\n", dev);
        net = 0;
        mask = 0;
    }

    handle = pcap_open_live(dev, BUFSIZ, 1, TIMEOUT_MS, errbuf);                // 0 = non promiscuo

    if(handle == NULL)
    {
        fprintf(stderr, "Couldn't open device %s: %s\n", dev, errbuf);
        return(2);
    }

    /* make sure we're capturing on an Ethernet device [2] */
    if(pcap_datalink(handle) != DLT_EN10MB)
    {
        fprintf(stderr, "%s is not an Ethernet\n", dev);
        exit(EXIT_FAILURE);
    }

    if(pcap_compile(handle, &fp, filter_exp, 0, net) == -1)
    {
        fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return(2);
    }

    if(pcap_setfilter(handle, &fp) == -1)
    {
        fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
        return(2);
    }

//  outFile = fopen("/usr/local/src/robot/debugging/pcap.log", "w");

    // Get into the loop (MAX_ACQUISITION = -1 means untill error)
    pcap_loop(handle, MAX_ACQUISITION, got_packet, NULL);

    printf("\nCapture complete.\n");

    /* And close the session */
    pcap_freecode(&fp);
    pcap_close(handle);

    printEverything();

    return(0); // everything ok
}


/*
 * dissect/print packet
 */
void got_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet)
{

    /* declare pointers to packet headers */
    const struct sniff_ethernet *ethernet;    /* The ethernet header [1] */
    const struct sniff_ip *ip;                /* The IP header  */
    const struct udp_header *udp_h;           /* The UFP header */
    const u_char *payload;                    /* Packet payload */

    int size_ip;
    int size_udp = sizeof(udp_header);
    int size_payload;

    /* define ethernet header */
    ethernet = (struct sniff_ethernet *)(packet);

    /* define/compute ip header offset */
    ip = (struct sniff_ip *)(packet + SIZE_ETHERNET);
    size_ip = IP_HL(ip)*4;

    if(size_ip < 20)
    {
        printf("   * Invalid IP header length: %u bytes\n", size_ip);
        return;
    }

    bool udp=false;

    /* determine protocol */
    switch(ip->ip_p)
    {
        case IPPROTO_TCP:
            print_debug("   Protocol: TCP\n");
            return;
        case IPPROTO_UDP:
            udp=true;
//          print_debug("   Protocol: UDP\n");
            break;
        case IPPROTO_ICMP:
            print_debug("   Protocol: ICMP\n");
            return;
        case IPPROTO_IP:
            print_debug("   Protocol: IP\n");
            return;
        default:
            print_debug("   Protocol: unknown\n");
            return;
    }

    /*
     *  OK, this packet is UDP.
     */
    /* define/compute udp header offset */
    udp_h = (struct udp_header *)(packet + SIZE_ETHERNET + size_ip);

    /* define/compute udp payload (segment) offset */
    payload = (u_char *)(packet + SIZE_ETHERNET + size_ip + size_udp);

    /* compute udp payload (segment) size */
    size_payload = ntohs(ip->ip_len) - (size_ip + size_udp);

    // Call debug functions
    ACE_INET_Addr sender_addr(ntohs(udp_h->srcport),  inet_ntoa(ip->ip_src));
    ACE_INET_Addr   dest_addr(ntohs(udp_h->destport), inet_ntoa(ip->ip_dst));
    check_received_pkt(ip->ip_src, ip->ip_dst, (void *) payload, size_payload, &header->ts);

    return;
}

