/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero, Andrea Merello
 * email:   valentina.gaggero@iit.it, andrea.merello@iit.it
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

// --------------------------------------------------------------------------------------------------------------------
// - doxy
// --------------------------------------------------------------------------------------------------------------------

/* @file     adc.c
    @brief      This fileimplements interface to use adc AD7689 and to filter adc data. 
                Note that funcions with name s_adc_AD7689_* operates with the specific component AD7689,
                while functions with name s_adc_* are general purposes for any adc.
    @author     valentina.gaggero@iit.it, andrea.merello@iit.it
    @date       07/14/2011
*/



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <p33fxxxx.h>
#include <string.h>
#include <libpic30.h>
#include <timer.h>

 
#include "hal_timer.h"

#include "EOStheFOOP.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "adc.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

//*************************************************************************************
//                              MACRO TO DEFINE SAMPLE RATE
//*************************************************************************************

// every BUFFER_SAMPLE_RATE the buffer is ready with ADC_NUM_SAMPLES for each channel
#define ADC_BUFFER_SAMPLE_RATE 300 // microsec.

#define ADC_NUM_SAMPLES 8

//5200ns. this is MAX sample rate for AD
#define ADC_SAMPLE_RATE ((ADC_BUFFER_SAMPLE_RATE)/6 /(ADC_NUM_SAMPLES) )

//Note: MATCH VALUE = 208 ==> PERIOD= 5,2 MICROSEC
#if ((ADC_SAMPLE_RATE)*1000L/25) < 208
    #warning ADC_SAMPLE_RATE_TOO_HIGH
#endif


//*************************************************************************************
//                                    END
//*************************************************************************************


//*************************************************************************************
//                              MACRO TO CONFIGURE AD7689
//*************************************************************************************

#define ADC_AD7689_SPI_SLAVE_SELECT /*ADC_AD7689_SPI_SLAVE_SELECT*/ LATBbits.LATB15 //if 0 ==> slave is selected

// DEFINE USED BY REGISTERS
#define AD7689_CFG_UPDATE 1
#define AD7689_CFG_NOUPDATE 0

#define AD7689_INPUTCFG_BIPOLARDIFFERENTIAL 0
#define AD7689_INPUTCFG_BIPOLARSINGLEENDED 2
#define AD7689_INPUTCFG_TEMPERATURESENSOR 3
#define AD7689_INPUTCFG_UNIPOLARDIFFERENTIAL 4
#define AD7689_INPUTCFG_UNIPOLARCOMREF 6
#define AD7689_INPUTCFG_UNIPOLARGNDREF 7

#define AD7689_BANDWIDTH_FULL 1
#define AD7689_BANDWIDTH_QUARTER 0

#define AD7689_REFERENCE_INTERNAL2500MV 0
#define AD7689_REFERENCE_INTERNAL4096MV 1
#define AD7689_REFERENCE_EXTERNALTEMPERATUREENA 2
#define AD7689_REFERENCE_EXTERNALTEMPERATUREENAINTERANLBUF 3
#define AD7689_REFERENCE_EXTERNAlTEMPERATUREDIS 5
#define AD7689_REFERENCE_EXTERNALTEMPERATUREDISINTERNALBUF 6

#define AD7689_SEQUENCE_DISABLED 0
#define AD7689_SEQUENCE_UPDATECFG 1
#define AD7689_SEQUENCE_SCANINPUTANDTEMPERATURE 2
#define AD7689_SEQUENCE_SCANINPUT 3

#define AD7689_READBACKCFG_ENABLE 0
#define AD7689_READBACKCFG_DISABLE 1
//*************************************************************************************
//                                    END
//*************************************************************************************


//USED TO CONVERT TO 1.15 FIXED POINT
#define HEX_VALC      0x8000

