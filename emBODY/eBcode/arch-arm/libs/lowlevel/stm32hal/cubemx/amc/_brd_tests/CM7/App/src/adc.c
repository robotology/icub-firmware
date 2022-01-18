/*******************************************************************************************************************//**
 * @file    adc.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 July, 14
 * @brief   A/D converter management for CM7 core
 **********************************************************************************************************************/

/* Includes ***********************************************************************************************************/
#include <string.h>
#include <stdlib.h>
#include "adc.h"
#include "main.h"
#include "stm32h7xx_hal_adc.h"
#include "led.h"
#include "console.h"
#include "FreeRTOS.h"


/* Private macros *****************************************************************************************************/

/* Number of channels managed by CM7 */
#define ADC_NUMBER_OF_CHANNELS  (6)

/* Calibration parameter stored by the manufacturer 
 * Given:
 *  VREF                Current reference voltage (mVolts)
 *  VREFINT             Current ADC measurement of VREFIN channel (16 bits raw data)
 *  VREFINT_CAL_VREF    Reference voltage during manufacturer calibration (mVolt)
 *  VREFINT_CAL         ADC measurement of VREFIN channel during manufacturer calibration (16 bits raw data)
 *
 * The following equation holds:
 *  VREFINT * VREF = VREFINT_CAL * VREFINT_CAL_VREF
 *
 * Hence:
 *  VREF = VREFINT_CAL * VREFINT_CAL_VREF / VREFINT
 *  VREF = Kvrefcal / VREFINT;
 *
 * The 32 bits product Kvrefcal = VREFINT_CAL * VREF_CAL is costant for a given device hence it can be evaluated in
 * advance, during initialization. The current VREF voltage can be tracked any time a new VREFINT value is measured.
 * Voltages read by other ADC channels are given by:
 *
 *  Vadcin = ADCIN * VREF/FULLSCALE
 *
 * where ADCIN is the raw data read by ADC, and FULLSCALE is 65536 (for a 16 bit conversion)
 */
#define VREFINT_CAL     (*VREFINT_CAL_ADDR)
#define VREF_NOM        (3300.0 /*mV*/)
#define VREFINT_NOM     (1212.0 /*mV*/)

/* Hardware related constants */
#define R10             (10000.0 /* Ohm */)
#define R11             (10000.0 /* Ohm */)
#define R110            (560000.0 /* Ohm */)
#define R115            (33000.0 /* Ohm */)
#define ACS70331_OFFS   (1500.0 /* mV */)
#define ACS70331_GAIN   (2.5 /* mA/mV */)
#define CIN_OFFS        ((int32_t)(ACS70331_OFFS*65536.0/VREF_NOM +0.5))
#define CIN_GAINx4      ((int32_t)(4.0*ACS70331_GAIN))
#define VCORE_GAIN      (1u)
#define VIN_GAIN        ((uint32_t)((R110+R115)/R115 + 0.5))
#define VAUX_GAIN       ((uint32_t)((R10+R11)/R11 + 0.5))

/* Running Average filter length */
#define FILTER_LENGTH   (128)

/* Utilities */
#define __DUMMY_READ(x)   while(0){volatile uint8_t u = (x);}
#define __BYTE(x)         (*(volatile uint8_t *)&(x))


/* Extenal variables **************************************************************************************************/

/* Declared in main.h */
extern ADC_HandleTypeDef hadc3;
extern DMA_HandleTypeDef hdma_adc3;
extern TIM_HandleTypeDef htim6;


/* Private typedefs ***************************************************************************************************/

enum AdcSignals 
{
    Cin,        /* ADC3_INP0 */
    Vin,        /* ADC3_INP1 */
    Vaux,       /* ADC3_INP2 */
    Vcore,      /* ADC3_INP3 */
    Vtemp,      /* Temperature sensor */
    Vrefint     /* Internal voltage reference 1.212V */
} ;

