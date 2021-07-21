# 1 "../src/driver/stm32g4-v120/src/stm32g4xx_hal_uart_ex.c"
# 1 "<built-in>" 1
# 1 "<built-in>" 3
# 373 "<built-in>" 3
# 1 "<command line>" 1
# 1 "<built-in>" 2
# 1 "../src/driver/stm32g4-v120/src/stm32g4xx_hal_uart_ex.c" 2
# 44 "../src/driver/stm32g4-v120/src/stm32g4xx_hal_uart_ex.c"
# 1 "../src/driver/stm32g4-v120/inc\\stm32g4xx_hal.h" 1
# 32 "../src/driver/stm32g4-v120/inc\\stm32g4xx_hal.h"
# 1 "../src/driver/stm32g4-v120/inc/../../../config/stm32hal_driver_cfg.h" 1
# 13 "../src/driver/stm32g4-v120/inc/../../../config/stm32hal_driver_cfg.h"
# 1 "../api\\stm32hal_define.h" 1
# 14 "../src/driver/stm32g4-v120/inc/../../../config/stm32hal_driver_cfg.h" 2
# 1 "../api\\stm32hal_driver.h" 1
# 212 "../api\\stm32hal_driver.h"
# 1 "../api/../src/driver/stm32g4-v120/inc/stm32g4xx_hal.h" 1
# 213 "../api\\stm32hal_driver.h" 2
# 15 "../src/driver/stm32g4-v120/inc/../../../config/stm32hal_driver_cfg.h" 2
# 132 "../src/driver/stm32g4-v120/inc/../../../config/stm32hal_driver_cfg.h"
# 1 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 1
# 244 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h"
# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc.h" 1
# 29 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_def.h" 1
# 30 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_def.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx.h" 1
# 121 "../src/driver/stm32g4-v120/inc/stm32g4xx.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g474xx.h" 1
# 66 "../src/driver/stm32g4-v120/inc/stm32g474xx.h"
typedef enum
{

  NonMaskableInt_IRQn = -14,
  HardFault_IRQn = -13,
  MemoryManagement_IRQn = -12,
  BusFault_IRQn = -11,
  UsageFault_IRQn = -10,
  SVCall_IRQn = -5,
  DebugMonitor_IRQn = -4,
  PendSV_IRQn = -2,
  SysTick_IRQn = -1,

  WWDG_IRQn = 0,
  PVD_PVM_IRQn = 1,
  RTC_TAMP_LSECSS_IRQn = 2,
  RTC_WKUP_IRQn = 3,
  FLASH_IRQn = 4,
  RCC_IRQn = 5,
  EXTI0_IRQn = 6,
  EXTI1_IRQn = 7,
  EXTI2_IRQn = 8,
  EXTI3_IRQn = 9,
  EXTI4_IRQn = 10,
  DMA1_Channel1_IRQn = 11,
  DMA1_Channel2_IRQn = 12,
  DMA1_Channel3_IRQn = 13,
  DMA1_Channel4_IRQn = 14,
  DMA1_Channel5_IRQn = 15,
  DMA1_Channel6_IRQn = 16,
  DMA1_Channel7_IRQn = 17,
  ADC1_2_IRQn = 18,
  USB_HP_IRQn = 19,
  USB_LP_IRQn = 20,
  FDCAN1_IT0_IRQn = 21,
  FDCAN1_IT1_IRQn = 22,
  EXTI9_5_IRQn = 23,
  TIM1_BRK_TIM15_IRQn = 24,
  TIM1_UP_TIM16_IRQn = 25,
  TIM1_TRG_COM_TIM17_IRQn = 26,
  TIM1_CC_IRQn = 27,
  TIM2_IRQn = 28,
  TIM3_IRQn = 29,
  TIM4_IRQn = 30,
  I2C1_EV_IRQn = 31,
  I2C1_ER_IRQn = 32,
  I2C2_EV_IRQn = 33,
  I2C2_ER_IRQn = 34,
  SPI1_IRQn = 35,
  SPI2_IRQn = 36,
  USART1_IRQn = 37,
  USART2_IRQn = 38,
  USART3_IRQn = 39,
  EXTI15_10_IRQn = 40,
  RTC_Alarm_IRQn = 41,
  USBWakeUp_IRQn = 42,
  TIM8_BRK_IRQn = 43,
  TIM8_UP_IRQn = 44,
  TIM8_TRG_COM_IRQn = 45,
  TIM8_CC_IRQn = 46,
  ADC3_IRQn = 47,
  FMC_IRQn = 48,
  LPTIM1_IRQn = 49,
  TIM5_IRQn = 50,
  SPI3_IRQn = 51,
  UART4_IRQn = 52,
  UART5_IRQn = 53,
  TIM6_DAC_IRQn = 54,
  TIM7_DAC_IRQn = 55,
  DMA2_Channel1_IRQn = 56,
  DMA2_Channel2_IRQn = 57,
  DMA2_Channel3_IRQn = 58,
  DMA2_Channel4_IRQn = 59,
  DMA2_Channel5_IRQn = 60,
  ADC4_IRQn = 61,
  ADC5_IRQn = 62,
  UCPD1_IRQn = 63,
  COMP1_2_3_IRQn = 64,
  COMP4_5_6_IRQn = 65,
  COMP7_IRQn = 66,
  HRTIM1_Master_IRQn = 67,
  HRTIM1_TIMA_IRQn = 68,
  HRTIM1_TIMB_IRQn = 69,
  HRTIM1_TIMC_IRQn = 70,
  HRTIM1_TIMD_IRQn = 71,
  HRTIM1_TIME_IRQn = 72,
  HRTIM1_FLT_IRQn = 73,
  HRTIM1_TIMF_IRQn = 74,
  CRS_IRQn = 75,
  SAI1_IRQn = 76,
  TIM20_BRK_IRQn = 77,
  TIM20_UP_IRQn = 78,
  TIM20_TRG_COM_IRQn = 79,
  TIM20_CC_IRQn = 80,
  FPU_IRQn = 81,
  I2C4_EV_IRQn = 82,
  I2C4_ER_IRQn = 83,
  SPI4_IRQn = 84,
  FDCAN2_IT0_IRQn = 86,
  FDCAN2_IT1_IRQn = 87,
  FDCAN3_IT0_IRQn = 88,
  FDCAN3_IT1_IRQn = 89,
  RNG_IRQn = 90,
  LPUART1_IRQn = 91,
  I2C3_EV_IRQn = 92,
  I2C3_ER_IRQn = 93,
  DMAMUX_OVR_IRQn = 94,
  QUADSPI_IRQn = 95,
  DMA1_Channel8_IRQn = 96,
  DMA2_Channel6_IRQn = 97,
  DMA2_Channel7_IRQn = 98,
  DMA2_Channel8_IRQn = 99,
  CORDIC_IRQn = 100,
  FMAC_IRQn = 101
} IRQn_Type;






# 1 "../src/driver/stm32g4-v120/inc/core_cm4.h" 1
# 29 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3





# 1 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\stdint.h" 1 3
# 56 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\stdint.h" 3
typedef signed char int8_t;
typedef signed short int int16_t;
typedef signed int int32_t;
typedef signed long long int int64_t;


typedef unsigned char uint8_t;
typedef unsigned short int uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long int uint64_t;





typedef signed char int_least8_t;
typedef signed short int int_least16_t;
typedef signed int int_least32_t;
typedef signed long long int int_least64_t;


typedef unsigned char uint_least8_t;
typedef unsigned short int uint_least16_t;
typedef unsigned int uint_least32_t;
typedef unsigned long long int uint_least64_t;




typedef signed int int_fast8_t;
typedef signed int int_fast16_t;
typedef signed int int_fast32_t;
typedef signed long long int int_fast64_t;


typedef unsigned int uint_fast8_t;
typedef unsigned int uint_fast16_t;
typedef unsigned int uint_fast32_t;
typedef unsigned long long int uint_fast64_t;






typedef signed int intptr_t;
typedef unsigned int uintptr_t;



typedef signed long long intmax_t;
typedef unsigned long long uintmax_t;
# 35 "../src/driver/stm32g4-v120/inc/core_cm4.h" 2 3
# 63 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
# 1 "../src/driver/stm32g4-v120/inc/cmsis_version.h" 1 3
# 29 "../src/driver/stm32g4-v120/inc/cmsis_version.h" 3
# 64 "../src/driver/stm32g4-v120/inc/core_cm4.h" 2 3
# 162 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
# 1 "../src/driver/stm32g4-v120/inc/cmsis_compiler.h" 1 3
# 47 "../src/driver/stm32g4-v120/inc/cmsis_compiler.h" 3
# 1 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 1 3
# 31 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3


# 1 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 1 3






# 1 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_acle.h" 1 3
# 39 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_acle.h" 3
static __inline__ void __attribute__((__always_inline__, __nodebug__)) __wfi(void) {
  __builtin_arm_wfi();
}



static __inline__ void __attribute__((__always_inline__, __nodebug__)) __wfe(void) {
  __builtin_arm_wfe();
}



static __inline__ void __attribute__((__always_inline__, __nodebug__)) __sev(void) {
  __builtin_arm_sev();
}



static __inline__ void __attribute__((__always_inline__, __nodebug__)) __sevl(void) {
  __builtin_arm_sevl();
}



static __inline__ void __attribute__((__always_inline__, __nodebug__)) __yield(void) {
  __builtin_arm_yield();
}







static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__swp(uint32_t __x, volatile uint32_t *__p) {
  uint32_t v;
  do
    v = __builtin_arm_ldrex(__p);
  while (__builtin_arm_strex(__x, __p));
  return v;
}
# 107 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_acle.h" 3
static __inline__ void __attribute__((__always_inline__, __nodebug__)) __nop(void) {
  __builtin_arm_nop();
}





static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__ror(uint32_t __x, uint32_t __y) {
  __y %= 32;
  if (__y == 0)
    return __x;
  return (__x >> __y) | (__x << (32 - __y));
}

static __inline__ uint64_t __attribute__((__always_inline__, __nodebug__))
__rorll(uint64_t __x, uint32_t __y) {
  __y %= 64;
  if (__y == 0)
    return __x;
  return (__x >> __y) | (__x << (64 - __y));
}

static __inline__ unsigned long __attribute__((__always_inline__, __nodebug__))
__rorl(unsigned long __x, uint32_t __y) {

  return __ror(__x, __y);



}



static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__clz(uint32_t __t) {
  return __builtin_clz(__t);
}

static __inline__ unsigned long __attribute__((__always_inline__, __nodebug__))
__clzl(unsigned long __t) {
  return __builtin_clzl(__t);
}

static __inline__ uint64_t __attribute__((__always_inline__, __nodebug__))
__clzll(uint64_t __t) {
  return __builtin_clzll(__t);
}


static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__cls(uint32_t __t) {
  return __builtin_arm_cls(__t);
}

static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__clsl(unsigned long __t) {

  return __builtin_arm_cls(__t);



}

static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__clsll(uint64_t __t) {
  return __builtin_arm_cls64(__t);
}


static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__rev(uint32_t __t) {
  return __builtin_bswap32(__t);
}

static __inline__ unsigned long __attribute__((__always_inline__, __nodebug__))
__revl(unsigned long __t) {

  return __builtin_bswap32(__t);



}

static __inline__ uint64_t __attribute__((__always_inline__, __nodebug__))
__revll(uint64_t __t) {
  return __builtin_bswap64(__t);
}


static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__rev16(uint32_t __t) {
  return __ror(__rev(__t), 16);
}

static __inline__ uint64_t __attribute__((__always_inline__, __nodebug__))
__rev16ll(uint64_t __t) {
  return (((uint64_t)__rev16(__t >> 32)) << 32) | __rev16(__t);
}

static __inline__ unsigned long __attribute__((__always_inline__, __nodebug__))
__rev16l(unsigned long __t) {

    return __rev16(__t);



}


static __inline__ int16_t __attribute__((__always_inline__, __nodebug__))
__revsh(int16_t __t) {
  return __builtin_bswap16(__t);
}


static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__rbit(uint32_t __t) {
  return __builtin_arm_rbit(__t);
}

static __inline__ uint64_t __attribute__((__always_inline__, __nodebug__))
__rbitll(uint64_t __t) {

  return (((uint64_t)__builtin_arm_rbit(__t)) << 32) |
         __builtin_arm_rbit(__t >> 32);



}

static __inline__ unsigned long __attribute__((__always_inline__, __nodebug__))
__rbitl(unsigned long __t) {

  return __rbit(__t);



}





