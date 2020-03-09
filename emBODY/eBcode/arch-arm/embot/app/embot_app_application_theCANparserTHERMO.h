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

#ifndef _EMBOT_APP_APPLICATION_THECANPARSERTHERMO_H_
#define _EMBOT_APP_APPLICATION_THECANPARSERTHERMO_H_

#include <vector>
#include <memory>
#include "embot_core.h"
#include "embot_app_canprotocol.h"
#include "embot_app_canprotocol_analog_polling.h"
#include "embot_app_canprotocol_analog_periodic.h"

namespace embot { namespace app { namespace application {
    
    class CANagentTHERMO
    {
    public:
        // interface
        virtual bool set(const embot::prot::can::analog::polling::Message_THERMOMETER_CONFIG_SET::Info &info) = 0;
        virtual bool set(const embot::prot::can::analog::polling::Message_THERMOMETER_TRANSMIT::Info &info) = 0;
    
        virtual bool get(const embot::prot::can::analog::polling::Message_THERMOMETER_CONFIG_GET::Info &info, embot::prot::can::analog::polling::Message_THERMOMETER_CONFIG_GET::ReplyInfo &replyinfo) = 0;
       
    public:
        virtual ~CANagentTHERMO() {};         
    };
           
    class theCANparserTHERMO
    {
    public:
        static theCANparserTHERMO& getInstance();
        
    public:
        
        struct Config
        {
            CANagentTHERMO* agent {nullptr};
            Config() = default;
            Config(CANagentTHERMO* a) : agent(a) {}
            bool isvalid() const { return (nullptr == agent) ? false : true; }
        }; 
        
        
        bool initialise(const Config &config); 
        
        // returns true if the canframe has been recognised. if so, any reply is sent if replies.size() > 0
        bool process(const embot::prot::can::Frame &frame, std::vector<embot::prot::can::Frame> &replies);

    private:
        theCANparserTHERMO(); 
        ~theCANparserTHERMO();


    private:    
        struct Impl;
        std::unique_ptr<Impl> pImpl;        
    };       


}}} // namespace embot { namespace app { namespace application


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
