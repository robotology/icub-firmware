
/* @file       main_ems001.c
    @brief      This file implements simple application for testing hal on ems001.
    @author    valentina.gaggero@iit.it
    @date       16/02/2011
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------
#include "string.h"
#include "stdio.h"
#include "hal.h"
#include "hal_trace.h"
#include "hal_arch.h"

#include "stdlib.h"
#include "hal_switch.h"


#include "stm32f10x.h"    //solo per test spi con tempo


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


//#define _TEST_CAN_
//#define _TEST_BASIC_
//#define _TEST_ETH_
//#define _CHECK_SWITCH_STATUS_ //enable check eth link status 
//#define _TEST_EEPROM_
//#define _TEST_SPI_
//#define _TEST_EEPROM_ON_FLASH_
//#define _TEST_I2C_ 
#define _TEST_SW_INTERRUPT_

//#ifdef _TEST_I2C_ 
//    #define _TEST_EEPROM_
//#endif



#define LED         0x100

#define APPLADDR    0x8010000

#ifdef _TEST_EEPROM_
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;
#endif

#ifdef _TEST_CAN_
uint8_t rec_msg1 = 0;
uint8_t rec_msg2 = 0;
#endif

#ifdef _TEST_SPI_
uint8_t spi_read_finish = 0; 
uint8_t spi3_read_finish = 0;
#define LED_ON      GPIOB->BSRR |= (1<<9);
#define LED_OFF     GPIOB->BRR |= (1<<9);
#endif


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

static void Delay(uint32_t dlyTicks);
static void LED_Config(void);
static void LED_On(uint32_t led);
static void LED_Off(uint32_t led);





static void s_test_hal_basic(void);

static void s_test_hal_can(void);

static void s_test_hal_eth(void);

static void s_test_hal_spi(void);

static void s_test_hal_i2c(void);

static hal_eth_frame_t * s_getframe(uint32_t size);

static void s_giveframeback(hal_eth_frame_t *fr);

static void s_alert_higherlevel(void);

#ifdef _TEST_EEPROM_
static TestStatus Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
static void s_test_eeprom(void);
#endif


static void s_test_eeprom_on_flash(void);

#ifdef _TEST_SW_INTERRUPT_
#include "stm32f1.h"
static void s_test_sw_interrupt(void);
#endif

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------

static volatile uint32_t msTicks; 
void s_tick(void); 

#ifdef _CHECK_SWITCH_STATUS_
#error pay attention: s_check_switch checks only phy2!!
void s_check_switch(void);
static volatile uint32_t count;
static uint8_t enacheckswitch = 0;
#endif

//inline uint32_t asasa(uint32_t a)
//{
//    return(a=2);
//}                          



#ifdef _TEST_EEPROM_
/* Private define ------------------------------------------------------------*/
#define EEPROM_WriteAddress1    (62*1024)//0x50
#define EEPROM_ReadAddress1     (62*1024)//0x50
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
uint8_t Tx1_Buffer[] = "ABCABC";
uint8_t Tx2_Buffer[] = "0123456";
uint8_t Tx3_Buffer[] = "0A1B3C";
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
    static hal_arch_arm_uniqueid64_t uniqueid = 0;

    uniqueid = hal_arch_arm_uniqueid64_get();
    uniqueid = uniqueid;

    hal_base_memory_getsize(hal_cfgMINE, &size04aligned); 

    if(0 != size04aligned)
    {
        data32aligned = (uint32_t*)calloc(size04aligned/4, sizeof(uint32_t));   
    }

    hal_base_initialise(hal_cfgMINE, data32aligned);

    hal_sys_systeminit();
    
    hal_trace_init();
#ifdef _CHECK_SWITCH_STATUS_
    hal_sys_systick_sethandler(s_check_switch, 1000, hal_int_priority00);
#else
    hal_sys_systick_sethandler(s_tick, 1000*1000, hal_int_priority00);
#endif

#ifdef _TEST_I2C_
    s_test_hal_i2c();
#endif


#ifdef _TEST_CAN_
    s_test_hal_can();
#endif

#ifdef _TEST_EEPROM_ON_FLASH_
    s_test_eeprom_on_flash();
#endif


