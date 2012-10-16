/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero, Marco Accame
 * email:   valentina.gaggero@iit.it, marco.accame@iit.it
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


/* @file       hal_eth.c
    @brief      This file implements internal implementation of the hal eth module.
    @author     marco.accame@iit.it
    @date       05/27/2010
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_ETH

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "hal_stm32xx_include.h"
#include "string.h"
#include "hal_stm32_base_hid.h" 
#include "hal_stm32_sys_hid.h"
#include "hal_brdcfg.h"
#include "hal_switch.h"

#include "utils/stm32gpio_hid.h"

//#define HAL_USE_EVENTVIEWER_ETH

#if defined(HAL_USE_EVENTVIEWER_ETH)
#include "eventviewer.h"
#include "hal_arch_arm.h"
#endif 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_eth.h" 


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_stm32_eth_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#include "hal_stm32_eth_def.h" 


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef uint32_t    U32;


typedef struct {
    uint32_t volatile Stat;
    uint32_t Ctrl;
    uint32_t Addr;
    uint32_t Next;
} hal_rx_desc_t;

typedef struct {
    uint32_t volatile CtrlStat;
    uint32_t Size;
    uint32_t Addr;
    uint32_t Next;
} hal_tx_desc_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_eth_supported_is(void);
static void s_hal_eth_initted_set(void);
static hal_boolval_t s_hal_eth_initted_is(void);

// keep the same names of the tcpnet driver, without s_ prefix
static void rx_descr_init (void);
static void tx_descr_init (void);


static void s_hal_eth_rmii_init(void);

static void s_hal_eth_mac_reset(void);
static void s_hal_eth_mac_init(const hal_eth_cfg_t *cfg, hal_eth_phymode_t phymode);

static int8_t s_hal_eth_gpioeth_init(stm32gpio_gpio_t gpio, uint8_t mode);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// keep teh same names of the tcpnet driver, without s_ prefix
static uint8_t RxBufIndex = 0;
static uint8_t TxBufIndex = 0;

// keep teh same names of the tcpnet driver, without s_ prefix. however, make them pointers
/* ENET local DMA buffers. */
static uint32_t (*rx_buf)[ETH_BUF_SIZE>>2] = NULL;
static uint32_t (*tx_buf)[ETH_BUF_SIZE>>2] = NULL;

// keep teh same names of the tcpnet driver, without s_ prefix. however, make them pointers
/* ENET local DMA Descriptors. */
static hal_rx_desc_t *Rx_Desc = NULL;
static hal_tx_desc_t *Tx_Desc = NULL;


// i need to initialise that with a proper value
static hal_eth_onframereception_t s_onframerx =
{
    .frame_new                  = NULL,
    .frame_movetohigherlayer    = NULL,
    .frame_alerthigherlayer     = NULL
};

// i need to initialise that with a proper value
static uint8_t *s_hal_mac = NULL;

static const hal_eth_network_functions_t s_hal_eth_fns = 
{
    .eth_init           = hal_eth_init, 
    .eth_enable         = hal_eth_enable, 
    .eth_disable        = hal_eth_disable, 
    .eth_sendframe      = hal_eth_sendframe
};


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern void ETH_IRQHandler(void);


extern hal_result_t hal_eth_init(const hal_eth_cfg_t *cfg)
{
    hal_eth_phymode_t phymode;

    if(hal_true != s_hal_eth_supported_is())
    {
        return(hal_res_NOK_unsupported);
    }


    if((NULL == cfg) || (hal_int_priorityNONE == cfg->priority) || (NULL == cfg->onframerx) || (NULL == cfg->onframerx->frame_new) || (NULL == cfg->onframerx->frame_movetohigherlayer) || (NULL == cfg->macaddress))
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_eth_init() needs a cfg w/ functions and mac addr and valid priority");
    }
    else
    {
        memcpy(&s_onframerx, cfg->onframerx, sizeof(hal_eth_onframereception_t));
        s_hal_mac = (uint8_t*)&(cfg->macaddress);
    }


//    eventviewer_init();
#if defined(HAL_USE_EVENTVIEWER_ETH)
    eventviewer_load(ev_ID_first_isr+hal_arch_arm_ETH_IRQn, ETH_IRQHandler);
