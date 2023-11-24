
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_icc_ltr.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"
#include "embot_hw_icc_ltr_bsp.h"

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



#if !defined(EMBOT_ENABLE_hw_icc_ltr)


namespace embot::hw::icc::ltr {

    bool supported(embot::hw::icc::LTR l)
    { return false; } 
    size_t size(embot::hw::icc::LTR l)
    { return 0; }
    result_t init(embot::hw::icc::LTR l, const Config &cfg)
    { return resNOK; }
    bool initialised(embot::hw::icc::LTR l)
    { return false; }
    
    size_t post(embot::hw::icc::LTR l, const embot::core::Data &data, embot::core::relTime acktimeout)
    { return 0; }
    size_t post(embot::hw::icc::LTR l, const embot::core::Data &data, const embot::core::Callback &onack)
    { return 0; } 
    bool acked(embot::hw::icc::LTR l, bool autoclear)
    { return false; }
    

    bool subscribe(embot::hw::icc::LTR l, const embot::hw::Subscription &onreceived)
    { return false; }
    bool available(embot::hw::icc::LTR l)
    { return false; }    
    size_t read(embot::hw::icc::LTR l, embot::core::Data &data)
    { return 0; } 
    
}   

#elif defined(EMBOT_ENABLE_hw_icc_ltr)

#include "embot_hw_mtx.h"
#include "embot_hw_icc_sig.h"
#include "embot_hw_icc_mem.h"
#include "embot_hw_utils.h"


namespace embot::hw::icc::ltr {


    // -- LTR
    
    struct ltrDRV : public embot::hw::utils::coreDRV
    {
        static constexpr uint8_t capacity {embot::core::tointegral(embot::hw::icc::LTR::maxnumberof)};
        
        std::array<MEM, capacity> memories {};
        std::array<SIG, capacity> signals {};
        std::array<SIG, capacity> acks {};     

            
        volatile uint32_t ackedmask {0};
        volatile uint32_t unreadmask {0};
        
        struct cbkInfo
        {
            embot::hw::icc::LTR l {embot::hw::icc::LTR::none};    
            ltrDRV *th {nullptr};          
        };

        std::array<Config, capacity> configs {};
        std::array<Subscription, capacity> subscriptions {};
        std::array<cbkInfo, capacity> cbkinfo {};
            
                    
        static void onsignal(void *p)
        {
            if(nullptr == p)
            {
                return;
            }
            cbkInfo *ci = reinterpret_cast<cbkInfo*>(p);
            
            DIR d = ci->th->direction(ci->l);
            
            if(DIR::rx == d)
            {
                embot::core::binary::bit::set(ci->th->unreadmask, embot::core::tointegral(ci->l));
            }
            else // if(DIR::tx == d) 
            {
                embot::core::binary::bit::set(ci->th->ackedmask, embot::core::tointegral(ci->l));
            }    
            
            ci->th->subscriptions[embot::core::tointegral(ci->l)].execute();            
        }

        constexpr ltrDRV() = default;
        constexpr ltrDRV(uint32_t m) : coreDRV(m) {}  

        constexpr bool initialised(embot::hw::icc::LTR l) const
        {
            return coreDRV::initialised(embot::core::tointegral(l));
        }
        
        void setinitialised(embot::hw::icc::LTR l, bool on, const Config &cfg)
        {
            if(true == supported(l))
            {
                uint8_t pos = embot::core::tointegral(l);
                coreDRV::setinitialised(pos, on);
               
                memories[pos] = embot::hw::icc::ltr::bsp::getBSP().getPROP(l)->mem; 
                signals[pos] = embot::hw::icc::ltr::bsp::getBSP().getPROP(l)->sig;
                acks[pos] = embot::hw::icc::ltr::bsp::getBSP().getPROP(l)->ack;
                embot::core::binary::bit::clear(ackedmask, pos);
                embot::core::binary::bit::clear(unreadmask, pos);
                cbkinfo[pos] = {l, this};
                configs[pos] = cfg;
                subscriptions[pos].clear();

                if(cfg.direction == DIR::rx)
                { 
                    embot::hw::icc::sig::subscribe(signals[pos], {{onsignal, &cbkinfo[pos]}, embot::hw::Subscription::MODE::permanent}); 
                }
// we dont need to subscribe if in tx mode because ... we wait for the ack using polling                
//                else
//                {
//                    embot::hw::icc::sig::subscribe(acks[pos], {{onsignal, &cbkinfo[pos]}, embot::hw::Subscription::MODE::permanent}); 
//                }
            }
        }
             
        
        MEM getmem(embot::hw::icc::LTR l) const
        {
            return (true == initialised(l)) ? memories[embot::core::tointegral(l)] : MEM::none;
        }
        
