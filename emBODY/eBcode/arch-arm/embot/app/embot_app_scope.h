

/*
 * Copyright (C) 2021 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------


#ifndef __EMBOT_APP_SCOPE_H_
#define __EMBOT_APP_SCOPE_H_

//#define EMBOT_APP_SCOPE_core

#include "embot_core.h"
#if !defined(EMBOT_APP_SCOPE_core)
#include "embot_hw_gpio.h"
#endif
#include "embot_tools.h"

namespace embot { namespace app { namespace scope {
    
    // it is the interface
    class Signal 
    {
    public:                      
        // interface
        virtual void on() = 0;
        virtual void off() = 0;
        
    protected:
        virtual ~Signal() {};    // cannot delete from interface but only from derived object
    };
    
    enum class SignalType { Dummy, Print, EViewer, GPIO };

    // this implementation does nothing
    class SignalDummy: public Signal
    {
    public:
               
        struct Config
        {  
            uint32_t nothing {0};
            Config() = default;
            bool isvalid() const { return true; }
        };
                    
        SignalDummy(const Config &cfg) {};
        virtual ~SignalDummy() {};
    
        virtual void on() override {};
        virtual void off() override {};        
    };  
    
    // this implementation prints out
    class SignalPrint: public Signal
    {
    public:
               
        struct Config
        {  
            using fpOn_off = void (*)(const std::string &tag, embot::core::Time ontime, embot::core::Time offtime, embot::core::relTime duration);
            // tag is what helps differentiate the print                          
            std::string tag {"tag"};
            // callback invoked at execution of off() which can be tailored to print info.
            fpOn_off on_off {default_on_off};
            Config() = default;
            Config(const std::string &t, fpOn_off oo = default_on_off) : tag(t), on_off(oo) {}
            bool isvalid() const { return true; }
            static void default_on_off(const std::string &tag, embot::core::Time ontime, embot::core::Time offtime, embot::core::relTime duration)
            {
                embot::core::print  (tag + ": [" + embot::core::TimeFormatter(ontime).to_string() + ", " + embot::core::TimeFormatter(offtime).to_string() + "]" +
                                         + " -> " + embot::core::TimeFormatter(duration).to_string()
                                    );                 
            }
        };
                    
        SignalPrint(const Config &cfg);
        virtual ~SignalPrint();
    
        virtual void on() override;
        virtual void off() override;

    private:        
        struct Impl;
        Impl *pImpl;           
    };     
    
    
    // this implementation uses the event viewer
    class SignalEViewer: public Signal
    {
    public:
        
        struct Config
        {  
            // every SignalEViewer must have a different pair of {LABEL, NAME}
            // name is what appears on the Event Viewer window and takes the form of a function name declared public
            // label is used by the trace channel to the name.
            enum class LABEL { one = 0, two = 1, three = 2, four = 3, five = 4, six = 5, seven = 6, eight = 7, numberof = 8, none = 31};    
            using NAME = void (*)();    
                           
            NAME name {nullptr};
            LABEL label {LABEL::none};
            
            Config() = default;
            constexpr Config(NAME na, LABEL la) : name(na), label(la) {}                 
            bool isvalid() const
            {   // name cannot be nullptr
                if((nullptr == name)) { return false; }  
                else { return true; }
            }
        };
                    
        SignalEViewer(const Config &cfg);
        virtual ~SignalEViewer();
    
        virtual void on() override;
        virtual void off() override;

    private:        
        struct Impl;
        Impl *pImpl;           
    }; 


#if !defined(EMBOT_APP_SCOPE_core)
    
    // this implementation uses a gpio which puts high or low.
    class SignalGPIO: public Signal
    {
    public:
        
        struct Config
        {
            embot::hw::GPIO gpio {};
            embot::hw::gpio::Config gpiocfg {embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::medium};
            embot::hw::gpio::State on {embot::hw::gpio::State::SET};
            
            Config() = default;
            constexpr Config(embot::hw::GPIO g, 
                             embot::hw::gpio::State o, 
                             embot::hw::gpio::Config gc = {embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::medium}
                             ) : gpio(g), on(o), gpiocfg(gc) {}                 
            bool isvalid() const { return gpio.isvalid(); }
        };
                    
        SignalGPIO(const Config &cfg);
        virtual ~SignalGPIO();
    
        virtual void on() override;
        virtual void off() override;

    private:        
        struct Impl;
        Impl *pImpl;           
    };

#endif    
    
    // this implementation collects [on, off] duration in a histogram
    class SignalHisto: public Signal
    {
    public:
               
        struct Config
        {   
            embot::tools::Histogram::Config hcfg {0, 8, 1};
            Config() = default;
            Config(embot::core::Time minduration, embot::core::relTime range, embot::core::relTime step = 1) : hcfg({minduration, minduration+range, step}) {}
            bool isvalid() const { return hcfg.isvalid(); }
        };
                    
        SignalHisto(const Config &cfg);
        virtual ~SignalHisto();
    
        virtual void on() override;
        virtual void off() override;
        
        const embot::tools::Histogram * histogram() const;

    private:        
        struct Impl;
        Impl *pImpl;           
    };         
    
} } } // namespace embot { namespace app { namespace scope {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