#endif
    
    // in here we allow a specific board to init all what is related to the phy.
    // in case of a phy accessed through the smi, this function must: a. init the smi, b. reset the phy, ... that's it.
    // instead in case of a switch accessed through i2c, this function must: a. init the i2c, reset the switch, that's it.    
    if(hal_false == hal_brdcfg_eth__phy_initialise())
    {
         hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_eth_init() called hal_brdcfg_eth__phy_initialise() which has failed");
    }        
    
  
    // init rmii
    s_hal_eth_rmii_init(); 


    // s_hal_eth_mac_reset();  // can be moved just after configuration of mac register  
    
    // in here we allow a specific board to configure and start the phy. with a mode (full/half) and a speed (10/100) whcih are board specifics,
    // but which we retrieve
    // in case of a phy accessed through the smi, this function must set the mode and the speed and return the result
    // instead in case of a switch accessed through i2c, this function must configure mode and speed and put the swicth operative. 
    memcpy(&phymode, &hal_brdcfg_eth__phymode, sizeof(hal_eth_phymode_t));
    hal_brdcfg_eth__phy_configure(&phymode); 
    
    
    if((hal_eth_speed_none == phymode.speed) || (hal_eth_mux_none == phymode.mux))
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_eth_init() called hal_brdcfg_eth__phy_configure() which does not support a phy mode");
    }
    
    
    s_hal_eth_mac_init(cfg, phymode);
    

    s_hal_eth_initted_set();
    
    return(hal_res_OK);
}


extern hal_result_t hal_eth_enable(void) 
{
    // acemor on 31 oct 2011: it is the same as NVIC_EnableIRQ(ETH_IRQn) or hal_sys_hid_irqn_enable()
    /* Ethernet Interrupt Enable function. */
    NVIC->ISER[1] = 1 << 29;

    return(hal_res_OK);
}


extern hal_result_t hal_eth_disable(void) 
{
    // acemor on 31 oct 2011: it is the same as NVIC_DisableIRQ(ETH_IRQn) or hal_sys_hid_irqn_disable()
    /* Ethernet Interrupt Disable function. */
    NVIC->ICER[1] = 1 << 29;

    return(hal_res_OK);
}


// changed NUM_TX_BUF with hal_base_hid_params.eth_dmatxbuffer_num
extern hal_result_t hal_eth_sendframe(hal_eth_frame_t *frame) 
{
    // called by tcpnet's send_frame(OS_FRAME *frame)
    
    // il caller passa (U32 *)&frame->data[0] e frame->length

    /* Send frame to ETH ethernet controller */
    U32 *sp,*dp;
    U32 i,j;


    if(hal_true != s_hal_eth_initted_is())
    {
        return(hal_res_NOK_generic);
    }


    j = TxBufIndex;
    /* Wait until previous packet transmitted. */
    while (Tx_Desc[j].CtrlStat & DMA_TX_OWN);

    sp = (U32 *)&frame->datafirstbyte[0];
    dp = (U32 *)(Tx_Desc[j].Addr & ~3);

    /* Copy frame data to ETH IO buffer. */
    for (i = (frame->length + 3) >> 2; i; i--) {
        *dp++ = *sp++;
    }
    Tx_Desc[j].Size      = frame->length;
    Tx_Desc[j].CtrlStat |= DMA_TX_OWN;
    if (++j == hal_base_hid_params.eth_dmatxbuffer_num) j = 0;
    TxBufIndex = j;
    /* Start frame transmission. */
    ETH->DMASR   = DSR_TPSS;
    ETH->DMATPDR = 0;

    return(hal_res_OK);
}


