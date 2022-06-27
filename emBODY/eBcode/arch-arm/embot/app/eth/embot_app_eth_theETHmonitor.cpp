
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_eth_theETHmonitor.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_os.h"
#include "embot_os_rtos.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

struct embot::app::eth::theETHmonitor::Impl
{
    Config _config {};
    bool _initted {false};
    
    embot::os::PeriodicThread *_thr {nullptr};

    embot::os::rtos::mutex_t *_mtx {nullptr};
        
    Impl() = default;      
    
    bool initialise(const Config &config);   
    
    bool set(const embot::core::Callback &tkr);
    bool tick();
    

    static void startup(embot::os::Thread *t, void *p);  
    static void onperiod(embot::os::Thread *t, void *p);  

private:
    bool ticker();        
};



void tETHmon(void *p) { reinterpret_cast<embot::os::Thread*>(p)->run(); }

bool embot::app::eth::theETHmonitor::Impl::initialise(const Config &config)
{
    if(true == _initted)
    {
        return true;
    }
    
    if(false == config.isvalid())
    {
        return false;
    }
    
    _config = config;
    
    embot::core::print("embot::app::eth::theETHmonitor::Impl::initialise()");    
    
 
    // create the mutex
    _mtx = embot::os::rtos::mutex_new();
    
    constexpr embot::core::relTime timeout {1000*embot::core::time1millisec};

    embot::os::PeriodicThread::Config tCfg { 
        _config.thread.stacksize, 
        _config.thread.priority, 
        startup, this,
        _config.period,
        onperiod,
        "tETHmonitor"
    };
       
        
    // create the periodic thread 
    _thr = new embot::os::PeriodicThread;          
    // and start it
    _thr->start(tCfg, tETHmon);
    
    _initted = true;
    return true;
}

bool embot::app::eth::theETHmonitor::Impl::set(const embot::core::Callback &ticker)
{
    embot::os::rtos::mutex_take(_mtx, embot::core::reltimeWaitForever);
    _config.ticker = ticker;
    embot::os::rtos::mutex_release(_mtx);  
    return true;
}

bool embot::app::eth::theETHmonitor::Impl::ticker()
{
    embot::os::rtos::mutex_take(_mtx, embot::core::reltimeWaitForever);
    _config.ticker.execute();
    embot::os::rtos::mutex_release(_mtx);  
    return true;
}


bool embot::app::eth::theETHmonitor::Impl::tick()
{
    // in here we check if .... we have any data to put into the error manager
    return true;
}



// the static ones 

void embot::app::eth::theETHmonitor::Impl::startup(embot::os::Thread *t, void *p)
{
    embot::app::eth::theETHmonitor::Impl *impl = reinterpret_cast<embot::app::eth::theETHmonitor::Impl*>(p);
    embot::core::print("TODO: tETHmon must yet init embot::eth:: etc"); 
}

void embot::app::eth::theETHmonitor::Impl::onperiod(embot::os::Thread *t, void *p)
{
    embot::app::eth::theETHmonitor::Impl *impl = reinterpret_cast<embot::app::eth::theETHmonitor::Impl*>(p);

    // add in here all the code for eth monitoring
}




// --------------------------------------------------------------------------------------------------------------------
// - the class
// --------------------------------------------------------------------------------------------------------------------

embot::app::eth::theETHmonitor& embot::app::eth::theETHmonitor::getInstance()
{
    static theETHmonitor* p = new theETHmonitor();
    return *p;
}

embot::app::eth::theETHmonitor::theETHmonitor()
{
    pImpl = std::make_unique<Impl>();
}  

    
embot::app::eth::theETHmonitor::~theETHmonitor() { }
        

bool embot::app::eth::theETHmonitor::initialise(const Config &config)
{
    return pImpl->initialise(config);
}


bool embot::app::eth::theETHmonitor::set(const embot::core::Callback &ticker)
{
    return pImpl->set(ticker);
}


bool embot::app::eth::theETHmonitor::tick()
{
    return pImpl->tick();
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


