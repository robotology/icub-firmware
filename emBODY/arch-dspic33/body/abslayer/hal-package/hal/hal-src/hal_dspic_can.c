/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Marco Accame
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

/* @file       hal_dspic_can.c
    @brief      This file implements internal implementation of the hal can module.
    @author     valentina.gaggero@iit.it, marco.accame@iit.it
    @date       05/27/2010
**/

// - modules to be built: contains the HAL_USE_* macros ---------------------------------------------------------------
#include "hal_brdcfg_modules.h"

#ifdef HAL_USE_CAN

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"

#include "p33fxxxx.h"
#include <libpic30.h>

#include "string.h"

#include "hal_brdcfg.h"
#include "hal_dspic_base_hid.h" 
#include "hal_sys.h"
#include "hal_arch_dspic.h" 


 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal_can.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------

#include "hal_dspic_can_hid.h" 

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define HAL_can_port2index(p)                   ((uint8_t)((p)))


// CAN Baud Rate Configuration        
#define HAL_CAN_FREQ                            40000000
#define HAL_CAN_NTQ                             20         // 20 Time Quanta in a Bit Time

#define HAL_CAN_BAUDRATE_1Mbps                  1000000 
#define HAL_CAN_BAUDRATE_500Kbps                500000  
#define HAL_CAN_BR_VAL_1Mbps                    ((HAL_CAN_FREQ/(2*HAL_CAN_NTQ*HAL_CAN_BAUDRATE_1Mbps))-1)
#define HAL_CAN_BR_VAL_500kbps                  ((HAL_CAN_FREQ/(2*HAL_CAN_NTQ*HAL_CAN_BAUDRATE_500Kbps))-1) //never tested

//#define HAL_CAN_DSPIC_DEF_QUEUE_CFG_INDEX_NULL    255

#define HAL_CAN_DMA_MSGBUF_NUM                  32 // se lo modifichi modifica anche la config della fifo dma

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

const hal_can_cfg_t hal_can_cfg_default =
{
    .runmode            = hal_can_runmode_isr_1txq1rxq,
    .baudrate           = hal_can_baudrate_1mbps, 
    .priorx             = hal_int_priority06,
    .priotx             = hal_int_priority06,
    .callback_on_rx     = NULL,
    .arg_cb_rx          = NULL,
    .callback_on_tx     = NULL,
    .arg_cb_tx          = NULL
};


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint8_t rxqnorm_num;
    uint8_t txqnorm_num;
    uint8_t txqhigh_num;
} hal_can_dspic_queue_cfg_t;


typedef struct
{
    uint16_t IDE:1;
    uint16_t SRR:1;
    uint16_t SID:11;        // SID, 11 bit id max of 0x7FF
    uint16_t unused1:3;

    uint16_t EID6to17:12;   // EID bit from 6 to 17
    uint16_t unused2:4;

    uint16_t DLC:4;         // Data Length Code
    uint16_t RB0:1;         // reserved
    uint16_t unused3:3;
    uint16_t RB1:1;         // reserved
    uint16_t RTR:1;         // remote trans. req.
    uint16_t EID0to5:6;     // EID bit from 0 to 5

    uint8_t data[8];

    uint16_t unused4:8;
    uint16_t FILHIT:5;      // FILter HIT number
    uint16_t unused5:3;
} hal_can_dspic_DMAframe_t;



/** @typedef    struct hal_can_DMAqueue_info_t
    @brief        contains info about a queue, but not memory where queue's item can be saved.
                we use DMARAM already allocated.    
**/
typedef struct
{
    uint8_t capacity;
    uint8_t size;
    uint8_t index_in;       //index  where insert frame
    uint8_t index_out;      //index  where remove frame
} hal_can_DMAqueue_info_t;


typedef struct
{
    hal_can_cfg_t               cfg;
    //hal_can_DMAqueue_info_t     canframes_rx_norm; the hw save these info for us.
    hal_can_DMAqueue_info_t     canframes_tx_norm;
    hal_can_DMAqueue_info_t     canframes_tx_high;
    uint8_t                     enabled;
} hal_can_portdatastructure_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_can_supported_is(hal_can_port_t port);
static void s_hal_can_initted_set(hal_can_port_t port);
static hal_boolval_t s_hal_can_initted_is(hal_can_port_t port);

// init and start CAN
static void s_hal_can_clk_init(hal_can_port_t port, hal_can_baudrate_t br_val);
static void s_hal_can_dmaqueue_info_init(hal_can_port_t port);
static void s_hal_can_dma_init(hal_can_port_t port);

// trasmission
static hal_result_t s_hal_can_isr_sendframes_canx(hal_can_port_t port);
static void s_hal_can_ena_dmabuff_withPrio(hal_can_port_t port, uint8_t buf_index);
static void s_hal_can_copy_frame_inDMA(hal_can_frame_t *frame, uint8_t dmaBuff_index);
static void s_hal_can_ena_dmabuff(uint8_t buffer_num);


// reception
static hal_result_t s_hal_can_copy_frame_fromDMA(hal_can_frame_t *frame, uint8_t dmaBuff_index);
static hal_result_t s_hal_can_rxFilter_set_mask(uint8_t mask_num, uint32_t mask_val, hal_can_frameID_format_t idformat);
static hal_result_t s_hal_can_rxFilter_set_id( uint8_t id_num, int32_t id_val, hal_can_frameID_format_t idformat, uint8_t mask_sel);



static hal_result_t s_hal_can_tx_withoutisr(hal_can_port_t port, hal_can_frame_t *frame);
static hal_result_t s_hal_can_tx_normprio(hal_can_port_t port, hal_can_frame_t *frame, hal_can_send_mode_t sm);
static hal_result_t s_hal_can_tx_highprio(hal_can_port_t port, hal_can_frame_t *frame);


static void s_hal_canperipheral_isr_tx_enable(hal_can_port_t port);
static void s_hal_canperipheral_isr_tx_disable(hal_can_port_t port);
static void s_hal_can_dma_rx_enable(hal_can_port_t port);
static void s_hal_can_dma_rx_disable(hal_can_port_t port);

static uint8_t s_hal_can_dma_rx_size(hal_can_port_t port);


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

