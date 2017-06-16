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
using namespace std;
using namespace embot;

static uint8_t n_txframes=0;
uint8_t tx_ended = 1;



namespace embot { namespace app { namespace can {
    
 
struct embot::app::can::SkinProtocol::Impl
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


embot::app::can::SkinProtocol::SkinProtocol()
: pImpl(new Impl)
{   

}


embot::app::can::SkinProtocol::~SkinProtocol()
{   
    delete pImpl;
}
    
bool embot::app::can::SkinProtocol::init()
{
    return true;
}


bool embot::app::can::SkinProtocol::parse(const embot::hw::can::Frame &frame)
{
    hw::can::Frame canframe = frame;

    canframe.data[0]++;
    hw::result_t res = hw::can::put(hw::can::Port::one, canframe);
    res = res;
    return true;
}

bool embot::app::can::SkinProtocol::parse(const embot::hw::can::Frame &frame, embot::app::can::Message &message, const embot::app::can::Address &address)
{
    
    return true;
}


bool embot::app::can::SkinProtocol::form(const embot::app::can::Message &message, const embot::app::can::Address &address, embot::hw::can::Frame &frame)
{
    frame.id = address;
    switch (message.type)
    {
        case 0:
        {
            frame.size = 6;
            frame.data[0] = 'c';
            frame.data[1] = 'i';
            frame.data[2] = 'a';
            frame.data[3] = 'o';
            frame.data[4] = *((uint8_t*)message.value);
            frame.data[5] = n_txframes;
        }break;
        case 1:
        {
            frame.size = 4;
            frame.data[0] = 'E';
            frame.data[1] = 'N';
            frame.data[2] = 'D';
            frame.data[3] = n_txframes;
            
        }break;

        case 2:
        {
            frame.size = 1;
            frame.data[0] = n_txframes;
            
        }break;
        default:
        {
            frame.size = 5;
            frame.data[0] = 'u';
            frame.data[1] = 'n';
            frame.data[2] = 'k';
            frame.data[3] = 'n';
            frame.data[4] = 'o';
        }
            
    }
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

namespace embot{ namespace app { namespace can {
    
static void callback_ontxframe(void *arg)
{
//do something
    //static uint8_t n_txframes=0;
    n_txframes++;
    return;
}

static void callback_onrxframe(void *arg)
{
    static uint32_t n_rxframes=0;
    n_rxframes++;
//do something
    return;
}

static void callback_ontxqueueempty(void *arg)
{
    //do somothing
    n_txframes = 0;
    tx_ended=1;
    return;
}

bool embot::app::can::theCANservice::init(Config &config)
{    
    pImpl->config = config;
    hw::can::Config canconfig;
    
    canconfig.ontxframe.callback = callback_ontxframe;
    canconfig.ontxframe.arg = nullptr;

    canconfig.onrxframe.callback = callback_onrxframe;
    canconfig.onrxframe.arg = nullptr;

    canconfig.txqueueempty.callback = callback_ontxqueueempty;
    canconfig.txqueueempty.arg = nullptr;
    
    canconfig.txcapacity = config.txqueuesize;
    canconfig.rxcapacity = config.rxqueuesize;
    
    hw::result_t hres = hw::can::init(hw::can::Port::one, canconfig);
    if(hw::resNOK == hres)
        return false;
    
    hres = hw::can::enable(hw::can::Port::one);
    if(hw::resNOK == hres)
        return false;
    
    return true;
}
   
   
   
std::uint8_t embot::app::can::theCANservice::sizeOfRXqueue()
{
    return hw::can::inputqueuesize(hw::can::Port::one);
}
        
std::uint8_t embot::app::can::theCANservice::sizeOfTXqueue()
{
    return hw::can::outputqueuesize(hw::can::Port::one);
}

bool embot::app::can::theCANservice::parse_core(uint8_t numofframes, uint8_t &remaining)
{
    for(uint8_t i=0; i<numofframes; i++)
    {
        hw::can::Frame canframe;
        hw::result_t res = hw::can::get(hw::can::Port::one, canframe, remaining);
        if(hw::resNOK == res)
            return false;
    
        pImpl->config.protocol->parse(canframe);
    }
    return true;
}

bool embot::app::can::theCANservice::parse()
{    
    uint8_t size = hw::can::inputqueuesize(hw::can::Port::one);
    uint8_t remaining =0;
    return (parse_core(size, remaining));
}
        
bool embot::app::can::theCANservice::parse(std::uint8_t maxnumber, std::uint8_t &remaining)
{ 
    uint8_t size = hw::can::inputqueuesize(hw::can::Port::one);
    uint8_t frame2read = 0;
    if(maxnumber>size)
        frame2read = size;
    else
        frame2read = maxnumber;
    
    return (parse_core(size, remaining));
}


bool embot::app::can::theCANservice::add(embot::app::can::Message &msg, embot::app::can::Address &toaddress)
{  
    embot::hw::can::Frame frame; 
    pImpl->config.protocol->form(msg, toaddress, frame); 
    

    hw::result_t res = hw::can::put(hw::can::Port::one, frame);
    if(hw::resNOK == res)
        return false;
    else
        return true;
}        

bool embot::app::can::theCANservice::transmit(embot::common::relTime timeout, std::uint8_t &transmitted)
{   
    #warning VALE: the check of timeout in theCANservice::transmit is not implemented!!!
    tx_ended = 0;   
    hw::result_t res = hw::can::transmit(hw::can::Port::one);
    if(hw::resNOK == res)
        return false;    
    return true;
} 
       
}}}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


