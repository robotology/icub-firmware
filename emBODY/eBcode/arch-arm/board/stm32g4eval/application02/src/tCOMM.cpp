
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "tCOMM.h"

#include "embot_app_application_theCANparserBasic.h"
#include "embot_app_application_theCANtracer.h"
#include "embot_hw_can.h"

#include "embot_app_application_theFAPreader.h"
#include "embot_app_application_thePOSreader2.h"
#include "embot_app_application_theCANparserPOS.h"

#include "embot_app_application_theCANparserMC.h"
#include "embot_app_application_thePZMdriver.h"


//#define TEST_NO_CAN

namespace embot { namespace app { namespace ctrl {
    
    embot::os::rtos::mutex_t *_mtxOf_frames2tx {nullptr};
    std::vector<embot::prot::can::Frame> _frames2tx {};

#if defined(TEST_NO_CAN)        
    std::vector<embot::prot::can::Frame> testframes;
#endif
        
    void init() 
    {
        _mtxOf_frames2tx = embot::os::rtos::mutex_new();
        _frames2tx.reserve(20);

#if defined(TEST_NO_CAN)          
        testframes.reserve(10);
#endif
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
//    embot::core::print("tCAN_tout");       
}


void embot::app::ctrl::tCOMM::userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes) const
{ 

    //embot::core::print("tCAN_RXframe");   
    
    // process w/ the basic parser. if not recognised call the parsers specific of the board
    if(true == embot::app::application::theCANparserBasic::getInstance().process(frame, outframes))
    {                   
    }
    else if(true == embot::app::application::theCANparserPOS::getInstance().process(frame, outframes))
    {
        // in here the thread tCOMM has called the parser which calls methods of theFAPreader which for instance
        // configure and start acquisition with a given rate.
    }
    else if(true == embot::app::application::theCANparserMC::getInstance().process(frame, outframes))
    {
        // in here the thread tCOMM has called the parser which calls methods of theFAPreader which for instance
        // configure and start acquisition with a given rate.
    }     

}


void embot::app::ctrl::tCOMM::userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const
{
    static volatile embot::core::Time ttt = 0;
   
    if(true == embot::core::binary::mask::check(eventmask, evtTRANSMITfaps))
    {  
        embot::app::application::thePOSreader2 &thereader = embot::app::application::thePOSreader2::getInstance(); 
        
#if !defined(TEST_NO_CAN)   
        thereader.get(outframes);
#if 0        
        if(outframes.size() > 0)
        {
            std::string str = std::string("tCAN: ") + std::to_string(outframes.size()) + " angles to TX = [ ";
            for(size_t i=0; i<outframes.size(); i++)
            {
                int16_t v = static_cast<int16_t>(outframes[i].data[2]) + (static_cast<int16_t>(outframes[i].data[3]) << 8);
                str += std::to_string(v/10);
                str += " ";
            }
            str += "] DEG @ ";
            str +=  embot::core::TimeFormatter(embot::core::now()).to_string();  
            embot::core::print(str);           
        }
#endif        
#else
        testframes.clear();
        thefap.get(testframes);  

        ttt = embot::core::now();

        if(testframes.size() > 0)
        {
//            std::string str = std::string("tCAN: ") + std::to_string(testframes.size()) + " angles to TX = [ ";
//            for(size_t i=0; i<testframes.size(); i++)
//            {
//                int16_t v = static_cast<int16_t>(testframes[i].data[2]) + (static_cast<int16_t>(testframes[i].data[3]) << 8);
//                str += std::to_string(v/10);
//                str += " ";
//            }
//            str += "] DEG @ ";
//            str +=  embot::core::TimeFormatter(embot::core::now()).to_string();  
//            embot::core::print(str);           
        }
        else
        {
            embot::core::print("xxxxx");   
        }
#endif        
    }
    else
    {
//        embot::core::print("zzzzz");   
    } 

//    //embot::core::wait(5000);    
//    
//    
//    // process a TX request
//    
////    if(true == embot::core::binary::mask::check(eventmask, evtTXcanframe)) 
////    {
////        ttt = embot::core::now();
////#if defined(enableTRACE_all)        
////        embot::core::TimeFormatter tf(ttt);        
////        embot::core::print("userdefOnEventANYother() -> evtTXcanframe received @ time = " + tf.to_string());    
////#endif  
//
////        // if we have any packet we were asked to transmit then we fill outframes and the we empty _frames2tx.
////        
////        if(true == embot::os::rtos::mutex_take(_mtxOf_frames2tx, txTimeout))
////        {
////            std::uint8_t num = _frames2tx.size();
////            if(num > 0)
////            {
////                for(std::uint8_t i=0; i<num; i++)
////                {
////                    outframes.push_back(_frames2tx[i]);                                       
////                }
////                
////                _frames2tx.clear();
////            } 
////            embot::os::rtos::mutex_release(_mtxOf_frames2tx);
////        }
////        else
////        {
////            // attempt again
////            t->setEvent(evtTXcanframe);
////        }
////    }     
    
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

//embot::os::Thread * embot::app::ctrl::tCOMM::thread()
//{
//    return embot::app::skeleton::os::evthreadcan::getEVTthread();
//}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



