
/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_bldc_hall.h"


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


#if !defined(EMBOT_ENABLE_hw_motor_bldc_hall)

#warning EMBOT_ENABLE_hw_motor_bldc_hall is undefined, so we use dummy implementation 

namespace embot::hw::motor::bldc::hall {
    
    bool init(embot::hw::MOTOR m, const Configuration &config) { return false; }
    bool deinit(embot::hw::MOTOR m) { return false; }
    bool initialized(embot::hw::MOTOR m) { return false; }
    bool start(embot::hw::MOTOR m, const Mode &mode) { return false; }
    bool isstarted(embot::hw::MOTOR m) { return false; };
    uint8_t getstatus(embot::hw::MOTOR m) { return 0; }    
    float angle(embot::hw::MOTOR m, embot::hw::motor::bldc::AngleType type) { return 0.0; }
    uint8_t sector(embot::hw::MOTOR m) { return 0; }
    
    bool status2sector(embot::hw::MOTOR m, uint8_t status, uint8_t &sector) { return false; }
    bool sector2status(embot::hw::MOTOR m, uint8_t sector, uint8_t &status) { return false; }
}

#elif defined(EMBOT_ENABLE_hw_motor_bldc_hall)


#include "embot_hw_motor_bldc_hall_bsp.h"

namespace embot::hw::motor::bldc::hall::impl {
        
    struct hallData
    {

    private:    
        struct LUT
        {
            
        #if 0
            there are 6 sectors, each of 360/6 = 60 degrees and centred around i*60, i = [0, 5]
            sectors are defined by a number, a centre, an interval, a h3h2h1 value as in followiung table
            
            | sector | centre | interval   | h3h2h1    |
            | ------ | ------ | ---------- | --------- |
            | s0     | 0      | [-30, +30) | 0b100 = 4 |
            | s1     | 60     | [30, 90)   | 0b110 = 6 |
            | s2     | 120    | [90, 150)  | 0b010 = 2 |
            | s3     | 180    | [150, 210) | 0b011 = 3 |
            | s4     | 240    | [210, 270) | 0b001 = 1 |
            | s5     | 300    | [270, 330) | 0b101 = 5 |
            
            ```text
            
                 330         30
                    \       /
                     \  s0 /       
                      \   /          
                   s5  \ /  s1
            270 ------------------- 90 
                   s4  / \  s2
                      /   \        
                     / s3  \
                    /       \
                 210         150 
                 
            ```     

        #endif            
            
            // it contains the table from hall values H3H2H1 = [1, 2, 3, 4, 5, 6] to the center of the sector expressed in degrees
            static constexpr std::array<float, embot::hw::motor::bldc::hall::numsectors+2> degrees =
            {
                /* ABC  (?)  */
                /* LLL ERROR */   0.0,
                /* LLH  240  */ 240.0, 
                /* LHL  120  */ 120.0,
                /* LHH  180  */ 180.0, 
                /* HLL    0  */   0.0, 
                /* HLH  300  */ 300.0,
                /* HHL   60  */  60.0,
                /* HHH ERROR */  0.0
            };    
                            
            // it is the lookup table from hall value H3H2H1 = [1, 2, 3, 4, 5, 6] to sector.
            static constexpr std::array<uint8_t, embot::hw::motor::bldc::hall::numsectors+2> sectors = {255, 4, 2, 3, 0, 5, 1, 255};  

            static constexpr std::array<uint8_t, embot::hw::motor::bldc::hall::numsectors> statuses = {4, 6, 2, 3, 1, 5};              
              
        };

    public:    
        
        // marco.accame on 18 may 2026: i prefer to keep the h3h2h1, firstacquisition, and other data freely accessible rather than using get() / set()  

        static constexpr size_t statushistory {2};
        std::array<uint8_t, statushistory> h3h2h1 = {0, 0}; 
        static constexpr uint8_t PREV = 0;  // to be used as an index for h3h2h1
        static constexpr uint8_t CURR = 1;  // to be used as an index for h3h2h1
        
        
        bool firstacquisition {true};
        
        // the current sector
        uint8_t sector {0};  
        
