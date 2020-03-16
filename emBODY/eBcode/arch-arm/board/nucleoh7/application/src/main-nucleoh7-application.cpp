
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/

// either use stm32hal alone or together with embot
//#define USE_BARE_STM32HAL

#if defined(USE_BARE_STM32HAL)

#include "stm32hal.h"

#else

#include "embot_core.h"
#include "embot_hw.h"
#include "embot_hw_bsp.h"
#include "embot_hw_led.h"
#include "embot_hw_button.h"
#include "embot_hw_sys.h"

#endif

#if defined(USE_BARE_STM32HAL)

// we need to provide stm32hal a timebase in units of 1 ms (as the hal driver by stm requires). 
// we can use what we want. typically it is used the systick timer which increments a volatile
// counter. the systick is initted @ 1 khz 

static volatile uint64_t s_1mstickcount = 0; // it must be volatile
constexpr uint32_t s_rate1khz = 1000;

void SysTick_Handler(void)
{
    s_1mstickcount++;
}

static void stm32hal_tick1msecinit()
{
    HAL_SYSTICK_Config(SystemCoreClock/s_rate1khz);
}

static uint32_t stm32hal_tick1msecget()
{
    return s_1mstickcount;
}

// stm32hal requires two functions: one which inits and one that gets the 1 ms time.
static const stm32hal_config_t stm32halcfg { stm32hal_tick1msecinit, stm32hal_tick1msecget };
    
#else

// embot hw requires a timebase in units of 1 us from which we can obtain the 1 ms units required by stm32hal.
// the timebase of 1 us is instead used by embot::core::now() 
// we can use the timebase that we want. 
// with the rtos we use a dedicated function such as osal_system_abstime_get() or even better embot::sys::now()
// in our case w/out rtos we can still use the systick timer which increments a volatile counter.
// we still tick @ 1 khz and we just apply a 1000 factor. as a result of that embot::core::now() will return
// values with 1000 us resolution.


static volatile uint64_t s_tickcount = 0; // it must be volatile
constexpr uint32_t s_rate = 10000;
constexpr uint32_t s_numof1usecintick = 1000 * 1000 / s_rate;

void SysTick_Handler(void)
{
    s_tickcount++;
}

static void embot_usectime_init(void)
{
    HAL_SYSTICK_Config(SystemCoreClock/s_rate);
}
   
embot::core::Time embot_usectime_get()
{
   return s_numof1usecintick * s_tickcount; 
}

constexpr embot::hw::Config hwconfig {embot_usectime_init, embot_usectime_get};

#endif


static void s_chips_init();
static void s_chips_tick();

// --------------------------------------------------------------------------------------------------------------------

int main(void)
{ 

#if defined(USE_BARE_STM32HAL)
    
    // inittes stm32hal. it internally calls initialization of HAL, configures clocks etc.
    stm32hal_init(&stm32halcfg);    

    // by inclusion of stm32hal.h we can use the HAL functions and also the macros which are
    // defined in main.h of cube-mx
    
    // in here we just blinck a led
    
    // turn led off and then on
    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);

    uint32_t flagON = 1;
    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
    
    for(;;)
    {        
        // delay of 500 ms: it is ok only if the 1 ms tick is correctly initted         
        HAL_Delay(500);
        
        if(1 == flagON)
        {
            flagON = 0;
            // turn off
            HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);            
        }
        else
        {
            flagON = 1;
            // turn on
            HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);                   
        }                    
    }
    
#else


   
    // init the embot hw. it internally calls stm32hal_init() which calls whatever stm32 HAL requires . 
    // it also calls embot::core::init()
    embot::hw::init(hwconfig);
    
    // init LED::one (which GPIO mapping is specified inside embot::hw::bsp) and play with it
    embot::hw::led::init(embot::hw::LED::one);
    embot::hw::led::on(embot::hw::LED::one);
    embot::hw::led::off(embot::hw::LED::one);
        
    // init the push button 
    embot::hw::button::init(embot::hw::BTN::one);
    
    // puts a message onto the debug window of the KEIL IDE
    embot::hw::sys::puts("hello world");
   
    // specify a delay using the units defined in embot::common namespace    
    embot::core::relTime delay = 500 * embot::core::time1millisec;
       
    // prepare timing statistics
    embot::core::Time t = embot::core::now();
    embot::core::Time prev = t;
    embot::core::relTime delta = t - prev;
        
    embot::hw::sys::puts("starting led blinking every " + std::to_string(delay / embot::core::time1millisec) + " millisec");
    embot::hw::sys::puts("push the user button to print statistics");
    
    s_chips_init();
            
    for(;;)
    {
        embot::core::Time t0 = embot::core::now();
        embot::hw::sys::delay(delay);        
        embot::core::relTime delta =  embot::core::now() - t0;
        
        embot::hw::led::toggle(embot::hw::LED::one);

        if(true == embot::hw::button::pressed(embot::hw::BTN::one))
        {
            embot::hw::sys::puts("button pressed: we are at " + std::to_string(embot::core::now() / embot::core::time1millisec) + " millisec after startup");
            embot::hw::sys::puts("the previous call of embot::hw::sys::delay() for " + std::to_string(delay / embot::core::time1millisec) + " ms has lasted: " + std::to_string(delta) + " us");           
            
//            delay = delay / 2;           
//            if(delay < (10 *  embot::core::time1millisec))
//            {
//                delay = embot::core::time1second;
//            }
//            embot::hw::sys::puts("delay is now " + std::to_string(delay / embot::core::time1millisec) + " ms");       

            s_chips_tick();
        }        
      
    }