//MACRO TO OPERATE WITH EMBOBJ FOOP
#define EVT_IIR_FILTER_START        0b00000010 // evt pos is 2 // defined also in adc.c
#define EVT_IIR_FILTER_CONTINUE     0b00000100 // evt pos is 3

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef union 
{
    struct
    {
        unsigned pad:2;
        // can be used to ask the AD7689 to loop back
        // che CFG content over SPI
        unsigned readbackcfg:1;
        // configures the MUX sequencer for analog inputs
        unsigned sequencer:2;
        // configure the voltage reference for AD
        unsigned reference:3;
        // enalbles or disables LP filter
        unsigned bandwidth:1;
        // chose the last input to be sampled in 
        // sequence or the unique input to be sampled
        // when sequencing is disabled
        unsigned inputsel:3;
        // configure analog input pads
        unsigned inputcfg:3;
        // flag to determine if SPI write value
        // should be considered valid for CFG
        // register update.
        // (set to 0 to read SPI without writing CFG)
        unsigned cfgupdate:1;
        
    }__attribute__ ((__packed__)) Bits;

    unsigned int w;

} __attribute__ ((__packed__))  adc_register_t;

typedef enum
{
    adc_spi_dmabuf_none = 0,
    adc_spi_dmabuf_1 = 1,
    adc_spi_dmabuf_2 = 2
} adc_spi_dmabuf_number_t;


//this structure contains all info used by adc durinG filtering phase
typedef struct
{
    uint8_t new_data;                  // set by DMA interrupt: it sets 1 if is_filtering = 1 and new data are available
    uint8_t is_filtering;              // 0 ==> idle, 1 ==> filtering
    uint8_t last_flt_data_buff_index;  // it is used to index filtered_values filed
    uint8_t actual_flt_buff_infex;     // working data buffer number actual used for filetering
    uint8_t ch_num;                     // analog channel to filter
} adc_filter_work_data_t;


typedef struct
{
    adc_spi_dmabuf_number_t     dma_buff_index;         // indicates which dma buffer contain the most recent data
    adc_state_t                 state;                  // indicates if adc is sampling.
    SIXsg_config_data_t         *SIXsg_config_ptr;      // pointer to 6SG application's config
    int16_t                     values[AN_CHANNEL_NUM][ADC_NUM_SAMPLES];
    int16_t                     filtered_values[2][AN_CHANNEL_NUM];
    adc_filter_work_data_t      flt_wd;
} adc_datastruct_t;


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static hal_result_t s_adc_ad7689_spi_init(void);
static void s_adc_ad7689_spi_gpio_init(void);
static void s_adc_ad7689_timers_init(void);
static void s_adc_ad7689_timer2_callback(void *arg);
static void s_adc_ad7689_timer5_callback(void *arg);
static void s_adc_dma_init(void);
static void s_adc_iir_filter_init(void);


static void s_adc_filter_all_channel(void);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

//Default configuration values oF ADC
static const adc_register_t adc_reg_default_values =
{
    .Bits.readbackcfg   = AD7689_READBACKCFG_DISABLE,
    .Bits.sequencer     = AD7689_SEQUENCE_SCANINPUT,
    .Bits.reference     = AD7689_REFERENCE_INTERNAL2500MV,
    .Bits.bandwidth     = AD7689_BANDWIDTH_QUARTER,
    .Bits.inputsel      = 5, // scan 0 to 5 inputs
    .Bits.inputcfg      = AD7689_INPUTCFG_UNIPOLARGNDREF,
    .Bits.cfgupdate     = AD7689_CFG_UPDATE 
};


/**********************  ping pong DMA buffers for AD reading **********************/
uint16_t adc_spi_dmabuf1[AN_CHANNEL_NUM * ADC_NUM_SAMPLES] __attribute__((space(dma),aligned(16)));
uint16_t adc_spi_dmabuf2[AN_CHANNEL_NUM * ADC_NUM_SAMPLES] __attribute__((space(dma),aligned(16)));


/********************** IIR Trasposed filter **********************/
static IIRTransposedStruct iir_filter_struct[AN_CHANNEL_NUM];


// temporary storage (delay line: contains past history and current state of a filtering operation)
fractional iir_delay1[AN_CHANNEL_NUM][IIR_LPF_N_MAX_BQ * 1] __attribute__((space(ymemory), far));
fractional iir_delay2[AN_CHANNEL_NUM][IIR_LPF_N_MAX_BQ * 1] __attribute__((space(ymemory), far));

// IIR transoposed LP filter coefs
fractional IirTransposedCoefs[ 5 * IIR_LPF_N_MAX_BQ] __attribute__((__space__(xmemory)));


/********************** ADC DATA **********************/

