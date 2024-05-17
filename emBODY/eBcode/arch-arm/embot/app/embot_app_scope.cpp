

/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_scope.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


struct embot::app::scope::SignalPrint::Impl
{   

    Impl() { }
    
    Config config {};
        
    embot::core::Time start {0};
    embot::core::Time stop {0};
    embot::core::relTime delta {0};
    
    void init(const Config &cfg)
    {        
        config = cfg;
        if(nullptr == config.on_off)
        {
            config.on_off = config.default_on_off;
        }
    }    
    
    void on()
    {
        start = embot::core::now();
    }
    
    void off() 
    {
        stop = embot::core::now();
        delta = stop - start;        
        config.on_off(config.tag, start, stop, delta);
    }
                          
};

#if !defined(EMBOT_APP_SCOPE_core)

struct embot::app::scope::SignalGPIO::Impl
{   

    Impl() { }
    
    Config config {};
        
    embot::hw::gpio::State sON {embot::hw::gpio::State::SET};
    embot::hw::gpio::State sOFF {embot::hw::gpio::State::RESET};
        
    void init(const Config &cfg)
    {
        config = cfg;
        sON = config.on;
        sOFF = (embot::hw::gpio::State::SET == sON) ? (embot::hw::gpio::State::RESET) : (embot::hw::gpio::State::SET);
        embot::hw::gpio::init(config.gpio, {embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::medium});
        off();
    }    
    
    void on()
    {
        embot::hw::gpio::set(config.gpio, sON);
    }
    
    void off() 
    {
        embot::hw::gpio::set(config.gpio, sOFF);
    }    
                      
};

#endif

// ------

#include "eventviewer.h"

void sig_dummy() {}
    
struct embot::app::scope::SignalEViewer::Impl
{   

    Impl() 
    {
         
    }
    
    Config config {};
    
    evEntityId_t evid {ev_ID_first_usrdef};
    evEntityId_t prev {ev_ID_idle};
        
    void init(const Config &cfg)
    {
        config = cfg;
        eventviewer_init();
        evid = ev_ID_first_usrdef+embot::core::tointegral(config.label);
        eventviewer_load(evid, config.name);                
    }
    
    void on()
    { 
        prev = eventviewer_switch_to(evid);
    }
    
    void off() 
    {
        eventviewer_switch_to(prev); 
    }
                      
};


// -----

struct embot::app::scope::SignalHisto::Impl
{   
    static constexpr embot::core::Time maxtimefilter {600*embot::core::time1second};
    Impl() { }
    
    Config config {};
        
    embot::core::Time start {0};
    embot::core::Time stop {0};
    embot::core::relTime delta {0};
    
    embot::tools::Histogram* histo {nullptr};
    
    void init(const Config &cfg)
    {        
        config = cfg;        
        histo = new embot::tools::Histogram;
        histo->init(config.hcfg);        
    }    
    
    void on()
    {
        start = embot::core::now();
    }
    
    void off() 
    {
        stop = embot::core::now();
        delta = stop - start;
        // add delta in histogram 
        if(delta < maxtimefilter)
        {
            histo->add(delta); 
        }            
    }
                          
};

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


embot::app::scope::SignalPrint::SignalPrint(const Config &cfg) 
: pImpl(new Impl)
{   
    pImpl->init(cfg);
}

embot::app::scope::SignalPrint::~SignalPrint()
{   
    delete pImpl;
}


void embot::app::scope::SignalPrint::on() 
{   
    pImpl->on();
}


void embot::app::scope::SignalPrint::off() 
{   
    pImpl->off();
}


// ---

#if !defined(EMBOT_APP_SCOPE_core)

embot::app::scope::SignalGPIO::SignalGPIO(const Config &cfg) 
: pImpl(new Impl)
{   
    pImpl->init(cfg);
}

embot::app::scope::SignalGPIO::~SignalGPIO()
{   
    delete pImpl;
}


void embot::app::scope::SignalGPIO::on() 
{   
    pImpl->on();
}


void embot::app::scope::SignalGPIO::off() 
{   
    pImpl->off();
}

#endif
// -----

embot::app::scope::SignalEViewer::SignalEViewer(const Config &cfg) 
: pImpl(new Impl)
{   
    pImpl->init(cfg);
}

embot::app::scope::SignalEViewer::~SignalEViewer()
{   
    delete pImpl;
}


void embot::app::scope::SignalEViewer::on() 
{   
    pImpl->on();
}


void embot::app::scope::SignalEViewer::off() 
{   
    pImpl->off();
}


// ---

embot::app::scope::SignalHisto::SignalHisto(const Config &cfg) 
: pImpl(new Impl)
{   
    pImpl->init(cfg);
}

embot::app::scope::SignalHisto::~SignalHisto()
{   
    delete pImpl;
}


void embot::app::scope::SignalHisto::on() 
{   
    pImpl->on();
}


void embot::app::scope::SignalHisto::off() 
{   
    pImpl->off();
}

const embot::tools::Histogram * embot::app::scope::SignalHisto::histogram() const
{
    return pImpl->histo;
}



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

