
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/




// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "motorhal.h"


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "adcm.h"
#include "enc.h"
#include "hall.h"
#include "pwm.h"
#include "motorhal_faults.h"

#include "embot_core.h"
#include <array>

#include "embot_hw_motor_bsp_amc2c.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------

namespace embot::hw::motor::adc {
    

#define hadc1 (embot::hw::motor::bsp::amc2c::hADC1)
#define hadc2 (embot::hw::motor::bsp::amc2c::hADC2)

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
    
    volatile bool _done {false};
    volatile size_t _count {0};
    Currents _currents {};
    std::array<uint64_t, 3> cumulativerawvalues {0, 0, 0};
    

    std::array<Converter, 3> _conv {};
    
    constexpr Calibrator() = default;
    
    void init()
    {
       for(auto &i : _conv) { i.clear(); } // but the clear is not necessary
       _count = 0;
       _done = false; 
       cumulativerawvalues.fill(0); 
       embot::hw::motor::adc::set({oncurrentscalib, this});
    }
    
    void stop()
    {
        embot::hw::motor::adc::set({});
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


    static void oncurrentscalib(void *owner, const Currents * const currents)
    {
        Calibrator *o = reinterpret_cast<Calibrator*>(owner);   
     
        // currents is in mA 
     
        // i use Converter::current2raw() because technically Currents does not contain the raw ADC values but transformed values
        o->cumulativerawvalues[0] += o->_conv[0].current2raw(currents->u);
        o->cumulativerawvalues[1] += o->_conv[1].current2raw(currents->v);
        o->cumulativerawvalues[2] += o->_conv[2].current2raw(currents->w);
        
        o->_count++;
        
        if(o->_count >= Calibrator::_maxcount)
        {
            // time to do business: prepare average currents, impose the offset to ADC, stop the calibrator  
            
            // dont use the >> _shift because ... 1. this operation is done only once, so who bother. 
            //                                    2. cumulativerawvalues is int64_t, so shift does niot work for negative values
            // impose offset to adc.
            volatile int64_t cc[3] {0, 0, 0};
            cc[0] = o->cumulativerawvalues[0] / o->_count;
            cc[1] = o->cumulativerawvalues[1] / o->_count;
            cc[2] = o->cumulativerawvalues[2] / o->_count;
            
            o->_conv[0].set(cc[0]);
            o->_conv[1].set(cc[1]);
            o->_conv[2].set(cc[2]);
            
            // stop: deregister this callback
            o->stop();
        }                     
    }    
    
};

struct adcm_Internals
{
    Configuration config {};
    Calibrator calibrator {};
        
    static void dummy_adc_callback(void *owner, const Currents * const currents) {}  
    static constexpr OnCurrents dummyADCcbk { dummy_adc_callback, nullptr };          
        
    adcm_Internals() = default;    
};

adcm_Internals _adcm_internals {};
    
// this is the callback executed inside the ADC handler  
// {c1, c2, c3} are in adc units [0, 64k)
// so, we need to transform them into currents expressed in milli-ampere
    // and call the callback imposed by the embot::hw::motor::setCallbackOnCurrents()
void adcm_on_acquisition_of_currents(uint16_t c1, uint16_t c2, uint16_t c3)
{
    // important note: {{c1, c2, c3} are in adc units [0, 64k). no conversion yet. 
    Currents currents = 
    {
        _adcm_internals.calibrator._conv[0].raw2current(c1), 
        _adcm_internals.calibrator._conv[1].raw2current(c2),
        _adcm_internals.calibrator._conv[2].raw2current(c3),
    };
    _adcm_internals.config.oncurrents.execute(&currents);         
}   
    
bool init(const Configuration &config)
{
    bool r {true};
    
    if(false == config.isvalid())
    {
        return false;
    }
    
    _adcm_internals.config = config;
    
    // 1. at first we want to be sure that everything is stopped, so:
    deinit();  
    
    adcm_FP_on_acquisition_of_currents = adcm_on_acquisition_of_currents;

    // 2. then we init

    HAL_StatusTypeDef hr = AdcMotInit();      
    r = (HAL_ERROR == hr) ? false : true;       

    
    return r;   
}  

bool calibrate(const Calibration &calib)
{
    bool r {true};
    
    if(calib.calibration != Calibration::CALIBRATION::current)
    {
        return r;
    }
    // in here we need to have zero pwm, so we force it
    PwmPhaseSet(0, 0, 0);
    PwmPhaseEnable(PWM_PHASE_NONE);   
    
    _adcm_internals.calibrator.init();

    r = _adcm_internals.calibrator.wait(calib.timeout);
   
    return r;   
}    


void set(const OnCurrents &cbk)
{
    // maybe better to protect execution of s_adcm_ADC_cbk by ...
    // temporarily suspending the AdcMotTransferComplete_cb() and AdcMotHalfTransfer_cb()  
    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
    HAL_ADC_UnRegisterCallback(&hadc1, HAL_ADC_CONVERSION_HALF_CB_ID);
    
    _adcm_internals.config.oncurrents = cbk.isvalid() ? cbk : _adcm_internals.dummyADCcbk;
    
    HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AdcMotTransferComplete_cb);
    HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_HALF_CB_ID, AdcMotHalfTransfer_cb);    
}

