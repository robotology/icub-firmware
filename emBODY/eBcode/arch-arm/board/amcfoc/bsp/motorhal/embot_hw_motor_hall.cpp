
/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/




// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------


#include "embot_hw_motor_hall.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_enc.h"

#include "embot_core.h"
#include <array>


// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


#if !defined(EMBOT_ENABLE_hw_motor_hall)

#warning EMBOT_ENABLE_hw_motor is undefined, so we use dummy implementation 

namespace embot::hw::motor::hall {
    
    bool init(embot::hw::MOTOR m, const Configuration &config) { return false; }
    bool deinit(embot::hw::MOTOR m) { return false; }
    bool start(embot::hw::MOTOR m, const Mode &mode) { return false; }
    bool isstarted(embot::hw::MOTOR m) { return false; };
    uint8_t getstatus(embot::hw::MOTOR m) { return 0; }    
    int32_t getangle(embot::hw::MOTOR m) { return 0; }  
    float angle(embot::hw::MOTOR m) { return 0.0; }
    
}

#elif defined(EMBOT_ENABLE_hw_motor_hall)

#if defined(STM32HAL_BOARD_AMCFOC_1CM7) 
#include "embot_hw_motor_bldc_bsp_amcfoc_1cm7.h"
#endif

#include "embot_hw_gpio.h"

    
namespace embot::hw::motor::hall::bsp {
    
    void init(embot::hw::MOTOR m);
    uint8_t triple(embot::hw::MOTOR m);
}

namespace embot::hw::motor::hall {
    

    
struct hall_Table
{
    static constexpr int16_t numHallSectors = 6;
    static constexpr float_t iCubDeg = 65536.0; // Range of angular values that can be represented with the encoder
    static constexpr float_t Deg2iCub = iCubDeg / 360.0;
    static constexpr int16_t hallAngleStep = 60.0 * Deg2iCub; // 60 degrees scaled by the range of possible values
    static constexpr int16_t minHallAngleDelta = 30.0 * Deg2iCub; // 30 degrees scaled by the range of possible values    

    // it keeps the table from hall values H3h2H1 = [1, 2, 3, 4, 5, 6] to the center of the sector expressed in icub degrees 
    static constexpr uint16_t hallAngleTable[] =
    {
        /* ABC  (?)  */
        /* LLL ERROR */ static_cast<uint16_t>(0),
        /* LLH  240  */ static_cast<uint16_t>(240.0 * Deg2iCub), /* 43690 */
        /* LHL  120  */ static_cast<uint16_t>(120.0 * Deg2iCub), /* 21845 */
        /* LHH  180  */ static_cast<uint16_t>(180.0 * Deg2iCub), /* 32768 */
        /* HLL    0  */ static_cast<uint16_t>(  0.0 * Deg2iCub), /*     0 */
        /* HLH  300  */ static_cast<uint16_t>(300.0 * Deg2iCub), /* 54613 */
        /* HHL   60  */ static_cast<uint16_t>( 60.0 * Deg2iCub), /* 10922 */
        /* HHH ERROR */ static_cast<uint16_t>(0)
    };
    
    
    static constexpr float hallDegreeTable[] =
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
    // the sector number x is centered in angle x*60 [deg] and is 60 deg wide
    static constexpr uint8_t hallSectorTable[] = {255, 4, 2, 3, 0, 5, 1, 255};
    
    static constexpr uint16_t status2angle(uint8_t hallstatus)
    {
        return hallAngleTable[hallstatus & 0x07];
    }
    
    static constexpr float status2degrees(uint8_t hallstatus)
    {
        return hallDegreeTable[hallstatus & 0x07];
    }

    static constexpr uint8_t status2sector(uint8_t hallstatus)
    {
        return hallSectorTable[hallstatus & 0x07];
    }
    
    static constexpr uint8_t angle2sector(uint16_t angle)
    {    
        return ((minHallAngleDelta + angle) / hallAngleStep) % numHallSectors;
    }

    constexpr hall_Table() = default;
};   

struct hall_Data
{
    static constexpr uint8_t PREV = 0;
    static constexpr uint8_t CURR = 1;
    
    
    // electrical angles
    int32_t electricalangleicubdeg {0};      // in icub degrees
    float electricalangle {0.0f};   // in degrees
    
    // delta
    float deltamechanical {60.0/1.0f};
    
