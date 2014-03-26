/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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
#include "string.h"

#include "hal_middleware_interface.h"
#include "hal_base_hid.h" 
#include "hal_brdcfg.h"
#include "hal_ethtransceiver.h"

#include "hal_periph_gpio_hid.h"


#include "hl_bits.h" 
#include "hal_heap.h"



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

#include "hal_periph_eth_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_eth_id2index(p)           ((uint8_t)((p)))


#define ETH_BUF_SIZE        1536        /* ETH Receive/Transmit buffer size  */
#define ETH_MTU             1514        /* Eth Maximum Transmission Unit */

/* MAC Configuration Register */
#define MCR_WD              0x00800000  /* Watchdog disable                  */
#define MCR_JD              0x00400000  /* Jabber disable                    */
#define MCR_IFG             0x000E0000  /* Interframe gap mask               */
#define MCR_CSD             0x00010000  /* Carrier sense disable             */
#define MCR_FES             0x00004000  /* Fast 100 Mb ethernet speed        */
#define MCR_ROD             0x00002000  /* Receive own disable               */
#define MCR_LM              0x00001000  /* Loopback mode                     */
#define MCR_DM              0x00000800  /* Duplex mode                       */
#define MCR_IPCO            0x00000400  /* IPv4 checksum offload             */
#define MCR_RD              0x00000200  /* Retry disable                     */
#define MCR_APCS            0x00000080  /* Automatic pad / CRC stripping     */
#define MCR_BL              0x00000060  /* Back-off limit mask               */
#define MCR_DC              0x00000010  /* Deferral check                    */
#define MCR_TE              0x00000008  /* Transmitter enable                */
#define MCR_RE              0x00000004  /* Receiver enable                   */

/* MAC Frame Filter Register */
#define MFFR_RA             0x80000000  /* Receive all                       */
#define MFFR_HPF            0x00000400  /* Hash or Perfect filter            */
#define MFFR_SAF            0x00000200  /* Source address filter             */
#define MFFR_SAIF           0x00000100  /* Source address inverse filtering  */
#define MFFR_PCF            0x000000C0  /* Pass control frames mask          */
#define MFFR_BFD            0x00000020  /* Broadcast frames disable          */
#define MFFR_PAM            0x00000010  /* Pass all multicast                */
#define MFFR_DAIF           0x00000008  /* Dest. address inverse filtering   */
#define MFFR_HM             0x00000004  /* Hash multicast                    */
#define MFFR_HU             0x00000002  /* Hash unicast                      */
#define MFFR_PM             0x00000001  /* Promiscuous mode                  */

/* MAC MII Address Register */
#define MMAR_PA             0x0000F800  /* PHY address mask                  */
#define MMAR_MR             0x000007C0  /* MII register address mask         */
#define MMAR_CR             0x0000001C  /* Clock range                       */
#define MMAR_MW             0x00000002  /* MII write                         */
#define MMAR_MB             0x00000001  /* MII busy                          */

/* MAC MII Data Register */
#define MMDR_MD             0x0000FFFF  /* MII 16-bit rw data                */

/* MII Management Time out values */
#define MII_WR_TOUT         0x00050000  /* MII Write timeout count           */
#define MII_RD_TOUT         0x00050000  /* MII Read timeout count            */

/* MAC Flow Control Register */
#define MFCR_PT             0xFFFF0000  /* Pause time mask                   */
#define MFCR_ZQPD           0x00000080  /* Zero-quanta pause disable         */
#define MFCR_PLT            0x00000030  /* Pause low threshold               */
#define MFCR_UPFD           0x00000008  /* Unicaste pause frame detect       */
#define MFCR_RFCE           0x00000004  /* Receive flow control enable       */
#define MFCR_TFCE           0x00000002  /* Transmit flow control enable      */
#define MFCR_FCB_BPA        0x00000001  /* Flow ctrl busy/back pressure act. */

/* MAC VLAN Tag Register */
#define MVTR_VLANTC         0x00010000  /* 12-bit VLAN tag comparison        */
#define MVTR_VLANTI         0x0000FFFF  /* VLAN tag identifier for rx frames */

/* MAC PMT Contrl and Status Register */
#define MPCSR_WFFRPR        0x80000000  /* Wakeup frame filter reg.ptr reset */
#define MPCSR_GU            0x00000200  /* Global unicast                    */
#define MPCSR_WFR           0x00000040  /* Wakeup frame received             */
#define MPCSR_MPR           0x00000020  /* Magic packet received             */
#define MPCSR_WFE           0x00000004  /* Wakeup frame enable               */
#define MPCSR_MPE           0x00000002  /* Magic packet enable               */
#define MPCSR_PD            0x00000001  /* Power down                        */

/* MAC Interrupt Status Register */
#define MSR_TSTS            0x00000200  /* Time stamp trigger status         */
#define MSR_MMCTS           0x00000040  /* MMC transmit status               */
#define MSR_MMCRS           0x00000020  /* MMC receive status                */
#define MSR_MMCS            0x00000010  /* MMC status                        */
#define MSR_PMTS            0x00000008  /* PMT status                        */

/* MAC Interrupt Mask Register */
#define MIMR_TSTIM          0x00000200  /* Time stamp trigger mask           */
#define MIMR_PMTIM          0x00000008  /* PMT interrupt mask                */

/* MAC Address Hi Control bits */
#define MADR_AE             0x80000000  /* Address enable                    */
#define MADR_SA             0x40000000  /* Source address compare            */
#define MADR_MBC            0x3F000000  /* Mask byte control                 */
#define MADR_AH             0x0000FFFF  /* MAC Address high                  */

/* MMC Control Register */
#define MMCR_MCF            0x00000008  /* MMC counter freeze                */
#define MMCR_ROR            0x00000004  /* Reset on read                     */
#define MMCR_CSR            0x00000002  /* Counter stop rollover             */
#define MMCR_CR             0x00000001  /* Counter reset                     */

/* MMC Receive Interrupt/Mask Registers */
#define MMRI_RGUFS          0x00020000  /* Received good ucast frames status */
#define MMRI_RFAES          0x00000040  /* Received frames align.error status*/
#define MMRI_RFCES          0x00000020  /* Receive frames CRC error status   */

/* MMC Transmit Interrupt/Mask Registers */
#define MMTIR_TGFS          0x00200000  /* Transmitted good frames status    */
#define MMTIR_TGFMSCS       0x00008000  /* Tx good frm.more single col.status*/
#define MMTIR_TGFSCS        0x00004000  /* Tx good frm.single collision stat.*/

/* PTP Time Stamp Register */
#define PTSCR_TTSARU        0x00000020  /* Time stamp addend register update */
#define PTSCR_TSITE         0x00000010  /* TS interrupt trigger enable       */
#define PTSCR_TSSTU         0x00000008  /* TS system time update             */
#define PTSCR_TSSTI         0x00000004  /* TS system time initialize         */
#define PTSCR_TSFCU         0x00000002  /* TS fine or coarse update          */
#define PTSCR_TSE           0x00000001  /* Time stamp enable                 */

/* DMA Bus Mode Register */
#define DBMR_AAB            0x02000000  /* Address-aligned beats             */
#define DBMR_FPM            0x01000000  /* 4xPBL mode                        */
#define DBMR_USP            0x00800000  /* Use separate PBL                  */
#define DBMR_RDP            0x007E0000  /* Rx DMA PBL mask                   */
#define DBMR_FB             0x00010000  /* Fixed burst                       */
#define DBMR_RTPR           0x0000C000  /* Rx Tx priority ratio              */
#define DBMR_PBL            0x00003F00  /* Programmable burst length mask    */
#define DBMR_DSL            0x0000007C  /* Descriptor skip length            */
#define DBMR_DA             0x00000002  /* DMA arbitration                   */
#define DBMR_SR             0x00000001  /* Software reset                    */

