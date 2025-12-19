/*
 * Copyright (C) 2024 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_motor_pwm.h"
#include "embot_hw_analog.h"

// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "embot_core.h"
#include <array>

// --------------------------------------------------------------------------------------------------------------------
// - configuration of peripherals and chips. it is done board by board. it contains a check vs correct STM32HAL_BOARD_*
// --------------------------------------------------------------------------------------------------------------------

#include "embot_hw_bsp_config.h"

// --------------------------------------------------------------------------------------------------------------------
// - all the rest
// --------------------------------------------------------------------------------------------------------------------


namespace embot::hw::motor::pwm::bsp {
    
    bool PwmInit(void);

//    #define PWM_MOTOR_NONE              (0x00000000UL)
    static constexpr auto PWM_MOTOR_1 = 0x00000001UL;
    static constexpr auto PWM_MOTOR_2 = 0x00000002UL;
//    #define PWM_MOTOR_ALL               (0x00000003UL)
    void PwmSetWidth(uint32_t mot, int32_t ph1, int32_t ph2, int32_t ph3);
    
}


#if !defined(EMBOT_ENABLE_hw_motor_pwm)

#warning EMBOT_ENABLE_hw_motor_pwm is undefined, so we use dummy implementation 

namespace embot::hw::motor::pwm {
    
    void init(embot::hw::MOTOR m, const Configuration &config) { return; }
    void deinit(embot::hw::MOTOR m) { return; }
    void enable(embot::hw::MOTOR m, bool on)  { return; }
    void setperc(embot::hw::MOTOR m, float u, float v, float w)  { return; }
    
}
#elif defined(EMBOT_ENABLE_hw_motor_pwm)

#if defined(STM32HAL_BOARD_AMCFOC_1CM7) || defined(STM32HAL_BOARD_AMCFOC_2CM4)
#include "embot_hw_motor_bldc_bsp_amcfoc.h"
#endif



namespace embot::hw::motor::pwm {


struct pwm_Internals
{
    
    struct Item
    {
        bool started {false};
        bool calibrating {false};
        Configuration config {};   
            
        Item() = default;
    };
    
    std::array<Item, embot::hw::motor::bldc::MAXnumber> _items {};
    
//    void pwmcomparevalue_protect(embot::hw::MOTOR m, bool on)
//    {
//        // if pwm timer is not started ... we dont protect
//        if(false == started)
//        {
//            return;
//        }
//        
//        if(on == true)
//        {
//            __HAL_TIM_DISABLE_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_IT_UPDATE);
//        }
//        else
//        {
//            __HAL_TIM_ENABLE_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_IT_UPDATE);      
//        }
//    }    
        
    pwm_Internals() = default;   
};

pwm_Internals _pwm_internals {};

void init(embot::hw::MOTOR m, const Configuration &config)
{
    _pwm_internals._items[embot::core::tointegral(m)].config =  config;
    _pwm_internals._items[embot::core::tointegral(m)].calibrating = false;

    static bool initted {false};
    if(false == initted)
    {
        bsp::PwmInit();
        initted = true;
    }
    
    _pwm_internals._items[embot::core::tointegral(m)].started = true;
    
    setperc(m, 0, 0, 0);
    
    // meglio essere sicuri ... vedi dentro PwmInit()
//    __HAL_TIM_DISABLE_IT(&htim1, TIM_IT_BREAK);
}

void deinit(embot::hw::MOTOR m)
{
    _pwm_internals._items[embot::core::tointegral(m)].started = false;
//    bsp::PwmDeInit();   
}


void set(embot::hw::MOTOR m, uint16_t u, uint16_t v, uint16_t w)
{
    if(true == _pwm_internals._items[embot::core::tointegral(m)].calibrating)
    {        
        u = v = w = 0;
    }

//    _pwm_internals.pwmcomparevalue_protect(true);    
//    PwmPhaseSet(u, v, w);
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
#if defined(STM32HAL_BOARD_AMCFOC_1CM7) || defined(STM32HAL_BOARD_AMCFOC_2CM4)
    static constexpr float maxpwmdiv100 = static_cast<float>(embot::hw::motor::bldc::bsp::amcfoc::PWMvals.valueofTIMperiod()) / 100.0;
#endif  
    
    uint16_t r = 0;
    if((x > 0) && (x < 100.0))
    {
        float t = x*maxpwmdiv100;
        r = static_cast<uint16_t>(t);
    }
    else if(x >= 100.0)
    {      
#if defined(STM32HAL_BOARD_AMCFOC_1CM7) || defined(STM32HAL_BOARD_AMCFOC_2CM4)
        r = embot::hw::motor::bldc::bsp::amcfoc::PWMvals.valueofTIMperiod();
#endif             
    }
    
    return r;    
}

void pwmset(embot::hw::MOTOR m, float u, float v, float w)
{
    uint32_t mm = (embot::hw::MOTOR::one == m) ? bsp::PWM_MOTOR_1 : bsp::PWM_MOTOR_2;    
    bsp::PwmSetWidth(mm, map2integer(u), map2integer(v), map2integer(w));
}

void setperc(embot::hw::MOTOR m, float u, float v, float w)
{
    if(true == _pwm_internals._items[embot::core::tointegral(m)].calibrating)
    {        
        u = v = w = 0;
    }
   
    pwmset(m, u, v, w); 
}
    
extern void enable(embot::hw::MOTOR m, bool on)
{
    #warning how do we enable / disable the motor?
//    PwmPhaseEnable((true == on) ? PWM_PHASE_ALL : PWM_PHASE_NONE);
}

} // namespace embot::hw::motor::pwm {


namespace embot::hw::motor::pwm::bsp {
       
    static constexpr auto PWM_FAULT_EMERGENCY_BUTTON  = 0x00002000UL;
    static constexpr auto PWM_FAULT_SYSTEM            = 0x00008000UL;
    static uint32_t Pwm1Status = 0;
    static uint32_t Pwm2Status = 0;

    /* Interrupt functions ************************************************************************************************/

    /*******************************************************************************************************************//**
     * @brief Call back function. Called by the TIM interrupt manager following the BRAEAK input activation
     * @param   *htim   pointer to the TIM peripheral handler
     * @return  void
     */
    static void Pwm1_break_cb(TIM_HandleTypeDef *htim)
    {
        if (0 != (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_SYSTEM_BREAK)||(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_BREAK ))))
            Pwm1Status |= PWM_FAULT_SYSTEM; 
    }    

    /*******************************************************************************************************************//**
     * @brief Call back function. Called by the TIM interrupt manager following the BREAK2 input activation
     * @param   *htim   pointer to the TIM peripheral handler
     * @return  void
     */
    static void Pwm1_break2_cb(TIM_HandleTypeDef *htim)
    {
        if (0 != __HAL_TIM_GET_FLAG(htim, TIM_FLAG_BREAK2)) Pwm1Status |= PWM_FAULT_EMERGENCY_BUTTON; 
    }


    /*******************************************************************************************************************//**
     * @brief Call back function. Called by the TIM interrupt manager following the BRAEAK input activation
     * @param   *htim   pointer to the TIM peripheral handler
     * @return  void
     */
    static void Pwm2_break_cb(TIM_HandleTypeDef *htim)
    {
        if (0 != (__HAL_TIM_GET_FLAG(htim, TIM_FLAG_SYSTEM_BREAK)||(__HAL_TIM_GET_FLAG(htim, TIM_FLAG_BREAK ))))
            Pwm2Status |= PWM_FAULT_SYSTEM; 
    }    

    /*******************************************************************************************************************//**
     * @brief Call back function. Called by the TIM interrupt manager following the BRAEAK2 input activation
     * @param   *htim   pointer to the TIM peripheral handler
     * @return  void
     */
    static void Pwm2_break2_cb(TIM_HandleTypeDef *htim)
    {
        if (0 != __HAL_TIM_GET_FLAG(htim, TIM_FLAG_BREAK2)) Pwm2Status |= PWM_FAULT_EMERGENCY_BUTTON; 
    }     
    
