
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "tCOMM.h"

#include "embot_app_application_theCANparserBasic.h"
#include "embot_app_application_theCANtracer.h"
#include "embot_hw_can.h"

namespace embot { namespace app { namespace ctrl {
    
    embot::os::rtos::mutex_t *_mtxOf_frames2tx {nullptr};
    std::vector<embot::prot::can::Frame> _frames2tx {};
        
    void init() 
    {
        _mtxOf_frames2tx = embot::os::rtos::mutex_new();
        _frames2tx.reserve(20);
    }
        
}}}    
 



void embot::app::ctrl::tCOMM::userdefStartup(embot::os::Thread *t, void *param) const
{
    
    embot::app::ctrl::init();
    
    // init of can basic paser
    embot::app::application::theCANparserBasic::getInstance().initialise({});
}


void embot::app::ctrl::tCOMM::userdefOnTimeout(embot::os::Thread *t, embot::os::EventMask eventmask, void *param) const
{
    static uint32_t cnt = 0;
    cnt++;    
}


void embot::app::ctrl::tCOMM::userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes) const
{        
    // process w/ the basic parser. if not recognised call the parsers specific of the board
    if(true == embot::app::application::theCANparserBasic::getInstance().process(frame, outframes))
    {                   
    }
//    else if(true == embot::app::application::theCANparserIMU::getInstance().process(frame, outframes))
//    {               
//    }

}

void embot::app::ctrl::tCOMM::userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const
{
    static embot::core::Time ttt = 0;
    
    // process a TX request
    
    if(true == embot::core::binary::mask::check(eventmask, evtTXcanframe)) 
    {
        ttt = embot::core::now();
#if defined(enableTRACE_all)        
        embot::core::TimeFormatter tf(ttt);        
        embot::core::print("userdefOnEventANYother() -> evtTXcanframe received @ time = " + tf.to_string());    
#endif  

        // if we have any packet we were asked to transmit then we fill outframes and the we empty _frames2tx.
        
        if(true == embot::os::rtos::mutex_take(_mtxOf_frames2tx, txTimeout))
        {
            std::uint8_t num = _frames2tx.size();
            if(num > 0)
            {
                for(std::uint8_t i=0; i<num; i++)
                {
                    outframes.push_back(_frames2tx[i]);                                       
                }
                
                _frames2tx.clear();
            } 
            embot::os::rtos::mutex_release(_mtxOf_frames2tx);
        }
        else
        {
            // attempt again
            t->setEvent(evtTXcanframe);
        }
    }     
    
}


bool embot::app::ctrl::tCOMM::add(const embot::prot::can::Frame &frame, const embot::core::relTime timeout)
{
    if(true == embot::os::rtos::mutex_take(_mtxOf_frames2tx, timeout))
    {
        _frames2tx.push_back(frame);
        embot::os::rtos::mutex_release(_mtxOf_frames2tx);
        return true;
    }
    
    return false;
}


bool embot::app::ctrl::tCOMM::trasmit()
{
    embot::os::Thread * t = embot::app::skeleton::os::evthreadcan::getEVTthread();
    t->setEvent(evtTXcanframe);   

    return true;    
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



