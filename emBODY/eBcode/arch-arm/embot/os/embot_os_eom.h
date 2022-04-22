
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_OS_EOM_H_
#define __EMBOT_OS_EOM_H_

#include "embot_os.h"
#include "embot_core.h"

#include "EOtheErrorManager.h"


#if 0

    This is an adaptation layer which allows the use of EOM services inside an embot system 
    and enables the easy running of the legacy code of the ETH boards (ems, etc.) on the 
    modern ETH ones (amc, etc.)
    
    The legacy ETH boards massively use the EO environment which is mostly portable across
    other RTOSes as long as we redefine either the OSAL or ... the EOM layer.
    
    The embot::os layer offers services such as a scheduler, a timer manager, a callback manager, 
    threads, mutexes etc.
    
    But also the EOM layer offers them.
    
    If we want to reuse code developed for the legacy ETH boards such as an FT service or MC one,
    then the available C modules use reference to the running environment through three types of
    objects / modules
    - EO objects / modules: they are portable because their calls to RTOS service are done 
      through the EOV objects (but sometimes also by EOM);
    - EOV objects: they are portable because they contain an interface to EOM objects done through
      function pointers. The EOV can be seen as ... C++ virtual interfaces to the implementation 
      of the RTOS services which is inside the EOM objects.
    - EOM objects: they contain the specific implementations. They should not be called directly
      by EO code unless when they need to create an object.
    
    The wanted use of the EO + EOV + EOM objects was the following:
    - everything is developed using ONLY EO object and if some RTOS service is required, then 
      the EO object should call only:
      - only once an EOM constructor to have a pointer to the RTOS object,
      - calls to EOV object using the above EOM pointer
    - the EOV uses a virtual table contained inside the EOM pointer to call the relevant function.
    - the EOM object contains all the relevant funtions and prepare the virtual table.

    In such a case we can just use a different set of EOM objects depending on the RTOS or
    on the environment.
    
    
    Inside this namespace we prepare a standard embot::os application to use EO objects
    which require to do EOV calls (and some EOM calls as well) to achieve some service.
    
    It is the case of use of:
    - EOVtheSystem
    - EOVtheTimerManager
    - EOVtheCallbackManager
    - eov_task_ ...(EOVtaskDerived *p, ...)
    - and as an exception to the rule: eom_task_ ...(EOMtask *p, ...)
    
        
    the usage would be to run
    
    ```C++
    
    void initSystem(embot::os::Thread *t, void* initparam)
    {
        embot::core::print("this is the INIT thread");    
            
        embot::os::theTimerManager::getInstance().start({});     
        embot::os::theCallbackManager::getInstance().start({});  

        embot::os::EOM::initialise({});
        
        // all other things now
        eom_ipnet_Initialise( ... );
        ...
        
    }
        
    int main(void)
    { 
        constexpr embot::os::InitThread::Config initcfg = { 6*1024, initSystem, nullptr };
        constexpr embot::os::IdleThread::Config idlecfg = { 4*1024, nullptr, nullptr, onIdle };
        constexpr embot::core::Callback onOSerror = { };
        constexpr embot::os::Config osconfig {embot::core::time1millisec, initcfg, idlecfg, onOSerror};
        
        // embot::os::init() internally calls embot::hw::init() which also calls embot::core::init()
        embot::os::init(osconfig);
        
        // now i start the os    
        embot::os::start();

        // just because i am paranoid (embot::os::start() never returns)
        for(;;);    
    }    
    
    ```
        

#endif

namespace embot { namespace os { namespace EOM {
    
    struct Config
    {
        uint32_t tbd {};
        eOerrman_cfg_t erromancfg {};
        constexpr Config() = default;  
        constexpr Config(const eOerrman_cfg_t &ecfg) : erromancfg(ecfg) {};            
    };
    
    void initialise(const Config &cfg);
    
}}} // namespace embot { namespace os { namespace EOM {


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

