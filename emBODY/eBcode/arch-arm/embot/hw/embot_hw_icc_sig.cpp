
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_icc_sig.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_icc_sig_bsp.h"

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



#if !defined(EMBOT_ENABLE_hw_icc_sig)

namespace embot::hw::icc::sig {
           
    bool supported(embot::hw::icc::SIG s)
    { return false; }   
    result_t init(embot::hw::icc::SIG s, const Config &cfg)
    { return resNOK; }
    bool initialised(embot::hw::icc::SIG s)
    { return false; }
    bool subscribe(embot::hw::icc::SIG s, const embot::hw::Subscription &onsignal)
    { return false; }     
    bool set(embot::hw::icc::SIG s, embot::core::relTime timeout)
    { return false; }
    bool check(embot::hw::icc::SIG s, bool autoclear)
    { return false; }
    bool clear(embot::hw::icc::SIG s)
    { return false; }  
}

#elif defined(EMBOT_ENABLE_hw_icc_sig)

#include "embot_hw_mtx.h"
#include "embot_hw_utils.h"

namespace embot::hw::icc::sig {
        
    // -- SIG
    
    struct sigDRV : public embot::hw::utils::coreDRV
    {
        static constexpr uint8_t capacity {embot::core::tointegral(embot::hw::icc::SIG::maxnumberof)};
        
        std::array<MTX, capacity> mutexes {};
            
        volatile uint32_t signalledmask {0};

        struct cbkInfo
        {
            embot::hw::icc::SIG s {embot::hw::icc::SIG::none};    
            sigDRV *th {nullptr};          
        };
                                
        std::array<Config, capacity> configs {};
        std::array<Subscription, capacity> subscriptions {};
        std::array<cbkInfo, capacity> cbkinfo {};
            
        constexpr sigDRV() = default;
        constexpr sigDRV(uint32_t m) : coreDRV(m) {}  

        constexpr bool initialised(embot::hw::icc::SIG s) const
        {
            return coreDRV::initialised(embot::core::tointegral(s));
        }
        
        void setinitialised(embot::hw::icc::SIG s, bool on, const Config &cfg)
        {
            if(true == supported(s))
            {
                uint8_t pos {embot::core::tointegral(s)};
                coreDRV::setinitialised(pos, on);
                mutexes[pos] = embot::hw::icc::sig::bsp::getBSP().getPROP(s)->mtx;
                cbkinfo[pos] = {s, this};
                configs[pos] = cfg;
                subscriptions[pos].clear();
                // for the benefit of the RX (or RXTX) mode only
                if((cfg.direction == DIR::rx) || (cfg.direction == DIR::txrx))
                { 
                    embot::hw::mtx::subscribe(mutexes[pos], {{onRXsignal, &cbkinfo[pos]}, embot::hw::Subscription::MODE::permanent});
                }
            }
        }
        
        MTX getmutex(embot::hw::icc::SIG s) const
        {
            return (true == initialised(s)) ? mutexes[embot::core::tointegral(s)] : MTX::none;
        }
        
        DIR direction(embot::hw::icc::SIG s) const
        {
            return configs[embot::core::tointegral(s)].direction;
        } 
        
        bool check(embot::hw::icc::SIG s) const
        {
            return embot::core::binary::bit::check(signalledmask, embot::core::tointegral(s));
        }
 
        void clear(embot::hw::icc::SIG s)
        {
            embot::core::binary::bit::clear(signalledmask, embot::core::tointegral(s));
        }
               
        static void onRXsignal(void *p)
        {
            if(nullptr == p)
            {
                return;
            }
            
            cbkInfo *ci = reinterpret_cast<cbkInfo*>(p);

            // it MUST be DIR::rx because we use onRXsignal only in such a case          
//            DIR d = ci->th->direction(ci->s);
//            
//            if(DIR::rx == d)
//            {
//                embot::core::binary::bit::set(ci->th->signalledmask, embot::core::tointegral(ci->s));
//            }  
            
            embot::core::binary::bit::set(ci->th->signalledmask, embot::core::tointegral(ci->s));
            
            ci->th->subscriptions[embot::core::tointegral(ci->s)].execute();            
        }        
   
    }; 

    
    sigDRV _sigdrive {};

        
    bool supported(embot::hw::icc::SIG s)
    {
        return embot::hw::icc::sig::bsp::getBSP().supported(s);;
    }
 
    
    bool initialised(embot::hw::icc::SIG s)
    {
        return _sigdrive.initialised(s);
    }   

    
    result_t init(embot::hw::icc::SIG s, const Config &cfg)
    {
        if(false == supported(s))
        {
            return resNOK;
        }
        
        if(true == initialised(s))
        {
            return resOK;
        }
        
        // init peripheral... actually it is done inside general bsp
        embot::hw::icc::sig::bsp::getBSP().init(s);
        
        result_t r = embot::hw::mtx::init(embot::hw::icc::sig::bsp::getBSP().getPROP(s)->mtx);
        
        if(resOK == r)
        {            
            _sigdrive.setinitialised(s, true, cfg);        
        }

        return r;        
    }
 