typedef struct
{
    int32_t avg;
    uint32_t idx;
    int32_t buf[FILTER_LENGTH];
} AdcMovingAverageFilter_t;


/* Private variables **************************************************************************************************/

/* Double buffer filled by the DMA */
static volatile __align(32) uint16_t AdcDmaBuffer[16];

/* VREF constant: VREFINT_CAL * VREFINT_CAL_VREF */
static uint32_t Kvrefcal = (uint32_t)(VREFINT_NOM * VREF_NOM + 0.5);

/* Measured VREF in mV. Equal to VCC */
static volatile uint16_t Vcc_mV = VREF_NOM;
static volatile uint16_t Vin_mV;
static volatile uint16_t Vaux_mV;
static volatile uint16_t Vcore_mV;
static volatile int16_t  Cin_mA;

static AdcMovingAverageFilter_t VrefFilter;
static AdcMovingAverageFilter_t CinFilter;

    
/* Local functions ****************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
static int32_t AdcMovingAverageFilter(AdcMovingAverageFilter_t *pFilter, int32_t sample)
{
    pFilter->avg -= pFilter->buf[pFilter->idx];
    pFilter->avg += (pFilter->buf[pFilter->idx] = sample);
    if (++(pFilter->idx) >= FILTER_LENGTH) pFilter->idx = 0;
    return (pFilter->avg + FILTER_LENGTH/2)/FILTER_LENGTH;
}

/* Callback functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
static void AdcDmaComplete_callback(volatile uint16_t sample[])
{
    uint16_t Vref;
    /* Calculate average values of Vrefint */
    Vref = (uint16_t)AdcMovingAverageFilter(&VrefFilter, (int32_t)sample[Vrefint]);
    /* Update current analog values */
    Vcc_mV   = (0 < Vref)? Kvrefcal / Vref : 3300u;
    Vcore_mV = (uint32_t)sample[Vcore] * VCORE_GAIN * (uint32_t)Vcc_mV / 65536u;
    Vaux_mV  = (uint32_t)sample[Vaux]  * VAUX_GAIN  * (uint32_t)Vcc_mV / 65536u;
    Vin_mV   = (uint32_t)sample[Vin]   * VIN_GAIN   * (uint32_t)Vcc_mV / 65536u;
    Cin_mA   = (int16_t)AdcMovingAverageFilter(&CinFilter,
               (int32_t)(int16_t)sample[Cin] * CIN_GAINx4 * (int32_t)(int16_t)Vcc_mV / (65536*4));
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
static void AdcHalfTransfer_callback(ADC_HandleTypeDef *hadc)
{
    /* Force the readback of the DMA buffer */
    SCB_InvalidateDCache_by_Addr((void *)AdcDmaBuffer, sizeof(AdcDmaBuffer));
    /* First half of the buffer ready */
    AdcDmaComplete_callback(&(AdcDmaBuffer[0]));
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
static void AdcTransferComplete_callback(ADC_HandleTypeDef *hadc)
{
    /* Force the readback of the DMA buffer */
    SCB_InvalidateDCache_by_Addr((void *)AdcDmaBuffer, sizeof(AdcDmaBuffer));
    /* Second half of the buffer ready */
    AdcDmaComplete_callback(&(AdcDmaBuffer[ADC_NUMBER_OF_CHANNELS]));
}


/* Exported functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
HAL_StatusTypeDef AdcInit(void)
{
    unsigned i;
    HAL_StatusTypeDef ret;
    if (HAL_OK == (ret = HAL_TIM_Base_Start(&htim6)))
    {
        if (HAL_OK == (ret = HAL_ADCEx_Calibration_Start(&hadc3, ADC_CALIB_OFFSET, ADC_SINGLE_ENDED)))
        {
            /* Register the callback functions */
            HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_HALF_CB_ID, AdcHalfTransfer_callback);
            HAL_ADC_RegisterCallback(&hadc3, HAL_ADC_CONVERSION_COMPLETE_CB_ID, AdcTransferComplete_callback);
            /* Calculate the proportionality constant */
            Kvrefcal = (uint32_t)VREFINT_CAL_VREF * (uint32_t)VREFINT_CAL;
            /* Init the Vref filter to the nominal value */
            for (i = 0; i < FILTER_LENGTH; i++) VrefFilter.buf[i] = 3300;
            VrefFilter.avg = FILTER_LENGTH * 3300;
            VrefFilter.idx = 0;
            /* Clear the Cin filter */
            memset((void *)&CinFilter, 0, sizeof(CinFilter));
            memset((void *)AdcDmaBuffer, 0, sizeof(AdcDmaBuffer));
            SCB_CleanInvalidateDCache_by_Addr((void *)AdcDmaBuffer, sizeof(AdcDmaBuffer));
            /* Start the converter in DMA mode */
            ret = HAL_ADC_Start_DMA(&hadc3, (void *)AdcDmaBuffer, 2*ADC_NUMBER_OF_CHANNELS);
            HAL_Delay(500);
        }
    }
    return ret;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
