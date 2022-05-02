
/*
 * Copyright (C) 2022 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

//// for api
#include "hal.h"
//#include "hal_trace.h"
//#include "hal_5v.h"

// for implementation

//#include "embot_hw.h"
//#include "embot_hw_led.h"
//#include "embot_hw_sys.h"
#include "embot_hw_can.h"
//#include "embot_hw_flash.h"
//#include "embot_hw_eeprom.h"

#include "embot_core.h"


void convert(const hal_can_cfg_t *cfg, embot::hw::can::Config &config)
{
    config.rxcapacity = cfg->capacityofrxfifoofframes;
    config.txcapacity = cfg->capacityoftxfifoofframes;
    config.onrxframe = {cfg->callback_on_rx, cfg->arg_cb_rx};
    config.ontxframe = {cfg->callback_on_tx, cfg->arg_cb_tx};
    config.txqueueempty = {};
}

embot::hw::CAN convert(hal_can_t id) { return (hal_can1 == id) ? embot::hw::CAN::one : embot::hw::CAN::two; }


//// can

extern hal_result_t hal_can_init(hal_can_t id, const hal_can_cfg_t *cfg)
{
    embot::hw::can::Config config {};
    convert(cfg, config);
    embot::hw::can::init(convert(id), config); 
    return hal_res_OK;
}    

extern hal_result_t hal_can_enable(hal_can_t id)
{
    embot::hw::can::enable(convert(id));
    return hal_res_OK;
}    

extern hal_result_t hal_can_disable(hal_can_t id)
{
    embot::hw::can::disable(convert(id));
    return hal_res_OK;
}    

extern hal_result_t hal_can_put(hal_can_t id, hal_can_frame_t *frame, hal_can_send_mode_t sm)
{
    embot::hw::can::put(convert(id), {frame->id, frame->size, frame->data});
    return hal_res_OK;
}    

extern hal_result_t hal_can_transmit(hal_can_t id)
{
    embot::hw::can::transmit(convert(id));
    return hal_res_OK;
}    

extern hal_result_t hal_can_received(hal_can_t id, uint8_t *numberof)
{
    if(nullptr != numberof)
    {
        *numberof = embot::hw::can::inputqueuesize(convert(id));
        return hal_res_OK;
    }        
    return hal_res_NOK_generic;
}   

extern hal_result_t hal_can_get(hal_can_t id, hal_can_frame_t *frame, uint8_t *remaining)
{
    embot::hw::can::Frame fr {};
    uint8_t rem {0};
    embot::hw::can::get(convert(id), fr, rem);
    frame->id = fr.id;
    frame->size = fr.size;
    std::memmove(frame->data, fr.data, fr.size);      
    if(nullptr != remaining)
    {
        *remaining = rem;
    }        
    return hal_res_OK;
}    



// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