#ifdef 	 _TEST_EEPROM_
	// s_test_eeprom();
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

#ifdef _TEST_SW_INTERRUPT_
s_test_sw_interrupt();
#endif

//    Delay(5000);

    for(;;)
    {
//        Delay(1000);
 //       LED_On(0);
 //       Delay(1000);
 //       LED_Off(0);
        if(++cnt > 1000)
        {
        //    hal_sys_systemreset();
			cnt=0;
        }
    }



}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------
static void s_test_hal_i2c(void)
{
    const hal_switch_cfg_t switch_cfg = {.dummy = 0};

    
//    hal_eeprom_init(hal_eeprom_i2c_01, NULL); 
  
//    /* First write in the memory followed by a read of the written data --------*/
//    /* Write on I2C EEPROM from EEPROM_WriteAddress1 */
//    hal_eeprom_write(hal_eeprom_i2c_01, EEPROM_WriteAddress1, BufferSize1, Tx1_Buffer);
//    
//    /* Read from I2C EEPROM from EEPROM_ReadAddress1 */
//    hal_eeprom_read(hal_eeprom_i2c_01, EEPROM_WriteAddress1, BufferSize1, Rx1_Buffer);
    
    
    
//    s_test_eeprom();

    hal_switch_init(&switch_cfg);

    hal_switch_init(&switch_cfg);

//    s_test_eeprom();

//    hal_sys_systemreset();

    
}
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
#ifdef _TEST_CAN_
//static void s_can_callbkb_onrec(void *arg)
//{
//    hal_result_t res;
//    hal_can_frame_t rec_frame, err_frame;
//    uint8_t remaining;
//    static uint8_t count =0;
//
//
//    err_frame.id = 0x1E;
//    err_frame.id_type = hal_can_frameID_std;
//    err_frame.frame_type = hal_can_frame_data;
//    err_frame.data[0] = 'E';
//    err_frame.data[1] = 'R';
//    err_frame.data[2] = 'R';
//    err_frame.data[3] = '0';
//    err_frame.data[4] = 'R';  
//    err_frame.size = 5;
//
//        
//    res = hal_can_get(hal_can_port1, &rec_frame, &remaining);
//    
//    if(hal_res_OK != res)
//    {
//        hal_can_put(hal_can_port1, &err_frame, hal_can_send_normprio_now);
//        return;
//    }
//
//     rec_frame.data[2] = (uint8_t)rec_frame.id;
//     rec_frame.id = 0x1E;
//
//     rec_frame.data[1] = remaining;
//     if(count < 6)
//     {
//        hal_can_put(hal_can_port1, &rec_frame, hal_can_send_normprio_now);
//        count++;
//     }
//    
//}

static void s_can_callbkb_onrec(void *arg)
{
    rec_msg1 = 1;
    
}

static void s_can_callbkb_onrec2(void *arg)
{
    rec_msg2 = 1;
    
}
static void s_can_onrec(hal_can_port_t port)
{
    hal_result_t res;
    hal_can_frame_t rec_frame, err_frame;
    uint8_t remaining;

    if(hal_can_port1 == port)
    {
        rec_msg1 = 0;
    }
    else
    {
        rec_msg2 = 0;
    }

    err_frame.id = 0x2E;
    err_frame.id_type = hal_can_frameID_std;
    err_frame.frame_type = hal_can_frame_data;
    err_frame.data[0] = 'E';
    err_frame.data[1] = 'R';
    err_frame.data[2] = 'R';
    err_frame.data[3] = '0';
    err_frame.data[4] = 'R';  
    err_frame.size = 5;

//    do
//    {    
        res = hal_can_get(port, &rec_frame, &remaining);
        
        if(hal_res_OK != res)
        {
            hal_can_put(port, &err_frame, hal_can_send_normprio_now);
            return;
        }
         rec_frame.data[2] = (uint8_t)rec_frame.id;
         rec_frame.id = (uint8_t)port;
    
         rec_frame.data[1] = remaining;
         res = hal_can_put(port, &rec_frame, hal_can_send_normprio_now);
//    }
//    while(remaining > 0);
    
}




