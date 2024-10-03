#if 1

/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_sys.h"
#include "embot_hw_led.h"



// in here we run the baremetal embot::hw application.
// for the required get1microtime() we can either use a very naked approach or even the systick

#define USE_SYSTICK_AS_TIME_BASE

#if defined(USE_SYSTICK_AS_TIME_BASE)

#include "stm32hal.h"

static volatile uint64_t s_1mstickcount = 0; // it must be volatile
constexpr uint32_t s_rate1khz = 1000;
#ifdef __cplusplus
extern "C" {
#endif
void SysTick_Handler(void)
{
    s_1mstickcount++;
}
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

static void stm32hal_tick1msecinit()
{
    HAL_SYSTICK_Config(SystemCoreClock/s_rate1khz);
}

static uint32_t stm32hal_tick1msecget()
{
    return s_1mstickcount;
}

embot::core::Time get1microtime1()
{
    return embot::core::time1millisec * stm32hal_tick1msecget();
}

constexpr embot::hw::Config hwCFG {stm32hal_tick1msecinit, get1microtime1};

#else

embot::core::Time get1microtime2()
{
    static constexpr embot::core::relTime resolution{10};
    // wait some usec and then returns the value
    static volatile uint64_t cnt = 0;
    embot::hw::sys::delay(resolution); 
    cnt += resolution;    
    return cnt;
}

constexpr embot::hw::Config hwCFG {nullptr, get1microtime2};

#endif // USE_SYSTICK_AS_TIME_BASE


// principal debug macros



// if defined it forces tge eeprom to have def2run = application, so that the eupdater will jump to application 
#undef DEBUG_forceEEPROM_DEF2RUNequalAPPLICATION
// if defined it does not jump and forces execution of defaultapplication() 
#undef DEBUG_stayinhere
// if defined it enables the CM4 core after clock initialization inside embot::hw::init()
#undef DEBUG_startCMxnow
#undef DEBUG_eraseEEPROM

#if defined(CM7launcher)
#define DEBUG_startCMxnow
#define DEBUG_stayinhere
constexpr embot::core::relTime applblinkrate {250*embot::core::time1millisec};
#else
constexpr embot::core::relTime applblinkrate {100*embot::core::time1millisec};
#endif

#warning evaluate if to use this very same code or file for the amc 

// used functions

void thejumper(); 

[[noreturn]] void defaultapplication(embot::core::relTime blinkrate = applblinkrate);


#include "embot_hw_dualcore.h"


#if defined(DEBUG_startCMxnow)
constexpr embot::hw::dualcore::Config dcc {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate };
#else
constexpr embot::hw::dualcore::Config dcc {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activateandhold };
#endif
    
int main(void)
{ 
    // hw init
    embot::hw::dualcore::config(dcc);
    embot::hw::init(hwCFG);
    
#if !defined(DEBUG_stayinhere) 
    // eval jump
    thejumper();
#endif    
    // run default application
    defaultapplication();         
}


[[noreturn]] void defaultapplication(embot::core::relTime blinkrate)
{        
    embot::hw::led::init(embot::hw::LED::one);
    for(;;)
    {
        embot::core::wait(blinkrate);        
        embot::hw::led::toggle(embot::hw::LED::one);
    }  
}


// - dependencies

//#if defined(DEBUG_stayinhere) 

//#else

#include "embot_hw_eeprom.h"

#include "eEmemorymap.h"
#include "eEsharedServices.h" 
#include "EoBoards.h"


#if defined(dontuseMEMmapping)
constexpr eEmoduleExtendedInfo_t s_loader_info_extended =
#else
constexpr eEmoduleExtendedInfo_t s_loader_info_extended __attribute__((section(EENV_MODULEINFO_LOADER_AT))) =
#endif
{
    .moduleinfo     =
    {
        .info           =
        {
            .entity     =
            {
                .type       = ee_entity_process,
                .signature  = ee_procLoader,
                .version    = 
                { 
                    .major = 3, 
                    .minor = 2
                },  
                .builddate  = 
                {
                    .year  = 2022,
                    .month = 9,
                    .day   = 16,
                    .hour  = 9,
                    .min   = 15
                }
            },
            .rom        = 
            {   
                .addr   = EENV_MEMMAP_ELOADER_ROMADDR,
                .size   = EENV_MEMMAP_ELOADER_ROMSIZE
            },
            .ram        = 
            {   
                .addr   = EENV_MEMMAP_ELOADER_RAMADDR,
                .size   = EENV_MEMMAP_ELOADER_RAMSIZE
            },
            .storage    = 
            {
                .type   = ee_strg_none,
                .size   = 0,
                .addr   = 0
            },
            .communication  = ee_commtype_none,
            .name           = "eLoader"
        },
        .protocols  =
        {
            .udpprotversion  = { .major = 0, .minor = 0},
            .can1protversion = { .major = 0, .minor = 0},
            .can2protversion = { .major = 0, .minor = 0},
            .gtwprotversion  = { .major = 0, .minor = 0}
        },
        .extra      = {"EXT"}
    },
    .compilationdatetime    = __DATE__ " " __TIME__,
    .userdefined            = {0}
};



#if !defined(STM32HAL_BOARD_AMCFOC_2CM4)
    #error --> specify BOARD_name
#else
static eEboardInfo_t s_loader_boardinfo =                        
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = eobrd_amc,
            .version    = 
            { 
                .major = 1, 
                .minor = 1
            },  
            .builddate  = 
            {
                .year  = 2022,
                .month = 4,
                .day   = 1,
                .hour  = 19,
                .min   = 19
            }
        },
        .rom        = 
        {   
            .addr   = EENV_ROMSTART,
            .size   = EENV_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = EENV_RAMSTART,
            .size   = EENV_RAMSIZE
        },
        .storage    = 
        {
            .type   = ee_strg_eeprom,
            .size   = EENV_STGSIZE,
            .addr   = EENV_STGSTART
        },
        .communication  = ee_commtype_eth | ee_commtype_can1 | ee_commtype_can2,
        .name           = "amc"
    },
    .uniqueid       = 0,
    .extra          = {0}
};