    // mechanical angles (computed in incremental steps and divided by polar couples)
    float mechanicalangle {0.0};    
    
    uint8_t sector {0};

    std::array<uint8_t, 2> h3h2h1 = {0, 0}; 

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
//        #warning .... rimuovere il return qui
//        return true;
        
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
    
    bool firstacquisition {true};       
    
    void reset()
    {
        electricalangleicubdeg = 0;
        electricalangle = 0.0f;
        mechanicalangle = 0.0f;
        deltamechanical = 60.0/1.0f;
        sector = 0;
        h3h2h1[hall_Data::CURR] = h3h2h1[hall_Data::PREV] = 0;
        firstacquisition = true;
    }
    

    
    hall_Data() = default;
};

//embot::hw::motor::bldc::MAXnumber
struct hall_Internals
{ 
    struct Item
    {
        bool started {false};
        Configuration config {};
        Mode mode {};    
        hall_Data data {};
            
        Item() = default;

        void reset()
        {
            started = false;
            config.acquisition = Configuration::ACQUISITION::deferred;
            mode.reset();
            data.reset();            
        }
        
        void load(const Configuration &c) 
        {
            config = c;
        }
    };
    
    std::array<Item, embot::hw::motor::bldc::MAXnumber> _items {};
    
//    bool started {false};
//    Configuration config {};
//    Mode mode {};    
//    hall_Data data {};
      
        
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
        
    enum class Order { H3H2H1, H3H1H2, H2H3H1, H2H1H3, H1H2H3, H1H3H2 };
    static constexpr Order order {Order::H3H2H1};
    
    uint8_t swap(embot::hw::MOTOR m, uint8_t threebits)
    {
        // we get the values of the three bits: H3|H2|H1. they correspond to C|B|A
        uint8_t x = threebits; // embot::hw::motor::hall::bsp::triple(m);
        
        
        
        uint8_t v = x & 0b111; // already masked by hall_INPUT() but better be sure
        
        if(embot::hw::motor::hall::Mode::SWAP::BC == _items[embot::core::tointegral(m)].mode.swap)
        {
            // in case swapBC is true, then we swap second (H2) w/ third (H3), so we have v = H2H3H1 = BCA
        
            v = ((x & 0b001)     )  |   // gets 0|0|H1 and keeps it where it is
                ((x & 0b010) << 1)  |   // gets 0|H2|0 and moves it up by one position
                ((x & 0b100) >> 1)  ;   // gets H3|0|0 and moves it down by one position
        } 
        
//        switch(order)
//        {
//            default:
//            case Order::H3H2H1:
//            {
//                v = v;
//            } break;
//            
//            case Order::H3H1H2:
//            {
//                v = ((x & 0b001) << 1)  |   // gets 0|0|H1 and move it up by one position
//                    ((x & 0b010) >> 1)  |   // gets 0|H2|0 and moves it down by one position
//                    ((x & 0b100) >> 1)  ;   // gets H3|0|0 and keeps it where it is                
//            } break;            
//            
//            case Order::H2H3H1:
//            {
//                v = ((x & 0b001)     )  |   // gets 0|0|H1 and keeps it where it is
//                    ((x & 0b010) << 1)  |   // gets 0|H2|0 and moves it up by one position
//                    ((x & 0b100) >> 1)  ;   // gets H3|0|0 and moves it down by one position                
//            } break;

//            case Order::H2H1H3:
//            {
//                v = ((x & 0b001) << 1)  |   // gets 0|0|H1 and moves it up by one position
//                    ((x & 0b010) << 1)  |   // gets 0|H2|0 and moves it up by one position
//                    ((x & 0b100) >> 2)  ;   // gets H3|0|0 and moves it down by two positions                
//            } break;            

//            case Order::H1H2H3:
//            {
//                v = ((x & 0b001) << 2)  |   // gets 0|0|H1 and moves it up by two positions
//                    ((x & 0b010) << 1)  |   // gets 0|H2|0 and keep it as it is
//                    ((x & 0b100) >> 2)  ;   // gets H3|0|0 and moves it down by two positions                
//            } break;   
// 
//            case Order::H1H3H2:
//            {
//                v = ((x & 0b001) << 2)  |   // gets 0|0|H1 and moves it up by two positions
//                    ((x & 0b010) >> 1)  |   // gets 0|H2|0 and move it down by one position
//                    ((x & 0b100) >> 1)  ;   // gets H3|0|0 and moves it down by one positions               
//            } break;             
//        }

        return v;        
    }
    