#endif    
    
    // just because i am paranoid
    for(;;);    
}


#if defined(USE_BARE_STM32HAL)

static void s_chips_init() {}
static void s_chips_tick() {}

#else

#include "embot_hw_bno055.h"

#include <cstring>

static void s_chips_init()
{
    constexpr embot::core::relTime timeout = 5*embot::core::time1millisec;
    
    embot::hw::bno055::Config bno055config { embot::hw::i2c::Descriptor { embot::hw::I2C::one, 400000 } };
    embot::hw::bno055::init(embot::hw::BNO055::one, bno055config); 
    
    constexpr embot::hw::bno055::Mode nonFusionMode = embot::hw::bno055::Mode::ACCGYRO;
    constexpr embot::hw::bno055::Mode fusionMode = embot::hw::bno055::Mode::NDOF;
    embot::hw::bno055::set(embot::hw::BNO055::one, nonFusionMode, timeout);   
}
    
volatile HAL_StatusTypeDef r = HAL_ERROR;

static void s_chips_tick()
{
    embot::hw::bno055::Data data;
    
    data.clear();
    
    embot::core::utils::Triple<float> a {0, 0, 0};
    

    
    if(embot::hw::result_t::OK == embot::hw::bno055::acquisition(embot::hw::BNO055::one, embot::hw::bno055::Set::AMG))
    {
        embot::hw::bno055::read(embot::hw::BNO055::one, data);
        data.getACC(a);
        embot::hw::sys::puts("BNO055: acc = (" + std::to_string(a.x) + ", " + std::to_string(a.y) + ", " + std::to_string(a.z) + ") m/(s*s)" );
    }
    else
    {  
        std::string txtout = "BNO055: invalid acc = (" + std::to_string(a.x) + ", " + std::to_string(a.y) + ", " + std::to_string(a.z) + ") m/(s*s)";        
        embot::hw::sys::puts(txtout );
        char text[32] = {'h', 'E', 'l', 'l', 'o', '!', ' '}; 
        // 'hello! " is transmited in about 600 usec @ 115200 w/ 1 bit stop 
        // (9 bits per char) -> 1/(115200/9) = 0.000078125 sec / byte. For 7 bytes -> 546 usec
        embot::core::Time t0 = embot::core::now();
        //r = HAL_UART_Transmit(&huart3, const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(txtout.c_str())), std::strlen(txtout.c_str()), 0xFFFF);
        r = HAL_UART_Transmit(&huart3, const_cast<uint8_t*>(reinterpret_cast<const uint8_t*>(text)), std::strlen(text), 0xFFFF);
        embot::core::relTime delta =  embot::core::now() - t0;
        embot::hw::sys::puts("uart tx lasted " + std::to_string(delta) + " us (with granularity given by the systick rate)"); 
        r = r;
    }
    
}

#endif

#if 0
#include "embot_hw_si7051.h"

volatile uint32_t temperatureISready = 0; 
static void setflag(void*)
{
    temperatureISready = 1;    
}


static void s_chips_init()
{
    embot::hw::si7051::init(embot::hw::SI7051::one, {embot::hw::I2C::one, 400000});
    
    embot::hw::si7051::Temperature temperature = 0;
    
    if(true == embot::hw::si7051::isalive(embot::hw::SI7051::one))
    {
        embot::core::Callback cbk { setflag, nullptr};
        temperatureISready =  0;
        embot::hw::si7051::acquisition(embot::hw::SI7051::one, cbk);
        for(;;)
        {
            if(1 == temperatureISready)
            {
                break;
            }
        }
        embot::hw::si7051::read(embot::hw::SI7051::one, temperature);
        
    }
    
}

#endif



// required events: IMUacquisition + IMUvalueisready + ADCacquisition + ADCvalueisready + TXframe
// one thread which processes them all + ledmanager + 

// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