uint16_t AdcGetVcore(void)
{
    return Vcore_mV;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
uint16_t AdcGetVcc(void)
{
    return Vcc_mV;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
uint16_t AdcGetVaux(void)
{
    return Vaux_mV;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
uint16_t AdcGetVin(void)
{
    return Vin_mV;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
int16_t AdcGetCin(void)
{
    return Cin_mA;
}


/*******************************************************************************************************************//**
 * @brief
 * @param
 * @return
 */
void AdcTest(void)
{
    char ch;
    uint32_t v;
    int32_t i;
    coprintf("A/D converter test started\n");
    HAL_Delay(500);
    coprintf("Manufacturer ADC calibration data\n"
             "VREFINT_CAL=%u, VREFINT_CAL_VREF=%u\n", VREFINT_CAL, VREFINT_CAL_VREF);
    coprintf("Vin   = %u mV", v = AdcGetVin());
    if ((v < 9500) || (v > 58000)) coprintf(", ATTENTION: Out of range");
    coprintf("\n");
    coprintf("Vcore = %u mV", v = AdcGetVcore());
    if ((v < 1140) || (v > 1260)) coprintf(", ATTENTION: Out of range");
    coprintf("\n");
    coprintf("Vcc   = %u mV", v = AdcGetVcc());
    if ((v < 3135) || (v > 3465)) coprintf(", ATTENTION: Out of range");
    coprintf("\n");
    coprintf("Vaux  = %u mV", v = AdcGetVaux());
    if ((v < 4750) || (v > 5250)) coprintf(", ATTENTION: Out of range");
    coprintf("\n");
    coprintf("Calibrating Iin\n"
             "Press Enter to terminate\n");
    while (!coRxReady() || ('\r' != (ch = coRxChar())))
    {
        v = hadc3.Instance->OFR1;
        coprintf("\r%5u, %+5d mA", v & 0xFFFFu, AdcGetCin()); HAL_Delay(100);
        switch (ch)
        {
            case '>':
                hadc3.Instance->OFR1 = (v & 0xFFFF0000u) | ((v + 10) & 0x0000FFFFu);
                break;
            case '+':
                hadc3.Instance->OFR1 = (v & 0xFFFF0000u) | ((v + 1) & 0x0000FFFFu);
                break;
            case '-':
                hadc3.Instance->OFR1 = (v & 0xFFFF0000u) | ((v - 1) & 0x0000FFFFu);
                break;
            case '<':
                hadc3.Instance->OFR1 = (v & 0xFFFF0000u) | ((v - 10) & 0x0000FFFFu);
                break;
        }
        ch = 0;
    }
    coprintf("\n");
    v = AdcGetVin();
    i = AdcGetCin();
    coprintf("Vin=%u mV, Iin=%d mA, Pin=%u mW\n", v, i, v*(uint32_t)abs(i)/1000u);
    coprintf("A/D converter test terminated\n");
}


/* END OF FILE ********************************************************************************************************/