    void process(embot::hw::MOTOR m, uint8_t hall)
    {
        // actually ... we
        // i validate it vs possible states and possible transitions       
        uint8_t ha = swap(m, hall);
        
//        static uint64_t lasT = 0;
//        uint64_t n = embot::core::now();
//        float delta = (n - lasT)/1000000.0;
//        
//        lasT = n;
//        static float poscum = 0;
             
        int8_t increment = 0;
        if(true == validate(m, ha, increment))
        {
            // i assign angles
            _items[embot::core::tointegral(m)].data.electricalangleicubdeg = _items[embot::core::tointegral(m)].mode.offset + hall_Table::status2angle(_items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::CURR]);  
            _items[embot::core::tointegral(m)].data.electricalangle = hall_Table::status2degrees(_items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::CURR]);
            _items[embot::core::tointegral(m)].data.sector = hall_Table::status2sector(_items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::CURR]);
            
            float delta = _items[embot::core::tointegral(m)].data.deltamechanical * static_cast<float>(increment);
            _items[embot::core::tointegral(m)].data.mechanicalangle += delta;
            
//            float prevposcum = poscum;
//            poscum += (60/7.0);
//            float v = (60.0/7.0) / delta;
//            float rpm = 60.0 * (v / 360.0);
//            embot::core::TimeFormatter tf(n);    
//            embot::core::print(tf.to_string() + "HALL capture: h3h2h1 = " + std::to_string(ha) + ", vel = " + std::to_string(v) + " deg/s or " + std::to_string(rpm) + " rpm");               
//            
            
