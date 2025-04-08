
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/



// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_dualcore.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_dualcore_bsp.h"
#include "embot_hw_sys.h"

#include <cstring>
#include <array>
#include "embot_core_binary.h"


#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace embot::hw;

namespace embot::hw::dualcore {
   
    
    const char *to_string(CORE c)
    {
        static constexpr std::array<const char *, embot::core::tointegral(CORE::maxnumberof)+1> cores
        {
            "CORE::cm4", "CORE::cm7", "CORE::none"
        };
        
        uint8_t i = embot::core::tointegral(c);
        if(i < embot::core::tointegral(CORE::maxnumberof))
        {
            return cores[i];
        }
        
        return cores[embot::core::tointegral(CORE::maxnumberof)];                    
    }
    
    const char *to_string(BOOT b)
    {
        static constexpr std::array<const char *, embot::core::tointegral(BOOT::maxnumberof)+1> boots
        {
            "BOOT::cm4master", "BOOT::cm7master", "BOOT::none"
        };
        
        uint8_t i = embot::core::tointegral(b);
        if(i < embot::core::tointegral(BOOT::maxnumberof))
        {
            return boots[i];
        }
        
        return boots[embot::core::tointegral(BOOT::maxnumberof)];     
    }  
    
} // namespace embot::hw::dualcore {


#if !defined(EMBOT_ENABLE_hw_dualcore)
    #define useDUMMYimpl_of_embot_hw_dualcore
#endif


#if defined(useDUMMYimpl_of_embot_hw_dualcore)

namespace embot::hw::dualcore {


    bool supported()
    { return false; }

    CORE core()
    { return CORE::none; }
    
    BOOT boot()
    { return BOOT::none; }

    bool ismaster()
    { return false; }
    
    bool config(const Config &c)
    { return false; }
    
    bool init()
    { return false; }             
    
} // namespace embot::hw::dualcore {

#elif !defined(useDUMMYimpl_of_embot_hw_dualcore)

namespace embot::hw::dualcore {

    // the public functions
    
    bool supported()
    {
        return embot::hw::dualcore::bsp::getBSP().supported();
    }
    
    BOOT boot()
    {
        if(false == supported())
        {
            return BOOT::none;
        }
        return embot::hw::dualcore::bsp::getBSP().getPROP()->boot;
    }

    CORE core()
    {
        if(false == supported())
        {
            return CORE::none;
        }
        return embot::hw::dualcore::bsp::getBSP().getPROP()->core;
    }
    
    bool ismaster()
    { 
        if(false == supported())
        {
            return false;
        }
        
        return embot::hw::dualcore::bsp::getBSP().ismaster();
    }
    
    bool config(const Config &c)
    {
        if(false == supported())
        {
            return false;
        }
        return embot::hw::dualcore::bsp::getBSP().config(c);
    }    
     
    void waitHWmutex(uint32_t mtx)
    {
        volatile uint32_t m = mtx;
        for(;;)
        {
            if(HAL_OK == HAL_HSEM_FastTake(m))
            {
                break;
            }
        }        
    }    

