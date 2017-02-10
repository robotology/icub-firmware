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

#include "embot_hw.h"

#include "bsp.h"
#include "gpio.h"

#include "stm32l4xx_hal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include <cstring>

#include <vector>


// --------------------------------------------------------------------------------------------------------------------
// - pimpl: private implementation (see scott meyers: item 22 of effective modern c++, item 31 of effective c++
// --------------------------------------------------------------------------------------------------------------------



// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


static void s_gpio_init(void)
{
    static bool initted = false;
    if(!initted)
    {
        MX_GPIO_Init();
        initted = true;
    }
}

namespace embot { namespace hw { namespace bsp {
    
    static bool initted = false; 

    bool isinitted()
    {
        return initted;
    }

    result_t init()
    {
        if(true == embot::hw::bsp::isinitted())
        {
            return resOK;
        }
        
        // put whatwever is required for ...
        
        bsp_init();
        
        initted = true;
        return resOK;
    }
    

    result_t led_init(LED led)
    {
        if(LED::zero != led)
        {
            return resNOK;
        }        
        s_gpio_init();
        return resOK;
    }
    
    result_t led_init_par(LED led, const void *par)
    {
        if(LED::zero != led)
        {
            return resNOK;
        }        
        s_gpio_init();
        return resOK;
    }
    
    result_t led_on(LED led)
    {
        if(LED::zero != led)
        {
            return resNOK;
        }            
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
        return resOK;
    }
    
    result_t led_off(LED led)
    {
        if(LED::zero != led)
        {
            return resNOK;
        } 
        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);        
        return resOK;
    }
    
    result_t led_toggle(LED led)
    {
        if(LED::zero != led)
        {
            return resNOK;
        }         
        HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);      
        return resOK;
    }
    

    result_t btn_init(BTN btn)
    {
        if(BTN::zero != btn)
        {
            return resNOK;
        }        
        s_gpio_init();
        return resOK;        
    }
    
    bool btn_ispressed(BTN btn)
    {
        GPIO_PinState b1state = HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin);    
        return (GPIO_PIN_RESET == b1state) ? (true) : (false);        
    }
    
    
}}} // namespace embot { namespace hw { namespace bsp {


    
namespace embot { namespace hw { namespace sys {
    
    
    std::uint32_t clock()
    {
        return SystemCoreClock;
    }
    
    void reset()
    {        
        NVIC_SystemReset();
    }
    
    bool canjump2address(std::uint32_t address)
    {
        return true;
    }
    
    void jump2address(std::uint32_t address)
    {
        volatile std::uint32_t jumpaddr;
        void (*app_fn)(void) = NULL;
        
        if(false == canjump2address(address))
        {
            return;
        }


        // prepare jump address 
        jumpaddr = *(volatile std::uint32_t*) (address + 4);
        // prepare jumping function
        app_fn = (void (*)(void)) jumpaddr;
        // initialize user application's stack pointer 
        __set_MSP(*(volatile uint32_t*) address);
        // jump.
        app_fn(); 
        
        // never in here.
        return;                 
    }
    
    void relocatevectortable(std::uint32_t offset)
    {
        SCB->VTOR = FLASH_BASE | (offset & (uint32_t)0x1FFFFF80);        
    }

}}} // namespace embot { namespace hw { namespace sys { 




namespace embot { namespace hw { namespace can {
    
    const bool s_supported[static_cast<unsigned int>(PortNumber::value)] = {true, false};
        
    static bool s_initted[static_cast<unsigned int>(PortNumber::value)] = {false, false};
    
    
    static Config s_config;    
    static std::vector<Frame> *s_Qtx;
    static std::vector<Frame> *s_rxQ;
    
    
    uint8_t port2index(Port p)
    {
        return static_cast<unsigned int>(p);
    }

    bool embot::hw::can::supported(Port p)
    {
        return s_supported[port2index(p)];
    }
    
    bool embot::hw::can::initted(Port p)
    {
        return s_initted[port2index(p)];
    }


    result_t embot::hw::can::init(Port p, const Config &config)
    {
        if(false == supported(p))
        {
            return resNOK;
        }
        
        if(true == initted(p))
        {
            return resOK;
        }
        
        s_config = config;
        
        // do whatever else is required .... for instance... init the buffers.
        
        s_Qtx = new std::vector<Frame>;
        s_rxQ = new std::vector<Frame>;
        s_Qtx->reserve(config.txcapacity);
        s_rxQ->reserve(config.rxcapacity);
        
        
        return resOK;        
    }

    result_t embot::hw::can::enable(Port p)
    {
        if(true == initted(p))
        {
            return resNOK;
        }  


        // do whatever is needed
        
        return resOK;           
    }
    
    
    result_t embot::hw::can::disable(Port p)
    {
        if(true == initted(p))
        {
            return resNOK;
        }  


        // do whatever is needed
        
        return resOK;           
    }    

    result_t embot::hw::can::put(Port p, const Frame &frame)
    {
        if(true == initted(p))
        {
            return resNOK;
        }  

        
        if(s_Qtx->size() == s_config.txcapacity)
        {
            s_Qtx->erase(s_Qtx->begin());
        }
        // do whatever is needed
        
        s_Qtx->push_back(frame);
        
        return resOK;           
    }   
    
    

    std::uint8_t embot::hw::can::outputqueuesize(Port p)
    {
        if(true == initted(p))
        {
            return 0;
        }  
        
        // protect ???
        return s_Qtx->size();
    }
    
    std::uint8_t embot::hw::can::inputqueuesize(Port p)
    {
        if(true == initted(p))
        {
            return 0;
        }  
        
        // protect ???
        return s_rxQ->size();  
    }

    result_t embot::hw::can::transmit(Port p)
    {
        if(true == initted(p))
        {
            return resNOK;
        } 

        // for now we do it ... but for later we may trigger some interrupt ...
        uint8_t size = s_Qtx->size();
        for(uint8_t i=0; i<size; i++)
        {
            Frame& frame = s_Qtx->at(i);
            // use frame ...
            
            if(nullptr != s_config.ontxframe.callback)
            {
                s_config.ontxframe.callback(s_config.ontxframe.arg);
            }            
        }
        s_Qtx->clear();
               
        
        return resOK;
    }


    result_t embot::hw::can::get(Port p, Frame &frame, std::uint8_t &remaining)
    {
        if(true == initted(p))
        {
            return resNOK;
        } 
        
        frame = s_rxQ->front();
        
        s_rxQ->erase(s_rxQ->begin());
        
        remaining = s_rxQ->size();
        
        return resOK;
        
    }

    
}}} // namespace embot { namespace hw { namespace can {
    

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

