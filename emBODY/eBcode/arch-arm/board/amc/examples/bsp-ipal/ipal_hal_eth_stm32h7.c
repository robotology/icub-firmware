/**
 * @file ipal_hal_eth_stm32h7.c
 * 
 * IPAL HAL ETH - Function implementations
 */
#include "ipal_hal_eth_stm32h7.h"


#if defined(USE_STM32HAL)
#include "stm32hal.h"
// for now i add EMBOT_ENABLE_hw_eth_phytimeout simply like that. 
// later on when this file gets absorbed inside embot::hw::bsp it 
// will be seen by the inclusion of embot_hw_bsp_config.h 
#define EMBOT_ENABLE_hw_eth_phytimeout
#else
#include "Driver_ETH_MAC.h"
#include "Driver_ETH_PHY.h"
#include "stm32h7xx.h"                  
#include "stm32h7xx_hal.h"
#endif
#include <string.h>
#include <stdbool.h>



/* ============================================================================================
 * MACROS
 * ============================================================================================ */

/* Mnemonic access to ARM PHY DRIVER */
#define PHYDRV				eth_arm_drv_link.phy

/* Mnemonic access to ARM MAC DRIVER */
#define MACDRV				eth_arm_drv_link.mac

/* Mnemonic access to ARM DRIVER Link State */
#define LINKSTATE			eth_arm_drv_link.link

/* ============================================================================================
 * EXTERNAL SYMBOLS
 * ============================================================================================ */

/* ============================================================================================
 * TYPE DEFINITIONS 
 * ============================================================================================ */

/**
 * Ethernet ARM Driver (MAC and PHY) and link status
 */
typedef struct 
{
	/* Registered MAC driver */
	ARM_DRIVER_ETH_MAC *mac;              
	
	/* Registered PHY driver */	
	ARM_DRIVER_ETH_PHY *phy;              
		
	/* Link state */
	ARM_ETH_LINK_STATE link;
	
} eth_arm_drv_link_t;

/* ============================================================================================
 * EXTERNAL SYMBOLS
 * ============================================================================================ */

/* Ethernet MAC & PHY Driver - Only one interface (driver number 0) is currently supported */
extern ARM_DRIVER_ETH_MAC Driver_ETH_MAC0;
extern ARM_DRIVER_ETH_PHY Driver_ETH_PHY0;

/* ============================================================================================
 * STATIC VARIABLES
 * ============================================================================================ */

/* IPAL Ethernet configuration */
static ipal_hal_eth_cfg_t ipal_eth_cfg;

/* Ethernet MAC driver and link */
static eth_arm_drv_link_t eth_arm_drv_link; 

/* True when transmission is in course (DMA is emptying the out buffer) */
static volatile bool tx_in_course = false;

/* ============================================================================================
 * PRIVATE FUNCTIONS
 * ============================================================================================ */
 
/**
 * Get frame from MAC controller
 */
static void ipal_to_arm_mac_addr(ipal_hal_eth_cfg_t *ipal_eth_cfg, ARM_ETH_MAC_ADDR *arm_eth_mac_addr )
{
	uint64_t mac_address = ipal_eth_cfg->macaddress;
	
	/* ARM Eth MAC address */
	arm_eth_mac_addr->b[0] = ((uint8_t *)&mac_address)[0];
	arm_eth_mac_addr->b[1] = ((uint8_t *)&mac_address)[1];
	arm_eth_mac_addr->b[2] = ((uint8_t *)&mac_address)[2];
	arm_eth_mac_addr->b[3] = ((uint8_t *)&mac_address)[3];
	arm_eth_mac_addr->b[4] = ((uint8_t *)&mac_address)[4];
	arm_eth_mac_addr->b[5] = ((uint8_t *)&mac_address)[5];
}

bool dbg_eth_irq = false;
uint8_t dbg_eth_irq_step = 0;
 
/**
 * Ethernet Frame receive callback
 * @param arm_event - arm cmsis driver event
 */
static void eth_frame_rx_cb(uint32_t arm_event)
{
	dbg_eth_irq = true;
	
	/* Signal event */
	if (arm_event == ARM_ETH_MAC_EVENT_RX_FRAME) 
	{	
		/* Signal event to higher layer */
		ipal_eth_cfg.onframerx->frame_movetohigherlayer(NULL);
	}
	
	if (arm_event == ARM_ETH_MAC_EVENT_TX_FRAME) 
	{	
		/* Signal the end of transmission */
		//ipal_eth_cfg.onframetx->frame_tx_done_cb();	
		tx_in_course = false;
	}
	
	
	dbg_eth_irq = false;
}
	
/* ============================================================================================
 * PUBLIC FUNCTIONS
 * ============================================================================================ */
 
/* --------------------------------------------------------------------------------------------
 * MOUNTED AS EXTERNAL FUNCTIONS IN IPAL_CFG
 * -------------------------------------------------------------------------------------------- */

/* 
 * ipal_hal_eth_stm32h7_init
 * See header file for details
 */