static adc_datastruct_t adc_data =
{
    .dma_buff_index     = adc_spi_dmabuf_none,
    .state              = adc_st_stopped,
    .SIXsg_config_ptr   = NULL,
    .flt_wd =
    {
        .new_data                   = 0,                  // set by DMA interrupt: it sets 1 if is_filtering = 1 and new data are available
        .is_filtering               = 0,              // 0 ==> idle, 1 ==> filtering
        .last_flt_data_buff_index   = 0,  // it is used to index filtered_values filed
        .actual_flt_buff_infex      = 0,      // working data buffer number actual used for filetering
        .ch_num                     = 0     
    }
    //.filtered_values = {Q15(0.6), Q15(0.8), Q15(0.5), Q15(0.7), Q15(0.4), Q15(0.2)}
};


static int16_t output_iir[AN_CHANNEL_NUM][ ADC_NUM_SAMPLES] = {{0}};

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------
extern hal_result_t adc_init(SIXsg_config_data_t *cfg_ptr)
{
    hal_result_t res;
    
    if(NULL == cfg_ptr)
    {
        return(hal_res_NOK_nullpointer);
    }
    adc_data.SIXsg_config_ptr = cfg_ptr;

    res = s_adc_ad7689_spi_init();

    if(hal_res_OK !=res)
    {
        return(res);
    }
    
    s_adc_dma_init();

    s_adc_ad7689_timers_init();

    s_adc_iir_filter_init();


    return(hal_res_OK);
}


extern void adc_start(void)
{
    if( tx_outMsg_off == adc_data.SIXsg_config_ptr->behaviour_cfg.tx_outMsg_mode)
    {
        return;
    }


    //stop dma
    DMA3CONbits.CHEN=0;

    // disable dma interrupt ch 3
    IEC2bits.DMA3IE = 0;
    SPI1STATbits.SPIEN =0;
    
    //re-init spi and dma
    s_adc_ad7689_spi_init();
    s_adc_dma_init();

    // the two timers must start at the same exact time to stay synchronized
    __builtin_disi(0x3fff);

    #warning verifica se posso usare hal oppure no!!!


    WriteTimer2(132);//hal_timer_write(hal_timerT2, 132);//WriteTimer2(132);
    WriteTimer5(0); //hal_timer_write(hal_timerT5, 0);//WriteTimer5(0);

    T2CONbits.TON = 1;// hal_timer_start(hal_timerT2);//T2CONbits.TON = 1;
    T5CONbits.TON = 1; //hal_timer_start(hal_timerT5); //T5CONbits.TON = 1;

    adc_data.state = adc_st_started;
    __builtin_disi(0);

    return;

}


extern void adc_stop(void)
{
    // the two timers must stop at the same exact time to stay synchronized
        __builtin_disi(0x3fff);
    // disable in reverse order (compensate the instruction cycle between starts)
//    hal_timer_stop(hal_timerT5); 
//    hal_timer_stop(hal_timerT2);
    T5CONbits.TON = 0;
    T2CONbits.TON = 0;

    adc_data.state = adc_st_stopped;
    __builtin_disi(0);
  
    return;

}

extern adc_state_t adc_is_running(void)
{
    return(adc_data.state);
}




void __attribute__((__interrupt__, no_auto_psv)) _DMA3Interrupt(void) 
{

//Set in which buffer new data are available
    if( (adc_spi_dmabuf_none == adc_data.dma_buff_index) || (adc_spi_dmabuf_2 == adc_data.dma_buff_index) )
    {
        adc_data.dma_buff_index = adc_spi_dmabuf_1;
    }
    else
    {
        adc_data.dma_buff_index = adc_spi_dmabuf_2;
    }

    /* if adc is not filtering, then sends evt to main loop to start filter phase
       because new data are avilable,
       else indicates new data are available so, when this filetr phase is finished, 
       it is possible to start new one immeditely with new data.
    */

   
    if(0 == adc_data.flt_wd.is_filtering) //is idle
    {
        eos_foop_SetEvent(eos_foop_GetHandle(), EVT_IIR_FILTER_START);
    }
    else
    {
        adc_data.flt_wd.new_data = 1;
    }


//    s_adc_filter_all_channel();


    IFS2bits.DMA3IF = 0; // Clear the DMA1 Interrupt Flag

}


