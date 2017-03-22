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
#include "stm32hal.h"


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



namespace embot { namespace hw { namespace bsp {
    
    static bool initted = false; 

    bool initialised()
    {
        return initted;
    }

    result_t init(const Config &config)
    {
        if(true == embot::hw::bsp::initialised())
        {
            return resOK;
        }
        
        // put whatwever is required for ...
        
        stm32hal_config_t cfg;
        cfg.tick1ms_get = config.get1mstick;
        cfg.initbsp = true;
        
        stm32hal_init(&cfg);
        
        initted = true;
        return resOK;
    }
    

}}} // namespace embot { namespace hw { namespace bsp {




namespace embot { namespace hw { namespace led {

    struct bspmap_t
    {
        std::uint32_t       mask;
        embot::hw::GPIO     gpio[static_cast<uint8_t>(LED::maxnumberof)];
    };
    
    // const support maps
    #if     defined(STM32HAL_BOARD_NUCLEO64)    
    
    static const bspmap_t bspmap = 
    {
        0x00000001,
        {
            {LD2_GPIO_Port, LD2_Pin},
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };
//        static const std::uint32_t              mask = 0x00000001;
//        static const embot::hw::GPIO            gpiomap[] =    
//        { 
//            {LD2_GPIO_Port, LD2_Pin} 
//        };
    #elif   defined(STM32HAL_BOARD_MTB4)
//        static const std::uint32_t              mask = 0;
//        static const embot::hw::GPIO            gpiomap[] =    
//        { 
//            {nullptr, 0} 
//        };
        #warning embot::hw::led::mask and embot::hw::led::gpiomap[] must be filled 
    #else
//        static const std::uint32_t              mask = 0;
//        static const embot::hw::GPIO            gpiomap[] =    
//        { 
//            {nullptr, 0} 
//        };
        #warning embot::hw::led::mask and embot::hw::led::gpiomap[] must be filled    
    #endif
      
    // initialised mask
        
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t led2index(LED led)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(led);
    }
        
    bool supported(LED led)
    {
        if((LED::none == led) || (LED::maxnumberof == led))
        {
            return false;
        }
        return embot::common::bit::check(bspmap.mask, led2index(led));
    }
    
    bool initialised(LED led)
    {
        if(LED::none == led)
        {
            return false;
        }
        return embot::common::bit::check(initialisedmask, led2index(led));
    }
        
    

    result_t init(LED led)
    {
        if(!supported(led))
        {
            return resNOK;
        }  
        
        if(initialised(led))
        {   // dont need to re-init
            return resOK;
        }
        
        // every led initalisation is done in the bsp ...
        
        if(!embot::hw::bsp::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
                
        embot::common::bit::set(initialisedmask, led2index(led));
        
        // we just switch it off        
        embot::hw::led::off(led);
        
        return resOK;        
    }

    
    result_t init_legacy(LED led, const void *par)
    {
        return init(led);
    }
    
    
    result_t on(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = &bspmap.gpio[led2index(led)];       
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), GPIO_PIN_SET);
        return resOK;        
    }
    
    result_t off(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = &bspmap.gpio[led2index(led)];       
        HAL_GPIO_WritePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin), GPIO_PIN_RESET);
        return resOK;          
    }
    
    result_t toggle(LED led)
    {
        if(!initialised(led))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = &bspmap.gpio[led2index(led)];       
        HAL_GPIO_TogglePin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin));
        return resOK;          
    }
    
    
}}} // namespace embot { namespace hw { namespace led 



namespace embot { namespace hw { namespace button {
    
    struct bspmap_t
    {
        std::uint32_t       mask;
        embot::hw::GPIO     gpio[static_cast<uint8_t>(BTN::maxnumberof)];
    };

    // const support maps
    #if     defined(STM32HAL_BOARD_NUCLEO64)  

