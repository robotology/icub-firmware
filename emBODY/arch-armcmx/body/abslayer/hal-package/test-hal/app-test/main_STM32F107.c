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



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"

//#include "hal_arch_arm.h"




#include "stdlib.h"

#include "hal.h"
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

//union uParameter {
//  uint8_t     bByte;   //  8 bit unsigned
//  uint16_t    iInt16;  // 16 bit signed
//  uint32_t    iInt32;  // 32 bit signed
//};
//typedef union uParameter USR_tu_Para;

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------

#define hal_timerx hal_timer2
#define hal_timery hal_timer3
//#define _TEST_CAN_
//#define _TEST_CRC_
//#define _TEST_BASIC_
//#define _TEST_ETH_
#define _TEST_EEPROM_
//#define _TEST_SPI_
//#define _TEST_EEPROM_ON_FLASH_  



#define LED         0x100

#define APPLADDR    0x8010000

#ifdef _TEST_EEPROM_
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
#endif

// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void Delay(uint32_t dlyTicks);
static void LED_Config(void);
static void LED_On(uint32_t led);
static void LED_Off(uint32_t led);

static void watchdog_cbk(void* p);

#ifdef _TEST_CRC_
    static void s_test_crc(void);
#endif

static void s_test_hal_basic(void);

static void s_test_hal_can(void);

static void s_test_hal_eth(void);

static void s_test_hal_spi(void);

static hal_eth_frame_t * s_getframe(uint32_t size);

static void s_giveframeback(hal_eth_frame_t *fr);

static void s_alert_higherlevel(void);

static void s_test_led(void);

#ifdef _TEST_EEPROM_
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
static void s_test_eeprom(void);
#endif


static void s_test_eeprom_on_flash(void);

static void s_test_cbk01(void* p);
static void s_test_cbk02(void* p);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static volatile uint32_t msTicks; 

static volatile uint32_t restartit = 0;

void s_tick(void); 

//inline uint32_t asasa(uint32_t a)
//{
//    return(a=2);
//}                          



#ifdef _TEST_EEPROM_
/* Private define ------------------------------------------------------------*/
#define EEPROM_WriteAddress1    0x50
#define EEPROM_ReadAddress1     0x50
#define BufferSize1             (countof(Tx1_Buffer)-1)
#define BufferSize2             (countof(Tx2_Buffer)-1)
#define BufferSize3             (countof(Tx3_Buffer)-1)
#define EEPROM_WriteAddress2    (EEPROM_WriteAddress1 + BufferSize1)
#define EEPROM_ReadAddress2     (EEPROM_ReadAddress1 + BufferSize1)
#define EEPROM_WriteAddress3    (EEPROM_WriteAddress2 + BufferSize2)
#define EEPROM_ReadAddress3     (EEPROM_ReadAddress2 + BufferSize2)

/* Private macro -------------------------------------------------------------*/
#define countof(a) (sizeof(a) / sizeof(*(a)))

/* Private variables ---------------------------------------------------------*/
uint8_t Tx1_Buffer[] = "AAABBBCCC";
uint8_t Tx2_Buffer[] = "ABCABC";
uint8_t Tx3_Buffer[] = "AAAAAAAA";
uint8_t Rx1_Buffer[BufferSize1], Rx2_Buffer[BufferSize2], Rx3_Buffer[BufferSize3];
volatile TestStatus TransferStatus1 = FAILED, TransferStatus2 = FAILED, TransferStatus3 = FAILED;
    
#endif










// --------------------------------------------------------------------------------------------------------------------
// - definition of main 
// --------------------------------------------------------------------------------------------------------------------

//#include "stdbool.h"


