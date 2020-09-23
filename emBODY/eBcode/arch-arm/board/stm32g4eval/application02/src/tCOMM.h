
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _TCOMM_H_
#define _TCOMM_H_


#include "embot_core.h"
#include "embot_core_binary.h"
#include "embot_app_skeleton_os_evthreadcan.h"
#include "embot_app_theCANboardInfo.h"
#include "embot_os_rtos.h"
#include <vector>


namespace embot { namespace app { namespace ctrl {
    
    // evRXcanframe is 0x00000001 or embot::core::binary::mask::pos2mask<embot::os::Event>(0);
    
    constexpr embot::os::Event evtTXcanframe = embot::core::binary::mask::pos2mask<embot::os::Event>(1); 
    
    constexpr embot::core::relTime txTimeout {3*embot::core::time1millisec};
    
    
    // this thread is derived from the application scheleton used so far by our simple applications
            
    class tCOMM final : public embot::app::skeleton::os::evthreadcan::evThreadCAN
    {
    public:
        tCOMM(const embot::app::skeleton::os::evthreadcan::evtConfig& ecfg, const embot::app::skeleton::os::evthreadcan::canConfig& ccfg, const embot::app::theCANboardInfo::applicationInfo& a) 
            : evThreadCAN(ecfg, ccfg, a) { init(); }
            
        // these are called by the comm thread
        void userdefStartup(embot::os::Thread *t, void *param) const override;
        void userdefOnTimeout(embot::os::Thread *t, embot::os::EventMask eventmask, void *param) const override;
        void userdefOnEventRXcanframe(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &outframes) const override;
        void userdefOnEventANYother(embot::os::Thread *t, embot::os::EventMask eventmask, void *param, std::vector<embot::prot::can::Frame> &outframes) const override;  

        // extra functions
        // these are called by other threads
        bool add(const embot::prot::can::Frame &frame, const embot::core::relTime timeout = 3*embot::core::time1millisec);
        
        bool trasmit();
        
    private:
         void init();         
    };   

    // we get the thread with
    // embot::os::Thread * embot::app::skeleton::os::evthreadcan::getEVTthread();
    
    
}}} // namespace embot { namespace app { namespace ctrl {



#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