static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smulbb(int32_t __a, int32_t __b) {
  return __builtin_arm_smulbb(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smulbt(int32_t __a, int32_t __b) {
  return __builtin_arm_smulbt(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smultb(int32_t __a, int32_t __b) {
  return __builtin_arm_smultb(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smultt(int32_t __a, int32_t __b) {
  return __builtin_arm_smultt(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smulwb(int32_t __a, int32_t __b) {
  return __builtin_arm_smulwb(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__,__nodebug__))
__smulwt(int32_t __a, int32_t __b) {
  return __builtin_arm_smulwt(__a, __b);
}
# 292 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_acle.h" 3
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__qadd(int32_t __t, int32_t __v) {
  return __builtin_arm_qadd(__t, __v);
}

static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__qsub(int32_t __t, int32_t __v) {
  return __builtin_arm_qsub(__t, __v);
}

static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__qdbl(int32_t __t) {
  return __builtin_arm_qadd(__t, __t);
}




static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlabb(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlabb(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlabt(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlabt(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlatb(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlatb(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlatt(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlatt(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlawb(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlawb(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlawt(int32_t __a, int32_t __b, int32_t __c) {
  return __builtin_arm_smlawt(__a, __b, __c);
}
# 345 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_acle.h" 3
typedef int32_t int8x4_t;
typedef int32_t int16x2_t;
typedef uint32_t uint8x4_t;
typedef uint32_t uint16x2_t;

static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__sxtab16(int16x2_t __a, int8x4_t __b) {
  return __builtin_arm_sxtab16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__sxtb16(int8x4_t __a) {
  return __builtin_arm_sxtb16(__a);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__uxtab16(int16x2_t __a, int8x4_t __b) {
  return __builtin_arm_uxtab16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__uxtb16(int8x4_t __a) {
  return __builtin_arm_uxtb16(__a);
}




static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__sel(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_sel(__a, __b);
}




static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__qadd8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_qadd8(__a, __b);
}
static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__qsub8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_qsub8(__a, __b);
}
static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__sadd8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_sadd8(__a, __b);
}
static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__shadd8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_shadd8(__a, __b);
}
static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__shsub8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_shsub8(__a, __b);
}
static __inline__ int8x4_t __attribute__((__always_inline__, __nodebug__))
__ssub8(int8x4_t __a, int8x4_t __b) {
  return __builtin_arm_ssub8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__uadd8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_uadd8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__uhadd8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_uhadd8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__uhsub8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_uhsub8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__uqadd8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_uqadd8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__uqsub8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_uqsub8(__a, __b);
}
static __inline__ uint8x4_t __attribute__((__always_inline__, __nodebug__))
__usub8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_usub8(__a, __b);
}




static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__usad8(uint8x4_t __a, uint8x4_t __b) {
  return __builtin_arm_usad8(__a, __b);
}
static __inline__ uint32_t __attribute__((__always_inline__, __nodebug__))
__usada8(uint8x4_t __a, uint8x4_t __b, uint32_t __c) {
  return __builtin_arm_usada8(__a, __b, __c);
}




static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__qadd16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_qadd16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__qasx(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_qasx(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__qsax(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_qsax(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__qsub16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_qsub16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__sadd16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_sadd16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__sasx(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_sasx(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__shadd16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_shadd16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__shasx(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_shasx(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__shsax(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_shsax(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__shsub16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_shsub16(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__ssax(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_ssax(__a, __b);
}
static __inline__ int16x2_t __attribute__((__always_inline__, __nodebug__))
__ssub16(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_ssub16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uadd16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uadd16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uasx(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uasx(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uhadd16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uhadd16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uhasx(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uhasx(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uhsax(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uhsax(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uhsub16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uhsub16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uqadd16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uqadd16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uqasx(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uqasx(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uqsax(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uqsax(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__uqsub16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_uqsub16(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__usax(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_usax(__a, __b);
}
static __inline__ uint16x2_t __attribute__((__always_inline__, __nodebug__))
__usub16(uint16x2_t __a, uint16x2_t __b) {
  return __builtin_arm_usub16(__a, __b);
}




static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlad(int16x2_t __a, int16x2_t __b, int32_t __c) {
  return __builtin_arm_smlad(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smladx(int16x2_t __a, int16x2_t __b, int32_t __c) {
  return __builtin_arm_smladx(__a, __b, __c);
}
static __inline__ int64_t __attribute__((__always_inline__, __nodebug__))
__smlald(int16x2_t __a, int16x2_t __b, int64_t __c) {
  return __builtin_arm_smlald(__a, __b, __c);
}
static __inline__ int64_t __attribute__((__always_inline__, __nodebug__))
__smlaldx(int16x2_t __a, int16x2_t __b, int64_t __c) {
  return __builtin_arm_smlaldx(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlsd(int16x2_t __a, int16x2_t __b, int32_t __c) {
  return __builtin_arm_smlsd(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smlsdx(int16x2_t __a, int16x2_t __b, int32_t __c) {
  return __builtin_arm_smlsdx(__a, __b, __c);
}
static __inline__ int64_t __attribute__((__always_inline__, __nodebug__))
__smlsld(int16x2_t __a, int16x2_t __b, int64_t __c) {
  return __builtin_arm_smlsld(__a, __b, __c);
}
static __inline__ int64_t __attribute__((__always_inline__, __nodebug__))
__smlsldx(int16x2_t __a, int16x2_t __b, int64_t __c) {
  return __builtin_arm_smlsldx(__a, __b, __c);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smuad(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_smuad(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smuadx(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_smuadx(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smusd(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_smusd(__a, __b);
}
static __inline__ int32_t __attribute__((__always_inline__, __nodebug__))
__smusdx(int16x2_t __a, int16x2_t __b) {
  return __builtin_arm_smusdx(__a, __b);
}
# 8 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 2 3
# 40 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
static __inline__ unsigned int __attribute__((__always_inline__, __nodebug__))
__disable_fiq(void) {
  unsigned int cpsr;

  __asm__ __volatile__("mrs %[cpsr], faultmask\n"
                       "cpsid f\n"
                       : [cpsr] "=r"(cpsr));
  return cpsr & 0x1;
# 62 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
}


static __inline__ unsigned int __attribute__((__always_inline__, __nodebug__))
__disable_irq(void) {
  unsigned int cpsr;


  __asm__ __volatile__("mrs %[cpsr], primask\n"
                       "cpsid i\n"
                       : [cpsr] "=r"(cpsr));
  return cpsr & 0x1;
# 89 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
}







static __inline__ void __attribute__((__always_inline__, __nodebug__))
__enable_fiq(void) {

  __asm__ __volatile__("cpsie f");
# 109 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
}


static __inline__ void __attribute__((__always_inline__, __nodebug__))
__enable_irq(void) {

  __asm__ __volatile__("cpsie i");
# 124 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
}

static __inline__ void __attribute__((__always_inline__, __nodebug__)) __force_stores(void) {
    __asm__ __volatile__ ("" : : : "memory", "cc");
}

static void __attribute__((__always_inline__, __nodebug__)) __memory_changed(void) {
    __asm__ __volatile__ ("" : : : "memory", "cc");
}

static void __attribute__((__always_inline__, __nodebug__)) __schedule_barrier(void) {
    __asm__ __volatile__ ("" : : : "memory", "cc");
}

static __inline__ int __attribute__((__always_inline__, __nodebug__))
__semihost(int val, const void *ptr) {
  register int v __asm__("r0") = val;
  register const void *p __asm__("r1") = ptr;
  __asm__ __volatile__(


      "bkpt 0xab"
# 160 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\arm_compat.h" 3
      : "+r"(v), "+r"(p)
      :
      : "memory", "cc");
  return v;
}


static __inline__ unsigned int __attribute__((__always_inline__, __nodebug__))
__vfp_status(unsigned int mask, unsigned int flags) {
  unsigned int fpscr;
  __asm__ __volatile__("vmrs %[fpscr], fpscr" : [fpscr] "=r"(fpscr));
  unsigned int set = mask & flags;
  unsigned int clear = mask & ~flags;
  unsigned int toggle = ~mask & flags;
  fpscr |= set;
  fpscr &= ~clear;
  fpscr ^= toggle;
  __asm__ __volatile__("vmsr fpscr, %[fpscr]" : : [fpscr] "r"(fpscr));
  return fpscr;
}
# 34 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 2 3
# 68 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

 struct __attribute__((packed)) T_UINT32 { uint32_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

 struct __attribute__((packed, aligned(1))) T_UINT16_WRITE { uint16_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

 struct __attribute__((packed, aligned(1))) T_UINT16_READ { uint16_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

 struct __attribute__((packed, aligned(1))) T_UINT32_WRITE { uint32_t v; };
#pragma clang diagnostic pop



#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpacked"

 struct __attribute__((packed, aligned(1))) T_UINT32_READ { uint32_t v; };
#pragma clang diagnostic pop
# 166 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_CONTROL(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, control" : "=r" (result) );
  return(result);
}
# 196 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_CONTROL(uint32_t control)
{
  __asm volatile ("MSR control, %0" : : "r" (control) : "memory");
}
# 220 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_IPSR(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, ipsr" : "=r" (result) );
  return(result);
}







__attribute__((always_inline)) static __inline uint32_t __get_APSR(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, apsr" : "=r" (result) );
  return(result);
}







__attribute__((always_inline)) static __inline uint32_t __get_xPSR(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, xpsr" : "=r" (result) );
  return(result);
}







__attribute__((always_inline)) static __inline uint32_t __get_PSP(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, psp" : "=r" (result) );
  return(result);
}
# 292 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_PSP(uint32_t topOfProcStack)
{
  __asm volatile ("MSR psp, %0" : : "r" (topOfProcStack) : );
}
# 316 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_MSP(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, msp" : "=r" (result) );
  return(result);
}
# 346 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_MSP(uint32_t topOfMainStack)
{
  __asm volatile ("MSR msp, %0" : : "r" (topOfMainStack) : );
}
# 397 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_PRIMASK(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, primask" : "=r" (result) );
  return(result);
}
# 427 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_PRIMASK(uint32_t priMask)
{
  __asm volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}
# 470 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __get_BASEPRI(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, basepri" : "=r" (result) );
  return(result);
}
# 500 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_BASEPRI(uint32_t basePri)
{
  __asm volatile ("MSR basepri, %0" : : "r" (basePri) : "memory");
}
# 525 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_BASEPRI_MAX(uint32_t basePri)
{
  __asm volatile ("MSR basepri_max, %0" : : "r" (basePri) : "memory");
}







__attribute__((always_inline)) static __inline uint32_t __get_FAULTMASK(void)
{
  uint32_t result;

  __asm volatile ("MRS %0, faultmask" : "=r" (result) );
  return(result);
}
# 566 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __set_FAULTMASK(uint32_t faultMask)
{
  __asm volatile ("MSR faultmask, %0" : : "r" (faultMask) : "memory");
}
# 902 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __ROR(uint32_t op1, uint32_t op2)
{
  op2 %= 32U;
  if (op2 == 0U)
  {
    return op1;
  }
  return (op1 >> op2) | (op1 << (32U - op2));
}
# 937 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint8_t __CLZ(uint32_t value)
{
# 948 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
  if (value == 0U)
  {
    return 32U;
  }
  return __builtin_clz(value);
}
# 1063 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __RRX(uint32_t value)
{
  uint32_t result;

  __asm volatile ("rrx %0, %1" : "=r" (result) : "r" (value) );
  return(result);
}
# 1078 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint8_t __LDRBT(volatile uint8_t *ptr)
{
  uint32_t result;

  __asm volatile ("ldrbt %0, %1" : "=r" (result) : "Q" (*ptr) );
  return ((uint8_t) result);
}
# 1093 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint16_t __LDRHT(volatile uint16_t *ptr)
{
  uint32_t result;

  __asm volatile ("ldrht %0, %1" : "=r" (result) : "Q" (*ptr) );
  return ((uint16_t) result);
}
# 1108 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline uint32_t __LDRT(volatile uint32_t *ptr)
{
  uint32_t result;

  __asm volatile ("ldrt %0, %1" : "=r" (result) : "Q" (*ptr) );
  return(result);
}
# 1123 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __STRBT(uint8_t value, volatile uint8_t *ptr)
{
  __asm volatile ("strbt %1, %0" : "=Q" (*ptr) : "r" ((uint32_t)value) );
}
# 1135 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __STRHT(uint16_t value, volatile uint16_t *ptr)
{
  __asm volatile ("strht %1, %0" : "=Q" (*ptr) : "r" ((uint32_t)value) );
}
# 1147 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline void __STRT(uint32_t value, volatile uint32_t *ptr)
{
  __asm volatile ("strt %1, %0" : "=Q" (*ptr) : "r" (value) );
}
# 1432 "../src/driver/stm32g4-v120/inc/cmsis_armclang.h" 3
__attribute__((always_inline)) static __inline int32_t __SMMLA (int32_t op1, int32_t op2, int32_t op3)
{
  int32_t result;

  __asm volatile ("smmla %0, %1, %2, %3" : "=r" (result): "r" (op1), "r" (op2), "r" (op3) );
  return(result);
}
# 48 "../src/driver/stm32g4-v120/inc/cmsis_compiler.h" 2 3
# 163 "../src/driver/stm32g4-v120/inc/core_cm4.h" 2 3
# 259 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef union
{
  struct
  {
    uint32_t _reserved0:16;
    uint32_t GE:4;
    uint32_t _reserved1:7;
    uint32_t Q:1;
    uint32_t V:1;
    uint32_t C:1;
    uint32_t Z:1;
    uint32_t N:1;
  } b;
  uint32_t w;
} APSR_Type;
# 298 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef union
{
  struct
  {
    uint32_t ISR:9;
    uint32_t _reserved0:23;
  } b;
  uint32_t w;
} IPSR_Type;
# 316 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef union
{
  struct
  {
    uint32_t ISR:9;
    uint32_t _reserved0:1;
    uint32_t ICI_IT_1:6;
    uint32_t GE:4;
    uint32_t _reserved1:4;
    uint32_t T:1;
    uint32_t ICI_IT_2:2;
    uint32_t Q:1;
    uint32_t V:1;
    uint32_t C:1;
    uint32_t Z:1;
    uint32_t N:1;
  } b;
  uint32_t w;
} xPSR_Type;
# 371 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef union
{
  struct
  {
    uint32_t nPRIV:1;
    uint32_t SPSEL:1;
    uint32_t FPCA:1;
    uint32_t _reserved0:29;
  } b;
  uint32_t w;
} CONTROL_Type;
# 406 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef struct
{
  volatile uint32_t ISER[8U];
        uint32_t RESERVED0[24U];
  volatile uint32_t ICER[8U];
        uint32_t RESERVED1[24U];
  volatile uint32_t ISPR[8U];
        uint32_t RESERVED2[24U];
  volatile uint32_t ICPR[8U];
        uint32_t RESERVED3[24U];
  volatile uint32_t IABR[8U];
        uint32_t RESERVED4[56U];
  volatile uint8_t IP[240U];
        uint32_t RESERVED5[644U];
  volatile uint32_t STIR;
} NVIC_Type;
# 440 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef struct
{
  volatile const uint32_t CPUID;
  volatile uint32_t ICSR;
  volatile uint32_t VTOR;
  volatile uint32_t AIRCR;
  volatile uint32_t SCR;
  volatile uint32_t CCR;
  volatile uint8_t SHP[12U];
  volatile uint32_t SHCSR;
  volatile uint32_t CFSR;
  volatile uint32_t HFSR;
  volatile uint32_t DFSR;
  volatile uint32_t MMFAR;
  volatile uint32_t BFAR;
  volatile uint32_t AFSR;
  volatile const uint32_t PFR[2U];
  volatile const uint32_t DFR;
  volatile const uint32_t ADR;
  volatile const uint32_t MMFR[4U];
  volatile const uint32_t ISAR[5U];
        uint32_t RESERVED0[5U];
  volatile uint32_t CPACR;
} SCB_Type;
# 719 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile const uint32_t ICTR;
  volatile uint32_t ACTLR;
} SCnSCB_Type;
# 759 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef struct
{
  volatile uint32_t CTRL;
  volatile uint32_t LOAD;
  volatile uint32_t VAL;
  volatile const uint32_t CALIB;
} SysTick_Type;
# 811 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef struct
{
  volatile union
  {
    volatile uint8_t u8;
    volatile uint16_t u16;
    volatile uint32_t u32;
  } PORT [32U];
        uint32_t RESERVED0[864U];
  volatile uint32_t TER;
        uint32_t RESERVED1[15U];
  volatile uint32_t TPR;
        uint32_t RESERVED2[15U];
  volatile uint32_t TCR;
        uint32_t RESERVED3[32U];
        uint32_t RESERVED4[43U];
  volatile uint32_t LAR;
  volatile const uint32_t LSR;
        uint32_t RESERVED5[6U];
  volatile const uint32_t PID4;
  volatile const uint32_t PID5;
  volatile const uint32_t PID6;
  volatile const uint32_t PID7;
  volatile const uint32_t PID0;
  volatile const uint32_t PID1;
  volatile const uint32_t PID2;
  volatile const uint32_t PID3;
  volatile const uint32_t CID0;
  volatile const uint32_t CID1;
  volatile const uint32_t CID2;
  volatile const uint32_t CID3;
} ITM_Type;
# 899 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef struct
{
  volatile uint32_t CTRL;
  volatile uint32_t CYCCNT;
  volatile uint32_t CPICNT;
  volatile uint32_t EXCCNT;
  volatile uint32_t SLEEPCNT;
  volatile uint32_t LSUCNT;
  volatile uint32_t FOLDCNT;
  volatile const uint32_t PCSR;
  volatile uint32_t COMP0;
  volatile uint32_t MASK0;
  volatile uint32_t FUNCTION0;
        uint32_t RESERVED0[1U];
  volatile uint32_t COMP1;
  volatile uint32_t MASK1;
  volatile uint32_t FUNCTION1;
        uint32_t RESERVED1[1U];
  volatile uint32_t COMP2;
  volatile uint32_t MASK2;
  volatile uint32_t FUNCTION2;
        uint32_t RESERVED2[1U];
  volatile uint32_t COMP3;
  volatile uint32_t MASK3;
  volatile uint32_t FUNCTION3;
} DWT_Type;
# 1046 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef struct
{
  volatile const uint32_t SSPSR;
  volatile uint32_t CSPSR;
        uint32_t RESERVED0[2U];
  volatile uint32_t ACPR;
        uint32_t RESERVED1[55U];
  volatile uint32_t SPPR;
        uint32_t RESERVED2[131U];
  volatile const uint32_t FFSR;
  volatile uint32_t FFCR;
  volatile const uint32_t FSCR;
        uint32_t RESERVED3[759U];
  volatile const uint32_t TRIGGER;
  volatile const uint32_t FIFO0;
  volatile const uint32_t ITATBCTR2;
        uint32_t RESERVED4[1U];
  volatile const uint32_t ITATBCTR0;
  volatile const uint32_t FIFO1;
  volatile uint32_t ITCTRL;
        uint32_t RESERVED5[39U];
  volatile uint32_t CLAIMSET;
  volatile uint32_t CLAIMCLR;
        uint32_t RESERVED7[8U];
  volatile const uint32_t DEVID;
  volatile const uint32_t DEVTYPE;
} TPI_Type;
# 1208 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef struct
{
  volatile const uint32_t TYPE;
  volatile uint32_t CTRL;
  volatile uint32_t RNR;
  volatile uint32_t RBAR;
  volatile uint32_t RASR;
  volatile uint32_t RBAR_A1;
  volatile uint32_t RASR_A1;
  volatile uint32_t RBAR_A2;
  volatile uint32_t RASR_A2;
  volatile uint32_t RBAR_A3;
  volatile uint32_t RASR_A3;
} MPU_Type;
# 1304 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef struct
{
        uint32_t RESERVED0[1U];
  volatile uint32_t FPCCR;
  volatile uint32_t FPCAR;
  volatile uint32_t FPDSCR;
  volatile const uint32_t MVFR0;
  volatile const uint32_t MVFR1;
  volatile const uint32_t MVFR2;
} FPU_Type;
# 1416 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
typedef struct
{
  volatile uint32_t DHCSR;
  volatile uint32_t DCRSR;
  volatile uint32_t DCRDR;
  volatile uint32_t DEMCR;
} CoreDebug_Type;
# 1648 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline void __NVIC_SetPriorityGrouping(uint32_t PriorityGroup)
{
  uint32_t reg_value;
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);

  reg_value = ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR;
  reg_value &= ~((uint32_t)((0xFFFFUL << 16U) | (7UL << 8U)));
  reg_value = (reg_value |
                ((uint32_t)0x5FAUL << 16U) |
                (PriorityGroupTmp << 8U) );
  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR = reg_value;
}







static __inline uint32_t __NVIC_GetPriorityGrouping(void)
{
  return ((uint32_t)((((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) >> 8U));
}
# 1679 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline void __NVIC_EnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    __asm volatile("":::"memory");
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __asm volatile("":::"memory");
  }
}
# 1698 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline uint32_t __NVIC_GetEnableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISER[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}
# 1717 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline void __NVIC_DisableIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICER[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
    __builtin_arm_dsb(0xF);
    __builtin_arm_isb(0xF);
  }
}
# 1736 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline uint32_t __NVIC_GetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}
# 1755 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline void __NVIC_SetPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ISPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}
# 1770 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline void __NVIC_ClearPendingIRQ(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->ICPR[(((uint32_t)IRQn) >> 5UL)] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  }
}
# 1787 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline uint32_t __NVIC_GetActive(IRQn_Type IRQn)
{
  if ((int32_t)(IRQn) >= 0)
  {
    return((uint32_t)(((((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IABR[(((uint32_t)IRQn) >> 5UL)] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
  }
  else
  {
    return(0U);
  }
}
# 1809 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority)
{
  if ((int32_t)(IRQn) >= 0)
  {
    ((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)] = (uint8_t)((priority << (8U - 4)) & (uint32_t)0xFFUL);
  }
  else
  {
    ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[(((uint32_t)IRQn) & 0xFUL)-4UL] = (uint8_t)((priority << (8U - 4)) & (uint32_t)0xFFUL);
  }
}
# 1831 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline uint32_t __NVIC_GetPriority(IRQn_Type IRQn)
{

  if ((int32_t)(IRQn) >= 0)
  {
    return(((uint32_t)((NVIC_Type *) ((0xE000E000UL) + 0x0100UL) )->IP[((uint32_t)IRQn)] >> (8U - 4)));
  }
  else
  {
    return(((uint32_t)((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHP[(((uint32_t)IRQn) & 0xFUL)-4UL] >> (8U - 4)));
  }
}
# 1856 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline uint32_t NVIC_EncodePriority (uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4)) ? (uint32_t)(4) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits = ((PriorityGroupTmp + (uint32_t)(4)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4));

  return (
           ((PreemptPriority & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL)) << SubPriorityBits) |
           ((SubPriority & (uint32_t)((1UL << (SubPriorityBits )) - 1UL)))
         );
}
# 1883 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline void NVIC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* const pPreemptPriority, uint32_t* const pSubPriority)
{
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(4)) ? (uint32_t)(4) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits = ((PriorityGroupTmp + (uint32_t)(4)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(4));

  *pPreemptPriority = (Priority >> SubPriorityBits) & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL);
  *pSubPriority = (Priority ) & (uint32_t)((1UL << (SubPriorityBits )) - 1UL);
}
# 1906 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline void __NVIC_SetVector(IRQn_Type IRQn, uint32_t vector)
{
  uint32_t vectors = (uint32_t )((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  (* (int *) (vectors + ((int32_t)IRQn + 16) * 4)) = vector;

}
# 1922 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline uint32_t __NVIC_GetVector(IRQn_Type IRQn)
{
  uint32_t vectors = (uint32_t )((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->VTOR;
  return (uint32_t)(* (int *) (vectors + ((int32_t)IRQn + 16) * 4));
}






__attribute__((__noreturn__)) static __inline void __NVIC_SystemReset(void)
{
  __builtin_arm_dsb(0xF);

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR = (uint32_t)((0x5FAUL << 16U) |
                           (((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->AIRCR & (7UL << 8U)) |
                            (1UL << 2U) );
  __builtin_arm_dsb(0xF);

  for(;;)
  {
    __builtin_arm_nop();
  }
}
# 1955 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
# 1 "../src/driver/stm32g4-v120/inc/mpu_armv7.h" 1 3
# 29 "../src/driver/stm32g4-v120/inc/mpu_armv7.h" 3
# 183 "../src/driver/stm32g4-v120/inc/mpu_armv7.h" 3
typedef struct {
  uint32_t RBAR;
  uint32_t RASR;
} ARM_MPU_Region_t;




static __inline void ARM_MPU_Enable(uint32_t MPU_Control)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL = MPU_Control | (1UL );

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR |= (1UL << 16U);

  __builtin_arm_dsb(0xF);
  __builtin_arm_isb(0xF);
}



static __inline void ARM_MPU_Disable(void)
{
  __builtin_arm_dmb(0xF);

  ((SCB_Type *) ((0xE000E000UL) + 0x0D00UL) )->SHCSR &= ~(1UL << 16U);

  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->CTRL &= ~(1UL );
}




static __inline void ARM_MPU_ClrRegion(uint32_t rnr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = 0U;
}





static __inline void ARM_MPU_SetRegion(uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}






static __inline void ARM_MPU_SetRegionEx(uint32_t rnr, uint32_t rbar, uint32_t rasr)
{
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RNR = rnr;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR = rbar;
  ((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RASR = rasr;
}






static __inline void ARM_MPU_OrderedMemcpy(volatile uint32_t* dst, const uint32_t* __restrict src, uint32_t len)
{
  uint32_t i;
  for (i = 0U; i < len; ++i)
  {
    dst[i] = src[i];
  }
}





static __inline void ARM_MPU_Load(ARM_MPU_Region_t const* table, uint32_t cnt)
{
  const uint32_t rowWordSize = sizeof(ARM_MPU_Region_t)/4U;
  while (cnt > 4U) {
    ARM_MPU_OrderedMemcpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), 4U*rowWordSize);
    table += 4U;
    cnt -= 4U;
  }
  ARM_MPU_OrderedMemcpy(&(((MPU_Type *) ((0xE000E000UL) + 0x0D90UL) )->RBAR), &(table->RBAR), cnt*rowWordSize);
}
# 1956 "../src/driver/stm32g4-v120/inc/core_cm4.h" 2 3
# 1976 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline uint32_t SCB_GetFPUType(void)
{
  uint32_t mvfr0;

  mvfr0 = ((FPU_Type *) ((0xE000E000UL) + 0x0F30UL) )->MVFR0;
  if ((mvfr0 & ((0xFUL << 4U) | (0xFUL << 8U))) == 0x020U)
  {
    return 1U;
  }
  else
  {
    return 0U;
  }
}
# 2017 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline uint32_t SysTick_Config(uint32_t ticks)
{
  if ((ticks - 1UL) > (0xFFFFFFUL ))
  {
    return (1UL);
  }

  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->LOAD = (uint32_t)(ticks - 1UL);
  __NVIC_SetPriority (SysTick_IRQn, (1UL << 4) - 1UL);
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->VAL = 0UL;
  ((SysTick_Type *) ((0xE000E000UL) + 0x0010UL) )->CTRL = (1UL << 2U) |
                   (1UL << 1U) |
                   (1UL );
  return (0UL);
}
# 2047 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
extern volatile int32_t ITM_RxBuffer;
# 2059 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline uint32_t ITM_SendChar (uint32_t ch)
{
  if (((((ITM_Type *) (0xE0000000UL) )->TCR & (1UL )) != 0UL) &&
      ((((ITM_Type *) (0xE0000000UL) )->TER & 1UL ) != 0UL) )
  {
    while (((ITM_Type *) (0xE0000000UL) )->PORT[0U].u32 == 0UL)
    {
      __builtin_arm_nop();
    }
    ((ITM_Type *) (0xE0000000UL) )->PORT[0U].u8 = (uint8_t)ch;
  }
  return (ch);
}
# 2080 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline int32_t ITM_ReceiveChar (void)
{
  int32_t ch = -1;

  if (ITM_RxBuffer != ((int32_t)0x5AA55AA5U))
  {
    ch = ITM_RxBuffer;
    ITM_RxBuffer = ((int32_t)0x5AA55AA5U);
  }

  return (ch);
}
# 2100 "../src/driver/stm32g4-v120/inc/core_cm4.h" 3
static __inline int32_t ITM_CheckChar (void)
{

  if (ITM_RxBuffer == ((int32_t)0x5AA55AA5U))
  {
    return (0);
  }
  else
  {
    return (1);
  }
}
# 187 "../src/driver/stm32g4-v120/inc/stm32g474xx.h" 2
# 1 "../src/driver/stm32g4-v120/inc/system_stm32g4xx.h" 1
# 58 "../src/driver/stm32g4-v120/inc/system_stm32g4xx.h"
extern uint32_t SystemCoreClock;

extern const uint8_t AHBPrescTable[16];
extern const uint8_t APBPrescTable[8];
# 87 "../src/driver/stm32g4-v120/inc/system_stm32g4xx.h"
extern void SystemInit(void);
extern void SystemCoreClockUpdate(void);
# 188 "../src/driver/stm32g4-v120/inc/stm32g474xx.h" 2
# 198 "../src/driver/stm32g4-v120/inc/stm32g474xx.h"
typedef struct
{
  volatile uint32_t ISR;
  volatile uint32_t IER;
  volatile uint32_t CR;
  volatile uint32_t CFGR;
  volatile uint32_t CFGR2;
  volatile uint32_t SMPR1;
  volatile uint32_t SMPR2;
       uint32_t RESERVED1;
  volatile uint32_t TR1;
  volatile uint32_t TR2;
  volatile uint32_t TR3;
       uint32_t RESERVED2;
  volatile uint32_t SQR1;
  volatile uint32_t SQR2;
  volatile uint32_t SQR3;
  volatile uint32_t SQR4;
  volatile uint32_t DR;
       uint32_t RESERVED3;
       uint32_t RESERVED4;
  volatile uint32_t JSQR;
       uint32_t RESERVED5[4];
  volatile uint32_t OFR1;
  volatile uint32_t OFR2;
  volatile uint32_t OFR3;
  volatile uint32_t OFR4;
       uint32_t RESERVED6[4];
  volatile uint32_t JDR1;
  volatile uint32_t JDR2;
  volatile uint32_t JDR3;
  volatile uint32_t JDR4;
       uint32_t RESERVED7[4];
  volatile uint32_t AWD2CR;
  volatile uint32_t AWD3CR;
       uint32_t RESERVED8;
       uint32_t RESERVED9;
  volatile uint32_t DIFSEL;
  volatile uint32_t CALFACT;
       uint32_t RESERVED10[2];
  volatile uint32_t GCOMP;
} ADC_TypeDef;

typedef struct
{
  volatile uint32_t CSR;
  uint32_t RESERVED1;
  volatile uint32_t CCR;
  volatile uint32_t CDR;
} ADC_Common_TypeDef;





typedef struct
{
  volatile uint32_t CREL;
  volatile uint32_t ENDN;
       uint32_t RESERVED1;
  volatile uint32_t DBTP;
  volatile uint32_t TEST;
  volatile uint32_t RWD;
  volatile uint32_t CCCR;
  volatile uint32_t NBTP;
  volatile uint32_t TSCC;
  volatile uint32_t TSCV;
  volatile uint32_t TOCC;
  volatile uint32_t TOCV;
       uint32_t RESERVED2[4];
  volatile uint32_t ECR;
  volatile uint32_t PSR;
  volatile uint32_t TDCR;
       uint32_t RESERVED3;
  volatile uint32_t IR;
  volatile uint32_t IE;
  volatile uint32_t ILS;
  volatile uint32_t ILE;
       uint32_t RESERVED4[8];
  volatile uint32_t RXGFC;
  volatile uint32_t XIDAM;
  volatile uint32_t HPMS;
       uint32_t RESERVED5;
  volatile uint32_t RXF0S;
  volatile uint32_t RXF0A;
  volatile uint32_t RXF1S;
  volatile uint32_t RXF1A;
       uint32_t RESERVED6[8];
  volatile uint32_t TXBC;
  volatile uint32_t TXFQS;
  volatile uint32_t TXBRP;
  volatile uint32_t TXBAR;
  volatile uint32_t TXBCR;
  volatile uint32_t TXBTO;
  volatile uint32_t TXBCF;
  volatile uint32_t TXBTIE;
  volatile uint32_t TXBCIE;
  volatile uint32_t TXEFS;
  volatile uint32_t TXEFA;
} FDCAN_GlobalTypeDef;





typedef struct
{
  volatile uint32_t CKDIV;
} FDCAN_Config_TypeDef;





typedef struct
{
  volatile uint32_t CSR;
} COMP_TypeDef;





typedef struct
{
  volatile uint32_t DR;
  volatile uint32_t IDR;
  volatile uint32_t CR;
  uint32_t RESERVED0;
  volatile uint32_t INIT;
  volatile uint32_t POL;
} CRC_TypeDef;




typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t CFGR;
  volatile uint32_t ISR;
  volatile uint32_t ICR;
} CRS_TypeDef;





typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t SWTRIGR;
  volatile uint32_t DHR12R1;
  volatile uint32_t DHR12L1;
  volatile uint32_t DHR8R1;
  volatile uint32_t DHR12R2;
  volatile uint32_t DHR12L2;
  volatile uint32_t DHR8R2;
  volatile uint32_t DHR12RD;
  volatile uint32_t DHR12LD;
  volatile uint32_t DHR8RD;
  volatile uint32_t DOR1;
  volatile uint32_t DOR2;
  volatile uint32_t SR;
  volatile uint32_t CCR;
  volatile uint32_t MCR;
  volatile uint32_t SHSR1;
  volatile uint32_t SHSR2;
  volatile uint32_t SHHR;
  volatile uint32_t SHRR;
  volatile uint32_t RESERVED[2];
  volatile uint32_t STR1;
  volatile uint32_t STR2;
  volatile uint32_t STMODR;
} DAC_TypeDef;





typedef struct
{
  volatile uint32_t IDCODE;
  volatile uint32_t CR;
  volatile uint32_t APB1FZR1;
  volatile uint32_t APB1FZR2;
  volatile uint32_t APB2FZ;
} DBGMCU_TypeDef;





typedef struct
{
  volatile uint32_t CCR;
  volatile uint32_t CNDTR;
  volatile uint32_t CPAR;
  volatile uint32_t CMAR;
} DMA_Channel_TypeDef;

typedef struct
{
  volatile uint32_t ISR;
  volatile uint32_t IFCR;
} DMA_TypeDef;





typedef struct
{
  volatile uint32_t CCR;
}DMAMUX_Channel_TypeDef;

typedef struct
{
  volatile uint32_t CSR;
  volatile uint32_t CFR;
}DMAMUX_ChannelStatus_TypeDef;

typedef struct
{
  volatile uint32_t RGCR;
}DMAMUX_RequestGen_TypeDef;

typedef struct
{
  volatile uint32_t RGSR;
  volatile uint32_t RGCFR;
}DMAMUX_RequestGenStatus_TypeDef;





typedef struct
{
  volatile uint32_t IMR1;
  volatile uint32_t EMR1;
  volatile uint32_t RTSR1;
  volatile uint32_t FTSR1;
  volatile uint32_t SWIER1;
  volatile uint32_t PR1;
  uint32_t RESERVED1;
  uint32_t RESERVED2;
  volatile uint32_t IMR2;
  volatile uint32_t EMR2;
  volatile uint32_t RTSR2;
  volatile uint32_t FTSR2;
  volatile uint32_t SWIER2;
  volatile uint32_t PR2;
} EXTI_TypeDef;





typedef struct
{
  volatile uint32_t ACR;
  volatile uint32_t PDKEYR;
  volatile uint32_t KEYR;
  volatile uint32_t OPTKEYR;
  volatile uint32_t SR;
  volatile uint32_t CR;
  volatile uint32_t ECCR;
       uint32_t RESERVED1;
  volatile uint32_t OPTR;
  volatile uint32_t PCROP1SR;
  volatile uint32_t PCROP1ER;
  volatile uint32_t WRP1AR;
  volatile uint32_t WRP1BR;
       uint32_t RESERVED2[4];
  volatile uint32_t PCROP2SR;
  volatile uint32_t PCROP2ER;
  volatile uint32_t WRP2AR;
  volatile uint32_t WRP2BR;
       uint32_t RESERVED3[7];
  volatile uint32_t SEC1R;
  volatile uint32_t SEC2R;
} FLASH_TypeDef;




typedef struct
{
  volatile uint32_t X1BUFCFG;
  volatile uint32_t X2BUFCFG;
  volatile uint32_t YBUFCFG;
  volatile uint32_t PARAM;
  volatile uint32_t CR;
  volatile uint32_t SR;
  volatile uint32_t WDATA;
  volatile uint32_t RDATA;
} FMAC_TypeDef;





typedef struct
{
  volatile uint32_t BTCR[8];
  volatile uint32_t PCSCNTR;
} FMC_Bank1_TypeDef;





typedef struct
{
  volatile uint32_t BWTR[7];
} FMC_Bank1E_TypeDef;





typedef struct
{
  volatile uint32_t PCR;
  volatile uint32_t SR;
  volatile uint32_t PMEM;
  volatile uint32_t PATT;
  uint32_t RESERVED0;
  volatile uint32_t ECCR;
} FMC_Bank3_TypeDef;





typedef struct
{
  volatile uint32_t MODER;
  volatile uint32_t OTYPER;
  volatile uint32_t OSPEEDR;
  volatile uint32_t PUPDR;
  volatile uint32_t IDR;
  volatile uint32_t ODR;
  volatile uint32_t BSRR;
  volatile uint32_t LCKR;
  volatile uint32_t AFR[2];
  volatile uint32_t BRR;
} GPIO_TypeDef;





typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t OAR1;
  volatile uint32_t OAR2;
  volatile uint32_t TIMINGR;
  volatile uint32_t TIMEOUTR;
  volatile uint32_t ISR;
  volatile uint32_t ICR;
  volatile uint32_t PECR;
  volatile uint32_t RXDR;
  volatile uint32_t TXDR;
} I2C_TypeDef;





typedef struct
{
  volatile uint32_t KR;
  volatile uint32_t PR;
  volatile uint32_t RLR;
  volatile uint32_t SR;
  volatile uint32_t WINR;
} IWDG_TypeDef;





typedef struct
{
  volatile uint32_t ISR;
  volatile uint32_t ICR;
  volatile uint32_t IER;
  volatile uint32_t CFGR;
  volatile uint32_t CR;
  volatile uint32_t CMP;
  volatile uint32_t ARR;
  volatile uint32_t CNT;
  volatile uint32_t OR;
} LPTIM_TypeDef;





typedef struct
{
  volatile uint32_t CSR;
  volatile uint32_t RESERVED[5];
  volatile uint32_t TCMR;
} OPAMP_TypeDef;





typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t CR3;
  volatile uint32_t CR4;
  volatile uint32_t SR1;
  volatile uint32_t SR2;
  volatile uint32_t SCR;
  uint32_t RESERVED;
  volatile uint32_t PUCRA;
  volatile uint32_t PDCRA;
  volatile uint32_t PUCRB;
  volatile uint32_t PDCRB;
  volatile uint32_t PUCRC;
  volatile uint32_t PDCRC;
  volatile uint32_t PUCRD;
  volatile uint32_t PDCRD;
  volatile uint32_t PUCRE;
  volatile uint32_t PDCRE;
  volatile uint32_t PUCRF;
  volatile uint32_t PDCRF;
  volatile uint32_t PUCRG;
  volatile uint32_t PDCRG;
  uint32_t RESERVED1[10];
  volatile uint32_t CR5;
} PWR_TypeDef;





typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t DCR;
  volatile uint32_t SR;
  volatile uint32_t FCR;
  volatile uint32_t DLR;
  volatile uint32_t CCR;
  volatile uint32_t AR;
  volatile uint32_t ABR;
  volatile uint32_t DR;
  volatile uint32_t PSMKR;
  volatile uint32_t PSMAR;
  volatile uint32_t PIR;
  volatile uint32_t LPTR;
} QUADSPI_TypeDef;





typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t ICSCR;
  volatile uint32_t CFGR;
  volatile uint32_t PLLCFGR;
  uint32_t RESERVED0;
  uint32_t RESERVED1;
  volatile uint32_t CIER;
  volatile uint32_t CIFR;
  volatile uint32_t CICR;
  uint32_t RESERVED2;
  volatile uint32_t AHB1RSTR;
  volatile uint32_t AHB2RSTR;
  volatile uint32_t AHB3RSTR;
  uint32_t RESERVED3;
  volatile uint32_t APB1RSTR1;
  volatile uint32_t APB1RSTR2;
  volatile uint32_t APB2RSTR;
  uint32_t RESERVED4;
  volatile uint32_t AHB1ENR;
  volatile uint32_t AHB2ENR;
  volatile uint32_t AHB3ENR;
  uint32_t RESERVED5;
  volatile uint32_t APB1ENR1;
  volatile uint32_t APB1ENR2;
  volatile uint32_t APB2ENR;
  uint32_t RESERVED6;
  volatile uint32_t AHB1SMENR;
  volatile uint32_t AHB2SMENR;
  volatile uint32_t AHB3SMENR;
  uint32_t RESERVED7;
  volatile uint32_t APB1SMENR1;
  volatile uint32_t APB1SMENR2;
  volatile uint32_t APB2SMENR;
  uint32_t RESERVED8;
  volatile uint32_t CCIPR;
  uint32_t RESERVED9;
  volatile uint32_t BDCR;
  volatile uint32_t CSR;
  volatile uint32_t CRRCR;
  volatile uint32_t CCIPR2;
} RCC_TypeDef;
# 722 "../src/driver/stm32g4-v120/inc/stm32g474xx.h"
typedef struct
{
  volatile uint32_t TR;
  volatile uint32_t DR;
  volatile uint32_t SSR;
  volatile uint32_t ICSR;
  volatile uint32_t PRER;
  volatile uint32_t WUTR;
  volatile uint32_t CR;
       uint32_t RESERVED0;
       uint32_t RESERVED1;
  volatile uint32_t WPR;
  volatile uint32_t CALR;
  volatile uint32_t SHIFTR;
  volatile uint32_t TSTR;
  volatile uint32_t TSDR;
  volatile uint32_t TSSSR;
       uint32_t RESERVED2;
  volatile uint32_t ALRMAR;
  volatile uint32_t ALRMASSR;
  volatile uint32_t ALRMBR;
  volatile uint32_t ALRMBSSR;
  volatile uint32_t SR;
  volatile uint32_t MISR;
       uint32_t RESERVED3;
  volatile uint32_t SCR;
} RTC_TypeDef;





typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
       uint32_t RESERVED0;
  volatile uint32_t FLTCR;
       uint32_t RESERVED1[6];
       uint32_t RESERVED2;
  volatile uint32_t IER;
  volatile uint32_t SR;
  volatile uint32_t MISR;
       uint32_t RESERVED3;
  volatile uint32_t SCR;
       uint32_t RESERVED4[48];
  volatile uint32_t BKP0R;
  volatile uint32_t BKP1R;
  volatile uint32_t BKP2R;
  volatile uint32_t BKP3R;
  volatile uint32_t BKP4R;
  volatile uint32_t BKP5R;
  volatile uint32_t BKP6R;
  volatile uint32_t BKP7R;
  volatile uint32_t BKP8R;
  volatile uint32_t BKP9R;
  volatile uint32_t BKP10R;
  volatile uint32_t BKP11R;
  volatile uint32_t BKP12R;
  volatile uint32_t BKP13R;
  volatile uint32_t BKP14R;
  volatile uint32_t BKP15R;
  volatile uint32_t BKP16R;
  volatile uint32_t BKP17R;
  volatile uint32_t BKP18R;
  volatile uint32_t BKP19R;
  volatile uint32_t BKP20R;
  volatile uint32_t BKP21R;
  volatile uint32_t BKP22R;
  volatile uint32_t BKP23R;
  volatile uint32_t BKP24R;
  volatile uint32_t BKP25R;
  volatile uint32_t BKP26R;
  volatile uint32_t BKP27R;
  volatile uint32_t BKP28R;
  volatile uint32_t BKP29R;
  volatile uint32_t BKP30R;
  volatile uint32_t BKP31R;
} TAMP_TypeDef;





typedef struct
{
  volatile uint32_t GCR;
  uint32_t RESERVED[16];
  volatile uint32_t PDMCR;
  volatile uint32_t PDMDLY;
} SAI_TypeDef;

typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t FRCR;
  volatile uint32_t SLOTR;
  volatile uint32_t IMR;
  volatile uint32_t SR;
  volatile uint32_t CLRFR;
  volatile uint32_t DR;
} SAI_Block_TypeDef;





typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t SR;
  volatile uint32_t DR;
  volatile uint32_t CRCPR;
  volatile uint32_t RXCRCR;
  volatile uint32_t TXCRCR;
  volatile uint32_t I2SCFGR;
  volatile uint32_t I2SPR;
} SPI_TypeDef;





typedef struct
{
  volatile uint32_t MEMRMP;
  volatile uint32_t CFGR1;
  volatile uint32_t EXTICR[4];
  volatile uint32_t SCSR;
  volatile uint32_t CFGR2;
  volatile uint32_t SWPR;
  volatile uint32_t SKR;
} SYSCFG_TypeDef;





typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t SMCR;
  volatile uint32_t DIER;
  volatile uint32_t SR;
  volatile uint32_t EGR;
  volatile uint32_t CCMR1;
  volatile uint32_t CCMR2;
  volatile uint32_t CCER;
  volatile uint32_t CNT;
  volatile uint32_t PSC;
  volatile uint32_t ARR;
  volatile uint32_t RCR;
  volatile uint32_t CCR1;
  volatile uint32_t CCR2;
  volatile uint32_t CCR3;
  volatile uint32_t CCR4;
  volatile uint32_t BDTR;
  volatile uint32_t CCR5;
  volatile uint32_t CCR6;
  volatile uint32_t CCMR3;
  volatile uint32_t DTR2;
  volatile uint32_t ECR;
  volatile uint32_t TISEL;
  volatile uint32_t AF1;
  volatile uint32_t AF2;
  volatile uint32_t OR ;
       uint32_t RESERVED0[220];
  volatile uint32_t DCR;
  volatile uint32_t DMAR;
} TIM_TypeDef;




typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t CR3;
  volatile uint32_t BRR;
  volatile uint32_t GTPR;
  volatile uint32_t RTOR;
  volatile uint32_t RQR;
  volatile uint32_t ISR;
  volatile uint32_t ICR;
  volatile uint32_t RDR;
  volatile uint32_t TDR;
  volatile uint32_t PRESC;
} USART_TypeDef;





typedef struct
{
  volatile uint16_t EP0R;
  volatile uint16_t RESERVED0;
  volatile uint16_t EP1R;
  volatile uint16_t RESERVED1;
  volatile uint16_t EP2R;
  volatile uint16_t RESERVED2;
  volatile uint16_t EP3R;
  volatile uint16_t RESERVED3;
  volatile uint16_t EP4R;
  volatile uint16_t RESERVED4;
  volatile uint16_t EP5R;
  volatile uint16_t RESERVED5;
  volatile uint16_t EP6R;
  volatile uint16_t RESERVED6;
  volatile uint16_t EP7R;
  volatile uint16_t RESERVED7[17];
  volatile uint16_t CNTR;
  volatile uint16_t RESERVED8;
  volatile uint16_t ISTR;
  volatile uint16_t RESERVED9;
  volatile uint16_t FNR;
  volatile uint16_t RESERVEDA;
  volatile uint16_t DADDR;
  volatile uint16_t RESERVEDB;
  volatile uint16_t BTABLE;
  volatile uint16_t RESERVEDC;
  volatile uint16_t LPMCSR;
  volatile uint16_t RESERVEDD;
  volatile uint16_t BCDR;
  volatile uint16_t RESERVEDE;
} USB_TypeDef;





typedef struct
{
  volatile uint32_t CSR;
  volatile uint32_t CCR;
} VREFBUF_TypeDef;





typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t CFR;
  volatile uint32_t SR;
} WWDG_TypeDef;





typedef struct
{
  volatile uint32_t CR;
  volatile uint32_t SR;
  volatile uint32_t DR;
} RNG_TypeDef;





typedef struct
{
  volatile uint32_t CSR;
  volatile uint32_t WDATA;
  volatile uint32_t RDATA;
} CORDIC_TypeDef;





typedef struct
{
  volatile uint32_t CFG1;
  volatile uint32_t CFG2;
  volatile uint32_t RESERVED0;
  volatile uint32_t CR;
  volatile uint32_t IMR;
  volatile uint32_t SR;
  volatile uint32_t ICR;
  volatile uint32_t TX_ORDSET;
  volatile uint32_t TX_PAYSZ;
  volatile uint32_t TXDR;
  volatile uint32_t RX_ORDSET;
  volatile uint32_t RX_PAYSZ;
  volatile uint32_t RXDR;
  volatile uint32_t RX_ORDEXT1;
  volatile uint32_t RX_ORDEXT2;
} UCPD_TypeDef;
# 1026 "../src/driver/stm32g4-v120/inc/stm32g474xx.h"
typedef struct
{
  volatile uint32_t MCR;
  volatile uint32_t MISR;
  volatile uint32_t MICR;
  volatile uint32_t MDIER;
  volatile uint32_t MCNTR;
  volatile uint32_t MPER;
  volatile uint32_t MREP;
  volatile uint32_t MCMP1R;
  uint32_t RESERVED0;
  volatile uint32_t MCMP2R;
  volatile uint32_t MCMP3R;
  volatile uint32_t MCMP4R;
  uint32_t RESERVED1[20];
}HRTIM_Master_TypeDef;


typedef struct
{
  volatile uint32_t TIMxCR;
  volatile uint32_t TIMxISR;
  volatile uint32_t TIMxICR;
  volatile uint32_t TIMxDIER;
  volatile uint32_t CNTxR;
  volatile uint32_t PERxR;
  volatile uint32_t REPxR;
  volatile uint32_t CMP1xR;
  volatile uint32_t CMP1CxR;
  volatile uint32_t CMP2xR;
  volatile uint32_t CMP3xR;
  volatile uint32_t CMP4xR;
  volatile uint32_t CPT1xR;
  volatile uint32_t CPT2xR;
  volatile uint32_t DTxR;
  volatile uint32_t SETx1R;
  volatile uint32_t RSTx1R;
  volatile uint32_t SETx2R;
  volatile uint32_t RSTx2R;
  volatile uint32_t EEFxR1;
  volatile uint32_t EEFxR2;
  volatile uint32_t RSTxR;
  volatile uint32_t CHPxR;
  volatile uint32_t CPT1xCR;
  volatile uint32_t CPT2xCR;
  volatile uint32_t OUTxR;
  volatile uint32_t FLTxR;
  volatile uint32_t TIMxCR2;
  volatile uint32_t EEFxR3;
  uint32_t RESERVED0[3];
}HRTIM_Timerx_TypeDef;


typedef struct
{
  volatile uint32_t CR1;
  volatile uint32_t CR2;
  volatile uint32_t ISR;
  volatile uint32_t ICR;
  volatile uint32_t IER;
  volatile uint32_t OENR;
  volatile uint32_t ODISR;
  volatile uint32_t ODSR;
  volatile uint32_t BMCR;
  volatile uint32_t BMTRGR;
  volatile uint32_t BMCMPR;
  volatile uint32_t BMPER;
  volatile uint32_t EECR1;
  volatile uint32_t EECR2;
  volatile uint32_t EECR3;
  volatile uint32_t ADC1R;
  volatile uint32_t ADC2R;
  volatile uint32_t ADC3R;
  volatile uint32_t ADC4R;
  volatile uint32_t DLLCR;
  volatile uint32_t FLTINR1;
  volatile uint32_t FLTINR2;
  volatile uint32_t BDMUPR;
  volatile uint32_t BDTAUPR;
  volatile uint32_t BDTBUPR;
  volatile uint32_t BDTCUPR;
  volatile uint32_t BDTDUPR;
  volatile uint32_t BDTEUPR;
  volatile uint32_t BDMADR;
  volatile uint32_t BDTFUPR;
  volatile uint32_t ADCER;
  volatile uint32_t ADCUR;
  volatile uint32_t ADCPS1;
  volatile uint32_t ADCPS2;
  volatile uint32_t FLTINR3;
  volatile uint32_t FLTINR4;
}HRTIM_Common_TypeDef;


typedef struct {
  HRTIM_Master_TypeDef sMasterRegs;
  HRTIM_Timerx_TypeDef sTimerxRegs[6];

  HRTIM_Common_TypeDef sCommonRegs;
}HRTIM_TypeDef;
# 122 "../src/driver/stm32g4-v120/inc/stm32g4xx.h" 2
# 141 "../src/driver/stm32g4-v120/inc/stm32g4xx.h"
typedef enum
{
  RESET = 0,
  SET = !RESET
} FlagStatus, ITStatus;

typedef enum
{
  DISABLE = 0,
  ENABLE = !DISABLE
} FunctionalState;


typedef enum
{
  SUCCESS = 0,
  ERROR = !SUCCESS
} ErrorStatus;
# 190 "../src/driver/stm32g4-v120/inc/stm32g4xx.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal.h" 1
# 191 "../src/driver/stm32g4-v120/inc/stm32g4xx.h" 2
# 31 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_def.h" 2
# 1 "../src/driver/stm32g4-v120/inc/Legacy/stm32_hal_legacy.h" 1
# 32 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_def.h" 2
# 1 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\stddef.h" 1 3
# 38 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\stddef.h" 3
  typedef signed int ptrdiff_t;
# 53 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\stddef.h" 3
    typedef unsigned int size_t;
# 71 "C:\\Keil_v5\\ARM\\ARMCLANG\\Bin\\..\\include\\stddef.h" 3
      typedef unsigned short wchar_t;
# 33 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_def.h" 2






typedef enum
{
  HAL_OK = 0x00U,
  HAL_ERROR = 0x01U,
  HAL_BUSY = 0x02U,
  HAL_TIMEOUT = 0x03U
} HAL_StatusTypeDef;




typedef enum
{
  HAL_UNLOCKED = 0x00U,
  HAL_LOCKED = 0x01U
} HAL_LockTypeDef;
# 30 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc.h" 2
# 47 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc.h"
typedef struct
{
  uint32_t PLLState;


  uint32_t PLLSource;


  uint32_t PLLM;


  uint32_t PLLN;


  uint32_t PLLP;


  uint32_t PLLQ;


  uint32_t PLLR;



}RCC_PLLInitTypeDef;




typedef struct
{
  uint32_t OscillatorType;


  uint32_t HSEState;


  uint32_t LSEState;


  uint32_t HSIState;


  uint32_t HSICalibrationValue;


  uint32_t LSIState;


  uint32_t HSI48State;


  RCC_PLLInitTypeDef PLL;

}RCC_OscInitTypeDef;




typedef struct
{
  uint32_t ClockType;


  uint32_t SYSCLKSource;


  uint32_t AHBCLKDivider;


  uint32_t APB1CLKDivider;


  uint32_t APB2CLKDivider;


}RCC_ClkInitTypeDef;
# 3328 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc_ex.h" 1
# 48 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc_ex.h"
typedef struct
{
  uint32_t PeriphClockSelection;


  uint32_t Usart1ClockSelection;


  uint32_t Usart2ClockSelection;


  uint32_t Usart3ClockSelection;



  uint32_t Uart4ClockSelection;




  uint32_t Uart5ClockSelection;




  uint32_t Lpuart1ClockSelection;


  uint32_t I2c1ClockSelection;


  uint32_t I2c2ClockSelection;


  uint32_t I2c3ClockSelection;




  uint32_t I2c4ClockSelection;



  uint32_t Lptim1ClockSelection;


  uint32_t Sai1ClockSelection;


  uint32_t I2sClockSelection;



  uint32_t FdcanClockSelection;




  uint32_t UsbClockSelection;



  uint32_t RngClockSelection;


  uint32_t Adc12ClockSelection;



  uint32_t Adc345ClockSelection;




  uint32_t QspiClockSelection;



  uint32_t RTCClockSelection;

}RCC_PeriphCLKInitTypeDef;




typedef struct
{
  uint32_t Prescaler;


  uint32_t Source;


  uint32_t Polarity;


  uint32_t ReloadValue;



  uint32_t ErrorLimitValue;


  uint32_t HSI48CalibrationValue;


}RCC_CRSInitTypeDef;




typedef struct
{
  uint32_t ReloadValue;


  uint32_t HSI48CalibrationValue;


  uint32_t FreqErrorCapture;



  uint32_t FreqErrorDirection;




}RCC_CRSSynchroInfoTypeDef;
# 1264 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc_ex.h"
HAL_StatusTypeDef HAL_RCCEx_PeriphCLKConfig(RCC_PeriphCLKInitTypeDef *PeriphClkInit);
void HAL_RCCEx_GetPeriphCLKConfig(RCC_PeriphCLKInitTypeDef *PeriphClkInit);
uint32_t HAL_RCCEx_GetPeriphCLKFreq(uint32_t PeriphClk);
# 1276 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc_ex.h"
void HAL_RCCEx_EnableLSECSS(void);
void HAL_RCCEx_DisableLSECSS(void);
void HAL_RCCEx_EnableLSECSS_IT(void);
void HAL_RCCEx_LSECSS_IRQHandler(void);
void HAL_RCCEx_LSECSS_Callback(void);
void HAL_RCCEx_EnableLSCO(uint32_t LSCOSource);
void HAL_RCCEx_DisableLSCO(void);
# 1292 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc_ex.h"
void HAL_RCCEx_CRSConfig(RCC_CRSInitTypeDef *pInit);
void HAL_RCCEx_CRSSoftwareSynchronizationGenerate(void);
void HAL_RCCEx_CRSGetSynchronizationInfo(RCC_CRSSynchroInfoTypeDef *pSynchroInfo);
uint32_t HAL_RCCEx_CRSWaitSynchronization(uint32_t Timeout);
void HAL_RCCEx_CRS_IRQHandler(void);
void HAL_RCCEx_CRS_SyncOkCallback(void);
void HAL_RCCEx_CRS_SyncWarnCallback(void);
void HAL_RCCEx_CRS_ExpectedSyncCallback(void);
void HAL_RCCEx_CRS_ErrorCallback(uint32_t Error);
# 3329 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc.h" 2
# 3341 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc.h"
HAL_StatusTypeDef HAL_RCC_DeInit(void);
HAL_StatusTypeDef HAL_RCC_OscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
HAL_StatusTypeDef HAL_RCC_ClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t FLatency);
# 3354 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rcc.h"
void HAL_RCC_MCOConfig(uint32_t RCC_MCOx, uint32_t RCC_MCOSource, uint32_t RCC_MCODiv);
void HAL_RCC_EnableCSS(void);
void HAL_RCC_EnableLSECSS(void);
void HAL_RCC_DisableLSECSS(void);
uint32_t HAL_RCC_GetSysClockFreq(void);
uint32_t HAL_RCC_GetHCLKFreq(void);
uint32_t HAL_RCC_GetPCLK1Freq(void);
uint32_t HAL_RCC_GetPCLK2Freq(void);
void HAL_RCC_GetOscConfig(RCC_OscInitTypeDef *RCC_OscInitStruct);
void HAL_RCC_GetClockConfig(RCC_ClkInitTypeDef *RCC_ClkInitStruct, uint32_t *pFLatency);

void HAL_RCC_NMI_IRQHandler(void);

void HAL_RCC_CSSCallback(void);
# 245 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2



# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_gpio.h" 1
# 48 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_gpio.h"
typedef struct
{
  uint32_t Pin;


  uint32_t Mode;


  uint32_t Pull;


  uint32_t Speed;


  uint32_t Alternate;

} GPIO_InitTypeDef;




typedef enum
{
  GPIO_PIN_RESET = 0U,
  GPIO_PIN_SET
} GPIO_PinState;
# 245 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_gpio.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_gpio_ex.h" 1
# 246 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_gpio.h" 2
# 259 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_gpio.h"
void HAL_GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_Init);
void HAL_GPIO_DeInit(GPIO_TypeDef *GPIOx, uint32_t GPIO_Pin);
# 272 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_gpio.h"
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
HAL_StatusTypeDef HAL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_IRQHandler(uint16_t GPIO_Pin);
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
# 249 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2



# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_dma.h" 1
# 47 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_dma.h"
typedef struct
{
  uint32_t Request;


  uint32_t Direction;



  uint32_t PeriphInc;


  uint32_t MemInc;


  uint32_t PeriphDataAlignment;


  uint32_t MemDataAlignment;


  uint32_t Mode;




  uint32_t Priority;

} DMA_InitTypeDef;




typedef enum
{
  HAL_DMA_STATE_RESET = 0x00U,
  HAL_DMA_STATE_READY = 0x01U,
  HAL_DMA_STATE_BUSY = 0x02U,
  HAL_DMA_STATE_TIMEOUT = 0x03U,
} HAL_DMA_StateTypeDef;




typedef enum
{
  HAL_DMA_FULL_TRANSFER = 0x00U,
  HAL_DMA_HALF_TRANSFER = 0x01U
} HAL_DMA_LevelCompleteTypeDef;





typedef enum
{
  HAL_DMA_XFER_CPLT_CB_ID = 0x00U,
  HAL_DMA_XFER_HALFCPLT_CB_ID = 0x01U,
  HAL_DMA_XFER_ERROR_CB_ID = 0x02U,
  HAL_DMA_XFER_ABORT_CB_ID = 0x03U,
  HAL_DMA_XFER_ALL_CB_ID = 0x04U

} HAL_DMA_CallbackIDTypeDef;




typedef struct __DMA_HandleTypeDef
{
  DMA_Channel_TypeDef *Instance;

  DMA_InitTypeDef Init;

  HAL_LockTypeDef Lock;

  volatile HAL_DMA_StateTypeDef State;

  void *Parent;

  void (* XferCpltCallback)(struct __DMA_HandleTypeDef *hdma);

  void (* XferHalfCpltCallback)(struct __DMA_HandleTypeDef *hdma);

  void (* XferErrorCallback)(struct __DMA_HandleTypeDef *hdma);

  void (* XferAbortCallback)(struct __DMA_HandleTypeDef *hdma);

  volatile uint32_t ErrorCode;

  DMA_TypeDef *DmaBaseAddress;

  uint32_t ChannelIndex;

  DMAMUX_Channel_TypeDef *DMAmuxChannel;

  DMAMUX_ChannelStatus_TypeDef *DMAmuxChannelStatus;

  uint32_t DMAmuxChannelStatusMask;

  DMAMUX_RequestGen_TypeDef *DMAmuxRequestGen;

  DMAMUX_RequestGenStatus_TypeDef *DMAmuxRequestGenStatus;

  uint32_t DMAmuxRequestGenStatusMask;

} DMA_HandleTypeDef;
# 748 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_dma.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_dma_ex.h" 1
# 52 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_dma_ex.h"
typedef struct
{
  uint32_t SyncSignalID;


  uint32_t SyncPolarity;


  FunctionalState SyncEnable;



  FunctionalState EventEnable;


  uint32_t RequestNumber;



} HAL_DMA_MuxSyncConfigTypeDef;





typedef struct
{
  uint32_t SignalID;


  uint32_t Polarity;


  uint32_t RequestNumber;


} HAL_DMA_MuxRequestGeneratorConfigTypeDef;
# 198 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_dma_ex.h"
HAL_StatusTypeDef HAL_DMAEx_ConfigMuxRequestGenerator(DMA_HandleTypeDef *hdma,
                                                      HAL_DMA_MuxRequestGeneratorConfigTypeDef *pRequestGeneratorConfig);
HAL_StatusTypeDef HAL_DMAEx_EnableMuxRequestGenerator(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMAEx_DisableMuxRequestGenerator(DMA_HandleTypeDef *hdma);



HAL_StatusTypeDef HAL_DMAEx_ConfigMuxSync(DMA_HandleTypeDef *hdma, HAL_DMA_MuxSyncConfigTypeDef *pSyncConfig);


void HAL_DMAEx_MUX_IRQHandler(DMA_HandleTypeDef *hdma);
# 749 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_dma.h" 2
# 760 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_dma.h"
HAL_StatusTypeDef HAL_DMA_Init(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_DeInit(DMA_HandleTypeDef *hdma);
# 770 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_dma.h"
HAL_StatusTypeDef HAL_DMA_Start(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Start_IT(DMA_HandleTypeDef *hdma, uint32_t SrcAddress, uint32_t DstAddress,
                                   uint32_t DataLength);
HAL_StatusTypeDef HAL_DMA_Abort(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_Abort_IT(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_PollForTransfer(DMA_HandleTypeDef *hdma, HAL_DMA_LevelCompleteTypeDef CompleteLevel,
                                          uint32_t Timeout);
void HAL_DMA_IRQHandler(DMA_HandleTypeDef *hdma);
HAL_StatusTypeDef HAL_DMA_RegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID, void (* pCallback)(DMA_HandleTypeDef *_hdma));
HAL_StatusTypeDef HAL_DMA_UnRegisterCallback(DMA_HandleTypeDef *hdma, HAL_DMA_CallbackIDTypeDef CallbackID);
# 789 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_dma.h"
HAL_DMA_StateTypeDef HAL_DMA_GetState(DMA_HandleTypeDef *hdma);
uint32_t HAL_DMA_GetError(DMA_HandleTypeDef *hdma);
# 253 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2



# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cortex.h" 1
# 49 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cortex.h"
typedef struct
{
  uint8_t Enable;

  uint8_t Number;

  uint32_t BaseAddress;
  uint8_t Size;

  uint8_t SubRegionDisable;

  uint8_t TypeExtField;

  uint8_t AccessPermission;

  uint8_t DisableExec;

  uint8_t IsShareable;

  uint8_t IsCacheable;

  uint8_t IsBufferable;

}MPU_Region_InitTypeDef;
# 270 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cortex.h"
void HAL_NVIC_SetPriorityGrouping(uint32_t PriorityGroup);
void HAL_NVIC_SetPriority(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
void HAL_NVIC_EnableIRQ(IRQn_Type IRQn);
void HAL_NVIC_DisableIRQ(IRQn_Type IRQn);
void HAL_NVIC_SystemReset(void);
uint32_t HAL_SYSTICK_Config(uint32_t TicksNumb);
# 286 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cortex.h"
uint32_t HAL_NVIC_GetPriorityGrouping(void);
void HAL_NVIC_GetPriority(IRQn_Type IRQn, uint32_t PriorityGroup, uint32_t* pPreemptPriority, uint32_t* pSubPriority);
uint32_t HAL_NVIC_GetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_SetPendingIRQ(IRQn_Type IRQn);
void HAL_NVIC_ClearPendingIRQ(IRQn_Type IRQn);
uint32_t HAL_NVIC_GetActive(IRQn_Type IRQn);
void HAL_SYSTICK_CLKSourceConfig(uint32_t CLKSource);
void HAL_SYSTICK_IRQHandler(void);
void HAL_SYSTICK_Callback(void);


void HAL_MPU_Enable(uint32_t MPU_Control);
void HAL_MPU_Disable(void);
void HAL_MPU_ConfigRegion(MPU_Region_InitTypeDef *MPU_Init);
# 257 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2



# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h" 1
# 32 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h" 1
# 393 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
typedef struct
{
  uint32_t CommonClock;
# 404 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
  uint32_t Multimode;




  uint32_t MultiDMATransfer;




  uint32_t MultiTwoSamplingDelay;





} LL_ADC_CommonInitTypeDef;
# 442 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
typedef struct
{
  uint32_t Resolution;




  uint32_t DataAlignment;




  uint32_t LowPowerMode;




} LL_ADC_InitTypeDef;
# 480 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
typedef struct
{
  uint32_t TriggerSource;







  uint32_t SequencerLength;




  uint32_t SequencerDiscont;






  uint32_t ContinuousMode;





  uint32_t DMATransfer;




  uint32_t Overrun;





} LL_ADC_REG_InitTypeDef;
# 540 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
typedef struct
{
  uint32_t TriggerSource;







  uint32_t SequencerLength;




  uint32_t SequencerDiscont;






  uint32_t TrigAuto;





} LL_ADC_INJ_InitTypeDef;
# 2659 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_DMA_GetRegAddr(ADC_TypeDef *ADCx, uint32_t Register)
{
  uint32_t data_reg_addr;

  if (Register == (0x00000000UL))
  {

    data_reg_addr = (uint32_t) &(ADCx->DR);
  }
  else
  {

    data_reg_addr = (uint32_t) &((((((ADCx) == ((ADC_TypeDef *) (((0x40000000UL) + 0x08000000UL) + 0x08000000UL))) || ((ADCx) == ((ADC_TypeDef *) (((0x40000000UL) + 0x08000000UL) + 0x08000100UL)))) ? ( (((ADC_Common_TypeDef *) (((0x40000000UL) + 0x08000000UL) + 0x08000300UL))) ) : ( (((ADC_Common_TypeDef *) (((0x40000000UL) + 0x08000000UL) + 0x08000700UL))) ) ))->CDR);
  }

  return data_reg_addr;
}
# 2729 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetCommonClock(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t CommonClock)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x3UL << (16U)) | (0xFUL << (18U))))) | (CommonClock))));
}
# 2757 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetCommonClock(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x3UL << (16U)) | (0xFUL << (18U)))));
}
# 2793 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetCommonPathInternalCh(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x1UL << (22U)) | (0x1UL << (23U)) | (0x1UL << (24U))))) | (PathInternal))));
}
# 2828 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetCommonPathInternalChAdd(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
{
  ((ADCxy_COMMON->CCR) |= (PathInternal));
}
# 2852 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetCommonPathInternalChRem(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t PathInternal)
{
  ((ADCxy_COMMON->CCR) &= ~(PathInternal));
}
# 2874 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetCommonPathInternalCh(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x1UL << (22U)) | (0x1UL << (23U)) | (0x1UL << (24U)))));
}
# 2918 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetCalibrationFactor(ADC_TypeDef *ADCx, uint32_t SingleDiff, uint32_t CalibrationFactor)
{
  (((ADCx->CALFACT)) = ((((((ADCx->CALFACT))) & (~(SingleDiff & ((0x7FUL << (16U)) | (0x7FUL << (0U)))))) | (CalibrationFactor << (((SingleDiff & (0x00010000UL)) >> ((16UL) - 4UL)) & ~(SingleDiff & (0x7FUL << (0U))))))));


}
# 2941 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetCalibrationFactor(ADC_TypeDef *ADCx, uint32_t SingleDiff)
{




  return (uint32_t)(((ADCx->CALFACT) & ((SingleDiff & ((0x7FUL << (16U)) | (0x7FUL << (0U)))))) >> ((SingleDiff & (0x00010000UL)) >> ((16UL) - 4UL)));
}
# 2967 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetResolution(ADC_TypeDef *ADCx, uint32_t Resolution)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x3UL << (3U))))) | (Resolution))));
}
# 2984 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetResolution(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR) & ((0x3UL << (3U)))));
}
# 3004 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetDataAlignment(ADC_TypeDef *ADCx, uint32_t DataAlignment)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x1UL << (15U))))) | (DataAlignment))));
}
# 3019 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetDataAlignment(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR) & ((0x1UL << (15U)))));
}
# 3075 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetLowPowerMode(ADC_TypeDef *ADCx, uint32_t LowPowerMode)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x1UL << (14U))))) | (LowPowerMode))));
}
# 3126 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetLowPowerMode(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR) & ((0x1UL << (14U)))));
}
# 3215 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetOffset(ADC_TypeDef *ADCx, uint32_t Offsety, uint32_t Channel, uint32_t OffsetLevel)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->OFR1)) + ((Offsety) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0x1UL << (31U)) | (0x1FUL << (26U)) | (0xFFFUL << (0U))))) | ((0x1UL << (31U)) | (Channel & ((0x1FUL << (26U)))) | OffsetLevel))));


}
# 3298 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetOffsetChannel(ADC_TypeDef *ADCx, uint32_t Offsety)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->OFR1)) + ((Offsety) << 2UL))));

  return (uint32_t) ((*preg) & ((0x1FUL << (26U))));
}
# 3324 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetOffsetLevel(ADC_TypeDef *ADCx, uint32_t Offsety)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->OFR1)) + ((Offsety) << 2UL))));

  return (uint32_t) ((*preg) & ((0xFFFUL << (0U))));
}
# 3357 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetOffsetState(ADC_TypeDef *ADCx, uint32_t Offsety, uint32_t OffsetState)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->OFR1)) + ((Offsety) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0x1UL << (31U))))) | (OffsetState))));


}
# 3383 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetOffsetState(ADC_TypeDef *ADCx, uint32_t Offsety)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->OFR1)) + ((Offsety) << 2UL))));

  return (uint32_t) ((*preg) & ((0x1UL << (31U))));
}
# 3412 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetOffsetSign(ADC_TypeDef *ADCx, uint32_t Offsety, uint32_t OffsetSign)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->OFR1)) + ((Offsety) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0x1UL << (24U))))) | (OffsetSign))));


}
# 3438 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetOffsetSign(ADC_TypeDef *ADCx, uint32_t Offsety)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->OFR1)) + ((Offsety) << 2UL))));

  return (uint32_t) ((*preg) & ((0x1UL << (24U))));
}
# 3467 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetOffsetSaturation(ADC_TypeDef *ADCx, uint32_t Offsety, uint32_t OffsetSaturation)
{
  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->OFR1)) + ((Offsety) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0x1UL << (25U))))) | (OffsetSaturation))));


}
# 3493 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetOffsetSaturation(ADC_TypeDef *ADCx, uint32_t Offsety)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->OFR1)) + ((Offsety) << 2UL))));

  return (uint32_t) ((*preg) & ((0x1UL << (25U))));
}
# 3520 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetGainCompensation(ADC_TypeDef *ADCx, uint32_t GainCompensation)
{
  (((ADCx->GCOMP)) = ((((((ADCx->GCOMP))) & (~((0x3FFFUL << (0U))))) | (GainCompensation))));
  (((ADCx->CFGR2)) = ((((((ADCx->CFGR2))) & (~((0x1UL << (16U))))) | (((GainCompensation == 0UL) ? 0UL : 1UL) << (16U)))));
}
# 3535 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetGainCompensation(ADC_TypeDef *ADCx)
{
  return ((((ADCx->CFGR2) & ((0x1UL << (16U)))) == (0x1UL << (16U))) ? ((ADCx->GCOMP) & ((0x3FFFUL << (0U)))) : 0UL);
}
# 3555 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetSamplingTimeCommonConfig(ADC_TypeDef *ADCx, uint32_t SamplingTimeCommonConfig)
{
  (((ADCx->SMPR1)) = ((((((ADCx->SMPR1))) & (~((0x1UL << (31U))))) | (SamplingTimeCommonConfig))));
}
# 3569 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetSamplingTimeCommonConfig(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->SMPR1) & ((0x1UL << (31U)))));
}
# 3649 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetTriggerSource(ADC_TypeDef *ADCx, uint32_t TriggerSource)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x3UL << (10U)) | (0x1FUL << (5U))))) | (TriggerSource))));
}
# 3715 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetTriggerSource(ADC_TypeDef *ADCx)
{
  volatile uint32_t TriggerSource = ((ADCx->CFGR) & ((0x1FUL << (5U)) | (0x3UL << (10U))));



  uint32_t ShiftExten = ((TriggerSource & (0x3UL << (10U))) >> (((10U)) - 2UL));



  return ((TriggerSource
           & (((((0x00000000UL) & (0x1FUL << (5U))) << (4U * 0UL)) | (((0x1FUL << (5U))) << (4U * 1UL)) | (((0x1FUL << (5U))) << (4U * 2UL)) | (((0x1FUL << (5U))) << (4U * 3UL)) ) >> ShiftExten) & (0x1FUL << (5U)))
          | ((((((0x00000000UL) & (0x3UL << (10U))) << (4U * 0UL)) | ((((0x1UL << (10U)))) << (4U * 1UL)) | ((((0x1UL << (10U)))) << (4U * 2UL)) | ((((0x1UL << (10U)))) << (4U * 3UL)) ) >> ShiftExten) & (0x3UL << (10U)))
         );
}
# 3742 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_IsTriggerSourceSWStart(ADC_TypeDef *ADCx)
{
  return ((((ADCx->CFGR) & ((0x3UL << (10U)))) == ((0x00000000UL) & (0x3UL << (10U)))) ? 1UL : 0UL);
}
# 3762 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetTriggerEdge(ADC_TypeDef *ADCx, uint32_t ExternalTriggerEdge)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x3UL << (10U))))) | (ExternalTriggerEdge))));
}
# 3777 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetTriggerEdge(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR) & ((0x3UL << (10U)))));
}
# 3800 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetSamplingMode(ADC_TypeDef *ADCx, uint32_t SamplingMode)
{
  (((ADCx->CFGR2)) = ((((((ADCx->CFGR2))) & (~((0x1UL << (26U)) | (0x1UL << (27U))))) | (SamplingMode))));
}
# 3815 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetSamplingMode(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR2) & ((0x1UL << (26U)) | (0x1UL << (27U)))));
}
# 3874 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetSequencerLength(ADC_TypeDef *ADCx, uint32_t SequencerNbRanks)
{
  (((ADCx->SQR1)) = ((((((ADCx->SQR1))) & (~((0xFUL << (0U))))) | (SequencerNbRanks))));
}
# 3928 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetSequencerLength(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->SQR1) & ((0xFUL << (0U)))));
}
# 3960 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetSequencerDiscont(ADC_TypeDef *ADCx, uint32_t SeqDiscont)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x1UL << (16U)) | (0x7UL << (17U))))) | (SeqDiscont))));
}
# 3983 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetSequencerDiscont(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR) & ((0x1UL << (16U)) | (0x7UL << (17U)))));
}
# 4086 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank, uint32_t Channel)
{




  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SQR1)) + ((((Rank & ((0x00000000UL) | (0x00000100UL) | (0x00000200UL) | (0x00000300UL))) >> (8UL))) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~(((0x1FUL << (0U))) << (Rank & (((0x1FUL << (0U)))))))) | (((Channel & ((0x1FUL << (26U)))) >> ((26U))) << (Rank & (((0x1FUL << (0U)))))))));


}
# 4199 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SQR1)) + ((((Rank & ((0x00000000UL) | (0x00000100UL) | (0x00000200UL) | (0x00000300UL))) >> (8UL))) << 2UL))));

  return (uint32_t)((((*preg) & (((0x1FUL << (0U))) << (Rank & (((0x1FUL << (0U)))))))

                     >> (Rank & (((0x1FUL << (0U)))))) << ((26U))
                   );
}
# 4228 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetContinuousMode(ADC_TypeDef *ADCx, uint32_t Continuous)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x1UL << (13U))))) | (Continuous))));
}
# 4245 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetContinuousMode(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR) & ((0x1UL << (13U)))));
}
# 4285 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetDMATransfer(ADC_TypeDef *ADCx, uint32_t DMATransfer)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x1UL << (0U)) | (0x1UL << (1U))))) | (DMATransfer))));
}
# 4320 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetDMATransfer(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR) & ((0x1UL << (0U)) | (0x1UL << (1U)))));
}
# 4345 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_SetOverrun(ADC_TypeDef *ADCx, uint32_t Overrun)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x1UL << (12U))))) | (Overrun))));
}
# 4359 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_GetOverrun(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR) & ((0x1UL << (12U)))));
}
# 4437 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetTriggerSource(ADC_TypeDef *ADCx, uint32_t TriggerSource)
{
  (((ADCx->JSQR)) = ((((((ADCx->JSQR))) & (~((0x1FUL << (2U)) | (0x3UL << (7U))))) | (TriggerSource))));
}
# 4502 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetTriggerSource(ADC_TypeDef *ADCx)
{
  volatile uint32_t TriggerSource = ((ADCx->JSQR) & ((0x1FUL << (2U)) | (0x3UL << (7U))));



  uint32_t ShiftJexten = ((TriggerSource & (0x3UL << (7U))) >> (((7U)) - 2UL));



  return ((TriggerSource
           & (((((0x00000000UL) & (0x1FUL << (2U))) << (4U * 0UL)) | (((0x1FUL << (2U))) << (4U * 1UL)) | (((0x1FUL << (2U))) << (4U * 2UL)) | (((0x1FUL << (2U))) << (4U * 3UL)) ) >> ShiftJexten) & (0x1FUL << (2U)))
          | ((((((0x00000000UL) & (0x3UL << (7U))) << (4U * 0UL)) | ((((0x1UL << (7U)))) << (4U * 1UL)) | ((((0x1UL << (7U)))) << (4U * 2UL)) | ((((0x1UL << (7U)))) << (4U * 3UL)) ) >> ShiftJexten) & (0x3UL << (7U)))
         );
}
# 4529 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_IsTriggerSourceSWStart(ADC_TypeDef *ADCx)
{
  return ((((ADCx->JSQR) & ((0x3UL << (7U)))) == ((0x00000000UL) & (0x3UL << (7U)))) ? 1UL : 0UL);
}
# 4549 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetTriggerEdge(ADC_TypeDef *ADCx, uint32_t ExternalTriggerEdge)
{
  (((ADCx->JSQR)) = ((((((ADCx->JSQR))) & (~((0x3UL << (7U))))) | (ExternalTriggerEdge))));
}
# 4564 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetTriggerEdge(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->JSQR) & ((0x3UL << (7U)))));
}
# 4590 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetSequencerLength(ADC_TypeDef *ADCx, uint32_t SequencerNbRanks)
{
  (((ADCx->JSQR)) = ((((((ADCx->JSQR))) & (~((0x3UL << (0U))))) | (SequencerNbRanks))));
}
# 4611 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetSequencerLength(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->JSQR) & ((0x3UL << (0U)))));
}
# 4629 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetSequencerDiscont(ADC_TypeDef *ADCx, uint32_t SeqDiscont)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x1UL << (20U))))) | (SeqDiscont))));
}
# 4644 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetSequencerDiscont(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR) & ((0x1UL << (20U)))));
}
# 4718 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank, uint32_t Channel)
{




  (((ADCx->JSQR)) = ((((((ADCx->JSQR))) & (~((((0x1FUL << (26U))) >> ((26U))) << (Rank & (((0x1FUL << (0U)))))))) | (((Channel & ((0x1FUL << (26U)))) >> ((26U))) << (Rank & (((0x1FUL << (0U)))))))));


}
# 4801 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetSequencerRanks(ADC_TypeDef *ADCx, uint32_t Rank)
{
  return (uint32_t)((((ADCx->JSQR) & ((((0x1FUL << (26U))) >> ((26U))) << (Rank & (((0x1FUL << (0U)))))))

                     >> (Rank & (((0x1FUL << (0U)))))) << ((26U))
                   );
}
# 4839 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetTrigAuto(ADC_TypeDef *ADCx, uint32_t TrigAuto)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x1UL << (25U))))) | (TrigAuto))));
}
# 4853 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetTrigAuto(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR) & ((0x1UL << (25U)))));
}
# 4899 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_INJ_SetQueueMode(ADC_TypeDef *ADCx, uint32_t QueueMode)
{
  (((ADCx->CFGR)) = ((((((ADCx->CFGR))) & (~((0x1UL << (21U)) | (0x1UL << (31U))))) | (QueueMode))));
}
# 4914 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_GetQueueMode(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR) & ((0x1UL << (21U)) | (0x1UL << (31U)))));
}
# 5180 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_INJ_ConfigQueueContext(ADC_TypeDef *ADCx,
                                                   uint32_t TriggerSource,
                                                   uint32_t ExternalTriggerEdge,
                                                   uint32_t SequencerNbRanks,
                                                   uint32_t Rank1_Channel,
                                                   uint32_t Rank2_Channel,
                                                   uint32_t Rank3_Channel,
                                                   uint32_t Rank4_Channel)
{






  uint32_t is_trigger_not_sw = (uint32_t)((TriggerSource != (0x00000000UL)) ? 1UL : 0UL);
  (((ADCx->JSQR)) = ((((((ADCx->JSQR))) & (~((0x1FUL << (2U)) | (0x3UL << (7U)) | (0x1FUL << (27U)) | (0x1FUL << (21U)) | (0x1FUL << (15U)) | (0x1FUL << (9U)) | (0x3UL << (0U))))) | ((TriggerSource & (0x1FUL << (2U))) | (ExternalTriggerEdge * (is_trigger_not_sw)) | (((Rank4_Channel & ((0x1FUL << (26U)))) >> ((26U))) << (((0x00000300UL) | ((27U))) & (((0x1FUL << (0U)))))) | (((Rank3_Channel & ((0x1FUL << (26U)))) >> ((26U))) << (((0x00000200UL) | ((21U))) & (((0x1FUL << (0U)))))) | (((Rank2_Channel & ((0x1FUL << (26U)))) >> ((26U))) << (((0x00000100UL) | ((15U))) & (((0x1FUL << (0U)))))) | (((Rank1_Channel & ((0x1FUL << (26U)))) >> ((26U))) << (((0x00000000UL) | ((9U))) & (((0x1FUL << (0U)))))) | SequencerNbRanks))));
# 5212 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
}
# 5325 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetChannelSamplingTime(ADC_TypeDef *ADCx, uint32_t Channel, uint32_t SamplingTime)
{




  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SMPR1)) + ((((Channel & ((0x00000000UL) | (0x02000000UL))) >> (25UL))) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0x7UL << (0U)) << ((Channel & (0x01F00000UL)) >> (20UL))))) | (SamplingTime << ((Channel & (0x01F00000UL)) >> (20UL))))));


}
# 5425 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetChannelSamplingTime(ADC_TypeDef *ADCx, uint32_t Channel)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->SMPR1)) + ((((Channel & ((0x00000000UL) | (0x02000000UL))) >> (25UL))) << 2UL))));

  return (uint32_t)(((*preg) & ((0x7UL << (0U)) << ((Channel & (0x01F00000UL)) >> (20UL))))

                    >> ((Channel & (0x01F00000UL)) >> (20UL))
                   );
}
# 5490 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetChannelSingleDiff(ADC_TypeDef *ADCx, uint32_t Channel, uint32_t SingleDiff)
{



  (((ADCx->DIFSEL)) = ((((((ADCx->DIFSEL))) & (~(Channel & (((0x7FFFFUL << (0U))))))) | ((Channel & (((0x7FFFFUL << (0U))))) & ((0x7FFFFUL << (0U)) >> (SingleDiff & ((0x10UL << (0U)) | (0x08UL << (0U)))))))));


}
# 5547 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetChannelSingleDiff(ADC_TypeDef *ADCx, uint32_t Channel)
{
  return (uint32_t)(((ADCx->DIFSEL) & ((Channel & (((0x7FFFFUL << (0U))))))));
}
# 5712 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetAnalogWDMonitChannels(ADC_TypeDef *ADCx, uint32_t AWDy, uint32_t AWDChannelGroup)
{




  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->CFGR)) + ((((AWDy & ((0x00000000UL) | (0x00100000UL) | (0x00200000UL))) >> (20UL)) + ((AWDy & ((0x00001UL << (0U)))) * (0x00000024UL))) << 2UL))));


  (((*preg)) = ((((((*preg))) & (~((AWDy & (((0x1FUL << (26U)) | (0x1UL << (24U)) | (0x1UL << (23U)) | (0x1UL << (22U))) | ((0x7FFFFUL << (0U)))))))) | (AWDChannelGroup & AWDy))));


}
# 5848 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetAnalogWDMonitChannels(ADC_TypeDef *ADCx, uint32_t AWDy)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->CFGR)) + ((((AWDy & ((0x00000000UL) | (0x00100000UL) | (0x00200000UL))) >> (20UL)) + ((AWDy & ((0x00001UL << (0U)))) * (0x00000024UL))) << 2UL))));


  uint32_t AnalogWDMonitChannels = (((*preg) & (AWDy)) & AWDy & (((0x1FUL << (26U)) | (0x1UL << (24U)) | (0x1UL << (23U)) | (0x1UL << (22U))) | ((0x7FFFFUL << (0U)))));





  if (AnalogWDMonitChannels != 0UL)
  {
    if (AWDy == (((0x1FUL << (26U)) | (0x1UL << (24U)) | (0x1UL << (23U)) | (0x1UL << (22U))) | (0x00000000UL)))
    {
      if ((AnalogWDMonitChannels & (0x1UL << (22U))) == 0UL)
      {

        AnalogWDMonitChannels = ((AnalogWDMonitChannels
                                  | (((0x7FFFFUL << (0U))))
                                 )
                                 & (~((0x1FUL << (26U))))
                                );
      }
      else
      {

        AnalogWDMonitChannels = (AnalogWDMonitChannels
                                 | ((0x00001UL << (0U)) << (AnalogWDMonitChannels >> (26U)))
                                );
      }
    }
    else
    {
      if ((AnalogWDMonitChannels & ((0x7FFFFUL << (0U)))) == ((0x7FFFFUL << (0U))))
      {

        AnalogWDMonitChannels = (((0x7FFFFUL << (0U)))
                                 | (((0x1UL << (24U)) | (0x1UL << (23U))))
                                );
      }
      else
      {


        AnalogWDMonitChannels = (AnalogWDMonitChannels
                                 | ((0x1UL << (24U)) | (0x1UL << (23U)) | (0x1UL << (22U)))
                                 | (((((AnalogWDMonitChannels) & ((0x7FFFFUL << (0U)))) == 0UL) ? ( ((AnalogWDMonitChannels) & ((0x1FUL << (26U)))) >> ((26U)) ) : ( (uint32_t)(__CLZ(__builtin_arm_rbit((AnalogWDMonitChannels)))) ) ) << (26U))
                                );
      }
    }
  }

  return AnalogWDMonitChannels;
}
# 5952 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_ConfigAnalogWDThresholds(ADC_TypeDef *ADCx, uint32_t AWDy, uint32_t AWDThresholdHighValue,
                                                     uint32_t AWDThresholdLowValue)
{





  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->TR1)) + ((((AWDy & (((0x00000000UL)) | ((0x00100000UL)) | ((0x00200000UL)))) >> ((20UL)))) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~((0xFFFUL << (16U)) | (0xFFFUL << (0U))))) | ((AWDThresholdHighValue << ((16U))) | AWDThresholdLowValue))));


}
# 6021 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetAnalogWDThresholds(ADC_TypeDef *ADCx, uint32_t AWDy, uint32_t AWDThresholdsHighLow,
                                                  uint32_t AWDThresholdValue)
{





  volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->TR1)) + ((((AWDy & (((0x00000000UL)) | ((0x00100000UL)) | ((0x00200000UL)))) >> ((20UL)))) << 2UL))));

  (((*preg)) = ((((((*preg))) & (~(AWDThresholdsHighLow))) | (AWDThresholdValue << ((AWDThresholdsHighLow & (0x00010000UL)) >> ((16UL) - 4UL))))));


}
# 6064 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetAnalogWDThresholds(ADC_TypeDef *ADCx, uint32_t AWDy, uint32_t AWDThresholdsHighLow)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->TR1)) + ((((AWDy & (((0x00000000UL)) | ((0x00100000UL)) | ((0x00200000UL)))) >> ((20UL)))) << 2UL))));

  return (uint32_t)(((*preg) & ((AWDThresholdsHighLow | (0xFFFUL << (0U)))))

                    >> (((AWDThresholdsHighLow & (0x00010000UL)) >> ((16UL) - 4UL)) & ~(AWDThresholdsHighLow & (0xFFFUL << (0U))))
                   );
}
# 6097 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetAWDFilteringConfiguration(ADC_TypeDef *ADCx, uint32_t AWDy, uint32_t FilteringConfig)
{

  (void)(AWDy);
  (((ADCx->TR1)) = ((((((ADCx->TR1))) & (~((0x7UL << (12U))))) | (FilteringConfig))));
}
# 6122 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetAWDFilteringConfiguration(ADC_TypeDef *ADCx, uint32_t AWDy)
{

  (void)(AWDy);
  return (uint32_t)(((ADCx->TR1) & ((0x7UL << (12U)))));
}
# 6162 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetOverSamplingScope(ADC_TypeDef *ADCx, uint32_t OvsScope)
{
  (((ADCx->CFGR2)) = ((((((ADCx->CFGR2))) & (~((0x1UL << (0U)) | (0x1UL << (1U)) | (0x1UL << (10U))))) | (OvsScope))));
}
# 6187 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetOverSamplingScope(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR2) & ((0x1UL << (0U)) | (0x1UL << (1U)) | (0x1UL << (10U)))));
}
# 6214 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetOverSamplingDiscont(ADC_TypeDef *ADCx, uint32_t OverSamplingDiscont)
{
  (((ADCx->CFGR2)) = ((((((ADCx->CFGR2))) & (~((0x1UL << (9U))))) | (OverSamplingDiscont))));
}
# 6233 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetOverSamplingDiscont(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR2) & ((0x1UL << (9U)))));
}
# 6272 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_ConfigOverSamplingRatioShift(ADC_TypeDef *ADCx, uint32_t Ratio, uint32_t Shift)
{
  (((ADCx->CFGR2)) = ((((((ADCx->CFGR2))) & (~(((0xFUL << (5U)) | (0x7UL << (2U)))))) | ((Shift | Ratio)))));
}
# 6292 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetOverSamplingRatio(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR2) & ((0x7UL << (2U)))));
}
# 6313 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetOverSamplingShift(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->CFGR2) & ((0xFUL << (5U)))));
}
# 6353 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetMultimode(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t Multimode)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x1FUL << (0U))))) | (Multimode))));
}
# 6377 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetMultimode(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x1FUL << (0U)))));
}
# 6428 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetMultiDMATransfer(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t MultiDMATransfer)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0x3UL << (14U)) | (0x1UL << (13U))))) | (MultiDMATransfer))));
}
# 6474 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetMultiDMATransfer(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0x3UL << (14U)) | (0x1UL << (13U)))));
}
# 6514 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_SetMultiTwoSamplingDelay(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t MultiTwoSamplingDelay)
{
  (((ADCxy_COMMON->CCR)) = ((((((ADCxy_COMMON->CCR))) & (~((0xFUL << (8U))))) | (MultiTwoSamplingDelay))));
}
# 6542 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_GetMultiTwoSamplingDelay(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return (uint32_t)(((ADCxy_COMMON->CCR) & ((0xFUL << (8U)))));
}
# 6568 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_EnableDeepPowerDown(ADC_TypeDef *ADCx)
{



  (((ADCx->CR)) = ((((((ADCx->CR))) & (~(((0x1UL << (31U)) | (0x1UL << (5U)) | (0x1UL << (4U)) | (0x1UL << (3U)) | (0x1UL << (2U)) | (0x1UL << (1U)) | (0x1UL << (0U)))))) | ((0x1UL << (29U))))));


}
# 6591 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_DisableDeepPowerDown(ADC_TypeDef *ADCx)
{



  ((ADCx->CR) &= ~(((0x1UL << (29U)) | ((0x1UL << (31U)) | (0x1UL << (5U)) | (0x1UL << (4U)) | (0x1UL << (3U)) | (0x1UL << (2U)) | (0x1UL << (1U)) | (0x1UL << (0U))))));
}