#endif


static void s_loader_shared_services_init(void);
static void s_loader_manage_error(embot::core::relTime rate = 100*embot::core::time1millisec);
static void s_loader_exec_loader(void);
static void s_loader_eval_jump_request_from_an_eproc(void);
static void s_loader_attempt_jump(eEprocess_t proc, uint32_t adr_in_case_proc_fails);

constexpr uint32_t LOADER_ADR_INVALID {0xffffffff};


void thejumper()
{
     
    s_loader_shared_services_init();
    
//#if defined(DEBUG_eraseEEPROM)    
//    ee_sharserv_sys_storage_reset();
//    s_loader_shared_services_init();
//#endif     
    
    s_loader_eval_jump_request_from_an_eproc();
    
    eEprocess_t startup = ee_procNone;
    eEresult_t eeres = ee_sharserv_part_proc_startup_get(&startup);
    if(ee_res_NOK_generic == eeres)
    {
        startup = ee_procUpdater;
    }
    s_loader_attempt_jump(startup, LOADER_ADR_INVALID);
    
    // if it fails a jump to startup ... do a last attempt to jump to eUpdater
    s_loader_attempt_jump(ee_procUpdater, LOADER_ADR_INVALID);

    // if we are in here we cannot jump to the startup and not even to the updater.
    s_loader_manage_error();        
}


// - static functions

static void s_loader_manage_error(embot::core::relTime rate)
{
    defaultapplication(rate);
}

static void s_on_sharserv_error(void)
{
    s_loader_manage_error();
}