/* The following values are required to work properly on dspic*/
static const hal_can_dspic_queue_cfg_t s_dspic_default_queue_cfg [hal_can_runmode_num] = 
{
    //hal_can_runmode_noisr_0q
    {
        .rxqnorm_num = 31,
        .txqnorm_num = 1,
        .txqhigh_num = 0,
    },

    //hal_can_runmode_isr_1txq1rxq
    {
        .rxqnorm_num = 28,
        .txqnorm_num = 4,
        .txqhigh_num = 0,
    },

    //hal_can_runmode_isr_2txq1rxq
    {
        .rxqnorm_num = 25,
        .txqnorm_num = 4,
        .txqhigh_num = 3,
    }
};

//this variable contains the default configuration to use, after user configuration has been checked.
//it is checked in hal_can_hid_getsize(..) 
//static uint8_t s_default_queue_cfg_index = HAL_CAN_DSPIC_DEF_QUEUE_CFG_INDEX_NULL;

static hal_can_portdatastructure_t *s_hal_can_portdatastruct_ptr[hal_can_ports_num] = {NULL, NULL}; //s_hal_can_portdatastruct_ptr

static hal_boolval_t s_hal_can_initted[hal_can_ports_num] = { hal_false, hal_false };

/* alloc buffer rx and tx in DMA */
static hal_can_dspic_DMAframe_t s_hal_can_dmabuffer[HAL_CAN_DMA_MSGBUF_NUM]__attribute__((space(dma),aligned(HAL_CAN_DMA_MSGBUF_NUM * (sizeof(hal_can_dspic_DMAframe_t)))));

static const uint8_t s_can_dmabuff_prio_tbl[] = {0, 1, 2, 3};

static uint32_t s_can_rxdmamask =  0;




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

extern hal_result_t hal_can_init(hal_can_port_t port, const hal_can_cfg_t *cfg)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    if(hal_false == s_hal_can_supported_is(port))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == cport)
    {
        //If i'm here means memory has not been allocated because hal config has wrong params.
        //remember: queue size can not be modify. see file....
        return(hal_res_NOK_generic);
    }

    if(NULL == cfg)
    {
        cfg  = &hal_can_cfg_default;
    }
    
    // set config
    memcpy(&cport->cfg, cfg, sizeof(hal_can_cfg_t));

    s_hal_can_dmaqueue_info_init(port);
   
    // init low level
    hal_brdcfg_can__phydevices_enable(port); /* Enable physical can device */

    /* put can module in configuration mode */
    C1CTRL1bits.REQOP = 4;
    while(C1CTRL1bits.OPMODE != 4);

    s_hal_can_clk_init(port, cport->cfg.baudrate);
    s_hal_can_dma_init(port);
    
    switch(cport->cfg.runmode)
    {
        case hal_can_runmode_noisr_0q:
        {
            s_can_rxdmamask = 0xFFFFFFFE;
        } break;
        
        case hal_can_runmode_isr_1txq1rxq:
        {
            s_can_rxdmamask = 0xFFFFFFF0;
        } break;
        
        case hal_can_runmode_isr_2txq1rxq:
        {
            s_can_rxdmamask = 0xFFFFFF80;
        } break;
        
        default:
        {
        
        } break;
    }

    if(hal_can_runmode_noisr_0q != cport->cfg.runmode)
    {
         /* set can interrupt priority for tx*/
        //IPC8bits.C1IP = cport->cfg.priotx;
        hal_sys_irqn_priority_set(hal_arch_dspic_CAN1_IRQn, cport->cfg.priotx);

        /* set dma interrupt for rx. don't consider rx can interrupt, but only dma rx interrupt*/
        //IPC1bits.DMA0IP = cport->cfg.priorx; // acemor: for rx we use dam1. thus DMA0IP is wrong.
        hal_sys_irqn_priority_set(hal_arch_dspic_DMA1_IRQn, cport->cfg.priorx); 
    }
    
    /* put can module in normal mode */
    C1CTRL1bits.REQOP = 0;
    while(C1CTRL1bits.OPMODE != 0);
    

    cport->enabled = 0;  
    
    
    s_hal_can_initted_set(port);

    //Add filter to receive each message. if no filter are set, no message are recieved.
    hal_can_receptionfilter_set(hal_can_port1, 0, 0x000, 0, 0x000, hal_can_frameID_std); 

    return(hal_res_OK);
}


extern hal_result_t hal_can_enable(hal_can_port_t port)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    if(hal_false == s_hal_can_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    // disable scheduling
    hal_base_hid_osal_scheduling_suspend();
    
    cport->enabled = 1;
    
    C1CTRL1bits.WIN = 0;

    DMA0CONbits.CHEN=1; //enable DMA buffer tx;
    s_hal_can_dma_rx_enable(port);

    //IEC2bits.C1IE = 0;  //disable interrupt tx
    //IEC0bits.DMA1IE = 0; //disable interrupt rx
    hal_sys_irqn_disable(hal_arch_dspic_CAN1_IRQn);
    hal_sys_irqn_disable(hal_arch_dspic_DMA1_IRQn);

    if(hal_can_runmode_noisr_0q != cport->cfg.runmode)
    {
        /* Enable DMA interrupt for RX instead of can-RX interrupt.
           So we know when datas are ready in buffer and the application can read them. 
        */
        //IEC0bits.DMA1IE = 1;
        //enable interrupt TX 
        //(it is necessary enable INT both in Interrupt Enable Register (general for all peritherals) and in CAN register)
        //C1INTEbits.TBIE = 1; 
        //IEC2bits.C1IE = 1;
        hal_sys_irqn_enable(hal_arch_dspic_CAN1_IRQn);
        hal_sys_irqn_enable(hal_arch_dspic_DMA1_IRQn);
        s_hal_canperipheral_isr_tx_enable(port); // enable the tx interrupt in can

        C1INTFbits.RBIF=0; // clear isr flag
        C1INTFbits.TBIF=0; // clear isr flag
    }
    
    // enable scheduling 
    hal_base_hid_osal_scheduling_restart();    
    
    return(hal_res_OK);
}


