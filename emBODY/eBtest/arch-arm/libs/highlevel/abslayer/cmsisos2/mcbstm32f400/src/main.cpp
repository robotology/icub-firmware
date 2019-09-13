
#include "stm32f4xx_hal.h"

#include "cmsisos2.h"

#include <string> 

static void SystemClock_Config(void);
static void Error_Handler(void);

extern int hl_sys_itm_puts(const char* str);

extern void threadmainCBK (void *arg);


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



int main(void)
{

    /* STM32F4xx HAL library initialization:
       - Configure the Flash prefetch, Flash preread and Buffer caches
       - Systick timer is configured by default as source of time base, but user 
             can eventually implement his proper time base source (a general purpose 
             timer for example or other time source), keeping in mind that Time base 
             duration should be kept 1ms since PPP_TIMEOUT_VALUEs are defined and 
             handled in milliseconds basis.
       - Low Level Initialization
     */
    HAL_Init();

    /* Configure the system clock to 168 MHz */
    SystemClock_Config();
    SystemCoreClockUpdate();
    
    hl_sys_itm_puts("about to start RTOS");

    /* Add your application code here
     */

    osKernelInitialize ();
    osThreadNew(threadmainCBK, NULL, &threadmainPROP);
    osKernelStart();  

    /* Infinite loop */
    while (1)
    {
    }
}



static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitTypeDef RCC_OscInitStruct;

  /* Enable Power Control clock */
  __HAL_RCC_PWR_CLK_ENABLE();

  /* The voltage scaling allows optimizing the power consumption when the device is 
     clocked below the maximum system frequency, to update the voltage scaling value 
     regarding system frequency refer to product datasheet.  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Enable HSE Oscillator and activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25U;
  RCC_OscInitStruct.PLL.PLLN = 336U;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7U;
  if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
     clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
  if(HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }

  /* STM32F405x/407x/415x/417x Revision Z devices: prefetch is supported */
  if (HAL_GetREVID() == 0x1001)
  {
    /* Enable the Flash prefetch */
    __HAL_FLASH_PREFETCH_BUFFER_ENABLE();
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
static void Error_Handler(void)
{
  /* User may add here some code to deal with this error */
  while(1)
  {
  }
}




// ------------------------------------------------------------------------------------------------------------------------

extern "C"
{
#include "Board_LED.h"                  // Board Support:LED
#include "Board_Buttons.h"              // Board Support:Buttons
}

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
      hl_sys_itm_puts(str);
    }

    if (active_flag == 1U){
      LED_On (0U);                                // Switch LED on
      osDelay (cmsisos2_sys_reltime2tick(500*cmsisos_reltime1millisec));                             // Delay 500 ms
      LED_Off (0U);                               // Switch off 
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
    state = (Buttons_GetState () & 1U);           // Get pressed button state
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
    (on) ? (LED_Off(1)) : (LED_On(1));
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
    hl_sys_itm_puts(str);
    
}

void threadmainCBK (void *arg)
{
    LED_Initialize ();                    // Initialize LEDs
    Buttons_Initialize ();                // Initialize Buttons

    hl_sys_itm_puts("about to start a thread to manage led0"); 
    tid_thrLED = osThreadNew (thrLED, NULL, &propL);  // Create LED thread
    if (tid_thrLED == NULL) { /* add error handling */ }

    hl_sys_itm_puts("about to start a thread to manage the user button: it selects/deslects blink of led"); 
    tid_thrBUT = osThreadNew (thrBUT, NULL, &propB);  // Create BUT thread
    if (tid_thrBUT == NULL) { /* add error handling */ }
    
    // build a timer
      
    osTimerAttr_t tmrOneattr {};
    tmrOneattr.name = nullptr;
    tmrOneattr.attr_bits = 0;  
    tmrOneattr.cb_mem = timerOneCBK;
    tmrOneattr.cb_size = sizeof(timerOneCBK);

    hl_sys_itm_puts("about to start a periodic timer w/ 1 second period which toggles led1 and write a time log");  
    timerOne = osTimerNew(timerOneCallback, osTimerPeriodic, nullptr, &tmrOneattr);        
    osTimerStart(timerOne, cmsisos2_sys_reltime2tick(cmsisos_reltime1second));    

    osThreadExit();    
}




extern int hl_sys_itm_puts(const char* str) 
{    
    uint32_t ch;
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






