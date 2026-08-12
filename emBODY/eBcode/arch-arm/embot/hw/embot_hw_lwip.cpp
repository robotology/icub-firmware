

/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_lwip.h"

#if 0

needs:
- netif_set_up()
- netif_set_link_up()
- opaque type netIF used only through it pointer
- must provide a err_t low_level_output(struct netif *netif, struct pbuf *p) plus complete use of its types
- void low_level_init(struct netif *netif) etc

- st functions

it must include the lwip/ header ....


#endif


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "lwip/init.h"
#include "netif/etharp.h"
#include "lwip/timeouts.h"
#include "lwip/udp.h"
#include "lwip/pbuf.h"

//#include "ethram.h"

#include "stm32hal.h"

#include "embot_hw_eth.h"
#include "embot_hw_eth_bsp.h"

// --------------------------------------------------------------------------------------------------------------------
// - some checks
// --------------------------------------------------------------------------------------------------------------------

static_assert(std::string_view(STM32HAL_STM32_FAMILY) == std::string_view("STM32H7"), "must be an STM32H7 board");
static_assert((STM32HAL_DRIVER_VERSION >= 0x1B5), "the stm32 hal must be 0x1B5 or higher");
static_assert((LWIP_VERSION >= 0x020201ff), "the lwip must be 2.2.1.255 or higher");
static_assert(std::string_view(LWIP_VERSION_STRING) == std::string_view("2.2.1"), "lwip version must be exactly 2.2.1");




// --------------------------------------------------------------------------------------------------------------------
// - eth ram
// --------------------------------------------------------------------------------------------------------------------


// 
// declaration part that could end un in a ethram.h file if we wanted
//

constexpr size_t ETH_RX_BUFFER_SIZE {1524};                 // so that can accomodate .
constexpr size_t ETH_RX_BUFFER_CNT {2*ETH_RX_DESC_CNT};     // the rx descr cnt is typically 4 but can be increased in stm32hal

//#define ETH_RX_BUFFER_SIZE            1000U
//#define ETH_RX_BUFFER_CNT               (2*ETH_RX_DESC_CNT)
//#define ETH_RX_BUFFER_CNT             8U
//#define ETH_TX_BUFFER_MAX             ((ETH_TX_DESC_CNT) * 2U)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/*
@Note: This interface is implemented to operate in zero-copy mode only:
        - Rx Buffers will be allocated from LwIP stack Rx memory pool,
          then passed to ETH HAL driver.
        - Tx Buffers will be allocated from LwIP stack memory heap,
          then passed to ETH HAL driver.

@Notes:
  1.a. ETH DMA Rx descriptors must be contiguous, the default count is 4,
       to customize it please redefine ETH_RX_DESC_CNT in ETH GUI (Rx Descriptor Length)
       so that updated value will be generated in stm32xxxx_hal_conf.h
  1.b. ETH DMA Tx descriptors must be contiguous, the default count is 4,
       to customize it please redefine ETH_TX_DESC_CNT in ETH GUI (Tx Descriptor Length)
       so that updated value will be generated in stm32xxxx_hal_conf.h

  2.a. Rx Buffers number must be between ETH_RX_DESC_CNT and 2*ETH_RX_DESC_CNT
  2.b. Rx Buffers must have the same size: ETH_RX_BUFFER_SIZE, this value must
       passed to ETH DMA in the init field (heth.Init.RxBuffLen)
  2.c  The RX Ruffers addresses and sizes must be properly defined to be aligned
       to L1-CACHE line size (32 bytes).
*/


typedef struct
{
  struct pbuf_custom pbuf_custom;
  uint8_t buff[(ETH_RX_BUFFER_SIZE + 31) & ~31] __ALIGNED(32);
} RxBuff_t;
// con 1000 ho sizeof() = 1056, con 1524 (max 1518 sul filo, max 1500 di IP) ho sizeof() = 1536   
    

#if defined ( __ICCARM__ ) /*!< IAR Compiler */

