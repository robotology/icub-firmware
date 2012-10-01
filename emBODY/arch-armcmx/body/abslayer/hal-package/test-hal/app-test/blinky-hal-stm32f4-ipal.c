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




// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static volatile uint32_t msTicks = 0;
static volatile uint8_t s_tick = 0;
static volatile uint8_t s_blink = 0;

static ipal_udpsocket_t* s_udpskt = NULL;

static mydata mmm = {1, 2, 3, 4};

static uint8_t send_msg = 0;


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
}


static void button_init(void)
{
    hal_gpio_init(hal_gpio_portG, hal_gpio_pin15, hal_gpio_dirINP, hal_gpio_speed_low);
}

static uint8_t button_ispushed(void)
{
    static uint8_t v = 0;
    v = hal_gpio_getval(hal_gpio_portG, hal_gpio_pin15);

    return((1==v) ? 0 : 1);
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





// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






