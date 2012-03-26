/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
 * Author:  Valentina Gaggero
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
// empty section


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "stdlib.h"
#include "string.h"
#include "hal.h"
#include "hal_arch_dspic.h"



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section



// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
//#include "entity_hid.h" 


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
#define _TEST_LED_
//#define _TEST_TIMER_
//#define _TEST_EEPROM_
//#define _TEST_NEWEEPROM_
//#define _TEST_CAN_  

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables. deprecated: better using _get(), _set() on static variables 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

#ifdef _TEST_NEWEEPROM_

#endif

#ifdef _TEST_EEPROM_
typedef struct
{
    uint8_t vector[400];
} my_small_data_t;
#endif


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void s_systick(void);

#ifdef _TEST_LED_
static void s_led_test(void);
static void s_led_only2foc_test(void);
#endif

#ifdef _TEST_TIMER_
static void s_timer_led_test(void);
static void s_timer2_callback(void *arg);
static void s_timer_can_test(void);
#endif

#ifdef _TEST_EEPROM_
static void s_eeprom_test(void);
#endif

#ifdef _TEST_NEWEEPROM_
static void s_eeprom_newtest(void);
#endif


#ifdef _TEST_CAN_
static void s_can_test_without_int(void);
static void s_can_test_1queue(void);
static void s_can_test_2queue(void);
static void s_can_test_receive(void);
static void s_can_test_process_a_received_frame(void);
static void s_can_call_back(void *arg);
static void s_can_test_receive_witoutISR(void);
#endif



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


#ifdef _TEST_EEPROM_
hal_arch_dspic_eeprom_emfl_DECLARE_VAR_IN_EE(ee_data_flash, my_small_data_t, ".edata") =
{
    .vector = {0xBB, 0xCC}
};

hal_arch_dspic_eeprom_emfl_DECLARE_VAR_IN_EE(ee_sh_data_flash, my_small_data_t, ".e_sh_data") =
{
    .vector = {0xDD, 0xEE}
};
#endif


#ifdef _TEST_TIMER_
static uint16_t timer_count = 0;
hal_time_t remaining_time = 0;
hal_can_frame_t frame2, frame4;
#endif

#ifdef _TEST_CAN_
static hal_can_frame_t frames_ptr[10];
static volatile uint8_t received_a_frame = 0;
#endif


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

// keep just before the main()
hal_arch_dspic_base_CONFIG_REGISTERS_BASIC(0)