static void s_can_printstatus(hal_can_port_t port, hal_can_status_t *status_ptr)
{
    char str[200];
    snprintf(str, sizeof(str)-1, "ERROR port%d: REC=%d TEC=%d w=%d p=%d boff=%d txfull=%d rxfull=%d", port, status_ptr->u.s.hw_status.REC,
                                                                                        status_ptr->u.s.hw_status.warning,
                                                                                        status_ptr->u.s.hw_status.passive,
                                                                                        status_ptr->u.s.hw_status.busoff,
                                                                                        status_ptr->u.s.hw_status.txqueueisfull,
                                                                                        status_ptr->u.s.hw_status.rxqueueisfull   
                                                                                        );
    hal_trace_puts(str);
}
volatile hal_can_status_t status1, status2;
static void s_can_callbkp_can1_onerr(void *arg)
{
    hal_can_getstatus(hal_can_port1, &status1);
//    s_can_printstatus(hal_can_port1, &status1);
}

static void s_can_callbkp_can2_onerr(void *arg)
{
    hal_can_getstatus(hal_can_port2, &status2);
//    s_can_printstatus(hal_can_port2, &status2);
}

volatile uint32_t a;
static void s_test_hal_can(void)
{
    hal_can_frame_t canframe;
    hal_result_t res;
    uint32_t count, i;
    uint32_t id1 = 0x1E;
    uint32_t id2 = 0x2E;
    


    hal_can_cfg_t my_can_cfg_default;

    my_can_cfg_default.runmode            = hal_can_runmode_isr_1txq1rxq;
    my_can_cfg_default.baudrate           = hal_can_baudrate_1mbps; 
    my_can_cfg_default.priorx             = hal_int_priority06;
    my_can_cfg_default.priotx             = hal_int_priority06;
    my_can_cfg_default.callback_on_rx     = s_can_callbkb_onrec;
    my_can_cfg_default.arg_cb_rx          = NULL;
    my_can_cfg_default.callback_on_tx     = NULL;
    my_can_cfg_default.arg_cb_tx          = NULL;
    my_can_cfg_default.callback_on_err    = s_can_callbkp_can1_onerr;
    my_can_cfg_default.arg_cb_err         = NULL;



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
    canframe.size = 8;
    

    res = hal_can_init(hal_can_port1, &my_can_cfg_default);

    if(hal_res_OK != res)
    {
        return;
    }


    my_can_cfg_default.callback_on_rx     = s_can_callbkb_onrec2;
    my_can_cfg_default.callback_on_err    = s_can_callbkp_can2_onerr;

    res = hal_can_init(hal_can_port2, &my_can_cfg_default);

    if(hal_res_OK != res)
    {
        return;
    }
    

//    hal_can_receptionfilter_set(hal_can_port1, 0, 0xFF, 0, 0x1E, hal_can_frameID_std);
//    hal_can_receptionfilter_set(hal_can_port2, 0, 0xFF, 0, 0x2E, hal_can_frameID_std);


//    hal_can_receptionfilter_set(hal_can_port1, 0, 0xFF, 0, 0x1E, hal_can_frameID_std);

    res = hal_can_enable(hal_can_port1);

    if(hal_res_OK != res)
    {
        return;
    }

     res = hal_can_enable(hal_can_port2);
    if(hal_res_OK != res)
    {
        return;
    }



//     canframe.size = 8;
//    for(i=0; i<5; i++)
//    {
//        canframe.data[0] = i;
//        canframe.id = id1;
//        hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_later);
//        canframe.id = id2; 
//        hal_can_put(hal_can_port2, &canframe, hal_can_send_normprio_later);
//    }
//    canframe.data[0] = 0xFF;
//    canframe.data[1] = 0xFF;
//    canframe.data[2] = 0xFF;
//    canframe.data[3] = 0xFF;
//    canframe.data[4] = 0xFF;
//    canframe.data[5] = 0xFF;
//    canframe.data[6] = 0xFF;
//    canframe.data[7] = 0xFF;
//    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now);
//    hal_can_put(hal_can_port2, &canframe, hal_can_send_normprio_now);
//    while(1)
//    {;}
//
//    canframe.id = id1;
//
//    canframe.size = 8;
//    canframe.data[0] = 5;
//    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_later);
//
//    canframe.size = 8;
//    canframe.data[0] = 6;
//    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now);
//
//
//    canframe.size = 8;
//    canframe.data[0] = 7;
//    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_later);
//
//    canframe.size = 8;
//    canframe.data[0] = 8;
//    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now);
//
//    canframe.size = 8;
//    canframe.data[0] = 9;
//    hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_later);

//     while(1)
//     {

//         if(rec_msg2)
//         {
//             rec_msg2 = 0;
//     
//             res = hal_can_get(hal_can_port2, &canframe, NULL);
//         
//             if(hal_res_OK == res)
//             {
//                 canframe.data[0] = 0xa1;
//                 hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_later);
//                 canframe.data[0] = 0xa2;
//                 hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_later);
//                 canframe.data[0] = 0xa3;
//                 hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_later);
//                 canframe.data[0] = 0xa4;
//                 hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_later);
//                 canframe.data[0] = 0xa5;
//                 hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now);


//             }
//         }
// //        count = 0;
//     
//     }

/*    while(1)
    {
        for(count = 0; count <10; count ++)
        {
//            canframe.data[0] = 0xFF;
//            hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now);
//            canframe.data[0] = count;
//            hal_can_put(hal_can_port2, &canframe, hal_can_send_normprio_now);
            if(rec_msg1)
            {
                s_can_onrec(hal_can_port1);
            }
            if(rec_msg2)
            {
                s_can_onrec(hal_can_port2);
            }
        }
        Delay(1000);
//        count = 0;
    
    }
    */
    
    while(1)
    {
//         for(count = 0; count <700; count ++)
//         {
//            canframe.data[0] = count;
//            while(hal_res_NOK_busy == hal_can_put(hal_can_port1, &canframe, hal_can_send_normprio_now))
//            {a = a;}
//             
//           hal_can_put(hal_can_port2, &canframe, hal_can_send_normprio_now);
//         }
     #include "stm32f10x_can.h"   
    CanTxMsg TxMessage =
    {
        .IDE   = CAN_ID_STD,     //only stdid are managed
        .ExtId = 0,              // since frame-id is std it is not used by stm32lib
        .RTR   = CAN_RTR_DATA   //only data frame are managed    
    };

        for(count = 0; count <1000; count ++)
        {
        TxMessage.StdId = canframe.id & 0x7FF;
        TxMessage.DLC = canframe.size;
        *(uint64_t*)TxMessage.Data = *((uint64_t*)&canframe.data);

       	while(CAN_NO_MB == CAN_Transmit( CAN1, &TxMessage))
        {
            a=a;
        }
        //CAN_Transmit( CAN1, &TxMessage);
        }
        hal_can_getstatus(hal_can_port1, &status1);
        hal_can_getstatus(hal_can_port1, &status2);
        //s_can_printstatus(hal_can_port1, &status);
         while(1);
    }

}
#endif