extern hal_result_t hal_can_disable(hal_can_port_t port) 
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    
    if(hal_false == s_hal_can_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    
    // disable scheduling
    hal_base_hid_osal_scheduling_suspend();

    cport->enabled = 0;

    C1CTRL1bits.WIN = 0;

    DMA0CONbits.CHEN=0; //disable DMA buffer tx;
    s_hal_can_dma_rx_disable(port);

    //IEC2bits.C1IE = 0;  //disable interrupt tx
    //IEC0bits.DMA1IE = 0; //disable interrupt rx
    // C1INTEbits.TBIE = 0;
    
    hal_sys_irqn_disable(hal_arch_dspic_CAN1_IRQn);
    hal_sys_irqn_disable(hal_arch_dspic_DMA1_IRQn);
    s_hal_canperipheral_isr_tx_disable(port); // disable the tx interrupt in can1

    // enable scheduling 
    hal_base_hid_osal_scheduling_restart();  
    
    return(hal_res_OK);
}


extern hal_result_t hal_can_put(hal_can_port_t port, hal_can_frame_t *frame, hal_can_send_mode_t sm) 
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    
    hal_result_t res = hal_res_NOK_generic;

    if(hal_false == s_hal_can_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == frame)
    {
        return(hal_res_NOK_nullpointer);
    }

    if( (hal_can_frame_remote == frame->frame_type) || (hal_can_frameID_ext == frame->id_type) )
    {
        return(hal_res_NOK_unsupported); // currently we don't manage hal_can_frame_remote and frame with extID
    }
    
    if(0 == cport->enabled)
    {
        return(hal_res_NOK_generic);
    }


    switch(cport->cfg.runmode)
    {
        case hal_can_runmode_noisr_0q:
        {   // used in bootloader for instance
        
            if(hal_can_send_normprio_now == sm)
            {
                res = s_hal_can_tx_withoutisr(port, frame); 
            }
            else
            {
                res = hal_res_NOK_unsupported;
            }
        
        } break;
        
        case hal_can_runmode_isr_1txq1rxq:
        case hal_can_runmode_isr_2txq1rxq:
        {
            if((hal_can_send_highprio_now == sm) && (hal_can_runmode_isr_1txq1rxq == cport->cfg.runmode))
            {   // cannot put inside a highprio queue if we only have the normprio queue
                res = hal_res_NOK_generic;
            }
            else if(hal_can_send_highprio_now == sm)
            {   // i am in highprio with the highprio queue (the second one) available
                res =  s_hal_can_tx_highprio(port, frame); 
            }
            else
            {   // can be hal_can_send_normprio_now or hal_can_send_normprio_later
                // in either 2q or 1q mode. but in any case i use the normal queue
                res = s_hal_can_tx_normprio(port, frame, sm);
            }
        
        } break;
   
        default:
        {
            res = hal_res_NOK_unsupported;
        } break;        
    
    }

    return(res);
}



extern hal_result_t hal_can_transmit(hal_can_port_t port)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    if(hal_false == s_hal_can_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }
    
    if(0 == cport->enabled)
    {
        return(hal_res_NOK_generic);
    }
    
    // disable scheduling
    hal_base_hid_osal_scheduling_suspend();
    
    // trigger the tx. the trigger will empty the canframes_tx_norm.
    s_hal_can_isr_sendframes_canx(port);
    
    // enable scheduling 
    hal_base_hid_osal_scheduling_restart();
    
    return(hal_res_OK);
}

extern hal_result_t hal_can_received(hal_can_port_t port, uint8_t *numberof) 
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    hal_result_t res = hal_res_NOK_generic;
    volatile uint8_t fifonextread;
    volatile uint8_t fifocurrent;

    if(hal_false == s_hal_can_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

    if( (NULL == cport) || (NULL == numberof) )
    {
        return(hal_res_NOK_generic);
    }

    if(0 == cport->enabled)
    {
        return(hal_res_NOK_generic);
    }
    
    // disable scheduling 
    hal_base_hid_osal_scheduling_suspend();
    
    //s_hal_can_dma_rx_disable(port);
    
    fifonextread = C1FIFObits.FNRB;
    fifocurrent  = C1FIFObits.FBP;
    //*numberof = (fifocurrent>fifonextread) ? (fifocurrent-fifonextread) : (fifonextread-fifocurrent);
    *numberof = s_hal_can_dma_rx_size(port);
 
    //s_hal_can_dma_rx_enable(port);
    
    // enable scheduling 
    hal_base_hid_osal_scheduling_restart();

    return(res);
}


extern hal_result_t hal_can_get(hal_can_port_t port, hal_can_frame_t *frame, uint8_t *remaining) 
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    hal_result_t res = hal_res_NOK_generic;
    volatile uint8_t fifocurrent;
    volatile uint8_t fifonextread;
    volatile uint8_t size = 0;

    if(hal_false == s_hal_can_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

    if(NULL == frame)
    {
        return(hal_res_NOK_nullpointer);
    }
    
    if(0 == cport->enabled)
    {
        return(hal_res_NOK_generic);
    } 
   
    // disable scheduling 
    hal_base_hid_osal_scheduling_suspend();
    
    // no need to disable the dma in rx ... i just read the index of first fifo buffer (the FIFO Next Read Buffer Pointer)
    //s_hal_can_dma_rx_disable(port);
    
    // get buffer index of head of rx queue. hw updates it for us.
    fifonextread = C1FIFObits.FNRB;
    fifocurrent  = C1FIFObits.FBP;
    if(NULL != remaining)
    {
        size = s_hal_can_dma_rx_size(port);

        if(0 != size)
        {
            size = size;
        }
    }
    

 
    // check the buffer size. if empty returns hal_res_NOK_nodata
    if( ( (fifonextread < 16) && ((!(C1RXFUL1 & (1<<fifonextread)))) ) || ( (fifonextread >= 16) && ( (!(C1RXFUL2 & (1<<(fifonextread-16))))) ) )
    {
        if(NULL != remaining)
        {
            *remaining = 0;
        }
        //s_hal_can_dma_rx_enable(port);
        hal_base_hid_osal_scheduling_restart();
        return(hal_res_NOK_nodata);
    }

    res = s_hal_can_copy_frame_fromDMA(frame, fifonextread);
    
    if(hal_res_OK != res)
    {
        if(NULL != remaining)
        {
            *remaining = size-1;
        }
        //s_hal_can_dma_rx_enable(port);
        hal_base_hid_osal_scheduling_restart();
        return(res);
    }

    // clear bit that means buffer full
    if(fifonextread < 16)
    {
        C1RXFUL1 = ~(1<<fifonextread);
    }
    else
    {
        C1RXFUL2 = ~(1<<(fifonextread-16));
    }
  
    if(NULL != remaining)
    {
        *remaining = size-1;
    }

    //s_hal_can_dma_rx_enable(port);
    // enable scheduling 
    hal_base_hid_osal_scheduling_restart();
    
    return(hal_res_OK); 
}