/* DMA Status Register */
#define DSR_TSTS            0x20000000  /* Timestamp trigger status          */
#define DSR_PMTS            0x10000000  /* PMT status                        */
#define DSR_MMCS            0x08000000  /* MMC status                        */
#define DSR_EBS             0x03800000  /* Error bits status mask            */
#define DSR_TPS             0x00700000  /* Transmit process state            */
#define DSR_RPS             0x000E0000  /* Receive process state             */
#define DSR_NIS             0x00010000  /* Normal interrupt summary          */
#define DSR_AIS             0x00008000  /* Abnormal interrupt summary        */
#define DSR_ERS             0x00004000  /* Early receive status              */
#define DSR_FBES            0x00002000  /* Fatal bus error status            */
#define DSR_ETS             0x00000400  /* Early transmit status             */
#define DSR_RWTS            0x00000200  /* Receive watchdog timeout status   */
#define DSR_RPSS            0x00000100  /* Receive process stopped status    */
#define DSR_RBUS            0x00000080  /* Receive buffer unavailable status */
#define DSR_RS              0x00000040  /* Receive status                    */
#define DSR_TUS             0x00000020  /* Transmit underflow status         */
#define DSR_ROS             0x00000010  /* Receive overflow status           */
#define DSR_TJTS            0x00000008  /* Transmit jabber timeout status    */
#define DSR_TBUS            0x00000004  /* Transmit buffer unavailable status*/
#define DSR_TPSS            0x00000002  /* Transmit process stopped status   */
#define DSR_TS              0x00000001  /* Transmit status                   */

/* DMA Operation Mode Register */
#define DOMR_DTCEFD         0x04000000  /* Dropping of TCP/IP chksum err dis.*/
#define DOMR_RSF            0x02000000  /* Receive store and forward         */
#define DOMR_DFRF           0x01000000  /* Disable flushing of received frms */
#define DOMR_TSF            0x00200000  /* Transmit storea and forward       */
#define DOMR_FTF            0x00100000  /* Flush transmit FIFO               */
#define DOMR_TTC            0x0001C000  /* Transmit treshold control mask    */
#define DOMR_ST             0x00002000  /* Start/stop transmission           */
#define DOMR_FEF            0x00000080  /* Forward error frames              */
#define DOMR_FUGF           0x00000040  /* Forward undersized good frames    */
#define DOMR_RTC            0x00000018  /* Receive threshold control mask    */
#define DOMR_OSF            0x00000004  /* Operate on second frame           */
#define DOMR_SR             0x00000002  /* Start/stop receive                */

/* DMA Interrupt Enable Register */
#define INT_NISE            0x00010000  /* Normal interrupt summary          */
#define INT_AISE            0x00008000  /* Abnormal interrupt summary        */
#define INT_ERIE            0x00004000  /* Early receive interrupt           */
#define INT_FBEIE           0x00002000  /* Fatal bus error interrupt         */
#define INT_ETIE            0x00000400  /* Early transmit interrupt          */
#define INT_RWTIE           0x00000200  /* Receive watchdog timeout interrupt*/
#define INT_RPSIE           0x00000100  /* Receive process stopped intterrupt*/
#define INT_RBUIE           0x00000080  /* Receive buffer unavailable inter. */
#define INT_RIE             0x00000040  /* Receive interrupt                 */
#define INT_TUIE            0x00000020  /* Transmit underflow interrupt      */
#define INT_ROIE            0x00000010  /* Receive overflow interrupt        */
#define INT_TJTIE           0x00000008  /* Transmit jabber timeout interrupt */
#define INT_TBUIE           0x00000004  /* Transmit buffer unavailable inter.*/
#define INT_TPSIE           0x00000002  /* Transmit process stopped interrupt*/
#define INT_TIE             0x00000001  /* Transmit interrupt                */

/* DMA Missed Frame and Buffer Overflow Counter Register */
#define DMFBOC_OFOC         0x10000000  /* Overflow bit for FIFO ovfl cntr.  */
#define DMFBOC_MFA          0x0FFE0000  /* Missed frames by the application  */
#define DMFBOC_OMFC         0x00010000  /* Ovfl bit for missed frame counter */
#define DMFBOC_MFC          0x0000FFFF  /* Missed frames by controller       */

/* DMA Descriptor TX Packet Control/Status */
#define DMA_TX_OWN          0x80000000  /* Own bit 1=DMA,0=CPU               */
#define DMA_TX_IC           0x40000000  /* Interrupt on completition         */
#define DMA_TX_LS           0x20000000  /* Last segment                      */
#define DMA_TX_FS           0x10000000  /* First segment                     */
#define DMA_TX_DC           0x08000000  /* Disable CRC                       */
#define DMA_TX_DP           0x04000000  /* Disable pad                       */
#define DMA_TX_TTSE         0x02000000  /* Transmit time stamp enable        */
#define DMA_TX_CIC          0x00C00000  /* Checksum insertion control        */
#define DMA_TX_TER          0x00200000  /* Transmit end of ring              */
#define DMA_TX_TCH          0x00100000  /* Second address chained            */
#define DMA_TX_TTSS         0x00020000  /* Transmit time stamp status        */
#define DMA_TX_IHE          0x00010000  /* IP header error status            */
#define DMA_TX_ES           0x00008000  /* Error summary                     */
#define DMA_TX_JT           0x00004000  /* Jabber timeout                    */
#define DMA_TX_FF           0x00002000  /* Frame flushed                     */
#define DMA_TX_IPE          0x00001000  /* IP payload error                  */
#define DMA_TX_LC           0x00000800  /* Loss of carrier                   */
#define DMA_TX_NC           0x00000400  /* No carrier                        */
#define DMA_TX_LCOL         0x00000200  /* Late collision                    */
#define DMA_TX_EC           0x00000100  /* Excessive collision               */
#define DMA_TX_VF           0x00000080  /* VLAN frame                        */
#define DMA_TX_CC           0x00000078  /* Collision count                   */
#define DMA_TX_ED           0x00000004  /* Excessive deferral                */
#define DMA_TX_UF           0x00000002  /* Underflow error                   */
#define DMA_TX_DB           0x00000001  /* Deferred bit                      */

/* DMA Descriptor RX Packet Status */
#define DMA_RX_OWN          0x80000000  /* Own bit 1=DMA,0=CPU               */
#define DMA_RX_AFM          0x40000000  /* Destination address filter fail   */
#define DMA_RX_FL           0x3FFF0000  /* Frame length mask                 */
#define DMA_RX_ES           0x00008000  /* Error summary                     */
#define DMA_RX_DE           0x00004000  /* Descriptor error                  */
#define DMA_RX_SAF          0x00002000  /* Source address filter fail        */
#define DMA_RX_LE           0x00001000  /* Length error                      */
#define DMA_RX_OE           0x00000800  /* Overflow error                    */
#define DMA_RX_VLAN         0x00000400  /* VLAN tag                          */
#define DMA_RX_FS           0x00000200  /* First descriptor                  */
#define DMA_RX_LS           0x00000100  /* Last descriptor                   */
#define DMA_RX_IPHCE        0x00000080  /* IPv4 header checksum error        */
#define DMA_RX_LC           0x00000040  /* late collision                    */
#define DMA_RX_FT           0x00000020  /* Frame type                        */
#define DMA_RX_RWT          0x00000010  /* Receive watchdog timeout          */
#define DMA_RX_RE           0x00000008  /* Receive error                     */
#define DMA_RX_DRE          0x00000004  /* Dribble bit error                 */
#define DMA_RX_CE           0x00000002  /* CRC error                         */
#define DMA_RX_RMAM         0x00000001  /* Rx MAC adr.match/payload cks.error*/

#define DMA_RX_ERROR_MASK   (DMA_RX_ES | DMA_RX_LE | DMA_RX_RWT | \
                             DMA_RX_RE | DMA_RX_CE)
#define DMA_RX_SEG_MASK     (DMA_RX_FS | DMA_RX_LS)

/* DMA Descriptor RX Packet Control */
#define DMA_RX_DIC          0x80000000  /* Disable interrupt on completition */
#define DMA_RX_RBS2         0x1FFF0000  /* Receive buffer 2 size             */
#define DMA_RX_RER          0x00008000  /* Receove end of ring               */
#define DMA_RX_RCH          0x00004000  /* Second address chained            */
#define DMA_RX_RBS1         0x00003FFF  /* Receive buffer 1 size             */

// removed because they are value related to an external phy and not to the mpu.
// the values are defined inside the specific board which uses a given phy
// /* Common PHY Registers addresses */
// #define PHY_REG_BMCR        0x00        /* Basic Mode Control Register       */
// #define PHY_REG_BMSR        0x01        /* Basic Mode Status Register        */
// #define PHY_REG_IDR1        0x02        /* PHY Identifier 1                  */
// #define PHY_REG_IDR2        0x03        /* PHY Identifier 2                  */
// #define PHY_REG_ANAR        0x04        /* Auto-Negotiation Advertisement    */
// #define PHY_REG_ANLPAR      0x05        /* Auto-Neg. Link Partner Abitily    */
//
// /* Common PHY Registers values */
// #define PHY_FULLD_100M      0x2100      /* Full Duplex 100Mbit               */
// #define PHY_HALFD_100M      0x2000      /* Half Duplex 100Mbit               */
// #define PHY_FULLD_10M       0x0100      /* Full Duplex 10Mbit                */
// #define PHY_HALFD_10M       0x0000      /* Half Duplex 10MBit                */
// #define PHY_AUTO_NEG        0x3000      /* Select Auto Negotiation           */

