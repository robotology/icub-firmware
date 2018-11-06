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

#include "embot_sys_theJumper.h"

#include "embot_hw.h"
#include "embot_hw_sys.h"

#include <new>


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#define NZI_SIZE        (32)    
#define NZI_START       (0x20000000)


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------

#if __ARMCOMPILER_VERSION > 6000000
static volatile std::uint32_t s_nzi_ram[NZI_SIZE/4]         __attribute__((section(".ARM.__at_0x20000000")));
#else
static volatile std::uint32_t s_nzi_ram[NZI_SIZE/4]       __attribute__((at(NZI_START)));
#endif


// in s_nzi_ram[0]: nziramSignature or whateever ... only nziramSignature validated a command otherwise it is none
static const std::uint32_t nziramSignature = 0x12345678;

// in s_nzi_ram[1]: the command. use Command values

// in s_nzi_ram[2]: a parameter. address where to jump if Command::jump in s_nzi_ram[1]

struct embot::sys::theJumper::Impl
{       
    bool initted;
//    Config config;
    
    
    Impl() 
    {              
        reset();
    }
    
    void reset(void) 
    {
        initted = false;
//        config.restart = nullptr;
//        config.jump2 = nullptr;        
    }
    
    void *operator new(std::size_t size) throw(std::bad_alloc)
    {
        static std::uint64_t s_data_impl[(sizeof(embot::sys::theJumper::Impl)+7)/8] = {0};
        return s_data_impl;
    }
    
};




// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


void * embot::sys::theJumper::operator new(std::size_t size) throw(std::bad_alloc)
{
    static std::uint64_t s_data_jumper[(sizeof(embot::sys::theJumper)+7)/8] = {0};
    return s_data_jumper;
}

embot::sys::theJumper::theJumper()
: pImpl(new Impl)
{   
    pImpl->initted = true;
}


//bool embot::sys::theJumper::init(Config &config)
//{   

//    pImpl->config = config;
//    
//    pImpl->initted = true;
//    return true;    
//}


embot::sys::theJumper::Command embot::sys::theJumper::get(std::uint32_t& param)
{ 
    Command cmd = Command::none;
    
//    if(false == pImpl->initted)
//    {
//        return cmd;
//    }
    
    if(nziramSignature != s_nzi_ram[0])
    {
        cmd = Command::none;
    }    
    else if(static_cast<std::uint32_t>(Command::none) == s_nzi_ram[1])
    {
        cmd = Command::none;
    }   
    else if(static_cast<std::uint32_t>(Command::jump) == s_nzi_ram[1])
    {
        param = s_nzi_ram[2];
        cmd = Command::jump;
    }    
    else if(static_cast<std::uint32_t>(Command::stay) == s_nzi_ram[1])
    {
        cmd = Command::stay;
    }
    
    s_nzi_ram[0] = nziramSignature;
    s_nzi_ram[1] = static_cast<std::uint32_t>(Command::none);
    s_nzi_ram[2] = 0;

    return cmd;
}

bool embot::sys::theJumper::set(Command command, std::uint32_t param)
{     
//    if(false == pImpl->initted)
//    {
//        return false;
//    }
    
    s_nzi_ram[0] = nziramSignature;
    s_nzi_ram[1] = static_cast<std::uint32_t>(command);
    if(Command::jump == command)
    {
        s_nzi_ram[2] = param;
    }
    
    return true;
}

        
//bool embot::sys::theJumper::eval(std::uint32_t &address, bool jumpnow)
//{
//    if(false == pImpl->initted)
//    {
//        return false;
//    }
//    
//    bool ret = false;
//    
//    if((nziramSignature == s_nzi_ram[0]) && (0 != s_nzi_ram[2]))
//    {
//        address = s_nzi_ram[2];
//        ret = true;
//    }
//    
//    s_nzi_ram[0] = 0;
//    s_nzi_ram[1] = 0;
//    
//    if(ret && jumpnow)
//    {
//        jump(address);        
//        ret = true;
//    }
//           
//    return ret;
//};
        
        
bool embot::sys::theJumper::jump(std::uint32_t address)
{
//    if(false == pImpl->initted)
//    {
//        return false;
//    }
    
//    bool ret = false;    
    // it jumps to the specified address. returns false only if it cannot jump.
    
//    if(nullptr != pImpl->config.jump2)
//    {
//        pImpl->config.jump2(address);
//    }
    embot::hw::sys::jump2address(address);
    
    return false;    
}      
  

//bool embot::sys::theJumper::set(std::uint32_t address)
//{
//    if(false == pImpl->initted)
//    {
//        return false;
//    }
//    
//    bool ret = false;    
//    // it sets in permanent memory an address where to start
//    
//    s_nzi_ram[0] = 0;
//    s_nzi_ram[1] = 0;
//    
//    if(0 == address)
//    {
//        return false;
//    }
//    
//    s_nzi_ram[0] = 0x12345678;
//    s_nzi_ram[1] = address;
//    
//    return ret;    

//}   

        
bool embot::sys::theJumper::restart()
{       
//    if(false == pImpl->initted)
//    {
//        return false;
//    }
    
//    // restart the micro by calling hal_sys_restart
//    if(nullptr != pImpl->config.restart)
//    {
//        pImpl->config.restart();
//    }
    
    embot::hw::sys::reset();
    return false;   
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