int main(void) 
{
    extern const hal_cfg_t *hal_cfgMINE;
    uint32_t size04aligned;
    uint32_t *data32aligned = NULL;
    uint32_t cnt = 0;
    hal_result_t res = hal_res_OK;

    static hal_arch_arm_uniqueid64_t uniqueid = 0;

    hal_can_frame_t canframe;



    uniqueid = hal_arch_arm_uniqueid64_get();
    uniqueid = uniqueid;

    hal_base_memory_getsize(hal_cfgMINE, &size04aligned); 

    if(0 != size04aligned)
    {
        data32aligned = (uint32_t*)calloc(size04aligned/4, sizeof(uint32_t));   
    }

    hal_base_initialise(hal_cfgMINE, data32aligned);

    hal_sys_systeminit();


    uniqueid = hal_arch_arm_uniqueid64_get();
    uniqueid = uniqueid;

#ifdef _TEST_CRC_
    s_test_crc();
#endif


//    s_test_hal_can();

#ifdef _TEST_EEPROM_
    s_test_eeprom();
#endif

    // 1 millisec.
    res = hal_sys_systick_sethandler(s_tick, 10, hal_int_priority00);

	//hal_base_hid_osal_scheduling_suspend();

	//hal_base_hid_osal_scheduling_restart();


    {   // timer
        hal_timer_cfg_t cfgx = 
        {
            .prescaler          = hal_timer_prescalerAUTO,         
            .countdown          = 1000*1000,//10*1000 + 10,
            .priority           = hal_int_priority03,
            .mode               = hal_timer_mode_periodic,
            .callback_on_exp    = s_test_cbk01,
            .arg                = (void*)hal_timerx
        };

        hal_timer_cfg_t cfgy = 
        {
            .prescaler          = hal_timer_prescalerAUTO,         
            .countdown          = 2000*1000,//10*1000 + 10,
            .priority           = hal_int_priority02,
            .mode               = hal_timer_mode_oneshot,
            .callback_on_exp    = s_test_cbk02,
            .arg                = (void*)hal_timery
        };

        hal_timer_init(hal_timerx, &cfgx, NULL);
        hal_timer_start(hal_timerx);
        hal_timer_init(hal_timery, &cfgy, NULL);
        hal_timer_start(hal_timery);


        for(;;)
        {
            static uint32_t forremx = 0;
            static uint32_t forremy = 0;

            hal_timer_remainingtime_get(hal_timerx, &forremx);
            forremx = forremx;
            if(forremx < 1000)
            {
                forremx = forremx;
            }
            hal_timer_remainingtime_get(hal_timery, &forremy);
            forremy = forremy;
         if(1 == restartit)
         {
            restartit = 0;
            //hal_timer_start(hal_timerx);
            
         }
        }

    }

    {   // watchdog  
//        const hal_watchdog_cfg_t cfgw = {5000, watchdog_cbk, NULL};
//        hal_watchdog_init(hal_watchdog_window, &cfgw);
//        hal_watchdog_start(hal_watchdog_window);

#ifdef HAL_USE_WATCHDOG 
        const hal_watchdog_cfg_t cfgw = {10*1000*1000, hal_int_priorityNONE, NULL, NULL};
        hal_watchdog_init(hal_watchdog_normal, &cfgw);
        hal_watchdog_start(hal_watchdog_normal);
#endif

    }

    {   // watchdog   
#ifdef HAL_USE_WATCHDOG
        const hal_watchdog_cfg_t cfgw = {5000, hal_int_priority00, watchdog_cbk, NULL};
        hal_watchdog_init(hal_watchdog_window, &cfgw);
        hal_watchdog_start(hal_watchdog_window);
#endif
    }

    
    s_test_led();

    LED_Config();


    if(res == hal_res_OK)
    {
        res = hal_res_NOK_generic;
    } 



#ifdef _TEST_EEPROM_ON_FLASH_
    s_test_eeprom_on_flash();
#endif

#ifdef _TEST_CAN_
    s_test_hal_can();

#endif




#ifdef 	 _TEST_EEPROM_
	 s_test_eeprom();
#endif



#ifdef _TEST_BASIC_
    s_test_hal_basic();
#endif



#ifdef _TEST_ETH_
    s_test_hal_eth();
#endif

#ifdef _TEST_SPI_
    s_test_hal_spi();
#endif




    Delay(5000);

    for(;;)
    {
        Delay(1000);
        LED_On(0);
        Delay(1000);
        LED_Off(0);
        if(++cnt > 10)
        {
        //    hal_sys_systemreset();
        }
    }



}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------

    

// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------

#ifdef _TEST_EEPROM_

static void s_test_eeprom_on_flash(void)
{
    static uint8_t dataeeflash0[3000] = {0, 11, 12, 13, 14, 15, 16, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0};
    static uint8_t dataeeflash1[3000] = {0};

    static uint8_t pagebuffer[2048];

    static uint32_t pageaddr = 0x08000000 + 128*1024; 
    static uint32_t offset   = 16; 

    hal_eeprom_cfg_t cfg;

    cfg.flashpagebuffer = pagebuffer;
    cfg.flashpagesize   = 2048;

    hal_eeprom_init(hal_eeprom_emulatedflash, &cfg);

    dataeeflash0[2048-offset] = 0xde; dataeeflash0[2048-offset+1] = 0xad;
    dataeeflash0[2048] = dataeeflash0[2049] = 3;

    hal_eeprom_write(hal_eeprom_emulatedflash, pageaddr+offset, 2500, dataeeflash0);
    hal_eeprom_read(hal_eeprom_emulatedflash, pageaddr+offset, 2500, dataeeflash1);
    hal_eeprom_erase(hal_eeprom_emulatedflash, pageaddr+offset, 2049);
}
#endif

static void s_can_callbkb_onrec(void *arg)
{
    hal_result_t res;
    hal_can_frame_t rec_frame, err_frame;
    uint8_t remaining;


    err_frame.id = 0xE;
    err_frame.id_type = hal_can_frameID_std;
    err_frame.frame_type = hal_can_frame_data;
    err_frame.data[0] = 'E';
    err_frame.data[1] = 'R';
    err_frame.data[2] = 'R';
    err_frame.data[3] = '0';
    err_frame.data[4] = 'R';  
    err_frame.size = 5;

        
    res = hal_can_get(hal_can_port1, &rec_frame, &remaining);
    
    if(hal_res_OK != res)
    {
        hal_can_put(hal_can_port1, &err_frame, hal_can_send_normprio_now);
        return;
    }

     rec_frame.data[0]++;
     rec_frame.data[1] = remaining;
     hal_can_put(hal_can_port1, &rec_frame, hal_can_send_normprio_now);

    
}


static void s_test_hal_can(void)
{
    hal_can_frame_t canframe;
    static hal_result_t res;
    uint8_t count;

    hal_can_cfg_t my_can_cfg_default;

    my_can_cfg_default.runmode            = hal_can_runmode_isr_1txq1rxq;
    my_can_cfg_default.baudrate           = hal_can_baudrate_1mbps; 
    my_can_cfg_default.priorx             = hal_int_priority00;
    my_can_cfg_default.priotx             = hal_int_priority00;
    my_can_cfg_default.callback_on_rx     = s_can_callbkb_onrec;
    my_can_cfg_default.arg                = NULL;


    //it is necessary initilize canframe fileds!!
    //hal_can_ functions don't send can frame not well-formed
     
    canframe.id = 0xE;
    canframe.id_type = hal_can_frameID_std;
    canframe.frame_type = hal_can_frame_data;
    canframe.data[0] = 0xCC;
    canframe.data[1] = 0xCC;
    canframe.data[2] = 0xCC;
    canframe.data[3] = 0xCC;
    canframe.data[4] = 0xCC;
    canframe.data[5] = 0xCC;
    canframe.data[6] = 0xCC;
    canframe.data[7] = 0xCC;

    res = hal_can_init(hal_can_port1, &my_can_cfg_default);

    if(hal_res_OK != res)
    {
        return;
    }

    res = hal_can_enable(hal_can_port1);
    if(hal_res_OK != res)
    {
        return;
    }

//    for(;;);

 
    canframe.size = 8;
    canframe.data[0] = 1;
    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_later);


    canframe.size = 8;
    canframe.data[0] = 2;
    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_later);


    canframe.size = 8;
    canframe.data[0] = 3;
    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_later);
   
       
    hal_can_transmit(hal_can_port1);


    canframe.size = 8;
    canframe.data[0] = 4;
    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now);

    canframe.size = 8;
    canframe.data[0] = 5;
    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now);

    canframe.size = 8;
    canframe.data[0] = 6;
    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now);

    canframe.size = 8;
    canframe.data[0] = 7;
    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now);

    while(1)
    {
        for(count = 0; count <10; count ++)
        {
            canframe.data[0] = 0xF0 + count;
            hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now);
//            canframe.data[0] = count;
//            hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now);

        }
        Delay(1000);

    
    }
}


