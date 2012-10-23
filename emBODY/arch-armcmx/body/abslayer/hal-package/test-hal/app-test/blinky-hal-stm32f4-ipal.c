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
#include "ipal.h"  

#include "hal_brdcfg_modules.h"

#include "hal_switch.h"

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
// empty-section


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
    
    
    myledsinit();
    
    button_init();
    
     // 1 millisec.
    res = hal_sys_systick_sethandler(myonsystick, 1000, hal_int_priority00);
    res =  res;
    
    
#if 0    
    s_test_mco2();
    for(;;);
#endif
    
    
#ifdef  HAL_USE_SWITCH   
//     extern const hal_eth_phymode_t hal_brdcfg_eth__phymode;
//     hal_eth_phymode_t phymode = hal_brdcfg_eth__phymode; 
//     hal_switch_init(NULL);
//     hal_switch_start(&phymode);
//     for(;;);
#endif    
 

    test_eeprom();   

    test_can();
    
    
    ipal_base_memory_getsize(ipal_cfgMINE, &size04aligned);
    if(0 != size04aligned)
    {
        data32aligned = (uint32_t*)calloc(size04aligned/4, sizeof(uint32_t));   
    }
    


    ipal_base_initialise(ipal_cfgMINE, data32aligned);
    ipal_sys_start();  


    s_udp_init();
    
    for(;;)
    {
        if(1 == button_ispushed())
        {
            send_msg = 1;
        }

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
    
    }
    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

    



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// -------------------------------------------------------------------------------------------------------------------- 



static void myledsinit(void)
{
    hal_result_t res;
    
    res = hal_led_init(hal_led0, NULL);
    res =  res;
    
    res = hal_led_init(hal_led1, NULL);
    res =  res;
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
    hal_can_init(hal_can_port1, NULL);
    hal_can_init(hal_can_port2, NULL);
}


static void test_eeprom(void)
{
//#ifdef HAL_USE_EEPROM
    static uint8_t data[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31};
    
    static uint8_t tmp[64] = {0};
    
    hal_result_t res;
    
    //hal_i2c4hal_init(hal_i2c_port1, NULL);

    res = hal_eeprom_init(hal_eeprom_i2c_01, NULL);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, 0, 12, tmp);
    res =  res;  
    
    //return;   
    
    res = hal_eeprom_erase(hal_eeprom_i2c_01, 0, 256);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, 0, 12, tmp);
    res =  res;    
    
    res = hal_eeprom_write(hal_eeprom_i2c_01, 0, 12, data);
    res =  res;
    
    res = hal_eeprom_read(hal_eeprom_i2c_01, 0, 16, tmp);
    res =  res;

//#endif
}

#include "hal_stm32xx_include.h"
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
#endif


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