extern const hal_eth_network_functions_t * hal_eth_get_network_functions(void)
{
    return(&s_hal_eth_fns);
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----

// changed NUM_RX_BUF with hal_base_hid_params.eth_dmarxbuffer_num
// changed OS_FRAME with hal_eth_frame_t
void ETH_IRQHandler(void) 
{
  // questa non va cambiata. pero' .... usa OS_FRAME e chiama alloc_mem() e put_in_queue().
  // bisogna usare alcuni puntatori a funzione che siano inizializzati / passati in hal_eth_init().
  // inoltre, se si vuole segnalare l'arrivo di un frame gia' qui dentro, allora bisogna definire
  // un altro puntatore a funzione hal_on_rx_frame().



  /* Ethernet Controller Interrupt function. */
  hal_eth_frame_t *frame;
  U32 i,RxLen;
  volatile U32 int_stat;
  U32 *sp,*dp;

//  acemor on 18-mar-2011: i removed them two as they were used only for debug purposes
//  static uint32_t txframes = 0;
//  static uint32_t rxframes = 0;

#if defined(HAL_USE_EVENTVIEWER_ETH)
  evEntityId_t prev = eventviewer_switch_to(ev_ID_first_isr+hal_arch_arm_ETH_IRQn);
#endif

  while (((int_stat = ETH->DMASR) & INT_NISE) != 0) {
    ETH->DMASR = int_stat;
    if (int_stat & INT_RIE) {
      /* Valid frame has been received. */
      i = RxBufIndex;
      if (Rx_Desc[i].Stat & DMA_RX_ERROR_MASK) {
        goto rel;
      }
      if ((Rx_Desc[i].Stat & DMA_RX_SEG_MASK) != DMA_RX_SEG_MASK) {
        goto rel;
      }
      RxLen = ((Rx_Desc[i].Stat >> 16) & 0x3FFF) - 4;
      if (RxLen > ETH_MTU) {
        /* Packet too big, ignore it and free buffer. */
        goto rel;
      }
      /* Flag 0x80000000 to skip sys_error() call when out of memory. */
      //frame = alloc_mem (RxLen | 0x80000000);
      frame = s_onframerx.frame_new(RxLen | 0x80000000);
      /* if 'alloc_mem()' has failed, ignore this packet. */
      if (frame != NULL) {
        sp = (U32 *)(Rx_Desc[i].Addr & ~3);
        dp = (U32 *)&frame->datafirstbyte[0];
        for (RxLen = (RxLen + 3) >> 2; RxLen; RxLen--) {
          *dp++ = *sp++;
        }
        //put_in_queue (frame);
        s_onframerx.frame_movetohigherlayer(frame);

        if(NULL != s_onframerx.frame_alerthigherlayer)
        {
            s_onframerx.frame_alerthigherlayer();
        }

        //rxframes ++;

      }
      /* Release this frame from ETH IO buffer. */
rel:  Rx_Desc[i].Stat = DMA_RX_OWN;

      if (++i == hal_base_hid_params.eth_dmarxbuffer_num) i = 0;
      RxBufIndex = i;
    }
    if (int_stat & INT_TIE) {
      /* Frame transmit completed. */
      //txframes++;
    }
  }

#if defined(HAL_USE_EVENTVIEWER_ETH)    
  eventviewer_switch_to(prev);
#endif
}


// ---- isr of the module: end ------


extern uint32_t hal_eth_hid_getsize(const hal_cfg_t *cfg)
{
    uint32_t size = 0;

    size += (ETH_BUF_SIZE * cfg->eth_dmarxbuffer_num);
    size += (ETH_BUF_SIZE * cfg->eth_dmatxbuffer_num);

    size += (sizeof(hal_rx_desc_t) * cfg->eth_dmarxbuffer_num);
    size += (sizeof(hal_tx_desc_t) * cfg->eth_dmatxbuffer_num); 

    return(size);
}

extern hal_result_t hal_eth_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{

    // removed dependency from nzi memory
    RxBufIndex = 0;
    TxBufIndex = 0;
    rx_buf = NULL;
    tx_buf = NULL;
    Rx_Desc = NULL;
    Tx_Desc = NULL;
    memset(&s_onframerx, 0, sizeof(s_onframerx));
    s_hal_mac = NULL;


    if(NULL == memory)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_eth_hid_setmem(): memory missing");
        return(hal_res_NOK_generic);
    }

    rx_buf = (uint32_t (*)[ETH_BUF_SIZE>>2]) memory;
    memory += ((ETH_BUF_SIZE * cfg->eth_dmarxbuffer_num)/4);
     
    tx_buf = (uint32_t (*)[ETH_BUF_SIZE>>2]) memory;
    memory += ((ETH_BUF_SIZE * cfg->eth_dmatxbuffer_num)/4);

    Rx_Desc = (hal_rx_desc_t*) memory;
    memory += ((sizeof(hal_rx_desc_t) * cfg->eth_dmarxbuffer_num)/4);

    Tx_Desc = (hal_tx_desc_t*) memory;
    memory += ((sizeof(hal_tx_desc_t) * cfg->eth_dmatxbuffer_num)/4);

    return(hal_res_OK);

}

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_eth_supported_is(void)
{
    return(hal_base_hid_byte_bitcheck(hal_brdcfg_eth__supported_mask, 0) );
}

static void s_hal_eth_initted_set(void)
{
    //s_hal_mac = s_hal_mac;
}

