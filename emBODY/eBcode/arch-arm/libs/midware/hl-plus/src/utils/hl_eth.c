/*
 * Copyright (C) 2013 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
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


/* @file       hl_eth.c
	@brief      This file contains internal implementation of the hl eth utilities.
	@author     marco.accame@iit.it
    @date       10/23/2013
**/

// - modules to be built: contains the HL_USE_* macros ---------------------------------------------------------------

#include "hl_cfg_plus_modules.h"


#if     defined(HL_USE_UTIL_ETH)


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "hl_core.h"        // contains the required stm32f10x_*.h or stm32f4xx*.h header files

#include "hl_gpio.h"
#include "hl_arch.h"
#include "hl_sys.h"

#include "hl_bits.h" 

#include "hl_ethtrans.h" 

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hl_eth.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hl_eth_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------


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
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef uint32_t    U32;


typedef uint32_t (*hl_eth_array_of_buffers_t)[ETH_BUF_SIZE>>2];


typedef struct {
    uint32_t volatile Stat;
    uint32_t Ctrl;
    uint32_t Addr;
    uint32_t Next;
} hl_rx_desc_t;

typedef struct {
    uint32_t volatile CtrlStat;
    uint32_t Size;
    uint32_t Addr;
    uint32_t Next;
} hl_tx_desc_t;



typedef struct
{
    hl_eth_cfg_t                config;  
    //hl_eth_onframereception_t   onframerx;  
    uint8_t                     rxbufindex;     
    uint8_t                     txbufindex;   
    hl_rx_desc_t*               rx_desc;
    hl_tx_desc_t*               tx_desc;
    hl_eth_array_of_buffers_t   rx_buffers;
    hl_eth_array_of_buffers_t   tx_buffers;
} hl_eth_internal_item_t;


typedef struct
{
    hl_bool_t                   eth_initted;
    hl_bool_t                   smi_initted;
    hl_bool_t                   rmiirefclock_initted;
    hl_eth_internal_item_t*     items[1];
} hl_eth_theinternals_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------


static hl_bool_t s_hl_eth_supported_is(void);
static void s_hl_eth_initted_set(void);
static hl_bool_t s_hl_eth_initted_is(void);

static void s_hl_eth_rx_descr_init(void);
static void s_hl_eth_tx_descr_init(void);

static void s_hl_eth_rmii_init(void);



static void s_hl_eth_mac_reset(void);
static void s_hl_eth_mac_init(const hl_eth_cfg_t *cfg, hl_ethtrans_phymode_t phymode);

static void s_hl_eth_rmii_prepare(void);
static void s_hl_eth_rmii_rx_init(void);
extern void s_hl_eth_rmii_tx_init(void);



static void s_hl_eth_fill_gpio_smi_init_altf(   hl_gpio_init_t* mdcinit,  hl_gpio_altf_t* mdcaltf, 
                                                hl_gpio_init_t* mdioinit, hl_gpio_altf_t* mdioaltf);

static void s_hl_eth_fill_gpio_rmii_refclock_init_altf(hl_gpio_init_t* refclockinit, hl_gpio_altf_t* refclockaltf); 

static void s_hl_eth_fill_gpio_rmii_rx_init_altf(   hl_gpio_init_t* crsdvinit, hl_gpio_altf_t* crsdvaltf, 
                                                    hl_gpio_init_t* rxd0init, hl_gpio_altf_t* rxd0altf,
                                                    hl_gpio_init_t* rxd1init, hl_gpio_altf_t* rxd1altf);
                                                    
static void s_hl_eth_fill_gpio_rmii_tx_init_altf(   hl_gpio_init_t* txeninit, hl_gpio_altf_t* txenaltf, 
                                                    hl_gpio_init_t* txd0init, hl_gpio_altf_t* txd0altf,
                                                    hl_gpio_init_t* txd1init, hl_gpio_altf_t* txd1altf);                                                    

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static const variables
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static hl_eth_theinternals_t s_hl_eth_theinternals =
{
    .eth_initted            = hl_false,
    .smi_initted            = hl_false,
    .rmiirefclock_initted   = hl_false,
    .items                  = { NULL }   
};



    
// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------