// #define PHY_Reset                       ((uint16_t)0x8000)      /* PHY Reset */
// #define PHY_Loopback                    ((uint16_t)0x4000)      /* Select loop-back mode */
// #define PHY_FULLDUPLEX_100M             ((uint16_t)0x2100)      /* Set the full-duplex mode at 100 Mb/s */
// #define PHY_HALFDUPLEX_100M             ((uint16_t)0x2000)      /* Set the half-duplex mode at 100 Mb/s */
// #define PHY_FULLDUPLEX_10M              ((uint16_t)0x0100)      /* Set the full-duplex mode at 10 Mb/s */
// #define PHY_HALFDUPLEX_10M              ((uint16_t)0x0000)      /* Set the half-duplex mode at 10 Mb/s */
// #define PHY_AutoNegotiation             ((uint16_t)0x1000)      /* Enable auto-negotiation function */
// #define PHY_Restart_AutoNegotiation     ((uint16_t)0x0200)      /* Restart auto-negotiation function */
// #define PHY_Powerdown                   ((uint16_t)0x0800)      /* Select the power down mode */
// #define PHY_Isolate                     ((uint16_t)0x0400)      /* Isolate PHY from MII */


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

extern hal_eth_hid_debug_support_t hal_eth_hid_DEBUG_support =
{
    .fn_inside_eth_isr  = NULL
};

// VALE
/* This variable can contain a ptr to a function that want to use a eth raw pkt (with ethernet and udp/tcp headers, etc)
   For example this function can be a parser to check a particular info in a pkt, like sequence number
   In oreder to use this variable, an external program have to declare hal_eth_lowLevelUsePacket_ptr like external and set
   to the ptr of application's function.
*/
void (*hal_eth_lowLevelUsePacket_ptr)(uint8_t* pkt_ptr, uint32_t size) = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef uint32_t    U32;


typedef uint32_t (*hal_eth_array_of_buffers_t)[ETH_BUF_SIZE>>2];


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



typedef struct
{
    hal_eth_cfg_t               config;  
    hal_eth_onframereception_t  onframerx;  
    uint8_t                     rxbufindex;     
    uint8_t                     txbufindex;   
    hal_rx_desc_t*              rx_desc;
    hal_tx_desc_t*              tx_desc;
    hal_eth_array_of_buffers_t  rx_buffers;
    hal_eth_array_of_buffers_t  tx_buffers;
} hal_eth_internal_item_t;


typedef struct
{
    uint8_t                     initted;
    hal_eth_internal_item_t*    items[hal_eths_number];
} hal_eth_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_eth_supported_is(void);
static void s_hal_eth_initted_set(hal_eth_t id);
static hal_boolval_t s_hal_eth_initted_is(hal_eth_t id);

// keep the same names of the tcpnet driver, without s_ prefix
static void s_hal_eth_rx_descr_init(void);
static void s_hal_eth_tx_descr_init(void);


static void s_hal_eth_rmii_init(void);

static void s_hal_eth_mac_reset(void);
static void s_hal_eth_mac_init(const hal_eth_cfg_t *cfg, hal_eth_phymode_t phymode);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------

static const hal_eth_network_functions_t s_hal_eth_fns = 
{
    .eth_init           = hal_eth_init, 
    .eth_enable         = hal_eth_enable, 
    .eth_disable        = hal_eth_disable, 
    .eth_sendframe      = hal_eth_sendframe
};


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


// keep teh same names of the tcpnet driver, without s_ prefix
//static uint8_t RxBufIndex = 0;
//static uint8_t TxBufIndex = 0;

// keep the same names of the tcpnet driver, without s_ prefix. however, make them pointers
/* ENET local DMA buffers. */
//static uint32_t (*rx_buf)[ETH_BUF_SIZE>>2] = NULL;
//static uint32_t (*tx_buf)[ETH_BUF_SIZE>>2] = NULL;

// keep the same names of the tcpnet driver, without s_ prefix. however, make them pointers
/* ENET local DMA Descriptors. */
//static hal_rx_desc_t *Rx_Desc = NULL;
//static hal_tx_desc_t *Tx_Desc = NULL;

//#warning WIP --> put all static variables inside s_hal_eth_theinternals


static hal_eth_theinternals_t s_hal_eth_theinternals =
{
    .initted            = 0,
    .items              = { NULL }   
};

#if 0
static hal_eth_internal_item_t s_hal_eth_theinternals =
{
    .config     =
    {
        .priority                   = hal_int_priority15,
        .onframerx                  = NULL,
        .macaddress                 = 0,
        .capacityoftxfifoofframes   = 0,
        .capacityofrxfifoofframes   = 0        
    },
    .onframerx  =
    {
        .frame_new                  = NULL,
        .frame_movetohigherlayer    = NULL,
        .frame_alerthigherlayer     = NULL
    }
    
};
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern void ETH_IRQHandler(void);


extern hal_result_t hal_eth_init(const hal_eth_cfg_t *cfg)
{
    const hal_eth_t id = hal_eth1;
    hal_eth_phymode_t usedphymode;
    hal_eth_internal_item_t* intitem = s_hal_eth_theinternals.items[HAL_eth_id2index(id)];

    if(hal_true != s_hal_eth_supported_is())
    {
        return(hal_res_NOK_unsupported);
    }
    
    if(hal_true == s_hal_eth_initted_is(id))
    {
        return(hal_res_NOK_generic);
    }
    
    #warning WIP --> make ipal use a cfg w/ capacityoftxfifoofframes and capacityofrxfifoofframes
    uint8_t capacityoftxfifoofframes = 1;   // cfg->capacityoftxfifoofframes
    uint8_t capacityofrxfifoofframes = 2;   // cfg->capacityofrxfifoofframes


    if((NULL == cfg) || (hal_int_priorityNONE == cfg->priority) || (NULL == cfg->onframerx) || (NULL == cfg->onframerx->frame_new) || (NULL == cfg->onframerx->frame_movetohigherlayer) || (NULL == cfg->macaddress))
    {
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_eth_init() needs a cfg w/ functions and mac addr and valid priority");
    }
    else if( (0 == capacityoftxfifoofframes) || (0 == capacityofrxfifoofframes))
    {
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_eth_init() needs a cfg w/ non-zero tx and rx queue");
    }

    if(NULL == intitem)
    {
        intitem = s_hal_eth_theinternals.items[HAL_eth_id2index(id)] = hal_heap_new(sizeof(hal_eth_internal_item_t));
    
    }
    
    memcpy(&intitem->config, cfg, sizeof(hal_eth_cfg_t));
    intitem->config.capacityoftxfifoofframes = capacityoftxfifoofframes;
    intitem->config.capacityofrxfifoofframes = capacityofrxfifoofframes;
    
    memcpy(&intitem->onframerx, cfg->onframerx, sizeof(hal_eth_onframereception_t));
    
    
    // give ram to the dma descriptors
    
    
    
    if((0 == capacityoftxfifoofframes) || (0 == capacityofrxfifoofframes))
    {
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_eth_init() needs a non-zero number of dma tx and rx buffers");
    }
    
    intitem->rx_buffers = (hal_eth_array_of_buffers_t) hal_heap_new(ETH_BUF_SIZE * capacityofrxfifoofframes);     
    intitem->tx_buffers = (hal_eth_array_of_buffers_t) hal_heap_new(ETH_BUF_SIZE * capacityoftxfifoofframes);
    intitem->rx_desc = (hal_rx_desc_t*) hal_heap_new(sizeof(hal_rx_desc_t) * capacityofrxfifoofframes);
    intitem->tx_desc = (hal_tx_desc_t*) hal_heap_new(sizeof(hal_tx_desc_t) * capacityoftxfifoofframes);
    //Rx_Desc = (hal_rx_desc_t*) hal_heap_new(sizeof(hal_rx_desc_t) * capacityofrxfifoofframes);
    //Tx_Desc = (hal_tx_desc_t*) hal_heap_new(sizeof(hal_tx_desc_t) * capacityoftxfifoofframes); 


//    eventviewer_init();
#if defined(HAL_USE_EVENTVIEWER_ETH)
    eventviewer_load(ev_ID_first_isr+hal_arch_arm_ETH_IRQn, ETH_IRQHandler);
#endif
    
    // in here we allow a specific board to init all what is related to the phy.
    // in case of a phy accessed through the smi, this function must: a. init the smi, b. reset the phy, ... that's it.
    // instead in case of a switch accessed through i2c, this function must: a. init the i2c, reset the switch, that's it.    
    if(hal_res_NOK_generic == hal_ethtransceiver_init(NULL))
    {
         hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_eth_init() called hal_ethtransceiver_init() which failed");
    }        
    
  
    // init rmii
    s_hal_eth_rmii_init(); 


    // s_hal_eth_mac_reset();  // can be moved just after configuration of mac register  
    
    // in here we allow a specific board to configure and start the phy. with a mode (full/half) and a speed (10/100) whcih are board specifics,
    // but which we retrieve
    // in case of a phy accessed through the smi, this function must set the mode and the speed and return the result
    // instead in case of a switch accessed through i2c, this function must configure mode and speed and put the swicth operative. 

    hal_ethtransceiver_config(hal_eth_phymode_auto, &usedphymode);
    
    
    if(hal_eth_phymode_none == usedphymode)
    {
        hal_base_on_fatalerror(hal_fatalerror_incorrectparameter, "hal_eth_init() called hal_ethtransceiver_config() which does not support a phy mode");
    }
    
    // for rmii we use the same config at the phy ...
    hal_eth_phymode_t rmiiphymode = usedphymode;
    s_hal_eth_mac_init(cfg, rmiiphymode);
    

    s_hal_eth_initted_set(id);
    
    return(hal_res_OK);
}