            // i may do....
            
//            switch(config.encodertuning)
//            {
//                case Configuration::ENCODERtuning::forcevalue:
//                {   
//                    embot::hw::motor::enc::force(data.electricalangleicubdeg);
//                } break;

//                case Configuration::ENCODERtuning::adjust:
//                {   
//                    encoderadjust();
//                } break;
//                 
//                case Configuration::ENCODERtuning::none:
//                default:
//                {   
//                    // do nothing
//                } break;               
//            }

        }      
    }
    
        
    bool validate(embot::hw::MOTOR m, uint8_t v, int8_t &increment)
    {
        bool r {true};
        
        increment = 0;
               
        // 1. validate h3h2h1
                
        if(false == hall_Data::isH3H2H1valid(v))
        {
            // error
//            data.h3h2h1[hall_Data::CURR] = data.h3h2h1[hall_Data::PREV];

            // RAISE ERROR STATE
//            motorhal_set_fault(DHESInvalidValue);
            return false;
        }
        
        // 2. update h3h2h1 status
        
        // first time of call after a reset()
        if(true == _items[embot::core::tointegral(m)].data.firstacquisition)
        {
            _items[embot::core::tointegral(m)].data.firstacquisition = false;
            // i use current value v for both
            _items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::PREV] = _items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::CURR] = v;
        }  
        else   
        {                    
            // i store previous h3h2h1 and i assign the new h3h2h1
            _items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::PREV] = _items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::CURR];
            _items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::CURR] = v;
        }
        
        // 3. validate transition from previous h3h2h1 to the current one.
        //    we can directly use a transition table or to verify that the sectors that corresponds to the two h3h2h1 values
        //    are the same or spaced by one
        
        if(_items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::PREV] != _items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::CURR])
        {
            if(false == hall_Data::isTransitionOfH3H2H1valid(_items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::PREV], _items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::CURR]))
            {
//                motorhal_set_fault(DHESInvalidSequence);
                return false; 
            }           
        }

        increment = hall_Data::incrementOfH3H2H1(_items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::PREV], _items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::CURR]);
        
        return true;                
    }
    
    void encoderadjust(embot::hw::MOTOR m)
    {
        // in here we do what is done for amcbldc in function s_pwm_updateHallStatus() for the case when MainConf.encoder.resolution is not zero.
        // that means we have an encoder and we want the hall to help its acquisition.
        
#if 0
        if (!s_pwm_hallStatus_old)
        {            
            hallCounter = 0;
            encoder_Reset();
            encoder_Force(electricalangleicubdeg);
        }
        else
        {
            // Check if the motor is rotating forward (counterclockwise)
            bool forward = ((sector-sector_old+numHallSectors)%numHallSectors)==1;
        
            if (forward) // forward
            {
                ++hallCounter;
                electricalangleicubdeg -= minHallAngleDelta; // -30 deg
            }
            else
            {
                --hallCounter;
                electricalangleicubdeg += minHallAngleDelta; // +30 deg
            }
             /*
            0) Use the Hall sensors to rotate until the wrap-around border is reached,
             then reset the encoder value
            */
            if (calibration_step == 0)
            {
                encoder_Force(electricalangleicubdeg);
                
                if ((sector == 0 && sector_old == 5) || (sector == 5 && sector_old == 0))
                {
                    encoder_Reset();
                    calibration_step = 1;
                }
            }
            /*
            1) Use the hall sensors to rotate. While rotating, store the encoder angle 
            every time a sector border is crossed. When all 6 borders are crossed,
            compute the offset to apply to the encoder by least squares fitting. After
            the offset is applied set encoderCalibrated to true
            */
            else if (calibration_step == 1)
            {
                encoder_Force(electricalangleicubdeg);
            
                // use the current sector if forward rotation or previous if reverse
                uint8_t sector_index = forward ? sector : (sector+1)%numHallSectors;
            
                // keep track of the encoder value between sectors
                border[sector_index] = encoder_GetUncalibrated();

                // found the s-th border, put a 1 in the mask
                border_flag |= 1 << sector_index;
                
                // If all sectors are found apply least squares fitting by computing average of
                // difference between measured values on borders and expected hall angle
                if (border_flag == 63) // 111111
                {
                    calibration_step = 2;
                
                    int32_t offset = int16_t(border[0] - MainConf.pwm.hall_offset + minHallAngleDelta);
                    offset += int16_t(border[1] - MainConf.pwm.hall_offset + minHallAngleDelta - hallAngleStep);
                    offset += int16_t(border[2] - MainConf.pwm.hall_offset + minHallAngleDelta - 2 * hallAngleStep);
                    offset += int16_t(border[3] - MainConf.pwm.hall_offset + minHallAngleDelta - 3 * hallAngleStep);
                    offset += int16_t(border[4] - MainConf.pwm.hall_offset + minHallAngleDelta - 4 * hallAngleStep);
                    offset += int16_t(border[5] - MainConf.pwm.hall_offset + minHallAngleDelta - 5 * hallAngleStep);
                
                    offset /= numHallSectors;
                     
                    embot::core::print("CALIBRATED\n");
                    
                    encoder_Calibrate(int16_t(offset));
                }
            }
            /*
            2) Update the forced value even if it is not used when the encoder is calibrated.
            Reset the encoder angle after a full rotation to avoid desynching
            */
            else if (calibration_step == 2)
            {
                encoder_Force(electricalangleicubdeg);
                // reset the angle after full rotation
                if ((sector == 0 && sector_old == 5) || (sector == 5 && sector_old == 0))
                {
                    encoder_Reset();
                }
            }

#endif        
        
    }
        
    
    static void capture(embot::hw::MOTOR m, hall_Internals &hint)
    {
        uint8_t hall = embot::hw::motor::hall::bsp::triple(m);
        hint.process(m, hall);        
    }    
};


hall_Internals _hall_internals {};


static void hall_OnCaptureM1(TIM_HandleTypeDef *htim)
{
    hall_Internals::capture(embot::hw::MOTOR::one, _hall_internals);
}


static void hall_OnCaptureM2(TIM_HandleTypeDef *htim)
{
    hall_Internals::capture(embot::hw::MOTOR::two, _hall_internals);
}


bool deinit(embot::hw::MOTOR m)
{
    _hall_internals.reset(m);
       
//    HallDeInit();
    
    return true;     
}

bool init(embot::hw::MOTOR m, const Configuration &config)
{ 
//    HallDeInit();
    
    embot::hw::motor::hall::bsp::init(m);
    
    _hall_internals.reset(m);
    
    _hall_internals.load(m, config);
    
    // _hall_internals.config.acquisition is always ACQUISITION::deferred
    
    return true;     
}

