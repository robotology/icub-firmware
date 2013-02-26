/*
 * Copyright (C) 2012 iCub Facility - Istituto Italiano di Tecnologia
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


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "string.h"  
#include "stdlib.h"
 
#include "hal.h"  
#include "sys/abs/api/ipal.h"  

#include "hal_brdcfg_modules.h"

#include "hal_switch.h"
#include "hal_led.h"
#include "hal_spi.h"

#include "hal_crc.h"

#include "hal_termometer.h"
#include "hal_gyroscope.h"
#include "hal_accelerometer.h"

#include "hal_utility_fifo.h"
#include "hal_utility_crc07.h"

#include "hal_brdcfg.h"


#include "eventviewer.h"

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

//#define USE_EVENTVIEWER
#undef USE_EVENTVIEWER

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


extern void pippo(void)
{
   hal_gpio_init(hal_gpio_portA, hal_gpio_pin1, hal_gpio_dirNONE, hal_gpio_speed_default);
}


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------

typedef struct
{
    uint32_t aaa;
    uint16_t bbb;
    uint8_t  ccc;
    uint8_t  ddd;
} mydata;

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

void idle(void);
void userdef1(void);
void userdef2(void);
void userdef3(void);
void userdef4(void);
static void s_eventviewer_init(void);

static void s_test_fifo(void);
static void s_test_fifoold(void);
static void s_test_fifonew(void);
static void s_test_fifoultra(void);
//static void s_test_fifomega(void);

static void myledsinit(void);
static void myled00toggle(void);
static void myled01toggle(void* p);
static void myonsystick(void);

static void button_init(void);
static uint8_t button_ispushed(void);


static void timer_poll(void) ;
static void s_udp_init(void);
static void s_udp_transmit(void);
static void s_udp_onreception(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t por);

static void test_eeprom(void);

static void test_can(void);

static void s_test_mco2(void);

static void s_myfuntxspima(void* p);
static void s_myfunrxspima(void* p);
static void s_myfuntxspisl(void* p);
static void s_myfunrxspisl(void* p);

#define TEST_SPI

#if     defined(HAL_BOARD_MCBSTM32C) || defined(HAL_BOARD_STM3210CEVAL)
#if defined(TEST_SPI)
static void s_test_spi_master_slave(void);
#endif
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static volatile uint32_t msTicks = 0;
static volatile uint8_t s_tick = 0;
static volatile uint8_t s_blink = 0;

static ipal_udpsocket_t* s_udpskt = NULL;

static mydata mmm = {1, 2, 3, 4};

static uint8_t send_msg = 0;

static const hal_gpio_cfg_t user_button = 
{
#if     defined(HAL_BOARD_MCBSTM32F400)    
    .port   = hal_gpio_portG,
    .pin    = hal_gpio_pin15,
    .speed  = hal_gpio_speed_low,
    .dir    = hal_gpio_dirINP
#elif   defined(HAL_BOARD_MCBSTM32C)
    .port   = hal_gpio_portB,
    .pin    = hal_gpio_pin7,
    .speed  = hal_gpio_speed_low,
    .dir    = hal_gpio_dirINP
#else
    .dir    = hal_gpio_dirNONE 
#endif    
};

static const hal_gpio_val_t user_notpushed_value = 
#if     defined(HAL_BOARD_MCBSTM32F400)    
    hal_gpio_valHIGH;
#elif   defined(HAL_BOARD_MCBSTM32C)
    hal_gpio_valHIGH; 
#else
    hal_gpio_valNONE; 
#endif    


// --------------------------------------------------------------------------------------------------------------------
// - declaration of externally defined functions or variable which dont have a .h file
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

int main(void) 
{
    extern const hal_cfg_t*     hal_cfgMINE;
    extern const ipal_cfg_t*    ipal_cfgMINE;
    uint32_t size04aligned;
    uint32_t *data32aligned = NULL;
    hal_result_t res = hal_res_OK;
    
    

    hal_base_memory_getsize(hal_cfgMINE, &size04aligned); 

    if(0 != size04aligned)
    {
        data32aligned = (uint32_t*)calloc(size04aligned/4, sizeof(uint32_t));   
    }

    hal_base_initialise(hal_cfgMINE, data32aligned);
    

    hal_sys_systeminit();
    
    
//     hal_crc_cfg_t crccfg = 
//     {
//         .order          = hal_crc_order_07,
//         .polynomial     = hal_crc_poly_crc07,
//         .crctblram      = NULL
//     };
//     uint8_t dat[] = {0xFE, 0xCA, 0x00, 0xE1}; //{0xE1,0x00, 0xCA, 0xFe};
//     uint32_t out = 0;
//     hal_crc_init(hal_crc0, &crccfg);
//     
//     hal_crc_compute(hal_crc0, hal_crc_mode_clear, dat, sizeof(dat), &out);   

//     out = out;    
    
//     static uint8_t crc07table[256];
//     hal_utility_crc07_table_get(0x09, crc07table);
//     
//     crc07table[0] = crc07table[0];
    
#if     defined(HAL_BOARD_MCBSTM32C) || defined(HAL_BOARD_STM3210CEVAL)
    #if defined(TEST_SPI)
     s_test_spi_master_slave();
    #endif
#endif
    
#ifdef HAL_USE_DEVICE_DISPLAY    
    
    hal_display_init(hal_display_dev1, NULL);
    
    hal_display_clearscreen(hal_display_dev1);
    
    //hal_display_settextproperties(hal_display_dev1, hal_display_font_24x16, hal_display_color_white, hal_display_color_black);
    
    hal_display_putstring(hal_display_dev1, 1, 4, "hello world");
    hal_display_putstring(hal_display_dev1, 2, 20, "my old friend");
    hal_display_putstring(hal_display_dev1, 3, 30, "line 3 w/ fonts 6x8");
    hal_display_putstring(hal_display_dev1, 4, 30, "line 4");
    hal_display_putstring(hal_display_dev1, 5, 30, "line 5");
    hal_display_putstring(hal_display_dev1, 6, 30, "line 6");    
    hal_display_putstring(hal_display_dev1, 7, 30, "line 7");
    hal_display_putstring(hal_display_dev1, 8, 30, "line 8");
    hal_display_putstring(hal_display_dev1, 9, 30, "line 9");
    hal_display_putstring(hal_display_dev1, 10, 30, "line 10"); 
    
    hal_display_font_t fnt = 
    {
        .textsize   = hal_display_font_size_16x24,
        .textcolor  = hal_display_color_blue,
        .backcolor  = hal_display_color_white        
    };
    hal_display_setfont(hal_display_dev1, fnt);
    
    hal_display_putstring(hal_display_dev1, 5, 0, "line w/ fonts 16x24"); 
#endif//HAL_USE_DEVICE_DISPLAY

s_eventviewer_init();
    
//    s_test_fifo();
    
//    s_test_fifo();
    
    
    
    
    myledsinit();
    
    button_init();
    
     // 1 millisec.
    res = hal_sys_systick_sethandler(myonsystick, 1000, hal_int_priority00);
    res =  res;
    
//    for(;;);
    
//     for(;;)
//     {
//                 myled01toggle(NULL);
//                 hal_sys_delay(500*1000);
//         
//     }
    
#if 0    
    s_test_mco2();
    for(;;);
#endif
    
#if 0    
//#ifdef  HAL_USE_SPI  
    hal_spi_cfg_t cfgg;
    memcpy(&cfgg, &hal_spi_cfg_default, sizeof(hal_spi_cfg_t));
    cfgg.onframetransm  = s_myfuntxspima;
    cfgg.onframereceiv  = s_myfunrxspima;
    cfgg.ownership      = hal_spi_ownership_master;
    //cfgg.activity = hal_spi_act_continuous;
    hal_spi_init(hal_spi_port1, &cfgg);

    cfgg.onframetransm  = s_myfuntxspisl;
    cfgg.onframereceiv  = s_myfunrxspisl;
    cfgg.ownership      = hal_spi_ownership_slave;
    hal_spi_init(hal_spi_port2, &cfgg);
    
    hal_test_spima_spisl();
    
//     uint8_t mydata[4] = {1, 2, 3, 4};
//     uint8_t ymdata[4] = {5, 6, 7, 8};
//     hal_spi_put(hal_spi_port2, mydata, 4, hal_false);
//     hal_spi_put(hal_spi_port2, ymdata, 4, hal_true);
//     
    hal_sys_delay(10000);
    
//    hal_spi_stop(hal_spi_port2);
    
    hal_sys_delay(10000);
    
    for(;;);
#endif   
    
    
#ifdef  HAL_USE_DEVICE_SWITCH   
    extern const hal_device_switch_hid_brdcfg_t hal_brdcfg_device_switch__theconfig;
    hal_eth_phymode_t phymode = hal_eth_phymode_none; 
    hal_switch_init(NULL);
    hal_switch_configure(hal_brdcfg_device_switch__theconfig.devcfg.targetphymode, &phymode);
//    for(;;);
#endif    
 

    test_eeprom();   

//    test_can();
#ifdef  HAL_USE_DEVICE_ACCELEROMETER
    hal_accelerometer_init(hal_accelerometer_port1, NULL);
#endif//HAL_USE_DEVICE_ACCELEROMETER
#ifdef  HAL_USE_DEVICE_GYROSCOPE
    hal_gyroscope_init(hal_gyroscope_port1, NULL);
#endif//HAL_USE_DEVICE_GYROSCOPE
#ifdef  HAL_USE_DEVICE_TERMOMETER
    hal_termometer_init(hal_termometer_port1, NULL);
#endif//HAL_USE_DEVICE_TERMOMETER
    
 
#ifdef HAL_USE_ETH

    ipal_base_memory_getsize(ipal_cfgMINE, &size04aligned);
    if(0 != size04aligned)
    {
        data32aligned = (uint32_t*)calloc(size04aligned/4, sizeof(uint32_t));   
    }
    


    ipal_base_initialise(ipal_cfgMINE, data32aligned);
    ipal_sys_start();  


    s_udp_init();
#endif//HAL_USE_ETH 
    
    uint8_t read = 255;
    
    for(;;)
    {
        if(0 == read--)
        {
        #ifdef  HAL_USE_DEVICE_TERMOMETER    
            hal_termometer_degrees_t degrees;
            hal_termometer_read(hal_termometer_port1, &degrees);
            degrees = degrees;    
        #endif//HAL_USE_DEVICE_TERMOMETER
        #ifdef  HAL_USE_DEVICE_GYROSCOPE            
            hal_gyroscope_angularrate_t angrate;
            hal_gyroscope_read(hal_gyroscope_port1, &angrate);
            angrate.xar = angrate.xar;      
        #endif//HAL_USE_DEVICE_GYROSCOPE
        #ifdef  HAL_USE_DEVICE_ACCELEROMETER    
            hal_accelerometer_acceleration_t accel;
            hal_accelerometer_read(hal_accelerometer_port1, &accel);
            accel.xac = accel.xac;
        #endif//HAL_USE_DEVICE_ACCELEROMETER    
        }
        
        if(1 == button_ispushed())
        {
            #ifdef  HAL_USE_DEVICE_TERMOMETER
            hal_termometer_degrees_t degrees;
            send_msg = 1;
            hal_termometer_read(hal_termometer_port1, &degrees);
            degrees = degrees;
            #endif//HAL_USE_DEVICE_TERMOMETER
        }
        
#ifdef HAL_USE_ETH
        timer_poll();
        ipal_sys_process_communication();
       
        if(1 == s_blink)
        {        
            myled01toggle(NULL);
            s_blink = 0;
        }
        if(1 == send_msg)
        {
            send_msg = 0;
            s_udp_transmit();
        }    
#endif     
    }
 
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

    



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 

void idle(void){}
void userdef1(void){}
void userdef2(void){}
void userdef3(void){}
void userdef4(void){}
    
static void s_eventviewer_init(void)
{
#ifdef  USE_EVENTVIEWER
    
    evEntityId_t prev;

    eventviewer_init();
    eventviewer_load(ev_ID_idle, idle);  
    eventviewer_load(ev_ID_systick, myonsystick);  
    eventviewer_load(ev_ID_first_usrdef+1, userdef1); 
    eventviewer_load(ev_ID_first_usrdef+2, userdef2);
    eventviewer_load(ev_ID_first_usrdef+3, userdef3);
    eventviewer_load(ev_ID_first_usrdef+4, userdef4);
    
    // the eventviewer shall stay most of time in idle
    // apart from some specific actions: systick, userdef1 and userdef2
    eventviewer_switch_to(ev_ID_idle);
#endif
}

static void myledsinit(void)
{
    evEntityId_t prev;
    hal_result_t res;
    
    res = hal_led_init(hal_led0, NULL);
    res =  res;
    
    res = hal_led_init(hal_led1, NULL);
    res =  res;
    

#ifdef  USE_EVENTVIEWER
    prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
#endif    
    hal_gpio_quickest_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valLOW);
    hal_gpio_quickest_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valHIGH);
    hal_gpio_quickest_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valLOW);
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev); 
#endif

#ifdef  USE_EVENTVIEWER    
    prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
#endif    
    hal_gpio_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valLOW);
    hal_gpio_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valHIGH);
    hal_gpio_setval(hal_brdcfg_device_led__theconfig.gpiocfg[0].port, hal_brdcfg_device_led__theconfig.gpiocfg[0].pin, hal_gpio_valLOW);
#ifdef  USE_EVENTVIEWER
    eventviewer_switch_to(prev);  
#endif    
}

static void myled00toggle(void)
{
    hal_result_t res;
    
    res = hal_led_toggle(hal_led0);
    res =  res;
}

static void myled01toggle(void* p)
{
    hal_result_t res;
    
    res = hal_led_toggle(hal_led1);
    res =  res;
}

static void myonsystick(void)
{
#ifdef  USE_EVENTVIEWER    
    evEntityId_t prev = eventviewer_switch_to(ev_ID_systick);
#endif
    
    static uint32_t count = 0;
    static const uint32_t max = 500;
    msTicks++;
    
    count++;
    if(max == count)
    {
        count = 0;
        myled00toggle();
    }
    
    if(0 == (msTicks%100))
    {
        s_tick = 1;
    }
    
    if(10000 == msTicks)
    {
//        hal_sys_systemreset();       
    }

#ifdef  USE_EVENTVIEWER    
    eventviewer_switch_to(prev);    
#endif    
}


static void button_init(void)
{
    if(hal_gpio_dirINP == user_button.dir)
    {
        hal_gpio_configure(user_button, NULL);
        //hal_gpio_init(user_button.port, user_button.pin, user_button.dir, user_button.speed);
    }
}

static uint8_t button_ispushed(void)
{
    hal_gpio_val_t v = user_notpushed_value;
    
    if(hal_gpio_dirINP == user_button.dir)
    {
        v = hal_gpio_getval(user_button.port, user_button.pin);
        return((user_notpushed_value == v) ? 0 : 1);        
    }
    else
    {
        return(0);
    }
}



static void s_udp_init(void)
{
    ipal_result_t res;
    
    ipal_tos_t tos = 
    {
        .precedence         = ipal_prec_immediate,
        .lowdelay           = 1,
        .highthroughput     = 1,
        .highreliability    = 1,
        .unused             = 0
    };

    s_udpskt = ipal_udpsocket_new(tos);
    res = ipal_udpsocket_bind(s_udpskt, IPAL_ipv4addr_INADDR_ANY, 1001);
    res = res;

    res = ipal_udpsocket_recv(s_udpskt, s_udp_onreception, &mmm);

    res = res;

}


static void timer_poll(void) 
{
    static uint32_t tt = 0;

    if(1 == s_tick) 
    {
        s_tick = 0;

        ipal_sys_timetick_increment();
        if(++tt == 5)
        {
            s_blink = 1;
            tt = 0;
        }
    }
}


static void s_udp_transmit(void)
{
    static ipal_result_t res;
    static uint8_t data[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    static uint8_t arped = 0;
    static uint8_t data64[64] = {0};
    static ipal_packet_t pkt;
    static uint8_t num = 0;

    pkt.data = data64;
    pkt.size = sizeof(data64);

    static ipal_ipv4addr_t ipv4addr = IPAL_ipv4addr(10, 255, 72, 101);  // put in here address of a known host, like your pc.



    if(0 == arped)
    {
        hal_trace_puts("arp request to pc 10.255.72.101");
        if(ipal_res_OK == ipal_arp_request(ipv4addr, ipal_arp_cache_permanently))
        {
            arped = 1;
            hal_trace_puts("pc 10.255.72.101 arped");
        }        
    }
    
    if(1 == arped)
    {
        num++;
        hal_trace_puts("transmitted packet to pc 10.255.72.101:1001");
        memset(data64, num, 64);
        res = ipal_udpsocket_sendto(s_udpskt, &pkt, ipv4addr, 1001);
        res = res;
    }

}

static void s_udp_onreception(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t por)
{
    static mydata *m = NULL;
    
    m = (mydata*)arg;

    send_msg = 1;

    hal_trace_puts("received a packet");

    skt = skt;
    pkt = pkt;
    adr = adr;
    por = por;

}

static void test_can(void)
{
#ifdef HAL_USE_CAN    
    hal_can_init(hal_can_port1, NULL);
    hal_can_init(hal_can_port2, NULL);
#endif    
}


static void test_eeprom(void)
{
#ifdef HAL_USE_EEPROM
    static uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    
    static uint8_t tmp[512] = {0};
    
    hal_result_t res;
    
    //hal_i2c4hal_init(hal_i2c_port1, NULL);

    res = hal_eeprom_init(hal_eeprom_i2c_01, NULL);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, 258, 12, tmp);
    res =  res;  
    
//    return;   
    
    res = hal_eeprom_erase(hal_eeprom_i2c_01, 0, 512);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, 258, 12, tmp);
    res =  res;    
    
    res = hal_eeprom_write(hal_eeprom_i2c_01, 256, 12, data);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, 256, 16, tmp);
    res =  res;

#endif
}

#include "hal_mpu_stm32xx_include.h"
#ifdef USE_STM32F4
static void hal_brdcfg_switch__mco2_init(void)
{
    // configure pc9 af, push pull, 50mhz
    // enable system configuration controller clock
    RCC->APB2ENR    |= (1 << 14);  
    // clocks port port c
    RCC->AHB1ENR    |= 0x00000004;

    GPIOC->MODER   &= ~0x000C0000;              // reset pc9
    GPIOC->MODER   |=  0x00080000;              // alternate function
    GPIOC->OTYPER  &= ~0x00000200;              // output push-pull (reset state)  
    GPIOC->OSPEEDR |=  0x000C0000;              // slew rate as 100MHz pin (C) or 50mhz (8)
    GPIOC->PUPDR   &= ~0x000C0000;              // no pull up, pull down

    GPIOC->AFR[1]  &= ~0x000000F0;
    GPIOC->AFR[1]  |=  0x00000000;              // AF0 (system) 

}

static void s_test_mco2(void)
{   
    hal_brdcfg_switch__mco2_init();
     
    RCC_PLLI2SCmd(DISABLE);
    RCC_PLLI2SConfig(200, 2); // 50mhz: 1mhz*200/10 = 100
    RCC_PLLI2SCmd(ENABLE);
    // wait until it is ready
    while(RCC_GetFlagStatus(RCC_FLAG_PLLI2SRDY) == RESET);
    // connect mco2 with plli2s divided by 2
    RCC_MCO2Config(RCC_MCO2Source_PLLI2SCLK, RCC_MCO2Div_2);
    
}

static void s_myfuntxspima(void* p)
{
    static uint8_t mio = 0;
    mio++;
}
static void s_myfunrxspima(void* p)
{
    static uint8_t mio = 0;
    mio++;
}

static void s_myfuntxspisl(void* p)
{
    static uint8_t mio = 0;
    mio++;
}

static void s_myfunrxspisl(void* p)
{
    static uint8_t mio = 0;
    mio++;
}




#endif

static void s_test_fifo(void)
{
    evEntityId_t prev;
//     prev = eventviewer_switch_to(ev_ID_first_usrdef+1);
//     s_test_fifoold();
//     eventviewer_switch_to(prev);
    prev = eventviewer_switch_to(ev_ID_first_usrdef+2);
    s_test_fifonew();
    eventviewer_switch_to(prev);
    prev = eventviewer_switch_to(ev_ID_first_usrdef+3);
    s_test_fifoultra();
    eventviewer_switch_to(prev);
//     prev = eventviewer_switch_to(ev_ID_first_usrdef+4);
//     s_test_fifomega();
//     eventviewer_switch_to(prev);    
}

// static void s_test_fifomega(void)
// {
// #if 1    
//     static hal_utility_fifo_t fifo16;
//     static uint8_t data12x16[12*16];
//     uint8_t *zz;
//     uint8_t size = 0;
//     hal_can_frame_t xx;
//     
//     hal_can_frame_t canframe = 
//     {
//         .id = 0, .id_type = hal_can_frameID_std, 
//         .frame_type = hal_can_frame_data, .size = 0, .unused = 0xF1, .data = {0}
//     };

//     
//     static hal_can_frame_t udatacanframes[12];
//     
//     hal_utility_fifo_init(&fifo16, 12, 16, (uint8_t*)&udatacanframes);
//     
//     uint32_t i;
//     for(i=0; i<100; i++)
//     {
//     
//         canframe.id = 0x12345678;
//         hal_utility_fifo_putxx(&fifo16, (uint8_t*)&canframe);
//         canframe.id = 0xaabbccdd;
//         canframe.data[0] = 0x11; canframe.data[7] = 0x77;
//         hal_utility_fifo_putxx(&fifo16, (uint8_t*)&canframe);
//         canframe.id = 0xdeaddead;
//         hal_utility_fifo_putxx(&fifo16, (uint8_t*)&canframe);
//         canframe.id = 0x10101010;
//         hal_utility_fifo_putxx(&fifo16, (uint8_t*)&canframe);
//         
//         zz = hal_utility_fifo_frontxx(&fifo16);
//         hal_utility_fifo_pop(&fifo16);
//         hal_utility_fifo_getxx(&fifo16, (uint8_t*)&xx, &size);
//         size = size;
//         hal_utility_fifo_reset(&fifo16);
//     }
// #elif 1
//     // 8 bytes
//     static hal_utility_fifo_t thefifo;
//     static uint8_t thebuffer[12*8];
//     uint8_t *zz;
//     uint8_t size = 0;
//     uint64_t thebytes;
//     uint64_t xx;
//     
//     hal_utility_fifo_init(&thefifo, 12, 8, (uint8_t*)&thebuffer);
//     
//     uint32_t i;
//     for(i=0; i<100; i++)
//     {
//     
//         thebytes = 0x12345678;
//         hal_utility_fifo_put08(&thefifo, (uint8_t*)&thebytes);
//         thebytes = 0xabcdef0012345678;
//         hal_utility_fifo_put08(&thefifo, (uint8_t*)&thebytes);
//         thebytes = 0x1111111111111111;
//         hal_utility_fifo_put08(&thefifo, (uint8_t*)&thebytes);
//         thebytes = 0x2222222222222222;
//         hal_utility_fifo_put08(&thefifo, (uint8_t*)&thebytes);
//         
//         zz = hal_utility_fifo_front08(&thefifo);
//         hal_utility_fifo_pop(&thefifo);
//         hal_utility_fifo_get08(&thefifo, (uint8_t*)&xx, &size);
//         size = size;
//         hal_utility_fifo_reset(&thefifo);
//     }    
// #endif    
// }


// #include "hal_stm32_base_hid.h"
// static void s_test_fifoold(void)
// {
// #if 1    
//     static hal_base_genericfifo_t gfifo16;
//     uint8_t *zz;
//     uint8_t size = 0;
//     hal_can_frame_t xx;
//     
//     hal_can_frame_t canframe = 
//     {
//         .id = 0, .id_type = hal_can_frameID_std, 
//         .frame_type = hal_can_frame_data, .size = 0, .unused = 0xF1, .data = {0}
//     };

//     
//     static hal_can_frame_t gdatacanframes[12];
//     
//     hal_base_hid_genericfifo_init(&gfifo16, 12, 16, (uint8_t*)&gdatacanframes);
//     
//     uint32_t i;
//     for(i=0; i<100; i++)
//     {
//     
//         canframe.id = 0x12345678;
//         hal_base_hid_genericfifo_put(&gfifo16, (uint8_t*)&canframe);
//         canframe.id = 0xaabbccdd;
//         canframe.data[0] = 0x11; canframe.data[7] = 0x77;
//         hal_base_hid_genericfifo_put(&gfifo16, (uint8_t*)&canframe);
//         canframe.id = 0xdeaddead;
//         hal_base_hid_genericfifo_put(&gfifo16, (uint8_t*)&canframe);
//         canframe.id = 0x10101010;
//         hal_base_hid_genericfifo_put(&gfifo16, (uint8_t*)&canframe);
//         
//         zz = hal_base_hid_genericfifo_front(&gfifo16);
//         hal_base_hid_genericfifo_pop(&gfifo16);
//         hal_base_hid_genericfifo_get(&gfifo16, (uint8_t*)&xx, &size);
//         size = size;
//         zz = hal_base_hid_genericfifo_front(&gfifo16);
//         hal_base_hid_genericfifo_pop(&gfifo16);        
//         hal_base_hid_genericfifo_reset(&gfifo16);
//     }
// #elif 0
//     // 8 bytes
//     static hal_base_genericfifo_t    thefifo;
//     static uint8_t thebuffer[12*8];
//     uint8_t *zz;
//     uint8_t size = 0;
//     uint64_t thebytes;
//     uint64_t xx;
//     
//     hal_base_hid_genericfifo_init(&thefifo, 12, 8, (uint8_t*)&thebuffer);
//     
//     uint32_t i;
//     for(i=0; i<100; i++)
//     {
//     
//         thebytes = 0x12345678;
//         hal_base_hid_genericfifo_put(&thefifo, (uint8_t*)&thebytes);
//         thebytes = 0xabcdef0012345678;
//         hal_base_hid_genericfifo_put(&thefifo, (uint8_t*)&thebytes);
//         thebytes = 0x1111111111111111;
//         hal_base_hid_genericfifo_put(&thefifo, (uint8_t*)&thebytes);
//         thebytes = 0x2222222222222222;
//         hal_base_hid_genericfifo_put(&thefifo, (uint8_t*)&thebytes);
//         
//         zz = hal_base_hid_genericfifo_front(&thefifo);
//         hal_base_hid_genericfifo_pop(&thefifo);
//         hal_base_hid_genericfifo_get(&thefifo, (uint8_t*)&xx, &size);
//         size = size;
//         zz = hal_base_hid_genericfifo_front(&thefifo);
//         hal_base_hid_genericfifo_pop(&thefifo);        
//         hal_base_hid_genericfifo_reset(&thefifo);
//     }    
// #endif    
// }


static void s_test_fifonew(void)
{
#if 1 
    // 16 bytes    
    static hal_utility_fifo_t fifo16;
    static uint8_t data12x16[12*16];
    uint8_t *zz;
    uint8_t size = 0;
    hal_can_frame_t xx;
    
    hal_can_frame_t canframe = 
    {
        .id = 0, .id_type = hal_can_frameID_std, 
        .frame_type = hal_can_frame_data, .size = 0, .unused = 0xF1, .data = {0}
    };

    
    static hal_can_frame_t datacanframes[12];
    
    hal_utility_fifo_init(&fifo16, 12, 16, (uint8_t*)&datacanframes, NULL);
    
    uint32_t i;
    for(i=0; i<100; i++)
    {
    
        canframe.id = 0x12345678;
        hal_utility_fifo_put(&fifo16, (uint8_t*)&canframe);
        canframe.id = 0xaabbccdd;
        canframe.data[0] = 0x11; canframe.data[7] = 0x77;
        hal_utility_fifo_put(&fifo16, (uint8_t*)&canframe);
        canframe.id = 0xdeaddead;
        hal_utility_fifo_put(&fifo16, (uint8_t*)&canframe);
        canframe.id = 0x10101010;
        hal_utility_fifo_put(&fifo16, (uint8_t*)&canframe);
        
        zz = hal_utility_fifo_front(&fifo16);
        hal_utility_fifo_pop(&fifo16);
        hal_utility_fifo_get(&fifo16, (uint8_t*)&xx, &size);
        size = size;
        zz = hal_utility_fifo_front(&fifo16);
        hal_utility_fifo_pop(&fifo16);
        hal_utility_fifo_reset(&fifo16);
    }
#elif 1
    // 8 bytes
    static hal_utility_fifo_t thefifo;
    static uint8_t thebuffer[12*8];
    uint8_t *zz;
    uint8_t size = 0;
    uint64_t thebytes;
    uint64_t xx;
    
    hal_utility_fifo_init(&thefifo, 12, 8, (uint8_t*)&thebuffer, NULL);
    
    uint32_t i;
    for(i=0; i<100; i++)
    {
    
        thebytes = 0x12345678;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0xabcdef0012345678;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x1111111111111111;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x2222222222222222;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        
        zz = hal_utility_fifo_front(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_get(&thefifo, (uint8_t*)&xx, &size);
        size = size;
        zz = hal_utility_fifo_front(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_reset(&thefifo);
    } 
#elif 1    
    // 4 bytes
    static hal_utility_fifo_t thefifo;
    static uint8_t thebuffer[12*4];
    uint8_t *zz;
    uint8_t size = 0;
    uint32_t thebytes;
    uint32_t xx;
    
    hal_utility_fifo_init(&thefifo, 12, 4, (uint8_t*)&thebuffer, NULL);
    
    uint32_t i;
    for(i=0; i<100; i++)
    {
    
        thebytes = 0x12345678;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0xabcdef00;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x11111111;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x22222222;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        
        zz = hal_utility_fifo_front(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_get(&thefifo, (uint8_t*)&xx, &size);
        size = size;
        zz = hal_utility_fifo_front(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_reset(&thefifo);
    } 
#elif 1    
    // 2 bytes
    static hal_utility_fifo_t thefifo;
    static uint8_t thebuffer[12*2];
    uint8_t *zz;
    uint8_t size = 0;
    uint16_t thebytes;
    uint16_t xx;
    
    hal_utility_fifo_init(&thefifo, 12, 2, (uint8_t*)&thebuffer, NULL);
    
    uint32_t i;
    for(i=0; i<100; i++)
    {
    
        thebytes = 0x1234;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0xabcd;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x1111;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x2222;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        
        zz = hal_utility_fifo_front(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_get(&thefifo, (uint8_t*)&xx, &size);
        size = size;
        zz = hal_utility_fifo_front(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_reset(&thefifo);
    } 
#elif 1    
    // 1 bytes
    static hal_utility_fifo_t thefifo;
    static uint8_t thebuffer[12*1];
    uint8_t *zz;
    uint8_t size = 0;
    uint8_t thebytes;
    uint8_t xx;
    
    hal_utility_fifo_init(&thefifo, 12, 1, (uint8_t*)&thebuffer, NULL);
    
    uint32_t i;
    for(i=0; i<100; i++)
    {
    
        thebytes = 0x12;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0xab;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x11;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x22;
        hal_utility_fifo_put(&thefifo, (uint8_t*)&thebytes);
        
        zz = hal_utility_fifo_front(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_get(&thefifo, (uint8_t*)&xx, &size);
        size = size;
        zz = hal_utility_fifo_front(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_reset(&thefifo);
    }      
#endif   
}


static void s_test_fifoultra(void)
{
#if 1    
    static hal_utility_fifo_t fifo16;
    static uint8_t data12x16[12*16];
    uint8_t *zz;
    uint8_t size = 0;
    hal_can_frame_t xx;
    
    hal_can_frame_t canframe = 
    {
        .id = 0, .id_type = hal_can_frameID_std, 
        .frame_type = hal_can_frame_data, .size = 0, .unused = 0xF1, .data = {0}
    };

    
    static hal_can_frame_t udatacanframes[12];
    
    hal_utility_fifo_init(&fifo16, 12, 16, (uint8_t*)&udatacanframes, NULL);
    
    uint32_t i;
    for(i=0; i<100; i++)
    {
    
        canframe.id = 0x12345678;
        hal_utility_fifo_put16(&fifo16, (uint8_t*)&canframe);
        canframe.id = 0xaabbccdd;
        canframe.data[0] = 0x11; canframe.data[7] = 0x77;
        hal_utility_fifo_put16(&fifo16, (uint8_t*)&canframe);
        canframe.id = 0xdeaddead;
        hal_utility_fifo_put16(&fifo16, (uint8_t*)&canframe);
        canframe.id = 0x10101010;
        hal_utility_fifo_put16(&fifo16, (uint8_t*)&canframe);
        
        zz = hal_utility_fifo_front16(&fifo16);
        hal_utility_fifo_pop(&fifo16);
        hal_utility_fifo_get16(&fifo16, (uint8_t*)&xx, &size);
        size = size;
        zz = hal_utility_fifo_front16(&fifo16);
        hal_utility_fifo_pop(&fifo16);        
        hal_utility_fifo_reset(&fifo16);
    }
#elif 1
    // 8 bytes
    static hal_utility_fifo_t thefifo;
    static uint8_t thebuffer[12*8];
    uint8_t *zz;
    uint8_t size = 0;
    uint64_t thebytes;
    uint64_t xx;
    
    hal_utility_fifo_init(&thefifo, 12, 8, (uint8_t*)&thebuffer, NULL);
    
    uint32_t i;
    for(i=0; i<100; i++)
    {
    
        thebytes = 0x12345678;
        hal_utility_fifo_put08(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0xabcdef0012345678;
        hal_utility_fifo_put08(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x1111111111111111;
        hal_utility_fifo_put08(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x2222222222222222;
        hal_utility_fifo_put08(&thefifo, (uint8_t*)&thebytes);
        
        zz = hal_utility_fifo_front08(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_get08(&thefifo, (uint8_t*)&xx, &size);
        size = size;
        zz = hal_utility_fifo_front08(&thefifo);
        hal_utility_fifo_pop(&thefifo);        
        hal_utility_fifo_reset(&thefifo);
    }    
#elif 1
    // 4 bytes
    static hal_utility_fifo_t thefifo;
    static uint8_t thebuffer[12*4];
    uint8_t *zz;
    uint8_t size = 0;
    uint32_t thebytes;
    uint32_t xx;
    
    hal_utility_fifo_init(&thefifo, 12, 4, (uint8_t*)&thebuffer, NULL);
    
    uint32_t i;
    for(i=0; i<100; i++)
    {
    
        thebytes = 0x12345678;
        hal_utility_fifo_put04(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0xabcdef00;
        hal_utility_fifo_put04(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x11111111;
        hal_utility_fifo_put04(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x22222222;
        hal_utility_fifo_put04(&thefifo, (uint8_t*)&thebytes);
        
        zz = hal_utility_fifo_front04(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_get04(&thefifo, (uint8_t*)&xx, &size);
        size = size;
        zz = hal_utility_fifo_front04(&thefifo);
        hal_utility_fifo_pop(&thefifo);        
        hal_utility_fifo_reset(&thefifo);
    }  
#elif 1
    // 2 bytes
    static hal_utility_fifo_t thefifo;
    static uint8_t thebuffer[12*4];
    uint8_t *zz;
    uint8_t size = 0;
    uint16_t thebytes;
    uint16_t xx;
    
    hal_utility_fifo_init(&thefifo, 12, 2, (uint8_t*)&thebuffer, NULL);
    
    uint32_t i;
    for(i=0; i<100; i++)
    {
    
        thebytes = 0x1234;
        hal_utility_fifo_put02(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0xabcd;
        hal_utility_fifo_put02(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x1111;
        hal_utility_fifo_put02(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x2222;
        hal_utility_fifo_put02(&thefifo, (uint8_t*)&thebytes);
        
        zz = hal_utility_fifo_front02(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_get02(&thefifo, (uint8_t*)&xx, &size);
        size = size;
        zz = hal_utility_fifo_front02(&thefifo);
        hal_utility_fifo_pop(&thefifo);        
        hal_utility_fifo_reset(&thefifo);
    }      
#elif 1
    // 1 bytes
    static hal_utility_fifo_t thefifo;
    static uint8_t thebuffer[12*1];
    uint8_t *zz;
    uint8_t size = 0;
    uint8_t thebytes;
    uint8_t xx;
    
    hal_utility_fifo_init(&thefifo, 12, 1, (uint8_t*)&thebuffer, NULL);
    
    uint32_t i;
    for(i=0; i<100; i++)
    {
    
        thebytes = 0x12;
        hal_utility_fifo_put01(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0xab;
        hal_utility_fifo_put01(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x11;
        hal_utility_fifo_put01(&thefifo, (uint8_t*)&thebytes);
        thebytes = 0x22;
        hal_utility_fifo_put01(&thefifo, (uint8_t*)&thebytes);
        
        zz = hal_utility_fifo_front01(&thefifo);
        hal_utility_fifo_pop(&thefifo);
        hal_utility_fifo_get01(&thefifo, (uint8_t*)&xx, &size);
        size = size;
        zz = hal_utility_fifo_front01(&thefifo);
        hal_utility_fifo_pop(&thefifo);        
        hal_utility_fifo_reset(&thefifo);
    }          
#endif    
}


#if     defined(HAL_BOARD_MCBSTM32C) || defined(HAL_BOARD_STM3210CEVAL)

#if defined(TEST_SPI)

static volatile uint8_t s_test_slave_recv[4] = {0, 0, 0, 0};

static void s_test_spiframe_slave_receiv(void* p)
{
    static uint8_t index = 0;
    s_test_slave_recv[index++] = 1;
}

static volatile uint8_t s_test_master_recv[4] = {0, 0, 0, 0};

static void s_test_spiframe_master_receiv(void* p)
{
    static uint8_t index = 0;
    s_test_master_recv[index++] = 1;
}

static void s_test_spi_master_txrx_loopback(void);
static void s_test_spi_master_txonly_slave_rxonly(void);
static void s_test_spi_master_txrx_slave_txrx(void);
static void s_test_spi_master_rxonly_slave_txonly(void) ;

static void s_test_spi_master_slave(void)
{
    //s_test_spi_master_txrx_loopback();
    //s_test_spi_master_txonly_slave_rxonly();   
    s_test_spi_master_txrx_slave_txrx();
    //s_test_spi_master_rxonly_slave_txonly();
}


// static void s_test_spi_master_txrx_loopback(void)  
// {    
//     static const hal_spi_cfg_t spicfg_master =
//     {
//         .ownership              = hal_spi_ownership_master,
//         .direction              = hal_spi_dir_txrx,
//         .activity               = hal_spi_act_singleframe,
//         .speed                  = hal_spi_speed_18000kbps, //hal_spi_speed_9000kbps, //hal_spi_speed_9000kbps, //hal_spi_speed_0562kbps, hal_spi_speed_1125kbps
//         .sizeofframe            = 4,
//         .onframetransm          = NULL,
//         .onframereceiv          = s_test_spiframe_master_receiv,
//         .capacityoftxfifoofframes   = 4,
//         .capacityofrxfifoofframes   = 4,
//         .dummytxvalue           = 0xa0        
//     };   
//     
//     
//     static volatile uint8_t frame00[] = {1, 2, 3, 4};
//     static volatile uint8_t frame01[] = {5, 6, 7, 8};
//     static volatile uint8_t frame02[] = {9, 10, 11, 12};
//     
//     static volatile uint8_t rxframe[4] = {0};

//     static uint8_t size = 0;
//     static uint8_t remaining = 0;
//     
//     hal_result_t res;
//     

//     hal_spi_init(hal_spi_port2, &spicfg_master);    


//     hal_spi_put(hal_spi_port2, frame00, 4, hal_false);      
//     hal_spi_put(hal_spi_port2, frame01, 4, hal_false);
//     
//     hal_spi_start(hal_spi_port2);
//     hal_sys_delay(100*1000);
//     
//     hal_spi_start(hal_spi_port2);
//     hal_sys_delay(100*1000);
//     
//     while(0 == s_test_master_recv[0]);
//     s_test_master_recv[0] = 0;  

//     
//     res = hal_spi_get(hal_spi_port2, rxframe, &size, &remaining);   
//     if((rxframe[0] != frame00[0]) || (rxframe[1] != frame00[1]) || (rxframe[2] != frame00[2]) || (rxframe[3] != frame00[3]))
//     {
//         for(;;);
//     }
//     hal_sys_delay(100000);
//     while(0 == s_test_master_recv[1]);
//     s_test_master_recv[1] = 0;  
//     res = hal_spi_get(hal_spi_port2, rxframe, &size, &remaining);
//     if((rxframe[0] != frame01[0]) || (rxframe[1] != frame01[1]) || (rxframe[2] != frame01[2]) || (rxframe[3] != frame01[3]))
//     {
//         for(;;);
//     }
//     res = hal_spi_get(hal_spi_port2, rxframe, &size, &remaining);
//     
//     rxframe[0] = rxframe[0];
//     
//         
//     res = res;   
//     for(;;);
// }

// static void s_test_spi_master_txonly_slave_rxonly(void)  
// {    
//     static const hal_spi_cfg_t spicfg_master =
//     {
//         .ownership              = hal_spi_ownership_master,
//         .direction              = hal_spi_dir_txonly,
//         .activity               = hal_spi_act_singleframe,
//         .speed                  = hal_spi_speed_9000kbps,
//         .sizeoftxframe          = 4,
//         .sizeofrxframe          = 4,
//         .onframetransm          = NULL,
//         .onframereceiv          = NULL,
//         .capacityoftxfifoofframes   = 4,
//         .capacityofrxfifoofframes   = 4,
//         .dummytxvalue           = 0xa0        
//     };   
//     
//     static const hal_spi_cfg_t spicfg_slave =
//     {
//         .ownership              = hal_spi_ownership_slave,
//         .direction              = hal_spi_dir_rxonly,
//         .activity               = hal_spi_act_singleframe,
//         .speed                  = hal_spi_speed_9000kbps,
//         .sizeoftxframe          = 4,
//         .sizeofrxframe          = 4,
//         .onframetransm          = NULL,
//         .onframereceiv          = s_test_spiframe_slave_receiv,
//         .capacityoftxfifoofframes   = 4,
//         .capacityofrxfifoofframes   = 4,
//         .dummytxvalue           = 0xc0    
//     }; 
//     
//     static volatile uint8_t frame00[] = {1, 2, 3, 4};
//     static volatile uint8_t frame01[] = {5, 6, 7, 8};
//     static volatile uint8_t frame02[] = {9, 10, 11, 12};
//     
//     static volatile uint8_t rxframe[4] = {0};

//     static uint8_t size = 0;
//     static uint8_t remaining = 0;
//     
//     hal_result_t res;
//     

//     hal_spi_init(hal_spi_port2, &spicfg_master);    
//     hal_spi_init(hal_spi_port3, &spicfg_slave);  

//     hal_spi_start(hal_spi_port3);
//     
//     hal_sys_delay(100*1000);

//     hal_spi_put(hal_spi_port2, frame00, 4, hal_false);      
//     hal_spi_put(hal_spi_port2, frame01, 4, hal_false);
//     
//     hal_spi_start(hal_spi_port2);
//     hal_sys_delay(100*1000);
//     
//     hal_spi_start(hal_spi_port2);
//     hal_sys_delay(100*1000);
//     
//     while(0 == s_test_slave_recv[0]);
//     s_test_slave_recv[0] = 0;  

//     
//     res = hal_spi_get(hal_spi_port3, rxframe, &size, &remaining);   
//     if((rxframe[0] != frame00[0]) || (rxframe[1] != frame00[1]) || (rxframe[2] != frame00[2]) || (rxframe[3] != frame00[3]))
//     {
//         for(;;);
//     }
//     hal_sys_delay(100000);
//     while(0 == s_test_slave_recv[1]);
//     s_test_slave_recv[1] = 0;  
//     res = hal_spi_get(hal_spi_port3, rxframe, &size, &remaining);
//     if((rxframe[0] != frame01[0]) || (rxframe[1] != frame01[1]) || (rxframe[2] != frame01[2]) || (rxframe[3] != frame01[3]))
//     {
//         for(;;);
//     }
//     res = hal_spi_get(hal_spi_port3, rxframe, &size, &remaining);
//     
//     rxframe[0] = rxframe[0];
//     
//         
//     res = res;   
//     for(;;);
// }

// hal_spi_speed_9000kbps
// hal_spi_speed_4500kbps
// hal_spi_speed_0562kbps
#define SPEED2USE hal_spi_speed_4500kbps
static void s_test_spi_master_txrx_slave_txrx(void)  
{    
    static const hal_spi_cfg_t spicfg_master =
    {
        .ownership                  = hal_spi_ownership_master,
        .direction                  = hal_spi_dir_txrx,
        .activity                   = hal_spi_act_framebased,
        .speed                      = SPEED2USE, //hal_spi_speed_9000kbps,
        .sizeofframe                = 4,
        .onframetransm              = NULL,
        .onframereceiv              = s_test_spiframe_master_receiv,
        .capacityoftxfifoofframes   = 4,
        .capacityofrxfifoofframes   = 4,
        .dummytxvalue               = 0xc0
    };   
    
    static const hal_spi_cfg_t spicfg_slave =
    {
        .ownership                  = hal_spi_ownership_slave,
        .direction                  = hal_spi_dir_txrx,
        .activity                   = hal_spi_act_framebased,
        .speed                      = SPEED2USE, //hal_spi_speed_0562kbps, //hal_spi_speed_9000kbps,
        .sizeofframe                = 4,
        .onframetransm              = NULL,
        .onframereceiv              = s_test_spiframe_slave_receiv,
        .capacityoftxfifoofframes   = 4,
        .capacityofrxfifoofframes   = 4,
        .dummytxvalue               = 0xa0    
    }; 
    
    static uint8_t txframemas00[] = {1, 2, 3, 4};
    static uint8_t txframemas01[] = {5, 6, 7, 8};
    static uint8_t txframemas02[] = {9, 10, 11, 12};
    
    static uint8_t txframesla00[] = {129, 130, 131, 132};
    static uint8_t txframesla01[] = {133, 134, 135, 136};
    static uint8_t txframesla02[] = {137, 138, 139, 140};    
    
    static uint8_t rxframemas[4] = {0};
    static uint8_t rxframesla[4] = {0};
    static uint8_t size = 0;
    static uint8_t remaining = 0;
    
    hal_result_t res;
    

    hal_spi_init(hal_spi_port3, &spicfg_master);    
    hal_spi_init(hal_spi_port2, &spicfg_slave);  
    
    // load the slave with data to transmit
    hal_spi_put(hal_spi_port2, txframesla00);
    hal_spi_put(hal_spi_port2, txframesla01);
//    hal_spi_put(hal_spi_port2, txframesla02);
    
    
    // now the master transmits something
    hal_spi_put(hal_spi_port3, txframemas00);      
    hal_spi_put(hal_spi_port3, txframemas01);
    hal_spi_put(hal_spi_port3, txframemas02);

    // now start the slave ...
    hal_spi_start(hal_spi_port2, 255);
    
    hal_sys_delay(500*1000); // 500 milli
    
    

    
    // and start the master
    hal_spi_start(hal_spi_port3, 0); // or 4
    
    // now we sleep for some time so that the isr does transfer
    hal_sys_delay(500*1000); // 500 milli
    
    // now the slave reads
    while(0 == s_test_slave_recv[0]);
    s_test_slave_recv[0] = 0;     
    res = hal_spi_get(hal_spi_port2, rxframesla, &remaining);   
//    while(0 == s_test_slave_recv[1]);
//    s_test_slave_recv[1] = 0;  
    res = hal_spi_get(hal_spi_port2, rxframesla, &remaining);
//    while(0 == s_test_slave_recv[2]);
//    s_test_slave_recv[2] = 0;     
    res = hal_spi_get(hal_spi_port2, rxframesla, &remaining);

    // now the master reads
    while(0 == s_test_master_recv[0]);
    s_test_master_recv[0] = 0;     
    res = hal_spi_get(hal_spi_port3, rxframemas, &remaining);   
    res = res;
//    while(0 == s_test_master_recv[1]);
//    s_test_master_recv[1] = 0;  
    res = hal_spi_get(hal_spi_port3, rxframemas, &remaining);
//    while(0 == s_test_master_recv[2]);
//    s_test_master_recv[2] = 0;     
    res = hal_spi_get(hal_spi_port3, rxframemas, &remaining);
        
    res = res; 
    for(;;);    
}

// static void s_test_spi_master_rxonly_slave_txonly(void)  
// {    
//     static const hal_spi_cfg_t spicfg_master =
//     {
//         .ownership              = hal_spi_ownership_master,
//         .direction                    = hal_spi_dir_rxonly,
//         .activity               = hal_spi_act_singleframe,
//         .speed                  = hal_spi_speed_1125kbps,
//         .sizeoftxframe          = 4,
//         .sizeofrxframe          = 4,
//         .onframetransm          = NULL,
//         .onframereceiv          = s_test_spiframe_master_receiv,
//         .capacityoftxfifoofframes   = 4,
//         .capacityofrxfifoofframes   = 4,
//         .dummytxvalue           = 0xfa    
//     };   
//     
//     static const hal_spi_cfg_t spicfg_slave =
//     {
//         .ownership              = hal_spi_ownership_slave,
//         .direction                    = hal_spi_dir_txonly,
//         .activity               = hal_spi_act_singleframe,
//         .speed                  = hal_spi_speed_1125kbps,
//         .sizeoftxframe          = 4,
//         .sizeofrxframe          = 4,
//         .onframetransm          = NULL,
//         .onframereceiv          = s_test_spiframe_slave_receiv,
//         .capacityoftxfifoofframes   = 4,
//         .capacityofrxfifoofframes   = 4,
//         .dummytxvalue           = 0xfe    
//     }; 
//     
//     static uint8_t txframemas00[] = {1, 2, 3, 4};
//     static uint8_t txframemas01[] = {5, 6, 7, 8};
//     static uint8_t txframemas02[] = {9, 10, 11, 12};
//     
//     static uint8_t txframesla00[] = {129, 130, 131, 132};
//     static uint8_t txframesla01[] = {133, 134, 135, 136};
//     static uint8_t txframesla02[] = {137, 138, 139, 140};    
//     
//     static uint8_t rxframemas[4] = {0};
//     static uint8_t rxframesla[4] = {0};
//     static uint8_t size = 0;
//     static uint8_t remaining = 0;
//     
//     hal_result_t res;
//     

//     hal_spi_init(hal_spi_port2, &spicfg_master);    
//     hal_spi_init(hal_spi_port3, &spicfg_slave);  
//     
//     // load the slave with data to transmit
//     hal_spi_put(hal_spi_port3, txframesla00, 4, hal_false);
//     hal_spi_put(hal_spi_port3, txframesla01, 4, hal_false);
// //    hal_spi_put(hal_spi_port3, txframesla02, 4, hal_false);

//     // now start the slave ...
//     hal_spi_start(hal_spi_port3);
//     
//     
//     // now the master transmits something
//     //hal_spi_put(hal_spi_port2, txframemas00, 4, hal_false);      
//     //hal_spi_put(hal_spi_port2, txframemas01, 4, hal_false);
//     //hal_spi_put(hal_spi_port2, txframemas02, 4, hal_false);
//     
//     // and start the master
//     hal_spi_start(hal_spi_port2);
//     
//     // now we sleep for some time so that the isr does transfer
//     hal_sys_delay(500*1000); // 500 milli
//     
//      // now the master reads
//     while(0 == s_test_master_recv[0]);
//     s_test_master_recv[0] = 0;     
//     res = hal_spi_get(hal_spi_port2, rxframemas, &size, &remaining);   
//     
//     hal_spi_start(hal_spi_port2);
//     while(0 == s_test_master_recv[1]);
//     s_test_master_recv[1] = 0;  
//     res = hal_spi_get(hal_spi_port2, rxframemas, &size, &remaining);
//     
//     hal_spi_start(hal_spi_port2);
//     while(0 == s_test_master_recv[2]);
//     s_test_master_recv[2] = 0;     
//     res = hal_spi_get(hal_spi_port2, rxframemas, &size, &remaining);
//     
//     
//    // now the slave reads
// //     while(0 == s_test_slave_recv[0]);
// //     s_test_slave_recv[0] = 0;     
//     res = hal_spi_get(hal_spi_port3, rxframesla, &size, &remaining);   
// //    while(0 == s_test_slave_recv[1]);
// //    s_test_slave_recv[1] = 0;  
//     res = hal_spi_get(hal_spi_port3, rxframesla, &size, &remaining);
// //    while(0 == s_test_slave_recv[2]);
// //    s_test_slave_recv[2] = 0;     
//     res = hal_spi_get(hal_spi_port3, rxframesla, &size, &remaining);
//     
//         
//     res = res; 
//     for(;;);    
// }
#endif

#endif



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






