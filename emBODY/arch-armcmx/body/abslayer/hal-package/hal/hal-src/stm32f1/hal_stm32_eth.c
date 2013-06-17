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
#include "stm32f1.h"
#include "string.h"
#include "hal_stm32_base_hid.h" 
#include "hal_stm32_sys_hid.h"
#include "hal_brdcfg.h"
#include "hal_switch.h"

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

// from NET_CONFIG.H       
//#define ETH_MTU         1514      /* Ethernet Frame Max Transfer Unit        */
//#define     STM3210C 1

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
/* This variable can contain a ptr to a function that want to use a eth raw pkt (with ethernet and udp/tcp headers, etc)
   For example this function can be a parser to check a particular info in a pkt, like sequence number
   In oreder to use this variable, an external program have to declare hal_eth_lowLevelUsePacket_ptr like external and set
   to the ptr of application's function.
*/
void (*hal_eth_lowLevelUsePacket_ptr)(uint8_t* pkt_ptr, uint32_t size) = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef uint8_t     U8;
typedef uint16_t    U16;
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

// keep teh same names of the tcpnet driver, without s_ prefix
static void rx_descr_init (void);
static void tx_descr_init (void);
//static void write_PHY (uint16_t PHYAddr, uint16_t PhyReg, uint16_t value);
//static uint16_t read_PHY (uint16_t PHYAddr, uint16_t PhyReg);
static void s_hal_eth_RCC_conf(void);
//static hal_result_t s_hal_eth_phyStatus_checkAndWait(uint16_t PHYAddr, uint16_t PhyReg, uint16_t checkMask);

static void s_hal_eth_rmii_init(void);

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
    /* Initialize the ETH ethernet controller. */
//    uint32_t regv;
//    hal_result_t res;
//    uint8_t set_full_mode =1;
//    uint8_t set_100mb = 1;
//    uint8_t i;
//    uint8_t all_links_are_down = 1;
//    uint8_t phy_link_is_up[HAL_BRDCFG_ETH__PHY_DEVICE_NUM];


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

    // this code is board-dependent
    hal_brdcfg_eth__phy_start(); 


    // code from here is common to every board
    s_hal_eth_rmii_init();

#if 0
// -----------------------------------------------------------------------------------------------------------

/*Valentina: 
ho eliminato la possibilita' di configurare i link a 10MBIT  o a 100MB o auto negoziare
in quanto questa configurazione poteva essere fatta solo a compile time definendo una macro che
ti faceva scegliere una delle 3 opzioni.
Fino ad oggi non usavamo nessuna macro quindi applicavamo la autonegoziazione.
Ma visti i problemi incontrati nella configurazione dello switch 
conviene configurare di default tutti i link a 100MB.
Ho lasciato comunque il codice per non buttarlo via...
*/


    /* Configure all PHY devices */
    #if defined (_10MBIT_)
    
    for(i=0; i< PHY_DEVICE_NUM; i++)
    {
        write_PHY (hal_brdcfg_eth__phy_device_list[i], PHY_REG_BMCR, PHY_FULLD_10M); /* Connect at 10MBit */
    }
    /* Note: I must not set any bit in ethernet register, 
       because in order to configure 10 Mbit FES bit in eTH register must be  equal to zero.  */
    
    #elif defined (_100MBIT_)
    
    for(i=0; i< HAL_BRDCFG_ETH__PHY_DEVICE_NUM; i++)
    {
        write_PHY (hal_brdcfg_eth__phy_device_list[i], PHY_REG_BMCR, PHY_FULLD_100M); /* Connect at 100MBit */
    }

    ETH->MACCR |= MCR_FES;

    #else
    /* Use autonegotiation about the link speed. */
 
    for(i=0; i< HAL_BRDCFG_ETH__PHY_DEVICE_NUM; i++)
    {
        write_PHY (hal_brdcfg_eth__phy_device_list[i], PHY_REG_BMCR, PHY_AUTO_NEG);
        res = s_hal_eth_phyStatus_checkAndWait(hal_brdcfg_eth__phy_device_list[i], PHY_REG_BMSR, HAL_ETH_AUTONEG_MASK);
        if (hal_res_OK != res)
        {
            //if autonegotiantion isn't completed then force link to 100Mb
            write_PHY (hal_brdcfg_eth__phy_device_list[i], PHY_REG_BMCR, PHY_FULLD_100M);
            //hal_on_fatalerror(hal_error_eth_cfgincorrect, "Autonegotiation incomplete\n");
            //return; //TODO: specify type of error: Autonegotiation incomplete.
        }
    }


    /* Verify link status*/
    for(i=0; i< HAL_BRDCFG_ETH__PHY_DEVICE_NUM; i++)
    {
        res = s_hal_eth_phyStatus_checkAndWait(hal_brdcfg_eth__phy_device_list[i], PHY_REG_BMSR, HAL_ETH_LINK_UP_MASK);
        if (hal_res_OK == res)
        {
            all_links_are_down &= 0x0;
            phy_link_is_up[i] = 1;
            
        }
        else
        {
             phy_link_is_up[i] = 0;
        }
    }

    /* Initialize MAC control register */
    
    
    ETH->MACCR  = MCR_ROD;  /* Receive own disable: the MAC disables the reception of frames in Half-duplex mode
                                This bit is not applicable if the MAC is operating in Full-duplex mode*/
    
    
    //if all links are down then configure eth with default values (100Mb and full duplex)
    //verify if all phy devices, which has link up, are configured in the same way
    if(!all_links_are_down)
    {
        i=0;
        while(i< HAL_BRDCFG_ETH__PHY_DEVICE_NUM)
        {
            if(phy_link_is_up[i])
            {
                regv = read_PHY (hal_brdcfg_eth__phy_device_list[i], PHY_REG_BMSR);
                set_full_mode = (set_full_mode && IS_FULL_MODE(regv)) ? 1 :0;
                set_100mb = (set_full_mode && IS_100MBIT_MODE(regv)) ? 1 :0;
            }
            i++;
        }
    }
    
    /* Configure Full/Half Duplex mode. */
    if (set_full_mode)
    {
        ETH->MACCR |= MCR_DM;
    }



    /* Configure 100MBit/10MBit mode. */
    
    if (set_100mb) 
    {
        ETH->MACCR |= MCR_FES;
    }

    //TODO: ci sarebbero altre cose da configurare....vedi stam_eth.c della ixxat.
    #endif