static void s_loader_shared_services_init(void)
{

    eEprocess_t defproc = ee_procNone;
    eEprocess_t startup = ee_procNone;
    
    sharserv_mode_t sharservmode = 
    {
        .onerror    = s_on_sharserv_error,
        .initmode   = sharserv_base_initmode_forcestorageinit
    };
    
    if(ee_res_OK != ee_sharserv_isvalid())
    {
        s_loader_manage_error();
    }
    else
    {
        // init sharserv
        if(ee_res_OK != ee_sharserv_init(&sharservmode))
        {
            s_loader_manage_error();
        }        
    }
    
    // now all are initted. then ...

    // put signature in partition table
    if(ee_res_OK != ee_sharserv_part_proc_synchronise(ee_procLoader, (const eEmoduleInfo_t *)&s_loader_info_extended))
    {
        s_loader_manage_error();
    }    
    
    // impose boardinfo
    s_loader_boardinfo.uniqueid = embot::hw::sys::uniqueid();
    
    if(ee_res_OK != ee_sharserv_info_boardinfo_synchronise(&s_loader_boardinfo))
    {
        s_loader_manage_error();
    }    
       
    // impose startup process
    if(ee_res_OK != ee_sharserv_part_proc_startup_get(&startup))
    {
        // we impose that the startup process is the updater
        ee_sharserv_part_proc_startup_set(ee_procUpdater);
    }  
    
    // impose def2run process
    if(ee_res_OK != ee_sharserv_part_proc_def2run_get(&defproc))
    {
        // we impose that the default process is the updater     
        ee_sharserv_part_proc_def2run_set(ee_procUpdater);      
    } 

#if defined(DEBUG_forceEEPROM_DEF2RUNequalAPPLICATION)
    // we impose that the application is the def2run
    ee_sharserv_part_proc_def2run_set(ee_procApplication);
#endif    
}

// used to eval the jump request coming from another process
static void s_loader_eval_jump_request_from_an_eproc(void)
{
    eEprocess_t pr = ee_procNone;

    if(ee_res_OK == ee_sharserv_ipc_gotoproc_get(&pr))
    {
        ee_sharserv_ipc_gotoproc_clr();
        
        if(ee_procUpdater == pr)
        {   // we communicate to the updater to stay forever and not to jump to default after the 5 (or what) seconds
            ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
        }
        
        // attempt only to the requested process.
        s_loader_attempt_jump(pr, LOADER_ADR_INVALID);
        
        // if in here ... the jump failed, thus ... behave as if no order at all
        ee_sharserv_ipc_gotoproc_clr();
        
//        // if in here ... the jump failed, thus ... it is better to go to the updater and stay there forever
//        ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
//        s_loader_attempt_jump(ee_procUpdater, LOADER_ADR_INVALID);
        
    }
    
    uint32_t address2jump = 0;
    if(ee_res_OK == ee_sharserv_ipc_jump2addr_get(&address2jump))
    {
        ee_sharserv_ipc_jump2addr_clr();
           
        if(ee_res_OK == ee_sharserv_sys_canjump(address2jump))
        {
            ee_sharserv_sys_jumpnow(address2jump);
        }   
       
    }    
}

// used to attempt a jump to a process 
static void s_loader_attempt_jump(eEprocess_t proc, uint32_t adr_in_case_proc_fails)
{
    uint32_t address = LOADER_ADR_INVALID;

    if(ee_procNone != proc)
    {

        if(ee_procLoader == proc)
        {
            // avoid recursive bootstraps
            s_loader_exec_loader();
        }
     
        // attempt to get the address of the proc from partition table.
        // it works only if the process already run before and register itself in the partition table.
        // if it fails ... use brute force mode
        if(ee_res_NOK_generic == ee_sharserv_part_proc_runaddress_get(proc, &address))
        {
            switch(proc)
            {
                case ee_procUpdater:        address = EENV_MEMMAP_EUPDATER_ROMADDR;         break;
                case ee_procApplication:    address = EENV_MEMMAP_EAPPLICATION_ROMADDR;     break;
                default:                    address = LOADER_ADR_INVALID;                   break;
            }
        }
        
        // if we retrieve the address ... attempt to jump
        if(LOADER_ADR_INVALID != address)
        {
            if(ee_res_OK == ee_sharserv_sys_canjump(address))
            {
                ee_sharserv_sys_jumpnow(address);
            }                
        }        

    }
    
    // if in here it means the we could not jump to the address of proc, thus we attempt an alternative.
    if(LOADER_ADR_INVALID != adr_in_case_proc_fails)
    {
        // attempt with adr_in_case_proc_fails 
        if(ee_res_OK == ee_sharserv_sys_canjump(adr_in_case_proc_fails))
        {
            ee_sharserv_sys_jumpnow(adr_in_case_proc_fails);
        }
    }

    // if i am in here we cannot jump ...
    return;
}

