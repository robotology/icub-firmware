
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_adc.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_motor_bldc_adc)

#warning EMBOT_ENABLE_hw_motor_bldc_adc is undefined, so we use dummy implementation 

namespace embot::hw::motor::bldc::adc {
    
    bool init(embot::hw::MOTOR m, const Configuration &config) { return false; }
    bool deinit(embot::hw::MOTOR m) { return false; }
    bool calibrate(embot::hw::MOTOR m, const Calibration &calib) { return false; }
    bool set(embot::hw::MOTOR m, const embot::hw::motor::bldc::OnCurrents &cbk) { return false; }
}

#elif defined(EMBOT_ENABLE_hw_motor_bldc_adc)

#include "embot_hw_motor_bldc_adc_bsp.h"

#define ADC_CLEANUP

namespace embot::hw::motor::bldc::adc::impl {
    
    // used to collect samples of adc values for the current when there is zero pwm
    // so that their mean value is computed and the subtracted to the acquired value
    struct Calibrator
    {
        static constexpr uint8_t _shift {10};
        static constexpr size_t _maxcount {1 << _shift};
        
        embot::hw::MOTOR mot {embot::hw::MOTOR::one};
        volatile bool _done {false};
        volatile size_t _count {0};
        
        embot::hw::motor::bldc::Currents _currents {};
        std::array<uint64_t, 3> cumulativerawvalues {0, 0, 0};
        
    #if defined(ADC_CLEANUP)
    #else 

        this is about how to calib the values
        struct Converter
        {
            static constexpr uint32_t defOffset {29789};
            uint32_t offset {defOffset};
            
            constexpr Converter() = default;
            constexpr Converter(uint32_t o) : offset(o) {};  

            void clear() 
            {
                set(defOffset);
            }        

            void set(uint32_t o) 
            {
                offset = o;
            }

            int32_t raw2current(uint32_t r)
            {
                int32_t t = static_cast<int32_t>(r - offset)*33000;
                return t/(256*1024);
            }
            
            uint32_t current2raw(int32_t c)
            {
                int32_t t = c*256*1024;
                return t/33000 + offset;
            }    
            
        };

        std::array<Converter, 3> _conv {};
    #endif  
    
        constexpr Calibrator() = default;
        
        void init(embot::hw::MOTOR m)
        {
    #if defined(ADC_CLEANUP)
    #else             
           for(auto &i : _conv) { i.clear(); } // but the clear is not necessary
    #endif
           _count = 0;
           _done = false; 
           cumulativerawvalues.fill(0); 
           mot = m;
           embot::hw::motor::bldc::OnCurrents oncurs { mot, oncurrentscalib, this };       
           embot::hw::motor::bldc::adc::set(mot, oncurs);
        }
 
        
        void stop()
        {
            embot::hw::motor::bldc::adc::set(mot, {});
            _count = 0;
           _done = true;  
        }
        
        
        bool wait(const embot::core::relTime timeout = 500*embot::core::time1millisec)
        {
            bool ret {false};
            
            volatile embot::core::Time endtime = embot::core::now() + timeout;
            
            for(;;)
            {
                volatile embot::core::Time n = embot::core::now();
                if(true == _done)
                { 
                    ret = true; 
                    break; 
                }
                else if(n > endtime)
                {
                    ret = false; 
                    stop();
                    break;
                }
            }

            return ret;
        }   