#pragma location=0x30000000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30000080
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */


#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((section(".RxDescripSection"))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((section(".TxDescripSection"))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif (defined ( __GNUC__ ) || defined ( __ARMCC_VERSION )) /* GNU Compiler */

__attribute__((section(".RxDescripSection"))) extern ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((section(".TxDescripSection"))) extern ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT];   /* Ethernet Tx DMA Descriptors */

#endif


#if defined ( __ICCARM__ )
#pragma location = 0x30000100
extern u8_t memp_memory_RX_POOL_base[];
#elif defined ( __CC_ARM )
__attribute__((section(".Rx_PoolSection"))) extern u8_t memp_memory_RX_POOL_base[];
#elif defined ( __ARMCC_VERSION )
__attribute__((section(".Rx_PoolSection"))) extern uint8_t memp_memory_RX_POOL_base[];
#elif defined ( __GNUC__ )
__attribute__((section(".Rx_PoolSection"))) extern u8_t memp_memory_RX_POOL_base[];
#endif

extern const struct memp_desc memp_RX_POOL;


//
// definition part that could end up in a ethram.cpp file if we wanted
//


#if defined ( __ICCARM__ ) /*!< IAR Compiler */

#pragma location=0x30000000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30000080
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */


#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((section(".RxDescripSection"))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((section(".TxDescripSection"))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */

#elif (defined ( __GNUC__ ) || defined ( __ARMCC_VERSION )) /* GNU Compiler */

__attribute__((section(".RxDescripSection"))) ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((section(".TxDescripSection"))) ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT];   /* Ethernet Tx DMA Descriptors */

#endif

// sizeof(ETH_DMADescTypeDef) = 24, 24*ETH_RX_DESC_CNT = 24*4 = 96. 

/* Memory Pool Declaration */
LWIP_MEMPOOL_DECLARE(RX_POOL, ETH_RX_BUFFER_CNT, sizeof(RxBuff_t), "Zero-copy RX PBUF pool");



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


constexpr uint32_t ETH_DMA_TRANSMIT_TIMEOUT {20};

enum class RxAllocStatus
{
    OK       = 0,
    ERROR    = 1
};

namespace embot::hw::lwip::data {
    
    embot::net::eth::IPconfig ipConfig {};        
    RxAllocStatus rxallocstatus {RxAllocStatus::OK};            
    netIF gnetif {};         
    embot::core::Callback onRXethframe {}; 
                   
}


namespace embot::hw::lwip::stm32 {
    
    // very important: the ETH_HandleTypeDef is typically owned by the ETH BSP,
    // but it is used almost in here. so in here we keep its pointer
    // and w/ the const i make sure it cannot be changed
    
    ETH_HandleTypeDef * const pEthHandle = {embot::hw::eth::bsp::getBSP().getPROP(embot::hw::EtH::one)->handle};   
    
    ETH_HandleTypeDef * getETHhandle() { return embot::hw::eth::bsp::getBSP().getPROP(embot::hw::EtH::one)->handle; }
    