extern hal_result_t hal_can_receptionfilter_set(hal_can_port_t port, uint8_t mask_num, uint32_t mask_val, uint8_t identifier_num,
                                                uint32_t identifier_val, hal_can_frameID_format_t idformat)
{
    if(hal_false == s_hal_can_initted_is(port))
    {
        return(hal_res_NOK_generic);
    }

    if(hal_can_frameID_ext == idformat)
    {
        return(hal_res_NOK_unsupported);
    }


    //check input
    if( (mask_num >= hal_brdcfg_can__rxfilter_masknum_supported) || (identifier_num >= hal_brdcfg_can__rxfilter_idnum_supported))
    {
        return(hal_res_NOK_unsupported);
    }

    s_hal_can_rxFilter_set_mask(mask_num, mask_val, idformat);

    s_hal_can_rxFilter_set_id(identifier_num, identifier_val, idformat, mask_num);


    //hal_base_hid_on_fatalerror(hal_fatalerror_unsupportedbehaviour, "hal_can_receptionfilter_set(): not supported");
    return(hal_res_OK);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------


// ---- isr of the module: begin ----

void __attribute__((interrupt, no_auto_psv)) _C1Interrupt(void)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(hal_can_port1)]; 
    uint8_t dmabuf_num;

    if(C1INTFbits.TBIF) // if a msg was sent
    {
        dmabuf_num = C1VECbits.ICODE;

        switch(cport->cfg.runmode)
        {
            case hal_can_runmode_isr_1txq1rxq:
            {   
                if(dmabuf_num >= cport->canframes_tx_norm.capacity)
                {
                    break; //error
                }            
                cport->canframes_tx_norm.size--;
                cport->canframes_tx_norm.index_out = (cport->canframes_tx_norm.index_out + 1)% cport->canframes_tx_norm.capacity;
                if(cport->canframes_tx_norm.size > 0)
                {
                    s_hal_can_isr_sendframes_canx(hal_can_port1);
                }
            
            } break;
        
            case hal_can_runmode_isr_2txq1rxq:
            {
                if(dmabuf_num < (cport->canframes_tx_high.capacity))
                {
                    cport->canframes_tx_high.size--;
                }
                else if(dmabuf_num < (cport->canframes_tx_norm.capacity + cport->canframes_tx_high.capacity ))
                {
                    cport->canframes_tx_norm.size--;
                    cport->canframes_tx_norm.index_out = (cport->canframes_tx_norm.index_out + 1)% cport->canframes_tx_norm.capacity;
                    if(cport->canframes_tx_norm.size > 0)
                    {
                        s_hal_can_isr_sendframes_canx(hal_can_port1);
                    }
                }
                else
                {
                    break; //error
                }
            } break;
            
            default:
            {
                break; //error
            }
        }
        //clear bit
        C1INTFbits.TBIF = 0;
        IFS2bits.C1IF = 0; 
    }
}

void __attribute__((interrupt, no_auto_psv)) _DMA1Interrupt(void)

{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(hal_can_port1)]; 

    
    if(NULL != cport->cfg.callback_on_rx)
    {
       cport->cfg.callback_on_rx(cport->cfg.arg_cb_rx); 
    }  
 
    // Clear the DMA1 Interrupt Flag;
    IFS0bits.DMA1IF = 0;    
}

// ---- isr of the module: end ------

 
extern uint32_t hal_can_hid_getsize(const hal_cfg_t *cfg)
{
    uint32_t size = 0;

    if(0 == cfg->can1_enable)
    {
        return(size);
    }    
    
    size += sizeof(hal_can_portdatastructure_t); 
    return(size);
}


extern hal_result_t hal_can_hid_setmem(const hal_cfg_t *cfg, uint32_t *memory)
{
    uint8_t *ram08 = (uint8_t*)memory;

    
    if(0 == cfg->can1_enable)
    {
        return(hal_res_OK);
    }    

    if(NULL == memory)
    {
        hal_base_hid_on_fatalerror(hal_fatalerror_missingmemory, "hal_can_hid_setmem(): memory missing");
        return(hal_res_NOK_generic);
    }

    s_hal_can_portdatastruct_ptr[HAL_can_port2index(hal_can_port1)] = (hal_can_portdatastructure_t*)ram08;
    ram08 += sizeof(hal_can_portdatastructure_t);

 
    return(hal_res_OK);
}




// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

static hal_boolval_t s_hal_can_supported_is(hal_can_port_t port)
{
    return(hal_base_hid_byte_bitcheck(hal_brdcfg_can__supported_mask, HAL_can_port2index(port)) );
}

static void s_hal_can_initted_set(hal_can_port_t port)
{
    s_hal_can_initted[HAL_can_port2index(port)] = hal_true;
}

static hal_boolval_t s_hal_can_initted_is(hal_can_port_t port)
{
    return(s_hal_can_initted[HAL_can_port2index(port)]);
}




static hal_result_t s_hal_can_isr_sendframes_canx(hal_can_port_t port)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    
    s_hal_canperipheral_isr_tx_disable(port); 

    if(0 == cport->canframes_tx_norm.size) //empty queue ==> ok
    {
        s_hal_canperipheral_isr_tx_enable(port);
        return(hal_res_OK);
    }

//    if(cport->canframes_tx_norm.index < cport->canframes_tx_norm.size)
//    {
//        first = cport->canframes_tx_norm.capacity - cport->canframes_tx_norm.size + cport->canframes_tx_norm.index;
//    }
//    else
//    {
//        first = cport->canframes_tx_norm.index - cport->canframes_tx_norm.size;
//    }
   
    s_hal_can_ena_dmabuff(cport->canframes_tx_high.capacity + cport->canframes_tx_norm.index_out); 
    //cport->canframes_tx_norm.size --;

    s_hal_canperipheral_isr_tx_enable(port);
    return(hal_res_OK);
 
}