// the default application in case a process ask to execute the loader
void s_loader_exec_loader(void)
{    
    defaultapplication();
}

//#endif


#else // #if 1

/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_sys.h"
#include "embot_hw_led.h"
#include "embot_hw_eeprom.h"
#include "eEmemorymap.h"

// in here we run the baremetal embot::hw application.
// for the required get1microtime() we can either use a very naked approach or even the systick

#define USE_SYSTICK_AS_TIME_BASE

#if defined(USE_SYSTICK_AS_TIME_BASE)

#include "stm32hal.h"

static volatile uint64_t s_1mstickcount = 0; // it must be volatile
constexpr uint32_t s_rate1khz = 1000;
#ifdef __cplusplus
extern "C" {
#endif
void SysTick_Handler(void)
{
    s_1mstickcount++;
}
#ifdef __cplusplus
}       // closing brace for extern "C"
#endif 

static void stm32hal_tick1msecinit()
{
    HAL_SYSTICK_Config(SystemCoreClock/s_rate1khz);
}

static uint32_t stm32hal_tick1msecget()
{
    return s_1mstickcount;
}

embot::core::Time get1microtime1()
{
    return embot::core::time1millisec * stm32hal_tick1msecget();
}

constexpr embot::hw::Config hwCFG {stm32hal_tick1msecinit, get1microtime1};

#else

embot::core::Time get1microtime2()
{
    static constexpr embot::core::relTime resolution{10};
    // wait some usec and then returns the value
    static volatile uint64_t cnt = 0;
    embot::hw::sys::delay(resolution); 
    cnt += resolution;    
    return cnt;
}

constexpr embot::hw::Config hwCFG {nullptr, get1microtime2};

#endif // USE_SYSTICK_AS_TIME_BASE


// principal debug macros

// if defined it forces tge eeprom to have def2run = application, so that the eupdater will jump to application 
#undef DEBUG_forceEEPROM_DEF2RUNequalAPPLICATION
// if defined it does not jump and forces execution of defaultapplication() 
#undef DEBUG_stayinhere
// if defined it enables the CM4 core after clock initialization inside embot::hw::init()
#undef DEBUG_startCMxnow

// used functions

void thejumper(); 

[[noreturn]] void defaultapplication(embot::core::relTime blinkrate = applblinkrate);


#include "embot_hw_dualcore.h"


#if defined(DEBUG_startCMxnow)
constexpr embot::hw::dualcore::Config dcc {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate };
#else
constexpr embot::hw::dualcore::Config dcc {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::donothing };
#endif
    
int main(void)
{ 
    // hw init
    embot::hw::dualcore::config(dcc);
    embot::hw::init(hwCFG);
#if !defined(DEBUG_stayinhere) 
    // eval jump
    thejumper();
#endif    
    // run default application
    defaultapplication();         
}


[[noreturn]] void defaultapplication(embot::core::relTime blinkrate)
{        
    embot::hw::led::init(embot::hw::LED::one);
    for(;;)
    {
        embot::core::wait(blinkrate);        
        embot::hw::led::toggle(embot::hw::LED::one);
    }  
}


// - dependencies

#include "eEmemorymap.h"
#include "eEsharedServices.h" 
#include "EoBoards.h"

#if defined(dontuseMEMmapping)
constexpr eEmoduleExtendedInfo_t s_loader_info_extended =
#else
constexpr eEmoduleExtendedInfo_t s_loader_info_extended __attribute__((section(EENV_MODULEINFO_LOADER_AT))) =
#endif
{
    .moduleinfo     =
    {
        .info           =
        {
            .entity     =
            {
                .type       = ee_entity_process,
                .signature  = ee_procLoader,
                .version    = 
                { 
                    .major = 3, 
                    .minor = 2
                },  
                .builddate  = 
                {
                    .year  = 2022,
                    .month = 9,
                    .day   = 16,
                    .hour  = 9,
                    .min   = 15
                }
            },
            .rom        = 
            {   
                .addr   = EENV_MEMMAP_ELOADER_ROMADDR,
                .size   = EENV_MEMMAP_ELOADER_ROMSIZE
            },
            .ram        = 
            {   
                .addr   = EENV_MEMMAP_ELOADER_RAMADDR,
                .size   = EENV_MEMMAP_ELOADER_RAMSIZE
            },
            .storage    = 
            {
                .type   = ee_strg_none,
                .size   = 0,
                .addr   = 0
            },
            .communication  = ee_commtype_none,
            .name           = "eLoader"
        },
        .protocols  =
        {
            .udpprotversion  = { .major = 0, .minor = 0},
            .can1protversion = { .major = 0, .minor = 0},
            .can2protversion = { .major = 0, .minor = 0},
            .gtwprotversion  = { .major = 0, .minor = 0}
        },
        .extra      = {"EXT"}
    },
    .compilationdatetime    = __DATE__ " " __TIME__,
    .userdefined            = {0}
};



