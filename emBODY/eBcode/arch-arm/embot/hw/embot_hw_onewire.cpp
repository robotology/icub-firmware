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
#include "stm32hal.h"
#include "embot_sys.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>
#include <vector>

using namespace std;


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

#if     !defined(HAL_GPIO_MODULE_ENABLED) || !defined(HAL_TIM_MODULE_ENABLED) || !defined(STM32HAL_BOARD_STRAIN2)



// in here we manage the case of no tim / gpio module being present in stm32hal. 
// or also for board not strain2

namespace embot { namespace hw { namespace onewire {

    bool supported(Channel c)                                                                           { return false; }
    bool initialised(Channel c)                                                                         { return false; }
    result_t init(Channel c, const Config &config)                                                      { return resNOK; }
    bool isrunning(Channel c)                                                                           { return false; }
    result_t write(Channel c, std::uint8_t reg, std::uint16_t value, embot::common::relTime timeout)    { return resNOK; }

}}} // namespace embot { namespace hw { namespace onewire {

#else


// there are two parts: 
// 1. the cpp driver under namespace embot::hw::onewire
// 2. the functions which stm library required to be redefined: IRQ handlers and callbacks 

// the implementation uses one timer, TIM6, which gives timing for writing over a one-wire interface
// each channel uses a well defined gpio

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
        return embot::common::bit::check(bspmap.mask, channel2index(c));
    }
    
    bool initialised(Channel c)
    {
        if(Channel::none == c)
        {
            return false;
        }
        return embot::common::bit::check(initialisedmask, channel2index(c));
    }    

    
    struct PrivateData
    {
        volatile bool transaction_isrunning;
        bool tim6_initted;
        
        std::uint64_t bits64;       // the bits already with start, stop, data all placed in their correct places   
        std::uint8_t bitsnumber;    // the number of bits: typicall 10*4 -> <0, 0x55, 1> <0, reg, 1> <0, data, 1> <0, data, 1>,
        std::uint8_t bitindex;      // the position of the bit64 to tx
        
        Channel activechannel;
                
        embot::hw::GPIO activegpio;
        
        Config config[static_cast<unsigned int>(Channel::maxnumberof)];
        
        PrivateData() { tim6_initted = false; transaction_isrunning = false; activechannel = Channel::none; }
    };
    
    static PrivateData s_privatedata;;
    
   
 

    void gpio_configure_out(const embot::hw::GPIO &gpio)
    {
        LL_GPIO_SetPinMode(static_cast<GPIO_TypeDef *>(gpio.port), gpio.pin, LL_GPIO_MODE_OUTPUT);		
        LL_GPIO_SetPinOutputType(static_cast<GPIO_TypeDef *>(gpio.port), gpio.pin, LL_GPIO_OUTPUT_OPENDRAIN);        
    }


    
    void gpio_out_set_high(const embot::hw::GPIO &gpio)
    {
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef *>(gpio.port), gpio.pin, GPIO_PIN_SET);
    }
    
    void gpio_out_set(const embot::hw::GPIO &gpio, GPIO_PinState state)
    {
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef *>(gpio.port), gpio.pin, state);
    }
    
    
    void callbackOnTick(TIM_HandleTypeDef *htim)
    {
       // the timer callback executes this code. now we do actions depending on what is inside s_privatedata. 
       
       // we just need to set the gpio low or high depending on bits64 value.
       // we do it for max bitsnumber values.
       
       if(s_privatedata.bitindex < s_privatedata.bitsnumber)
       {
            // set s_privatedata.bitindex
            bool high = embot::common::bit::check(s_privatedata.bits64, s_privatedata.bitindex);
            gpio_out_set(s_privatedata.activegpio, (true == high) ? (GPIO_PIN_SET) : (GPIO_PIN_RESET));
            s_privatedata.bitindex++;
       }
       else if(s_privatedata.bitindex >= s_privatedata.bitsnumber)
       {
            // we are after the last stop bit .... we return.
            gpio_out_set(s_privatedata.activegpio, GPIO_PIN_SET);
            s_privatedata.bitsnumber = 0;
            s_privatedata.bitindex = 0;
            s_privatedata.bits64 = 0xffffffffffffffff; // just in case.
           
            HAL_TIM_Base_Stop_IT(htim);           
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
        gpio_configure_out(config.gpio);
        gpio_out_set(config.gpio, GPIO_PIN_SET);
        
        if(false == s_privatedata.tim6_initted)
        {
            MX_TIM6_Init();
            s_privatedata.tim6_initted = true;
            s_privatedata.transaction_isrunning = false;
            s_privatedata.activechannel = Channel::none;
        }
        
        
        embot::common::bit::set(initialisedmask, channel2index(c));

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
 
    

    result_t write(Channel c, std::uint8_t reg, std::uint16_t value, embot::common::relTime timeout)
    {
        if(false == initialised(c))
        {
            return resNOK;
        }

        // prepare the buffer, start the tim. wait until we are done. impose a timeout of ... however
        
        if(true == s_privatedata.config[channel2index(c)].usepreamble)
        {            
            s_privatedata.bits64 = 0;
            // set bits 9, 19, 29, 39
            embot::common::bit::set(s_privatedata.bits64, 9);
            embot::common::bit::set(s_privatedata.bits64, 19);
            embot::common::bit::set(s_privatedata.bits64, 29);
            embot::common::bit::set(s_privatedata.bits64, 39);
            // set the four masks: 0x55 << 1, etc.
            std::uint64_t msk64 = 0;
            msk64 = static_cast<std::uint64_t>(0x55) << 1;
            embot::common::msk::set(s_privatedata.bits64, msk64);
            msk64 = static_cast<std::uint64_t>(reg & 0x0f) << 11;
            embot::common::msk::set(s_privatedata.bits64, msk64);
            msk64 = static_cast<std::uint64_t>(value & 0xff) << 21;
            embot::common::msk::set(s_privatedata.bits64, msk64);
            msk64 = static_cast<std::uint64_t>((value >> 8) & 0xff) << 31;
            embot::common::msk::set(s_privatedata.bits64, msk64);
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
        HAL_TIM_Base_Start_IT(&htim6);
        
        
        embot::common::relTime start = embot::sys::timeNow();
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
                HAL_TIM_Base_Stop_IT(&htim6);
                gpio_out_set(s_privatedata.config[channel2index(c)].gpio, GPIO_PIN_SET);
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


void TIM6_DAC_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim6);
}


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(TIM6 != htim->Instance)
    { 
        return;
    }
    
    embot::hw::onewire::callbackOnTick(htim);
  
}


#endif //else .... !defined(HAL_GPIO_MODULE_ENABLED) || !defined(HAL_TIM_MODULE_ENABLED) ! defined(STM32HAL_BOARD_STRAIN2)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