    bool set(embot::hw::icc::SIG s, embot::core::relTime timeout)
    {
        if(false == initialised(s))
        {
            return false;
        }
        
        DIR d = _sigdrive.direction(s);
        bool OK = (DIR::tx == d) || (DIR::txrx == d);
        if(!OK)
        {
            return false;
        }        
        
        embot::hw::MTX m = _sigdrive.getmutex(s);
        
        embot::hw::mtx::take(m, timeout);
        embot::hw::mtx::release(m);
        
        return true;  
    }   
    
    
    bool subscribe(embot::hw::icc::SIG s, const embot::hw::Subscription &onsignal)
    {
        if(false == initialised(s))
        {
            return false;
        }
        
        DIR d = _sigdrive.direction(s);
        bool OK = (DIR::rx == d) || (DIR::txrx == d);
        if(!OK)
        {
            return false;
        }
        
        embot::hw::MTX m = _sigdrive.getmutex(s);

        uint8_t pos = embot::core::tointegral(s);
        _sigdrive.subscriptions[pos] = onsignal; 

        return true;
    }


    bool check(embot::hw::icc::SIG s, bool autoclear)
    {
        if(false == initialised(s))
        {
            return false;
        }
        
        DIR d = _sigdrive.direction(s);
        bool OK = (DIR::rx == d) || (DIR::txrx == d);
        if(!OK)        
        {
            return false;
        }
        
        bool r = _sigdrive.check(s);   
        if((true == r) && (true == autoclear))
        {
            _sigdrive.clear(s);
        }
        return r;        
    }
 
    
    bool clear(embot::hw::icc::SIG s)
    {
        if(false == initialised(s))
        {
            return false;
        }
        
        DIR d = _sigdrive.direction(s);
        bool OK = (DIR::rx == d) || (DIR::txrx == d);
        if(!OK)
        {
            return false;
        }
        
        _sigdrive.clear(s);     
        
        return true;        
    } 

} // namespace embot::hw::icc::sig {



#endif //#elif defined(EMBOT_ENABLE_hw_icc_sig)



namespace embot::hw::icc::sig::test {

#if !defined(EMBOT_ENABLE_hw_icc_sig)  

    void init(DIR dir) {}
    void tick() {}
    
#else    
       
    struct TXRXtest
    {
        static constexpr embot::hw::icc::SIG s3 {embot::hw::icc::SIG::three};
        volatile bool triggered {false};
        
        static void trg(void *p)
        {
            TXRXtest *o = reinterpret_cast<TXRXtest*>(p);
            o->triggered = true;
        } 

        void init()
        {
            embot::hw::icc::sig::init(s3, {DIR::txrx});            
        }


        void tick()
        {           
            volatile bool signalled {false};
            
#if defined(USE_ALSO_POLLING)       
            // polling mode       
            // make sure to unsubscribe
            embot::hw::icc::sig::subscribe(s3, {});
         

            for(size_t i=0; i<3; i++)
            {
                signalled = embot::hw::icc::sig::check(s3);
                
                embot::hw::icc::sig::set(s3);
                
                for(;;)
                {
                    signalled = embot::hw::icc::sig::check(s3);
                    
                    if(true == signalled)
                    {
                        embot::hw::icc::sig::clear(s3);
                        signalled = embot::hw::icc::sig::check(s3);
                        signalled = signalled;
                        break;
                    }
                    
                    embot::core::wait(embot::core::time1millisec);
                    
                }  
            }
#endif

            // subscription mode
            embot::hw::icc::sig::subscribe(s3, {{trg, this}, embot::hw::Subscription::MODE::permanent});
            
            for(size_t i=0; i<10; i++)
            {
                signalled = embot::hw::icc::sig::check(s3);
                
                triggered = false;
                
                embot::hw::icc::sig::set(s3);
                
                for(;;)
                {                   
                    if(true == triggered)
                    {
                        triggered = false;
                        embot::hw::icc::sig::clear(s3);
                        signalled = embot::hw::icc::sig::check(s3);
                        signalled = signalled;
                        break;
                    }
                    // embot::core::wait(embot::core::time1millisec);
                    
                }  
            }
            
        }
        
    };
    
