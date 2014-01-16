
#include "hl_common.h"
#include "hl_sys.h"


static void s_hl_cpu_set_sys_clock(void);
static void s_hl_cpu_system_core_clock_update(void);

typedef struct 
{
    uint32_t                        max;
    uint32_t                        cpu;
    uint32_t                        fastbus;
    uint32_t                        slowbus;
} hl_cpu_hid_speeds_t;

typedef enum 
{
    hl_cpu_refclock_internal                = 0,
    hl_cpu_refclock_external_xtl            = 1,
    hl_cpu_refclock_external_osc            = 2,
    hl_cpu_refclock_pll_on_internal         = 3,
    hl_cpu_refclock_pll_on_external_xtl     = 4,   
    hl_cpu_refclock_pll_on_external_osc     = 5   
} hl_cpu_hid_refclock_t;

typedef struct 
{   // pllfreq = n*(source/m), speedcpu = pllfreq/p, 48mhz = pllfreq/q 
    uint32_t                        m;    
    uint32_t                        n;
    uint32_t                        p;
    uint32_t                        q;
} hl_cpu_hid_pll_cfg_t;

typedef struct 
{
    hl_bool_t                       keepinternalclockatstartup; // if hl_true: it runs at intclockspeed at startup and changes to refclock inside hl_cpu_systeminit()
    hl_cpu_hid_refclock_t           refclock;                   // the clock or mixture of clock and pll to use          
    uint32_t                        intclockspeed;              // the speed of the internal clock   
    uint32_t                        extclockspeed;              // teh speed of teh external clock
    hl_cpu_hid_pll_cfg_t            pllcfg;                     // the way the pll, if used, is configured
} hl_cpu_hid_clock_cfg_t;


typedef struct
{
    hl_cpu_hid_speeds_t             speeds;             // the speeds for cpu, fast bus and slow bus that the system has
    hl_cpu_hid_clock_cfg_t          clockcfg;           // the configuration of clock etc., which are used to achieve the above speeds
} hl_cpu_hid_brdcfg_t;


static const hl_cpu_hid_brdcfg_t cpu_brdcfg =
{
    .speeds             =
    { 
        .max                = 168000000,
        .cpu                = 168000000,
        .fastbus            =  84000000,
        .slowbus            =  42000000
    },
    .clockcfg                      =
    {
        .keepinternalclockatstartup     = hl_false,      // false is the preferred mode
        .refclock                       = hl_cpu_refclock_pll_on_external_xtl,  
        .intclockspeed                  = HL_INTclockspeed,
        .extclockspeed                  = HL_EXTclockspeed,
        .pllcfg                         =
        {   // pllfreq = n*(source/m), speedcpu = pllfreq/p, 48mhz = pllfreq/q
            .m                  =  25, 
            .n                  = 168,
            .p                  =   2,
            .q                  =   7
        }
    }
};




// redefinition of function weakly defined in system_stm32f4xx.c so that the mpus uses hsi and runs at 16MHz
void SystemInit(void)
{

    /* FPU settings ------------------------------------------------------------*/
    #if (__FPU_PRESENT == 1) && (__FPU_USED == 1)
        SCB->CPACR |= ((3UL << 10*2)|(3UL << 11*2));  /* set CP10 and CP11 Full Access */
    #endif

    
    /* Reset the RCC clock configuration to the default reset state ------------*/
    /* Set HSION bit */
    RCC->CR |= (uint32_t)0x00000001;

    /* Reset CFGR register */
    RCC->CFGR = 0x00000000;

    /* Reset HSEON, CSSON and PLLON bits */
    RCC->CR &= (uint32_t)0xFEF6FFFF;

    /* Reset PLLCFGR register */
    RCC->PLLCFGR = 0x24003010;

    /* Reset HSEBYP bit */
    RCC->CR &= (uint32_t)0xFFFBFFFF;

    /* Disable all interrupts */
    RCC->CIR = 0x00000000;

//#ifdef DATA_IN_ExtSRAM
//  SystemInit_ExtMemCtl(); 
//#endif /* DATA_IN_ExtSRAM */
         
//  /* Configure the System clock source, PLL Multiplier and Divider factors, 
//     AHB/APBx prescalers and Flash settings ----------------------------------*/
//  SetSysClock();

  /* Configure the Vector Table location add offset address ------------------*/
//#ifdef VECT_TAB_SRAM
//  SCB->VTOR = SRAM_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal SRAM */
//#else
//  SCB->VTOR = FLASH_BASE | VECT_TAB_OFFSET; /* Vector Table Relocation in Internal FLASH */
//#endif

    SCB->VTOR = FLASH_BASE; /* Vector Table Relocation in Internal FLASH */
    
    // now the HSI is selected as system clock, and no prescaler is used     

    if(hl_false == cpu_brdcfg.clockcfg.keepinternalclockatstartup)
    {   // apply the clock straigth away
        s_hl_cpu_set_sys_clock();
        s_hl_cpu_system_core_clock_update();        
    }
    else
    {   // keep the hsi
        // update value ...
        SystemCoreClockUpdate();
    }
}