static void s_test_hal_basic(void)
{

    hal_result_t res;

    uint32_t data04[9];

    uint8_t data01[8];
    uint32_t word = 0x12345678;
    uint16_t hword = 0x1234;
 

    LED_Config();
    LED_On(0);
    LED_Off(0);

    LED_On(0);

    hal_gpio_getval(hal_gpio_portE, hal_gpio_pin8);

//    hal_display_init(NULL);
//
//    hal_display_clear(hal_col_white);
//
//    hal_display_settext(hal_font_24x16, hal_col_lightgrey, hal_col_yellow);
//    hal_display_putstring(2, "  hello yellow");
//
//    hal_display_settext(hal_font_24x16, hal_col_red, hal_col_green);
//    hal_display_putstring(3, "  hello green on red ");


//    hal_initialise(NULL, NULL);

//    hal_flash_setlatency(hal_flash_1cycles);


    res = hal_flash_unlock();

    res = hal_flash_erase(0x08020003, 1);
    res = hal_flash_write(0x08020000, 4, &word);
    res = hal_flash_write(0x08020007, 2, &hword);

    res = hal_flash_write(0x08020010, sizeof(data04), data04);

    res = hal_flash_write(0x08020040, sizeof(data01), (uint32_t*)data01);

    res = hal_flash_erase(0x08020003, 4096);
    res = hal_flash_write(0x08020010, sizeof(data04), data04);
    res = hal_flash_write(0x08020030, 8, data04);
    res = hal_flash_write(0x08020080, 2, &data04[0]);
    res = hal_flash_write(0x08020090, 2, &hword);
    res = hal_flash_write(0x08020000+4096, sizeof(data04), data04);
    res = hal_flash_erase(0x08020003, 4096);

}


static void s_test_hal_eth(void)
{
    
    static hal_eth_cfg_t cfg;
    static hal_eth_onframereception_t frx;
//    static hal_eth_macaddress_t mac = hal_ETH_macaddress(0, 1, 2, 3, 4, 5);
    static hal_eth_frame_t *pframe;
    static uint32_t datafr[256] = {255};
    pframe = (hal_eth_frame_t*) &datafr[0];

    frx.frame_new                   = s_getframe;
    frx.frame_movetohigherlayer     = s_giveframeback;
    frx.frame_alerthigherlayer      = s_alert_higherlevel;

    pframe->length = 256;
    pframe->index  = 0;
//    frame.data   = datafr;

    cfg.onframerx           = &frx;
    cfg.macaddress          = hal_ETH_macaddress(0, 1, 2, 3, 4, 5);


    hal_eth_init(&cfg);

    hal_eth_enable();
    
    hal_eth_sendframe(pframe);
    hal_eth_enable();    
}



static void Delay(uint32_t dlyTicks) 
{
    volatile uint32_t curTicks;

    curTicks = msTicks;
    while((msTicks - curTicks) < dlyTicks);
}

static void watchdog_cbk(void* p)
{
#ifdef HAL_USE_WATCHDOG    
    hal_watchdog_refresh(hal_watchdog_window);
#endif
}