        // angle from the detected sector
        float electricalangle {0.0f};   // in degrees
        
        // delta used for mechanical: 60 divided by number of polar pairs
        float deltamechanical {60.0/1.0f};
        
        // mechanical angle (computed in incremental steps of deltamechanical 
        float mechanicalangle {0.0};    
        
        int64_t mechanicalsteps {0}; // mechanicalangle = mechanicalsteps * deltamechanical;
        

        static constexpr bool isH3H2H1valid(uint8_t v)
        {
            return (0 != v) && (v < 7);
        }
        
        static constexpr bool isTransitionOfH3H2H1valid(uint8_t prev, uint8_t curr)
        {   // the prev and curr values must be valid
            // the following lut tells which h3h2h1 is expected for clockwise and counter clockwise rotation
            // given the value of the previous h3h2h1 which must be in range [1, 6]
            constexpr uint8_t nextclockwise[7] =        {255, 5, 3, 1, 6, 4, 2};
            constexpr uint8_t nextcounterclockwise[7] = {255, 3, 6, 2, 5, 1, 4};
            
            if(false == isH3H2H1valid(prev))
            {
                return false;
            }
            
            if((curr == prev) || (curr == nextclockwise[prev]) || (curr == nextcounterclockwise[prev]))
            {   // it is ok if the transition happens by a single step clockwise or counterclockise
                return true;
            }
            
            return false;
        }
        
        static constexpr int8_t incrementOfH3H2H1(uint8_t prev, uint8_t curr)
        {   // the prev and curr values must be valid
            // the following lut tells which h3h2h1 is expected for clockwise and counter clockwise rotation
            // given the value of the previous h3h2h1 which must be in range [1, 6]
            constexpr uint8_t nextclockwise[7] =        {255, 5, 3, 1, 6, 4, 2};
            constexpr uint8_t nextcounterclockwise[7] = {255, 3, 6, 2, 5, 1, 4};
            
            if(false == isH3H2H1valid(prev))
            {
                return 0;
            }
            
            if(curr == prev) 
            {
                return 0;
            }
            else if(curr == nextclockwise[prev])
            {
                return +1;
            }
            else if(curr == nextcounterclockwise[prev])
            {   
                return -1;
            }
            
            return 0;
        }    

        static constexpr float status2degrees(uint8_t hallstatus)
        {
            return LUT::degrees[hallstatus & 0x07];
        }

        static constexpr uint8_t status2sector(uint8_t hallstatus)
        {
            return LUT::sectors[hallstatus & 0x07];
        }        

        static constexpr uint8_t sector2status(uint8_t sect)
        {
            if(sect >= embot::hw::motor::bldc::hall::numsectors)
            {
                return 255;
            }
            return LUT::statuses[sect];
        }            
        
        void reset()
        {
            electricalangle = 0.0f;
            mechanicalangle = 0.0f;
            mechanicalsteps = 0;
            deltamechanical = 60.0/1.0f;
            sector = 0;
            h3h2h1[hallData::CURR] = h3h2h1[hallData::PREV] = 0;
            firstacquisition = true;
        }
           
        hallData() = default;
    };


    struct hall_Internals
    { 
        struct Item
        {
            bool started {false};
            Configuration config {};
            Mode mode {};    
            hallData data {};
                
            Item() = default;

            void reset()
            {
                started = false;
                //config.acquisition = Configuration::ACQUISITION::deferred;
                mode.clear();
                data.reset();            
            }
            
            void load(const Configuration &c) 
            {
                config = c;
            }
        };
        
        std::array<Item, embot::hw::motor::bldc::MAXnumber> _items {};
     
            
        hall_Internals() = default;   
            
            
        void reset(embot::hw::MOTOR m)
        {
            _items[embot::core::tointegral(m)].reset();
        } 

        void load(embot::hw::MOTOR m, const Configuration &c)
        {
            _items[embot::core::tointegral(m)].load(c);
        }  