extern hl_result_t hl_eth_init(const hl_eth_cfg_t *cfg)
{
    hl_ethtrans_phymode_t usedphymode = hl_ethtrans_phymode_none;
    hl_eth_internal_item_t* intitem = s_hl_eth_theinternals.items[0];

    if(hl_true != s_hl_eth_supported_is())
    {
        return(hl_res_NOK_unsupported);
    }
    
    if(hl_true == hl_eth_initted_is())
    {
        return(hl_res_NOK_generic);
    }
    
    
    uint8_t capacityoftxfifoofframes = cfg->capacityoftxfifoofframes;
    uint8_t capacityofrxfifoofframes = cfg->capacityofrxfifoofframes;


    if((NULL == cfg) || (hl_irqpriorityNONE == cfg->priority) || (0 == cfg->macaddress) )
    {
        hl_sys_on_error(hl_error_incorrectparameter, "hl_eth_init() ... ");
    }
    else if( (0 == capacityoftxfifoofframes) || (0 == capacityofrxfifoofframes))
    {
        hl_sys_on_error(hl_error_incorrectparameter, "hl_eth_init() has zero tx and/or rx queue");
    }

    if(NULL == intitem)
    {
        intitem = s_hl_eth_theinternals.items[0] = hl_sys_heap_new(sizeof(hl_eth_internal_item_t));   
    }
    
    if(NULL == intitem)
    {
        hl_sys_on_error(hl_error_generic, "hl_eth_init() did get enough heap");
    }    

    memcpy(&intitem->config, cfg, sizeof(hl_eth_cfg_t));
    intitem->config.capacityoftxfifoofframes = capacityoftxfifoofframes;
    intitem->config.capacityofrxfifoofframes = capacityofrxfifoofframes;
       
    
    // give ram to the dma descriptors
    
    intitem->rx_buffers = (hl_eth_array_of_buffers_t) hl_sys_heap_new(ETH_BUF_SIZE * capacityofrxfifoofframes);     
    intitem->tx_buffers = (hl_eth_array_of_buffers_t) hl_sys_heap_new(ETH_BUF_SIZE * capacityoftxfifoofframes);
    intitem->rx_desc = (hl_rx_desc_t*) hl_sys_heap_new(sizeof(hl_rx_desc_t) * capacityofrxfifoofframes);
    intitem->tx_desc = (hl_tx_desc_t*) hl_sys_heap_new(sizeof(hl_tx_desc_t) * capacityoftxfifoofframes);
    
    if((NULL == intitem->rx_buffers) || (NULL ==intitem->tx_buffers) || (NULL ==intitem->rx_desc) || (NULL == intitem->tx_desc))
    {
        hl_sys_on_error(hl_error_generic, "hl_eth_init() did get enough heap");
    }
    
   
    // in here we allow a specific board to init all what is related to the phy.
    // in case of a phy accessed through the smi, this function must: a. init the smi, b. reset the phy, ... that's it.
    // instead in case of a switch accessed through i2c, this function must: a. init the i2c, reset the switch, that's it.    
    if(hl_res_OK != hl_ethtrans_init(NULL))
    {
         hl_sys_on_error(hl_error_generic, "hl_eth_init() called hl_ethtrans_init() which failed");
    }        

 
    // init rmii
    s_hl_eth_rmii_init(); 


    // s_hl_eth_mac_reset();  // can be moved just after configuration of mac register  
    
    // in here we allow a specific board to configure and start the phy. with a mode (full/half) and a speed (10/100) whcih are board specifics,
    // but which we retrieve
    // in case of a phy accessed through the smi, this function must set the mode and the speed and return the result
    // instead in case of a switch accessed through i2c, this function must configure mode and speed and put the swicth operative. 
    
    // hl_ethtrans_phymode_fullduplex100mbps or hl_ethtrans_phymode_auto
    hl_ethtrans_config(&usedphymode);
    //#warning --> using hl_ethtrans_phymode_fullduplex100mbps: VERIFY ITTTTTTTTTTTTTTTTTTTT
    
    if(hl_ethtrans_phymode_none == usedphymode)
    {
        hl_sys_on_error(hl_error_generic, "hl_eth_init() called hl_ethtrans_config() which does not support a phy mode");
    }
    
    // for rmii we use the same config at the phy ...
    hl_ethtrans_phymode_t rmiiphymode = usedphymode;
    s_hl_eth_mac_init(cfg, rmiiphymode);
    

//     char str[128];
//     snprintf(str, sizeof(str), "using rmii w/ phymode %d", (int)rmiiphymode); 
//     hl_sys_on_warning(str);
      
    s_hl_eth_initted_set();
    
    return(hl_res_OK);
}


extern hl_bool_t hl_eth_initted_is(void)
{
    return(s_hl_eth_initted_is());
}