// as AdcMotDeInit() but ... see the changes in analog_DeInit() of amcbldc
bool deinit()
{
    bool r = true;    
    // AdcMotDeInit() unchanged is OK 
    // AdcMotDeInit();
    // i also clear the callback
    set({});    
    return r;    
}
   
    
    
} // namespace embot::hw::motor::adc {


namespace embot::hw::motor::enc {
    
#define ENC_TIM (embot::hw::motor::bsp::amc2c::hTIM5)
    
#define ENC_INDEX_LEADING_EDGE          TIM_CHANNEL_3
#define ENC_INDEX_IT_LEADING_EDGE       TIM_IT_CC3
#define ENC_INDEX_ACTIVE_LEADING_EDGE   HAL_TIM_ACTIVE_CHANNEL_3
#define ENC_INDEX_TRAILING_EDGE         TIM_CHANNEL_4
#define ENC_INDEX_IT_TRAILING_EDGE      TIM_IT_CC4
#define ENC_INDEX_ACTIVE_TRAILING_EDGE  HAL_TIM_ACTIVE_CHANNEL_4
    

struct enc_Conversion
{
    int32_t offset {0};
    int32_t factor {1}; 
    
    int32_t convert(int32_t v) const
    {
       return offset + (factor*v);
    }

    constexpr enc_Conversion() = default;  
    constexpr enc_Conversion(int32_t o, int32_t f) : offset(o), factor(f) {}        
};

struct enc_Internals
{
    bool started {false};
    Configuration config {};
    Mode mode {};
    enc_Conversion conversion {};
    int32_t forcedvalue {0};    
        
    enc_Internals() = default;   
};


enc_Internals _enc_internals {};

extern bool init(const Configuration &config)
{
    EncDeInit();
    _enc_internals.started = false;
    _enc_internals.config = config;
    return true;
}

extern bool deinit()
{
    /* Stop any pending operation */
    HAL_TIM_IC_Stop_IT(&ENC_TIM, ENC_INDEX_TRAILING_EDGE);
    HAL_TIM_IC_Stop(&ENC_TIM, ENC_INDEX_LEADING_EDGE);
    HAL_TIM_Encoder_Stop(&ENC_TIM, TIM_CHANNEL_ALL);

    embot::hw::motor::bsp::amc2c::DeInit_TIM5();    
    
    _enc_internals.started = false;
    
    return true;
}

extern bool start(const Mode& mode)
{
    if(false == mode.isvalid())
    {
        return false;
    }
    
    if(true == _enc_internals.started)
    {   // if already started i first stop. i can do that by calling init({})
        init({});
    }        
           
    
    _enc_internals.mode = mode;
    
    #warning marco.accame: so far we dont manage mode.calibrate_with_hall and mode.use_index
    
    _enc_internals.conversion.factor = 65536L*_enc_internals.mode.num_polar_couples/_enc_internals.mode.resolution;
        
    embot::hw::motor::bsp::amc2c::Init_TIM5(_enc_internals.mode.resolution, _enc_internals.mode.num_polar_couples);
    
    // and now i can do what is required ... i also start the index ... who cares
    
    bool ret = true;
    
    #warning TODO: we can use EncInit() in here

    /* Register the callback function used to signal the activation of the Index pulse */
    if (HAL_OK == HAL_TIM_RegisterCallback(&ENC_TIM, HAL_TIM_IC_CAPTURE_CB_ID, EncCapture_cb))
    {
        /* Clear local variables */
//        EncStatus = ENC_STATUS_IDLE;
//        EncAbsoluteZero = 0;
//        EncRotorZero = 0;
        /* Clear counter */
        __HAL_TIM_SET_COUNTER(&ENC_TIM, 0);
        /* Start timers in encoder mode */
        if (HAL_OK == HAL_TIM_Encoder_Start(&ENC_TIM, TIM_CHANNEL_ALL))
        {
            /* Enable leading edge capture, without interrupts */
            HAL_TIM_IC_Start(&ENC_TIM, ENC_INDEX_LEADING_EDGE);
            /* Enable trailing edge capture, with interrupts */
            HAL_TIM_IC_Start_IT(&ENC_TIM, ENC_INDEX_TRAILING_EDGE);
            ret = true;
        }
        else
        {
            /* Failed start of the timer */
            HAL_TIM_UnRegisterCallback(&ENC_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
            ret = false;
        }
    }
    /* Errors detected */    
       
    _enc_internals.started = ret;
    
    return ret;
}

extern bool isstarted()
{
    return _enc_internals.started;
}

extern int32_t getvalue()
{
    if(false == _enc_internals.started)
    {
        return _enc_internals.forcedvalue;
    }
    
    int32_t v = __HAL_TIM_GetCounter(&ENC_TIM);
    
    return _enc_internals.conversion.convert(v);
}

void force(int32_t value)
{
    _enc_internals.forcedvalue = value;
}


    
} // namespace embot::hw::motor::enc {



namespace embot::hw::motor::hall {
    
#define HALL_TIM (embot::hw::motor::bsp::amc2c::hTIM4)
    
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
    
