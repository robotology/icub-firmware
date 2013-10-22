/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author: Valentina Gaggero
 * email:   valentina.gaggero@iit.it
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

/*  @file       main_6sg.c
    @brief      This file implements the core of 6SG application
    @author     valentina.gaggero@iit.it
    @date       04/14/2011
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdlib.h"
#include <string.h>



// hal dspic33f
#include "hal.h"
#include "hal_arch_dspic.h"
#include "hal_led.h"

// embobj-generic 
#include "EoCommon.h"
//#include "EOtheMemoryPool.h"
//#include "EOtheErrormanager.h"

// embobj-singletask
#include "EOStheSystem.h"
//#include "EOStheTimerManager.h"
//#include "EOStheFOOP.h"

//application
#include "parser.h"
#include "calculator.h"
#include "6sg_config.h"
#include "sender_outMsg.h"
#include "adc.h"
#include "amplifier.h"

#ifdef _DEBUG_
    #include "debug_util.h"
#endif

#include "EOaction_hid.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define led_yellow                  hal_led0
#define led_red                     hal_led1 

#warning -> acemor: dont use 0b0000 etc because it is not standard c

//events: defined in order by priority (high --> low)
//#define EVT_SENDERTMR_TIMEOUT     (0x1<<0)  // evt pos is 1 //defined in trasmitter_outMsg.c 
#define EVT_IIR_FILTER_START        (0x1<<1)    // evt pos is 2 // defined also in adc.c
#define EVT_IIR_FILTER_CONTINUE     (0x1<<2)    // evt pos is 3 // defined also in adc.c
#define EVT_MSG_RECEIVED            (0x1<<3)    // evt pos is 4 
#define EVT_LED                     (0x1<<4)    // evt pos is 5 

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

void s_init(void *p, uint32_t u);
static void s_can_init(uint8_t board_address);
static void s_led_flash(void);
static void s_on_event(uint32_t evtpos, eOevent_t evtmsk);
static void s_on_tick(void);
static void s_can_callBkp_onRec(void *arg);
static void s_send_outgoing_msg(void);
static void s_errman_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info);
static void s_appl_init(void); //init application's modules
static void s_hal_init(void);  //init hal by allocating memory and by init hw if it is necessary.
static void s_hal_sys_init(void);// init system

static void s_resetCause_send(void);
static void s_eeprom_init(void);

#ifdef _DEBUG_
static void s_send_outgoing_msg_TEST(void);
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------
#warning Removed const from declaration of  eOssystem_cfg_t s_syscfg

/*
    Note: I removed const from declaration of  eOssystem_cfg_t s_syscfg
          because it produced fatal error trapped by  _AddressError.
*/

static /*const*/ eOssystem_cfg_t s_syscfg =
{
    .hal_fns                                =
    {
        .hal_base_init                      = s_hal_init, //s_eos_test_initialise_hal,
        .hal_sys_systeminit                 = s_hal_sys_init, // (eOvoid_fp_void_t)hal_sys_systeminit,
        .hal_sys_systick_sethandler         = (eOvoid_fp_voidfpvoiduint32uint8_t)hal_sys_systick_sethandler,
        .hal_sys_atomic_bitwiseAND          = hal_sys_atomic_bitwiseAND,
        .hal_sys_atomic_bitwiseOR           = hal_sys_atomic_bitwiseOR,
        .hal_sys_criticalsection_take       = (eOres_fp_voidp_uint32_t)hal_sys_criticalsection_take,
        .hal_sys_criticalsection_release    = (eOres_fp_voidp_t)hal_sys_criticalsection_release,
        .hal_sys_irq_disable                = hal_sys_irq_disable,
        .hal_sys_irq_enable                 = hal_sys_irq_enable     
    }, 
    .fsal_fns                               =
    {
        .fsal_init                          = NULL
    }, 
    .userdef                                =
    {
        .systickperiod                      = 500,
        .systickpriority                    = hal_int_priority07, 
        .on_systick                         = s_on_tick //NULL //s_my_on_tick 
    }
};

