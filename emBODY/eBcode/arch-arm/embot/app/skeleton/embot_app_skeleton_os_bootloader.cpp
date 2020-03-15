
/*
 * Copyright (C) 2019 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_skeleton_os_bootloader.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"
#include "embot_os_Thread.h"
#include "embot_app_theBootloader.h"
#include "embot_app_bootloader_theCANparser.h"
#include "embot_app_theLEDmanager.h"
#include "embot_hw_can.h"


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot { namespace app { namespace skeleton { namespace os { namespace bootloader {
            
    struct ActivityParam
    {
        static const embot::core::relTime BlinkFastPeriod = 200*embot::core::time1millisec;
        static const embot::core::relTime BlinkSlowPeriod = 500*embot::core::time1millisec;
        static const embot::core::relTime BlinkMadlyPeriod = 50*embot::core::time1millisec;    
        
        embot::core::relTime blinkingperiod {BlinkFastPeriod};
        const embot::app::theCANboardInfo::bootloaderInfo *info {nullptr};
        
        ActivityParam() = default;
        ActivityParam(embot::core::relTime ti, const embot::app::theCANboardInfo::bootloaderInfo *in) : blinkingperiod(ti), info(in) {}
    };


}}}}}


namespace embot { namespace app { namespace skeleton { namespace os { namespace bootloader {
    
    constexpr embot::core::relTime timebeforeJump = 5 * embot::core::time1second;
    void activity_function(void* param);
    ActivityParam activity_param { ActivityParam::BlinkFastPeriod, nullptr};
    

    [[noreturn]] void run(const embot::app::theCANboardInfo::bootloaderInfo &info)
    {

        embot::app::theBootloader & bootloader = embot::app::theBootloader::getInstance();
            
        embot::app::theBootloader::evalRes res = bootloader.eval();
        
        // if in here, it means that we must execute. let's fill the embot::app::theBootloader::Config according to res
        activity_param.info = &info; // i copy into param the info about teh board
        embot::app::theBootloader::Config config { {activity_function, &activity_param}, timebeforeJump };
            
        switch(res)
        {       
            case embot::app::theBootloader::evalRes::jumpaftercountdown:
            {
                config.countdown = timebeforeJump;
                activity_param.blinkingperiod = ActivityParam::BlinkFastPeriod;            
            } break;

            case embot::app::theBootloader::evalRes::stayforever:
            {
                config.countdown = 0;
                activity_param.blinkingperiod = ActivityParam::BlinkSlowPeriod;            
            } break;
            
            case embot::app::theBootloader::evalRes::jumpfailed:        
            case embot::app::theBootloader::evalRes::unexected:
            default:
            {
                config.countdown = 0;
                activity_param.blinkingperiod = ActivityParam::BlinkMadlyPeriod;            
            } break;
        }
        
        // and now we execute. this function is [[noreturn]]
        // it inits the hw::bsp and then starts the scheduler. 
        // the OS init thread will: start timer manager and callback manager, and finally call activity_function(&activity_param).
        // 
        bootloader.execute(config);
    
        for(;;);
    }

  
}}}}}




namespace embot { namespace app { namespace skeleton { namespace os { namespace bootloader {
    
    constexpr std::uint8_t maxINPcanframes = 16;
    constexpr std::uint8_t maxOUTcanframes = 32;
    constexpr embot::os::Event evRXcanframe = 0x00000001;

    static void eventhread_init(embot::os::Thread *t, void *p);
    static void eventhread_onevent(embot::os::Thread *t, embot::os::Event evt, void *p);
    
    static void alerteventhread(void *arg);

    static std::vector<embot::prot::can::Frame> outframes;


    void activity_function(void* param)
    {
        ActivityParam* pp = reinterpret_cast<ActivityParam*>(param);    
        
        // manage the basic canboardinfo
        embot::app::theCANboardInfo &canbrdinfo = embot::app::theCANboardInfo::getInstance();   
        canbrdinfo.synch(*activity_param.info);    
        
        // manage the led blinking period
        embot::core::relTime period = 0;    
        if(nullptr != pp)
        {
            period = pp->blinkingperiod;
        }    

        // start the led pulser
        static const std::initializer_list<embot::hw::LED> allleds = {embot::hw::LED::one};  
        embot::app::theLEDmanager &theleds = embot::app::theLEDmanager::getInstance();     
        theleds.init(allleds);    
        theleds.get(embot::hw::LED::one).pulse(period, 0); 
           

        // start thread waiting for can messages.  
        const embot::core::relTime waitEventTimeout = 50*1000; //50*1000; //5*1000*1000;            
        embot::os::EventThread::Config configEV;    
        configEV.startup = eventhread_init;
        configEV.onevent = eventhread_onevent;
        configEV.param = nullptr;
        configEV.stacksize = 4*1024;
        configEV.priority = embot::os::Priority::high200;
        configEV.timeout = waitEventTimeout;
        
        embot::os::EventThread* eventhread = new embot::os::EventThread; 
        eventhread->start(configEV);
            
        // start the canparser
        embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
        embot::app::bootloader::theCANparser::Config config {embot::prot::can::Process::bootloader};
        canparser.initialise(config);  

        // finally init can. it will be enabled only inside teh startup() of the eventhread
        embot::hw::result_t r = embot::hw::resNOK;
        embot::hw::can::Config canconfig; 
        canconfig.rxcapacity = maxINPcanframes;
        canconfig.txcapacity = maxOUTcanframes;
        canconfig.onrxframe = embot::core::Callback(alerteventhread, eventhread); 
        embot::hw::can::init(embot::hw::CAN::one, canconfig);
        embot::hw::can::setfilters(embot::hw::CAN::one, embot::app::theCANboardInfo::getInstance().getCANaddress());  
    }
        
    static void alerteventhread(void *arg)
    {
        embot::os::EventThread* evtsk = reinterpret_cast<embot::os::EventThread*>(arg);
        if(nullptr != evtsk)
        {
            evtsk->setEvent(evRXcanframe);
        }
    }

    static void eventhread_init(embot::os::Thread *t, void *p)
    {
        embot::hw::result_t r = embot::hw::can::enable(embot::hw::CAN::one);  
        r = r;         
        outframes.reserve(maxOUTcanframes);
    }

    static void eventhread_onevent(embot::os::Thread *t, embot::os::EventMask eventmask, void *p)
    {
        if(0 == eventmask)
        {   // timeout ...     
            return;
        }
        
        // we clear the frames to be trasmitted
        outframes.clear();   
    
        if(true == embot::core::binary::mask::check(eventmask, evRXcanframe))
        {        
            embot::hw::can::Frame canframe;
            std::uint8_t remaining = 0;
            if(embot::hw::resOK == embot::hw::can::get(embot::hw::CAN::one, canframe, remaining))
            {        
                embot::app::bootloader::theCANparser &canparser = embot::app::bootloader::theCANparser::getInstance();
                if(true == canparser.process({canframe.id, canframe.size, canframe.data}, outframes))
                {
                }
                
                if(remaining > 0)
                {
                    t->setEvent(evRXcanframe);                 
                }
            }        
        }
        
        
        // if we have any packet we transmit them
        std::uint8_t num = outframes.size();
        if(num > 0)
        {
            for(std::uint8_t i=0; i<num; i++)
            {
                embot::hw::can::put(embot::hw::CAN::one, {outframes[i].id, outframes[i].size, outframes[i].data});                                       
            }
            embot::hw::can::transmit(embot::hw::CAN::one);                    
        }
     
    }


}}}}}


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