static hal_boolval_t s_hal_eth_initted_is(void)
{
    return((NULL == s_hal_mac) ? (hal_false) : (hal_true));
}



// changed NUM_RX_BUF with hal_base_hid_params.eth_dmarxbuffer_num
static void rx_descr_init (void) {
  /* Initialize Receive DMA Descriptor array. */
  U32 i,next;

  RxBufIndex = 0;
  for (i = 0, next = 0; i < hal_base_hid_params.eth_dmarxbuffer_num; i++) {
    if (++next == hal_base_hid_params.eth_dmarxbuffer_num) next = 0;
    Rx_Desc[i].Stat = DMA_RX_OWN;
    Rx_Desc[i].Ctrl = DMA_RX_RCH | ETH_BUF_SIZE;
    Rx_Desc[i].Addr = (U32)&rx_buf[i];
    Rx_Desc[i].Next = (U32)&Rx_Desc[next];
  }
  ETH->DMARDLAR = (U32)&Rx_Desc[0];
}

// changed NUM_TX_BUF with hal_base_hid_params.eth_dmatxbuffer_num
static void tx_descr_init (void) {
  /* Initialize Transmit DMA Descriptor array. */
  U32 i,next;

  TxBufIndex = 0;
  for (i = 0, next = 0; i < hal_base_hid_params.eth_dmatxbuffer_num; i++) {
    if (++next == hal_base_hid_params.eth_dmatxbuffer_num) next = 0;
    Tx_Desc[i].CtrlStat = DMA_TX_TCH | DMA_TX_LS | DMA_TX_FS;
    Tx_Desc[i].Addr     = (U32)&tx_buf[i];
    Tx_Desc[i].Next     = (U32)&Tx_Desc[next];
  }
  ETH->DMATDLAR = (U32)&Tx_Desc[0];
}


static void s_hal_eth_rmii_init(void)
{
    
    hal_eth_hid_rmii_prepare();

    hal_eth_hid_rmii_rx_init(); 
    hal_eth_hid_rmii_tx_init();    
    
    // cannot remove hal_eth_hid_rmii_refclock_init() from here. it is unclear if it can be used also inside the hal_brdcfg_eth__phy_start()
    hal_eth_hid_rmii_refclock_init();
    
}

static void s_hal_eth_mac_reset(void)
{
    // software reset of the mac: wait until done
    // When this bit is set, the MAC DMA controller resets all MAC Subsystem internal registers
    // and logic. It is cleared automatically after the reset operation has completed in all of the core
    // clock domains. Read a 0 value in this bit before re-programming any register of the core.
    ETH->DMABMR  |= 0x00000001;
    while(ETH->DMABMR & 0x00000001); 
}


static void s_hal_eth_mac_init(const hal_eth_cfg_t *cfg, hal_eth_phymode_t phymode)
{
    // reset mac register
    s_hal_eth_mac_reset(); 
    
    // initialise mac control register
    ETH->MACCR  = 0x00008000;       // clear

#if 0
    ETH->MACCR |= MCR_FES;
    ETH->MACCR |= MCR_DM;

#else    
    if(hal_eth_speed_100 == phymode.speed)
    {
        ETH->MACCR |= MCR_FES;              // config 100mbs 
    }
    else
    {
//        ETH->MACCR &= ~MCR_FES;             // nothing needed for 10 mbps
    }
    
    if(hal_eth_mux_fullduplex == phymode.mux)
    {
        ETH->MACCR |= MCR_DM;               // config full duplex mode
    }
    else
    {
//        ETH->MACCR &= ~MCR_DM;              // nothing needed for half duplex
    }
#endif
    
    /* MAC address filter register, accept multicast packets. */
    ETH->MACFFR = MFFR_HPF | MFFR_PAM;  // config hash or perfect address filter and pass all multicast (PAM)

    /* Ethernet MAC flow control register */
    ETH->MACFCR = MFCR_ZQPD;   // xero-quanta pause disable
    
    /* Set the Ethernet MAC Address registers */
    ETH->MACA0HR = ((U32)s_hal_mac[5] <<  8) | (U32)s_hal_mac[4];
    ETH->MACA0LR = ((U32)s_hal_mac[3] << 24) | (U32)s_hal_mac[2] << 16 | ((U32)s_hal_mac[1] <<  8) | (U32)s_hal_mac[0];
   
   
    /* Initialize Tx and Rx DMA Descriptors */
    rx_descr_init ();
    tx_descr_init ();
    
    /* Flush FIFO, start DMA Tx and Rx */
    ETH->DMAOMR = DOMR_FTF | DOMR_ST | DOMR_SR;
    
    /* Enable receiver and transmiter */
    ETH->MACCR |= MCR_TE | MCR_RE;
    
    /* Reset all interrupts */
    ETH->DMASR  = 0xFFFFFFFF;
    
    /* Enable Rx and Tx interrupts. */
    ETH->DMAIER = INT_NISE | INT_AISE | INT_RBUIE | INT_RIE;

    // acemor on 31 oct 2011: added priority support for ETH_IRQn
    hal_sys_irqn_priority_set(ETH_IRQn, cfg->priority);    
}

