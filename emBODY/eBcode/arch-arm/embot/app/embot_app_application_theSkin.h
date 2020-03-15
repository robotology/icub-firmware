/*
 * Copyright (C) 2017 iCub Facility - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
 * website: www.robotcub.org
 * Permission is granted to copy, distribute, and/or modify this program
 * under the terms of the GNU General Public License, version 2 or any
 * later version published by the Free Software Foundation.
 *
 * A copy of the license can be found at
 * http://www.robotcub.org/icub/license/gpl.txt
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General
 * Public License for more details
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef _EMBOT_APP_APPLICATION_THESKIN_H_
#define _EMBOT_APP_APPLICATION_THESKIN_H_

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_os.h"
#include "embot_os_Thread.h"
#include "embot_app_application_theCANparserSkin.h"
#include <vector>
#include <memory>

namespace embot { namespace app { namespace application {
           
    class theSkin : public CANagentSKIN
    {
    public:
        static theSkin& getInstance();
        
        
    public:
        struct Config
        {
            embot::os::Event    tickevent;
            embot::os::Thread*       totask;
            Config() : tickevent(0), totask(nullptr) {}
        }; 
        
        
        bool initialise(Config &config);   

//        bool configure(embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::Info &brdcfg);
//        bool configure(embot::prot::can::analog::polling::Message_SKIN_SET_TRIANG_CFG::Info &trgcfg);        
//        bool configure(embot::prot::can::analog::polling::Message_SKIN_OBSOLETE_TACT_SETUP::Info &tactsetup);
        
        bool start();
        bool stop();        
        bool tick(std::vector<embot::prot::can::Frame> &replies);
        
        // interface to CANagentSKIN
        virtual bool set(const embot::prot::can::analog::polling::Message_SKIN_SET_BRD_CFG::Info &info);
        virtual bool set(const embot::prot::can::analog::polling::Message_SKIN_SET_TRIANG_CFG::Info &info);  
        virtual bool set(const embot::prot::can::analog::polling::Message_SET_TXMODE::Info &info); 
        virtual bool set(const embot::prot::can::analog::polling::Message_SKIN_OBSOLETE_TACT_SETUP::Info &info);

    private:
        theSkin(); 
        ~theSkin(); 

    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;        
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