        void load(embot::hw::MOTOR m, const Mode &mo)
        {
            _items[embot::core::tointegral(m)].mode = mo;
            _items[embot::core::tointegral(m)].mode.polarpairs = (mo.polarpairs == 0) ? 1 : mo.polarpairs;
            float div = static_cast<float>(_items[embot::core::tointegral(m)].mode.polarpairs);
            _items[embot::core::tointegral(m)].data.deltamechanical = 60.0f / div;        
        }  

        bool isstarted(embot::hw::MOTOR m) const
        {
            return _items[embot::core::tointegral(m)].started;
        }
        
        void setstarted(embot::hw::MOTOR m, bool yes) 
        {
             _items[embot::core::tointegral(m)].started = yes;
        }

//    #if defined(USE_Order)      
//        enum class Order { H3H2H1, H3H1H2, H2H3H1, H2H1H3, H1H2H3, H1H3H2 };
//        static constexpr Order order {Order::H3H2H1};
//    #endif
        
        uint8_t swap(embot::hw::MOTOR m, uint8_t threebits)
        {
            // we get the values of the three bits: H3|H2|H1. they correspond to C|B|A
            uint8_t x = threebits; // embot::hw::motor::hall::bsp::triple(m);
                            
            uint8_t v = x & 0b111; // already masked by hall_INPUT() but better be sure
            
//            if(embot::hw::motor::bldc::hall::Mode::SWAP::BC == _items[embot::core::tointegral(m)].mode.swap)
//            {
//                // in case swapBC is true, then we swap second (H2) w/ third (H3), so we have v = H2H3H1 = BCA
//            
//                v = ((x & 0b001)     )  |   // gets 0|0|H1 and keeps it where it is
//                    ((x & 0b010) << 1)  |   // gets 0|H2|0 and moves it up by one position
//                    ((x & 0b100) >> 1)  ;   // gets H3|0|0 and moves it down by one position
//            } 
//            
//            // in case we want a more general swap mode we can use the Order order variable 
//            // instead of Mode::SWAP and use following code
//    #if defined(USE_Order)       
            switch(_items[embot::core::tointegral(m)].mode.order)
            {
                default:
                case Mode::Order::H3H2H1:
                {
                    v = v;
                } break;
                
                case Mode::Order::H3H1H2:
                {
                    v = ((x & 0b001) << 1)  |   // gets 0|0|H1 and move it up by one position
                        ((x & 0b010) >> 1)  |   // gets 0|H2|0 and moves it down by one position
                        ((x & 0b100) >> 1)  ;   // gets H3|0|0 and keeps it where it is                
                } break;            
                
                case Mode::Order::H2H3H1:
                {
                    v = ((x & 0b001)     )  |   // gets 0|0|H1 and keeps it where it is
                        ((x & 0b010) << 1)  |   // gets 0|H2|0 and moves it up by one position
                        ((x & 0b100) >> 1)  ;   // gets H3|0|0 and moves it down by one position                
                } break;

                case Mode::Order::H2H1H3:
                {
                    v = ((x & 0b001) << 1)  |   // gets 0|0|H1 and moves it up by one position
                        ((x & 0b010) << 1)  |   // gets 0|H2|0 and moves it up by one position
                        ((x & 0b100) >> 2)  ;   // gets H3|0|0 and moves it down by two positions                
                } break;            

                case Mode::Order::H1H2H3:
                {
                    v = ((x & 0b001) << 2)  |   // gets 0|0|H1 and moves it up by two positions
                        ((x & 0b010) << 1)  |   // gets 0|H2|0 and keep it as it is
                        ((x & 0b100) >> 2)  ;   // gets H3|0|0 and moves it down by two positions                
                } break;   
     
                case Mode::Order::H1H3H2:
                {
                    v = ((x & 0b001) << 2)  |   // gets 0|0|H1 and moves it up by two positions
                        ((x & 0b010) >> 1)  |   // gets 0|H2|0 and move it down by one position
                        ((x & 0b100) >> 1)  ;   // gets H3|0|0 and moves it down by one positions               
                } break;             
            }
//    #endif

            return v;        
        }
        