static void s_test_hal_basic(void)
{

    uint32_t data04[7];

    uint8_t data01[8];
 
    hal_sys_systick_sethandler(s_tick, 1000*1000, hal_int_priority00);

    LED_Config();
    LED_On(0);
    LED_Off(0);

    LED_On(0);

    hal_gpio_getval(hal_gpio_portE, hal_gpio_pin8);

	for(;;)
    {
		Delay(1000);
		LED_On(0);
 		Delay(1000);
 		LED_Off(0);
    }


//TODO
//    hal_flash_setlatency(hal_flash_1cycles);
//
//
//    hal_flash_unlock();
//
//    hal_flash_erasepage(0x08020003);
//    hal_flash_writeword(0x08020000, 0x12345678);
//    hal_flash_writehalfword(0x08020007, 0x1234);
//
//    hal_flash_writedata(0x08020010, sizeof(data04), data04);
//
//    hal_flash_writedata(0x08020040, sizeof(data01), (uint32_t*)data01);
}


static void s_test_hal_eth(void)
{
    static hal_eth_cfg_t cfg;
    static hal_eth_onframereception_t frx;
    //static uint8_t mac[6] = {0, 1, 2, 3, 4, 5};
    static hal_eth_frame_t *pframe;
    static uint32_t datafr[256] = {255};
    pframe = (hal_eth_frame_t*) &datafr[0];
    const hal_switch_cfg_t switch_cfg = {.dummy = 0};
    hal_led_cfg_t ledcfg = {.dummy=0};

    hal_led_init(hal_led0, &ledcfg);
    hal_led_off(hal_led0);
    hal_led_init(hal_led1, &ledcfg); //led green
    hal_led_off(hal_led1);
    hal_led_init(hal_led2, &ledcfg);
    hal_led_off(hal_led2);
    hal_led_init(hal_led3, &ledcfg);
    hal_led_off(hal_led3);


    frx.frame_new                   = s_getframe;
    frx.frame_movetohigherlayer     = s_giveframeback;
    frx.frame_alerthigherlayer      = s_alert_higherlevel;

    pframe->length = 256;
    pframe->index  = 0;
//    frame.data   = datafr;


    cfg.onframerx           = &frx;
    cfg.macaddress          = hal_ETH_macaddress(0, 1, 2, 3, 4, 5);


    hal_switch_init(&switch_cfg);

    hal_eth_init(&cfg);
#ifdef _CHECK_SWITCH_STATUS_
    enacheckswitch = 1;
#endif   
    
    //    hal_eth_enable();
//    
//    hal_eth_sendframe(pframe);
//    hal_eth_enable(); 

    for(;;)
    {
        pframe= pframe;
    }   
}



