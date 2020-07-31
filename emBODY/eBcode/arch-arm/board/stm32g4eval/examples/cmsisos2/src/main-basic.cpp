
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


#include "stm32hal.h"
#include "cmsisos2.h"

// stm32hal requires two functions: one which inits and one that gets the 1 ms time.
static void stm32hal_tick1msecinit() {}
static uint32_t stm32hal_tick1msecget() 
{ 
    static volatile uint32_t tt = 0; 
    for(int i = (SystemCoreClock >> 14U); i > 0U; i--) {
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
        __NOP(); __NOP(); __NOP(); __NOP(); __NOP(); __NOP();
    }
    return ++tt; 
}
static const stm32hal_config_t systickcfg = { stm32hal_tick1msecinit, stm32hal_tick1msecget };

// they use STM32 HAL 
static void led_init();
static void led_toggle();
static void led_on();
static void led_off();
static int itm_puts(const char *str);

static void led_on1();
static void led_off1();

// --------------------------------------------------------------------------------------------------------------------

extern void threadmainCBK (void *arg);

extern osThreadAttr_t threadmainPROP;

int main(void)
{ 
    stm32hal_init(&systickcfg);
    
    
    itm_puts("hello world");
    
    led_init();
    itm_puts("led initted");
    
    led_toggle();
    
    osRtxInfo.kernel.state = osRtxKernelInactive;
    osKernelInitialize ();
    osThreadNew(threadmainCBK, NULL, &threadmainPROP);
    osKernelStart();  
    
    for(;;);
        
//    for(;;)
//    {
//        HAL_Delay(500);
//        led_toggle();
//        itm_puts("led toggled");
//    }    
}


// -------------------------------------------------------------------------------------------------------------------

// we need to provide stm32hal a timebase in units of 1 ms (as the hal driver by stm requires). 
// we can use what we want. typically it is used the systick timer which increments a volatile
// counter. the systick is initted @ 1 khz 



int itm_puts(const char* str) 
{    

    if(nullptr == str)
    {
        return(0);
    }


    std::uint32_t ch;
    int num = 0;
    while('\0' != (ch = *str))
    {
        ITM_SendChar(ch);
        str++;
        num++;
    }
     
    ITM_SendChar('\n');
    return(++num);    
}

// the led section. we use PB0

#if defined(STM32HAL_BOARD_STM32G4EVAL)

    #warning we are using board stm32g4eval, led is on pg9
    // led1 is on PG9
    static GPIO_TypeDef * GPIOLED = GPIOG;
    static const uint16_t PinLED = GPIO_PIN_9;
    static void clockEnableLED() {  __HAL_RCC_GPIOG_CLK_ENABLE(); }

#elif defined(STM32HAL_BOARD_NUCLEOH7)

    #warning we are using board nucleoh7, led is on pb0
    // led1 is on PG9
    static GPIO_TypeDef * GPIOLED = GPIOB;
    static const uint16_t PinLED = GPIO_PIN_0;
    static void clockEnableLED() {  __HAL_RCC_GPIOB_CLK_ENABLE(); }
    

// needed because we have removed that from stm32hal
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 
{     
    // nothing
}  

#endif

static void led_init()
{
    // start clock
    clockEnableLED();
    
    // configure led
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = PinLED;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOLED, &GPIO_InitStruct);
    
    // switch it on
    HAL_GPIO_WritePin(GPIOLED, PinLED, GPIO_PIN_SET); 
    
    // switch it off
    HAL_GPIO_WritePin(GPIOLED, PinLED, GPIO_PIN_RESET);    
}

static void led_toggle()
{
    HAL_GPIO_TogglePin(GPIOLED, PinLED);    
}

static void led_on()
{
    HAL_GPIO_WritePin(GPIOLED, PinLED, GPIO_PIN_SET);    
}

static void led_off()
{
    HAL_GPIO_WritePin(GPIOLED, PinLED, GPIO_PIN_RESET);    
}

static void led_on1() 
{
    led_on();
    itm_puts("L1:on()");
}

static void led_off1()
{
    led_off();
    itm_puts("L1:off()");
}

/// -- cmsisos2


// ------------------------------------------------------------------------------------------------------------------------

#include <cstdio>

uint64_t buffer[4096/8] = {};
    
uint32_t thread_cb[osRtxThreadCbSize/4U] = {0};
osThreadAttr_t threadmainPROP = 
{
    "main",
    osThreadDetached,
    thread_cb, 
    sizeof(thread_cb),
    
    buffer,
    sizeof(buffer),
        osPriorityNormal,
    0,
    0        
};



static osThreadId_t tid_thrLED;         // Thread id of thread: LED
static osThreadId_t tid_thrBUT;         // Thread id of thread: BUT

/*------------------------------------------------------------------------------
  thrLED: blink LED
 *----------------------------------------------------------------------------*/
