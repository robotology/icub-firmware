
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/




// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_adc.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------


#include "embot_core.h"
#include <array>

#include "embot_hw_sys.h"


// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_motor_adc)


#elif defined(EMBOT_ENABLE_hw_motor_adc)

#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
#include "embot_hw_motor_bldc_bsp_amcfoc_1cm7.h"
#endif


using fp_uint8_float_float_float_t = void (*)(uint8_t m, float, float, float);

namespace embot::hw::motor::adc::bsp {
    
    void AinInit(void);
    void AinDeInit(void);
    
}

void dummyONcurrents(uint8_t mot, float v1, float v2, float v3)
{
    
}

fp_uint8_float_float_float_t on_acquisition_of_currents {dummyONcurrents};

namespace embot::hw::motor::adc {
    

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


    struct Calibrator
    {
        static constexpr uint8_t _shift {10};
        static constexpr size_t _maxcount {1 << _shift};
        
        embot::hw::MOTOR mot {embot::hw::MOTOR::one};
        volatile bool _done {false};
        volatile size_t _count {0};
        
        embot::hw::motor::bldc::Currents _currents {};
        std::array<uint64_t, 3> cumulativerawvalues {0, 0, 0};
        

        std::array<Converter, 3> _conv {};
        
        constexpr Calibrator() = default;
        
        void init(embot::hw::MOTOR m)
        {
           for(auto &i : _conv) { i.clear(); } // but the clear is not necessary
           _count = 0;
           _done = false; 
           cumulativerawvalues.fill(0); 
           mot = m;
           embot::hw::motor::bldc::OnCurrents oncurs { mot, oncurrentscalib, this };       
           embot::hw::motor::adc::set(mot, oncurs);
    //       embot::hw::motor::adc::set(mot, {oncurrentscalib, this});
        }
        
        void stop()
        {
            embot::hw::motor::adc::set(mot, {});
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
            
    //    static void dummy_adc_callback(void *owner, const Currents * const currents) {}  
    //    static constexpr OnCurrents dummyADCcbk { dummy_adc_callback, nullptr };    
    //    static void dummy_adc_motorcallback(const embot::hw::MOTOR m, const PhaseCurrents * const phasecurrents) {}      
    //    static constexpr OnMotorPhaseCurrents dummyADCmotorcbk { };    
            
        adcm_Internals() = default;    
    };

    adcm_Internals _adcm_internals {};
        
        
    void adc_on_currents(MOTOR m, const embot::hw::motor::bldc::Currents * const currs)
    {
        _adcm_internals._items[embot::core::tointegral(m)].config.oncurrents.execute(currs);  
    }    
        
    //// this is the callback executed inside the ADC handler  
    //// {c1, c2, c3} are in adc units [0, 64k)
    //// so, we need to transform them into currents expressed in milli-ampere
    //    // and call the callback imposed by the embot::hw::motor::setCallbackOnCurrents()
    //void adcm_on_acquisition_of_currents(uint16_t c1, uint16_t c2, uint16_t c3)
    //{
    //    
    //    embot::hw::MOTOR m {embot::hw::MOTOR::one};
    //    
    //    // important note: {{c1, c2, c3} are in adc units [0, 64k). no conversion yet. 
    //    Currents currents = 
    //    {
    //        _adcm_internals._items[embot::core::tointegral(m)].calibrator._conv[0].raw2current(c1), 
    //        _adcm_internals._items[embot::core::tointegral(m)].calibrator._conv[1].raw2current(c2),
    //        _adcm_internals._items[embot::core::tointegral(m)].calibrator._conv[2].raw2current(c3),
    //    };
    //    _adcm_internals._items[embot::core::tointegral(m)].config.oncurrents.execute(&currents);         
    //}   
    //    

