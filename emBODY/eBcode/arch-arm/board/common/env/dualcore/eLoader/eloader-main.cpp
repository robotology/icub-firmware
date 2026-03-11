

/*
 * Copyright (C) 2026 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

#if 0

this file can be used by the dualcore boards for the loader and it supports both
- traditional boot mode (only by amc)
- the one specified by EMBOT_ENABLE_hw_dualcore defined inside embot_hw_bsp_config.h (by amcfoc and others) 

moreover, the flash addresses can be obtained by using macros inside eEmemorymap.h but also by calling 
embot::hw::sys::partition(embot::hw::flash::Partition::ID::whatever).address which however cannot be used as 
constexpr 

this new version of the eloader simplify the stream of execution and take sint account possible errors in EEPROM 
initialization 

#endif



// --------------------------------------------------------------------------------------------------------------------
// macros that should typically stay undefined

// if defined, every time the loader runs, it will force the eeprom to have def2run = application
// caveat: best using this macro just for one run
#undef DEBUG_forceEEPROM_DEF2RUNequalAPPLICATION
// if defined, the loader does not jump and forces execution of defaultapplication() 
#undef DEBUG_stayinhere
// if defined, the loader enables and starts the other core inside embot::hw::init()
#undef DEBUG_startOtherCOREnow
// if defined, every time the loader runs after reset (not forced by IPC) it forces the erase of the EEPROM 
// and its reinitalization. 
// caveat: best using this macro just for one run
#undef DEBUG_eraseEEPROM

// --------------------------------------------------------------------------------------------------------------------
// macros that are defined by external definition of others

// macro shaper for having the code of the loader that acts as a launcher of the other core
#if defined(CM7launcher) || defined(CM4launcher)
#define OtherCORElauncher
#endif

// the OtherCORElauncher starts the other core and does not force a jump
#if defined(OtherCORElauncher)
#define DEBUG_startOtherCOREnow
#define DEBUG_stayinhere
#endif


// --------------------------------------------------------------------------------------------------------------------
// macros that can be used for extra feature or for debug

#define attemptFIXeeprom

#if defined(attemptFIXeeprom)
// if defined, in case the eeprom is initted correctly but than not read correctly it avoids to force a value of
// def2run = updater. the disadvantage is that when EEPROM is erased the FirmwareUpdater tells the def2run is None
#define DONTFORCE_def2run
#define DEBUG_log2eeprom
//#define continuousRESET
#endif


// --------------------------------------------------------------------------------------------------------------------
// includes

#include "stm32hal.h"
#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_sys.h"
#include "embot_hw_led.h"
#include "embot_hw_bsp_config.h"
#include "embot_hw_eeprom.h"

#include "eEsharedServices.h" 
#include "board-info.h"
#include "module-info.h"

// --------------------------------------------------------------------------------------------------------------------
// hw config

namespace eloader::hw::config {
    
// in here we run the baremetal embot::hw application.
// for the required get1microtime() we can either use a very naked approach or even the systick

#define USE_SYSTICK_AS_TIME_BASE

#if defined(USE_SYSTICK_AS_TIME_BASE)


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

} // namespace eloader::hw::config {


// --------------------------------------------------------------------------------------------------------------------
// some constants

#if defined(OtherCORElauncher)
constexpr embot::core::relTime applblinkrate {250*embot::core::time1millisec};
#else
constexpr embot::core::relTime applblinkrate {100*embot::core::time1millisec};
#endif


// --------------------------------------------------------------------------------------------------------------------
// some functions

#if defined(EMBOT_ENABLE_hw_dualcore)
#warning EMBOT_ENABLE_hw_dualcore is defined
#include "embot_hw_dualcore.h"
void prepareHWinit()
{
#if defined(DEBUG_startOtherCOREnow)
    constexpr embot::hw::dualcore::Config dcc {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activate };
#else
    constexpr embot::hw::dualcore::Config dcc {embot::hw::dualcore::Config::HW::forceinit, embot::hw::dualcore::Config::CMD::activateandhold };
#endif
    embot::hw::dualcore::config(dcc);
}
#else
#include "embot_hw_bsp_amc.h"
#warning EMBOT_ENABLE_hw_dualcore is NOT defined
void prepareHWinit()
{
#if defined(DEBUG_startOtherCOREnow)
    constexpr embot::hw::bsp::amc::OnSpecialize onspec { embot::hw::bsp::amc::OnSpecialize::CM4MODE::activate, false, false };
#else
    constexpr embot::hw::bsp::amc::OnSpecialize onspec { embot::hw::bsp::amc::OnSpecLoader };
#endif	
    embot::hw::bsp::amc::set(onspec);
}
#endif

void initHWbasic();
void thejumper(); 
[[noreturn]] void defaultapplication(embot::core::relTime blinkrate = applblinkrate);

// --------------------------------------------------------------------------------------------------------------------
// main
    
int main(void)
{  
    initHWbasic();
    
#if !defined(DEBUG_stayinhere) 
    thejumper();
#endif  
    
    // only if the jump fails, then run default application
    defaultapplication();         
}


// --------------------------------------------------------------------------------------------------------------------
// other functions

void initHWbasic()
{
    // actions required before call of embot::hw::init()
    prepareHWinit();        
    embot::hw::init(eloader::hw::config::hwCFG); 
}

void ipcJUMP();
void eepromJUMP();
void blindJUMP();

static void s_loader_manage_error(embot::core::relTime rate = 100*embot::core::time1millisec)
{
    defaultapplication(rate);
}
    
void thejumper()
{ 
    // attempt an ipc jump. that means that is the updater or application that order to jump somewhere. 
    // for here we just need to use ipc ram, no eeprom is needed 
    // if there is an ipc command ipcJUMP() does not return     
    ipcJUMP();

    // there was not an ipc jump. i now need eeprom, but i want to be sure that we can init it, so ...   
    // init eeprom just now and keep result
    embot::hw::result_t r = embot::hw::eeprom::init(embot::hw::EEPROM::one, {});
    
    // keep it commented. uncomment it just for test         
//    r = embot::hw::resNOK;
        
    if(embot::hw::resOK == r)
    {   // the eeprom is available. i use it to synch some info to it and then i do a jump to def2run
        eepromJUMP();
    }
    else
    {   // the eeprom fails initialization. i attempt a jump w/out reading from eeprom where to jump
        blindJUMP();
    }
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


// --------------------------------------------------------------------------------------------------------------------
// other functions

static void s_loader_shared_services_init(void);
static void s_loader_shared_services_synch(void);
static void s_loader_exec_loader(void);
enum class attemptJUMPmode { dontuseSTORAGEinfo, useSTORAGEinfo };
static void s_loader_attempt_jump(eEprocess_t proc, attemptJUMPmode ajm, uint32_t adr_in_case_proc_fails);

constexpr uint32_t LOADER_ADR_INVALID {0xffffffff};


// it checks if an eproc requests a jump. no eeprom is needed in here
void ipcJUMP()
{
    // important:
    // we can use shalbase_ipc_*() and shalbase_system_*() functions w/out a shalbase_init()
    
    eEprocess_t pr = ee_procNone;

    if(ee_res_OK == ee_sharserv_ipc_gotoproc_get(&pr))
    {
        ee_sharserv_ipc_gotoproc_clr();
        
        if(ee_procUpdater == pr)
        {   // we communicate to the updater to stay forever and not to jump to default after the 5 (or what) seconds
            ee_sharserv_ipc_gotoproc_set(ee_procUpdater);
        }
        
        // attempt only to the requested process and dontuseSTORAGEinfo because EEPROM is not initted yet
        s_loader_attempt_jump(pr, attemptJUMPmode::dontuseSTORAGEinfo, LOADER_ADR_INVALID);
        
        // if in here ... the jump failed, thus ... behave as if no order at all
        ee_sharserv_ipc_gotoproc_clr();        
    }
    
    // else we dont have a process to jump to but maybe an address ....
    uint32_t address2jump = 0;
    if(ee_res_OK == ee_sharserv_ipc_jump2addr_get(&address2jump))
    {
        ee_sharserv_ipc_jump2addr_clr();
           
        if(ee_res_OK == ee_sharserv_sys_canjump(address2jump))
        {
            ee_sharserv_sys_jumpnow(address2jump);
        }          
    }
    
    // else ... we dont have any ipc or we have but we have failed to jump there.
    // so: we attempt something else
        
}

void blindJUMP()
{    
    // we dont know anything from eeprom, so we attempt a jump to ee_procUpdater
    uint32_t address = embot::hw::sys::partition(embot::hw::flash::Partition::ID::eupdater).address;
    
    if(ee_res_OK == ee_sharserv_sys_canjump(address))
    {
        ee_sharserv_sys_jumpnow(address);
    }     
}


void eepromJUMP()
{    
    // we have the eeprom already activated, so we use the shared services to operate w/ that 
        
#if defined(DEBUG_eraseEEPROM)
    embot::hw::eeprom::erase(embot::hw::EEPROM::one, 3*embot::core::time1millisec); // 0, 8*1024);   
#endif  
    
    // inits the complete shared services: 
    // - shalbase 
    //    - w/ eeprom storage that if already initted does not reinit itself again;
    //    - w/ ipc
    // - shalpart that loads the partition table from eeprom to cached ram
    
    s_loader_shared_services_init();
    
    // in here we uses the eeprom storage to enforce synchronization of some data (uinque id, board type, eloader version)
    s_loader_shared_services_synch();    

    // and finally we attempt jumping by getting the startup process from the eeprom storage or to the default
        
    eEprocess_t startup = ee_procNone;
    if(ee_res_NOK_generic == ee_sharserv_part_proc_startup_get(&startup))
    {
        startup = ee_procUpdater;
    }
    s_loader_attempt_jump(startup, attemptJUMPmode::useSTORAGEinfo, LOADER_ADR_INVALID);
    
    // if it fails a jump to startup ... do a last attempt to jump to eUpdater
    s_loader_attempt_jump(ee_procUpdater, attemptJUMPmode::useSTORAGEinfo, LOADER_ADR_INVALID);

    // if we are in here we cannot jump to the startup and not even to the updater.
    // the error will be managed elsewhere
}


// --------------------------------------------------------------------------------------------------------------------
// other functions

static void s_on_sharserv_error(void)
{
    s_loader_manage_error();
}

static void s_loader_shared_services_init(void)
{    
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
    
}

static void s_loader_shared_services_synch(void)
{   
    // now all are initted. then ...
    
    eEprocess_t defproc = ee_procNone;
    eEprocess_t startup = ee_procNone;    
      
    // make sure signature of eloader in partition table is ok
    if(ee_res_OK != ee_sharserv_part_proc_synchronise(ee_procLoader, (const eEmoduleInfo_t *)env::dualcore::module::info::get()))
    {
        s_loader_manage_error();
    }    
    
    // impose unique id
    env::dualcore::board::info::set(embot::hw::sys::uniqueid());
    
    // make sure info of the board stored in eeprom is correct
    if(ee_res_OK != ee_sharserv_info_boardinfo_synchronise(env::dualcore::board::info::get()))
    {
        s_loader_manage_error();
    }  

#if defined(DEBUG_log2eeprom)
    bool log2eeprom {false};
#endif    
       
    // makes sure that the startup process is in partition table.
    // after the EEPROM is erased (or first time ever the eloader runs) the startup is not imposed, 
    // function returns NOK, so we need to set it to updater
    if(ee_res_OK != ee_sharserv_part_proc_startup_get(&startup))
    {
        // we impose that the startup process is the updater
        ee_sharserv_part_proc_startup_set(ee_procUpdater);
#if defined(DEBUG_log2eeprom)
        log2eeprom = true;
#endif          
    }  
    
    // makes sure that the def2run process is in partition table.
    // after the EEPROM is erased (or first time ever the eloader runs) the def2run is not imposed, 
    // function returns NOK, so we need to set it to ... updater
    // the macro DONTFORCE_def2run does not force any value for it. 
    if(ee_res_OK != ee_sharserv_part_proc_def2run_get(&defproc))
    {
#if defined(DONTFORCE_def2run)
        // we dont impose a defproc ... 
#else
        // we impose that the default process is the updater     
        ee_sharserv_part_proc_def2run_set(ee_procUpdater);      
#endif  

#if defined(DEBUG_log2eeprom)
        log2eeprom = true;
#endif                
    }
    
#if defined(DEBUG_log2eeprom)
    if(true == log2eeprom)
    {
        char buffer[64] = {0};
        snprintf(buffer, sizeof(buffer), "happened: st=0x%02X, dp=0x%02X", startup, defproc);         
        ee_sharserv_info_deviceinfo_item_set(sharserv_info_page32, buffer);      
    }
#endif       
 

#if defined(DEBUG_forceEEPROM_DEF2RUNequalAPPLICATION)
    // we impose that the application is the def2run
    ee_sharserv_part_proc_def2run_set(ee_procApplication);
#endif 
   
}

// used to attempt a jump to a process 
static void s_loader_attempt_jump(eEprocess_t proc, attemptJUMPmode ajm, uint32_t adr_in_case_proc_fails)
{
    uint32_t address = LOADER_ADR_INVALID;

    if(ee_procNone != proc)
    {
        if(ee_procLoader == proc)
        {
            // avoid recursive bootstraps
            s_loader_exec_loader();
        }
        
        // now we get an address to jump to
        
        // if we are allowed we
        // attempt to get the address of the proc from partition table.
        // it works only if the process already run before and register itself in the partition table.
        if(attemptJUMPmode::useSTORAGEinfo == ajm)
        {
            ee_sharserv_part_proc_runaddress_get(proc, &address);
        }
        
        // if we are not allowed or if the previous fails then we  ... use brute force mode
        if(LOADER_ADR_INVALID == address)
        {
            switch(proc)
            {
                case ee_procUpdater:
                {
                    address = embot::hw::sys::partition(embot::hw::flash::Partition::ID::eupdater).address;
                } break;
                case ee_procApplication:    
                {
                    address = embot::hw::sys::partition(embot::hw::flash::Partition::ID::eapplication00).address;
                } break;
                default:
                {
                    address = LOADER_ADR_INVALID;
                } break;
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



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

