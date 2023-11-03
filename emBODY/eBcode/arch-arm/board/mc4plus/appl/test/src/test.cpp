
/*
 * Copyright (C) 2023 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "test.h"


#include "embot_core.h"

#include "hal2.h"
#include "hal_motor.h"
#include "hal_adc.h"

namespace test::motor {

    static void s_eo_motioncontrol_mc4plusbased_hal_init_motors_adc_feedbacks(void)
    {   // low level init for motors and adc in mc4plus

        static bool _initted {false};
        
        if(_initted)
        {
           return;
        }
        
        _initted =  true;
        
        // currently the motors are initialized all together and without config
        hal_motor_init(hal_motorALL, nullptr);        
        // first initialize all ADC as indipendents
        hal_adc_common_structure_init();        
        // init the ADC to read the (4) current values of the motor and (4) analog inputs (hall_sensors, if any) using ADC1/ADC3
        // always initialized at the moment, but the proper interface for reading the values is in EOappEncodersReader
        hal_adc_dma_init_ADC1_ADC3_hall_sensor_current();   
        // verify that we dont use the bad memory bank ...    
        //init ADC2 to read supplyVoltage
        hal_adc_dma_init_ADC2_tvaux_tvin_temperature();
    }

    constexpr hal_motor_t mot {hal_motor1};

    void init()
    {
        s_eo_motioncontrol_mc4plusbased_hal_init_motors_adc_feedbacks();
        hal_motor_enable(mot);
    }

    void deinit()
    {
        hal_motor_disable(mot);
        embot::core::print("the rest is sadly missing");
    }

    void pmwset(int16_t pwm)
    {
        // pwm is clipped in [-PWM_PERIOD/2, -PWM_MINDUTY] U [+PWM_MINDUTY, +PWM_PERIOD/2]
        // where PWM_MINDUTY = 20 and PWM_PERIOD = 6720
        // so, ... [-3360, -20] U [+20, +3360]
        hal_motor_pwmset(mot, pwm);
    }

    void tick()
    {
        int16_t ma = hal_adc_get_current_motor_mA(mot);
        uint32_t vcc = hal_adc_get_supplyVoltage_mV();
        int16_t pwm = hal_motor_pwmget(mot);
        embot::core::print("values are: I = " + std::to_string(ma) + " [mA], " + "VCC = " + std::to_string(vcc) + " [mV], " + "PWM = " + 
                                std::to_string(pwm) +  " [raw]");
    }

}


void test_embot_hw_init()
{
    test::motor::init();   
    test::motor::pmwset(50);
}


void test_embot_hw_tick()
{   
    test::motor::tick();
}

// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------