    bool init(embot::hw::MOTOR m, const Configuration &config)
    {
        bool r {true};
        
        if(false == config.isvalid())
        {
            return false;
        }
         
        _adcm_internals._items[embot::core::tointegral(m)].config = config;
        
        // 1. at first we want to be sure that everything is stopped, so:
        deinit(m);  
        
    //    adcm_FP_on_acquisition_of_currents = adcm_on_acquisition_of_currents;
        
    //    on_acquisition_of_currents = dummyONcurrents;

        // 2. then we init the bsp

        bsp::AinInit();      
     
        
        return r;   
    }  

    bool calibrate(embot::hw::MOTOR m, const Calibration &calib)
    {
        bool r {true};
        
        if(calib.calibration != Calibration::CALIBRATION::current)
        {
            return r;
        }
        // in here we need to have zero pwm, so we force it
    //    PwmPhaseSet(0, 0, 0);
    //    PwmPhaseEnable(PWM_PHASE_NONE);   
        
        _adcm_internals._items[embot::core::tointegral(m)].calibrator.init(m);

        r = _adcm_internals._items[embot::core::tointegral(m)].calibrator.wait(calib.timeout);
       
        return r;   
    }    


    bool set(embot::hw::MOTOR m, const embot::hw::motor::bldc::OnCurrents &cbk)
    {
        // marco.accame: the config.oncurrent.execute() verifies that is valid before calling its .action(), 
        // so we are sure that we never call a nullptr function ptr
        _adcm_internals._items[embot::core::tointegral(m)].config.oncurrents = cbk;    

        return true;
    }


    //void set(embot::hw::MOTOR m, const OnCurrents &cbk)
    //{
    ////    // maybe better to protect execution of s_adcm_ADC_cbk by ...
    ////    // temporarily suspending the AdcMotTransferComplete_cb() and AdcMotHalfTransfer_cb()  
    ////    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
    ////    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_CONVERSION_HALF_CB_ID);
    ////    
    ////    _adcm_internals.config.oncurrents = cbk.isvalid() ? cbk : _adcm_internals.dummyADCcbk;
    ////    
    ////    HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AdcMotTransferComplete_cb);
    ////    HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_HALF_CB_ID, AdcMotHalfTransfer_cb);    
    //}

    //void set1(embot::hw::MOTOR m, const OnMotorPhaseCurrents &mcbk)
    //{
    //    _adcm_internals._items[embot::core::tointegral(m)].config.onmotorphasecurrents[embot::core::tointegral(m)] = mcbk.isvalid() ? mcbk : _adcm_internals.dummyADCmotorcbk;
    //}

    // as AdcMotDeInit() but ... see the changes in analog_DeInit() of amcbldc
    bool deinit(embot::hw::MOTOR m)
    {
        bool r = true;    
        // AdcMotDeInit() unchanged is OK 
        // AdcMotDeInit();
        // i also clear the callback
        constexpr embot::hw::motor::bldc::OnCurrents dummy {};
        embot::hw::motor::bldc::set(embot::hw::MOTOR::one, dummy);  
        embot::hw::motor::bldc::set(embot::hw::MOTOR::two, dummy);         
        return r;    
    }
   
    
    
} // namespace embot::hw::motor::adc {


namespace embot::hw::motor::adc::bsp {
    
//    /* LSB current of ADC1 and ADC2 */
//    float AinAdc1Lsb = ADC_LSB/(U9_GAIN * R68);
//    float AinAdc2Lsb = ADC_LSB/(U12_GAIN * R72);    
    /* LSB current of ADC1 and ADC2 */
    constexpr float AinAdc1Lsb {1.0};
    constexpr float AinAdc2Lsb {1.0};    
    
    constexpr size_t ADC1_NUMBER_OF_CHANNELS {3};
    constexpr size_t ADC2_NUMBER_OF_CHANNELS {3};
    constexpr size_t ADC3_NUMBER_OF_CHANNELS {10};
    