static void s_hal_can_ena_dmabuff(uint8_t buffer_num)
{
    switch(buffer_num)
    {
        case 0:
        {        
            C1TR01CONbits.TXREQ0=1;
        }break;
        
        case 1:
        {
            C1TR01CONbits.TXREQ1=1;
        }break;
        
        case 2:
        {
            C1TR23CONbits.TXREQ2=1;
        }break;
        
        case 3:
        {
            C1TR23CONbits.TXREQ3=1;
        }break;
        
        case 4:
        {
            C1TR45CONbits.TXREQ4=1;
        }break;
        
        case 5:
        {
            C1TR45CONbits.TXREQ5=1;
        }break;
        
        case 6:
        {
            C1TR67CONbits.TXREQ6=1;
        }break;

        default:
        {
            return;
        }
    }
    return;
}




static hal_result_t s_hal_can_copy_frame_fromDMA(hal_can_frame_t *frame, uint8_t dmaBuff_index)
{
    hal_can_dspic_DMAframe_t *DMAframe;
    DMAframe = &s_hal_can_dmabuffer[dmaBuff_index];
    
    //NOTE: DMAframe.IDE = 0 indicates frame with standard ID
    if( (1 == DMAframe->IDE)||(1 == DMAframe->SRR) ) //extended id or remote request trasmission
    {
        return(hal_res_NOK_unsupported);
    }


    //NOTE: sometimes is possible to receive a frame with DLC field containing a value bigger then 8.
    //This depend on hardware!!
    if(DMAframe->DLC > 8)
    {
        return(hal_res_NOK_hwerror); 
    }
    
    memset(frame, 0, sizeof(hal_can_frame_t));
    

    //currently we use only this type of frame
    frame->frame_type = hal_can_frame_data;
    frame->id_type = hal_can_frameID_std;

    // copy data from received frame
    frame->id = DMAframe->SID;

    frame->size = DMAframe->DLC;

    memcpy(&frame->data, &DMAframe->data, frame->size);

    return(hal_res_OK);
}









static hal_result_t s_hal_can_tx_withoutisr(hal_can_port_t port, hal_can_frame_t *frame)
{
    //verify if the only buffer is empty and so copy frame into it.
    if(C1TR01CONbits.TXREQ0)
    {
        return(hal_res_NOK_busy);
    }
    s_hal_can_copy_frame_inDMA(frame, 0);
    
    C1TR01CONbits.TXREQ0=1; //trasmission request
    
    return(hal_res_OK);
}



static hal_result_t s_hal_can_tx_normprio(hal_can_port_t port, hal_can_frame_t *frame, hal_can_send_mode_t sm)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    hal_result_t res = hal_res_NOK_generic;
    uint8_t buff_num;
    
   
    // disable scheduling. 
    hal_base_hid_osal_scheduling_suspend();
    
    // disable interrupt in can 1 or 2 for tx depending on value of port
    s_hal_canperipheral_isr_tx_disable(port);

//    if(hal_can_runmode_isr_1txq1rxq == cport->cfg.runmode)
//    {
//        res = s_hal_can_tx_circular_dmabuff(port, &cport->canframes_tx_high, &buff_num);
//    }
    if(cport->canframes_tx_norm.size == cport->canframes_tx_norm.capacity) //queue is full
    {
        s_hal_canperipheral_isr_tx_enable(port);
        res = hal_res_NOK_busy;
    }
    else
    {
        buff_num = cport->canframes_tx_norm.index_in + cport->canframes_tx_high.capacity;
        
        s_hal_can_copy_frame_inDMA(frame,buff_num);
        
        cport->canframes_tx_norm.index_in = (cport->canframes_tx_norm.index_in + 1)%cport->canframes_tx_norm.capacity;
        
        cport->canframes_tx_norm.size++;
        
        res = hal_res_OK;
    }
    
    s_hal_canperipheral_isr_tx_enable(port);

    if(hal_can_send_normprio_now == sm)
    {   // if send-now i empty the queue
        s_hal_can_isr_sendframes_canx(port); //hal_can_transmit();
    }
    
    // enable scheduling. 
    hal_base_hid_osal_scheduling_restart();

    

    return(res);

}




static hal_result_t s_hal_can_tx_highprio(hal_can_port_t port, hal_can_frame_t *frame)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];
    uint8_t buff_num;
    hal_result_t res;

    // disable scheduling
    hal_base_hid_osal_scheduling_suspend();
    
    //disable interrupt tx
    s_hal_canperipheral_isr_tx_disable(port);
    
//    res = s_hal_can_tx_circular_dmabuff(port, &cport->canframes_tx_high, &buff_num);
//
//    s_hal_can_ena_dmabuff_withPrio(port, buff_num);
//
    if(cport->canframes_tx_high.size == cport->canframes_tx_high.capacity) //queue is full
    {
        s_hal_canperipheral_isr_tx_enable(port);
        return(hal_res_NOK_busy); 
    }
    
    buff_num = cport->canframes_tx_high.capacity - 1 - cport->canframes_tx_high.index_in;
    //buff_num = cport->canframes_tx_high.index_in;
    s_hal_can_copy_frame_inDMA(frame, buff_num);
    
    s_hal_can_ena_dmabuff_withPrio(port, buff_num);
    
    cport->canframes_tx_high.size ++;
    cport->canframes_tx_high.index_in = ((cport->canframes_tx_high.index_in + 1)%cport->canframes_tx_high.capacity);

    s_hal_canperipheral_isr_tx_enable(port);

    // enable scheduling. 
    hal_base_hid_osal_scheduling_restart();

    return(res);
}

//static hal_result_t s_hal_can_tx_circular_dmabuff(hal_can_port_t port, hal_can_DMAqueue_info_t *dma_queue_ptr)
//{
//    if(dma_queue_ptr->size == dma_queue_ptr->capacity) //queue is full
//    {
//        return(hal_res_NOK_busy); 
//    }
//    
//    buff_num = dma_queue_ptr->capacity -1 - dma_queue_ptr->index;
//    
//    s_hal_can_copy_frame_inDMA(frame,buff_num);
//    
//    //s_hal_can_ena_dmabuff_withPrio(port, buff_num);
//    
//    dma_queue_ptr->size ++;
//    dma_queue_ptr->index = ((dma_queue_ptr->index + 1)%dma_queue_ptr->capacity);
//
//}