extern void adc_filtering_start(void)
{
    adc_spi_dmabuf_number_t n;
    uint16_t sample, offs;
    hal_time_t remaining_time;

//    hal_timer_remaining_read(hal_timer4, &remaining_time);
//    if(remaining_time < 100)
//    {
//        eos_foop_SetEvent(eos_foop_GetHandle(), EVT_IIR_FILTER_START);
//        return;
//    }

//    s_adc_filter_all_channel();

    adc_data.flt_wd.ch_num = 0;

    IEC2bits.DMA3IE = 0; //disable DMA interrupt
    adc_data.flt_wd.is_filtering = 1; // set filter phase is started
    n = adc_data.dma_buff_index;
    IEC2bits.DMA3IE = 1; //ena DMA interrupt

    if(adc_spi_dmabuf_1 == n)
    {
        for( sample = 0, offs = 0; sample < ADC_NUM_SAMPLES; offs += 6, sample++)
        {
//            adc_data.values[0][sample] = adc_spi_dmabuf1[0 + offs] + HEX_VALC;
//            adc_data.values[1][sample] = adc_spi_dmabuf1[1 + offs] + HEX_VALC;
//            adc_data.values[2][sample] = adc_spi_dmabuf1[2 + offs] + HEX_VALC;
//            adc_data.values[3][sample] = adc_spi_dmabuf1[3 + offs] + HEX_VALC;
//            adc_data.values[4][sample] = adc_spi_dmabuf1[4 + offs] + HEX_VALC;
//            adc_data.values[5][sample] = adc_spi_dmabuf1[5 + offs] + HEX_VALC;

//con swap            
            adc_data.values[1][sample] = adc_spi_dmabuf1[0 + offs] + HEX_VALC;
            adc_data.values[2][sample] = adc_spi_dmabuf1[1 + offs] + HEX_VALC;
            adc_data.values[3][sample] = adc_spi_dmabuf1[2 + offs] + HEX_VALC;
            adc_data.values[4][sample] = adc_spi_dmabuf1[3 + offs] + HEX_VALC;
            adc_data.values[5][sample] = adc_spi_dmabuf1[4 + offs] + HEX_VALC;
            adc_data.values[0][sample] = adc_spi_dmabuf1[5 + offs] + HEX_VALC;


//            adc_data.values[0][sample] = 0x1 + HEX_VALC;
//            adc_data.values[1][sample] = 0x2 + HEX_VALC;
//            adc_data.values[2][sample] = 0x3 + HEX_VALC;
//            adc_data.values[3][sample] = 0x4 + HEX_VALC;
//            adc_data.values[4][sample] = 0x5 + HEX_VALC;
//            adc_data.values[5][sample] = 0x6 + HEX_VALC;
        }

        adc_data.flt_wd.actual_flt_buff_infex = 1;
    }
    else
    {
        for( sample = 0, offs = 0; sample < ADC_NUM_SAMPLES; offs += 6, sample++)
        {
//            adc_data.values[0][sample] = adc_spi_dmabuf2[0 + offs] + HEX_VALC;
//            adc_data.values[1][sample] = adc_spi_dmabuf2[1 + offs] + HEX_VALC;
//            adc_data.values[2][sample] = adc_spi_dmabuf2[2 + offs] + HEX_VALC;
//            adc_data.values[3][sample] = adc_spi_dmabuf2[3 + offs] + HEX_VALC;
//            adc_data.values[4][sample] = adc_spi_dmabuf2[4 + offs] + HEX_VALC;
//            adc_data.values[5][sample] = adc_spi_dmabuf2[5 + offs] + HEX_VALC;

//con swap            
            adc_data.values[1][sample] = adc_spi_dmabuf2[0 + offs] + HEX_VALC;
            adc_data.values[2][sample] = adc_spi_dmabuf2[1 + offs] + HEX_VALC;
            adc_data.values[3][sample] = adc_spi_dmabuf2[2 + offs] + HEX_VALC;
            adc_data.values[4][sample] = adc_spi_dmabuf2[3 + offs] + HEX_VALC;
            adc_data.values[5][sample] = adc_spi_dmabuf2[4 + offs] + HEX_VALC;
            adc_data.values[0][sample] = adc_spi_dmabuf2[5 + offs] + HEX_VALC;

//            adc_data.values[0][sample] = 0x1 + HEX_VALC;
//            adc_data.values[1][sample] = 0x2 + HEX_VALC;
//            adc_data.values[2][sample] = 0x3 + HEX_VALC;
//            adc_data.values[3][sample] = 0x4 + HEX_VALC;
//            adc_data.values[4][sample] = 0x5 + HEX_VALC;
//            adc_data.values[5][sample] = 0x6 + HEX_VALC;
        }

        adc_data.flt_wd.actual_flt_buff_infex = 2;
    }
  
    eos_foop_SetEvent(eos_foop_GetHandle(), EVT_IIR_FILTER_CONTINUE);

}