extern void hal_eth_hid_rmii_rx_init(void)
{
#if     defined(USE_STM32F1) 
    
#if 0    
    
    AFIO->MAPR      |= (1 << 21);               // Ethernet MAC I/O remapping to: RX_DV-CRS_DV/PD8, RXD0/PD9, RXD1/PD10, RXD2/PD11, RXD3/PD12 
 
    // enable clock for port d
    RCC->APB2ENR    |= 0x00000021;
    
    // ETH_RMII_CRS_DV (PD8), ETH_RMII_RXD0 (PD9), ETH_RMII_RXD1 (PD10) ... as remapped by setting bit 21 of AFIO->MAPR
    GPIOD->CRH      &= 0xFFFFF000;              // reset pd8, pd9, pd10
    GPIOD->CRH      |= 0x00000444;              // pins configured in reset state (floating input)

#else
    hal_boolval_t eth_remap = (stm32gpio_portD == hal_brdcfg_eth__gpio_ETH_RMII_CRS_DV.port) ? (hal_true) : (hal_false); 
    // actually is true if also other things are satisfied ... 
    // if true then: RX_DV-CRS_DV->PD8, RXD0->PD9, RXD1->PD10; else: RX_DV-CRS_DV->PA7, RXD0->PC4, RXD1->PC5.
    GPIO_PinRemapConfig(GPIO_Remap_ETH, (hal_true == eth_remap) ? ENABLE : DISABLE);
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_CRS_DV, 1);
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_RXD0, 1);
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_RXD1, 1);    
#endif
    
#elif   defined(USE_STM32F4)

#if 0 
    // enable system configuration controller clock
    RCC->APB2ENR    |= (1 << 14);  
    
    // clocks port a and port c
    RCC->AHB1ENR    |= 0x00000005;
    
    // ETH_RMII_CRS_DV (PA7)
    
    GPIOA->MODER    &= ~0x0000C000;              // reset pa7
    GPIOA->MODER    |=  0x00008000;              // alternate function
    GPIOA->OTYPER   &= ~0x00000080;              // output push-pull (reset state) 
    GPIOA->OSPEEDR  |=  0x0000C000;              // slew rate as 100MHz pin (0x0000C000) or 50mhz (0x00008000)
    GPIOA->PUPDR    &= ~0x0000C000;              // no pull up, pull down 

    GPIOA->AFR[0]   &= ~0xF0000000;
    GPIOA->AFR[0]   |=  0xB0000000;              // AF11 (ethernet)
  

    // ETH_RMII_RXD0 (PC4), ETH_RMII_RXD1 (PC5)
 
    GPIOC->MODER   &= ~0x00000F00;              // reset pc4 and pc5
    GPIOC->MODER   |=  0x00000A00;              // alternate function
    GPIOC->OTYPER  &= ~0x00000030;              // output push-pull (reset state)  
    GPIOC->OSPEEDR |=  0x00000F00;              // slew rate as 100MHz pin (0x00000F00) or 50mhz (0x00000A00)
    GPIOC->PUPDR   &= ~0x00000F00;              // no pull up, pull down
    GPIOC->AFR[0]  &= ~0x00FF0000;
    GPIOC->AFR[0]  |=  0x00BB0000;              // AF11 (ethernet) 
#else
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_CRS_DV, 0);
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_RXD0, 0);
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_RXD1, 0);
#endif
#endif
}

// cambiata
extern void hal_eth_hid_rmii_tx_init(void)
{
#if     defined(USE_STM32F1) 
#if 0    
    // enable clock for port b
    RCC->APB2ENR    |= 0x00000009;
  
    //  ETH_RMII_TX_EN (PB11), ETH _RMII_TXD0 (PB12), ETH _RMII_TXD1 (PB13)
    GPIOB->CRH      &= 0xFF000FFF;              // reset pb11, pb12, pb13
    GPIOB->CRH      |= 0x00BBB000;              // output max 50mhz, alternate function output push-pull.
#else
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_TX_EN, 0);
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_TXD0, 0);
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_TXD1, 0);     
#endif    
    