static void s_hal_can_ena_dmabuff_withPrio(hal_can_port_t port, uint8_t buf_index)
{
    uint8_t i_prio = buf_index;

    switch( buf_index)
    {
        case 0:
        {        
            C1TR01CONbits.TX0PRI = s_can_dmabuff_prio_tbl[i_prio];
            C1TR01CONbits.TXREQ0=1;
            return; 
        }
        case 1:
        {
            C1TR01CONbits.TX1PRI = s_can_dmabuff_prio_tbl[i_prio];
            C1TR01CONbits.TXREQ1=1;
            return; 
        }
        case 2:
        {
            C1TR23CONbits.TX2PRI = s_can_dmabuff_prio_tbl[i_prio];
            C1TR23CONbits.TXREQ2=1;
            return; 
        }
        case 3:
        {
            C1TR23CONbits.TX3PRI = s_can_dmabuff_prio_tbl[i_prio];
            C1TR23CONbits.TXREQ3=1;
            return; 
        }
    }
}




static void s_hal_can_clk_init(hal_can_port_t port, hal_can_baudrate_t br_val)
{
    //TODO copiato dall'esempio
    /* FCAN is selected to be FCY
       FCAN = FCY = 40MHz
     */
    C1CTRL1bits.CANCKS = 0x1; //probabilmente questo non serve perche' questo bit non e' implemenatto sul nostro micro.

    /*
    Bit Time = (Sync Segment + Propagation Delay + Phase Segment 1 + Phase Segment 2)=20*TQ
    Phase Segment 1 = 8TQ
    Phase Segment 2 = 6Tq
    Propagation Delay = 5Tq
    Sync Segment = 1TQ
    CiCFG1<BRP> =(FCAN /(2 ×N×FBAUD))– 1
    */

    /* Synchronization Jump Width set to 4 TQ */
    C1CFG1bits.SJW = 0x3;
    
    /* Baud Rate Prescaler */
    if(hal_can_baudrate_1mbps == br_val)
    {
        C1CFG1bits.BRP = HAL_CAN_BR_VAL_1Mbps;
    }
    else
    {
        C1CFG1bits.BRP = HAL_CAN_BR_VAL_500kbps;
    }   

    /* Phase Segment 1 time is 8 TQ */
    C1CFG2bits.SEG1PH=0x7;
    /* Phase Segment 2 time is set to be programmable */
    C1CFG2bits.SEG2PHTS = 0x1;
    /* Phase Segment 2 time is 6 TQ */
    C1CFG2bits.SEG2PH = 0x5;
    /* Propagation Segment time is 5 TQ */
    C1CFG2bits.PRSEG = 0x4;
    /* Bus line is sampled three times at the sample point */
    C1CFG2bits.SAM = 0x1;
}


//static uint8_t s_hal_can_dspic_check_queue_cfg(hal_can_dspic_queue_cfg_t *queue_cfg)
//{
//    uint8_t i;
//
//    for(i=0; i<hal_can_send_mode_num; i++)
//    {
//        if( (queue_cfg->rxqnorm_num == s_default_queue_cfg[i].rxqnorm_num) &&
//            (queue_cfg->txqnorm_num == s_default_queue_cfg[i].txqnorm_num) &&
//            (queue_cfg->txqhigh_num == s_default_queue_cfg[i].txqhigh_num) )
//        {
//            return(i);
//        }
//    }
//    return(HAL_CAN_DSPIC_DEF_QUEUE_CFG_INDEX_NULL);
//}



static void s_hal_can_dmaqueue_info_init(hal_can_port_t port)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    cport->canframes_tx_norm.capacity = s_dspic_default_queue_cfg[cport->cfg.runmode].txqnorm_num;
    cport->canframes_tx_norm.size = 0;
    cport->canframes_tx_norm.index_in = 0;
    cport->canframes_tx_norm.index_out = 0;

    cport->canframes_tx_high.capacity = s_dspic_default_queue_cfg[cport->cfg.runmode].txqhigh_num;
    cport->canframes_tx_high.size = 0;
    cport->canframes_tx_high.index_in = 0;
    cport->canframes_tx_norm.index_out = 0;

}



/* configure dma channels for tx and rx of can1 peripheral */
static void s_can_dma_channels_config(void)
{
    /* ---- configure DMA channel 0 for can tx ---- */

    /* disable Peripheral Write Collision detection */
    DMACS0bits.PWCOL0 = 0b0;  //TODO is it correct???
    /* disable DMA module Write Collision detection */
    DMACS0bits.XWCOL0 = 0b0;  //TODO is it correct???


    /* Data Transfer Size = word */
    DMA0CONbits.SIZE = 0x0;
    /* Transfer Direction = Read from DPSRAM address, write to peripheral address */
    DMA0CONbits.DIR = 0x1;
    /* DMA Channel Addressing Mode = Peripheral Indirect Addressing mode */
    DMA0CONbits.AMODE = 0x2;
    /* DMA Channel Operating Mode = Continuous, Ping-Pong modes disabled */
    DMA0CONbits.MODE = 0x0;
    /* Block Transfer Interrupt = Initiate interrupt when all of the data has been moved */
    DMA0CONbits.HALF = 0x0;


    /* configure address of ECAN Transmit Data Register(C1TXD) */
    DMA0PAD=0x0442;

    /* define data block size to transfer. It is equal to can message size (= 8 word).
       Note: the module sum 1 to this number. 
    */
    DMA0CNT=0x0007;

    /* associate the DMA channel with a specific DMA capable peripheral (TX Data Request)*/
    DMA0REQ=0x0046;

    /* set satrt address of DMA buffer */
    DMA0STA=  __builtin_dmaoffset(s_hal_can_dmabuffer);



    /* ---- configure DMA channel 1 for can rx ---- */

    /* disable Peripheral Write Collision detection */
    DMACS0bits.PWCOL1 = 0b0;  //TODO corretto???
    /* disable DMA module Write Collision detection */
    DMACS0bits.XWCOL1 = 0b0;

    /* Data Transfer Size = word */
    DMA1CONbits.SIZE = 0x0;
    /* Transfer Direction = Read from Peripheral address, write to DPSRAM address */
    DMA1CONbits.DIR = 0x0;
    /* DMA Channel Addressing Mode = Peripheral Indirect Addressing mode */
    DMA1CONbits.AMODE = 0x2;
    /* DMA Channel Operating Mode = Continuous, Ping-Pong modes disabled */
    DMA1CONbits.MODE = 0x0;
    /* Block Transfer Interrupt = Initiate interrupt when all of the data has been moved */
    DMA1CONbits.HALF = 0x0;


    /* configure address of ECAN Receive Data Register(C1RXD) */
    DMA1PAD = 0x0440;

    /* define data block size to transfer. It is equal to can message size (= 8 word).
     * Note: the module sum 1 to this number. */
    DMA1CNT = 0x0007;

    /* associate the DMA channel with a specific DMA capable peripheral (RX Data Ready)*/
    DMA1REQ = 0x0022;

    /* set start address of DMA buffer */
    DMA1STA =  __builtin_dmaoffset(s_hal_can_dmabuffer);
}


