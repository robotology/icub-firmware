/*******************************************************************************************************************//**
 * @file    analog.c
 * @author  G.Zini
 * @version 1.0
 * @date    2021 January, 20
 * @brief   Analog signals management
 **********************************************************************************************************************/

// CODE SHAPER
#if defined(USE_STM32HAL)
#include "motorhal_config.h"
#endif

#if defined(USE_STM32HAL)
// API
#include "analog.h"
#endif

/* Includes ***********************************************************************************************************/
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#if defined(USE_STM32HAL)
#include "stm32hal.h"

#include "pwm.h"

#if !defined(HALCONFIG_DONTUSE_FLASH)
#include "flash.h"
#endif

#else
#include "adc.h"
#include "analog.h"
#include "console.h"
#include "flash.h"
#include "led.h"
#include "main.h"
#include "pwm.h"
#include "utilities.h"
#include "FreeRTOS.h"
#endif

#if (USE_HAL_ADC_REGISTER_CALLBACKS != 1)
    #error Flag ADC1 in menu "Project Manager -> Advanced Settings -> Register CallBack" of CubeMx must be ENABLED
#endif


/* Private macros *****************************************************************************************************/

/* Used by the Moving Average Filter */
#define ANALOG_AVG_FILTER_SHIFT     (7)
#define ANALOG_AVG_FILTER_LENGTH    (1U<<ANALOG_AVG_FILTER_SHIFT)

/* Calibration parameter stored by the manufacturer 
 * Given:
 *  VREF                Current reference voltage
 *  VREFINT             Current ADC measurement of VREFIN channel
 *  VREF_CAL            Reference voltage during manufacturer calibration
 *  VREFINT_CAL         ADC measurement of VREFIN channel during manufacturer calibration
 *
 * The following equation holds:
 *  VREFINT * VREF = VREFINT_CAL * VREF_CAL
 *
 * Hence:
 *  VREF = VREFINT_CAL * VREF_CAL / VREFINT = k_vref / VREFINT;
 *
 * The product k_vref = VREFINT_CAL * VREF_CAL is costant hence it can be evaluated in advance, during initialization.
 * The current VREF voltage can be tracked any time a new VREFINT value is measured
 */
#define VREF_CAL    ((double)VREFINT_CAL_VREF * mVolt)
#define VREFINT_CAL (*VREFINT_CAL_ADDR)
#define VREF_NOM    (3.300 * Volt)
#define VREFINT_NOM (1.212 * Volt)

/* Scaling factor */
#define SCALING_FACTOR  (65536.0/4096.0)

/* Hardware component definitions */
#define kOhm        (1000.0)
#define Ohm         (1.000)
#define mOhm        (0.001)
#define Volt        (1.000)
#define mVolt       (0.001)
#define uVolt       (0.000001)
#define nVolt       (0.000000001)
#define Ampere      (1.000)
#define mAmpere     (0.001)

/* Hardware related constants */
#define R37			(560.0 * kOhm)
#define R43			(560.0 * kOhm)
#define R44			(33.0 * kOhm)
#define R47			(33.0 * kOhm)
#define ACS70331_OFFS   (1500.0 * mVolt)
#define ACS70331_GAIN   (2.5 * Ampere/Volt)
#define CIN_OFFS    (4096.0 * ACS70331_OFFS/VREF_NOM +0.5)
#define CIN_GAIN    ((int32_t)(SCALING_FACTOR * ACS70331_GAIN + 0.5))
#define VIN_GAIN	((uint32_t)(SCALING_FACTOR * (R47+R43)/R47 + 0.5))
#define VPH_GAIN	((uint32_t)(SCALING_FACTOR * (R44+R37)/R44 + 0.5))


/* Private typedefs ***************************************************************************************************/

/*************************************************************************************************************
 * NOTE: The following data structures comply with the workaround described in section 2.7.7 of the document *
 * ES0430, rev 6: STM32G471xx/473xx/474xx/483xx/484xx device errata.                                         *
 * The workaround is mandatory for devices with REV_ID <= 0x2001 (package marking Z)                         *
 *************************************************************************************************************/
 