#warning manage motor 2 by uing htimHALL1 or htimHALL2
bool start(embot::hw::MOTOR m, const Mode &mode)
{ 
    bool ret {false};  

    _hall_internals.load(m, mode);   
        
    hall_Internals::capture(m, _hall_internals);

    if(0 == embot::core::tointegral(m))
    {
        // marco.accame: i stop things
        
        /* Stop counter */
        HAL_TIM_Base_Stop(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL1);
        /* Stop interrupt */
        HAL_TIM_IC_Stop(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL1, TIM_CHANNEL_1);
        
        // marco.accame: i register the callback and start counter.
        /* Register the callback function for CH1 capture event */
        if (HAL_OK == HAL_TIM_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL1, HAL_TIM_IC_CAPTURE_CB_ID, hall_OnCaptureM1))
        {
            /* Start counter */
            if (HAL_OK == HAL_TIM_Base_Start(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL1))
            {
    // marco.accame: i DONT do this call HallSetPwm() 
    //            /* Zero amplitude field */
    //            HallSetPwm(0);
                /* Start channel 1 in interrupt mode */
                if (HAL_OK == HAL_TIM_IC_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL1, TIM_CHANNEL_1))
                {
                    ret = true;
                }
                else
                {
                    ret = false;
                    /* Stop counter */
                    HAL_TIM_Base_Stop(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL1);
                    /* Disable all */
    //                PwmPhaseEnable(PWM_PHASE_NONE);
                }
            }
            else
            {
                ret = false;
                /* Remove the callback function */
                HAL_TIM_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL1, HAL_TIM_IC_CAPTURE_CB_ID);
            }
        }    
    
    }   
    else if (1 == embot::core::tointegral(m))
    {
        // marco.accame: i stop things
        
        /* Stop counter */
        HAL_TIM_Base_Stop(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL2);
        /* Stop interrupt */
        HAL_TIM_IC_Stop(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL2, TIM_CHANNEL_1);
        
        // marco.accame: i register the callback and start counter.
        /* Register the callback function for CH1 capture event */
        if (HAL_OK == HAL_TIM_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL2, HAL_TIM_IC_CAPTURE_CB_ID, hall_OnCaptureM2))
        {
            /* Start counter */
            if (HAL_OK == HAL_TIM_Base_Start(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL2))
            {
    // marco.accame: i DONT do this call HallSetPwm() 
    //            /* Zero amplitude field */
    //            HallSetPwm(0);
                /* Start channel 1 in interrupt mode */
                if (HAL_OK == HAL_TIM_IC_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL2, TIM_CHANNEL_1))
                {
                    ret = true;
                }
                else
                {
                    ret = false;
                    /* Stop counter */
                    HAL_TIM_Base_Stop(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL2);
                    /* Disable all */
    //                PwmPhaseEnable(PWM_PHASE_NONE);
                }
            }
            else
            {
                ret = false;
                /* Remove the callback function */
                HAL_TIM_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::cm7::htimHALL2, HAL_TIM_IC_CAPTURE_CB_ID);
            }
        }    
    }
    _hall_internals._items[embot::core::tointegral(m)].started = ret;
    
    // i read it again
    hall_Internals::capture(m, _hall_internals);
    
    
    return ret;
}

bool isstarted(embot::hw::MOTOR m)
{
    return _hall_internals._items[embot::core::tointegral(m)].started;
}

uint8_t getstatus(embot::hw::MOTOR m)
{
    return _hall_internals._items[embot::core::tointegral(m)].data.h3h2h1[hall_Data::CURR];
}   

int32_t getangle(embot::hw::MOTOR m)
{
    return _hall_internals._items[embot::core::tointegral(m)].data.electricalangleicubdeg;
} 


float angle(embot::hw::MOTOR m, embot::hw::motor::bldc::AngleType type)
{
    return (embot::hw::motor::bldc::AngleType::hall_electrical == type) ? _hall_internals._items[embot::core::tointegral(m)].data.electricalangle : _hall_internals._items[embot::core::tointegral(m)].data.mechanicalangle;
}

uint8_t sector(embot::hw::MOTOR m)
{
    return _hall_internals._items[embot::core::tointegral(m)].data.sector;
}

} // namespace embot::hw::motor::hall {




namespace embot::hw::motor::hall::bsp {
    