//    volatile uint32_t updatemot[2] = {0, 0};
//    
//    static void MOT1pwmUpdateEvent_cb(TIM_HandleTypeDef *phtim)
//    {
//        updatemot[0]++;
//    }

//    static void MOT2pwmUpdateEvent_cb(TIM_HandleTypeDef *phtim)
//    {
//        updatemot[1]++;
//    }
    
    bool PwmInit(void)
    {
        /* Stop Motor 1 PWM (if not already stopped) */
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_1);
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_2);
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_2);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_4); // SPEEDGOAT
        HAL_TIM_PWM_Stop(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_6);
        
        /* Stop Motor 2 PWM (if not already stopped) */
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_1);
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_2);
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_2);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_4); // SPEEDGOAT
        HAL_TIM_PWM_Stop(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_6);

        /* Stop counters */
        HAL_TIM_Base_Stop(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1);
        HAL_TIM_Base_Stop(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2);
        
        /* 0% duty cycle */
        PwmSetWidth(PWM_MOTOR_1|PWM_MOTOR_2, 0, 0, 0);
        
        /* CAUTION
         * =======
         * The following statement stops the PWM generators and forces their outputs LOW (lower transistor ON) whenever the
         * debugger triggers any breakpoint on CM4. Remove the statement if you prefer to leave the PWM outputs always
         * active during debugging. Both solutions have drawbacks that must be carefully evaluated by the programmer
         */
        DBGMCU->APB2FZ2 |= 0x00000003;
        
        // marco.accame: this section starts the two MOT timers out of synch by acting on the counter and on the counting direction