/* Data record of ADC1 */
typedef struct
{
  uint16_t vref;  /* Channel 18: Internal voltage reference */
  uint16_t nul1;
  uint16_t vph1;  /* Channel_1: Phase 1 voltage */
  uint16_t nul2;
  uint16_t vph2;  /* Channel_3: Phase 2 voltage  */
  uint16_t nul3;
  uint16_t vph3;  /* Channel_4: Phase 3 voltage  */
  uint16_t nul4;
  uint16_t vin;   /* Channel 12: Input voltage */
  uint16_t nul5;
  int16_t cin;    /* Channel 14: Input average current */
  int16_t nul6;
  uint16_t temp;  /* Channel 16: Internal temperature sensor */
  uint16_t nul7;
} ADC1_Record_t;

/* Data record of ADC2 */
typedef struct
{
  int16_t cph1;   /* Channel_3: Phase 1 current */
  int16_t nul8;
  int16_t cph2;   /* Channel_4: Phase 2 current  */
  int16_t nul9;
  int16_t cph3;   /* Channel_5: Phase 3 current  */
  int16_t nul10;
} ADC2_Record_t;

typedef struct
{
    int32_t     avg;
    uint32_t    idx;
    int32_t     buf[ANALOG_AVG_FILTER_LENGTH];
} analogAvgFilterTypeDef;

/* Private variables **************************************************************************************************/

/* DMA Buffer of ADC1 and ADC2 */
static ADC1_Record_t adc1_Buffer;
static ADC2_Record_t adc2_Buffer[2];

/* VREF constant: VREFINT_CAL * VREFINT_CAL_VREF */
static uint32_t k_vref = 4096 * VREFINT_NOM / mVolt;

/* Measured VREF in mV. Equal to VCC */
static uint16_t vref_mV = VREF_NOM / mVolt;

/* Average filters for current measurements */
static analogAvgFilterTypeDef cinFilter;
static analogAvgFilterTypeDef cph1Filter;
static analogAvgFilterTypeDef cph2Filter;
static analogAvgFilterTypeDef cph3Filter;

/* Public variables ***************************************************************************************************/


/* Private functions **************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  
 */
static int32_t analogMovingAverage(analogAvgFilterTypeDef *filter, int32_t sample)
{
    filter->avg -= filter->buf[filter->idx];
    filter->avg += (filter->buf[filter->idx] = sample);
    if (++(filter->idx) >= ANALOG_AVG_FILTER_LENGTH) filter->idx = 0;
    return filter->avg;
}


/* Callback functions *************************************************************************************************/

/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  
 */
static void adc1_TransferComplete_cb(ADC_HandleTypeDef *hadc)
{
	if (0 != adc1_Buffer.vref) vref_mV = (k_vref + (adc1_Buffer.vref>>1))/adc1_Buffer.vref;
    else vref_mV = VREF_NOM / mVolt;
    analogMovingAverage(&cinFilter, adc1_Buffer.cin);
}

/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  
 */
static void adc2_HalfTransferComplete_cb(ADC_HandleTypeDef *hadc)
{
    pwmSetCurrents_cb(adc2_Buffer[0].cph1, adc2_Buffer[0].cph2, adc2_Buffer[0].cph3);
    analogMovingAverage(&cph1Filter, adc2_Buffer[0].cph1);
    analogMovingAverage(&cph2Filter, adc2_Buffer[0].cph2);
    analogMovingAverage(&cph3Filter, adc2_Buffer[0].cph3);
}

/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  
 */
static void adc2_TransferComplete_cb(ADC_HandleTypeDef *hadc)
{
    pwmSetCurrents_cb(adc2_Buffer[1].cph1, adc2_Buffer[1].cph2, adc2_Buffer[1].cph3);
    analogMovingAverage(&cph1Filter, adc2_Buffer[1].cph1);
    analogMovingAverage(&cph2Filter, adc2_Buffer[1].cph2);
    analogMovingAverage(&cph3Filter, adc2_Buffer[1].cph3);
}