int main(void)
{
    extern const hal_cfg_t* hal_cfgMINE;
    uint32_t sizememory;
    uint32_t* memory = NULL;

    // how much memory do we need?
    hal_base_memory_getsize(hal_cfgMINE, &sizememory);
    
    // get sizememory bytes of ram and put them in memory;
    memory = memory;

    if(0 != sizememory)
    {
        memory = (uint32_t*)calloc(sizememory/4, sizeof(uint32_t));   
    }

    
    // pass memory
    hal_base_initialise(hal_cfgMINE, memory);
    
    // init the system.
    
    hal_sys_systeminit();

    hal_sys_systick_sethandler(s_systick, 1000, hal_int_priority01);

#ifdef _TEST_CAN_
    {
        uint8_t i;
        //init can frames
        for(i=0; i<10; i++)
        {
            frames_ptr[i].id = 0x0F;
            frames_ptr[i].id_type = hal_can_frameID_std;
            frames_ptr[i].frame_type = hal_can_frame_data;
            frames_ptr[i].size = 8;
            memset(&(frames_ptr[i].data), i, 8);
        }
    }
#endif


#ifdef _TEST_CAN_
    //s_can_test_without_int();
    //s_can_test_1queue();
    //s_can_test_2queue();
    s_can_test_receive();
    //s_can_test_receive_witoutISR();
#endif


#ifdef _TEST_EEPROM_
    s_eeprom_test();
#endif

#ifdef _TEST_NEWEEPROM_
    s_eeprom_newtest();
#endif

    
#ifdef _TEST_TIMER_
    //s_timer_led_test();
    s_timer_can_test();
#endif

#ifdef _TEST_LED_
    s_led_test();
    //s_led_only2foc_test();
#endif

    for(;;);
    
    return(0);
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#ifdef _TEST_LED_
static void s_led_test(void)
{
    #define led_red         hal_led0 
    hal_led_cfg_t l_cfg = { .dummy = 0 };
    
    hal_led_init(led_red, &l_cfg);
    hal_led_on(led_red);

    
    while(1)
    {
        hal_led_toggle(led_red);
        hal_arch_dspic_delay(1000000);   
    }
    
}


static void s_led_only2foc_test(void)
{
    TRISAbits.TRISA4 = 0; // ==>trisa reg 0 0x0007

 #define TurnOnLedGreen()  { TRISAbits.TRISA4 = 0; PORTAbits.RA4 = 1; }
  #define TurnOnLedRed()    { TRISAbits.TRISA4 = 0; PORTAbits.RA4 = 0; }
  #define TurnOffLed()      { TRISAbits.TRISA4 = 1; }


    while(1)
    {
        TurnOnLedGreen();
        hal_arch_dspic_delay(1000000); 
        TurnOnLedRed();
        hal_arch_dspic_delay(1000000);
        TurnOffLed(); // ==>trisa reg 0 0x0017
  
    }
    

}
#endif

#ifdef _TEST_TIMER_
static void s_timer2_callback(void *arg)
{
    hal_led_toggle(hal_led0); 
    hal_timer_status_t status = hal_timer_status_get(hal_timer2);
    status = status;
}
static void s_timer4_callback(void *arg)
{
    hal_timer_status_t status = hal_timer_status_get(hal_timer4);
    status = status;
}
static void s_timer_led_test(void)
{
    int16_t i;
    hal_result_t res;
    hal_timer_status_t status;
    hal_led_cfg_t l_cfg = { .dummy = 0 };
    hal_time_t error;
    hal_timer_cfg_t t2_cfg = 
    {
        .prescaler = hal_timer_prescaler64,         
        .countdown = 100000,
        .priority = hal_int_priority05,
        .mode = hal_timer_mode_periodic,
        .callback_on_exp = s_timer2_callback,
        .arg = NULL
    };

    hal_timer_cfg_t t4_cfg = 
    {
        .prescaler = hal_timer_prescaler64,         
        .countdown = 1000,
        .priority = hal_int_priority05,
        .mode = hal_timer_mode_oneshot,
        .callback_on_exp = s_timer4_callback,
        .arg = NULL
    };


    hal_led_init(hal_led0, &l_cfg);
    
    res  = hal_timer_init(hal_timer2, &t2_cfg, &error);
    if(hal_res_OK != res)
    {
        while(1);
    }
    hal_timer_offset_write(hal_timer2, 0x1E0); //100 micro
    hal_timer_interrupt_enable(hal_timer2);

    
    status = hal_timer_status_get(hal_timer2);
    status = hal_timer_status_get(hal_timer4);

    res  = hal_timer_init(hal_timer4, &t4_cfg, &error);
    if(hal_res_OK != res)
    {
        while(1);
    }

    hal_timer_interrupt_enable(hal_timer4);

    status = hal_timer_status_get(hal_timer2);
    status = hal_timer_status_get(hal_timer4);

    hal_timer_start(hal_timer2);
    hal_timer_start(hal_timer4);
    while(1)
    {
        res = hal_timer_remainingtime_get(hal_timer2, &remaining_time);
        status = hal_timer_status_get(hal_timer2);
        status = hal_timer_status_get(hal_timer4);

        if(hal_res_OK != res)
        {
            while(1);
        }

        for(i=0; i<2000; i++)
        {
            timer_count++;
            if(timer_count > 3000)
            {
                timer_count = 0;
            }
                
        }
    }

}


static void s_timer_callbackSendCanMsg(void *arg)
{
    hal_can_frame_t *frame_ptr = (hal_can_frame_t*)arg;
    hal_timer_status_t status = hal_timer_status_get(hal_timer2);
    status = status;
    hal_can_put(hal_can_port1, frame_ptr, hal_can_send_normprio_now);
}

static void s_timer_can_test(void)
{
    int16_t i;
    hal_result_t res;
    hal_timer_status_t status;
    hal_led_cfg_t l_cfg = { .dummy = 0 };
    hal_time_t error;



    hal_can_cfg_t config = 
    {
        .runmode = hal_can_runmode_isr_1txq1rxq, 
        .baudrate = hal_can_baudrate_1mbps,
        .priorx = hal_int_priority05,
        .priotx = hal_int_priority05,
        .callback_on_rx = NULL,
        .arg = NULL    
    };
   hal_timer_cfg_t t2_cfg = 
    {
        .prescaler = hal_timer_prescaler64,         
        .countdown = 100000,
        .priority = hal_int_priority05,
        .mode = hal_timer_mode_periodic,
        .callback_on_exp = s_timer_callbackSendCanMsg,
        .arg = &frame2
    };

    hal_timer_cfg_t t4_cfg = 
    {
        .prescaler = hal_timer_prescaler64,         
        .countdown = 1000,
        .priority = hal_int_priority05,
        .mode = hal_timer_mode_periodic, //hal_timer_mode_oneshot,
        .callback_on_exp = s_timer_callbackSendCanMsg,
        .arg = &frame4
    };


    frame2.id = 0x0F;
    frame2.id_type = hal_can_frameID_std;
    frame2.frame_type = hal_can_frame_data;
    frame2.size = 8;
    memset(&(frame2.data),2, 8);

    frame4.id = 0x0F;
    frame4.id_type = hal_can_frameID_std;
    frame4.frame_type = hal_can_frame_data;
    frame4.size = 8;
    memset(&(frame4.data), 4, 8);


    hal_can_init(hal_can_port1, &config);

    hal_can_enable(hal_can_port1);


    
    res  = hal_timer_init(hal_timer2, &t2_cfg, &error);
    if(hal_res_OK != res)
    {
        while(1);
    }
    hal_timer_offset_write(hal_timer2, 0x1E0); //100 micro
    hal_timer_interrupt_enable(hal_timer2);

    
    status = hal_timer_status_get(hal_timer2);
    status = hal_timer_status_get(hal_timer4);

    res  = hal_timer_init(hal_timer4, &t4_cfg, &error);
    if(hal_res_OK != res)
    {
        while(1);
    }

    hal_timer_interrupt_enable(hal_timer4);

    status = hal_timer_status_get(hal_timer2);
    status = hal_timer_status_get(hal_timer4);

    hal_timer_start(hal_timer2);
    hal_timer_start(hal_timer4);

    while(1)
    {;}



}
#endif



#ifdef _TEST_EEPROM_
static void s_eeprom_test(void)
{

    uint32_t EE_addr_data;
    uint32_t EE_addr_sh_data;

    static my_small_data_t ee_data_ram;
    static my_small_data_t ee_sh_data_ram;

    memset(&ee_data_ram, 0x22, sizeof(my_small_data_t));
    memset(&ee_sh_data_ram, 0x44, sizeof(my_small_data_t));


    hal_arch_dspic_eeprom_emfl_GET_ADDR(EE_addr_data, ee_data_flash);
    hal_arch_dspic_eeprom_emfl_GET_ADDR(EE_addr_sh_data, ee_sh_data_flash);


    hal_eeprom_init(hal_eeprom_emulatedflash, NULL);


    hal_eeprom_read(hal_eeprom_emulatedflash, EE_addr_data, sizeof(my_small_data_t), (void*)&ee_data_ram);
    hal_eeprom_read(hal_eeprom_emulatedflash, EE_addr_sh_data, sizeof(my_small_data_t), (void*)&ee_sh_data_ram);

    hal_eeprom_erase(hal_eeprom_emulatedflash, EE_addr_data, sizeof(my_small_data_t)); 
    hal_eeprom_read(hal_eeprom_emulatedflash, EE_addr_data, sizeof(my_small_data_t), (void*)&ee_data_ram);


    memset(&ee_data_ram, 0x33, sizeof(my_small_data_t));
    hal_eeprom_write(hal_eeprom_emulatedflash, EE_addr_data, sizeof(my_small_data_t), (void*)&ee_data_ram);

    memset(&ee_data_ram, 0x0, sizeof(my_small_data_t));
    hal_eeprom_read(hal_eeprom_emulatedflash, EE_addr_data, sizeof(my_small_data_t), (void*)&ee_data_ram);
    hal_eeprom_read(hal_eeprom_emulatedflash, EE_addr_sh_data, sizeof(my_small_data_t), (void*)&ee_sh_data_ram);

    while(1)
    {;}

}

#endif


#ifdef _TEST_NEWEEPROM_
uint8_t flashpagebuffer[1024] =  {0};
uint16_t ee_data_page0[512];
uint16_t ee_data_page1[512];
uint16_t ee_data_ram0[512];
uint16_t ee_data_ram1[512];
static void s_eeprom_newtest(void)
{

    uint32_t EE_addr_data;
    uint32_t EE_addr_sh_data;
    uint32_t eepromflash_base = 0;
    uint32_t eepromflash_size = 0;
    volatile uint32_t offset = 8;
    uint16_t i = 0;
    
    hal_eeprom_cfg_t eeprom_cfg =
    {
        .flashpagebuffer = NULL,
        .flashpagesize   = 0
    };
   
     

    memset(&ee_data_page0, 0, sizeof(ee_data_page0));
    memset(&ee_data_page1, 0, sizeof(ee_data_page1));
    memset(&ee_data_ram0, 0x11, sizeof(ee_data_ram0));
    for(i=0; i<512; i++)
    {
        ee_data_ram1[i] = i;   
    }
    
    eeprom_cfg.flashpagebuffer = flashpagebuffer;
    eeprom_cfg.flashpagesize   = 1024;
     

    hal_eeprom_init(hal_eeprom_emulatedflash, &eeprom_cfg);
    
    eepromflash_base = hal_eeprom_get_baseaddress(hal_eeprom_emulatedflash);
    eepromflash_size = hal_eeprom_get_totalsize(hal_eeprom_emulatedflash);

    hal_eeprom_read(hal_eeprom_emulatedflash, eepromflash_base, 128, ee_data_page0);
    hal_eeprom_read(hal_eeprom_emulatedflash, eepromflash_base+1024, 128, ee_data_page1);

    //hal_eeprom_erase(hal_eeprom_emulatedflash, eepromflash_base+offset, 1024); 
    hal_eeprom_read(hal_eeprom_emulatedflash, eepromflash_base, 1024, ee_data_page0);
    hal_eeprom_read(hal_eeprom_emulatedflash, eepromflash_base+1024, 1024, ee_data_page1);

    hal_eeprom_write(hal_eeprom_emulatedflash, eepromflash_base+offset, 1024, ee_data_ram0);
    hal_eeprom_read(hal_eeprom_emulatedflash, eepromflash_base, 1024, ee_data_page0);
    hal_eeprom_read(hal_eeprom_emulatedflash, eepromflash_base+1024, 1024, ee_data_page1);

    hal_eeprom_write(hal_eeprom_emulatedflash, eepromflash_base+1024, 32, ee_data_ram1);
    hal_eeprom_write(hal_eeprom_emulatedflash, eepromflash_base+1024+512, 32, ee_data_ram1);
    hal_eeprom_read(hal_eeprom_emulatedflash, eepromflash_base, 1024, ee_data_page0);
    hal_eeprom_read(hal_eeprom_emulatedflash, eepromflash_base+1024, 1024, ee_data_page1);
    
    hal_eeprom_read(hal_eeprom_emulatedflash, eepromflash_base, 1024, ee_data_page0);
    hal_eeprom_read(hal_eeprom_emulatedflash, eepromflash_base+1024, 1024, ee_data_page1);
    hal_eeprom_read(hal_eeprom_emulatedflash, eepromflash_base+1024, 1024, ee_data_page1);

    offset = 0;
    offset = offset;


    while(1)
    {
        offset = offset;
        offset = 0;
    }

}
#endif


#ifdef _TEST_CAN_
static void s_can_test_without_int(void)
{
    uint8_t i;
    hal_result_t ret;    
    hal_can_cfg_t config = 
    {
        .runmode = hal_can_runmode_noisr_0q,
        .baudrate = hal_can_baudrate_1mbps,
        .priorx = hal_int_priority00,
        .priotx = hal_int_priority00,
        .callback_on_rx = NULL,
        .arg = NULL    
    };


    hal_can_init(hal_can_port1, &config);
    hal_can_enable(hal_can_port1);

    while(1)
    {
        for(i=0; i<8; i++)
        {
            ret = hal_can_put(hal_can_port1, &frames_ptr[i], hal_can_send_normprio_now);
            while(hal_res_NOK_busy == ret)
            {
                hal_arch_dspic_delay(100);
                ret = hal_can_put(hal_can_port1, &frames_ptr[i], hal_can_send_normprio_now);
            }

            hal_arch_dspic_delay(1000);
        }
    }

}


static void s_can_test_1queue(void)
{
    uint8_t i;
    hal_result_t ret, ret2;    
    hal_can_cfg_t config = 
    {
        .runmode = hal_can_runmode_isr_1txq1rxq, 
        .baudrate = hal_can_baudrate_1mbps,
        .priorx = hal_int_priority05,
        .priotx = hal_int_priority05,
        .callback_on_rx = NULL,
        .arg = NULL    
    };


    hal_can_init(hal_can_port1, &config);

    hal_can_enable(hal_can_port1);

    while(1)
    {
        for(i=0; i<8; i++)
        {
//            if(i <4)
//            {
//                ret = hal_can_put(hal_can_port1, &frames_ptr[i], hal_can_send_normprio_later);
//            }
//            else
//            {
                ret = hal_can_put(hal_can_port1, &frames_ptr[i], hal_can_send_normprio_now);
//            }
            if(ret != hal_res_OK)
            {
                if(ret == hal_res_NOK_busy)
                {
                    while(hal_res_NOK_busy == ret)
                    {
                    hal_arch_dspic_delay(100);
                    ret = hal_can_put(hal_can_port1, &frames_ptr[i], hal_can_send_normprio_now);
                    ret2++;
                    }
                }
                else
                {
                    while(1); //errore
                }
            }
        }
        hal_arch_dspic_delay(1000);
    }

}
static void s_can_call_back(void *arg)
{
    received_a_frame = 1;
}

static void s_can_test_2queue(void)
{
    hal_can_cfg_t config = 
    {
        .runmode = hal_can_runmode_isr_2txq1rxq,
        .baudrate = hal_can_baudrate_1mbps,
        .priorx = hal_int_priority05,
        .priotx = hal_int_priority05,
        .callback_on_rx = s_can_call_back,
        .arg = NULL    
    };


    hal_can_init(hal_can_port1, &config);

    hal_can_receptionfilter_set(hal_can_port1, 0, 0x00F, 0, 0x00A, hal_can_frameID_std);

    hal_can_enable(hal_can_port1);

    while(1)
    {
        hal_can_put(hal_can_port1, &frames_ptr[0], hal_can_send_normprio_later);
        hal_can_put(hal_can_port1, &frames_ptr[1], hal_can_send_normprio_now);
        
    
         hal_can_put(hal_can_port1, &frames_ptr[4], hal_can_send_highprio_now );
         hal_can_put(hal_can_port1, &frames_ptr[5], hal_can_send_highprio_now );
     
         hal_can_put(hal_can_port1, &frames_ptr[7], hal_can_send_normprio_now);

         hal_can_put(hal_can_port1, &frames_ptr[6], hal_can_send_highprio_now );
         hal_can_put(hal_can_port1, &frames_ptr[8], hal_can_send_normprio_later);
        
        __delay32(800000);
    }

}


static void s_can_test_receive(void)
{
    hal_can_cfg_t config = 
    {
        .runmode = hal_can_runmode_isr_2txq1rxq,
        .baudrate = hal_can_baudrate_1mbps,
        .priorx = hal_int_priority05,
        .priotx = hal_int_priority05,
        .callback_on_rx = s_can_call_back,
        .arg = NULL    
    };
    hal_led_cfg_t l_cfg = { .dummy = 0 };
    uint8_t howmany = 0;
    hal_result_t res;
    
    hal_led_init(hal_led0, &l_cfg);

    hal_can_init(hal_can_port1, &config);

    //hal_can_receptionfilter_set(hal_can_port1, 0, 0x000, 0, 0x000, hal_can_frameID_std);

    hal_can_enable(hal_can_port1);

    while(1)
    {
        hal_led_toggle(hal_led0);
        hal_arch_dspic_delay(1000000); 
        res = hal_can_received(hal_can_port1, &howmany);        
        if(1 == received_a_frame)
        {
            s_can_test_process_a_received_frame();
        }

    }
    #warning --> problemi se il buffer di ricezione si riempie. 

}

static void s_can_test_process_a_received_frame(void)
{
    
    hal_can_frame_t rec_frame;
    hal_result_t res = hal_res_NOK_generic;
    hal_result_t txres = hal_res_NOK_generic;
    uint8_t remaining;
    uint8_t txattempts = 0;

//    hal_can_get(hal_can_port1, &rec_frame, &remaining);
//    rec_frame.data[0] = 0xCC;
//    hal_can_put(hal_can_port1, &rec_frame, hal_can_send_normprio_now);

    do
    {
        res = hal_can_get(hal_can_port1, &rec_frame, &remaining);
        rec_frame.data[0] = 0xA0;
        txattempts = 0;
        do
        {
            txattempts++;
            txres = hal_can_put(hal_can_port1, &rec_frame, hal_can_send_normprio_now);
            rec_frame.data[0]++;
        } 
        while((txattempts < 10) && (hal_res_OK != txres));
    } 
    while((hal_res_OK == res) && (0 != remaining));

    received_a_frame = 0;
}

static void s_can_test_receive_witoutISR(void)
{
    hal_result_t res; 
    hal_can_frame_t rec_frame;
    uint8_t remaining;
    uint32_t adr = 0xD;

    hal_can_cfg_t config = 
    {
        .runmode = hal_can_runmode_noisr_0q,
        .baudrate = hal_can_baudrate_1mbps,
        .priorx = hal_int_priorityNONE,
        .priotx = hal_int_priorityNONE,
        .callback_on_rx = NULL,
        .arg = NULL    
    };
    hal_led_cfg_t l_cfg = { .dummy = 0 };
    
    hal_led_init(hal_led0, &l_cfg);

    hal_can_init(hal_can_port1, &config);

    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 0, (0x700|adr), hal_can_frameID_std);
   
    hal_can_receptionfilter_set(hal_can_port1, 0, 0x70F, 1, (0x700|0xF), hal_can_frameID_std);

    hal_can_enable(hal_can_port1);

    while(1)
    {
        hal_led_toggle(hal_led0);
        hal_arch_dspic_delay(30000);
        res = hal_can_get(hal_can_port1, &rec_frame, &remaining);
        if(hal_res_OK == res)
        {
            rec_frame.data[0] = 0xCC;
            hal_can_put(hal_can_port1, &rec_frame, hal_can_send_normprio_now);
        }
        

    }

}