static void s_hal_can_dma_init(hal_can_port_t port)
{
    hal_can_portdatastructure_t *cport = s_hal_can_portdatastruct_ptr[HAL_can_port2index(port)];

    s_can_dma_channels_config();

    C1CTRL1bits.WIN = 0;
    
    
    /* RX FIFO Starts at end of tx buffers */
    C1FCTRLbits.FSA = cport->canframes_tx_norm.capacity + cport->canframes_tx_high.capacity;
    C1FCTRLbits.DMABS = 0x6; // = 0b110; //set RX fifo ends 

    switch(cport->cfg.runmode)
    {
        case hal_can_runmode_noisr_0q:
        {
            C1TR01CONbits.TXEN0 = 1;            // set buffer 0 as transmit buffer
            C1TR01CONbits.TXEN1 = 0;            // set buffer 1 as receive buffer
            C1TR23CONbits.TXEN2 = 0;            // set buffer 2 as receive buffer
            C1TR23CONbits.TXEN3 = 0;            // set buffer 3 as receive buffer
            C1TR45CONbits.TXEN4 = 0;            // set buffer 4 as receive buffer
            C1TR45CONbits.TXEN5 = 0;            // set buffer 5 as receive buffer
            C1TR67CONbits.TXEN6 = 0;            // set buffer 6 as receive buffer
            C1TR67CONbits.TXEN7 = 0;            // set buffer 7 as receive buffer

            C1TR01CONbits.TX0PRI = 0b11;         // set priority highest to transmit buffer 0
        } break;


        case hal_can_runmode_isr_1txq1rxq:
        {
            C1TR01CONbits.TXEN0 = 1;            // set buffer 0 as transmit buffer
            C1TR01CONbits.TXEN1 = 1;            // set buffer 1 as transmit buffer
            C1TR23CONbits.TXEN2 = 1;            // set buffer 2 as transmit buffer
            C1TR23CONbits.TXEN3 = 1;            // set buffer 3 as transmit buffer
            C1TR45CONbits.TXEN4 = 0;            // set buffer 4 as receive buffer
            C1TR45CONbits.TXEN5 = 0;            // set buffer 5 as receive buffer
            C1TR67CONbits.TXEN6 = 0;            // set buffer 6 as receive buffer
            C1TR67CONbits.TXEN7 = 0;            // set buffer 7 as receive buffer 
            
            /* Buffer priority will be set during execution, because circular mode is performed turning prio values */
        } break;


        case hal_can_runmode_isr_2txq1rxq:
        {
            C1TR01CONbits.TXEN0 = 1;            // set buffer 0 as transmit buffer
            C1TR01CONbits.TXEN1 = 1;            // set buffer 1 as transmit buffer
            C1TR23CONbits.TXEN2 = 1;            // set buffer 2 as transmit buffer
            C1TR23CONbits.TXEN3 = 1;            // set buffer 3 as transmit buffer
            C1TR45CONbits.TXEN4 = 1;            // set buffer 4 as transmit buffer
            C1TR45CONbits.TXEN5 = 1;            // set buffer 5 as transmit buffer
            C1TR67CONbits.TXEN6 = 1;            // set buffer 6 as transmit buffer
            C1TR67CONbits.TXEN7 = 0;            // set buffer 7 as receive buffer
            
            /* HighPrio queue is used like circular buffer. it can be done by turning prio values, 
               like hal_can_runmode_isr_1txq1rxq, but only on 3 buffer and three hightest priority values*/

            /* Set lowest priority to LowPrio queue */
            C1TR23CONbits.TX3PRI=0;         // set priority highest to transmit buffer 3
            C1TR45CONbits.TX4PRI=0;         // set priority highest to transmit buffer 4
            C1TR45CONbits.TX5PRI=0;         // set priority highest to transmit buffer 5
            C1TR45CONbits.TX5PRI=0;         // set priority highest to transmit buffer 6
        } break;

    }


    //clean all flags for full and overflow buffer
    C1RXFUL1 = 0x0000;
    C1RXFUL2 = 0x0000;
    C1RXOVF1 = 0x0000;
    C1RXOVF2 = 0x0000;
}

static void s_hal_can_copy_frame_inDMA(hal_can_frame_t *frame, uint8_t dmaBuff_index)
{
    hal_can_dspic_DMAframe_t *DMAframe;

    DMAframe = &s_hal_can_dmabuffer[dmaBuff_index];

    memset(DMAframe, 0, sizeof(hal_can_dspic_DMAframe_t));
    //NOTE: DMAframe.IDE = 0 indicates frame with standard ID

    DMAframe->SID =(uint16_t)frame->id;

    DMAframe->DLC = frame->size;

    memcpy(&DMAframe->data, &frame->data, DMAframe->DLC);
}



