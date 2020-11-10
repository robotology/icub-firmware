
/*
 * Copyright (C) 2020 iCub Tech - Istituto Italiano di Tecnologia
 * Author:  Marco Accame
 * email:   marco.accame@iit.it
*/


// --------------------------------------------------------------------------------------------------------------------
// - external dependencies
// --------------------------------------------------------------------------------------------------------------------

#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "gpio.h"

#include "stdio.h"
#include "string.h"


// --------------------------------------------------------------------------------------------------------------------
// - declaration of extern public interface
// --------------------------------------------------------------------------------------------------------------------

#include "test_tlv.h"

// --------------------------------------------------------------------------------------------------------------------
// - #define with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of extern variables, but better using _get(), _set() 
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - typedef with internal scope
// --------------------------------------------------------------------------------------------------------------------
// empty-section


// --------------------------------------------------------------------------------------------------------------------
// - declaration of static functions
// --------------------------------------------------------------------------------------------------------------------

void discover(void);

typedef enum { x3E = 0x3E, xBC = 0xBC } adr_t;
void set_address(adr_t adr);

void read_no_reg(uint8_t addr, void *pointer, uint8_t capacity);


void discovery(I2C_HandleTypeDef *bus, uint8_t *addresses, uint8_t sizeofaddresses, uint8_t *numof);

// --------------------------------------------------------------------------------------------------------------------
// - definition (and initialisation) of static variables
// --------------------------------------------------------------------------------------------------------------------




// --------------------------------------------------------------------------------------------------------------------
// - definition of extern public functions
// --------------------------------------------------------------------------------------------------------------------

uint8_t foundsI2C1[32] = {0};
uint8_t numI2C1 = 0;
uint8_t foundsI2C2[32] = {0};
uint8_t numI2C2 = 0;
uint8_t foundsI2C3[32] = {0};
uint8_t numI2C3 = 0;
uint8_t foundsI2C4[32] = {0};
uint8_t numI2C4 = 0;

void test_tlv_init(void)
{
    volatile HAL_StatusTypeDef status = HAL_ERROR;
    
    MX_GPIO_Init();
    
    // power off and on again. wait 3 ms
    
   
    HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin|MAGVCC1_Pin, GPIO_PIN_RESET);
    HAL_Delay(20);
    HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin|MAGVCC1_Pin, GPIO_PIN_SET);
    HAL_Delay(20);
    
    
    // init dma + i2c
    
    MX_DMA_Init();
    MX_I2C1_Init(); 
    MX_I2C2_Init(); 
    MX_I2C3_Init();
    MX_I2C4_Init();    
    HAL_Delay(20);
    
    discovery(&hi2c1, foundsI2C1, sizeof(foundsI2C1), &numI2C1);
    discovery(&hi2c2, foundsI2C2, sizeof(foundsI2C2), &numI2C2);
    discovery(&hi2c3, foundsI2C3, sizeof(foundsI2C3), &numI2C3);
    discovery(&hi2c4, foundsI2C4, sizeof(foundsI2C4), &numI2C4);
    
    for(;;);
    
//    set_address(x3E);   
//    discover();
    
//    set_address(xBC);   
//    discover();    
    
    // ping the chip @ address 0x3E
    
    static const uint8_t tlvadr = 0x3E; // 0xBC
    static const uint32_t timeout = 5;
    
//    status = HAL_I2C_IsDeviceReady(&hi2c2, tlvadr, 3, timeout);
 //   status = status;
    
        
    // reset
    volatile HAL_StatusTypeDef r = HAL_I2C_Master_Transmit(&hi2c2, 0, NULL,0, 5);
    r = r;
    HAL_Delay(5);
    
    
    status = HAL_I2C_IsDeviceReady(&hi2c2, tlvadr, 3, timeout);
    status = status;
    
        discover();
    
}

void test_tlv_ping(void)
{
    
}

uint8_t rxdata[20] = {0};

void test_tlv_config(void)
{
    memset(rxdata, 0, sizeof(rxdata));
    read_no_reg(0x3E, rxdata, 10);
    for(;;);    
}


// --------------------------------------------------------------------------------------------------------------------
// - definition of static functions 
// --------------------------------------------------------------------------------------------------------------------


void set_address(adr_t adr)
{
    // power off, power on
    // send 2 or 0
    HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin|MAGVCC1_Pin, GPIO_PIN_RESET);
    HAL_Delay(5);
    HAL_GPIO_WritePin(GPIOE, MAGVCC2_Pin|MAGVCC1_Pin, GPIO_PIN_SET);
    HAL_Delay(5);

    HAL_I2C_IsDeviceReady(&hi2c2, (x3E == adr) ? 0 : 4, 1, 5);    
}

uint8_t map[256] = {0};
volatile uint8_t found = 0;
uint8_t addresses[16] = {0};
void discover(void)
{
    memset(addresses, 0, sizeof(addresses));
    memset(map, 0, sizeof(map));
    found = 0;
    
    static const uint8_t start = 2;
    for(int i=start; i<256; i++)
    {
        //map[i] = (HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, i, 1, 5)) ? 1 : 0;
        if(HAL_OK == HAL_I2C_IsDeviceReady(&hi2c2, i, 1, 5))
        {
            map[i] = 1;
            if(found < sizeof(addresses))
            {
                addresses[found] = i;
            }
            found++;
        }
    }
    
    // se start = 0 oppure 1, allora indirizzo  = 3E / 3F
    // altrimenti se start >= allora = BC / BD
    
    found = found;
}

void read_no_reg(uint8_t addr, void *pointer, uint8_t capacity)
{
    
    volatile HAL_StatusTypeDef r = HAL_I2C_Master_Receive_DMA( &hi2c2, 
                                        addr, 
                                        pointer,  
                                        capacity
                                    );  
    r = r;
}

void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{   
    static volatile uint32_t tx = 0;
    tx++;  
    
}


void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *I2cHandle)
{
    static volatile uint32_t rx = 0;
    rx++;       
} 

void discovery(I2C_HandleTypeDef *bus, uint8_t *addrs, uint8_t sizeofaddrs, uint8_t *numof)
{
    
    memset(addrs, 0xff, sizeofaddrs);
    
    uint8_t ff = 0;
    *numof = 0;
    
    static const uint8_t start = 2;
    for(int i=start; i<256; i++)
    {
        if((i%2) == 1)
        {
            continue;
        }
        if(HAL_OK == HAL_I2C_IsDeviceReady(bus, i, 1, 5))
        {
            if(ff < sizeofaddrs)
            {
                addrs[ff] = i;
            }
            ff++;
            (*numof)++;
        }
    }    
    
    
    
}


// --------------------------------------------------------------------------------------------------------------------
// - end-of-file (leave a blank line after)
// --------------------------------------------------------------------------------------------------------------------