#warning -> acemor: mettere const in eOerrman_cfg_t
static eOerrman_cfg_t errman_cfg =
{
    .extfn              =
    {
        .usr_on_error   = s_errman_OnError
    }
};

static const eOstimerman_cfg_t s_thetimerman_cfg = 
{
#ifdef _TIMER_HW_
    .timernum = 0
#else 
    .timernum = 3
#endif
};

static const eOscallbackman_cfg_t s_thecallbackman_cfg = 
{
    .queuesize = 2
};

static const eOsfoop_cfg_t s_thefoopcfg = 
{ 
    .messagefifosize                    = 0,
    .callbackfifosize                   = 2, // overhidden by cbkman cfg  
    .usrfn                              =
    {
        .on_startup                     = NULL,
        .on_event                       = s_on_event,
        .on_message                     = NULL
    } 
};




//// struct with configuration data for 6SG application
//// Here are defined only data that must be configured at compile time.
//// other configuration data will be read from eeprom.
//static SIXsg_config_data_t my_6sg_cfg =
//{
////    .gen_ee_data = {0}, here don't care (saved in eeprom)
////    .SIXsg_ee_data = {0}, here don't care (saved in eeprom)
//    .behaviour_cfg = 
//    {
//        .send_ack_each_cmd = 1, //if = 1 send ack for each command (polling msg)
//        .save_in_ee_immediately = 1, //if = 1 save current values of ee_data in eeprom each time a cmd change a value of data in eeprom.      
//        .filt_data_mode = filtMode_iir, // filtMode_iir, //TODO: da spostare per adc
//        .tx_outMsg_mode = tx_outMsg_off  //tx_outMsg_torqueData_on
//    }
//};

static config_behaviour_t my_6sg_cfg_behav =
{
    .send_ack_each_cmd = 0, //if = 1 send ack for each command (polling msg)
    .save_in_ee_immediately = 1, //if = 1 save current values of ee_data in eeprom each time a cmd change a value of data in eeprom.      
    .filt_data_mode = filtMode_none,// filtMode_iir, //TODO: da spostare per adc
    .tx_outMsg_mode = tx_outMsg_readOnly  //tx_outMsg_torqueData_on
};




static const uint8_t s_start_str[] = "START";
static const uint8_t s_hello_str[] = "HELLO";
static const uint8_t s_error_str[] = "ERROR";
//questa var globale si potrebbe mettere locale in tutti i metodi che la usano chimando la funzione 
//SIXsg_config_get, ma per ora non voglio alterare troppo il codice
#warning VALE-->remove global var
SIXsg_config_data_t *cfg_ptr = NULL;

// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

/// keep just before the main()
hal_arch_dspic_base_CONFIG_REGISTERS_BASIC(0)

int main(void)
{
   eos_sys_Initialise(  &s_syscfg,
                        NULL,                   // mempool cfg
                        &errman_cfg,            // errman cfg 
                        &s_thetimerman_cfg,     // timerman cfg
                        &s_thecallbackman_cfg,  // callbackman cfg
                        &s_thefoopcfg           // the foop cfg
                      );  
    
    eos_sys_Start(eos_sys_GetHandle(), s_appl_init);

    return(0);

}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

  
// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

inline void s_check_fault_error(hal_result_t result)
{
    if(hal_res_OK != result)
    {
        s_errman_OnError(eo_errortype_fatal, 0, "", "");
    }
}

static void s_errman_OnError(eOerrmanErrorType_t errtype, eOid08_t taskid, const char *eobjstr, const char *info)
{
    hal_can_frame_t frame;

    if( (eo_errortype_weak == errtype) || (eo_errortype_fatal == errtype) )
    {
        //notify error    
        frame.id = cfg_ptr->gen_ee_data.board_address;
        frame.id_type = hal_can_frameID_std;
        frame.frame_type = hal_can_frame_data;
        frame.size = 5;
        memcpy(frame.data, s_error_str, 5);
       
        hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);
        while(1)
        {;}
          
    }
    else // eo_errortype_info or eo_errortype_warning
    {
        return;
    }

}