extern void adc_filtering_continue(void)
{

    switch (adc_data.SIXsg_config_ptr->behaviour_cfg.filt_data_mode)
    {
        case filtMode_none:
        {
            adc_data.filtered_values[adc_data.flt_wd.actual_flt_buff_infex-1][adc_data.flt_wd.ch_num] = adc_data.values[adc_data.flt_wd.ch_num][ADC_NUM_SAMPLES-1];
        } break;
        
        case filtMode_iir:
        {
            IIRTransposed( ADC_NUM_SAMPLES, &output_iir[adc_data.flt_wd.ch_num][0], &adc_data.values[adc_data.flt_wd.ch_num][0], &iir_filter_struct[adc_data.flt_wd.ch_num]);
            adc_data.filtered_values[adc_data.flt_wd.actual_flt_buff_infex-1][adc_data.flt_wd.ch_num] = output_iir[adc_data.flt_wd.ch_num][ADC_NUM_SAMPLES-1];
        } break;
        
        case filtMode_average:
        {
            //currently not implemented
            adc_data.filtered_values[adc_data.flt_wd.actual_flt_buff_infex-1][adc_data.flt_wd.ch_num] = 0;
        } break; 
    }
    
    adc_data.flt_wd.ch_num++;

    if(adc_data.flt_wd.ch_num < 6)
    {
        eos_foop_SetEvent(eos_foop_GetHandle(),  EVT_IIR_FILTER_CONTINUE);  
        return;
    }

    if(6 == adc_data.flt_wd.ch_num)
    {
        adc_data.flt_wd.ch_num = 0;
        adc_data.flt_wd.last_flt_data_buff_index = adc_data.flt_wd.actual_flt_buff_infex;

        IEC2bits.DMA3IE = 0; //disable DMA interrupt because new data is shared with it
//        if new data are available send evt to start filter phase again.
        if(adc_data.flt_wd.new_data)
        {
            adc_data.flt_wd.new_data = 0;
            IEC2bits.DMA3IE = 1; //enable DMA intterupt
            eos_foop_SetEvent(eos_foop_GetHandle(), EVT_IIR_FILTER_START);  
            return;
        }
        else
        {
            adc_data.flt_wd.is_filtering = 0; //exit from filter phase
            IEC2bits.DMA3IE = 1;
        }
    }  
    return;
 
}

extern hal_result_t adc_get_data(int16_t *buff)
{
    int16_t aux[AN_CHANNEL_NUM];
    int8_t i;

    if(NULL == buff)
    {
        return(hal_res_NOK_nullpointer);
    }

    if(adc_spi_dmabuf_none == adc_data.flt_wd.last_flt_data_buff_index)
    {
        return(hal_res_NOK_nodata);
    }
    
    memcpy(buff, &adc_data.filtered_values[adc_data.flt_wd.last_flt_data_buff_index-1][0], sizeof(int16_t)*AN_CHANNEL_NUM );
 
    return(hal_res_OK);
}