    /* DMA circular buffers for ADC1 and ADC2: Dual samples per phase, double buffer */
    volatile  int16_t AinDma1Buffer[4 * ADC1_NUMBER_OF_CHANNELS] __attribute__((aligned (4)));
    volatile  int16_t AinDma2Buffer[4 * ADC2_NUMBER_OF_CHANNELS] __attribute__((aligned (4)));

    /* DMA circular buffers for ADC3: Double buffer */
    volatile uint16_t AinDma3Buffer[2 * ADC3_NUMBER_OF_CHANNELS] __attribute__((aligned (4)));

    /* ADC1, ADC2 and ADC3 raw data */
    volatile  int16_t AinAdc2Buffer[ADC2_NUMBER_OF_CHANNELS];  /* Motor 1 currents */
    volatile  int16_t AinAdc1Buffer[ADC1_NUMBER_OF_CHANNELS];  /* Motor 2 currents */
    volatile uint16_t AinAdc3Buffer[ADC3_NUMBER_OF_CHANNELS];  /* Miscellaneous data */    
    
    
    /* Calibrated motor currents (Amperes) */
    float AinMot1Current[3] = {0};
    float AinMot2Current[3] = {0};    
    
    
    #warning: non si usa 512, ma ... valore di maxpwm diviso 2
    
    #warning: qui prendo il sample 0 se ho rcc > 0.5*arr a significare che e'  il sample nel mezzo del pulse e sample 1 se ho < (quello ad inizio) 
    // ma questo significherebbe che il primo che il adc prende e' il centrale. 
    // boh .... mi sembra errato. il tim1 sia inizializzato in count down con partenza ARR/2 quindi accade prima il pulse del canale 6 che triggera
    // in downcounting 
    
    
#if 0
    
    ### Details of ADC acquisition 
    
    Each motor has two samples for each of the three currents. The first sample is taken at the beginning of the PWM period
    and the second in the middle. 


#endif    
    
    
    