ipal_result_t ipal_hal_eth_stm32h7_init(ipal_hal_eth_cfg_t *cfg)
{ 
	ARM_ETH_MAC_CAPABILITIES cap;
	ARM_ETH_MAC_ADDR arm_eth_mac_addr;
	ARM_ETH_LINK_STATE link;
	ARM_ETH_LINK_INFO link_info;
	
	/* Copy the configuration, passed as parameter */
	memcpy(&ipal_eth_cfg, cfg, sizeof(ipal_hal_eth_cfg_t));
	
	/* Sets reference to CMSIS Drivers */
	eth_arm_drv_link.phy 	= &Driver_ETH_PHY0;
	eth_arm_drv_link.mac 	= &Driver_ETH_MAC0;
	eth_arm_drv_link.link = ARM_ETH_LINK_DOWN;
	
	/* Get MAC capabilities */
  cap = MACDRV->GetCapabilities ();
	
	/* Currently unused */
	cap = cap;

	/* Receive callback is passed as parameter */
  MACDRV->Initialize (eth_frame_rx_cb);
	
	/* Clock and power on the peripheral */
  MACDRV->PowerControl (ARM_POWER_FULL);
	
	/* Sets MAC address */
	ipal_to_arm_mac_addr(&ipal_eth_cfg, &arm_eth_mac_addr);
	MACDRV->SetMacAddress(&arm_eth_mac_addr);
	
  MACDRV->Control (ARM_ETH_MAC_CONTROL_TX, 0);
  MACDRV->Control (ARM_ETH_MAC_CONTROL_RX, 0);

  /* Initialize Physical Media Interface */
  if (PHYDRV->Initialize (MACDRV->PHY_Read, MACDRV->PHY_Write) == ARM_DRIVER_OK) {
    PHYDRV->PowerControl (ARM_POWER_FULL);
		PHYDRV->SetInterface (cap.media_interface);
    PHYDRV->SetMode (ARM_ETH_PHY_AUTO_NEGOTIATE);
  }
	else 
	{
		return ipal_res_NOK_generic; 
	}

#if defined(EMBOT_ENABLE_hw_eth_phytimeout)
    // it is just a tentative. not yet tested. i dont know what happens if 
    // the cable is not attached and attached later. implementation on the ems
    // allows recover. in here if cable is not attached everything is to be tested.    
    const uint32_t phytimeout = 100;
    volatile bool timedout = false;
    uint32_t now = HAL_GetTick();
    uint32_t delta = 0;
	for(;;)
	{        
		link = PHYDRV->GetLinkState();
        if(ARM_ETH_LINK_UP == link)
        {
            break;
        }
        else if((delta = (HAL_GetTick() - now)) > phytimeout)
        {
            timedout = true;
            break;
        }
	} 
#else    
	/* MARTINO: bloccante. Rivedere */
	do 
	{
		link = PHYDRV->GetLinkState();
	} while (link == ARM_ETH_LINK_DOWN);
#endif
	
	/* Start EMAC DMA */
	link_info = PHYDRV->GetLinkInfo();
	
	MACDRV->Control (ARM_ETH_MAC_CONFIGURE,
									link_info.speed  << ARM_ETH_MAC_SPEED_Pos  | 
									link_info.duplex << ARM_ETH_MAC_DUPLEX_Pos |
                  ARM_ETH_MAC_ADDRESS_BROADCAST         );
  
	MACDRV->Control(ARM_ETH_MAC_CONTROL_TX,1);
  MACDRV->Control(ARM_ETH_MAC_CONTROL_RX,1);
  
	return ipal_res_OK;
}

/* 
 * ipal_hal_eth_stm32h7_sendframe
 * See header file for details
 */
ipal_result_t ipal_hal_eth_stm32h7_sendframe(ipal_hal_eth_frame_t *frame)
{
	uint32_t flags;
	
	//ipal_hal_eth_frame_t *eth_frame = (ipal_hal_eth_frame_t *)frame; 
	
	/* Wait for the transmission of previous frame */
	while(tx_in_course);
	
	/* Set TX in course */
	tx_in_course = true;
	
	/* MARTINO: fragmented frame are not supported */
	flags = 0; 
	
	/* Send frame using CMSIS driver */
	MACDRV->SendFrame(frame->payload, frame->length, flags);
	
	return ipal_res_OK;
}
	
/* 
 * ipal_hal_eth_stm32h7_enable
 * See header file for details
 */
ipal_result_t ipal_hal_eth_stm32h7_enable(void)
{
	NVIC_EnableIRQ(ETH_IRQn);
	
	return ipal_res_OK;
}	
	
/* 
 * ipal_hal_eth_stm32h7_disable
 * See header file for details
 */
ipal_result_t ipal_hal_eth_stm32h7_disable(void)
{
	NVIC_DisableIRQ(ETH_IRQn);
	
	return ipal_res_OK;
}	

/* -------------------------------------------------------------------------------------------
 * AUXILIARY FUNCTIONS
 * Mounted as external functions in ipal_hal_eth_aux_function
 * ------------------------------------------------------------------------------------------- */

/* Get frame size */                                                                         
uint16_t ipal_hal_eth_stm32h7_get_frame_size(void)
{
	return MACDRV->GetRxFrameSize();
}
	
/* Get frame */
void ipal_hal_eth_stm32h7_get_frame(uint16_t length, uint8_t* frame)
{
	/* Drop oversized packet */
	if (length > 1514) 
	{
		MACDRV->ReadFrame (NULL, 0);
  }
	else 
	{
		MACDRV->ReadFrame (frame, length);
	}
}


