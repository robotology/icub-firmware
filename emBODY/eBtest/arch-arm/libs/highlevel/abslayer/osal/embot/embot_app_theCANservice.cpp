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


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_theCANservice.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot.h"

#include <cstring>

#include "embot_hw.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------


namespace embot { namespace app { namespace can {
    
 
struct embot::app::can::StrainProtocol::Impl
{    
    std::uint8_t dummy;    

    
    Impl() 
    {
        dummy = 0;     
    }
    
    ~Impl()
    {
        dummy = 0;
    }
};


embot::app::can::StrainProtocol::StrainProtocol()
: pImpl(new Impl)
{   

}


embot::app::can::StrainProtocol::~StrainProtocol()
{   
    delete pImpl;
}
    
bool embot::app::can::StrainProtocol::init()
{
    return true;
}


bool embot::app::can::StrainProtocol::parse(const embot::app::can::Frame &frame)
{
    
    return true;
}

bool embot::app::can::StrainProtocol::parse(const embot::app::can::Frame &frame, embot::app::can::Message &message, const embot::app::can::Address &address)
{
    
    return true;
}


bool embot::app::can::StrainProtocol::form(const embot::app::can::Message &message, const embot::app::can::Address &address, embot::app::can::Frame &frame)
{
    
    return true;
}
    
}}}




struct embot::app::can::theCANservice::Impl
{    
    std::uint8_t dd;
    
    embot::app::can::theCANservice::Config config;
    
    Impl() 
    {              
        dd = 0;   
    }
};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



embot::app::can::theCANservice::theCANservice()
: pImpl(new Impl)
{       

}



bool embot::app::can::theCANservice::init(Config &config)
{    
    pImpl->config = config;
    
    return true;
}
   
   
   
std::uint8_t embot::app::can::theCANservice::sizeOfRXqueue()
{
    return 0;
}
        
std::uint8_t embot::app::can::theCANservice::sizeOfTXqueue()
{
    return 0;
}

bool embot::app::can::theCANservice::parse()
{    
    return true;
}
        
bool embot::app::can::theCANservice::parse(std::uint8_t maxnumber, std::uint8_t &remaining)
{ 
    
    embot::app::can::Frame frame;
    pImpl->config.protocol->parse(frame);
    return true;
}


bool embot::app::can::theCANservice::add(embot::app::can::Message &msg, embot::app::can::Address &toaddress)
{  
    embot::app::can::Frame frame; 
    pImpl->config.protocol->form(msg, toaddress, frame); 
    // push-back into fifo    
    return true;
}        

bool embot::app::can::theCANservice::transmit(embot::common::relTime timeout, std::uint8_t &transmitted)
{    
    return true;
} 
       


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