#if !defined(STM32HAL_BOARD_AMC2CM4)
    #error --> specify BOARD_name
#else
static eEboardInfo_t s_loader_boardinfo =                        
{
    .info           =
    {
        .entity     =
        {
            .type       = ee_entity_board,
            .signature  = eobrd_amc,
            .version    = 
            { 
                .major = 1, 
                .minor = 1
            },  
            .builddate  = 
            {
                .year  = 2022,
                .month = 4,
                .day   = 1,
                .hour  = 19,
                .min   = 19
            }
        },
        .rom        = 
        {   
            .addr   = EENV_ROMSTART,
            .size   = EENV_ROMSIZE
        },
        .ram        = 
        {   
            .addr   = EENV_RAMSTART,
            .size   = EENV_RAMSIZE
        },
        .storage    = 
        {
            .type   = ee_strg_eeprom,
            .size   = EENV_STGSIZE,
            .addr   = EENV_STGSTART
        },
        .communication  = ee_commtype_eth | ee_commtype_can1 | ee_commtype_can2,
        .name           = "amc"
    },
    .uniqueid       = 0,
    .extra          = {0}
};

#endif


static void s_loader_shared_services_init(void);
static void s_loader_shared_services_init(void);
static void s_loader_manage_error(embot::core::relTime rate = 100*embot::core::time1millisec);
static void s_loader_exec_loader(void);
static void s_loader_eval_jump_request_from_an_eproc(void);
static void s_loader_attempt_jump(eEprocess_t proc, uint32_t adr_in_case_proc_fails);

constexpr uint32_t LOADER_ADR_INVALID {0xffffffff};


void thejumper()
{
#if defined(DEBUG_eraseEEPROM)    
    ee_sharserv_sys_storage_reset();
#endif    
    
    s_loader_shared_services_init();
    
    s_loader_eval_jump_request_from_an_eproc();
    
    eEprocess_t startup = ee_procNone;
    eEresult_t eeres = ee_sharserv_part_proc_startup_get(&startup);
    if(ee_res_NOK_generic == eeres)
    {
        startup = ee_procUpdater;
    }
    s_loader_attempt_jump(startup, LOADER_ADR_INVALID);
    
    // if it fails a jump to startup ... do a last attempt to jump to eUpdater
    s_loader_attempt_jump(ee_procUpdater, LOADER_ADR_INVALID);

    // if we are in here we cannot jump to the startup and not even to the updater.
    s_loader_manage_error();        
}


// - static functions

static void s_loader_manage_error(embot::core::relTime rate)
{
    defaultapplication(rate);
}

static void s_on_sharserv_error(void)
{
    s_loader_manage_error();
}