    using gpioTriple = std::array<embot::hw::GPIO, 3>;
    constexpr std::array<gpioTriple, 2> hallports
    {{
        {{  // MOTOR::one
            {embot::hw::GPIO::PORT::D, embot::hw::GPIO::PIN::twelve}, 
            {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::seven},  
            {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::eight}
        }},
        {{  // MOTOR::two
            {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::four}, 
            {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::five},  
            {embot::hw::GPIO::PORT::B, embot::hw::GPIO::PIN::zero}
        }}
    }}; 

    
    constexpr embot::hw::gpio::Config cfg {embot::hw::gpio::Mode::OUTPUTpushpull, embot::hw::gpio::Pull::nopull, embot::hw::gpio::Speed::veryhigh };
    
    
    void init(embot::hw::MOTOR m)
    {
        // already initted by timer ....
//        uint8_t mo = embot::core::tointegral(m);
//        for(const auto &i : hallports[mo])
//        {
//            embot::hw::gpio::init(i, cfg);
//        }
    }

    uint8_t triple(embot::hw::MOTOR m)
    {
        uint8_t mo = embot::core::tointegral(m);
        uint8_t v = 0;
        for(size_t i=0; i<3; i++)
        {
            if(embot::hw::gpio::State::SET == embot::hw::gpio::get(hallports[mo][i]))
            {
                embot::core::binary::bit::set(v, i);
            }
        }
        
 //       embot::core::print("hall value for " + embot::hw::motor::bldc::to_string(m) + " = " + std::to_string(v));
        
        return v;        
    }
    
}

namespace embot::hw::motor::hall::bsp::debug {
    
    // i assume s = [0, 5]. if not i return hall of sector 0
    uint8_t sector2hall(uint8_t s, bool swapbc)
    {
        constexpr uint8_t halltable[6] =          { 0b100, 0b110, 0b010, 0b011, 0b001, 0b101 };
        constexpr uint8_t halltableBCswapped[6] = { 0b010, 0b110, 0b100, 0b101, 0b001, 0b011 };
        
        if(false == swapbc)
            return (s<6) ? halltable[s] : halltable[0];
        else
            return (s<6) ? halltable[s] : halltable[0];
    }
    
    int mymod(int a, int b)
    {
        int r = a % b;
        return r < 0 ? r + b : r;
    }

    uint8_t mechanicalangle2hall(float mechangle, uint8_t npoles, bool swapbc)
    {       
        int32_t elecangle = mymod(static_cast<int32_t>(mechangle * static_cast<float>(npoles)), 360);
        // now i have elecngle in [0, 360)
        
        // i set it to be 
        
        // i get sector
        uint8_t s = (elecangle + 30) / 60;
        
        return sector2hall(s, swapbc);
    }

//     return (embot::hw::motor::bldc::AngleType::hall_electrical == type) ? _hall_internals._items[embot::core::tointegral(m)].data.electricalangle : _hall_internals._items[embot::core::tointegral(m)].data.mechanicalangle;
    volatile bool stop {false};   
    void set(embot::hw::MOTOR m, float angle, embot::hw::motor::bldc::AngleType type)
    {
        if(embot::hw::motor::bldc::AngleType::hall_mechanical == type)
        {
            uint8_t npoles = _hall_internals._items[embot::core::tointegral(m)].mode.polarpairs;
            bool swapbc = (_hall_internals._items[embot::core::tointegral(m)].mode.swap == Mode::SWAP::BC);
            uint8_t hall = mechanicalangle2hall(angle, npoles, swapbc);
            // and now i load the hall value
            
            _hall_internals.process(m, hall);
            
        }
        
    }
    
  
    
    struct Wave::Impl    
    {
        Config _config {};  
        
        uint64_t _step {0};
        embot::core::Time _time {0};
        float _incr {0.0f};
        float _value {0};

        bool configure(const Config &cfg) 
        { 
            _config = cfg;    

            _incr = _config.ticktime * _config.maxvalue / (_config.period/2);
            
            return true; 
        }
        
        void tick()
        {
            _step++;
            _time += _config.ticktime;
            if(_time <= (_config.period/2))
            {
                _value += _incr; 
            }
            else if(_time < (_config.period))
            {
                _value -= _incr;
            }
            else
            {
                _step = 0;
                _time = 0;
                _value = 0.0;
            }            
        }
            
        float get() const { return _value; }
    };
 


    
    Wave::Wave()
    : pImpl(new Impl())
    { 

    }

    Wave::~Wave()
    {   
        delete pImpl;
    }

    bool Wave::configure(const Config &cfg)
    {
        return pImpl->configure(cfg);
    }   

    
    void Wave::tick()
    {
        pImpl->tick();
    }
        
    float Wave::get() const
    {
        return pImpl->get();
    }
    
}


#endif // #elif defined(EMBOT_ENABLE_hw_motor_hall)

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