#define EXPERIMENTAL_ETH_ENABLE_DISABLE_BUT_IT_IS_TO_BE_TESTED
extern hal_result_t hal_eth_enable(void) 
{
#if     !defined(EXPERIMENTAL_ETH_ENABLE_DISABLE_BUT_IT_IS_TO_BE_TESTED)
    // acemor on 31 oct 2011: it is the same as NVIC_EnableIRQ(ETH_IRQn) or hal_sys_hid_irqn_enable()
    /* Ethernet Interrupt Enable function. */
    
    NVIC->ISER[1] = 1 << 29;
#else    
    // for __DSB() and __ISB, see: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka13559.html
    
    NVIC_ClearPendingIRQ(ETH_IRQn); // shall i clear or shall i not clear?
    
    NVIC_EnableIRQ(ETH_IRQn);
    __DSB();
    __ISB();
    
    // i need to force the isr if since last call of hal_eth_disable() the dma has moved a frame in the descriptor
    
    if(INT_RIE == (INT_RIE & ETH->DMASR))
    {
        NVIC_SetPendingIRQ(ETH_IRQn);
    }    
#endif    
    return(hal_res_OK);
}


extern hal_result_t hal_eth_disable(void) 
{
#if     !defined(EXPERIMENTAL_ETH_ENABLE_DISABLE_BUT_IT_IS_TO_BE_TESTED)
    // acemor on 31 oct 2011: it is the same as NVIC_DisableIRQ(ETH_IRQn) or hal_sys_hid_irqn_disable()
    /* Ethernet Interrupt Disable function. */
    
    NVIC->ICER[1] = 1 << 29;
#else      
    // for __DSB() and __ISB, see: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka13559.html
    
    NVIC_DisableIRQ(ETH_IRQn);
    __DSB();
    __ISB();
#endif  
    return(hal_res_OK);
}