static __inline uint32_t LL_ADC_IsDeepPowerDownEnabled(ADC_TypeDef *ADCx)
{
  return ((((ADCx->CR) & ((0x1UL << (29U)))) == ((0x1UL << (29U)))) ? 1UL : 0UL);
}
# 6624 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_EnableInternalRegulator(ADC_TypeDef *ADCx)
{



  (((ADCx->CR)) = ((((((ADCx->CR))) & (~(((0x1UL << (31U)) | (0x1UL << (5U)) | (0x1UL << (4U)) | (0x1UL << (3U)) | (0x1UL << (2U)) | (0x1UL << (1U)) | (0x1UL << (0U)))))) | ((0x1UL << (28U))))));


}
# 6643 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_DisableInternalRegulator(ADC_TypeDef *ADCx)
{
  ((ADCx->CR) &= ~(((0x1UL << (28U)) | ((0x1UL << (31U)) | (0x1UL << (5U)) | (0x1UL << (4U)) | (0x1UL << (3U)) | (0x1UL << (2U)) | (0x1UL << (1U)) | (0x1UL << (0U))))));
}







static __inline uint32_t LL_ADC_IsInternalRegulatorEnabled(ADC_TypeDef *ADCx)
{
  return ((((ADCx->CR) & ((0x1UL << (28U)))) == ((0x1UL << (28U)))) ? 1UL : 0UL);
}
# 6675 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_Enable(ADC_TypeDef *ADCx)
{



  (((ADCx->CR)) = ((((((ADCx->CR))) & (~(((0x1UL << (31U)) | (0x1UL << (5U)) | (0x1UL << (4U)) | (0x1UL << (3U)) | (0x1UL << (2U)) | (0x1UL << (1U)) | (0x1UL << (0U)))))) | ((0x1UL << (0U))))));


}
# 6695 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_Disable(ADC_TypeDef *ADCx)
{



  (((ADCx->CR)) = ((((((ADCx->CR))) & (~(((0x1UL << (31U)) | (0x1UL << (5U)) | (0x1UL << (4U)) | (0x1UL << (3U)) | (0x1UL << (2U)) | (0x1UL << (1U)) | (0x1UL << (0U)))))) | ((0x1UL << (1U))))));


}
# 6714 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabled(ADC_TypeDef *ADCx)
{
  return ((((ADCx->CR) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_IsDisableOngoing(ADC_TypeDef *ADCx)
{
  return ((((ADCx->CR) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}
# 6753 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_StartCalibration(ADC_TypeDef *ADCx, uint32_t SingleDiff)
{



  (((ADCx->CR)) = ((((((ADCx->CR))) & (~((0x1UL << (30U)) | ((0x1UL << (31U)) | (0x1UL << (5U)) | (0x1UL << (4U)) | (0x1UL << (3U)) | (0x1UL << (2U)) | (0x1UL << (1U)) | (0x1UL << (0U)))))) | ((0x1UL << (31U)) | (SingleDiff & ((0x1UL << (30U))))))));


}







static __inline uint32_t LL_ADC_IsCalibrationOnGoing(ADC_TypeDef *ADCx)
{
  return ((((ADCx->CR) & ((0x1UL << (31U)))) == ((0x1UL << (31U)))) ? 1UL : 0UL);
}
# 6800 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_StartConversion(ADC_TypeDef *ADCx)
{



  (((ADCx->CR)) = ((((((ADCx->CR))) & (~(((0x1UL << (31U)) | (0x1UL << (5U)) | (0x1UL << (4U)) | (0x1UL << (3U)) | (0x1UL << (2U)) | (0x1UL << (1U)) | (0x1UL << (0U)))))) | ((0x1UL << (2U))))));


}
# 6820 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_StopConversion(ADC_TypeDef *ADCx)
{



  (((ADCx->CR)) = ((((((ADCx->CR))) & (~(((0x1UL << (31U)) | (0x1UL << (5U)) | (0x1UL << (4U)) | (0x1UL << (3U)) | (0x1UL << (2U)) | (0x1UL << (1U)) | (0x1UL << (0U)))))) | ((0x1UL << (4U))))));


}







static __inline uint32_t LL_ADC_REG_IsConversionOngoing(ADC_TypeDef *ADCx)
{
  return ((((ADCx->CR) & ((0x1UL << (2U)))) == ((0x1UL << (2U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_REG_IsStopConversionOngoing(ADC_TypeDef *ADCx)
{
  return ((((ADCx->CR) & ((0x1UL << (4U)))) == ((0x1UL << (4U)))) ? 1UL : 0UL);
}
# 6867 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_StartSamplingPhase(ADC_TypeDef *ADCx)
{
  ((ADCx->CFGR2) |= ((0x1UL << (25U))));
}
# 6889 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_REG_StopSamplingPhase(ADC_TypeDef *ADCx)
{
  ((ADCx->CFGR2) &= ~((0x1UL << (25U))));
}
# 6903 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_ReadConversionData32(ADC_TypeDef *ADCx)
{
  return (uint32_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}
# 6918 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint16_t LL_ADC_REG_ReadConversionData12(ADC_TypeDef *ADCx)
{
  return (uint16_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}
# 6933 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint16_t LL_ADC_REG_ReadConversionData10(ADC_TypeDef *ADCx)
{
  return (uint16_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}
# 6948 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint8_t LL_ADC_REG_ReadConversionData8(ADC_TypeDef *ADCx)
{
  return (uint8_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}
# 6963 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint8_t LL_ADC_REG_ReadConversionData6(ADC_TypeDef *ADCx)
{
  return (uint8_t)(((ADCx->DR) & ((0xFFFFUL << (0U)))));
}
# 6990 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_REG_ReadMultiConversionData32(ADC_Common_TypeDef *ADCxy_COMMON, uint32_t ConversionData)
{
  return (uint32_t)(((ADCxy_COMMON->CDR) & (ConversionData))

                    >> ((__CLZ(__builtin_arm_rbit(ConversionData))) & 0x1FUL)
                   );
}
# 7025 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_INJ_StartConversion(ADC_TypeDef *ADCx)
{



  (((ADCx->CR)) = ((((((ADCx->CR))) & (~(((0x1UL << (31U)) | (0x1UL << (5U)) | (0x1UL << (4U)) | (0x1UL << (3U)) | (0x1UL << (2U)) | (0x1UL << (1U)) | (0x1UL << (0U)))))) | ((0x1UL << (3U))))));


}
# 7045 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_INJ_StopConversion(ADC_TypeDef *ADCx)
{



  (((ADCx->CR)) = ((((((ADCx->CR))) & (~(((0x1UL << (31U)) | (0x1UL << (5U)) | (0x1UL << (4U)) | (0x1UL << (3U)) | (0x1UL << (2U)) | (0x1UL << (1U)) | (0x1UL << (0U)))))) | ((0x1UL << (5U))))));


}







static __inline uint32_t LL_ADC_INJ_IsConversionOngoing(ADC_TypeDef *ADCx)
{
  return ((((ADCx->CR) & ((0x1UL << (3U)))) == ((0x1UL << (3U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_INJ_IsStopConversionOngoing(ADC_TypeDef *ADCx)
{
  return ((((ADCx->CR) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1UL : 0UL);
}
# 7094 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_INJ_ReadConversionData32(ADC_TypeDef *ADCx, uint32_t Rank)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + ((((Rank & ((0x00000000UL) | (0x00000100UL) | (0x00000200UL) | (0x00000300UL))) >> (8UL))) << 2UL))));

  return (uint32_t)(((*preg) & ((0xFFFFUL << (0U))))

                   );
}
# 7121 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint16_t LL_ADC_INJ_ReadConversionData12(ADC_TypeDef *ADCx, uint32_t Rank)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + ((((Rank & ((0x00000000UL) | (0x00000100UL) | (0x00000200UL) | (0x00000300UL))) >> (8UL))) << 2UL))));

  return (uint16_t)(((*preg) & ((0xFFFFUL << (0U))))

                   );
}
# 7148 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint16_t LL_ADC_INJ_ReadConversionData10(ADC_TypeDef *ADCx, uint32_t Rank)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + ((((Rank & ((0x00000000UL) | (0x00000100UL) | (0x00000200UL) | (0x00000300UL))) >> (8UL))) << 2UL))));

  return (uint16_t)(((*preg) & ((0xFFFFUL << (0U))))

                   );
}
# 7175 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint8_t LL_ADC_INJ_ReadConversionData8(ADC_TypeDef *ADCx, uint32_t Rank)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + ((((Rank & ((0x00000000UL) | (0x00000100UL) | (0x00000200UL) | (0x00000300UL))) >> (8UL))) << 2UL))));

  return (uint8_t)(((*preg) & ((0xFFFFUL << (0U))))

                  );
}
# 7202 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint8_t LL_ADC_INJ_ReadConversionData6(ADC_TypeDef *ADCx, uint32_t Rank)
{
  const volatile uint32_t *preg = ((volatile uint32_t *)((uint32_t) ((uint32_t)(&(ADCx->JDR1)) + ((((Rank & ((0x00000000UL) | (0x00000100UL) | (0x00000200UL) | (0x00000300UL))) >> (8UL))) << 2UL))));

  return (uint8_t)(((*preg) & ((0xFFFFUL << (0U))))

                  );
}
# 7228 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_ADRDY(ADC_TypeDef *ADCx)
{
  return ((((ADCx->ISR) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_IsActiveFlag_EOC(ADC_TypeDef *ADCx)
{
  return ((((ADCx->ISR) & ((0x1UL << (2U)))) == ((0x1UL << (2U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_IsActiveFlag_EOS(ADC_TypeDef *ADCx)
{
  return ((((ADCx->ISR) & ((0x1UL << (3U)))) == ((0x1UL << (3U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_IsActiveFlag_OVR(ADC_TypeDef *ADCx)
{
  return ((((ADCx->ISR) & ((0x1UL << (4U)))) == ((0x1UL << (4U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_IsActiveFlag_EOSMP(ADC_TypeDef *ADCx)
{
  return ((((ADCx->ISR) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_IsActiveFlag_JEOC(ADC_TypeDef *ADCx)
{
  return ((((ADCx->ISR) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_IsActiveFlag_JEOS(ADC_TypeDef *ADCx)
{
  return ((((ADCx->ISR) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_IsActiveFlag_JQOVF(ADC_TypeDef *ADCx)
{
  return ((((ADCx->ISR) & ((0x1UL << (10U)))) == ((0x1UL << (10U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_IsActiveFlag_AWD1(ADC_TypeDef *ADCx)
{
  return ((((ADCx->ISR) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_IsActiveFlag_AWD2(ADC_TypeDef *ADCx)
{
  return ((((ADCx->ISR) & ((0x1UL << (8U)))) == ((0x1UL << (8U)))) ? 1UL : 0UL);
}







static __inline uint32_t LL_ADC_IsActiveFlag_AWD3(ADC_TypeDef *ADCx)
{
  return ((((ADCx->ISR) & ((0x1UL << (9U)))) == ((0x1UL << (9U)))) ? 1UL : 0UL);
}
# 7352 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_ClearFlag_ADRDY(ADC_TypeDef *ADCx)
{
  ((ADCx->ISR) = ((0x1UL << (0U))));
}







static __inline void LL_ADC_ClearFlag_EOC(ADC_TypeDef *ADCx)
{
  ((ADCx->ISR) = ((0x1UL << (2U))));
}







static __inline void LL_ADC_ClearFlag_EOS(ADC_TypeDef *ADCx)
{
  ((ADCx->ISR) = ((0x1UL << (3U))));
}







static __inline void LL_ADC_ClearFlag_OVR(ADC_TypeDef *ADCx)
{
  ((ADCx->ISR) = ((0x1UL << (4U))));
}







static __inline void LL_ADC_ClearFlag_EOSMP(ADC_TypeDef *ADCx)
{
  ((ADCx->ISR) = ((0x1UL << (1U))));
}







static __inline void LL_ADC_ClearFlag_JEOC(ADC_TypeDef *ADCx)
{
  ((ADCx->ISR) = ((0x1UL << (5U))));
}







static __inline void LL_ADC_ClearFlag_JEOS(ADC_TypeDef *ADCx)
{
  ((ADCx->ISR) = ((0x1UL << (6U))));
}







static __inline void LL_ADC_ClearFlag_JQOVF(ADC_TypeDef *ADCx)
{
  ((ADCx->ISR) = ((0x1UL << (10U))));
}







static __inline void LL_ADC_ClearFlag_AWD1(ADC_TypeDef *ADCx)
{
  ((ADCx->ISR) = ((0x1UL << (7U))));
}







static __inline void LL_ADC_ClearFlag_AWD2(ADC_TypeDef *ADCx)
{
  ((ADCx->ISR) = ((0x1UL << (8U))));
}







static __inline void LL_ADC_ClearFlag_AWD3(ADC_TypeDef *ADCx)
{
  ((ADCx->ISR) = ((0x1UL << (9U))));
}
# 7475 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_ADRDY(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}
# 7487 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV_ADRDY(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (16U)))) == ((0x1UL << (16U)))) ? 1UL : 0UL);
}
# 7499 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_EOC(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (18U)))) == ((0x1UL << (18U)))) ? 1UL : 0UL);
}
# 7511 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV_EOC(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (18U)))) == ((0x1UL << (18U)))) ? 1UL : 0UL);
}
# 7523 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_EOS(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (3U)))) == ((0x1UL << (3U)))) ? 1UL : 0UL);
}
# 7535 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV_EOS(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (19U)))) == ((0x1UL << (19U)))) ? 1UL : 0UL);
}
# 7547 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_OVR(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (4U)))) == ((0x1UL << (4U)))) ? 1UL : 0UL);
}
# 7559 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV_OVR(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (20U)))) == ((0x1UL << (20U)))) ? 1UL : 0UL);
}
# 7571 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_EOSMP(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}
# 7583 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV_EOSMP(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (17U)))) == ((0x1UL << (17U)))) ? 1UL : 0UL);
}
# 7595 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_JEOC(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1UL : 0UL);
}
# 7607 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV_JEOC(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (21U)))) == ((0x1UL << (21U)))) ? 1UL : 0UL);
}
# 7619 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_JEOS(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}
# 7631 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV_JEOS(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (22U)))) == ((0x1UL << (22U)))) ? 1UL : 0UL);
}
# 7643 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_JQOVF(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (10U)))) == ((0x1UL << (10U)))) ? 1UL : 0UL);
}
# 7655 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV_JQOVF(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (26U)))) == ((0x1UL << (26U)))) ? 1UL : 0UL);
}
# 7667 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_AWD1(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}
# 7679 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV_AWD1(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (23U)))) == ((0x1UL << (23U)))) ? 1UL : 0UL);
}
# 7691 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_AWD2(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (8U)))) == ((0x1UL << (8U)))) ? 1UL : 0UL);
}
# 7703 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV_AWD2(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (24U)))) == ((0x1UL << (24U)))) ? 1UL : 0UL);
}
# 7715 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_MST_AWD3(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (9U)))) == ((0x1UL << (9U)))) ? 1UL : 0UL);
}
# 7727 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsActiveFlag_SLV_AWD3(ADC_Common_TypeDef *ADCxy_COMMON)
{
  return ((((ADCxy_COMMON->CSR) & ((0x1UL << (25U)))) == ((0x1UL << (25U)))) ? 1UL : 0UL);
}
# 7747 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline void LL_ADC_EnableIT_ADRDY(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) |= ((0x1UL << (0U))));
}







static __inline void LL_ADC_EnableIT_EOC(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) |= ((0x1UL << (2U))));
}







static __inline void LL_ADC_EnableIT_EOS(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) |= ((0x1UL << (3U))));
}







static __inline void LL_ADC_EnableIT_OVR(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) |= ((0x1UL << (4U))));
}







static __inline void LL_ADC_EnableIT_EOSMP(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) |= ((0x1UL << (1U))));
}







static __inline void LL_ADC_EnableIT_JEOC(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) |= ((0x1UL << (5U))));
}







static __inline void LL_ADC_EnableIT_JEOS(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) |= ((0x1UL << (6U))));
}







static __inline void LL_ADC_EnableIT_JQOVF(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) |= ((0x1UL << (10U))));
}







static __inline void LL_ADC_EnableIT_AWD1(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) |= ((0x1UL << (7U))));
}







static __inline void LL_ADC_EnableIT_AWD2(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) |= ((0x1UL << (8U))));
}







static __inline void LL_ADC_EnableIT_AWD3(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) |= ((0x1UL << (9U))));
}







static __inline void LL_ADC_DisableIT_ADRDY(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) &= ~((0x1UL << (0U))));
}







static __inline void LL_ADC_DisableIT_EOC(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) &= ~((0x1UL << (2U))));
}







static __inline void LL_ADC_DisableIT_EOS(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) &= ~((0x1UL << (3U))));
}







static __inline void LL_ADC_DisableIT_OVR(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) &= ~((0x1UL << (4U))));
}







static __inline void LL_ADC_DisableIT_EOSMP(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) &= ~((0x1UL << (1U))));
}







static __inline void LL_ADC_DisableIT_JEOC(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) &= ~((0x1UL << (5U))));
}







static __inline void LL_ADC_DisableIT_JEOS(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) &= ~((0x1UL << (6U))));
}







static __inline void LL_ADC_DisableIT_JQOVF(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) &= ~((0x1UL << (10U))));
}







static __inline void LL_ADC_DisableIT_AWD1(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) &= ~((0x1UL << (7U))));
}







static __inline void LL_ADC_DisableIT_AWD2(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) &= ~((0x1UL << (8U))));
}







static __inline void LL_ADC_DisableIT_AWD3(ADC_TypeDef *ADCx)
{
  ((ADCx->IER) &= ~((0x1UL << (9U))));
}
# 7990 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_ADRDY(ADC_TypeDef *ADCx)
{
  return ((((ADCx->IER) & ((0x1UL << (0U)))) == ((0x1UL << (0U)))) ? 1UL : 0UL);
}
# 8002 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_EOC(ADC_TypeDef *ADCx)
{
  return ((((ADCx->IER) & ((0x1UL << (2U)))) == ((0x1UL << (2U)))) ? 1UL : 0UL);
}
# 8014 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_EOS(ADC_TypeDef *ADCx)
{
  return ((((ADCx->IER) & ((0x1UL << (3U)))) == ((0x1UL << (3U)))) ? 1UL : 0UL);
}
# 8026 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_OVR(ADC_TypeDef *ADCx)
{
  return ((((ADCx->IER) & ((0x1UL << (4U)))) == ((0x1UL << (4U)))) ? 1UL : 0UL);
}
# 8038 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_EOSMP(ADC_TypeDef *ADCx)
{
  return ((((ADCx->IER) & ((0x1UL << (1U)))) == ((0x1UL << (1U)))) ? 1UL : 0UL);
}
# 8050 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_JEOC(ADC_TypeDef *ADCx)
{
  return ((((ADCx->IER) & ((0x1UL << (5U)))) == ((0x1UL << (5U)))) ? 1UL : 0UL);
}
# 8062 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_JEOS(ADC_TypeDef *ADCx)
{
  return ((((ADCx->IER) & ((0x1UL << (6U)))) == ((0x1UL << (6U)))) ? 1UL : 0UL);
}
# 8074 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_JQOVF(ADC_TypeDef *ADCx)
{
  return ((((ADCx->IER) & ((0x1UL << (10U)))) == ((0x1UL << (10U)))) ? 1UL : 0UL);
}
# 8086 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_AWD1(ADC_TypeDef *ADCx)
{
  return ((((ADCx->IER) & ((0x1UL << (7U)))) == ((0x1UL << (7U)))) ? 1UL : 0UL);
}
# 8098 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_AWD2(ADC_TypeDef *ADCx)
{
  return ((((ADCx->IER) & ((0x1UL << (8U)))) == ((0x1UL << (8U)))) ? 1UL : 0UL);
}
# 8110 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
static __inline uint32_t LL_ADC_IsEnabledIT_AWD3(ADC_TypeDef *ADCx)
{
  return ((((ADCx->IER) & ((0x1UL << (9U)))) == ((0x1UL << (9U)))) ? 1UL : 0UL);
}
# 8125 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_adc.h"
ErrorStatus LL_ADC_CommonDeInit(ADC_Common_TypeDef *ADCxy_COMMON);
ErrorStatus LL_ADC_CommonInit(ADC_Common_TypeDef *ADCxy_COMMON, LL_ADC_CommonInitTypeDef *ADC_CommonInitStruct);
void LL_ADC_CommonStructInit(LL_ADC_CommonInitTypeDef *ADC_CommonInitStruct);



ErrorStatus LL_ADC_DeInit(ADC_TypeDef *ADCx);


ErrorStatus LL_ADC_Init(ADC_TypeDef *ADCx, LL_ADC_InitTypeDef *ADC_InitStruct);
void LL_ADC_StructInit(LL_ADC_InitTypeDef *ADC_InitStruct);


ErrorStatus LL_ADC_REG_Init(ADC_TypeDef *ADCx, LL_ADC_REG_InitTypeDef *ADC_REG_InitStruct);
void LL_ADC_REG_StructInit(LL_ADC_REG_InitTypeDef *ADC_REG_InitStruct);


ErrorStatus LL_ADC_INJ_Init(ADC_TypeDef *ADCx, LL_ADC_INJ_InitTypeDef *ADC_INJ_InitStruct);
void LL_ADC_INJ_StructInit(LL_ADC_INJ_InitTypeDef *ADC_INJ_InitStruct);
# 33 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h" 2
# 50 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
typedef struct
{
  uint32_t Ratio;


  uint32_t RightBitShift;


  uint32_t TriggeredMode;


  uint32_t OversamplingStopReset;







} ADC_OversamplingTypeDef;
# 87 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
typedef struct
{
  uint32_t ClockPrescaler;
# 100 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
  uint32_t Resolution;


  uint32_t DataAlign;



  uint32_t GainCompensation;
# 118 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
  uint32_t ScanConvMode;







  uint32_t EOCSelection;


  FunctionalState LowPowerAutoWait;
# 141 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
  FunctionalState ContinuousConvMode;



  uint32_t NbrOfConversion;





  FunctionalState DiscontinuousConvMode;





  uint32_t NbrOfDiscConversion;



  uint32_t ExternalTrigConv;




  uint32_t ExternalTrigConvEdge;



  uint32_t SamplingMode;


  FunctionalState DMAContinuousRequests;




  uint32_t Overrun;
# 189 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
  FunctionalState OversamplingMode;



  ADC_OversamplingTypeDef Oversampling;


} ADC_InitTypeDef;
# 209 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
typedef struct
{
  uint32_t Channel;



  uint32_t Rank;




  uint32_t SamplingTime;
# 231 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
  uint32_t SingleDiff;
# 243 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
  uint32_t OffsetNumber;



  uint32_t Offset;






  uint32_t OffsetSign;



  FunctionalState OffsetSaturation;




} ADC_ChannelConfTypeDef;
# 272 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
typedef struct
{
  uint32_t WatchdogNumber;




  uint32_t WatchdogMode;




  uint32_t Channel;




  FunctionalState ITMode;


  uint32_t HighThreshold;
# 302 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
  uint32_t LowThreshold;
# 312 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
  uint32_t FilteringConfig;




} ADC_AnalogWDGConfTypeDef;





typedef struct
{
  uint32_t ContextQueue;



  uint32_t ChannelCount;
} ADC_InjectionConfigTypeDef;
# 384 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
typedef struct __ADC_HandleTypeDef



{
  ADC_TypeDef *Instance;
  ADC_InitTypeDef Init;
  DMA_HandleTypeDef *DMA_Handle;
  HAL_LockTypeDef Lock;
  volatile uint32_t State;
  volatile uint32_t ErrorCode;
  ADC_InjectionConfigTypeDef InjectionConfig ;

  void (* ConvCpltCallback)(struct __ADC_HandleTypeDef *hadc);
  void (* ConvHalfCpltCallback)(struct __ADC_HandleTypeDef *hadc);
  void (* LevelOutOfWindowCallback)(struct __ADC_HandleTypeDef *hadc);
  void (* ErrorCallback)(struct __ADC_HandleTypeDef *hadc);
  void (* InjectedConvCpltCallback)(struct __ADC_HandleTypeDef *hadc);
  void (* InjectedQueueOverflowCallback)(struct __ADC_HandleTypeDef *hadc);
  void (* LevelOutOfWindow2Callback)(struct __ADC_HandleTypeDef *hadc);
  void (* LevelOutOfWindow3Callback)(struct __ADC_HandleTypeDef *hadc);
  void (* EndOfSamplingCallback)(struct __ADC_HandleTypeDef *hadc);
  void (* MspInitCallback)(struct __ADC_HandleTypeDef *hadc);
  void (* MspDeInitCallback)(struct __ADC_HandleTypeDef *hadc);

} ADC_HandleTypeDef;





typedef enum
{
  HAL_ADC_CONVERSION_COMPLETE_CB_ID = 0x00U,
  HAL_ADC_CONVERSION_HALF_CB_ID = 0x01U,
  HAL_ADC_LEVEL_OUT_OF_WINDOW_1_CB_ID = 0x02U,
  HAL_ADC_ERROR_CB_ID = 0x03U,
  HAL_ADC_INJ_CONVERSION_COMPLETE_CB_ID = 0x04U,
  HAL_ADC_INJ_QUEUE_OVEFLOW_CB_ID = 0x05U,
  HAL_ADC_LEVEL_OUT_OF_WINDOW_2_CB_ID = 0x06U,
  HAL_ADC_LEVEL_OUT_OF_WINDOW_3_CB_ID = 0x07U,
  HAL_ADC_END_OF_SAMPLING_CB_ID = 0x08U,
  HAL_ADC_MSPINIT_CB_ID = 0x09U,
  HAL_ADC_MSPDEINIT_CB_ID = 0x0AU
} HAL_ADC_CallbackIDTypeDef;




typedef void (*pADC_CallbackTypeDef)(ADC_HandleTypeDef *hadc);
# 1917 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc_ex.h" 1
# 47 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc_ex.h"
typedef struct
{
  uint32_t Ratio;


  uint32_t RightBitShift;

} ADC_InjOversamplingTypeDef;
# 72 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc_ex.h"
typedef struct
{
  uint32_t InjectedChannel;



  uint32_t InjectedRank;




  uint32_t InjectedSamplingTime;
# 94 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc_ex.h"
  uint32_t InjectedSingleDiff;
# 106 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc_ex.h"
  uint32_t InjectedOffsetNumber;



  uint32_t InjectedOffset;






  uint32_t InjectedOffsetSign;


  FunctionalState InjectedOffsetSaturation;



  uint32_t InjectedNbrOfConversion;





  FunctionalState InjectedDiscontinuousConvMode;
# 140 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc_ex.h"
  FunctionalState AutoInjectedConv;
# 149 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc_ex.h"
  FunctionalState QueueInjectedContext;
# 160 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc_ex.h"
  uint32_t ExternalTrigInjecConv;





  uint32_t ExternalTrigInjecConvEdge;





  FunctionalState InjecOversamplingMode;



  ADC_InjOversamplingTypeDef InjecOversampling;


} ADC_InjectionConfTypeDef;







typedef struct
{
  uint32_t Mode;


  uint32_t DMAAccessMode;



  uint32_t TwoSamplingDelay;




} ADC_MultiModeTypeDef;
# 1316 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc_ex.h"
HAL_StatusTypeDef HAL_ADCEx_Calibration_Start(ADC_HandleTypeDef *hadc, uint32_t SingleDiff);
uint32_t HAL_ADCEx_Calibration_GetValue(ADC_HandleTypeDef *hadc, uint32_t SingleDiff);
HAL_StatusTypeDef HAL_ADCEx_Calibration_SetValue(ADC_HandleTypeDef *hadc, uint32_t SingleDiff,
                                                       uint32_t CalibrationFactor);


HAL_StatusTypeDef HAL_ADCEx_InjectedStart(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedStop(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedPollForConversion(ADC_HandleTypeDef *hadc, uint32_t Timeout);


HAL_StatusTypeDef HAL_ADCEx_InjectedStart_IT(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_InjectedStop_IT(ADC_HandleTypeDef *hadc);



HAL_StatusTypeDef HAL_ADCEx_MultiModeStart_DMA(ADC_HandleTypeDef *hadc, uint32_t *pData, uint32_t Length);
HAL_StatusTypeDef HAL_ADCEx_MultiModeStop_DMA(ADC_HandleTypeDef *hadc);
uint32_t HAL_ADCEx_MultiModeGetValue(ADC_HandleTypeDef *hadc);



uint32_t HAL_ADCEx_InjectedGetValue(ADC_HandleTypeDef *hadc, uint32_t InjectedRank);


void HAL_ADCEx_InjectedConvCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_ADCEx_InjectedQueueOverflowCallback(ADC_HandleTypeDef *hadc);
void HAL_ADCEx_LevelOutOfWindow2Callback(ADC_HandleTypeDef *hadc);
void HAL_ADCEx_LevelOutOfWindow3Callback(ADC_HandleTypeDef *hadc);
void HAL_ADCEx_EndOfSamplingCallback(ADC_HandleTypeDef *hadc);


HAL_StatusTypeDef HAL_ADCEx_RegularStop(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_RegularStop_IT(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_RegularStop_DMA(ADC_HandleTypeDef *hadc);

HAL_StatusTypeDef HAL_ADCEx_RegularMultiModeStop_DMA(ADC_HandleTypeDef *hadc);
# 1363 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc_ex.h"
HAL_StatusTypeDef HAL_ADCEx_InjectedConfigChannel(ADC_HandleTypeDef *hadc,ADC_InjectionConfTypeDef* sConfigInjected);

HAL_StatusTypeDef HAL_ADCEx_MultiModeConfigChannel(ADC_HandleTypeDef *hadc, ADC_MultiModeTypeDef *multimode);

HAL_StatusTypeDef HAL_ADCEx_EnableInjectedQueue(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_DisableInjectedQueue(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_DisableVoltageRegulator(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADCEx_EnterADCDeepPowerDownMode(ADC_HandleTypeDef *hadc);
# 1918 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h" 2
# 1929 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_DeInit(ADC_HandleTypeDef *hadc);
void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc);
void HAL_ADC_MspDeInit(ADC_HandleTypeDef *hadc);



HAL_StatusTypeDef HAL_ADC_RegisterCallback(ADC_HandleTypeDef *hadc, HAL_ADC_CallbackIDTypeDef CallbackID,
                                           pADC_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_ADC_UnRegisterCallback(ADC_HandleTypeDef *hadc, HAL_ADC_CallbackIDTypeDef CallbackID);
# 1951 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *hadc, uint32_t Timeout);
HAL_StatusTypeDef HAL_ADC_PollForEvent(ADC_HandleTypeDef *hadc, uint32_t EventType, uint32_t Timeout);


HAL_StatusTypeDef HAL_ADC_Start_IT(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_Stop_IT(ADC_HandleTypeDef *hadc);


HAL_StatusTypeDef HAL_ADC_Start_DMA(ADC_HandleTypeDef *hadc, uint32_t *pData, uint32_t Length);
HAL_StatusTypeDef HAL_ADC_Stop_DMA(ADC_HandleTypeDef *hadc);


uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc);


HAL_StatusTypeDef HAL_ADC_StartSampling(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef HAL_ADC_StopSampling(ADC_HandleTypeDef *hadc);


void HAL_ADC_IRQHandler(ADC_HandleTypeDef *hadc);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_ADC_ConvHalfCpltCallback(ADC_HandleTypeDef *hadc);
void HAL_ADC_LevelOutOfWindowCallback(ADC_HandleTypeDef *hadc);
void HAL_ADC_ErrorCallback(ADC_HandleTypeDef *hadc);
# 1986 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef *hadc, ADC_ChannelConfTypeDef *sConfig);
HAL_StatusTypeDef HAL_ADC_AnalogWDGConfig(ADC_HandleTypeDef *hadc, ADC_AnalogWDGConfTypeDef *AnalogWDGConfig);
# 1997 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
uint32_t HAL_ADC_GetState(ADC_HandleTypeDef *hadc);
uint32_t HAL_ADC_GetError(ADC_HandleTypeDef *hadc);
# 2012 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_adc.h"
HAL_StatusTypeDef ADC_ConversionStop(ADC_HandleTypeDef *hadc, uint32_t ConversionGroup);
HAL_StatusTypeDef ADC_Enable(ADC_HandleTypeDef *hadc);
HAL_StatusTypeDef ADC_Disable(ADC_HandleTypeDef *hadc);
void ADC_DMAConvCplt(DMA_HandleTypeDef *hdma);
void ADC_DMAHalfConvCplt(DMA_HandleTypeDef *hdma);
void ADC_DMAError(DMA_HandleTypeDef *hdma);
# 261 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2







# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cordic.h" 1
# 49 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cordic.h"
typedef enum
{
  HAL_CORDIC_STATE_RESET = 0x00U,
  HAL_CORDIC_STATE_READY = 0x01U,
  HAL_CORDIC_STATE_BUSY = 0x02U,
  HAL_CORDIC_STATE_ERROR = 0x03U
} HAL_CORDIC_StateTypeDef;







typedef struct

{
  CORDIC_TypeDef *Instance;

  int32_t *pInBuff;

  int32_t *pOutBuff;

  uint32_t NbCalcToOrder;

  uint32_t NbCalcToGet;

  uint32_t DMADirection;

  DMA_HandleTypeDef *hdmaIn;

  DMA_HandleTypeDef *hdmaOut;

  HAL_LockTypeDef Lock;

  volatile HAL_CORDIC_StateTypeDef State;

  volatile uint32_t ErrorCode;
# 98 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cordic.h"
} CORDIC_HandleTypeDef;




typedef struct
{
  uint32_t Function;


  uint32_t Scale;


  uint32_t InSize;


  uint32_t OutSize;


  uint32_t NbWrite;


  uint32_t NbRead;


  uint32_t Precision;


} CORDIC_ConfigTypeDef;
# 523 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cordic.h"
HAL_StatusTypeDef HAL_CORDIC_Init(CORDIC_HandleTypeDef *hcordic);
HAL_StatusTypeDef HAL_CORDIC_DeInit(CORDIC_HandleTypeDef *hcordic);
void HAL_CORDIC_MspInit(CORDIC_HandleTypeDef *hcordic);
void HAL_CORDIC_MspDeInit(CORDIC_HandleTypeDef *hcordic);
# 541 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cordic.h"
HAL_StatusTypeDef HAL_CORDIC_Configure(CORDIC_HandleTypeDef *hcordic, CORDIC_ConfigTypeDef *sConfig);
HAL_StatusTypeDef HAL_CORDIC_Calculate(CORDIC_HandleTypeDef *hcordic, int32_t *pInBuff, int32_t *pOutBuff, uint32_t NbCalc, uint32_t Timeout);
HAL_StatusTypeDef HAL_CORDIC_CalculateZO(CORDIC_HandleTypeDef *hcordic, int32_t *pInBuff, int32_t *pOutBuff, uint32_t NbCalc, uint32_t Timeout);
HAL_StatusTypeDef HAL_CORDIC_Calculate_IT(CORDIC_HandleTypeDef *hcordic, int32_t *pInBuff, int32_t *pOutBuff, uint32_t NbCalc);
HAL_StatusTypeDef HAL_CORDIC_Calculate_DMA(CORDIC_HandleTypeDef *hcordic, int32_t *pInBuff, int32_t *pOutBuff, uint32_t NbCalc, uint32_t DMADirection);
# 554 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cordic.h"
void HAL_CORDIC_ErrorCallback(CORDIC_HandleTypeDef *hcordic);
void HAL_CORDIC_CalculateCpltCallback(CORDIC_HandleTypeDef *hcordic);
# 564 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cordic.h"
void HAL_CORDIC_IRQHandler(CORDIC_HandleTypeDef *hcordic);
# 573 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_cordic.h"
HAL_CORDIC_StateTypeDef HAL_CORDIC_GetState(CORDIC_HandleTypeDef *hcordic);
uint32_t HAL_CORDIC_GetError(CORDIC_HandleTypeDef *hcordic);
# 269 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2



# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_crc.h" 1
# 47 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_crc.h"
typedef enum
{
  HAL_CRC_STATE_RESET = 0x00U,
  HAL_CRC_STATE_READY = 0x01U,
  HAL_CRC_STATE_BUSY = 0x02U,
  HAL_CRC_STATE_TIMEOUT = 0x03U,
  HAL_CRC_STATE_ERROR = 0x04U
} HAL_CRC_StateTypeDef;




typedef struct
{
  uint8_t DefaultPolynomialUse;





  uint8_t DefaultInitValueUse;




  uint32_t GeneratingPolynomial;




  uint32_t CRCLength;






  uint32_t InitValue;


  uint32_t InputDataInversionMode;






  uint32_t OutputDataInversionMode;



} CRC_InitTypeDef;




typedef struct
{
  CRC_TypeDef *Instance;

  CRC_InitTypeDef Init;

  HAL_LockTypeDef Lock;

  volatile HAL_CRC_StateTypeDef State;

  uint32_t InputDataFormat;







} CRC_HandleTypeDef;
# 288 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_crc.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_crc_ex.h" 1
# 127 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_crc_ex.h"
HAL_StatusTypeDef HAL_CRCEx_Polynomial_Set(CRC_HandleTypeDef *hcrc, uint32_t Pol, uint32_t PolyLength);
HAL_StatusTypeDef HAL_CRCEx_Input_Data_Reverse(CRC_HandleTypeDef *hcrc, uint32_t InputReverseMode);
HAL_StatusTypeDef HAL_CRCEx_Output_Data_Reverse(CRC_HandleTypeDef *hcrc, uint32_t OutputReverseMode);
# 289 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_crc.h" 2
# 299 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_crc.h"
HAL_StatusTypeDef HAL_CRC_Init(CRC_HandleTypeDef *hcrc);
HAL_StatusTypeDef HAL_CRC_DeInit(CRC_HandleTypeDef *hcrc);
void HAL_CRC_MspInit(CRC_HandleTypeDef *hcrc);
void HAL_CRC_MspDeInit(CRC_HandleTypeDef *hcrc);
# 311 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_crc.h"
uint32_t HAL_CRC_Accumulate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength);
uint32_t HAL_CRC_Calculate(CRC_HandleTypeDef *hcrc, uint32_t pBuffer[], uint32_t BufferLength);
# 321 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_crc.h"
HAL_CRC_StateTypeDef HAL_CRC_GetState(CRC_HandleTypeDef *hcrc);
# 273 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2
# 284 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h"
# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_exti.h" 1
# 45 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_exti.h"
typedef enum
{
  HAL_EXTI_COMMON_CB_ID = 0x00UL
} EXTI_CallbackIDTypeDef;





typedef struct
{
  uint32_t Line;
  void (* PendingCallback)(void);
} EXTI_HandleTypeDef;




typedef struct
{
  uint32_t Line;

  uint32_t Mode;

  uint32_t Trigger;

  uint32_t GPIOSel;


} EXTI_ConfigTypeDef;
# 276 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_exti.h"
HAL_StatusTypeDef HAL_EXTI_SetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_GetConfigLine(EXTI_HandleTypeDef *hexti, EXTI_ConfigTypeDef *pExtiConfig);
HAL_StatusTypeDef HAL_EXTI_ClearConfigLine(EXTI_HandleTypeDef *hexti);
HAL_StatusTypeDef HAL_EXTI_RegisterCallback(EXTI_HandleTypeDef *hexti, EXTI_CallbackIDTypeDef CallbackID, void (*pPendingCbfn)(void));
HAL_StatusTypeDef HAL_EXTI_GetHandle(EXTI_HandleTypeDef *hexti, uint32_t ExtiLine);
# 290 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_exti.h"
void HAL_EXTI_IRQHandler(EXTI_HandleTypeDef *hexti);
uint32_t HAL_EXTI_GetPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void HAL_EXTI_ClearPending(EXTI_HandleTypeDef *hexti, uint32_t Edge);
void HAL_EXTI_GenerateSWI(EXTI_HandleTypeDef *hexti);
# 285 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2



# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fdcan.h" 1
# 49 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fdcan.h"
typedef enum
{
  HAL_FDCAN_STATE_RESET = 0x00U,
  HAL_FDCAN_STATE_READY = 0x01U,
  HAL_FDCAN_STATE_BUSY = 0x02U,
  HAL_FDCAN_STATE_ERROR = 0x03U
} HAL_FDCAN_StateTypeDef;




typedef struct
{
  uint32_t ClockDivider;





  uint32_t FrameFormat;


  uint32_t Mode;


  FunctionalState AutoRetransmission;


  FunctionalState TransmitPause;


  FunctionalState ProtocolException;


  uint32_t NominalPrescaler;



  uint32_t NominalSyncJumpWidth;




  uint32_t NominalTimeSeg1;


  uint32_t NominalTimeSeg2;


  uint32_t DataPrescaler;



  uint32_t DataSyncJumpWidth;




  uint32_t DataTimeSeg1;


  uint32_t DataTimeSeg2;


  uint32_t StdFiltersNbr;


  uint32_t ExtFiltersNbr;


  uint32_t TxFifoQueueMode;


} FDCAN_InitTypeDef;




typedef struct
{
  uint32_t IdType;


  uint32_t FilterIndex;




  uint32_t FilterType;




  uint32_t FilterConfig;


  uint32_t FilterID1;




  uint32_t FilterID2;




} FDCAN_FilterTypeDef;




typedef struct
{
  uint32_t Identifier;




  uint32_t IdType;



  uint32_t TxFrameType;


  uint32_t DataLength;


  uint32_t ErrorStateIndicator;


  uint32_t BitRateSwitch;



  uint32_t FDFormat;



  uint32_t TxEventFifoControl;


  uint32_t MessageMarker;



} FDCAN_TxHeaderTypeDef;




typedef struct
{
  uint32_t Identifier;




  uint32_t IdType;


  uint32_t RxFrameType;


  uint32_t DataLength;


  uint32_t ErrorStateIndicator;


  uint32_t BitRateSwitch;



  uint32_t FDFormat;



  uint32_t RxTimestamp;



  uint32_t FilterIndex;




  uint32_t IsFilterMatchingFrame;




} FDCAN_RxHeaderTypeDef;




typedef struct
{
  uint32_t Identifier;




  uint32_t IdType;


  uint32_t TxFrameType;


  uint32_t DataLength;


  uint32_t ErrorStateIndicator;


  uint32_t BitRateSwitch;



  uint32_t FDFormat;



  uint32_t TxTimestamp;



  uint32_t MessageMarker;



  uint32_t EventType;


} FDCAN_TxEventFifoTypeDef;




typedef struct
{
  uint32_t FilterList;




  uint32_t FilterIndex;




  uint32_t MessageStorage;


  uint32_t MessageIndex;






} FDCAN_HpMsgStatusTypeDef;




typedef struct
{
  uint32_t LastErrorCode;


  uint32_t DataLastErrorCode;



  uint32_t Activity;


  uint32_t ErrorPassive;




  uint32_t Warning;





  uint32_t BusOff;




  uint32_t RxESIflag;




  uint32_t RxBRSflag;




  uint32_t RxFDFflag;




  uint32_t ProtocolException;




  uint32_t TDCvalue;


} FDCAN_ProtocolStatusTypeDef;




typedef struct
{
  uint32_t TxErrorCnt;


  uint32_t RxErrorCnt;


  uint32_t RxErrorPassive;





  uint32_t ErrorLogging;





} FDCAN_ErrorCountersTypeDef;




typedef struct
{
  uint32_t StandardFilterSA;


  uint32_t ExtendedFilterSA;


  uint32_t RxFIFO0SA;


  uint32_t RxFIFO1SA;


  uint32_t TxEventFIFOSA;


  uint32_t TxFIFOQSA;


} FDCAN_MsgRamAddressTypeDef;





typedef struct __FDCAN_HandleTypeDef



{
  FDCAN_GlobalTypeDef *Instance;

  FDCAN_InitTypeDef Init;

  FDCAN_MsgRamAddressTypeDef msgRam;

  uint32_t LatestTxFifoQRequest;


  volatile HAL_FDCAN_StateTypeDef State;

  HAL_LockTypeDef Lock;

  volatile uint32_t ErrorCode;


  void (* TxEventFifoCallback)(struct __FDCAN_HandleTypeDef *hfdcan, uint32_t TxEventFifoITs);
  void (* RxFifo0Callback)(struct __FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
  void (* RxFifo1Callback)(struct __FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs);
  void (* TxFifoEmptyCallback)(struct __FDCAN_HandleTypeDef *hfdcan);
  void (* TxBufferCompleteCallback)(struct __FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes);
  void (* TxBufferAbortCallback)(struct __FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes);
  void (* HighPriorityMessageCallback)(struct __FDCAN_HandleTypeDef *hfdcan);
  void (* TimestampWraparoundCallback)(struct __FDCAN_HandleTypeDef *hfdcan);
  void (* TimeoutOccurredCallback)(struct __FDCAN_HandleTypeDef *hfdcan);
  void (* ErrorCallback)(struct __FDCAN_HandleTypeDef *hfdcan);
  void (* ErrorStatusCallback)(struct __FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs);

  void (* MspInitCallback)(struct __FDCAN_HandleTypeDef *hfdcan);
  void (* MspDeInitCallback)(struct __FDCAN_HandleTypeDef *hfdcan);



} FDCAN_HandleTypeDef;





typedef enum
{
  HAL_FDCAN_TX_FIFO_EMPTY_CB_ID = 0x00U,
  HAL_FDCAN_HIGH_PRIO_MESSAGE_CB_ID = 0x01U,
  HAL_FDCAN_TIMESTAMP_WRAPAROUND_CB_ID = 0x02U,
  HAL_FDCAN_TIMEOUT_OCCURRED_CB_ID = 0x03U,
  HAL_FDCAN_ERROR_CALLBACK_CB_ID = 0x04U,

  HAL_FDCAN_MSPINIT_CB_ID = 0x05U,
  HAL_FDCAN_MSPDEINIT_CB_ID = 0x06U,

} HAL_FDCAN_CallbackIDTypeDef;




typedef void (*pFDCAN_CallbackTypeDef)(FDCAN_HandleTypeDef *hfdcan);
typedef void (*pFDCAN_TxEventFifoCallbackTypeDef)(FDCAN_HandleTypeDef *hfdcan, uint32_t TxEventFifoITs);
typedef void (*pFDCAN_RxFifo0CallbackTypeDef)(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
typedef void (*pFDCAN_RxFifo1CallbackTypeDef)(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs);
typedef void (*pFDCAN_TxBufferCompleteCallbackTypeDef)(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes);
typedef void (*pFDCAN_TxBufferAbortCallbackTypeDef)(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes);
typedef void (*pFDCAN_ErrorStatusCallbackTypeDef)(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs);
# 1129 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fdcan.h"
HAL_StatusTypeDef HAL_FDCAN_Init(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_DeInit(FDCAN_HandleTypeDef *hfdcan);
void HAL_FDCAN_MspInit(FDCAN_HandleTypeDef *hfdcan);
void HAL_FDCAN_MspDeInit(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_EnterPowerDownMode(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_ExitPowerDownMode(FDCAN_HandleTypeDef *hfdcan);



HAL_StatusTypeDef HAL_FDCAN_RegisterCallback(FDCAN_HandleTypeDef *hfdcan, HAL_FDCAN_CallbackIDTypeDef CallbackID,
                                             pFDCAN_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_FDCAN_UnRegisterCallback(FDCAN_HandleTypeDef *hfdcan, HAL_FDCAN_CallbackIDTypeDef CallbackID);
HAL_StatusTypeDef HAL_FDCAN_RegisterTxEventFifoCallback(FDCAN_HandleTypeDef *hfdcan,
                                                        pFDCAN_TxEventFifoCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_FDCAN_UnRegisterTxEventFifoCallback(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_RegisterRxFifo0Callback(FDCAN_HandleTypeDef *hfdcan,
                                                    pFDCAN_RxFifo0CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_FDCAN_UnRegisterRxFifo0Callback(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_RegisterRxFifo1Callback(FDCAN_HandleTypeDef *hfdcan,
                                                    pFDCAN_RxFifo1CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_FDCAN_UnRegisterRxFifo1Callback(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_RegisterTxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan,
                                                             pFDCAN_TxBufferCompleteCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_FDCAN_UnRegisterTxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_RegisterTxBufferAbortCallback(FDCAN_HandleTypeDef *hfdcan,
                                                          pFDCAN_TxBufferAbortCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_FDCAN_UnRegisterTxBufferAbortCallback(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_RegisterErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan,
                                                        pFDCAN_ErrorStatusCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_FDCAN_UnRegisterErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan);
# 1168 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fdcan.h"
HAL_StatusTypeDef HAL_FDCAN_ConfigFilter(FDCAN_HandleTypeDef *hfdcan, FDCAN_FilterTypeDef *sFilterConfig);
HAL_StatusTypeDef HAL_FDCAN_ConfigGlobalFilter(FDCAN_HandleTypeDef *hfdcan, uint32_t NonMatchingStd,
                                               uint32_t NonMatchingExt, uint32_t RejectRemoteStd,
                                               uint32_t RejectRemoteExt);
HAL_StatusTypeDef HAL_FDCAN_ConfigExtendedIdMask(FDCAN_HandleTypeDef *hfdcan, uint32_t Mask);
HAL_StatusTypeDef HAL_FDCAN_ConfigRxFifoOverwrite(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo, uint32_t OperationMode);
HAL_StatusTypeDef HAL_FDCAN_ConfigRamWatchdog(FDCAN_HandleTypeDef *hfdcan, uint32_t CounterStartValue);
HAL_StatusTypeDef HAL_FDCAN_ConfigTimestampCounter(FDCAN_HandleTypeDef *hfdcan, uint32_t TimestampPrescaler);
HAL_StatusTypeDef HAL_FDCAN_EnableTimestampCounter(FDCAN_HandleTypeDef *hfdcan, uint32_t TimestampOperation);
HAL_StatusTypeDef HAL_FDCAN_DisableTimestampCounter(FDCAN_HandleTypeDef *hfdcan);
uint16_t HAL_FDCAN_GetTimestampCounter(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_ResetTimestampCounter(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_ConfigTimeoutCounter(FDCAN_HandleTypeDef *hfdcan, uint32_t TimeoutOperation,
                                                 uint32_t TimeoutPeriod);
HAL_StatusTypeDef HAL_FDCAN_EnableTimeoutCounter(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_DisableTimeoutCounter(FDCAN_HandleTypeDef *hfdcan);
uint16_t HAL_FDCAN_GetTimeoutCounter(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_ResetTimeoutCounter(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_ConfigTxDelayCompensation(FDCAN_HandleTypeDef *hfdcan, uint32_t TdcOffset,
                                                      uint32_t TdcFilter);
HAL_StatusTypeDef HAL_FDCAN_EnableTxDelayCompensation(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_DisableTxDelayCompensation(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_EnableISOMode(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_DisableISOMode(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_EnableEdgeFiltering(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_DisableEdgeFiltering(FDCAN_HandleTypeDef *hfdcan);
# 1202 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fdcan.h"
HAL_StatusTypeDef HAL_FDCAN_Start(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_Stop(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_AddMessageToTxFifoQ(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxHeaderTypeDef *pTxHeader,
                                                uint8_t *pTxData);
uint32_t HAL_FDCAN_GetLatestTxFifoQRequestBuffer(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_AbortTxRequest(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndex);
HAL_StatusTypeDef HAL_FDCAN_GetRxMessage(FDCAN_HandleTypeDef *hfdcan, uint32_t RxLocation,
                                         FDCAN_RxHeaderTypeDef *pRxHeader, uint8_t *pRxData);
HAL_StatusTypeDef HAL_FDCAN_GetTxEvent(FDCAN_HandleTypeDef *hfdcan, FDCAN_TxEventFifoTypeDef *pTxEvent);
HAL_StatusTypeDef HAL_FDCAN_GetHighPriorityMessageStatus(FDCAN_HandleTypeDef *hfdcan,
                                                         FDCAN_HpMsgStatusTypeDef *HpMsgStatus);
HAL_StatusTypeDef HAL_FDCAN_GetProtocolStatus(FDCAN_HandleTypeDef *hfdcan, FDCAN_ProtocolStatusTypeDef *ProtocolStatus);
HAL_StatusTypeDef HAL_FDCAN_GetErrorCounters(FDCAN_HandleTypeDef *hfdcan, FDCAN_ErrorCountersTypeDef *ErrorCounters);
uint32_t HAL_FDCAN_IsTxBufferMessagePending(FDCAN_HandleTypeDef *hfdcan, uint32_t TxBufferIndex);
uint32_t HAL_FDCAN_GetRxFifoFillLevel(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo);
uint32_t HAL_FDCAN_GetTxFifoFreeLevel(FDCAN_HandleTypeDef *hfdcan);
uint32_t HAL_FDCAN_IsRestrictedOperationMode(FDCAN_HandleTypeDef *hfdcan);
HAL_StatusTypeDef HAL_FDCAN_ExitRestrictedOperationMode(FDCAN_HandleTypeDef *hfdcan);
# 1228 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fdcan.h"
HAL_StatusTypeDef HAL_FDCAN_ConfigInterruptLines(FDCAN_HandleTypeDef *hfdcan, uint32_t ITList, uint32_t InterruptLine);
HAL_StatusTypeDef HAL_FDCAN_ActivateNotification(FDCAN_HandleTypeDef *hfdcan, uint32_t ActiveITs,
                                                 uint32_t BufferIndexes);
HAL_StatusTypeDef HAL_FDCAN_DeactivateNotification(FDCAN_HandleTypeDef *hfdcan, uint32_t InactiveITs);
void HAL_FDCAN_IRQHandler(FDCAN_HandleTypeDef *hfdcan);
# 1241 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fdcan.h"
void HAL_FDCAN_TxEventFifoCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t TxEventFifoITs);
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs);
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs);
void HAL_FDCAN_TxFifoEmptyCallback(FDCAN_HandleTypeDef *hfdcan);
void HAL_FDCAN_TxBufferCompleteCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes);
void HAL_FDCAN_TxBufferAbortCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t BufferIndexes);
void HAL_FDCAN_HighPriorityMessageCallback(FDCAN_HandleTypeDef *hfdcan);
void HAL_FDCAN_TimestampWraparoundCallback(FDCAN_HandleTypeDef *hfdcan);
void HAL_FDCAN_TimeoutOccurredCallback(FDCAN_HandleTypeDef *hfdcan);
void HAL_FDCAN_ErrorCallback(FDCAN_HandleTypeDef *hfdcan);
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs);
# 1260 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fdcan.h"
uint32_t HAL_FDCAN_GetError(FDCAN_HandleTypeDef *hfdcan);
HAL_FDCAN_StateTypeDef HAL_FDCAN_GetState(FDCAN_HandleTypeDef *hfdcan);
# 289 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2



# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash.h" 1
# 46 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash.h"
typedef struct
{
  uint32_t TypeErase;

  uint32_t Banks;


  uint32_t Page;


  uint32_t NbPages;

} FLASH_EraseInitTypeDef;




typedef struct
{
  uint32_t OptionType;

  uint32_t WRPArea;


  uint32_t WRPStartOffset;

  uint32_t WRPEndOffset;

  uint32_t RDPLevel;

  uint32_t USERType;

  uint32_t USERConfig;
# 87 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash.h"
  uint32_t PCROPConfig;


  uint32_t PCROPStartAddr;


  uint32_t PCROPEndAddr;

  uint32_t BootEntryPoint;

  uint32_t SecBank;





  uint32_t SecSize;



} FLASH_OBProgramInitTypeDef;




typedef enum
{
  FLASH_PROC_NONE = 0,
  FLASH_PROC_PAGE_ERASE,
  FLASH_PROC_MASS_ERASE,
  FLASH_PROC_PROGRAM,
  FLASH_PROC_PROGRAM_LAST
} FLASH_ProcedureTypeDef;




typedef enum
{
  FLASH_CACHE_DISABLED = 0,
  FLASH_CACHE_ICACHE_ENABLED,
  FLASH_CACHE_DCACHE_ENABLED,
  FLASH_CACHE_ICACHE_DCACHE_ENABLED
} FLASH_CacheTypeDef;




typedef struct
{
  HAL_LockTypeDef Lock;
  volatile uint32_t ErrorCode;
  volatile FLASH_ProcedureTypeDef ProcedureOnGoing;
  volatile uint32_t Address;
  volatile uint32_t Bank;
  volatile uint32_t Page;
  volatile uint32_t NbPagesToErase;
  volatile FLASH_CacheTypeDef CacheToReactivate;
} FLASH_ProcessTypeDef;
# 786 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash_ex.h" 1
# 53 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash_ex.h"
HAL_StatusTypeDef HAL_FLASHEx_Erase(FLASH_EraseInitTypeDef *pEraseInit, uint32_t *PageError);
HAL_StatusTypeDef HAL_FLASHEx_Erase_IT(FLASH_EraseInitTypeDef *pEraseInit);
HAL_StatusTypeDef HAL_FLASHEx_OBProgram(FLASH_OBProgramInitTypeDef *pOBInit);
void HAL_FLASHEx_OBGetConfig(FLASH_OBProgramInitTypeDef *pOBInit);
HAL_StatusTypeDef HAL_FLASHEx_EnableSecMemProtection(uint32_t Bank);
void HAL_FLASHEx_EnableDebugger(void);
void HAL_FLASHEx_DisableDebugger(void);
# 71 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash_ex.h"
void FLASH_PageErase(uint32_t Page, uint32_t Banks);
void FLASH_FlushCaches(void);
# 787 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash.h" 2
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash_ramfunc.h" 1
# 49 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash_ramfunc.h"
__attribute__((section(".RamFunc"))) HAL_StatusTypeDef HAL_FLASHEx_EnableRunPowerDown(void);
__attribute__((section(".RamFunc"))) HAL_StatusTypeDef HAL_FLASHEx_DisableRunPowerDown(void);

__attribute__((section(".RamFunc"))) HAL_StatusTypeDef HAL_FLASHEx_OB_DBankConfig(uint32_t DBankConfig);
# 788 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash.h" 2





extern FLASH_ProcessTypeDef pFlash;
# 807 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash.h"
HAL_StatusTypeDef HAL_FLASH_Program(uint32_t TypeProgram, uint32_t Address, uint64_t Data);
HAL_StatusTypeDef HAL_FLASH_Program_IT(uint32_t TypeProgram, uint32_t Address, uint64_t Data);

void HAL_FLASH_IRQHandler(void);

void HAL_FLASH_EndOfOperationCallback(uint32_t ReturnValue);
void HAL_FLASH_OperationErrorCallback(uint32_t ReturnValue);
# 822 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash.h"
HAL_StatusTypeDef HAL_FLASH_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_Lock(void);

HAL_StatusTypeDef HAL_FLASH_OB_Unlock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Lock(void);
HAL_StatusTypeDef HAL_FLASH_OB_Launch(void);
# 836 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash.h"
uint32_t HAL_FLASH_GetError(void);
# 848 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_flash.h"
HAL_StatusTypeDef FLASH_WaitForLastOperation(uint32_t Timeout);
# 293 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2



# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fmac.h" 1
# 47 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fmac.h"
typedef enum
{
  HAL_FMAC_STATE_RESET = 0x00U,
  HAL_FMAC_STATE_READY = 0x20U,
  HAL_FMAC_STATE_BUSY = 0x24U,
  HAL_FMAC_STATE_BUSY_RD = 0x25U,
  HAL_FMAC_STATE_BUSY_WR = 0x26U,
  HAL_FMAC_STATE_TIMEOUT = 0xA0U,
  HAL_FMAC_STATE_ERROR = 0xE0U
} HAL_FMAC_StateTypeDef;







typedef struct

{
  FMAC_TypeDef *Instance;

  uint32_t FilterParam;


  uint8_t InputAccess;


  uint8_t OutputAccess;


  int16_t *pInput;

  uint16_t InputCurrentSize;

  uint16_t *pInputSize;


  int16_t *pOutput;

  uint16_t OutputCurrentSize;

  uint16_t *pOutputSize;


  DMA_HandleTypeDef *hdmaIn;

  DMA_HandleTypeDef *hdmaOut;

  DMA_HandleTypeDef *hdmaPreload;
# 119 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fmac.h"
  HAL_LockTypeDef Lock;

  volatile HAL_FMAC_StateTypeDef State;


  volatile HAL_FMAC_StateTypeDef RdState;


  volatile HAL_FMAC_StateTypeDef WrState;


  volatile uint32_t ErrorCode;


} FMAC_HandleTypeDef;
# 163 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fmac.h"
typedef struct
{
  uint8_t InputBaseAddress;



  uint8_t InputBufferSize;


  uint32_t InputThreshold;



  uint8_t CoeffBaseAddress;



  uint8_t CoeffBufferSize;


  uint8_t OutputBaseAddress;



  uint8_t OutputBufferSize;


  uint32_t OutputThreshold;



  int16_t *pCoeffA;


  uint8_t CoeffASize;

  int16_t *pCoeffB;


  uint8_t CoeffBSize;

  uint8_t InputAccess;


  uint8_t OutputAccess;


  uint32_t Clip;




  uint32_t Filter;


  uint8_t P;

  uint8_t Q;

  uint8_t R;

} FMAC_FilterConfigTypeDef;
# 581 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fmac.h"
HAL_StatusTypeDef HAL_FMAC_Init(FMAC_HandleTypeDef *hfmac);
HAL_StatusTypeDef HAL_FMAC_DeInit(FMAC_HandleTypeDef *hfmac);
void HAL_FMAC_MspInit(FMAC_HandleTypeDef *hfmac);
void HAL_FMAC_MspDeInit(FMAC_HandleTypeDef *hfmac);
# 600 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fmac.h"
HAL_StatusTypeDef HAL_FMAC_FilterConfig(FMAC_HandleTypeDef *hfmac, FMAC_FilterConfigTypeDef *pConfig);
HAL_StatusTypeDef HAL_FMAC_FilterConfig_DMA(FMAC_HandleTypeDef *hfmac, FMAC_FilterConfigTypeDef *pConfig);
HAL_StatusTypeDef HAL_FMAC_FilterPreload(FMAC_HandleTypeDef *hfmac, int16_t *pInput, uint8_t InputSize,
                                         int16_t *pOutput, uint8_t OutputSize);
HAL_StatusTypeDef HAL_FMAC_FilterPreload_DMA(FMAC_HandleTypeDef *hfmac, int16_t *pInput, uint8_t InputSize,
                                             int16_t *pOutput, uint8_t OutputSize);
HAL_StatusTypeDef HAL_FMAC_FilterStart(FMAC_HandleTypeDef *hfmac, int16_t *pOutput, uint16_t *pOutputSize);
HAL_StatusTypeDef HAL_FMAC_AppendFilterData(FMAC_HandleTypeDef *hfmac, int16_t *pInput, uint16_t *pInputSize);
HAL_StatusTypeDef HAL_FMAC_ConfigFilterOutputBuffer(FMAC_HandleTypeDef *hfmac, int16_t *pOutput, uint16_t *pOutputSize);
HAL_StatusTypeDef HAL_FMAC_PollFilterData(FMAC_HandleTypeDef *hfmac, uint32_t Timeout);
HAL_StatusTypeDef HAL_FMAC_FilterStop(FMAC_HandleTypeDef *hfmac);
# 619 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fmac.h"
void HAL_FMAC_ErrorCallback(FMAC_HandleTypeDef *hfmac);
void HAL_FMAC_HalfGetDataCallback(FMAC_HandleTypeDef *hfmac);
void HAL_FMAC_GetDataCallback(FMAC_HandleTypeDef *hfmac);
void HAL_FMAC_HalfOutputDataReadyCallback(FMAC_HandleTypeDef *hfmac);
void HAL_FMAC_OutputDataReadyCallback(FMAC_HandleTypeDef *hfmac);
void HAL_FMAC_FilterConfigCallback(FMAC_HandleTypeDef *hfmac);
void HAL_FMAC_FilterPreloadCallback(FMAC_HandleTypeDef *hfmac);
# 634 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fmac.h"
void HAL_FMAC_IRQHandler(FMAC_HandleTypeDef *hfmac);
# 643 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_fmac.h"
HAL_FMAC_StateTypeDef HAL_FMAC_GetState(FMAC_HandleTypeDef *hfmac);
uint32_t HAL_FMAC_GetError(FMAC_HandleTypeDef *hfmac);
# 297 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2
# 312 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h"
# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c.h" 1
# 48 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c.h"
typedef struct
{
  uint32_t Timing;



  uint32_t OwnAddress1;


  uint32_t AddressingMode;


  uint32_t DualAddressMode;


  uint32_t OwnAddress2;


  uint32_t OwnAddress2Masks;


  uint32_t GeneralCallMode;


  uint32_t NoStretchMode;


} I2C_InitTypeDef;
# 108 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c.h"
typedef enum
{
  HAL_I2C_STATE_RESET = 0x00U,
  HAL_I2C_STATE_READY = 0x20U,
  HAL_I2C_STATE_BUSY = 0x24U,
  HAL_I2C_STATE_BUSY_TX = 0x21U,
  HAL_I2C_STATE_BUSY_RX = 0x22U,
  HAL_I2C_STATE_LISTEN = 0x28U,
  HAL_I2C_STATE_BUSY_TX_LISTEN = 0x29U,

  HAL_I2C_STATE_BUSY_RX_LISTEN = 0x2AU,

  HAL_I2C_STATE_ABORT = 0x60U,
  HAL_I2C_STATE_TIMEOUT = 0xA0U,
  HAL_I2C_STATE_ERROR = 0xE0U

} HAL_I2C_StateTypeDef;
# 148 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c.h"
typedef enum
{
  HAL_I2C_MODE_NONE = 0x00U,
  HAL_I2C_MODE_MASTER = 0x10U,
  HAL_I2C_MODE_SLAVE = 0x20U,
  HAL_I2C_MODE_MEM = 0x40U

} HAL_I2C_ModeTypeDef;
# 186 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c.h"
typedef struct __I2C_HandleTypeDef
{
  I2C_TypeDef *Instance;

  I2C_InitTypeDef Init;

  uint8_t *pBuffPtr;

  uint16_t XferSize;

  volatile uint16_t XferCount;

  volatile uint32_t XferOptions;


  volatile uint32_t PreviousState;

  HAL_StatusTypeDef(*XferISR)(struct __I2C_HandleTypeDef *hi2c, uint32_t ITFlags, uint32_t ITSources);

  DMA_HandleTypeDef *hdmatx;

  DMA_HandleTypeDef *hdmarx;

  HAL_LockTypeDef Lock;

  volatile HAL_I2C_StateTypeDef State;

  volatile HAL_I2C_ModeTypeDef Mode;

  volatile uint32_t ErrorCode;

  volatile uint32_t AddrEventCount;


  void (* MasterTxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
  void (* MasterRxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
  void (* SlaveTxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
  void (* SlaveRxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
  void (* ListenCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
  void (* MemTxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
  void (* MemRxCpltCallback)(struct __I2C_HandleTypeDef *hi2c);
  void (* ErrorCallback)(struct __I2C_HandleTypeDef *hi2c);
  void (* AbortCpltCallback)(struct __I2C_HandleTypeDef *hi2c);

  void (* AddrCallback)(struct __I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode);

  void (* MspInitCallback)(struct __I2C_HandleTypeDef *hi2c);
  void (* MspDeInitCallback)(struct __I2C_HandleTypeDef *hi2c);


} I2C_HandleTypeDef;





typedef enum
{
  HAL_I2C_MASTER_TX_COMPLETE_CB_ID = 0x00U,
  HAL_I2C_MASTER_RX_COMPLETE_CB_ID = 0x01U,
  HAL_I2C_SLAVE_TX_COMPLETE_CB_ID = 0x02U,
  HAL_I2C_SLAVE_RX_COMPLETE_CB_ID = 0x03U,
  HAL_I2C_LISTEN_COMPLETE_CB_ID = 0x04U,
  HAL_I2C_MEM_TX_COMPLETE_CB_ID = 0x05U,
  HAL_I2C_MEM_RX_COMPLETE_CB_ID = 0x06U,
  HAL_I2C_ERROR_CB_ID = 0x07U,
  HAL_I2C_ABORT_CB_ID = 0x08U,

  HAL_I2C_MSPINIT_CB_ID = 0x09U,
  HAL_I2C_MSPDEINIT_CB_ID = 0x0AU

} HAL_I2C_CallbackIDTypeDef;




typedef void (*pI2C_CallbackTypeDef)(I2C_HandleTypeDef *hi2c);
typedef void (*pI2C_AddrCallbackTypeDef)(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode);
# 570 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c_ex.h" 1
# 96 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c_ex.h"
HAL_StatusTypeDef HAL_I2CEx_ConfigAnalogFilter(I2C_HandleTypeDef *hi2c, uint32_t AnalogFilter);
HAL_StatusTypeDef HAL_I2CEx_ConfigDigitalFilter(I2C_HandleTypeDef *hi2c, uint32_t DigitalFilter);







HAL_StatusTypeDef HAL_I2CEx_EnableWakeUp(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2CEx_DisableWakeUp(I2C_HandleTypeDef *hi2c);







void HAL_I2CEx_EnableFastModePlus(uint32_t ConfigFastModePlus);
void HAL_I2CEx_DisableFastModePlus(uint32_t ConfigFastModePlus);
# 571 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c.h" 2
# 581 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c.h"
HAL_StatusTypeDef HAL_I2C_Init(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DeInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MspDeInit(I2C_HandleTypeDef *hi2c);



HAL_StatusTypeDef HAL_I2C_RegisterCallback(I2C_HandleTypeDef *hi2c, HAL_I2C_CallbackIDTypeDef CallbackID,
                                           pI2C_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_I2C_UnRegisterCallback(I2C_HandleTypeDef *hi2c, HAL_I2C_CallbackIDTypeDef CallbackID);

HAL_StatusTypeDef HAL_I2C_RegisterAddrCallback(I2C_HandleTypeDef *hi2c, pI2C_AddrCallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_I2C_UnRegisterAddrCallback(I2C_HandleTypeDef *hi2c);
# 604 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c.h"
HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size,
                                          uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData, uint16_t Size,
                                         uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Slave_Receive(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                    uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                   uint16_t MemAddSize, uint8_t *pData, uint16_t Size, uint32_t Timeout);
HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint32_t Trials,
                                        uint32_t Timeout);


HAL_StatusTypeDef HAL_I2C_Master_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                             uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                            uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Write_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Read_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                      uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                                 uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                                uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_IT(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size,
                                               uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_EnableListen_IT(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_DisableListen_IT(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef HAL_I2C_Master_Abort_IT(I2C_HandleTypeDef *hi2c, uint16_t DevAddress);


HAL_StatusTypeDef HAL_I2C_Master_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                              uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Master_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                             uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Slave_Receive_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Write_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                        uint16_t MemAddSize, uint8_t *pData, uint16_t Size);
HAL_StatusTypeDef HAL_I2C_Mem_Read_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint16_t MemAddress,
                                       uint16_t MemAddSize, uint8_t *pData, uint16_t Size);

HAL_StatusTypeDef HAL_I2C_Master_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                                  uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Master_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *pData,
                                                 uint16_t Size, uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Transmit_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size,
                                                 uint32_t XferOptions);
HAL_StatusTypeDef HAL_I2C_Slave_Seq_Receive_DMA(I2C_HandleTypeDef *hi2c, uint8_t *pData, uint16_t Size,
                                                uint32_t XferOptions);
# 669 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c.h"
void HAL_I2C_EV_IRQHandler(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ER_IRQHandler(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_SlaveRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_AddrCallback(I2C_HandleTypeDef *hi2c, uint8_t TransferDirection, uint16_t AddrMatchCode);
void HAL_I2C_ListenCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_ErrorCallback(I2C_HandleTypeDef *hi2c);
void HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef *hi2c);
# 689 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_i2c.h"
HAL_I2C_StateTypeDef HAL_I2C_GetState(I2C_HandleTypeDef *hi2c);
HAL_I2C_ModeTypeDef HAL_I2C_GetMode(I2C_HandleTypeDef *hi2c);
uint32_t HAL_I2C_GetError(I2C_HandleTypeDef *hi2c);
# 313 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2
# 336 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h"
# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd.h" 1
# 29 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_usb.h" 1
# 48 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_usb.h"
typedef enum
{
  USB_DEVICE_MODE = 0
} USB_ModeTypeDef;




typedef struct
{
  uint32_t dev_endpoints;



  uint32_t speed;


  uint32_t ep0_mps;

  uint32_t phy_itface;


  uint32_t Sof_enable;

  uint32_t low_power_enable;

  uint32_t lpm_enable;

  uint32_t battery_charging_enable;
} USB_CfgTypeDef;

typedef struct
{
  uint8_t num;


  uint8_t is_in;


  uint8_t is_stall;


  uint8_t type;


  uint8_t data_pid_start;


  uint16_t pmaadress;


  uint16_t pmaaddr0;


  uint16_t pmaaddr1;


  uint8_t doublebuffer;


  uint16_t tx_fifo_num;



  uint32_t maxpacket;


  uint8_t *xfer_buff;

  uint32_t xfer_len;

  uint32_t xfer_count;

  uint32_t xfer_len_db;

  uint8_t xfer_fill_db;

} USB_EPTypeDef;
# 185 "../src/driver/stm32g4-v120/inc/stm32g4xx_ll_usb.h"
HAL_StatusTypeDef USB_CoreInit(USB_TypeDef *USBx, USB_CfgTypeDef cfg);
HAL_StatusTypeDef USB_DevInit(USB_TypeDef *USBx, USB_CfgTypeDef cfg);
HAL_StatusTypeDef USB_EnableGlobalInt(USB_TypeDef *USBx);
HAL_StatusTypeDef USB_DisableGlobalInt(USB_TypeDef *USBx);
HAL_StatusTypeDef USB_SetCurrentMode(USB_TypeDef *USBx, USB_ModeTypeDef mode);
HAL_StatusTypeDef USB_SetDevSpeed(USB_TypeDef *USBx, uint8_t speed);
HAL_StatusTypeDef USB_FlushRxFifo(USB_TypeDef *USBx);
HAL_StatusTypeDef USB_FlushTxFifo(USB_TypeDef *USBx, uint32_t num);
HAL_StatusTypeDef USB_ActivateEndpoint(USB_TypeDef *USBx, USB_EPTypeDef *ep);
HAL_StatusTypeDef USB_DeactivateEndpoint(USB_TypeDef *USBx, USB_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPStartXfer(USB_TypeDef *USBx, USB_EPTypeDef *ep);
HAL_StatusTypeDef USB_WritePacket(USB_TypeDef *USBx, uint8_t *src, uint8_t ch_ep_num, uint16_t len);
void *USB_ReadPacket(USB_TypeDef *USBx, uint8_t *dest, uint16_t len);
HAL_StatusTypeDef USB_EPSetStall(USB_TypeDef *USBx, USB_EPTypeDef *ep);
HAL_StatusTypeDef USB_EPClearStall(USB_TypeDef *USBx, USB_EPTypeDef *ep);
HAL_StatusTypeDef USB_SetDevAddress(USB_TypeDef *USBx, uint8_t address);
HAL_StatusTypeDef USB_DevConnect(USB_TypeDef *USBx);
HAL_StatusTypeDef USB_DevDisconnect(USB_TypeDef *USBx);
HAL_StatusTypeDef USB_StopDevice(USB_TypeDef *USBx);
HAL_StatusTypeDef USB_EP0_OutStart(USB_TypeDef *USBx, uint8_t *psetup);
uint32_t USB_ReadInterrupts(USB_TypeDef *USBx);
uint32_t USB_ReadDevAllOutEpInterrupt(USB_TypeDef *USBx);
uint32_t USB_ReadDevOutEPInterrupt(USB_TypeDef *USBx, uint8_t epnum);
uint32_t USB_ReadDevAllInEpInterrupt(USB_TypeDef *USBx);
uint32_t USB_ReadDevInEPInterrupt(USB_TypeDef *USBx, uint8_t epnum);
void USB_ClearInterrupts(USB_TypeDef *USBx, uint32_t interrupt);

HAL_StatusTypeDef USB_ActivateRemoteWakeup(USB_TypeDef *USBx);
HAL_StatusTypeDef USB_DeActivateRemoteWakeup(USB_TypeDef *USBx);
void USB_WritePMA(USB_TypeDef *USBx, uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes);
void USB_ReadPMA(USB_TypeDef *USBx, uint8_t *pbUsrBuf, uint16_t wPMABufAddr, uint16_t wNBytes);
# 30 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd.h" 2
# 49 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd.h"
typedef enum
{
  HAL_PCD_STATE_RESET = 0x00,
  HAL_PCD_STATE_READY = 0x01,
  HAL_PCD_STATE_ERROR = 0x02,
  HAL_PCD_STATE_BUSY = 0x03,
  HAL_PCD_STATE_TIMEOUT = 0x04
} PCD_StateTypeDef;


typedef enum
{
  LPM_L0 = 0x00,
  LPM_L1 = 0x01,
  LPM_L2 = 0x02,
  LPM_L3 = 0x03,
} PCD_LPM_StateTypeDef;

typedef enum
{
  PCD_LPM_L0_ACTIVE = 0x00,
  PCD_LPM_L1_ACTIVE = 0x01,
} PCD_LPM_MsgTypeDef;

typedef enum
{
  PCD_BCD_ERROR = 0xFF,
  PCD_BCD_CONTACT_DETECTION = 0xFE,
  PCD_BCD_STD_DOWNSTREAM_PORT = 0xFD,
  PCD_BCD_CHARGING_DOWNSTREAM_PORT = 0xFC,
  PCD_BCD_DEDICATED_CHARGING_PORT = 0xFB,
  PCD_BCD_DISCOVERY_COMPLETED = 0x00,

} PCD_BCD_MsgTypeDef;





typedef USB_TypeDef PCD_TypeDef;
typedef USB_CfgTypeDef PCD_InitTypeDef;
typedef USB_EPTypeDef PCD_EPTypeDef;
# 99 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd.h"
typedef struct

{
  PCD_TypeDef *Instance;
  PCD_InitTypeDef Init;
  volatile uint8_t USB_Address;
  PCD_EPTypeDef IN_ep[8];
  PCD_EPTypeDef OUT_ep[8];
  HAL_LockTypeDef Lock;
  volatile PCD_StateTypeDef State;
  volatile uint32_t ErrorCode;
  uint32_t Setup[12];
  PCD_LPM_StateTypeDef LPM_State;
  uint32_t BESL;


  uint32_t lpm_active;


  uint32_t battery_charging_active;

  void *pData;
# 141 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd.h"
} PCD_HandleTypeDef;







# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd_ex.h" 1
# 52 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd_ex.h"
HAL_StatusTypeDef HAL_PCDEx_PMAConfig(PCD_HandleTypeDef *hpcd,
                                       uint16_t ep_addr,
                                       uint16_t ep_kind,
                                       uint32_t pmaadress);


HAL_StatusTypeDef HAL_PCDEx_ActivateLPM(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCDEx_DeActivateLPM(PCD_HandleTypeDef *hpcd);


HAL_StatusTypeDef HAL_PCDEx_ActivateBCD(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCDEx_DeActivateBCD(PCD_HandleTypeDef *hpcd);
void HAL_PCDEx_BCD_VBUSDetect(PCD_HandleTypeDef *hpcd);

void HAL_PCDEx_LPM_Callback(PCD_HandleTypeDef *hpcd, PCD_LPM_MsgTypeDef msg);
void HAL_PCDEx_BCD_Callback(PCD_HandleTypeDef *hpcd, PCD_BCD_MsgTypeDef msg);
# 149 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd.h" 2
# 218 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd.h"
HAL_StatusTypeDef HAL_PCD_Init(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd);
void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd);
# 293 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd.h"
HAL_StatusTypeDef HAL_PCD_Start(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_Stop(PCD_HandleTypeDef *hpcd);
void HAL_PCD_IRQHandler(PCD_HandleTypeDef *hpcd);

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd);
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd);

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
# 317 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd.h"
HAL_StatusTypeDef HAL_PCD_DevConnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DevDisconnect(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_SetAddress(PCD_HandleTypeDef *hpcd, uint8_t address);
HAL_StatusTypeDef HAL_PCD_EP_Open(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint16_t ep_mps, uint8_t ep_type);
HAL_StatusTypeDef HAL_PCD_EP_Close(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Receive(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len);
HAL_StatusTypeDef HAL_PCD_EP_Transmit(PCD_HandleTypeDef *hpcd, uint8_t ep_addr, uint8_t *pBuf, uint32_t len);
uint32_t HAL_PCD_EP_GetRxCount(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_SetStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_ClrStall(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_EP_Flush(PCD_HandleTypeDef *hpcd, uint8_t ep_addr);
HAL_StatusTypeDef HAL_PCD_ActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
HAL_StatusTypeDef HAL_PCD_DeActivateRemoteWakeup(PCD_HandleTypeDef *hpcd);
# 338 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pcd.h"
PCD_StateTypeDef HAL_PCD_GetState(PCD_HandleTypeDef *hpcd);
# 337 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2



# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pwr.h" 1
# 48 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pwr.h"
typedef struct
{
  uint32_t PVDLevel;


  uint32_t Mode;

}PWR_PVDTypeDef;
# 342 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pwr.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pwr_ex.h" 1
# 50 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pwr_ex.h"
typedef struct
{
  uint32_t PVMType;

  uint32_t Mode;

}PWR_PVMTypeDef;
# 742 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pwr_ex.h"
uint32_t HAL_PWREx_GetVoltageRange(void);
HAL_StatusTypeDef HAL_PWREx_ControlVoltageScaling(uint32_t VoltageScaling);
void HAL_PWREx_EnableBatteryCharging(uint32_t ResistorSelection);
void HAL_PWREx_DisableBatteryCharging(void);
void HAL_PWREx_EnableInternalWakeUpLine(void);
void HAL_PWREx_DisableInternalWakeUpLine(void);
HAL_StatusTypeDef HAL_PWREx_EnableGPIOPullUp(uint32_t GPIO, uint32_t GPIONumber);
HAL_StatusTypeDef HAL_PWREx_DisableGPIOPullUp(uint32_t GPIO, uint32_t GPIONumber);
HAL_StatusTypeDef HAL_PWREx_EnableGPIOPullDown(uint32_t GPIO, uint32_t GPIONumber);
HAL_StatusTypeDef HAL_PWREx_DisableGPIOPullDown(uint32_t GPIO, uint32_t GPIONumber);
void HAL_PWREx_EnablePullUpPullDownConfig(void);
void HAL_PWREx_DisablePullUpPullDownConfig(void);
void HAL_PWREx_EnableSRAM2ContentRetention(void);
void HAL_PWREx_DisableSRAM2ContentRetention(void);

void HAL_PWREx_EnablePVM1(void);
void HAL_PWREx_DisablePVM1(void);


void HAL_PWREx_EnablePVM2(void);
void HAL_PWREx_DisablePVM2(void);

void HAL_PWREx_EnablePVM3(void);
void HAL_PWREx_DisablePVM3(void);
void HAL_PWREx_EnablePVM4(void);
void HAL_PWREx_DisablePVM4(void);
HAL_StatusTypeDef HAL_PWREx_ConfigPVM(PWR_PVMTypeDef *sConfigPVM);


void HAL_PWREx_EnableLowPowerRunMode(void);
HAL_StatusTypeDef HAL_PWREx_DisableLowPowerRunMode(void);
void HAL_PWREx_EnterSTOP0Mode(uint8_t STOPEntry);
void HAL_PWREx_EnterSTOP1Mode(uint8_t STOPEntry);
void HAL_PWREx_EnterSHUTDOWNMode(void);

void HAL_PWREx_PVD_PVM_IRQHandler(void);

void HAL_PWREx_PVM1Callback(void);


void HAL_PWREx_PVM2Callback(void);

void HAL_PWREx_PVM3Callback(void);
void HAL_PWREx_PVM4Callback(void);


void HAL_PWREx_EnableUCPDStandbyMode(void);
void HAL_PWREx_DisableUCPDStandbyMode(void);


void HAL_PWREx_EnableUCPDDeadBattery(void);
void HAL_PWREx_DisableUCPDDeadBattery(void);
# 343 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pwr.h" 2
# 355 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pwr.h"
void HAL_PWR_DeInit(void);
void HAL_PWR_EnableBkUpAccess(void);
void HAL_PWR_DisableBkUpAccess(void);
# 368 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_pwr.h"
HAL_StatusTypeDef HAL_PWR_ConfigPVD(PWR_PVDTypeDef *sConfigPVD);
void HAL_PWR_EnablePVD(void);
void HAL_PWR_DisablePVD(void);



void HAL_PWR_EnableWakeUpPin(uint32_t WakeUpPinPolarity);
void HAL_PWR_DisableWakeUpPin(uint32_t WakeUpPinx);


void HAL_PWR_EnterSLEEPMode(uint32_t Regulator, uint8_t SLEEPEntry);
void HAL_PWR_EnterSTOPMode(uint32_t Regulator, uint8_t STOPEntry);
void HAL_PWR_EnterSTANDBYMode(void);

void HAL_PWR_EnableSleepOnExit(void);
void HAL_PWR_DisableSleepOnExit(void);
void HAL_PWR_EnableSEVOnPend(void);
void HAL_PWR_DisableSEVOnPend(void);

void HAL_PWR_PVDCallback(void);
# 341 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2







# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rng.h" 1
# 51 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rng.h"
typedef struct
{
  uint32_t ClockErrorDetection;
} RNG_InitTypeDef;
# 63 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rng.h"
typedef enum
{
  HAL_RNG_STATE_RESET = 0x00U,
  HAL_RNG_STATE_READY = 0x01U,
  HAL_RNG_STATE_BUSY = 0x02U,
  HAL_RNG_STATE_TIMEOUT = 0x03U,
  HAL_RNG_STATE_ERROR = 0x04U

} HAL_RNG_StateTypeDef;
# 83 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rng.h"
typedef struct

{
  RNG_TypeDef *Instance;

  RNG_InitTypeDef Init;

  HAL_LockTypeDef Lock;

  volatile HAL_RNG_StateTypeDef State;

  volatile uint32_t ErrorCode;

  uint32_t RandomNumber;
# 106 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rng.h"
} RNG_HandleTypeDef;
# 295 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rng.h"
HAL_StatusTypeDef HAL_RNG_Init(RNG_HandleTypeDef *hrng);
HAL_StatusTypeDef HAL_RNG_DeInit(RNG_HandleTypeDef *hrng);
void HAL_RNG_MspInit(RNG_HandleTypeDef *hrng);
void HAL_RNG_MspDeInit(RNG_HandleTypeDef *hrng);
# 316 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rng.h"
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber(RNG_HandleTypeDef *hrng, uint32_t *random32bit);
HAL_StatusTypeDef HAL_RNG_GenerateRandomNumber_IT(RNG_HandleTypeDef *hrng);
uint32_t HAL_RNG_ReadLastRandomNumber(RNG_HandleTypeDef *hrng);

void HAL_RNG_IRQHandler(RNG_HandleTypeDef *hrng);
void HAL_RNG_ErrorCallback(RNG_HandleTypeDef *hrng);
void HAL_RNG_ReadyDataCallback(RNG_HandleTypeDef *hrng, uint32_t random32bit);
# 331 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_rng.h"
HAL_RNG_StateTypeDef HAL_RNG_GetState(RNG_HandleTypeDef *hrng);
uint32_t HAL_RNG_GetError(RNG_HandleTypeDef *hrng);
# 349 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2
# 376 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h"
# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h" 1
# 47 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
typedef struct
{
  uint32_t Prescaler;



  uint32_t CounterMode;


  uint32_t Period;





  uint32_t ClockDivision;


  uint32_t RepetitionCounter;
# 74 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
  uint32_t AutoReloadPreload;

} TIM_Base_InitTypeDef;




typedef struct
{
  uint32_t OCMode;


  uint32_t Pulse;




  uint32_t OCPolarity;


  uint32_t OCNPolarity;



  uint32_t OCFastMode;




  uint32_t OCIdleState;



  uint32_t OCNIdleState;


} TIM_OC_InitTypeDef;




typedef struct
{
  uint32_t OCMode;


  uint32_t Pulse;




  uint32_t OCPolarity;


  uint32_t OCNPolarity;



  uint32_t OCIdleState;



  uint32_t OCNIdleState;



  uint32_t ICPolarity;


  uint32_t ICSelection;


  uint32_t ICFilter;

} TIM_OnePulse_InitTypeDef;




typedef struct
{
  uint32_t ICPolarity;


  uint32_t ICSelection;


  uint32_t ICPrescaler;


  uint32_t ICFilter;

} TIM_IC_InitTypeDef;




typedef struct
{
  uint32_t EncoderMode;


  uint32_t IC1Polarity;


  uint32_t IC1Selection;


  uint32_t IC1Prescaler;


  uint32_t IC1Filter;


  uint32_t IC2Polarity;


  uint32_t IC2Selection;


  uint32_t IC2Prescaler;


  uint32_t IC2Filter;

} TIM_Encoder_InitTypeDef;




typedef struct
{
  uint32_t ClockSource;

  uint32_t ClockPolarity;

  uint32_t ClockPrescaler;

  uint32_t ClockFilter;

} TIM_ClockConfigTypeDef;




typedef struct
{
  uint32_t ClearInputState;

  uint32_t ClearInputSource;

  uint32_t ClearInputPolarity;

  uint32_t ClearInputPrescaler;

  uint32_t ClearInputFilter;

} TIM_ClearInputConfigTypeDef;






typedef struct
{
  uint32_t MasterOutputTrigger;

  uint32_t MasterOutputTrigger2;

  uint32_t MasterSlaveMode;






} TIM_MasterConfigTypeDef;




typedef struct
{
  uint32_t SlaveMode;

  uint32_t InputTrigger;

  uint32_t TriggerPolarity;

  uint32_t TriggerPrescaler;

  uint32_t TriggerFilter;


} TIM_SlaveConfigTypeDef;






typedef struct
{
  uint32_t OffStateRunMode;

  uint32_t OffStateIDLEMode;

  uint32_t LockLevel;

  uint32_t DeadTime;

  uint32_t BreakState;

  uint32_t BreakPolarity;

  uint32_t BreakFilter;

  uint32_t BreakAFMode;

  uint32_t Break2State;

  uint32_t Break2Polarity;

  uint32_t Break2Filter;

  uint32_t Break2AFMode;

  uint32_t AutomaticOutput;

} TIM_BreakDeadTimeConfigTypeDef;




typedef enum
{
  HAL_TIM_STATE_RESET = 0x00U,
  HAL_TIM_STATE_READY = 0x01U,
  HAL_TIM_STATE_BUSY = 0x02U,
  HAL_TIM_STATE_TIMEOUT = 0x03U,
  HAL_TIM_STATE_ERROR = 0x04U
} HAL_TIM_StateTypeDef;




typedef enum
{
  HAL_TIM_CHANNEL_STATE_RESET = 0x00U,
  HAL_TIM_CHANNEL_STATE_READY = 0x01U,
  HAL_TIM_CHANNEL_STATE_BUSY = 0x02U,
} HAL_TIM_ChannelStateTypeDef;




typedef enum
{
  HAL_DMA_BURST_STATE_RESET = 0x00U,
  HAL_DMA_BURST_STATE_READY = 0x01U,
  HAL_DMA_BURST_STATE_BUSY = 0x02U,
} HAL_TIM_DMABurstStateTypeDef;




typedef enum
{
  HAL_TIM_ACTIVE_CHANNEL_1 = 0x01U,
  HAL_TIM_ACTIVE_CHANNEL_2 = 0x02U,
  HAL_TIM_ACTIVE_CHANNEL_3 = 0x04U,
  HAL_TIM_ACTIVE_CHANNEL_4 = 0x08U,
  HAL_TIM_ACTIVE_CHANNEL_5 = 0x10U,
  HAL_TIM_ACTIVE_CHANNEL_6 = 0x20U,
  HAL_TIM_ACTIVE_CHANNEL_CLEARED = 0x00U
} HAL_TIM_ActiveChannel;





typedef struct __TIM_HandleTypeDef



{
  TIM_TypeDef *Instance;
  TIM_Base_InitTypeDef Init;
  HAL_TIM_ActiveChannel Channel;
  DMA_HandleTypeDef *hdma[7];

  HAL_LockTypeDef Lock;
  volatile HAL_TIM_StateTypeDef State;
  volatile HAL_TIM_ChannelStateTypeDef ChannelState[6];
  volatile HAL_TIM_ChannelStateTypeDef ChannelNState[4];
  volatile HAL_TIM_DMABurstStateTypeDef DMABurstState;


  void (* Base_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* Base_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* IC_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* IC_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* OC_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* OC_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* PWM_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* PWM_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* OnePulse_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* OnePulse_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* Encoder_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* Encoder_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* HallSensor_MspInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* HallSensor_MspDeInitCallback)(struct __TIM_HandleTypeDef *htim);
  void (* PeriodElapsedCallback)(struct __TIM_HandleTypeDef *htim);
  void (* PeriodElapsedHalfCpltCallback)(struct __TIM_HandleTypeDef *htim);
  void (* TriggerCallback)(struct __TIM_HandleTypeDef *htim);
  void (* TriggerHalfCpltCallback)(struct __TIM_HandleTypeDef *htim);
  void (* IC_CaptureCallback)(struct __TIM_HandleTypeDef *htim);
  void (* IC_CaptureHalfCpltCallback)(struct __TIM_HandleTypeDef *htim);
  void (* OC_DelayElapsedCallback)(struct __TIM_HandleTypeDef *htim);
  void (* PWM_PulseFinishedCallback)(struct __TIM_HandleTypeDef *htim);
  void (* PWM_PulseFinishedHalfCpltCallback)(struct __TIM_HandleTypeDef *htim);
  void (* ErrorCallback)(struct __TIM_HandleTypeDef *htim);
  void (* CommutationCallback)(struct __TIM_HandleTypeDef *htim);
  void (* CommutationHalfCpltCallback)(struct __TIM_HandleTypeDef *htim);
  void (* BreakCallback)(struct __TIM_HandleTypeDef *htim);
  void (* Break2Callback)(struct __TIM_HandleTypeDef *htim);
  void (* EncoderIndexCallback)(struct __TIM_HandleTypeDef *htim);
  void (* DirectionChangeCallback)(struct __TIM_HandleTypeDef *htim);
  void (* IndexErrorCallback)(struct __TIM_HandleTypeDef *htim);
  void (* TransitionErrorCallback)(struct __TIM_HandleTypeDef *htim);

} TIM_HandleTypeDef;





typedef enum
{
  HAL_TIM_BASE_MSPINIT_CB_ID = 0x00U
  , HAL_TIM_BASE_MSPDEINIT_CB_ID = 0x01U
  , HAL_TIM_IC_MSPINIT_CB_ID = 0x02U
  , HAL_TIM_IC_MSPDEINIT_CB_ID = 0x03U
  , HAL_TIM_OC_MSPINIT_CB_ID = 0x04U
  , HAL_TIM_OC_MSPDEINIT_CB_ID = 0x05U
  , HAL_TIM_PWM_MSPINIT_CB_ID = 0x06U
  , HAL_TIM_PWM_MSPDEINIT_CB_ID = 0x07U
  , HAL_TIM_ONE_PULSE_MSPINIT_CB_ID = 0x08U
  , HAL_TIM_ONE_PULSE_MSPDEINIT_CB_ID = 0x09U
  , HAL_TIM_ENCODER_MSPINIT_CB_ID = 0x0AU
  , HAL_TIM_ENCODER_MSPDEINIT_CB_ID = 0x0BU
  , HAL_TIM_HALL_SENSOR_MSPINIT_CB_ID = 0x0CU
  , HAL_TIM_HALL_SENSOR_MSPDEINIT_CB_ID = 0x0DU
  , HAL_TIM_PERIOD_ELAPSED_CB_ID = 0x0EU
  , HAL_TIM_PERIOD_ELAPSED_HALF_CB_ID = 0x0FU
  , HAL_TIM_TRIGGER_CB_ID = 0x10U
  , HAL_TIM_TRIGGER_HALF_CB_ID = 0x11U

  , HAL_TIM_IC_CAPTURE_CB_ID = 0x12U
  , HAL_TIM_IC_CAPTURE_HALF_CB_ID = 0x13U
  , HAL_TIM_OC_DELAY_ELAPSED_CB_ID = 0x14U
  , HAL_TIM_PWM_PULSE_FINISHED_CB_ID = 0x15U
  , HAL_TIM_PWM_PULSE_FINISHED_HALF_CB_ID = 0x16U
  , HAL_TIM_ERROR_CB_ID = 0x17U
  , HAL_TIM_COMMUTATION_CB_ID = 0x18U
  , HAL_TIM_COMMUTATION_HALF_CB_ID = 0x19U
  , HAL_TIM_BREAK_CB_ID = 0x1AU
  , HAL_TIM_BREAK2_CB_ID = 0x1BU
  , HAL_TIM_ENCODER_INDEX_CB_ID = 0x1CU
  , HAL_TIM_DIRECTION_CHANGE_CB_ID = 0x1DU
  , HAL_TIM_INDEX_ERROR_CB_ID = 0x1EU
  , HAL_TIM_TRANSITION_ERROR_CB_ID = 0x1FU
} HAL_TIM_CallbackIDTypeDef;




typedef void (*pTIM_CallbackTypeDef)(TIM_HandleTypeDef *htim);
# 2345 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
# 1 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim_ex.h" 1
# 48 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim_ex.h"
typedef struct
{
  uint32_t IC1Polarity;


  uint32_t IC1Prescaler;


  uint32_t IC1Filter;


  uint32_t Commutation_Delay;

} TIM_HallSensor_InitTypeDef;




typedef struct
{
  uint32_t Source;

  uint32_t Enable;

  uint32_t Polarity;

}
TIMEx_BreakInputConfigTypeDef;




typedef struct
{
  uint32_t Polarity;

  uint32_t Prescaler;

  uint32_t Filter;

  FunctionalState FirstIndexEnable;

  uint32_t Position;

  uint32_t Direction;

} TIMEx_EncoderIndexConfigTypeDef ;
# 1957 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim_ex.h"
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Init(TIM_HandleTypeDef *htim, TIM_HallSensor_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_DeInit(TIM_HandleTypeDef *htim);

void HAL_TIMEx_HallSensor_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIMEx_HallSensor_MspDeInit(TIM_HandleTypeDef *htim);


HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_IT(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIMEx_HallSensor_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_HallSensor_Stop_DMA(TIM_HandleTypeDef *htim);
# 1982 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim_ex.h"
HAL_StatusTypeDef HAL_TIMEx_OCN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);


HAL_StatusTypeDef HAL_TIMEx_OCN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);


HAL_StatusTypeDef HAL_TIMEx_OCN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_OCN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 2002 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim_ex.h"
HAL_StatusTypeDef HAL_TIMEx_PWMN_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);


HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIMEx_PWMN_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIMEx_PWMN_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 2021 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim_ex.h"
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel);


HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIMEx_OnePulseN_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
# 2036 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim_ex.h"
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent(TIM_HandleTypeDef *htim, uint32_t InputTrigger,
                                              uint32_t CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_IT(TIM_HandleTypeDef *htim, uint32_t InputTrigger,
                                                 uint32_t CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_ConfigCommutEvent_DMA(TIM_HandleTypeDef *htim, uint32_t InputTrigger,
                                                  uint32_t CommutationSource);
HAL_StatusTypeDef HAL_TIMEx_MasterConfigSynchronization(TIM_HandleTypeDef *htim,
                                                        TIM_MasterConfigTypeDef *sMasterConfig);
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakDeadTime(TIM_HandleTypeDef *htim,
                                                TIM_BreakDeadTimeConfigTypeDef *sBreakDeadTimeConfig);
HAL_StatusTypeDef HAL_TIMEx_ConfigBreakInput(TIM_HandleTypeDef *htim, uint32_t BreakInput,
                                             TIMEx_BreakInputConfigTypeDef *sBreakInputConfig);
HAL_StatusTypeDef HAL_TIMEx_GroupChannel5(TIM_HandleTypeDef *htim, uint32_t Channels);
HAL_StatusTypeDef HAL_TIMEx_RemapConfig(TIM_HandleTypeDef *htim, uint32_t Remap);
HAL_StatusTypeDef HAL_TIMEx_TISelection(TIM_HandleTypeDef *htim, uint32_t TISelection, uint32_t Channel);

HAL_StatusTypeDef HAL_TIMEx_DisarmBreakInput(TIM_HandleTypeDef *htim, uint32_t BreakInput);
HAL_StatusTypeDef HAL_TIMEx_ReArmBreakInput(TIM_HandleTypeDef *htim, uint32_t BreakInput);
HAL_StatusTypeDef HAL_TIMEx_DitheringEnable(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_DitheringDisable(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_OC_ConfigPulseOnCompare(TIM_HandleTypeDef *htim, uint32_t PulseWidthPrescaler,
                                                    uint32_t PulseWidth);
HAL_StatusTypeDef HAL_TIMEx_ConfigSlaveModePreload(TIM_HandleTypeDef *htim, uint32_t Source);
HAL_StatusTypeDef HAL_TIMEx_EnableSlaveModePreload(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_DisableSlaveModePreload(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_EnableDeadTimePreload(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_DisableDeadTimePreload(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_ConfigDeadTime(TIM_HandleTypeDef *htim, uint32_t Deadtime);
HAL_StatusTypeDef HAL_TIMEx_ConfigAsymmetricalDeadTime(TIM_HandleTypeDef *htim, uint32_t FallingDeadtime);
HAL_StatusTypeDef HAL_TIMEx_EnableAsymmetricalDeadTime(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_DisableAsymmetricalDeadTime(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_ConfigEncoderIndex(TIM_HandleTypeDef *htim,
                                               TIMEx_EncoderIndexConfigTypeDef *sEncoderIndexConfig);
HAL_StatusTypeDef HAL_TIMEx_EnableEncoderIndex(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_DisableEncoderIndex(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_EnableEncoderFirstIndex(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIMEx_DisableEncoderFirstIndex(TIM_HandleTypeDef *htim);
# 2082 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim_ex.h"
void HAL_TIMEx_CommutCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_CommutHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_BreakCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_Break2Callback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_EncoderIndexCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_DirectionChangeCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_IndexErrorCallback(TIM_HandleTypeDef *htim);
void HAL_TIMEx_TransitionErrorCallback(TIM_HandleTypeDef *htim);
# 2099 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim_ex.h"
HAL_TIM_StateTypeDef HAL_TIMEx_HallSensor_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_ChannelStateTypeDef HAL_TIMEx_GetChannelNState(TIM_HandleTypeDef *htim, uint32_t ChannelN);
# 2114 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim_ex.h"
void TIMEx_DMACommutationCplt(DMA_HandleTypeDef *hdma);
void TIMEx_DMACommutationHalfCplt(DMA_HandleTypeDef *hdma);
# 2346 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h" 2
# 2357 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Base_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_Base_Start(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_Base_Start_IT(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_Base_Stop_IT(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_Base_Start_DMA(TIM_HandleTypeDef *htim, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Base_Stop_DMA(TIM_HandleTypeDef *htim);
# 2379 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_OC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_OC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_OC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_OC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_OC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_OC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 2401 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_PWM_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_PWM_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_PWM_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_PWM_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_PWM_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 2423 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_IC_Init(TIM_HandleTypeDef *htim);
HAL_StatusTypeDef HAL_TIM_IC_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_IC_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_IC_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_IC_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_IC_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 2445 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_OnePulse_Init(TIM_HandleTypeDef *htim, uint32_t OnePulseMode);
HAL_StatusTypeDef HAL_TIM_OnePulse_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OnePulse_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_OnePulse_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_OnePulse_Start(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop(TIM_HandleTypeDef *htim, uint32_t OutputChannel);

HAL_StatusTypeDef HAL_TIM_OnePulse_Start_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
HAL_StatusTypeDef HAL_TIM_OnePulse_Stop_IT(TIM_HandleTypeDef *htim, uint32_t OutputChannel);
# 2464 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_Encoder_Init(TIM_HandleTypeDef *htim, TIM_Encoder_InitTypeDef *sConfig);
HAL_StatusTypeDef HAL_TIM_Encoder_DeInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Encoder_MspInit(TIM_HandleTypeDef *htim);
void HAL_TIM_Encoder_MspDeInit(TIM_HandleTypeDef *htim);

HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_Encoder_Start_IT(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_IT(TIM_HandleTypeDef *htim, uint32_t Channel);

HAL_StatusTypeDef HAL_TIM_Encoder_Start_DMA(TIM_HandleTypeDef *htim, uint32_t Channel, uint32_t *pData1,
                                            uint32_t *pData2, uint16_t Length);
HAL_StatusTypeDef HAL_TIM_Encoder_Stop_DMA(TIM_HandleTypeDef *htim, uint32_t Channel);
# 2487 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
void HAL_TIM_IRQHandler(TIM_HandleTypeDef *htim);
# 2497 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
HAL_StatusTypeDef HAL_TIM_OC_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_PWM_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_IC_ConfigChannel(TIM_HandleTypeDef *htim, TIM_IC_InitTypeDef *sConfig, uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_OnePulse_ConfigChannel(TIM_HandleTypeDef *htim, TIM_OnePulse_InitTypeDef *sConfig,
                                                 uint32_t OutputChannel, uint32_t InputChannel);
HAL_StatusTypeDef HAL_TIM_ConfigOCrefClear(TIM_HandleTypeDef *htim, TIM_ClearInputConfigTypeDef *sClearInputConfig,
                                           uint32_t Channel);
HAL_StatusTypeDef HAL_TIM_ConfigClockSource(TIM_HandleTypeDef *htim, TIM_ClockConfigTypeDef *sClockSourceConfig);
HAL_StatusTypeDef HAL_TIM_ConfigTI1Input(TIM_HandleTypeDef *htim, uint32_t TI1_Selection);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro(TIM_HandleTypeDef *htim, TIM_SlaveConfigTypeDef *sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_SlaveConfigSynchro_IT(TIM_HandleTypeDef *htim, TIM_SlaveConfigTypeDef *sSlaveConfig);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                              uint32_t BurstRequestSrc, uint32_t *BurstBuffer, uint32_t BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_MultiWriteStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                                   uint32_t BurstRequestSrc, uint32_t *BurstBuffer, uint32_t BurstLength,
                                                   uint32_t DataLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_WriteStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                             uint32_t BurstRequestSrc, uint32_t *BurstBuffer, uint32_t BurstLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_MultiReadStart(TIM_HandleTypeDef *htim, uint32_t BurstBaseAddress,
                                                  uint32_t BurstRequestSrc, uint32_t *BurstBuffer, uint32_t BurstLength,
                                                  uint32_t DataLength);
HAL_StatusTypeDef HAL_TIM_DMABurst_ReadStop(TIM_HandleTypeDef *htim, uint32_t BurstRequestSrc);
HAL_StatusTypeDef HAL_TIM_GenerateEvent(TIM_HandleTypeDef *htim, uint32_t EventSource);
uint32_t HAL_TIM_ReadCapturedValue(TIM_HandleTypeDef *htim, uint32_t Channel);
# 2531 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PeriodElapsedHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_OC_DelayElapsedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_IC_CaptureHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_PWM_PulseFinishedHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_TriggerHalfCpltCallback(TIM_HandleTypeDef *htim);
void HAL_TIM_ErrorCallback(TIM_HandleTypeDef *htim);



HAL_StatusTypeDef HAL_TIM_RegisterCallback(TIM_HandleTypeDef *htim, HAL_TIM_CallbackIDTypeDef CallbackID,
                                           pTIM_CallbackTypeDef pCallback);
HAL_StatusTypeDef HAL_TIM_UnRegisterCallback(TIM_HandleTypeDef *htim, HAL_TIM_CallbackIDTypeDef CallbackID);
# 2558 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
HAL_TIM_StateTypeDef HAL_TIM_Base_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OC_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_PWM_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_IC_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_OnePulse_GetState(TIM_HandleTypeDef *htim);
HAL_TIM_StateTypeDef HAL_TIM_Encoder_GetState(TIM_HandleTypeDef *htim);


HAL_TIM_ActiveChannel HAL_TIM_GetActiveChannel(TIM_HandleTypeDef *htim);
HAL_TIM_ChannelStateTypeDef HAL_TIM_GetChannelState(TIM_HandleTypeDef *htim, uint32_t Channel);
HAL_TIM_DMABurstStateTypeDef HAL_TIM_DMABurstState(TIM_HandleTypeDef *htim);
# 2582 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_hal_tim.h"
void TIM_Base_SetConfig(TIM_TypeDef *TIMx, TIM_Base_InitTypeDef *Structure);
void TIM_TI1_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ICPolarity, uint32_t TIM_ICSelection, uint32_t TIM_ICFilter);
void TIM_OC2_SetConfig(TIM_TypeDef *TIMx, TIM_OC_InitTypeDef *OC_Config);
void TIM_ETR_SetConfig(TIM_TypeDef *TIMx, uint32_t TIM_ExtTRGPrescaler,
                       uint32_t TIM_ExtTRGPolarity, uint32_t ExtTRGFilter);

void TIM_DMADelayPulseHalfCplt(DMA_HandleTypeDef *hdma);
void TIM_DMAError(DMA_HandleTypeDef *hdma);
void TIM_DMACaptureCplt(DMA_HandleTypeDef *hdma);
void TIM_DMACaptureHalfCplt(DMA_HandleTypeDef *hdma);
void TIM_CCxChannelCmd(TIM_TypeDef *TIMx, uint32_t Channel, uint32_t ChannelState);


void TIM_ResetCallback(TIM_HandleTypeDef *htim);
# 377 "../api\\../src/config/stm32hal_driver_cfg_of_amcbldc_v120.h" 2
# 133 "../src/driver/stm32g4-v120/inc/../../../config/stm32hal_driver_cfg.h" 2

# 1 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h" 1
# 72 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
typedef struct
{
  uint32_t Pin;


  uint32_t Mode;




  uint32_t Speed;




  uint32_t OutputType;




  uint32_t Pull;




  uint32_t Alternate;



} LL_GPIO_InitTypeDef;
# 286 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline void LL_GPIO_SetPinMode(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Mode)
{
  (((GPIOx->MODER)) = ((((((GPIOx->MODER))) & (~(((0x3UL << (0U)) << ((__CLZ(__builtin_arm_rbit(Pin))) * 2U))))) | ((Mode << ((__CLZ(__builtin_arm_rbit(Pin))) * 2U))))));
}
# 320 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline uint32_t LL_GPIO_GetPinMode(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->MODER) & (((0x3UL << (0U)) << ((__CLZ(__builtin_arm_rbit(Pin))) * 2U)))) >> ((__CLZ(__builtin_arm_rbit(Pin))) * 2U));

}
# 355 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline void LL_GPIO_SetPinOutputType(GPIO_TypeDef *GPIOx, uint32_t PinMask, uint32_t OutputType)
{
  (((GPIOx->OTYPER)) = ((((((GPIOx->OTYPER))) & (~(PinMask))) | ((PinMask * OutputType)))));
}
# 389 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline uint32_t LL_GPIO_GetPinOutputType(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->OTYPER) & (Pin)) >> (__CLZ(__builtin_arm_rbit(Pin))));
}
# 426 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline void LL_GPIO_SetPinSpeed(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Speed)
{
  (((GPIOx->OSPEEDR)) = ((((((GPIOx->OSPEEDR))) & (~(((0x3UL << (0U)) << ((__CLZ(__builtin_arm_rbit(Pin))) * 2U))))) | ((Speed << ((__CLZ(__builtin_arm_rbit(Pin))) * 2U))))));

}
# 463 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline uint32_t LL_GPIO_GetPinSpeed(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->OSPEEDR) & (((0x3UL << (0U)) << ((__CLZ(__builtin_arm_rbit(Pin))) * 2U)))) >> ((__CLZ(__builtin_arm_rbit(Pin))) * 2U));

}
# 497 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline void LL_GPIO_SetPinPull(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Pull)
{
  (((GPIOx->PUPDR)) = ((((((GPIOx->PUPDR))) & (~(((0x3UL << (0U)) << ((__CLZ(__builtin_arm_rbit(Pin))) * 2U))))) | ((Pull << ((__CLZ(__builtin_arm_rbit(Pin))) * 2U))))));
}
# 529 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline uint32_t LL_GPIO_GetPinPull(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->PUPDR) & (((0x3UL << (0U)) << ((__CLZ(__builtin_arm_rbit(Pin))) * 2U)))) >> ((__CLZ(__builtin_arm_rbit(Pin))) * 2U));

}
# 569 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline void LL_GPIO_SetAFPin_0_7(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Alternate)
{
  (((GPIOx->AFR[0])) = ((((((GPIOx->AFR[0]))) & (~(((0xFUL << (0U)) << ((__CLZ(__builtin_arm_rbit(Pin))) * 4U))))) | ((Alternate << ((__CLZ(__builtin_arm_rbit(Pin))) * 4U))))));

}
# 606 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline uint32_t LL_GPIO_GetAFPin_0_7(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->AFR[0]) & (((0xFUL << (0U)) << ((__CLZ(__builtin_arm_rbit(Pin))) * 4U)))) >> ((__CLZ(__builtin_arm_rbit(Pin))) * 4U));

}
# 646 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline void LL_GPIO_SetAFPin_8_15(GPIO_TypeDef *GPIOx, uint32_t Pin, uint32_t Alternate)
{
  (((GPIOx->AFR[1])) = ((((((GPIOx->AFR[1]))) & (~(((0xFUL << (0U)) << ((__CLZ(__builtin_arm_rbit(Pin >> 8U))) * 4U))))) | ((Alternate << ((__CLZ(__builtin_arm_rbit(Pin >> 8U))) * 4U))))));

}
# 684 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline uint32_t LL_GPIO_GetAFPin_8_15(GPIO_TypeDef *GPIOx, uint32_t Pin)
{
  return (uint32_t)(((GPIOx->AFR[1]) & (((0xFUL << (0U)) << ((__CLZ(__builtin_arm_rbit(Pin >> 8U))) * 4U)))) >> ((__CLZ(__builtin_arm_rbit(Pin >> 8U))) * 4U));

}
# 720 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline void LL_GPIO_LockPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  volatile uint32_t temp;
  ((GPIOx->LCKR) = ((0x1UL << (16U)) | PinMask));
  ((GPIOx->LCKR) = (PinMask));
  ((GPIOx->LCKR) = ((0x1UL << (16U)) | PinMask));

  temp = ((GPIOx->LCKR));
  (void) temp;
}
# 755 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline uint32_t LL_GPIO_IsPinLocked(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  return ((((GPIOx->LCKR) & (PinMask)) == (PinMask)) ? 1UL : 0UL);
}







static __inline uint32_t LL_GPIO_IsAnyPinLocked(GPIO_TypeDef *GPIOx)
{
  return ((((GPIOx->LCKR) & ((0x1UL << (16U)))) == ((0x1UL << (16U)))) ? 1UL : 0UL);
}
# 785 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline uint32_t LL_GPIO_ReadInputPort(GPIO_TypeDef *GPIOx)
{
  return (uint32_t)(((GPIOx->IDR)));
}
# 814 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline uint32_t LL_GPIO_IsInputPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  return ((((GPIOx->IDR) & (PinMask)) == (PinMask)) ? 1UL : 0UL);
}
# 826 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline void LL_GPIO_WriteOutputPort(GPIO_TypeDef *GPIOx, uint32_t PortValue)
{
  ((GPIOx->ODR) = (PortValue));
}







static __inline uint32_t LL_GPIO_ReadOutputPort(GPIO_TypeDef *GPIOx)
{
  return (uint32_t)(((GPIOx->ODR)));
}
# 866 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline uint32_t LL_GPIO_IsOutputPinSet(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  return ((((GPIOx->ODR) & (PinMask)) == (PinMask)) ? 1UL : 0UL);
}
# 895 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline void LL_GPIO_SetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  ((GPIOx->BSRR) = (PinMask));
}
# 924 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline void LL_GPIO_ResetOutputPin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  ((GPIOx->BRR) = (PinMask));
}
# 953 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
static __inline void LL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint32_t PinMask)
{
  uint32_t odr = ((GPIOx->ODR));
  ((GPIOx->BSRR) = (((odr & PinMask) << 16u) | (~odr & PinMask)));
}
# 968 "../api\\../src/driver/stm32g4-v120/inc/stm32g4xx_ll_gpio.h"
ErrorStatus LL_GPIO_DeInit(GPIO_TypeDef *GPIOx);
ErrorStatus LL_GPIO_Init(GPIO_TypeDef *GPIOx, LL_GPIO_InitTypeDef *GPIO_InitStruct);
void LL_GPIO_StructInit(LL_GPIO_InitTypeDef *GPIO_InitStruct);
# 135 "../src/driver/stm32g4-v120/inc/../../../config/stm32hal_driver_cfg.h" 2
# 33 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal.h" 2
# 529 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal.h"
HAL_StatusTypeDef HAL_Init(void);
HAL_StatusTypeDef HAL_DeInit(void);
void HAL_MspInit(void);
void HAL_MspDeInit(void);
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);
# 544 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal.h"
void HAL_IncTick(void);
void HAL_Delay(uint32_t Delay);
uint32_t HAL_GetTick(void);
uint32_t HAL_GetTickPrio(void);
HAL_StatusTypeDef HAL_SetTickFreq(uint32_t Freq);
uint32_t HAL_GetTickFreq(void);
void HAL_SuspendTick(void);
void HAL_ResumeTick(void);
uint32_t HAL_GetHalVersion(void);
uint32_t HAL_GetREVID(void);
uint32_t HAL_GetDEVID(void);
# 565 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal.h"
void HAL_DBGMCU_EnableDBGSleepMode(void);
void HAL_DBGMCU_DisableDBGSleepMode(void);
void HAL_DBGMCU_EnableDBGStopMode(void);
void HAL_DBGMCU_DisableDBGStopMode(void);
void HAL_DBGMCU_EnableDBGStandbyMode(void);
void HAL_DBGMCU_DisableDBGStandbyMode(void);
# 580 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal.h"
extern volatile uint32_t uwTick;
extern uint32_t uwTickPrio;
extern uint32_t uwTickFreq;
# 592 "../src/driver/stm32g4-v120/inc/stm32g4xx_hal.h"
void HAL_SYSCFG_CCMSRAMErase(void);
void HAL_SYSCFG_EnableMemorySwappingBank(void);
void HAL_SYSCFG_DisableMemorySwappingBank(void);


void HAL_SYSCFG_VREFBUF_VoltageScalingConfig(uint32_t VoltageScaling);
void HAL_SYSCFG_VREFBUF_HighImpedanceConfig(uint32_t Mode);
void HAL_SYSCFG_VREFBUF_TrimmingConfig(uint32_t TrimmingValue);
HAL_StatusTypeDef HAL_SYSCFG_EnableVREFBUF(void);
void HAL_SYSCFG_DisableVREFBUF(void);


void HAL_SYSCFG_EnableIOSwitchBooster(void);
void HAL_SYSCFG_DisableIOSwitchBooster(void);
void HAL_SYSCFG_EnableIOSwitchVDD(void);
void HAL_SYSCFG_DisableIOSwitchVDD(void);

void HAL_SYSCFG_CCMSRAM_WriteProtectionEnable(uint32_t Page);
# 45 "../src/driver/stm32g4-v120/src/stm32g4xx_hal_uart_ex.c" 2