extern const volatile uint8_t AHBPrescTable[16];

// redefinition of the function weakly defined inside system_stm32f4xx.c
void SystemCoreClockUpdate(void)
{
    uint32_t tmp = 0, pllvco = 0, pllp = 2, pllsource = 0, pllm = 2;
  
    /* Get SYSCLK source -------------------------------------------------------*/
    tmp = RCC->CFGR & RCC_CFGR_SWS;

    switch(tmp)
    {
        case 0x00:  /* HSI used as system clock source */
        {
            SystemCoreClock = HSI_VALUE;
        } break;
        
        case 0x04:  /* HSE used as system clock source */
        {
            if(0 == cpu_brdcfg.clockcfg.extclockspeed)
            {
                hl_sys_on_error(hl_error_unsupportedbehaviour, "SystemCoreClockUpdate() called with HSE mode but HSE clock is not defined");
            }
            SystemCoreClock = cpu_brdcfg.clockcfg.extclockspeed;
        } break;
        
        case 0x08:  /* PLL used as system clock source */
        {
            /* PLL_VCO = (HSE_VALUE or HSI_VALUE /  HAL_SYS_PLL_M) *  HAL_SYS_PLL_N
                SYSCLK = PLL_VCO /  HAL_SYS_PLL_P
             */    
            pllsource = (RCC->PLLCFGR & RCC_PLLCFGR_PLLSRC) >> 22;
            pllm = RCC->PLLCFGR & RCC_PLLCFGR_PLLM;
      
            if (pllsource != 0)
            {
                /* HSE used as PLL clock source */
                if(0 == cpu_brdcfg.clockcfg.extclockspeed)
                {
                    hl_sys_on_error(hl_error_unsupportedbehaviour, "SystemCoreClockUpdate() called with PLL w/ HSE ref mode but HSE clock is not defined");
                }
                pllvco = (cpu_brdcfg.clockcfg.extclockspeed / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);
            }
            else
            {
                /* HSI used as PLL clock source */
                pllvco = (HSI_VALUE / pllm) * ((RCC->PLLCFGR & RCC_PLLCFGR_PLLN) >> 6);      
            }

            pllp = (((RCC->PLLCFGR & RCC_PLLCFGR_PLLP) >>16) + 1 ) *2;
            SystemCoreClock = pllvco/pllp;
            
        } break;
        
        default:
        {
            SystemCoreClock = HSI_VALUE;
        } break;
    }
    
    /* Compute HCLK frequency --------------------------------------------------*/
    /* Get HCLK prescaler */
    tmp = AHBPrescTable[((RCC->CFGR & RCC_CFGR_HPRE) >> 4)];
    /* HCLK frequency */
    SystemCoreClock >>= tmp;
}