#define EXPERIMENTAL_ETH_ENABLE_DISABLE_BUT_IT_IS_TO_BE_TESTED
extern hl_result_t hl_eth_enable(void) 
{
#if     !defined(EXPERIMENTAL_ETH_ENABLE_DISABLE_BUT_IT_IS_TO_BE_TESTED)
    // acemor on 31 oct 2011: it is the same as NVIC_EnableIRQ(ETH_IRQn) or hl_sys_hid_irqn_enable()
    /* Ethernet Interrupt Enable function. */
    
    NVIC->ISER[1] = 1 << 29;
#else    
    // for __DSB() and __ISB, see: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka13559.html
    
    NVIC_ClearPendingIRQ(ETH_IRQn); // shall i clear or shall i not clear?
    
    NVIC_EnableIRQ(ETH_IRQn);
    __DSB();
    __ISB();
    
    // i need to force the isr if since last call of hl_eth_disable() the dma has moved a frame in the descriptor
    
    if(INT_RIE == (INT_RIE & ETH->DMASR))
    {
        NVIC_SetPendingIRQ(ETH_IRQn);
    }    
#endif    
    return(hl_res_OK);
}


extern hl_result_t hl_eth_disable(void) 
{
#if     !defined(EXPERIMENTAL_ETH_ENABLE_DISABLE_BUT_IT_IS_TO_BE_TESTED)
    // acemor on 31 oct 2011: it is the same as NVIC_DisableIRQ(ETH_IRQn) or hl_sys_hid_irqn_disable()
    /* Ethernet Interrupt Disable function. */
    
    NVIC->ICER[1] = 1 << 29;
#else      
    // for __DSB() and __ISB, see: http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.faqs/ka13559.html
    
    NVIC_DisableIRQ(ETH_IRQn);
    __DSB();
    __ISB();
#endif  
    return(hl_res_OK);
}


extern void hl_eth_smi_init(void)
{   
    if(hl_true == s_hl_eth_theinternals.smi_initted)
    {
        return;
    }

    hl_gpio_init_t gpio_init_mdc;    
    hl_gpio_altf_t gpio_altf_mdc;    
    hl_gpio_init_t gpio_init_mdio;
    hl_gpio_altf_t gpio_altf_mdio;   

    

    s_hl_eth_fill_gpio_smi_init_altf(   &gpio_init_mdc,  &gpio_altf_mdc, 
                                        &gpio_init_mdio, &gpio_altf_mdio);
    
    hl_gpio_init(&gpio_init_mdc);
    hl_gpio_altf(&gpio_altf_mdc);
    
    hl_gpio_init(&gpio_init_mdio);
    hl_gpio_altf(&gpio_altf_mdio);
    
    
#if     defined(HL_USE_MPU_ARCH_STM32F1)    
    const uint32_t mdcclockrange = 0x00000000; // MDC Clock range: 60-72MHz. MDC = Management data clock. (RMII signal)
#elif   defined(HL_USE_MPU_ARCH_STM32F4) 
    const uint32_t mdcclockrange = 0x00000004; // MDC clock range:  bits 4:2 CR: clock range. value 1 -> HCLK = 100-168 MHz, MDC Clock = HCLK/62
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif 
    // MDC Clock range
    ETH->MACMIIAR   = mdcclockrange;   


    s_hl_eth_theinternals.smi_initted = hl_true;
   
}


// reads the 16 bits of register REGaddr in the physical with address PHYaddr. both REGaddr and PHYaddr range is 0-31
extern uint16_t hl_eth_smi_read(uint8_t PHYaddr, uint8_t REGaddr)
{
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    if(hl_false == s_hl_eth_theinternals.smi_initted)
    {
        return(0);
    } 
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)   
    
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
extern void hl_eth_smi_write(uint8_t PHYaddr, uint8_t REGaddr, uint16_t value)
{ 
#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)
    if(hl_false == s_hl_eth_theinternals.smi_initted)
    {
        return;
    } 
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)  
    
    uint32_t tout = 0;   
    
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




extern void hl_eth_rmii_refclock_init(void)
{   // used by mac but also by external phy or switch
    if(hl_true == s_hl_eth_theinternals.rmiirefclock_initted)
    {
        return;
    }
    
    hl_gpio_init_t gpio_init_refclock;    
    hl_gpio_altf_t gpio_altf_refclock;

    s_hl_eth_fill_gpio_rmii_refclock_init_altf(&gpio_init_refclock, &gpio_altf_refclock);
    
    hl_gpio_init(&gpio_init_refclock);
    hl_gpio_altf(&gpio_altf_refclock);    
}



