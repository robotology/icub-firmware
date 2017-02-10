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

#ifndef _EMBOT_APP_THECANSERVICE_H_
#define _EMBOT_APP_THECANSERVICE_H_

#include "embot_common.h"

#include "embot_sys.h"

namespace embot { namespace app { namespace can {
    
    enum class Clas { pollingMotorControl = 0, periodicMotorControl = 1, pollingAnalogSensor = 2, periodicAnalogSensor = 3, pollingSkin = 4, periodicInertialSensor = 5, bootloader = 7 };

    struct Message
    {
        std::uint8_t    clas;       // use embot::app::can:Clas
        std::uint8_t    type;       // use they are: ICUBCANPROTO_POL_MC_CMD__SET_VELOCITY_PID, etc.
        std::uint16_t   filler16;
        void*           value;        
    };
    
    using Address = std::uint8_t;   // so far it is only an id and in here it is mostly 0.
    
    struct Frame            // however, it is better to define it inside embot::hw::can 
    {
        std::uint32_t       id;
        std::uint8_t        size;
        std::uint8_t        filler[3];
        std::uint8_t        data[8];        
    };
     
}}}

namespace embot { namespace app { namespace can {
    
    class Protocol
    {
    public:
                         
//        Protocol();
//        virtual ~Protocol();
             
        // it calls the action associated to ...
        virtual bool parse(const embot::app::can::Frame &frame) = 0;
        // it only returns the message and the sender. the caller will manage the action associated.
        virtual bool parse(const embot::app::can::Frame &frame, embot::app::can::Message &message, const embot::app::can::Address &address) = 0;
        // the caller takes the frame and adds it to the ...
        virtual bool form(const embot::app::can::Message &message, const embot::app::can::Address &address, embot::app::can::Frame &frame) = 0;           
    }; 


    class StrainProtocol: public Protocol
    {
    public:
        StrainProtocol();
        ~StrainProtocol();
     
        bool init();
        
        virtual bool parse(const embot::app::can::Frame &frame);
        virtual bool parse(const embot::app::can::Frame &frame, embot::app::can::Message &message, const embot::app::can::Address &address);
        virtual bool form(const embot::app::can::Message &message, const embot::app::can::Address &address, embot::app::can::Frame &frame);    

    private:        
        struct Impl;
        Impl *pImpl;          
    };
    
}}}


namespace embot { namespace app { namespace can {
    
       
    class theCANservice
    {
    public:
        static theCANservice& getInstance()
        {
            static theCANservice* p = new theCANservice();
            return *p;
        }
                
    public:
    
        struct Config
        {
            embot::common::relTime      canstabilizationtime;
            std::uint8_t                rxqueuesize;
            std::uint8_t                txqueuesize;
            embot::app::can::Protocol   *protocol;
        }; 
        
        
        bool init(Config &config);
        
        std::uint8_t sizeOfRXqueue();
        
        std::uint8_t sizeOfTXqueue();
        
        bool parse();
        
        bool parse(std::uint8_t maxnumber, std::uint8_t &remaining);
        
        bool add(embot::app::can::Message &msg, embot::app::can::Address &toaddress);
        
        bool transmit(embot::common::relTime timeout, std::uint8_t &transmitted);
                
    private:
        theCANservice(); 
  
    public:
        // remove copy constructors and copy assignment operators
        theCANservice(const theCANservice&) = delete;
        theCANservice(theCANservice&) = delete;
        void operator=(const theCANservice&) = delete;
        void operator=(theCANservice&) = delete;

    private:    
        struct Impl;
        Impl *pImpl;        
    };       


}}} // namespace embot { namespace app { namespace can


#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------