        void process(embot::hw::MOTOR m, uint8_t hall)
        {
            // eval swap
            uint8_t ha = swap(m, hall);
                         
            // validate it vs possible states and possible transitions
            int8_t increment = 0;
            if(true == validate(m, ha, increment))
            {
                // i assign angles
                _items[embot::core::tointegral(m)].data.electricalangle = hallData::status2degrees(_items[embot::core::tointegral(m)].data.h3h2h1[hallData::CURR]);
                _items[embot::core::tointegral(m)].data.sector = hallData::status2sector(_items[embot::core::tointegral(m)].data.h3h2h1[hallData::CURR]);
//                #warning in order to avoid rounding errors we could use a new variable int64_t countsteps and then    
//                float delta = _items[embot::core::tointegral(m)].data.deltamechanical * static_cast<float>(increment);
//                _items[embot::core::tointegral(m)].data.mechanicalangle += delta;
                
                _items[embot::core::tointegral(m)].data.mechanicalsteps += increment;
                
                _items[embot::core::tointegral(m)].data.mechanicalangle = _items[embot::core::tointegral(m)].data.deltamechanical * _items[embot::core::tointegral(m)].data.mechanicalsteps;
                
                _items[embot::core::tointegral(m)].mode.onhall.execute(
                    _items[embot::core::tointegral(m)].data.mechanicalangle, 
                    _items[embot::core::tointegral(m)].data.electricalangle,  
                    _items[embot::core::tointegral(m)].data.h3h2h1[impl::hallData::CURR],
                   _items[embot::core::tointegral(m)].data.sector
                );

            }      
        }
               
        bool validate(embot::hw::MOTOR m, uint8_t v, int8_t &increment)
        {
            bool r {true};
            
            increment = 0;
                   
            // 1. validate h3h2h1    
            
            if(false == hallData::isH3H2H1valid(v))
            {
                // error
    //            data.h3h2h1[hallData::CURR] = data.h3h2h1[hallData::PREV];

                // RAISE ERROR STATE
    //            motorhal_set_fault(DHESInvalidValue);
                return false;
            }
            
            // 2. update h3h2h1 status   
            
            if(true == _items[embot::core::tointegral(m)].data.firstacquisition)
            {   
                // first time of call after a reset() i use current value v for both
                _items[embot::core::tointegral(m)].data.firstacquisition = false;
                _items[embot::core::tointegral(m)].data.h3h2h1[hallData::PREV] = _items[embot::core::tointegral(m)].data.h3h2h1[hallData::CURR] = v;
            }  
            else   
            {                    
                // i store previous h3h2h1 and i assign the new h3h2h1
                _items[embot::core::tointegral(m)].data.h3h2h1[hallData::PREV] = _items[embot::core::tointegral(m)].data.h3h2h1[hallData::CURR];
                _items[embot::core::tointegral(m)].data.h3h2h1[hallData::CURR] = v;
            }
            
            // 3. validate transition from previous h3h2h1 to the current one.
            //    we can directly use a transition table or to verify that the sectors that corresponds to the two h3h2h1 values
            //    are the same or spaced by one
            
            if(_items[embot::core::tointegral(m)].data.h3h2h1[hallData::PREV] != _items[embot::core::tointegral(m)].data.h3h2h1[hallData::CURR])
            {
                if(false == hallData::isTransitionOfH3H2H1valid(_items[embot::core::tointegral(m)].data.h3h2h1[hallData::PREV], _items[embot::core::tointegral(m)].data.h3h2h1[hallData::CURR]))
                {
    //                motorhal_set_fault(DHESInvalidSequence);
                    return false; 
                }           
            }

            increment = hallData::incrementOfH3H2H1(_items[embot::core::tointegral(m)].data.h3h2h1[hallData::PREV], _items[embot::core::tointegral(m)].data.h3h2h1[hallData::CURR]);
            
            return true;                
        }
        
        void encoderadjust(embot::hw::MOTOR m)
        {
            // in here we do what is done for amcbldc in function s_pwm_updateHallStatus() for the case when MainConf.encoder.resolution is not zero.
            // that means we have an encoder and we want the hall to help its acquisition.
            
            // but for now we dont do it        
        }        
   
    };


