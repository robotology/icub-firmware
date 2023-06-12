
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_icc_mem.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_icc_mem_bsp.h"

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


// read also: https://www.reddit.com/r/stm32/comments/mwb8zq/stm32h7how_to_share_a_structure_between_cores/



#if !defined(EMBOT_ENABLE_hw_icc_mem)

namespace embot::hw::icc::mem {

    bool supported(embot::hw::icc::MEM m)
    { return false; } 
    size_t size(embot::hw::icc::MEM m)
    { return 0; }
    result_t init(embot::hw::icc::MEM m)
    { return resNOK; }    
    bool initialised(embot::hw::icc::MEM m)
    { return false; }
    size_t set(embot::hw::icc::MEM m, const embot::core::Data &data, embot::core::relTime timeout)
    { return 0; }    
    size_t get(embot::hw::icc::MEM m, embot::core::Data &data, embot::core::relTime timeout)
    { return 0; } 
    bool take(embot::hw::icc::MEM m, embot::core::relTime timeout)
    { return false; }
    void * memory(embot::hw::icc::MEM m)
    { return nullptr; }
    void release(embot::hw::icc::MEM m)\
    { }

}

 
#elif defined(EMBOT_ENABLE_hw_icc_mem)

#include "embot_hw_mtx.h"
#include "embot_hw_utils.h"


namespace embot::hw::icc::mem {
    
    // -- MEM
        
    struct memDRV : public embot::hw::utils::coreDRV
    {
        static constexpr uint8_t capacity {embot::core::tointegral(embot::hw::icc::MEM::maxnumberof)};
        
        std::array<MTX, capacity> mutexes {};

        constexpr memDRV() = default;
        constexpr memDRV(uint32_t m) : coreDRV(m) {}  

        constexpr bool initialised(embot::hw::icc::MEM m) const
        {
            return coreDRV::initialised(embot::core::tointegral(m));
        }
        
        void setinitialised(embot::hw::icc::MEM m, bool on)
        {
            if(true == supported(m))
            {
                coreDRV::setinitialised(embot::core::tointegral(m), on);
                mutexes[embot::core::tointegral(m)] = embot::hw::icc::mem::bsp::getBSP().getPROP(m)->mtx;
            }
        }
        
        MTX getmutex(embot::hw::icc::MEM m) const
        {
            return (true == initialised(m)) ? mutexes[embot::core::tointegral(m)] : MTX::none;
        }
   
    }; 

    memDRV _memdrive {};  

    bool supported(embot::hw::icc::MEM m)
    {
        return embot::hw::icc::mem::bsp::getBSP().supported(m);;
    }

    size_t size(embot::hw::icc::MEM m)
    {
        size_t s {0};
        
        if(true == supported(m))
        {
            s = embot::hw::icc::mem::bsp::getBSP().getPROP(m)->size();
        }        
       
        return s;
    } 
    
    void * memory(embot::hw::icc::MEM m)
    {
        void * me {nullptr};
        
        if(true == supported(m))
        {
            me = embot::hw::icc::mem::bsp::getBSP().getPROP(m)->address();
        }        
       
        return me;        
    }

    
    bool initialised(embot::hw::icc::MEM m)
    {
        return _memdrive.initialised(m);
    }   
 
    
    result_t init(embot::hw::icc::MEM m)
    {
        if(false == supported(m))
        {
            return resNOK;
        }
        
        if(true == initialised(m))
        {
            return resOK;
        }
        
        embot::hw::icc::mem::bsp::getBSP().init(m);
        
        result_t r = embot::hw::mtx::init(embot::hw::icc::mem::bsp::getBSP().getPROP(m)->mtx);
        
        if(resOK == r)
        {            
            _memdrive.setinitialised(m, true);        
        }

        return r;                     
    } 


    size_t set(embot::hw::icc::MEM m, const embot::core::Data &data, embot::core::relTime timeout)
    {
        size_t s {0};
        
        if(false == initialised(m))
        {
            return s;
        }
        
        if(false == data.isvalid())
        {
            return s;
        }
        embot::hw::MTX mtx = embot::hw::icc::mem::bsp::getBSP().getPROP(m)->mtx;
        if(true == embot::hw::mtx::take(mtx, timeout))
        {
            void *me = embot::hw::icc::mem::bsp::getBSP().getPROP(m)->address();
            s = std::min(data.capacity, embot::hw::icc::mem::bsp::getBSP().getPROP(m)->size());            
            std::memmove(me, data.pointer, s);         
            embot::hw::mtx::release(mtx);
        }
        
        return s;
    }

    
    size_t get(embot::hw::icc::MEM m, embot::core::Data &data, embot::core::relTime timeout)
    {
        size_t s {0};
        
        if(false == initialised(m))
        {
            return s;
        }
        
        if(false == data.isvalid())
        {
            return s;
        }
        
        embot::hw::MTX mtx = embot::hw::icc::mem::bsp::getBSP().getPROP(m)->mtx;
        if(true == embot::hw::mtx::take(mtx, timeout))
        {
            void *me = embot::hw::icc::mem::bsp::getBSP().getPROP(m)->address();
            s = std::min(data.capacity, embot::hw::icc::mem::bsp::getBSP().getPROP(m)->size());
            std::memmove(data.pointer, me, s);          
            embot::hw::mtx::release(mtx);
        }
       
        return s;
    } 
 
    
    bool take(embot::hw::icc::MEM m, embot::core::relTime timeout)
    {
        if(false == initialised(m))
        {
            return false;
        }
        
        embot::hw::MTX mtx = embot::hw::icc::mem::bsp::getBSP().getPROP(m)->mtx;        
        return embot::hw::mtx::take(mtx, timeout);
    }
    
    
    void release(embot::hw::icc::MEM m)
    {
        if(true == initialised(m))
        {
            embot::hw::MTX mtx = embot::hw::icc::mem::bsp::getBSP().getPROP(m)->mtx;        
            embot::hw::mtx::release(mtx);
        }        
    }    

} // namespace embot::hw::icc::mem {



