
/* @file       eos-test-main.c
    @brief      This file implements a test for embobj with the use of abslayer under singletask environment
    @author     marco.accame@iit.it
    @date       04/07/2011
**/

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stdint.h" 
#include "stdlib.h"
#include "p33fxxxx.h"
#include "string.h"
#include <libpic30.h>
#include <timer.h>

// hal dspic33f
#include "hal_can.h"
#include "hal_system.h"
#include "hal_error.h"

// embobj-generic
#include "EOtheMemoryPool.h"
#include "EOtheErrormanager.h"
#include "EoCommon.h"


// embobj-singletask
#include "EOStheSystem.h"

#include "EOStheTimerManager.h"

#include "EOtimer.h"
#include "EOaction.h"


// test
//#include "test-eocontainers.h"
//#include "test-eogpio.h"
//#include "test-eosystem.h"
//#include "test-eostatemachine.h"







// --------------------------------------------------------------------------------------------------------------------
// - declaration of external variables 
// --------------------------------------------------------------------------------------------------------------------

//extern const hal_params_cfg_t *hal_params_cfgMINE;
//extern const fsal_params_cfg_t *fsal_params_cfgMINE;
 
// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern hidden interface 
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
//// CONFIGURATION BITS INITIALIZATION 
// ATTENTION!!! this code must be inserted before the beginning of your C source code

// no Boot sector and write protection disabled
_FBS (BSS_NO_FLASH & BWRP_WRPROTECT_OFF);

// no secure sector and write protection disaabled
_FSS (RSS_NO_RAM & SSS_NO_FLASH & SWRP_WRPROTECT_OFF);

// Code protection disabled
_FGS(GSS_OFF); 

// Clock switching disabled Fail safe Clock Monitor disabled
// External clock with PLL x8 (10MHz*8->Fcycle=80/4=20MIPS)
//_FOSCSEL(IESO_OFF & FNOSC_PRIPLL);  NON PARTO CON PLL
_FOSCSEL(IESO_OFF & FNOSC_FRC);

// Clock switching and clock monitor disabled, EC external clock, OSCOUT as IOPIN
//_FOSC(FCKSM_CSDCMD & POSCMD_EC & OSCIOFNC_ON); NON PARTO CON PLL
_FOSC(FCKSM_CSECMD & POSCMD_EC & OSCIOFNC_ON)

// Turn off Watchdog Timer
_FWDT (FWDTEN_OFF);
//_FWDT(WDT_ON & WDTPSA_512 & WDTPSB_1); // WD enabled 1:512*16

// PWM mode is Port registers PWM high & low active high
// alternate I2C mapped to SDA1/SCL1
// FPOR power on reset 128ms
_FPOR (PWMPIN_ON & HPOL_ON & LPOL_ON & FPWRT_PWR128);

// Use PGC3/PGD3 for programming and debugging
_FICD (ICS_PGD3 & JTAGEN_OFF); // & COE_ON ); //BKBUG_OFF



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------

//extern const eOmempool_cfg_t mempool_cfg = 
//{
//    eo_mempool_alloc_dynamic,
//    0, NULL,
//    0, NULL,
//    0, NULL,
//    0, NULL
//};

static /*const */ hal_sys_config_t s_hal_syscfg =
{
    .osc = 
    {
        .pllin = 10,  
    }
};





// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------



void s_eos_test_init(void *p, uint32_t u);
static void s_eos_test_initialise_hal(void);
static void s_eos_test_initialise_fsal(void);
static void s_eos_test_nomemory_anymore(void);

static void s_eos_test_on_event(uint32_t evtpos, eOevent_t evtmsk);
static void s_eos_test_on_message(eOmessage_t msg);

static void s_eos_test_on_tick(void);

static void s_eos_test_callback10(void);

static void s_errman_OnError(eOerrmanErrorType_t errtype, uint32_t taskid, const char *eobjstr, const char *info);



// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------


static const eOerrman_fn_cfg_t errman_cfg =
{
    .usr_on_error   = s_errman_OnError
};