extern hl_result_t hl_eth_sendframe(hl_eth_frame_t *frame)
{
    // called by tcpnet's send_frame(OS_FRAME *frame)
    
    // il caller passa (U32 *)&frame->data[0] e frame->length

    /* Send frame to ETH ethernet controller */
    U32 *sp,*dp;
    U32 i,j;
    
    hl_eth_internal_item_t* intitem = s_hl_eth_theinternals.items[0];

#if     !defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)  
    if(hl_true != s_hl_eth_initted_is())
    {
        return(hl_res_NOK_generic);
    }
#endif//!defined(HL_BEH_REMOVE_RUNTIME_PARAM_CHECK)    

    j = intitem->txbufindex; 
    /* Wait until previous packet transmitted. */
    while (intitem->tx_desc[j].CtrlStat & DMA_TX_OWN);

    sp = (U32 *)&frame->datafirstbyte[0];
    dp = (U32 *)(intitem->tx_desc[j].Addr & ~3);


    /* Copy frame data to ETH IO buffer. */
    for (i = (frame->length + 3) >> 2; i; i--) {
        *dp++ = *sp++;
    }

    intitem->tx_desc[j].Size      = frame->length;
    intitem->tx_desc[j].CtrlStat |= DMA_TX_OWN;    
    if (++j == intitem->config.capacityoftxfifoofframes) j = 0;
    intitem->txbufindex = j; 
    /* Start frame transmission. */
    ETH->DMASR   = DSR_TPSS;
    ETH->DMATPDR = 0;

    return(hl_res_OK);    
}


__weak extern hl_eth_frame_t* hl_eth_frame_new(uint32_t len)
{
    return(NULL);
}