#endif












#if 0

static void s_incr_b(void)
{
    b++;
}


static void s_t2_callback(void)
{
    //hal_led_toogle(led_red);
    c_data.info_ptr->func();
}


static void s_timer_test(void)
{
    
    hal_timer_config_t timer_config_data;
    uint8_t i, ret;    
    hal_can_config_info_t config;

    config.runMode = hal_can_runningMode_without_int;
    config.baudrate = hal_can_baudrate_1M;
    config.prio_rx = hal_can_IntPrio5;
    config.prio_tx = hal_can_IntPrio5;

    hal_can_init(&config);
    hal_can_set_rxFilter_mask( 0, 0x70F, 0);
    hal_can_enable();

    timer_config_data.gate_op = 0;
    timer_config_data.idle = 0;
    timer_config_data.source = TIMER_SOURCE_INTERNAL;
    timer_config_data.prescaler = hal_timer_prescaler64;
    timer_config_data.call_bkp_fn = s_timer_callback; 

    //timer 1 configuration: it establishes when sampling and parse received can cmd
    timer_config_data.match_value = 312;
    hal_timer_config(hal_timerT3, &timer_config_data);
    hal_timer_start(hal_timerT3);
    hal_timer_interrupt_config(hal_timerT3, hal_timer_priority4);
    hal_timer_interrupt_ena(hal_timerT3);

    while(1)
    {
       if( count == 100000)
        {
            
            hal_timer_interrupt_disa(hal_timerT3);
            hal_timer_stop(hal_timerT3);
            count = 0;
            hal_timer_matchValue_set(hal_timerT3, hal_timer_prescaler64, 1560);
            hal_timer_start(hal_timerT3);
            hal_timer_interrupt_ena(hal_timerT3);
            
        }
        
    }

}
static void s_timer_lib(void)
{

    uint8_t i, ret;    
    hal_can_config_info_t config;
    uint32_t T1Value = 0;

    config.runMode = hal_can_runningMode_without_int;
    config.baudrate = hal_can_baudrate_1M;
    config.prio_rx = hal_can_IntPrio5;
    config.prio_tx = hal_can_IntPrio5;

    hal_can_init(&config);
    hal_can_set_rxFilter_mask( 0, 0x70F, 0);
    hal_can_enable();

    
    OpenTimer1(T1_ON & T1_GATE_OFF & T1_IDLE_STOP & T1_PS_1_256 & 
    T1_SYNC_EXT_OFF & T1_SOURCE_INT, 31250);
    
    //Disables interrupt timer
    ConfigIntTimer1(T1_INT_OFF);
    
    
    while(1)
    {
       T1Value = ReadTimer1();
        if(T1Value >= 31250) // timeout
        {
            WriteTimer1(0); 
            hal_can_put(&frames_ptr[2], hal_send_now);
        } 
    }

}
static void s_timer_test_bootLoader(void)
{

    hal_timer_config_t timer_config_data;
    uint8_t i, ret;    
    hal_can_config_info_t config;
    uint32_t T1Value = 0;

    config.runMode = hal_can_runningMode_without_int;
    config.baudrate = hal_can_baudrate_1M;
    config.prio_rx = hal_can_IntPrio5;
    config.prio_tx = hal_can_IntPrio5;


    hal_can_init(&config);
    hal_can_set_rxFilter_mask( 0, 0x70F, 0);
    hal_can_enable();

    timer_config_data.gate_op = 0;
    timer_config_data.idle = 0;
    timer_config_data.use32bit = 0;
    timer_config_data.source = TIMER_SOURCE_INTERNAL;
    timer_config_data.prescaler = hal_timer_prescaler256;
    timer_config_data.call_bkp_fn = NULL;
    timer_config_data.match_value = 31250; // 6400;//15625;//31250; 

    hal_timer_config(hal_timerT1, &timer_config_data);
    hal_timer_write(hal_timerT1, 0);
    hal_timer_interrupt_disa(hal_timerT1);
    hal_timer_start(hal_timerT1); 

//OpenTimer1(T1_ON & T1_GATE_OFF & T1_IDLE_STOP & T1_PS_1_256 & 
//    T1_SYNC_EXT_OFF & T1_SOURCE_INT, 31250);
//ConfigIntTimer1(T1_INT_OFF);



    while(1)
     {
        T1Value = hal_timer_read(hal_timerT1);
        if(T1Value >= timer_config_data.match_value) // timeout
        {
            hal_timer_stop(hal_timerT1);
            hal_timer_write(hal_timerT1, 0);  
            hal_can_put(&frames_ptr[2], hal_send_now);
            hal_timer_start(hal_timerT1); 
        }

    }
}