/* Public functions ***************************************************************************************************/


/*******************************************************************************************************************//**
 * @brief   Measure IPHASE1 current
 * @param   void
 * @return  IPHASE1 current in mA [-2500mA .. 2500mA]
 */
int32_t analogConvertCurrent(int32_t raw)
{
	return CIN_GAIN * vref_mV * raw >> 16u;
}


/*******************************************************************************************************************//**
 * @brief   Measure VCC voltage in mV. Value must be inside the interval 3200 mV to 3400 mV
 * @param   void
 * @return  VCC voltage in mV
 */
uint32_t analogVcc(void)
{
	return vref_mV;
}

/*******************************************************************************************************************//**
 * @brief   Measure VIN voltage
 * @param   void
 * @return  VIN voltage in mV [9500mV .. 57000mV]
 */
uint32_t analogVin(void)
{
    /* Convert value in mV */
	return VIN_GAIN * vref_mV * adc1_Buffer.vin >> 16u;
}

/*******************************************************************************************************************//**
 * @brief   Measure VPHASE1 voltage
 * @param   void
 * @return  VPHASE1 voltage in mV [0 .. 57000mV]
 */
uint32_t analogVph1(void)
{
	return VPH_GAIN * vref_mV * adc1_Buffer.vph1 >> 16u;
}

/*******************************************************************************************************************//**
 * @brief   Measure VPHASE2 voltage
 * @param   void
 * @return  VPHASE2 voltage in mV [0 .. 57000mV]
 */
uint32_t analogVph2(void)
{
	return VPH_GAIN * vref_mV * adc1_Buffer.vph2 >> 16u;
}

/*******************************************************************************************************************//**
 * @brief   Measure VPHASE3 voltage
 * @param   void
 * @return  VPHASE3 voltage in mV [0 .. 57000mV]
 */
uint32_t analogVph3(void)
{
	return VPH_GAIN * vref_mV * adc1_Buffer.vph3 >> 16u;
}

/*******************************************************************************************************************//**
 * @brief   Measure IIN current
 * @param   void
 * @return  IIN current in mA [-2500mA .. 2500mA]
 */
int32_t analogCin(void)
{
    int32_t raw = cinFilter.avg >> ANALOG_AVG_FILTER_SHIFT;
	return CIN_GAIN * vref_mV * raw >> 16u;
}

/*******************************************************************************************************************//**
 * @brief   Measure IPHASE1 current
 * @param   void
 * @return  IPHASE1 current in mA [-2500mA .. 2500mA]
 */
int32_t analogCph1(void)
{
    int32_t raw = cph1Filter.avg >> ANALOG_AVG_FILTER_SHIFT;
	return CIN_GAIN * vref_mV * raw >> 16u;
}

/*******************************************************************************************************************//**
 * @brief   Measure IPHASE2 current
 * @param   void
 * @return  IPHASE2 current in mA [-2500mA .. 2500mA]
 */
int32_t analogCph2(void)
{
    int32_t raw = cph2Filter.avg >> ANALOG_AVG_FILTER_SHIFT;
	return CIN_GAIN * vref_mV * raw >> 16u;
}

/*******************************************************************************************************************//**
 * @brief   Measure IPHASE3 current
 * @param   void
 * @return  IPHASE3 current in mA [-2500mA .. 2500mA]
 */
int32_t analogCph3(void)
{
    int32_t raw = cph3Filter.avg >> ANALOG_AVG_FILTER_SHIFT;
	return CIN_GAIN * vref_mV * raw >> 16u;
}

/*******************************************************************************************************************//**
 * @brief   Read the offset value of the Iin sensor
 * @param   void
 * @return  Offset current in mA
 */
int32_t analogGetOffsetIin( void )
{
    return LL_ADC_GetOffsetLevel(ADC1, LL_ADC_OFFSET_4);
}

/*******************************************************************************************************************//**
 * @brief   Write the offset value of the Iin sensor
 * @param   Offset current in mA
 * @return  Previous value of Offset current in mA
 */