static void Delay(uint32_t dlyTicks) 
{
    uint32_t curTicks;

    curTicks = msTicks;
    while((msTicks - curTicks) < dlyTicks);
}

#ifdef _TEST_SPI_
/*
static void encoder0_cbk(void* p)
{
    static uint32_t mio_risultato=0xAABBCC;
    
    hal_encoder_get_value(hal_encoder6, &mio_risultato);
}


static void encoder1_cbk(void* p)
{
    static uint32_t mio_risultato=0;
    
    hal_encoder_get_value(hal_encoder7, &mio_risultato );
}


static void encoder2_cbk(void* p)
{
    static uint32_t mio_risultato=0;
    
    hal_encoder_get_value(hal_encoder8, &mio_risultato );
}

static void encoder6_cbk(void* p)
{
    static uint32_t mio_risultato=0xAABBCC;
    
    hal_encoder_get_value(hal_encoder6, &mio_risultato);
}


static void encoder7_cbk(void* p)
{
    static uint32_t mio_risultato=0;
    
    hal_encoder_get_value(hal_encoder7, &mio_risultato );
}


static void encoder8_cbk(void* p)
{
    static uint32_t mio_risultato=0;
    
    hal_encoder_get_value(hal_encoder8, &mio_risultato );
}
*/



static void encoder1_cbk(void* p)
{
    hal_encoder_read_start(hal_encoder2);
}


static void encoder2_cbk(void* p)
{
    hal_encoder_read_start(hal_encoder3);
}


static void encoder3_cbk(void* p)
{
    LED_OFF;
    spi_read_finish = 1; 
}

static void encoder7_cbk(void* p)
{
    hal_encoder_read_start(hal_encoder8);
}


static void encoder8_cbk(void* p)
{
    hal_encoder_read_start(hal_encoder9);
}


static void encoder9_cbk(void* p)
{
//in lettura sequenziale
//    LED_OFF;
//spi_read_finish = 3;

//in lettura parallela    
    spi3_read_finish = 1; 

}




