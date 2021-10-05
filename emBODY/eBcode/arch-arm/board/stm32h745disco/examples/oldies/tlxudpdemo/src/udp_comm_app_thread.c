/** 
 * @file udp_comm_app_thread.c
 *
 * UDP Communication App example
 * Implementation of function prototypes defined in udp_comm_app_thread.h
 */
#include "udp_comm_app_thread.h"
#include "udp_comm_app_thread_conf.h"
#include "cmsis_os2.h"       
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* IPAL specific includes  */
#include "ipal.h"

/* Some auxiliary functions */
#include "tlx_ipal_aux.h"

#include <stdbool.h>

/* ==============================================================================
 * MACROS
 * ============================================================================== */

#define UDP_APP_MAX_PAYLOAD (1024)

/* ==============================================================================
 * TYPE DEFINITIONS
 * ============================================================================== */

typedef union
{
	uint16_t value;
	
	struct 
	{
		uint8_t lsb;
		uint8_t msb;
	} bytes;
	
} uint16_union_t;

/* ==============================================================================
 * STATIC VARIABLES
 * ============================================================================== */

/* Set when a frame (answer) is received, reset before sending next */
static volatile bool answer_received = true;
 
/* Net thread ID */
static osThreadId_t tid_udp_comm_app_thread;

/* Net thread attributes */
static osThreadAttr_t udp_comm_app_attr = 
{
	.name = "UDP Comm Attr",
	.stack_size = 2056
};

/* IPAL UDP socket */
static ipal_udpsocket_t *ipal_udp_socket;

/* Type of service */
static const ipal_tos_t ipal_udp_socket_TOS  = 
{
	.precedence 			= ipal_prec_immediate,
	.lowdelay					= 1,
	.highthroughput 	= 1,
  .highreliability	= 0
};

/* UDP SEND PAYLOAD */
static uint8_t udp_app_send_payload[UDP_APP_MAX_PAYLOAD];

/* ==============================================================================
 * EXTERN SYMBOLS
 * ============================================================================== */

/* IPAL app cfg */
extern ipal_cfg_t ipal_app_cfg;

/* ==============================================================================
 * PRIVATE FUNCTIONS
 * ============================================================================== */

/* Error handling for this application module */
static void udp_comm_app_error_handling(void)
{
	while(1)
	{
	}
}

volatile uint16_t dbg_missed_frames = 0;

/*
 * IPAL UDP callback function of reception
 */
static void udp_rcv_cb_func(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t port)
{
	static uint16_union_t old_cnt = { .value = 0};
	uint16_union_t cur_cnt;
	
	uint8_t remote_addr[4];
	
	uint8_t *rcv_buf = (uint8_t *)arg;
	
	memcpy(rcv_buf, pkt->data, pkt->size);
	
	tlx_ipal_IPALaddr_to_array(adr, remote_addr);
	
	answer_received = true;
	
	printf("Received a datagram from %d.%d.%d.%d:%d = %02X %02X\n", 
					remote_addr[0],
					remote_addr[1],
					remote_addr[2],
					remote_addr[3],
					port,
					rcv_buf[0], rcv_buf[1]);

	/* Coherency control */
	cur_cnt.bytes.lsb = rcv_buf[1];
	cur_cnt.bytes.msb = rcv_buf[0];
	
	if (cur_cnt.value != old_cnt.value)
	{
		/* Error HANDLER here */
		dbg_missed_frames++;
	
		old_cnt.value = cur_cnt.value;
		old_cnt.value += 1;		
		/* Error HANDLER here */
	}
	else 
	{
		old_cnt.value += 1;
	}
}

/**
 * The UDP Communication Thread
 */