    // it is the lookup table from hall value H3H2H1 = [1, 2, 3, 4, 5, 6] to sector.
    // the sector number x is centered in angle x*60 [deg] and is 60 deg wide
    static constexpr uint8_t hallSectorTable[] = {255, 4, 2, 3, 0, 5, 1, 255};
    
    static constexpr uint16_t status2angle(uint8_t hallstatus)
    {
        return hallAngleTable[hallstatus & 0x07];
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
    
    // in icub degrees
    int32_t angle {0};

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
        
        if((curr == prev) || (curr == nextclockwise[prev]) || (curr == nextcounterclockwise[prev]))
        {   // it is ok if the transition happens by a single step clockwise or counterclockise
            return true;
        }
        
        return false;
    }
    
    bool firstacquisition {true};       
    
    void reset()
    {
        angle = 0;
        h3h2h1[hall_Data::CURR] = h3h2h1[hall_Data::PREV] = 0;
        firstacquisition = true;
    }
    

    
    hall_Data() = default;
};


struct hall_Internals
{
    bool started {false};
    Configuration config {};
    Mode mode {};    
    hall_Data data {};  
      

    void reset()
    {
        started = false;
        config.acquisition = Configuration::ACQUISITION::deferred;
        mode.swap = Mode::SWAP::none;
        mode.offset = 0;
        data.reset();
    }        
        
    hall_Internals() = default;   
    
    
    uint8_t input()
    {
        // we get the values of the three bits: H3|H2|H1. they correspond to C|B|A
        uint8_t x = hall_INPUT(); 
        
        uint8_t v = x & 0b111; // already masked by hall_INPUT() but better be sure
        
        if(embot::hw::motor::hall::Mode::SWAP::BC == mode.swap)
        {
            // in case swapBC is true, then we swap second (H2) w/ third (H3), so we have v = H2H3H1 = BCA
        
            v = ((x & 0b001)     )  |   // gets 0|0|H1 and keeps it where it is
                ((x & 0b010) << 1)  |   // gets 0|H2|0 and moves it up by one position
                ((x & 0b100) >> 1)  ;   // gets H3|0|0 and moves it down by one position
        } 

        return v;        
    }
    
    void acquire()
    {
        // actually ... we
        // get the input and validate it vs possible states and possible transitions       
        
        if(true == validate(input()))
        {
            // i assign angle
            data.angle = mode.offset + hall_Table::status2angle(data.h3h2h1[hall_Data::CURR]);  
            
            // i may do....
            
            switch(config.encodertuning)
            {
                case Configuration::ENCODERtuning::forcevalue:
                {   
                    embot::hw::motor::enc::force(data.angle);
                } break;

                case Configuration::ENCODERtuning::adjust:
                {   
                    encoderadjust();
                } break;
                 
                case Configuration::ENCODERtuning::none:
                default:
                {   
                    // do nothing
                } break;               
            }

        }      
    }
    
        
    bool validate(uint8_t v)
    {
        bool r {true};
               
        // 1. validate h3h2h1
                
        if(false == hall_Data::isH3H2H1valid(v))
        {
            // error
//            data.h3h2h1[hall_Data::CURR] = data.h3h2h1[hall_Data::PREV];

            // RAISE ERROR STATE
            motorhal_set_fault(DHESInvalidValue);
            return false;
        }
        
        // 2. update h3h2h1 status
        
        // first time of call after a reset()
        if(true == data.firstacquisition)
        {
            data.firstacquisition = false;
            // i use current value v for both
            data.h3h2h1[hall_Data::PREV] = data.h3h2h1[hall_Data::CURR] = v;
        }  
        else   
        {                    
            // i store previous h3h2h1 and i assign the new h3h2h1
            data.h3h2h1[hall_Data::PREV] = data.h3h2h1[hall_Data::CURR];
            data.h3h2h1[hall_Data::CURR] = v;
        }
        
        // 3. validate transition from previous h3h2h1 to the current one.
        //    we can directly use a transition table or to verify that the sectors that corresponds to the two h3h2h1 values
        //    are the same or spaced by one
        
        if(data.h3h2h1[hall_Data::PREV] != data.h3h2h1[hall_Data::CURR])
        {
            if(false == hall_Data::isTransitionOfH3H2H1valid(data.h3h2h1[hall_Data::PREV], data.h3h2h1[hall_Data::CURR]))
            {
                motorhal_set_fault(DHESInvalidSequence);
                return false; 
            }                
        }
        
        return true;                
    }
    