static eOssystem_cfg_t s_syscfg =
{   
    .mempoolcfg                         = NULL,
    .errmancfg                          = NULL, //(eOerrman_cfg_t*)&errman_cfg,
//    .onstartup_fn                       = s_eos_test_init,
//    .ontick_fn                          = s_eos_test_on_tick,
//    .onevent_fn                         = s_eos_test_on_event,
//    .onmessage_fn                       = s_eos_test_on_message,
    .messagefifosize                    = 0,//4,
    .callbackfifosize                   = 2,//4,
    .systickperiod                      = 200,
    .usrfn                              =
    {
        .on_startup                         = s_eos_test_init,
        .on_systick                         = s_eos_test_on_tick,
        .on_event                           = s_eos_test_on_event,
        .on_message                         = NULL,//s_eos_test_on_message
    },
    .envfn                              =
    {
        .hal_sys_systick_sethandler         = (eOvoid_fp_voidfpvoiduint32_t)hal_sys_systick_sethandler,
        .hal_sys_atomic_bitwiseAND          = hal_sys_atomic_bitwiseAND,
        .hal_sys_atomic_bitwiseOR           = hal_sys_atomic_bitwiseOR,
        .hal_sys_criticalsection_take       = (eOres_fp_voidp_uint32_t)hal_sys_criticalsection_take,
        .hal_sys_criticalsection_release    = (eOres_fp_voidp_t)hal_sys_criticalsection_release
    }
};


static EOtimer *s_eos_test_mytmr05 = NULL;
static EOtimer *s_eos_test_mytmr10 = NULL;
static EOtimer *s_eos_test_mytmr02 = NULL;

static EOaction *s_eos_test_myact = NULL;


// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------



int main(void)
{

    
    eos_sys_Initialise(&s_syscfg);  
    
    eos_sys_Start(eos_sys_GetHandle());


    return(0);
}



// --------------------------------------------------------------------------------------------------------------------
// - definition of extern hidden functions 
// --------------------------------------------------------------------------------------------------------------------
// empty-section

  


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


void s_eos_test_init(void *p, uint32_t u)
{
    s_eos_test_initialise_hal();
    s_eos_test_initialise_fsal();
    hal_sys_init(&s_hal_syscfg); 

    eos_timerman_Initialise(NULL);

    s_eos_test_myact = eo_action_New();
    s_eos_test_mytmr10 = eo_timer_New();
    s_eos_test_mytmr02 = eo_timer_New();
    s_eos_test_mytmr05 = eo_timer_New();


    eo_action_SetCallback(s_eos_test_myact, s_eos_test_callback10, eos_sys_GetHandle());
    eo_timer_Start(s_eos_test_mytmr10, eo_lifetimeNOSINCHRO, 1000, eo_tmrmode_FOREVER, s_eos_test_myact);

//    eo_action_SetMessage(s_eos_test_myact, 0x10000002, eos_sys_GetHandle());
//    eo_timer_Start(s_eos_test_mytmr02, eo_lifetimeNOSINCHRO, 2*1000, eo_tmrmode_FOREVER, s_eos_test_myact);
//
//    eo_action_SetMessage(s_eos_test_myact, 0x10000005, eos_sys_GetHandle());
//    eo_timer_Start(s_eos_test_mytmr05, eo_lifetimeNOSINCHRO, 5*1000, eo_tmrmode_FOREVER, s_eos_test_myact);


    //printf("startup() just called\n");
}

// give hal memory, initialise sys, leds of mcbstm32c all off, display 
static void s_eos_test_initialise_hal(void)
{
//    uint32_t ram04size = 0;
//    uint32_t *ram04data = NULL;
//    
//    hal_memory_getsize(hal_params_cfgMINE, &ram04size);
//    
//    if(0 != ram04size)
//    {
//        ram04data = (uint32_t*)calloc(ram04size/4, sizeof(uint32_t));
//        
//        if(NULL == ram04data)
//        {
//            s_eos_test_nomemory_anymore();
//        }
//    }
//
//    hal_initialise(hal_params_cfgMINE, ram04data);
//    
//    // system
//    hal_sys_systeminit();
//
//    // gpio out: leds
//    hal_gpio_init(hal_gpio_portE, hal_gpio_pin8,  hal_gpio_dirOUT, hal_gpio_speed_2MHz);
//    hal_gpio_init(hal_gpio_portE, hal_gpio_pin9,  hal_gpio_dirOUT, hal_gpio_speed_2MHz);
//    hal_gpio_init(hal_gpio_portE, hal_gpio_pin10, hal_gpio_dirOUT, hal_gpio_speed_2MHz);
//    hal_gpio_init(hal_gpio_portE, hal_gpio_pin11, hal_gpio_dirOUT, hal_gpio_speed_2MHz);
//    hal_gpio_init(hal_gpio_portE, hal_gpio_pin12, hal_gpio_dirOUT, hal_gpio_speed_2MHz);
//    hal_gpio_init(hal_gpio_portE, hal_gpio_pin13, hal_gpio_dirOUT, hal_gpio_speed_2MHz);
//    hal_gpio_init(hal_gpio_portE, hal_gpio_pin14, hal_gpio_dirOUT, hal_gpio_speed_2MHz);
//    hal_gpio_init(hal_gpio_portE, hal_gpio_pin15, hal_gpio_dirOUT, hal_gpio_speed_2MHz);
//    // gpio in: button user
//    hal_gpio_init(hal_gpio_portB, hal_gpio_pin7, hal_gpio_dirINP, hal_gpio_speed_2MHz);
//    // led8 on
//    hal_gpio_setval(hal_gpio_portE, hal_gpio_pin8,  hal_gpio_valHIGH);
//    
//    // display
//    hal_display_init();
//    hal_display_clear(hal_col_white);
//    hal_display_settext(hal_font_24x16, hal_col_darkgrey, hal_col_yellow);
//    hal_display_putstring(1, "testing embobj");    
//    
}