__weak extern void hl_eth_on_frame_received(hl_eth_frame_t* frame)
{
    
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

void ETH_IRQHandler(void) 
{
    hl_eth_internal_item_t* intitem = s_hl_eth_theinternals.items[0];
    
  // questa non va cambiata. pero' .... usa OS_FRAME e chiama alloc_mem() e put_in_queue().
  // bisogna usare alcuni puntatori a funzione che siano inizializzati / passati in hl_eth_init().
  // inoltre, se si vuole segnalare l'arrivo di un frame gia' qui dentro, allora bisogna definire
  // un altro puntatore a funzione hl_on_rx_frame().

    uint8_t* rxframedata = NULL;
    uint16_t rxframesize = 0;

  /* Ethernet Controller Interrupt function. */
  hl_eth_frame_t *frame;
  U32 i,RxLen;
  volatile U32 int_stat;
  U32 *sp,*dp;


  
  while (((int_stat = ETH->DMASR) & INT_NISE) != 0) {   // -> read dma status register until we have the normal interrupt bit set
    ETH->DMASR = int_stat;                              // -> reset the dma status register
    if (int_stat & INT_RIE) {                           // -> in case of received frame ...
      /* Valid frame has been received. */
      i = intitem->rxbufindex; //RxBufIndex;
      if (intitem->rx_desc[i].Stat & DMA_RX_ERROR_MASK) {  
        goto rel;
      }

      if ((intitem->rx_desc[i].Stat & DMA_RX_SEG_MASK) != DMA_RX_SEG_MASK) {
        goto rel;
      }
      
      RxLen = ((intitem->rx_desc[i].Stat >> 16) & 0x3FFF) - 4;   // -> retrieve the length of teh frame
      rxframesize = RxLen;
      if (RxLen > ETH_MTU) {
        /* Packet too big, ignore it and free buffer. */
        goto rel;
      }
      /* Flag 0x80000000 to skip sys_error() call when out of memory. */
      //frame = intitem->onframerx.frame_new(RxLen | 0x80000000);
      frame = hl_eth_frame_new(RxLen | 0x80000000);
      /* if 'frame_new()' has failed, ignore this packet. */
      if (frame != NULL) {
        sp = (U32 *)(intitem->rx_desc[i].Addr & ~3);
        dp = (U32 *)&frame->datafirstbyte[0];
        rxframedata = (uint8_t*)sp;
        for (RxLen = (RxLen + 3) >> 2; RxLen; RxLen--) {
          *dp++ = *sp++;
        }

//        intitem->onframerx.frame_movetohigherlayer(frame);
//         if(NULL != intitem->onframerx.frame_alerthigherlayer)
//         {
//             intitem->onframerx.frame_alerthigherlayer();
//         }
        
        // it moves and alerts 
        hl_eth_on_frame_received(frame);
        
      }
      else
      {
          rxframedata = 0;          
      }
      
//     if(NULL != hl_eth_hid_DEBUG_support.fn_inside_eth_isr)
//     {            
//         hl_eth_hid_DEBUG_support.fn_inside_eth_isr(rxframedata, rxframesize);
//     }
        
      /* Release this frame from ETH IO buffer. */

rel:  intitem->rx_desc[i].Stat = DMA_RX_OWN;
    
      
    if (++i == intitem->config.capacityofrxfifoofframes) i = 0;
      intitem->rxbufindex = i; 
    }
    if (int_stat & INT_TIE) {
      /* Frame transmit completed. */

    }
  }             // -> we execute a new cycle only if in the execution time of the loop a new interrupt has arrived. 

}

// ---- isr of the module: end ------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


static hl_bool_t s_hl_eth_supported_is(void)
{
    if(NULL == hl_eth_map)
    {
        return(hl_false);
    }
    return(hl_eth_map->supported);
}

static void s_hl_eth_initted_set(void)
{
    s_hl_eth_theinternals.eth_initted = hl_true;
}

static hl_bool_t s_hl_eth_initted_is(void)
{
    return(s_hl_eth_theinternals.eth_initted);
}



static void s_hl_eth_rmii_init(void)
{
    
    s_hl_eth_rmii_prepare();

    s_hl_eth_rmii_rx_init(); 
    s_hl_eth_rmii_tx_init();    
    
    // cannot remove hl_eth_rmii_refclock_init() from here. it is unclear if it can be used also inside the hl_brdcfg_eth__phy_start()
    hl_eth_rmii_refclock_init();
    
}



static void s_hl_eth_mac_reset(void)
{
    // software reset of the mac: wait until done
    // When this bit is set, the MAC DMA controller resets all MAC Subsystem internal registers
    // and logic. It is cleared automatically after the reset operation has completed in all of the core
    // clock domains. Read a 0 value in this bit before re-programming any register of the core.
    ETH->DMABMR  |= 0x00000001;
    while(ETH->DMABMR & 0x00000001); 
}



static void s_hl_eth_mac_init(const hl_eth_cfg_t *cfg, hl_ethtrans_phymode_t phymode)
{
    uint8_t *macaddr = (uint8_t*)&(cfg->macaddress);
    // reset mac register
    s_hl_eth_mac_reset(); 
    
    // initialise mac control register
    ETH->MACCR  = 0x00008000;       // clear
    
    
    // init phymode
  
    if((hl_ethtrans_phymode_halfduplex100mbps == phymode) || (hl_ethtrans_phymode_fullduplex100mbps == phymode))
    {
        ETH->MACCR |= MCR_FES;              // config 100mbs 
    }
//     else
//     {
//         ETH->MACCR &= ~MCR_FES;             // nothing needed for 10 mbps
//     }
    
    if((hl_ethtrans_phymode_fullduplex10mbps == phymode) || (hl_ethtrans_phymode_fullduplex100mbps == phymode))
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
    ETH->MACFCR = MFCR_ZQPD;   // zero-quanta pause disable
    
    /* Set the Ethernet MAC Address registers */
    ETH->MACA0HR = ((uint32_t)macaddr[5] <<  8) | (uint32_t)macaddr[4];
    ETH->MACA0LR = ((uint32_t)macaddr[3] << 24) | (uint32_t)macaddr[2] << 16 | ((uint32_t)macaddr[1] <<  8) | (uint32_t)macaddr[0];
   
   
    /* Initialize Tx and Rx DMA Descriptors */
    s_hl_eth_rx_descr_init();
    s_hl_eth_tx_descr_init();
    
    /* Flush FIFO, start DMA Tx and Rx */
    ETH->DMAOMR = DOMR_FTF | DOMR_ST | DOMR_SR;
    
    /* Enable receiver and transmitter */
    ETH->MACCR |= MCR_TE | MCR_RE;
    
    /* Reset all interrupts */
    ETH->DMASR  = 0xFFFFFFFF;
    
    /* Enable Rx and Tx interrupts. */
    ETH->DMAIER = INT_NISE | INT_AISE | INT_RBUIE | INT_RIE;

    // acemor on 31 oct 2011: added priority support for ETH_IRQn
    hl_sys_irqn_priority_set(ETH_IRQn, cfg->priority);    
}


static void s_hl_eth_rx_descr_init(void) 
{
    uint32_t i,next;
    hl_eth_internal_item_t* intitem = s_hl_eth_theinternals.items[0];

    intitem->rxbufindex = 0;
    for(i=0, next=0; i<intitem->config.capacityofrxfifoofframes; i++) 
    {
        if(++next == intitem->config.capacityofrxfifoofframes) 
            next = 0;
        intitem->rx_desc[i].Stat = DMA_RX_OWN;
        intitem->rx_desc[i].Ctrl = DMA_RX_RCH | ETH_BUF_SIZE;
        intitem->rx_desc[i].Addr = (uint32_t)&intitem->rx_buffers[i];
        intitem->rx_desc[i].Next = (uint32_t)&intitem->rx_desc[next];
    }
    ETH->DMARDLAR = (uint32_t)&intitem->rx_desc[0];
}


static void s_hl_eth_tx_descr_init(void) 
{
    uint32_t i,next;
    hl_eth_internal_item_t* intitem = s_hl_eth_theinternals.items[0];

    intitem->txbufindex = 0; //TxBufIndex = 0;
    for(i=0, next=0; i<intitem->config.capacityoftxfifoofframes; i++) 
    {
        if(++next == intitem->config.capacityoftxfifoofframes) 
            next = 0;
        intitem->tx_desc[i].CtrlStat = DMA_TX_TCH | DMA_TX_LS | DMA_TX_FS;
        intitem->tx_desc[i].Addr     = (uint32_t)&intitem->tx_buffers[i];
        intitem->tx_desc[i].Next     = (uint32_t)&intitem->tx_desc[next];
    }
    ETH->DMATDLAR = (uint32_t)&intitem->tx_desc[0];
}

static void s_hl_eth_rmii_rx_init(void)
{
    hl_gpio_init_t gpio_init_crsdv;     
    hl_gpio_altf_t gpio_altf_crsdv;   
    hl_gpio_init_t gpio_init_rxd0;     
    hl_gpio_altf_t gpio_altf_rxd0;    
    hl_gpio_init_t gpio_init_rxd1; 
    hl_gpio_altf_t gpio_altf_rxd1;
    

    s_hl_eth_fill_gpio_rmii_rx_init_altf(   &gpio_init_crsdv, &gpio_altf_crsdv, 
                                            &gpio_init_rxd0,  &gpio_altf_rxd0,
                                            &gpio_init_rxd1,  &gpio_altf_rxd1);
    
    hl_gpio_init(&gpio_init_crsdv);
    hl_gpio_altf(&gpio_altf_crsdv);
    
    hl_gpio_init(&gpio_init_rxd0);
    hl_gpio_altf(&gpio_altf_rxd0);
    
    hl_gpio_init(&gpio_init_rxd1);
    hl_gpio_altf(&gpio_altf_rxd1);    
    
}

static void s_hl_eth_rmii_tx_init(void)
{
    hl_gpio_init_t gpio_init_txen;      
    hl_gpio_altf_t gpio_altf_txen;  
    hl_gpio_init_t gpio_init_txd0;     
    hl_gpio_altf_t gpio_altf_txd0;    
    hl_gpio_init_t gpio_init_txd1; 
    hl_gpio_altf_t gpio_altf_txd1;
    

    s_hl_eth_fill_gpio_rmii_tx_init_altf(   &gpio_init_txen, &gpio_altf_txen, 
                                            &gpio_init_txd0,  &gpio_altf_txd0,
                                            &gpio_init_txd1,  &gpio_altf_txd1);
    
    hl_gpio_init(&gpio_init_txen);
    hl_gpio_altf(&gpio_altf_txen);
    
    hl_gpio_init(&gpio_init_txd0);
    hl_gpio_altf(&gpio_altf_txd0);
    
    hl_gpio_init(&gpio_init_txd1);
    hl_gpio_altf(&gpio_altf_txd1);    
    
}
 



static void s_hl_eth_rmii_prepare(void)
{
#if     defined(HL_USE_MPU_ARCH_STM32F1)    
    // step 1.
    // reset Ethernet MAC
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_ETH_MAC, ENABLE);           // or -> RCC->AHBRSTR    |= 0x00004000;              // put ethernet mac in reset mode
    // no need to do anything in here as in the stm32f4x
    RCC_AHBPeriphResetCmd(RCC_AHBPeriph_ETH_MAC, DISABLE);          // or -> RCC->AHBRSTR    &=~0x00004000;              // remove ethernet mac from reset mode
   
    // enable RMII and remap rx pins:
    GPIO_ETH_MediaInterfaceConfig(GPIO_ETH_MediaInterface_RMII);    // or -> AFIO->MAPR      |= (1 << 23);               // impose rmii     

    // enable clocks for ethernet (RX, TX, MAC)
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx | RCC_AHBPeriph_ETH_MAC_Rx, ENABLE); // or -> RCC->AHBENR     |= 0x0001C000;
    