// changed NUM_TX_BUF with s_hal_eth_theinternals.config.capacityoftxfifoofframes (former hal_base_hid_params.eth_dmatxbuffer_num)
extern hal_result_t hal_eth_sendframe(hal_eth_frame_t *frame) 
{
    // called by tcpnet's send_frame(OS_FRAME *frame)
    
    // il caller passa (U32 *)&frame->data[0] e frame->length

    /* Send frame to ETH ethernet controller */
    U32 *sp,*dp;
    U32 i,j;
    
    const hal_eth_t id = hal_eth1;
    hal_eth_internal_item_t* intitem = s_hal_eth_theinternals.items[HAL_eth_id2index(id)];


    if(hal_true != s_hal_eth_initted_is(hal_eth1))
    {
        return(hal_res_NOK_generic);
    }


    j = intitem->txbufindex; //TxBufIndex;
    /* Wait until previous packet transmitted. */
    //while (Tx_Desc[j].CtrlStat & DMA_TX_OWN);
    while (intitem->tx_desc[j].CtrlStat & DMA_TX_OWN);

    sp = (U32 *)&frame->datafirstbyte[0];
    dp = (U32 *)(intitem->tx_desc[j].Addr & ~3);
    //dp = (U32 *)(Tx_Desc[j].Addr & ~3);

    /* Copy frame data to ETH IO buffer. */
    for (i = (frame->length + 3) >> 2; i; i--) {
        *dp++ = *sp++;
    }
    //Tx_Desc[j].Size      = frame->length;
    //Tx_Desc[j].CtrlStat |= DMA_TX_OWN;
    intitem->tx_desc[j].Size      = frame->length;
    intitem->tx_desc[j].CtrlStat |= DMA_TX_OWN;    
    if (++j == intitem->config.capacityoftxfifoofframes) j = 0;
    intitem->txbufindex = j; //TxBufIndex = j;
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

// changed NUM_RX_BUF with s_hal_eth_theinternals.items[0]->config.capacityofrxfifoofframes (former hal_base_hid_params.eth_dmarxbuffer_num)
// changed OS_FRAME with hal_eth_frame_t
void ETH_IRQHandler(void) 
{
    const hal_eth_t id = hal_eth1;
    hal_eth_internal_item_t* intitem = s_hal_eth_theinternals.items[HAL_eth_id2index(id)];
    
  // questa non va cambiata. pero' .... usa OS_FRAME e chiama alloc_mem() e put_in_queue().
  // bisogna usare alcuni puntatori a funzione che siano inizializzati / passati in hal_eth_init().
  // inoltre, se si vuole segnalare l'arrivo di un frame gia' qui dentro, allora bisogna definire
  // un altro puntatore a funzione hal_on_rx_frame().

    uint8_t* rxframedata = NULL;
    uint16_t rxframesize = 0;

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

  
  while (((int_stat = ETH->DMASR) & INT_NISE) != 0) {   // -> read dma status register until we have the normal interrupt bit set
    ETH->DMASR = int_stat;                              // -> reset the dma status register
    if (int_stat & INT_RIE) {                           // -> in case of received frame ...
      /* Valid frame has been received. */
      i = intitem->rxbufindex; //RxBufIndex;
      if (intitem->rx_desc[i].Stat & DMA_RX_ERROR_MASK) {  
      //if (Rx_Desc[i].Stat & DMA_RX_ERROR_MASK) {
        goto rel;
      }
      //if ((Rx_Desc[i].Stat & DMA_RX_SEG_MASK) != DMA_RX_SEG_MASK) {
      if ((intitem->rx_desc[i].Stat & DMA_RX_SEG_MASK) != DMA_RX_SEG_MASK) {
        goto rel;
      }
      
      //RxLen = ((Rx_Desc[i].Stat >> 16) & 0x3FFF) - 4;   // -> retrieve the length of teh frame
      RxLen = ((intitem->rx_desc[i].Stat >> 16) & 0x3FFF) - 4;   // -> retrieve the length of teh frame
      rxframesize = RxLen;
      if (RxLen > ETH_MTU) {
        /* Packet too big, ignore it and free buffer. */
        goto rel;
      }
      /* Flag 0x80000000 to skip sys_error() call when out of memory. */
      //frame = alloc_mem (RxLen | 0x80000000);
      frame = intitem->onframerx.frame_new(RxLen | 0x80000000);
      /* if 'alloc_mem()' has failed, ignore this packet. */
      if (frame != NULL) {
        //sp = (U32 *)(Rx_Desc[i].Addr & ~3);
        sp = (U32 *)(intitem->rx_desc[i].Addr & ~3);
        dp = (U32 *)&frame->datafirstbyte[0];
        rxframedata = (uint8_t*)dp/*sp*/; // VALE
        for (RxLen = (RxLen + 3) >> 2; RxLen; RxLen--) {
          *dp++ = *sp++;
        }
        
        if(hal_eth_lowLevelUsePacket_ptr != NULL) // VALE
        {
            hal_eth_lowLevelUsePacket_ptr(&frame->datafirstbyte[0], rxframesize);
        }
        
        //put_in_queue (frame);
        intitem->onframerx.frame_movetohigherlayer(frame);

        if(NULL != intitem->onframerx.frame_alerthigherlayer)
        {
            intitem->onframerx.frame_alerthigherlayer();
        }

        //rxframes ++;
        
      }
      else
      {
          rxframedata = 0;          
      }
      
    if(NULL != hal_eth_hid_DEBUG_support.fn_inside_eth_isr)
    {            
        hal_eth_hid_DEBUG_support.fn_inside_eth_isr(rxframedata, rxframesize);
    }
        
      /* Release this frame from ETH IO buffer. */
//rel:  Rx_Desc[i].Stat = DMA_RX_OWN;
rel:  intitem->rx_desc[i].Stat = DMA_RX_OWN;
    
      
    if (++i == intitem->config.capacityofrxfifoofframes) i = 0;
      intitem->rxbufindex = i; //RxBufIndex = i;
    }
    if (int_stat & INT_TIE) {
      /* Frame transmit completed. */
      //txframes++;
    }
  }             // -> we execute a new cycle only if in the execution time of the loop a new interrupt has arrived. 

#if defined(HAL_USE_EVENTVIEWER_ETH)    
  eventviewer_switch_to(prev);
#endif
}


// ---- isr of the module: end ------


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_bool_t s_hal_eth_supported_is(void)
{
    return(hal_brdcfg_eth__theconfig.supported);
}

static void s_hal_eth_initted_set(hal_eth_t id)
{
    hl_bits_byte_bitset(&s_hal_eth_theinternals.initted, HAL_eth_id2index(id));
}

static hal_boolval_t s_hal_eth_initted_is(hal_eth_t id)
{
    return((hal_boolval_t)hl_bits_byte_bitcheck(s_hal_eth_theinternals.initted, HAL_eth_id2index(id)));
}



// changed NUM_RX_BUF with s_hal_eth_theinternals.config.capacityofrxfifoofframes (former hal_base_hid_params.eth_dmarxbuffer_num)
static void s_hal_eth_rx_descr_init(void) 
{
  /* Initialize Receive DMA Descriptor array. */
  U32 i,next;
    const hal_eth_t id = hal_eth1;
    hal_eth_internal_item_t* intitem = s_hal_eth_theinternals.items[HAL_eth_id2index(id)];

  intitem->rxbufindex = 0; //RxBufIndex = 0;
  for (i = 0, next = 0; i < intitem->config.capacityofrxfifoofframes; i++) {
    if (++next == intitem->config.capacityofrxfifoofframes) next = 0;
    intitem->rx_desc[i].Stat = DMA_RX_OWN;
    intitem->rx_desc[i].Ctrl = DMA_RX_RCH | ETH_BUF_SIZE;
    intitem->rx_desc[i].Addr = (U32)&intitem->rx_buffers[i];
    intitem->rx_desc[i].Next = (U32)&intitem->rx_desc[next];
  }
  ETH->DMARDLAR = (U32)&intitem->rx_desc[0];
}

// changed NUM_TX_BUF with s_hal_eth_theinternals.config.capacityoftxfifoofframes
static void s_hal_eth_tx_descr_init(void) 
{
  /* Initialize Transmit DMA Descriptor array. */
  U32 i,next;
    const hal_eth_t id = hal_eth1;
    hal_eth_internal_item_t* intitem = s_hal_eth_theinternals.items[HAL_eth_id2index(id)];

  intitem->txbufindex = 0; //TxBufIndex = 0;
  for (i = 0, next = 0; i < intitem->config.capacityoftxfifoofframes; i++) {
    if (++next == intitem->config.capacityoftxfifoofframes) next = 0;
    intitem->tx_desc[i].CtrlStat = DMA_TX_TCH | DMA_TX_LS | DMA_TX_FS;
    intitem->tx_desc[i].Addr     = (U32)&intitem->tx_buffers[i];
    intitem->tx_desc[i].Next     = (U32)&intitem->tx_desc[next];
  }
  ETH->DMATDLAR = (U32)&intitem->tx_desc[0];
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
    uint8_t *macaddr = (uint8_t*)&(cfg->macaddress);
    // reset mac register
    s_hal_eth_mac_reset(); 
    
    // initialise mac control register
    ETH->MACCR  = 0x00008000;       // clear

   
    if((hal_eth_phymode_halfduplex100mbps == phymode) || (hal_eth_phymode_fullduplex100mbps == phymode))
    {
        ETH->MACCR |= MCR_FES;              // config 100mbs 
    }
//     else
//     {
//         ETH->MACCR &= ~MCR_FES;             // nothing needed for 10 mbps
//     }
    
    if((hal_eth_phymode_fullduplex10mbps == phymode) || (hal_eth_phymode_fullduplex100mbps == phymode))
    {
        ETH->MACCR |= MCR_DM;               // config full duplex mode
    }
//     else
//     {
//         ETH->MACCR &= ~MCR_DM;              // nothing needed for half duplex
//     }

    
    /* MAC address filter register, accept multicast packets. */
    ETH->MACFFR = MFFR_HPF | MFFR_PAM;  // config hash or perfect address filter and pass all multicast (PAM)

    /* Ethernet MAC flow control register */
    ETH->MACFCR = MFCR_ZQPD;   // xero-quanta pause disable
    
    /* Set the Ethernet MAC Address registers */
    ETH->MACA0HR = ((U32)macaddr[5] <<  8) | (U32)macaddr[4];
    ETH->MACA0LR = ((U32)macaddr[3] << 24) | (U32)macaddr[2] << 16 | ((U32)macaddr[1] <<  8) | (U32)macaddr[0];
   
   
    /* Initialize Tx and Rx DMA Descriptors */
    s_hal_eth_rx_descr_init();
    s_hal_eth_tx_descr_init();
    
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
    hal_gpio_cfg_t config;
#if     defined(HAL_USE_CPU_FAM_STM32F1) 
    static hal_gpio_altcfg_t rmii_rx_altcfg = 
    {
        .gpioext    =
        {
            .GPIO_Pin       = 0,
            .GPIO_Speed     = GPIO_Speed_50MHz,
            .GPIO_Mode      = GPIO_Mode_IN_FLOATING,  
        },
        .afname     = HAL_GPIO_AFNAME_NONE,
        .afmode     = HAL_GPIO_AFMODE_NONE
    };
    rmii_rx_altcfg.afname = GPIO_Remap_ETH;
    rmii_rx_altcfg.afmode = (hal_gpio_portD == hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_CRS_DV.gpio.port) ? (ENABLE) : (DISABLE);    
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
    static const hal_gpio_altcfg_t rmii_rx_altcfg = 
    {
        .gpioext    =
        {
             .GPIO_Pin       = 0,
             .GPIO_Mode      = GPIO_Mode_AF,
             .GPIO_Speed     = GPIO_Speed_100MHz,
             .GPIO_OType     = GPIO_OType_PP,
             .GPIO_PuPd      = GPIO_PuPd_NOPULL  
        },
        .afname     = GPIO_AF_ETH,
        .afmode     = ENABLE
    };
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 

    memcpy(&config, &hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_CRS_DV.config, sizeof(hal_gpio_cfg_t));
    config.altcfg = (hal_gpio_altcfg_t*) &rmii_rx_altcfg;
    hal_gpio_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_CRS_DV.gpio, &config);
    
    memcpy(&config, &hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD0.config, sizeof(hal_gpio_cfg_t));
    config.altcfg = (hal_gpio_altcfg_t*) &rmii_rx_altcfg;
    hal_gpio_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD0.gpio, &config);    
    
    memcpy(&config, &hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD1.config, sizeof(hal_gpio_cfg_t));
    config.altcfg = (hal_gpio_altcfg_t*) &rmii_rx_altcfg;
    hal_gpio_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD1.gpio, &config);    
    
    
//    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_CRS_DV, &rmii_rx_altcfg);    
//    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD0, &rmii_rx_altcfg);   
//    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD1, &rmii_rx_altcfg);    
}

