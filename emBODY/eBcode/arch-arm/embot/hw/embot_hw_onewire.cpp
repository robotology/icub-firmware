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

#include "embot_hw_onewire.h"




// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "stm32hal.h"
#include "embot_sys.h"
#include "embot_hw_timer.h"
#include <cstring>
#include <vector>

using namespace std;

#include "embot_binary.h"

// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

//#if 0
// 
//// in here we manage the case of no tim / gpio module being present in stm32hal. 
//// or also for board not strain2
// 
//namespace embot { namespace hw { namespace onewire {
// 
//    bool supported(Channel c)                                                                           { return false; }
//    bool initialised(Channel c)                                                                         { return false; }
//    result_t init(Channel c, const Config &config)                                                      { return resNOK; }
//    bool isrunning(Channel c)                                                                           { return false; }
//    result_t write(Channel c, std::uint8_t reg, std::uint16_t value, embot::common::relTime timeout)    { return resNOK; }
// 
//}}} // namespace embot { namespace hw { namespace onewire {
// 
//#else


// there are two parts: 
// 1. the cpp driver under namespace embot::hw::onewire
// 2. the functions which stm library required to be redefined: IRQ handlers and callbacks 

// the implementation uses one timer, TIM6, which gives timing for writing over a one-wire interface
// each channel uses a well defined gpio

// we implement one wire communication using a interrrupt-driven bit-banging technique, where timing is done using a dedicated hw timer

namespace embot { namespace hw { namespace onewire {
        
    struct bspmap_t
    {
        std::uint32_t       mask;
    };
    
    // const support maps
    #if     defined(STM32HAL_BOARD_STRAIN2)    
    
    static const bspmap_t bspmap = 
    {
        0x0000003F  // means... 6 channels
    };
   
    #else
        static const bspmap_t bspmap = 
        {
            0x00000000
        };
    #endif
      
    // initialised mask       
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t channel2index(Channel c)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(c);
    }
        
    bool supported(Channel c)
    {
        if((Channel::none == c) || (Channel::maxnumberof == c))
        {
            return false;
        }
        return embot::binary::bit::check(bspmap.mask, channel2index(c));
    }
    
    bool initialised(Channel c)
    {
        if(Channel::none == c)
        {
            return false;
        }
        return embot::binary::bit::check(initialisedmask, channel2index(c));
    }    

    
    static const embot::hw::timer::Timer tickingTimer = embot::hw::timer::Timer::six;
    struct PrivateData
    {
        volatile bool transaction_isrunning;
        bool tickingTimerIsInitted;
        
        std::uint64_t bits64;       // the bits already with start, stop, data all placed in their correct places   
        std::uint8_t bitsnumber;    // the number of bits: typicall 10*4 -> <0, 0x55, 1> <0, reg, 1> <0, data, 1> <0, data, 1>,
        std::uint8_t bitindex;      // the position of the bit64 to tx
        
        Channel activechannel;
                
        embot::hw::gpio::GPIO activegpio;
        
        Config config[static_cast<unsigned int>(Channel::maxnumberof)];
        
        PrivateData() { tickingTimerIsInitted = false; transaction_isrunning = false; activechannel = Channel::none; }
    };
    
    static PrivateData s_privatedata;
    
   
    // used just for test...
    // static std::uint8_t testbuffer[40] = {0};
   
    void callbackOnTick(void *arg)
    {
        // the timer callback executes this code. now we do actions depending on what is inside s_privatedata. 
              
        // we just need to set the gpio low or high depending on bits64 value.
        // we do it for max bitsnumber values.
       
        if(s_privatedata.bitindex < s_privatedata.bitsnumber)
        {
            // set s_privatedata.bitindex
            bool high = embot::binary::bit::check(s_privatedata.bits64, s_privatedata.bitindex);
            embot::hw::gpio::set(s_privatedata.activegpio, (true == high) ? (embot::hw::gpio::State::SET) : (embot::hw::gpio::State::RESET));
            //testbuffer[s_privatedata.bitindex] = high;
            s_privatedata.bitindex++;
        }
        else if(s_privatedata.bitindex >= s_privatedata.bitsnumber)
        {
            // we are after the last stop bit .... we return.
            embot::hw::gpio::set(s_privatedata.activegpio, embot::hw::gpio::State::SET);
            s_privatedata.bitsnumber = 0;
            s_privatedata.bitindex = 0;
            s_privatedata.bits64 = 0xffffffffffffffff; // just in case.
           
            embot::hw::timer::stop(tickingTimer);            
            s_privatedata.transaction_isrunning = false;     
            s_privatedata.activegpio.load(nullptr, 0);    
            s_privatedata.activechannel = Channel::none;           
        }
      
    }
    
   
    result_t init(Channel c, const Config &config)
    {
        if(false == supported(c))
        {
            return resNOK;
        }
        
        if(true == initialised(c))
        {
            return resOK;
        }
        
        s_privatedata.config[channel2index(c)] = config;
                        
        // init peripherals: gpio and timer TIM6
        // gpio init is globally initted by stm32hal_bsp_init() because it hold all gpio peripherals
        // MX_GPIO_Init();
        // however, we must config it as output and put it high 
        embot::hw::gpio::configure(config.gpio, embot::hw::gpio::Mode::OUTPUTopendrain);
        embot::hw::gpio::set(config.gpio, embot::hw::gpio::State::SET);
        
        if(false == s_privatedata.tickingTimerIsInitted)
        {
            embot::hw::timer::Config cc;
            cc.time = embot::common::time1millisec / static_cast<std::uint32_t>(config.rate);
            cc.mode = embot::hw::timer::Mode::periodic;
            cc.onexpiry.callback = callbackOnTick;
            cc.onexpiry.arg = nullptr;
            embot::hw::timer::init(tickingTimer, cc);
            s_privatedata.tickingTimerIsInitted = true;
            s_privatedata.transaction_isrunning = false;
            s_privatedata.activechannel = Channel::none;
        }
        
        
        embot::binary::bit::set(initialisedmask, channel2index(c));

        return resOK;
    }
    