#elif   defined(USE_STM32F4)

#if 0
    if(portg)
    {
        // enable system configuration controller clock
        RCC->APB2ENR    |= (1 << 14);  
        
        // clock port g
        RCC->AHB1ENR    |= 0x00000040;

        // ETH_RMII_TX_EN (PG11), ETH_RMII_TXD0 (PG13), ETH _RMII_TXD1 (PG14)
        GPIOG->MODER   &= ~0x3CC00000;              // reset pg11, pg13, pg14
        GPIOG->MODER   |=  0x28800000;              // alternate function 
        GPIOG->OTYPER  &= ~0x00006800;              // output push-pull (reset state) 
        GPIOG->OSPEEDR |=  0x3CC00000;              // slew rate as 100MHz pin (0x3CC00000) or 50mhz (0x28800000)
        GPIOG->PUPDR   &= ~0x3CC00000;              // no pull up, pull down 

        GPIOG->AFR[1]  &= ~0x0FF0F000;
        GPIOG->AFR[1]  |=  0x0BB0B000;              // AF11 (ethernet) 
    }
    else if(portb)
    {
        // enable system configuration controller clock
        RCC->APB2ENR    |= (1 << 14);  
        
        // clock port b
        RCC->AHB1ENR    |= 0x00000002;

        // ETH_RMII_TX_EN (PB11), ETH_RMII_TXD0 (PB12), ETH _RMII_TXD1 (PB13)
        GPIOB->MODER   &= ~0x0FC00000;              // reset pb11, pb12, pb13
        GPIOB->MODER   |=  0x0A800000;              // alternate function 
        GPIOB->OTYPER  &= ~0x00003800;              // output push-pull (reset state) 
        GPIOB->OSPEEDR |=  0x0FC00000;              // slew rate as 100MHz pin (0x0FC00000) or 50mhz (0x0A800000)
        GPIOB->PUPDR   &= ~0x0FC00000;              // no pull up, pull down 

        GPIOB->AFR[1]  &= ~0x00FFF000;
        GPIOB->AFR[1]  |=  0x00BBB000;              // AF11 (ethernet)        
    }
#else
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_TX_EN, 0);
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_TXD0, 0);
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_TXD1, 0);   
#endif
#endif
}
 
// mode 1 is input, mode 0 is output
static int8_t s_hal_eth_gpioeth_init(stm32gpio_gpio_t gpio, uint8_t mode)
{  
#if     defined(USE_STM32F1) 
    static const GPIO_InitTypeDef ethgpioinit = 
    {
        .GPIO_Pin       = GPIO_Pin_All,
        .GPIO_Speed     = GPIO_Speed_50MHz,
        .GPIO_Mode      = GPIO_Mode_AF_PP,        
    };
    
    GPIO_InitTypeDef gpioinit;
    
    // port clock with alternate function
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | stm32gpio_hid_thegpioclocks[(uint8_t)gpio.port], ENABLE);
    
    // config port-pin
    memcpy(&gpioinit, &ethgpioinit, sizeof(GPIO_InitTypeDef));
    gpioinit.GPIO_Pin = stm32gpio_hid_thepins[(uint8_t)gpio.pin];
    if(1 == mode)
    {
        gpioinit.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
    GPIO_Init(stm32gpio_hid_thegpioports[(uint8_t)gpio.port], &gpioinit);

    return(0);    

#elif   defined(USE_STM32F4) 

  
     static const GPIO_InitTypeDef ethgpioinit = 
     {   // safe one
         .GPIO_Pin       = GPIO_Pin_All,
         .GPIO_Mode      = GPIO_Mode_AF,
         .GPIO_Speed     = GPIO_Speed_100MHz,
         .GPIO_OType     = GPIO_OType_PP,
         .GPIO_PuPd      = GPIO_PuPd_NOPULL    
     };
    
    
    GPIO_InitTypeDef gpioinit;
    
    // sys clock for af
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
    
    // port clock
    RCC_AHB1PeriphClockCmd(stm32gpio_hid_thegpioclocks[(uint8_t)gpio.port], ENABLE);
    
    // config port-pin
    memcpy(&gpioinit, &ethgpioinit, sizeof(GPIO_InitTypeDef));
    gpioinit.GPIO_Pin = stm32gpio_hid_thepins[(uint8_t)gpio.pin];
    GPIO_Init(stm32gpio_hid_thegpioports[(uint8_t)gpio.port], &gpioinit);

    // config af-eth
    GPIO_PinAFConfig(stm32gpio_hid_thegpioports[(uint8_t)gpio.port], stm32gpio_hid_thepinnums[(uint8_t)gpio.pin], GPIO_AF_ETH);

    return(0);
#endif    
}