// cambiata
extern void hal_eth_hid_rmii_tx_init(void)
{
    hal_gpio_cfg_t config;
#if     defined(HAL_USE_CPU_FAM_STM32F1) 
    static const hal_gpio_altcfg_t rmii_tx_altcfg = 
    {
        .gpioext    =
        {
            .GPIO_Pin       = 0,
            .GPIO_Speed     = GPIO_Speed_50MHz,
            .GPIO_Mode      = GPIO_Mode_AF_PP, 
        },
        .afname     = HAL_GPIO_AFNAME_NONE,
        .afmode     = HAL_GPIO_AFMODE_NONE
    };
#elif   defined(HAL_USE_CPU_FAM_STM32F4)   
    static const hal_gpio_altcfg_t rmii_tx_altcfg = 
    {
        .gpioext    =
        {
             .GPIO_Pin       = 0,
             .GPIO_Mode      = GPIO_Mode_AF,
             .GPIO_Speed     = GPIO_Speed_100MHz,
             .GPIO_OType     = GPIO_OType_PP,
             .GPIO_PuPd      = GPIO_PuPd_NOPULL  
        },
        .afname     = GPIO_AF_ETH,
        .afmode     = ENABLE
    };
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
    
    memcpy(&config, &hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TX_EN.config, sizeof(hal_gpio_cfg_t));
    config.altcfg = (hal_gpio_altcfg_t*) &rmii_tx_altcfg;
    hal_gpio_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TX_EN.gpio, &config);    
    
    memcpy(&config, &hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD0.config, sizeof(hal_gpio_cfg_t));
    config.altcfg = (hal_gpio_altcfg_t*) &rmii_tx_altcfg;
    hal_gpio_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD0.gpio, &config);     

    memcpy(&config, &hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD1.config, sizeof(hal_gpio_cfg_t));
    config.altcfg = (hal_gpio_altcfg_t*) &rmii_tx_altcfg;
    hal_gpio_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD1.gpio, &config);      
    
//    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TX_EN, &rmii_tx_altcfg);    
//    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD0, &rmii_tx_altcfg);   
//    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD1, &rmii_tx_altcfg);        
}
 


extern void hal_eth_hid_smi_init(void)
{
    hal_gpio_cfg_t config;
#if     defined(HAL_USE_CPU_FAM_STM32F1)    
    static const hal_gpio_altcfg_t smi_altcfg = 
    {
        .gpioext    =
        {
            .GPIO_Pin       = 0,
            .GPIO_Speed     = GPIO_Speed_50MHz,
            .GPIO_Mode      = GPIO_Mode_AF_PP, 
        },
        .afname     = HAL_GPIO_AFNAME_NONE,
        .afmode     = HAL_GPIO_AFMODE_NONE
    };
    const uint32_t mdcclockrange = 0x00000000; // MDC Clock range: 60-72MHz. MDC = Management data clock. (RMII signal)
#elif   defined(HAL_USE_CPU_FAM_STM32F4) 
    static const hal_gpio_altcfg_t smi_altcfg = 
    {
        .gpioext    =
        {
             .GPIO_Pin       = 0,
             .GPIO_Mode      = GPIO_Mode_AF,
             .GPIO_Speed     = GPIO_Speed_100MHz,
             .GPIO_OType     = GPIO_OType_PP,
             .GPIO_PuPd      = GPIO_PuPd_NOPULL  
        },
        .afname     = GPIO_AF_ETH,
        .afmode     = ENABLE
    };
    const uint32_t mdcclockrange = 0x00000004; // MDC clock range:  bits 4:2 CR: clock range. value 1 -> HCLK = 100-168 MHz, MDC Clock = HCLK/62
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif   

    memcpy(&config, &hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDC.config, sizeof(hal_gpio_cfg_t));
    config.altcfg = (hal_gpio_altcfg_t*) &smi_altcfg;
    hal_gpio_init(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDC.gpio, &config);         

    memcpy(&config, &hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDIO.config, sizeof(hal_gpio_cfg_t));
    config.altcfg = (hal_gpio_altcfg_t*) &smi_altcfg;
    hal_gpio_init(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDIO.gpio, &config);     
    
//     hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDC, &smi_altcfg);    
//     hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDIO, &smi_altcfg);  
     
    // MDC Clock range
    ETH->MACMIIAR   = mdcclockrange;   
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


extern void hal_eth_hid_rmii_refclock_init(void)
{   // used by mac but also by external phy or switch
    
    hal_gpio_cfg_t config;
    
#if     defined(HAL_USE_CPU_FAM_STM32F1)     
    static const hal_gpio_altcfg_t rmii_clk_altcfg = 
    {
        .gpioext    =
        {
            .GPIO_Pin       = 0,
            .GPIO_Speed     = GPIO_Speed_50MHz,
            .GPIO_Mode      = GPIO_Mode_IN_FLOATING,  
        },
        .afname     = HAL_GPIO_AFNAME_NONE,
        .afmode     = HAL_GPIO_AFMODE_NONE
    };
#elif   defined(HAL_USE_CPU_FAM_STM32F4) 
    static const hal_gpio_altcfg_t rmii_clk_altcfg = 
    {
        .gpioext    =
        {
             .GPIO_Pin       = 0,
             .GPIO_Mode      = GPIO_Mode_AF,
             .GPIO_Speed     = GPIO_Speed_100MHz,
             .GPIO_OType     = GPIO_OType_PP,
             .GPIO_PuPd      = GPIO_PuPd_NOPULL  
        },
        .afname     = GPIO_AF_ETH,
        .afmode     = ENABLE
    };        
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif 
    
    memcpy(&config, &hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_REF_CLK.config, sizeof(hal_gpio_cfg_t));
    config.altcfg = (hal_gpio_altcfg_t*) &rmii_clk_altcfg;
    hal_gpio_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_REF_CLK.gpio, &config);     
    
    //hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_REF_CLK, &rmii_clk_altcfg);      
}


extern void hal_eth_hid_rmii_prepare(void)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1)    
    // step 1.
    // reset Ethernet MAC
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_ETH_MAC, ENABLE);           // or -> RCC->AHBRSTR    |= 0x00004000;              // put ethernet mac in reset mode
    // no need to do anything in here as in the stm32f4x
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_ETH_MAC, DISABLE);          // or -> RCC->AHBRSTR    &=~0x00004000;              // remove ethernet mac from reset mode
   
    // enable RMII and remap rx pins:
    GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);    // or -> AFIO->MAPR      |= (1 << 23);               // impose rmii     

    // enable clocks for ethernet (RX, TX, MAC)
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx | RCC_AHBPeriph_ETH_MAC_Rx, ENABLE); // or -> RCC->AHBENR     |= 0x0001C000;
    
#elif   defined(HAL_USE_CPU_FAM_STM32F4)

    // step 1. enable system configuration controller clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);          // RCC->APB2ENR    |= (1 << 14);  
    
    // step 2. in strict order: reset ethernet mac, set rmii (or mii) on register syscfg_pmc. only after that enable the mac clocks 
    //         see reference manual of stm32f4 in section of SYSCFG_PMC register.     
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_ETH_MAC, ENABLE);         // RCC->AHB1RSTR   |=  (1 << 25);              // put ethernet mac in reset mode
    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); // SYSCFG->PMC     |=  (1 << 23);              // impose rmii {if one wanted mii and not rmii: SYSCFG->PMC &= ~(1 << 23);}
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_ETH_MAC, DISABLE);        // RCC->AHB1RSTR   &= ~(1 << 25);              // remove ethernet mac from reset mode
    
    // step 3. enable clocks for ethernet (RX, TX, MAC) 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx | RCC_AHB1Periph_ETH_MAC_Rx, ENABLE); // RCC->AHB1ENR |= 0x0E000000;

#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif    
}



#endif//HAL_USE_ETH


// ---- removed or old functions
#if 0

#if 0 
// mode 1 is input, mode 0 is output
static int8_t s_hal_eth_gpioeth_init(stm32gpio_gpio_t gpio, uint8_t mode)
{  
#if     defined(HAL_USE_CPU_FAM_STM32F1) 
    static const GPIO_InitTypeDef ethgpioinit = 
    {
        .GPIO_Pin       = 0,
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

#elif   defined(HAL_USE_CPU_FAM_STM32F4) 
  
     static const GPIO_InitTypeDef ethgpioinit = 
     {   // safe one
         .GPIO_Pin       = 0,
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
#else //defined(HAL_USE_CPU_FAM_*)
    #error ERR --> choose a HAL_USE_CPU_FAM_*
#endif    
}
#endif

extern void hal_eth_hid_rmii_rx_init(void)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1) 
    
    static hal_gpio_altcfg_t rmii_rx_altcfg = 
    {
        .gpioext    =
        {
            .GPIO_Pin       = 0,
            .GPIO_Speed     = GPIO_Speed_50MHz,
            .GPIO_Mode      = GPIO_Mode_IN_FLOATING,  
        },
        .afname     = HAL_GPIO_AFNAME_NONE,
        .afmode     = HAL_GPIO_AFMODE_NONE
    };
        
    rmii_rx_altcfg.afname = GPIO_Remap_ETH;
    rmii_rx_altcfg.afmode = (hal_gpio_portD == hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_CRS_DV.port) ? (ENABLE) : (DISABLE);
    
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_CRS_DV, &rmii_rx_altcfg);    
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD0, &rmii_rx_altcfg);   
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD1, &rmii_rx_altcfg);    
    