static void encoder6_cbk(void* p)
{
    static uint32_t mio_risultato=0xAABBCC;
    
    hal_encoder_get_value(hal_encoder6, &mio_risultato);
}


static void encoder7_cbk(void* p)
{
    static uint32_t mio_risultato=0xAABBCC;
    
    hal_encoder_get_value(hal_encoder7, &mio_risultato);
}


static void encoder8_cbk(void* p)
{
    static uint32_t mio_risultato=0xAABBCC;
    
    hal_encoder_get_value(hal_encoder8, &mio_risultato);
}


static void s_test_hal_spi()
{
    uint32_t res;
    static uint32_t s_res;
    uint32_t i;
/*
	Note: in order to test spi_encoder on mcbstm32c you need the mcbstm32c board modified with two AEA boards.
	On this board you can read correct values from hal_encoder6 only, because hal_encoder7 doesn't work properly.
	the instructions to read from hal_encoder7 and hal_encoder8 have been written only for example use.
*/
    hal_encoder_cfg_t enc_cfg6 = 
    { 
        .priority           = hal_int_priority07,
        .callback_on_rx     = encoder6_cbk,
        .arg                = NULL
    };

    hal_encoder_cfg_t enc_cfg7 = 
    {
        .priority           = hal_int_priority07,   
        .callback_on_rx     = encoder7_cbk,
        .arg                = NULL
    };

    hal_encoder_cfg_t enc_cfg8 = 
    {
        .priority           = hal_int_priority07,   
        .callback_on_rx     = encoder8_cbk,
        .arg                = NULL
    };
   
    // encoder6 is on spi3, mux0
    hal_encoder_init(hal_encoder6, &enc_cfg6);
    // encoder7 is on spi3, mux1
    hal_encoder_init(hal_encoder7, &enc_cfg7);
    // encoder8 is on spi3, mux2
    hal_encoder_init(hal_encoder8, &enc_cfg8);

    hal_encoder_read_block(hal_encoder6, &res);

    hal_encoder_read_block(hal_encoder7, &res);

    hal_encoder_read_block(hal_encoder8, &res);
    
    hal_encoder_read_start(hal_encoder6);

    while(1)
    {;}

//while(1)
//{
// i = 500;
// hal_encoder_read_start(hal_spi_port3, hal_spiEncoder_3);
// while(i>0)
// {i--;}
// res = 0;
// res = hal_encoder_read_block(hal_spi_port3, hal_spiEncoder_1);
//}

}


static void LED_Config(void) 
{
    // assume port E. the LED is led 8
    hal_gpio_init(hal_gpio_portE, hal_gpio_pin8, hal_gpio_dirOUT, hal_gpio_speed_high);
}


static void LED_On(uint32_t led) 
{
    hal_gpio_setval(hal_gpio_portE, hal_gpio_pin8, hal_gpio_valHIGH); 
}


static void LED_Off(uint32_t led) 
{
    hal_gpio_setval(hal_gpio_portE, hal_gpio_pin8, hal_gpio_valLOW);  
}


static void s_tick(void)
{
    static uint32_t rem = 0;
    msTicks++;
//     hal_watchdog_refresh(hal_watchdog_window);
//    hal_watchdog_refresh(hal_watchdog_normal);

	hal_base_hid_osal_scheduling_suspend();

	hal_base_hid_osal_scheduling_restart();

    hal_timer_remainingtime_get(hal_timery, &rem);
    rem = rem;
}


static hal_eth_frame_t * s_getframe(uint32_t size)
{
    static hal_eth_frame_t *pframe;
    static uint32_t dataframe[256] = {0};

    pframe = (hal_eth_frame_t*)&dataframe[0];
    pframe->length = size;
    pframe->index = 0;
    return(pframe);
}