    struct TXtest
    {
        static constexpr embot::hw::icc::SIG s1 {embot::hw::icc::SIG::one};  // tx  
        static constexpr embot::hw::icc::SIG s2 {embot::hw::icc::SIG::two};  // rx
        volatile bool TXreceivedback {false};
        
        static void TXtrg(void *p)
        {
            TXtest *o = reinterpret_cast<TXtest*>(p);
            o->TXreceivedback = true;
        }
    
        void init()
        {  
            embot::hw::icc::sig::init(s1, {DIR::tx});
            embot::hw::icc::sig::init(s2, {DIR::rx});
            TXreceivedback = false;
            // subscription to a receiver so that we wait till the other core running testRX alerts
            embot::hw::icc::sig::subscribe(s2, {{TXtrg, this}, embot::hw::Subscription::MODE::permanent});
        }            
    
    
        void tick()
        {               
            for(size_t i=0; i<1; i++)
            {                               
                volatile bool rx1 = embot::hw::icc::sig::check(s1);
                volatile bool rx2 = embot::hw::icc::sig::check(s2); 
                
                // set the signal for the benefit of the other core
                embot::hw::icc::sig::set(s1);
                
                // wait for the ack from the other core
                for(;;)
                { 
                    rx1 = embot::hw::icc::sig::check(s1);
                    rx2 = embot::hw::icc::sig::check(s2);                
                    if(true == TXreceivedback)
                    {
                        rx1 = embot::hw::icc::sig::check(s1);
                        rx2 = embot::hw::icc::sig::check(s2);
                        embot::hw::icc::sig::clear(s2);
                        rx2 = embot::hw::icc::sig::check(s2);
                        rx2 = rx2;
                        TXreceivedback = false;
                        break;
                    }
                    
                    // embot::core::wait(embot::core::time1millisec);                
                } 
                
            }             
            
        }
        
    };
        
    
    struct RXtest
    {
        static constexpr embot::hw::icc::SIG s1 {embot::hw::icc::SIG::one};  // rx  
        static constexpr embot::hw::icc::SIG s2 {embot::hw::icc::SIG::two};  // tx
        
        volatile bool RXreceived {false};
        
        static void RXtrg(void *p)
        {
            RXtest *o = reinterpret_cast<RXtest*>(p);
            o->RXreceived = true;
        }
    
        void init()
        {
            embot::hw::icc::sig::init(s1, {DIR::rx});
            embot::hw::icc::sig::init(s2, {DIR::tx});
            RXreceived = false;
            // subscription to a receiver so that we know when the other core calls a sig::set(s1)
            embot::hw::icc::sig::subscribe(s1, {{RXtrg, this}, embot::hw::Subscription::MODE::permanent});            
        }
        
        void tick()
        {
            for(size_t i=0; i<1; i++)
            {               
                volatile bool rx1 = embot::hw::icc::sig::check(s1);
                volatile bool rx2 = embot::hw::icc::sig::check(s2); 
                               
                // wait for the other core to alert us
                for(;;)
                { 
                    rx1 = embot::hw::icc::sig::check(s1);
                    rx2 = embot::hw::icc::sig::check(s2);                
                    if(true == RXreceived)
                    {
                        rx1 = embot::hw::icc::sig::check(s1);
                        rx2 = embot::hw::icc::sig::check(s2);
                        embot::hw::icc::sig::clear(s1);
                        rx1 = embot::hw::icc::sig::check(s1);
                        rx1 = rx1;
                        RXreceived = false;
                        break;
                    }
                    
                    // embot::core::wait(embot::core::time1millisec);                
                }

                // good, i have received. and now i set it back
                embot::hw::icc::sig::set(s2);
            }

        }            

        
    };
    

    TXRXtest _txrxtest {};
    TXtest _txtest {};
    RXtest _rxtest {};
        
    DIR _dir {DIR::none};    
        
    void init(DIR dir)
    {
        _dir = dir;
        switch(_dir)
        {
            case DIR::tx:
            {
                _txtest.init();
            } break;
 
            case DIR::rx:
            {
                _rxtest.init();
            } break;
            
            case DIR::txrx:
            {
                _txrxtest.init();
            } break;
            
            default: 
            {
            } break;
            
        }        
    }
    
    void tick()
    {
        switch(_dir)
        {
            case DIR::tx:
            {
                _txtest.tick();
            } break;
 
            case DIR::rx:
            {
                _rxtest.tick();
            } break;
            
            case DIR::txrx:
            {
                _txrxtest.tick();
            } break;
            
            default: 
            {
            } break;
            
        }               
    }
    
    
#endif // #if !defined(EMBOT_ENABLE_hw_icc_sig) 
    
} 





// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

