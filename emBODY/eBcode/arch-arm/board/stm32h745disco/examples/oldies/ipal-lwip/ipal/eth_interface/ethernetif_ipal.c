/**
 * @file ethernetif_hw.c
 *
 * Specific (hardware dependent) implementation of ETH hardware access functions
 */
#include "ethernetif_ipal.h"
#include "ethernetif_use_ipal.h"
#include "ipal_hal_eth_types.h"
#include "ipal_debug.h"

/* IPAL friend functions */
#include "ipal_hid.h"

#include "ipal_hal_eth_lwip_aux_functions.h"

#ifdef FRAME_QUEUE
	#include "lwip_frame_queue_handler.h"
#endif

/* LWIP specific functions */
#include "lwip/pbuf.h"
#include "arch/sys_arch.h"
#include "lwip/sys.h"
#include "lwip/stats.h"
#include "lwip/snmp.h"

#include <string.h>
#include <stdio.h>

/* ======================================================================================
 * PUBLIC FUNCTIONS
 * ====================================================================================== */

/* The ETH STatus */
extern eth_status_t eth_status;


/* ======================================================================================
 * STATIC VARIABLES
 * ====================================================================================== */
 
/* Transmission done callback (function declaration) */
static void lwip_tx_done_cb(void);
 
/*
 * On Frame Reception data structure
 */
static ipal_eth_onframereception_t ipal_eth_onframe_reception = 
{
	/* Frame new - Unused */
	.frame_new = NULL,
    
	/* Move to higher layer */
	.frame_movetohigherlayer = ethernetif_IPAL_ethernetif_input,
        
	/* Alert higher layer - Unused */
	.frame_alerthigherlayer = NULL
};

#if (0)

	/*
	 * Sets tx done callback
	 */
	static ipal_eth_onframe_transmission_t ipal_eth_onframe_transmission =
	{
		.frame_tx_done_cb = lwip_tx_done_cb
	};

#endif

/* ======================================================================================
 * PRIVATE FUNCTIONS
 * ====================================================================================== */

/*
 * Transmission done callback.
 * It will be invoked by interrupt handler.
 */
static void lwip_tx_done_cb(void)
{
	/* This say that a new transmission could start, the previous one has been completed */
	eth_status.tx_in_course = false;
}

/* ======================================================================================
 * PUBLIC FUNCTIONS
 * ====================================================================================== */

/* -----------------------------------------------------------------------------------------------
 * DRIVER FUNCTIONS
 * ----------------------------------------------------------------------------------------------- */
 
/*
 * ethernetif_IPAL_low_level_init
 * See header file for details
 */
void ethernetif_IPAL_low_level_init(void)
{
	ipal_result_t res;
	
	/* IPAL ETH Configuration */
	ipal_eth_cfg_t ipal_eth_cfg = 
	{
		.priority 		= 0,
		.dummy_0			= 0,
		.dummy_1			= 0,
		.dummy_2    	= 0,       
		.onframerx		= &ipal_eth_onframe_reception,
		//.onframetx		= &ipal_eth_onframe_transmission,
		.macaddress		= ipal_base_hid_cfg.eth_mac
	};
	
	/* Get a reference to IPAL external functions */
	ipal_fn_cfg_t *ipal_ext_func = &ipal_base_hid_cfg.extfn;

	/* Hardware dependent initialization function */
	res = ipal_ext_func->hal_eth_init(&ipal_eth_cfg);
	
	if(ipal_res_OK != res)
	{
		ipal_base_hid_on_fatalerror(ipal_error_generic, "low_level_init: extfn.hal_eth_init failed ");
	}	
}

/*
 * ethernetif_IPAL_low_level_output
 * See header file for details
 */
ipal_result_t ethernetif_IPAL_low_level_output(eth_frame_t *frame)
{
	ipal_result_t res;
	
	/* Get a reference to IPAL external functions */
	ipal_fn_cfg_t *ipal_ext_func = &ipal_base_hid_cfg.extfn;
	
	/* Send frame using hw dependent driver functions */
	res = ipal_ext_func->hal_eth_sendframe(frame);
	
	return res;
}

/* -----------------------------------------------------------------------------------------------
 * CALLBACKS
 * ----------------------------------------------------------------------------------------------- */

extern uint8_t dbg_eth_irq_step;

/*
 * ethernetif_IPAL_ethernetif_input
 * EXECUTED IN IRQ HANDLER as User callback
 * See header file for details
 */
void ethernetif_IPAL_ethernetif_input(ipal_eth_frame_t *frame)
{	
	dbg_ipal_irqrx_frame_detected = true;
	dbg_rcv_frame++;
	
	#ifndef FRAME_QUEUE
		frame = frame;
	
		/* Just notify packet */
		eth_status.n_rx_events_signaled += 1;
	
		//for(uint16_t i=0; i<1000; i++);
	
	#else
		u16_t len;
		frame = frame;
		lwip_rxframe_container_t *rx_container;
		uint8_t *payload;
	
		/* Get frame length from the controller */
		len = ipal_hal_eth_aux_functions.get_frame_size();
	
		if (len == 0)
		{
			/* Do nothing */
		}
		else if (len > 1514) 
		{
			/* Drop oversized packet */
			ipal_hal_eth_aux_functions.get_frame(0, NULL);
		}
		else 
		{			
			/* Get storage for frame */
			rx_container = lwip_frame_queue_RX_NEW();
			
			rx_container->length = len;
			payload = (uint8_t *)rx_container->payload;
			
			/* Give space for ETH_PAD */
			#if ETH_PAD_SIZE
				payload += ETH_PAD_SIZE;
				rx_container->payload = payload;
			#endif
							
			/* Get frame */
			ipal_hal_eth_aux_functions.get_frame(len, payload);
		}
	#endif
	
		/* Signal external thread that a new frame is available */
		ipal_hal_eth_aux_functions.signal_rx_frame();
		
		dbg_ipal_irqrx_frame_detected = false;
}

 