#elif   defined(HL_USE_MPU_ARCH_STM32F4)

    // step 1. enable system configuration controller clock
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);          // RCC->APB2ENR    |= (1 << 14);  
    
    // step 2. in strict order: reset ethernet mac, set rmii (or mii) on register syscfg_pmc. only after that enable the mac clocks 
    //         see reference manual of stm32f4 in section of SYSCFG_PMC register.     
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_ETH_MAC, ENABLE);         // RCC->AHB1RSTR   |=  (1 << 25);              // put ethernet mac in reset mode
    SYSCFG_ETH_MediaInterfaceConfig(SYSCFG_ETH_MediaInterface_RMII); // SYSCFG->PMC     |=  (1 << 23);              // impose rmii {if one wanted mii and not rmii: SYSCFG->PMC &= ~(1 << 23);}
    RCC_AHB1PeriphResetCmd(RCC_AHB1Periph_ETH_MAC, DISABLE);        // RCC->AHB1RSTR   &= ~(1 << 25);              // remove ethernet mac from reset mode
    
    // step 3. enable clocks for ethernet (RX, TX, MAC) 
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_ETH_MAC | RCC_AHB1Periph_ETH_MAC_Tx | RCC_AHB1Periph_ETH_MAC_Rx, ENABLE); // RCC->AHB1ENR |= 0x0E000000;