void s_timer_callback(void)
{
    hal_result_t ret;

    ret = hal_can_put(&frames_ptr[2], hal_send_now);
    if(ret != hal_res_OK)
    {
        while((ret == hal_res_NOK_busy) )
        {
            __delay32(800);
            ret = hal_can_put(&frames_ptr[2], hal_send_now);
        }
    }
    count++;
  

  IFS1bits.T5IF = 0; // clear flag  
}





static void s_can_test_receiving(void)
{
    static hal_can_frame_t rec_frame;
    hal_can_config_info_t config;

    config.runMode = hal_can_runningMode_without_int;
    config.baudrate = hal_can_baudrate_1M;
    config.prio_rx = hal_can_IntPrio5;
    config.prio_tx = hal_can_IntPrio5;
    config.callbkp_on_rec = my_call_bkp;

    hal_can_init(&config);
    hal_can_set_rxFilter_mask( 0, 0x70F, 0);
    hal_can_set_rxFilter( 0, 0x70F, 0, 0);
    hal_can_set_rxFilter( 1, 0x71F, 0, 0);
    hal_can_enable();

    while(1)
    {
        if(my_flag)
        {
            hal_can_get(&rec_frame);
            my_flag = 0;
        }

    }
}
static void my_call_bkp(void)
{
    my_flag = 1;
}
static void s_can_test_receiving_and_send(void)
{
    static hal_can_frame_t rec_frame;
    hal_can_config_info_t config;
    hal_result_t ret;

    config.runMode = hal_can_runningMode_int_1queue;
    config.baudrate = hal_can_baudrate_1M;
    config.prio_rx = hal_can_IntPrio5;
    config.prio_tx = hal_can_IntPrio5;
    config.callbkp_on_rec = my_call_bkp;


    hal_can_init(&config);
    hal_can_set_rxFilter_mask( 0, 0x70F, 0);
    hal_can_set_rxFilter( 0, 0x70F, 0, 0);
    hal_can_set_rxFilter( 1, 0x71F, 0, 0);
    hal_can_enable();

    while(1)
    {
        if(my_flag)
        {
            my_flag = 0;
            ret = hal_can_get(&rec_frame);
            if(ret != hal_res_OK)
            {
                continue;
            }
            ret = hal_can_put_inPrioqueue(&rec_frame);
            if(ret != hal_res_OK)
            {
                while(ret == hal_res_NOK_busy)
                {
                    __delay32(800);
                    ret = hal_can_put_inPrioqueue(&rec_frame);
                }
            }
        }

    }
}