#endif //#elif defined(EMBOT_ENABLE_hw_icc_mem)

#include "embot_hw_icc_sig.h"

namespace embot::hw::icc::mem::test {

#if !defined(EMBOT_ENABLE_hw_icc_mem)   
    
    void init(embot::hw::icc::DIR dir, const embot::hw::icc::mem::Config &cfg) {}
    void tick() {}
    
#else
    
//    static embot::core::dummyMutex dm {};
//    struct BetterConfig
//    {
////        static constexpr embot::core::dummyMutex dm {};
//        embot::core::Mutex *mu { &dm };
//        constexpr BetterConfig() = default;
//        BetterConfig(embot::core::Mutex *m) : mu((nullptr != m) ? m : &dm) {}
//    };    
    
    struct TestDataExchange
    {
        uint32_t n {0};
        char str[128] {0};
        
        TestDataExchange() = default;
        
        void load(const std::string &s)
        {
            n++;
            std::snprintf(str, sizeof(str), "%s", s.c_str()); 
        }
        
        void print()
        {
            embot::core::print("n = " + std::to_string(n) + ": " + str);
        }   
    };
    
    struct tester
    {
        virtual void init(const embot::hw::icc::mem::Config &cfg) = 0;
        virtual void tick() = 0;
        ~tester() {}
    };
    
 
    struct NOtester : public tester
    { 
        void init(const embot::hw::icc::mem::Config &cfg) override {}        
        void tick() override {}        
    };
    
    struct TXtester : public tester
    {
        static constexpr embot::hw::icc::MEM MEM01 {embot::hw::icc::MEM::one};
        static constexpr embot::hw::icc::SIG SIG01 {embot::hw::icc::SIG::one};
    
        size_t nn {0};
        TestDataExchange tde {};
        embot::core::Data dd { &tde, sizeof(tde) };   
        
        void init(const embot::hw::icc::mem::Config &cfg) override
        {
            embot::hw::icc::mem::init(MEM01);
            embot::hw::icc::sig::init(SIG01, {DIR::tx});            
        }
        
        void tick() override
        {
            nn++;
            
            embot::core::TimeFormatter tf(embot::core::now());    
            tde.load(std::string("i am the sender: @" + tf.to_string() + 
                     " my internal counter nn = ") + std::to_string(nn));
            embot::hw::icc::mem::set(MEM01, dd);
            embot::hw::icc::sig::set(SIG01);               
        }
        
    };
    
    
    struct RXtester : public tester
    {
        static constexpr embot::hw::icc::MEM MEM01 {embot::hw::icc::MEM::one};
        static constexpr embot::hw::icc::SIG SIG01 {embot::hw::icc::SIG::one};
    
        TestDataExchange tde {};
        embot::core::Data dd { &tde, sizeof(tde) };   
        
        void init(const embot::hw::icc::mem::Config &cfg) override
        {
            embot::hw::icc::mem::init(MEM01);
            embot::hw::icc::sig::init(SIG01, {DIR::rx});            
        }
        
        void tick() override
        {
            bool sigRX {false};
            // wait for the sig to be set
            if(true == embot::hw::icc::sig::check(SIG01))
            {
                embot::hw::icc::sig::clear(SIG01);
                sigRX = true;                 
            }
            
            process(sigRX);
        }
        
        void process(bool rxsig)
        {
            embot::core::TimeFormatter tf(embot::core::now());
            
            embot::hw::icc::mem::get(MEM01, dd);
            
            embot::core::print("@" + tf.to_string() + ": sig = " + std::to_string(rxsig) + ", msg = " + tde.str);            
        }
        
    };    
    
    DIR _dir {DIR::none};
    NOtester _notester {};
    TXtester _txtester {};
    RXtester _rxtester {};
        
    tester *_ptester {&_notester};
    
    void init(embot::hw::icc::DIR dir, const embot::hw::icc::mem::Config &cfg)
    {
        _dir = dir;
        
        switch(_dir)
        {
            case DIR::tx:
            {
                _ptester = &_txtester;
            } break;

            case DIR::rx:
            {
                _ptester = &_rxtester;
            } break;
            
            default:
            {
            } break;
        }
        
        _ptester->init(cfg);
    }
    
    
    void tick()
    {
        _ptester->tick(); 
    }
    
#endif    
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

