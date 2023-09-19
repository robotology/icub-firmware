
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_app_board_amc2c_test01.h"



// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor.h"


namespace embot::tools::filter {
    
    struct MovingAverage
    {
        struct Config
        {
            static constexpr uint8_t exponent {10};
            static constexpr size_t size {2 << exponent};            
            bool clip {false};            
            std::pair<int32_t, int32_t> limits {-100, +100};            
        };
        
        Config config {};
        std::array<int32_t, Config::size> status {};
        size_t index {0};
        int64_t accumulator {0};
        int32_t average {0};
        bool ready {false};
        
        int32_t clip(const int32_t &v, bool &clipped)
        {
            clipped = false;
            if(true == config.clip)
            {                            
                if(v < config.limits.first)
                {
                    clipped = true;
                    return config.limits.first;                
                }
                
                if(v > config.limits.second)
                {
                    clipped = true;
                    return config.limits.second;                
                }
                
                //return std::clamp(v, config.limits.first, config.limits.second);
            }
            
            return v;          
        }
        
        void clear()
        {
            ready = false;
            index = 0;
            accumulator = 0;
            average = 0;           
        }
        
        bool add(const int32_t &v)
        {
            bool clipped {false};
            int32_t vc = clip(v, clipped);
            
            if(ready)
            {
                accumulator -= status[index];
            }
            
            accumulator += vc;
            status[index] = vc;
            index = (index+1) % status.size(); 
            
            if(0 == index)
            {
                ready = true;
            }  
            
            if(ready)
            {
//                average = accumulator << Config::exponent;
                average = accumulator / status.size();
            }

            return ready;    
        }
        
        int32_t value() const
        {
            return average;
        }
    };
    
    
}




namespace embot::app::board::amc2c::test01 {
    
    embot::prot::can::Address address = 0;
    void onCurrents_FOC_innerloop(void *owner, const embot::hw::motor::Currents * const currents);
    
    void Startup(embot::prot::can::Address adr)
    {
        address = adr;
        
        // init motor
        embot::hw::motor::init(embot::hw::MOTOR::one, {});
        
        // assign the callback to the adc acquistion of currents
        embot::hw::motor::setCallbackOnCurrents(embot::hw::MOTOR::one, onCurrents_FOC_innerloop, nullptr);
            
        // enable the motor
        embot::hw::motor::enable(embot::hw::MOTOR::one, true);
    }
    
    struct Shared
    {
        enum class Mode { movingaverage = 0, raw = 1};
        volatile bool transmit {false};
        volatile embot::core::Time txperiod {0};
        volatile bool imposepwm {false};
        embot::hw::motor::PWMperc pwm {0.0, 0.0, 0.0};  
        std::array<int16_t, 3> adcvalues {}; 
        volatile Mode mode {Mode::movingaverage};
        
        std::array<embot::tools::filter::MovingAverage, 3> mav {};


        Shared() = default;   

        void clear()
        {
            transmit = false;
            txperiod = 0;
            imposepwm = false;
            pwm = {0.0, 0.0, 0.0};  
            adcvalues = {0, 0, 0}; 
            mode = Mode::raw;            
        }
    };
    
    
    Shared _shared {};
    

    
    void parse(const std::vector<embot::prot::can::Frame> &input);
    