int32_t analogSetOffsetIin(int32_t offs)
{
	int32_t prev = LL_ADC_GetOffsetLevel(ADC1, LL_ADC_OFFSET_4);
    LL_ADC_SetOffset(ADC1, LL_ADC_OFFSET_4, LL_ADC_CHANNEL_14, offs);
	return prev;
}

/*******************************************************************************************************************//**
 * @brief   Read the offset value of the Phase 1 sensor
 * @param   void
 * @return  Offset current in mA
 */
int32_t analogGetOffsetIph1( void )
{
    return LL_ADC_GetOffsetLevel(ADC2, LL_ADC_OFFSET_1);
}

/*******************************************************************************************************************//**
 * @brief   Write the offset value of the Phase 1 sensor
 * @param   Offset current in mA
 * @return  Previous value of Offset current in mA
 */
int32_t analogSetOffsetIph1(int32_t offs)
{
	int32_t prev = LL_ADC_GetOffsetLevel(ADC2, LL_ADC_OFFSET_1);
    LL_ADC_SetOffset(ADC2, LL_ADC_OFFSET_1, ADC_CHANNEL_3, offs);
	return prev;
}

/*******************************************************************************************************************//**
 * @brief   Read the offset value of the Phase 2 sensor
 * @param   void
 * @return  Offset current in mA
 */
int32_t analogGetOffsetIph2( void )
{
    return LL_ADC_GetOffsetLevel(ADC2, LL_ADC_OFFSET_2);
}

/*******************************************************************************************************************//**
 * @brief   Write the offset value of the Phase 2 sensor
 * @param   Offset current in mA
 * @return  Previous value of Offset current in mA
 */
int32_t analogSetOffsetIph2(int32_t offs)
{
	int32_t prev = LL_ADC_GetOffsetLevel(ADC2, LL_ADC_OFFSET_2);
    LL_ADC_SetOffset(ADC2, LL_ADC_OFFSET_2, ADC_CHANNEL_4, offs);
	return prev;
}

/*******************************************************************************************************************//**
 * @brief   Read the offset value of the Phase 3 sensor
 * @param   void
 * @return  Offset current in mA
 */
int32_t analogGetOffsetIph3( void )
{
    return LL_ADC_GetOffsetLevel(ADC2, LL_ADC_OFFSET_3);
}

/*******************************************************************************************************************//**
 * @brief   Write the offset value of the Phase 3 sensor
 * @param   Offset current in mA
 * @return  Previous value of Offset current in mA
 */
int32_t analogSetOffsetIph3(int32_t offs)
{
	int32_t prev = LL_ADC_GetOffsetLevel(ADC2, LL_ADC_OFFSET_3);
    LL_ADC_SetOffset(ADC2, LL_ADC_OFFSET_3, ADC_CHANNEL_5, offs);
	return prev;
}

/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  
 */