    void encoderadjust()
    {
        // in here we do what is done for amcbldc in function s_pwm_updateHallStatus() for the case when MainConf.encoder.resolution is not zero.
        // that means we have an encoder and we want the hall to help its acquisition.
        
#if 0
        if (!s_pwm_hallStatus_old)
        {            
            hallCounter = 0;
            encoder_Reset();
            encoder_Force(angle);
        }
        else
        {
            // Check if the motor is rotating forward (counterclockwise)
            bool forward = ((sector-sector_old+numHallSectors)%numHallSectors)==1;
        
            if (forward) // forward
            {
                ++hallCounter;
                angle -= minHallAngleDelta; // -30 deg
            }
            else
            {
                --hallCounter;
                angle += minHallAngleDelta; // +30 deg
            }
             /*
            0) Use the Hall sensors to rotate until the wrap-around border is reached,
             then reset the encoder value
            */
            if (calibration_step == 0)
            {
                encoder_Force(angle);
                
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
                encoder_Force(angle);
            
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
                encoder_Force(angle);
                // reset the angle after full rotation
                if ((sector == 0 && sector_old == 5) || (sector == 5 && sector_old == 0))
                {
                    encoder_Reset();
                }
            }

#endif        
        
    }
        
    
    static void capture(hall_Internals &hint)
    {
        hint.acquire();        
    }    
};


hall_Internals _hall_internals {};


static void hall_OnCapture(TIM_HandleTypeDef *htim)
{
    hall_Internals::capture(_hall_internals);
}


bool deinit()
{
    _hall_internals.reset();
       
    HallDeInit();
    
    return true;     
}

bool init(const Configuration &config)
{ 
    HallDeInit();
    
    _hall_internals.reset();
    
    _hall_internals.config = config;
    
    // _hall_internals.config.acquisition is always ACQUISITION::deferred
    
    return true;     
}

bool start(const Mode &mode)
{ 
    bool ret {true};  

    _hall_internals.mode = mode;        
        
    hall_Internals::capture(_hall_internals);

    // marco.accame: i stop things
    
    /* Stop counter */
    HAL_TIM_Base_Stop(&HALL_TIM);
    /* Stop interrupt */
    HAL_TIM_IC_Stop(&HALL_TIM, TIM_CHANNEL_1);
    
    // marco.accame: i register the callback and start counter.
    /* Register the callback function for CH1 capture event */
    if (HAL_OK == HAL_TIM_RegisterCallback(&HALL_TIM, HAL_TIM_IC_CAPTURE_CB_ID, hall_OnCapture))
    {
        /* Start counter */
        if (HAL_OK == HAL_TIM_Base_Start(&HALL_TIM))
        {
// marco.accame: i DONT do this call HallSetPwm() 
//            /* Zero amplitude field */
//            HallSetPwm(0);
            /* Start channel 1 in interrupt mode */
            if (HAL_OK == HAL_TIM_IC_Start_IT(&HALL_TIM, TIM_CHANNEL_1))
            {
                ret = true;
            }
            else
            {
                ret = false;
                /* Stop counter */
                HAL_TIM_Base_Stop(&HALL_TIM);
                /* Disable all */
//                PwmPhaseEnable(PWM_PHASE_NONE);
            }
        }
        else
        {
            ret = false;
            /* Remove the callback function */
            HAL_TIM_UnRegisterCallback(&HALL_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
        }
    }
    
    _hall_internals.started = ret;
    
    // i read it again
    hall_Internals::capture(_hall_internals);
    
    
    return ret;
}

bool isstarted()
{
    return _hall_internals.started;
}

uint8_t getstatus()
{
    return _hall_internals.data.h3h2h1[hall_Data::CURR];
}   

int32_t getangle()
{
    return _hall_internals.data.angle;
} 

} // namespace embot::hw::motor::hall {


namespace embot::hw::motor::pwm {
    
#define htim1 (embot::hw::motor::bsp::amc2c::hTIM1)


struct pwm_Internals
{
    volatile bool started {false};
    volatile bool calibrating {false};
    Configuration config {};
        