static void s_can_test_receiving_and_send_withPrio(void)
{
    static hal_can_frame_t rec_frame;
    hal_can_config_info_t config;
    hal_result_t ret;

    config.runMode = hal_can_runningMode_int_2queue;
    config.baudrate = hal_can_baudrate_1M;
    config.prio_rx = hal_can_IntPrio5;
    config.prio_tx = hal_can_IntPrio5;
    config.callbkp_on_rec = my_call_bkp;

    hal_can_init(&config);

    hal_can_set_rxFilter_mask( 0, 0x70F, 0);
    hal_can_set_rxFilter( 0, 0x70F, 0, 0);
    hal_can_set_rxFilter( 1, 0x71F, 0, 0);
    hal_can_enable();

    while(1)
    {
        if(my_flag)
        {
            my_flag = 0;
            ret = hal_can_get(&rec_frame);
            if(ret != hal_res_OK)
            {
                continue;
            }
            hal_can_put(&frames_ptr[0], hal_send_now);
            hal_can_put(&frames_ptr[1], hal_send_now);
            ret = hal_can_put_inPrioqueue(&rec_frame);
           
            if(ret != hal_res_OK)
            {
                while(ret == hal_res_NOK_busy)
                {
                    __delay32(800);
                    ret = hal_can_put_inPrioqueue(&rec_frame);
                }
            }
        }

    }
}