HAL_StatusTypeDef analogInit(void)
{
	HAL_StatusTypeDef result = HAL_ERROR;

	/* Check ADC initialization */
	if ((NULL != hadc1.Instance) && (NULL != hadc2.Instance))
	{
		/* Stop any ADC operation */
		HAL_ADC_Stop_DMA(&hadc1);
		HAL_ADC_Stop_DMA(&hadc2);

		/* Calibrate ADC1 and ADC2 */
		if ((HAL_OK == HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED)) &&
		    (HAL_OK == HAL_ADCEx_Calibration_Start(&hadc2, ADC_SINGLE_ENDED)))
		{
			/* Calculate the proportionality constant */
			k_vref = (uint32_t)VREFINT_CAL_VREF * (uint32_t)VREFINT_CAL;

            /* If the board has never been calibrated */
            if (0 == MainConf.analog.cinOffs)
            {
                /* Configure default values */
                MainConf.analog.cinOffs   = CIN_OFFS;
                MainConf.analog.cphOffs[0] = CIN_OFFS;
                MainConf.analog.cphOffs[1] = CIN_OFFS;
                MainConf.analog.cphOffs[2] = CIN_OFFS;
            }
            
            /* Set the calibration parameters */
            analogSetOffsetIin(MainConf.analog.cinOffs);
            analogSetOffsetIph1(MainConf.analog.cphOffs[0]);
            analogSetOffsetIph2(MainConf.analog.cphOffs[1]);
            analogSetOffsetIph3(MainConf.analog.cphOffs[2]);
			
			/* Clear the input buffers */
			memset((void *)&adc1_Buffer, 0, sizeof(adc1_Buffer));
			memset((void *)&adc2_Buffer, 0, sizeof(adc2_Buffer));

			/* Register the interrupt callback function for the DMA Transfer Complete interrupt*/
			if ((HAL_OK == HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID, adc1_TransferComplete_cb)) &&
				(HAL_OK == HAL_ADC_RegisterCallback(&hadc2, HAL_ADC_CONVERSION_HALF_CB_ID, adc2_HalfTransferComplete_cb)) &&
				(HAL_OK == HAL_ADC_RegisterCallback(&hadc2, HAL_ADC_CONVERSION_COMPLETE_CB_ID, adc2_TransferComplete_cb)))
			{
				/* Enable the Transfer Complete interrupts of the DMA */
				__HAL_DMA_ENABLE_IT(hadc1.DMA_Handle, DMA_IT_TC);
				__HAL_DMA_ENABLE_IT(hadc2.DMA_Handle, DMA_IT_TC);
				/* Enable the ADC in DMA mode */
				if ((HAL_OK == HAL_ADC_Start_DMA(&hadc1, (uint32_t *)&adc1_Buffer, sizeof(adc1_Buffer)/sizeof(uint16_t))) &&
					(HAL_OK == HAL_ADC_Start_DMA(&hadc2, (uint32_t *)&adc2_Buffer, sizeof(adc2_Buffer)/sizeof(uint16_t))))
				{
					/* DMA is running now */
					result = HAL_OK;
				}
			}
		}
	}
	return result;
}

#if defined(HALCONFIG_DONTUSE_TESTS)
#else

/*******************************************************************************************************************//**
 * @brief   
 * @param   
 * @return  
 */