    bool init()
    {
        if(false == supported())
        {
            return false;
        }
        const Config &cfg = embot::hw::dualcore::bsp::getBSP().config();
        
        embot::hw::dualcore::CORE co = embot::hw::dualcore::bsp::getBSP().getPROP()->core;
        embot::hw::dualcore::BOOT bo = embot::hw::dualcore::bsp::getBSP().getPROP()->boot;
        embot::hw::MTX mtx = embot::hw::dualcore::bsp::getBSP().getPROP()->mtx;
        uint32_t hsem = embot::core::tointegral(mtx);
        
        uint32_t other_RCC_BOOT_Cx = (embot::hw::dualcore::CORE::cm7 == co) ? RCC_BOOT_C2 : RCC_BOOT_C1;
        bool IamMaster = ((embot::hw::dualcore::CORE::cm7 == co) && (embot::hw::dualcore::BOOT::cm7master == bo)) ||
                         ((embot::hw::dualcore::CORE::cm4 == co) && (embot::hw::dualcore::BOOT::cm4master == bo));
                     
#if 1
        if(true == IamMaster)
        {
            // i init the hw for both
            // then i process the acttion for the other core

            if(embot::hw::dualcore::Config::HW::forceinit == cfg.hw)
            {
                embot::hw::dualcore::bsp::getBSP().init();
            }
            
            SystemCoreClockUpdate();
            
            // and now i process the command for the other core

            constexpr uint32_t procID0 {0};
            
            switch(cfg.othercore)
            {
                case embot::hw::dualcore::Config::CMD::activateandhold:
                {
                    // 1. init the hsems and take hsem-0
                    __HAL_RCC_HSEM_CLK_ENABLE();
                    HAL_HSEM_FastTake(hsem);

                    // 2. enable the other core C2
                    HAL_RCCEx_EnableBootCore(other_RCC_BOOT_Cx);                
                } break;

                case embot::hw::dualcore::Config::CMD::release:
                {
                    // 1. init the hsems (just in case) and release hsem-0
                    __HAL_RCC_HSEM_CLK_ENABLE();
                    HAL_HSEM_Release(hsem, procID0);                       
                } break;

                case embot::hw::dualcore::Config::CMD::activate:
                {
                    // 1. init the hsems and take hsem-0
                    __HAL_RCC_HSEM_CLK_ENABLE();
                    HAL_HSEM_FastTake(hsem);

                    // 2. enable the other core C2
                    HAL_RCCEx_EnableBootCore(other_RCC_BOOT_Cx);  

                    // 3. wait a bit to allow the other core to wake up, bootstrap and execute until the holding of the semaphore 
                    //    it is not necessary but it is nice to proceed knowing that the other core (if good code is present) has the
                    //    hw::bsp initted and is waiting for the semaphore to be released
                    constexpr embot::core::relTime tinywait {20*embot::core::time1microsec};                    
                    embot::hw::sys::delay(tinywait);                    

                    // 4. and release hsem-0
                    HAL_HSEM_Release(hsem, procID0);
                    
                } break;  
                
                case embot::hw::dualcore::Config::CMD::donothing:
                default: 
                {
                    break;
                }
            }              
            
            
        }
        else
        {
            // just wait until the master core activates the slave core and unlocks the HSEM 0
            __HAL_RCC_HSEM_CLK_ENABLE();
            waitHWmutex(hsem);
            
            // then i do some hw init specific to me
            if(embot::hw::dualcore::Config::HW::forceinit == cfg.hw)
            {
                embot::hw::dualcore::bsp::getBSP().init();
            }
            
            SystemCoreClockUpdate();            
        }

#else        
        if((embot::hw::dualcore::CORE::cm7 == co) && (embot::hw::dualcore::BOOT::cm7master == bo))
        {
            if(embot::hw::dualcore::Config::HW::forceinit == cfg.hw)
            {
                embot::hw::dualcore::bsp::getBSP().start();
            }
            
            SystemCoreClockUpdate();
            
            // and now i process the command for the other core

            constexpr uint32_t procID0 {0};
            
            switch(cfg.othercore)
            {
                case embot::hw::dualcore::Config::CMD::activateandhold:
                {
                    // 1. init the hsems and take hsem-0
                    __HAL_RCC_HSEM_CLK_ENABLE();
                    HAL_HSEM_FastTake(hsem);

                    // 2. enable the other core C2
                    HAL_RCCEx_EnableBootCore(other_RCC_BOOT_Cx);                
                } break;

                case embot::hw::dualcore::Config::CMD::release:
                {
                    // 1. init the hsems (just in case) and release hsem-0
                    __HAL_RCC_HSEM_CLK_ENABLE();
                    HAL_HSEM_Release(hsem, procID0);                       
                } break;

                case embot::hw::dualcore::Config::CMD::activate:
                {
                    // 1. init the hsems and take hsem-0
                    __HAL_RCC_HSEM_CLK_ENABLE();
                    HAL_HSEM_FastTake(hsem);

                    // 2. enable the other core C2
                    HAL_RCCEx_EnableBootCore(other_RCC_BOOT_Cx);   
                    
                    // 3. wait a bit to allow the other core to wake up, bootstrap and execute until the holding of the semaphore 
                    //    it is not necessary but it is nice to proceed knowing that the other core (if good code is present) has the
                    //    hw::bsp initted and is waiting for the semaphore to be released
                    constexpr embot::core::relTime tinywait {20*embot::core::time1microsec};                    
                    embot::hw::sys::delay(tinywait); 

                    // 4. and release hsem-0
                    HAL_HSEM_Release(hsem, procID0);
                    
                } break;  
                
                case embot::hw::dualcore::Config::CMD::donothing:
                default: 
                {
                    break;
                }
            }            
        }
        else if((embot::hw::dualcore::CORE::cm7 == co) && (embot::hw::dualcore::BOOT::cm4master == bo))
        {

            // just wait until the master core activates the slave core and unlocks the HSEM 0
            __HAL_RCC_HSEM_CLK_ENABLE();
            waitHWmutex(hsem);
            
            // then i do some hw init
            if(embot::hw::dualcore::Config::HW::forceinit == cfg.hw)
            {
                embot::hw::dualcore::bsp::getBSP().start();
            }
            
            SystemCoreClockUpdate();
                                  
        }
        else if((embot::hw::dualcore::CORE::cm4 == co) && (embot::hw::dualcore::BOOT::cm4master == bo))
        {
            if(embot::hw::dualcore::Config::HW::forceinit == cfg.hw)
            {
                embot::hw::dualcore::bsp::getBSP().start();
            }
            
            SystemCoreClockUpdate();
            
            
            // and now i process the command for the other core

            constexpr uint32_t procID0 {0};
            
            switch(cfg.othercore)
            {
                case embot::hw::dualcore::Config::CMD::activateandhold:
                {
                    // 1. init the hsems and take hsem-0
                    __HAL_RCC_HSEM_CLK_ENABLE();
                    HAL_HSEM_FastTake(hsem);

                    // 2. enable the other core c1
                    HAL_RCCEx_EnableBootCore(other_RCC_BOOT_Cx);                
                } break;

                case embot::hw::dualcore::Config::CMD::release:
                {
                    // 1. init the hsems (just in case) and release hsem-0
                    __HAL_RCC_HSEM_CLK_ENABLE();
                    HAL_HSEM_Release(hsem, procID0);                       
                } break;

                case embot::hw::dualcore::Config::CMD::activate:
                {
                    // 1. init the hsems and take hsem-0
                    __HAL_RCC_HSEM_CLK_ENABLE();
                    HAL_HSEM_FastTake(hsem);

                    // 2. enable the other core c1
                    HAL_RCCEx_EnableBootCore(other_RCC_BOOT_Cx);   
                    
                    // 3. wait a bit to allow the other core to wake up, bootstrap and execute until the holding of the semaphore 
                    //    it is not necessary but it is nice to proceed knowing that the other core (if good code is present) has the
                    //    hw::bsp initted and is waiting for the semaphore to be released
                    constexpr embot::core::relTime tinywait {20*embot::core::time1microsec};                    
                    embot::hw::sys::delay(tinywait); 

                    // 4. and release hsem-0
                    HAL_HSEM_Release(hsem, procID0);
                    
                } break;  
                
                case embot::hw::dualcore::Config::CMD::donothing:
                default: 
                {
                    break;
                }
            }            

        }
        else if((embot::hw::dualcore::CORE::cm4 == co) && (embot::hw::dualcore::BOOT::cm7master == bo))
        {
            // just wait until the master core activates the slave core and unlocks the HSEM 0
            __HAL_RCC_HSEM_CLK_ENABLE();
            waitHWmutex(hsem);
            
            // then i do some hw init
            if(embot::hw::dualcore::Config::HW::forceinit == cfg.hw)
            {
                embot::hw::dualcore::bsp::getBSP().start();
            }
            
            SystemCoreClockUpdate();            
        }

#endif
        
        return true;  
    }       
    
} // namespace embot::hw::dualcore {

 


#endif // !defined(useDUMMYimpl_of_embot_hw_dualcore)


    



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

