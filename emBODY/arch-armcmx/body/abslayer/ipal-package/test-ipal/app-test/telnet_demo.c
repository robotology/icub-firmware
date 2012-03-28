/*
 * Copyright (C) 2011 Department of Robotics Brain and Cognitive Sciences - Istituto Italiano di Tecnologia
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


#include "ipal.h"




#include <stdio.h>


#include <string.h>


#include "stdlib.h"

#include "hal.h"
#include "fsal.h"

uint8_t blink = 0;
uint8_t LEDrun;
uint8_t LCDupdate;
uint8_t   lcd_text[2][16+1] = {"Line 1",           /* Buffer for LCD text         */
                          "Line 2"};


static void s_udp_init(void);
static void s_udp_transmit(void);
//static uint16_t s_udp_onrec(ipal_udpsocket_id_t sktid, ipal_arrayaddr_t rxip, ipal_port_t rxport, uint8_t *rxdata, uint16_t rxsize);
static ipal_udpsocket_t* s_udpskt = NULL;



/*--------------------------- LED_out ---------------------------------------*/

void LED_out (uint32_t val) 
{
    static uint8_t v = 0;
    v = hal_gpio_getval(hal_gpio_portE, hal_gpio_pin8);
    hal_gpio_setval(hal_gpio_portE, hal_gpio_pin8, (v == 1) ? (hal_gpio_valLOW) : (hal_gpio_valHIGH));
}


/*--------------------------- AD_in -----------------------------------------*/
//
//U16 AD_in (uint32_t ch) {
//  /* Read ARM Analog Input */
//  uint32_t val = 0;
//
//  if (ch < 1) {
//    val = ADC1->DR & 0x0FFF;
//  }
//  return (val);
//}
//extern uint8_t send_msg = 0;


static void button_init(void)
{
    hal_gpio_init(hal_gpio_portB, hal_gpio_pin7, hal_gpio_dirINP, hal_gpio_speed_low);
}

static uint8_t button_ispushed(void)
{
    static uint8_t v = 0;
    v = hal_gpio_getval(hal_gpio_portB, hal_gpio_pin7);

    return((1==v) ? 0 : 1);
}

/*--------------------------- timer_poll ------------------------------------*/

volatile uint8_t tick = 0;

static void timer_poll () {
  /* System tick timer running in poll mode */
  static uint32_t tt = 0;
                         
  if(1 == tick) 
  {
    tick = 0;

    ipal_sys_timetick_increment();
    if(++tt == 5)
    {
        blink = 1;
        tt = 0;
    }
  }
}

volatile uint32_t msTicks = 0;
                            /* counts 1ms timeTicks */
void systick_handler(void) 
{
    msTicks += 10;                             /* counts 1ms timeTicks */ 
    if(0 == (msTicks%100))
    {
        tick = 1;
    } 
}


/*--------------------------- fputc -----------------------------------------*/



/*--------------------------- blink_led -------------------------------------*/

static void blink_led () {

  static uint32_t cnt;

  if (blink == 1) 
  {
    /* Every 100 ms */
    blink = 0;
    LED_out(1);
  }
}


/*---------------------------------------------------------------------------*/


extern uint16_t tnet_onexec(const char *cmd, char *rep, uint8_t *quitflag);


extern const ipal_cfg_t *ipal_cfgMINE;

extern const hal_cfg_t *hal_cfgMINE;