extern void hal_eth_hid_smi_init(void)
{
#if     defined(USE_STM32F1) 

#if 0    
    // 0. clocks port a and port c as alternate functions   
    RCC->APB2ENR    |= 0x00000015;

    // 1. MDC:          configure Port C ethernet pin: PC1-->MDC (Ethernet MIIM interface clock)
    GPIOC->CRL      &= 0xFFFFFF0F;              // reset pc1
    GPIOC->CRL      |= 0x000000B0;              // output max 50mhz, alternate function output push-pull.   
    
    // 2. MDIO:         configure Port A ethernet pin: PA2-->MDIO
    GPIOA->CRL      &= 0xFFFFF0FF;              // reset pa2
    GPIOA->CRL      |= 0x00000B00;              // output max 50mhz, alternate function output push-pull.      
    
    // MDC Clock range: 60-72MHz. MDC = Management data clock. (RMII signal)
    ETH->MACMIIAR   = 0x00000000;
#else
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_MDC, 0);
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_MDIO, 0);
    // MDC Clock range: 60-72MHz. MDC = Management data clock. (RMII signal)
    ETH->MACMIIAR   = 0x00000000;
#endif    
#elif   defined(USE_STM32F4) 
  
#if 0 
    // enable system configuration controller clock
    RCC->APB2ENR    |= (1 << 14);  
    
    // 0. clocks port a and port c
    RCC->AHB1ENR    |= 0x00000005;
    
    // 1. MDC:              PC1 -> ETH_MDC
    GPIOC->MODER    &= ~0x0000000C;             // reset pc1
    GPIOC->MODER    |=  0x00000008;             // alternate function
    GPIOC->OTYPER   &= ~0x00000002;             // output push-pull (reset state) 
    GPIOC->OSPEEDR  |=  0x0000000C;             // slew rate as 100MHz pin (0x0000000C) or 50mhz (0x00000008)
    GPIOC->PUPDR    &= ~0x0000000C;             // no pull up, pull down
    
    GPIOC->AFR[0]   &= ~0x000000F0;
    GPIOC->AFR[0]   |=  0x000000B0;             // AF11 (ethernet)
    
    
    // 2. MDIO:             PA2 -> ETH_MDIO 
    GPIOA->MODER    &= ~0x00000030;             // reset pa2
    GPIOA->MODER    |=  0x00000020;             // alternate function
    GPIOA->OTYPER   &= ~0x00000004;             // output push-pull (reset state) 
    GPIOA->OSPEEDR  |=  0x00000030;             // slew rate as 100MHz pin (0x00000030) or 50mhz (0x00000020)
    GPIOA->PUPDR    &= ~0x00000030;             // no pull up, pull down

    GPIOA->AFR[0]   &= ~0x00000F00;
    GPIOA->AFR[0]   |=  0x00000B00;             // AF11 (ethernet)
    
    // 3. MDC clock range:  bits 4:2 CR: clock range. value 1 -> HCLK = 100-168 MHz, MDC Clock = HCLK/62
    ETH->MACMIIAR   = 0x00000004; 
#else   
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_MDC, 0);
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_MDIO, 0);    
    // MDC clock range:  bits 4:2 CR: clock range. value 1 -> HCLK = 100-168 MHz, MDC Clock = HCLK/62
    ETH->MACMIIAR   = 0x00000004; 
#endif    
   
#endif    
}

// reads the 16 bits of register REGaddr in the physical with address PHYaddr. both REGaddr and PHYaddr range is 0-31
extern uint16_t hal_eth_hid_smi_read(uint8_t PHYaddr, uint8_t REGaddr)
{
    uint32_t tout;
    
    ETH->MACMIIAR = ((PHYaddr & 0x1F) << 11) | ((REGaddr & 0x1F) << 6) | MMAR_MB;
    
    // wait until operation is completed 
    for(tout=0; tout<MII_RD_TOUT; tout++)
    {
        if(0 == (ETH->MACMIIAR & MMAR_MB))
        {
            break;
        }
    }
    return(ETH->MACMIIDR & MMDR_MD);
}