static void s_giveframeback(hal_eth_frame_t *fr)
{
    static hal_eth_frame_t *fraptr = NULL;
    fraptr = fr;

    if(NULL == fraptr)
    {
        fraptr = fraptr;
    }
}

static void s_alert_higherlevel(void)
{
    static uint32_t alerted = 0;

    alerted ++;
    if(100 == alerted)
    {
        alerted = 99;
        alerted ++;
    }
}



#ifdef _TEST_EEPROM_
/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared.
  * @param  BufferLength: buffer's length
  * @retval PASSED: pBuffer1 identical to pBuffer2
  *         FAILED: pBuffer1 differs from pBuffer2
  */
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  while(BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    
    pBuffer1++;
    pBuffer2++;
  }

  return PASSED;  
}



static void s_test_eeprom(void)
{
    static uint8_t data[512] = {0};
   /* Initialize the I2C EEPROM driver ----------------------------------------*/
    hal_eeprom_init(hal_eeprom_i2c_01, NULL);  

    hal_eeprom_erase(hal_eeprom_i2c_01, 0, 2*1024);

    hal_eeprom_read(hal_eeprom_i2c_01, 1024, 512, data);

    /* First write in the memory followed by a read of the written data --------*/
    /* Write on I2C EEPROM from EEPROM_WriteAddress1 */
    hal_eeprom_write(hal_eeprom_i2c_01, EEPROM_WriteAddress1, BufferSize1, Tx1_Buffer);
    
    /* Read from I2C EEPROM from EEPROM_ReadAddress1 */
    hal_eeprom_read(hal_eeprom_i2c_01, EEPROM_WriteAddress1, BufferSize1, Rx1_Buffer);
    
    /* Check if the data written to the memory is read correctly */
    TransferStatus1 = Buffercmp(Tx1_Buffer, Rx1_Buffer, BufferSize1);
    /* TransferStatus1 = PASSED, if the transmitted and received data 
     to/from the EEPROM are the same */
    /* TransferStatus1 = FAILED, if the transmitted and received data 
     to/from the EEPROM are different */
    
    /* Wait for EEPROM standby state */
    //hal_eeprom_waitstandbystate(); this function has been removed
    
    /* Second write in the memory followed by a read of the written data -------*/
    /* Write on I2C EEPROM from EEPROM_WriteAddress2 */
    hal_eeprom_write(hal_eeprom_i2c_01, EEPROM_WriteAddress2, BufferSize2, Tx2_Buffer);
    
    hal_eeprom_write(hal_eeprom_i2c_01, EEPROM_WriteAddress3, BufferSize3, Tx3_Buffer);
    
    /* Read from I2C EEPROM from EEPROM_ReadAddress2 */
    hal_eeprom_read(hal_eeprom_i2c_01, EEPROM_WriteAddress2, BufferSize2, Rx2_Buffer);
    
    /* Check if the data written to the memory is read correctly */
    TransferStatus2 = Buffercmp(Tx2_Buffer, Rx2_Buffer, BufferSize2);
    /* TransferStatus2 = PASSED, if the transmitted and received data 
     to/from the EEPROM are the same */
    /* TransferStatus2 = FAILED, if the transmitted and received data 
     to/from the EEPROM are different */
    
    hal_eeprom_read(hal_eeprom_i2c_01, EEPROM_WriteAddress3, BufferSize3, Rx3_Buffer);
    
    TransferStatus3 = Buffercmp(Tx3_Buffer, Rx3_Buffer, BufferSize3);
    
    while (1)
    {
    }

}

#endif //_TEST_EEPROM_


static void s_test_led(void)
{
    static hal_result_t res;
    static hal_gpio_val_t val = hal_gpio_valHIGH;
    
    res = hal_gpio_init(hal_gpio_portE, hal_gpio_pin8, hal_gpio_dirOUT, hal_gpio_speed_low);

    for(;;)
    {
        
        val = (hal_gpio_valHIGH == val) ? (hal_gpio_valLOW) : (hal_gpio_valHIGH);
        
        //sleep(1000*1000); // microseconds

        res = hal_gpio_setval(hal_gpio_portE, hal_gpio_pin8, val);
    } 
}