static void s_hal_init(void)
{
    extern const hal_cfg_t* hal_cfgAPP;
    uint32_t sizememory;
    uint32_t* memory = NULL;

    // how much memory do we need?
    hal_base_memory_getsize(hal_cfgAPP, &sizememory);
    
    if(0 != sizememory)
    {
        memory = (uint32_t*)calloc(sizememory/4, sizeof(uint32_t));   
    }
    
    // pass memory
    hal_base_initialise(hal_cfgAPP, memory);

}

static void s_hal_sys_init(void)
{
    //initialise basic system services    
    hal_sys_systeminit();
}
  

static void s_appl_init(void)
{
    hal_can_frame_t frame;
    hal_result_t res;
    hal_led_cfg_t l_cfg = { .dummy = 0 };

    // initialise peripherals
    hal_led_init(led_yellow, &l_cfg);
    hal_led_init(led_red, &l_cfg);

    // init eeprom
    s_eeprom_init();




    // get config data from eeprom
    res = SIXsg_config_init(&my_6sg_cfg_behav); 
    s_check_fault_error(res);

    
    //set here global var
    cfg_ptr = SIXsg_config_get();
    if(NULL == cfg_ptr)
    {
        s_check_fault_error(hal_res_NOK_generic);
        #warning: migliora gestione errore grave!!!!
    }

 //   hal_led_on(led_red);
    s_can_init(cfg_ptr->gen_ee_data.board_address);

   // hal_led_on(led_yellow);
    // init application modules
    res = adc_init(cfg_ptr);
    s_check_fault_error(res);
    ampl_init(cfg_ptr);     // Note: here I don't check result, because I'm sure &my_6sg_cfg now exists.
    sender_init(cfg_ptr);
    parser_init(cfg_ptr);
    calculator_init(cfg_ptr);

    // start using the application modules
    ampl_set_gain_and_offset();
    adc_start();
    sender_start();

    
    s_resetCause_send();

    // notify I'm alive    
    frame.id = cfg_ptr->gen_ee_data.board_address;
    frame.id_type = hal_can_frameID_std;
    frame.frame_type = hal_can_frame_data;
    frame.size = 5;
    memcpy(frame.data, s_start_str, 5);
   
    res = hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);    

    s_led_flash();
    
    #warning acemor -> LATBbits.LATB9 ?? da togliere assolutamente 
    LATBbits.LATB9 = 0x0;
}



static void s_resetCause_send(void)
{
    hal_can_frame_t frame;

    #warning VALE-> add this function to HAL??

    frame.id = cfg_ptr->gen_ee_data.board_address;
    frame.id_type = hal_can_frameID_std;
    frame.frame_type = hal_can_frame_data;
    frame.size = 8;
    memset(frame.data, 0, 8);

    #warning acemor -> RCONbits.POR etc ?? da togliere assolutamente 
    if(RCONbits.POR!=0)
    {
        frame.data[0] = 1;
    }
    if(RCONbits.BOR!=0)
    {   
        frame.data[1] = 1;
    }
    if(RCONbits.WDTO!=0)
    {
        frame.data[2] = 1;
    }

    if(RCONbits.SWR!=0) // Software Reset (Instruction) 
    {
        frame.data[3] = 1;
    }
    if(RCONbits.EXTR!=0) // External Reset (MCLR) Pin
    {
        frame.data[4] = 1;
    } 

    if(RCONbits.CM!=0) //  Configuration Mismatch Flag 
    {
        frame.data[5] = 1;
    } 

    if(RCONbits.IOPUWR!=0) //  : Illegal Opcode or Uninitialized W Access Reset Flag
    {
        frame.data[6] = 1;
    } 
    
    if(RCONbits.TRAPR!=0) //    Trap Reset Flag bit
    {
        frame.data[7] = 1;
    }
    
    hal_can_put(hal_can_port1, &frame, hal_can_send_normprio_now);
}