//// give fsal memory, format if there isnt file acemor.sig, printf on itm 
static void s_eos_test_initialise_fsal(void)
{
//    uint32_t ram04size = 0;
//    uint32_t *ram04data = NULL;
//    fsal_info_search_t infosearch;
//    FILE *fp = NULL;
//    const uint32_t data[] = {0, 0, 7};
//    uint32_t len = 0;
//    
//    fsal_memory_getsize(fsal_params_cfgMINE, &ram04size);
//
//    
//    if(0 != ram04size)
//    {
//        ram04data = (uint32_t*)calloc(ram04size/4, sizeof(uint32_t));
//        
//        if(NULL == ram04data)
//        {
//            s_eos_test_nomemory_anymore();
//        }
//    }
//
//    fsal_initialise(fsal_params_cfgMINE, ram04data);
//
//    printf("fsal initialisation done\n");
//    printf("used: stdio_enable = %d and eflash_enable = %d\n", fsal_params_cfgMINE->stdio_enable, 
//                                                               fsal_params_cfgMINE->eflash_enable);
//
//
//    if(1 == fsal_params_cfgMINE->eflash_enable)
//    {
//        infosearch.fileid = 0;
//
//        if(fsal_res_NOK_generic == fsal_find("F:acemor.sig", &infosearch))
//        {
//            fsal_format(fsal_drive_flash);
//            printf("formatted the drive\n");
//    
//            fp = fopen("F:acemor.sig", "w");
//            if(NULL != fp)
//            {
//                len = fwrite(data, 1, sizeof(data), fp);
//                fclose(fp);
//                printf("created file F:acemor.sig. it has %d bytes\n", len);
//            }
//            else
//            {
//                printf("failed to create file acemor.sig\n");
//            }
//    
//        }
//        else
//        {
//            fsal_defrag(fsal_drive_flash);
//            printf("file F:acemor.sig is already present. just defragged the drive\n");
//        }
//    }
//   
  
}





static void s_errman_OnError(eOerrmanErrorType_t errtype, uint32_t taskid, const char *eobjstr, const char *info)
{

    for(;;);
}


static void s_eos_test_nomemory_anymore(void)
{
    for(;;);
}


static void s_eos_test_on_event(uint32_t evtpos, eOevent_t evtmsk)
{
    static uint32_t aaa = 0;
    static uint32_t bbb = 0;

    aaa = evtpos;
    aaa = aaa;
    bbb = evtmsk;
    bbb = bbb;

    if(0 == evtpos)
    {
        eos_timerman_Tick(eos_timerman_GetHandle());
    }
}

static void s_eos_test_on_message(eOmessage_t msg)
{
    static uint32_t ccc = 0;
    static uint64_t ntick = 0;
    
    ntick = eos_sys_LifeTimeGet(eos_sys_GetHandle()); 
    ntick = ntick;
 
    ccc = msg;
    ccc = ccc;
}

static void s_eos_test_on_tick(void)
{
    uint64_t ntick = eos_sys_LifeTimeGet(eos_sys_GetHandle());

    eos_sys_SetEvent(eos_sys_GetHandle(), 0x00000001);

    if(9 == (ntick%10))
    {
        eos_sys_SetEvent(eos_sys_GetHandle(), 0x00000002);
//        eos_sys_SendMessage(eos_sys_GetHandle(), 0x000000FF);
//        eos_sys_SendMessage(eos_sys_GetHandle(), 0x000000FE);
    }
}

static void s_eos_test_callback10(void)
{
    static uint64_t tt = 0;

    tt = eos_sys_LifeTimeGet(eos_sys_GetHandle());

    tt = tt;
}



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------