#endif
// --------------------------------------------------------------------------------------------------------------


    /* Ethernet MAC configuration register */
    ETH->MACCR |= MCR_FES;  //config 100Mb (Fast Eth)
    ETH->MACCR |= MCR_DM;   //config duple Mode

    
    
    /* MAC address filter register, accept multicast packets. */
    ETH->MACFFR = MFFR_HPF | MFFR_PAM;  // config Hash or perfect address filter and pass all multicast(PAM)

    /* Ethernet MAC flow control register */
    ETH->MACFCR = MFCR_ZQPD;   //Zero-quanta pause disable
    
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
    ETH->DMAIER = INT_NISE | INT_RIE | INT_TIE;

    // acemor on 31 oct 2011: added priority support for ETH_IRQn
    hal_sys_irqn_priority_set(ETH_IRQn, cfg->priority);


    s_hal_eth_initted_set();
    
    return(hal_res_OK);
}

extern hal_result_t hal_eth_enable(void) 
{
//     // acemor on 31 oct 2011: it is the same as NVIC_EnableIRQ(ETH_IRQn) or hal_sys_hid_irqn_enable()
//     /* Ethernet Interrupt Enable function. */
//     NVIC->ISER[1] = 1 << 29;
//     
    
    
    // for __DSB() and __ISB, see: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka13559.html
    
    //   NVIC_ClearPendingIRQ(ETH_IRQn); // shall i clear or shall i not clear? NOT CLEAR!!!
    
    NVIC_EnableIRQ(ETH_IRQn);
    __DSB();
    __ISB();
    
    // i need to force the isr if since last call of hal_eth_disable() the dma has moved a frame in the descriptor 
    // or all descriptors are owned by cpu
    if((INT_RIE == (INT_RIE & ETH->DMASR)) || (INT_RBUIE == (INT_RBUIE & ETH->DMASR)))
    {
        NVIC_SetPendingIRQ(ETH_IRQn);
    }    

    return(hal_res_OK);
}