static void s_on_event(uint32_t evtpos, eOevent_t evtmsk)
{

    switch(evtpos) 
    {

        case 0: // EVT_SENDERTMR_TIMEOUT
        {
#ifdef _DEBUG_
            ledRED_toogle;
#endif
            s_send_outgoing_msg();
//            s_send_outgoing_msg_TEST();

        }
        break;

        case 1: // EVT_IIR_FILTER_START 
        {
            adc_filtering_start();
        }
        break;

        case 2: // EVT_IIR_FILTER_CONTINUE 
        {
            adc_filtering_continue();
        }
        break;


        case 3: //EVT_MSG_RECEIVED
        {
            parse_message();
            //parse_test();
        }
        break;

        case 4: //EVT_LED
        {

#ifndef _DEBUG_
            hal_led_toggle(led_yellow);
#endif
        }
        break;
        
    }
    
}


static void s_on_tick(void)
{

    static uint32_t tick_num = 0;

    tick_num++;

    if(tick_num == 1000)
    {
        eos_foop_SetEvent(eos_foop_GetHandle(), EVT_LED);
        tick_num = 0;
    }

}

static void s_eeprom_init(void)
{
   static uint8_t flash[1024];
#define CRC16_USE_CRC16TABLE_IN_ROM
#ifndef CRC16_USE_CRC16TABLE_IN_ROM
    static uint16_t crc16table[256];
#else
    static uint16_t* crc16table = NULL;
#endif
    hal_eeprom_cfg_t eepromcfg;
    hal_crc_cfg_t crccfg;


    eepromcfg.flashpagebuffer   = flash;
    eepromcfg.flashpagesize     = sizeof(flash);    
    hal_eeprom_init(hal_eeprom_emulatedflash, &eepromcfg);
    
    crccfg.order                = hal_crc_order_16;
    crccfg.polynomial           = hal_crc_poly_crc16_ccitt;
    crccfg.crctblram            = crc16table; // not when hal_crc_poly_crc16_ccitt uses a precalculated const internal table
    hal_crc_init(hal_crc0, &crccfg);
}



static void s_can_init(uint8_t board_address)
{

    hal_can_cfg_t config = 
    {
        .runmode        = hal_can_runmode_isr_2txq1rxq,
        .baudrate       = hal_can_baudrate_1mbps,
        .priorx         = hal_int_priority03,
        .priotx         = hal_int_priority02,
        .callback_on_rx = s_can_callBkp_onRec,
        .arg_cb_rx      = NULL,   
        .callback_on_tx = NULL,
        .arg_cb_tx      = NULL   
    };

    hal_can_init(hal_can_port1, &config);

    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 0, (CAN_MSG_CLASS_POLLING| board_address), hal_can_frameID_std);
    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 1, (CAN_MSG_CLASS_POLLING| 0xF), hal_can_frameID_std);
    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 2, (CAN_MSG_CLASS_LOADER| board_address), hal_can_frameID_std);
    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 3, (CAN_MSG_CLASS_LOADER| 0xF), hal_can_frameID_std);

    hal_can_enable(hal_can_port1);
}

static void s_can_callBkp_onRec(void *arg)
{
    eos_foop_SetEvent(eos_foop_GetHandle(), EVT_MSG_RECEIVED);
}