    HAL_StatusTypeDef ETH_Init()
    { 
        static bool initted {false};

        if(initted)
        {
            return HAL_OK;
        }
        
        // EthHandle.Init.MACAddr is a pointer to a static arry of 6 bytes, ordered w/ the oui0 in lsb
        // so, 00:1A:2B:3C:4D:5E is inside a arry[6] = {0x00, 0x1A, 0x2B, 0x3C, 0x4D, 0x5E}
        pEthHandle->Instance = ETH;
        pEthHandle->Init.MACAddr = embot::hw::lwip::data::ipConfig.macaddr.v;
        pEthHandle->Init.MediaInterface = HAL_ETH_RMII_MODE;
        pEthHandle->Init.RxDesc = DMARxDscrTab;
        pEthHandle->Init.TxDesc = DMATxDscrTab;
        pEthHandle->Init.RxBuffLen = ETH_RX_BUFFER_SIZE;
        
        // embot::hw::lwip::get(dmarxdes, dmatxdes, ethrxbufsize); e questo modulo li possiede. 

        // configure ethernet peripheral (GPIOs, clocks, MAC, DMA)
        HAL_StatusTypeDef r = HAL_ETH_Init(pEthHandle);   

        initted = (HAL_OK == r); 
        
        return r;
    }

    
    void ethernet_link_check_state(netIF *netif)
    {
        ETH_MACConfigTypeDef MACConf = {0};
        //      int32_t PHYLinkState = 0U;
        uint32_t linkchanged = 0U, speed = 0U, duplex =0U;


        static uint8_t first = 1;

        if(1 == first)
        {
            linkchanged = 1;
            first = 0;
        }
              
        
        if(linkchanged)
        {
            duplex = ETH_FULLDUPLEX_MODE;
            speed = ETH_SPEED_100M;

            
            HAL_ETH_GetMACConfig(pEthHandle, &MACConf);
            MACConf.DuplexMode = duplex;
            MACConf.Speed = speed;
            HAL_ETH_SetMACConfig(pEthHandle, &MACConf);
            HAL_ETH_Start_IT(pEthHandle);  
            netif_set_up(netif);
            netif_set_link_up(netif);
        }

        return;

//    #else


//      PHYLinkState = LAN8742_GetLinkState(&LAN8742);

//      if(netif_is_link_up(netif) && (PHYLinkState <= LAN8742_STATUS_LINK_DOWN))
//      {
//        HAL_ETH_Stop(&EthHandle);
//        netif_set_down(netif);
//        netif_set_link_down(netif);
//      }
//      else if(!netif_is_link_up(netif) && (PHYLinkState > LAN8742_STATUS_LINK_DOWN))
//      {
//        switch (PHYLinkState)
//        {
//        case LAN8742_STATUS_100MBITS_FULLDUPLEX:
//          duplex = ETH_FULLDUPLEX_MODE;
//          speed = ETH_SPEED_100M;
//          linkchanged = 1;
//          break;
//        case LAN8742_STATUS_100MBITS_HALFDUPLEX:
//          duplex = ETH_HALFDUPLEX_MODE;
//          speed = ETH_SPEED_100M;
//          linkchanged = 1;
//          break;
//        case LAN8742_STATUS_10MBITS_FULLDUPLEX:
//          duplex = ETH_FULLDUPLEX_MODE;
//          speed = ETH_SPEED_10M;
//          linkchanged = 1;
//          break;
//        case LAN8742_STATUS_10MBITS_HALFDUPLEX:
//          duplex = ETH_HALFDUPLEX_MODE;
//          speed = ETH_SPEED_10M;
//          linkchanged = 1;
//          break;
//        default:
//          break;
//        }
//     
//        if(linkchanged)
//        {
//          /* Get MAC Config MAC */
//          HAL_ETH_GetMACConfig(&EthHandle, &MACConf);
//          MACConf.DuplexMode = duplex;
//          MACConf.Speed = speed;
//          HAL_ETH_SetMACConfig(&EthHandle, &MACConf);
//          //HAL_ETH_Start(&EthHandle);
//          HAL_ETH_Start_IT(&EthHandle);  
//          netif_set_up(netif);
//          netif_set_link_up(netif);
//        }
//      }
//    #endif 

    }      
    