#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif    
}


static void s_hl_eth_fill_gpio_rmii_rx_init_altf(   hl_gpio_init_t* crsdvinit, hl_gpio_altf_t* crsdvaltf, 
                                                    hl_gpio_init_t* rxd0init, hl_gpio_altf_t* rxd0altf,
                                                    hl_gpio_init_t* rxd1init, hl_gpio_altf_t* rxd1altf)
{
    static const hl_gpio_init_t s_hl_eth_rmiirx_gpio_init  =
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)  
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_mode      = GPIO_Mode_IN_FLOATING        
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)  
        .port        = hl_gpio_portNONE,
        .mode        =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_mode      = GPIO_Mode_AF,
            .gpio_speed     = GPIO_Speed_100MHz,
            .gpio_otype     = GPIO_OType_PP,
            .gpio_pupd      = GPIO_PuPd_NOPULL
        }
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif         
    }; 
    
    // at first we copy the default configuration of crsdv, rxd0, rxd1
    
    memcpy(crsdvinit, &s_hl_eth_rmiirx_gpio_init, sizeof(hl_gpio_init_t));   
    memcpy(rxd0init,  &s_hl_eth_rmiirx_gpio_init, sizeof(hl_gpio_init_t));
    memcpy(rxd1init,  &s_hl_eth_rmiirx_gpio_init, sizeof(hl_gpio_init_t));
    
    // then we verify the pin mapping and the altfun ... ok don't do it.
    // but you could put it in here. maybe by calling an external function which depends on the mpu
    
    // then we set the port and pin of crsdv, rxd0, rxd1
    hl_gpio_fill_init(crsdvinit, &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_CRS_DV);
    hl_gpio_fill_init(rxd0init,  &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_RXD0);
    hl_gpio_fill_init(rxd1init,  &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_RXD1);
    
    // then we set altfun of crsdv, rxd0, rxd1
    hl_gpio_fill_altf(crsdvaltf, &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_CRS_DV);
    hl_gpio_fill_altf(rxd0altf,  &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_RXD0);    
    hl_gpio_fill_altf(rxd1altf,  &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_RXD1);     
}


static void s_hl_eth_fill_gpio_rmii_tx_init_altf(   hl_gpio_init_t* txeninit, hl_gpio_altf_t* txenaltf, 
                                                    hl_gpio_init_t* txd0init, hl_gpio_altf_t* txd0altf,
                                                    hl_gpio_init_t* txd1init, hl_gpio_altf_t* txd1altf)
{
    static const hl_gpio_init_t s_hl_eth_rmiitx_gpio_init  =
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)  
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_mode      = GPIO_Mode_AF_PP        
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)  
        .port        = hl_gpio_portNONE,
        .mode        =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_mode      = GPIO_Mode_AF,
            .gpio_speed     = GPIO_Speed_100MHz,
            .gpio_otype     = GPIO_OType_PP,
            .gpio_pupd      = GPIO_PuPd_NOPULL
        }
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif         
    }; 
    
    // at first we copy the default configuration of txen, txd0, txd1
    
    memcpy(txeninit, &s_hl_eth_rmiitx_gpio_init, sizeof(hl_gpio_init_t));   
    memcpy(txd0init,  &s_hl_eth_rmiitx_gpio_init, sizeof(hl_gpio_init_t));
    memcpy(txd1init,  &s_hl_eth_rmiitx_gpio_init, sizeof(hl_gpio_init_t));
    
    // then we verify the pin mapping and the altfun ... ok don't do it.
    // but you could put it in here. maybe by calling an external function which depends on the mpu
    
    // then we set the port and pin of txen, txd0, txd1
    hl_gpio_fill_init(txeninit, &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_TX_EN);
    hl_gpio_fill_init(txd0init,  &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_TXD0);
    hl_gpio_fill_init(txd1init,  &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_TXD1);
    
    // then we set altfun of txen, txd0, txd1
    hl_gpio_fill_altf(txenaltf, &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_TX_EN);
    hl_gpio_fill_altf(txd0altf,  &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_TXD0);    
    hl_gpio_fill_altf(txd1altf,  &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_TXD1);     
}