    void OnTick(const std::vector<embot::prot::can::Frame> &input, std::vector<embot::prot::can::Frame> &output)
    { 
        output.clear();
        
        if(input.size() > 0)
        {
            parse(input);
        }
        
        if(_shared.transmit)
        {
            _shared.transmit = false;  
            
            // prepare a frame and add it inside output
            embot::prot::can::Frame f 
            {
                static_cast<uint32_t>(0xF00 | (address << 4)),
                8,
                {
                    0x42,
                    0x00,
                    static_cast<uint8_t>(_shared.adcvalues[0] & 0xff), static_cast<uint8_t>(_shared.adcvalues[0] >> 8), 
                    static_cast<uint8_t>(_shared.adcvalues[1] & 0xff), static_cast<uint8_t>(_shared.adcvalues[1] >> 8),
                    static_cast<uint8_t>(_shared.adcvalues[2] & 0xff), static_cast<uint8_t>(_shared.adcvalues[2] >> 8),
                }
            };
            
            output.push_back(f);     

            int16_t i0 = static_cast<int16_t>(_shared.adcvalues[0]);
            int16_t i1 = static_cast<int16_t>(_shared.adcvalues[1]);
            int16_t i2 = static_cast<int16_t>(_shared.adcvalues[2]);
//            constexpr float conv {1000.0};
//            float I0 = conv*33.0*i0 / (256*1024);
//            float I1 = conv*33.0*i1 / (256*1024);
//            float I2 = conv*33.0*i2 / (256*1024);
            embot::core::print("curs = (" + std::to_string(i0) + ", " + std::to_string(i1) + ", " + std::to_string(i2) + ")");
          //  embot::core::print("curs [mA] = (" + std::to_string(I0) + ", " + std::to_string(I1) + ", " + std::to_string(I2) + ")");                    
        }
          
              
    }
    
    
       
    
    // others
    void onCurrents_FOC_innerloop(void *owner, const embot::hw::motor::Currents * const currents)
    {        
        // 1. copy currents straight away, so that we can use them
        embot::hw::motor::Currents currs = *currents;
        
        _shared.mav[0].add(currs.u);
        _shared.mav[1].add(currs.v);
        _shared.mav[2].add(currs.w);
       
        // 2. and i filter them with a ... moving average.
        static embot::core::Time timelasttx {0};
        
        // 3. if time to transmit comes ... set transmit = true;    
        if(0 != _shared.txperiod)
        {
            embot::core::Time now = embot::core::now();
            embot::core::Time delta = now - timelasttx;
            if(delta >= _shared.txperiod)
            { 
                if(Shared::Mode::movingaverage == _shared.mode)
                {                    
                    _shared.adcvalues[0] = _shared.mav[0].value();
                    _shared.adcvalues[1] = _shared.mav[1].value();
                    _shared.adcvalues[2] = _shared.mav[2].value();
                }
                else
                {
                    _shared.adcvalues[0] = currs.u;
                    _shared.adcvalues[1] = currs.v;
                    _shared.adcvalues[2] = currs.w;                    
                }

                timelasttx = now;
                _shared.transmit = true;
            }
        }            

        // 4. if i need to change the pwm ...
        if(_shared.imposepwm)
        {
            _shared.imposepwm = false;
            embot::hw::motor::setPWM(embot::hw::MOTOR::one, _shared.pwm);
        }        

    }
    
    void parse(const std::vector<embot::prot::can::Frame> &input)
    {
        
        for(size_t s=0; s<input.size(); s++)
        {
            uint32_t id = input[s].id;
            
            
            if((0x70f == id) || ((0x700 | address) == id))
            {
                // class management directed to this board or in bradcast
                switch(input[s].data[0])
                {
                    case 0x40:
                    {
                        _shared.mode = (0 == (input[s].data[1])) ? Shared::Mode::movingaverage : Shared::Mode::raw;
                        _shared.pwm.a = static_cast<float>(input[s].data[2] + 256*input[s].data[3]);
                        _shared.pwm.b = static_cast<float>(input[s].data[4] + 256*input[s].data[5]);
                        _shared.pwm.c = static_cast<float>(input[s].data[6] + 256*input[s].data[7]);
                        _shared.imposepwm = true;
                    } break;
                    
                    case 0x41:
                    {
                        _shared.txperiod = 1000 * input[s].data[1];
                    } break;
                    
                }
            }
            
        }
        
        
    }

} // end of namespace



// - end-of-file (leave a blank line after)----------------------------------------------------------------------------


