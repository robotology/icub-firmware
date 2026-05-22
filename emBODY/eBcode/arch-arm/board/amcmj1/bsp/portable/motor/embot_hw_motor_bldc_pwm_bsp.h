/*
 * Copyright (C) 2026 MESH - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// - include guard ----------------------------------------------------------------------------------------------------

#ifndef __EMBOT_HW_MOTOR_BLDC_PWM_BPS_H_
#define __EMBOT_HW_MOTOR_BLDC_PWM_BPS_H_

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_motor_bldc_pwm.h"

namespace embot::hw::motor::bldc::pwm::bsp {
    
    // this struct keeps the value of the period to be written in the ARR register
    // and also the offsets for triggeringe external events.
    // gzini used period = 1024 and 24 for offset

    struct TIMconfig
    {   // the default is for pwm @ 10.24 us (10240 ns) that w/ a 200 mHz clock gets arr = 1024

        constexpr TIMconfig() = default;
        constexpr TIMconfig(uint32_t pwmnanosec, uint32_t off = defoffset) 
            : autoreloadregister(pwmnanosec/10), offset(off), ccrfactor(0.01*autoreloadregister) {};
        
        constexpr uint32_t period() const { return autoreloadregister; }       
        constexpr uint32_t lowtrigger() const { return offset; } 
        constexpr uint32_t hightrigger() const { return autoreloadregister-offset; }
        constexpr uint32_t ccr(const float dutycyclepercentage) const { return static_cast<uint32_t>(ccrfactor*dutycyclepercentage); } 
        
    private:
        static constexpr uint32_t timclockspeed {200*1000*1000};
        static constexpr uint32_t defoffset {24};
        static constexpr uint32_t defautoreloadregister {1024};         
        uint32_t autoreloadregister { defautoreloadregister }; 
        uint32_t offset { defoffset };  
        float ccrfactor {0.01*defautoreloadregister};        
    }; 

    constexpr TIMconfig timCFGgzini     {10240, 24};    
    constexpr TIMconfig timCFG066666Hz  {15000, 36};
    constexpr TIMconfig timCFG100000Hz  {10000, 24};
    constexpr TIMconfig timCFG050000Hz  {20000, 24};
    
    enum class pwmFREQ { kHz050dot0, kHz066dot6, kHz097dot6, kHz100dot0, gzini = kHz097dot6};
    
    struct BSP : public embot::hw::bsp::SUPP
    {
        constexpr static std::uint8_t maxnumberof = embot::core::tointegral(embot::hw::MOTOR::maxnumberof);
        constexpr BSP(std::uint32_t msk) : SUPP(msk) {}
        constexpr BSP() : SUPP(0) {}            
   
        bool init(embot::hw::MOTOR m) const;
        bool deinit(embot::hw::MOTOR m) const;
        void set(embot::hw::MOTOR m, const embot::hw::motor::bldc::PWM3 &pwm) const;  
        void enable(embot::hw::MOTOR m, bool on) const; 

        const TIMconfig & timconfig() const;    
    };
    
    const BSP& getBSP();        
            
} // namespace embot::hw::motor::bldc::pwm::bsp {



namespace embot::hw::motor::bldc::pwm::bsp::stm32 {
        
    void HAL_ADC_MspInit(void* p);          // ADC_HandleTypeDef*
    void HAL_ADC_MspDeInit(void* p);        // ADC_HandleTypeDef*
    
    void HAL_TIM_Base_MspInit(void* p);     // TIM_HandleTypeDef*  
    void HAL_TIM_Base_MspDeInit(void* p);   // TIM_HandleTypeDef*   
    
} // namespace embot::hw::motor::bldc::pwm::bsp::stm32 {

    
#endif  // include-guard


// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