void analogTest(void)
{
    char ch;
    char coBuffer[80];
    const char *curs;
    int32_t value;

    coprintf("Analog Test\n"); HAL_Delay(250);
    coprintf("Vin = %5u mV, Iin = %+5d mA, Vcc = %5u mV\n", analogVin(), analogCin(), analogVcc()); HAL_Delay(250);

    /* Infinite loop */
    for(;;)
    {
        coprintf("0   : Set Iin offset\n"
                 "1   : Set phase 1 offset\n"
                 "2   : Set phase 2 offset\n"
                 "3   : Set phase 3 offset\n"
                 "D   : Display currents/voltages\n"
                 "Z   : Autozero phase offsets\n"
                 "ESC : Exit test\n"
                 "? "); HAL_Delay(250);
        ch = coRxChar();
        if (isprint(ch))
        {
            coprintf("%c\n", ch);
            switch (toupper(ch))
            {
                case '0':
                    while (1)
                    {
                        coprintf("\rIin = %+5d mA", analogCin()); HAL_Delay(250);
                        if (coRxReady())
                        {
                            ch = coRxChar();
                            value = analogGetOffsetIin();
                            if (ch == '+') --value;
                            else if (ch == '-') ++value;
                            else if (ch == '<') value += 10;
                            else if (ch == '>') value -= 10;
                            else { coprintf("\n"); break; }
                            analogSetOffsetIin(value);
                            MainConf.analog.cinOffs = value;
                        }
                    }
                    break;

                case '1':
                    while (1)
                    {
                        coprintf("\rIph1 = %+5d mA", analogCph1()); HAL_Delay(250);
                        if (coRxReady())
                        {
                            ch = coRxChar();
                            value = analogGetOffsetIph1();
                            if (ch == '+') --value;
                            else if (ch == '-') ++value;
                            else if (ch == '<') value += 10;
                            else if (ch == '>') value -= 10;
                            else { coprintf("\n"); break; }
                            analogSetOffsetIph1(value);
                            MainConf.analog.cphOffs[0] = value;
                        }
                    }
                    break;

                case '2':
                    while (1)
                    {
                        coprintf("\rIph2 = %+5d mA", analogCph2()); HAL_Delay(250);
                        if (coRxReady())
                        {
                            ch = coRxChar();
                            value = analogGetOffsetIph2();
                            if (ch == '+') --value;
                            else if (ch == '-') ++value;
                            else if (ch == '<') value += 10;
                            else if (ch == '>') value -= 10;
                            else { coprintf("\n"); break; }
                            analogSetOffsetIph2(value);
                            MainConf.analog.cphOffs[1] = value;
                        }
                    }
                    break;

                case '3':
                    while (1)
                    {
                        coprintf("\rIph3 = %+5d mA", analogCph3()); HAL_Delay(250);
                        if (coRxReady())
                        {
                            ch = coRxChar();
                            value = analogGetOffsetIph3();
                            if (ch == '+') --value;
                            else if (ch == '-') ++value;
                            else if (ch == '<') value += 10;
                            else if (ch == '>') value -= 10;
                            else { coprintf("\n"); break; }
                            analogSetOffsetIph3(value);
                            MainConf.analog.cphOffs[2] = value;
                        }
                    }
                    break;

                case 'D':
                    while (1)
                    {
                        coprintf("Press RETURN to stop\n");  HAL_Delay(100);
                        /*       "|xxxxx|xxxxx/+xxxx|xxxxx/+xxxx|xxxxx/+xxxx|xxxxx/+xxxx|"  */
                        coprintf("| Vcc  |    Vin    | Phase #1  | Phase #2  | Phase #3  |\n");  HAL_Delay(100);
                        coprintf("|  mV  |   mV/mA   |   mV/mA   |   mV/mA   |   mV/mA   |\n");  HAL_Delay(100);
                        do
                        {
                            coprintf("\r|%5u |", analogVcc()); HAL_Delay(100);
                            coprintf("%5u/%+-5d|", analogVin(),  analogCin());  HAL_Delay(100);
                            coprintf("%5u/%+-5d|", analogVph1(), analogCph1()); HAL_Delay(100);
                            coprintf("%5u/%+-5d|", analogVph2(), analogCph2()); HAL_Delay(100);
                            coprintf("%5u/%+-5d|", analogVph3(), analogCph3()); HAL_Delay(100);
                            coprintf("%+-5d", (analogCph1()+analogCph2()+analogCph3())/3); HAL_Delay(100);
                        } while (!coRxReady());
                        coprintf("\nPWM? ");
                        coEditString(coBuffer, sizeof(coBuffer));
                        coprintf("\n");
                        curs = &(coBuffer[0]);
                        ch = skipblank(&curs);
                        if (isdigit(ch))
                        {
//                            if (atosl(&curs, &value) && ('\0' == skipblank(&curs)) && (HAL_OK == pwmSetValue(value)))
//                            {
//                                coprintf("Motor PWM = %d\n", value);
//                                continue;
//                            }
                        }
                        else if ('\0' == ch) break;
                        coprintf("ERROR: %s\n", curs);
                    }
                    pwmSet(0, 0, 0);
                    break;

                case 'Z':
                    coprintf("Sorry: not implemented yet\n", ch);
                    break;

                default:
                    coprintf("Unknown command: %c\n", ch);
                    break;
            }
        }
        else if ('\x1B' == ch)
        {
            coprintf("\n");
            coprintf("- Iin offset:     %+d LSB\n", analogGetOffsetIin());
            coprintf("- Iphase1 offset: %+d LSB\n", analogGetOffsetIph1());
            coprintf("- Iphase2 offset: %+d LSB\n", analogGetOffsetIph2());
            coprintf("- Iphase3 offset: %+d LSB\n", analogGetOffsetIph3());
            coprintf("Exit test\n");
            return ;
        }
    }
}

#endif // HALCONFIG_DONTUSE_TESTS

/* END OF FILE ********************************************************************************************************/