    constexpr uint32_t pwm50perc = embot::hw::motor::bldc::bsp::amcfoc::cm7::PWMvals.valueofTIMperiod()/2; // originally 512
    
//    #warning TODO: change it and document why

#if 0
    the currents are in range [-25, +25] A
    they are scaled by the currenet sensor to [-10, +10] V and then by a partitor to [0, 2.5] V
    then the ADC samples the [0, 2.5] with 12 bits resolution.
    the ADC is connfigured to give a signed int16_t which returns 0 if V = 1.25, +2047 if V = 2.5 and -2048 is V = 0
    but what matters is that the whole current range of 50000 mA is mapped into 12 bits (aka 4096)
    so that each tick value of the adc represents 50000/4096 = 12.20703125 mA
    the correct conversion is
    ``` C++
        `int32_t sampletomilliampere(int16_t s) { return static_cast<int32_t>(12.20703125*static_cast<float>(s)); }
    ```    
    but we instead use integer operations so:
    - we multiply first by (50000/4) = 12500
    - we divide by 1024 by a right shift of 10 positions
    - since i am paranoid i also check the sign so that i always shift positive numbers 
      NOTE: C / C++ maintains the sign when shifting negative numbers but ..... i am paranoid   

#endif
    
    int32_t sampletomilliampere(int16_t s)
    {
        int32_t v = static_cast<int32_t>(s)*12500;
        bool neg = (v < 0);
        if(true == neg)
        {
            v = -v;
        }
        
        v >>= 10;
        
        return (true == neg) ? -v : +v;
    }
    
//    #warning REMOVE generator()
//    int16_t generator()
//    {
//        static int16_t v {0};

//        
//        v++;
//        
//        if(v>1023)
//        {
//            v = 0;
//        }
//        
//        return v;        
//    }
    
    /*******************************************************************************************************************//**
     * @brief   Callback function called by the DMA handler when the 1st half of the analog buffer has been loaded
     * @param   ADC handler (unused)
     * @return  void
     */
    static void AinAdc1HT_cb(ADC_HandleTypeDef *hadc)
    {
        /* Motor 2: first sample is in pulse LOW */
        AinAdc1Buffer[0] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT2, TIM_CHANNEL_1) >= pwm50perc)? AinDma1Buffer[0] : AinDma1Buffer[1];
        AinAdc1Buffer[1] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT2, TIM_CHANNEL_2) >= pwm50perc)? AinDma1Buffer[2] : AinDma1Buffer[3];
        AinAdc1Buffer[2] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT2, TIM_CHANNEL_3) >= pwm50perc)? AinDma1Buffer[4] : AinDma1Buffer[5];
//        /* Convert in float */
//        AinMot2Current[0] = AinAdc1Lsb * (float)AinAdc1Buffer[0];
//        AinMot2Current[1] = AinAdc1Lsb * (float)AinAdc1Buffer[1];
//        AinMot2Current[2] = AinAdc1Lsb * (float)AinAdc1Buffer[2];
        //on_acquisition_of_currents(1, AinMot2Current[0], AinMot2Current[1], AinMot2Current[2]);        
        int32_t u = sampletomilliampere(AinAdc1Buffer[0]);
        int32_t v = sampletomilliampere(AinAdc1Buffer[1]);
        int32_t w = sampletomilliampere(AinAdc1Buffer[2]);
        embot::hw::motor::bldc::Currents cc {0.001f * u, 0.001f * v, 0.001f * w};
        constexpr embot::hw::MOTOR m {embot::hw::MOTOR::two};
        _adcm_internals._items[embot::core::tointegral(m)].config.oncurrents.execute(&cc);

    }

    /*******************************************************************************************************************//**
     * @brief   Callback function called by the DMA handler when the 2nd half of the analog buffer has been loaded
     * @param   ADC handler (unused)
     * @return  void
     */
    static void AinAdc1TC_cb(ADC_HandleTypeDef *hadc)
    {
        /* Motor 2: first sample is in pulse LOW */
        AinAdc1Buffer[0] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT2, TIM_CHANNEL_1) >= pwm50perc)? AinDma1Buffer[6]  : AinDma1Buffer[7];
        AinAdc1Buffer[1] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT2, TIM_CHANNEL_2) >= pwm50perc)? AinDma1Buffer[8]  : AinDma1Buffer[9];
        AinAdc1Buffer[2] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT2, TIM_CHANNEL_3) >= pwm50perc)? AinDma1Buffer[10] : AinDma1Buffer[11];
        /* Convert in float */
//        AinMot2Current[0] = AinAdc1Lsb * (float)AinAdc1Buffer[0];
//        AinMot2Current[1] = AinAdc1Lsb * (float)AinAdc1Buffer[1];
//        AinMot2Current[2] = AinAdc1Lsb * (float)AinAdc1Buffer[2];
        //on_acquisition_of_currents(1, AinMot2Current[0], AinMot2Current[1], AinMot2Current[2]);
        
        int32_t u = sampletomilliampere(AinAdc1Buffer[0]);
        int32_t v = sampletomilliampere(AinAdc1Buffer[1]);
        int32_t w = sampletomilliampere(AinAdc1Buffer[2]);
        embot::hw::motor::bldc::Currents cc {0.001f * u, 0.001f * v, 0.001f * w};
        constexpr embot::hw::MOTOR m {embot::hw::MOTOR::two};
        _adcm_internals._items[embot::core::tointegral(m)].config.oncurrents.execute(&cc);        
        
//         embot::hw::motor::adc::Currents c {sampletomilliampere(AinAdc1Buffer[0]), sampletomilliampere(AinAdc1Buffer[1]), sampletomilliampere(AinAdc1Buffer[2])};

//        embot::hw::motor::PhaseCurrents pc {0.001f * c.u, 0.001f * c.v, 0.001f * c.w};
//        constexpr embot::hw::MOTOR m {embot::hw::MOTOR::two};
//        _adcm_internals._items[embot::core::tointegral(m)].config.onmotorphasecurrents[embot::core::tointegral(m)].execute(m, &pc);         
    }

    
    /*******************************************************************************************************************//**
     * @brief   Callback function called by the DMA handler when the 1st half of the analog buffer has been loaded
     * @param   ADC handler (unused)
     * @return  void
     */
    static void AinAdc2HT_cb(ADC_HandleTypeDef *hadc)
    {
        /* Motor 1: first sample is in pulse HIGH */
        AinAdc2Buffer[0] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT1, TIM_CHANNEL_1) >= pwm50perc)? AinDma2Buffer[1] : AinDma2Buffer[0];
        AinAdc2Buffer[1] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT1, TIM_CHANNEL_2) >= pwm50perc)? AinDma2Buffer[3] : AinDma2Buffer[2];
        AinAdc2Buffer[2] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT1, TIM_CHANNEL_3) >= pwm50perc)? AinDma2Buffer[5] : AinDma2Buffer[4];
        /* Convert in float */
//        AinMot1Current[0] = AinAdc2Lsb * (float)AinAdc2Buffer[0];
//        AinMot1Current[1] = AinAdc2Lsb * (float)AinAdc2Buffer[1];
//        AinMot1Current[2] = AinAdc2Lsb * (float)AinAdc2Buffer[2];
        //on_acquisition_of_currents(0, AinMot1Current[0], AinMot1Current[1], AinMot1Current[2]);
//        embot::hw::motor::adc::Currents c {sampletomilliampere(AinAdc2Buffer[0]), sampletomilliampere(AinAdc2Buffer[1]), sampletomilliampere(AinAdc2Buffer[2])};
////        _adcm_internals.config.onmotorphasecurrents.execute(embot::hw::MOTOR::one, &c);
//        
//        embot::hw::motor::PhaseCurrents pc {0.001f * c.u, 0.001f * c.v, 0.001f * c.w};
//        constexpr embot::hw::MOTOR m {embot::hw::MOTOR::one};
//        _adcm_internals._items[embot::core::tointegral(m)].config.onmotorphasecurrents[embot::core::tointegral(m)].execute(m, &pc);     

        int32_t u = sampletomilliampere(AinAdc2Buffer[0]);
        int32_t v = sampletomilliampere(AinAdc2Buffer[1]);
        int32_t w = sampletomilliampere(AinAdc2Buffer[2]);
        embot::hw::motor::bldc::Currents cc {0.001f * u, 0.001f * v, 0.001f * w};
        constexpr embot::hw::MOTOR m {embot::hw::MOTOR::one};
        _adcm_internals._items[embot::core::tointegral(m)].config.oncurrents.execute(&cc);         
    }

    /*******************************************************************************************************************//**
     * @brief   Callback function called by the DMA handler when the 2nd half of the analog buffer has been loaded
     * @param   ADC handler (unused)
     * @return  void
     */
    static void AinAdc2TC_cb(ADC_HandleTypeDef *hadc)
    {
        /* Motor 1: first sample is in pulse HIGH */
        AinAdc2Buffer[0] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT1, TIM_CHANNEL_1) >= pwm50perc)? AinDma2Buffer[7]  : AinDma2Buffer[6];
        AinAdc2Buffer[1] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT1, TIM_CHANNEL_2) >= pwm50perc)? AinDma2Buffer[9]  : AinDma2Buffer[8];
        AinAdc2Buffer[2] = (__HAL_TIM_GetCompare(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimMOT1, TIM_CHANNEL_3) >= pwm50perc)? AinDma2Buffer[11] : AinDma2Buffer[10];
        /* Convert in float */
//        AinMot1Current[0] = AinAdc2Lsb * (float)AinAdc2Buffer[0];
//        AinMot1Current[1] = AinAdc2Lsb * (float)AinAdc2Buffer[1];
//        AinMot1Current[2] = AinAdc2Lsb * (float)AinAdc2Buffer[2];
//        on_acquisition_of_currents(0, AinMot1Current[0], AinMot1Current[1], AinMot1Current[2]);
//        embot::hw::motor::adc::Currents c {sampletomilliampere(AinAdc2Buffer[0]), sampletomilliampere(AinAdc2Buffer[1]), sampletomilliampere(AinAdc2Buffer[2])};
////        _adcm_internals.config.onmotorphasecurrents.execute(embot::hw::MOTOR::one, &c);  
//        embot::hw::motor::PhaseCurrents pc {0.001f * c.u, 0.001f * c.v, 0.001f * c.w};
//        constexpr embot::hw::MOTOR m {embot::hw::MOTOR::one};
//        _adcm_internals._items[embot::core::tointegral(m)].config.onmotorphasecurrents[embot::core::tointegral(m)].execute(m, &pc);     

        int32_t u = sampletomilliampere(AinAdc2Buffer[0]);
        int32_t v = sampletomilliampere(AinAdc2Buffer[1]);
        int32_t w = sampletomilliampere(AinAdc2Buffer[2]);
        embot::hw::motor::bldc::Currents cc {0.001f * u, 0.001f * v, 0.001f * w};
        constexpr embot::hw::MOTOR m {embot::hw::MOTOR::one};
        _adcm_internals._items[embot::core::tointegral(m)].config.oncurrents.execute(&cc);           
    }

    /*******************************************************************************************************************//**
     * @brief   Callback function called by the DMA handler when the 1st half of the analog buffer has been loaded
     * @param   ADC handler (unused)
     * @return  void
     */
    static void AinAdc3HT_cb(ADC_HandleTypeDef *hadc)
    {
//        UNUSED(hadc);
//        BaseType_t woken = pdFALSE;
        /* Copy the 1st half of the buffer filled by DMA */
        memcpy((void *)AinAdc3Buffer, (void *)&(AinDma3Buffer[0]), sizeof(AinAdc3Buffer));
        /* Release control to the interrupt handler task */
//        xSemaphoreGiveFromISR(AinAdc3SemHandle, &woken );
//        portYIELD_FROM_ISR(woken);
    }

    /*******************************************************************************************************************//**
     * @brief   Callback function called by the DMA handler when the 2nd half of the analog buffer has been loaded
     * @param   ADC handler (unused)
     * @return  void
     */
    static void AinAdc3TC_cb(ADC_HandleTypeDef *hadc)
    {
//        UNUSED(hadc);
//        BaseType_t woken = pdFALSE;
        /* Copy the 2nd half of the buffer filled by DMA */
        memcpy((void *)AinAdc3Buffer, (void *)&(AinDma3Buffer[ADC3_NUMBER_OF_CHANNELS]), sizeof(AinAdc3Buffer));
        /* Release control to the interrupt handler task */
//        xSemaphoreGiveFromISR(AinAdc3SemHandle, &woken );
//        portYIELD_FROM_ISR(woken);
    }    
    
    void AinInit(void)
    {
        /* Stop the ADC3 trigger */
        HAL_TIM_Base_Stop(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimTriggerOfadcOTHERS);
        /* Create the binary semaphore */
//        if ((NULL ==  AinAdc3SemHandle) &&
//            (NULL != (AinAdc3SemHandle = xSemaphoreCreateBinary())))
        {
            /* Create the task */
//            if ((NULL == AinAdc3TaskHandle) &&
//                (pdPASS == xTaskCreate(AinAdc3Task, "ADC3 task", 128, NULL, uxTaskPriorityGet(NULL), &AinAdc3TaskHandle)) &&
//                (NULL != AinAdc3TaskHandle))
            {
                
                __HAL_TIM_SetCounter(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimTriggerOfadcOTHERS, 0);
                HAL_TIM_Base_Start(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimTriggerOfadcOTHERS);                  
                
                /* Stop all ADCs */
                HAL_ADC_Stop_DMA(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT2);
                HAL_ADC_Stop_DMA(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT1);
                HAL_ADC_Stop_DMA(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS);

#warning RICORDATI DI FAR PARTIRE I LORO CLOCK NL MSP                
                /* Calibrate all ADCs. Caution: blocking functions! */
                HAL_ADCEx_Calibration_Start(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT2, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
                HAL_ADCEx_Calibration_Start(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT1, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);
                HAL_ADCEx_Calibration_Start(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS, ADC_CALIB_OFFSET_LINEARITY, ADC_SINGLE_ENDED);

                /* Register all the callback functions */
                HAL_ADC_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT2, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AinAdc1HT_cb);
                HAL_ADC_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT2, HAL_ADC_CONVERSION_HALF_CB_ID,     AinAdc1TC_cb);
                HAL_ADC_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT1, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AinAdc2HT_cb);
                HAL_ADC_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT1, HAL_ADC_CONVERSION_HALF_CB_ID,     AinAdc2TC_cb);
                HAL_ADC_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AinAdc3HT_cb);
                HAL_ADC_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS, HAL_ADC_CONVERSION_HALF_CB_ID,     AinAdc3TC_cb);

                /* Start conversions */
                HAL_ADC_Start_DMA(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT2, (uint32_t *)AinDma1Buffer, sizeof(AinDma1Buffer)/sizeof(AinDma1Buffer[0]));
                HAL_ADC_Start_DMA(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT1, (uint32_t *)AinDma2Buffer, sizeof(AinDma2Buffer)/sizeof(AinDma2Buffer[0]));
                HAL_ADC_Start_DMA(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS, (uint32_t *)AinDma3Buffer, sizeof(AinDma3Buffer)/sizeof(AinDma3Buffer[0]));
                
                
                // start timer for hadcOTHERS   
                //embot::hw::sys::delay(100);                
//                __HAL_TIM_SetCounter(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimTriggerOfadcOTHERS, 0);
//                HAL_TIM_Base_Start(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimTriggerOfadcOTHERS);                
                
                /* All done */
                return;
            }
            /* Cannot run the task */
//            AinAdc3TaskHandle = NULL;
            /* Remove the semaphore */
//            vSemaphoreDelete(AinAdc3SemHandle);
//            AinAdc3SemHandle = NULL;
        }
        /* Stop ADC3 trigger */
        HAL_TIM_Base_Stop(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimTriggerOfadcOTHERS);
        /* Out of memory */
//        return HAL_ERROR;
    }
    
    
    void AinDeInit(void)
    {
//        /* Kill the ADC3 management task */
//        if (NULL != AinAdc3TaskHandle)
//        {
//            vTaskDelete(AinAdc3TaskHandle);
//            AinAdc3TaskHandle = NULL;
//        }
        
//        /* Remove the semaphore */
//        if (NULL != AinAdc3SemHandle)
//        {
//            vSemaphoreDelete(AinAdc3SemHandle);
//            AinAdc3SemHandle = NULL;
//        }

        /* Stop all ADCs */
        HAL_ADC_Stop_DMA(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT2);
        HAL_ADC_Stop_DMA(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT1);
        HAL_ADC_Stop_DMA(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS);
        
        /* Remove all callback functions */
        HAL_ADC_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT2, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
        HAL_ADC_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT2, HAL_ADC_CONVERSION_HALF_CB_ID);
        HAL_ADC_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT1, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
        HAL_ADC_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcMOT1, HAL_ADC_CONVERSION_HALF_CB_ID);
        HAL_ADC_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
        HAL_ADC_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::hadcOTHERS, HAL_ADC_CONVERSION_HALF_CB_ID);
    }    
    
}


#endif // #elif defined(EMBOT_ENABLE_hw_motor_adc)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