static void s_test_cbk01(void* p)
{
    static hal_gpio_val_t val = hal_gpio_valNONE;
    hal_timer_t tt = (hal_timer_t)p;
    tt = tt;

	hal_base_hid_osal_scheduling_suspend();

	hal_base_hid_osal_scheduling_restart();

    if(hal_gpio_valNONE == val)
    {
        hal_gpio_init(hal_gpio_portE, hal_gpio_pin8, hal_gpio_dirOUT, hal_gpio_speed_low); 
        val = hal_gpio_valHIGH;  
    }

    val = (hal_gpio_valHIGH == val) ? (hal_gpio_valLOW) : (hal_gpio_valHIGH);
    hal_gpio_setval(hal_gpio_portE, hal_gpio_pin8, val);
    
    //restartit = 1;  
    //hal_timer_start(hal_timerx); 
}

static void s_test_cbk02(void* p)
{
    static hal_gpio_val_t val = hal_gpio_valNONE;
    hal_timer_t tt = (hal_timer_t)p;
    tt = tt;

    if(hal_gpio_valNONE == val)
    {
        hal_gpio_init(hal_gpio_portE, hal_gpio_pin9, hal_gpio_dirOUT, hal_gpio_speed_low); 
        val = hal_gpio_valHIGH;  
    }

    val = (hal_gpio_valHIGH == val) ? (hal_gpio_valLOW) : (hal_gpio_valHIGH);
    hal_gpio_setval(hal_gpio_portE, hal_gpio_pin9, val);
    
    //restartit = 1;  
    hal_timer_start(hal_timery); 
}


#ifdef _TEST_CRC_
static void s_test_crc(void)
{
    static uint8_t data[16] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'l', 'm', 'n', 'o', 'p', 'q', 'r' };
    //static uint8_t data[16] = {'0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '0' };
    //static uint8_t data[16] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 };

    static uint16_t tblcrc16[256] = {0};
    //static uint8_t data[16] = {0};

    static uint32_t crc = 0;

    hal_result_t res;

    hal_crc_cfg_t cfg;


    res = hal_crc_init(hal_crc0, NULL);

    res = hal_crc_compute(hal_crc0, hal_crc_mode_clear, data, 16, &crc);
    //res = hal_crc_compute(hal_crc0, data, 16, (uint32_t*)&data[8]);

    res = hal_crc_compute(hal_crc0, hal_crc_mode_clear, &data[0], 4, &crc);
    res = hal_crc_compute(hal_crc0, hal_crc_mode_accumulate, &data[4], 4, &crc);
    res = hal_crc_compute(hal_crc0, hal_crc_mode_accumulate, &data[8], 4, &crc);
    res = hal_crc_compute(hal_crc0, hal_crc_mode_accumulate, &data[12], 4, &crc);


    res =  res;

    cfg.order       = hal_crc_order_16;
    cfg.polynomial  = hal_crc_poly_crc16_ccitt;
    cfg.crctblram   = NULL; //tblcrc16;

    res = hal_crc_init(hal_crc1, &cfg);

    res = hal_crc_compute(hal_crc1, hal_crc_mode_clear, data, 14, &crc);
    res = hal_crc_compute(hal_crc1, hal_crc_mode_clear, &data[0], 4, &crc);
    res = hal_crc_compute(hal_crc1, hal_crc_mode_accumulate, &data[4], 4, &crc);
    res = hal_crc_compute(hal_crc1, hal_crc_mode_accumulate, &data[8], 4, &crc);
    res = hal_crc_compute(hal_crc1, hal_crc_mode_accumulate, &data[12], 2, &crc);
    //res = hal_crc_compute(hal_crc1, data, 8, (uint32_t*)&data[8]);

    res =  res;

 
 
    for(;;);












}
#endif

// end of file



