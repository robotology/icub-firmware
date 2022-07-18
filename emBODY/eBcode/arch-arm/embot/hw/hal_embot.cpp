
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - public interface
// --------------------------------------------------------------------------------------------------------------------

#include "hal.h"

// --------------------------------------------------------------------------------------------------------------------
// - implementation of selected functions using embot::hw 
// --------------------------------------------------------------------------------------------------------------------

// --- basic

#include "embot_hw_types.h"

constexpr hal_result_t halresult(embot::hw::result_t r)
{ 
    return (embot::hw::resOK == r) ? hal_res_OK : hal_res_NOK_generic;
}

constexpr hal_boolval_t halbool(bool b)
{ 
    return (b) ? hal_true : hal_false;
}

// --- hal_can

#include "embot_hw_can.h"

//constexpr hal_can_t convert(embot::hw::CAN id)
//{ 
//    if(embot::core::tointegral(id) >= embot::core::tointegral(embot::hw::CAN::maxnumberof))
//    {
//        return hal_can1;
//    }
//    return static_cast<hal_can_t>(id); 
//};

constexpr embot::hw::CAN convert(hal_can_t id)
{ 
    return static_cast<embot::hw::CAN>(id); 
};

extern hal_boolval_t hal_can_supported_is(hal_can_t id)
{
    return halbool(embot::hw::can::supported(convert(id)));
}

//extern hal_boolval_t hal_can_initted_is(hal_can_t id);

extern hal_result_t hal_can_init(hal_can_t id, const hal_can_cfg_t *cfg)
{
    if(nullptr == cfg)
    {
        return hal_res_NOK_generic;
    }
      
    embot::hw::can::Config config 
    { 
        cfg->capacityoftxfifoofframes, cfg->capacityofrxfifoofframes,
        {cfg->callback_on_tx, cfg->arg_cb_tx}, 
        {},
        {cfg->callback_on_rx, cfg->arg_cb_rx}    
    };
    
    return halresult(embot::hw::can::init(convert(id), config));
}

extern hal_result_t hal_can_enable(hal_can_t id)
{
    return halresult(embot::hw::can::enable(convert(id)));
}


//extern hal_result_t hal_can_disable(hal_can_t id);

extern hal_result_t hal_can_put(hal_can_t id, hal_can_frame_t *frame, hal_can_send_mode_t sm)
{
    if(nullptr == frame)
    {
        return hal_res_NOK_generic;
    }
    
    embot::hw::can::Frame fr
    {
        frame->id, frame->size, frame->data
    };
    
    if(embot::hw::resOK != embot::hw::can::put(convert(id), fr))
    {
        return hal_res_NOK_generic;
    }        
 
    if(hal_can_send_normprio_now == sm)
    {
        return hal_can_transmit(id);
    }
    
    return hal_res_OK;
}


extern hal_result_t hal_can_transmit(hal_can_t id)
{
    return halresult(embot::hw::can::transmit(convert(id)));   
}


extern hal_result_t hal_can_received(hal_can_t id, uint8_t *numberof)
{
    if(nullptr == numberof)
    {
        return hal_res_NOK_generic;
    }
    *numberof = embot::hw::can::inputqueuesize(convert(id));
    return hal_res_OK;
}



extern hal_result_t hal_can_get(hal_can_t id, hal_can_frame_t *frame, uint8_t *remaining)
{
    if(nullptr == frame)
    {
        return hal_res_NOK_generic;
    } 

    embot::hw::can::Frame fr{};
    uint8_t rem {0};
    if(embot::hw::resNOK == embot::hw::can::get(convert(id), fr, rem))
    {
        return hal_res_NOK_generic;
    }
    
    // copy fr
    frame->id = fr.id;
    frame->id_type = hal_can_frameID_std;
    frame->frame_type = hal_can_frame_data;
    frame->size = fr.size;
    std::memmove(frame->data, fr.data, sizeof(frame->data));
    if(nullptr != remaining)
    {
        *remaining = rem;
    }
    
    return hal_res_OK;
}


extern hal_result_t hal_can_out_get(hal_can_t id, uint8_t *numberof)
{
    if(nullptr == numberof)
    {
        return hal_res_NOK_generic;
    }
    
    *numberof = embot::hw::can::outputqueuesize(convert(id));
    
    return hal_res_OK;
}

//extern hal_result_t hal_can_getstatus(hal_can_t id, hal_can_status_t *status);


// --- hal_timer


#include "embot_hw_timer.h"

#warning TODO implement and use embot::hw::timer
    
embot::hw::TIMER convert(hal_timer_t id)
{
    if(hal_timerNONE == id)
    {
        return embot::hw::TIMER::none;
    }
    return static_cast<embot::hw::TIMER>(id);
}

hal_timer_status_t convert(embot::hw::timer::Status s)
{
    return static_cast<hal_timer_status_t>(s);
}
    
extern hal_result_t hal_timer_init(hal_timer_t id, const hal_timer_cfg_t *cfg, hal_reltime_t *error)
{
    embot::hw::timer::Config config {cfg->countdown, static_cast<embot::hw::timer::Mode>(cfg->mode), {cfg->callback_on_exp, cfg->arg}};
    if(false == embot::hw::timer::initialised(convert(id)))
    {
        return halresult(embot::hw::timer::init(convert(id), config));
    }
    else
    {
        return halresult(embot::hw::timer::configure(convert(id), config));
    }
}

extern hal_result_t hal_timer_start(hal_timer_t id)
{
    return halresult(embot::hw::timer::start(convert(id)));
}

extern hal_result_t hal_timer_stop(hal_timer_t id)
{
    return halresult(embot::hw::timer::stop(convert(id)));
}

extern hal_timer_status_t hal_timer_status_get(hal_timer_t id)
{
    embot::hw::timer::Status s = embot::hw::timer::status(convert(id));
    return convert(s);
}


// --- hal_adc

#include "hal_adc.h"

extern hal_dma_voltage_t hal_adc_get_hall_sensor_analog_input_mV(uint8_t motor)
{
    return 0;
}


// --- hal_motor

#include "hal_motor.h"


extern hal_result_t hal_motor_pwmset(hal_motor_t id, int16_t pwmvalue)
{
    return hal_res_NOK_generic;
}


extern hal_result_t hal_motor_enable(hal_motor_t id)
{
    return hal_res_NOK_generic;
}


extern hal_result_t hal_motor_disable(hal_motor_t id)
{
    return hal_res_NOK_generic;
}

extern hal_bool_t hal_motor_externalfaulted(void)
{
    return hal_false;
}

extern hal_result_t hal_motor_reenable_break_interrupts(void)
{
    return hal_res_NOK_generic;
}

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------