static void udp_comm_app_thread(void *argument)
{
	uint32_t delay;
	ipal_result_t udp_err;
	ipal_packet_t ipal_send_packet;
	
	/* Just a counter */
	uint16_union_t cnt;
	
	/* Sets buffer in packet */
	ipal_send_packet.size = 4;
	ipal_send_packet.data = udp_app_send_payload;
	
	osDelay(5000);
	
	/* Getting an UDP Socket */
	printf("Getting UDP Socket ...\n");
	
	/* Gets UDP Socket */
	ipal_udp_socket = ipal_udpsocket_new(ipal_udp_socket_TOS);
	
	/* Check if failed */
	if (ipal_udp_socket == NULL)
	{
		printf("Failed getting UDP socket ...\n");
		
		udp_comm_app_error_handling();
	}
	
	/* Binding UDP Socket */
	printf("Binding UDP socket ...\n");
	
	/* Bind UDP Socket to local address and port */
	udp_err =	ipal_udpsocket_bind(ipal_udp_socket, 
																ipal_app_cfg.eth_ip,
																TLX_UDP_PORT);

	/* Checks for failure */
	if (udp_err != ipal_res_OK)
	{
		printf("Binding UDP socket ...\n");
		
		udp_comm_app_error_handling();
	}
	
	printf("Connecting to remote system ...\n");
	
	/* Connect to remote system */
	udp_err = ipal_udpsocket_connect(ipal_udp_socket, 
																	 tlx_ipal_get_IPAL_ipv4addr(
																			TLX_UDP_REMOTE_IP0, 
																			TLX_UDP_REMOTE_IP1, 
																			TLX_UDP_REMOTE_IP2, 
																			TLX_UDP_REMOTE_IP3), 
																	 TLX_UDP_REMOTE_PORT);
	
	if (udp_err != ipal_res_OK)
	{
		/* Opening an UDP Socket */
		printf("Impossible to connect to remote system ...\n");
		
		udp_comm_app_error_handling();
	}
	
	printf("Registering receive callback ...\n");
	
	
	/* Register receive callback */
	udp_err = ipal_udpsocket_recv(ipal_udp_socket, udp_rcv_cb_func, NULL);
	
	if (udp_err != ipal_res_OK)
	{
		/* Opening an UDP Socket */
		printf("Impossible to register the receive callback ...\n");
		
		udp_comm_app_error_handling();
	}
	
	cnt.value = 0;
	
	/* Initialize random number system */
	srand(10);
	
	/* Main cycle */
	while (1) 
	{
		if (answer_received)
		{
			/* If answer received, send next */
			answer_received = false;
			
			/* Sending datagram */
			printf("Sending a datagram to %d.%d.%d.%d:%d...\n",
								TLX_UDP_REMOTE_IP0,
								TLX_UDP_REMOTE_IP1,
								TLX_UDP_REMOTE_IP2,
								TLX_UDP_REMOTE_IP3,
								TLX_UDP_REMOTE_PORT);
			
			udp_app_send_payload[0] = 'h'; //cnt.bytes.msb;
			udp_app_send_payload[1] = 'i'; // cnt.bytes.lsb;
            udp_app_send_payload[2] = '/n'; // cnt.bytes.lsb;
            udp_app_send_payload[3] = 0; // cnt.bytes.lsb;
			
			/* Fill send buffer */
			cnt.value++;
			
			udp_err = ipal_udpsocket_send(ipal_udp_socket, &ipal_send_packet);
			
			if (udp_err != ipal_res_OK)
			{
				printf("Error occurred sending packet to remote system ...\n");
				
				udp_comm_app_error_handling();
			}
			else 
			{
				printf("...sent!: %02X %02X\n", udp_app_send_payload[0], udp_app_send_payload[1]);
			}		
		}
		
		
		delay = 10 + rand()%50;
						
		/* Wait for a random delay between 10 and 60 ms */
		osDelay(delay);
		
  }
}

/* ==============================================================================
 * PUBLIC FUNCTIONS
 * ============================================================================== */
 
/* 
 * udp_comm_app_thread_init
 * See header file for details
 */
int udp_comm_app_thread_init(void)
{
	tid_udp_comm_app_thread = osThreadNew(udp_comm_app_thread, NULL, &udp_comm_app_attr);
  
	if (tid_udp_comm_app_thread == NULL) {
    return(-1);
  }
 
  return(0);
}