//#define SHIFT_NOACTION
#define SHIFT_comp
#if defined(SHIFT_NOACTION)

#else

        // cosi' ho le fasi dei due motori opposte. e gli update accadono allo stesso tempo ma per il mot 1 si ha undeflow e per il mot 2 overflow e vice versa
        /* Configure motor 1 PWM */
        /* Start PWM counter from 512 in up direction. First sample when PWM pulse is LOW */
        uint32_t offsetMOT1 = 512;
        #if defined(SHIFT_comp)
        offsetMOT1 = embot::hw::motor::bldc::bsp::amcfoc::PWMvals.valueofTIMperiod()/2;
        #endif     
        //Set PWM/timer counter and upcounting direction        
        __HAL_TIM_SetCounter(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, offsetMOT1);
        embot::hw::motor::bldc::bsp::amcfoc::htimMOT1.Instance->CR1 = (embot::hw::motor::bldc::bsp::amcfoc::htimMOT1.Instance->CR1 & ~TIM_CR1_CMS);
        embot::hw::motor::bldc::bsp::amcfoc::htimMOT1.Instance->CR1 = (embot::hw::motor::bldc::bsp::amcfoc::htimMOT1.Instance->CR1 & ~TIM_CR1_DIR) | TIM_CR1_CMS_0;
        /* Configure motor 2 PWM */
        /* Start PWM counter from 512 in down direction. First sample when PWM pulse is HIGH */
        uint32_t offsetMOT2 = 512;
        #if defined(SHIFT_comp)
        offsetMOT2 = embot::hw::motor::bldc::bsp::amcfoc::PWMvals.valueofTIMperiod()/2;
        #endif
        //Set PWM/timer counter and downward counting direction
        __HAL_TIM_SetCounter(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, offsetMOT2);
        embot::hw::motor::bldc::bsp::amcfoc::htimMOT2.Instance->CR1 = (embot::hw::motor::bldc::bsp::amcfoc::htimMOT2.Instance->CR1 & ~TIM_CR1_CMS);
        embot::hw::motor::bldc::bsp::amcfoc::htimMOT2.Instance->CR1 = (embot::hw::motor::bldc::bsp::amcfoc::htimMOT2.Instance->CR1 | TIM_CR1_DIR ) | TIM_CR1_CMS_0;

// cosi' ho gli update equispaziati
//        uint32_t offsetMOTslave = embot::hw::motor::bldc::bsp::amcfoc::PWMvals.valueofTIMperiod()/2;
//        __HAL_TIM_SetCounter(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, offsetMOTslave);
//        __HAL_TIM_SetCounter(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, 0);
#endif        
        /* Clear the status registers */
        Pwm1Status = 0;
        Pwm2Status = 0;
        
        /* Register the callback functions */
        HAL_TIM_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, HAL_TIM_BREAK_CB_ID,  Pwm1_break_cb);
        HAL_TIM_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, HAL_TIM_BREAK2_CB_ID, Pwm1_break2_cb);
        HAL_TIM_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, HAL_TIM_BREAK_CB_ID,  Pwm2_break_cb);
        HAL_TIM_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, HAL_TIM_BREAK2_CB_ID, Pwm2_break2_cb);
        
//        HAL_TIM_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, HAL_TIM_PERIOD_ELAPSED_CB_ID, MOT1pwmUpdateEvent_cb);
//        HAL_TIM_RegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, HAL_TIM_PERIOD_ELAPSED_CB_ID, MOT2pwmUpdateEvent_cb);

        /* Start Motor 1 (TIM8 slave timer) */
        HAL_TIMEx_PWMN_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_1);
        HAL_TIMEx_PWMN_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_2);
        HAL_TIMEx_PWMN_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_1);
        HAL_TIM_PWM_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_2);
        HAL_TIM_PWM_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_4);
        HAL_TIM_PWM_Start(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_5);
        HAL_TIM_PWM_Start(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_6);
        
        /* Start Motor 2 (TIM1 master timer) */
        HAL_TIMEx_PWMN_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_1);
        HAL_TIMEx_PWMN_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_2);
        HAL_TIMEx_PWMN_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_3);
        HAL_TIM_PWM_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_1);
        HAL_TIM_PWM_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_2);
        HAL_TIM_PWM_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_3);
        HAL_TIM_PWM_Start_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_4);
        HAL_TIM_PWM_Start(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_5);
        HAL_TIM_PWM_Start(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_6);
        
        /* Start counters */
        HAL_TIM_Base_Start(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1);
        HAL_TIM_Base_Start(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2);
        