static void s_can_testReceive_without_int(void)
{
    uint8_t i, ret;    
    hal_can_config_info_t config;
    static hal_can_frame_t rec_frame;

    config.runMode = hal_can_runningMode_without_int;
    config.baudrate = hal_can_baudrate_1M;
    config.prio_rx = hal_can_IntPrio5;
    config.prio_tx = hal_can_IntPrio5;
    config.callbkp_on_rec = NULL;

    

    hal_can_init(&config);
    hal_can_set_rxFilter_mask( 0, 0x70F, 0);
    hal_can_set_rxFilter( 0, 0x70F, 0, 0);
    hal_can_set_rxFilter( 1, 0x71F, 0, 0);
    hal_can_enable();

    while(1)
    {
        ret = hal_can_get(&rec_frame);
        if(ret != hal_res_OK)
        {
            continue;
            __delay32(800);
        }
        else
        {
            //hal_can_put(&frames_ptr[0], hal_send_now);
            ret = hal_can_put(&rec_frame, hal_send_now);
            ret = hal_can_put(&frames_ptr[2], hal_send_now);
            if(ret != hal_res_OK)
            {
                while(ret == hal_res_NOK_busy) 
                {
                    __delay32(800);
                    ret = hal_can_put(&frames_ptr[2], hal_send_now);
                }
            }
            
        }
    }

}


