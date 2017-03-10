


#if defined(STM32HAL_EXTERNAL_TICK_NONE)
#warning  STM32HAL_EXTERNAL_TICK_NONE is defined: this application will work, but some funtions of stm32hal which use HAL_GetTick() will not work
#endif

#include "stm32hal.h" // to see bsp_* functions and types

#include "eventviewer.h"

#include "embot.h"
#include "embot_hw.h"

void idle(void){}
void sig1(void){}


#undef DO_NOTHING

int main(void)
{ 
    embot::hw::sys::relocatevectortable(embot::hw::sys::addressOfApplication - embot::hw::sys::startOfFLASH);    
    embot::hw::bsp::init();
    
    eventviewer_init();
    eventviewer_load(ev_ID_idle, idle);
    eventviewer_load(ev_ID_first_usrdef, sig1);
    eventviewer_switch_to(ev_ID_idle);

#if     defined(DO_NOTHING)
    
    // description: do nothing but executing a dummy loop
    
    for(;;);   
    
#else
    
//    // description: by pressing btn we toggle led at release
//    
//    
//    embot::hw::bsp::btn_init(embot::hw::bsp::BTN::zero);    
//    for(;;)
//    {
//        if(true == embot::hw::bsp::btn_ispressed(embot::hw::bsp::BTN::zero))
//        {
//            while(embot::hw::bsp::btn_ispressed(embot::hw::bsp::BTN::zero));
//            embot::hw::bsp::led_toggle(embot::hw::bsp::LED::zero);   
//            
//            evEntityId_t prev = eventviewer_switch_to(ev_ID_first_usrdef);
//            embot::hw::sys::delay(20*1000);
//            eventviewer_switch_to(prev);            
//        }                
//    }
    


    // description: by pressing btn we toggle the status of led on/off at release. 
    // when the led is on it actually pulses at 4 hz
    // we use funtion HAL_Delay() 
    

    
    embot::hw::bsp::btn_init(embot::hw::bsp::BTN::zero);    

#if defined(STM32HAL_EXTERNAL_TICK_SYSTICK)
    // start systick at 1ms.
    HAL_SYSTICK_Config(SystemCoreClock/1000);
#endif
    
    static bool ledON = true;
    for(;;)
    {
                
        if(ledON)
        {
            HAL_Delay(50); 
            evEntityId_t prev = eventviewer_switch_to(ev_ID_first_usrdef);
            embot::hw::sys::delay(50*1000);
            eventviewer_switch_to(prev);
            
            embot::hw::bsp::led_toggle(embot::hw::bsp::LED::zero);
        }        
            
        
        if(true == embot::hw::bsp::btn_ispressed(embot::hw::bsp::BTN::zero))
        {
            while(embot::hw::bsp::btn_ispressed(embot::hw::bsp::BTN::zero));
            
            if(ledON)
            {
                ledON = false;
                embot::hw::bsp::led_off(embot::hw::bsp::LED::zero);
            }
            else
            {
                ledON = true;
                embot::hw::bsp::led_on(embot::hw::bsp::LED::zero);
            }
            
        }                
    }
    
#endif
    
}





///