    static const bspmap_t bspmap = 
    {
        0x00000001,
        {
            {B1_GPIO_Port, B1_Pin},
            {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };    

    #elif   defined(STM32HAL_BOARD_MTB4)
        #warning bspmap must be filled 
    #else
    static const bspmap_t bspmap = 
    {
        0x00000000,
        {
            {nullptr, 0, nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}, {nullptr, 0}
        }        
    };  
    #endif
      
    // initialised mask
        
    static std::uint32_t initialisedmask = 0;
    
    std::uint8_t btn2index(BTN btn)
    {   // use it only after verification of supported() ...
        return static_cast<uint8_t>(btn);
    }
        
    bool supported(BTN btn)
    {
        if((BTN::none == btn) || (BTN::maxnumberof == btn))
        {
            return false;
        }
        return embot::common::bit::check(bspmap.mask, btn2index(btn));
    }
    
    bool initialised(BTN btn)
    {
        if(BTN::none == btn)
        {
            return false;
        }
        return embot::common::bit::check(initialisedmask, btn2index(btn));
    }
        
    

    result_t init(BTN btn)
    {
        if(!supported(btn))
        {
            return resNOK;
        }  
        
        if(initialised(btn))
        {   // dont need to re-init
            return resOK;
        }
        
        // every button initalisation is done in the bsp ...
        
        if(!embot::hw::bsp::initialised())
        {   // requires embot::hw::bsp::init()
            return resNOK;
        }
                
        embot::common::bit::set(initialisedmask, btn2index(btn));
                
        return resOK;        
    }

    
    
    
    bool pressed(BTN btn)
    {
        if(!initialised(btn))
        {
            return resNOK;
        }  
        const embot::hw::GPIO *gpio = &bspmap.gpio[btn2index(btn)];       
        
        GPIO_PinState b1state = HAL_GPIO_ReadPin(static_cast<GPIO_TypeDef*>(gpio->port), static_cast<uint16_t>(gpio->pin));    
        return (GPIO_PIN_RESET == b1state) ? (true) : (false);         
    }
    
    
}}} // namespace embot { namespace hw { namespace button 


    
namespace embot { namespace hw { namespace sys {
    
    
#if     defined(STM32HAL_BOARD_NUCLEO64)
    const std::uint32_t startOfFLASH            = 0x08000000;
    const std::uint32_t addressOfBootloader     = 0x08000000;
    const std::uint32_t maxsizeOfBootloader     = 124*1024;
    const std::uint32_t addressOfStorage        = 0x0801F000;
    const std::uint32_t maxsizeOfStorage        = 4*1024;
    const std::uint32_t addressOfApplication    = 0x08020000;
    const std::uint32_t maxsizeOfApplication    = 128*1024;
#elif   defined(STM32HAL_BOARD_MTB4)

#endif

 
#if     defined(STM32HAL_BOARD_NUCLEO64) || defined(STM32HAL_BOARD_MTB4)
    
__asm static void ss_hl_sys_asm_xnumARMv7ops(uint32_t numberof) 
{
   align
dowaitloop
   subs r0,r0,#1
   bne dowaitloop
   bx lr 
   align    
}

    static void ss_bsp_delay(uint64_t t)
    {   
        static uint64_t s_hl_sys_numofops1sec = 0;
        static uint32_t s_hl_sys_used_systemcoreclock = 0;
        if(s_hl_sys_used_systemcoreclock != SystemCoreClock)
        {

            // to occupy a millisec i execute an operation for a number of times which depends on: 
            // SystemCoreClock, cortex gain(1.25 dmips/mhz), flash access, etc.
            // to overcome all this i just consider SystemCoreClock w/out 1.25 gain and i measures
            // extra gain with on a simple assembly function which should take 4 cycles per iteration (?).      
            //s_hl_sys_numofops1sec = (5*(SystemCoreClock)) / 4; 
            s_hl_sys_used_systemcoreclock = SystemCoreClock;
            s_hl_sys_numofops1sec = SystemCoreClock;
            
            // with art technology enabled the flash is seen as fast as the cpu. wow. 
            s_hl_sys_numofops1sec /= 3;             


            // at this point i normalise the variable to keep not the nymber of operations for 1 sec,
            // but for 1024*1024 microsec. by doing so, later on i shift by 20 instead of using a division. 
            s_hl_sys_numofops1sec <<= 20;
            s_hl_sys_numofops1sec /= 1000000;
        }
        
        
        volatile uint64_t num = s_hl_sys_numofops1sec * t;
        num >>= 20; 
        //num -= offset; //we may remove some cycles to compensates for previous instructions, but ... we dont do it. it depends on c compiler optimisation 
        if(0 == num)
        {
            return;
        }
        ss_hl_sys_asm_xnumARMv7ops((uint32_t)num);
    }
    
#endif
    
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


    void delay(embot::common::Time t)
    {   
        ss_bsp_delay(t);
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