static void s_hl_eth_fill_gpio_rmii_refclock_init_altf(hl_gpio_init_t* refclockinit, hl_gpio_altf_t* refclockaltf)
{
    static const hl_gpio_init_t s_hl_eth_rmiirefclock_gpio_init  =
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)  
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_mode      = GPIO_Mode_IN_FLOATING        
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)  
        .port        = hl_gpio_portNONE,
        .mode        =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_mode      = GPIO_Mode_AF,
            .gpio_speed     = GPIO_Speed_100MHz,
            .gpio_otype     = GPIO_OType_PP,
            .gpio_pupd      = GPIO_PuPd_NOPULL
        }
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif         
    }; 
    
    // at first we copy the default configuration of refclock
    
    memcpy(refclockinit, &s_hl_eth_rmiirefclock_gpio_init, sizeof(hl_gpio_init_t));   
    
    // then we verify the pin mapping and the altfun ... ok don't do it.
    // but you could put it in here. maybe by calling an external function which depends on the mpu
    
    // then we set the port and pin of refclock
    hl_gpio_fill_init(refclockinit, &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_REF_CLK);

    
    // then we set altfun of refclock
    hl_gpio_fill_altf(refclockaltf, &hl_eth_map->gpiomap.gpio_mif.rmii.ETH_RMII_REF_CLK);
}


static void s_hl_eth_fill_gpio_smi_init_altf(   hl_gpio_init_t* mdcinit,  hl_gpio_altf_t* mdcaltf, 
                                                hl_gpio_init_t* mdioinit, hl_gpio_altf_t* mdioaltf)
{
    static const hl_gpio_init_t s_hl_eth_smi_gpio_init  =
    {
#if     defined(HL_USE_MPU_ARCH_STM32F1)  
        .port           = hl_gpio_portNONE,
        .mode           =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_speed     = GPIO_Speed_50MHz,
            .gpio_mode      = GPIO_Mode_AF_PP        
        }
#elif   defined(HL_USE_MPU_ARCH_STM32F4)  
        .port        = hl_gpio_portNONE,
        .mode        =
        {
            .gpio_pins      = hl_gpio_pinNONE,
            .gpio_mode      = GPIO_Mode_AF,
            .gpio_speed     = GPIO_Speed_100MHz,
            .gpio_otype     = GPIO_OType_PP,
            .gpio_pupd      = GPIO_PuPd_NOPULL
        }
#else //defined(HL_USE_MPU_ARCH_*)
    #error ERR --> choose a HL_USE_MPU_ARCH_*
#endif         
    }; 
    
    // at first we copy the default configuration of mdc and mdio
    memcpy(mdcinit,  &s_hl_eth_smi_gpio_init, sizeof(hl_gpio_init_t));   
    memcpy(mdioinit, &s_hl_eth_smi_gpio_init, sizeof(hl_gpio_init_t));
    
    // then we verify the pin mapping and the altfun ... ok don't do it.
    // but you could put it in here. maybe by calling an external function which depends on the mpu
    
    // then we set the port and pin of mdc and mdio
    hl_gpio_fill_init(mdcinit,  &hl_eth_map->gpiomap.gpio_smi.ETH_MDC);
    hl_gpio_fill_init(mdioinit, &hl_eth_map->gpiomap.gpio_smi.ETH_MDIO);
    
    // then we set altfun of mdc and mdio
    hl_gpio_fill_altf(mdcaltf,  &hl_eth_map->gpiomap.gpio_smi.ETH_MDC);
    hl_gpio_fill_altf(mdioaltf, &hl_eth_map->gpiomap.gpio_smi.ETH_MDIO);       
}


#endif//defined(HL_USE_UTIL_ETH)

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