    bool isrunning(Channel c)
    { 
        if(false == initialised(c))
        {
            return false;
        }
        
        return s_privatedata.transaction_isrunning;
    }
 

/*  
    from datasheet of texas instruments PGA308, Figure 4-2. One-Wire Protocol Timing Diagram
    Write to PGA308 Register Timing


bit pos 0   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15  16  17  18  19
            ---     ---     ---     ---     ===     --- --- --- ---                 ===
        STA| 1 | 0 | 1 | 0 | 1 | 0 | 1 | 0 |STO|STA|P00|P01|P02|P03| 0 | 0 | 0 | 0 |STO
        ===     ---     ---     ---     ---     === --- --- --- --- --- --- --- --- 
    
            initialization byte (55h)               register write command for RAM             

               
bit pos 20  21  22  23  24  25  26  27  28  29  30  31  32  33  34  35  36  37  38  39    
            --- --- --- --- --- --- --- --- ===     --- --- --- --- --- --- --- --- ===
        STA|D00|D01|D02|D03|D04|D05|D06|D07|STO|STA|D08|D09|D10|D11|D12|D13|D14|D15|STO
        === --- --- --- --- --- --- --- ---     === --- --- --- --- --- --- --- --- 


            register data (8 LSBs)                  register data (8 MSBs)               
    
    constant part:
    STA(0) are in positions:    00, 10, 20, 30
    STO(1) are in positions:    09, 19, 29, 39
    0x55 is in positions:       [01-08]
    so far, ordered with bitpos 0 at right (as in memory):
    (1000 0000 0010 0000 0000 1000 0000 0010 1010 1010)b = 0x80200802aa
    
    variable part:
    reg is in positions:        [11-18] but its first nibble is actually in [11-14] 
    [D00-D07] are in positions: [21-28]
    [D08-D15]                   [31-38]
    

 */    
 
    #define USEBITS64PRECOMPUTED

    result_t write(Channel c, std::uint8_t reg, std::uint16_t value, embot::common::relTime timeout)
    {
        if(false == initialised(c))
        {
            return resNOK;
        }

        // prepare the buffer, start the tim. wait until we are done. impose a timeout of ... however
                        
        if(true == s_privatedata.config[channel2index(c)].usepreamble)
        {  

            std::uint64_t msk64 = 0; 
            
#if defined(USEBITS64PRECOMPUTED) 
            // it has stop bits in positions 9, 19, 29, 39 and 0x55 in positions [1-8] 
            s_privatedata.bits64 = 0x80200802aa;    
#else  
            i dont use this code anymore
            // i set all bits to zero.            
            s_privatedata.bits64 = 0;
            
            // clr start bits: 0, 10, 20, 30
            // dont do that: they already have zero value
            
            // set stop bits: 9, 19, 29, 39
            embot::binary::bit::set(s_privatedata.bits64, 9);
            embot::binary::bit::set(s_privatedata.bits64, 19);
            embot::binary::bit::set(s_privatedata.bits64, 29);
            embot::binary::bit::set(s_privatedata.bits64, 39);
            
            // assign 0x55 to bits [1-8]            
            msk64 = static_cast<std::uint64_t>(0x55) << 1;
            embot::binary::mask::set(s_privatedata.bits64, msk64);
            
            // so far we have 0x80200802aa (confirmed with the debugger)
#endif

            // assign reg & 0x0f to bits [11-18] 
            msk64 = static_cast<std::uint64_t>(reg & 0x0f) << 11;
            embot::binary::mask::set(s_privatedata.bits64, msk64);
            
            // assign value & 0xff to bits [21-28]
            msk64 = static_cast<std::uint64_t>(value & 0xff) << 21;
            embot::binary::mask::set(s_privatedata.bits64, msk64);
            
            // assign (value >> 8) & 0xff to bits [31-38]
            msk64 = static_cast<std::uint64_t>((value >> 8) & 0xff) << 31;
            embot::binary::mask::set(s_privatedata.bits64, msk64);            

            

            s_privatedata.bitindex = 0;
            s_privatedata.bitsnumber = 40;    

            s_privatedata.activegpio = s_privatedata.config[channel2index(c)].gpio;    
            s_privatedata.activechannel = c;             

            s_privatedata.transaction_isrunning = true;
        }
        else
        {
            return resNOK;
        }
        
        // ok: the timer starts.
        embot::hw::timer::start(tickingTimer);
        
        
        embot::common::Time start = embot::sys::timeNow();
        result_t res = resOK;
        for(;;)
        {
            if(false == s_privatedata.transaction_isrunning)
            {
                break;
            }
            if((embot::common::timeWaitForever != timeout) && ((embot::sys::timeNow() - start) > timeout))
            {
                res = resNOK;
                // stop timer, set bit high
                embot::hw::timer::stop(tickingTimer);
                embot::hw::gpio::set(s_privatedata.activegpio, embot::hw::gpio::State::SET);
                s_privatedata.activegpio.load(nullptr, 0);

                s_privatedata.bitsnumber = 0;
                s_privatedata.bitindex = 0;
                s_privatedata.bits64 = 0xffffffffffffffff; // just in case.
                s_privatedata.transaction_isrunning = false;  
                s_privatedata.activechannel = Channel::none;                 
                
                break;
            }
        }
        
        return res;              
    }
 
    
}}} // namespace embot { namespace hw { namespace onewire {


// - stm32hal.lib needs some handlers being compiled in here: IRQ handlers and callbacks.


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

