
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

//// for api
#include "hal.h"
//#include "hal_5v.h"

// for implementation

//#include "embot_hw.h"
//#include "embot_hw_led.h"
//#include "embot_hw_sys.h"
//#include "embot_hw_can.h"
//#include "embot_hw_flash.h"


// that is the only required so far
extern void hal_sys_irq_disable(void) 
{
    // nothing so far
}   


// eth

//extern hal_result_t hal_eth_init(const hal_eth_cfg_t *cfg)
//{
//    return hal_res_OK;
//}

//extern hal_result_t hal_eth_enable(void)
//{
//    return hal_res_OK;
//}

//extern hal_result_t hal_eth_disable(void)
//{
//    return hal_res_OK;
//}

//extern hal_result_t hal_eth_sendframe(hal_eth_frame_t *frame)
//{
//    return hal_res_OK;
//}


// led

//embot::hw::LED s_hal_to_embothw(hal_led_t id)
//{
//    embot::hw::LED l {embot::hw::LED::none};
//    uint8_t index = static_cast<uint8_t>(id);
//    if(index < embot::core::tointegral(embot::hw::LED::maxnumberof))
//    {
//        l = static_cast<embot::hw::LED>(index);
//    }
//    
//    return l;   
//}

//extern hal_result_t hal_led_init(hal_led_t id, const hal_led_cfg_t *cfg)
//{
//    embot::hw::result_t r = embot::hw::led::init(s_hal_to_embothw(id));
//    return (embot::hw::resOK == r) ? hal_res_OK : hal_res_NOK_generic;
//}

//extern hal_result_t hal_led_on(hal_led_t id)
//{
//    embot::hw::result_t r = embot::hw::led::on(s_hal_to_embothw(id));
//    return (embot::hw::resOK == r) ? hal_res_OK : hal_res_NOK_generic;
//}

//extern hal_result_t hal_led_off(hal_led_t id)
//{
//    embot::hw::result_t r = embot::hw::led::off(s_hal_to_embothw(id));
//    return (embot::hw::resOK == r) ? hal_res_OK : hal_res_NOK_generic;
//}


//extern hal_result_t hal_led_toggle(hal_led_t id)
//{
//    embot::hw::result_t r = embot::hw::led::toggle(s_hal_to_embothw(id));
//    return (embot::hw::resOK == r) ? hal_res_OK : hal_res_NOK_generic;
//}

// sys

//extern hal_result_t hal_sys_delay(hal_reltime_t reltime)
//{
//    return hal_res_OK;
//}

//extern void hal_sys_irq_disable(void) 
//{

//}    

//extern void hal_sys_irq_enable(void) 
//{

//}

//extern hal_result_t hal_sys_systemreset(void) 
//{
//    return hal_res_OK;
//}


//extern hal_result_t hal_sys_canexecuteataddress(uint32_t addr)
//{
//    return hal_res_OK;
//}

//extern hal_result_t hal_sys_executenowataddress(uint32_t addr) 
//{
//    return hal_res_OK;
//}

//extern hal_result_t hal_sys_vectortable_relocate(uint32_t offset) 
//{
//    return hal_res_OK;
//}

// eeprom

//extern hal_result_t hal_eeprom_init(hal_eeprom_t id, const hal_eeprom_cfg_t *cfg) 
//{
//    return hal_res_OK;
//}

//extern hal_result_t hal_eeprom_read(hal_eeprom_t id, uint32_t addr, uint32_t size, void *data) 
//{
//    return hal_res_OK;
//}

//extern hal_result_t hal_eeprom_write(hal_eeprom_t id, uint32_t addr, uint32_t size, void *data) 
//{
//    return hal_res_OK;
//}

//extern hal_result_t hal_eeprom_erase(hal_eeprom_t id, uint32_t addr, uint32_t size) 
//{
//    return hal_res_OK;
//}

// flash

//extern hal_result_t hal_flash_unlock(void)
//{
//    return hal_res_OK;
//}

//extern hal_result_t hal_flash_erase(uint32_t addr, uint32_t size) 
//{
//    return hal_res_OK;
//}

//extern hal_result_t hal_flash_write(uint32_t addr, uint32_t size, void *data) 
//{
//    return hal_res_OK;
//}

//// uniqueid

//extern hal_uniqueid_id64bit_t hal_uniqueid_id64bit_get(void)
//{
//    return 0;
//}

//// can

//extern hal_result_t hal_can_init(hal_can_t id, const hal_can_cfg_t *cfg)
//{
//    return hal_res_OK;
//}    


//extern hal_result_t hal_can_enable(hal_can_t id)
//{
//    return hal_res_OK;
//}    


//extern hal_result_t hal_can_disable(hal_can_t id)
//{
//    return hal_res_OK;
//}    



//extern hal_result_t hal_can_put(hal_can_t id, hal_can_frame_t *frame, hal_can_send_mode_t sm)
//{
//    return hal_res_OK;
//}    

//extern hal_result_t hal_can_transmit(hal_can_t id)
//{
//    return hal_res_OK;
//}    


//extern hal_result_t hal_can_received(hal_can_t id, uint8_t *numberof)
//{
//    return hal_res_OK;
//}   

//extern hal_result_t hal_can_get(hal_can_t id, hal_can_frame_t *frame, uint8_t *remaining)
//{
//    return hal_res_OK;
//}    

// trace

//extern int hal_trace_puts(const char * str)
//{
//    return embot::hw::sys::puts(str);
//}

// 5v

//extern hal_result_t hal_5v_init(const hal_5v_cfg_t *cfg)
//{
//    return hal_res_OK;
//} 

//extern hal_result_t hal_5v_on(void)
//{
//    return hal_res_OK;
//} 


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------