    void pwmcomparevalue_protect(bool on)
    {
        // if pwm timer is not started ... we dont protect
        if(false == started)
        {
            return;
        }
        
        if(on == true)
        {
            __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_UPDATE);
        }
        else
        {
            __HAL_TIM_ENABLE_IT(&htim1, TIM_IT_UPDATE);      
        }
    }    
        
    pwm_Internals() = default;   
};

pwm_Internals _pwm_internals {};

extern void init(const Configuration &config)
{
    _pwm_internals.config =  config;
    _pwm_internals.calibrating = false;

    PwmInit();
    
    _pwm_internals.started = true;
    
    // meglio essere sicuri ... vedi dentro PwmInit()
    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_BREAK);
}

extern void deinit()
{
    _pwm_internals.started = false;
    PwmDeInit();   
}


extern void set(uint16_t u, uint16_t v, uint16_t w)
{
    if(true == _pwm_internals.calibrating)
    {        
        u = v = w = 0;
    }

//    _pwm_internals.pwmcomparevalue_protect(true);    
    PwmPhaseSet(u, v, w);
//    _pwm_internals.pwmcomparevalue_protect(false);
    
//    if(true == _pwm_internals.calibrating)
//    {        
//        u = v = w = 0;
//    }
//    else
//    {   
//        /* Reduce PWM values in counter units */
//        u = pwm_COMPARE_VALUE(u);
//        v = pwm_COMPARE_VALUE(v);
//        w = pwm_COMPARE_VALUE(w);
//    }
//    
//    /* Must be interrupt-safe */
//    _pwm_internals.pwmcomparevalue_protect(true);  
//    PwmComparePhaseU = u;
//    PwmComparePhaseV = v;
//    PwmComparePhaseW = w;
//    _pwm_internals.pwmcomparevalue_protect(false);   
}

uint16_t map2integer(float x)
{
    static constexpr float maxpwmdiv100 = static_cast<float>(embot::hw::motor::bsp::amc2c::PWMvals.value100perc) / 100.0;
    uint16_t r = 0;
    if((x > 0) && (x < 100.0))
    {
        float t = x*maxpwmdiv100;
        r = static_cast<uint16_t>(t);
    }
    else if(x >= 100.0)
    {
        r = embot::hw::motor::bsp::amc2c::PWMvals.value100perc;
    }
    
    return r;    
}

extern void pwmset(float u, float v, float w)
{ 
    PwmPhaseSet(map2integer(u), map2integer(v), map2integer(w));
}

extern void setperc(float u, float v, float w)
{
    if(true == _pwm_internals.calibrating)
    {        
        u = v = w = 0;
    }
   
    pwmset(u, v, w); 
}
    
extern void enable(bool on)
{
    PwmPhaseEnable((true == on) ? PWM_PHASE_ALL : PWM_PHASE_NONE);
}

} // namespace embot::hw::motor::pwm {


