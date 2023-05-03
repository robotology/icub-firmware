
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
    static int32_t raw2current(int32_t r)
    {
        // return CIN_GAIN * vref_mV * r >> 16u;
        return r;       
    }
    
    static int32_t current2raw(int32_t c)
    {
        // int32_t tmp = (curr << 16) / (CIN_GAIN * vref_mV);
        // return tmp;
        return c;       
    }    
    
    constexpr Converter() = default;
};


struct Calibrator
{
    static constexpr uint8_t _shift {10};
    static constexpr size_t _maxcount {1 << _shift};
    
    volatile bool _done {false};
    volatile size_t _count {0};
    Currents _currents {};
    std::array<int64_t, 3> cumulativerawvalues {0, 0, 0};
    
    constexpr Calibrator() = default;
    
    void init()
    {
       _count = 0;
       _done = false; 
       cumulativerawvalues.fill(0); 
       set({oncurrents, this});
    }
    
    void stop()
    {
        set({});
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


    static void oncurrents(void *owner, const Currents * const currents)
    {
        Calibrator *o = reinterpret_cast<Calibrator*>(owner);   
        
        // i use Converter::current2raw() because technically Currents does not contain the raw ADC values but transformed values
        o->cumulativerawvalues[0] += Converter::current2raw(currents->u);
        o->cumulativerawvalues[1] += Converter::current2raw(currents->v);
        o->cumulativerawvalues[2] += Converter::current2raw(currents->w);
        
        o->_count++;
        
        if(o->_count >= Calibrator::_maxcount)
        {
            // time to do business: prepare average currents, impose the offset to ADC, stop the calibrator  
            
            // dont use the >> _shift because ... 1. this operation is done only once, so who bother. 
            //                                    2. cumulativerawvalues is int64_t. it may be (dont think so) negative so it is too complicate to optimize
            // impose offset to adc.
//            volatile int64_t cc[3] {0, 0, 0};
//            cc[0] = o->cumulativerawvalues[0] / _maxcount;
//            cc[1] = o->cumulativerawvalues[1] / _maxcount;
//            cc[2] = o->cumulativerawvalues[2] / _maxcount;
            AdcMotSetOffset(ADCMOT_PHASE_U, AdcMotGetOffset(ADCMOT_PHASE_U) + (o->cumulativerawvalues[0] / _maxcount));
            AdcMotSetOffset(ADCMOT_PHASE_V, AdcMotGetOffset(ADCMOT_PHASE_V) + (o->cumulativerawvalues[1] / _maxcount));
            AdcMotSetOffset(ADCMOT_PHASE_W, AdcMotGetOffset(ADCMOT_PHASE_W) + (o->cumulativerawvalues[2] / _maxcount));
            
            // stop
            o->stop();
            
            return;
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
    
 
void adcm_on_acquisition_of_currents(int16_t c1, int16_t c2, int16_t c3)
{
    // important note: {c1, c2, c3} are the raw ADC acquisitions as in adcMotCurrents[0, 1, 2]
    // the callback requires converted currents. but to be fair in adcm.c there is no concept of how to convert them as in the amcbldc
    // so, boh ... for now i do nothing    
    Currents currents = {Converter::raw2current(c1), Converter::raw2current(c2), Converter::raw2current(c3)};
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
    AdcMotDeInit();
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
        return 0;
    }
    
    int32_t v = __HAL_TIM_GetCounter(&ENC_TIM);
    
    return _enc_internals.conversion.convert(v);
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

    static constexpr uint16_t hallAngleTable[] =
    {
        /* ABC  (°)  */
        /* LLL ERROR */ static_cast<uint16_t>(0),
        /* LLH  240  */ static_cast<uint16_t>(240.0 * Deg2iCub), /* 43690 */
        /* LHL  120  */ static_cast<uint16_t>(120.0 * Deg2iCub), /* 21845 */
        /* LHH  180  */ static_cast<uint16_t>(180.0 * Deg2iCub), /* 32768 */
        /* HLL    0  */ static_cast<uint16_t>(  0.0 * Deg2iCub), /*     0 */
        /* HLH  300  */ static_cast<uint16_t>(300.0 * Deg2iCub), /* 54613 */
        /* HHL   60  */ static_cast<uint16_t>( 60.0 * Deg2iCub), /* 10922 */
        /* HHH ERROR */ static_cast<uint16_t>(0)
    };
    
    static constexpr uint16_t status2angle(uint8_t hallstatus)
    {
        return hallAngleTable[hallstatus & 0x07];
    }
    
    static constexpr uint8_t angle2sector(uint16_t angle)
    {    
        return ((minHallAngleDelta + angle) / hallAngleStep) % numHallSectors;
    }

    constexpr hall_Table() = default;
};   

struct hall_Data
{
    uint8_t order[3] = {0, 1, 2};
    int32_t counter {0};
    uint16_t angle {0};
    volatile uint8_t status {0};
    
    void reset(Mode::SWAP s = Mode::SWAP::none)
    {
        counter = 0;
        angle = 0;
        status = 0;
        set(s);
    }
    
    void set(Mode::SWAP s)
    {
        order[0] = 0; 
        order[1] = (Mode::SWAP::BC == s) ? 2 : 1;
        order[2] = (Mode::SWAP::BC == s) ? 1 : 2;        
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
};


hall_Internals _hall_internals {};


static uint8_t hall_Get()
{
    uint8_t x = hall_INPUT(); // we get the values of the three bits: H3|H2|H1. they are also called C|B|A
    // and now we move them according to order[]. 
    // if we have order = {0, 1, 2} we dont reorder anything, so we have v = x = H3H2H1 = CBA.
    // but in case swapBC is true, then we have  order {0, 2, 1} which swap second w/third, so v = H2H3H1 =  BCA
    
    uint8_t v = (((v>>0)&0x1) >> _hall_internals.data.order[0])      |   // H1 (pos 0) is moved to the final position
                (((v>>1)&0x1) >> _hall_internals.data.order[1])      |   // H2 (pos 1) is moved to the final position
                (((v>>2)&0x1) >> _hall_internals.data.order[2]);         // H3 (pos 2) is moved to the final position
    return v;
}


static void hall_OnCapture(TIM_HandleTypeDef *htim)
{
    _hall_internals.data.status = hall_Get();
    
    // in here we DONT ... perform any calibration of the encoder ....
    // because in the wrist we dont have encoder. we may implement this calibration later on
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
    
    _hall_internals.data.set(_hall_internals.mode.swap);
    
    _hall_internals.data.status = hall_Get();

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
    _hall_internals.data.status = hall_Get();
    
    
    return ret;
}

bool isstarted()
{
    return _hall_internals.started;
}

uint8_t getstatus(void)
{
    return _hall_internals.data.status;
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

    _pwm_internals.pwmcomparevalue_protect(true);    
    PwmPhaseSet(u, v, w);
    _pwm_internals.pwmcomparevalue_protect(false);
    
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
    
extern void enable(bool on)
{
    PwmPhaseEnable((true == on) ? PWM_PHASE_ALL : PWM_PHASE_NONE);
}

} // namespace embot::hw::motor::pwm {


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