static void s_test_hal_spi()
{
    static uint32_t res;
    uint32_t i;
    static uint32_t all_res[hal_encoders_num];


    hal_encoder_cfg_t enc_cfg1 = 
    {   
        .callback_on_rx     = encoder1_cbk,
        .arg                = NULL
    };

    hal_encoder_cfg_t enc_cfg2 = 
    {   
        .callback_on_rx     = encoder2_cbk,
        .arg                = NULL
    };

    hal_encoder_cfg_t enc_cfg3 = 
    {   
        .callback_on_rx     = encoder3_cbk,
        .arg                = NULL
    };

        hal_encoder_cfg_t enc_cfg7 = 
    {   
        .callback_on_rx     = encoder7_cbk,
        .arg                = NULL
    };

    hal_encoder_cfg_t enc_cfg8 = 
    {   
        .callback_on_rx     = encoder8_cbk,
        .arg                = NULL
    };

    hal_encoder_cfg_t enc_cfg9 = 
    {   
        .callback_on_rx     = encoder9_cbk,
        .arg                = NULL
    };
 

    memset(all_res, 0, hal_encoders_num*sizeof(uint32_t));

    //init GPIO_pin per test temporizzazioni

    RCC->APB2ENR |= 0x0000008;  //abilito clock su porta B
    GPIOB->CRH &= 0xFFFFFF0F;
    GPIOB->CRH |= 0xFFFFFF2F;
    
    LED_OFF; 


     // encoder0 is on spi1, mux0
    hal_encoder_init(hal_encoder1, &enc_cfg1);   
    // encoder1 is on spi1, mux1
    hal_encoder_init(hal_encoder2, &enc_cfg2);
    // encoder2 is on spi1, mux2
    hal_encoder_init(hal_encoder3, &enc_cfg3);
     // encoder7 is on spi3, mux0
    hal_encoder_init(hal_encoder7, &enc_cfg7);   
    // encoder8 is on spi3, mux1
    hal_encoder_init(hal_encoder8, &enc_cfg8);
    // encoder9 is on spi3, mux2
    hal_encoder_init(hal_encoder9, &enc_cfg9);
 
/*
    hal_encoder_read_block(hal_encoder1, &res);

    hal_encoder_read_block(hal_encoder2, &res);

    hal_encoder_read_block(hal_encoder3, &res);

    
    hal_encoder_read_block(hal_encoder7, &res);

    hal_encoder_read_block(hal_encoder8, &res);

    hal_encoder_read_block(hal_encoder9, &res);
*/  

//---- Lettura in sequenza dei sei ancoder. -----
/*


    LED_ON;  
    hal_encoder_read_start(hal_encoder1);

    while(1)
    {
        if(1 == spi_read_finish)
        {
//            LED_ON;
            hal_encoder_read_start(hal_encoder7); 
            spi_read_finish = 0;       
        }
        if(3 == spi_read_finish)
        {
            for(i=0; i<hal_encoders_num; i++)
            {
                hal_encoder_get_value((hal_encoder_t)i, &all_res[(hal_encoder_t)i]);
            }   
            while(1)
            {;}
        }
    
    }
*/


//---- Lettura in parallelo dei sei encoder. -----



    LED_ON;  
    hal_encoder_read_start(hal_encoder1);
    hal_encoder_read_start(hal_encoder7); 

    while(1)
    {
        if((1 == spi_read_finish) && (1 == spi3_read_finish))
        {
            for(i=0; i<hal_encoders_num; i++)
            {
                hal_encoder_get_value((hal_encoder_t)i, &all_res[(hal_encoder_t)i]);
            }   
            while(1)
            {;}
        }
    
    }




//while(1)
//{
// i = 500;
// hal_spi_encoder_read_start(hal_spi_port3, hal_spiEncoder_3);
// while(i>0)
// {i--;}
// res = 0;
// res = hal_spi_encoder_read_block(hal_spi_port3, hal_spiEncoder_1);
//}

}
#endif

static void LED_Config(void) 
{
    // assume port E. the LED is led 8
    hal_gpio_init(hal_gpio_portE, hal_gpio_pin8, hal_gpio_dirOUT, hal_gpio_speed_low);
	hal_gpio_init(hal_gpio_portE, hal_gpio_pin10, hal_gpio_dirOUT, hal_gpio_speed_low);
	hal_gpio_init(hal_gpio_portE, hal_gpio_pin12, hal_gpio_dirOUT, hal_gpio_speed_low);
	hal_gpio_init(hal_gpio_portE, hal_gpio_pin15, hal_gpio_dirOUT, hal_gpio_speed_low);

}


static void LED_On(uint32_t led) 
{
    hal_gpio_setval(hal_gpio_portE, hal_gpio_pin8, hal_gpio_valLOW);
	hal_gpio_setval(hal_gpio_portE, hal_gpio_pin10, hal_gpio_valLOW);
	hal_gpio_setval(hal_gpio_portE, hal_gpio_pin12, hal_gpio_valLOW);
	hal_gpio_setval(hal_gpio_portE, hal_gpio_pin15, hal_gpio_valLOW); 
}