extern hal_result_t hal_eth_disable(void) 
{
//     // acemor on 31 oct 2011: it is the same as NVIC_DisableIRQ(ETH_IRQn) or hal_sys_hid_irqn_disable()
//     /* Ethernet Interrupt Disable function. */
//     NVIC->ICER[1] = 1 << 29;
//     
    
    // for __DSB() and __ISB, see: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka13559.html
    
    NVIC_DisableIRQ(ETH_IRQn);
    __DSB();
    __ISB();

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



extern hal_result_t hal_eth_check_links(uint8_t *linkst_mask, uint8_t *links_num)
{
    return(hal_brdcfg_eth__check_links(linkst_mask, links_num));
}


extern hal_result_t hal_eth_get_links_status(hal_eth_phy_status_t** link_list, uint8_t *links_num)
{
    return(hal_brdcfg_eth__get_links_status(link_list, links_num));
}

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

// ---- isr of the module: begin ----

void ETH_IRQHandler (void) {
  /* Ethernet Controller Interrupt function. */
//  OS_FRAME *frame; removed
  U32 i,RxLen;
  U32 *sp,*dp;
  //added 
  hal_eth_frame_t *frame; 
  uint32_t size = 0;    
    

#if defined(HAL_USE_EVENTVIEWER_ETH)
  evEntityId_t prev = eventviewer_switch_to(ev_ID_first_isr+hal_arch_arm_ETH_IRQn);
#endif
  

  i = RxBufIndex;
  do {
    /* Valid frame has been received. */
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

//====> following code is sostitute with .....
//     /* Flag 0x80000000 to skip sys_error() call when out of memory. */
//     frame = alloc_mem (RxLen | 0x80000000);
//     /* if 'alloc_mem()' has failed, ignore this packet. */
//     if (frame != NULL) {
//       sp = (U32 *)(Rx_Desc[i].Addr & ~3);
//       dp = (U32 *)&frame->data[0];
//       for (RxLen = (RxLen + 3) >> 2; RxLen; RxLen--) {
//         *dp++ = *sp++;
//       }
//       put_in_queue (frame);
//     }
///====> .... this code
      /* Flag 0x80000000 to skip sys_error() call when out of memory. */
      frame = s_onframerx.frame_new(RxLen | 0x80000000);
      size = (RxLen | 0x80000000);
      /* if 'alloc_mem()' has failed, ignore this packet. */
      if (frame != NULL) {
        sp = (U32 *)(Rx_Desc[i].Addr & ~3);
        dp = (U32 *)&frame->datafirstbyte[0];
        for (RxLen = (RxLen + 3) >> 2; RxLen; RxLen--) {
          *dp++ = *sp++;
        }
                
        
        if(hal_eth_lowLevelUsePacket_ptr != NULL)
        {
            hal_eth_lowLevelUsePacket_ptr(&frame->datafirstbyte[0], size);
        }
        
        //put_in_queue (frame);
        s_onframerx.frame_movetohigherlayer(frame);
        if(NULL != s_onframerx.frame_alerthigherlayer)
        {
            s_onframerx.frame_alerthigherlayer();
        }
    }
//     else
//     {
//         //error: memory is missing!!
//     }

        /* Release this frame from ETH IO buffer. */
rel:Rx_Desc[i].Stat = DMA_RX_OWN;

    if (++i == hal_base_hid_params.eth_dmarxbuffer_num) i = 0;
  } while ((Rx_Desc[i].Stat & DMA_RX_OWN) == 0);
  RxBufIndex = i;

  if (ETH->DMASR & INT_RBUIE) {
    /* Rx DMA suspended, resume DMA reception. */
    ETH->DMASR   = INT_RBUIE;
    ETH->DMARPDR = 0;
  }
  /* Clear the interrupt pending bits. */
  ETH->DMASR = INT_NISE | INT_RIE;


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

extern const hal_eth_network_functions_t * hal_eth_get_network_functions(void)
{
    return(&s_hal_eth_fns);
}

extern void hal_eth_hid_smi_write(uint16_t PHYAddr, uint16_t PhyReg, uint16_t value)
{
    write_PHY (PHYAddr, PhyReg, value);
}

extern uint16_t hal_eth_hid_smi_read(uint16_t PHYAddr, uint16_t PhyReg)
{
    return(read_PHY (PHYAddr, PhyReg));
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

/**
  * @brief  writes @value in @PhyReg of physical addressed by @PHYAddr and
  *         waits until operation is completed for max MII_WR_TOUT.
  * @param      PHYAddr: address of physical
  *             PhyReg: address of physical's register
  *             value: value to write
  * @retval none
  */
extern void write_PHY (uint16_t PHYAddr, uint16_t PhyReg, uint16_t value)
{
    U32 tout;
    
    ETH->MACMIIDR = value;
    ETH->MACMIIAR = PHYAddr<< 11 | PhyReg << 6 | MMAR_MW | MMAR_MB;
    
    /* Wait utill operation completed */
    tout = 0;
    for (tout = 0; tout < MII_WR_TOUT; tout++)
    {
        if ((ETH->MACMIIAR & MMAR_MB) == 0)
        {
            break;
        }
    }
}

/**
  * @brief  reads @PhyReg values of physical addressed by @PHYAddr and
  *         waits until operation is completed for max MII_RD_TOUT.
  * @param      PHYAddr: address of physical
  *             PhyReg: address of physical's register
  *             value: value to write
  * @retval  @PhyReg values
  */
extern uint16_t read_PHY (uint16_t PHYAddr, uint16_t PhyReg)
{
    uint32_t tout;
    
    ETH->MACMIIAR = PHYAddr << 11 | PhyReg << 6 | MMAR_MB;
    
    /* Wait until operation completed */
    tout = 0;
    for (tout = 0; tout < MII_RD_TOUT; tout++)
    {
        if ((ETH->MACMIIAR & MMAR_MB) == 0)
        {
            break;
        }
    }
    return (ETH->MACMIIDR & MMDR_MD);
}


static void s_hal_eth_RCC_conf(void)
{

  /* Reset Ethernet MAC */
  RCC->AHBRSTR  |= 0x00004000;
  RCC->AHBRSTR  &=~0x00004000;

  /* Since eth uses pin on ports A,B,C,D then enable clock on these ports */
  RCC->APB2ENR |= 0x0000003D;

  /* Enable clock for MAC, MAC-RX and MAC-TX. */
  RCC->AHBENR  |= 0x0001C000;

}

#if 0
/**
  * @brief  checks @checkMask bitmsk is true in if @PhyReg values 
  *         of physical addressed by @PHYAddr  and
  *         waits until operation is completed for max TIMEOUT.
  * @param      PHYAddr: address of physical
  *             PhyReg: address of physical's register
  *             checkMask: bit mask
  * @retval  @PhyReg values
  */
static hal_result_t s_hal_eth_phyStatus_checkAndWait(uint16_t PHYAddr, uint16_t PhyReg, uint16_t checkMask)
{            
    uint32_t tout, regv;

    tout = 0; //reset timeout count;
    do
    {
      tout++;
      regv = read_PHY (PHYAddr, PhyReg);

    }
    while( (tout<HAL_ETH_PHY_WR_TIMEOUT) && (!(regv & checkMask)) );

    if(HAL_ETH_PHY_WR_TIMEOUT == tout)
    {
        return (hal_res_NOK_generic);
    }
    else
    {
        return(hal_res_OK);
    }


}
#endif

static void s_hal_eth_rmii_init(void)
{
/*
    Note: Ref_clock pin (A1) in initialized in  hal_brdcfg_eth__phy_start, 
    even if it belongs to rmii port.
    Its initilization must be do in phy init to provide clock to phy;
    so, in case of switch, it can work even if eth is not initilized.
*/

    /* Enable clock for Port A,B,C,D and MAC */
    s_hal_eth_RCC_conf();   //DA PROBLEMI SE E' GIA ABILITATO???

    
    /* Enable RMII and remap following pins:
    RX_DV-CRS_DV--> PD8,  RDX0-->PD9 RXD1-->PD10, RDX2-->PD11, RXD3-->PD12 */
    AFIO->MAPR   |= 0x00A00000;
    
    /* these pins must be configured in this way on all boards.
    They dipend on micro. (see table 176 of datasheet)*/
    
    /* Configure Port A ethernet pin: PA2-->MDIO */
    GPIOA->CRL   &= 0xFFFFF0FF;
    GPIOA->CRL   |= 0x00000B00;  
  
    /* Configure Port B ethernet pins: PB11 -->TXEN, PB12-->TX0, PB13-->TX1 */
    GPIOB->CRH   &= 0xFF000FFF;
    GPIOB->CRH   |= 0x00BBB000;    /*All pins are congigured in AlterFunc 50MHz*/

  
    /* Configure Port C ethernet pin: PC1-->MDC (Ethernet MIIM interface clock)*/
    GPIOC->CRL   &= 0xFFFFFF0F;
    GPIOC->CRL   |= 0x000000B0;    /*Pin 1 is congigured in AlterFunc 50MHz*/ 
         
    /* Configure Port D ethernet pins (RXD0, RXD1 AND CRSDV=CARRIER SENSE DATA VALID) */
    GPIOD->CRH   &= 0xFFFFF000;
    GPIOD->CRH   |= 0x00000444;     /*All pins are congigured reset state*/

    /* Software Reset DMA */
    ETH->DMABMR  |= DBMR_SR;
    while (ETH->DMABMR & DBMR_SR);
    
    /* Set MDC Clock range 60-72MHz. 
    MDC = Management data clock. (RMII signal)*/
    ETH->MACMIIAR = 0x00000000;

}

#endif//HAL_USE_ETH

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