//        __HAL_TIM_ENABLE_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_IT_UPDATE);
//        __HAL_TIM_ENABLE_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_IT_UPDATE);
        
        return true;
    }
    
        
    void PwmSetWidth(uint32_t mot, int32_t ph1, int32_t ph2, int32_t ph3)
    {
        constexpr uint32_t MAX_PWM {embot::hw::motor::bldc::bsp::amcfoc::PWMvals.valueofTIMperiod()};
        constexpr uint32_t MAX_PWM_90PERC {(80*embot::hw::motor::bldc::bsp::amcfoc::PWMvals.valueofTIMperiod())/100};
        constexpr uint32_t MAX_PWM_10PERC {(10*embot::hw::motor::bldc::bsp::amcfoc::PWMvals.valueofTIMperiod())/100};
//        int32_t min;
//        uint32_t sat = 0;
        /* Select the minumum value */
//        min = (ph1 <= ph2)? ((ph1 <= ph3)? ph1 : ph3) : ((ph2 <= ph3)? ph2 : ph3);
//        /* Shift to minimum and saturate to maximum */
//        ph1 -= min; if (ph1 > MAX_PWM) {ph1 = MAX_PWM; sat |= PWM_PHASE1;}
//        ph2 -= min; if (ph2 > MAX_PWM) {ph2 = MAX_PWM; sat |= PWM_PHASE2;}
//        ph3 -= min; if (ph3 > MAX_PWM) {ph3 = MAX_PWM; sat |= PWM_PHASE3;}
        /* Set motor #1 */
        if (PWM_MOTOR_1 & mot)
        {
            /* Must be atomic */
 //           taskDISABLE_INTERRUPTS();
            __HAL_TIM_SetCompare(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_1, ph1);
            __HAL_TIM_SetCompare(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_2, ph2);
            __HAL_TIM_SetCompare(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_3, ph3);
            __HAL_TIM_SetCompare(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_4, MAX_PWM_10PERC);  // SYNC FOR SPEEDGOAT
//            taskENABLE_INTERRUPTS();
        }
        /* Set motor #2 */
        if (PWM_MOTOR_2 & mot)
        {
            /* Must be atomic */
//            taskDISABLE_INTERRUPTS();
            __HAL_TIM_SetCompare(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_1, ph1);
            __HAL_TIM_SetCompare(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_2, ph2);
            __HAL_TIM_SetCompare(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_3, ph3);
            __HAL_TIM_SetCompare(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_4, MAX_PWM_10PERC);  // SYNC FOR SPEEDGOAT
//            taskENABLE_INTERRUPTS();
        }
        /* Saturation status */
        //return sat;
    }


    void PwmDeInit(void)
    {
        /* Disable main outputs */
        __HAL_TIM_MOE_DISABLE(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1);
        __HAL_TIM_MOE_DISABLE(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2);
        
        /* 0% duty cycle */
        PwmSetWidth(PWM_MOTOR_1|PWM_MOTOR_2, 0, 0, 0);

        /* Stop Motor 1 PWM */
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_1);
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_2);
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_6);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_2);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, TIM_CHANNEL_4);

        /* Stop Motor 2 PWM */
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_1);
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_2);
        HAL_TIMEx_PWMN_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_3);    

        HAL_TIM_PWM_Stop(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_6);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_1);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_2);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_3);
        HAL_TIM_PWM_Stop_IT(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, TIM_CHANNEL_4);
        
        /* Remove the registered functions */
        HAL_TIM_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, HAL_TIM_BREAK_CB_ID);
        HAL_TIM_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT1, HAL_TIM_BREAK2_CB_ID);
        HAL_TIM_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, HAL_TIM_BREAK_CB_ID);
        HAL_TIM_UnRegisterCallback(&embot::hw::motor::bldc::bsp::amcfoc::htimMOT2, HAL_TIM_BREAK2_CB_ID);
    }    

}



#endif // #elif defined(EMBOT_ENABLE_hw_motor_pwm)


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