    err_t low_level_output(struct netif *netif, struct pbuf *p)
    {
        netif->state = netif->state;
    
        uint32_t i = 0U;
        struct pbuf *q = NULL;
        err_t errval = ERR_OK;
        ETH_BufferTypeDef Txbuffer[ETH_TX_DESC_CNT] = {0};
        ETH_TxPacketConfigTypeDef TxConfig;

        memset(Txbuffer, 0 , ETH_TX_DESC_CNT*sizeof(ETH_BufferTypeDef));

        /* Set Tx packet config common parameters */
        memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
        TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
        TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
        TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;

        for(q = p; q != NULL; q = q->next)
        {
            if(i >= ETH_TX_DESC_CNT)
            {
                return ERR_IF;
            }

            Txbuffer[i].buffer = (uint8_t *)q->payload;
            Txbuffer[i].len = q->len;

            if(i>0)
            {
                Txbuffer[i-1].next = &Txbuffer[i];
            }

            if(q->next == NULL)
            {
                Txbuffer[i].next = NULL;
            }

            i++;
        }

        TxConfig.Length = p->tot_len;
        TxConfig.TxBuffer = Txbuffer;
        TxConfig.pData = p;

        HAL_ETH_Transmit(pEthHandle, &TxConfig, ETH_DMA_TRANSMIT_TIMEOUT);

        return errval;                
    }

    
    void low_level_init(struct netif *netif)
    {        
        netif->state = netif->state;
                
        embot::hw::MACaddress mac {embot::hw::lwip::data::ipConfig.macaddr.toU64()};
        embot::hw::eth::init(embot::hw::EtH::one, mac);
        
        // ETH_Init() can be called multiuple times. it is now called inside embot::hw::eth::init(embot::hw::EtH::one, mac);
        // and also in here.
        HAL_StatusTypeDef hal_eth_init_status {HAL_OK};
        
        hal_eth_init_status = ETH_Init();
        



#if LWIP_ARP || LWIP_ETHERNET
        /* set MAC hardware address length */
        netif->hwaddr_len = ETH_HWADDR_LEN;
        
        static_assert(ETH_HWADDR_LEN == sizeof(netif->hwaddr), "");

        // set MAC hardware address in same format as for st. in here we must copy
        std::memmove(netif->hwaddr, embot::hw::lwip::data::ipConfig.macaddr.v, sizeof(netif->hwaddr));

        /* maximum transfer unit */
        netif->mtu = ETH_MAX_PAYLOAD;

        /* device capabilities */
        /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
        netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

        /* Initialize the RX POOL */
        LWIP_MEMPOOL_INIT(RX_POOL);

//#if defined(amcmj1BOARD)


    //    HAL_ETH_Start_IT(&EthHandle);

    //    eth_bsp_init(&EthHandle);

        netif_set_up(netif);
        netif_set_link_up(netif);

//#else
//    /* Set PHY IO functions */
//    LAN8742_RegisterBusIO(&LAN8742, &LAN8742_IOCtx);

//    /* Initialize the LAN8742 ETH PHY */
//    if(LAN8742_Init(&LAN8742) != LAN8742_STATUS_OK)
//    {
//    netif_set_link_down(netif);
//    netif_set_down(netif);
//    return;
//    }
//#endif

    if (hal_eth_init_status == HAL_OK)
    {
        /* Get link state */
        ethernet_link_check_state(netif);
    }
    else
    {
        //    Error_Handler();
    }
    
#endif /* LWIP_ARP || LWIP_ETHERNET */

    }
}


namespace embot::hw::lwip {
           