namespace embot::hw::analog {
    
#define hadc3 (embot::hw::motor::bsp::amc2c::hADC3)
#define htim6 (embot::hw::motor::bsp::amc2c::hTIM6)
    
constexpr uint8_t NUMBER_OF_ADC3_CHANNELS = {6}; // for ADC3

/* Hardware related constants */
constexpr uint32_t VREFINT_CAL = {24243};    /* (*VREFINT_CAL_ADDR) is the 16 bits calibration value stored by the manufacturer at location 0x1FF1E860 */
constexpr double VREF_NOM      = {3300.0};   /* mV  */
constexpr double VREFINT_NOM   = {1212.0};   /* mV  */
constexpr double R110          = {560000.0}; /* Ohm */
constexpr double R115          = {33000.0};  /* Ohm */
constexpr uint32_t VIN_GAIN    = static_cast<uint32_t>((R110+R115)/R115 + 0.5);

typedef struct
{
    int32_t avg;
    uint32_t idx;
    std::array<int32_t, 128> buf;
} AdcMovingAverageFilter_t;

enum AdcSignals 
{
    Cin,        /* ADC3_INP0 */
    Vin,        /* ADC3_INP1 */
    Vaux,       /* ADC3_INP2 */
    Vcore,      /* ADC3_INP3 */
    Vtemp,      /* Temperature sensor */
    Vrefint     /* Internal voltage reference 1.212V */
};

/* VREF constant: VREFINT_CAL * VREFINT_CAL_VREF */
static uint32_t Kvrefcal = static_cast<uint32_t>(VREFINT_NOM * VREF_NOM + 0.5);

/* Measured VREF in mV. Equal to VCC */
static volatile uint16_t Vcc_mV = VREF_NOM;
static volatile uint16_t Vin_mV;
static AdcMovingAverageFilter_t VrefFilter;
//static AdcMovingAverageFilter_t CinFilter;


/* DMA buffer for ADC3 raw data */
//static uint16_t AdcPwrRawData[16]; // TODO: re-check (compare) this parameter w/ the one used by GZini (it is different)
alignas(32) static std::array<uint16_t, 16> AdcPwrRawData;

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
int32_t AdcMovingAverageFilter(AdcMovingAverageFilter_t *pFilter, int32_t sample)
{
    pFilter->avg -= pFilter->buf[pFilter->idx];
    pFilter->avg += (pFilter->buf[pFilter->idx] = sample);
    if (++(pFilter->idx) >= pFilter->buf.size()) pFilter->idx = 0;
    return (pFilter->avg + (pFilter->buf.size() >> 1)) / pFilter->buf.size();
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void Adc3DmaComplete_callback(volatile uint16_t sample[])
{
    uint16_t Vref;
    /* Calculate average values of Vrefint */
    Vref = static_cast<uint16_t>(AdcMovingAverageFilter(&VrefFilter, static_cast<int32_t>(sample[Vrefint])));
    /* Update current analog values */
    Vcc_mV   = (0 < Vref)? Kvrefcal / Vref : 3300u;
//    Vcore_mV = (uint32_t)sample[Vcore] * VCORE_GAIN * (uint32_t)Vcc_mV / 65536u;
//    Vaux_mV  = (uint32_t)sample[Vaux]  * VAUX_GAIN  * (uint32_t)Vcc_mV / 65536u;
    Vin_mV   = static_cast<uint32_t>(sample[Vin]) * VIN_GAIN * static_cast<uint32_t>(Vcc_mV) / 65536u;
//    Cin_mA   = (int16_t)AdcMovingAverageFilter(&CinFilter,
//               (int32_t)(int16_t)sample[Cin] * CIN_GAINx4 * (int32_t)(int16_t)Vcc_mV / (65536*4));
}

/* Callback functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcPwrHalfTransfer_callback(ADC_HandleTypeDef *hadc)
{
    /* Force the readback of the DMA buffer */
    //SCB_InvalidateDCache_by_Addr((void *)AdcPwrRawData, sizeof(AdcPwrRawData));
    /* First half of the buffer ready */
    Adc3DmaComplete_callback(&(AdcPwrRawData[0]));
}


void AdcPwrTransferComplete_callback(ADC_HandleTypeDef *hadc)
{
    /* Force the readback of the DMA buffer */
    //SCB_InvalidateDCache_by_Addr((void *)AdcPwrRawData, sizeof(AdcPwrRawData));
    /* Second half of the buffer ready */
    Adc3DmaComplete_callback(&(AdcPwrRawData[NUMBER_OF_ADC3_CHANNELS]));
}

    
extern void init(const Configuration &config)
{
    if (HAL_OK == HAL_TIM_Base_Start(&htim6))
    {
        /* Calibrate ADC3 */
        if (HAL_OK == HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED))
        {
            /* Register DMA callback functions */
            if ((HAL_OK == HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_HALF_CB_ID, AdcPwrHalfTransfer_callback)) &&
                (HAL_OK == HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AdcPwrTransferComplete_callback)))
            {
                
                /* Calculate the proportionality constant */
                Kvrefcal = static_cast<uint32_t>(VREFINT_CAL_VREF) * VREFINT_CAL;
                
                /* Init the Vref filter to the nominal value */
                for (uint8_t i = 0; i < VrefFilter.buf.size(); i++)
                {
                    VrefFilter.buf[i] = 3300;
                }
                VrefFilter.avg = VrefFilter.buf.size() * 3300;
                VrefFilter.idx = 0;
                
                /* Clear the Cin filter */
//                memset((void *)&CinFilter, 0, sizeof(CinFilter));
                std::fill(std::begin(AdcPwrRawData), std::end(AdcPwrRawData), 0);
//                SCB_CleanInvalidateDCache_by_Addr((void *)AdcDmaBuffer, sizeof(AdcDmaBuffer));
                
                /* Start the converter in DMA mode */
                HAL_ADC_Start_DMA(&hadc3, (uint32_t *)AdcPwrRawData.data(), 2*NUMBER_OF_ADC3_CHANNELS);
            }
        }
    }
}