extern hal_result_t adc_get_ch_value(uint8_t ch_num, uint8_t filtered, int16_t *value)
{
    if( (ch_num >= AN_CHANNEL_NUM) || (NULL == value) )
    {
        return(hal_res_NOK_wrongparam);
    }

    if(adc_spi_dmabuf_none == adc_data.flt_wd.last_flt_data_buff_index)
    {
        return(hal_res_NOK_nodata);
    }

    if(filtered)
    {
        *value = adc_data.filtered_values[adc_data.flt_wd.last_flt_data_buff_index-1][ch_num];
    }
    else
    {
        *value = adc_data.values[ch_num][ADC_NUM_SAMPLES-1];
    }

    return(hal_res_OK);
    
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_adc_iir_filter_init(void)
{
    int16_t i;
    // inizializzazione filtri IIR
    for(i=0; i<AN_CHANNEL_NUM; i++)
    {
        iir_filter_struct[i].numSectionsLess1 = 2;  // Nbq - 1
        iir_filter_struct[i].coeffsBase = &adc_data.SIXsg_config_ptr->SIXsg_ee_data.iir_tr_coef[0];
        iir_filter_struct[i].coeffsPage = COEFFS_IN_DATA;
        iir_filter_struct[i].delayBase1 = &iir_delay1[i][0];
        iir_filter_struct[i].delayBase2 = &iir_delay2[i][0];
        iir_filter_struct[i].finalShift = 0;
        // Zero the filter state
        IIRTransposedInit(&iir_filter_struct[i]);
    }
}


// Configure DMA channel for SPI data transfer
static void s_adc_dma_init(void)
{
    // Clears interrupt flag
    IFS2bits.DMA3IF =0;
    // DMA in continuous mode
    DMA3CON = 0x0000;
    // enable ping pong
    DMA3CONbits.MODE =2; 
    // set DMA destination buffers 
    DMA3STA = __builtin_dmaoffset(&adc_spi_dmabuf1);
    DMA3STB = __builtin_dmaoffset(&adc_spi_dmabuf2);
    // set DMA source address
    DMA3PAD = (volatile uint16_t) &SPI1BUF;
    // transfer one word at a time 
    DMA3CNT = 6*ADC_NUM_SAMPLES -1;
    // Binds DMA3 CH to SPI module
    DMA3REQ = 0x000A;
    // Enables DMA CH3
    DMA3CONbits.CHEN=1;
    // clears DMA interrupt flag
    IFS2bits.DMA3IF = 0;
    // Enable dma interrupt ch 3
    IEC2bits.DMA3IE = 1;

    IPC9 = 0;
    IPC9bits.DMA3IP = 4; //set interrupt priority
}


static hal_result_t s_adc_ad7689_spi_init(void)
{
    volatile uint16_t dummy;
    uint32_t wait;
    int16_t i,j;

    s_adc_ad7689_spi_gpio_init();

    // initialise SPI module and AD7689 
    // writes configuration register
    
    SPI1CON1bits.PPRE = 0b11 ; // set primary prescaler to 1:1
    SPI1CON1bits.SPRE = 0b100 ; // set secondary prescaler to 1:4
    SPI1CON1bits.MSTEN = 1; // spi in master mode
    
    // the AD changes the miso on falling edge.
    // the AD samples the mosi on rising edge.
    SPI1CON1bits.CKP = 0; // clock is high when idle
    SPI1CON1bits.CKE = 1; //SDO (mosi) changes on falling edge, A/D should sample on rising
    SPI1CON1bits.SMP = 1; // miso sample phase: opposite of mosi
    SPI1CON1bits.MODE16 = 1; // 16bit operating mode
    SPI1CON1bits.DISSDO = 0; // do NOT disable sdo (mosi) pin
    SPI1CON1bits.DISSCK = 0; // do NOT disable clk pin
    
    SPI1CON2bits.FRMEN = 0; //disables frame sync mode
    
    SPI1STATbits.SPIEN = 1; // enables spi module
    
    // initialize CS as high
    ADC_AD7689_SPI_SLAVE_SELECT = 1;
    asm("nop");
    asm("nop");
    asm("nop");
    asm("nop");
    
    // now SPI is being fired two times.
    // the first time we clock in the CFG register.
    // the second time we just throw away an otherwise invalid reaging.
    // In fact, according to the AD datasheet, the CFG register value
    // will be used for the next sample, and each sample is collected
    // on the next turn.
    
    ADC_AD7689_SPI_SLAVE_SELECT = 0;
    
    // Programming can be done in the ACQ phase or on the CNV phase
    // or across ACQ->CNV phase but NOT across CNV->ACQ phase otherwise
    // it will fail.
    // Transition from CNV to ACQ is done by AD7689 when CNV is done.
    // tConv is approx 1.2us, SPI runs @ 10Mhz x 16 clk, so it takes about 
    // 1.6us. For this reason it is not possible to lower the SS and start
    // writing SPI. It is easy and safe to lower SS, wait for (more than)
    // 1.2 us and then start SPI writing on next ACQ phase.
    for(i=0;i < 20; i++)
    {
        // waits about 25*4*20ns = 2 us plus 'for' loop overhead
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
    }
    // writes the CFG register in the ACQ phase
    SPI1BUF = adc_reg_default_values.w; //ad7689_cfg.w; 
    
    //SPI should take about 1.6 us to complete
    // wait for 25*4*30 = 3us plus 'if' instruction and for loop overhead     
    for(i=0; i< 30; i++)
    {
        if(SPI1STATbits.SPIRBF != 0)
        { 
            break;
        }
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
    }

    // check if the previous loop broke out or timed out.
    if(SPI1STATbits.SPIRBF == 0)
    {
        return(hal_res_NOK_hwerror);
    }
    
    // empty SPI buffer
    dummy = SPI1BUF;
    
    // deselect slave
    ADC_AD7689_SPI_SLAVE_SELECT = 1;
    
    // A/D now starts to generate VREF.
    // wait for voltage to stabilize
    
    /* 25*10*100000 ns = 25 ms */
    wait = 100000;
    while(wait--)
    {
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
        asm("nop");
    }

    // now CFG reg has been wrote, but we need another SPI
    // cycle in order to have CFG to have effect.
    
    // reconfigure SPI for reading.
    SPI1STATbits.SPIEN = 0; // enables spi module
    SPI1CON1bits.CKP = 1; // clock is high when idle
    SPI1STATbits.SPIEN = 1; // enables spi module
    
    //throw away 2 samples in orederto avoid shifts in channel sequence
    for(j=0;j<2;j++)
    {
        asm("nop");
        
        // select slave again
        ADC_AD7689_SPI_SLAVE_SELECT = 0;
        
        // wait for the ACQ+CNV phases
        for(i=0;i < 60; i++)
        {
            // waits about 25*4*60ns = 6 us plus 'for' loop overhead
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
        }
        // write SPI
        SPI1BUF = 0; 
      
        //SPI should take about 1.6 us to complete
        // wait for 25*4*30 = 3us plus 'if' instruction and for loop overhead     
        for(i=0; i<30; i++)
        {
            if(SPI1STATbits.SPIRBF != 0)
            { 
                break;
            }
            asm("nop");
            asm("nop");
            asm("nop");
            asm("nop");
        }
        
        // check if the previous loop broke out or timed out.
        if(SPI1STATbits.SPIRBF == 0)
        {
            return(hal_res_NOK_hwerror);
        }
        
        //empty spi buffer
        dummy = SPI1BUF;
        
        ADC_AD7689_SPI_SLAVE_SELECT = 1;
    
    }
    // now the SPI is configured and working!
    return(hal_res_OK);
}

static void s_adc_ad7689_spi_gpio_init(void)
{

    // issue an UNLOCK sequence to change OSCCON.IOLOCK (bit 6)
    __builtin_write_OSCCONL( OSCCON & ~0x40 );
    
    // default all pins to DIGITAL
    AD1PCFGL = 0xffff; 
    
    
    /********* Outputs *********/ 
    TRISBbits.TRISB15 = 0;
    // RPOR7bits.RP15R = 0b01001;// SPI Slave Select
    TRISBbits.TRISB14 = 0;
    RPOR7bits.RP14R = 0b00111;// SPI mosi
    TRISBbits.TRISB13 = 0;
    RPOR6bits.RP13R = 0b01000; // SPI ck
    

    /********* Inputs *********/
    RPINR20bits.SDI1R = 12; // Connect SPI1 MISO on RP12.

    // issue an LOCK sequence
    __builtin_write_OSCCONL( OSCCON | 0x40 );

}


static void s_adc_ad7689_timers_init(void)
{
//1) Setup timer 2 registers for CNV

    hal_timer_cfg_t tmr_cfg = 
    {
        .prescaler          = hal_timer_prescaler1,         
        .countdown          = ADC_SAMPLE_RATE,
        .priority           = hal_int_priority00,
        .mode               = hal_timer_mode_periodic,
        .callback_on_exp    = s_adc_ad7689_timer2_callback,
        .arg                = NULL
    };

    #warning --> asfidanken: the offset is always in nanoseconds ...
    
    hal_timer_init(hal_timer2, &tmr_cfg, NULL);
    hal_timer_offset_write(hal_timer2, 3300); // reset timer. 3.3 us phase vs timer 5.
    hal_timer_interrupt_enable(hal_timer2);



 
//2) Setup timer 5 registers for SPI
    
    tmr_cfg.callback_on_exp = s_adc_ad7689_timer5_callback;

    hal_timer_init(hal_timer5, &tmr_cfg, NULL);
    hal_timer_offset_write(hal_timer5, 0);
    hal_timer_interrupt_enable(hal_timer5);
 
}


static void s_adc_ad7689_timer2_callback(void *arg)
{
    ADC_AD7689_SPI_SLAVE_SELECT = 1;

    // clear INT flag and wait 25 ns (10 are needed)
    //here I must wait 10 ns al least, so I put a null instruction (25 ns)
    asm("nop");

    // Trigger the new conversion 
    ADC_AD7689_SPI_SLAVE_SELECT = 0;
}

static void s_adc_ad7689_timer5_callback(void *arg)
{
    // starts the SPI for reading prev conv
    SPI1BUF = 0;
}



static void s_adc_filter_all_channel(void)
{

    uint16_t sample, offs;

    if(adc_spi_dmabuf_1 == adc_data.dma_buff_index)
    {
        for( sample = 0, offs = 0; sample < ADC_NUM_SAMPLES; offs += 6, sample++)
        {
            adc_data.values[0][sample] = adc_spi_dmabuf1[0 + offs] + HEX_VALC;
            adc_data.values[1][sample] = adc_spi_dmabuf1[1 + offs] + HEX_VALC;
            adc_data.values[2][sample] = adc_spi_dmabuf1[2 + offs] + HEX_VALC;
            adc_data.values[3][sample] = adc_spi_dmabuf1[3 + offs] + HEX_VALC;
            adc_data.values[4][sample] = adc_spi_dmabuf1[4 + offs] + HEX_VALC;
            adc_data.values[5][sample] = adc_spi_dmabuf1[5 + offs] + HEX_VALC;
        }

        adc_data.flt_wd.actual_flt_buff_infex = 1;
    }
    else
    {
        for( sample = 0, offs = 0; sample < ADC_NUM_SAMPLES; offs += 6, sample++)
        {
            adc_data.values[0][sample] = adc_spi_dmabuf2[0 + offs] + HEX_VALC;
            adc_data.values[1][sample] = adc_spi_dmabuf2[1 + offs] + HEX_VALC;
            adc_data.values[2][sample] = adc_spi_dmabuf2[2 + offs] + HEX_VALC;
            adc_data.values[3][sample] = adc_spi_dmabuf2[3 + offs] + HEX_VALC;
            adc_data.values[4][sample] = adc_spi_dmabuf2[4 + offs] + HEX_VALC;
            adc_data.values[5][sample] = adc_spi_dmabuf2[5 + offs] + HEX_VALC;
        }

        adc_data.flt_wd.actual_flt_buff_infex = 2;
    }

    
   switch (adc_data.SIXsg_config_ptr->behaviour_cfg.filt_data_mode)
    {
        case filtMode_none:
        {
            for(adc_data.flt_wd.ch_num = 0 ; adc_data.flt_wd.ch_num < 6; adc_data.flt_wd.ch_num++)
            {
                adc_data.filtered_values[adc_data.flt_wd.actual_flt_buff_infex-1][adc_data.flt_wd.ch_num] = adc_data.values[adc_data.flt_wd.ch_num][ADC_NUM_SAMPLES-1];
            }
        }
        break;
        
        case filtMode_iir:
        {

            for(adc_data.flt_wd.ch_num = 0 ; adc_data.flt_wd.ch_num < 6; adc_data.flt_wd.ch_num++)
            {
                IIRTransposed( ADC_NUM_SAMPLES, &output_iir[adc_data.flt_wd.ch_num][0], &adc_data.values[adc_data.flt_wd.ch_num][0], &iir_filter_struct[adc_data.flt_wd.ch_num]);
                adc_data.filtered_values[adc_data.flt_wd.actual_flt_buff_infex-1][adc_data.flt_wd.ch_num] = output_iir[adc_data.flt_wd.ch_num][ADC_NUM_SAMPLES-1];
            }

        }
        break;
        
        case filtMode_average:
        {
            //currently not implemented
            adc_data.filtered_values[adc_data.flt_wd.actual_flt_buff_infex-1][adc_data.flt_wd.ch_num] = 0;
        }
        break; 
    }
    

    adc_data.flt_wd.last_flt_data_buff_index = adc_data.flt_wd.actual_flt_buff_infex;
            

}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



