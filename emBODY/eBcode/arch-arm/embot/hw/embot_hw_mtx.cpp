
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_mtx.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_mtx_bsp.h"

#include <cstring>
#include <vector>
#include "embot_core_binary.h"


#if defined(USE_STM32HAL)
    #include "stm32hal.h"
#else
    #warning this implementation is only for stm32hal
#endif


using namespace embot::hw;


// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------



#if !defined(EMBOT_ENABLE_hw_mtx)

namespace embot::hw::mtx {
    
    bool supported(embot::hw::MTX m)
    { return false; }   
    result_t init(embot::hw::MTX m)
    { return resNOK; }  
    bool initialised(embot::hw::MTX m)
    { return false; }    
    bool take(embot::hw::MTX m, embot::core::relTime timeout)
    { return false; }       
    void release(embot::hw::MTX m)
    { }
    bool check(embot::hw::MTX m)
    { return false; }       
    bool subscribe(embot::hw::MTX m, const embot::hw::Subscription &onrelease)
    { return false; }      
    
}   // namespace embot::hw::mtx {

#elif defined(EMBOT_ENABLE_hw_mtx)

#include "embot_hw_utils.h"

namespace embot::hw::mtx {
    
    struct mtxDRV : public embot::hw::utils::coreDRV
    {
        static constexpr uint8_t pidid0 {0};
        static constexpr uint8_t capacity {embot::core::tointegral(embot::hw::MTX::maxnumberof)};
        
        std::array<bsp::hwSEM, capacity> hwsems {};       
        std::array<embot::hw::Subscription, capacity> subscriptions {}; 
            
//        uint32_t notificationmask {0};

        constexpr mtxDRV() = default;
        constexpr mtxDRV(uint32_t m) : coreDRV(m) {}  

        constexpr bool initialised(embot::hw::MTX m) const
        {
            return coreDRV::initialised(embot::core::tointegral(m));
        }
        
        void setinitialised(embot::hw::MTX m, bool on)
        {
            if(true == supported(m))
            {
                coreDRV::setinitialised(embot::core::tointegral(m), on);
                hwsems[embot::core::tointegral(m)] = embot::hw::mtx::bsp::getBSP().getPROP(m)->hsem;
            }
        }
        
        bsp::hwSEM gethsem(embot::hw::MTX m) const
        {
            return (true == initialised(m)) ? hwsems[embot::core::tointegral(m)] : 33;
        }
            
        bool setnotification(embot::hw::MTX m, const embot::hw::Subscription &sub)
        {            
            subscriptions[embot::core::tointegral(m)] = sub;
            bool on = sub.isvalid();
//            if(true == on)
//            {
//                embot::core::binary::bit::set(notificationmask, embot::core::tointegral(m));
//            }
//            else
//            {
//                embot::core::binary::bit::clear(notificationmask, embot::core::tointegral(m));
//            }
            
            return on;
        }         

//        constexpr bool notification(embot::hw::MTX m) const
//        {
//            return embot::core::binary::bit::check(notificationmask, embot::core::tointegral(m));
//        }        
        
        bool onrelease(embot::hw::MTX m)
        {
            bool retrigger {false}; 
            if(true == initialised(m))
            {
                subscriptions[embot::core::tointegral(m)].callback.execute();
                retrigger = (Subscription::MODE::permanent == subscriptions[embot::core::tointegral(m)].mode) ? true : false;
            } 
            return retrigger;            
        }        
    };
    
    
    
    // the driver
    mtxDRV _mtxdriver {};
    
        
    bool supported(embot::hw::MTX m)
    {
        return embot::hw::mtx::bsp::getBSP().supported(m);;
    }
   
    
    bool initialised(embot::hw::MTX m)
    {
        return _mtxdriver.initialised(m);
    }   
 
    
    result_t init(embot::hw::MTX m)
    {
        if(false == supported(m))
        {
            return resNOK;
        }
        
        if(true == initialised(m))
        {
            return resOK;
        }
        
        // init peripheral... actually it is done inside general bsp
        embot::hw::mtx::bsp::getBSP().init(m);
                    
        _mtxdriver.setinitialised(m, true);
        
        return resOK;    
    }

    
    bool take(embot::hw::MTX m, embot::core::relTime timeout)
    {        
        bool r {false};
        
        if(false == initialised(m))
        {
            return r;
        }
        
        embot::core::Time expiry {embot::core::timeNone};
        
        if(embot::core::reltimeWaitForever != timeout)
        {
            expiry = embot::core::now() + timeout;
        }
        
        uint32_t hsem = _mtxdriver.gethsem(m);
        
        for(;;)
        {
            if(HAL_OK == HAL_HSEM_Take(hsem, _mtxdriver.pidid0))
            {
                r = true;
                break;
            }
            else if((embot::core::timeNone != expiry) && (embot::core::now() > expiry))
            {   // timeout sadly happens
                break;
            }
        }
        
        return r; 
    }
        
    
    void release(embot::hw::MTX m)
    {
        if(false == initialised(m))
        {
            return;
        }
        
        uint32_t hsem = _mtxdriver.gethsem(m);
        HAL_HSEM_Release(hsem, _mtxdriver.pidid0);    
    } 
    
    
    bool check(embot::hw::MTX m)
    {
        bool r {false};

        if(false == initialised(m))
        {
            return r;
        }
        uint32_t hsem = _mtxdriver.gethsem(m);
        
        r = (0 == HAL_HSEM_IsSemTaken(hsem)) ? false : true;   

        return r;        
    }        
      

    bool subscribe(embot::hw::MTX m, const embot::hw::Subscription &onrelease)
    {       
        if(false == initialised(m))
        {
            return false;
        }

        uint32_t hsem = _mtxdriver.gethsem(m);        
        uint32_t msk = __HAL_HSEM_SEMID_TO_MASK(hsem); 
        
        if(true == _mtxdriver.setnotification(m, onrelease))
        {
            HAL_HSEM_ActivateNotification(msk);
        }
        else
        {
            HAL_HSEM_DeactivateNotification(msk);
        }
                
        return true;       
    }  
   

    void s_onrelease(embot::hw::MTX m)
    {
        if(true == initialised(m))
        {           
            bool retrigger = _mtxdriver.onrelease(m);
            
            if(true == retrigger)
            {                
                uint32_t hsem = _mtxdriver.gethsem(m);
                uint32_t msk = __HAL_HSEM_SEMID_TO_MASK(hsem);
                HAL_HSEM_ActivateNotification(msk);
            }
            else
            {
                _mtxdriver.setnotification(m, {});
            }
        }
    }        
          
    
}   // namespace embot::hw::mtx {


extern "C"
{
    void HAL_HSEM_FreeCallback(uint32_t SemMask)
    {
        if(0 == SemMask)
        {
            return;
        }
        
        for(uint8_t hsem=0; hsem<32; hsem++)
        {   // i have up to 32 hsem. i must retrieve the MTX from the hsem
            if(true == embot::core::binary::bit::check(SemMask, hsem))
            {
                embot::hw::MTX m = embot::hw::mtx::bsp::getBSP().toMTX(hsem);
                embot::hw::mtx::s_onrelease(m);
                
                // early exit when all hsem are managed
                embot::core::binary::bit::clear(SemMask, hsem);                
                if(0 == SemMask)
                {
                    return;
                }
            }
        }          
    }    
    
}

#endif //#elif defined(EMBOT_ENABLE_hw_mtx)





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