extern void deinit()
{
    HAL_ADCEx_MultiModeStop_DMA(&hadc3);
    HAL_ADC_UnRegisterCallback(&hadc3, HAL_ADC_CONVERSION_COMPLETE_CB_ID);
    HAL_ADC_UnRegisterCallback(&hadc3, HAL_ADC_CONVERSION_HALF_CB_ID);
}

float getVin()
{
    // return Vin in Volts
    return Vin_mV * 0.001f;
}
    
} // namespace embot::hw::analog
 


// old code whihc may become useful later on


#if 0

#error it is old code. i keep it because we have a draft correction of encoder using the hall w/ s_hall_updateHallStatus()
    
extern bool hall_Init()
{
    bool ret {false);
        
    s_hall_Settings.swapBC = false;
    s_hall_Settings.offset = 0;
    s_hall_Settings.order[0] = 0;
    s_hall_Settings.order[1] = 1;
    s_hall_Settings.order[2] = 2;
    s_hall_Settings.counter = 0;
    s_hall_Settings.angle = 0;

  
// on amcbldc it is just like taht    
//    /* Start position counter */
//    hallCounter = 0;
// 
//    /* Check TIM3 configuration and start it */
//    return ((TIM3 == htim3.Instance) &&
//            (HAL_OK == HAL_TIM_RegisterCallback(&htim3, HAL_TIM_IC_CAPTURE_CB_ID, hallStatusChange_cb)) &&
//            (HAL_OK == HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1)) &&
//            (HAL_OK == HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2)) &&
//            (HAL_OK == HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_3)) &&
//            (HAL_OK == HAL_TIM_Base_Start_IT(&htim3)))? HAL_OK : HAL_ERROR;
//  

// on amc2c we just ...

#warning attenzione: HallInit() potrebbe fare cose che non vogliamo tipo mettere il pwm a zero usando gli hall che magari non abbiamo
// quindi riscrivo le cose

    // marco.accame: i get code from HallInit() and i adapt it

    // marco.accame: i stop things
    
    /* Stop counter */
    HAL_TIM_Base_Stop(&HALL_TIM);
    /* Stop interrupt */
    HAL_TIM_IC_Stop(&HALL_TIM, TIM_CHANNEL_1);
    
    // marco.accame: i register the callback and start counter.
    /* Register the callback function for CH1 capture event */
    if (HAL_OK == HAL_TIM_RegisterCallback(&HALL_TIM, HAL_TIM_IC_CAPTURE_CB_ID, hall_OnCapture))
    {
        /* Start counter */
        if (HAL_OK == HAL_TIM_Base_Start(&HALL_TIM))
        {
// marco.accame: i DONT do this call HallSetPwm() 
//            /* Zero amplitude field */
//            HallSetPwm(0);
            /* Start channel 1 in interrupt mode */
            if (HAL_OK == HAL_TIM_IC_Start_IT(&HALL_TIM, TIM_CHANNEL_1))
            {
                ret = true;
            }
            else
            {
                ret = false;
                /* Stop counter */
                HAL_TIM_Base_Stop(&HALL_TIM);
                /* Disable all */
//                PwmPhaseEnable(PWM_PHASE_NONE);
            }
        }
        else
        {
            ret = false;
            /* Remove the callback function */
            HAL_TIM_UnRegisterCallback(&HALL_TIM, HAL_TIM_IC_CAPTURE_CB_ID);
        }
    }
    
    return ret;
}


extern bool hall_DeInit()
{
    s_hall_Settings.swapBC = false;
    s_hall_Settings.offset = 0;
    s_hall_Settings.order[0] = 0;
    s_hall_Settings.order[1] = 1;
    s_hall_Settings.order[2] = 2;
    s_hall_Settings.counter = 0;
    s_hall_Settings.angle = 0;
    
    HallDeInit();
    
    return true;    
}


extern bool hall_Config(bool swapBC, uint16_t pwm_hall_offset)
{
    #warning TODO-embot::hw::motor: hall_Config()
    
    s_hall_Settings.swapBC = swapBC;
    s_hall_Settings.offset = pwm_hall_offset;
    
    s_hall_Settings.order[0] = 0;
    s_hall_Settings.order[1] = (true == swapBC) ? 2 : 1;
    s_hall_Settings.order[2] = (true == swapBC) ? 1 : 2;
    
    s_hall_Settings.counter = 0;
    s_hall_Settings.angle = 0;
    
    // now we need to force an update of the hall status.
    // this update is surely done inside the callback HallCapture_cb() associated to HAL_TIM_IC_CAPTURE_CB_ID
    // however the callback happens only when the rotor moves and in here we want   
    // to have measures even if it does not.
    // in amcbldc we have function s_pwm_updateHallStatus()
    #warning DOUBT: should we stop the callback of the IRQ Handler in here? what if it triggers when we execute?
        
    s_hall_updateHallStatus();
    
    // ma perche' si fa dopo? ....
    s_hall_Settings.counter = 0;

//    s_pwm_updateHallStatus();
//    
//    /* Start position counter */
//    hallCounter = 0;
    
    return true;
}