static void s_send_outgoing_msg(void)
{
#ifdef _DEBUG_
    LATAbits.LATA3 =  ~LATAbits.LATA3;
#endif
//TODO: manca il controllo sulla saturazione!!!
    hal_can_frame_t frame;
    hal_result_t res;

    calc_data_output_t output;
    int16_t adc_values[AN_CHANNEL_NUM];

    
    // 1) get values from ADC
    res = adc_get_data(adc_values);
    if(hal_res_NOK_nodata == res)
    {
        return;
    }


    // 2) calculate data 
    if(tx_outMsg_torqueData_on ==  cfg_ptr->behaviour_cfg.tx_outMsg_mode)
    {
        calculate_torque_and_force_data(adc_values, &output);
    }
    else if(tx_outMsg_uncalibData_on ==  cfg_ptr->behaviour_cfg.tx_outMsg_mode)
    {
        calculate_uncalibrated_data(adc_values, &output);
    }
    else
    {
        return;
    }

    
    // 3) send data
    frame.id_type = hal_can_frameID_std;
    frame.frame_type = hal_can_frame_data;
    frame.size = 6; //o 7 con saturazione!!!
    
    output.array[0] += 0x8000; 
    output.array[1] += 0x8000; 
    output.array[2] += 0x8000; 
    output.array[3] += 0x8000; 
    output.array[4] += 0x8000; 
    output.array[5] += 0x8000; 
    
    //send first 3 values
    //(note: since board 6sg does not distinguish torque value from force, but they are all force, here I use array)
    frame.id = (CAN_MSG_CLASS_PERIODIC) | (cfg_ptr->gen_ee_data.board_address<<4) | (CAN_CMD_TORQUE_VECTOR) ; //0xB
    memcpy(frame.data, &output.array[3], sizeof(int16_t) *3);
    hal_can_put(hal_can_port1, &frame, hal_can_send_highprio_now );

    //send last 3 values
    frame.id = (CAN_MSG_CLASS_PERIODIC) | (cfg_ptr->gen_ee_data.board_address<<4) | (CAN_CMD_FORCE_VECTOR) ; //0xA
    memcpy(frame.data, &output.array[0], sizeof(int16_t) *3);
    hal_can_put(hal_can_port1, &frame, hal_can_send_highprio_now );

#ifdef _DEBUG_
    LATAbits.LATA3 =  ~LATAbits.LATA3;
#endif
}






#ifdef _DEBUG_
static void s_send_outgoing_msg_TEST(void)
{

    hal_can_frame_t frame;
    int16_t adc_values[AN_CHANNEL_NUM] = {1,2,3,4,5,6};
    calc_data_output_t output;
    hal_result_t res;

    res = adc_get_data(adc_values);
    if(hal_res_NOK_nodata == res)
    {
        return;
    }

//    calculate_torque_and_force_data(adc_values, &output);

    frame.id_type = hal_can_frameID_std;
    frame.frame_type = hal_can_frame_data;
    frame.size = 6; //o 7 con saturazione!!!
    if(tx_outMsg_torqueData_on ==  cfg_ptr->behaviour_cfg.tx_outMsg_mode)
    {
        frame.id = (CAN_MSG_CLASS_PERIODIC) | (cfg_ptr->gen_ee_data.board_address<<4) | (CAN_CMD_TORQUE_VECTOR) ;
        memcpy(frame.data, &adc_values[0], sizeof(int16_t)*3);
        hal_can_put(hal_can_port1, &frame, hal_can_send_highprio_now );
    }
    else if(tx_outMsg_uncalibData_on ==  cfg_ptr->behaviour_cfg.tx_outMsg_mode) 
    {
        //send torque values
        frame.id = (CAN_MSG_CLASS_PERIODIC) | (cfg_ptr->gen_ee_data.board_address<<4) | (CAN_CMD_TORQUE_VECTOR) ;
        memcpy(frame.data, &adc_values[0], sizeof(int16_t) *3);
        hal_can_put(hal_can_port1, &frame, hal_can_send_highprio_now );

        //send force values
        frame.id = (CAN_MSG_CLASS_PERIODIC) | (cfg_ptr->gen_ee_data.board_address<<4) | (CAN_CMD_FORCE_VECTOR) ;
        memcpy(frame.data, &adc_values[3], sizeof(int16_t) *3);
        hal_can_put(hal_can_port1, &frame, hal_can_send_highprio_now );
    }
    else if(tx_outMsg_forceData_on ==  cfg_ptr->behaviour_cfg.tx_outMsg_mode) 
    {
        frame.id = (CAN_MSG_CLASS_PERIODIC) | (cfg_ptr->gen_ee_data.board_address<<4) | (CAN_CMD_FORCE_VECTOR) ;
        memcpy(frame.data, &adc_values[3], sizeof(int16_t)*3);
        hal_can_put(hal_can_port1, &frame, hal_can_send_highprio_now );
    }

}
#endif

static void s_led_flash(void)
{
    uint16_t i;
    for(i=1;i<=40;i++)
    {  
        hal_led_toggle(led_yellow);
        hal_arch_dspic_delay(10000);
    }
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