int main (void) 
{
    uint8_t send_msg = 0;

    uint32_t hal_memory_size = 0;
    uint32_t *data32aligned = NULL;

    
    uint32_t ram32sizeip;
    uint32_t *ram32dataip = NULL;

//  fsal
    uint32_t ram32size;
    uint32_t *ram32data = NULL;
    extern const fsal_params_cfg_t *fsal_params_cfgMINE;


    hal_base_memory_getsize(hal_cfgMINE, &hal_memory_size); 
    
    if(0 != hal_memory_size)
    {
        data32aligned = (uint32_t*)calloc(hal_memory_size/4, sizeof(uint32_t));   
    }

    hal_base_initialise(hal_cfgMINE, data32aligned);

    hal_sys_systeminit();

    hal_gpio_init(hal_gpio_portE, hal_gpio_pin8, hal_gpio_dirOUT, hal_gpio_speed_low);

    button_init();

    hal_sys_systick_sethandler(systick_handler, 10*1000, hal_int_priority00);



    fsal_memory_getsize(fsal_params_cfgMINE, &ram32size);
    ram32data = (uint32_t*)calloc(ram32size/4, sizeof(uint32_t));


    fsal_initialise(fsal_params_cfgMINE, ram32data);

    fsal_format(fsal_drive_ram);
    fsal_format(fsal_drive_flash);

    hal_trace_puts("initialisation done\n");

    ipal_base_memory_getsize(ipal_cfgMINE, &ram32sizeip);
    ram32dataip = (uint32_t*)calloc(ram32sizeip/4, sizeof(uint32_t));
    ipal_base_initialise(ipal_cfgMINE, ram32dataip);
    ipal_sys_start();


    


    s_udp_init(); 

    while(1) 
    {
        send_msg = button_ispushed();
        timer_poll ();
        ipal_sys_process_communication();
        blink_led ();
        if(1 == send_msg)
        {
            send_msg = 0;
            s_udp_transmit();
        }
    }
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// telnet config




extern uint16_t tnet_onexec(const char *cmd, char *rep, uint8_t *quitflag)
{
    uint16_t cmdlen =0;
    static uint16_t cnt = 0;
    uint16_t retlen = 0;
    int arg1 = 0;

    cmdlen = strlen(cmd);
    *quitflag = 0;

    if(0 == strncmp(cmd, "help", strlen("help")))
    {
        if(cmdlen > 5)
        {
            sscanf((const char*)(cmd+5), "%d", &arg1);
            ipal_ftp_stop();
        } 
        
        retlen = sprintf ((char *)rep,"\r\n help found # %d w/ arg %d", cnt++, arg1);    

    }
    else if(0 == strncmp(cmd, "quit", strlen("quit")))
    {
        retlen = sprintf ((char *)rep,"\r\n quitting telnet... bye\r\n"); 
        *quitflag = 1;
        ipal_ftp_restart();
    }
    else
    {
        retlen = sprintf ((char *)rep,"\r\n command not found. type help for supported commands");
    }

    return(retlen);
}



typedef struct
{
    uint32_t aaa;
    uint16_t bbb;
    uint8_t  ccc;
    uint8_t  ddd;
} mydata;

static mydata mmm = {1, 2, 3, 4};

static ipal_tos_t tos = 
{
    .precedence         = ipal_prec_immediate,
    .lowdelay           = 1,
    .highthroughput     = 1,
    .highreliability    = 1,
    .unused             = 0
};

static void s_udp_onreception(void *arg, ipal_udpsocket_t *skt, ipal_packet_t *pkt, ipal_ipv4addr_t adr, ipal_port_t por)
{
    static mydata *m = NULL;
    
    m = (mydata*)arg;

    skt = skt;
    pkt = pkt;
    adr = adr;
    por = por;

}

static void s_udp_init(void)
{
    static ipal_result_t res;

    s_udpskt = ipal_udpsocket_new(tos);
    res = ipal_udpsocket_bind(s_udpskt, IPAL_ipv4addr_INADDR_ANY, 1001);
    res = res;

    res = ipal_udpsocket_recv(s_udpskt, s_udp_onreception, &mmm);

    res = res;

}

static void s_udp_transmit(void)
{
    static ipal_result_t res;
    static uint8_t data[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    static uint8_t arped = 0;
    static uint8_t data64[64] = {0};
    static ipal_packet_t pkt;

    pkt.data = data64;
    pkt.size = sizeof(data64);

    static ipal_ipv4addr_t ipv4addr = IPAL_ipv4addr(10, 255, 37, 204);  // put in here address of a known host, like your pc.



    if(0 == arped)
    {
        if(ipal_res_OK == ipal_arp_request(ipv4addr, ipal_arp_cache_permanently))
        {
            arped = 1;
        }
    }
    else
    {
        memset(data64, 0xaa, 64);
        res = ipal_udpsocket_sendto(s_udpskt, &pkt, ipv4addr, 1001);
        res = res;
    }

}

//static uint16_t s_udp_onrec(ipal_udpsocket_id_t sktid, ipal_arrayaddr_t rxip, ipal_port_t rxport, uint8_t *rxdata, uint16_t rxsize)
//{
//    sktid= sktid;
//    rxip = rxip;
//    rxport = rxport;
//    rxdata = rxdata;
//    rxsize = rxsize;
//
//    return(0);
//}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