static void s_hl_cpu_set_sys_clock(void)
{
    
    // in here we can make many verifications ...  but we just dont care and use 
    // data defined in the brdcfg file   
    
    
    // --- if specified, then init the external clock: xtal or oscillator
    
    if(0 != cpu_brdcfg.clockcfg.extclockspeed)
    {
        __IO uint32_t StartUpCounter = 0, HSEStatus = 0;
            
        // we enable the bypass only if we want using an external oscillator, either directly or through pll
        if( (hl_cpu_refclock_external_osc        == cpu_brdcfg.clockcfg.refclock) ||
            (hl_cpu_refclock_pll_on_external_osc == cpu_brdcfg.clockcfg.refclock) )
        {
            // enable hse bypass     
            RCC->CR |= ((uint32_t)RCC_CR_HSEBYP);
        }        
    
        // enable HSE 
        RCC->CR |= ((uint32_t)RCC_CR_HSEON);

        // wait till HSE is ready and if timeout is reached exit 
        do
        {
            HSEStatus = RCC->CR & RCC_CR_HSERDY;
            StartUpCounter++;
        } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

        if(RESET != (RCC->CR & RCC_CR_HSERDY))
        {
            HSEStatus = (uint32_t)0x01;
        }
        else
        {
            HSEStatus = (uint32_t)0x00;
        }


        // if HSE fails to start-up, the application will have wrong clock
        // configuration. user can add here some code to deal with this error

        if((uint32_t)0x00 == HSEStatus)
        {
            // we just do nothing. it means that we use the default clock which is already initted: the internal one.
            // we also do not configure any prescaler for the fast and slow bus.
            return;
        }
        
    }

    // --- configure regulator voltage
    // select regulator voltage output Scale 1 mode, system frequency up to 168 MHz
    RCC->APB1ENR |= RCC_APB1ENR_PWREN;
    PWR->CR |= PWR_CR_VOS;
    
    
    // --- configure prescalers for internal buses
    
    
    uint32_t hpre_div    = RCC_CFGR_HPRE_DIV1;
    uint32_t ppre2_div   = 0; //RCC_CFGR_PPRE2_DIV2;
    uint32_t ppre1_div   = 0; //RCC_CFGR_PPRE1_DIV4;
    
    uint8_t factorfast = cpu_brdcfg.speeds.cpu / cpu_brdcfg.speeds.fastbus;   
    switch(factorfast)
    {   // APB2 is fast bus
        default:
        case 1: 
        {
            ppre2_div   = RCC_CFGR_PPRE2_DIV1;     
        } break;
        
        case 2: 
        {
            ppre2_div   = RCC_CFGR_PPRE2_DIV2;     
        } break; 
        
        case 4: 
        {
            ppre2_div   = RCC_CFGR_PPRE2_DIV4;       
        } break;  
        
        case 8: 
        {
            ppre2_div   = RCC_CFGR_PPRE2_DIV8;        
        } break;         
    }

    uint8_t factorslow = cpu_brdcfg.speeds.cpu / cpu_brdcfg.speeds.slowbus;    
    switch(factorslow)
    {   // APB2 is slow bus
        default:
        case 1: 
        {
            ppre1_div   = RCC_CFGR_PPRE1_DIV1;     
        } break;
        
        case 2: 
        {
            ppre1_div   = RCC_CFGR_PPRE1_DIV2;     
        } break; 
        
        case 4: 
        {
            ppre1_div   = RCC_CFGR_PPRE1_DIV4;       
        } break;  
        
        case 8: 
        {
            ppre1_div   = RCC_CFGR_PPRE1_DIV8;        
        } break;         
    }    
    
    // apply AHB (system) prescaler 
    RCC->CFGR |= hpre_div;      // RCC_CFGR_HPRE_DIV1;
    
    // apply APB2 (fast bus) prescaler  
    RCC->CFGR |= ppre2_div;     // RCC_CFGR_PPRE2_DIV2;
    
    // apply APB1 (slow bus) prescaler
    RCC->CFGR |= ppre1_div;     // RCC_CFGR_PPRE1_DIV4;
    
    
    // --- if required, configure the pll  
        
    if( (hl_cpu_refclock_pll_on_internal     == cpu_brdcfg.clockcfg.refclock) ||
        (hl_cpu_refclock_pll_on_external_xtl == cpu_brdcfg.clockcfg.refclock) ||
        (hl_cpu_refclock_pll_on_external_osc == cpu_brdcfg.clockcfg.refclock) )
    {
        uint32_t pll_m = cpu_brdcfg.clockcfg.pllcfg.m;
        uint32_t pll_n = cpu_brdcfg.clockcfg.pllcfg.n;
        uint32_t pll_p = cpu_brdcfg.clockcfg.pllcfg.p;
        uint32_t pll_q = cpu_brdcfg.clockcfg.pllcfg.q;

        // configure the main PLL 
        RCC->PLLCFGR =  pll_m | ( pll_n << 6) | ((( pll_p >> 1) -1) << 16) |
                       (RCC_PLLCFGR_PLLSRC_HSE) | ( pll_q << 24);

        // enable the main PLL 
        RCC->CR |= RCC_CR_PLLON;

        // wait till the main PLL is ready 
        while(0 == (RCC->CR & RCC_CR_PLLRDY))
        {
        }
    }
   
    // --- configure flash prefetch, instruction cache, data cache and wait state 
    
    FLASH->ACR = FLASH_ACR_ICEN | FLASH_ACR_DCEN | FLASH_ACR_LATENCY_5WS;
    
    
    // --- configure the system source clock
    
    uint32_t clockcfg = 0;
    uint32_t clocksta = 0;
    switch(cpu_brdcfg.clockcfg.refclock)
    {  
        default:
        case hl_cpu_refclock_internal:
        {
            clockcfg = RCC_CFGR_SW_HSI;  
            clocksta = RCC_CFGR_SWS_HSI;            
        } break;
               
        case hl_cpu_refclock_external_xtl:
        case hl_cpu_refclock_external_osc:
        {
            clockcfg = RCC_CFGR_SW_HSE;          
            clocksta = RCC_CFGR_SWS_HSE; 
        } break;                
                       
        case hl_cpu_refclock_pll_on_internal:
        case hl_cpu_refclock_pll_on_external_xtl:
        case hl_cpu_refclock_pll_on_external_osc:
        {
            clockcfg = RCC_CFGR_SW_PLL;             
            clocksta = RCC_CFGR_SWS_PLL;
        } break;
        
    }
    
    // select the system clock source 
    RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
    RCC->CFGR |= clockcfg;

    // wait till the chosen clock source is effective in clock status register
    while(clocksta != (RCC->CFGR & (uint32_t)RCC_CFGR_SWS ));
    {       
    }       
//     /* Select the main PLL as system clock source */
//     RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
//     RCC->CFGR |= RCC_CFGR_SW_PLL;

//     /* Wait till the main PLL is used as system clock source */
//     while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS ) != RCC_CFGR_SWS_PLL);
//     {
//     }
  
}

static void s_hl_cpu_system_core_clock_update(void)
{
    SystemCoreClockUpdate();
}