static void s_loader_shared_services_init(void)
{
    eEprocess_t defproc = ee_procNone;
    eEprocess_t startup = ee_procNone;
    
    sharserv_mode_t sharservmode = 
    {
        .onerror    = s_on_sharserv_error,
        .initmode   = sharserv_base_initmode_forcestorageinit
    };
    
    if(ee_res_OK != ee_sharserv_isvalid())
    {
        s_loader_manage_error();
    }
    else
    {
        // init sharserv
        if(ee_res_OK != ee_sharserv_init(&sharservmode))
        {
            s_loader_manage_error();
        }        
    }
    
    // now all are initted. then ...

    // put signature in partition table
    if(ee_res_OK != ee_sharserv_part_proc_synchronise(ee_procLoader, (const eEmoduleInfo_t *)&s_loader_info_extended))
    {
        s_loader_manage_error();
    }    
    
    // impose boardinfo
    s_loader_boardinfo.uniqueid = embot::hw::sys::uniqueid();
    
    if(ee_res_OK != ee_sharserv_info_boardinfo_synchronise(&s_loader_boardinfo))
    {
        s_loader_manage_error();
    }    
       
    // impose startup process
    if(ee_res_OK != ee_sharserv_part_proc_startup_get(&startup))
    {
        // we impose that the startup process is the updater
        ee_sharserv_part_proc_startup_set(ee_procUpdater);
    }  
    
    // impose def2run process
    if(ee_res_OK != ee_sharserv_part_proc_def2run_get(&defproc))
    {
        // we impose that the default process is the updater     
        ee_sharserv_part_proc_def2run_set(ee_procUpdater);      
    } 

#if defined(DEBUG_forceEEPROM_DEF2RUNequalAPPLICATION)
    // we impose that the application is the def2run
    ee_sharserv_part_proc_def2run_set(ee_procApplication);
#endif    
}

// used to eval the jump request coming from another process
static void s_loader_eval_jump_request_from_an_eproc(void)
{
    eEprocess_t pr = ee_procNone;

    if(ee_res_OK == ee_sharserv_ipc_gotoproc_get(&pr))
    {
        ee_sharserv_ipc_gotoproc_clr();
        
        if(ee_procUpdater == pr)
        {   // we communicate to the updater to stay forever and not to jump to default after the 5 (or what) seconds
            ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
        }
        
        // attempt only to the requested process.
        s_loader_attempt_jump(pr, LOADER_ADR_INVALID);
        
        // if in here ... the jump failed, thus ... behave as if no order at all
        ee_sharserv_ipc_gotoproc_clr();
        
//        // if in here ... the jump failed, thus ... it is better to go to the updater and stay there forever
//        ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
//        s_loader_attempt_jump(ee_procUpdater, LOADER_ADR_INVALID);
        
    }
    
    uint32_t address2jump = 0;
    if(ee_res_OK == ee_sharserv_ipc_jump2addr_get(&address2jump))
    {
        ee_sharserv_ipc_jump2addr_clr();
           
        if(ee_res_OK == ee_sharserv_sys_canjump(address2jump))
        {
            ee_sharserv_sys_jumpnow(address2jump);
        }   
       
    }    
}

// used to attempt a jump to a process 
static void s_loader_attempt_jump(eEprocess_t proc, uint32_t adr_in_case_proc_fails)
{
    uint32_t address = LOADER_ADR_INVALID;

    if(ee_procNone != proc)
    {

        if(ee_procLoader == proc)
        {
            // avoid recursive bootstraps
            s_loader_exec_loader();
        }
     
        // attempt to get the address of the proc from partition table.
        // it works only if the process already run before and register itself in the partition table.
        // if it fails ... use brute force mode
        if(ee_res_NOK_generic == ee_sharserv_part_proc_runaddress_get(proc, &address))
        {
            switch(proc)
            {
                case ee_procUpdater:        address = EENV_MEMMAP_EUPDATER_ROMADDR;         break;
                case ee_procApplication:    address = EENV_MEMMAP_EAPPLICATION_ROMADDR;     break;
                default:                    address = LOADER_ADR_INVALID;                   break;
            }
        }
        
        // if we retrieve the address ... attempt to jump
        if(LOADER_ADR_INVALID != address)
        {
            if(ee_res_OK == ee_sharserv_sys_canjump(address))
            {
                ee_sharserv_sys_jumpnow(address);
            }                
        }        

    }
    
    // if in here it means the we could not jump to the address of proc, thus we attempt an alternative.
    if(LOADER_ADR_INVALID != adr_in_case_proc_fails)
    {
        // attempt with adr_in_case_proc_fails 
        if(ee_res_OK == ee_sharserv_sys_canjump(adr_in_case_proc_fails))
        {
            ee_sharserv_sys_jumpnow(adr_in_case_proc_fails);
        }
    }

    // if i am in here we cannot jump ...
    return;
}

// the default application in case a process ask to execute the loader
void s_loader_exec_loader(void)
{    
    defaultapplication(applblinkrate);
}

#endif

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