static void s_can_test_1queue_with_ee(void)
{
    uint8_t i, ret, j;    
    hal_can_config_info_t config;
    EE_address_t EE_addr_2;
    static my_small_data_t small_data_ram2;
    static my_small_data_t small_data_ram2_bis;

    config.runMode = hal_can_runningMode_int_1queue;
    config.baudrate = hal_can_baudrate_1M;
    config.prio_rx = hal_can_IntPrio5;
    config.prio_tx = hal_can_IntPrio5;

    hal_can_init(&config);
    hal_can_set_rxFilter_mask( 0, 0x70F, 0);
    hal_can_set_rxFilter( 0, 0x70F, 0, 0);
    hal_can_set_rxFilter( 1, 0x71F, 0, 0);;
    hal_can_enable();

    frames_ptr[0].data[0] = 'S';
    frames_ptr[0].data[0] = 'T';
    frames_ptr[0].data[0] = 'A';
    frames_ptr[0].data[0] = 'R';
    frames_ptr[0].data[0] = 'T';
    hal_can_put_inPrioqueue(&frames_ptr[0]);

    memset(&small_data_ram2, 0xAA, sizeof(my_small_data_t));
    HAL_EE_GET_ADDR(EE_addr_2, small_data_flash2);
    hal_eeprom_read(EE_addr_2, sizeof(my_small_data_t), (void*)&small_data_ram2);
    small_data_ram2.vector[1]++;

    memset(&small_data_ram2_bis, 0x11, sizeof(my_small_data_t));
    hal_eeprom_write(EE_addr_2, sizeof(my_ee_data_t), (void*)&small_data_ram2);
    hal_eeprom_read(EE_addr_2, sizeof(my_small_data_t), (void*)&small_data_ram2_bis);

    for(i=0; i<8; i++)
    {
        for(j=0; j<8; j++)
        {
            frames_ptr[i].data[j] = small_data_ram2_bis.vector[j];
            frames_ptr[i].data[j] = small_data_ram2_bis.vector[j];
        }
    }

    while(1)
    {
        for(i=0; i<8; i++)
        {
            
            ret = hal_can_put_inPrioqueue(&frames_ptr[i]);
            if(ret != hal_res_OK)
            {
                if(ret == hal_res_NOK_busy)
                {
                    while(hal_can_put_inPrioqueue(&frames_ptr[i])== hal_res_NOK_busy)
                    {__delay32(8000);}
                }
                else
                {
                    while(1); //errore
                }
            }
        }
        __delay32(8000000);
    }

}
static void s_can_test_2queue_onlyLowUSe(void)
{
    uint8_t i, ret;    
    hal_can_config_info_t config;

    config.runMode = hal_can_runningMode_int_2queue;
    config.baudrate = hal_can_baudrate_1M;
    config.prio_rx = hal_can_IntPrio5;
    config.prio_tx = hal_can_IntPrio5;

    hal_can_init(&config);
    hal_can_set_rxFilter_mask( 0, 0x70F, 0);
    hal_can_set_rxFilter( 0, 0x70F, 0, 0);
    hal_can_set_rxFilter( 1, 0x71F, 0, 0);
    hal_can_enable();


    while(1)
    {
        for(i=0; i<9; i++)
        {
            ret = hal_can_put(&frames_ptr[i], hal_send_after);
            if(ret != hal_res_OK)
            {
                if(ret == hal_res_NOK_busy)
                {
                    while(hal_can_put(&frames_ptr[i], hal_send_now)== hal_res_NOK_busy);
                }
                else
                {
                    while(1); //errore
                }
            }
        }
        ret = hal_can_put(&frames_ptr[9], hal_send_now);
        __delay32(800000);
    }
}

#endif




static void s_systick(void)
{
    static uint8_t initted = 0;
    static uint32_t cnt = 0;

    if(0 == initted)
    {
        hal_led_init(hal_led0, NULL);
        hal_led_off(hal_led0);  
        initted = 1; 
    }

    cnt++;

    if(0 == (cnt % 1000))
    {
        hal_led_toggle(hal_led0);
    }
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