// #if 0    
//     
//     AFIO->MAPR      |= (1 << 21);               // Ethernet MAC I/O remapping to: RX_DV-CRS_DV/PD8, RXD0/PD9, RXD1/PD10, RXD2/PD11, RXD3/PD12 
//  
//     // enable clock for port d in alt fun
//     RCC->APB2ENR    |= 0x00000021;
//     
//     // ETH_RMII_CRS_DV (PD8), ETH_RMII_RXD0 (PD9), ETH_RMII_RXD1 (PD10) ... as remapped by setting bit 21 of AFIO->MAPR
//     GPIOD->CRH      &= 0xFFFFF000;              // reset pd8, pd9, pd10
//     GPIOD->CRH      |= 0x00000444;              // pins configured in reset state (floating input)

// #else
//     hal_boolval_t eth_remap = (stm32gpio_portD == hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_CRS_DV.port) ? (hal_true) : (hal_false); 
//     // actually is true if also other things are satisfied ... 
//     // if true then: RX_DV-CRS_DV->PD8, RXD0->PD9, RXD1->PD10; else: RX_DV-CRS_DV->PA7, RXD0->PC4, RXD1->PC5.
//     GPIO_PinRemapConfig(GPIO_Remap_ETH, (hal_true == eth_remap) ? ENABLE : DISABLE);
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_CRS_DV, 1);
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD0, 1);
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD1, 1);    
// #endif
    
#elif   defined(HAL_USE_CPU_FAM_STM32F4)

    static const hal_gpio_altcfg_t rmii_rx_altcfg = 
    {
        .gpioext    =
        {
             .GPIO_Pin       = 0,
             .GPIO_Mode      = GPIO_Mode_AF,
             .GPIO_Speed     = GPIO_Speed_100MHz,
             .GPIO_OType     = GPIO_OType_PP,
             .GPIO_PuPd      = GPIO_PuPd_NOPULL  
        },
        .afname     = GPIO_AF_ETH,
        .afmode     = ENABLE
    };

    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_CRS_DV, &rmii_rx_altcfg);    
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD0, &rmii_rx_altcfg);   
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD1, &rmii_rx_altcfg); 

// #if 0 
//     // enable system configuration controller clock
//     RCC->APB2ENR    |= (1 << 14);  
//     
//     // clocks port a and port c
//     RCC->AHB1ENR    |= 0x00000005;
//     
//     // ETH_RMII_CRS_DV (PA7)
//     
//     GPIOA->MODER    &= ~0x0000C000;              // reset pa7
//     GPIOA->MODER    |=  0x00008000;              // alternate function
//     GPIOA->OTYPER   &= ~0x00000080;              // output push-pull (reset state) 
//     GPIOA->OSPEEDR  |=  0x0000C000;              // slew rate as 100MHz pin (0x0000C000) or 50mhz (0x00008000)
//     GPIOA->PUPDR    &= ~0x0000C000;              // no pull up, pull down 

//     GPIOA->AFR[0]   &= ~0xF0000000;
//     GPIOA->AFR[0]   |=  0xB0000000;              // AF11 (ethernet)
//   

//     // ETH_RMII_RXD0 (PC4), ETH_RMII_RXD1 (PC5)
//  
//     GPIOC->MODER   &= ~0x00000F00;              // reset pc4 and pc5
//     GPIOC->MODER   |=  0x00000A00;              // alternate function
//     GPIOC->OTYPER  &= ~0x00000030;              // output push-pull (reset state)  
//     GPIOC->OSPEEDR |=  0x00000F00;              // slew rate as 100MHz pin (0x00000F00) or 50mhz (0x00000A00)
//     GPIOC->PUPDR   &= ~0x00000F00;              // no pull up, pull down
//     GPIOC->AFR[0]  &= ~0x00FF0000;
//     GPIOC->AFR[0]  |=  0x00BB0000;              // AF11 (ethernet) 
// #else
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_CRS_DV, 0);
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD0, 0);
//    s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_RXD1, 0);   
// #endif
#endif
}

// cambiata
extern void hal_eth_hid_rmii_tx_init(void)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1) 

    static const hal_gpio_altcfg_t rmii_tx_altcfg = 
    {
        .gpioext    =
        {
            .GPIO_Pin       = 0,
            .GPIO_Speed     = GPIO_Speed_50MHz,
            .GPIO_Mode      = GPIO_Mode_AF_PP, 
        },
        .afname     = HAL_GPIO_AFNAME_NONE,
        .afmode     = HAL_GPIO_AFMODE_NONE
    };
        
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TX_EN, &rmii_tx_altcfg);    
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD0, &rmii_tx_altcfg);   
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD1, &rmii_tx_altcfg);        
    
// #if 0    
//     // enable clock for port b
//     RCC->APB2ENR    |= 0x00000009;
//   
//     //  ETH_RMII_TX_EN (PB11), ETH _RMII_TXD0 (PB12), ETH _RMII_TXD1 (PB13)
//     GPIOB->CRH      &= 0xFF000FFF;              // reset pb11, pb12, pb13
//     GPIOB->CRH      |= 0x00BBB000;              // output max 50mhz, alternate function output push-pull.
// #else
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TX_EN, 0);
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD0, 0);
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD1, 0);     
// #endif    
    
#elif   defined(HAL_USE_CPU_FAM_STM32F4)
    
    static const hal_gpio_altcfg_t rmii_tx_altcfg = 
    {
        .gpioext    =
        {
             .GPIO_Pin       = 0,
             .GPIO_Mode      = GPIO_Mode_AF,
             .GPIO_Speed     = GPIO_Speed_100MHz,
             .GPIO_OType     = GPIO_OType_PP,
             .GPIO_PuPd      = GPIO_PuPd_NOPULL  
        },
        .afname     = GPIO_AF_ETH,
        .afmode     = ENABLE
    };

    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TX_EN, &rmii_tx_altcfg);    
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD0, &rmii_tx_altcfg);   
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD1, &rmii_tx_altcfg);

// #if 0
//     if(portg)
//     {
//         // enable system configuration controller clock
//         RCC->APB2ENR    |= (1 << 14);  
//         
//         // clock port g
//         RCC->AHB1ENR    |= 0x00000040;

//         // ETH_RMII_TX_EN (PG11), ETH_RMII_TXD0 (PG13), ETH _RMII_TXD1 (PG14)
//         GPIOG->MODER   &= ~0x3CC00000;              // reset pg11, pg13, pg14
//         GPIOG->MODER   |=  0x28800000;              // alternate function 
//         GPIOG->OTYPER  &= ~0x00006800;              // output push-pull (reset state) 
//         GPIOG->OSPEEDR |=  0x3CC00000;              // slew rate as 100MHz pin (0x3CC00000) or 50mhz (0x28800000)
//         GPIOG->PUPDR   &= ~0x3CC00000;              // no pull up, pull down 

//         GPIOG->AFR[1]  &= ~0x0FF0F000;
//         GPIOG->AFR[1]  |=  0x0BB0B000;              // AF11 (ethernet) 
//     }
//     else if(portb)
//     {
//         // enable system configuration controller clock
//         RCC->APB2ENR    |= (1 << 14);  
//         
//         // clock port b
//         RCC->AHB1ENR    |= 0x00000002;

//         // ETH_RMII_TX_EN (PB11), ETH_RMII_TXD0 (PB12), ETH _RMII_TXD1 (PB13)
//         GPIOB->MODER   &= ~0x0FC00000;              // reset pb11, pb12, pb13
//         GPIOB->MODER   |=  0x0A800000;              // alternate function 
//         GPIOB->OTYPER  &= ~0x00003800;              // output push-pull (reset state) 
//         GPIOB->OSPEEDR |=  0x0FC00000;              // slew rate as 100MHz pin (0x0FC00000) or 50mhz (0x0A800000)
//         GPIOB->PUPDR   &= ~0x0FC00000;              // no pull up, pull down 