void thrLED (void *argument) {
  uint32_t active_flag = 1U;
    
   char str[64] = {0};

  for (;;) {
    if (osThreadFlagsWait (1U, osFlagsWaitAny, cmsisos2_sys_reltime2tick(cmsisos_reltime1millisec)) == 1U) {
      active_flag ^=1U; 
      snprintf(str, sizeof(str), "commuted blinking of led0 to %s", (1 == active_flag) ? "ON" : "OFF");
      itm_puts(str);
    }

    if (active_flag == 1U){
//      led_on();                                // Switch LED on
      osDelay (cmsisos2_sys_reltime2tick(500*cmsisos_reltime1millisec));                             // Delay 500 ms
      led_off();                               // Switch off 
      osDelay (cmsisos2_sys_reltime2tick(500*cmsisos_reltime1millisec));                             // Delay 500 ms
    }
    else {
      osDelay (cmsisos2_sys_reltime2tick(500*cmsisos_reltime1millisec));                             // Delay 500 ms
    }
  }
}

/*------------------------------------------------------------------------------
  thrBUT: check button state
 *----------------------------------------------------------------------------*/
static void thrBUT(void *argument) {
  uint32_t last;
  uint32_t state;

  for (;;) {
    state = 0; //(Buttons_GetState () & 1U);           // Get pressed button state
    if (state != last){
      if (state == 1){
        osThreadFlagsSet (tid_thrLED, 1U);        // Set flag to thrLED
      }
    }
    last = state;
    osDelay (cmsisos2_sys_reltime2tick(100*cmsisos_reltime1millisec));
  }
}

uint64_t bufferB[1024] = {};
uint32_t thread_cbB[osRtxThreadCbSize/4U] = {0};     
osThreadAttr_t propB = 
{
    "B",
    osThreadDetached,
    thread_cbB, 
    sizeof(thread_cbB),
    
    bufferB,
    sizeof(bufferB),
        osPriorityNormal,
    0,
    0
        
};

uint64_t bufferL[1024] = {};
uint32_t thread_cbL[osRtxThreadCbSize/4U] = {0};    
osThreadAttr_t propL = 
{
    "L",
    osThreadDetached,
    thread_cbL, 
    sizeof(thread_cbL),
    
    bufferL,
    sizeof(bufferL),
        osPriorityNormal,
    0,
    0
        
};

static uint32_t tickcount = 0;
static uint32_t tickfreq = 0;
static uint64_t syscount = 0;
static uint32_t sysfreq = 0;

uint64_t gettime()
{
    uint64_t t = osKernelGetSysTimerCount();
    uint64_t d = osKernelGetSysTimerFreq() / 1000000;
    return t / d;
}



static uint64_t tt = 0;

static volatile uint64_t time0 = 0;
static volatile uint64_t time1 = 0;
static volatile uint64_t time2 = 0;

static uint32_t timerOneCBK[osRtxTimerCbSize/4] = {0};
static osTimerId_t timerOne {nullptr};   
static void timerOneCallback(void *p)
{
    static bool on = false;
    
    time0 = cmsisos2_sys_abstime();
    (on) ? (led_off1()) : (led_on1());
    on = !on;   

    tickcount = osKernelGetTickCount();
    tickfreq = osKernelGetTickFreq();
    
    syscount = osKernelGetSysTimerCount();
    
    sysfreq = osKernelGetSysTimerFreq();
    sysfreq /= 1000000;
    
    tt = syscount / sysfreq;
    
    time1 = cmsisos2_sys_abstime();
    time2 = cmsisos2_sys_abstime();
    
    char str[128] = {0};
    snprintf(str, sizeof(str), "absolute time now is %lldsec %lldms %lldusec (%lld us)", time0/1000000,  (time0%1000000)/1000, (time0%1000), time0);
    itm_puts(str);
    
}

void threadmainCBK (void *arg)
{
//    LED_Initialize ();                    // Initialize LEDs
//    Buttons_Initialize ();                // Initialize Buttons

    itm_puts("about to start a thread to manage led0"); 
    tid_thrLED = osThreadNew (thrLED, NULL, &propL);  // Create LED thread
    if (tid_thrLED == NULL) { /* add error handling */ }

    itm_puts("about to start a thread to manage the user button: it selects/deslects blink of led"); 
    tid_thrBUT = osThreadNew (thrBUT, NULL, &propB);  // Create BUT thread
    if (tid_thrBUT == NULL) { /* add error handling */ }
    
    // build a timer
      
    osTimerAttr_t tmrOneattr {};
    tmrOneattr.name = nullptr;
    tmrOneattr.attr_bits = 0;  
    tmrOneattr.cb_mem = timerOneCBK;
    tmrOneattr.cb_size = sizeof(timerOneCBK);

    itm_puts("about to start a periodic timer w/ 1 second period which toggles led1 and write a time log");  
    timerOne = osTimerNew(timerOneCallback, osTimerPeriodic, nullptr, &tmrOneattr);        
    osTimerStart(timerOne, cmsisos2_sys_reltime2tick(cmsisos_reltime1second));    

    osThreadExit();    
}




// - end-of-file (leave a blank line after)----------------------------------------------------------------------------