uint8_t hall_GetStatus(void)
{
    return HallStatus;
}

static volatile uint8_t s_hall_hallStatus_old = 0;

static void s_hall_updateHallStatus(void)
{
    hall_status = hall_Get();
#if 0    
    s_hall_hallStatus_old = HallStatus;
    static int8_t calibration_step = 0;
    
    static uint16_t border[6]={0};
    static uint8_t border_flag = 0;
    
    /* Read current value of HALL1, HALL2 and HALL3 signals in bits 2..0 */
    HallStatus = hall_Get();
    uint16_t angle = s_hall_Settings.offset + hallAngleTable[HallStatus];
    
    // Check which sector between [0 ... 5] the rotor is in
    int16_t sector = ((minHallAngleDelta + angle) / hallAngleStep) % numHallSectors;
    static int16_t sector_old = sector;
    
    s_hall_Settings.angle = angle;
    
    if(0 == enc_GetResolution()) // no encoder
    {
        enc_ForceAngle(angle);
    }
    else
    {
        if (!s_hall_hallStatus_old) // madonna, ma che significa??? 
        {   
            // forse: che HallStatus e' diverso da zero (e' zero allo startup) e che quindi qualcuno ha gia' chiamato la sua lettura 
            // e quindi ... si esce usando l'angolo ottenuto. 
            // attenzione pero': questo codice e' preso dalla amcbldc dove forse ci sono inizializzazioni diverse ...          
            s_hall_Settings.counter = 0;
            enc_Reset();
            enc_ForceAngle(angle);
        }
        else
        {
            // Check if the motor is rotating forward (counterclockwise)
            bool forward = ((sector-sector_old+numHallSectors)%numHallSectors)==1;
        
            if (forward) // forward
            {
                ++s_hall_Settings.counter;
                angle -= minHallAngleDelta; // -30 deg
            }
            else
            {
                --s_hall_Settings.counter;
                angle += minHallAngleDelta; // +30 deg
            }
             /*
            0) Use the Hall sensors to rotate until the wrap-around border is reached,
             then reset the encoder value
            */
            if (calibration_step == 0)
            {
                enc_ForceAngle(angle);
                
                if ((sector == 0 && sector_old == 5) || (sector == 5 && sector_old == 0))
                {
                    enc_Reset();
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
                enc_ForceAngle(angle);
            
                // use the current sector if forward rotation or previous if reverse
                uint8_t sector_index = forward ? sector : (sector+1)%numHallSectors;
            
                // keep track of the encoder value between sectors
                border[sector_index] = enc_GetUncalibrated();

                // found the s-th border, put a 1 in the mask
                border_flag |= 1 << sector_index;
                
                // If all sectors are found apply least squares fitting by computing average of
                // difference between measured values on borders and expected hall angle
                if (border_flag == 63) // 111111
                {
                    calibration_step = 2;
                
                    int32_t offset = int16_t(border[0] - s_hall_Settings.offset + minHallAngleDelta);
                    offset += int16_t(border[1] - s_hall_Settings.offset + minHallAngleDelta - hallAngleStep);
                    offset += int16_t(border[2] - s_hall_Settings.offset + minHallAngleDelta - 2 * hallAngleStep);
                    offset += int16_t(border[3] - s_hall_Settings.offset + minHallAngleDelta - 3 * hallAngleStep);
                    offset += int16_t(border[4] - s_hall_Settings.offset + minHallAngleDelta - 4 * hallAngleStep);
                    offset += int16_t(border[5] - s_hall_Settings.offset + minHallAngleDelta - 5 * hallAngleStep);
                
                    offset /= numHallSectors;
                     
                    embot::core::print("CALIBRATED\n");
                    
                    enc_Calibrate(int16_t(offset));
                }
            }
            /*
            2) Update the forced value even if it is not used when the encoder is calibrated.
            Reset the encoder angle after a full rotation to avoid desynching
            */
            else if (calibration_step == 2)
            {
                enc_ForceAngle(angle);
                // reset the angle after full rotation
                if ((sector == 0 && sector_old == 5) || (sector == 5 && sector_old == 0))
                {
                    enc_Reset();
                }
            }
        }
    }
    // update the old sector and hall status
    sector_old = sector;
    s_hall_hallStatus_old = HallStatus;
    
    //return hallStatus;
#endif    
}


#endif // old code


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