    err_t ethernetif_init(netIF *netif)
    {
        err_t r {ERR_OK};
        
        netif->state = netif->state;

        LWIP_ASSERT("netif != NULL", (netif != NULL));

        #if LWIP_NETIF_HOSTNAME
        /* Initialize interface hostname */
        netif->hostname = "lwip";
        #endif /* LWIP_NETIF_HOSTNAME */

        netif->name[0] = 's';
        netif->name[1] = 't';
        
        /* We directly use etharp_output() here to save a function call.
        * You can instead declare your own function an call etharp_output()
        * from it if you have to do some checks before sending (e.g. if link
        * is available...) */

        #if LWIP_IPV4
        #if LWIP_ARP || LWIP_ETHERNET
        #if LWIP_ARP
        netif->output = etharp_output;
        #else
        /* The user should write its own code in low_level_output_arp_off function */
        netif->output = low_level_output_arp_off;
        #endif /* LWIP_ARP */
        #endif /* LWIP_ARP || LWIP_ETHERNET */
        #endif /* LWIP_IPV4 */
        netif->linkoutput = embot::hw::lwip::stm32::low_level_output;

        /* initialize the hardware */
        stm32::low_level_init(netif);
        
        return r;        
    }
 
    
    void ethernet_link_status_updated(netIF *netif)
    {
        if (netif_is_link_up(netif))
        {
            #if LWIP_DHCP
            /* Update DHCP state machine */
            DHCP_state = DHCP_START;
            #else
            //    BSP_LED_On(LED2);
            //    BSP_LED_Off(LED3);
            #endif /* LWIP_DHCP */
            }
            else
            {
            #if LWIP_DHCP
            /* Update DHCP state machine */
            DHCP_state = DHCP_LINK_DOWN;
            #else
            //    BSP_LED_Off(LED2);
            //    BSP_LED_On(LED3);
            #endif /* LWIP_DHCP */
        }        
    }
  
        
    void netconfig(const embot::net::eth::IPconfig &ipconfig)
    {        
        data::ipConfig = ipconfig;
        
        ip_addr_t ipaddr    {data::ipConfig.ipaddr.value()};
        ip_addr_t netmask   {data::ipConfig.ipmask.value()};
        ip_addr_t gw        {data::ipConfig.gateway.value()};
        
        netif_add(embot::hw::lwip::network(), &ipaddr, &netmask, &gw, nullptr, &ethernetif_init, &ethernet_input);
        
        netif_set_default(embot::hw::lwip::network());
        
        ethernet_link_status_updated(embot::hw::lwip::network());
        
#if LWIP_NETIF_LINK_CALLBACK
        netif_set_link_callback(embot::hw::lwip::network(), ethernet_link_status_updated);
#endif        
    }
    

    
    void Ethernet_Link_Periodic_Handle()
    {
        static embot::core::Time prevtick {0};
        constexpr embot::core::Time delta {100*embot::core::time1millisec};
        embot::core::Time n = embot::core::now();
        
            if((n - prevtick) >= delta)
            {
                prevtick = n;
                embot::hw::lwip::stm32::ethernet_link_check_state(embot::hw::lwip::network());
            }
    }  

    
    void pbuf_free_custom(struct pbuf *p)
    {
        struct pbuf_custom* custom_pbuf = (struct pbuf_custom*)p;
        LWIP_MEMPOOL_FREE(RX_POOL, custom_pbuf);
        /* If the Rx Buffer Pool was exhausted, signal the ethernetif_input task to
         * call HAL_ETH_GetRxDataBuffer to rebuild the Rx descriptors. */
        if (data::rxallocstatus == RxAllocStatus::ERROR)
        {
            data::rxallocstatus = RxAllocStatus::OK;
        }
    }

    
    static struct pbuf * low_level_input(struct netif *netif)
    {
        netif->state = netif->state;        
        struct pbuf *p = NULL;

        if(data::rxallocstatus == RxAllocStatus::OK)
        {
            HAL_ETH_ReadData(embot::hw::lwip::stm32::pEthHandle, (void **)&p);
        }
        return p;
    }

    
    void ethernetif_input(struct netif *netif)
    {
        netif->state = netif->state;
        
        struct pbuf *p = NULL;

        do
        {
          p = low_level_input( netif );
          if (p != NULL)
          {
            if (netif->input( p, netif) != ERR_OK )
            {
              pbuf_free(p);
            }
          }

        } while(p!=NULL);

    }    
     
    
} // namespace embot::hw::lwip {



// --------------------------------------------------------------------------------------------------------------------
// - the c part needed by stm32 or by lwip
// --------------------------------------------------------------------------------------------------------------------


// needed by lwip

extern "C"
{
    u32_t sys_now(void)
    {
        return embot::core::now() / 1000;
    }
}

// for stm32