        SIG getsig(embot::hw::icc::LTR l) const
        {
            return (true == initialised(l)) ? signals[embot::core::tointegral(l)] : SIG::none;
        }

        SIG getack(embot::hw::icc::LTR l) const
        {
            return (true == initialised(l)) ? acks[embot::core::tointegral(l)] : SIG::none;
        } 

        DIR direction(embot::hw::icc::LTR l) const
        {
            return configs[embot::core::tointegral(l)].direction;
        }         
        
    }; 

    ltrDRV _ltrdrive {};
 
    
    bool supported(embot::hw::icc::LTR l)
    {
       return embot::hw::icc::ltr::bsp::getBSP().supported(l);
    }
 
    
    bool initialised(embot::hw::icc::LTR l)
    {
        return _ltrdrive.initialised(l);
    }  
 
    
    size_t size(embot::hw::icc::LTR l)
    {
        size_t s {0};
        
        if(true == supported(l))
        {
            s = embot::hw::icc::mem::size(embot::hw::icc::ltr::bsp::getBSP().getPROP(l)->mem);
        }        
        
        return s;
    } 
   
    
    result_t init(embot::hw::icc::LTR l, const Config &cfg)
    {
        if(false == supported(l))
        {
            return resNOK;
        }
        
        if(true == initialised(l))
        {
            return resOK;
        }
        
        if(false == cfg.isvalid())
        {
            return resNOK;
        }
        
        // init peripheral... actually it is done inside general bsp
        embot::hw::icc::ltr::bsp::getBSP().init(l);
        
        result_t r = resNOK;
        MEM _m = embot::hw::icc::ltr::bsp::getBSP().getPROP(l)->mem;
        SIG _s = embot::hw::icc::ltr::bsp::getBSP().getPROP(l)->sig;
        SIG _a = embot::hw::icc::ltr::bsp::getBSP().getPROP(l)->ack;
        
        
        // we need to init the SIGs  _s and _a in the proper way:
        // if DIR::tx -> _s {DIR::tx}  + _a {DIR::rx} because it is a sender which signals w/ _s and waits for _a
        // if DIR::rx -> _s {DIR::rx}  + _a {DIR::tx} because it is a receiver which waits for _s and signals w/ _a
        embot::hw::icc::sig::Config c_s {(DIR::tx == cfg.direction) ? DIR::tx : DIR::rx};
        embot::hw::icc::sig::Config c_a {(DIR::tx == cfg.direction) ? DIR::rx : DIR::tx};
        
        if((resOK == embot::hw::icc::mem::init(_m)) && (resOK == embot::hw::icc::sig::init(_s, c_s)) && (resOK == embot::hw::icc::sig::init(_a, c_a)))
        {
            r = resOK;
        }
                  
        if(resOK == r)
        {            
            _ltrdrive.setinitialised(l, true, cfg);        
        }
       
        return r;   
    }
    
    
    bool acked(embot::hw::icc::LTR l, bool autoclear)
    {
        if(false == initialised(l))
        {
            return false;
        } 

        bool r = embot::core::binary::bit::check(_ltrdrive.ackedmask, embot::core::tointegral(l)); 
        
        if((true == r) && (true == autoclear))
        {
            embot::core::binary::bit::clear(_ltrdrive.ackedmask, embot::core::tointegral(l)); 
        }
        
        return r;
    }
    
    
    size_t post(embot::hw::icc::LTR l, const embot::core::Data &data, embot::core::relTime acktimeout)
    {
        size_t s {0};
        embot::core::Time expiry {embot::core::timeNone};
        
        if(false == initialised(l))
        {
            return s;
        }
        
        if(false == data.isvalid())
        {
            return s;
        }  
        
        MEM _m = _ltrdrive.getmem(l);
        SIG _s = _ltrdrive.getsig(l);
        SIG _a = _ltrdrive.getack(l);
        
        uint8_t pos = embot::core::tointegral(l);
        
        embot::core::binary::bit::clear(_ltrdrive.ackedmask, pos);

        _ltrdrive.subscriptions[pos].clear();       
        embot::hw::icc::sig::subscribe(_a, {{_ltrdrive.onsignal, &_ltrdrive.cbkinfo[pos]}, embot::hw::Subscription::MODE::oneshot});         
        

        if(embot::core::reltimeWaitNone != acktimeout)
        {        
            // also: i compute an expiry only if i decide to use it otherwise i keep expiry at embot::core::timeNone 
            if(embot::core::reltimeWaitForever != acktimeout)
            {               
                expiry = embot::core::now() + acktimeout;           
            }
        }
        
           
        // in here i do business: i set the data and i signal to the receiver
        s = embot::hw::icc::mem::set(_m, data);
        embot::hw::icc::sig::set(_s);  
        
        // if i have to wait ... i wait until acked or timeout occurr
        if(embot::core::reltimeWaitNone != acktimeout)
        {
            // ok, i wait
            bool r {false};
            for(;;)
            {
                if(true == embot::core::binary::bit::check(_ltrdrive.ackedmask, pos))
                {
                    r = true;
                    break;
                }
                else if((embot::core::timeNone != expiry) && (embot::core::now() > expiry))
                {   // timeout sadly happens
                    break;
                }
            }
        }        
        
        return s;
    }        

        
    size_t post(embot::hw::icc::LTR l, const embot::core::Data &data, const embot::core::Callback &onack)
    {
        size_t s {0};
        
        if(false == initialised(l))
        {
            return s;
        }
        
        if(false == data.isvalid())
        {
            return s;
        }  
        
        MEM _m = _ltrdrive.getmem(l);
        SIG _s = _ltrdrive.getsig(l);
        SIG _a = _ltrdrive.getack(l);
        
        uint8_t pos = embot::core::tointegral(l);
        
        embot::core::binary::bit::clear(_ltrdrive.ackedmask, pos);
                    
        _ltrdrive.subscriptions[pos] = {onack, embot::hw::Subscription::MODE::oneshot};
        embot::hw::icc::sig::subscribe(_a, {{_ltrdrive.onsignal, &_ltrdrive.cbkinfo[pos]}, embot::hw::Subscription::MODE::oneshot});
        
        
        // in here i do business
        s = embot::hw::icc::mem::set(_m, data);
        embot::hw::icc::sig::set(_s);  
        
        // and i return
        return s;
    } 
      
    
    bool subscribe(embot::hw::icc::LTR l, const embot::hw::Subscription &onreceived)
    {
        bool r {false};
        
        if(false == initialised(l))
        {
            return r;
        }
        
        uint8_t pos = embot::core::tointegral(l);
        
        SIG _s = _ltrdrive.getsig(l);
        _ltrdrive.subscriptions[pos] = onreceived;
        
        r = true;
        
        return r;
    }    

    
    bool available(embot::hw::icc::LTR l)
    {
        if(false == initialised(l))
        {
            return false;
        } 
        uint8_t pos = embot::core::tointegral(l);
        return embot::core::binary::bit::check(_ltrdrive.unreadmask, pos);          
    }
 
    
    size_t read(embot::hw::icc::LTR l, embot::core::Data &data)
    {
        size_t s {0};
        
        if(false == initialised(l))
        {
            return s;
        }
        
        if(false == data.isvalid())
        {
            return s;
        }  
        
        if(false == available(l))
        {
            return s;
        }
        
        MEM _m = _ltrdrive.getmem(l);
        SIG _a = _ltrdrive.getack(l);
        
        uint8_t pos = embot::core::tointegral(l);
        
        s = embot::hw::icc::mem::get(_m, data);
        embot::hw::icc::sig::set(_a);

        embot::core::binary::bit::clear(_ltrdrive.unreadmask, pos);        
        
        return s;
    }      
    
}   // namespace embot::hw::icc::ltr {


#endif //#elif defined(EMBOT_ENABLE_hw_icc_ltr)


#include "embot_hw_icc_printer.h"

namespace embot::hw::icc::ltr::test {

#if !defined(EMBOT_ENABLE_hw_icc_ltr)      
    
    void init(DIR dir) {}
    void tick(const std::string &str) {}
    
#else
        
    void init(DIR dir)
    {
        embot::hw::icc::printer::test::init(dir);
    }
    
    void tick(const std::string &str)
    {
        embot::hw::icc::printer::test::tick(str);
    }

#endif    
}  



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