    hall_Internals _hall_internals {};


} // namespace embot::hw::motor::bldc::hall::impl {


namespace embot::hw::motor::bldc::hall::bsp {
    
    void BSP::capture(embot::hw::MOTOR m, uint8_t triple) const
    {
        embot::hw::motor::bldc::hall::impl::_hall_internals.process(m, triple);
    }
    
} // namespace embot::hw::motor::bldc::hall::bsp {

namespace embot::hw::motor::bldc::hall {

    uint8_t initialisedmask {0};
    
    bool init(embot::hw::MOTOR m, const Configuration &config)
    { 
        impl::_hall_internals.reset(m);    
        impl::_hall_internals.load(m, config);    
        embot::hw::motor::bldc::hall::bsp::getBSP().init(m);
        embot::core::binary::bit::set(initialisedmask, embot::core::tointegral(m));
        return true;     
    }

    bool deinit(embot::hw::MOTOR m)
    {
        embot::hw::motor::bldc::hall::bsp::getBSP().deinit(m);
        impl::_hall_internals.reset(m); 
        embot::core::binary::bit::clear(initialisedmask, embot::core::tointegral(m));        
        return true;     
    }
    
    bool initialised(MOTOR m)
    {
        return embot::core::binary::bit::check(initialisedmask, embot::core::tointegral(m));
    }      

    bool start(embot::hw::MOTOR m, const Mode &mode)
    { 
        bool ret {true};  
        uint8_t status {0};
        
        impl::_hall_internals.load(m, mode); 
        
        // i force the process of a freshly read triple
        status = embot::hw::motor::bldc::hall::bsp::getBSP().triple(m);        
        impl::_hall_internals.process(m, status);
        
        embot::hw::motor::bldc::hall::bsp::getBSP().start(m);
        
        impl::_hall_internals.setstarted(m, true); 
        
        // i force the process of a freshly read triple AGAIN
        status = embot::hw::motor::bldc::hall::bsp::getBSP().triple(m);        
        impl::_hall_internals.process(m, status);
           
        return ret;
    }

    bool stop(embot::hw::MOTOR m)
    {
        bool ret {true};  
                   
        embot::hw::motor::bldc::hall::bsp::getBSP().stop(m);
        impl::_hall_internals.reset(m);  
        impl::_hall_internals.setstarted(m, false); 
        
        return ret;
    }        

    bool isstarted(embot::hw::MOTOR m)
    {
        return impl::_hall_internals.isstarted(m); 
    }


    uint8_t getstatus(embot::hw::MOTOR m)
    {
        return impl::_hall_internals._items[embot::core::tointegral(m)].data.h3h2h1[impl::hallData::CURR];
    }   


    float angle(embot::hw::MOTOR m, embot::hw::motor::bldc::AngleType type)
    {
        float v {0.0};
        
        switch(type)
        {
            case embot::hw::motor::bldc::AngleType::hall_electrical:
            {
                v = impl::_hall_internals._items[embot::core::tointegral(m)].data.electricalangle;
            } break;
            
            case embot::hw::motor::bldc::AngleType::hall_mechanical: 
            {
                v = impl::_hall_internals._items[embot::core::tointegral(m)].data.mechanicalangle;
                
            } break;
            
            default: 
            {
                v  = 0.0;
            } break;
        }
        
        return v;
    }

    uint8_t sector(embot::hw::MOTOR m)
    {
        return impl::_hall_internals._items[embot::core::tointegral(m)].data.sector;
    }
    
    
    bool status2sector(embot::hw::MOTOR m, uint8_t status, uint8_t &sector) 
    { 
        if(false == impl::hallData::isH3H2H1valid(status))
        {
            return false;
        }
        
        sector = impl::hallData::status2sector(status);
        return true;        
    }
    
    
    bool sector2status(embot::hw::MOTOR m, uint8_t sector, uint8_t &status)
    { 
        if(sector >= numsectors)
        {
            return false;
        }
        
        status = impl::hallData::sector2status(sector);

        return true;        
    }    

} // namespace embot::hw::motor::hall {



#endif // #elif defined(EMBOT_ENABLE_hw_motor_hall)

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