//         GPIOB->AFR[1]  &= ~0x00FFF000;
//         GPIOB->AFR[1]  |=  0x00BBB000;              // AF11 (ethernet)        
//     }
// #else
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TX_EN, 0);
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD0, 0);
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_TXD1, 0);   
// #endif
#endif
}
 


extern void hal_eth_hid_smi_init(void)
{
#if     defined(HAL_USE_CPU_FAM_STM32F1) 
    
    static const hal_gpio_altcfg_t smi_altcfg = 
    {
        .gpioext    =
        {
            .GPIO_Pin       = 0,
            .GPIO_Speed     = GPIO_Speed_50MHz,
            .GPIO_Mode      = GPIO_Mode_AF_PP, 
        },
        .afname     = HAL_GPIO_AFNAME_NONE,
        .afmode     = HAL_GPIO_AFMODE_NONE
    };
        
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDC, &smi_altcfg);    
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDIO, &smi_altcfg);  
     
    // MDC Clock range: 60-72MHz. MDC = Management data clock. (RMII signal)
    ETH->MACMIIAR   = 0x00000000;
    
// #if 0    
//     // 0. clocks port a and port c as alternate functions   
//     RCC->APB2ENR    |= 0x00000015;

//     // 1. MDC:          configure Port C ethernet pin: PC1-->MDC (Ethernet MIIM interface clock)
//     GPIOC->CRL      &= 0xFFFFFF0F;              // reset pc1
//     GPIOC->CRL      |= 0x000000B0;              // output max 50mhz, alternate function output push-pull.   
//     
//     // 2. MDIO:         configure Port A ethernet pin: PA2-->MDIO
//     GPIOA->CRL      &= 0xFFFFF0FF;              // reset pa2
//     GPIOA->CRL      |= 0x00000B00;              // output max 50mhz, alternate function output push-pull.      
//     
//     // MDC Clock range: 60-72MHz. MDC = Management data clock. (RMII signal)
//     ETH->MACMIIAR   = 0x00000000;
// #else
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDC, 0);
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDIO, 0);
//     // MDC Clock range: 60-72MHz. MDC = Management data clock. (RMII signal)
//     ETH->MACMIIAR   = 0x00000000;
// #endif    
#elif   defined(HAL_USE_CPU_FAM_STM32F4) 

    static const hal_gpio_altcfg_t smi_altcfg = 
    {
        .gpioext    =
        {
             .GPIO_Pin       = 0,
             .GPIO_Mode      = GPIO_Mode_AF,
             .GPIO_Speed     = GPIO_Speed_100MHz,
             .GPIO_OType     = GPIO_OType_PP,
             .GPIO_PuPd      = GPIO_PuPd_NOPULL  
        },
        .afname     = GPIO_AF_ETH,
        .afmode     = ENABLE
    };

    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDC, &smi_altcfg);    
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDIO, &smi_altcfg);   
   
    // MDC clock range:  bits 4:2 CR: clock range. value 1 -> HCLK = 100-168 MHz, MDC Clock = HCLK/62
    ETH->MACMIIAR   = 0x00000004;  
    
// #if 0 
//     // enable system configuration controller clock
//     RCC->APB2ENR    |= (1 << 14);  
//     
//     // 0. clocks port a and port c
//     RCC->AHB1ENR    |= 0x00000005;
//     
//     // 1. MDC:              PC1 -> ETH_MDC
//     GPIOC->MODER    &= ~0x0000000C;             // reset pc1
//     GPIOC->MODER    |=  0x00000008;             // alternate function
//     GPIOC->OTYPER   &= ~0x00000002;             // output push-pull (reset state) 
//     GPIOC->OSPEEDR  |=  0x0000000C;             // slew rate as 100MHz pin (0x0000000C) or 50mhz (0x00000008)
//     GPIOC->PUPDR    &= ~0x0000000C;             // no pull up, pull down
//     
//     GPIOC->AFR[0]   &= ~0x000000F0;
//     GPIOC->AFR[0]   |=  0x000000B0;             // AF11 (ethernet)
//     
//     
//     // 2. MDIO:             PA2 -> ETH_MDIO 
//     GPIOA->MODER    &= ~0x00000030;             // reset pa2
//     GPIOA->MODER    |=  0x00000020;             // alternate function
//     GPIOA->OTYPER   &= ~0x00000004;             // output push-pull (reset state) 
//     GPIOA->OSPEEDR  |=  0x00000030;             // slew rate as 100MHz pin (0x00000030) or 50mhz (0x00000020)
//     GPIOA->PUPDR    &= ~0x00000030;             // no pull up, pull down

//     GPIOA->AFR[0]   &= ~0x00000F00;
//     GPIOA->AFR[0]   |=  0x00000B00;             // AF11 (ethernet)
//     
//     // 3. MDC clock range:  bits 4:2 CR: clock range. value 1 -> HCLK = 100-168 MHz, MDC Clock = HCLK/62
//     ETH->MACMIIAR   = 0x00000004; 
// #else   
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDC, 0);
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_smi.ETH_MDIO, 0);    
//     // MDC clock range:  bits 4:2 CR: clock range. value 1 -> HCLK = 100-168 MHz, MDC Clock = HCLK/62
//     ETH->MACMIIAR   = 0x00000004; 
// #endif    
   
#endif    
}

// --> shall we use it for the phy as well ???
extern void hal_eth_hid_rmii_refclock_init(void)
{   // used by mac but also by external phy or switch
#if     defined(HAL_USE_CPU_FAM_STM32F1) 
    
    static const hal_gpio_altcfg_t rmii_clk_altcfg = 
    {
        .gpioext    =
        {
            .GPIO_Pin       = 0,
            .GPIO_Speed     = GPIO_Speed_50MHz,
            .GPIO_Mode      = GPIO_Mode_IN_FLOATING,  
        },
        .afname     = HAL_GPIO_AFNAME_NONE,
        .afmode     = HAL_GPIO_AFMODE_NONE
    };
        
    
    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_REF_CLK, &rmii_clk_altcfg);    


// #if 0    
//     // clock gpioa as alternate function
//     RCC->APB2ENR    |= 0x00000005;
//     
//     // init the ETH_RMII_REF_CLK (PA1)
//     GPIOA->CRL      &= 0xFFFFFF0F;              // reset pa1
//     GPIOA->CRL      |= 0x00000040;              // pin configured in reset state (floating input)
// #else
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_REF_CLK, 1);
// #endif
    
#elif   defined(HAL_USE_CPU_FAM_STM32F4) 

    static const hal_gpio_altcfg_t rmii_clk_altcfg = 
    {
        .gpioext    =
        {
             .GPIO_Pin       = 0,
             .GPIO_Mode      = GPIO_Mode_AF,
             .GPIO_Speed     = GPIO_Speed_100MHz,
             .GPIO_OType     = GPIO_OType_PP,
             .GPIO_PuPd      = GPIO_PuPd_NOPULL  
        },
        .afname     = GPIO_AF_ETH,
        .afmode     = ENABLE
    };

    hal_gpio_configure(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_REF_CLK, &rmii_clk_altcfg);    

// #if 0        
//     // enable system configuration controller clock
//     RCC->APB2ENR    |= (1 << 14);      
//     
//     // enable GPIOA clock
//     RCC->AHB1ENR    |= 0x00000001;    

//     // init the ETH_RMII_REF_CLK (PA1)
//     GPIOA->MODER    &= ~0x0000000C;              // reset pa1
//     GPIOA->MODER    |=  0x00000008;              // alternate function
//     GPIOA->OTYPER   &= ~0x00000002;              // output push-pull (reset state) 
//     GPIOA->OSPEEDR  |=  0x0000000C;              // slew rate as 100MHz pin (0x0000000C) or 50mhz (0x00000008)
//     GPIOA->PUPDR    &= ~0x0000000C;              // no pull up, pull down

//     GPIOA->AFR[0]   &= ~0x000000F0;
//     GPIOA->AFR[0]   |=  0x000000B0;              // AF11 (ethernet)    

// #else
//     s_hal_eth_gpioeth_init(hal_brdcfg_eth__theconfig.gpio_mif.rmii.ETH_RMII_REF_CLK, 0);
// #endif

#endif    
}


#endif
// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