static hal_result_t s_hal_can_rxFilter_set_mask( uint8_t mask_num, uint32_t mask_val, hal_can_frameID_format_t idformat)
{
    uint32_t sid = 0,eid15_0=0, eid17_16=0;
    uint16_t *maskRegAddr, *maskExtRegAddr; 

    // unlock mask changes
    C1CTRL1bits.WIN=1;
    
    // Obtain the Address of CiRXMmSID register for given Mask number "m"
    maskRegAddr = (uint16_t *)(&C1RXM0SID + (mask_num << 1));
    maskExtRegAddr = maskRegAddr + 1;

    // Bit-filed manupulation to write to Filter Mask register
    if(hal_can_frameID_ext == idformat) 
    {     
        // Filter Extended Identifier
        eid15_0 = (mask_val & 0xFFFF);
        eid17_16= (mask_val>>16) & 0x3;
        sid = (mask_val>>18) & 0x7FF;
        
        // 0x008 for set mide bit: so filter maches only eid or only sid.
        *maskRegAddr=((sid)<<5) + 0x0008 + eid17_16;  // Write to CiRXMnSID Register
        *maskExtRegAddr= eid15_0;  // Write to CiRXMnEID Register
    }
    else
    {
        // Filter Standard Identifier
        sid = (mask_val & 0x7FF);
        
        // 0x008 for set mide bit: so filter maches only eid or only sid.
        *maskRegAddr = ((sid)<<5) + 0x0008;  // Write to CiRXMnSID Register    
        *maskExtRegAddr = 0;  // Write to CiRXMnEID Register
    }

    // lock mask changes
    C1CTRL1bits.WIN=0;

    return(hal_res_OK);    
}



static hal_result_t s_hal_can_rxFilter_set_id(uint8_t id_num, int32_t id_val, hal_can_frameID_format_t idformat, uint8_t mask_sel) 
{
  
    uint32_t sid = 0, eid15_0 = 0, eid17_16 = 0;
    uint16_t *sidRegAddr, *exidRegAddr, *bufPntRegAddr, *maskSelRegAddr, *fltEnRegAddr;
    uint16_t aux;
        
    // unlock filter changes
    C1CTRL1bits.WIN=1;
    
    // Obtain the Address of CiRXFnSID, CiBUFPNTn, CiFMSKSELn and CiFEN register for a given filter number "n"
    sidRegAddr = (uint16_t *)(&C1RXF0SID + (id_num << 1)); // I need moltiply for 2 (<< 1), beause after C1RXF0SID there is C1RXF0EID register. regiters memory map
    exidRegAddr = sidRegAddr + 1; // C1RXF0EID is next to C1RXF0SID. see regiters memory map.

    fltEnRegAddr = (uint16_t *)(&C1FEN1);

    // Bit-field manupulation to write to Filter identifier register
    if(hal_can_frameID_ext == idformat) 
    {
        // Filter Extended Identifier
        eid15_0 = (id_val & 0xFFFF);
        eid17_16= (id_val>>16) & 0x3;
        sid = (id_val>>18) & 0x7FF;
        
        // 0x8 ==> set EXIDE bit so , the filter maches only EID.
        *sidRegAddr =(((sid)<<5) + 0x8) + eid17_16;  // Write to CiRXFnSID Register
        *exidRegAddr = eid15_0;  // Write to CiRXFnEID Register
    }
    else
    {  
        // Filter Standard Identifier
        sid = (id_val & 0x7FF);            
        *sidRegAddr=(sid)<<5;  // Write to CiRXFnSID Register in bit from 5 to 15
        *exidRegAddr = 0;  // Write to CiRXFnEID Register
    }

    //select the correct buffer and mask register
    if( (id_num <=3) )
    {
        bufPntRegAddr = (uint16_t *)(&C1BUFPNT1);
        maskSelRegAddr = (uint16_t *)(&C1FMSKSEL1);
    }
    else if( (id_num >= 4) && (id_num <=7) ) 
    {
        bufPntRegAddr = (uint16_t *)(&C1BUFPNT2);
        maskSelRegAddr = (uint16_t *)(&C1FMSKSEL1);
    }
    else if( (id_num >= 8) && (id_num <=11) ) 
    {
        bufPntRegAddr = (uint16_t *)(&C1BUFPNT3);
        maskSelRegAddr = (uint16_t *)(&C1FMSKSEL2);
    }
    else if( (id_num >= 12) && (id_num <=15) ) 
    {
        bufPntRegAddr = (uint16_t *)(&C1BUFPNT4);
        maskSelRegAddr = (uint16_t *)(&C1FMSKSEL2);
    }
     
    //set the buffer where can messages will be inserted if they mach filter
    // I do filter_num%4 because in each register 4 buffer pointer.
    // I moltiply for 4, because buffer pointer is codifies in 4 bits.
    aux = 0xFFFF - (0xF <<(4* (id_num%4) ));          
    *bufPntRegAddr &= aux; //clear bits
    *bufPntRegAddr |= 0xF << (4*(id_num%4));  // set 0xF buffer, that is FIFO queue.

    if(mask_sel < 3 )
    {    
        aux = 0xFFFF - ( 0x3 <<(2*(id_num%8)) );
        *maskSelRegAddr &= aux; //clear bits
        *maskSelRegAddr |=(  mask_sel <<(2*(id_num%8)) );
    }

    *fltEnRegAddr |= (0x1 << id_num);  // Write to C1FEN1 Register
    
    // lock filter changes
    C1CTRL1bits.WIN=0;

    return(hal_res_OK);
}


//#define HAL_CAN_INT_TX_ENA(p)                   C1INTEbits.TBIE = 1;
//#define HAL_CAN_INT_TX_DISA(p)                  C1INTEbits.TBIE = 0;

static void s_hal_canperipheral_isr_tx_enable(hal_can_port_t port)
{   // i enable the global can isr to happen for tx buffer empty
    C1INTEbits.TBIE = 1;    // TX Buffer Interrupt Enable bit
}

static void s_hal_canperipheral_isr_tx_disable(hal_can_port_t port)
{   // i disable the global can isr to happen for tx buffer empty   
    C1INTEbits.TBIE = 0;    // TX Buffer Interrupt Enable bit
}
    

static void s_hal_can_dma_rx_enable(hal_can_port_t port)
{
    DMA1CONbits.CHEN = 1; //enable DMA buffer rx;
}

static void s_hal_can_dma_rx_disable(hal_can_port_t port)
{
    DMA1CONbits.CHEN = 0; //disable DMA buffer rx;
}


static uint8_t s_hal_can_dma_rx_size(hal_can_port_t port)
{
    volatile uint32_t bits;
    uint32_t i = 0;
    uint8_t size = 0;
    uint32_t msk;
    bits = (uint32_t)C1RXFUL1;
    bits |=  (((uint32_t)C1RXFUL2) << 16);
    bits &= s_can_rxdmamask;
    
    for(i=0; i<32; i++)
    {
        msk = 0x00000001L<<i;
        if((bits & msk) == msk)
        {
            size++;
        }
    }
    
    return(size);
   
}

#endif//HAL_USE_CAN

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