extern "C"
{
    void HAL_ETH_RxAllocateCallback(uint8_t **buff)
    {
        struct pbuf_custom *p = (struct pbuf_custom *)LWIP_MEMPOOL_ALLOC(RX_POOL);
        if (p)
        {
            /* Get the buff from the struct pbuf address. */
            *buff = (uint8_t *)p + offsetof(RxBuff_t, buff);
            p->custom_free_function = embot::hw::lwip::pbuf_free_custom;
            /* Initialize the struct pbuf.
            * This must be performed whenever a buffer's allocated because it may be
            * changed by lwIP or the app, e.g., pbuf_free decrements ref. */
            pbuf_alloced_custom(PBUF_RAW, 0, PBUF_REF, p, *buff, ETH_RX_BUFFER_SIZE);
        }
        else
        {
            embot::hw::lwip::data::rxallocstatus = RxAllocStatus::ERROR;
            *buff = NULL;
        }
    }
}

extern "C"
{
    void HAL_ETH_RxLinkCallback(void **pStart, void **pEnd, uint8_t *buff, uint16_t Length)
    {
        struct pbuf **ppStart = (struct pbuf **)pStart;
        struct pbuf **ppEnd = (struct pbuf **)pEnd;
        struct pbuf *p = NULL;

        /* Get the struct pbuf from the buff address. */
        p = (struct pbuf *)(buff - offsetof(RxBuff_t, buff));
        p->next = NULL;
        p->tot_len = 0;
        p->len = Length;

        /* Chain the buffer. */
        if (!*ppStart)
        {
            /* The first buffer of the packet. */
            *ppStart = p;
        }
        else
        {
            /* Chain the buffer to the end of the packet. */
            (*ppEnd)->next = p;
        }
        *ppEnd  = p;

        /* Update the total length of all the buffers of the chain. Each pbuf in the chain should have its tot_len
        * set to its own length, plus the length of all the following pbufs in the chain. */
        for (p = *ppStart; p != NULL; p = p->next)
        {
            p->tot_len += Length;
        }

        #if defined(CORE_CM7)
            /* Invalidate data cache because Rx DMA's writing to physical memory makes it stale. */
            //  SCB_InvalidateDCache_by_Addr((uint32_t *)buff, Length);
        #else
            #warning removed cache invalidate
        #endif
    }
}

extern "C"
{
    void HAL_ETH_TxFreeCallback(uint32_t * buff)
    {
        pbuf_free((struct pbuf *)buff);
    }
}

extern "C"
{
    void HAL_ETH_RxCpltCallback(ETH_HandleTypeDef *heth)
    {
        //LWIP_UNUSED_ARG(heth);
        embot::hw::lwip::data::onRXethframe.execute();
    }
}

extern "C"
{
    void HAL_ETH_ErrorCallback(ETH_HandleTypeDef *heth)
    {
        if ((HAL_ETH_GetDMAError(heth) & ETH_DMACSR_RBU) == ETH_DMACSR_RBU)
        {
            embot::hw::lwip::data::onRXethframe.execute();
        }
    }
}

extern "C"
{
    void ETH_IRQHandler(void)
    {
        HAL_ETH_IRQHandler(embot::hw::lwip::stm32::pEthHandle);
    }
    
    void ETH_WKUP_IRQHandler(void)
    {
        HAL_ETH_IRQHandler(embot::hw::lwip::stm32::pEthHandle);
    }        
}


// --------------------------------------------------------------------------------------------------------------------
// - external functions
// --------------------------------------------------------------------------------------------------------------------


namespace embot::hw::lwip {
        
    void netconfig(const embot::net::eth::IPconfig &ipconfig, const embot::core::Callback &onRXframe)
    {
        data::onRXethframe = onRXframe;
        netconfig(ipconfig);       
    }
    
    
    void checklink()
    {
        #if LWIP_NETIF_LINK_CALLBACK
        Ethernet_Link_Periodic_Handle();
        #endif        
    }
    
    void checkinput()
    {
        ethernetif_input(embot::hw::lwip::network());        
    }
    
    netIF *network()
    {
        return &data::gnetif; 
    }
    
    
    void hal_ETH_init()
    {
        embot::hw::lwip::stm32::ETH_Init();
    }
    
} // namespace embot::hw::lwip {



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