// writes the 16 bits of value in register REGaddr in the physical with address PHYaddr. both REGaddr and PHYaddr range is 0-31
extern void hal_eth_hid_smi_write(uint8_t PHYaddr, uint8_t REGaddr, uint16_t value)
{
    uint32_t tout;
    
    ETH->MACMIIDR = value;
    ETH->MACMIIAR = ((PHYaddr & 0x1F) << 11) | ((REGaddr & 0x1F) << 6) | MMAR_MW | MMAR_MB;
    
    // wait until operation is completed
    for(tout=0; tout < MII_WR_TOUT; tout++)
    {
        if(0 == (ETH->MACMIIAR & MMAR_MB))
        {
            break;
        }
    }
}


#warning --> shall we use it for the phy as well ???
extern void hal_eth_hid_rmii_refclock_init(void)
{   // used by mac but also by external phy or switch
#if     defined(USE_STM32F1) 

#if 0    
    // clock gpioa as alternate function
    RCC->APB2ENR    |= 0x00000005;
    
    // init the ETH_RMII_REF_CLK (PA1)
    GPIOA->CRL      &= 0xFFFFFF0F;              // reset pa1
    GPIOA->CRL      |= 0x00000040;              // pin configured in reset state (floating input)
#else
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_REF_CLK, 1);
#endif
    
#elif   defined(USE_STM32F4) 

#if 0        
    // enable system configuration controller clock
    RCC->APB2ENR    |= (1 << 14);      
    
    // enable GPIOA clock
    RCC->AHB1ENR    |= 0x00000001;    

    // init the ETH_RMII_REF_CLK (PA1)
    GPIOA->MODER    &= ~0x0000000C;              // reset pa1
    GPIOA->MODER    |=  0x00000008;              // alternate function
    GPIOA->OTYPER   &= ~0x00000002;              // output push-pull (reset state) 
    GPIOA->OSPEEDR  |=  0x0000000C;              // slew rate as 100MHz pin (0x0000000C) or 50mhz (0x00000008)
    GPIOA->PUPDR    &= ~0x0000000C;              // no pull up, pull down

    GPIOA->AFR[0]   &= ~0x000000F0;
    GPIOA->AFR[0]   |=  0x000000B0;              // AF11 (ethernet)    

#else
    s_hal_eth_gpioeth_init(hal_brdcfg_eth__gpio_ETH_RMII_REF_CLK, 0);
#endif

#endif    
}


extern void hal_eth_hid_rmii_prepare(void)
{
#if     defined(USE_STM32F1)    
    // step 1.
    // reset Ethernet MAC
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_ETH_MAC, ENABLE);           // or -> RCC->AHBRSTR    |= 0x00004000;              // put ethernet mac in reset mode
    // no need to do anything in here as in the stm32f4x
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_ETH_MAC, DISABLE);          // or -> RCC->AHBRSTR    &=~0x00004000;              // remove ethernet mac from reset mode
   
    // enable RMII and remap rx pins:
    GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);    // or -> AFIO->MAPR      |= (1 << 23);               // impose rmii     

    // enable clocks for ethernet (RX, TX, MAC)
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx | RCC_AHBPeriph_ETH_MAC_Rx, ENABLE); // or -> RCC->AHBENR     |= 0x0001C000;
    
#elif   defined(USE_STM32F4)

    // step 1. enable system configuration controller clock
    RCC->APB2ENR    |= (1 << 14);  
    
    // step 2. in strict order: reset ethernet mac, set rmii (or mii) on register syscfg_pmc. only after that enable the mac clocks 
    //         see reference manual of stm32f4 in section of SYSCFG_PMC register.     
    RCC->AHB1RSTR   |=  (1 << 25);              // put ethernet mac in reset mode
    SYSCFG->PMC     |=  (1 << 23);              // imposet rmii {if one wanted mii and not rmii: SYSCFG->PMC &= ~(1 << 23);}
    RCC->AHB1RSTR   &= ~(1 << 25);              // remove ethernet mac from reset mode
    
    // step 3. enable clocks for ethernet (RX, TX, MAC) 
    RCC->AHB1ENR |= 0x0E000000;

#endif    
}



#endif//HAL_USE_ETH



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