        static void oncurrentscalib(MOTOR h, const embot::hw::motor::bldc::Currents * const currs, void *owner)
        {
            Calibrator *o = reinterpret_cast<Calibrator*>(owner);   
         
    //        // currents is in A 
    //     
    //        // i use Converter::current2raw() because technically Currents does not contain the raw ADC values but transformed values
    //        o->cumulativerawvalues[0] += o->_conv[0].current2raw(currents->u);
    //        o->cumulativerawvalues[1] += o->_conv[1].current2raw(currents->v);
    //        o->cumulativerawvalues[2] += o->_conv[2].current2raw(currents->w);
            
            o->_count++;
            
            if(o->_count >= Calibrator::_maxcount)
            {
    //            // time to do business: prepare average currents, impose the offset to ADC, stop the calibrator  
    //            
    //            // dont use the >> _shift because ... 1. this operation is done only once, so who bother. 
    //            //                                    2. cumulativerawvalues is int64_t, so shift does niot work for negative values
    //            // impose offset to adc.
    //            volatile int64_t cc[3] {0, 0, 0};
    //            cc[0] = o->cumulativerawvalues[0] / o->_count;
    //            cc[1] = o->cumulativerawvalues[1] / o->_count;
    //            cc[2] = o->cumulativerawvalues[2] / o->_count;
    //            
    //            o->_conv[0].set(cc[0]);
    //            o->_conv[1].set(cc[1]);
    //            o->_conv[2].set(cc[2]);
                
                // stop: deregister this callback
                o->stop();
            }                     
        }    
        
    };
    
    

    struct adcm_Internals
    {
        struct Item
        {
            Configuration config {};
            Calibrator calibrator {};   
            Item() = default;            
        };
        
        std::array<Item, embot::hw::motor::bldc::MAXnumber> _items {};
                 
        adcm_Internals() = default;    
    };
 

} // namespace embot::hw::motor::bldc::adc::impl {    


#include "embot_hw_motor_bldc_pwm.h"

namespace embot::hw::motor::bldc::adc {
    
    impl::adcm_Internals _adcm_internals {};
        
    // and finally the functions defined in the API
    
    bool initted {false};
    
    bool init(embot::hw::MOTOR m, const Configuration &config)
    {
        bool r {true};
        
        if(false == config.isvalid())
        {
            return false;
        }
         
        _adcm_internals._items[embot::core::tointegral(m)].config = config;
        
        if(false == initted)
        {
            embot::hw::motor::bldc::adc::bsp::getBSP().init(m);
            embot::hw::motor::bldc::adc::set(m, config.oncurrents); 
            initted = true;
        }        
        
        return r;   
    }  

    bool calibrate(embot::hw::MOTOR m, const Calibration &calib)
    {
        bool r {true};
        
        
        if(calib.mode == Calibration::Mode::none)
        {
            return r;
        }
        
        // so far calibration is disabled
        embot::core::print("runtime warning: we manage only Calibration::Mode::none");
        
        return r;
        
        // in here we need to have zero pwm, so we force it        
        embot::hw::motor::bldc::pwm::set(m, {0, 0, 0});
        
        _adcm_internals._items[embot::core::tointegral(m)].calibrator.init(m);
        r = _adcm_internals._items[embot::core::tointegral(m)].calibrator.wait(calib.timeout); 
        set(m, {});
        return r;
    }    


    bool set(embot::hw::MOTOR m, const embot::hw::motor::bldc::OnCurrents &cbk)
    {
        // marco.accame: the config.oncurrent.execute() verifies that is valid before calling its .action(), 
        // so we are sure that we never call a nullptr function ptr
        //_adcm_internals._items[embot::core::tointegral(m)].config.oncurrents = cbk;    
        embot::hw::motor::bldc::adc::bsp::getBSP().set(m, cbk);
        return true;
    }


    bool deinit(embot::hw::MOTOR m)
    {
        constexpr embot::hw::motor::bldc::OnCurrents dummy {};
            
        bool r = true;    
        embot::hw::motor::bldc::adc::bsp::getBSP().deinit(m);
        initted = false;
        embot::hw::motor::bldc::adc::set(m, dummy);
       
        // embot::hw::motor::bldc::set(embot::hw::MOTOR::one, dummy);  
        //embot::hw::motor::bldc::set(embot::hw::MOTOR::two, dummy);         
        return r;    
    }
     
    
} // namespace embot::hw::motor::bldc::adc {


#endif // #elif defined(EMBOT_ENABLE_hw_motor_bldc_adc)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