static void LED_Off(uint32_t led) 
{
    hal_gpio_setval(hal_gpio_portE, hal_gpio_pin8, hal_gpio_valHIGH);
	hal_gpio_setval(hal_gpio_portE, hal_gpio_pin10, hal_gpio_valHIGH);
	hal_gpio_setval(hal_gpio_portE, hal_gpio_pin12, hal_gpio_valHIGH);
	hal_gpio_setval(hal_gpio_portE, hal_gpio_pin15, hal_gpio_valHIGH);  
}


static void s_tick(void)
{
    msTicks++;
}
#ifdef _CHECK_SWITCH_STATUS_
static void s_check_switch(void)
{
/*
//these types will be defined in hal_switch.h asap
typedef struct
{
    uint32_t autoNeg_done:1;
    uint32_t linkisgood:1;
    uint32_t linkspeed:1; // 1== 100Mb 0==10Mb
    uint32_t linkduplex:1; //1==full 0==half
    uint32_t dummy:28;
} hal_switch_portstatus;

typedef struct
{
    hal_switch_portstatus port1;
    hal_switch_portstatus port2;
} hal_switch_status;
*/
    static hal_switch_status status = {0};
    count++;
    
    if( count < 100)
    {
        return;
    }
    count = 0;
    if(enacheckswitch == 0)
    {
        return;
    }        

#define emsappl_ledred          hal_led0
#define emsappl_ledgreen        hal_led1 
#define emsappl_ledyellow       hal_led2
#define emsappl_ledorange       hal_led3

    hal_switch_getStatus(&status);

    if(status.port2.autoNeg_done)
    {
        hal_led_on(emsappl_ledgreen);
    }
    else
    {
        hal_led_off(emsappl_ledgreen);
    }
    
    if(status.port2.linkspeed)
    {
        hal_led_on(emsappl_ledred);
    }
    else
    {
        hal_led_off(emsappl_ledred);
    }
    
    
    if(status.port2.linkduplex)
    {
        hal_led_on(emsappl_ledyellow);
    }
    else
    {
        hal_led_off(emsappl_ledyellow);
    }

    if(status.port2.linkisgood)
    {
        hal_led_on(emsappl_ledorange);
    }
    else
    {
        hal_led_off(emsappl_ledorange);
    }

}
#endif

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
    static uint8_t buffer[200] = {0};
    /* Initialize the I2C EEPROM driver ----------------------------------------*/
    hal_eeprom_init(hal_eeprom_i2c_01, NULL); 
  
//    hal_eeprom_erase(hal_eeprom_i2c_01, 0, 2*1024); 

    memset(buffer, 0, 200);
    hal_eeprom_read(hal_eeprom_i2c_01, 0, 200, buffer);

    memset(buffer, 1, 200);
    hal_eeprom_write(hal_eeprom_i2c_01, 0, 200, buffer);

    memset(buffer, 0, 200);
    hal_eeprom_read(hal_eeprom_i2c_01, 0, 200, buffer);

//    hal_eeprom_erase(hal_eeprom_i2c_01, 0, 64*1024);

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
  //hal_eeprom_waitstandbystate();

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

//  while (1)
//  {
//  }

}
#endif //_TEST_EEPROM_


#ifdef _TEST_SW_INTERRUPT_
//configure extenal line 0 su PA0 pin in order to generate an sw interrurpt
void EXTI0_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;
  
  /* Enable GPIOA clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

  /* Configure PA0 pin as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

 
  /* Connect EXTI0 Line to PA0 pin */
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
    
    

  /* Configure EXTI0 line */
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI0 Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  
  NVIC_Init(&NVIC_InitStructure);
}





static uint32_t sw_interrupt = 0;
static void s_test_sw_interrupt(void)
{
    
     uint32_t i;
     EXTI0_Config();
    while(1)
    {
        for(i = 0; i< 2000; i++)
        {
            __nop();
        }
        EXTI_GenerateSWInterrupt(EXTI_Line0);
    }
}

void EXTI0_IRQHandler(void)
{

    sw_interrupt++;
    EXTI_ClearITPendingBit(EXTI_Line0);

    
    
}  
    
    

#endif
